using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

using Viatronix.Logging;

namespace Viatronix.Utilities.Licensing.LicenseInstaller
{
  /// <summary>
  /// Interaction logic for App.xaml
  /// </summary>
  public partial class App : Application
  {

    #region methods

    /// <summary>
    /// Handles the startup of the application.
    /// </summary>
    private void OnStartupHandler(object sender, StartupEventArgs args)
    {
      Log.Initialize();
    } // OnStartupHandler( args )

    #endregion

  } // class App
} // namespace Viatronix.Utilities.Licensing.LicenseInstaller
