
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
  /// Validates the delete user profile stored proc.
  /// </summary>
  [TestClass]
  public class DeleteUserProfileTests
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
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after each test ends.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a succcessful deletion of a user's profile.
    /// </summary>
    [TestMethod]
    public void DelUsrPrf_TestValidDelete()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUserProfile(m_serviceContextId, username, "SampleProfile", "<profile />");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [v3d].[UserProfiles] WHERE [login] = \'{0}\' AND [name] = \'{1}\'",
            username, "SampleProfile"));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");
          Assert.AreEqual("<profile />", ds.Tables[0].Rows[0]["profile"].ToString(), "The user profile xml did not match expectations.");
        }

        SecurityGateway.DeleteUserProfile(m_serviceContextId, username, "SampleProfile");

        database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [v3d].[UserProfiles] WHERE [login] = \'{0}\' AND [name] = \'{1}\'",
            username, "SampleProfile"));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(0, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // DelUsrPrf_TestValidDelete()


    /// <summary>
    /// Tests a delete user profile request with a login that doesn't exist.
    /// </summary>
    [TestMethod]
    public void DelUsrPrf_TestLoginNotFound()
    {
      try
      {
        SecurityGateway.DeleteUserProfile(m_serviceContextId, "a", "user profile");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // DelUsrPrf_TestLoginNotFound()


    /// <summary>
    /// Tests a delete user profile request with a profile name that doesn't exist.
    /// </summary>
    [TestMethod]
    public void DelUsrPrf_TestProfileNameNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.DeleteUserProfile(m_serviceContextId, username, "userprofile");
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
    } // DelUsrPrf_TestProfileNameNotFound()

    #endregion

  } // class DeleteUserProfileTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
