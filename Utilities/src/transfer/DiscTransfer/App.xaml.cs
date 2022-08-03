using System;
using System.Threading;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

using Viatronix.Logging;

namespace Viatronix.Utilities.Transfer.DiscTransfer
{
  /// <summary>
  /// Interaction logic for App.xaml
  /// </summary>
  public partial class App : Application
  {

    #region methods

    /// <summary>
    /// Handles the startup of this application.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnStartup(StartupEventArgs args)
    {
      base.OnStartup(args);

      Application.Current.DispatcherUnhandledException += new System.Windows.Threading.DispatcherUnhandledExceptionEventHandler(OnUnhandledExceptionHandler);

      Log.Initialize();
    } // OnStartup( args )

    #endregion

    #region event handlers

    private void OnUnhandledExceptionHandler(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs args)
    {
      Log.Error(args.Exception.Message, "", "");
    }

    #endregion

  } // class App
} // namespace Viatronix.Utilities.Transfer.DiscTransfer
