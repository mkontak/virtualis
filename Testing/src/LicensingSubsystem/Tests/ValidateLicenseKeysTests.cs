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
  /// Validates the ValidateLicenseKeys stored procedure.
  /// </summary>
  [TestClass]
  public class ValidateLicenseKeysTests
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
    /// Tests validating valid license keys.
    /// </summary>
    [TestMethod]
    public void ValLicKey_TestValidKeys()
    {
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

      LicensingGateway.ValidateLicenseKeys();

      Assert.AreEqual(2, LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Available).Length, "The number of available licenses does not match expectations.");
    } // ValLicKey_TestValidKeys()


    /// <summary>
    /// Tests a spoofed key being in the license table and checks that the stored proc catches it.
    /// </summary>
    [TestMethod]
    public void ValLicKey_TestSpoofedKey()
    {
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 1, 120);
      var licenses = LicensingGateway.GetLicenses();

      string activeKey = string.Empty;
      if(licenses.Length > 0)
      {
        activeKey = licenses[0].Item1;
      }
      else
      {
        Assert.Fail("The number of licenses coming back does not match expectations.");
      }

      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format("UPDATE [v3d].[Licenses] SET [key] = \'GOOFBALL\' WHERE [key] = \'{0}\'", activeKey));
        database.ExecuteNonQuery(cmd);
      }

      LicensingGateway.ValidateLicenseKeys();

      Assert.AreEqual(1, LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Disabled).Length, "The number of disabled licenses does not match expectations.");
      Assert.AreEqual("GOOFBALL", LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Disabled)[0].Item1, "The key returned does not match expectations.");
    } // ValLicKey_TestSpoofedKey()

    #endregion

  } // class ValidateLicenseKeysTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
