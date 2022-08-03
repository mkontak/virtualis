using System;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Validates the unlocking of a license.
  /// </summary>
  [TestClass]
  public class UnlockLicenseTests
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
    /// Tests a license unlock and then attempts to acquire a license.
    /// </summary>
    [TestMethod]
    public void UnlLic_TestLockUnlockAcquire()
    {
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 1, 120);

      string activeKey = string.Empty;
      Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Available);

      if (licenses.Length > 0)
      {
        activeKey = licenses[0].Item1;
      }
      else
      {
        Assert.Fail("No licenses were available.");
      }

      LicensingGateway.LockLicense(activeKey);
      Assert.AreEqual(LicensingGateway.LicenseStates.Locked, LicensingGateway.GetLicenseState(activeKey), "The license lock operation failed.");

      LicensingGateway.UnlockLicense(activeKey);
      Assert.AreEqual(LicensingGateway.LicenseStates.Available, LicensingGateway.GetLicenseState(activeKey), "The license unlock operation failed.");

      LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
    } // UnlLic_TestLockUnlockAcquire()

    #endregion

  } // class UnlockLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
