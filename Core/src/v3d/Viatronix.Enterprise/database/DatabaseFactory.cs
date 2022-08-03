// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DatabaseFactory.cs
//
// Description: Warps the Microsoft's enterprise libraries DatabaseFactory::CreateDatabase() method so that if it fails if will attempt to read the connection
//              strings from the $(APPDATA)\Viatronix\Config\connections.config file. This allows the connection strings in the app.config to override the 
//              connection string in the connections.config file.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Data
{

  /// <summary>
  /// Class wraps the Enterprise libraries CreateDatabase to that if the connection configuration can be in the application configuration file or
  /// the connections.config file.
  /// </summary>
  public class DatabaseFactory
  {
    #region fields

    /// <summary>
    /// Connections config file 
    /// </summary>
    private static readonly string m_connectionsLocation = "Viatronix\\Config\\connections.config";

    /// <summary>
    /// Connections list
    /// </summary>
    private static Dictionary<string, DatabaseConnectionStrings> m_connectionStrings = new Dictionary<string, DatabaseConnectionStrings>();


    /// <summary>
    /// Deafult connection strings
    /// </summary>
    private static DatabaseConnectionStrings m_defaultConnectionString = null;
    
    #endregion
    
    /// <summary>
    /// Creates the Database object for the name specified. Looks for the speicied database in the connextion.xml config file.
    /// </summary>
    /// <param name="application">Application to load db for</param>
    /// <param name="name">Name of the database</param>
    /// <returns></returns>
    public static Database CreateDatabase(string databaseName, string connectionsName = "default")
    {


      System.Threading.Mutex mutex = new System.Threading.Mutex(false);

      mutex.WaitOne();

      // ==========================================
      // Check if the connection have been read in
      // ===========================================
      if ( m_connectionStrings.Count == 0 )
      {

 
        // Set the connections path APPDATA\Viatronix\Config\connectsion.config
        string connectionsFile = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.CommonApplicationData), m_connectionsLocation);

        // ===================================
        // Only read the file if it exists
        // ===================================
        if (File.Exists(connectionsFile))
        {


          // Parse the XML
          XmlDocument doc = new XmlDocument();
          doc.Load(connectionsFile);

          foreach (XmlNode connectionStrings in doc.DocumentElement.SelectNodes("connectionStrings"))
          {
            DatabaseConnectionStrings db = DatabaseConnectionStrings.Create(connectionStrings);

            // ======================================================================================
            // If the application and provider are empty then this is the default connection string
            // =====================================================================================
            if (db.Name.Length == 0)
            {
              if (!m_connectionStrings.ContainsKey("default"))
              {
                m_defaultConnectionString = db;
                m_connectionStrings.Add("default", db);
              }
              else
                Logging.Log.Warning("Default connection strings have already been defined, please fix the database connection strings file", "DatabaseFactory", "CreateDatabase");
            }
            else
            {
              if (!m_connectionStrings.ContainsKey(db.Name))
                m_connectionStrings.Add(db.Name, db);
              else
                Logging.Log.Warning(string.Format("{0} connection strings have already been defined, please fix the database connection strings file"), "DatabaseFactory", "CreateDatabase");
            }

          } // END ... For each database connection strings group

 
        } // END ... If the connections.config exists


        mutex.Close();
 
      } // END ... If the connections have not been read in yet



      // ==================================================================================
      // If the connection name is not found in the file the throw exception
      // ==================================================================================
      if (!m_connectionStrings.ContainsKey(connectionsName))
          throw new ApplicationException(string.Format("Database connection strings for {0} was not defined",connectionsName));


      // =============================================================
      // Make sure the database was specified in the connection string.
      // =============================================================
      if ( ! m_connectionStrings[connectionsName].ConnectionStrings.ContainsKey(databaseName) )
        throw new ApplicationException(string.Format("Connection string for {0} was not defined", databaseName));


      try
      {
        return new Microsoft.Practices.EnterpriseLibrary.Data.Sql.SqlDatabase(m_connectionStrings[connectionsName].ConnectionStrings[databaseName]);
      }
      catch (Exception ex)
      {
        Log.Error(string.Format("Failed to create database object for {0} : {1}", databaseName, ex.Message), "DatabaseFactory", "CreateDatabase");
        throw;
      }
      

    } // 

  } // class DatabaseFactory


} // namespace Viatronix.Enterprise.Data
