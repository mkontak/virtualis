using System;
using System.Data;
using System.Data.Common;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Data.SqlTypes;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Validates the locking of a license.
  /// </summary>
  [TestClass]
  public class LockLicenseTests
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
    /// Tests a successful locking of a license.
    /// </summary>
    [TestMethod]
    public void LckLic_TestSuccess()
    {
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

      Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses();
      string activeKey = string.Empty;

      if (licenses.Length > 0)
      {
        activeKey = licenses[0].Item1;
      }
      else
      {
        Assert.Fail("No licenses could be found.");
      }

      LicensingGateway.LockLicense(activeKey);
      Assert.AreEqual(LicensingGateway.LicenseStates.Locked, LicensingGateway.GetLicenseState(activeKey));
    } // LckLic_TestSuccess()


    /// <summary>
    /// Tests a successful locking of a license.
    /// </summary>
    [TestMethod]
    public void LckLic_TestLockAndAcquireLicense()
    {
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 1, 120);

      Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses();
      string activeKey = string.Empty;

      if (licenses.Length > 0)
      {
        activeKey = licenses[0].Item1;
      }
      else
      {
        Assert.Fail("No licenses could be found.");
      }

      LicensingGateway.LockLicense(activeKey);
      Assert.AreEqual(LicensingGateway.LicenseStates.Locked, LicensingGateway.GetLicenseState(activeKey));

      try
      {
        string xml = LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
        Assert.Fail("The database did not emmit an error when trying to acquire a license when all licenses are locked.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901001, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // LckLic_TestLockAndAcquireLicense()

    #endregion

  } // class LockLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
