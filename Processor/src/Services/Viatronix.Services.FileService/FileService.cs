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
using Viatronix.IO;


namespace Viatronix.Services
{

  /// <summary>
  /// Implements a windows service for the file server defined in the config file.
  /// </summary>
  public partial class FileService : ServiceBase
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
    private static string m_serviceName = "FileService";

    /// <summary>
    /// display name
    /// </summary>
    private static string m_displayName = "Viatronix File Service";

 
    /// <summary>
    /// server
    /// </summary>
    private Viatronix.IO.FileServer m_server = null;


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
    public FileService()
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
        System.Windows.Forms.MessageBox.Show("File server started, press OK to exit", "File Service", System.Windows.Forms.MessageBoxButtons.OK);

      }
      catch (Exception ex)
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("File Service failed to start  : " + ex.Message, EventLogEntryType.Error);

        // Log critical error
        Viatronix.Logging.Log.Critical("File service failed to start : " + ex.Message, "FileService", "Start");
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

      m_eventLogger.Source = FileService.Name;

      m_eventLogger.WriteEntry("File Service is initializing");

      FileService service = new FileService();

      service.ServiceName = FileService.Name;



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
          m_eventLogger.WriteEntry("File service threw exception : " + ex.Message);
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

      Viatronix.Logging.Log.Information(string.Format("Delaying startup for {0} milliseconds", delay), "FileService", "OnStart");

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


      Viatronix.Logging.Log.Information("Starting the File Server service", "FileService", "OnStart");

      StringBuilder argList = new StringBuilder();

      //===================================
      // Dump out the args to the log file
      //==================================
      foreach(string arg in args )
      {
        argList.Append(string.Format(" {0} ", arg));
      } // END ... for each arguments

      Viatronix.Logging.Log.Debug(string.Format("Arguments [{0}]", argList.ToString()), "FileService", "OnStart");


      


      try
      {

        /// Delay the start if necessary
        Delay();

        /// Get the server list
        XmlNode serverNode = (XmlNode)System.Configuration.ConfigurationManager.GetSection("server");

        // Create the server
        m_server = Viatronix.IO.FileServer.Create(serverNode);

        // Log information
        Viatronix.Logging.Log.Information("Starting File Server", "FileService", "OnStart");

        // Start the server
        m_server.Start();

      }
      catch (Exception ex)
      {

        m_eventLogger.WriteEntry("File Service failed to start server : " + ex.Message, EventLogEntryType.Error);

        Viatronix.Logging.Log.Error("Failed to start File Server : " + ex.Message, "FileService", "OnStart");

        throw;
      }


    } // OnStart()

    /// <summary>
    /// Stop event handler
    /// </summary>
    protected override void OnStop()
    {

      Viatronix.Logging.Log.Information("Stopping the File Server service", "Service", "OnStart");

      try
      {

        m_server.Stop();

      }
      catch (Exception ex)
      {

        Logging.Log.Error("Failed to stop File Server service : " + ex.Message, "FileService", "OnStop");

      }


 

    } // OnStop()



    #endregion

  } // class FileService

} // namespace Viatronix.Enterprise
