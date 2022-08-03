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
  /// Validates the release context stored procedure.
  /// </summary>
  [TestClass]
  public class ReleaseContextTests
  {

    #region fields

    /// <summary>
    /// The service user's context id.
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
    /// Tests a valid situation of a user releasing his own context.
    /// </summary>
    [TestMethod]
    public void RelCxt_TestValidRelease()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        Guid contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Validate that the context did get added to the database.

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Contexts] WHERE [login]=\'{0}\' AND [id]=\'{1}\'", username, contextId.ToString()));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables returned did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of rows returned did not match expectations.");

          Assert.AreEqual(contextId, Guid.Parse(ds.Tables[0].Rows[0]["id"].ToString()), "The results returned did not match expectations.");
        }

        SecurityGateway.ReleaseContext(contextId, username);

        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Contexts] WHERE [login]=\'{0}\' AND [id]=\'{1}\'", username, contextId.ToString()));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables returned did not match expectations.");
          Assert.AreEqual(0, ds.Tables[0].Rows.Count, "The number of rows returned did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // RelCxt_TestValidRelease()


    /// <summary>
    /// Simulates an administrator releasing another user.
    /// </summary>
    [TestMethod]
    public void RelCxt_TestValidPermissions()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      string administrator = "admin01";
      Guid administratorContextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertUser(m_serviceContextId, administrator, "He is Administrator Man!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Administrator");

        administratorContextId = SecurityGateway.AcquireContext(administrator, "tester01$");

        SecurityGateway.ReleaseContext(administratorContextId, username);
      }
      finally
      {
        SecurityGateway.ReleaseContext(administratorContextId, administrator);

        SecurityGateway.DeleteUser(m_serviceContextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, administrator);
      }
    } // RelCxt_TestValidPermissions()


    /// <summary>
    /// Simulates a physician trying to release another physician's context.
    /// </summary>
    [TestMethod]
    public void RelCxt_TestInvalidPermissions()
    {
      string username = "user01";
      string username2 = "user02";
      Guid contextIdUser01 = Guid.Empty;
      Guid contextIdUser02 = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUser(m_serviceContextId, username2, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextIdUser01 = SecurityGateway.AcquireContext(username, "tester01$");
        contextIdUser02 = SecurityGateway.AcquireContext(username2, "tester01$");

        SecurityGateway.ReleaseContext(contextIdUser01, username2);
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901201, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextIdUser01, username);
        SecurityGateway.ReleaseContext(contextIdUser02, username2);

        SecurityGateway.DeleteUser(m_serviceContextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username2);
      }
    } // RelCxt_TestInvalidPermissions()


    /// <summary>
    /// Tests releasing a context and verifying the audit log caught it.
    /// </summary>
    [TestMethod]
    public void RelCxt_TestAuditLogEntry()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        DateTimeOffset start = LicensingGateway.GetDatabaseDateTime();

        SecurityGateway.ReleaseContext(contextId, username);

        DateTimeOffset end = LicensingGateway.GetDatabaseDateTime();

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [Auditing].[v3d].[UserHistory] WHERE [login]=\'{0}\' AND [action]=\'logout\' ORDER BY [date] DESC",
            username));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of rows did not match expectations.");

          DateTimeOffset date = DateTimeOffset.Parse(ds.Tables[0].Rows[0]["date"].ToString());

          Assert.IsTrue(start.Subtract(new TimeSpan(0, 0, 1)) < date && date < end.AddSeconds(1), "The date of the operation did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // RelCxt_TestAuditLogEntry()


    /// <summary>
    /// Tests trying to release a context that doesn't exist.
    /// </summary>
    [TestMethod]
    public void RelCxt_TestContextNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.ReleaseContext(Guid.NewGuid(), username);

        Assert.Fail("No exception was emmitted when trying to release a context that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // RelCxt_TestContextNotFound()


    /// <summary>
    /// Simulates a physician logging into multiple applications, then having maybe a crash or two, preventing the expected release. 
    /// He must seek the admin to force his context to release.
    /// </summary>
    [TestMethod]
    public void RelCxt_TestForceRelease()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.AcquireContext(username, "tester01$", null, 30, contextId);
        SecurityGateway.AcquireContext(username, "tester01$", null, 30, contextId);
        SecurityGateway.AcquireContext(username, "tester01$", null, 30, contextId);
        SecurityGateway.AcquireContext(username, "tester01$", null, 30, contextId);

        SecurityGateway.ReleaseContext(contextId, username, true);

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Contexts] WHERE [login] = \'{0}\'", username));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables returned did match expectations.");
          Assert.AreEqual(0, ds.Tables[0].Rows.Count, "The number of rows returned did match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // RelCxt_TestForceRelease()

    #endregion

  } // class ReleaseContextTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
