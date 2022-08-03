// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.IO;
using System.Threading;
using System.Configuration;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Windows.Media;
using System.Windows.Forms;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;

using Viatronix.UI;
using Viatronix.Utilities.Dicom.DicomSend.Models;
using Viatronix.Utilities.Dicom.DicomSend.Enumerations;

namespace Viatronix.Utilities.Dicom.DicomSend
{
  /// <summary>
  /// View model that manages the multitude of send models.
  /// </summary>
  public class SendManagerViewModel : Viatronix.UI.ViewModels.ViewModelBase
  {

    #region events

    /// <summary>
    /// Notifies that none of the directory or all of it's subdirectories have files that fit the filter.
    /// </summary>
    public event EventHandler NoFilesToSend;

    #endregion

    #region fields

    /// <summary>
    /// The arguments that will be used across the threads.
    /// </summary>
    private SendDicomArgs m_parametersModel = new SendDicomArgs();

    /// <summary>
    /// The list of view models that will concurrently send to the 
    /// </summary>
    private ObservableCollection<SendViewModel> m_sendViewModels = new ObservableCollection<SendViewModel>();

    /// <summary>
    /// Make sure we don't handle the same id twice and create more than enough threads.
    /// </summary>
    private List<Guid?> m_handledIds = new List<Guid?>();

    /// <summary>
    /// The directories that need to be sent.
    /// </summary>
    private List<string> m_directories = new List<string>();

    /// <summary>
    /// The empty directories.
    /// </summary>
    private List<string> m_emptyDirectories = new List<string>();

    /// <summary>
    /// Did we abort this cycle?
    /// </summary>
    private bool m_aborted = false;

    /// <summary>
    /// Determines whether we are active.
    /// </summary>
    private bool m_isActive = false;

    /// <summary>
    /// The mutex that will synchronize access across the threads.
    /// </summary>
    private Mutex m_mutex = new Mutex();

    /// <summary>
    /// The number of viable directories at the beginning of the send.
    /// </summary>
    private int m_directoryCount = 0;

    /// <summary>
    /// How many threads have completed over the send.
    /// </summary>
    private int m_completedThreads = 0;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public SendManagerViewModel()
    {
      DispatcherHelper.Initialize();

      DirectorySelectionCommand = new RelayCommand(() => SelectDirectory(), () => IsNotActive);
      StartCommand = new RelayCommand(() => Start(), () => IsNotActive);
      AbortCommand = new RelayCommand(() => Abort(), () => IsActive);
    } // SendManagerViewModel()

    #endregion

    #region properties

    /// <summary>
    /// List of objects that send to the server.
    /// </summary>
    public ObservableCollection<SendViewModel> Senders
    {
      get { return m_sendViewModels; }
      set
      {
        m_sendViewModels = value;
        RaisePropertyChanged("Senders");
      }
    } // Senders


    /// <summary>
    /// Gets or sets the directory that contains the Dicom we shall send.
    /// </summary>
    public string Directory
    {
      get { return m_parametersModel.Directory; }
      set
      {
        m_parametersModel.Directory = value;
        RaisePropertyChanged("Directory");
      }
    } // Directory


    /// <summary>
    /// Gets or sets the Filter that determines those files that should be sent.
    /// </summary>
    public string Filter
    {
      get { return m_parametersModel.Filter; }
      set
      {
        m_parametersModel.Filter = value;
        RaisePropertyChanged("Filter");
      }
    } // Filter


    /// <summary>
    /// Gets or sets the Client's AETitle that should send to the Server.
    /// </summary>
    public string AETitle
    {
      get { return m_parametersModel.AETitle; }
      set
      {
        m_parametersModel.AETitle = value;
        RaisePropertyChanged("AETitle");
      }
    } // AETitle


    /// <summary>
    /// Gets or sets the Server's Dicom Receiver AETitle.
    /// </summary>
    public string CallingAETitle
    {
      get { return m_parametersModel.CallingAETitle; }
      set
      {
        m_parametersModel.CallingAETitle = value;
        RaisePropertyChanged("CallingAETitle");
      }
    } // CallingAETitle


    /// <summary>
    /// Gets or sets the server that we should connect to.
    /// </summary>
    public string Server
    {
      get { return m_parametersModel.Server; }
      set
      {
        m_parametersModel.Server = value;
        RaisePropertyChanged("Server");
      }
    } // Server


    /// <summary>
    /// Gets or sets the port on the server to connect to.
    /// </summary>
    public int Port
    {
      get { return m_parametersModel.Port; }
      set
      {
        m_parametersModel.Port = value;
        RaisePropertyChanged("Port");
      }
    } // Port


    /// <summary>
    /// Gets or sets the transfer syntax.
    /// </summary>
    public TransferSyntaxTypes TransferSyntax
    {
      get { return m_parametersModel.TransferSyntax; }
      set
      {
        m_parametersModel.TransferSyntax = value;
        RaisePropertyChanged("TransferSyntax");
      }
    } // TransferSyntax


    /// <summary>
    /// Gets or sets if the user wishes to store as multiple simultanious threads.
    /// </summary>
    public bool IsStoreAsMultiAssoc
    {
      get { return m_parametersModel.IsStoreAsMultiAssoc; }
      set
      {
        m_parametersModel.IsStoreAsMultiAssoc = value;
        RaisePropertyChanged("IsStoreAsMultiAssoc");
        //RaisePropertyChanged("IsMultiAssocAndNotActive");
      }
    } // IsStoreAsMultiAssoc


    /// <summary>
    /// Gets or sets the thread count.  This determines the number of simultanious connections to the Server's Dicom Receiver.
    /// </summary>
    public int MultiAssocThreadCount
    {
      get { return m_parametersModel.MultiAssocThreads; }
      set
      {
        m_parametersModel.MultiAssocThreads = value;
        RaisePropertyChanged("MultiAssocThreadCount");
      }
    } // MultiAssocThreadCount


    /// <summary>
    /// Is Multi Association Threads text box enabled?
    /// </summary>
    //public bool IsMultiAssocAndNotActive
    //{
    //  get { return IsNotActive && IsStoreAsMultiAssoc; }
    //} // IsMultiAssocAndNotActive


    /// <summary>
    /// Gets or sets the active state.
    /// </summary>
    public bool IsActive
    {
      get { return m_isActive; }
      set
      {
        m_isActive = value;
        RaisePropertyChanged("IsActive");
        RaisePropertyChanged("IsNotActive");
        //RaisePropertyChanged("IsMultiAssocAndNotActive");
      }
    } // IsActive


    /// <summary>
    /// Gets or sets whether or not we are NOT active.
    /// </summary>
    public bool IsNotActive
    {
      get { return !m_isActive; }
    } // IsNotActive

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the Directory Selection command.
    /// </summary>
    public RelayCommand DirectorySelectionCommand
    {
      get;
      set;
    } // DirectorySelectionCommand


    /// <summary>
    /// Gets or sets the Start command.
    /// </summary>
    public RelayCommand StartCommand
    {
      get;
      set;
    } // StartCommand


    /// <summary>
    /// Gets or sets the abort command.
    /// </summary>
    public RelayCommand AbortCommand
    {
      get;
      set;
    } // AbortCommand

    #endregion

    #region methods

    /// <summary>
    /// Presents a directory selection dialog and updates the Directory Text Box.
    /// </summary>
    private void SelectDirectory()
    {
      FolderBrowserDialogEx folderBrowser = new FolderBrowserDialogEx();
      folderBrowser.Description = "DICOM Folder Root";
      folderBrowser.SelectedPath = System.IO.Directory.Exists(Directory) ? Directory : ConfigurationManager.AppSettings["DefaultDirectory"];
      folderBrowser.ShowBothFilesAndFolders = false;
      folderBrowser.ShowEditBox = true;
      folderBrowser.ShowNewFolderButton = false;
      folderBrowser.DontIncludeNetworkFoldersBelowDomainLevel = true;

      if (folderBrowser.ShowDialog() == DialogResult.OK && !string.IsNullOrEmpty(folderBrowser.SelectedPath))
      {
        Directory = folderBrowser.SelectedPath;
      }
    } // SelectDirectory()


    /// <summary>
    /// Starts the send.
    /// </summary>
    public void Start()
    {
      m_handledIds.Clear();
      m_directories.Clear();
      m_sendViewModels.Clear();
      m_emptyDirectories.Clear();

      if (m_parametersModel.IsStoreAsMultiAssoc)
      {
        if (m_parametersModel.Directory != string.Empty && System.IO.Directory.Exists(m_parametersModel.Directory))
        {
          BuildDirectoryList(m_parametersModel.Directory);
        }

        m_completedThreads = 0;
        m_directoryCount = m_directories.Count;

        if (m_directories.Count > 0)
        {
          IsActive = true;
          m_aborted = false;

          for (int i = 0; i < m_emptyDirectories.Count; ++i)
          {
            // Add a shell of a model noting that the directoy is empty.
            SendViewModel viewModel = new SendViewModel();
            viewModel.Directory = m_emptyDirectories[i];
            viewModel.Status = SendStatusTypes.Failed;
            viewModel.Message = "No files match the specified filter.";
            m_sendViewModels.Add(viewModel);
          }

          for (int i = 0; i < MultiAssocThreadCount; ++i)
          {
            if (m_directories.Count > 0)
            {
              SendViewModel viewModel = new SendViewModel();
              viewModel.Finished += new EventHandler(OnSendFinishedHandler);

              viewModel.Arguments = (SendDicomArgs)m_parametersModel.Clone();
              viewModel.Directory = m_directories[0];
              m_directories.RemoveAt(0);

              viewModel.Start();

              m_sendViewModels.Add(viewModel);
            }
          }
        }
        else
        {
          if (NoFilesToSend != null)
          {
            NoFilesToSend(this, EventArgs.Empty);
          }
        }
      }
      else
      {
        string[] files = System.IO.Directory.GetFiles(m_parametersModel.Directory, m_parametersModel.Filter, SearchOption.AllDirectories);
        if (files.Length > 0)
        {
          IsActive = true;
          m_aborted = false;

          SendViewModel viewModel = new SendViewModel();
          viewModel.Finished += new EventHandler(OnSendFinishedHandler);

          viewModel.Arguments = (SendDicomArgs)m_parametersModel.Clone();
          viewModel.Directory = m_parametersModel.Directory;

          viewModel.Start();

          m_sendViewModels.Add(viewModel);
        }
        else
        {
          // Add a shell of a model noting that the directoy is empty.
          SendViewModel viewModel = new SendViewModel();
          viewModel.Directory = m_parametersModel.Directory;
          viewModel.Status = SendStatusTypes.Failed;
          viewModel.Message = "No files match the specified filter.";
          m_sendViewModels.Add(viewModel);
        }
      }
    } // Start()


    /// <summary>
    /// Aborts the send on all senders.
    /// </summary>
    public void Abort()
    {
      m_aborted = true;
      foreach (SendViewModel viewModel in m_sendViewModels)
      {
        viewModel.Abort();
      }
    } // Abort()


    /// <summary>
    /// Builds a directory list of folders that contain one or more file specified in the pattern.  Recursive function.
    /// </summary>
    /// <param name="directory">The directory we should examine.</param>
    private void BuildDirectoryList(string directory)
    {
      if (System.IO.Directory.GetFiles(directory, m_parametersModel.Filter).Length > 0)
      {
        m_directories.Add(directory);
      }
      else
      {
        if (System.IO.Directory.GetDirectories(directory).Length == 0)
        {
          m_emptyDirectories.Add(directory);
        }
      }

      foreach(string childDirectory in System.IO.Directory.GetDirectories(directory))
      {
        BuildDirectoryList(childDirectory);
      }
    } // BuildDirectoryList( directory )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles a SendViewModel's Finished Event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnSendFinishedHandler(object sender, EventArgs args)
    {
      try
      {
        m_mutex.WaitOne();
        ++m_completedThreads;

        if (m_parametersModel.IsStoreAsMultiAssoc)
        {
          Guid? id = null;
          SendViewModel origin = sender as SendViewModel;
          if (origin != null)
          {
            id = origin.ID;
          }

          if (m_directories.Count > 0 && !m_aborted && !m_handledIds.Contains(id))
          {
            SendViewModel viewModel = new SendViewModel();
            DispatcherHelper.CheckBeginInvokeOnUI(() => Senders.Add(viewModel));

            viewModel.Finished += new EventHandler(OnSendFinishedHandler);
            viewModel.Arguments = (SendDicomArgs)m_parametersModel.Clone();
            viewModel.Directory = m_directories[0];
            m_directories.RemoveAt(0);

            viewModel.Start();
          }
          else if (m_aborted || m_completedThreads == m_directoryCount)
          {
            IsActive = false;
          }

          if (id != null)
          {
            m_handledIds.Add(id);
          }
        }
        else
        {
          IsActive = false;
        }
      }
      finally
      {
        m_mutex.ReleaseMutex();
      }
    } // OnCompletedHandler( sender, args )

    #endregion

  } // class SendManagerViewModel
} // namespace Viatronix.Utilities.Dicom.DicomSend
