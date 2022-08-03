// $Id: StoredProcedures.cs,v 1.3 2010/02/16 20:01:21 mkontak Exp $
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
using System.Management;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Security.Principal;
using System.Xml;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.Security.AccessControl;
using System.Text.RegularExpressions;
using Viatronix.Sql.Filtering;



/// <summary>
/// Drive Info 
/// </summary>
public class  StoreDriveInfo
{
  public StoreDriveInfo(  string name, DriveInfo info )
  {
    Name = name;
    Capacity = info.TotalSize;
    Used = info.TotalSize - info.TotalFreeSpace;
    Available = info.TotalFreeSpace;
    Message = string.Empty;
  }

  public StoreDriveInfo(string name, string message)
  {
    Name = name;
    Capacity = 0;
    Used = 0;
    Available = 0;
    Message = message;
  }

  public string Name;
  public Int64 Capacity;
  public Int64 Used;
  public Int64 Available;
  public string Message;
};

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

  #region fields

      /// <summary>
    /// Log Path
    /// </summary>
    private static string m_logPath = System.IO.Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix\\Logs\\Database");

  /// <summary>
  /// Log file name
  /// </summary>
  private static string m_logfilename = "sql_system.log";

  #endregion


  #region System Database Procedures

  /// <summary>
  /// Decrypts the value
  /// </summary>
  /// <param name="value"></param>
  /// <param name="decryptedValue"></param>
  /// <returns></returns>
  [Microsoft.SqlServer.Server.SqlProcedure]
  public static int sp_decrypt(string value, out string decryptedValue)
  {


    decryptedValue = Viatronix.Sql.Helper.Decrypt(value);

    return 1;

  } // Encrypt(string value, out string encryptedValue )

  /// <summary>
  /// Encrypts the value supplied
  /// </summary>
  /// <param name="value"></param>
  /// <param name="encryptedValue"></param>
  /// <returns></returns>
  [Microsoft.SqlServer.Server.SqlProcedure]
  public static int sp_encrypt(string value, out string encryptedValue)
  {


    encryptedValue = Viatronix.Sql.Helper.Encrypt(value);

    return 1;

  } // Encrypt(string value, out string encryptedValue )


  /// <summary>
  /// Performs a regular expression comparison
  /// </summary>
  /// <param name="ignoreCase">Flag idicating that we are ignoring the case</param>
  /// <param name="pattern">Expression pattern</param>
  /// <param name="value">Value to be compared</param>
  /// <returns></returns>
  [Microsoft.SqlServer.Server.SqlProcedure]
  public static int sp_regExpMatch(bool ignoreCase, string pattern, string value)
  {
    bool isMatch = false;

    /// Create the regualt expression 
    Regex rgx = (ignoreCase ? new Regex(pattern, RegexOptions.IgnoreCase) : new Regex(pattern));

    /// Check the value againast the regular expression for a match
    isMatch = rgx.IsMatch(value);


    return ( isMatch ? 1 : 0 );
  } // RegExMatch

  /// <summary>
  /// Sets the named event specified
  /// </summary>
  /// <param name="name">Event name</param>
  [Microsoft.SqlServer.Server.SqlProcedure]
  public static int sp_setNamedEvent(string name)
  {
    return Viatronix.Sql.Helper.SetNamedEvent(m_logfilename, name);
  } // SetNamedEvent(string name)

  /// <summary>
  /// Sets the named event 
  /// </summary>
  /// <param name="xml">xml</param>
  [Microsoft.SqlServer.Server.SqlProcedure]
  public static int sp_resetNamedEvent(string name)
  {
    return Viatronix.Sql.Helper.ResetNamedEvent(m_logfilename, name);
 

  } // ResetNamedEvent(string name)

  /// <summary>
  /// Creates the named event 
  /// </summary>
  /// <param name="xml">xml</param>
  [Microsoft.SqlServer.Server.SqlProcedure]
  public static int sp_createNamedEvent(string name)
  {

    return Viatronix.Sql.Helper.CreateNamedEvent(m_logfilename,name);


  } // CreateEvent(string name)



  [SqlFunction(
      DataAccess = DataAccessKind.Read,
      FillRowMethodName = "FillRow",
      TableDefinition = "name nvarchar(32), cap bigint , used bigint , avail bigint , msg nvarchar(512) ")]
  public static System.Collections.IEnumerable fn_getDriveInfo()
  {

    List<StoreDriveInfo> infos = new List<StoreDriveInfo>();

    string sql = "SELECT [name], [local] FROM [v3d].[Stores] ";

 
    using (SqlConnection connection = new SqlConnection("context connection=true"))
    {
      connection.Open();

      using (SqlCommand command = new SqlCommand(sql,connection)) 
      {

        // Read the schema
        using (SqlDataReader reader = command.ExecuteReader())
        {
          while (reader.Read())
          {
            string name = reader.GetString(0);
            string local = reader.GetString(1);

            string root = System.IO.Path.GetPathRoot(local);

            if (root.StartsWith("\\\\"))
              infos.Add(new StoreDriveInfo(name, "Cannot get drive information for UNC paths"));
            else
            {
              try
              {
                infos.Add(new StoreDriveInfo(name, new DriveInfo(root)));
              }
              catch (Exception ex)
              {
                infos.Add(new StoreDriveInfo(name, ex.Message));
              }
            }
          }
        } // END ... using reader

      }


    }




    return infos;

  } // GetDriveInfo(string name)

  public static void FillRow(
      object info,
      out SqlString name,
      out SqlInt64 cap,
      out SqlInt64 used,
      out SqlInt64 avail,
      out SqlString msg)
  {

    name = ((StoreDriveInfo)info).Name;
    cap = ((StoreDriveInfo)info).Capacity;
    avail = ((StoreDriveInfo)info).Available;
    used = ((StoreDriveInfo)info).Used;
    msg = ((StoreDriveInfo)info).Message;

 }


  /// <summary>
  /// Serializes the xml received. It is assumed the node's name is used for the filename and the location attribute is used to
  /// determine the directory.
  /// </summary>
  /// <param name="xml">xml</param>
  [Microsoft.SqlServer.Server.SqlProcedure]
  public static int sp_getAvailableStore( out string store )
  {

    int rc = 1;

    store = string.Empty;
    string xml = string.Empty;

    string largestSeriesSizeValue = string.Empty;
    double largestSeriesSize = 0.0;


    try
    {


      //===================================================
      // Attempt to read the schema XML from the database
      //===================================================
      using (SqlConnection connection = new SqlConnection("context connection=true"))
      {

        //// Open the connection
        connection.Open();


        LogMessage( "GetAvailableStore", "GetAvailableStore starting");


        LogMessage( "GetAvailableStore", "Getting LargetsSeriesSize from the databases settings");

        string sql = "SELECT [System].[v3d].[fn_getSetting]( 'LargestSeriesSize', '') AS SIZE";

        SqlCommand command = new SqlCommand(sql, connection);

        // Read the schema
        using (SqlDataReader reader = command.ExecuteReader())
        {
          while (reader.Read())
          {
            largestSeriesSizeValue = reader.GetString(0);
          }
        } // END ... using reader

        LogMessage( "GetAvailableStore", "Largest Series Size:  " + largestSeriesSizeValue);

 
        //================================
        // Make sure the setting exists
        //=================================
        if (largestSeriesSizeValue.Length == 0)
        {
          LogMessage("GetAvailableStore", "901800: No largest series size setting found");
          throw new System.MissingFieldException("901800: No largest dataset size setting found");
        } // END ... If the average dataset size setting is missing
        else
        {

          LogMessage( "GetAvailableStore", "Looking for tags");

          int[] powers = { 4, 3, 2, 1 };
          string[] tags = { "TB", "GB", "MB", "KB" };

          for (int i = 0; i < tags.Length; ++i)
          {

            int index = largestSeriesSizeValue.IndexOf(tags[i]);
            if (index > 0)
            {
              largestSeriesSizeValue = largestSeriesSizeValue.Substring(0, index);
              LogMessage( "GetAvailableStore", "Largest Series Size:  " + largestSeriesSizeValue);

              if (!double.TryParse(largestSeriesSizeValue.Trim(), out largestSeriesSize))
              {
                LogMessage("GetAvailableStore", "901801: Invalid largest series size setting found");
                throw new System.MissingFieldException("901801: Invalid largest series size setting found");
              }

              largestSeriesSize = largestSeriesSize * Math.Pow(1024.0, (double)powers[i]);
              break;
            }
          }


          sql = "SELECT * FROM [v3d].[Stores] WHERE [enabled] = 1 ORDER BY [priority] FOR XML RAW('store'), TYPE, ROOT('collection')";

          // Sql command
          command = new SqlCommand(sql, connection);


          // Read the schema
          using (SqlDataReader reader = command.ExecuteReader())
          {
            while (reader.Read())
            {
              xml = reader.GetString(0);
            }
          } // END ... using reader

        } // END ... using

        LogMessage( "GetAvailableStore", xml);

        // Create and xml ducument to read the xml
        XmlDocument doc = new XmlDocument();

        // Read the xml
        doc.LoadXml(xml.ToString());

        // 
        XmlNodeList stores = doc.DocumentElement.SelectNodes("store");

        //===============================================
        // Check for NO storage devices in the database
        //===============================================
        if (stores.Count == 0)
          throw new System.MissingMemberException("901802: No storeage device defined");


        //=============================
        // Look for all storage device
        //=============================
        foreach (XmlNode node in stores)
        {

          // Name is the key so it must be present
          string name = node.Attributes["name"].Value.ToString();

          LogMessage( "GetAvailableStore", "Name: " + name);

          // Extract the location attribute
          string local = (node.Attributes["local"] == null ? string.Empty : node.Attributes["local"].Value.ToString());

          LogMessage( "GetAvailableStore", "Local: " + local);

          if (local.Length == 0)
          {
            LogMessage("GetAvailableStore", "901803: Missing local setting for storage device");
            throw new System.MissingFieldException("901803: Missing local setting for storage device");
          }

          //===============================================
          // If the location does not exists then create it
          //===============================================
          if (!System.IO.Directory.Exists(local))
          {
            System.IO.Directory.CreateDirectory(local);
          }

          //==================================================================
          // Cannot check the disk space on a UNC path we assume all is well
          //===================================================================
          if (!local.StartsWith("\\\\"))
          {

            string root = System.IO.Path.GetPathRoot(local);

            LogMessage( "GetAvailableStore", "Root: " + root);


            DriveInfo driveInfo = new DriveInfo(root);




            LogMessage( "GetAvailableStore", "Total Fee Bytes: " + driveInfo.TotalFreeSpace.ToString());
            LogMessage( "GetAvailableStore", "Largest Series Size: " + largestSeriesSize.ToString());

            //===============================================================================================
            // If total bytes free is greater then the average dataset size the select this storage device
            //===============================================================================================
            if (driveInfo.TotalFreeSpace > (long)largestSeriesSize)
            {
              store = name;
              break;
            }
          }
          else
          {
            store = name;
            break;
          }

        }  // END ... For each storage devoce

        LogMessage( "GetAvailableStore", "Store: " + store);

        //==========================================
        // If there is no storage device the error
        //=========================================
        if (store.Length == 0)
        {
          LogMessage("GetAvailableStore", "901804: Not enough storage space");
          throw new System.IO.IOException("901804: Not enough storage space");
        } // END ... If no storage device is found

      }

    }
    catch (Exception ex)
    {
      rc = 0;

      LogMessage( "GetAvailableStore", "Failed to get store : " + ex.Message);

      if (ex.Message.StartsWith("90"))
        throw;
      else
        throw new Exception("901805: " + ex.Message);
    }

    return rc;

  } // GetAvailableStore(SqlXml xml)


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
    Viatronix.Sql.Helper.LogMessage(m_logfilename, method, message);

  }
 


  #endregion


};  // class StoredProcedures


#region revision history

// $Log: StoredProcedures.cs,v $
// Revision 1.3  2010/02/16 20:01:21  mkontak
// Changed the way the hashing works
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2  2010/02/12 14:15:02  mkontak
// Updates
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1  2010/02/11 13:56:15  mkontak
// Reorganized database and split out system stored procedures
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
// $Header: /cvs/cvsRepository/V3D/src/Viatronix.SqlServer.System/StoredProcedures.cs,v 1.3 2010/02/16 20:01:21 mkontak Exp $
// $Id: StoredProcedures.cs,v 1.3 2010/02/16 20:01:21 mkontak Exp $

#endregion
