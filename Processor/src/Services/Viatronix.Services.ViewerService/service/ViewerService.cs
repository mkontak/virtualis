using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ServiceModel;
using System.ServiceProcess;
using System.Configuration;
using Viatronix.Launching;

namespace Viatronix.Services.ViewerService
{
  public partial class ViewerService : ServiceBase
  {
    #region fields

    /// <summary>
    /// log source
    /// </summary>
    private const string Source = "Viatronix.Viewer.Services.Server";

    /// <summary>
    /// service host
    /// </summary>
    private ServiceHost m_host = null;

    /// <summary>
    /// Event logger
    /// </summary>
    private static EventLog m_eventLogger = new EventLog("Application");


    /// <summary>
    /// List of viewers
    /// </summary>
    private List<Session> m_viewers = new List<Session>();

    private ViewerRestService m_service;


    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public ViewerService()
    {
      // Name the Windows Service
      this.ServiceName = "Viatronix Viewer Service";
    } // ViewerService()

    #endregion

    #region methods

    /// <summary>
    /// Main method
    /// </summary>
    [STAThread]
    public static void Main( string[] args )
    {

      m_eventLogger.Source = "ViewerService";

      m_eventLogger.WriteEntry("Viewer Service is initializing");

      ViewerService service = new ViewerService();




      try
      {

#if !DEBUG

        if (args.Length > 0 && args[0] == "-test")
           service.Start();
        else
           ServiceBase.Run( service );

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
          m_eventLogger.WriteEntry("Viewer service threw exception : " + ex.Message);
      }


    } // Main()

    /// <summary>
    /// Performs the delay start of the service
    /// </summary>
    private void Delay()
    {
      // Get the startup delay
      int delay = (System.Configuration.ConfigurationManager.AppSettings["delay"] != null ? Int32.Parse(System.Configuration.ConfigurationManager.AppSettings["delay"]) : 0);

      Viatronix.Logging.Log.Information(string.Format("Delaying startup for {0} milliseconds", delay), "Service", "OnStart");

      // Sleep for the specified delay amount waiting for the database startup to complete
      System.Threading.Thread.Sleep(delay);

    } // Delay()


    /// <summary>
    /// Starts the window service (Debug only)
    /// </summary>
    public void Start()
    {
  
      try
      {


        // Create dummy args
        string[] args = new string[0];

        // Start service
        OnStart(args);

        // Show message box waiting for the user to click exit
        System.Windows.Forms.MessageBox.Show("Viewer server running. Dismiss to shutdown.", "Viewer Server");

      }
      catch (Exception ex)
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Viewer Service failed to start  : " + ex.Message, EventLogEntryType.Error);

        // Log critical error
        Viatronix.Logging.Log.Critical("Viewer service failed to start : " + ex.Message, "ViewerService", "Start");
      }
      finally
      {
        // Stop the service
        OnStop();
      }

      /// Does not return from the start so I throw and exception
      throw new System.Exception("DONE");


    } // Start()



    /// <summary>
    /// Starts the windows service
    /// </summary>
    /// <param name="args">command line args</param>
    protected override void OnStart( string[] args )
    {
      try
      {


        /// Initialize the Loggers
        Viatronix.Logging.Log.Initialize();

        
        Logging.Log.Debug( "Viewer Service is starting...", "ViewerService", "OnStart" );

        /// Delay the start if necessary
        Delay();

        if( m_host != null)
          m_host.Close();


        m_service = new ViewerRestService();

        m_host = new ServiceHost(m_service);
        var behavior = m_host.Description.Behaviors.Find<ServiceBehaviorAttribute>();
        behavior.InstanceContextMode = InstanceContextMode.Single;

        //m_host = new ServiceHost(typeof(Viatronix.Services.ViewerRestService));

        
        
        // do we need user validation
        //if( m_host.Description.Endpoints.Any( endpoint => 
        //  {
        //    WebHttpBinding binding = endpoint.Binding as WebHttpBinding;
        //    if (binding != null)
        //      return binding.Security.Mode == WebHttpSecurityMode.TransportCredentialOnly;

        //    return false;
        //  }))
        //{
        //  m_host.Credentials.UserNameAuthentication.UserNamePasswordValidationMode = System.ServiceModel.Security.UserNamePasswordValidationMode.Custom;
        //  m_host.Credentials.UserNameAuthentication.CustomUserNamePasswordValidator = new SubscriptionServerUserNamePasswordValidtor();
        //}

        m_host.Open();


        Logging.Log.Debug("Service started.", "ViewerService", "OnStart");
      }
      catch( Exception e )
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Viewer Service failed to start  : " + e.Message, EventLogEntryType.Error);

        Logging.Log.Critical(e.Message, "ViewerService", "OnStart");

        throw;
      }
    } // OnStart( args )


    /// <summary>
    /// Stops the service
    /// </summary>
    protected override void OnStop()
    {
      try
      {
        Logging.Log.Information("Service stopping...", "ViewerService", "OnStop");

        if( m_host != null)
        {
          m_host.Close();
          m_host = null;
        }

        m_service.Dispose();

        Logging.Log.Information("Service stopped.", "ViewerService", "OnStop");

      }
      catch( Exception e )
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Viewer Service failed to stop  : " + e.Message, EventLogEntryType.Error);

        Logging.Log.Critical(e.Message, "ViewerService", "OnStop");

        throw;
      }
    } // OnStop()


    #endregion

  }
}
