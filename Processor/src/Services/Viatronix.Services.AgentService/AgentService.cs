using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Xml;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Viatronix;

namespace WindowsService1
{
  /// <summary>
  /// Implements a windows service for the dicom servers defined in the config file.
  /// </summary>
  public partial class AgentService : ServiceBase
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
    /// Service name
    /// </summary>
    private static string m_serviceName = "AgentService";

    /// <summary>
    /// display name
    /// </summary>
    private static string m_displayName = "Viatronix Agent Service";


    /// <summary>
    /// server
    /// </summary>
    private Viatronix.Enterprise.Servers.AgentServer m_server;


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
    public AgentService()
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
        System.Windows.Forms.MessageBox.Show("Agent server started, press OK to exit", "Agent Service", System.Windows.Forms.MessageBoxButtons.OK);

      }
      catch (Exception ex)
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Agent Service failed to start  : " + ex.Message, EventLogEntryType.Error);

        // Log critical error
        Viatronix.Logging.Log.Critical("Agent service failed to start : " + ex.Message, "AgentService", "Start");
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

      m_eventLogger.Source = AgentService.Name;

      m_eventLogger.WriteEntry("Agent Service is initializing");

      AgentService service = new AgentService();

      service.ServiceName = AgentService.Name;



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
        if (ex.Message == "DONE")
          Environment.Exit(0);
        else
          m_eventLogger.WriteEntry("Agent service threw exception : " + ex.Message);
      }


      return;


    } // Main()


    /// <summary>
    /// Performs the delay start of the service
    /// </summary>
    private void Delay()
    {
      // Get the startup delay
      int delay = (System.Configuration.ConfigurationManager.AppSettings["delay"] != null ? Int32.Parse(System.Configuration.ConfigurationManager.AppSettings["delay"]) : 0);

      Viatronix.Logging.Log.Information(string.Format("Delaying startup for {0} milliseconds", delay), "AgentService", "OnStart");

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

      Viatronix.Logging.Log.Error("Received server failed event : " + args.Args.Message, "AgentService", "OnServerFailed");

      m_timer = new System.Threading.Timer(OnTimerCallback, null, 500, 0);

    } // OnServerFailed(object sender, Viatronix.EventArgs<Exception> args)

    /// <summary>
    /// Start event handler
    /// </summary>
    /// <param name="args">Startup arguments</param>
    protected override void OnStart(string[] args)
    {
      /// Initialize the Loggers
      Viatronix.Logging.Log.Initialize();


      Viatronix.Logging.Log.Information("Starting the  agent service", "AgentService", "OnStart");

      StringBuilder argList = new StringBuilder();

      //===================================
      // Dump out the args to the log file
      //==================================
      foreach (string arg in args)
      {
        argList.Append(string.Format(" {0} ", arg));
      } // END ... for each arguments

      Viatronix.Logging.Log.Debug(string.Format("Arguments [{0}]", argList.ToString()), "AgentService", "OnStart");





      try
      {

        /// Delay the start if necessary
        Delay();

        /// Get the server list
        XmlNode serverNode = (XmlNode)System.Configuration.ConfigurationManager.GetSection("server");

        // Create the dicom server
        m_server = Viatronix.Enterprise.Servers.AgentServer.Create(serverNode);

        m_server.Failed += OnServerFailed;

        // Log information
        Viatronix.Logging.Log.Information("Starting agent  server", "AgentService", "OnStart");

        // Start the server
        m_server.Start();

      }
      catch (Exception ex)
      {

        m_eventLogger.WriteEntry("Agent Service failed to start server : " + ex.Message, EventLogEntryType.Error);

        Viatronix.Logging.Log.Error("Failed to start agent server : " + ex.Message, "AgentService", "OnStart");

        throw;
      }


    } // OnStart()

    /// <summary>
    /// Stop event handler
    /// </summary>
    protected override void OnStop()
    {

      Viatronix.Logging.Log.Information("Stopping the agent service", "Service", "OnStart");

      try
      {

        m_server.Abort();

      }
      catch (Exception ex)
      {

        Viatronix.Logging.Log.Error("Failed to stop agent service : " + ex.Message, "AgentService", "OnStop");

      }



    } // OnStop()



    #endregion

  } // class AgentService
}
