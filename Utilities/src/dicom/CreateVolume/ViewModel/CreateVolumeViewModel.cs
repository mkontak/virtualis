using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using System.Windows;
using Viatronix.UI;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using Viatronix.Conversion;
using System.Windows.Media.Imaging;
using System.Drawing;
using Viatronix.Enterprise.Processing;

namespace Viatronix.Utilities.Dicom.CreateVolume.ViewModel
{

  /// <summary>
  /// View Model for the creation of a volume
  /// </summary>
  class CreateVolumeViewModel : Viatronix.UI.ViewModels.ViewModelBase
  {

    #region delegates

    /// <summary>
    ///  Delegae used to show a message box
    /// </summary>
    /// <param name="type"></param>
    /// <param name="message"></param>
    /// <param name="header"></param>
    public delegate void ShowMessageBoxDelegate(string type, string message, string header);

    #endregion

    #region fields

    /// <summary>
    /// Icon
    /// </summary>
    private Bitmap m_icon;//= new System.Drawing.Bitmap(Application.GetResourceStream(new Uri("pack://application:,,,/Resources/app.bmp")).Stream);


    /// <summary>
    /// Conversion model
    /// </summary>
    private ConversionModel m_conversionModel = new ConversionModel();

    /// <summary>
    /// Delegate used to show a message
    /// </summary>
    private ShowMessageBoxDelegate m_showMessageDelegate;

    /// <summary>
    /// Create Volume command
    /// </summary>
    private RelayCommand m_createCommand;

    /// <summary>
    /// Cancel command
    /// </summary>
    private RelayCommand m_cancelCommand;

    /// <summary>
    /// Folder Browser
    /// </summary>
    private RelayCommand<string> m_folderBrowserCommand;

    /// <summary>
    /// Minimum
    /// </summary>
    private int m_minimum = 0;

    /// <summary>
    /// Maximum 
    /// </summary>
    private int m_maximum = 100;

    /// <summary>
    /// Progress
    /// </summary>
    private int m_progress = 0;

    /// <summary>
    /// Volume File
    /// </summary>
    private string m_volumeFile = string.Empty;

    /// <summary>
    /// Path
    /// </summary>
    private string m_path = string.Empty;

    /// <summary>
    /// Pattern
    /// </summary>
    private string m_pattern = string.Empty;

    /// <summary>
    /// Status
    /// </summary>
    private string m_status = string.Empty;

    /// <summary>
    /// Create enabled
    /// </summary>
    private bool m_createEnabled = true;

    /// <summary>
    /// Cancel enabled
    /// </summary>
    private bool m_cancelEnabled = true;

    #endregion


    #region constructors/destructors

    /// <summary>
    /// Constructor
    /// </summary>
    public CreateVolumeViewModel()
    {

      m_conversionModel.Started += new PipelineEventHandler(OnConversionStarted);
      m_conversionModel.Updated += new PipelineEventHandler(OnConversionUpdated);
      m_conversionModel.Completed += new PipelineEventHandler(OnConversionCompleted);
      m_conversionModel.Failed += new PipelineEventHandler(OnConversionFailed);
 
      m_showMessageDelegate = new ShowMessageBoxDelegate(ShowMessageBox);
 
      GalaSoft.MvvmLight.Threading.DispatcherHelper.Initialize();

      ServiceInjector.InjectServices();


      CancelEnabled = true;
      CreateEnabled = true;

      m_volumeFile = m_conversionModel.VolumeFile;
      m_path = m_conversionModel.Path;
      m_pattern = m_conversionModel.Pattern;

      Viatronix.Logging.Log.Initialize();



    } // CreateVolumeViewModel()

  

    #endregion

    #region event handlers

 
  
    /// <summary>
    /// Handles the conversion completed event
    /// </summary>
    /// <param name="args">Event args</param>
    void OnConversionCompleted(PipelineEventArgs args)
    {
      Maximum = 100;
      Progress = 0;
      Status = "";
      
      GalaSoft.MvvmLight.Threading.DispatcherHelper.UIDispatcher.BeginInvoke(m_showMessageDelegate, new string[] { "information", "Conversion Completed : " + args.Message, "Success" });
       this.CreateEnabled = true;
      this.CancelEnabled = false;

    } // OnConversionCompleted(ConversionEventArgs args)

    /// <summary>
    /// Handles the convrsion failed event
    /// </summary>
    /// <param name="args">Event args</param>
    void OnConversionFailed(PipelineEventArgs args)
    {
      Maximum = 100;
      Progress = 0;
      Status = "";
      
      GalaSoft.MvvmLight.Threading.DispatcherHelper.UIDispatcher.BeginInvoke(m_showMessageDelegate, new string[] { "error", "Conversion Failed" + Environment.NewLine + Environment.NewLine + args.Message, "Failure" });
      this.CreateEnabled = true;
      this.CancelEnabled = false;

    } // OnConversionFailed(ConversionEventArgs args)

    /// <summary>
    /// Handles a process start event by setting the mew MAX for the progress bar
    /// </summary>
    /// <param name="args"></param>
    void OnConversionStarted(PipelineEventArgs args)
    {
      Maximum  = args.Remaining;
      Progress = args.Completed;
      Status = "Conversion started";
      this.CreateEnabled = false;
      this.CancelEnabled = true;

    } // OnProcessStarted(ConversionEventArgs args)

    /// <summary>
    /// Handles the process updated event by updateing the progess bar value and status
    /// </summary>
    /// <param name="args"></param>
    void OnConversionUpdated(PipelineEventArgs args)
    {
      Maximum = args.Remaining + args.Completed;
      Progress = args.Completed;
      Status = string.Format("{0}:  {1}", args.Stage, args.Message);
    } // OnProcessUpdated(ConversionEventArgs args)

  

    #endregion

    #region methods

    /// <summary>
    /// Shows the message box specified
    /// </summary>
    /// <param name="type">Type of messsage box</param>
    /// <param name="message">Message</param>
    /// <param name="header">Header</param>
    private void ShowMessageBox(string type, string message, string header)
    {
      if (type == "error")
        GetService<IMessageBoxService>().ShowError(message, header);
      else
        GetService<IMessageBoxService>().Show(message);

    } // OnShowMessage(string type, string message, string header)


    #endregion

    #region command handlers

    private void OnBrowseFolders(string type)
    {
      FolderBrowserDialogEx folderBrowser = new FolderBrowserDialogEx();
      folderBrowser.Description = type +  " Folder";

      if ( string.Compare(type,"volume",true ) == 0 )
        folderBrowser.SelectedPath = System.IO.Path.GetDirectoryName(this.VolumeFile);
      else if ( string.Compare(type,"dicom",true) == 0 )
        folderBrowser.SelectedPath = this.Path;

      folderBrowser.ShowBothFilesAndFolders = false;
      folderBrowser.ShowEditBox = true;
      folderBrowser.ShowNewFolderButton = true;
      folderBrowser.DontIncludeNetworkFoldersBelowDomainLevel = true;

      folderBrowser.ShowDialog();

      if (string.Compare(type, "volume", true) == 0)
        this.VolumeFile = folderBrowser.SelectedPath;
      else if (string.Compare(type, "dicom", true) == 0)
        this.Path = folderBrowser.SelectedPath;

    } 

    /// <summary>
    /// Handles the create volume event
    /// </summary>
    private void OnCreateCommand()
    {

      // ===================================================
      // Make sure the conversion is not already processing
      // ===================================================
      if (m_conversionModel.Processing)
      {
        ShowMessageBox("error", "Volume creation already started", "Create Volume Started");
        return;
      }

      // ===========================
      // Make sure the path is set
      // ===========================
      if (m_conversionModel.Path.Length == 0)
      {
        ShowMessageBox("error", "No dicom file path specified", "Invalid path");
        return;
      }

      // ============================
      // Make sure the path exists
      // ============================
      if (!System.IO.Directory.Exists(m_conversionModel.Path))
      {
        ShowMessageBox("error", "Dicom path does not exist", "Invalid path");
        return;
      }

      this.CreateEnabled = false;
      this.CancelEnabled = true;

      /// Attempt to create
      m_conversionModel.CreateVolume();

 
    } // OnCreateCommand()


    /// <summary>
    /// Handles the cancel event
    /// </summary>
    void OnCancelCommand()
    {

      if ( m_conversionModel.Processing )
      {
        this.CreateEnabled = true;
        this.CancelEnabled = false;

        m_conversionModel.Abort();
      }
      else
        System.Windows.Application.Current.Shutdown();

    } // OnCacnelcCOmmand()


    #endregion


    #region properties

    /// <summary>
    /// Gets the folder browsere command
    /// </summary>
    public RelayCommand<string> FolderBrowserCommand
    {
      get
      {
        if (m_folderBrowserCommand == null)
          m_folderBrowserCommand = new RelayCommand<string>(param => OnBrowseFolders(param), param => true);

        return m_folderBrowserCommand;
      }
    } // CreateCommand


    /// <summary>
    /// Gets the create colume command
    /// </summary>
    public RelayCommand CreateCommand
    {
      get
      {
        if (m_createCommand == null)
          m_createCommand = new RelayCommand(() => OnCreateCommand(), () => true);

        return m_createCommand;
      }
    } // CreateCommand


    /// <summary>
    /// Gets the cancecl command
    /// </summary>
    public RelayCommand CancelCommand
    {
      get
      {
        if (m_cancelCommand == null)
          m_cancelCommand = new RelayCommand(() => OnCancelCommand(), () => true);

        return m_cancelCommand;
      }
    } // CancelCOmmand



    /// <summary>
    /// Gets/Sets the pattern
    /// </summary>
    public string Pattern 
    {
      get { return m_conversionModel.Pattern; }
      set
      {

        m_conversionModel.Pattern = value;
        Set<string>("Pattern",  ref m_pattern, value);
       }
    } // Pattern

    /// <summary>
    /// Sets/Getse the Create Enabled property
    /// </summary>
    public bool CreateEnabled
    {
      get { return m_createEnabled; }
      set
      {
        
        Set<bool>("CreateEnabled", ref m_createEnabled, value);
 
      }
    } // CreateEnabled

    /// <summary>
    /// Sets/Getse the Create Enabled property
    /// </summary>
    public bool CancelEnabled
    {
      get { return m_cancelEnabled; }
      set
      {
        
        Set<bool>("CancelEnabled", ref m_cancelEnabled, value);
        
      }
    } // CancelEnabled

    /// <summary>
    /// Gets/Sets the Path
    /// </summary>
    public string Path
    {
      get { return m_conversionModel.Path; }
      set
      {
        m_conversionModel.Path = value;
        Set<string>("Path", ref m_path, value);
  
          
        VolumeFile = m_conversionModel.VolumeFile;

        if (string.IsNullOrEmpty(m_conversionModel.Path))
          this.CreateCommand.CanExecute(false);
        else
          this.CreateCommand.CanExecute(true);

    }
    } //Path


    /// <summary>
    /// Gets/Sets the volume file
    /// </summary>
    public string VolumeFile
    {
      get { return m_conversionModel.VolumeFile; }
      set
      {
        m_conversionModel.VolumeFile = value;
        Set<string>("VolumeFile", ref m_volumeFile, value);
      }
    } // VolumeFile

    /// <summary>
    /// Gets/Set the minimum value
    /// </summary>
    public int Minimum
    {
      get { return m_minimum; }
      set
      {
        m_minimum = value;
        RaisePropertyChanged("Minimum");
      }
    } // Minimum

    /// <summary>
    /// Gets/Set the maximum value
    /// </summary>
    public int Maximum
    {
      get { return m_maximum; }
      set
      {
        m_maximum = value;
        RaisePropertyChanged("Maximum");
      }
    } // Maximum

    /// <summary>
    /// Gets/Set the Icon
    /// </summary>
    public Bitmap Icon
    {
      get { return m_icon; }
      set
      {
        m_icon = value;
        RaisePropertyChanged("Icon");
      }
    } // m_icon

    /// <summary>
    /// Gets/Set the progress value
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
    /// Gets/Sets the status
    /// </summary>
    public string Status
    {
      get { return m_status; }
      set
      {
        m_status = value;
        RaisePropertyChanged("Status");
      }
    } // Status

    #endregion



  }
}
