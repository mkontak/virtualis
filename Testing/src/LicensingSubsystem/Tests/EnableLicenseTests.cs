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
  /// Validates the enable license stored procedure.
  /// </summary>
  [TestClass]
  public class EnableLicenseTests
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
    /// Tests a successful license disable/enable.
    /// </summary>
    [TestMethod]
    public void EnbLic_TestSuccess()
    {
      Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Available);

      string activeKey = string.Empty;
      if (licenses.Length > 0)
      {
        activeKey = licenses[0].Item1;
      }

      // Disable the license and verify the change.
      LicensingGateway.DisableLicense(activeKey);
      Assert.AreEqual(LicensingGateway.LicenseStates.Disabled, LicensingGateway.GetLicenseState(activeKey), "The disable operation did not produce the expected results.");

      // Enable the license and verify the change.
      LicensingGateway.EnableLicense(activeKey);
      Assert.AreEqual(LicensingGateway.LicenseStates.Available, LicensingGateway.GetLicenseState(activeKey), "The enable operation did not produce the expected results.");
    } // EnbLic_TestSuccess()

    #endregion

  } // class EnableLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
