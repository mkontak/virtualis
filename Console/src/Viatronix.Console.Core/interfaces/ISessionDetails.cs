// $Id: ISessionDetails.cs,v 1.3 2006/03/06 16:14:04 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using Viatronix.Enterprise.Entities;
using Viatronix.Console.Entities;


namespace Viatronix.Console
{
	/// <summary>
	/// Summary description for ISessionDetails.
	/// </summary>
	public interface ISessionDetails
	{
		
    /// <summary>
    /// Initializes the ISessionDetails object.
    /// </summary>
    /// <param name="session">The session object.</param>
    void Initialize( Series session );

    /// <summary>
    /// Returns a collection of snapshots contained in the session object.
    /// </summary>
    /// <returns>SnapshotCollection</returns>
    SnapshotCollection GetSnapshots();

    /// <summary>
    /// Returns the snapshot at the specified location.
    /// </summary>
    /// <param name="index">The snapshot index</param>
    /// <returns>A snapshot object</returns>
    Snapshot GetSnapshot( int index );

    /// <summary>
    /// Returns the number of snapshots contained in the session object.
    /// </summary>
    /// <returns>The number of snapshots.</returns>
    int GetSnapshotCount();

    /// <summary>
    /// Returns a collection of movies contained in the session object.
    /// </summary>
    /// <returns>MovieCollection</returns>
    MovieCollection GetMovies();

    /// <summary>
    /// Returns the movie at the specified location.
    /// </summary>
    /// <param name="index">The movie index</param>
    /// <returns>A movie object</returns>
    Movie GetMovie( int index );

    /// <summary>
    /// Returns the number of movies contained in the session object.
    /// </summary>
    /// <returns>The number of movies.</returns>
    int GetMovieCount();

    /// <summary>
    /// Returns a collection of Base64 encoded bitmap strings.
    /// </summary>
    /// <returns>a collection of strings.</returns>
    string[] GetReport();  

    /// <summary>
    /// Returns a Base64 encoded report page.
    /// </summary>
    /// <param name="index">The page index</param>
    /// <returns>A Base64 encoded string</returns>
    string GetReportPage( int index ); 
 
    /// <summary>
    /// Returns the number of report pages contained in the session object.
    /// </summary>
    /// <returns>The number of report pages.</returns>
    int GetReportPageCount();

    /// <summary>
    /// Extracts the PDF Report from the Session report.xml
    /// </summary>
    /// <param name="pdfFile">string</param>
    void SaveReportToPDF( string pdfFile );
    
  } // interface ISessionDetails
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: ISessionDetails.cs,v $
// Revision 1.3  2006/03/06 16:14:04  romy
// added SaveReportToPDF  method
//
// Revision 1.2  2006/03/06 16:12:02  romy
// added Save to PDF Report method
//
// Revision 1.1  2006/01/03 16:19:48  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/interfaces/ISessionDetails.cs,v 1.3 2006/03/06 16:14:04 romy Exp $
// $Id: ISessionDetails.cs,v 1.3 2006/03/06 16:14:04 romy Exp $

#endregion