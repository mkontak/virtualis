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

using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

    #region fields

    /// <summary>
    /// The data context.
    /// </summary>
    private TransferManagerViewModel m_viewModel = new TransferManagerViewModel();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();

      DataContext = m_viewModel;

      m_viewModel.Initializing += new EventHandler(OnViewModelInitializingHandler);
      m_viewModel.Initialized += new EventHandler(OnViewModelInitializedHandler);
      m_viewModel.InitializationFailure += new EventHandler(OnViewModelInitializationFailureHandler);
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
} // namespace Viatronix.Utilities.Transfer
