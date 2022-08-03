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
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Services
{

  /// <summary>
  /// Implements a windows service for the dicom servers defined in the config file.
  /// </summary>
  public partial class DicomService : ServiceBase
  {

    #region fields

    /// <summary>
    /// Shutdown task used to delay and shutdown the server
    /// </summary>
    private System.Threading.Tasks.Task m_shutdownTask;

    /// <summary>
    /// Server name
    /// </summary>
    private string m_server = System.Environment.MachineName;

    /// <summary>
    /// Dicom Service Log Source
    /// </summary>
    private static string m_logSource = "DicomService";

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
    private static string m_serviceName = "DicomService";

    /// <summary>
    /// display name
    /// </summary>
    private static string m_displayName = "Viatronix Dicom Service";

    /// <summary>
    /// server
    /// </summary>
    private List<Viatronix.Enterprise.Servers.DicomServer> m_servers = new List<Viatronix.Enterprise.Servers.DicomServer>();


    /// <summary>
    /// Flag indicating that we are storing to the database
    /// </summary>
    private bool m_databaseAccess = Boolean.Parse(System.Configuration.ConfigurationManager.AppSettings["database"]);

    /// <summary>
    /// Flag indicating that we are storing thumbnails
    /// </summary>
    private bool m_saveThumbnails = Boolean.Parse(System.Configuration.ConfigurationManager.AppSettings["thumbnails"]);

    /// <summary>
    /// User
    /// </summary>
    private string m_user = string.Empty;

    /// <summary>
    /// Password
    /// </summary>
    private string m_password = string.Empty;


    #endregion

    #region constructors 

    /// <summary>
    /// Constructor
    /// </summary>
    public DicomService()
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
        System.Windows.Forms.MessageBox.Show("Dicom servers started [Count = " + m_servers.Count + "], press OK to exit", "Dicom Service", System.Windows.Forms.MessageBoxButtons.OK);

      }
      catch (Exception ex)
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Dicom Service failed to start  : " + ex.Message, EventLogEntryType.Error);

        // Log critical error
        Viatronix.Logging.Log.Critical(m_logSource, "Dicom service failed to start : " + ex.Message, "DcomService", "Start");
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

      m_eventLogger.Source = DicomService.Name;

      m_eventLogger.WriteEntry("Dicom Service is initializing");

      DicomService service = new DicomService();

      service.ServiceName = DicomService.Name;



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
          m_eventLogger.WriteEntry("Dicom Service threw exception : " + ex.Message);
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

      Viatronix.Logging.Log.Information(m_logSource, string.Format("Delaying startup for {0} milliseconds", delay), "Service", "OnStart");

      // Sleep for the specified delay amount waiting for the database startup to complete
      System.Threading.Thread.Sleep(delay);

    } // Delay()


     #endregion

 

    #region event handlers

    /// <summary>
    /// Start event handler
    /// </summary>
    /// <param name="args">Startup arguments</param>
    protected override void OnStart(string[] args)
    {

      /// Initialize the Loggers
      Logging.Log.Initialize();


      Viatronix.Logging.Log.Information(m_logSource, "Starting the dicom service", "DcomService", "OnStart");

      StringBuilder argList = new StringBuilder();

      //===================================
      // Dump out the args to the log file
      //==================================
      foreach(string arg in args )
      {
        argList.Append(string.Format(" {0} ", arg));
      } // END ... for each arguments

      Viatronix.Logging.Log.Debug(m_logSource, string.Format("Arguments [{0}]", argList.ToString()), "DcomService", "OnStart");


      


      try
      {

        /// Delay the start if necessary
        Delay();


        /// Get the server list
        XmlNodeList serverList = (XmlNodeList)System.Configuration.ConfigurationManager.GetSection("servers");

        //====================================================================================
        // Extract each servers config from the list to create dicom servers and start them
        //=====================================================================================
        foreach (XmlNode node in serverList)
        {

          // Create the dicom server
          Viatronix.Enterprise.Servers.DicomServer server = Viatronix.Enterprise.Servers.DicomServer.Create(node);

          // Hook up the event handlers
          //server.Started += new EventHandler(OnServerStarted);
          server.Failed += new EventHandler<EventArgs<Exception>>(OnServerFailed);

          // Add server to list
          m_servers.Add(server);

          // Log information
          Viatronix.Logging.Log.Information(m_logSource, 
                                            "Starting dicom server [" + m_servers.Count.ToString() + "] : AETitle = " + server.AETitle + "   Port = " + server.Port.ToString(), 
                                            "DcomService", "OnStart");

          // Start the server
          server.Start();

        } // END ... For each server config

      }
      catch (Exception ex)
      {

        m_eventLogger.WriteEntry("Dicom Service failed to start servers : " + ex.Message, EventLogEntryType.Error);

        Viatronix.Logging.Log.Error(m_logSource, "Failed to start dicom server(s) : " + ex.Message, "DcomService", "OnStart");

        throw;
      }


    } // OnStart()

    /// <summary>
    /// Stop event handler
    /// </summary>
    protected override void OnStop()
    {

      Viatronix.Logging.Log.Information(m_logSource, "Stopping the dicom service", "DcomService", "OnStart");


      // ============================================================
      // Loop through all the servers to stop each one individually
      // ============================================================
      foreach (Viatronix.Enterprise.Servers.DicomServer server in m_servers)
      {
        string svr = string.Format("({0},{1})", server.AETitle, server.Port);


        // ====================================================================================
        // Put try catch inside loop so that each server that fails will log an error message
        // ====================================================================================
        try
        {
          // Log stopping message
          Viatronix.Logging.Log.Information(m_logSource, 
                                            string.Format("Stopping dicom server {1}", m_servers.Count, svr), 
                                            "DcomService", "OnStart");

          // Aborts the server
          server.Abort();
        }
        catch (Exception ex)
        {
          // Log Exception
          Viatronix.Logging.Log.Error(m_logSource, 
                                      string.Format("Dicom server {0} failed to stop : {1}", svr, ex.Message), 
                                      "DcomService", "OnStart");
        }

      } // END ... For each dicom server
 
      // Clear all servers from the list
      m_servers.Clear();


    } // OnStop()

    /// <summary>
    /// Handles the server failed event. 
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">arguments</param>
    void OnServerFailed(object sender, EventArgs<Exception> args)
    {


      m_shutdownTask = new System.Threading.Tasks.Task(() =>
        {

          /// Give time for the callback to return
          System.Threading.Thread.Sleep(500);

          ///  Issue a stop
          OnStop();

          // Log failed server event
          m_eventLogger.WriteEntry("Dicom server failed : " + args.Args.Message, EventLogEntryType.Error);

          // Force an exit
          System.Environment.Exit(-1);

        });



    } // OnServerStarted(object sender, EventArgs<Exception> args)


    #endregion

  } // class DcomService

} // namespace Viatronix.Services
