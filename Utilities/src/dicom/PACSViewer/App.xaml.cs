// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Configuration;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Windows;
using System.Windows.Threading;

namespace Viatronix.Utilities.Dicom.PACSViewer
{
  /// <summary>
  /// Interaction logic for App.xaml
  /// </summary>
  public partial class App : Application
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public App()
    {
      Logging.Log.Initialize();
      Application.Current.DispatcherUnhandledException += new DispatcherUnhandledExceptionEventHandler(OnApplicationExceptionHandler);
    } // App()

    #endregion

    #region methods

    /// <summary>
    /// Handles the startup of the application.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected override void OnStartup(StartupEventArgs args)
    {
      base.OnStartup(args);

      if (Convert.ToBoolean(ConfigurationManager.AppSettings["serviceMode"]))
      {
        StartupUri = new Uri("ServiceWindow.xaml", UriKind.Relative);
      }
      else
      {
        StartupUri = new Uri("MainWindow.xaml", UriKind.Relative);
      }
    } // OnStartup( args )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the application's unhandled exceptions.  We'll show a message box with the contents of the exception.
    /// </summary>
    /// <param name="sender">The origin of the exception.</param>
    /// <param name="args"Argu></param>
    private void OnApplicationExceptionHandler(object sender, DispatcherUnhandledExceptionEventArgs args)
    {
      MessageBox.Show(string.Format("An unhandled error occured.  Exception: {0}", args.Exception.Message));
    } // OnApplicationExceptionHandler( sender, args )

    #endregion

  } // class App
} // namespace Viatronix.Utilities.Dicom.PACSViewer
