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
  /// Checks to ensure that the Database settings have the accepted values.
  /// </summary>
  public class DatabaseSettingsTest : IDatabaseValidationTest
  {

    /// <summary>
    /// Validates the settings in the database.
    /// </summary>
    /// <param name="database">The database to connect to.</param>
    public bool Execute(SqlDatabase database)
    {
      bool result = true;
      List<Tuple<string, string, string>> entries = (List<Tuple<string, string, string>>)ConfigurationManager.GetSection("dbSettings");

      foreach (Tuple<string, string, string> entry in entries)
      {
        if (!CheckSetting(database, entry))
        {
          result = false;
        }
      }

      return result;
    } // Execute( database )


    /// <summary>
    /// Checks the setting with the value.
    /// </summary>
    /// <param name="database">The database to query.</param>
    /// <param name="entry">A tuple containing the subsystem, name and expected value.</param>
    /// <returns>True if the value matches the expected value.</returns>
    public bool CheckSetting(SqlDatabase database, Tuple<string, string, string> entry)
    {
      bool result = true;
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand getVersionCmd = database.GetSqlStringCommand(string.Format("SELECT System.v3d.fn_getSetting(N\'{0}\', N\'{1}\')", entry.Item1, entry.Item2));
        string value = database.ExecuteScalar(getVersionCmd).ToString();

        if (value != entry.Item3)
        {
          Console.WriteLine(string.Format("Database Supplied Setting Value of Sub System: {0} Name: {1} did not meet expectations.  Expected: {2}  Received: {3}", entry.Item1, entry.Item2, entry.Item3, value));
          result = false;
        }
      }

      return result;
    } // CheckSetting( database, entry )

  } // class DatabaseSettingsTest
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations
