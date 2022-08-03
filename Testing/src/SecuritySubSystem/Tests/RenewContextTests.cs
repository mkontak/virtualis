using System;
using System.Data;
using System.Data.Sql;
using System.Data.Common;
using System.Data.SqlClient;
using System.Data.SqlTypes;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.SecuritySubSystem.Tests
{
  /// <summary>
  /// Validates the renew context stored proc.
  /// </summary>
  [TestClass]
  public class RenewContextTests
  {

    #region fields

    /// <summary>
    /// The service context id.
    /// </summary>
    private Guid m_serviceContextId = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before each test begins.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();

      m_serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);
      
      LicensingGateway.RegisterLicenseDatabase();
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after each test ends.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      LicensingGateway.ClearLicenses();
      LicensingGateway.ClearRegistration();

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a valid renew context, and check that the expiration has indeed moved up.
    /// </summary>
    [TestMethod]
    public void RnwCxt_TestValidRenew()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$", null, 60);

        DateTimeOffset start = LicensingGateway.GetDatabaseDateTime().AddMinutes(30);

        SecurityGateway.RenewContext(contextId, 30);

        DateTimeOffset end = LicensingGateway.GetDatabaseDateTime().AddMinutes(30);

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Contexts] WHERE [login]=\'{0}\'",
            username));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables does not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows does not match expectations.");
          
          DateTimeOffset date = (DateTimeOffset)ds.Tables[0].Rows[0]["exp"];
          Assert.IsTrue(start <= date && date <= end, "The expiration date does not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // RnwCxt_TestValidRenew()


    /// <summary>
    /// Tests renewing a context that doesn't exist.
    /// </summary>
    [TestMethod]
    public void RnwCxt_TestContextNotFound()
    {
      try
      {
        SecurityGateway.RenewContext(Guid.Empty);
        Assert.Fail("No error was emmitted when trying to renew a context that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // RnwCxt_TestContextNotFound()


    /// <summary>
    /// Tests renewing an expired user.
    /// </summary>
    [TestMethod]
    public void RnwCxt_TestRenewExpiredUser()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "UPDATE [v3d].[Users] SET [exp]=\'{0}\' WHERE [login]=\'{1}\'",
            DateTimeOffset.Now.Subtract(new TimeSpan(1, 0, 0)), username));

          database.ExecuteNonQuery(cmd);
        }

        SecurityGateway.RenewContext(contextId);
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // RnwCxt_TestRenewExpiredUser()


    /// <summary>
    /// Tests renewing an expired context.
    /// </summary>
    [TestMethod]
    public void RnwCxt_TestRenewExpiredContext()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$", null, -15);

        SecurityGateway.RenewContext(contextId);
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // RnwCxt_TestRenewExpiredContext()

    #endregion

  } // class RenewContextTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
