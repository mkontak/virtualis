// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.ServiceModel;
using System.Text;
using System.Threading;
using System.Configuration;

namespace Viatronix.Enterprise.Service
{
  public partial class EnterpriseService : ServiceBase
  {
    
    #region fields

    /// <summary>
    /// log source
    /// </summary>
    private const string Source = "Viatronix.Services.EnterpriseService";

    /// <summary>
    /// service host
    /// </summary>
    private ServiceHost m_host = null;

    /// <summary>
    /// Event logger
    /// </summary>
    private static EventLog m_eventLogger = new EventLog("Application");


    /// <summary>
    /// Servers
    /// </summary>
    private Dictionary<string, Viatronix.Enterprise.Servers.Server> m_servers = new Dictionary<string, Viatronix.Enterprise.Servers.Server>();


    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public EnterpriseService()
    {
      // Name the Windows Service
      this.ServiceName = "Viatronix Enterprise Service";
    } // EnterpriseService()

    #endregion

    #region methods

    /// <summary>
    /// Main method
    /// </summary>
    public static void Main( string[] args )
    {

      m_eventLogger.Source = "EnterpriseService";

      m_eventLogger.WriteEntry("Enterprise Service is initializing");

      EnterpriseService service = new EnterpriseService();




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
          m_eventLogger.WriteEntry("Enterprise service threw exception : " + ex.Message);
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
        System.Windows.Forms.MessageBox.Show("Enterprise server running. Dismiss to shutdown.", "Enterprise Server");

      }
      catch (Exception ex)
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Enterprise Service failed to start  : " + ex.Message, EventLogEntryType.Error);

        // Log critical error
        Viatronix.Logging.Log.Critical("Enterprise service failed to start : " + ex.Message, "EnterpriseService", "Start");
      }
      finally
      {
        // Stop the service
        Stop();
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

        
        Log.Debug( "Enterprise Service is starting...", "EnterpriseService", "OnStart" );

        /// Delay the start if necessary
        Delay();

        /// Loads the schema's no which will validate them
        Viatronix.Schema.LoadSchemas();

        /// Starts any defined internal servers
        Viatronix.Enterprise.Providers.ServerResourceProvider.StartServers();

        if( m_host != null)
          m_host.Close();

   

        m_host = new ServiceHost( typeof( Viatronix.Services.RestService ));

        
        // do we need user validation
        if( m_host.Description.Endpoints.Any( endpoint => 
          {
            WebHttpBinding binding = endpoint.Binding as WebHttpBinding;
            if (binding != null)
              return binding.Security.Mode == WebHttpSecurityMode.TransportCredentialOnly;

            
            
            return false;
          }))
        {
          m_host.Credentials.UserNameAuthentication.UserNamePasswordValidationMode = System.ServiceModel.Security.UserNamePasswordValidationMode.Custom;
          m_host.Credentials.UserNameAuthentication.CustomUserNamePasswordValidator = new SubscriptionServerUserNamePasswordValidtor();
        }

        m_host.Open();

       
        Log.Debug( "Service started.", "EnterpriseService", "OnStart" );
      }
      catch( Exception e )
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Enterprise Service failed to start  : " + e.Message, EventLogEntryType.Error);

        Log.Critical( e.Message, "EnterpriseService", "OnStart" );

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
        Log.Information("Service stopping...", "EnterpriseService", "OnStop");

        if( m_host != null)
        {
          m_host.Close();
          m_host = null;
        }

        /// Stops internal servers
        Viatronix.Enterprise.Providers.ServerResourceProvider.StopServers();

        Log.Information("Service stopped.", "EnterpriseService", "OnStop");
      }
      catch( Exception e )
      {
        // Log failed service startup event 
        m_eventLogger.WriteEntry("Enterprise Service failed to stop  : " + e.Message, EventLogEntryType.Error);

        Log.Critical( e.Message, "EnterpriseService", "OnStop" );

        throw;
      }
    } // OnStop()


    #endregion

  } // class EnterpriseService
} // namespace Viatronix.Enterprise.Server
