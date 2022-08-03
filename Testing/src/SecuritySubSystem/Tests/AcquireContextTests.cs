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
  /// Validates the acquire context stored proc.
  /// </summary>
  [TestClass]
  public class AcquireContextTests
  {

    #region fields

    /// <summary>
    /// The service's context id.
    /// </summary>
    private static Guid m_serviceContextId = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before each tests are run.
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
    /// Performs cleanup after each tests are run.
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
    /// Tests a valid acquire.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestValidAcquire()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$", 
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AcquireContext(username, "tester01$");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AcqCxt_TestValidAcquire()


    /// <summary>
    /// Tests acquiring a context with an account that doesn't exist.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestAccountNotFound()
    {
      try
      {
        SecurityGateway.AcquireContext("user01", "tester01$");
        Assert.Fail("No exception was emmitted when trying to acquire a context with an account that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AcqCxt_TestAccountNotFound()


    /// <summary>
    /// Tests a user that has a context already.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestUserHasContextAlready()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AcquireContext(username, "tester01$");
        SecurityGateway.AcquireContext(username, "tester01$");

        Assert.Fail("No exception was emmitted when trying to acquire a context when the user already has one.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901202, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AcqCxt_TestUserHasContextAlready()


    /// <summary>
    /// Tests an invalid password.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestInvalidPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$", 
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AcquireContext(username, "abcd12!");

        Assert.Fail("No exception was emmitted when trying to acquire a context with an invalid password.");
      }
      catch(SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AcqCxt_TestInvalidPassword()


    /// <summary>
    /// Tests an invalid context provided when acquiring a context (up the ref count).
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestInvalidContext()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AcquireContext(username, "aaa", null, 30, Guid.NewGuid());

        Assert.Fail("No exception was emmitted when trying to acquire a context with an invalid context.");
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
    } // AcqCxt_TestInvalidContext()


    /// <summary>
    /// Tests logging in as a user that has been locked.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestLoginWithLockedUser()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Locked, DateTime.Now.AddYears(300));

        SecurityGateway.AcquireContext(username, "tester01$");

        Assert.Fail("No exception was emmitted when attempting to login with a locked user.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900994, errorCode, "The expected errorcode returned didn't match expectations.");

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
          Assert.AreEqual(900994, errorCode, "The expected errorcode returned didn't match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AcqCxt_TestLoginWithLockedUser()


    /// <summary>
    /// Tests logging in as a user that has been expired.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestLoginWithExpiredUser()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.Subtract(new TimeSpan(1, 0, 0, 0)));

        SecurityGateway.AcquireContext(username, "tester01$");

        Assert.Fail("No exception was emmitted when attempting to login with an expired user.");
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
    } // AcqCxt_TestLoginWithExpiredUser()


    /// <summary>
    /// Tests logging in as a user that has been disabled.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestLoginWithDisabledUser()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Disabled, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        Assert.Fail("No exception was emmitted when attempting to login with a disabled user.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900997, errorCode, "The expected errorcode returned didn't match expectations.");

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
          Assert.AreEqual(900997, errorCode, "The expected errorcode returned didn't match expectations.");
        }
      }
      finally
      {
        if(contextId != Guid.Empty)
        {
          SecurityGateway.ReleaseContext(contextId, username);
          contextId = Guid.Empty;
        }

        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AcqCxt_TestLoginWithDisabledUser()


    /// <summary>
    /// Tests logging in as a user that has an expired password.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestLoginWithExpiredPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "UPDATE [v3d].[Passwords] SET [v3d].[Passwords].[exp] = \'{0}\' WHERE [login] = \'{1}\'",
            DateTimeOffset.Now.Subtract(new TimeSpan(1, 1, 1)).ToString("o"), username));

          database.ExecuteNonQuery(cmd);
        }

        SecurityGateway.AcquireContext(username, "tester01$");

        Assert.Fail("No exception was emmitted when attempting to login with a disabled user.");
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
    } // AcqCxt_TestLoginWithExpiredPassword()


    /// <summary>
    /// Tests the auditing for a login.
    /// </summary>
    [TestMethod]
    public void AcqCxt_TestAuditing()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        DateTimeOffset start = LicensingGateway.GetDatabaseDateTime().Subtract(new TimeSpan(0, 0, 1));

        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        DateTimeOffset end = LicensingGateway.GetDatabaseDateTime().AddSeconds(1);

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [Auditing].[v3d].[UserHistory] WHERE [login] = \'{0}\' AND [action] = \'login\' ORDER BY [date] DESC",
            username));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of expected tables doesn't match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of expected rows doesn't match expectations.");
          
          DateTimeOffset date = DateTimeOffset.Parse(ds.Tables[0].Rows[0]["date"].ToString());
          Assert.IsTrue(start < date && date < end, "The login date did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AcqCxt_TestAuditing()

    #endregion

  } // class AcquireContextTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
