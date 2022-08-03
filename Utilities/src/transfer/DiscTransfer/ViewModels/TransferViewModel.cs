using System;
using System.IO;
using System.Windows.Forms;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;

using Viatronix.Logging;
using Viatronix.Utilities.Transfer.DiscTransfer.Models;

namespace Viatronix.Utilities.Transfer.DiscTransfer.ViewModels
{
  /// <summary>
  /// View model to control the main window.
  /// </summary>
  public class TransferViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The model to perform the transfer.
    /// </summary>
    private TransferModel m_transferModel = new TransferModel();

    /// <summary>
    /// Is the transfer currently active?
    /// </summary>
    private bool m_isActive = false;

    /// <summary>
    /// The directory to transfer.
    /// </summary>
    private string m_directory = string.Empty;

    /// <summary>
    /// The progress.
    /// </summary>
    private int m_progress = 0;

    /// <summary>
    /// The maximum progress.
    /// </summary>
    private int m_maxProgress = 100;

    /// <summary>
    /// The text to accompany the progress.
    /// </summary>
    private string m_progressText = string.Empty;

    /// <summary>
    /// The list of available devices.
    /// </summary>
    private ObservableCollection<DeviceItem> m_devices = new ObservableCollection<DeviceItem>();

    /// <summary>
    /// The selected device item.
    /// </summary>
    private DeviceItem m_selectedDevice = null;

    /// <summary>
    /// Should verify after burning the disc?
    /// </summary>
    private bool m_isVerificationEnabled = true;

    /// <summary>
    /// A timer to query for devices.
    /// </summary>
    private System.Threading.Timer m_deviceTimer = null;

    /// <summary>
    /// Auto reset event to synch the background thread with the gui.
    /// </summary>
    private AutoResetEvent m_event = new AutoResetEvent(false);

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public TransferViewModel()
    {
      // Initialize on the main thread.
      DispatcherHelper.Initialize();

      IsActive = false;

      m_transferModel.Progress += new ProgressEventHandler(OnTransferProgressHandler);
      m_transferModel.LoadMedia += new LoadEventHandler(OnTransferLoadMediaHandler);
      m_transferModel.Erasing += new EraseEventHandler(OnTransferErasingHandler);
      m_transferModel.Erased += new EventHandler(OnTransferErasedHandler);

      m_transferModel.TransferStarted += new EventHandler(OnTransferStartedHandler);
      m_transferModel.TransferCompleted += new EventHandler(OnTransferCompletedHandler);
      m_transferModel.TransferFailed += new ExceptionEventHandler(OnTransferFailedHandler);

      BrowseCommand = new RelayCommand(BrowseDirectory);
      TransferCommand = new RelayCommand(Transfer);
      CancelCommand = new RelayCommand(Cancel);

      m_deviceTimer = new System.Threading.Timer(new TimerCallback(OnDeviceTimerCallbackHandler), null, 500, 500);
    } // ManagerViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the directory to transfer.
    /// </summary>
    public string Directory
    {
      get { return m_directory; }
      set
      { 
        m_directory = value;
        RaisePropertyChanged("Directory");
      }
    } // Directory


    /// <summary>
    /// Gets or sets the current progress.
    /// </summary>
    public int Progress
    {
      get { return m_progress; }
      set
      {
        m_progress = value;
        RaisePropertyChanged("Progress");
      }
    } // Progress


    /// <summary>
    /// Gets or sets the max progress.
    /// </summary>
    public int MaxProgress
    {
      get { return m_maxProgress; }
      set
      {
        m_maxProgress = value;
        RaisePropertyChanged("MaxProgress");
      }
    } // MaxProgress


    /// <summary>
    /// Gets or sets the description of what's going on.
    /// </summary>
    public string ProgressText
    {
      get { return m_progressText; }
      set
      {
        m_progressText = value;
        RaisePropertyChanged("ProgressText");
      }
    } // ProgressText


    /// <summary>
    /// Gets or sets the list of devices.
    /// </summary>
    public ObservableCollection<DeviceItem> Devices
    {
      get { return m_devices; }
      set
      {
        m_devices = value;
        RaisePropertyChanged("Devices");
      }
    } // Devices


    /// <summary>
    /// Gets or sets the selected device.
    /// </summary>
    public DeviceItem SelectedDevice
    {
      get { return m_selectedDevice; }
      set
      {
        m_selectedDevice = value;
        RaisePropertyChanged("SelectedDevice");
      }
    } // SelectedDevice


    /// <summary>
    /// Should we verify the burnt data after the write.
    /// </summary>
    public bool IsVerificationEnabled
    {
      get { return m_isVerificationEnabled; }
      set
      {
        m_isVerificationEnabled = value;
        RaisePropertyChanged("IsVerificationEnabled");
      }
    } // IsVerificationEnabled


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
    /// Gets if the transfer is active.
    /// </summary>
    public bool IsNotActive
    {
      get { return !m_isActive; }
    } // InActive

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the browse comamnd.
    /// </summary>
    public RelayCommand BrowseCommand
    {
      get;
      private set;
    } // BrowseCommand


    /// <summary>
    /// Gets or sets the start command.
    /// </summary>
    public RelayCommand TransferCommand
    {
      get;
      private set;
    } // TransferCommand


    /// <summary>
    /// Gets or sets the cancel command.
    /// </summary>
    public RelayCommand CancelCommand
    {
      get;
      private set;
    } // CancelCommand

    #endregion
    
    #region methods

    /// <summary>
    /// Browses for the directory.
    /// </summary>
    private void BrowseDirectory()
    {
      FolderBrowserDialog dialog = new FolderBrowserDialog();
      if (dialog.ShowDialog() == DialogResult.OK)
      {
        Directory = dialog.SelectedPath;
      }
    } // BrowseDirectory()


    /// <summary>
    /// Starts the transfer.
    /// </summary>
    private void Transfer()
    {
      if (!string.IsNullOrEmpty(Directory.Trim()))
      {
        if (System.IO.Directory.Exists(Directory))
        {
          Progress = 0;
          MaxProgress = 100;
          ProgressText = string.Empty;

          m_transferModel.Transfer(SelectedDevice.DeviceId, IsVerificationEnabled, Directory);
        }
        else
        {
          MessageBox.Show("Transfer Directory does not exist, please specify valid directory and try again.", "Directory Not Found", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
      }
      else
      {
        MessageBox.Show("Please specify Transfer Directory and try again.", "Specify Transfer Directory", MessageBoxButtons.OK, MessageBoxIcon.Warning);
      }
    } // Transfer()


    /// <summary>
    /// Cancels the transfer.
    /// </summary>
    private void Cancel()
    {
      if (MessageBox.Show("Cancelling the transfer in progress may damage the disc or device.  Do you wish to continue?", "Cancel Transfer", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.Yes)
      {
        m_transferModel.Cancel();
      }
    } // Cancel()


    /// <summary>
    /// Enables the timer.
    /// </summary>
    private void EnableTimer()
    {
      m_deviceTimer.Change(10000, 10000);
    } // EnableTimer()


    /// <summary>
    /// Disables the timer.
    /// </summary>
    private void DisableTimer()
    {
      m_deviceTimer.Change(Timeout.Infinite, Timeout.Infinite);
    } // DisableTimer()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the transfer Progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferProgressHandler(object sender, ProgressEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        Log.Debug(string.Format("Progress  [MESSAGE=\"{0}\"]  [PROGRESS=\"{1}\"]  [TOTAL=\"{2}\"]", args.Message, args.Progress, args.Total), "TransferViewModel", "OnTransferProgressHandler");

        Progress = args.Progress;
        MaxProgress = args.Total;
        ProgressText = args.Message;
      });
    } // OnTransferProgressHandler( sender, args )


    /// <summary>
    /// Handles the device's request for media to be loaded.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferLoadMediaHandler(object sender, LoadEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        if (MessageBox.Show("Please insert media to begin transfer.", "Inert Media", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning) != DialogResult.OK)
          args.Abort = true;

        m_event.Set();
      });

      m_event.WaitOne();
    } // OnTransferLoadMediaHandler( sender, args )


    /// <summary>
    /// Handles the transfer Erasing event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferErasingHandler(object sender, EraseEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        DialogResult result = MessageBox.Show("Would you like to erase the disc?", "Erase Disc", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
        if (result == DialogResult.Yes)
        {
          args.Erase = true;
        }
        else if (result == DialogResult.No)
        {
          args.Erase = false;
        }
        else if (result == DialogResult.Cancel)
        {
          args.Abort = true;
          args.Erase = false;
        }

        m_event.Set();
      });

      m_event.WaitOne();
    } // OnTransferErasingHandler( sender, args )


    /// <summary>
    /// Handles the transfer Erased event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferErasedHandler(object sender, EventArgs args)
    {
    } // OnTransferErasedHandler( sender, args )


    /// <summary>
    /// Handles the TransferModel's Started event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferStartedHandler(object sender, EventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        IsActive = true;
        DisableTimer();
      });
    } // OnTransferStartedHandler( sender, args )


    /// <summary>
    /// Handles the TransferModel's Completed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferCompletedHandler(object sender, EventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        IsActive = false;
        MessageBox.Show("Transfer was Successful!", "Operation Successful", MessageBoxButtons.OK, MessageBoxIcon.Information);
        EnableTimer();
      });
    } // OnTransferCompletedHandler( sender, args )



    /// <summary>
    /// Handles the TransferModel's Failed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferFailedHandler(object sender, Exception args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        IsActive = false;

        if (args is InsufficientCapacityException)
        {
          MessageBox.Show("Media has insufficient capacity to hold directory contents.  Please insert media with higher capacity.", "Insufficient Capacity", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        else if (args is LoadAbortException)
        {
          // Do Nothing...
        }
        else if (args is OperationCanceledException)
        {
          MessageBox.Show("Transfer was cancelled by user.", "Transfer Cancelled", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
        else
        {
          Log.Error(string.Format("An error has occurred while performing the transfer.  [ERROR=\"{0}\"]", args.Message), "TransferViewModel", "OnTransferFailedHandler");
          MessageBox.Show("An error has occurred while performing the transfer.\n               Please contact Viatronix Support.", "Transfer Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        EnableTimer();
      });
    } // OnTransferFailedHandler( sender, args )


    /// <summary>
    /// Handles the device timer's callback.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnDeviceTimerCallbackHandler(object args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        try
        {
          DisableTimer();

          string deviceId = SelectedDevice != null ? SelectedDevice.DeviceId : string.Empty;

          Devices.Clear();
          foreach (DeviceItem device in m_transferModel.QueryDevices())
          {
            Devices.Add(device);
          }

          if (deviceId == string.Empty)
          {
            SelectedDevice = Devices.Count > 0 ? Devices[0] : null;
          }
          else
          {
            foreach (DeviceItem item in Devices)
            {
              if (deviceId == item.DeviceId)
              {
                SelectedDevice = item;
                break;
              }
            }

            if (SelectedDevice == null)
            {
              SelectedDevice = Devices.Count > 0 ? Devices[0] : null;
            }
          }
        }
        finally
        {
          EnableTimer();
        }
      });
    } // OnDeviceTimerCallbackHandler( args )

    #endregion

  } // class TransferViewModel
} // namespace Viatronix.Utilities.Transfer.DiscTransfer.ViewModels
