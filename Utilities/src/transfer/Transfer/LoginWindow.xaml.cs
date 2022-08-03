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
using System.Windows.Shapes;

using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Utilities.Transfer
{
  /// <summary>
  /// Interaction logic for LoginWindow.xaml
  /// </summary>
  public partial class LoginWindow : Window
  {

    #region fields

    /// <summary>
    /// The security provider.
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public LoginWindow()
    {
      InitializeComponent();
    } // LoginWindow()

    #endregion

    #region properties
    
    /// <summary>
    /// Gets the username.
    /// </summary>
    public string Username
    {
      get { return m_usernameTextBox.Text; }
    } // Username


    /// <summary>
    /// Gets the password.
    /// </summary>
    public string Password
    {
      get { return m_passwordTextBox.Password; }
    } // Username

    #endregion

    #region methods

    /// <summary>
    /// Handles the intialize.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnInitialized(EventArgs args)
    {
      base.OnInitialized(args);

      var providers = ProviderFactory.Create();

      IProvider provider = null;
      if (providers.TryFind(ProviderType.Security, ref provider))
      {
        m_securityProvider = (ISecurityProvider)provider;
      }
    } // OnInitialized( args )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the ok Button's Click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnOKClickHandler(object sender, EventArgs args)
    {
      DialogResult = true;
    } // OnOKClickHandler( sender, args )


    /// <summary>
    /// Handles the cancel Button's Click event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnCancelClickHandler(object sender, EventArgs args)
    {
      DialogResult = false;
      MessageBox.Show("User cancelled login.  Application will now close.", "Login Operation Cancelled", MessageBoxButton.OK, MessageBoxImage.Warning);
    } // OnCancelClickHandler( sender, args )

    #endregion

  } // class LoginWindow
} // namespace Viatronix.Utilities.Transfer
