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
  /// Validates the authenticate stored proc.
  /// </summary>
  [TestClass]
  public class AuthenticateTests
  {

    #region fields

    /// <summary>
    /// The service context id.
    /// </summary>
    private static Guid m_serviceContextId = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before each test is run.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();

      m_serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);

      LicensingGateway.RegisterLicenseDatabase();
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);
    } // TestInitialize( context )


    /// <summary>
    /// Performs cleanup after each test is run.
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
    /// Tests the valid authentication.
    /// </summary>
    [TestMethod]
    public void Auth_TestValidAuthentication()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.Authenticate(contextId, username, "tester01$");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestValidAuthentication()


    /// <summary>
    /// Tests authentication with an invalid username.
    /// </summary>
    [TestMethod]
    public void Auth_TestInvalidUsername()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.Authenticate(contextId, "user02", "tester01$");
        Assert.Fail("The database did not emmit an error when trying to authenticate with a username that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestInvalidUsername()


    /// <summary>
    /// Tests authentication with an invalid password.
    /// </summary>
    [TestMethod]
    public void Auth_TestInvalidPassword()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.Authenticate(contextId, username, "tester02$");
        Assert.Fail("The database did not emmit an error when trying to authenticate with an invalid password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestInvalidPassword()


    /// <summary>
    /// Tests authentication with an invalid context.
    /// </summary>
    [TestMethod]
    public void Auth_TestInvalidContext()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.Authenticate(Guid.NewGuid(), "user02", "tester01$");
        Assert.Fail("The database did not emmit an error when trying to authenticate with a contextId that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestInvalidContext()


    /// <summary>
    /// Tests authentication with a user that has a context, but has subsequently been locked.
    /// </summary>
    [TestMethod]
    public void Auth_TestWithLockedUser()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" state=\"{1}\" />",
          username, (int)SecurityGateway.UserStates.Locked));

        SecurityGateway.Authenticate(contextId, username, "tester01$");

        Assert.Fail("No exception was emmitted when trying to authenticate a locked user.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900994, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestWithLockedUser()


    /// <summary>
    /// Tests authentication with a user that has a context, but has subsequently expired.
    /// </summary>
    [TestMethod]
    public void Auth_TestWithExpiredUser()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" exp=\"{1}\" />",
          username, DateTimeOffset.Now.Subtract(new TimeSpan(0, 1, 0)).ToString("o")));

        SecurityGateway.Authenticate(contextId, username, "tester01$");

        Assert.Fail("No exception was emmitted when trying to authenticate a locked user.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestWithExpiredUser()


    /// <summary>
    /// Tests authentication with a user that has a context, but has subsequently been disabled.
    /// </summary>
    [TestMethod]
    public void Auth_TestWithDisabledUser()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" state=\"{1}\" />",
          username, (int)SecurityGateway.UserStates.Disabled));

        SecurityGateway.Authenticate(contextId, username, "tester01$");

        Assert.Fail("No exception was emmitted when trying to authenticate a disabled user.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900997, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestWithDisabledUser()


    /// <summary>
    /// Tests authentication with a user that has a context, but his password has subsequently expired.
    /// </summary>
    [TestMethod]
    public void Auth_TestWithExpiredPassword()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "UPDATE [v3d].[Passwords] SET [exp]=\'{0}\' WHERE [login]=\'{1}\'",
            DateTimeOffset.Now.Subtract(new TimeSpan(0, 1, 0)).ToString("o"), username));

          database.ExecuteNonQuery(cmd);
        }

        SecurityGateway.Authenticate(contextId, username, "tester01$");

        Assert.Fail("No exception was emmitted when trying to authenticate a user with an expired password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // Auth_TestWithExpiredPassword()

    #endregion

  } // class AuthenticateTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
