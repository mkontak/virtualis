using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Data.Common;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Performs tests verifying the system date.
  /// </summary>
  [TestClass]
  public class CheckSystemDateTests
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
    /// Performs cleanup after each test.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();
    } // TestCleanup()


    /// <summary>
    /// Checks a successful system state.
    /// </summary>
    [TestMethod]
    public void ChkSysDate_TestSuccess()
    {
      Assert.IsTrue(LicensingGateway.CheckSystemDate(), "The system date check failed when giving an all go test with a valid system setup.");
    } // ChkSysDate_TestSuccess()


    /// <summary>
    /// Checks a successful system state.
    /// </summary>
    [TestMethod]
    public void ChkSysDate_TestInvalidSystem()
    {
      // Update the time in the database to tomorrow and then try to authenticate.
      // It should notice that the dates were manipulated.
      DateTimeOffset futureDate = DateTimeOffset.Now.Add(new TimeSpan(5, 1, 1, 1));

      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format("UPDATE [v3d].[Install] SET sysdate = '{0}'", futureDate.ToString("o")));
        database.ExecuteNonQuery(cmd);
      }

      Assert.IsFalse(LicensingGateway.CheckSystemDate(), "The system date check failed when giving an all go with a system that was altered.");
    } // ChkSysDate_TestInvalidSystem()

    #endregion

  } // class CheckSystemDateTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
