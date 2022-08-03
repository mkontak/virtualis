using System;
using System.Xml;
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
  /// Unit tests that verify the check the system's expiration date is working.
  /// </summary>
  [TestClass]
  public class CheckExpirationDateTests
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
    /// Performs cleanup after each test.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();
    } // TestCleanup()


    /// <summary>
    /// Tests a successful execution with a system that is registered at the time of the test.
    /// </summary>
    [TestMethod]
    public void ChkExp_TestSuccess()
    {
      Assert.IsTrue(LicensingGateway.CheckExpirationDate(), "An error occurred while verifying the state of the database.");
    } // ChkSysExp_TestSuccess()


    /// <summary>
    /// Tests an expired system.
    /// </summary>
    [TestMethod]
    public void ChkExp_TestExpiration()
    {
      DateTimeOffset oldDate = DateTimeOffset.Now.Subtract(new TimeSpan(1, 1, 1, 1, 1));

      // Set the expiration date of the system to a previous date.

      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format("UPDATE [v3d].[Install] SET [expdate] = '{0}'", oldDate));
        database.ExecuteNonQuery(cmd);
      }

      Assert.IsFalse(LicensingGateway.CheckExpirationDate(), "An error occurred while verifying the expired state of the database.");
    } // ChkSysExp_TestExpiration()

    #endregion

  } // class CheckExpirationDateTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
