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
  /// Tests the database settings test.
  /// </summary>
  [TestClass]
  public class DatabaseSettingsValidationUnitTest
  {

    #region fields

    /// <summary>
    /// The database to connect to.
    /// </summary>
    private SqlDatabase m_database = new SqlDatabase(ConfigurationManager.AppSettings["connectionString"]);

    /// <summary>
    /// The database settings test.
    /// </summary>
    private DatabaseSettingsTest m_settingsTest = new DatabaseSettingsTest();

    #endregion

    #region methods

    /// <summary>
    /// Tests the database setting test with a valid subsystem, name and value.
    /// </summary>
    [TestMethod]
    public void DatabaseSettings_ValidSetting()
    {
      Assert.AreEqual(true, m_settingsTest.CheckSetting(m_database, new Tuple<string,string,string>("Security", "LicenseTtlUnits", "day")), "Setting has an incorrect value.");
    } // DatabaseSettings_ValidSetting()


    /// <summary>
    /// Tests the database setting test with a invalid SubSystem, but valid name and value.
    /// </summary>
    [TestMethod]
    public void DatabaseSettings_InvalidSubSystem()
    {
      Assert.AreEqual(false, m_settingsTest.CheckSetting(m_database, new Tuple<string,string,string>("Bogus", "LicenseTtlUnits", "day")), "Setting value was correct with valid name and value, but invalid SubSystem.");
    } // DatabaseSettings_InvalidSubSystem()


    /// <summary>
    /// Tests the database setting test with a valid SubSystem and value, but invalid name.
    /// </summary>
    [TestMethod]
    public void DatabaseSettings_InvalidName()
    {
      Assert.AreEqual(false, m_settingsTest.CheckSetting(m_database, new Tuple<string,string,string>("Security", "Bogus", "day")), "Setting value was correct with valid SubSystem and value, but incorrect name.");
    } // DatabaseSettings_InvalidName()


    /// <summary>
    /// Tests the database setting test with a valid SubSystem and name, but invalid value.
    /// </summary>
    [TestMethod]
    public void DatabaseSettings_InvalidValue()
    {
      Assert.AreEqual(false, m_settingsTest.CheckSetting(m_database, new Tuple<string,string,string>("Security", "LicenseTtlUnits", "Bogus")), "Setting value was correct with valid SubSystem and name, but incorrect value.");
    } // DatabaseSettings_InvalidValue()


    #endregion

  } // class DatabaseSettingsUnitTest
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.UnitTests
