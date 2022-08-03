// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

namespace Viatronix.Utilities.Dicom.DicomSend
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
      this.Startup += new StartupEventHandler(this.OnApplicationStartupHandler);

      InitializeComponent();
    } // App()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the application startup.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnApplicationStartupHandler(object sender, EventArgs args)
    {
      Viatronix.Logging.Log.Initialize();
    } // OnApplicationStartupHandler( sender, args )

    #endregion

  } // class App
} // namespace Viatronix.Utilities.Dicom.DicomSend
