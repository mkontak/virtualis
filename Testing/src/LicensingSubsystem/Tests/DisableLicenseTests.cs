using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.Common;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Collections.Generic;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Tests involving disabling a license.
  /// </summary>
  [TestClass]
  public class DisableLicenseTests
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
    /// Attempts to disable a license and verifies that it was disabled.
    /// </summary>
    [TestMethod]
    public void DisLic_TestSuccess()
    {
      Tuple<string, LicensingGateway.LicenseStates>[] availableLicenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Available);
      string activeKey = string.Empty;

      if (availableLicenses.Length > 0)
      {
        activeKey = availableLicenses[0].Item1;
        LicensingGateway.DisableLicense(activeKey);
      }
      else
      {
        Assert.Fail("No licenses are available.");
      }

      Assert.AreEqual(LicensingGateway.LicenseStates.Disabled, LicensingGateway.GetLicenseState(activeKey), "");
    } // DisLic_TestSuccess()


    /// <summary>
    /// Tests an attempt to disable a license that is in use.
    /// </summary>
    [TestMethod]
    public void DisLic_TestInUseDisable()
    {
      string licenseKey = LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);

      LicensingGateway.DisableLicense(licenseKey);

      LicensingGateway.LicenseStates? state = LicensingGateway.GetLicenseState(licenseKey);

      Assert.AreNotEqual(LicensingGateway.LicenseStates.Disabled, state, "The in-use license was incorrectly marked as disabled.");
      Assert.AreNotEqual(null, state, "The license could not be found.");
    } // DisLic_TestInUseDisable()


    /// <summary>
    /// Disables all licenses and tries to acquire a license.
    /// </summary>
    [TestMethod]
    public void DisLic_TestNoLicenses()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();

      LicensingGateway.RegisterLicenseDatabase();
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 1, 120);

      Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Available);

      string activeKey = string.Empty;
      if (licenses.Length > 0)
      {
        activeKey = licenses[0].Item1;
      }
      else
      {
        Assert.Fail("No licenses are available.");
      }

      LicensingGateway.DisableLicense(activeKey);

      LicensingGateway.LicenseStates? state = LicensingGateway.GetLicenseState(activeKey);
      Assert.AreNotEqual(null, state, "The license could not be found after making the change.");
      Assert.AreEqual(LicensingGateway.LicenseStates.Disabled, state, "Disabling this license failed.");

      try
      {
        LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901001, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // DisLic_TestNoLicenses()

    #endregion

  } // class DisableLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
