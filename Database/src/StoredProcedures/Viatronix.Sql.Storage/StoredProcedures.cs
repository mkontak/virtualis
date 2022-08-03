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
using System.Management;
using System.Collections.Generic;
using System.Security.Principal;
using System.Xml;
using System.IO;
using System.IO.Compression;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using Viatronix.Sql;


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
    /// Log file name
    /// </summary>
    private static string m_logPath = System.IO.Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix\\Logs\\Sql");

    /// <summary>
    /// Log file name
    /// </summary>
    private static string m_logfilename = "vxdbsql_storage.log";

    #endregion

    #region Storage Database Procedures



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
      return Viatronix.Sql.Helper.ResetNamedEvent(m_logfilename, name);
    } // CreateEvent(string name)



    /// <summary>
    /// Gets the system id that we use for the licensing
    /// </summary>
    /// <param name="systemid">This is the system id that needs to be returned</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static void sp_getSystemId(out string systemid)
    {

      systemid = Viatronix.Sql.Helper.GetSystemId(SqlContext.WindowsIdentity);
    } // GetSystemId(out string systemid)


    /// <summary>
    /// Serializes the xml receieved. It is assumed the node's name is used for the filename and the location attribute is used to
    /// determine the directory.
    /// </summary>
    /// <param name="xml">xml</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_createDirectory(string DIRECTORY)
    {
      int rc = 0;

      /// Cannot serialize if the data is NULL
      if (!System.IO.Directory.Exists(DIRECTORY))
      {
        System.IO.Directory.CreateDirectory(DIRECTORY);
        rc = 1;
      }

      return rc;


    } // SerializeXml(SqlXml xml)


    /// <summary>
    /// Serializes the xml receieved. It is assumed the node's name is used for the filename and the location attribute is used to
    /// determine the directory.
    /// </summary>
    /// <param name="xml">xml</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_fileExists(string FILEPATH)
    {

      /// Cannot serialize if the data is NULL
      if (string.IsNullOrEmpty(FILEPATH))
        return 0;

      return (System.IO.File.Exists(FILEPATH) ? 1 : 0);


    } // SerializeXml(SqlXml xml)

    /// <summary>
    /// Serializes the xml receieved. It is assumed the node's name is used for the filename and the location attribute is used to
    /// determine the directory.
    /// </summary>
    /// <param name="xml">xml</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_serializeXml(SqlXml xml)
    {

      /// Cannot serialize if the data is NULL
      if (xml == null || xml.IsNull) 
        return 0;

      int rc = 1;
      string uid = string.Empty;
      string type = "series";

      try
      {

        string sXml = xml.Value.Replace("\\", "\\\\");

        LogMessage("SerializeXml", sXml );

         // Create and xml ducment to read the xml
        XmlDocument doc = new XmlDocument();

        // Read the xml
        doc.LoadXml(sXml);

        foreach (XmlNode node in doc.DocumentElement.ChildNodes)
        {
          // Extract the node name
          string name = node.Name;

          LogMessage("SerializeXml", "Node to serialize is " + name);

          // Extract the location attribute
          type = node.Name;
          string location = (node.Attributes["loc"] == null ? string.Empty : node.Attributes["loc"].Value.ToString());
          uid = (node.Attributes["uid"] == null ? string.Empty : node.Attributes["uid"].Value.ToString());

          LogMessage("SerializeXml", "Location is " + location);
 
          // If the root or location is missing just return 
          if (location.Length == 0)
            return 0;

          //===============================================
          // If the path does not exists then create it
          //===============================================
          if (!Directory.Exists(location))
          {
            LogMessage("SerializeXml", "Creating " + location);

            Directory.CreateDirectory(location);

          }   // END ... If the location does not exist

          string filePath = Path.Combine(location, name + ".xml");

          LogMessage("SerializeXml", "Writing xml to " + filePath);

          //=======================
          // Set up the XML writer
          //======================
          XmlTextWriter writer = new XmlTextWriter(filePath, System.Text.Encoding.ASCII);
          writer.Formatting = Formatting.Indented;

          XmlDocument nodeDoc = new XmlDocument();

          nodeDoc.LoadXml(node.OuterXml);

          // Write the xml
          nodeDoc.Save(writer);

          // Close the writer
          writer.Close();

        }

      }
      catch (Exception ex)
      {
        LogMessage("SerializeXml", "Failed to serialize xml : " + ex.Message);

        rc = 0;

        if (!string.IsNullOrEmpty(uid))
        {
          try
          {
            using (SqlConnection connection = new SqlConnection("context connection=true"))
            {

              // Open the connection
              connection.Open();

              Helper.LogErrorMessage(connection.Database, "sp_serializeXml", 901402, ex.Message);


              string sql = string.Format("INSERT INTO [Storage].[v3d].[SerializeErrors] ( uid, type, msg ) VALUES ( '{0}', '{1}', '{2}' ) ", uid, type, ex.Message);

              SqlCommand command = new SqlCommand(sql, connection);

              command.ExecuteNonQuery();

            }


            // (901402) Filaed to serialize xml
            //throw new Exception("901402:" + ex.Message);
          }
          catch ( Exception dbex )
          {
            LogMessage("SerializeXml", "Failed to add SerializeError : " + dbex.Message);
          }
        }

      }

      return rc;

    } // SerializeXml(SqlXml xml)

    /// <summary>
    /// Deletes the specified series directory and all data/sub-directories recursivly. Will also
    /// delete the parent (study) directory if there are no more series beneath it.
    /// </summary>
    /// <param name="path">Directory to be deleted</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_deleteSeriesDirectory(string seriesDirectory)
    {
      int rc = 1;

      ///
      string studyDirectory = System.IO.Path.GetDirectoryName(seriesDirectory);

      /// Delete the series directory
      rc = Helper.DeleteDirectory(m_logfilename, seriesDirectory);

      // ======================================================
      // Only need to delete the study directory if it exists
      // =====================================================
      if ( Directory.Exists(studyDirectory) )
        if (Directory.GetDirectories(studyDirectory).Length == 0)
          rc = Helper.DeleteDirectory(m_logfilename, studyDirectory);

      return rc;

    }
    /// <summary>
    /// Deletes the directory and all data/sub-directories recursivly
    /// </summary>
    /// <param name="path">Directory to be deleted</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_deleteDirectory(string uid, string type, string directory)
    {

      int rc = 1;

      try
      {
        Helper.DeleteDirectory(m_logfilename, directory);
      }
      catch (Exception ex)
      {
        LogMessage("DeleteDirectory", "Deleting of directory failed : " + ex.Message);

        rc = 0;

        if (!string.IsNullOrEmpty(uid))
        {
          try
          {
            using (SqlConnection connection = new SqlConnection("context connection=true"))
            {

              // Open the connection
              connection.Open();

              Helper.LogErrorMessage(connection.Database, "sp_deleteDirectory", 901403, ex.Message);


              string sql = string.Format("INSERT INTO [Storage].[v3d].[DirectoryErrors] ( uid, type, path,  msg ) VALUES ( '{0}', '{1}', '{2}', '{3}' ) ", uid, type, directory, ex.Message);

              SqlCommand command = new SqlCommand(sql, connection);

              command.ExecuteNonQuery();

            }


            // (901402) Filaed to serialize xml
            //throw new Exception("901402:" + ex.Message);
          }
          catch ( Exception dbex )
          {
            LogMessage("DeleteDirectory", "Deleting of directory failed : " + dbex.Message);
          }
        }

      }

       
      return rc;

    } // DeleteDirectoryRecursive(string dir)


    /// <summary>
    /// Deletes all files that match the supplied patter
    /// </summary>
    /// <param name="path">Directory to look for files in</param>
    /// <param name="pattern">file pattern</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_deleteFiles(string path, string pattern)
    {

      int rc = 0;

      try
      {


        LogMessage("DeleteFiles", string.Format("Attempting to delete files [path: {0}]  [pattern: {1}] ", path, pattern));

        //==============================================
        // Only need to delete files if the path exists
        //==============================================
        if (Directory.Exists(path))
        {

          //======================================
          // Delete all the directorories first
          //======================================
          foreach (string file in Directory.GetFiles(path, pattern))
          {
            LogMessage("DeleteFiles", string.Format("Deleteing file {0} ", file));

            // Call ourself
            File.Delete(file);

            rc++;

          } // END ... For each directory

        } // END ... If the path exists 
        else
          LogMessage("DeleteFiles", string.Format("Supplied path {0} does not exist", path));

      }
      catch (Exception ex)
      {
        LogMessage("DeleteFiles", "Deleting of files failed : " + ex.Message);

        // (901404) Deletion of file failed
        throw new Exception("901404:" + ex.Message);
      }

      return rc;

    } // DeleteFiles(string files, string pattern)

    /// <summary>
    /// Deletes all files that match the supplied patter
    /// </summary>
    /// <param name="path">Directory to look for files in</param>
    /// <param name="pattern">file pattern</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_deleteFile(string file)
    {

      int rc = 1;

      try
      {

 
        LogMessage("DeleteFile", string.Format("Attempting to delete {0}", file));

        //==============================================
        // Only need to delete file if it exists
        //==============================================
        if (File.Exists(file))
        {

           // Call ourself
           File.Delete(file);

        } // END ... If the path exists 
        else
          LogMessage("DeleteFile", string.Format("{0} does not exist", file));

      }
      catch (Exception ex)
      {
        LogMessage("DeleteFile", string.Format("Deleting of {0} : {1}", file, ex.Message));

        rc = 0;

        throw new Exception("901404:" + ex.Message);
      }

      return rc;

    } // DeleteFiles(string files, string pattern)



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
    } // LogMessage(string path, string method, string message)
    


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
