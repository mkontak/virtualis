// $Id: StoredProcedures.cs,v 1.11 2010/02/12 14:14:36 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Threading;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using Microsoft.SqlServer.Server;
using System.Collections.Generic;
using System.Security.Principal;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;



  /// <summary>
  /// Stored procedures assembly
  /// 
  /// Contains the stored procedure used within the ViatronixDB database
  /// 
  /// The following SQL commands are to be run seperatly in order to utilize this assembly in the SQL server. SQL server must be version 2005 or up
  ///
  /// 1. CREATE ASSEMBLY [System.Management] FROM 'C:\Windows\Microsoft.NET\Framework\v2.0.50727\System.Management.dll' WITH PERMISSION_SET = UNSAFE;
  /// 
  /// 2. CREATE ASSEMBLY [Viatronix.v3D.Management] FROM 'C:\CVS\V3D\bin\vxUtilities\Viatronix.v3D.Management.dll' WITH PERMISSION_SET = UNSAFE;
  /// 
  /// 3. CREATE PROCEDURE sp_getsystemid @SYSTEM_ID NVARCHAR(256) OUTPUT  AS EXTERNAL NAME [Viatronix.v3D.Management].StoredProcedures.GetSystemId
  /// 
  /// Updates that do not change the stored procedures contract can be done with the following command
  /// 
  /// ALTER ASSEMBLY [Viatronix.v3D.Management] FROM 'C:\CVS\V3D\bin\vxUtilities\Viatronix.v3D.Management.dll' WITH PERMISSION_SET = UNSAFE;
  /// 
  /// IMPORTANT:
  /// 
  ///     Any change to the DLL will require all procedures and assemblies to be dropped an readded...
  /// 
  /// </summary>
  public partial class StoredProcedures
  {


    #region Master Database Procedures


    /// <summary>
    /// Serializes the xml receieved. It is assumed the node's name is used for the filename and the location attribute is used to
    /// determine the directory.
    /// </summary>
    /// <param name="xml">xml</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_logErrorMessage(string database, string procedure, int errorCode, string errorMessage)
    {
      
      
      try
      {
        if ( string.IsNullOrEmpty(database) )
          database = "Database";

        string [] folders = new string[] { System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix", "Logs" };

        string logPath = System.IO.Path.Combine(folders);

        if (!Directory.Exists(logPath))
          Directory.CreateDirectory(logPath);

        logPath = Path.Combine(logPath, database + ".log");

        using (System.IO.StreamWriter writer = new StreamWriter(logPath, true))
        {
          writer.WriteLine(string.Format("{0:MM/dd/yyyy-HH:mm:ss}  {1,-32}  {2,-8:D} {3}", DateTime.Now, procedure, errorCode.ToString(), errorMessage)); 
        }

        return 1;
      }
      catch (Exception)
      {
        // Do nothing if we have errors
        return 0;
      }
    }



    #endregion

  


  };  // class StoredProcedures

//} // namespave Viatronix.v3D.Sql.Management

#region revision history

// $Log: StoredProcedures.cs,v $
// Revision 1.11  2010/02/12 14:14:36  mkontak
// Updates
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10  2009/11/20 14:59:17  mkontak
// Fixes for processing of the Jobs
//
// Revision 1.9  2009/11/04 22:06:39  mkontak
// Added image count to series insert
//
// Revision 1.8  2009/11/04 21:08:58  mkontak
// Minor change
//
// Revision 1.7  2009/11/02 19:23:00  mkontak
// Added logging
//
// Revision 1.6  2009/10/29 20:59:07  mkontak
// Fixes to external stored procedures
//
// Revision 1.5  2009/10/22 15:01:46  mkontak
// Need to remove <dicomHeader> tags from the DicomHeader series property
// before inserting into database. These tags are automatically added when retreiving
// from the database.
//
// Revision 1.4  2009/10/09 19:01:04  mkontak
// Changes to external stored procedure for receiving dicom images
//
// Revision 1.3  2009/09/25 18:59:09  mkontak
// Changes to fix Dicom Receiving
//
// Revision 1.2  2009/08/12 19:31:49  mkontak
// Moved some procedures into Security
//
// Revision 1.1  2009/08/05 13:40:25  mkontak
// Moved/Renamed
//
// Revision 1.3  2009/08/04 12:10:40  mkontak
// no message
//
// Revision 1.2  2009/07/31 19:14:36  mkontak
// Added stored procedures for file system syncing
//
// Revision 1.1  2009/07/15 16:32:06  mkontak
// Moved Management Project
//
// Revision 1.2  2009/04/28 12:24:57  mkontak
// Cleanup unneeded usings
//
// Revision 1.1  2008/10/21 13:31:50  mkontak
// Fix for sp_addSeries
//
// Revision 1.1  2008/05/06 20:50:20  mkontak
// Initial check in
//
// $Header: /cvs/cvsRepository/V3D/src/Viatronix.SqlServer.Archive/StoredProcedures.cs,v 1.11 2010/02/12 14:14:36 mkontak Exp $
// $Id: StoredProcedures.cs,v 1.11 2010/02/12 14:14:36 mkontak Exp $

#endregion
