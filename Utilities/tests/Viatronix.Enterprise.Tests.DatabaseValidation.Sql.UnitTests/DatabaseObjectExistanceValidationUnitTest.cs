// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Configuration;
using System.Text;
using System.Collections.Generic;
using System.Linq;

using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations;

namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.UnitTests
{
  /// <summary>
  /// Tests the object existance validation test.
  /// </summary>
  [TestClass]
  public class ObjectExistanceValidationUnitTest
  {

    #region fields

    /// <summary>
    /// The database to connect to.
    /// </summary>
    SqlDatabase m_database = new SqlDatabase(ConfigurationManager.AppSettings["connectionString"]);

    /// <summary>
    /// The existance test.
    /// </summary>
    ObjectExistanceTest m_existanceTest = new ObjectExistanceTest();

    #endregion

    #region methods

    /// <summary>
    /// Checks a valid database and checks to see if it exists.
    /// </summary>
    [TestMethod]
    public void DatabaseExists_Valid()
    {
      Assert.AreEqual(true, m_existanceTest.DatabaseExists(m_database, "Storage"), "Valid database was not found in the server.");
    } // DatabaseExists_Valid()


    /// <summary>
    /// Checks that an invalid database generates an error.
    /// </summary>
    [TestMethod]
    public void DatabaseExists_Invalid()
    {
      Assert.AreEqual(false, m_existanceTest.DatabaseExists(m_database, "UNKNOWN"), "Invalid database was found in the server.");
    } // DatabaseExists_Invalid()


    /// <summary>
    /// Checks a valid table and checks to see if it exists.
    /// </summary>
    [TestMethod]
    public void DatabaseTableExists_Valid()
    {
      Assert.AreEqual(true, m_existanceTest.DatabaseObjectExists(m_database, "Storage.v3d.Series"), "Valid table was not found in the server.");
    } // DatabaseTableExists_Valid()


    /// <summary>
    /// Checks that an invalid table generates an error.
    /// </summary>
    [TestMethod]
    public void DatabaseTableExists_Invalid()
    {
      Assert.AreEqual(false, m_existanceTest.DatabaseObjectExists(m_database, "Storage.v3d.Bogus"), "Invalid table was found in the server.");
    } // DatabaseTableExists_Invalid()


    /// <summary>
    /// Checks a valid stored procedure and checks to see if it exists.
    /// </summary>
    [TestMethod]
    public void DatabaseStoredProcedureExists_Valid()
    {
      Assert.AreEqual(true, m_existanceTest.DatabaseObjectExists(m_database, "Storage.v3d.sp_addSeries"), "Valid stored procedure was not found in the server.");
    } // DatabaseStoredProcedureExists_Valid()


    /// <summary>
    /// Checks that an invalid stored procedure generates an error.
    /// </summary>
    [TestMethod]
    public void DatabaseStoredProcedureExists_Invalid()
    {
      Assert.AreEqual(false, m_existanceTest.DatabaseObjectExists(m_database, "Storage.v3d.sp_bogus"), "Invalid stored procedure was found in the server.");
    } // DatabaseStoredProcedureExists_Invalid()


    /// <summary>
    /// Checks a valid function and checks to see if it exists.
    /// </summary>
    [TestMethod]
    public void DatabaseFunctionExists_Valid()
    {
      Assert.AreEqual(true, m_existanceTest.DatabaseObjectExists(m_database, "Storage.v3d.fn_getSetting"), "Valid function was not found in the server.");
    } // DatabaseFunctionExists_Invalid()


    /// <summary>
    /// Checks that an invalid function generates an error.
    /// </summary>
    [TestMethod]
    public void DatabaseFunctionExists_Invalid()
    {
      Assert.AreEqual(false, m_existanceTest.DatabaseObjectExists(m_database, "Storage.v3d.fn_bogus"), "Invalid function was found in the server.");
    } // DatabaseFunctionExists_Invalid()

    #endregion

  } // class ObjectExistanceTest
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.UnitTests
