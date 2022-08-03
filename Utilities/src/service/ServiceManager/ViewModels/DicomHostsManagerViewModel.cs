using System;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Threading;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;

using Viatronix;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Utilities.Service.ServiceManager;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  /// <summary>
  /// A view model for the dicom hosts tab.
  /// </summary>
  public class DicomHostsManagerViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The list of hosts.
    /// </summary>
    private ObservableCollection<DicomHostViewModel> m_dicomHosts = new ObservableCollection<DicomHostViewModel>();

    /// <summary>
    /// The selected host.
    /// </summary>
    private DicomHostViewModel m_selectedDicomHost = null;

    /// <summary>
    /// A timer to refresh the host list.
    /// </summary>
    private DispatcherTimer m_refreshTimer = null;

    /// <summary>
    /// The timer reset event.
    /// </summary>
    private AutoResetEvent m_timerResetEvent = new AutoResetEvent(true);

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DicomHostsManagerViewModel()
    {
      AddDicomHostCommand = new RelayCommand(OnAddDicomHost);
      ModifyDicomHostCommand = new RelayCommand(OnModifyDicomHost);
      DeleteDicomHostCommand = new RelayCommand(OnDeleteDicomHost);

      OnRefresh();
      m_refreshTimer = new DispatcherTimer(new TimeSpan(0, 0, 10), DispatcherPriority.ApplicationIdle, new EventHandler(OnRefreshTimerHandler), DispatcherHelper.UIDispatcher);
      m_refreshTimer.Start();
    } // DicomHostsViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the hosts.
    /// </summary>
    public ObservableCollection<DicomHostViewModel> DicomHosts
    {
      get { return m_dicomHosts; }
      set
      {
        m_dicomHosts = value;
        RaisePropertyChanged("DicomHosts");
      }
    } // DicomHosts


    /// <summary>
    /// Gets or sets the selected host.
    /// </summary>
    public DicomHostViewModel SelectedDicomHost
    {
      get { return m_selectedDicomHost; }
      set
      {
        m_selectedDicomHost = value;
        RaisePropertyChanged("SelectedDicomHost");
        RaisePropertyChanged("IsDicomHostSelected");
      }
    } // SelectedDicomHost


    /// <summary>
    /// Gets if a host is selected.
    /// </summary>
    public bool IsDicomHostSelected
    {
      get { return SelectedDicomHost != null; }
    } // IsDicomHostSelected

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the add host command.
    /// </summary>
    public RelayCommand AddDicomHostCommand
    {
      get;
      private set;
    } // AddDicomHostCommand


    /// <summary>
    /// Gets or sets the modify host command.
    /// </summary>
    public RelayCommand ModifyDicomHostCommand
    {
      get;
      private set;
    } // ModifyDicomHostCommand
    

    /// <summary>
    /// Gets or sets the delete host command.
    /// </summary>
    public RelayCommand DeleteDicomHostCommand
    {
      get;
      private set;
    } // DeleteDicomHostCommand

    #endregion

    #region methods

    /// <summary>
    /// Occurs to refresh.
    /// </summary>
    private void OnRefresh()
    {
      DicomHosts.Clear();
      foreach (DicomHost host in Global.GetDicomHosts())
      {
        DicomHosts.Add(new DicomHostViewModel(host));
      }
    } // OnRefresh()


    /// <summary>
    /// Adds the dicom host.
    /// </summary>
    private void OnAddDicomHost()
    {
      try
      {
        m_refreshTimer.Stop();

        DicomHostWindow window = new DicomHostWindow();
        DicomHostViewModel host = new DicomHostViewModel();

        window.DicomHost = host;
        window.EditingMode = EditingModes.Insert;

        window.ShowDialog();
      }
      finally
      {
        OnRefresh();
        m_refreshTimer.Start();
      }
    } // OnAddDicomHost()


    /// <summary>
    /// Modifies the dicom host.
    /// </summary>
    private void OnModifyDicomHost()
    {
      try
      {
        m_refreshTimer.Stop();

        if (SelectedDicomHost != null)
        {
          DicomHostWindow window = new DicomHostWindow();

          window.DicomHost = SelectedDicomHost;
          window.EditingMode = EditingModes.Update;

          window.ShowDialog();
        }
      }
      finally
      {
        OnRefresh();
        m_refreshTimer.Start();
      }
    } // OnModifyDicomHost()


    /// <summary>
    /// Deletes the dicom host.
    /// </summary>
    private void OnDeleteDicomHost()
    {
      try
      {
        m_refreshTimer.Stop();

        if (SelectedDicomHost != null)
        {
          if (MessageBox.Show("Are you sure you want to delete the selected Host?", "Delete Host", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
          {
            Global.DeleteDicomHost(SelectedDicomHost.DicomHost);
          }
        }

        SelectedDicomHost = null;
        
      }
      finally
      {
        OnRefresh();
        m_refreshTimer.Start();
      }
    } // OnDeleteDicomHost()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the refresh timer's event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnRefreshTimerHandler(object sender, EventArgs args)
    {
      try
      {
        m_refreshTimer.Stop();
        m_timerResetEvent.WaitOne();

        OnRefresh();
      }
      finally
      {
        m_timerResetEvent.Set();
        m_refreshTimer.Start();
      }
    } // OnRefreshTimerHandler( sender, args )

    #endregion

  } // class DicomHostsViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
