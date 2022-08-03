using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using Viatronix.Dicom;
using System.Drawing;
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
using Viatronix.Dicom.MVVM.ViewModels;

namespace DicomHeaderViewer
{

 
  public class MainViewModel : ViewModelBase
  {

 
    #region Fields

    /// <summary>
    /// 
    /// </summary>
    private bool m_showHeader = false;

    private bool m_busy = false;

    /// <summary>
    /// Header View
    /// </summary>
    private HeaderViewModel m_headerVieModel = null;
 
    private System.Windows.Visibility m_headerVisibility = Visibility.Hidden;



     /// <summary>
    /// Close command
    /// </summary>
    private RelayCommand m_closeCommand;


    /// <summary>
    /// Load files command
    /// </summary>
    private RelayCommand m_openFileCommand;

    /// <summary>
    /// 
    /// </summary>
    private RelayCommand<string> m_dropCommand = null;



    #endregion

  
    #region constructor/destructorw

    /// <summary>
    /// Constructor
    /// </summary>
    public MainViewModel()
    {

    } // MainViewModel()



    #endregion 
    
    #region methods

    /// <summary>
    /// Loads file to system
    /// </summary>
    /// <param name="dicomFile">Dicom file to read image from</param>
    private void OpenFileHandler()
    {

      Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();

      openFileDialog.Title = "Dicom File";
      openFileDialog.Multiselect = false;
      openFileDialog.Filter = "Dicom Files|*.dcm|All Files|*.*";
      openFileDialog.ShowDialog();

      OpenFile(openFileDialog.FileName);




    } // LoadFile(string dicomFile)

    private void OpenFile(string file)
    {
      if (file.Length == 0)
        return;

      try
      {
        Header =  HeaderViewModel.Create(file);
      }
      catch (Exception ex)
      {
        MessageBox.Show(string.Format("Loading of {0} failed : {1}", file, ex.Message), "File Error", MessageBoxButton.OK, MessageBoxImage.Error);
      }

    }

    public bool CanExecute()
    {

      return true;
    }



 
    #endregion

    #region Event Handling Methods

 
     /// <summary>
     /// <summary>
    /// Close handler
    /// </summary>
    public void OnCloseHandler()
    {

    } // OnLoadFilesHandler()
    

    #endregion // Event Handling Methods



    #region properties


  
    /// <summary>
    /// Sets/Gets the header
    /// </summary>
    public HeaderViewModel Header
    {
      get { return m_headerVieModel; }
      set
      {
        Set<HeaderViewModel>("Header", ref m_headerVieModel, value);
      }
    }

    public RelayCommand<string> DropCommand
    {
      get
      {
        if (m_dropCommand == null)
          m_dropCommand = new RelayCommand<string>(param => OpenFile(param), param => true);

        return m_dropCommand;

      }
    }


 
    /// <summary>
    /// Gets the LoadFilesCommand
    /// </summary>
    public RelayCommand CloseCommand
    {
      get
      {
        if (m_closeCommand == null)
          m_closeCommand = new RelayCommand(() => OnCloseHandler(), () => true);

        return m_closeCommand;
      }
    } // CloseCommand

    /// <summary>
    /// Gets the LoadFilesCommand
    /// </summary>
    public RelayCommand OpenFileCommand
    {
      get
      {
        if (m_openFileCommand == null)
          m_openFileCommand = new RelayCommand(() => OpenFileHandler(), () => true);

        return m_openFileCommand;
      }
    } // OpenFileCommand


    public bool IsBusy
    {
      get { return m_busy; }
      set { Set<bool>("IsBusy", ref m_busy, value); }
    }
 
     #endregion


  } // class DicomViewModel

}
