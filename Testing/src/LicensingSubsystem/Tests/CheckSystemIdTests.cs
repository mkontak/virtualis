using System;
using System.Data;
using System.Data.Sql;
using System.Data.SqlTypes;
using System.Data.SqlClient;
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
  /// Tests the check system id stored proc.
  /// </summary>
  [TestClass]
  public class CheckSystemIdTests
  {

    #region methods

    /// <summary>
    /// Performs initialization before each test is run.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();

      LicensingGateway.RegisterLicenseDatabase();
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();
    } // TestCleanup()


    /// <summary>
    /// Verifies the system id against the hardware.
    /// </summary>
    [TestMethod]
    public void ChkSysId_TestSuccess()
    {
      Assert.IsTrue(LicensingGateway.CheckSystemId(), "The system id did not match expectations.");
    } // ChkSysId_TestSuccess()


    /// <summary>
    /// Verifies the an invalid system id against the hardware.
    /// </summary>
    [TestMethod]
    public void ChkSysId_TestInvalidSystemId()
    {
      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("UPDATE [v3d].[Install] SET sysid = \'Bogus System Id\'");
        database.ExecuteNonQuery(cmd);
      }

      Assert.IsFalse(LicensingGateway.CheckSystemId(), "The system recognized the spoofed system id as valid!");
    } // ChkSysId_TestInvalidSystemId()

    #endregion

  } // class CheckSystemIdTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
