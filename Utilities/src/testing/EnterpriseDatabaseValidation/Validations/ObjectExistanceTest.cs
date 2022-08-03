// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Data;
using System.Data.Common;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations
{
  /// <summary>
  /// Test to ensure all database objects exist.
  /// </summary>
  public class ObjectExistanceTest : IDatabaseValidationTest
  {

    /// <summary>
    /// Validates that all the objects provided in the objects config section exist.
    /// </summary>
    public bool Execute(SqlDatabasee database)
    {
      bool result = true;
      List<string> databases = (List<string>)ConfigurationManager.GetSection("databases");
      List<string> objects = (List<string>)ConfigurationManager.GetSection("objects");

      foreach (string db in databases)
      {
        if (!DatabaseExists(database, db))
        {
          result = false;
        }
      }

      foreach (string obj in objects)
      {
        if (!DatabaseObjectExists(database, obj))
        {
          result = false;
        }
      }

      return result;
    } // Execute( database )


    /// <summary>
    /// Checks to see if the database exists.
    /// </summary>
    /// <param name="database">The database to connect to.</param>
    /// <param name="obj">The object to check for existance.</param>
    /// <returns>True if the object exists, false otherwise.</returns>
    public bool DatabaseExists(SqlDatabase database, string dbName)
    {
      bool result = true;
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand existCmd = database.GetSqlStringCommand(string.Format("SELECT COUNT(*) FROM sys.sysdatabases WHERE name=\'{0}\'", dbName));
        int count = Convert.ToInt32(database.ExecuteScalar(existCmd));

        if (count < 1)
        {
          Console.WriteLine(string.Format("Database was not found.  Name: {0}", dbName));
          result = false;
        }
      }

      return result;
    } // DatabaseExists( database, dbName )


    /// <summary>
    /// Checks to see if the database object exists.
    /// </summary>
    /// <param name="database">The database to connect to.</param>
    /// <param name="obj">The object to check for existance.</param>
    /// <returns>True if the object exists, false otherwise.</returns>
    public bool DatabaseObjectExists(SqlDatabase database, string obj)
    {
      bool result = true;

      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand existCmd = database.GetSqlStringCommand(string.Format("SELECT COUNT(*) FROM sys.objects WHERE object_id = OBJECT_ID(\'{0}\')", obj));
        int count = Convert.ToInt32(database.ExecuteScalar(existCmd));

        if (count < 1)
        {
          Console.WriteLine(string.Format("Database object was not found.  Name: {0}", obj));
          result = false;
        }
      }

      return result;
    } // DatabaseObjectExists( database, obj )

  } // class ObjectExistanceTest
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations
