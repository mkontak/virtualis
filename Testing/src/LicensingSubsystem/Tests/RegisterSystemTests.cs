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
  /// Validsates the register system stored proc.
  /// </summary>
  [TestClass]
  public class RegisterSystemTests
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
    /// Validates registering a system and then attempting to acquire a license.
    /// </summary>
    [TestMethod]
    public void RegSys_TestRegisterSystem()
    {
      // Attempt to register the system.
      LicensingGateway.RegisterSystem(SYSTEM_ID, "(local)\\V3DENT", "127.0.0.1", DateTimeOffset.Now);

      // Attempt to acquire a license.
      string xml = LicensingGateway.AcquireLicense(SYSTEM_ID);
    } // RegSys_TestRegisterSystem()


    /// <summary>
    /// Performs a double registration of a server and verifies that it fails.
    /// </summary>
    [TestMethod]
    public void RegSys_TestDoubleRegister()
    {
      int attempts = 0;

      try
      {
         ++attempts;
        LicensingGateway.RegisterSystem(SYSTEM_ID, "(local)\\V3DENT", "127.0.0.1", DateTimeOffset.Now);

        ++attempts;
        LicensingGateway.RegisterSystem(SYSTEM_ID, "(local)\\V3DENT", "127.0.0.1", DateTimeOffset.Now);
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901003, errorCode, "The expected errorcode returned didn't match expectations.");
        Assert.AreEqual(2, attempts, "The expected number of attempts did not match expectations.");
      }
    } // RegSys_TestDoubleRegister()

    #endregion

  } // class RegisterSystemTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
