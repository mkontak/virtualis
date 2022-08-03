using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

using GalaSoft.MvvmLight.Threading;

namespace Viatronix.Utilities.Service.ServiceManager
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
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnStartup(StartupEventArgs args)
    {
      base.OnStartup(args);

      DispatcherHelper.Initialize();

      Global.Initialize();
    } // OnStartup( args )

    #endregion

  } // class App
} // namespace Viatronix.Utilities.Service.ServiceManager
