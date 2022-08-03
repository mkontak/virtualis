// $Id: Server.cs,v 1.15.2.1 2011/05/18 01:57:01 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.WorkQueue.Server/Server.cs,v 1.15.2.1 2011/05/18 01:57:01 mkontak Exp $
//
// Copyright© (2004), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: [Mark Kontak] [mkontak@viatronix.com]



using System;
using System.Threading;
using System.Data;
using System.Text;
using System.Xml;
using System.Collections;
using System.Data.SqlClient;
using System.Collections.Generic;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;
using System.Security.AccessControl;
using System.Security.Cryptography;
using System.Security.Principal;
using System.Net;
using System.Net.Sockets;
using Viatronix.Messaging;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Processing;

namespace Viatronix.Enterprise.Servers
{

  /// <summary>
  /// Summary description for Server.
  /// </summary>
  public class WorkQueueServer : QueueServer
  {


    #region enums

    // Wait Events 
    private enum WaitEvents : int
    {
      ProcessCompleted = 0,
      ServerAborted,
      AbortJobed             
    };


    #endregion

    #region TaskProcess


    protected struct JobProcess
    {
      public Job Job;
      public System.Threading.Tasks.Task Task;
      public EventWaitHandle AbortEvent;
    }

    #endregion

 
    #region constants

 

    /// <summary>
    /// Machine Name
    /// </summary>
    private static string m_machineName = System.Environment.MachineName;

    #endregion

    #region fields

    /// <summary>
    /// Internal procedures
    /// </summary>
    private Dictionary<string, Type> m_procedures = new Dictionary<string, Type>();

    /// <summary>
    /// Loaded assemblies
    /// </summary>
    private Dictionary<string, System.Reflection.Assembly> m_loadedAssemblies = new Dictionary<string, System.Reflection.Assembly>();

    /// <summary>
    /// Task abort event name
    /// </summary>
    private string m_taskAbortEventName = "WorkQueueAbortJob";

    /// <summary>
    /// Port
    /// </summary>
    private UInt32 m_port = 0;


    /// <summary>
    /// Maximum number of task allowed
    /// </summary>
    private UInt16 m_maximumJobsAllowed = 2;

    /// <summary>
    /// Disk error threshold
    /// </summary>
    private UInt32 m_diskErrorThreshold = 0;

    /// <summary>
    /// Disk warning threshold
    /// </summary>
    private UInt32 m_diskWarningThreshold = 0;




    /// <summary>
    /// List of jobs  the work queue is processing
    /// </summary>
    private Dictionary<string, Job> m_jobs = new Dictionary<string, Job>();


    /// <summary>
    /// Threaded jobs being processed
    /// </summary>
    private Dictionary<string, JobProcess> m_processes = new Dictionary<string, JobProcess>();


    /// <summary>
    /// Settings
    /// </summary>
    private List<Setting> m_settings = new List<Setting>();


    #endregion

    #region constructors and destructors

    /// <summary>
    /// Constructor
    /// </summary>
    private WorkQueueServer() : base("WorkQueue") { }
 

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="settings"></param>
    public WorkQueueServer(string settings) : base("WorkQueue", settings) { }


    #endregion

    #region properties

    /// <summary>
    /// Logger name
    /// </summary>
    public string Logger
    { get { return m_logger;  } }


    /// <summary>
    /// Context Id
    /// </summary>
    public string ContextId
    { get { return m_context;  } }

    /// <summary>
    /// Message Handler Port
    /// </summary>
    public uint MessageHandlerPort
    { get { return m_port;  } }

    #endregion

    #region overrides

    /// <summary>
    /// Initializes the server the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>WorkQueueServer object</returns>
    protected override void Initialize(XmlNode node)
    {

      try
      {

        base.Initialize(node);


        Setting setting = new Setting();

        if (m_settings.TryFind("DiskErrorThreshold", ref setting))
          m_diskErrorThreshold = UInt32.Parse(setting.Value);

        if (m_settings.TryFind("DiskWarningThreshold", ref setting))
          m_diskWarningThreshold = UInt32.Parse(setting.Value);


        node.TryGetAttributeValue<UInt32>("diskErrorThreshold", ref m_diskErrorThreshold);
        node.TryGetAttributeValue<UInt32>("diskWarningThreshold", ref m_diskWarningThreshold);
        node.TryGetAttributeValue<UInt32>("messagePort", ref m_port);

        m_maximumJobsAllowed = UInt16.Parse(node.Attributes["concurrentTasks"].Value.ToString());

        if (!node.TryGetAttributeValue<string>("taskAbortEventName", ref m_taskAbortEventName) || string.IsNullOrEmpty(m_taskAbortEventName))
          throw new System.Configuration.ConfigurationErrorsException("Missing task abort event name");

          // Event Security
        EventWaitHandleSecurity security = new EventWaitHandleSecurity();
        security.AddAccessRule(new EventWaitHandleAccessRule(new SecurityIdentifier(WellKnownSidType.WorldSid, null), EventWaitHandleRights.FullControl, AccessControlType.Allow));

        bool created;
        AddEvent(m_taskAbortEventName, new EventWaitHandle(false, EventResetMode.ManualReset, "Global\\" + m_taskAbortEventName, out created, security));

 

      }
      catch (Exception ex)
      {
        LogErr( string.Format("Invalid configuration : {0}", ex.Message), "Server", "Create");
        throw;
      }


    } // Initialize(XmlNode node)

    /// <summary>
    /// Aborts the work queue server
    /// </summary>
    public override void Abort(Dictionary<string, string> parameters = null)
    {


      if (parameters != null)
      {
        string job = string.Empty;
        string task = string.Empty;

        if ( parameters.TryGetValue("job", out job))
        {

          if (parameters.TryGetValue("task", out task))
          {
            string key = job + "|" + task.ToLower();

            if (m_processes.ContainsKey(key))
              m_processes[key].AbortEvent.Set();
           
          }

        }

      }
      else
      {

        base.Abort();
 
       }

 
      /// Shutdown the message handler
      ShutdownMessageHandler();


    } // Abort()


    /// <summary>
    /// Executes the WorkQueue 
    /// </summary>
    public override void RunSynchronously()
    {

      // Make sure the server abort event is not set 
      ResetEvent("abort");


      try
      {


        // Started
        OnStarted();

        /// Resets the JOBS that were processing
        StorageGateway.ResetJobs(m_context);


        System.Threading.Tasks.Task messageHandler = null;
        if ( m_port > 0 )
        {

          messageHandler = new System.Threading.Tasks.Task( MessageHandler );
 
          messageHandler.Start();
  
        }



        while (true)
        {



          // Get the next job to processing
          Job job = GetNextJob();


          LogDbg(string.Format("Starting {0}-{1}", job.Id, job.Task.Name), "WorkQueueServer", "RunSynchronously");

          // Job process
          JobProcess process = new JobProcess();

          // ================================================================
          // Initialize the job process
          //  - Create an abort event
          //  - Set the job
          //  - Start a this job
          // ===============================================================
          process.AbortEvent  = CreateEventHandle(job);
          process.Job         = job;
          process.Task        = System.Threading.Tasks.Task.Factory.StartNew(() => Process(job, process.AbortEvent, this.AbortEvent));

          // Inserts the JobProcess into the procesisng list
          InsertJobToProcessList(process);



        }  // END ... while forever


      }
      catch ( AbortException )
      {

        OnAborted();

        LogErr( "Work queue server is being aborted", "Server", "Run");

        throw;

      }
      catch (Exception ex)
      {

 
        LogErr("Work queue server encountered exception : " + ex.Message, "Server", "RunSynchronously");


        OnFailed(ex);

        throw;

      }

    } // RunSynchronously()


    #endregion

    #region methods


    #region Job methods


    /// <summary>
    /// Attempts to fail the specified job. There will be a total of 3 attempts to 
    /// fail the job so that data in the database is properly cleaned up.
    /// </summary>
    /// <param name="job">Job to be completed</param>   
    public void FailJob(Job job, string message)
    {
      LogErr(string.Format("Failing job={0} task={1} : {2}", job.Id, job.Task.Name, message), "WorkQueueServer", "FailJob");


      int retryCount = 1;
      string errorMessage = string.Empty;

      do
      {

        try
        {
          errorMessage = string.Empty;

          StorageGateway.FailJob(m_context, job, string.Format("{0} failed : {1}", job.Task.Name, message));
          
          break;
        }
        catch (Exception ex)
        {
          errorMessage = ex.Message;
          LogWrn(string.Format("Attempting to fail the job {0} failed: {1}, attempting retry {2}", job.Id, ex.Message, retryCount), "WorkQueueServer", "FailJob");
        }

      } while (++retryCount < 3);

      if (!string.IsNullOrEmpty(errorMessage))
        throw new ViatronixException(string.Format("Attempting to fail the job failed : {0}", errorMessage), ErrorCodes.None);



    } // FailTask(Task task)

    /// <summary>
    /// Attempts to complete the specified job. There will be a total of 3 attempts to 
    /// complete the job so that data in the database is properly cleaned up.
    /// </summary>
    /// <param name="job">Job to be completed</param>
    public void CompleteJob(Job job)
    {

      LogInfo(string.Format("Completing job={0} task={1}", job.Id, job.Task.Name), "WorkQueueServer", "CompleteJob");

      int retryCount = 1;
      string errorMessage = string.Empty;

      do
      {

        try
        {
          errorMessage = string.Empty;
          StorageGateway.CompleteJob(m_context, job);
          break;
        }
        catch (Exception ex)
        {
          errorMessage = ex.Message;
          LogWrn(string.Format("Attempting to complete the job {0} failed : {1}, attempting retry {2}", job.Id, ex.Message, retryCount), "WorkQueueServer", "CompleteJob");
        }

      } while (++retryCount < 3);

      if (!string.IsNullOrEmpty(errorMessage))
        throw new ViatronixException(string.Format("Attempting to complete the job failed : {0}", errorMessage), ErrorCodes.None);


    } // CompleteTask(Task task)

    /// <summary>
    /// Handles a abort task
    /// </summary>
    /// <param name="task"></param>
    public void AbortJob(Job job)
    {

      LogErr(string.Format("Aborting job={0} task={1}", job.Id, job.Task.Name), "WorkQueueServer", "AbortJob");

      //RemoveJobFromMasterList(job);

      StorageGateway.AbortJob(m_context, job);
 
    } // AbortTask(Task task)


    /// <summary>
    /// Attempts to start the specified job. There will be a total of 3 attempts to 
    /// start the job so that data in the database is properly cleaned up.
    /// </summary>
    /// <param name="job">Job to be completed</param>
    public void StartJob(Job job)
    {
      LogInfo(string.Format("Starting job={0} task={1}", job.Id, job.Task.Name), "WorkQueueServer", "StartJob");


      int retryCount = 1;
      string errorMessage = string.Empty;

      do
      {

        try
        {
          errorMessage = string.Empty;
          StorageGateway.StartJob(m_context, job);
          break;
        }
        catch (Exception ex)
        {
          errorMessage = ex.Message;
          LogWrn(string.Format("Attempting to start the job {0} failed : {1}, attempting retry {2}", job.Id, ex.Message, retryCount), "WorkQueueServer", "StartJob");
        }

      } while (++retryCount < 3);

      if (!string.IsNullOrEmpty(errorMessage))
        throw new ViatronixException(string.Format("Attempting to start the job failed : {0}", errorMessage), ErrorCodes.None);


    } // StartTask(Task task)


    /// <summary>
    /// Attempts to update the specified job. There will be a total of 3 attempts to 
    /// update the job so that data in the database is properly cleaned up.
    /// </summary>
    /// <param name="job">Job to be completed</param>
    public void UpdateJob(Job job)
    {
      LogInfo(string.Format("Updating job {0}", job.ToXml()), "WorkQueueServer", "UpdateJob");

      int retryCount = 1;
      string errorMessage = string.Empty;

      do
      {

        try
        {
          errorMessage = string.Empty;
          StorageGateway.UpdateJob(m_context, job);
          break;
        }
        catch (Exception ex)
        {
          errorMessage = ex.Message;
          LogWrn(string.Format("Attempting to update the job {0} failed : {1}, attempting retry {2}", job.Id, ex.Message, retryCount), "WorkQueueServer", "UpdateJob");
        }

      } while (++retryCount < 3);

      if (!string.IsNullOrEmpty(errorMessage))
        throw new ViatronixException(string.Format("Attempting to update the job failed : {0}", errorMessage), ErrorCodes.None);


    } // UpdateJob(Job job)

    
    #endregion



    /// <summary>
    /// Message Handler 
    /// </summary>
    private void MessageHandler()
    {
      TcpListener listener = null;

      // ===============================================================================================
      // First start the message handler and if we cannot not then throw an exception and log an error
      // ===============================================================================================
      try
      {

        IPAddress address = null;
        foreach (IPAddress addr in Dns.GetHostAddresses(System.Environment.MachineName))
        {
          if (addr.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
          {
            address = addr;
            break;
          }
        }

        if (address == null)
          throw new NotFoundException("Could not find IP4 address");

        // Create a TCP listener
        listener = new TcpListener(address, (Int32)m_port);

        if (listener == null)
          throw new OutOfMemoryException("Failed to allocate TCP listener");

        LogInfo( string.Format("Starting message handler on port {0}", m_port), "WorkQueueServer", "Run");

        /// Start the listener
        listener.Start();

      }
      catch (Exception ex)
      {
        LogErr("Failed to start message handler : " + ex.Message, "WorkQueueServer", "Run");
        throw;
      }


      // ==================================================
      // Loop forever waiting for messages to be received
      // ==================================================
      while (true)
      {

        // ====================
        // Wait for connection
        // =====================
        using (TcpClient client = listener.AcceptTcpClient())
        {

          ProcessingMessage message;
          try
          {
            byte[] buffer = new byte[client.ReceiveBufferSize];


            int bytesRread = client.GetStream().Read(buffer, 0, client.ReceiveBufferSize);

            


            string xml = Encoding.ASCII.GetString(buffer);

            // ================================
            // Check for the shutdown command
            // ================================
            if (xml.StartsWith("shutdown"))
            {
              LogInfo("Shutting down the message handler", "WorkQueue", "MessageHandler");
              listener.Stop();
              break;
            }

            LogDbg(string.Format("Received message {0}: {1}", bytesRread, xml), "WorkQueueServer", "MessageHandler");

            message = ProcessingMessage.Create(xml);

            if (m_jobs.ContainsKey(message.JobId))
            {
              Job job = m_jobs[message.JobId];

              job.Progress = message.Progress;
              job.Message = message.Message;
              job.Stage = message.Stage;

              StorageGateway.UpdateJob(m_context, job);
            }
            else
              LogWrn(string.Format("Cannot update job {0}, not found in list", message.JobId), "WorkQueueServer", "MessageHandler");
          }
          catch (Exception ex)
          {
            LogErr( "Failed to process message : " + ex.Message, "WorkQueueServer", "Run");
          }


        } // END ... using

      } // END ... while forever

    }


    /// <summary>
    /// Processes the job supplied
    /// </summary>
    /// <param name="job">Job to be processed</param>
    /// <param name="jobAbortHandle">Abort handle for the job</param>
    /// <param name="serverAbortHandle">Main servers abort handle</param>
    private void Process(Job job, EventWaitHandle jobAbortHandle, EventWaitHandle serverAbortHandle )
    {

      try
      {

        // ==============================================================================================
        // If the job's task does not have the executable associated with it then we are running the 
        // job interally by loading the assembly. Otherwsie we run the seperate process executable.
        // =============================================================================================
        if (string.IsNullOrEmpty(job.Task.Executable))
          ProcessProcedure(job, jobAbortHandle, serverAbortHandle);
        else
          ProcessExeceutable(job, jobAbortHandle, serverAbortHandle);

      }
      catch (Exception ex)
      {
        throw new ViatronixException(string.Format("Job {0} failed : {0}", job.Id, ex.Message), ex, ErrorCodes.None);
      }
      finally
      {

        if (jobAbortHandle != null)
          jobAbortHandle.Close();


        DeleteJobFromWorkQueue(job);

      }

    }

    /// <summary>
    /// Processes the job as an internal procedure. These are procedures are listing an a section of the configuration
    /// file and the a name lookup is doen to map the tasks name to the procedure configuration settings. the procedure 
    /// needs to implement the IPipeline interface for thos to work.
    /// </summary>
    /// <param name="job">Job to process</param>
    /// <param name="jobAbortHandle">Jobs abort handle</param>
    /// <param name="serverAbortHandle">Main servers abort handle</param>
    private void ProcessProcedure(Job job, EventWaitHandle jobAbortHandle, EventWaitHandle serverAbortHandle)
    {

      try
      {
        LogInfo(string.Format("Job {0}-{1} - processing procedure", job.Id, job.Task.Name), "WorkQueueServer", "ProcessProcedure");

        if ( ! m_procedures.ContainsKey(job.Task.Name) )
          throw new Exception(String.Format("{0} procedure does not exist", job.Task.Name));

        /// Create the instance of the IPipeline
        IPipeline pipeline = (IPipeline)Activator.CreateInstance(m_procedures[job.Task.Name]);

        pipeline.Started      += new PipelineEventHandler(OnPipelineStarted);
        pipeline.Completed    += new PipelineEventHandler(OnPipelineCompleted);
        pipeline.Failed       += new PipelineEventHandler(OnPipelineFailed);
        pipeline.Updated      += new PipelineEventHandler(OnPipelineUpdated);


        /// Start the task (Starting the task will create the series)
        StartJob(job);


        // Get the paths associated with the JOB
        Dictionary<string, string> paths = GetPaths(job);


        Dictionary<string, string> args = new Dictionary<string,string>();

        args.Add("sourceFilePath", paths["src"]);

        args.Add("destFilePath", paths["dst"]);

        args.Add("dicomFilePath", paths["dcm"]);


        args.Add("volumeFilePath", paths["vol"]);

        args.Add("job", job.Id);

        args.Add("app", job.Application);


        args.Add("logLevel", job.Task.LogLevel.ToString());

        string logFilePath = String.Format("{0}.log", paths["dst"]);
        args.Add("logFilePath", logFilePath);
        LogDbg(string.Format("logFilePath ... {0}", logFilePath), "WorkQueueServer", "ProcessProcedure");


        // ===============
        // Add parameters
        // ================
        foreach (Parameter parameter in job.Task.Parameters)
        {  
          string [] parts = parameter.CommandLine.Split(' ');

          args.Add(parts[0].TrimStart('-'), parts[1]);
          
        }


        foreach (KeyValuePair<string, string> pair in args)
        {
          LogDbg(string.Format("{0} : {1}", pair.Key, pair.Value), "WorkQueueServer", "ProcessProcedure");
        }


        EventWaitHandle pipelineHandle =  pipeline.Start(args);



 
        WaitHandle[] handles = new WaitHandle[3];

        handles[(int)WaitEvents.ProcessCompleted]   = pipelineHandle;
        handles[(int)WaitEvents.ServerAborted]      = this.AbortEvent;
        handles[(int)WaitEvents.AbortJobed]         = GetEvent(m_taskAbortEventName);

        LogDbg(string.Format("Waiting for {0}-{1} to complete", job.Id, job.Task.Name), "WorkQueueProcess", "ProcessProcedure");

        int handle = (job.Task.MaximumRunTime > 0 ? WaitHandle.WaitAny(handles, job.Task.MaximumRunTime * 1000) : WaitHandle.WaitAny(handles));

        // ===================================
        // check if the wait timed out
        // ====================================
        if (handle == WaitHandle.WaitTimeout)
        {
          {
            // Abort the process
            pipeline.Abort();

            pipeline = null; 

            string errorMessage =  ( job.Task.MaximumRunTime > 60 ? 
                                      string.Format("Process {0} timed out ( runtime >= {1} minutes )", job.Task.Name, job.Task.MaximumRunTime/60) :
                                          string.Format("Process {0} timed out ( runtime >= {1} seconds )", job.Task.Name, job.Task.MaximumRunTime) );

            // Throw exception indicating the process timed out
            throw new ViatronixException(errorMessage, ErrorCodes.None);
          }

        } while (handle != -1);




        switch (handle)
        {


          case (int)WaitEvents.ProcessCompleted: // Process Completed

            ProcessCompleted(job, pipeline.GetResults().ToXml());
            break;



          case (int)WaitEvents.AbortJobed:   // Task Aborted
            {

              pipeline.Abort();

              AbortJob(job);

              string  message = String.Format("Job {0}-{1} aborted", job.Id, job.Id);

              LogErr(message, "WorkQueueServer", "ProcessAssembly");

              throw new AbortException(message);

            }




          case (int)WaitEvents.ServerAborted:     // Server Aborted
            {
              pipeline.Abort();

              LogErr("Server is aborting", "WorkQueueProcess", "Process");

              throw new AbortException("Server is aborting");
            }


          default:
            LogErr( "Invalid event handle ", "WorkQueueProcess", "Process");
            break;

        }; // END ... Switch on handle



      }
      catch (AbortException)
      {
        throw;
      }
      catch (Exception ex)
      {

        FailJob(job, ex.Message);

        string message = string.Format("Job {0}-{1} - failed  : {2}", job.Id, job.Task.Name, ex.Message);

        LogErr(message, "WorkQueueServer", "ProcessAssembly");

        throw new ViatronixException(message, ex, ErrorCodes.None);
      }



        
    } // ProcesProcedure()

    /// <summary>
    /// Processes the executable job as we have done in all past work queue server's. The executable is a seperate application
    /// that is launched from within the WorkQueueServer
    /// </summary>
    /// <param name="job">Job to be processed</param>
    /// <param name="jobAbortHandle">Job abort handle</param>
    /// <param name="serverAbortHandle">Main server abort handle</param>
    private void ProcessExeceutable(Job job, EventWaitHandle jobAbortHandle, EventWaitHandle serverAbortHandle)
    {

      System.Diagnostics.Process process = null;



      try
      {
        string executable = Helper.ParseExpression(job.Task.Executable);


        LogInfo(string.Format("Job {0} - processing executable {1}", job.Id, executable), "WorkQueueServer", "ProcessExeceutable");

        // Make sure the executable exists
        if (!System.IO.File.Exists(executable))
          throw new NotFoundException("Invalid executable [" + executable + "]");



        /// Start the task (Starting the task will create the series)
        StartJob(job);


        // Get the paths associated with the JOB
        Dictionary<string, string> paths = GetPaths(job);


        // Need to build the argument list for the executable
        StringBuilder args = new StringBuilder();


        args.Append(string.Format("-sourceFilePath \"{0}\" ", paths["src"]));
        args.Append(string.Format("-destFilePath \"{0}\" ", paths["dst"]));
        args.Append(string.Format("-dicomFilePath \"{0}\" ", paths["dcm"]));
        args.Append(string.Format("-volumeFilePath \"{0}\" ", paths["vol"]));
        args.Append(string.Format("-job \"{0}\" ", job.Id));
        if ( job.Application.Length > 0 )
          args.Append(string.Format("-app \"{0}\" ", job.Application));
        args.Append(string.Format("-messageHost \"{0}\" ", System.Environment.MachineName));
        args.Append(string.Format("-messagePort \"{0}\" ", m_port));
        //args.Append(string.Format("-logLevel {0} ", job.Task.LogLevel));
        args.Append("-logLevel 6 ");

        args.Append(string.Format("-logFilePath {0}.log", paths["dst"]));

        // ===============
        // Add parameters
        // ================
        foreach (Parameter parameter in job.Task.Parameters)
        {
          args.Append(string.Format(" {0} ", parameter.CommandLine));
        }



        // New process
        process = new System.Diagnostics.Process();

        // Setup the processes startup information
        process.StartInfo.Arguments = args.ToString();
        process.StartInfo.CreateNoWindow = true;
        process.StartInfo.UseShellExecute = false;
        process.StartInfo.FileName = executable;
        process.StartInfo.RedirectStandardOutput = true;


        LogInfo(string.Format("{0} {1}", executable, process.StartInfo.Arguments), "WorkQueueProcess", "ProcessExecutable");

        // Starts the process
        process.Start();


        // Holder for the stadard out
        string stdout = string.Empty;

        // ==================================================================================================
        // Creates a task that reads the stadard out from the process this is so that it does not hang up
        // ==================================================================================================
        System.Threading.Tasks.Task task = System.Threading.Tasks.Task.Factory.StartNew(() =>
        {
          stdout = process.StandardOutput.ReadToEnd();
        } );


        // Setup the wait handles
        WaitHandle[] handles = new WaitHandle[3];

        handles[(int)WaitEvents.ProcessCompleted] = new System.Threading.AutoResetEvent(false);
        handles[(int)WaitEvents.ProcessCompleted].SafeWaitHandle = new Microsoft.Win32.SafeHandles.SafeWaitHandle(process.Handle, false);
        handles[(int)WaitEvents.ServerAborted]  = this.AbortEvent;
        handles[(int)WaitEvents.AbortJobed] = GetEvent(m_taskAbortEventName);

        LogDbg( string.Format("Job {0} - waiting for {1} to complete", job.Id, job.Task.Name), "WorkQueueProcess", "ProcessExecutable");


        // ==============================================================================================================================
        // If the maximum runtime was specified then wait until  an event occurs or a timeout, otherwise wait forever for the an event
        // ==============================================================================================================================
        int handle = (job.Task.MaximumRunTime > 0 ? WaitHandle.WaitAny(handles, job.Task.MaximumRunTime * 1000) : WaitHandle.WaitAny(handles));

        // ===================================
        // check if the wait timed out
        // ====================================
        if (handle < 0 || handle > (handles.Length-1))
        {
          // Kill the process
          process.Kill();

          string errorMessage =  ( job.Task.MaximumRunTime > 60 ? 
                                    string.Format("Process {0} timed out ( runtime >= {1} minutes )", job.Task.Name, job.Task.MaximumRunTime/60) :
                                        string.Format("Process {0} timed out ( runtime >= {1} seconds )", job.Task.Name, job.Task.MaximumRunTime) );

          // Throw exception indicating the process timed out
          throw new ViatronixException(errorMessage, ErrorCodes.None);
        } // END ... If the timeout was hit

  
        // ============================================================
        // Finish the processing based on the event that was detected
        // ===========================================================
        switch (handle)
        {


          case (int)WaitEvents.ProcessCompleted: // Process Completed
            {

              task.Wait();

              // ===============================================
              // If the exit code is not 0 then it is a failure
              // ===============================================
              if (process.ExitCode != 0)
                throw new ViatronixException(string.Format("Exit code {0}", process.ExitCode), ErrorCodes.None);
              else
                ProcessCompleted(job, stdout);

            }
            break;



          case (int)WaitEvents.AbortJobed:   // Task Aborted
            {

              /// Kill the process
              process.Kill();

              LogErr( string.Format("Job {0}-{1} - aborted", job.Id, job.Task.Name), "WorkQueueServer", "ProcessExecutable");

              StorageGateway.AbortJob(m_context, job);


            }
            throw new AbortException(string.Format("Job {0}-{1} aborted", job.Id, job.Task.Name));




          case (int)WaitEvents.ServerAborted:     // Server Aborted
            {
              /// Kill the process
              process.Kill();

              LogErr( "Server is aborting", "WorkQueueProcess", "ProcessExecutable");

            }
            throw new AbortException("Server is aborting");


          default:

            LogErr("Invalid event handle ", "WorkQueueProcess", "ProcessExecutable");
            break;

        }; // END ... Switch on handle



      }
      catch (AbortException)
      {
        throw;
      }
      catch (Exception ex)
      {

        FailJob(job, ex.Message);

        throw new ViatronixException(string.Format("Failed  job={0} task={1} : {2}", job.Id, job.Task.Name, ex.Message), ex, ErrorCodes.None);

      }
      finally
      {
        if (process != null)
          process.Close();

      }
    

        
    } // Process()


    /// <summary>
    /// Process completed.
    ///   Calls the Update and uses the applcation and task name to formulate the 
    ///   datrabase stored procedure name. 
    ///     example:    App = ""      Task = "conversion"     SP Name = sp_updateConversion
    ///                 App = "VC"    Task = "preprocessed"   SP Name = sp_updateVCPreprocessed
    ///               
    /// </summary>
    /// <param name="job">Job that completed</param>
    /// <param name="results">xml resulst</param>
    void ProcessCompleted(Job job, string results)
    {

      LogDbg(string.Format("Processing Results : {0}", results), "WorkQueueServer", "ProcessCompeleted");

      // ===================================================
      // If the results are empty then there is no updating
      // ====================================================
      if (!string.IsNullOrEmpty(results))
      {
        StringBuilder record = new StringBuilder(string.Format("<workqueue jid=\"{0}\" app=\"{1}\" task=\"{2}\" >", job.Id, job.Application, job.Task.Name));
        record.Append(results);
        record.Append("</workqueue>");

        /// call sp_updateWorkQueue store procedure
        StorageGateway.Update(m_context, "workqueue", record.ToString());
      }

      /// Complete the JOB (This will call the database which adds the new job)
      CompleteJob(job);

    }

    /// <summary>
    /// Creates the named abort event for the task
    /// <param name="task">Task to create event for</param>
    /// </summary>
    private EventWaitHandle CreateEventHandle(Job job)
    {
 
      bool created = false;


      string name = "Global\\" + job.Id;

      // Event Security
      EventWaitHandleSecurity security = new EventWaitHandleSecurity();
      security.AddAccessRule(new EventWaitHandleAccessRule(new SecurityIdentifier(WellKnownSidType.WorldSid, null), EventWaitHandleRights.FullControl, AccessControlType.Allow));

      // Create the wait handle for detecting the start event
      EventWaitHandle handle = new EventWaitHandle(false, EventResetMode.ManualReset, name, out created, security);


      if (created)
        LogInfo(string.Format("Created the job abort event {0}", job.Id), "WorkQueueProcess", "CreateEventHandle");
      else
        Log.Warning(m_logger, string.Format("Failed to create job abort event  {0}", job.Id), "WorkQueueProcess", "CreateEventHandle");

      return handle;

    } // CreateEventHandle(Job job)



    /// <summary>
    /// Gets the list of paths used for processing
    /// </summary>
    /// <param name="job"Job</param>
    /// <param name="task">Task</param>
    /// <returns>List of paths retrieved for the job and task</returns>
    public Dictionary<string, string> GetPaths(Job job)
    {

      Dictionary<string, string> paths = new Dictionary<string, string>();

      XmlBuilder builder = new XmlBuilder("jobpath");
      builder.AddAttribute("loc", "local");
      builder.AddAttribute("id", job.Id);

      string xml = StorageGateway.Get(m_context, "jobpath", builder.ToXml());

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);


      foreach (XmlAttribute attribute in doc.DocumentElement.Attributes)
      {
        paths.Add(attribute.Name, attribute.Value);
      }

      return paths;

    } // GetPaths(Task task)

    /// <summary>
    /// Deletes the task from the process list
    /// </summary>
    /// <param name="task"></param>
    private void DeleteJobFromWorkQueue(Job job)
    {

      Log.Debug(string.Format("Removing job={0} task={1} from the work queue system", job.Id, job.Task.Name), "WorkQueueService", "DeleteJob");

      RemoveJobFromMasterList(job);

      RemoveJobFromProcessList(job);
    } // DeleteTask()

    /// <summary>
    /// Removes the job from the process list
    /// </summary>
    /// <param name="job"></param>
    private void RemoveJobFromProcessList(Job job)
    {

      Mutex mutex = new Mutex(false, "WORKQUEUE_PROCESSES_DICTIONARY");
       mutex.WaitOne();

      try
      {
        if (!m_processes.ContainsKey(job.Id))
          LogWrn(string.Format("Job {0} task={1} does not exist in the process list, cannot remove", job.Id, job.Task.Name), "WorkQueuServer", "RemoveJobFromProcessList");
        else
        {

          LogInfo(string.Format("Removing job={0} task={1} from the process list", job.Id, job.Task.Name), "WorkQueuServer", "RemoveJobFromProcessList");

          m_processes.Remove(job.Id);

        }


      }
      finally
      {
        mutex.ReleaseMutex();
      }

    }   // RemoveJobFromProcessList(Job job)

    /// <summary>
    /// Removes the job from the process list
    /// </summary>
    /// <param name="job"></param>
    private void InsertJobToProcessList(JobProcess jobProcess)
    {
      
      Mutex mutex = new Mutex(false, "WORKQUEUE_PROCESSES_DICTIONARY");
      mutex.WaitOne();
   
      try
      {

        if (m_processes.ContainsKey(jobProcess.Job.Id))
          LogWrn(string.Format("Job {0} task={1} already exists in the process list, cannot add", jobProcess.Job.Id, jobProcess.Job.Task.Name), "WorkQueuServer", "InsertJobToProcessList");
        else
        {
          LogInfo(string.Format("Adding job={0} task={1} to the process list", jobProcess.Job.Id, jobProcess.Job.Task.Name), "WorkQueuServer", "InsertJobToProcessList");
          m_processes.Add(jobProcess.Job.Id, jobProcess);
        }
  
      }
      finally
      {
        mutex.ReleaseMutex();
      }


    }   // RemoveJobFromProcessList(Job job)


    /// <summary>
    /// Shutdown the message handler
    /// </summary>
    private void ShutdownMessageHandler()
    {
      TcpClient client = new TcpClient();

      client.Connect(System.Environment.MachineName, (int)m_port);

      string message = "shutdown";

      client.GetStream().Write(Encoding.ASCII.GetBytes(message), 0, message.Length);

      client.Close();


    }

 
    /// <summary>
    /// Gets the next job to process. Will only return if a job is available for processing otherwise it remains blocking for either 
    /// a pending event or abort event. If the maximum concurrent tasks are already running then we will wait for a task to completed 
    /// before find the next task to process.
    /// </summary>
    /// <returns>Job to be processed</returns>
    private Job GetNextJob()
    {

      Job job = null;

      try
      {

        LogInfo("Attempting to get the next job to be processed", "WorkQueueServer", "GetNextTask");

        // ====================================================================================================
        // Will wait for any task to finish if we have reached the maximum number of concurrent tasks allowed.
        // =====================================================================================================
        WaitForJobs();

 
        // ===================================================================
        // Loop waiting for a job or task to become available for processing.
        // ===================================================================
        do
        {

          LogDbg("Attempting to get then next available pending job from the database", "WorkQueueServer", "GetNextJob");


          job = StorageGateway.GetNextJob(m_context, System.Environment.MachineName);

          // ===================================================================================================================
          // If no job is available for processing then we wait for the next job to be added to the list (WaitForPendingJobs).
          // Otherwise add the job to the master list so that it can be picked up for processing when a thread is available.
          // ===================================================================================================================
          if (job == null)
            ResetEvent(WaitForPendingJobs());
          else
            InsertJobToMasterList(job);

        } while (job == null);

      }
      catch (Exception ex)
      {
        LogErr("Failed to get next job : " + ex.Message, "WorkQueuServer", "GetNextJob");
        throw;
      }

      return job;
 
 
    } // GetNextJob()


    /// <summary>
    /// Adds job to the dictionary
    /// </summary>
    /// <param name="job"></param>
    void InsertJobToMasterList(Job job)
    {

      Mutex mutex = new Mutex(false, "WORKQUEUE_JOBS_DICTIONARY");
      mutex.WaitOne();

      try
      {
        if (m_jobs.ContainsKey(job.Id))
          LogWrn(string.Format("Job {0} task={1} already exists in the dictionary", job.Id, job.Task.Name), "WorkQueueServer", "AddJobToDictionary");
        else
        {
          LogInfo(string.Format("Adding  job={0} task={1} to the dictionary", job.Id, job.Task.Name ), "WorkQueueServer", "AddJobToDictionary");
          m_jobs.Add(job.Id, job);
        }
      }
      finally
      {
        mutex.ReleaseMutex();
     }


    } // InsertJobToMasterList(Job job)

    /// <summary>
    /// Removes the job from the dictionary
    /// </summary>
    /// <param name="job"></param>
    void RemoveJobFromMasterList(Job job)
    {
      Mutex mutex = new Mutex(false, "WORKQUEUE_JOBS_DICTIONARY");
      mutex.WaitOne();

      try
      {
        if ( ! m_jobs.ContainsKey(job.Id))
          LogWrn(string.Format("Job {0} task={1} does not exists in the dictionary, cannot be removed", job.Id, job.Task.Name), "WorkQueueServer", "RemoveJobFromMasterList");
        else
        {
          m_jobs.Remove(job.Id);

          LogInfo(string.Format("Removing job={0} task={1} from dictionary", job.Id, job.Task.Name), "WorkQueueServer", "RemoveJobFromMasterList");
        }

      }
      finally
      {
        mutex.ReleaseMutex();
      }

    } // RemoveJobFromMasterList(Job job)


    /// <summary>
    /// Waits for a spare job slot to become available
    /// </summary>
    private void WaitForJobs()
    {

      // =================================================================================
      // If we are at the maximum task allowed then we need to wait for one to complete
      // =================================================================================
      if (m_jobs.Count == m_maximumJobsAllowed)
      {
        // =============================================================================================
        // Exceptions thrown by tasks will be propagated to the main thread
        // while it waits for the tasks. The actual exceptions will be wrapped in AggregateException.
        // ===============================================================================================
        try
        {

          LogInfo("Maximum concurrent jobs reached, waiting for any current job to complete", "WorkQueueServer", "WaitForJobs");

          System.Threading.Tasks.Task[] tasks = new System.Threading.Tasks.Task[m_jobs.Count];
          int i = 0;
          foreach (KeyValuePair<string, JobProcess> pair in m_processes)
          { tasks[i++] = pair.Value.Task; }

          // Wait for any of the threads being processed to complete
          int index = System.Threading.Tasks.Task.WaitAny(tasks);

          // Log a debig message that a job terminated and we can continue
          LogDbg(string.Format("Job {0} terminated, continuing with the next job on the list", index), "WorkQueuServer", "WaitForJobs");


          //CleanupTasks();

        }
        catch (AggregateException ex)
        {

          LogErr("One or mores jobs failed, see exception below", "WorkQueuServer", "WaitForJobs");

          // =======================================================================
          // Loop through the aggregate exceptions dumping the error to the logger
          // =======================================================================
          for (int j = 0; j < ex.InnerExceptions.Count; j++)
          {
            string message = ex.InnerExceptions[j].Message;

            Char[] del = { ' ' };
            string[] fields = message.Split(del);

            LogErr(message, "WorkQueuServer", "Run");
          } // END ... For each inner exception


        } // END ... catch
        catch (Exception ex)
        {
          LogErr("Error occurred while waiting for jobs : " + ex.Message, "WorkQueueServer", "WaitForJobs");
        }

      } // END ... If we are at the maximum jobs allowed


    } // WaitForJobs()


    /// <summary>
    /// Creates the server using the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>WorkQueueServer object</returns>
    public static WorkQueueServer Create(XmlNode node)
    {
      WorkQueueServer server = new WorkQueueServer();

      if (server == null)
        throw new OutOfMemoryException("Failed to allocate WorkQueueServer");

      server.Initialize(node);

      try
      {
        XmlNode procedures = node.SelectSingleNode("procedures");

        if (procedures != null)
        {
          foreach (XmlNode procedure in procedures.SelectNodes("procedure"))
          {
            server.m_procedures.Add(procedure.Attributes["name"].Value, Type.GetType(procedure.Attributes["typename"].Value));
          }
        }

      }
      catch (Exception ex)
      {
        server = null;
        Log.Error("Invalid procedure specified in configurtaion : " + ex.Message, "WorkQueueServer", "Create");
        throw;
      }

      return server;




    } // Create(XmlNode node)


 
    #endregion


    #region event handlers

    public void OnPipelineStarted(PipelineEventArgs args)
    {
      // Do Nothing
    }

    public void OnPipelineCompleted(PipelineEventArgs args)
    {


    }

    public void OnPipelineFailed(PipelineEventArgs args)
    {
      if (m_jobs.ContainsKey(args.JodId))
        FailJob(m_jobs[args.JodId], args.Message);

    }

    public void OnPipelineUpdated(PipelineEventArgs args)
    {
      // ===============================
      // If the job exists in the list
      // ===============================
      if (m_jobs.ContainsKey(args.JodId))
      {
        // Get the jobs task being processed
        Job job = m_jobs[args.JodId];
        job.Message = args.Message;
        job.Progress = args.Progress;
        job.Stage = args.Stage;

        UpdateJob(job);
      } // END ... If the job exists in the list
     
    }


    #endregion

    #region properties

    /// <summary>
    ///  Gets the settings
    /// </summary>
    List<Setting> Settings
    {
      get
      {
        if (m_settings == null)
        {
          EntityCollection.Load<Setting>(m_settings, SystemGateway.Get(m_context, "systemsetting", XmlBuilder.Create("systemsetting", new Dictionary<string, string>() { { "subsystem", "System" } })));
        }

        return m_settings;
      }
    }


    /// <summary>
    /// Set/Get of the disk warning threshold
    /// </summary>
    public UInt32 DiskWarningThreshold
    {
      get { return m_diskWarningThreshold; }
      set { m_diskWarningThreshold = value; }
    }

    /// <summary>
    /// Set/Get of the disk error threshold
    /// </summary>
    public UInt32 DiskErrorThreshold
    {
      get { return m_diskErrorThreshold; }
      set { m_diskErrorThreshold = value; }
    }




    /// <summary>
    /// Sets/Gets the maximum tasks (pipelines) allowed
    /// </summary>
    public UInt16 MaximumTasksAllowed
    {
      get { return m_maximumJobsAllowed; }
      set { m_maximumJobsAllowed = value; }
    }



 

    #endregion

  } // class WorkQueueServer

} // namespace Viatronix.Enterprise.Servers


