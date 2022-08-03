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
  /// Tests the acquireLicense stored proc.
  /// </summary>
  [TestClass]
  public sealed class AcquireLicenseTests
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
    /// Tests a successful license acquisition.
    /// </summary>
    [TestMethod]
    public void AcqLic_TestSuccess()
    {
      string licenseKey = LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);

      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format(
          "SELECT * FROM [v3d].[Licenses] WHERE [key]=\'{0}\' AND [state]=\'{1}\'", licenseKey, 
          (int)LicensingGateway.LicenseStates.InUse));

        DataSet ds = database.ExecuteDataSet(cmd);

        Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
        Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");
      }
    } // AcqLic_TestSuccess()


    /// <summary>
    /// Tests acquiring a license if the system is disabled.
    /// </summary>
    [TestMethod]
    public void AcqLic_TestSystemDisabled()
    {
      try
      {
        Database database = LicensingGateway.CreateDatabase();
        using (IDbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("UPDATE [v3d].[Install] SET date = \'2011-11-21 12:00:15.9630366 -05:00\', sysdate = \'2011-11-21 12:00:15.9630366 -05:00\', expdate = \'2011-11-21 12:00:15.9630366 -05:00\'");
          database.ExecuteNonQuery(cmd);
        }

        LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);

        Assert.Fail("Database did not emmit an error when trying to get a license from a system that is disabled.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900980, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AcqLic_TestSystemDisabled()


    /// <summary>
    /// Tests acquiring a license if the system is disabled.
    /// </summary>
    [TestMethod]
    public void AcqLic_TestNotInstalled()
    {
      try
      {
        LicensingGateway.ClearRegistration();
        LicensingGateway.ClearLicenses();

        LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);

        Assert.Fail("Database did not emmit an error when trying to get a license from a system that wasn't installed.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900981, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AcqLic_TestNotInstalled()


    /// <summary>
    /// Tests a system connecting that is not registered to get a license.
    /// </summary>
    [TestMethod]
    public void AcqLic_TestNotRegistered()
    {
      try
      {
        string licenseXml = LicensingGateway.AcquireLicense("abc123");
        Assert.Fail("Database did not emmit an error when trying to get a license from a system that wasn't registerred with the license server.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900982, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AcqLic_TestNotRegistered()


    /// <summary>
    /// Tests a system connecting when all licenses are in use.
    /// </summary>
    [TestMethod]
    public void AcqLic_TestInUse()
    {
      int attempt = 0;
      try
      {
        ++attempt;
        LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
        
        ++attempt;
        LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
        
        ++attempt;
        LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);
        Assert.Fail("Database did not emmit an error when trying to acquire more licenses than exist in the database.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901001, errorCode, "The expected errorcode returned didn't match expectations.");

        // Two licenses are added before the test begins, so verify that the failure occurred on the third and final attempt.
        Assert.AreEqual(3, attempt, "The expected number of acquisitions did not meet expectations.");
      }
    } // AcqLic_TestInUse()

    #endregion

  } // class AcquireLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
