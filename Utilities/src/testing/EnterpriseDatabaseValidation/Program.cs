// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

using Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations;

namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql
{
  /// <summary>
  /// Contains the main entry point into the program.
  /// </summary>
  internal class Program
  {

    #region fields

    /// <summary>
    /// The username to login to the database with.
    /// </summary>
    private static string m_username = string.Empty;

    /// <summary>
    /// The password to login to the database with.
    /// </summary>
    private static string m_password = string.Empty;

    /// <summary>
    /// The server to connect to.
    /// </summary>
    private static string m_server = string.Empty;

    #endregion

    #region methods

    /// <summary>
    /// The main entry point into the program.
    /// </summary>
    /// <param name="args">Arguments to take into acccount.</param>
    static void Main(string[] args)
    {
      HandleArguments(args);

      // @TODO: The checks for tables/sp/fn fails if an initial database is not provided, I don't know why.  master doesn't work.
      SqlDatabase database = new SqlDatabase(string.Format("user id={0};password={1};server={2};database=Storage", m_username, m_password, m_server));
      
      List<IDatabaseValidationTest> tests = (List<IDatabaseValidationTest>)ConfigurationManager.GetSection("validationTests");
      foreach (IDatabaseValidationTest test in tests)
      {
        Console.WriteLine(string.Format("Name: {0}\t\tStarting Execution.", test.GetType().Name));

        bool passed = test.Execute(database);

        Console.WriteLine(string.Format("Name: {0}\t\t{1}", test.GetType().Name, passed ? "Passed" : "Failed"));
      }
    } // Main( args )


    /// <summary>
    /// Handles the arguments provided to the program.
    /// </summary>
    /// <param name="args"></param>
    private static void HandleArguments(string[] args)
    {
      m_username = string.Empty;
      m_password = string.Empty;
      m_server   = string.Empty;

      for (int i = 0; i < args.Length; ++i)
      {
        string lower = args[i].ToLower();
        if (lower == "-username" || lower == "-login")
        {
          if (args.Length > i + 1)
          {
            m_username = args[i + 1];
          }
          else
          {
            //Logging.Log.Warning(SimulationTestManager.LOGSOURCE, "Failed to convert use argument, will user default value.", "Program", "HandleArguments");
          }
        }
        else if (lower == "-password")
        {
          if (args.Length > i + 1)
          {
            m_password = args[i + 1];
          }
          else
          {
            //Logging.Log.Warning(SimulationTestManager.LOGSOURCE, "Failed to convert use argument, will user default value.", "Program", "HandleArguments");
          }
        }
        else if (lower == "-server")
        {
          if (args.Length > i + 1)
          {
            m_server = args[i + 1];
          }
          else
          {
            //Logging.Log.Warning(SimulationTestManager.LOGSOURCE, "Failed to convert use argument, will user default value.", "Program", "HandleArguments");
          }
        }
      }
    } // HandleArguments( args )

    #endregion

  } // class Program
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql
