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
  /// Validates the update user stored proc.
  /// </summary>
  [TestClass]
  public class UpdateUserTests
  {

    #region fields

    /// <summary>
    /// The service context id.
    /// </summary>
    private Guid m_serviceContextId = Guid.Empty;

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

      LicensingGateway.RegisterLicenseDatabase(DateTimeOffset.Now.AddYears(300));
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);
    } // TestInitialize()


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
    /// Tests a valid update and validates the change.
    /// </summary>
    [TestMethod]
    public void UpdUsr_TestValidUpdate()
    {
      string username = "user01";
      DateTimeOffset exp = DateTimeOffset.Now;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 VIATRONIX", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" state=\"{1}\" exp=\"{2}\" />", username,
          (int)SecurityGateway.UserStates.Disabled, exp.ToString("o")));

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Users] WHERE [login]=\'{0}\' AND [state]=\'{1}\' AND [exp]=\'{2}\'",
            username, (int)SecurityGateway.UserStates.Disabled, exp.ToString("o")));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");
          Assert.AreEqual("USER01 VIATRONIX", ds.Tables[0].Rows[0]["name"].ToString(),
            "The login name for the row returned did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // UpdUsr_TestValidUpdate()


    /// <summary>
    /// Tests a non-existant state for update.
    /// </summary>
    [TestMethod]
    public void UpdUsr_TestStateNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" state=\"100000\" />", username));
      }
      catch(SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // UpdUsr_TestStateNotFound()


    /// <summary>
    /// Tests calling the update with a password template that doesn't exist.
    /// </summary>
    [TestMethod]
    public void UpdUsr_TestPasswordTemplateNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" ptmp=\"goofball\" />", username));
      }
      catch(SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // UpdUsr_TestPasswordTemplateNotFound()


    /// <summary>
    /// Tests calling the update with an authentication type that doesn't exist.
    /// </summary>
    [TestMethod]
    public void UpdUsr_TestAuthenticationTypeNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" auth=\"goofball\" />", username));
      }
      catch(SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // UpdUsr_TestAuthenticationTypeNotFound()


    /// <summary>
    /// Tests calling the update with an account that doesn't exist.
    /// </summary>
    [TestMethod]
    public void UpdUsr_TestAccountNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        SecurityGateway.UpdateUser(m_serviceContextId, "<user login=\"goofball-0002\" name=\"HELLO\" />");
      }
      catch(SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // UpdUsr_TestAccountNotFound()


    /// <summary>
    /// Tests calling the update to alter a system account.
    /// </summary>
    [TestMethod]
    public void UpdUsr_TestUpdateSystemAccountForNonService()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Administrator");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.UpdateUser(contextId, string.Format("<user login=\"{0}\" name=\"GOOFBALL!!!\" />", SecurityGateway.SRVCUSERNAME));
        Assert.Fail("No exception was emmited when trying to alter a system account.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901204, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // UpdUsr_TestUpdateSystemAccount()


    /// <summary>
    /// Tests updating a system account with a service account.
    /// </summary>
    [TestMethod]
    public void UpdUsr_TestUpdateSystemAccountForService()
    {
      string auditorUsername = "auditor";
      string auditorName = string.Empty;

      //
      // Get the auditor's name.
      //

      string results = SecurityGateway.GetUser(m_serviceContextId, string.Format("<user login=\"{0}\" />", auditorName));

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      XmlNodeList nodes = doc.DocumentElement.SelectNodes(string.Format(
        "/collection[@id=\"user\"]/user[@login=\"{0}\"]", auditorUsername));

      Assert.AreEqual(1, nodes.Count, "The collection returned did not match expectations.");

      auditorName = nodes[0].Attributes["name"].Value;

      //
      // Update the auditor user and verify the name was changed.
      //

      SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" name=\"{1}\" />",
        auditorUsername, "My name is..."));

      results = SecurityGateway.GetUser(m_serviceContextId, string.Format("<user login=\"{0}\" name=\"{1}\" />",
        auditorUsername, "My name is..."));

      doc = new XmlDocument();
      doc.LoadXml(results);

      nodes = doc.DocumentElement.SelectNodes(string.Format(
        "/collection[@id=\"user\"]/user[@login=\"{0}\" and @name=\"{1}\"]", auditorUsername, "My name is..."));

      Assert.AreEqual(1, nodes.Count, "The collection returned did not match expectations.");

      //
      // Revert the value to the original value and verify the name was reverted.
      //

      SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" name=\"{1}\" />",
        auditorUsername, auditorName));

      results = SecurityGateway.GetUser(m_serviceContextId, string.Format("<user login=\"{0}\" name=\"{1}\" />",
        auditorUsername, auditorName));

      doc = new XmlDocument();
      doc.LoadXml(results);

      nodes = doc.DocumentElement.SelectNodes(string.Format(
        "/collection[@id=\"user\"]/user[@login=\"{0}\" and @name=\"{1}\"]", auditorUsername, auditorName));

      Assert.AreEqual(1, nodes.Count, "The collection returned did not match expectations.");

    } // UpdUsr_TestUpdateSystemAccountForService()

    #endregion

  } // class UpdateUserTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
