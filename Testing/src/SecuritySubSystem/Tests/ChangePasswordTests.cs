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
  /// Validates the changePassword stored proc.
  /// </summary>
  [TestClass]
  public class ChangePasswordTests
  {

    #region fields

    /// <summary>
    /// The service context id.
    /// </summary>
    private Guid m_serviceContextId = Guid.Empty;

    /// <summary>
    /// The service username.
    /// </summary>
    private string m_serviceUserame = "service";

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
    /// Tests a valid change by an administrator.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestValidChangeByAdminUser()
    {
      string username = "user01";
      string administrator = "adminuser01";
      Guid contextId = Guid.Empty;
      Guid adminContextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUser(m_serviceContextId, administrator, "He is Administratoror!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Administrator");

        adminContextId = SecurityGateway.AcquireContext(administrator, "tester01$");

        SecurityGateway.ChangePassword(adminContextId, username, "tester01$", "tester55$");

        contextId = SecurityGateway.AcquireContext(username, "tester55$");
      }
      finally
      {
        SecurityGateway.ReleaseContext(adminContextId, administrator);
        SecurityGateway.ReleaseContext(contextId, username);

        SecurityGateway.DeleteUser(m_serviceContextId, administrator);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // ChngPwd_TestValidChangeByAdminUser()


    /// <summary>
    /// Tests invalid parameters provided to the change password.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestInvalidParameters()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        try
        {
          SecurityGateway.ChangePassword(m_serviceContextId, string.Empty, "tester01$", "tester55$");
          Assert.Fail("No exception was emmitted when trying to change the password without specifying the login");
        }
        catch (SqlException e)
        {
          int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
          Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
        }

        // Sleep to allow time to pass before next hitting the database.
        System.Threading.Thread.Sleep(1000);

        try
        {
          SecurityGateway.ChangePassword(null, username, string.Empty, "tester55$");
          Assert.Fail("No exception was thrown when trying to change a user\'s password without specificying the old password.");
        }
        catch (SqlException e)
        {
          int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
          Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
        }

        // Sleep to allow time to pass before next hitting the database.
        System.Threading.Thread.Sleep(1000);

        try
        {
          SecurityGateway.ChangePassword(contextId, username, "tester01$", string.Empty);
          Assert.Fail("No exception was thrown when trying to change a user\'s password without specificying a new password.");
        }
        catch (SqlException e)
        {
          int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
          Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
        }
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // ChngPwd_TestInvalidParameters()


    /// <summary>
    /// Tests a valid password change by the original user.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestValidChangeOwnPassword()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;
      
      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.ChangePassword(contextId, username, "tester01$", "tester55$");

        SecurityGateway.Authenticate(contextId, username, "tester55$");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // ChngPwd_TestValidChangeOwnPassword()


    /// <summary>
    /// Tests invalid permissions.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestInvalidPermissions()
    {
      string username = "user01";
      string username2 = "user02";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUser(m_serviceContextId, username2, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.ChangePassword(contextId, username2, "tester01$", "tester55$");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901201, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);

        SecurityGateway.DeleteUser(m_serviceContextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username2);
      }
    } // ChngPwd_TestInvalidPermissions()


    /// <summary>
    /// Tests a change pwd on an expired user.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestExpiredUser()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.Subtract(new TimeSpan(1, 0, 0, 0)), "Physician");

        SecurityGateway.ChangePassword(null, username, "tester01$", "tester55$");
        Assert.Fail("No exception was emmitted when trying to change the password of an expired user.");
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
    } // ChngPwd_TestExpiredUser()


    /// <summary>
    /// Tests a change pwd on an expired user.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestExpiredPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "texasABCDEF012568$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "dod");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "UPDATE [v3d].[Passwords] SET [exp] = \'{0}\' WHERE [login] = \'{1}\'",
            DateTime.Now.Subtract(new TimeSpan(1, 0, 0, 0)).ToString(), username));

          database.ExecuteNonQuery(cmd);

          cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Passwords] WHERE [login]=\'{0}\'", username));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");
        }

        SecurityGateway.ChangePassword(m_serviceContextId, username, "texasABCDEF012568$", "zyxuWTUV048624$");

        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Passwords] WHERE [login]=\'{0}\'", username));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(2, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");
        }
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [Auditing].[v3d].[UserHistory] WHERE [login]=\'{0}\' AND [action]=\'login\' " +
            "AND [status]=\'false\'", username));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");

          errorCode = Convert.ToInt32(e.Message.Substring(0, ds.Tables[0].Rows[0]["msg"].ToString().IndexOf(":")));
          Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // ChngPwd_TestExpiredPassword()


    /// <summary>
    /// Tests changing the password of a user that doesn't exist.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestUserNotFound()
    {
      try
      {
        SecurityGateway.ChangePassword(m_serviceContextId, "user01", "tester01$", "tester55$");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ChngPwd_TestUserNotFound()


    /// <summary>
    /// Tests to make sure that the audit log was found when chaning a password.
    /// </summary>
    [TestMethod]
    public void ChngPwd_TestAuditLogEntry()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        DateTimeOffset start = LicensingGateway.GetDatabaseDateTime();
        SecurityGateway.ChangePassword(m_serviceContextId, username, "tester01$", "tester55$");
        DateTimeOffset end = LicensingGateway.GetDatabaseDateTime();

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [Auditing].[v3d].[UserHistory] WHERE [login]=\'{0}\' AND [action]=\'{1}\' ORDER BY [date] DESC",
            username, "chgpwd"));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The expected number of tables coming back did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The expected number of rows coming back did not match expectations.");

          DateTimeOffset date = DateTimeOffset.Parse(ds.Tables[0].Rows[0]["date"].ToString());
          Assert.IsTrue(start.Subtract(new TimeSpan(0, 1, 0)) < date && date < end.AddMinutes(1), "The action date did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // ChngPwd_TestAuditLogEntry()

    #endregion

  } // class ChangePasswordTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
