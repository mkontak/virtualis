using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Data.SqlTypes;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Tests the release license stored procedure.
  /// </summary>
  [TestClass]
  public sealed class ReleaseLicenseTests
  {

    #region methods

    /// <summary>
    /// Performs initialization before each test begins.
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
    /// Performs cleanup after each test ends.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();
    } // TestCleanup()


    /// <summary>
    /// Tests a successful release of a license.
    /// </summary>
    [TestMethod]
    public void RelLic_TestSuccess()
    {
      // Acquire a license and then release it.
      string licenseKey = LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
      LicensingGateway.ReleaseLicense(licenseKey);
    } // RelLic_TestSuccess()


    /// <summary>
    /// Tests a release of a key that doesn't exist.
    /// </summary>
    [TestMethod]
    public void RelLic_TestInvalidKey()
    {
      try
      {
        LicensingGateway.ReleaseLicense("");
        Assert.Fail("No exception was emitted from the database when releasing an invalid license key.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // RelLic_TestInvalidKey()


    /// <summary>
    /// Tests the release with a missing key.
    /// </summary>
    public void RelLic_TestMissingKey()
    {
      try
      {
        LicensingGateway.ReleaseLicense(null);
        Assert.Fail("No exception was emitted from the database when releasing a missing license key.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // RelLic_TestMissingKey()

    #endregion

  } // class ReleaseLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
