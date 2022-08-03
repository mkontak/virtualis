using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Viatronix.Utilities.General.AdministerHostsTable.ViewModels;
using Viatronix.Utilities.General.AdministerHostsTable.ValidationRules;

namespace Viatronix.Utilities.General.AdministerHostsTable
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    
    #region fields

    /// <summary>
    /// The manager.
    /// </summary>
    private MainWindowViewModel m_manager = new MainWindowViewModel();

    /// <summary>
    /// Keeps track if the name input validation failed.
    /// </summary>
    private bool m_nameError = false;

    /// <summary>
    /// Keeps track if the server input validation failed.
    /// </summary>
    private bool m_serverError = false;

    /// <summary>
    /// Keeps track if the aetitle input validation failed.
    /// </summary>
    private bool m_aeTitleError = false;

    /// <summary>
    /// Keeps track if the port input validation failed.
    /// </summary>
    private bool m_portError = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();

      m_manager.EditingStateChanged += new StateChanged<bool>(OnEditingStateChangedHandler);
      DataContext = m_manager;
    } // MainWindow()

    #endregion

    #region methods

    /// <summary>
    /// Is the input valid.
    /// </summary>
    /// <returns>True if input is valid, false otherwise.</returns>
    private bool IsValidationSuccessful()
    {
      return !m_nameError && !m_serverError && !m_portError && !m_aeTitleError;
    } // IsValidationSuccessful()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the window's loaded event.
    /// </summary>
    /// <param name="source"></param>
    /// <param name="args"></param>
    private void OnWindowLoadedHandler(object source, EventArgs args)
    {
      m_manager.Initialize();
    } // OnWindowLoadedHandler( source, args )


    /// <summary>
    /// Handles the window's closing event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnWindowClosingHandler(object sender, CancelEventArgs args)
    {
      if (m_manager.Modified)
      {
        MessageBoxResult result = MessageBox.Show(Messages.GetMessage("VX029"), Messages.GetMessage("VX030"), MessageBoxButton.YesNoCancel, MessageBoxImage.Question);

        if (result == MessageBoxResult.Yes)
        {
          m_manager.SaveCommand.Execute(null);
        }

        args.Cancel = (result == MessageBoxResult.Cancel);
      }
    } // OnWindowClosingHandler( sender, args )


    /// <summary>
    /// Handles the editing state being changed.  We do this because the Window.Style data trigger won't work.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="oldValue">The original value.</param>
    /// <param name="newValue">The new value.</param>
    private void OnEditingStateChangedHandler(object sender, bool oldValue, bool newValue)
    {
      // We are editing.
      if (newValue)
      {
        this.Width = 525;
        this.Height = 520;
      }
      else
      {
        this.Width = 525;
        this.Height = 242;
      }
    } // OnEditingStateChangedHandler( sender, oldValue, newValue )

    #endregion

  } // class MainWindow
} // namespace Viatronix.Utilities.General.AdministerHostsTable
