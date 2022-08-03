using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Xml;
using System.Drawing;
using System.Data.Common;
using System.Threading;
using Viatronix.Dicom;


namespace Viatronix.Services
{

  /// <summary>
  /// Implements a windows service for the dicom servers defined in the config file.
  /// </summary>
  public partial class WorkQueueService : ServiceBase
  {

    #region fields

    /// <summary>
    /// Server name
    /// </summary>
    private string m_serverName = System.Environment.MachineName;

     /// <summary>
    /// Event logger
    /// </summary>
    private static EventLog m_eventLogger = new EventLog("Application");


    /// <summary> 
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// Service name
    /// </summary>
    private static string m_serviceName = "WorkQueueService";

    /// <summary>
    /// display name
    /// </summary>
    private static string m_displayName = "Viatronix Work Queue Service";

 
    /// <summary>
    /// server
    /// </summary>
    private Viatronix.Enterprise.Servers.WorkQueueServer m_server;


    /// <summary>
    /// User
    /// </summary>
    private string m_user = string.Empty;

    /// <summary>
    /// Password
    /// </summary>
    private string m_password = string.Empty;

    /// <summary>
    /// Timer used to shutdown upon server failed event
    /// </summary>
    private System.Threading.Timer m_timer = null;

    #endregion

    #region constructors 

    /// <summary>
    /// Constructor
    /// </summary>
    public WorkQueueService()
    { 
  
      InitializeComponent();
    }

    #endregion

    #region properties

    /// <summary>
    /// Returns the friendly display name
    /// </summary>
    public static string Name
    {
      get { return m_serviceName; }
    } // DisplayName

    /// <summary>
    /// Returns the friendly display name
    /// </summary>
    public static string DisplayName
    {
      get { return m_displayName; }
    } // DisplayName

    #endregion

    #region methods

 
    /// <summary>
    /// Starts the service for debugging
    /// </summary>
    //[Conditional("DEBUG")]
    public void Start()
    {

      try
      {


        // Create dummy args
        string[] args = new string[0];

        // Start service
        OnStart(args);

        // Show message box waiting for the user to click exit
        System.Windows.Forms.MessageBox.Show("Work Queue server started, press OK to exit", "Work Queue Service", System.Windows.Forms.MessageBoxButtons.OK);

      }
      catch (Exception ex)
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Work Queue Service failed to start  : " + ex.Message, EventLogEntryType.Error);

        // Log critical error
        Viatronix.Logging.Log.Critical("Work Queue service failed to start : " + ex.Message, "WorkQueueService", "Start");
      }
      finally
      {
        // Stop the service
        Stop();
      }

      /// Does not return from the start so I throw and exception
      throw new System.Exception("DONE");
 
    } // Start()


 
 


    #endregion


    #region Main

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    static void Main(string[] args)
    {

      m_eventLogger.Source = WorkQueueService.Name;

      m_eventLogger.WriteEntry("Work Queue Service is initializing");

      WorkQueueService service = new WorkQueueService();

      service.ServiceName = WorkQueueService.Name;



      try
      {

#if !DEBUG

        if (args.Length > 0 && args[0] == "-test")
          service.Start();
        else
        {
          System.ServiceProcess.ServiceBase[] ServicesToRun = { service };
          System.ServiceProcess.ServiceBase.Run(ServicesToRun);
        }

#else
        // Used for Debugging
        service.Start();
 
#endif
      }
      catch (Exception ex)
      {
        if ( ex.Message == "DONE" )
          Environment.Exit(0); 
        else
          m_eventLogger.WriteEntry("Work queue service threw exception : " + ex.Message);
      }


      return;


    } // Main()


    /// <summary>
    /// Performs the delay start of the service
    /// </summary>
    private void Delay()
    {
      // Get the startup delay
      int delay = ( System.Configuration.ConfigurationManager.AppSettings["delay"] != null ? Int32.Parse(System.Configuration.ConfigurationManager.AppSettings["delay"]) : 0);

      Viatronix.Logging.Log.Information(string.Format("Delaying startup for {0} milliseconds", delay), "WorkQueueService", "OnStart");

      // Sleep for the specified delay amount waiting for the database startup to complete
      System.Threading.Thread.Sleep(delay);

    } // Delay()


    #endregion

 

    #region event handlers

    /// <summary>
    /// Handles the timer callback
    /// </summary>
    /// <param name="state"></param>
    private void OnTimerCallback(object state)
    {
      Stop();
    }

    /// <summary>
    /// Handles the server failed event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnServerFailed(object sender, Viatronix.EventArgs<Exception> args)
    {

      Logging.Log.Error("Received server failed event : " + args.Args.Message, "WorkQueueService", "OnServerFailed");

      m_timer = new Timer(OnTimerCallback, null, 500, 0);
      
    } // OnServerFailed(object sender, Viatronix.EventArgs<Exception> args)

    /// <summary>
    /// Start event handler
    /// </summary>
    /// <param name="args">Startup arguments</param>
    protected override void OnStart(string[] args)
    {
      /// Initialize the Loggers
      Logging.Log.Initialize();


      Viatronix.Logging.Log.Information("Starting the  work queue service", "WorkQueueService", "OnStart");

      StringBuilder argList = new StringBuilder();

      //===================================
      // Dump out the args to the log file
      //==================================
      foreach(string arg in args )
      {
        argList.Append(string.Format(" {0} ", arg));
      } // END ... for each arguments

      Viatronix.Logging.Log.Debug(string.Format("Arguments [{0}]", argList.ToString()), "WorkQueueService", "OnStart");


      


      try
      {

        /// Delay the start if necessary
        Delay();

        /// Get the server list
        XmlNode serverNode = (XmlNode)System.Configuration.ConfigurationManager.GetSection("server");

        // Create the dicom server
        m_server = Viatronix.Enterprise.Servers.WorkQueueServer.Create(serverNode);

        m_server.Failed += OnServerFailed;

        // Log information
        Viatronix.Logging.Log.Information("Starting work queue  server", "WorkQueueService", "OnStart");

        // Start the server
        m_server.Start();

      }
      catch (Exception ex)
      {

        m_eventLogger.WriteEntry("Work Queue Service failed to start server : " + ex.Message, EventLogEntryType.Error);

        Viatronix.Logging.Log.Error("Failed to start work queue server : " + ex.Message, "WorkQueueService", "OnStart");

        throw;
      }


    } // OnStart()

    /// <summary>
    /// Stop event handler
    /// </summary>
    protected override void OnStop()
    {

      Viatronix.Logging.Log.Information("Stopping the work queue service", "Service", "OnStart");

      try
      {

        m_server.Abort();

      }
      catch (Exception ex)
      {

        Logging.Log.Error("Failed to stop work queue service : " + ex.Message, "WorkQueueService", "OnStop");

      }



    } // OnStop()



    #endregion

  } // class WorkQueueService

} // namespace Viatronix.Enterprise.Dicom
