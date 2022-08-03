using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Viatronix.Utilities.Service.ServiceManager.ViewModels;

namespace Viatronix.Utilities.Service.ServiceManager
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

    #region fields

    private SeviceManagerViewModel m_model;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();

      m_model = new SeviceManagerViewModel();
      this.DataContext = m_model;

      m_model.Initializing += new EventHandler(OnViewModelInitializingHandler);
      m_model.Initialized += new EventHandler(OnViewModelInitializedHandler);
      m_model.InitializationFailure += new EventHandler(OnViewModelInitializationFailureHandler);
    } // MainWindow()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the view model's Initializing event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnViewModelInitializingHandler(object sender, EventArgs args)
    {
      WindowState = WindowState.Minimized;
    } // OnViewModelInitializingHandler( sender, args )


    /// <summary>
    /// Handles the view model's Initialized event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnViewModelInitializedHandler(object sender, EventArgs args)
    {
      WindowState = WindowState.Normal;
    } // OnViewModelInitializedHandler( sender, args )


    /// <summary>
    /// Handles the viewmodel's InitializationFailure event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnViewModelInitializationFailureHandler(object sender, EventArgs args)
    {
      Close();
    } // OnViewModelInitializationFailureHandler( sender, args )

    #endregion

  } // class MainWindow
} // namespace Viatronix.Utilities.Service.ServiceManager
