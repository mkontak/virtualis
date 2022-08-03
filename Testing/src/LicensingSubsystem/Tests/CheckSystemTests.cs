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
  /// Tests the validation of the licensing system.
  /// </summary>
  [TestClass]
  public class CheckSystemTests
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
    /// Tests a valid system.
    /// </summary>
    [TestMethod]
    public void ChkSys_TestSuccess()
    {
      Assert.IsTrue(LicensingGateway.CheckSystem(), "The system validation has failed.");
    } // ChkSys_TestSuccess()


    /// <summary>
    /// Tests an uninstalled system. 
    /// </summary>
    [TestMethod]
    public void ChkSys_TestUnRegistered()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();

      try
      {
        Assert.IsFalse(LicensingGateway.CheckSystem(), "The system check did not notice that the system is unregistered.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900981, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ChkSys_TestUnRegistered()


    /// <summary>
    /// Tests an expired system.
    /// </summary>
    [TestMethod]
    public void ChkSys_TestExpiration()
    {
      DateTimeOffset oldDate = DateTimeOffset.Now.Subtract(new TimeSpan(1, 1, 1, 1, 1));

      // Set the expiration date of the system to a previous date.

      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format("UPDATE [v3d].[Install] SET [expdate] = '{0}'", oldDate));
        database.ExecuteNonQuery(cmd);
      }

      try
      {
        Assert.IsFalse(LicensingGateway.CheckSystem(), "An error occurred while verifying the expired state of the database.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900980, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ChkSys_TestExpiration()


    /// <summary>
    /// Tests an invalid system date .
    /// </summary>
    [TestMethod]
    public void ChkSys_TestInvalidSystemDate()
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

      try
      {
        Assert.IsFalse(LicensingGateway.CheckSystem(), "The system check failed when giving an all go with a system that was altered.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900980, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ChkSys_TestInvalidSystemDate()


    /// <summary>
    /// Verifies the an invalid system id against the hardware.
    /// </summary>
    [TestMethod]
    public void ChkSys_TestInvalidSystemId()
    {
      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("UPDATE [v3d].[Install] SET sysid = \'Bogus System Id\'");
        database.ExecuteNonQuery(cmd);
      }

      try
      {
        Assert.IsFalse(LicensingGateway.CheckSystem(), "The system recognized the spoofed system id as valid!");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900980, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ChkSys_TestInvalidSystemId()

    #endregion

  } // class CheckSystemTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
