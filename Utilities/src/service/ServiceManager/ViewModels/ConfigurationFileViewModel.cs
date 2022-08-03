using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  /// <summary>
  /// The configuration file view model.
  /// </summary>
  public class ConfigurationFileViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The configuration file's name.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// The configuration file's file path.
    /// </summary>
    private string m_filename = string.Empty;

    /// <summary>
    /// The contents.
    /// </summary>
    private string m_contents = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public ConfigurationFileViewModel()
    {
      EditDocumentCommand = new RelayCommand(OnEditDocument);
      InitializeCommand = new RelayCommand(OnInitialize);
      SaveDocumentCommand = new RelayCommand(OnSaveDocument);
      RevertDocumentCommand = new RelayCommand(OnRevertDocument);
      WindowClosingCommand = new RelayCommand<CancelEventArgs>(OnWindowClosing);
    } // ConfigurationFileViewModel()

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
    /// Gets or sets the filename.
    /// </summary>
    public string Filename
    {
      get { return m_filename; }
      set
      {
        m_filename = value;
        RaisePropertyChanged("Filename");
      }
    } // Filename


    /// <summary>
    /// Gets or sets the contents of the file.
    /// </summary>
    public string Contents
    {
      get { return m_contents; }
      set
      {
        m_contents = value;
        RaisePropertyChanged("Contents");
      }
    } // Contents

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the edit document command.
    /// </summary>
    public RelayCommand EditDocumentCommand
    {
      get;
      private set;
    } // EditDocumentCommand


    /// <summary>
    /// Gets or sets the initialize command.
    /// </summary>
    public RelayCommand InitializeCommand
    {
      get;
      private set;
    } // InitializeCommand


    /// <summary>
    /// Gets or sets the save document command.
    /// </summary>
    public RelayCommand SaveDocumentCommand
    {
      get;
      private set;
    } // SaveDocumentCommand


    /// <summary>
    /// Gets or sets the revert document command.
    /// </summary>
    public RelayCommand RevertDocumentCommand
    {
      get;
      private set;
    } // RevertDocumentCommand


    /// <summary>
    /// Gets or sets the window closing event.
    /// </summary>
    public RelayCommand<CancelEventArgs> WindowClosingCommand
    {
      get;
      private set;
    } // WindowClosingCommand

    #endregion

    #region methods

    /// <summary>
    /// Handles editing the document.
    /// </summary>
    private void OnEditDocument()
    {
      ConfigurationFileWindow window = new ConfigurationFileWindow();
      window.DataContext = this;

      window.ShowDialog();
    } // OnEditDocument()


    /// <summary>
    /// Handles initialization.
    /// </summary>
    private void OnInitialize()
    {
      if (File.Exists(Filename))
      {
        using (TextReader reader = new StreamReader(Filename))
        {
          Contents = reader.ReadToEnd();
        }
      }
    } // OnInitialize()


    /// <summary>
    /// Handles saving the document.
    /// </summary>
    private void OnSaveDocument()
    {
      using(TextWriter writer = new StreamWriter(Filename, false))
      {
        writer.Write(m_contents);
      }
    } // OnSaveDocument()


    /// <summary>
    /// Handles reverting the document.
    /// </summary>
    private void OnRevertDocument()
    {
      OnInitialize();
    } // OnRevertDocument()


    /// <summary>
    /// Handles the window closing.
    /// </summary>
    /// <param name="args">Argunments to take into account.</param>
    private void OnWindowClosing(CancelEventArgs args)
    {
      MessageBoxResult msgBoxResult = MessageBox.Show("Would you like to save your changes before closing?", "Save Before Closing?", MessageBoxButton.YesNoCancel, MessageBoxImage.Question);

      if(msgBoxResult == MessageBoxResult.Yes)
      {
        OnSaveDocument();
      }
      else if(msgBoxResult == MessageBoxResult.No)
      {
        // Do nothing.
      }
      else if(msgBoxResult == MessageBoxResult.Cancel)
      {
        args.Cancel = true;
      }
    } // OnWindowClosing( args )

    #endregion

  } // class ConfigurationFileViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
