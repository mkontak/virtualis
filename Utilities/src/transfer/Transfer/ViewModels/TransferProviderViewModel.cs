using System;
using System.IO;
using System.Xml;
using System.Threading;
using System.Reflection;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Threading;

using Viatronix.Logging;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Registry;
using Viatronix.Utilities.Transfer.Models;

using Viatronix.Anonymization;

namespace Viatronix.Utilities.Transfer.ViewModels
{
  /// <summary>
  /// A transfer provider view model.
  /// </summary>
  public class TransferProviderViewModel : ViewModelBase
  {

    #region events

    /// <summary>
    /// Notifies that the transfer has started.
    /// </summary>
    public event EventHandler Started;

    /// <summary>
    /// Notifies that the transfer has compeleted successfully.
    /// </summary>
    public event EventHandler Completed;

    /// <summary>
    /// Notifies that the transfer has failed and provides the error.
    /// </summary>
    public event ExceptionEventHandler Failed;

    /// <summary>
    /// Notifies the current state of the transfer.
    /// </summary>
    public event ProgressEventHandler Progress;

    #endregion

    #region fields

    /// <summary>
    /// The name of the provider.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// The tooltip to display.
    /// </summary>
    private string m_tooltip = string.Empty;

    /// <summary>
    /// The typename to handle the transfer.
    /// </summary>
    private string m_transferModelTypename = string.Empty;

    /// <summary>
    /// Is anonymize enabled for this provider?
    /// </summary>
    private bool m_isAnonymizeEnabled = false;

    /// <summary>
    /// Can the user pick and choose what will be transferred?
    /// </summary>
    private bool m_canChangeTransferContents = true;

    /// <summary>
    /// The list of transfer items.
    /// </summary>
    private List<TransferItemViewModel> m_transferItems = new List<TransferItemViewModel>();

    /// <summary>
    /// Auto reset event to synch the background thread with the gui.
    /// </summary>
    private AutoResetEvent m_event = new AutoResetEvent(false);

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public TransferProviderViewModel()
    {
    } // TransferProviderViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the name.
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set
      {
        m_name = value;
        RaisePropertyChanged("Name");
      }
    } // Name


    /// <summary>
    /// Gets or sets the tooltip.
    /// </summary>
    public string Tooltip
    {
      get { return m_tooltip; }
      set
      {
        m_tooltip = value;
        RaisePropertyChanged("Tooltip");
      }
    } // Tooltip


    /// <summary>
    /// Gets or sets the transfer model typename.
    /// </summary>
    public string TransferModelTypename
    {
      get { return m_transferModelTypename; }
      set { m_transferModelTypename = value; }
    } // TransferModelTypename


    /// <summary>
    /// Gets or sets if anonymize is enabled for this provider or not.
    /// </summary>
    public bool IsAnonymizeEnabled
    {
      get { return m_isAnonymizeEnabled; }
      set
      {
        m_isAnonymizeEnabled = value;
        RaisePropertyChanged("IsAnonymizeEnabled");
      }
    } // IsAnonymizeEnabled


    /// <summary>
    /// Gets or sets whether the user can pick and choose what to transfer.
    /// </summary>
    public bool CanChangeTransferContents
    {
      get { return m_canChangeTransferContents; }
      set
      {
        m_canChangeTransferContents = value;
        RaisePropertyChanged("CanChangeTransferContents");
      }
    } // CanChangeTransferContents


    /// <summary>
    /// Gets or sets the transfer items.
    /// </summary>
    public List<TransferItemViewModel> TransferItems
    {
      get { return m_transferItems; }
      set { m_transferItems = value; }
    } // TransferItems

    #endregion

    #region methods

    /// <summary>
    /// Creates a transfer provider from the xml.
    /// </summary>
    /// <param name="providerNode">A node that contains the transfer provider.</param>
    /// <returns>The transfer provider.</returns>
    public static TransferProviderViewModel Create(XmlNode providerNode)
    {
      if (providerNode.Name == "transferProvider")
      {
        TransferProviderViewModel viewModel = new TransferProviderViewModel();

        viewModel.Name = providerNode.Attributes["name"].Value;
        viewModel.Tooltip = providerNode.Attributes["tooltip"] != null ? providerNode.Attributes["tooltip"].Value : string.Empty;
        viewModel.TransferModelTypename = providerNode.Attributes["typename"].Value;
        viewModel.IsAnonymizeEnabled = providerNode.Attributes["isAnonymizeEnabled"] != null ? System.Convert.ToBoolean(providerNode.Attributes["isAnonymizeEnabled"].Value) : false;
        viewModel.CanChangeTransferContents = providerNode.Attributes["canChangeTransferContents"] != null ? System.Convert.ToBoolean(providerNode.Attributes["canChangeTransferContents"].Value) : false;

        foreach (XmlNode itemNode in providerNode.SelectNodes("transferItems/item"))
        {
          viewModel.TransferItems.Add(TransferItemViewModel.Create(itemNode));
        }

        return viewModel;
      }
      else
      {
        throw new ArgumentException("Supplied node does not meet expectations.");
      }
    } // Create( node )


    /// <summary>
    /// Starts the transfer.
    /// </summary>
    /// <param name="studies">The studies to be transferred.</param>
    /// <param name="args">Arguments to take into account.</param>
    /// <param name="isAnonymizeChecked"></param>
    public void StartTransfer(StudyViewModel[] studies, object args, bool isAnonymizeChecked)
    {
      TransferModel model = (TransferModel)Activator.CreateInstance(Type.GetType(TransferModelTypename));
      model.Progress += new ProgressEventHandler(OnTransferProgressHandler);

      Thread thread = new Thread(() =>
      {
        string staging = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix", "Transfer");

        try
        {
          OnStarted(EventArgs.Empty);

          if(Directory.Exists(staging))
          {
            try
            {
              Directory.Delete(staging, true);
            }
            catch(Exception e)
            {
              Log.Warning(string.Format("Failed to delete a previous instance of the Trasfer temporary directory.  [DIR=\"{0}\"]", staging), "TransferProviderViewModel", "StartTransfer");
              throw new DeleteDirectoryFailedException("Failed to delete a previous instance of the Trasfer temporary directory.", e);
            }
          }

          if (!Directory.Exists(staging))
          {
            Directory.CreateDirectory(staging);
          }

          model.CreateStagingArea(studies, staging);
          model.BuildFileList(studies, staging);

          if (isAnonymizeChecked)
          {
            AnonymizerWizard wizard = new AnonymizerWizard(DispatcherHelper.UIDispatcher, Path.Combine(SystemSettings.CurrentSystemSettings.ConfigDirectory, "Console", "AnonymizationRules.xml"));
            foreach(StudyViewModel study in studies)
            {
              string studyDirectory = study.StagingUrl;

              List<Series> seriesColl = new List<Series>();
              foreach(SeriesViewModel series in study.Series)
              {
                seriesColl.Add(series.Series);
              }

              wizard.Anonymize(seriesColl, staging);

              if(Directory.Exists(studyDirectory))
              {
                Directory.Delete(studyDirectory, true);
              }

              study.Study.Uid = study.Series[0].Series.StudyUid;
            }
          }

          if (model is DiscTransferModel)
          {
            DiscTransferModel discTransferModel = (DiscTransferModel)model;

            // Hook up to disc events.

            discTransferModel.DeviceEmpty += new DeviceEmptyEventHandler(OnTransferLoadMediaHandler);
            discTransferModel.InsufficientMediaCapacity += new InsufficientMediaCapacityEventHandler(OnTransferInsufficientMediaCapacityHandler);
            discTransferModel.Erasing += new EraseEventHandler(OnTransferEraseMediaHandler);
          }

          model.Transfer(studies, args, staging);

          OnCompleted(EventArgs.Empty);
        }
        catch(Exception e)
        {
          OnFailed(new ExceptionEventArgs(e));
        }
        finally
        {
          try
          {
            Directory.Delete(staging, true);
          }
          catch(Exception)
          {
            // Failed to delete staging area...

            Log.Warning("Failed to delete staging area.", "TransferProviderViewModel", "StartTransfer");
          }

          if (model is DiscTransferModel)
          {
            DiscTransferModel discTransferModel = (DiscTransferModel)model;

            // Hook up to disc events.

            discTransferModel.DeviceEmpty -= new DeviceEmptyEventHandler(OnTransferLoadMediaHandler);
            discTransferModel.InsufficientMediaCapacity -= new InsufficientMediaCapacityEventHandler(OnTransferInsufficientMediaCapacityHandler);
            discTransferModel.Erasing -= new EraseEventHandler(OnTransferEraseMediaHandler);
          }
        }
      });

      thread.Start();
    } // StartTransfer( studies, args )


    /// <summary>
    /// Determines if the provider can handle the provided transfer list.
    /// </summary>
    /// <param name="transferList">The list of resources to be transferred.</param>
    /// <returns>True if this provider is applicible to the contents of the transfer list.</returns>
    public bool IsProviderValid(TransferList transferList)
    {
      foreach(Study study in transferList.Studies)
      {
        foreach(Series series in study.Series)
        {
          foreach(TransferItemViewModel item in TransferItems)
          {
            if (item.IsMatch(series))
            {
              return true;
            }
          }
        }
      }

      return false;
    } // IsProviderValid( transferList )


    /// <summary>
    /// Returns the list of studies (and their series) that apply to this provider.
    /// </summary>
    /// <param name="transferList">The list of resources to be transferred.</param>
    /// <returns>The list of studies (and their series) that apply to this provider.</returns>
    public ObservableCollection<StudyViewModel> RetrieveStudiesForTransfer(TransferList transferList)
    {
      ObservableCollection<StudyViewModel> studies = new ObservableCollection<StudyViewModel>();

      foreach(Study study in transferList.Studies)
      {
        StudyViewModel studyViewModel = new StudyViewModel(study, false);

        foreach(Series series in study.Series)
        {
          SeriesViewModel seriesViewModel = new SeriesViewModel(series);

          // Find the most complete match, type and application.
          foreach(TransferItemViewModel item in m_transferItems)
          {
            if(item.IsCompleteMatch(series))
            {
              seriesViewModel.TransferItems.Add((TransferItemViewModel)item.Clone());
            }
          }

          // Find any matches as long as they aren't in the list already.
          foreach(TransferItemViewModel item in m_transferItems)
          {
            if(item.IsMatch(series) && !seriesViewModel.TransferItems.Contains(item.Name))
            {
              seriesViewModel.TransferItems.Add((TransferItemViewModel)item.Clone());
            }
          }

          if(seriesViewModel.TransferItems.Count > 0)
          {
            studyViewModel.Series.Add(seriesViewModel);
          }
        }

        if(studyViewModel.Series.Count > 0)
        {
          studies.Add(studyViewModel);
        }
      }

      return studies;
    } // RetrieveStudiesForTransfer( transferList )


    /// <summary>
    /// Notifies that the transfer has started.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStarted(EventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        if(Started != null)
        {
          Started(this, args);
        }
      });
    } // OnStarted( args )


    /// <summary>
    /// Notifies that the transfer has completed successfully.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnCompleted(EventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        if (Completed != null)
        {
          Completed(this, args);
        }
      });
    } // OnCompleted( args )


    /// <summary>
    /// Notifies that the transfer has failed.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFailed(ExceptionEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        if (Failed != null)
        {
          Failed(this, args);
        }
      });
    } // OnFailed( args )


    /// <summary>
    /// Notifies the current state of the transfer.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    private void OnProgress(ProgressEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        if (Progress != null)
        {
          Progress(this, args);
        }
      });
    } // OnProgress( args )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the model's LoadMedia event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferLoadMediaHandler(object sender, DeviceEmptyEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        if (MessageBox.Show("Please insert media to begin transfer.", "Inert Media", MessageBoxButton.OKCancel, MessageBoxImage.Warning) != MessageBoxResult.OK)
          args.Abort = true;

        m_event.Set();
      });

      m_event.WaitOne();
    } // OnTransferLoadMediaHandler( sender, args )


    /// <summary>
    /// Handles the model's LoadMedia event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferInsufficientMediaCapacityHandler(object sender, InsufficientMediaCapacityEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        string requiredCapacity = args.MinimumCapacity > 1000000000 ? Math.Round((double)(args.MinimumCapacity) / 1000000000.0, 2) + " GB" : (int)(((double)(args.MinimumCapacity) / 1000000.0) + 0.5) + " MB";
        if (MessageBox.Show(string.Format("Media capacity is insufficient to hold selected transfer contents.\n       Please insert media with at least {0}.", requiredCapacity), "Inert Media", MessageBoxButton.OKCancel, MessageBoxImage.Warning) != MessageBoxResult.OK)
        {
          args.Abort = true;
        }

        m_event.Set();
      });

      m_event.WaitOne();
    } // OnTransferInsufficientMediaCapacityHandler( sender, args )


    /// <summary>
    /// Handles the model's EraseMedia event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferEraseMediaHandler(object sender, EraseEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() =>
      {
        MessageBoxResult result = MessageBox.Show("Would you like to erase the disc?", "Erase Disc", MessageBoxButton.YesNoCancel, MessageBoxImage.Question);
        if (result == MessageBoxResult.Yes)
        {
          args.Erase = true;
        }
        else if (result == MessageBoxResult.No)
        {
          args.Erase = false;
        }
        else if (result == MessageBoxResult.Cancel)
        {
          args.Abort = true;
          args.Erase = false;
        }

        m_event.Set();
      });

      m_event.WaitOne();
    } // OnTransferEraseMediaHandler( sender, args )


    /// <summary>
    /// Handles the model's Progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnTransferProgressHandler(object sender, ProgressEventArgs args)
    {
      OnProgress(args);
    } // OnTransferProgressHandler( sender, args )

    #endregion

  } // class TransferProviderViewModel
} // namespace Viatronix.Utilities.Transfer.ViewModels
