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
  /// Checks to ensure versioned objects have the expected version.
  /// </summary>
  public class VersionedObjectsTest : IDatabaseValidationTest
  {

    /// <summary>
    /// Validates that the versions are valid.
    /// </summary>
    /// <param name="database">The database to connect to.</param>
    public bool Execute(SqlDatabase database)
    {
      bool result = true;
      Dictionary<string, string> nameVersionMap = (Dictionary<string, string>)ConfigurationManager.GetSection("versionedObjects");
      foreach (KeyValuePair<string, string> keyPair in nameVersionMap)
      {
        if (!CheckVersion(database, keyPair))
        {
          result = false;
        }
      }

      return result;
    } // Execute( database )


    /// <summary>
    /// Checks the version.
    /// </summary>
    /// <param name="database">The database to query.</param>
    /// <param name="keyPair">An object/version map.</param>
    /// <returns>True if the object is versioned correctly, false otherwise.</returns>
    public bool CheckVersion(SqlDatabase database, KeyValuePair<string, string> keyPair)
    {
      bool result = true;
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand getVersionCmd = database.GetSqlStringCommand(string.Format("SELECT System.v3d.fn_getVersion(\'{0}\')", keyPair.Key));
        string version = database.ExecuteScalar(getVersionCmd).ToString();

        if (version != keyPair.Value)
        {
          Console.WriteLine(string.Format("Database Supplied Version of {0} did not meet expectations.  Expected: {1}  Received: {2}", keyPair.Key, keyPair.Value, version));
          result = false;
        }
      }

      return result;
    } // CheckVersion( database, keyPair )

  } // class VersionedObjectsTest
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations
