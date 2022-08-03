// $Id: ISessionProvider.cs,v 1.5 2007/03/07 22:06:46 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )


using System;
using Viatronix.v3D.Core;
using System.Collections;
using System.IO;

using Viatronix.Reporting;
using System.Drawing;
using System.Configuration;
using System.Diagnostics;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
	/// <summary>
	/// ISessionProvider,implement this interface to provide Study specific session data.
	/// </summary>
  public interface ISessionProvider
  {
    void Initialize ( Series series, string stageArea );

    /// <summary>
    /// GetSnapshotsCollection Method
    /// </summary>
    ArrayList GetSnapshotsCollection();

    /// <summary>
    /// Get Movie collection method inclusing its preview images
    /// </summary>
    /// <returns></returns>
    ArrayList GetMovieCollection();

    /// <summary>
    /// Is report Available method
    /// </summary>
    /// <param name="reportExtension">reportExtension</param>
    /// <returns></returns>
    bool IsReportAvailable( string reportExtension );

    /// <summary>
    /// Export Report to PDF Method
    /// </summary>
    /// <returns></returns>
    string ExportReportToPdf(); 

    /// <summary>
    /// Extract snapshots from the Report
    /// </summary>
    /// <param name="frame"></param>
    /// <returns></returns>
    System.Collections.ArrayList ExtractReportSnapshots( );

  }//ISessionProvider

} // interface ISessionProvider


#region revision history

// $Log: 
// $Header: 
// $Id: 

#endregion