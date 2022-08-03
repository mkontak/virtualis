// $Id: StoredProcedures.cs,v 1.5 2010/02/16 20:03:42 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using Microsoft.SqlServer.Server;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Security.Principal;
using System.Xml;
using System.Text;
using Viatronix.Database.Sql;




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
  ///     Any change to the DLL will require all procedures and assemblies to be dropped an re-added...
  /// 
  /// </summary>
  public partial class StoredProcedures
  {

    #region fields


    /// <summary>
    /// Log file name
    /// </summary>
    private static string m_filename = "sql_security.log";

    #endregion

    #region Licensing Database Procedures


 
    /// <summary>
    /// Gets a hash key that is used for licensing
    /// </summary>
    /// <param name="systemId">Security System id</param>
    /// <param name="date">Date issued</param>
    /// <param name="number">License Number</param>
    /// <param name="key">Key to be returned</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_getHashKey(string systemId, string transactionId, int number, out string key)
    {

      // List of values to be hashed
      List<string> list = new List<string>();

      // Add the processor id
      list.Add(systemId);

      // Add the serial number
      list.Add(transactionId);

      list.Add(number.ToString());

      key = Viatronix.Sql.Helper.Hash(list);

      return 1;

    } // GetHashKey(string date, int number, out string key)


    /// <summary>
    /// Gets the system id that we use for the licensing
    /// </summary>
    /// <param name="systemid">This is the system id that needs to be returned</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static void sp_getSystemId(out string systemid)
    {
      systemid = Viatronix.Sql.Helper.GetSystemId(SqlContext.WindowsIdentity);

    } // GetSystemId(out string systemid)



    #endregion

    #region Helper Methods

    /// <summary>
    /// Logs a message to the specified file
    /// </summary>
    /// <param name="path">path</param>
    /// <param name="message">Message to log</param>
    private static void LogMessage(string method, string message)
    {
      Viatronix.Sql.Helper.LogMessage(m_filename, method, message);
    }




    #endregion


  };  // class StoredProcedures


#region revision history

// $Log: StoredProcedures.cs,v $
// Revision 1.5  2010/02/16 20:03:42  mkontak
// Changed the way the hashing works
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4  2010/02/12 14:14:52  mkontak
// Updates
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3  2010/02/01 16:07:49  mkontak
// Added windows authentication
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1  2009/09/25 19:00:19  mkontak
// Initial check in
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
// $Header: /cvs/cvsRepository/V3D/src/Viatronix.SqlServer.Security/StoredProcedures.cs,v 1.5 2010/02/16 20:03:42 mkontak Exp $
// $Id: StoredProcedures.cs,v 1.5 2010/02/16 20:03:42 mkontak Exp $

#endregion
