using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Configuration.Install;

namespace Viatronix
{

  #region FolderMonitoringServiveInstall class

  /// <summary>
  /// Impelements an installer for the dicom service
  /// </summary>
  [RunInstaller(true)]
  public class FolderMonitoringServiveInstaller : System.Configuration.Install.Installer
  {


    #region fields

    /// <summary>
    /// servce installer
    /// </summary>
    private ServiceInstaller m_serviceInstaller;


    /// <summary>
    /// process installer
    /// </summary>
    private ServiceProcessInstaller m_serviceProcessInstaller;

    #endregion


    #region construtors

    /// <summary>
    /// Construtor
    /// </summary>
    public FolderMonitoringServiveInstaller()
      : base()
    {
      m_serviceProcessInstaller = new ServiceProcessInstaller();
      m_serviceInstaller = new ServiceInstaller();

      m_serviceProcessInstaller.Account = ServiceAccount.LocalSystem;
      m_serviceInstaller.StartType = ServiceStartMode.Automatic;
      m_serviceInstaller.ServiceName = "FolderMonitoringService";
      m_serviceInstaller.DisplayName = "Viatronix Folder Monitoring Service";

      Installers.Add(m_serviceInstaller);
      Installers.Add(m_serviceProcessInstaller);

      // Attach the 'Committed' event.
      this.Committed += new InstallEventHandler(CommittedHandler);

      // Attach the 'Committing' event.
      this.Committing += new InstallEventHandler(CommittingHandler);

    } // DicomServiceInstaller()

    #endregion


    #region methods

    /// <summary>
    /// Installs service
    /// </summary>
    /// <param name="stateServer">state server</param>
    public override void Install(IDictionary stateServer)
    {
      Microsoft.Win32.RegistryKey system,
        //HKEY_LOCAL_MACHINE\Services\CurrentControlSet
                                  currentControlSet,
        //...\Services
                                  services,
        //...\<Service Name>
                                  service,
        //...\Parameters - this is where you can put service-specific configuration
                                  config;

      Console.WriteLine("Perfoming installation");

      try
      {
        //Let the project installer do its job
        base.Install(stateServer);


        Console.WriteLine("Updating the service description");

        //Open the HKEY_LOCAL_MACHINE\SYSTEM key
        system = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("System");

        //Open CurrentControlSet
        currentControlSet = system.OpenSubKey("CurrentControlSet");

        //Go to the services key
        services = currentControlSet.OpenSubKey("Services");

        //Open the key for your service, and allow writing
        service = services.OpenSubKey(m_serviceInstaller.ServiceName, true);

        //Add your service's description as a REG_SZ value named "Description"
        service.SetValue("Description", "Viatronix folder monitoring service will email and log events prtaining to changes in the folder.");

        //(Optional) Add some custom information your service will use...
        config = service.CreateSubKey("Parameters");

      }
      catch (Exception ex)
      {
        Console.WriteLine("An exception was thrown during service installation:\n" + ex.ToString());
      }
    } // Install(IDictionary stateServer)

    /// <summary>
    /// Uninstalls service
    /// </summary>
    /// <param name="stateServer"></param>
    public override void Uninstall(IDictionary stateServer)
    {
      Microsoft.Win32.RegistryKey system,
        currentControlSet,
        services,
        service;

      try
      {
        //Drill down to the service key and open it with write permission
        system = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("System");
        currentControlSet = system.OpenSubKey("CurrentControlSet");
        services = currentControlSet.OpenSubKey("Services");
        service = services.OpenSubKey(m_serviceInstaller.ServiceName, true);
        //Delete any keys you created during installation (or that your service created)
        service.DeleteSubKeyTree("Parameters");
        //...
      }
      catch (Exception ex)
      {
        Console.WriteLine("Exception encountered while uninstalling service:\n" + ex.ToString());
      }
      finally
      {
        //Let the project installer do its job
        base.Uninstall(stateServer);
      }
    }

    #endregion


    #region event handler

    /// <summary>
    /// Handles the comitting event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="e">instaler args</param>
    private void CommittingHandler(object sender, System.Configuration.Install.InstallEventArgs e)
    {
      Console.WriteLine("");
      Console.WriteLine("Committing Event occured.");
      Console.WriteLine("");
    } // CommittingHandler(object sender, System.Configuration.Install.InstallEventArgs e)


    /// <summary>
    /// Handles the comitted event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="e">installer args</param>
    private void CommittedHandler(object sender, System.Configuration.Install.InstallEventArgs e)
    {
      Console.WriteLine("");
      Console.WriteLine("Committed Event occured.");
      Console.WriteLine("");
    }   // CommittedHandler(object sender, System.Configuration.Install.InstallEventArgs e)




    #endregion

    #region overrides

    /// <summary>
    /// Overrides the commit methods
    /// </summary>
    /// <param name="savedState">saved state</param>
    public override void Commit(IDictionary savedState)
    {
      base.Commit(savedState);
    } // Commit(IDictionary savedState)

    /// <summary>
    /// Overrdies the rollback methods
    /// </summary>
    /// <param name="savedState">saved state</param>
    public override void Rollback(IDictionary savedState)
    {
      base.Rollback(savedState);
    } // Rollback(IDictionary savedState)

    #endregion

  }	// class DicomServiceInstaller

  #endregion


  #region FolderMonitoringServive Class

  /// <summary>
  /// Implements the Folder Monitor Service
  /// </summary>
  public partial class FolderMonitoringService : ServiceBase
  {


    /// <summary>
    /// Folders to monitor
    /// </summary>
    private Server m_server;

    public FolderMonitoringService()
    {
      m_server = new Server((Dictionary<string,Folder>)(System.Configuration.ConfigurationManager.GetSection("folders")));
    }


    #region methods

    /// <summary>
    /// Starts the service
    /// </summary>
    public void Start()
    {
      OnStart(new string[0]);
      while (true) ;
    } // Start()

    #endregion

    protected override void OnStart(string[] args)
    {

      m_server.Start();     

    }

    protected override void OnStop()
    {
      m_server.Abort();
    }


  }

  #endregion

}
