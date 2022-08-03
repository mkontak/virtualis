using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using Viatronix.Dicom;
using Viatronix.Dicom.Imaging;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Reflection;
using System.Xml;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using System.Windows.Threading;
using GalaSoft.MvvmLight.Threading;
using Viatronix.Dicom.ViewModel;
using Viatronix.Dicom.MVVM.ViewModels;
using Viatronix.UI.ViewModels;
using System.Windows.Data;
using DicomViewer.Windows;

namespace DicomViewer
{

 
  public class MainViewModel : WorkspaceViewModel
  {

    #region


    #endregion

    #region Fields


    /// <summary>
    /// Busy flag
    /// </summary>
    private bool m_busy = false;

    /// <summary>
    /// 
    /// </summary>
    private HeaderWindow m_headerWindow = null;

    /// <summary>
    /// 
    /// </summary>
    private System.Windows.Visibility m_headerVisibility = Visibility.Hidden;

    /// <summary>
    /// Loading thread
    /// </summary>
     private BackgroundWorker m_backgroundWorker = null;

     /// <summary>
     /// Collection of images
     /// </summary>
    private readonly ObservableCollection<SeriesViewModel> m_series = new ObservableCollection<SeriesViewModel>();


    /// <summary>
    /// Current selected image
    /// </summary>
    private SeriesViewModel m_selectedSeries = null;


    /// <summary>
    /// View header command
    /// </summary>
    private RelayCommand m_viewHeaderCommand;

    /// <summary>
    /// Load files command
    /// </summary>
    private RelayCommand m_loadFilesCommand;

    /// <summary>
    /// Load directory command
    /// </summary>
    private RelayCommand m_loadDirectoryCommand;

    /// <summary>
    /// 
    /// </summary>
    private RelayCommand<string[]> m_dropCommand = null;

     /// <summary>
    /// Select series command
    /// </summary>
    private RelayCommand<SeriesViewModel> m_selectSeriesCommand;

    /// <summary>
    /// Status
    /// </summary>
    private string m_status = string.Empty;

    /// <summary>
    /// Lock object
    /// </summary>
    private object m_lock = new object();

    #endregion

  
    #region constructor/destructorw

    /// <summary>
    /// Constructor
    /// </summary>
    public MainViewModel()
    {
      this.RequestClose += new EventHandler(this.OnCloseHandler);
      BindingOperations.EnableCollectionSynchronization(m_series, m_lock);

    } // MainViewModel()



    #endregion 
    
    #region methods

    /// <summary>
    /// Loads file to system
    /// </summary>
    /// <param name="dicomFile">Dicom file to read image from</param>
    private void LoadFile(string dicomFile)
    {

      m_backgroundWorker.ReportProgress(0,string.Format("Loading {0}", dicomFile));


      try
      {
        bool seriesFound = false;

        IImage image = ImageFactory.Create(dicomFile, ParameterType.FILE);

        string seriesUId = image.SeriesInstanceUid;

        SeriesViewModel newSeries = null;

        foreach (SeriesViewModel series in m_series)
        {
          if (series.Uid == seriesUId)
          {
            seriesFound = true;
            newSeries = series;
            break;
          }

        } // END ... For each series

        if (!seriesFound)
        {
          newSeries = SeriesViewModel.Create(image);
          newSeries.ImageLoading += OnImageLoading;
          m_series.Add(newSeries);
        }

        newSeries.Load(image);



 
      }
      catch (Exception ex)
      {
         m_backgroundWorker.ReportProgress(0,  string.Format("Loading of {0} failed : {1}", dicomFile, ex.Message));
      }


    } // LoadFile(string dicomFile)


    private void OnImageLoading(object sender, ImageLoadingEventArgs args )
    {
       m_backgroundWorker.ReportProgress(0, args.Message);

    }

    public bool CanExecute()
    {

      return true;
    }

    /// <summary>
    /// Loads all files in the list
    /// </summary>
    /// <param name="items">Lust of files and/or directories to load</param>
    void Load(string[] items)
    {


      foreach (string item in items)
      {
        if (System.IO.Directory.Exists(item))
        {
          LoadDirectory(item);
        }
        else
        {
          LoadFile(item);
        }
      }

      this.Status = string.Empty;

    } // Load(string[] items)

    /// <summary>
    /// Loads all files in the list
    /// </summary>
    /// <param name="items">Lust of files and/or directories to load</param>
    void LoadFiles(string[] files)
    {
      foreach (string file in files)
      {
        LoadFile(file);
      }

    } // Load(string[] items)



    /// <summary>
    /// Loads the directory and all its sub-directories into the viewer
    /// </summary>
    /// <param name="path">Path to be loaded</param>
    void LoadDirectory(object obj)
    {

      string path = (string)obj;

      if (path.Length > 0)
      {

        this.Status = string.Format("Loading directory {0}", path);

        /// Loads Files
        LoadFiles(System.IO.Directory.GetFiles(path, "*.dcm"));

        string[] dirs = System.IO.Directory.GetDirectories(path);

        foreach (string dir in dirs)
        {
          LoadDirectory(dir);
        }

      }

 
    } // LoadDirectory(string path)

    /// <summary>
    /// Handles the loading of the files and directories by starting a background thread to perform the work.
    /// </summary>
    /// <param name="list">List of files and or directories</param>
    private void LoadHandler(string [] list )
    {

      if (list.Length > 0)
      {
        IsBusy = true;


        if (m_backgroundWorker != null)
          m_backgroundWorker.Dispose();

        m_backgroundWorker = new BackgroundWorker();
        m_backgroundWorker.WorkerReportsProgress = true;

        m_backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(OnWorkerCompleted);
        m_backgroundWorker.ProgressChanged += new ProgressChangedEventHandler(OnWorkerProgressChanged);
        m_backgroundWorker.DoWork += new DoWorkEventHandler(OnDoWork);
        m_backgroundWorker.RunWorkerAsync(list);
      } // END ... If there are entries in the list

    } // LoadHandler(string [] list )

    #endregion

    #region Event Handling Methods

    void HeaderWindowClosed(object sender, EventArgs args)
    {
      m_headerWindow = null;
    }

    /// <summary>
    /// Captures property changed events triggered in the ImageViewModel objects
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnSeriesPropertyChanged(object sender, PropertyChangedEventArgs e)
    {


      if (e.PropertyName == "IsSelected")
      {

        this.SelectedSeries = ((SeriesViewModel)sender);

      }

    } // OnSeriesPropertyChanged(object sender, PropertyChangedEventArgs e)

    /// <summary>
    /// Load files handler
    /// </summary>
    public void OnLoadFilesHandler()
    {

      Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();

      openFileDialog.Title = "Dicom Files";
      openFileDialog.Multiselect = true;
      openFileDialog.Filter = "Dicom Files|*.dcm|All Files|*.*";
      bool? dialogResult = openFileDialog.ShowDialog();

      if ( dialogResult.HasValue && dialogResult.Value )
        LoadHandler(openFileDialog.FileNames);



    } // OnLoadFilesHandler()

    /// <summary>
    /// Close handler
    /// </summary>
    public void OnCloseHandler(object sender, EventArgs args)
    {

      if (m_headerWindow != null)
        m_headerWindow.Close();

    } // OnLoadFilesHandler()


    /// <summary>
    /// Close handler
    /// </summary>
    public void OnCloseHeaderWindows(object sender, EventArgs args)
    {

      m_headerWindow = null;
      

    } // OnLoadFilesHandler()

    /// <summary>
    /// Load directory handler
    /// </summary>
    public void OnLoadDirectoryHandler()
    {

 
      Viatronix.Utilities.UI.FolderBrowserDialogEx folderBrowser = new Viatronix.Utilities.UI.FolderBrowserDialogEx();
      folderBrowser.Description = "Dicom Folder";
      folderBrowser.ShowBothFilesAndFolders = false;
      folderBrowser.ShowEditBox = false;
      folderBrowser.ShowNewFolderButton = false;
      folderBrowser.DontIncludeNetworkFoldersBelowDomainLevel = true;

      folderBrowser.ShowDialog();

      if (folderBrowser.SelectedPath.Length > 0)
      {
 
        string [] files = new string [1];
        files[0] = folderBrowser.SelectedPath;

        LoadHandler(files);

       }


    } //  OnLoadDirectoryHandler()


    /// <summary>
    /// Handles the select series command
    /// </summary>
    /// <param name="series">Newly selected series</param>
    public void OnSelectSeriesHandler(SeriesViewModel series)
    {
      this.SelectedSeries = series;
    } //  OnSelectSeriesHandler(SeriesViewModel series)

    /// <summary>
    /// Worker thread DoWork handler
    /// </summary>
    /// <param name="sender">Sender </param>
    /// <param name="args">Input arguement</param>
    void OnDoWork(object sender, DoWorkEventArgs args)
    {
      Load((string [])args.Argument);
    } // OnDoWork(object sender, DoWorkEventArgs args)

    /// <summary>
    /// Worker threads PorgressChanged event handler
    /// </summary>
    /// <param name="sende">Sender</param>
    /// <param name="args">Progress args</param>
    void OnWorkerProgressChanged(object sende, ProgressChangedEventArgs args)
    {

      if (args.UserState is string)
        this.Status = (string)args.UserState;
      //else if (args.UserState is ProgressInformation)
      //{
      //  ProgressInformation info = (ProgressInformation)args.UserState;

      //  if ( info.AddSeries )
      //    m_series.Add(info.Series);

      //  info.Series.AddImage(info.Image);
      // }
  
    } // OnWorkerProgressChanged(object sende, ProgressChangedEventArgs args)

    /// <summary>
    /// Worker thread completed event handler
    /// </summary>
    /// <param name="sender">Sender</param>
    /// <param name="args">Completed args</param>
    void OnWorkerCompleted(object sender, RunWorkerCompletedEventArgs args)
    {
      this.IsBusy = false;
      this.Status = string.Empty;

    } // OnWorkerCompleted(object sender, RunWorkerCompletedEventArgs args)

    #endregion // Event Handling Methods



    #region properties

 
      /// <summary>
      /// Gets the ViewHeaderCommand
      /// </summary>
      public RelayCommand ViewHeaderCommand
      {
        get
        {
          if (m_viewHeaderCommand == null)
            m_viewHeaderCommand = new RelayCommand(() =>
              {

                if (m_selectedSeries != null)
                {
                  if (m_headerWindow == null && m_selectedSeries != null)
                  {
                    m_headerWindow = new HeaderWindow();
                    m_headerWindow.Closed += new EventHandler(OnCloseHeaderWindows);
                  }

                  m_headerWindow.DataContext = SelectedSeries.SelectedImage.Header;
                  m_headerWindow.Show();
                  
                }

              }
              , () => true);

          return m_viewHeaderCommand;
        }
      } // ViewHeaderCommand


    /// <summary>
    /// Handles the DROP command for a DICOM file to dragged and dropeed on the application
    /// </summary>
    public RelayCommand<string[]> DropCommand
    {
      get
      {
        if (m_dropCommand == null)
          m_dropCommand = new RelayCommand<string[]>(param => LoadHandler(param), param => true);

        return m_dropCommand;

      }
    }  // DropCommand


 
     /// <summary>
    /// Gets the SelectSeriesCommand 
    /// </summary>
    public RelayCommand<SeriesViewModel> SelectSeriesCommand
    {
      get
      {
        if (m_selectSeriesCommand == null)
          m_selectSeriesCommand = new RelayCommand<SeriesViewModel>(param => OnSelectSeriesHandler((SeriesViewModel)param), param => true);

        return m_selectSeriesCommand;
      }
    } // SelectSeriesColmmand

 
    /// <summary>
    /// Gets the LoadFilesCommand
    /// </summary>
    public RelayCommand LoadFilesCommand
    {
      get
      {
        if (m_loadFilesCommand == null)
          m_loadFilesCommand = new RelayCommand(() => OnLoadFilesHandler(), () => true);

        return m_loadFilesCommand;
      }
    } // LoadFilesCommand

    /// <summary>
    /// Gets the LoadDirectoryCommand
    /// </summary>
    public RelayCommand LoadDirectoryCommand
    {
      get
      {
        if (m_loadDirectoryCommand == null)
          m_loadDirectoryCommand = new RelayCommand(() => OnLoadDirectoryHandler(), () => true);

        return m_loadDirectoryCommand;
      }
    } // LoadDirectoryCommand

    /// <summary>
    /// Gets the series collection
    /// </summary>
    public ObservableCollection<SeriesViewModel> Series
    {

      get { return m_series; }


    } // Series

 
    /// <summary>
    /// Gets/Sets the status text
    /// </summary>
    public string Status
    {
      get { return m_status; }
      set
      {  Set<string>("Status", ref m_status, value, true); }

    } // Status



     public System.Windows.Visibility HeaderVisibility
     {
       get { return m_headerVisibility; }
     }

    public SeriesViewModel SelectedSeries
    {
      get { return m_selectedSeries; }
      set
      {
        if ( m_selectedSeries != null )
          m_selectedSeries.IsSelected = false;

        m_selectedSeries = value;
        m_selectedSeries.IsSelected = true;
        if ( m_headerWindow != null )
          m_headerWindow.DataContext = m_selectedSeries.SelectedImage.Header;
        RaisePropertyChanged("SelectedSeries");
        RaisePropertyChanged("IsSeriesSelected");
      }
    }

    public bool IsSeriesSelected
    {
      get { return (m_selectedSeries == null ? false : true); }
    }

     /// <summary>
    /// Sets/Gets the IsBusy flag
    /// </summary>
    public bool IsBusy
    {
      get { return m_busy; }

      set
      {
        m_busy = value;
        RaisePropertyChanged("IsBusy");
      }
    }

    #endregion


  } // class DicomViewModel

}
