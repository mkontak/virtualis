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
  /// Tests the unregister system stored proc.
  /// </summary>
  [TestClass]
  public class UnregisterSystemTests
  {

    #region constants

    /// <summary>
    /// The system id.
    /// </summary>
    private const string SYSTEM_ID = "GOOFBALL";

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before each test.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.UnregisterSystem(SYSTEM_ID);
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
      LicensingGateway.UnregisterSystem(SYSTEM_ID);
      LicensingGateway.ClearLicenses();
    } // TestCleanup()



    /// <summary>
    /// Tests unregistration of a system after acquiring a license.
    /// </summary>
    [TestMethod]
    public void Urgsys_TestUnregistration()
    {
      // Register the system.
      LicensingGateway.RegisterSystem(SYSTEM_ID, "(local)\\V3DENT", "127.0.0.1", DateTimeOffset.Now);

      string licenseKey = LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);

      // Unregister the system.
      LicensingGateway.UnregisterSystem(SYSTEM_ID);
     
      // Acquire a license.
      LicensingGateway.ReleaseLicense(licenseKey);

      try
      {
        // This system id was unregistered, should return an error.
        LicensingGateway.AcquireLicense(SYSTEM_ID);
        Assert.Fail("No expection was thrown when attempting to acquire a license on a machine that is not registerred.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900982, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // Urgsys_TestUnregistration()

    #endregion

  } // class UnregisterSystemTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
