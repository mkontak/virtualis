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
  /// Validates the forcePassword change stored proc.
  /// </summary>
  [TestClass]
  public class ForcePasswordChangeTests
  {

    #region fields

    /// <summary>
    /// The service context id to perform testing...
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

      LicensingGateway.RegisterLicenseDatabase(DateTimeOffset.Now.AddYears(300));
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
    /// Tests forcing a password change and then attempt to login.
    /// </summary>
    [TestMethod]
    public void FrcPwdChng_TestValidForceAndLogin()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.ForcePasswordChange(m_serviceContextId, username);

        // Now try to login, should get notified I need to change my password.
        contextId = SecurityGateway.AcquireContext(username, "tester01$");
        Assert.Fail("No exception was emmitted when trying to force a password change and then trying to login.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901223, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // FrcPwdChng_TestValidForceAndLogin()


    /// <summary>
    /// Tests having invalid permissions when trying to force a password change.
    /// </summary>
    [TestMethod]
    public void FrcPwdChng_TestInvalidPermissions()
    {
      string username = "user01";
      string username2 = "user02";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUser(m_serviceContextId, username2, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.ForcePasswordChange(contextId, username2);
        Assert.Fail("No exception was emmitted when trying to force a password change on another user without proper permissions.");
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
    } // FrcPwdChng_TestInvalidPermissions()


    /// <summary>
    /// Tests forcing a user that doesn't exist to change his password, we expect an error.
    /// </summary>
    [TestMethod]
    public void FrcPwdChng_TestAccountNotFound()
    {
      try
      {
        SecurityGateway.ForcePasswordChange(m_serviceContextId, "user01");
        Assert.Fail("No exception was emmitted when trying to force a password change on an account that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // FrcPwdChng_TestAccountNotFound()


    /// <summary>
    /// Tests forcing a password change with an invalid context id.
    /// </summary>
    [TestMethod]
    public void FrcPwdChng_TestContextNotFound()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.ForcePasswordChange(Guid.NewGuid(), username);
        Assert.Fail("No exception was emmitted when trying to force a password change and then trying to login.");
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
    } // FrcPwdChng_TestContextNotFound()


    /// <summary>
    /// Tests to make sure that the audit log was found when forcing a password change.
    /// </summary>
    [TestMethod]
    public void FrcPwdChng_TestAuditLogEntry()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        DateTimeOffset start = LicensingGateway.GetDatabaseDateTime();
        SecurityGateway.ForcePasswordChange(m_serviceContextId, username);
        DateTimeOffset end = LicensingGateway.GetDatabaseDateTime();

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [Auditing].[v3d].[UserHistory] WHERE [login]=\'{0}\' AND [action]=\'{1}\' ORDER BY [date] DESC",
            username, "frcpwdchg"));

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
    } // FrcPwdChng_TestAuditLogEntry()

    #endregion

  } // class ForcePasswordChangeTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
