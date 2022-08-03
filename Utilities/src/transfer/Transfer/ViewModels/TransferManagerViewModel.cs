using System;
using System.IO;
using System.Threading;
using System.Configuration;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;

using Viatronix.Dicom;
using Viatronix.Imapi;
using Viatronix.Logging;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Data;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Utilities.Transfer.ViewModels
{
  /// <summary>
  /// The view model to manage the application.
  /// </summary>
  public class TransferManagerViewModel : ViewModelBase
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
    /// Is the transfer active?
    /// </summary>
    private bool m_isActive = false;

    /// <summary>
    /// The list of providers.
    /// </summary>
    private List<IProvider> m_providers = new List<IProvider>();

    /// <summary>
    /// The security provider.
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    /// <summary>
    /// The data provider.
    /// </summary>
    private IDataProvider m_dataProvider = null;
    
    /// <summary>
    /// The transfer providers.
    /// </summary>
    private ObservableCollection<TransferProviderViewModel> m_transferProviders = new ObservableCollection<TransferProviderViewModel>();

    /// <summary>
    /// The selected transfer provider.
    /// </summary>
    private TransferProviderViewModel m_selectedTransferProvider = null;

    /// <summary>
    /// Is anonymization going to be done?
    /// </summary>
    private bool m_isAnonymizeChecked = false;

    /// <summary>
    /// The list of hosts.
    /// </summary>
    private ObservableCollection<DicomHost> m_hosts = new ObservableCollection<DicomHost>();

    /// <summary>
    /// The list of devices.
    /// </summary>
    private ObservableCollection<DeviceViewModel> m_devices = new ObservableCollection<DeviceViewModel>();

    /// <summary>
    /// The selected device.
    /// </summary>
    private DeviceViewModel m_selectedDevice = null;

    /// <summary>
    /// The transfer argument.
    /// </summary>
    private object m_transferArgs = null;

    /// <summary>
    /// The transfer list.
    /// </summary>
    private TransferList m_transferList = null;

    /// <summary>
    /// The list of studies.
    /// </summary>
    private ObservableCollection<StudyViewModel> m_studies = new ObservableCollection<StudyViewModel>();

    /// <summary>
    /// The list of transfer items, that apply to the selected series.
    /// </summary>
    private ObservableCollection<TransferItemViewModel> m_transferItems = new ObservableCollection<TransferItemViewModel>();
    
    /// <summary>
    /// A timer to refresh the device.
    /// </summary>
    private System.Threading.Timer m_devicesRefreshTimer = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public TransferManagerViewModel()
    {
      InitializeCommand = new RelayCommand(OnInitialize);
      FinalizeCommand = new RelayCommand(OnFinalize);
      WindowClosingCommand = new RelayCommand<CancelEventArgs>(OnWindowClosing);
      SelectedItemChangedCommand = new RelayCommand<RoutedPropertyChangedEventArgs<object>>(OnSelectedTreeViewItemChanged);
      BrowseCommand = new RelayCommand(OnBrowse);
      StartTransferCommand = new RelayCommand(OnTransfer);

      IsActive = false;
    } // ExporterManagerViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets if the transfer is active.
    /// </summary>
    public bool IsActive
    {
      get { return m_isActive; }
      set
      {
        m_isActive = value;
        RaisePropertyChanged("IsActive");
        RaisePropertyChanged("IsNotActive");
      }
    } // IsActive


    /// <summary>
    /// Gets if the transfer is NOT active.
    /// </summary>
    public bool IsNotActive
    {
      get { return !m_isActive; }
    } // IsNotActive


    /// <summary>
    /// Gets or sets the transfer modes.
    /// </summary>
    public ObservableCollection<TransferProviderViewModel> TransferProviders
    {
      get { return m_transferProviders; }
      set
      {
        m_transferProviders = value;
        RaisePropertyChanged("TransferProviders");
      }
    } // TransferProviders


    /// <summary>
    /// Gets or sets the selected transfer mode.
    /// </summary>
    public TransferProviderViewModel SelectedTransferProvider
    {
      get { return m_selectedTransferProvider; }
      set
      {
        TransferProviderViewModel oldValue = m_selectedTransferProvider;

        m_selectedTransferProvider = value;

        OnSelectedTransferProviderChanged(oldValue, value);

        RaisePropertyChanged("SelectedTransferProvider");
      }
    } // SelectedTransferProvider


    /// <summary>
    /// Gets or sets if user wants to anonymize the transfer.
    /// </summary>
    public bool IsAnonymizeChecked
    {
      get { return m_isAnonymizeChecked; }
      set
      {
        m_isAnonymizeChecked = value;

        OnAnonymizeCheckedChanged(m_isAnonymizeChecked);

        RaisePropertyChanged("IsAnonymizeChecked");
      }
    } // IsAnonymizeChecked


    /// <summary>
    /// Gets or sets the list of hosts.
    /// </summary>
    public ObservableCollection<DicomHost> Hosts
    {
      get { return m_hosts; }
      set
      {
        m_hosts = value;
        RaisePropertyChanged("Hosts");
      }
    } // Hosts


    /// <summary>
    /// Gets or sets the list of device view models.
    /// </summary>
    public ObservableCollection<DeviceViewModel> Devices
    {
      get { return m_devices; }
      set
      {
        m_devices = value;
        RaisePropertyChanged("Devices");
      }
    } // Devices


    /// <summary>
    /// Gets or sets the arguments provided to the transfer model.
    /// </summary>
    public object TransferArgs
    {
      get { return m_transferArgs; }
      set
      {
        m_transferArgs = value;
        RaisePropertyChanged("TransferArgs");
      }
    } // TransferArgs


    /// <summary>
    /// Gets or sets the transfer list.
    /// </summary>
    public TransferList TransferList
    {
      get { return m_transferList; }
      set
      {
        m_transferList = value;
        RaisePropertyChanged("TransferList");
      }
    } // TransferList


    /// <summary>
    /// Gets or sets the studies.
    /// </summary>
    public ObservableCollection<StudyViewModel> Studies
    {
      get { return m_studies; }
      set
      {
        m_studies = value;
        RaisePropertyChanged("Studies");
      }
    } // Studies


    /// <summary>
    /// Gets or sets the transfer items that apply to the selected series.
    /// </summary>
    public ObservableCollection<TransferItemViewModel> TransferItems
    {
      get { return m_transferItems; }
      set
      {
        m_transferItems = value;
        RaisePropertyChanged("TransferItems");
      }
    } // TransferItems

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the initialization command.
    /// </summary>
    public RelayCommand InitializeCommand
    {
      get;
      private set;
    } // InitializeCommand


    /// <summary>
    /// Gets or sets the closed command.
    /// </summary>
    public RelayCommand FinalizeCommand
    {
      get;
      private set;
    } // FinalizeCommand


    /// <summary>
    /// Gets or sets a command to handle the window closing.
    /// </summary>
    public RelayCommand<CancelEventArgs> WindowClosingCommand
    {
      get;
      private set;
    } // WindowClosingCommand


    /// <summary>
    /// Gets or sets the command to handle the TreeView's item selection having been changed.
    /// </summary>
    public RelayCommand<RoutedPropertyChangedEventArgs<object>> SelectedItemChangedCommand
    {
      get;
      private set;
    } // SelectedItemChangedCommand


    /// <summary>
    /// Gets or sets the browse command.
    /// </summary>
    public RelayCommand BrowseCommand
    {
      get;
      private set;
    } // BrowseCommand


    /// <summary>
    /// Gets or sets the start transfer command.
    /// </summary>
    public RelayCommand StartTransferCommand
    {
      get;
      private set;
    } // StartTransferCommand

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization of the view model.
    /// </summary>
    private void OnInitialize()
    {
      try
      {
        IsActive = true;
        if(Initializing != null)
        {
          Initializing(this, EventArgs.Empty);
        }

        m_providers = ProviderFactory.Create();

        IProvider provider = null;
        if (m_providers.TryFind(ProviderType.Security, ref provider))
        {
          m_securityProvider = (ISecurityProvider)provider;
          PerformLogin((ISecurityProvider)provider, App.User);
          if (!((ISecurityProvider)provider).IsAuthenticated)
          {
            System.Windows.MessageBox.Show("User elected to cancel Login.  Application will now close.", "Operation Cancelled", MessageBoxButton.OK, MessageBoxImage.Warning);
            throw new ApplicationException("Login failed.");
          }
        }
        else
        {
          throw new ApplicationException("Failed to retrieve Security Provider.");
        }

        provider = null;
        if (m_providers.TryFind(ProviderType.Renewal, ref provider))
        {
          ((IRenewalProvider)provider).RenewalFailed += new RenewalEventHandler(OnRenewContextFailedHandler);
        }

        provider = null;
        if (m_providers.TryFind(ProviderType.Data, ref provider))
        {
          m_dataProvider = (IDataProvider)provider;
        }
        else
        {
          throw new ApplicationException("Failed to retrieve Security Provider.");
        }

        try
        {
          TransferList = m_dataProvider.GetTransferList(App.Uid);
          foreach (Study study in TransferList.Studies)
          {
            Studies.Add(new StudyViewModel(study));
          }
        }
        catch(LockedException)
        {
          Log.Error(string.Format("The provider study/series is locked.  [UID=\"{0}\"]", App.Uid), "TransferManagerViewModel", "OnInitialize");

          System.Windows.MessageBox.Show("The provided study/series is locked.  Please contact Viatronix Customer Support.", "Study/Series Locked", MessageBoxButton.OK, MessageBoxImage.Error);
          throw new ApplicationException("The provided study/series is locked.");
        }
        catch(Viatronix.Enterprise.NotFoundException e)
        {
          Log.Error(string.Format("The provided study/series uid could not be found.  [UID=\"{0}\"]  [ERROR=\"{1}\"]", App.Uid, e.Message), "TransferManagerViewModel", "Initialize");

          System.Windows.MessageBox.Show("The provided study/series uid could not be found.  Please contact Viatronix Customer Support.", "Study/Series Not Found", MessageBoxButton.OK, MessageBoxImage.Error);
          throw new ApplicationException("The provided study/series uid could not be found.");
        }
        catch (Exception e)
        {
          Log.Error(string.Format("An error occurred retrieving the provided study/series.  [UID=\"{0}\"]  [ERROR=\"{1}\"]", App.Uid, e.Message), "TransferManagerViewModel", "Initialize");

          System.Windows.MessageBox.Show("An error occurred retrieving the provided study/series.  Please contact Viatronix Customer Support.", "Could Not Start", MessageBoxButton.OK, MessageBoxImage.Error);
          throw new ApplicationException("An error occurred retrieving the provided study/series.");
        }

        foreach (TransferProviderViewModel transferProvider in (TransferProviderViewModel[])ConfigurationManager.GetSection("transferProviders"))
        {
          if (transferProvider.IsProviderValid(m_transferList))
          {
            TransferProviders.Add(transferProvider);
          }
        }

        List<DicomHost> hosts = m_dataProvider.GetDicomHosts(XmlBuilder.Create("DicomHost", new Dictionary<string,string>() {{ "ServiceType", DicomServiceTypes.Store.ToString() }} ));
        foreach (DicomHost host in hosts)
        {
          Hosts.Add(host);
        }

        using (DiscMaster discMaster = new DiscMaster())
        {
          for (int i = 0; i < discMaster.Count; ++i)
          {
            DeviceViewModel device = new DeviceViewModel();
            device.Initialize(discMaster[i]);
            Devices.Add(device);
          }
        }

        // Sets the selected transfer provider to the first entry.
        SelectedTransferProvider = TransferProviders.Count > 0 ? TransferProviders[0] : null;

        m_devicesRefreshTimer = new System.Threading.Timer(new TimerCallback(OnDeviceRefreshHandler), null, 0, 5000);
      }
      catch(Exception e)
      {
        Log.Error(string.Format("Failed to start application.  [ERROR=\"{0}\"]", e.Message), "TransferManagerViewModel", "OnInitialize");
        if(InitializationFailure != null)
        {
          InitializationFailure(this, EventArgs.Empty);
        }
      }
      finally
      {
        IsActive = false;
        if (Initialized != null)
        {
          Initialized(this, EventArgs.Empty);
        }
      }
    } // OnInitialize()


    /// <summary>
    /// Performs finalization of the view model.
    /// </summary>
    private void OnFinalize()
    {
      if (m_securityProvider.IsAuthenticated)
      {
        if (TransferList != null)
        {
          m_dataProvider.DeleteLocks(TransferList.TransactionId);
        }

        m_securityProvider.Logout();
      }
    } // OnFinalize()


    /// <summary>
    /// Handles the window's closing event.
    /// </summary>
    /// <param name="args">Argument to take into account.</param>
    private void OnWindowClosing(CancelEventArgs args)
    {
      if(IsActive)
      {
        System.Windows.MessageBox.Show("Cannot close window at this time; transfer is currently active.", "Transfer Active", MessageBoxButton.OK, MessageBoxImage.Warning);
        args.Cancel = true;
      }
    } // OnWindowClosing( args )


    /// <summary>
    /// Handles the selected transfer mode being changed.
    /// </summary>
    /// <param name="oldValue"></param>
    /// <param name="newValue"></param>
    private void OnSelectedTransferProviderChanged(TransferProviderViewModel oldValue, TransferProviderViewModel newValue)
    {
      if (oldValue != null)
      {
        oldValue.Started -= new EventHandler(OnTransferStartedHandler);
        oldValue.Completed -= new EventHandler(OnTransferCompletedHandler);
        oldValue.Failed -= new ExceptionEventHandler(OnTransferFailedHandler);
        oldValue.Progress -= new ProgressEventHandler(OnTransferProgressHandler);
      }

      IsAnonymizeChecked = false;

      if(newValue.Name == "PACS")
      {
        if (Hosts.Count > 0)
        {
          TransferArgs = Hosts[0];
        }
      }
      else if (newValue.Name == "Patient Disc" || newValue.Name == "Archival Disc")
      {
        if(Devices.Count > 0)
        {
          TransferArgs = Devices[0];
        }
      }
      else if (newValue.Name == "File System Archival")
      {
        TransferArgs = string.Empty;
      }

      // Retrieve studies/series for transfer.
      Studies = newValue.RetrieveStudiesForTransfer(m_transferList);

      newValue.Started   += new EventHandler(OnTransferStartedHandler);
      newValue.Completed += new EventHandler(OnTransferCompletedHandler);
      newValue.Failed    += new ExceptionEventHandler(OnTransferFailedHandler);
      newValue.Progress  += new ProgressEventHandler(OnTransferProgressHandler);
    } // OnSelectedTransferProviderChanged( oldValue, newValue )


    /// <summary>
    /// Handles the tree view's selected item having been changed.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnSelectedTreeViewItemChanged(RoutedPropertyChangedEventArgs<object> args)
    {
      TransferItems.Clear();

      if(args.NewValue is StudyViewModel)
      {
        // Do nothing...
      }
      else if(args.NewValue is SeriesViewModel)
      {
        SeriesViewModel series = (SeriesViewModel)args.NewValue;

        foreach(TransferItemViewModel item in series.TransferItems)
        {
          TransferItems.Add(item);
        }
      }
    } // OnSelectedTreeViewItemChanged( args )


    /// <summary>
    /// Handles the browse command.
    /// </summary>
    private void OnBrowse()
    {
      System.Windows.Forms.FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();
      if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        TransferArgs = dialog.SelectedPath;
      }
    } // OnBrowse()


    /// <summary>
    /// Handles starting the transfer.
    /// </summary>
    private void OnTransfer()
    {
      SelectedTransferProvider.StartTransfer(Studies.ToArray(), TransferArgs, IsAnonymizeChecked);
    } // OnTransfer()


    /// <summary>
    /// Handles the anonymization flag changing.
    /// </summary>
    /// <param name="checkedState">The new value.</param>
    private void OnAnonymizeCheckedChanged(bool checkedState)
    {
      foreach(StudyViewModel study in Studies)
      {
        foreach(SeriesViewModel series in study.Series)
        {
          foreach(TransferItemViewModel item in series.TransferItems)
          {
            if(item.IsDisabledAnonymization)
            {
              item.IsSelected = !checkedState;
            }
          }
        }
      }
    } // OnAnonymizeCheckedChanged( checkedState )


    /// <summary>
    /// Handles the timer refresh to update the device information.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnDeviceRefreshHandler(object args)
    {
      try
      {
        // Stop the timer...
        m_devicesRefreshTimer.Change(Timeout.Infinite, Timeout.Infinite);

        foreach (DeviceViewModel device in Devices)
        {
          device.Refresh();
        }
      }
      finally
      {
        // Start the timer back up... 
        m_devicesRefreshTimer.Change(5000, 5000);
      }
    } // OnDeviceRefreshHandler( args )


    /// <summary>
    /// Attempts to login using the user argument.
    /// </summary>
    /// <param name="userArgs">The user information.</param>
    /// <returns>The user from the argument.</returns>
    private static User PerformLogin(ISecurityProvider securityProvider, string userArgs)
    {
      string username = string.Empty;
      string password = string.Empty;
      string contextId = string.Empty;

      string[] components = userArgs.Split('|');
      if (components.Length == 1)
      {
        contextId = components[0];
      }
      else if (components.Length == 2)
      {
        username = components[0];
        password = components[1];
      }
      else if (components.Length == 3)
      {
        username = components[0];
        password = components[1];
        contextId = components[2];
      }

      try
      {
        return securityProvider.Login(username, password, contextId);
      }
      catch (Exception)
      {
      }

      if (!securityProvider.IsAuthenticated)
      {
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
              return securityProvider.Login(loginWindow.Username, loginWindow.Password);
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

      return null;
    } // PerformLogin( userArgs )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the renew context failure to close the application.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnRenewContextFailedHandler(ProviderEventArgs args)
    {
      Log.Error("Context could not be renewed.  Application will be shut down...", "TransferManagerViewModel", "OnRenewContextFailedHandler");
      System.Windows.MessageBox.Show("Failed to renew context, application is closing down.", "Failed To Renew Context", MessageBoxButton.OK, MessageBoxImage.Warning);

      App.Current.Shutdown();
    } // OnRenewContextFailedHandler( args )


    /// <summary>
    /// Handles the TransferProviderViewModel's Started event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferStartedHandler(object sender, EventArgs args)
    {
      IsActive = true;
    } // OnTransferStartedHandler( sender, args )


    /// <summary>
    /// Handles the TransferProviderViewModel's Completed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferCompletedHandler(object sender, EventArgs args)
    {
      IsActive = false;
    } // OnTransferCompletedHandler( sender, args )


    /// <summary>
    /// Handles the TransferProviderViewModel's Failed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferFailedHandler(object sender, ExceptionEventArgs args)
    {
      IsActive = false;

      if (args.Exception is LoadAbortException)
      {
        // Do Nothing...
      }
      else if (args.Exception is OperationCanceledException)
      {
        System.Windows.MessageBox.Show("Transfer was cancelled by user.", "Transfer Cancelled", MessageBoxButton.OK, MessageBoxImage.Warning);
      }
      else
      {
        Log.Error(string.Format("An error has occurred while performing the transfer.  [ERROR=\"{0}\"]", args.Exception.Message), "TransferViewModel", "OnTransferFailedHandler");
        System.Windows.MessageBox.Show("An error has occurred while performing the transfer.\n               Please contact Viatronix Support.", "Transfer Failed", MessageBoxButton.OK, MessageBoxImage.Error);
      }
    } // OnTransferFailedHandler( sender, args )


    /// <summary>
    /// Handles the TransferProviderViewModel's Progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferProgressHandler(object sender, ProgressEventArgs args)
    {
    } // OnTransferProgressHandler( sender, args )

    #endregion

  } // class TransferManagerViewModel
} // namespace Viatronix.Utilities.Transfer.ViewModels
