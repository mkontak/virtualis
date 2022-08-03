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
using System.Text;
using System.Collections.Generic;
using System.Linq;

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

using Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations;

namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.UnitTests
{
  /// <summary>
  /// Tests the VersionedTest.
  /// </summary>
  [TestClass]
  public class DatabaseVersionedValidationUnitTest
  {

    #region fields

    /// <summary>
    /// The database.
    /// </summary>
    SqlDatabase m_database = new SqlDatabase(ConfigurationManager.AppSettings["connectionString"]);

    /// <summary>
    /// The versioning test.
    /// </summary>
    VersionedObjectsTest m_versionTest = new VersionedObjectsTest();

    #endregion

    #region methods

    /// <summary>
    /// Tests the versioned object validation test with a valid database and correct version.
    /// </summary>
    [TestMethod]
    public void VersionedObject_ValidDatabase()
    {
      Assert.AreEqual(true, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("Security", "4.0")), "Valid database did not have the correct version.");
    } // VersionedObject_ValidDatabase()


    /// <summary>
    /// Tests the versioned object validation test with a valid database and incorrect version.
    /// </summary>
    [TestMethod]
    public void VersionedObject_InvalidDatabaseVersion()
    {
      Assert.AreEqual(false, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("Security", "3.0")), "Valid database has incorrect version.");
    } // VersionedObject_InvalidDatabaseVersion()


    /// <summary>
    /// Tests the versioned object validation test with an invalid database.
    /// </summary>
    [TestMethod]
    public void VersionedObject_InvalidDatatabase()
    {
      Assert.AreEqual(false, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("SecurityBogus", "4.0")), "Invalid database has incorrect version.");
    } // VersionedObject_InvalidDatatabase()


    /// <summary>
    /// Tests the versioned object validation test with a valid table and version.
    /// </summary>
    [TestMethod]
    public void VersionedObject_ValidTable()
    {
      Assert.AreEqual(true, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("Users", "2.0")), "Valid table has incorrect version");
    } // VersionedObject_ValidTable()


    /// <summary>
    /// Tests the versioned object validation test with a valid table, but with an invalid version.
    /// </summary>
    [TestMethod]
    public void VersionedObject_InvalidTableVersion()
    {
      Assert.AreEqual(false, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("Users", "3.0")), "Valid table has incorrect version.");
    } // VersionedObject_InvalidTableVersion()


    /// <summary>
    /// Tests the versioned object validation test with an invalid table.
    /// </summary>
    [TestMethod]
    public void VersionedObject_InvalidTable()
    {
      Assert.AreEqual(false, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("UsersInvalid", "2.0")), "Invalid table has correct version for a table.");
    } // VersionedObject_InvalidTable()


    /// <summary>
    /// Tests the versioned object validation test with a valid dynamic table.
    /// </summary>
    [TestMethod]
    public void VersionedObject_ValidDynamicTable()
    {
      Assert.AreEqual(true, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("dicomInfo", "3.0")), "Valid dynamic table has incorrect version");
    } // VersionedObject_ValidDynamicTable()


    /// <summary>
    /// Tests the versioned object validation test with a valid dynamic table and an invalid version.
    /// </summary>
    [TestMethod]
    public void VersionedObject_InvalidDynamicTableVersion()
    {
      Assert.AreEqual(false, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("dicomInfo", "5.0")), "Valid dynamic table has incorrect version.");
    } // VersionedObject_InvalidDynamicTableVersion()


    /// <summary>
    /// Tests the versioned object validation test with an invalid dynamic table.
    /// </summary>
    [TestMethod]
    public void VersionedObject_InvalidDynamicTable()
    {
      Assert.AreEqual(false, m_versionTest.CheckVersion(m_database, new KeyValuePair<string, string>("dicomInfoInvalid", "3.0")), "Invalid dynamic table has correct version for a dynamic table.");
    } // VersionedObject_InvalidDynamicTable()

    #endregion

  } // class DatabaseVersionedUnitTest
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.UnitTests
