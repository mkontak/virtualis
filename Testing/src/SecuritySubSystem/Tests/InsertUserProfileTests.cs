using System;
using System.Xml;
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
  /// Tests the insert userprofile 
  /// </summary>
  [TestClass]
  public class InsertUserProfileTests
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
    /// Performs cleanup after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a valid insertion of a user profile.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestValidInsert()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUserProfile(m_serviceContextId, username, "TestProfile", "<profile />");

        // Verify the sp did as expected.
        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[UserProfiles] WHERE [login] = \'{0}\' AND [name] = \'TestProfile\'", username));
          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.IsTrue(ds.Tables.Count > 0, "The dataset did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of user profiles did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsUsrPrf_TestValidInsert()


    /// <summary>
    /// Tests insert user profile with an invalid context.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestInvalidContext()
    {
      try
      {
        SecurityGateway.InsertUserProfile(Guid.Empty, "user01", "TestProfile", "<profile />");
        Assert.Fail("The database did not emmit an error when trying to insert a user profile without a valid context.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsUsrPrf_TestInvalidContext()


    /// <summary>
    /// Tests the insert user profile with a username that isn't in the system.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestAccountNotFound()
    {
      try
      {
        SecurityGateway.InsertUserProfile(m_serviceContextId, "user01", "TestProfile", "<profile />");
        Assert.Fail("The database did not emmit an error when trying to insert a user profile without a username not found.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsUsrPrf_TestAccountNotFound()


    /// <summary>
    /// Tests inserting a user profile with an empty username.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestEmptyUsername()
    {
      try
      {
        SecurityGateway.InsertUserProfile(m_serviceContextId, "", "TestProfile", "<profile />");
        Assert.Fail("The database did not emmit an error when trying to insert a user profile with a blank username.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsUsrPrf_TestEmptyUsername()


    /// <summary>
    /// Tests inserting a user profile with an empty profile name.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestEmptyProfileName()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUserProfile(m_serviceContextId, "user01", "", "<profile />");
        Assert.Fail("The database did not emmit an error when trying to insert a user profile with a blank user profile.");
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
    } // InsUsrPrf_TestEmptyProfileName()


    /// <summary>
    /// Tests inserting a user profile with an empty user profile.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestEmptyUserProfile()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUserProfile(m_serviceContextId, "user01", "TestProfile", string.Empty);
        Assert.Fail("The database did not emmit an error when trying to insert a user profile with a blank user profile xml.");
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
    } // InsUsrPrf_TestEmptyProfileName()


    /// <summary>
    /// Tests inserting a user profile with invalid user profile xml.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestInvalidUserProfileXml()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUserProfile(m_serviceContextId, "user01", "TestProfile", "<goofball />");
        Assert.Fail("The database did not emmit an error when trying to insert a user profile with invalid user profile xml.");
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
    } // InsUsrPrf_TestEmptyProfileName()


    /// <summary>
    /// Tests a double insertion of a user profile.  Validates that the second insert updates the profile.
    /// </summary>
    [TestMethod]
    public void InsUsrPrf_TestDoubleInsertCausesUpdate()
    {
      string username = "user01";
      string profileName = "TestProfile";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUserProfile(m_serviceContextId, username, profileName, "<profile />");
        SecurityGateway.InsertUserProfile(m_serviceContextId, username, profileName, "<profile changed=\"true\" />");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[UserProfiles] WHERE [login] = \'{0}\' AND [name] = \'{1}\'", username, profileName));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables returned did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of rows returned did not match expectations.");

          XmlDocument doc = new XmlDocument();
          doc.LoadXml(ds.Tables[0].Rows[0]["profile"].ToString());

          Assert.AreEqual("profile", doc.DocumentElement.Name, "The profile's root name did not match expectations.");
          Assert.IsTrue(doc.DocumentElement.Attributes["changed"] != null &&
            Convert.ToBoolean(doc.DocumentElement.Attributes["changed"].Value),
            "The profile's root name did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUserProfile(m_serviceContextId, username, profileName);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsUsrPrf_TestDoubleInsertCausesUpdate()

    #endregion

  } // class InsertUserProfileTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
