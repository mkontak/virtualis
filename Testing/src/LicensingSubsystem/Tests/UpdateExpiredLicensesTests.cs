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
  /// Validates the update expired licenses.
  /// </summary>
  [TestClass]
  public class UpdateExpiredLicensesTests
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
    /// Tests the check for expired licenses stored proc.
    /// </summary>
    [TestMethod]
    public void UpdExpLic_TestExpiredLicenses()
    {
      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format("UPDATE [v3d].[Licenses] SET [v3d].[Licenses].[exp] = \'{0}\'", DateTime.Now.Subtract(new TimeSpan(1, 0, 0, 0))));
        database.ExecuteNonQuery(cmd);
      }

      LicensingGateway.UpdateExpiredLicenses();

      Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Expired);

      Assert.AreEqual(2, licenses.Length, "The number of expired licenses did not meet expectations.");
    } // UpdExpLic_TestExpiredLicenses()

    #endregion

  } // class UpdateExpiredLicensesTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
