using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Collections.Generic;
using System.Data.Common;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Tests the get system id.
  /// </summary>
  [TestClass]
  public class GetSystemIdTests
  {

    #region methods

    /// <summary>
    /// Performs initialization before each test.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();

      LicensingGateway.RegisterLicenseDatabase();
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);
    } // TestInitialize()


    /// <summary>
    /// Cleans up after each test.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();
    } // TestCleanup()


    /// <summary>
    /// Validates that a call to getsystemid returns the correct result.
    /// </summary>
    [TestMethod]
    public void GetSysId_TestSuccess()
    {
      DataSet ds = null;

      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[Install]");
        ds = database.ExecuteDataSet(cmd);
      }

      if (ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
      {
        Assert.AreEqual(ds.Tables[0].Rows[0]["sysId"].ToString(), LicensingGateway.GetSystemId(), "The systemid coming from the hardware did not match what is found in the install table.");
      }
      else
      {
        Assert.Fail("The system has not been installed.");
      }
    } // GetSysId_TestSuccess()

    #endregion

  } // class GetSystemIdTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
