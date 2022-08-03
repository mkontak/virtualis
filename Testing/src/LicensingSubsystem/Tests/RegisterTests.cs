using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.Common;
using System.Data.SqlTypes;
using System.Data.SqlClient;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.Practices.EnterpriseLibrary.Common;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Performs validation on the registration.
  /// </summary>
  [TestClass]
  public class RegisterTests
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
    /// Tests the registration.
    /// </summary>
    [TestMethod]
    public void Reg_TestRegistration()
    {
      try
      {
        DateTimeOffset expDate = DateTimeOffset.Now;

        DateTimeOffset startDate = LicensingGateway.GetDatabaseDateTime();
        LicensingGateway.RegisterLicenseDatabase(expDate);
        DateTimeOffset endDate = LicensingGateway.GetDatabaseDateTime();

        DataSet ds = null;

        Database database = LicensingGateway.CreateDatabase();
        using (IDbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[Install]");
          ds = database.ExecuteDataSet(cmd);
        }

        if (ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
        {
          // Compare the expiration date times within one minute of accuracy.
          DateTimeOffset exp = (DateTimeOffset)(ds.Tables[0].Rows[0]["expDate"]);

          Assert.AreEqual(expDate, exp, "The expected expiration date doesn't match expectations.");

          // Compare the install date times within one minute of accuracy.
          DateTimeOffset inst = (DateTimeOffset)(ds.Tables[0].Rows[0]["date"]);

          // Make sure the two dates are within five seconds of each other.
          Assert.IsTrue(startDate.Subtract(new TimeSpan(0, 1, 0)) < inst && inst < endDate.AddMinutes(1), "The registration date does not match expectations.");
        }
      }
      finally
      {
        LicensingGateway.ClearRegistration();
      }
    } // Reg_TestRegistration()

    #endregion

  } // class RegisterTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
