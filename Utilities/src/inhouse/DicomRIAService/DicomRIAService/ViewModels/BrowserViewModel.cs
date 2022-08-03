using System;
using System.Linq;
using System.Windows.Data;
using System.Threading;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using DicomRIAService.Web.Services;
using DicomRIAService.Views;
using DicomRIAService.Web.Models;

namespace DicomRIAService.ViewModels
{
  /// <summary>
  /// This class contains properties that a View can data bind to.
  /// <para>
  /// Use the <strong>mvvminpc</strong> snippet to add bindable properties to this ViewModel.
  /// </para>
  /// <para>
  /// You can also use Blend to data bind with the tool's support.
  /// </para>
  /// <para>
  /// See http://www.galasoft.ch/mvvm/getstarted
  /// </para>
  /// </summary>
  public class BrowserViewModel : ViewModelBase
  {

    #region fields

    private DatasetsDomainContext m_context = new DatasetsDomainContext();

    //private ObservableCollection< DatasetViewModel > m_datasets = new ObservableCollection< DatasetViewModel >();

    //private ObservableCollection< ProgressViewModel > m_progress = new ObservableCollection< ProgressViewModel >();

    private PagedCollectionView m_datasets = null;

    private PagedCollectionView m_progress = null;

    private DatasetViewModel m_selectedDataset = null;

    private int m_attempts = 0;

    private Timer m_timer = null;

    private ProgressViewModel m_sendProgress = new ProgressViewModel( new DicomRIAService.Web.Models.Progress() { aetitle="v3D", server="localhost", port=104 });

    private RelayCommand m_sendCommand = null;

    #endregion

    #region construction

    /// <summary>
    /// Initializes a new instance of the SendDatasetViewModel class.
    /// </summary>
    public BrowserViewModel()
    {
      m_context.Load( m_context.GetDatasetsQuery() ).Completed += ( sender, args ) => 
      {
        this.Datasets = new PagedCollectionView( from dataset in m_context.Datasets select new DatasetViewModel( dataset ));
      };

      m_timer = new Timer( (state) => LoadProgress(), null, 0, Timeout.Infinite );
    }

    public override void Cleanup()
    {
      // Clean own resources if needed
      if( m_timer != null )
      {
        m_timer.Change( Timeout.Infinite, Timeout.Infinite );
        m_timer.Dispose();
      }

      m_timer = null;

      base.Cleanup();
    }

    #endregion

    #region properties

    public PagedCollectionView Datasets
    {
      get { return m_datasets; }
      set
      {
        m_datasets = value;
        RaisePropertyChanged( "Datasets" );
      }
    }


    public PagedCollectionView Progress
    {
      get { return m_progress; }
      set
      {
        m_progress = value;
        RaisePropertyChanged( "Progress" );
      }
    }


    public DatasetViewModel SelectedDataset
    {
      get { return m_selectedDataset; }
      set
      {
        if( m_selectedDataset != value )
        {
          if( m_selectedDataset != null )
            m_selectedDataset.IsSelected = false;

          m_selectedDataset = value;
          if( m_selectedDataset != null )
            m_selectedDataset.IsSelected = true;

          RaisePropertyChanged( "SelectedDataset" );
        }
      }
    }

    #endregion

    #region commands

    public RelayCommand SendCommand
    {
      get
      {
        if( m_sendCommand == null )
          m_sendCommand = new RelayCommand( () => OnSendHandler() );

        return m_sendCommand;
      }
    }

    #endregion

    #region event handlers

    private void OnSendHandler()
    {
      SendViewModel send = new SendViewModel( m_selectedDataset, m_sendProgress );
      SendWindow window = new SendWindow( send );
      window.Closed += ( sender, args ) => OnClosedHandler( ( window.DialogResult.HasValue && window.DialogResult.Value ) ? m_sendProgress : null );
      window.Show(); 
    }

    private void OnClosedHandler( ProgressViewModel viewModel )
    {
      if( viewModel != null )
      {
        Progress progress = new Progress();
        progress.uid = m_selectedDataset.UID;
        progress.aetitle = viewModel.AETitle;
        progress.server =  viewModel.Server;
        progress.port = viewModel.Port;
        progress.message = "1";
        progress.name = "1";
        progress.date = DateTime.Now;
        progress.state = "Sending";

        m_context.Progresses.Add( progress );
        m_context.SubmitChanges();

        StartLoad();
      }
    }

    #endregion

    #region methods

    private void StartLoad()
    {
      m_attempts = 0;
      m_timer.Change( 100, Timeout.Infinite );
    }

    private void LoadProgress()
    {
      m_timer.Change( Timeout.Infinite, Timeout.Infinite );
      m_context.Progresses.Clear();

      bool isSending = false;
      m_context.Load( m_context.GetProgressesQuery() ).Completed += ( sender, args ) => 
      {
        var list = from progress in m_context.Progresses select new ProgressViewModel( progress );
        foreach( ProgressViewModel vm in list )
        {
          if( vm.State == "Sending" )
          {
            isSending = true;
            m_attempts = 31;
            break;
          }
        }

        this.Progress = new PagedCollectionView( list );

        if( isSending || m_attempts++ < 30 )
          m_timer.Change( 100, Timeout.Infinite );
        else
          m_timer.Change( 5000, Timeout.Infinite );
      };
    }

    #endregion

  }
}