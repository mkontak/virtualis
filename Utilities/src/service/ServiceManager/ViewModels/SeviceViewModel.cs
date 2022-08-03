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

using Viatronix.Enterprise;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Utilities.Service.ServiceManager.Models;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  /// <summary>
  /// Service view model.
  /// </summary>
  class SeviceViewModel : ViewModelBase
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
    private ServiceModel m_model = new ServiceModel();

    private DatabaseViewModel m_databaseViewModel = null;
    private DicomHostsManagerViewModel m_dicomHostsViewModel = null;
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
    public SeviceViewModel() 
    {
      m_databaseViewModel = new DatabaseViewModel(m_model.ServiceProvider);
      m_dicomHostsViewModel = new DicomHostsManagerViewModel();
      m_agentJobsViewModel = new AgentJobsViewModel(m_model.ServiceProvider);

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
    } // DatabaseViewModel


    /// <summary>
    /// Gets the dicom hosts view model.
    /// </summary>
    public DicomHostsManagerViewModel DicomHostsViewModel
    {
      get { return m_dicomHostsViewModel; }
    } // DicomHostsViewModel


    /// <summary>
    /// Database view model
    /// </summary>
    public AgentJobsViewModel AgentJobsViewModel
    {
      get { return m_agentJobsViewModel; }
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
        ISecurityProvider securityProvider = null;

        if(providers.TryFind(ProviderType.Security, ref provider))
        {
          securityProvider = (ISecurityProvider)provider;
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
              securityProvider.Login(loginWindow.Username, loginWindow.Password);
            }
            catch (LoginFailedException)
            {
              // Loop around and try again.
            }
          }
          else
          {
            break;
          }
        }
        while (!securityProvider.IsAuthenticated);
      }
      catch(Exception e)
      {
        if(InitializationFailure != null)
        {
          InitializationFailure(this, EventArgs.Empty);
        }
      }
      finally
      {
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
    } // OnFinalize()

    #endregion

  } // class SeviceViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
