// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: AgentServer.cs
//
// Description: Implements a queuing server for agent items. These agents are jobs that are to be performed at a set time.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

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
using System.Data.Common;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

namespace Viatronix.Enterprise.Servers
{

  #region AgentItemComparer

  /// <summary>
  /// Comparer for the AgentItems list. Sorts by the NextRun date and time
  /// </summary>
  internal class AgentItemComparer : IComparer<AgentItem>
  {
 
    #region methods

    /// <summary>
    /// Comparer for agent jobs sorting based on the Next Run DateTime
    /// </summary>
    /// <param name="x">AgentItem </param>
    /// <param name="y">AgentItem</param>
    /// <returns></returns>
    public int Compare(AgentItem x, AgentItem y)
    {
      if (x == null)
      {
        if (y == null)
          return 0;
        else
          return -1;
      }
      else
      {

        if (y != null)
        {
          return x.NextRun == y.NextRun ? 0 : (x.NextRun < y.NextRun ? -1 : 1);
        }
        else
          return 1;

      }
    } // Comapare


    #endregion

  } // class AgentItemComparer

  #endregion

  #region class AgentItem

  /// <summary>
  /// Agent item used for sorting and linking the Agent Jobs with the tasks
  /// </summary>
  internal class AgentItem
  {

    #region fields

    /// <summary>
    /// Job to be run
    /// </summary>
    private AgentJob m_job = null;


    /// <summary>
    /// Next time this job is to be run
    /// </summary>
    private DateTime? m_nextRun = null;

    /// <summary>
    /// Task associated with the job
    /// </summary>
    private System.Threading.Tasks.Task m_task = null;


    #endregion


    #region methods

    /// <summary>
    /// Gets the names hashcode
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_job.Name.GetHashCode();
    } // GetHashCode()

    /// <summary>
    /// Creates the AgentItem for the collection
    /// </summary>
    /// <param name="job">Agent job</param>
    /// <param name="date">Current date to use for determining the next run</param>
    /// <returns></returns>
    public static AgentItem Create(AgentJob job)
    {
      AgentItem item = new AgentItem();

      if (item == null)
        throw new OutOfMemoryException("Failed to allocate AgentItem");

      item.m_job = job;
      item.m_nextRun = job.GetNextRunTime(DateTime.Now);

      return item;
    } // Create()

    #endregion

    #region properties

 
    /// <summary>
    /// Gets the Next run date/time
    /// </summary>
    public DateTime NextRun
    { get { return m_nextRun.Value; } }

    /// <summary>
    /// Gets the associate job
    /// </summary>
    public AgentJob Job
    { get { return m_job; } }


    /// <summary>
    /// Gets/Sets th
    /// </summary>
    public System.Threading.Tasks.Task Task
    { 
      get { return m_task; }
      set { m_task = value; }
    } // Task


    #endregion

  } // class AgentItem

  #endregion


  #region class AgentServer

  /// <summary>
  /// Summary description for Server.
  /// </summary>
  public class AgentServer : QueueServer
  {


    #region enums

    // Wait Events 
    private enum WaitEvents : int
    {
      ProcessCompleted  = 0,
      ServerAborted     = 1,
      AbortJobed        = 2
    };


    #endregion

    #region constants
    /// <summary>
    /// Machine Name
    /// </summary>
    private static string m_machineName = System.Environment.MachineName;

    /// <summary>
    /// Insert Agent Job Event Name
    /// </summary>
    private const string AgentJobInsertEventName = "AgentJobInsertJob";

    #endregion

    #region fields

    /// <summary>
    /// Agent Job Logging Directory
    /// </summary>
    private string m_loggingDirectory = System.IO.Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix\\Logs\\AgentServer");

    /// <summary>
    /// Actual timmed AgentJobs
    /// </summary>
    private List<AgentItem> m_items = new List<AgentItem>();


    /// <summary>
    /// List of agent jobs
    /// </summary>
    private List<AgentJob> m_jobs = new List<AgentJob>();

    /// <summary>
    /// Internal procedures
    /// </summary>
    private Dictionary<string, Type> m_procedures = new Dictionary<string, Type>();

    /// <summary>
    /// Comparer used for sorting
    /// </summary>
    private AgentItemComparer m_comparer = new AgentItemComparer();

    /// <summary>
    /// Insert Job event index
    /// </summary>
    private int m_insertJobEventIndex = 0;



    #endregion

    #region constructors and destructors

    /// <summary>
    /// Constructor
    /// </summary>
    private AgentServer() : base("AgentServer") 
    {
      m_insertJobEventIndex = AddEvent(AgentJobInsertEventName, new EventWaitHandle(false, EventResetMode.ManualReset));

    }


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="settings"></param>
    public AgentServer(string settings) : base("AgentServer", settings)
    {
      m_insertJobEventIndex = AddEvent(AgentJobInsertEventName, new EventWaitHandle(false, EventResetMode.ManualReset));
    }


    #endregion

    #region properties

    /// <summary>
    /// Agent Job Inserted event
    /// </summary>
    public int AgentJobInsertedEvent
    { get { return m_insertJobEventIndex; } }

    /// <summary>
    /// Logger name
    /// </summary>
    public string Logger
    { get { return m_logger; } }


    /// <summary>
    /// Context Id
    /// </summary>
    public string ContextId
    { get { return m_context; } }


    #endregion

    #region overrides

    /// <summary>
    /// Initializes the server the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>AgentServer object</returns>
    protected override void Initialize(XmlNode node)
    {

      try
      {

        base.Initialize(node);


        // ==================================================================
        // If the logging directory was sepcified then override the default
        // ==================================================================
        string directory = string.Empty;
        if (node.TryGetAttributeValue<string>("loggingDirectory", ref directory))
          m_loggingDirectory = directory;


        // =================================================
        // Create the agent server logging directory
        // ==============================================
        if (!System.IO.Directory.Exists(m_loggingDirectory))
          System.IO.Directory.CreateDirectory(m_loggingDirectory);



      }
      catch (Exception ex)
      {
        LogErr(string.Format("Invalid configuration : {0}", ex.Message), "Server", "Create");
        throw;
      }


    } // Initialize(XmlNode node)

    /// <summary>
    /// Aborts the work queue server
    /// </summary>
    public override void Abort(Dictionary<string, string> parameters = null)
    {

        base.Abort();

 

    } // Abort()


    /// <summary>
    /// Executes the AgentSever synchronously 
    /// </summary>
    public override void RunSynchronously()
    {

      // Make sure the server abort event is not set 
      ResetEvent("abort");


      try
      {


        // Started
        OnStarted();

        // Get the initial list of agent jobs
        GetAgentJobs();


        while (true)
        {
          // ===================================================================================================================
          // If there are no items build the list. This is always done the first time through the loop since no items are
          // in the list yet.
          // =================================================================================================================
          if ( m_items.Count == 0 )
            BuildAgentItemList();


          // Current date and time
          DateTime now = DateTime.Now;

          // Last item checked (set to null initially)
          AgentItem item = null;

          // ===================================================
          // While there are items in the list 
          // ===================================================
          while ( m_items.Count > 0 )
          {
            // Get the current item
            item = m_items[0];

            // ======================================================================================================
            // If the Next Run time is in the future then break out of loop so that we can wait until it is time to 
            // run this job.
            // =====================================================================================================

            if (item.NextRun > now)
              break;

            // Removes the agent job (always 0 since we are continually looking at the first item in the list)
            RemoveAgentItem(0);

            // ==================================================
            // Run the appropriate process type (SQL or EXE )
            // =================================================
            if (item.Job.Type == AgentJobTypes.SQL)
              ProcessSql(item);
            else
              ProcessExe(item);

          } // END ... While there are items in the list


          // Event index
          int eventIndex = WaitHandle.WaitTimeout;

          // =======================================================================================
          // If there is an item and the list still contains items use the items Next Run Time to 
          // calculate the wait taimeout when  waiting for the pending jobs event.
          // ========================================================================================
          if (item != null && m_items.Count > 0)
          {

            // Calculate the time span to wait before we need to start this job
            TimeSpan span = item.NextRun.Subtract(now);

            eventIndex = WaitForPendingJobs((int)span.TotalMilliseconds);

          } // END ... If there is a item that was not started
          else
            eventIndex = WaitForPendingJobs();


          // ===============================================================================
          // If the event triggered was the PendingJobsEvent then get the new agent jobs
          // ===============================================================================
          if ( eventIndex == PendingJobsEvent )
            GetAgentJobs();


          /// Resets the event triggered
          ResetEvent(eventIndex);

        }  // END ... while forever


      }
      catch (AbortException)
      {

        OnAborted();

        LogErr("Agent server is being aborted", "AgentServer", "Run");

        throw;

      }
      catch (Exception ex)
      {


        LogErr("Agent server encountered exception : " + ex.Message, "AgentServer", "RunSynchronously");


        OnFailed(ex);

        throw;

      }

    } // RunSynchronously()


    #endregion

    #region methods

    /// <summary>
    /// Pops the first element from the list
    /// </summary>
    /// <param name="item"></param>
    private void RemoveAgentItem(int index)
    {


      Mutex mutex = new Mutex(false, "AGENTSERVER_ITEMS");
      mutex.WaitOne();

      try
      {

        // ===============================================================
        // If the index is within the range then remove it from the list
        // ================================================================
        if (index >= 0 && index < m_items.Count)
        {
          LogDbg(string.Format("Removing agent item with index {0} from the list", index), "AgentServer", "RemoveAgentItem");

          m_items.RemoveAt(index);
        }

      }
      finally
      {
        mutex.ReleaseMutex();
      }


    } // RemoveAgentItem()

    /// <summary>
    /// Removes the specified agent item from the list
    /// </summary>
    /// <param name="item"></param>
    private void RemoveAgentItem(AgentItem item)
    {

      Mutex mutex = new Mutex(false, "AGENTSERVER_ITEMS");
      mutex.WaitOne();

      try
      {

        if (m_items.Contains(item))
        {
          LogDbg(string.Format("Removing {0} from the list {1}", item.Job.Name, item.NextRun), "AgentServer", "RemoveAgentItem");
          m_items.Remove(item);
        }
        else
          LogWrn(string.Format("{0} dores exists in the list cannot remove", item.Job.Name), "AgentServer", "InsertAgentItem");

      }
      finally
      {
        mutex.ReleaseMutex();
      }


    } // RemoveAgentItem()



    /// <summary>
    /// Inserts the specified agent item into the list
    /// </summary>
    /// <param name="item"></param>
    private void InsertAgentItem(AgentItem item)
    {

      Mutex mutex = new Mutex(false, "AGENTSERVER_ITEMS");
      mutex.WaitOne();

      try
      {

        if (!m_items.Contains(item))
        {
          LogDbg(string.Format("Inserting {0} into the list, NextRun: {1}", item.Job.Name, item.NextRun), "AgentServer", "InsertAgentItem");

          m_items.Add(item);

          SetEvent(AgentJobInsertEventName);
        }
        else
          LogWrn(string.Format("{0} already exists in the list cannot insert", item.Job.Name), "AgentServer", "InsertAgentItem");

      }
      finally
      {
        mutex.ReleaseMutex();
      }


    } // InsertAgentItem()



    /// <summary>
    /// Processes the SQL command
    /// </summary>
    /// <param name="item">Item to be processed</param>
    private void ProcessSql(AgentItem item)
    {

        item.Task = System.Threading.Tasks.Task.Factory.StartNew(() =>
        {
          string logFilePath = System.IO.Path.Combine(m_loggingDirectory, item.Job.Name + ".log");

          
          using (System.IO.StreamWriter writer = new System.IO.StreamWriter(logFilePath, false))
          {
            writer.WriteLine(string.Format("Starting {0}   procedure: {1}   paramaters: {2}", item.Job.Name, item.Job.Procedure, item.Job.Parameters));


            LogInfo(string.Format("Starting {0}   procedure: {1}   paramaters: {2}", item.Job.Name, item.Job.Procedure, item.Job.Parameters), "AgentServer", "ProcessSql");

            string procedure = item.Job.Procedure;

            string[] sections = procedure.Split(new Char[] { '.' });

            string database = sections[0].Trim(new Char[] { '[', ']' } );

            try
            {


              // Removal is now done  prior to the starting of this task
              //RemoveAgentItem(item);


              // Create the database
              Microsoft.Practices.EnterpriseLibrary.Data.Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(database.ToLower(), "AgentServer");

             
              string connectionString = db.ConnectionString;

              if (!string.IsNullOrEmpty(item.Job.User))
              {
                string password = Viatronix.Encryption.Crypto.Decrypt(item.Job.Password);

                connectionString = string.Format("User Id={1};Password={2};{0}", db.ConnectionStringWithoutCredentials, item.Job.User, password);
              }

              using (SqlConnection connection = new SqlConnection(connectionString))
              {

                connection.InfoMessage += new SqlInfoMessageEventHandler((object sender, SqlInfoMessageEventArgs args) =>
                {
                  foreach (System.Data.SqlClient.SqlError sqlError in args.Errors)
                  {
                    writer.WriteLine(sqlError.Message);
                  }

                });

                connection.Open();

                SqlCommand command = connection.CreateCommand();
                command.CommandType = CommandType.StoredProcedure;
                command.CommandText = procedure;



                if (item.Job.RequiresContext)
                  command.Parameters.Add(new SqlParameter("ID", m_context));
                  //db.AddInParameter(command, "ID", DbType.String, m_context);           // Context ID

                if (item.Job.Parameters.Length > 0)
                {
                  string[] parameters = item.Job.Parameters.Split(new Char[] { ',' });

                  foreach (string parameter in parameters)
                  {
                    string[] pair = parameter.Split(new Char[] { '=' });

                    if (pair.Length != 2)
                      throw new InvalidDateException(string.Format("{0} parameters are not formatted correctly {1}", item.Job.Name, item.Job.Parameters));

                    string[] values = pair[1].Split(new Char[] { '/' });

                    object value = values[0];
                    DbType type = DbType.String;

                    // ============================================
                    // Attempt to map the value to specified type
                    // =============================================
                    if (values.Length == 2)
                    {

                      if (string.Compare(values[1], "bit", true) == 0)
                      {
                        value = Int16.Parse(values[0]);
                        type = DbType.Int16;
                      }
                      else if (string.Compare(values[1], "int", true) == 0)
                      {
                        value = Int32.Parse(values[0]);
                        type = DbType.Int32;
                      }
                      else if (string.Compare(values[1], "bigint", true) == 0)
                      {
                        value = Int64.Parse(values[0]);
                        type = DbType.Int64;
                      }
                      else if (string.Compare(values[1], "nvarchar", true) == 0)
                        type = DbType.String;
                      else if (string.Compare(values[1], "xml", true) == 0)
                        type = DbType.Xml;
                      else
                        type = DbType.String;

                    }

                    command.Parameters.Add(new SqlParameter(pair[0], value));
                    //db.AddInParameter(command, pair[0], type, value);

                  }
                }

        
                // Execute
                //db.ExecuteStoredProcedure(command);

                command.ExecuteNonQuery();

              }

              InsertAgentJobHistory(item.Job, States.Completed, string.Empty);

              writer.WriteLine(string.Format("{0} Completed", item.Job.Name));

            }
            catch (Exception ex)
            {
              writer.WriteLine(string.Format("{0} failed : {1}", item.Job.Name, ex.Message));

              InsertAgentJobHistory(item.Job, States.Failed, ex.Message);
            }

            /// Inserts the item again, but recalculates the NEXT RUN time
            InsertAgentItem(AgentItem.Create(item.Job));

          };  // END ... using the writestream

        });

     
      
    }

    /// <summary>
    /// Processes a command or executable (external commands)
    /// </summary>
    /// <param name="item">Job definitition</param>
    private void ProcessExe(AgentItem item)
    {


      item.Task = System.Threading.Tasks.Task.Factory.StartNew(() =>
      {


        string logFilePath = System.IO.Path.Combine(m_loggingDirectory, item.Job.Name + ".log");

        using ( System.IO.StreamWriter writer = new System.IO.StreamWriter(logFilePath, false) )
        {

          writer.WriteLine(string.Format("Starting {0}   procedure: {1}   paramaters: {2}", item.Job.Name, item.Job.Procedure, item.Job.Parameters));

          LogInfo(string.Format("Starting {0}   procedure: {1}   paramaters: {2}", item.Job.Name, item.Job.Procedure, item.Job.Parameters), "AgentServer", "ProcessExe");

          try
          {
              System.Diagnostics.Process process = new System.Diagnostics.Process();

              // Setup the processes startup information


              // If the user name was specified then use the user/password
              // specified
              // =============================================================
              if ( !string.IsNullOrEmpty(item.Job.User) )
              {
                process.StartInfo.UserName = item.Job.User;

                string password  = Viatronix.Encryption.Crypto.Decrypt(item.Job.Password);
                System.Security.SecureString securePassword = new System.Security.SecureString();

                foreach (char ch in password)
                {
                  securePassword.AppendChar(ch);
                }

                process.StartInfo.Password = securePassword;

              }
              process.StartInfo.Arguments = item.Job.Parameters;
              process.StartInfo.CreateNoWindow = true;
              process.StartInfo.UseShellExecute = false;
              process.StartInfo.FileName = item.Job.Procedure;
              process.StartInfo.RedirectStandardOutput = true;
              process.StartInfo.RedirectStandardError = true;


              LogInfo(string.Format("Process Starting ... {0} {1}", item.Job.Procedure, process.StartInfo.Arguments), "AgentServer", "ProcessExe");

              // Starts the process
              process.Start();

              // Holder for the stadard out
              string stdout = string.Empty;

              // ==================================================================================================
              // Creates a task that reads the stadard out from the process this is so that it does not hang up
              // ==================================================================================================
              System.Threading.Tasks.Task stdinTask = System.Threading.Tasks.Task.Factory.StartNew(() =>
              {
                stdout = process.StandardOutput.ReadToEnd();
              });

              // Holder for the stadard out
              string stderr = string.Empty;

              // ==================================================================================================
              // Creates a task that reads the stadard out from the process this is so that it does not hang up
              // ==================================================================================================
              System.Threading.Tasks.Task stderrTask = System.Threading.Tasks.Task.Factory.StartNew(() =>
              {
                stderr = process.StandardError.ReadToEnd();
              });




              // Setup the wait handles
              WaitHandle[] handles = new WaitHandle[2];

              handles[(int)WaitEvents.ProcessCompleted] = new System.Threading.AutoResetEvent(false);
              handles[(int)WaitEvents.ProcessCompleted].SafeWaitHandle = new Microsoft.Win32.SafeHandles.SafeWaitHandle(process.Handle, false);
              handles[(int)WaitEvents.ServerAborted] = this.AbortEvent;



              // ==============================================================================================================================
              // If the maximum runtime was specified then wait until  an event occurs or a timeout, otherwise wait forever for the an event
              // ==============================================================================================================================
              int handle = (item.Job.MaximumRunTime > 0 ? WaitHandle.WaitAny(handles, item.Job.MaximumRunTime * 1000) : WaitHandle.WaitAny(handles));

              // ===================================
              // check if the wait timed out
              // ====================================
              if (handle < 0 || handle > (handles.Length - 1))
              {
                // Kill the process
                process.Kill();

                string errorMessage = (item.Job.MaximumRunTime > 60 ?
                                          string.Format("{0} timed out ( runtime >= {1} minutes )", item.Job.Name, item.Job.MaximumRunTime / 60) :
                                              string.Format("{0} timed out ( runtime >= {1} seconds )", item.Job.Name, item.Job.MaximumRunTime));

                // Throw exception indicating the process timed out
                throw new ViatronixException(errorMessage, ErrorCodes.None);

              } // END ... If the timeout was hit


              writer.WriteLine("STDOUT:");
              writer.WriteLine("");
              writer.Write(stdout);
              writer.WriteLine("");

              writer.WriteLine("STDERR:");
              writer.WriteLine("");
              writer.Write(stderr);
              writer.WriteLine("");


              // ============================================================
              // Finish the processing based on the event that was detected
              // ===========================================================
              switch (handle)
              {


                case (int)WaitEvents.ProcessCompleted: // Process Completed
                  {

                    // ===============================================
                    // If the exit code is not 0 then it is a failure
                    // ===============================================
                    if (process.ExitCode != 0)
                    {
                      writer.WriteLine(string.Format("Process {0} exited with error code {1}", item.Job.Name, process.ExitCode));
                      throw new ViatronixException(string.Format("Exit code {0}", process.ExitCode), ErrorCodes.None);
                    }

                  }
                  break;


                case (int)WaitEvents.ServerAborted:     // Server Aborted
                  {
                    writer.WriteLine(string.Format("Process {0} has detected that the server is aborting", item.Job.Name, process.ExitCode));

                    /// Kill the process
                    process.Kill();

                    LogErr(string.Format("{0} detected AgentServer is aborting", item.Job.Name), "AgentServer", "ProcessExe");

                  }
                  throw new AbortException("Server is aborting");


                default:
                  {
                    writer.WriteLine(string.Format("Process {0} has detected an invalid handle was retruned from the wait", item.Job.Name, process.ExitCode));

                    LogErr(string.Format("{0} detected invalid event handle ", item.Job.Name), "AgentServer", "ProcessExe");
                  }
                  break;

              }; // END ... Switch on handle


              InsertAgentJobHistory(item.Job, States.Completed);

              writer.WriteLine(string.Format("{0} Completed", item.Job.Name));

          }
          catch (AbortException ex)
          {
             writer.WriteLine(string.Format("{0}  Aborting : {1}",item.Job.Name, ex.Message));

            InsertAgentJobHistory(item.Job, States.Aborted, ex.Message);
          }
          catch (Exception ex)
          {
            writer.WriteLine(string.Format("{0} Failed : {1}", item.Job.Name, ex.Message));

            InsertAgentJobHistory(item.Job, States.Failed, ex.Message);
          }

          /// Inserts the itrem again, but recalculates the NEXT RUN time
          InsertAgentItem(AgentItem.Create(item.Job));

        };  // END ... using the FileStrean


      });

    }
 

    /// <summary>
    /// Insert the agent job history 
    /// </summary>
    /// <param name="job">Agent job jusrt processed</param>
    /// <param name="state">State of the job</param>
    /// <param name="message">Error message if state was failed</param>
    public void InsertAgentJobHistory(AgentJob job, States state, string message = "")
    {

      if ( state == States.Failed )
        LogErr(string.Format("{0} {1} : {2}", job.Name, state, message), "AgentServer", "InsertAgentJobHistory");
      else
        LogInfo(string.Format("{0} {1}", job.Name, state), "AgentServer", "InsertAgentJobHistory");


      AgentJobHistory history = AgentJobHistory.Create(job, state, message);

      AuditingGateway.Insert(m_context, "agentjobhistory", history);


    } // InsertAgentJobHistory(AgentJob job, States state, string message)

    /// <summary>
    /// Builds the agent tem list from the job 
    /// </summary>
    private void BuildAgentItemList()
    {

      Mutex mutex = new Mutex(false, "AGENTSERVER_ITEMS");
      mutex.WaitOne();

      try
      {

        LogDbg("Building the agent item list from the agent jobs", "AgentServer", "BuildAgentItemList");

        m_items.Clear();

        foreach (AgentJob job in m_jobs)
        {
          AgentItem item = AgentItem.Create(job);

          LogDbg(string.Format("Inserting {0} into the list, NextRun: {1}", item.Job.Name, item.NextRun), "AgentServer", "InsertAgentItem");

          m_items.Add(item);
        }

        m_items.Sort(m_comparer);
      }
      finally
      {
        mutex.ReleaseMutex();
      }

    } // BuildAgentItemList()


    /// <summary>
    /// Gets the list of agent jobs
    /// </summary>
    /// <returns></returns>
    private void GetAgentJobs()
    {

      LogDbg("Retrieving the agent jobs", "AgentServer", "GetAgentJobs");

      m_jobs.Clear();

      string xml = SystemGateway.Get(m_context, "agentjob", XmlBuilder.Create("agentjob", new Dictionary<string,string> { {"enabled", "1" } } ));

      EntityCollection.Load<AgentJob>(m_jobs, xml);

    } // GetAgentJobs()



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
        LogWrn(string.Format("Failed to create job abort event  {0}", job.Id), "WorkQueueProcess", "CreateEventHandle");

      return handle;

    } // CreateEventHandle(Job job)


 
    /// <summary>
    /// Creates the server using the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>AgentServer object</returns>
    public static AgentServer Create(XmlNode node)
    {
      AgentServer server = new AgentServer();

      if (server == null)
        throw new OutOfMemoryException("Failed to allocate AgentServer");

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
        Log.Error(server.m_logger, "Invalid procedure specified in configurtaion : " + ex.Message, "AgentServer", "Create");
        throw;
      }

      return server;




    } // Create(XmlNode node)



    #endregion

  } // class AgentServer

  #endregion

} // namespace Viatronix.Enterprise.Servers


