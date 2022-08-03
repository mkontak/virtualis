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
  /// Tests authentication of the user.
  /// </summary>
  [TestClass]
  public class AuthenticateUserTests
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
    } // TestInitialize()


    /// <summary>
    /// Performs initialization after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests for a login not found.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TestLoginNotFound()
    {
      try
      {
        SecurityGateway.AuthenticateUser("abcdeft", "abcdefghj1@3");
        Assert.Fail("No exception was thrown when trying to authenticate invalid user.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AuthUsr_TestLoginNotFound()


    /// <summary>
    /// Tests forcing a password change on a user, and authenticateUser picks it up.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TestPasswordChangeRequired()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman", "tester01$",
          (SecurityGateway.UserStates.Available | SecurityGateway.UserStates.PasswordChange),
          DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AuthenticateUser(username, "tester01$");
        Assert.Fail("No exception was thrown when trying to authenticate a user whose password needs to be changged.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901223, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AuthUsr_TestPasswordChangeRequired()


    /// <summary>
    /// Tests expiration on a user, and authenticateUser picks it up.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TestExpiredUser()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.Subtract(new TimeSpan(300, 0, 0, 0)), "Physician");

        SecurityGateway.AuthenticateUser(username, "tester01$");
        Assert.Fail("No exception was thrown when trying to authenticate a user whose password needs to be changged.");
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
    } // AuthUsr_TestExpiredUser()


    /// <summary>
    /// Tests locking a user, and authenticateUser picks it up.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TestLockedUser()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman", "tester01$",
          SecurityGateway.UserStates.Locked, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AuthenticateUser(username, "tester01$");
        Assert.Fail("No exception was thrown when trying to authenticate a user whose password needs to be changged.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900994, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AuthUsr_TestLockedUser()


    /// <summary>
    /// Tests locking a user, and authenticateUser picks it up.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TestUserHasNoPasssword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format("DELETE FROM [v3d].[Passwords] WHERE [login] = \'{0}\'", username));
          database.ExecuteNonQuery(cmd);
        }

        SecurityGateway.AuthenticateUser(username, "tester01$");
        Assert.Fail("No exception was thrown when trying to authenticate a user who has no assigned password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901223, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AuthUsr_TestUserHasNoPasssword()


    /// <summary>
    /// Tests authenticateUser with an expired password.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TestExpiredPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddDays(5), "Physician");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format("UPDATE [v3d].[Passwords] SET [exp] = '{0}' WHERE [login] = \'{1}\'",
            DateTime.Now.Subtract(new TimeSpan(1, 0, 0, 0)), username));

          database.ExecuteNonQuery(cmd);
        }

        SecurityGateway.AuthenticateUser(username, "tester01$");
        Assert.Fail("No exception was thrown when trying to authenticate a user with an expired password.");
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
    } // AuthUsr_TestExpiredPassword()


    /// <summary>
    /// Tests authenticateUser with an invalid password.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TesInvalidPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AuthenticateUser(username, "password1!");
        Assert.Fail("No exception was thrown when trying to authenticate a user with an incorrect password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AuthUsr_TesInvalidPassword()


    /// <summary>
    /// Tests an invalid login.
    /// </summary>
    [TestMethod]
    public void AuthUsr_TestValidUser()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman", "tester01$",
          (SecurityGateway.UserStates.Available),
          DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.AuthenticateUser(username, "tester01$");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // AuthUsr_TestInvalidLogin()

    #endregion

  } // class AuthenticateUserTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
