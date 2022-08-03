// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: IDatabaseGateway.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)



using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Data.Common;
using System.Xml;
using System.Threading;
using System.Text.RegularExpressions;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Viatronix.Enterprise.Logging;
using Viatronix;

namespace Viatronix.Enterprise.Data
{

  /// <summary>
  /// 
  /// Implements a base database gateway that peformed the 4 basic database tasks.
  /// 
  ///   GET       - Gets data from the database
  ///   INSERT    - Inserts data into the database
  ///   DELETE    - Delete data from the database
  ///   UPDATE    - Updates data in the database
  ///   
  /// </summary>
  public class DatabaseGateway
  {


    #region fields

    /// <summary>
    /// xml data column size
    /// </summary>
    private const int XML_SIZE = 20000;

    #endregion



    #region methods

    /// <summary>
    /// Executes the appropriate sp_getXXXXX stored procedure in the database specified. The database name must appear in the connections file in the 
    /// Viatronix configuration directory. 
    /// </summary>
    /// <param name="databaseName">Database Name</param>
    /// <param name="context">Context id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="query">Query xml</param>
    /// <param name="source">Connection Strings name</param>
    /// <returns></returns>
    public static string Get(string databaseName, string context, string resource, string query, string source = "default" )
    {
 

      try
      {
        // Create the database
        Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(databaseName, source);

        using (DbConnection connection = db.CreateConnection())
        {
          string storeProcedure = string.Format("[v3d].[sp_get{0}]", resource);

          DbCommand command = db.GetStoredProcCommand(storeProcedure);

          db.AddInParameter(command, "ID", DbType.String, context);           // Context ID

          if (!string.IsNullOrEmpty(query))
            db.AddInParameter(command, "QUERY", DbType.String, query);          // QUERY Xml

          db.AddOutParameter(command, "RESULTS", DbType.Xml, XML_SIZE);       // RESULTS Xml returned

          // Execute
          db.ExecuteStoredProcedure(command);

          // Get the results
          string results = Convert.ToString(db.GetParameterValue(command, "RESULTS"));

          return results;

        }
      }
      catch(ViatronixException)
      {
        // Exception was already figured out.
        throw;
      }
      catch (DbException ex)
      {
        throw ExceptionFactory.Create(ex);
      }
      catch (Exception ex)
      {
        throw ExceptionFactory.Create(ex);
      }

    } // Get(string databaseName, string context, string resource, string query)

    /// <summary>
    /// Executes the appropriate sp_insertXXXXX stored procedure in the database specified. The database name must appear in the connections file in the 
    /// Viatronix configuration directory. 
    /// </summary>
    /// <param name="databaseName">Database Name</param>
    /// <param name="context">Context id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="record">Record to delete</param>
    /// <param name="source">Connection Strings name</param>
    /// <returns></returns>
    public static void Insert(string databaseName, string context, string resource, string record, string source = "default" )
    {

      try
      {
        // Create the database
        Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(databaseName, source);

        using (DbConnection connection = db.CreateConnection())
        {
          string storeProcedure = string.Format("[v3d].[sp_insert{0}]", resource);

          DbCommand command = db.GetStoredProcCommand(storeProcedure);

          db.AddInParameter(command, "ID", DbType.String, context);       // Context ID
          db.AddInParameter(command, "RECORD", DbType.Xml, record);       // Record Xml

          // Execute
          db.ExecuteStoredProcedure(command);


        }
      }
      catch (DbException ex)
      {
        throw ExceptionFactory.Create(ex);
      }
      catch (Exception ex)
      {
        throw ExceptionFactory.Create(ex);
      }

    } // Insert(string databaseName, string context, string resource, string query)

    /// <summary>
    /// Executes the appropriate sp_UpdateXXXXX stored procedure in the database specified. The database name must appear in the connections file in the 
    /// Viatronix configuration directory. 
    /// </summary>
    /// <param name="databaseName">Database Name</param>
    /// <param name="context">Context id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="record">Record to delete</param>
    /// <param name="source">Connection Strings name</param>
    /// <returns></returns>
    public static string Update(string databaseName, string context, string resource, string record, string source = "default" )
    {

      try
      {
        // Create the database
        Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(databaseName, source);

        using (DbConnection connection = db.CreateConnection())
        {
          string storeProcedure = string.Format("[v3d].[sp_update{0}]", resource);

          DbCommand command = db.GetStoredProcCommand(storeProcedure);

          db.AddInParameter(command, "ID", DbType.String, context);       // Context ID
          db.AddInParameter(command, "RECORD", DbType.Xml, record);       // Record Xml


          // Execute
          db.ExecuteStoredProcedure(command);

          // Get the results
          string results = Convert.ToString(db.GetParameterValue(command, "RECORD"));

          return results;

        }
      }
      catch (Exception ex)
      {
        throw ExceptionFactory.Create(ex);
      }
    } // Update(string databaseName, string resource, string query)

    /// <summary>
    /// Executes the appropriate sp_deleteXXXXX stored procedure in the database specified. The database name must appear in the connections file in the 
    /// Viatronix configuration directory. 
    /// </summary>
    /// <param name="databaseName">Database Name</param>
    /// <param name="context">Context id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="record">Record to delete</param>
    /// <param name="source">Connection Strings name</param>
    /// <returns></returns>
    public static void Delete(string databaseName, string context, string resource, string record, string source = "default" )
    {

      try
      {

        // Create the database
        Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(databaseName, source);

        using (DbConnection connection = db.CreateConnection())
        {
          string storeProcedure = string.Format("[v3d].[sp_delete{0}]", resource);

          DbCommand command = db.GetStoredProcCommand(storeProcedure);

          db.AddInParameter(command, "ID", DbType.String, context);             // Context ID
          db.AddInParameter(command, "RECORD", DbType.String, record);          // Record Xml

          // Execute
          db.ExecuteStoredProcedure(command);

        }
      }
      catch (Exception ex)
      {
        throw ExceptionFactory.Create(ex);
      }

    } // Delete(string databaseName, string context, string resource, string record)

    #endregion

  } // interface DatabaseGateway



} // namespace Viatronix.Enterprise.Data
