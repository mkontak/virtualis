using System;
using System.ComponentModel;
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

using Viatronix.Utilities.Transfer.DiscTransfer.ViewModels;

namespace Viatronix.Utilities.Transfer.DiscTransfer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

    #region fields

    /// <summary>
    /// The manager view model.
    /// </summary>
    private TransferViewModel m_managerViewModel = new TransferViewModel();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();

      DataContext = m_managerViewModel;
    } // MainWindow()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the window's Closing event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnWindowClosingHandler(object sender, CancelEventArgs args)
    {
      if (m_managerViewModel.IsActive)
      {
        MessageBox.Show("Transfer is currently in progress.  If neccessary, cancel transfer before attempting to close window.", "Cannot Close", MessageBoxButton.OK, MessageBoxImage.Warning);
        args.Cancel = true;
      }
    } // OnWindowClosingHandler( sender, args )

    #endregion

  } // class MainWindow
} // namespace Viatronix.Utilities.Transfer.DiscTransfer
