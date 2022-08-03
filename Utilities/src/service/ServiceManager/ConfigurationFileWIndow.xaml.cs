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
using System.Windows.Shapes;

namespace Viatronix.Utilities.Service.ServiceManager
{
  /// <summary>
  /// Interaction logic for ConfigurationFileWIndow.xaml
  /// </summary>
  public partial class ConfigurationFileWindow : Window
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public ConfigurationFileWindow()
    {
      InitializeComponent();
    } // ConfigurationFileWindow()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the OK button's Click handler.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnOKClickHandler(object sender, EventArgs args)
    {
      DialogResult = true;
    } // OnOKClickHandler( sender, args )


    /// <summary>
    /// Handles the Cancel button's Click handler.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnCancelClickHandler(object sender, EventArgs args)
    {
      DialogResult = false;
    } // OnCancelClickHandler( sender, args )

    #endregion

  } // class ConfigurationFileWindow
} // namespace Viatronix.Utilities.Service.ServiceManager
