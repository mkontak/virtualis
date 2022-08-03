using System;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Helpers;
using System.Windows.Controls;

using Viatronix.Logging;
using Viatronix.Enterprise;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Utilities.Service.ServiceManager.Models;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  /// <summary>
  /// Service view model.
  /// </summary>
  class SeviceManagerViewModel : ViewModelBase
  {

    #region events

    /// <summary>
    /// Notifies that the initialize has started.
    /// </summary>
    public event EventHandler Initializing;

    /// <summary>
    /// Notifies that the initialize has completed.
    /// </summary>
    public event EventHandler Initialized;

    /// <summary>
    /// Notifies that an initialization failure.
    /// </summary>
    public event EventHandler InitializationFailure;

    #endregion

    #region fields

    /// <summary>
    /// The service manager.
    /// </summary>
    private ServiceModel m_model = null;

    /// <summary>
    /// The security provider.
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    /// <summary>
    /// The service provider.
    /// </summary>
    private Viatronix.Enterprise.SDK.Providers.IServiceProvider m_serviceProvider = null;

    private DatabaseViewModel m_databaseViewModel = null;
    private DicomHostsManagerViewModel m_dicomHostsViewModel = null;
    private ConfigurationFilesManagerViewModel m_configurationFilesViewModel = null;
    private AgentJobsViewModel m_agentJobsViewModel = null;

    private RelayCommand m_refreshCommand;
    private RelayCommand m_updateCommand;
    private RelayCommand m_deleteCommand;

    private RelayCommand<Viatronix.Enterprise.Entities.Database> m_selectedDatabaseChanged;
    private RelayCommand<Viatronix.Enterprise.Entities.Table> m_selectedTableChanged;


    /// <summary>
    /// Status message
    /// </summary>
    private string m_statusMessage = string.Empty;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    public SeviceManagerViewModel() 
    {
      InitializeCommand = new RelayCommand(OnInitialize);
      FinalizeCommand = new RelayCommand(OnFinalize);
    } // SeviceViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Database view model
    /// </summary>
    public DatabaseViewModel DatabaseViewModel
    {
      get { return m_databaseViewModel; }
      private set
      {
        m_databaseViewModel = value;
        RaisePropertyChanged("DatabaseViewModel");
      }
    } // DatabaseViewModel


    /// <summary>
    /// Gets the dicom hosts view model.
    /// </summary>
    public DicomHostsManagerViewModel DicomHostsViewModel
    {
      get { return m_dicomHostsViewModel; }
      private set
      {
        m_dicomHostsViewModel = value;
        RaisePropertyChanged("DicomHostsViewModel");
      }
    } // DicomHostsViewModel


    /// <summary>
    /// Gets or sets the configuration files view model.
    /// </summary>
    public ConfigurationFilesManagerViewModel ConfigurationFilesViewModel
    {
      get { return m_configurationFilesViewModel; }
      set
      {
        m_configurationFilesViewModel = value;
        RaisePropertyChanged("ConfigurationFilesViewModel");
      }
    } // ConfigurationFilesViewModel


    /// <summary>
    /// Database view model
    /// </summary>
    public AgentJobsViewModel AgentJobsViewModel
    {
      get { return m_agentJobsViewModel; }
      private set
      {
        m_agentJobsViewModel = value;
        RaisePropertyChanged("AgentJobsViewModel");
      }
    } // AgentJobsViewModel


    /// <summary>
    /// Gets/Sets the status message
    /// </summary>
    public string StatusMessage
    {
      get { return m_statusMessage; }
      set { RaisePropertyChanged<string>("StatusMessage", m_statusMessage, value, false); }
    } // StatusMessage

    #endregion
    
    #region commands

    /// <summary>
    /// Gets or sets the initialize command.
    /// </summary>
    public RelayCommand InitializeCommand
    {
      get;
      private set;
    } // InitializeCommand


    /// <summary>
    /// Gets or sets the finalize command.
    /// </summary>
    public RelayCommand FinalizeCommand
    {
      get;
      private set;
    } // FinalizeCommand

    #endregion
    
    #region methods

    /// <summary>
    /// Performs initialization.
    /// </summary>
    private void OnInitialize()
    {
      try
      {
        if(Initializing != null)
        {
          Initializing(this, EventArgs.Empty);
        }

        // --------------------------------
        // Perform the login.
        // ================================

        List<IProvider> providers = ProviderFactory.Create();

        IProvider provider = null;
        if(providers.TryFind(ProviderType.Security, ref provider))
        {
          m_securityProvider = (ISecurityProvider)provider;
        }
        else
        {
          throw new ViatronixException("Application configuration was incorrect.", ErrorCodes.None);
        }

        provider = null;
        if (providers.TryFind(ProviderType.Service, ref provider))
        {
          m_serviceProvider = (Viatronix.Enterprise.SDK.Providers.IServiceProvider)provider;
        }
        else
        {
          throw new ViatronixException("Application configuration was incorrect.", ErrorCodes.None);
        }

        do
        {
          // Show login form.
          LoginWindow loginWindow = new LoginWindow();
          loginWindow.Topmost = true;
          loginWindow.Focus();

          if (loginWindow.ShowDialog() == true)
          {
            try
            {
              m_securityProvider.Login(loginWindow.Username, loginWindow.Password);
            }
            catch (ViatronixException)
            {
              // Loop around and try again.
            }
          }
          else
          {
            throw new ViatronixException("User chose to cancel.", ErrorCodes.None);
          }
         }
        while (!m_securityProvider.IsAuthenticated);
      }
      catch(Exception)
      {
        if(InitializationFailure != null)
        {
          InitializationFailure(this, EventArgs.Empty);
        }
      }
      finally
      {
        if(m_securityProvider.IsAuthenticated)
        {
          m_model = new ServiceModel(m_securityProvider, m_serviceProvider);

          m_databaseViewModel = new DatabaseViewModel(m_model.ServiceProvider);
          DicomHostsViewModel = new DicomHostsManagerViewModel();
          ConfigurationFilesViewModel = new ConfigurationFilesManagerViewModel();
          m_agentJobsViewModel = new AgentJobsViewModel(m_model.ServiceProvider);
        }

        if (Initialized != null)
        {
          Initialized(this, EventArgs.Empty);
        }
      }
    } // OnInitialize()


    /// <summary>
    /// Performs finalization.
    /// </summary>
    private void OnFinalize()
    {
      try
      {
        m_securityProvider.Logout();
      }
      catch(Exception e)
      {
        Log.Error(string.Format("Failed to log out.  [ERROR=\"{0}\"]", e.Message), "ServiceManagerViewModel", "OnFinalize");
      }
    } // OnFinalize()

    #endregion

  } // class SeviceManagerViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
