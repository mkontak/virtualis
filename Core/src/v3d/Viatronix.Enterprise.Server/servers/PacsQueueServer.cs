
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
using Viatronix.Enterprise.Logging;


namespace Viatronix.Enterprise.Servers
{

  /// <summary>
  /// PACS Queue Server
  /// </summary>
  public class PacsQueueServer : QueueServer
  {

    #region constants


    /// <summary>
    /// Machine Name
    /// </summary>
    private static string m_machineName = System.Environment.MachineName;

    #endregion

    #region fields


 
    /// <summary>
    /// Jobs Pending Event name
    /// </summary>
    private string m_jobsPendingEventName = string.Empty;

 
    /// <summary>
    /// AE Title
    /// </summary>
    private string m_aetitle = "V3D";

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
    private Dictionary<int, PacsJob> m_jobs = new Dictionary<int, PacsJob>();

    /// <summary>
    /// Threaded tasks being processed
    /// </summary>
    private List<System.Threading.Tasks.Task> m_tasks = new List<System.Threading.Tasks.Task>();



    /// <summary>
    /// Settings
    /// </summary>
    private List<Setting> m_settings = null;


    #endregion

    #region constructors and destructors

    /// <summary>
    /// Constructor
    /// </summary>
    private PacsQueueServer() : base("PacsQueue") { }

    
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="settings"></param>
    public PacsQueueServer(string settings) : base("PacsQueue", settings) { }

    #endregion

    #region overrides

    /// <summary>
    /// Creates the server using the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>PacsQueueServer object</returns>
    protected override  void Initialize(XmlNode node)
    {

      try
      {

        base.Initialize(node);

        Setting setting = new Setting();

        if (m_settings.TryFind("DiskErrorThreshold", ref setting))
          m_diskErrorThreshold = UInt32.Parse(setting.Value);

        if (m_settings.TryFind("DiskWarningThreshold", ref setting))
          m_diskWarningThreshold = UInt32.Parse(setting.Value);



        m_aetitle = node.Attributes["aetitle"].Value;
        node.TryGetAttributeValue<UInt32>("diskErrorThreshold", ref m_diskErrorThreshold);
        node.TryGetAttributeValue<UInt32>("diskWarningThreshold", ref m_diskWarningThreshold);

 

        m_maximumJobsAllowed = UInt16.Parse(node.Attributes["concurrentTasks"].Value.ToString());

      }
      catch (Exception ex)
      {
        LogErr(string.Format("Invalid configuration : {0}", ex.Message), "PacsQueueServer", "Initialize");
        throw;
      }



    } // Initialize(XmlNode node)



    /// <summary>
    /// Executes the WorkQueue 
    /// </summary>
    public override void RunSynchronously()
    {

 

      try
      {


        // Started
        OnStarted();

        /// Resets the PACS JOBS that were processing
        StorageGateway.ResetJobs(m_context);




        while (true)
        {



          // Get the next task to process
          PacsJob job = GetNextJob(); ;

 
    

          LogDbg(string.Format("Starting {0}-{1}-{2}", job.Id, job.Operation, job.SeriesUid), "PacsQueueServer", "Run");

          System.Threading.Tasks.Task threadTask = System.Threading.Tasks.Task.Factory.StartNew(() =>
          {


            try
            {

              switch (job.Operation)
              {

                case PacsJob.Operations.Query:
                  throw new NotSupportedException("PACS QUERY operation not supported");
                case PacsJob.Operations.Retreive:
                  Viatronix.Dicom.CMove move = Viatronix.Dicom.CMove.Create(m_aetitle, job.Host);
                  break;
                case PacsJob.Operations.Store:
                  throw new NotSupportedException("PACS STORE operation not supported");
                default:
                  throw new NotSupportedException("PACS operation not supported");
              }
             
  
              
            }
            catch (Exception ex)
            {

              LogErr(string.Format("Job {0} failed : {1}", job.Operation, ex.Message), "PacsQueueServer", "Run");

              job.State = PacsJob.States.Failed;
              job.Message = ex.Message;
              StorageGateway.Update(m_context, "pacsjob", job.ToXml() );


              throw new ViatronixException(string.Format("{0} failed : {0}", job.Operation, ex.Message), ex, ErrorCodes.None);
            }


          });

          // Adds task to the list
          m_tasks.Add(threadTask);

        }  // END ... while forever


      }
      catch ( AbortException )
      {

        OnAborted();

        LogErr("Pacs queue server is being aborted", "Server", "Run");

        throw;

      }
      catch (Exception ex)
      {

        OnFailed(ex);

        LogErr("Pacs queue server encountered exception : " + ex.Message, "Server", "Run");

        LogErr("Pacs queue is stopping", "Server", "Run");

        throw;

      }

    } // Run()


    #endregion

    #region methods

    /// <summary>
    /// Cleanup completed and failed tasks
    /// </summary>
    private void CleanupTasks()
    {
      List<int> removalList = new List<int>();

      // ====================================
      // Remove any task completed or failed
      // ====================================
      for (int i = m_tasks.Count - 1; i >= 0; --i)
      {
        System.Threading.Tasks.Task task = m_tasks[i];

        if (m_tasks[i].IsCompleted || m_tasks[i].IsFaulted)
          m_tasks.RemoveAt(i);

      } // END ... for each task



    }

    /// <summary>
    /// Cleans up any completed or failed jobs from the list
    /// </summary>
    private void CleanupJobs()
    {

      LogDbg("Cleaning up any completed or failed pacs jobs from the internal list", "PacsQueueServer", "GetNextTask");

      /// Setup removal keys for jobs
      List<int> removalKeys = new List<int>();

      // ===============================================================
      // Remove any completed of failed jobs we are done processing them
      // ================================================================
      foreach (KeyValuePair<int, PacsJob> pair in m_jobs)
      {

        // ======================================================================================
        // If the job is completed or failed we will delete it from out list so that 
        // a new job can be retrieved.
        // =====================================================================================
        if (pair.Value.State == PacsJob.States.Completed || pair.Value.State == PacsJob.States.Failed)
          removalKeys.Add(pair.Key);

      } // END ... For each


      // ======================
      // Do the actual removal
      // ======================
      foreach (int key in removalKeys)
      { m_jobs.Remove(key); }

    } // CleanupJobs()


    /// <summary>
    /// Gets the next task to process. Will only return if a task is available for processing otherwise it remains blocking for either 
    /// a pending event or abort event. If the maximum concurrent tasks are already running then we will wait for a task to completed 
    /// before find the next task to process.
    /// </summary>
    /// <returns>Task to be processed</returns>
    private PacsJob GetNextJob()
    {

      PacsJob job = null;

      LogDbg("Attempting to get the next pacs job to be processed", "PacsQueueServer", "GetNextJob");

      // ==============================================================================================
      // Will wait for any task to finish if we have reached the maximum number of concurrent tasks
      // allowed.
      // =============================================================================================
      WaitForJobs();


      // ===================================================================
      // Loop waiting for a job or task to become available for processing.
      // ===================================================================
      do
      {


        // CLeanup any completed of failed jobs in the internal list
        CleanupJobs();


        // ==========================================================================================
        // If the we have less then the maximum number of jobs then get the next job and add to
        // out internal list.
        // =========================================================================================
        if (m_jobs.Count < m_maximumJobsAllowed)
        {
          LogDbg("Attempting to get pending job from the database", "PacsQueueServer", "GetNextTask");

          job = StorageGateway.GetPacsNextJob(m_context, System.Environment.MachineName);

          // If it is not null then add to the list
          if (job != null)
            m_jobs.Add(job.Id, job);

        } // END ... If we have less then the maximum job allowed


        // ======================================================================
        // If no job was returned then get find a job with a pending task
        // ======================================================================
        if (job == null)
        {

          LogDbg("Checking for job that is pending in the internal list", "PacsQueueServer", "GetNextTask");

          // ===============================================================
          // Find the first job not processing
          // ================================================================
          foreach (KeyValuePair<int, PacsJob> pair in m_jobs)
          {
            if (pair.Value.State == PacsJob.States.Pending)
            {
              job = pair.Value;
              break;
            }
          } // END ... For each
        }


        // ==========================================================================================================
        // If a job is available for processing 
        // ==========================================================================================================
        if (job == null )
          WaitForPendingJobs();
 

      } while (job == null);


      return job;


    } // GetNextJob()




    /// <summary>
    /// Gets the list of paths used for processing
    /// </summary>
    /// <param name="job"Job</param>
    /// <param name="task">Task</param>
    /// <returns>List of paths retrieved for the job and task</returns>
    private Dictionary<string, string> GetPaths(Job job)
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

    } // GetPaths()


    /// <summary>
    /// Waits for a spare task slot to become available
    /// </summary>
    private void WaitForJobs()
    {

      // =================================================================================
      // If we are at the maximum task allowed then we need to wait for one to complete
      // =================================================================================
      if (m_tasks.Count == m_maximumJobsAllowed)
      {
        // =============================================================================================
        // Exceptions thrown by tasks will be propagated to the main thread
        // while it waits for the tasks. The actual exceptions will be wrapped in AggregateException.
        // ===============================================================================================
        try
        {

          LogInfo("Maximum concurrent tasks reached, waiting for any current task to completed", "PacsQueueServer", "Run");

          // Wait for any of the threads being processed
          int index = System.Threading.Tasks.Task.WaitAny(m_tasks.ToArray());


          CleanupTasks();

        }
        catch (AggregateException ex)
        {

          LogErr("One or mores tasks failed, see exception below", "WorkQueuServer", "Run");

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


      } // END ... If we are at the maximum tasks allowed


    } // WaitForTaskSlot()

 

    /// <summary>
    /// Creates the server using the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>PacsQueueServer object</returns>
    public static PacsQueueServer Create(XmlNode node)
    {
      PacsQueueServer server = new PacsQueueServer();

      if (server == null)
        throw new OutOfMemoryException("Failed to allocate PacsQueueServer");

      server.Initialize(node);

      return server;
    } // Create(XmlNode node)



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


  }
}
