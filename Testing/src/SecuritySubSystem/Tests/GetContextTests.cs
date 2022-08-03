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
  /// Validates the getContext stored proc.
  /// </summary>
  [TestClass]
  public class GetContextTests
  {

    #region fields

    /// <summary>
    /// The service user's context id.
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

      LicensingGateway.RegisterLicenseDatabase();
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 4, 120);
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
    /// Tests a valid get with the query parameter.
    /// </summary>
    [TestMethod]
    public void GetCxt_TestValidGetWithQueryParam()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        string results = SecurityGateway.GetContext(m_serviceContextId, string.Format("<context id=\"{0}\" />", contextId.ToString()));
        
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"context\"]/context").Count, "The number of returned contexts did not match expectations.");
        Assert.AreEqual(contextId, Guid.Parse(doc.DocumentElement.SelectNodes("/collection[@id=\"context\"]/context")[0].Attributes["id"].Value), "The returned results do not match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GetCxt_TestValidGetWithQueryParam()


    /// <summary>
    /// Tests invalid permissions when trying to get the context.
    /// </summary>
    [TestMethod]
    public void GetCxt_TestInvalidPermissions()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.GetContext(contextId, string.Format("<context login=\"{0}\" />", contextId.ToString()));
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
      }
    } // GetCxt_TestInvalidPermissions()


    /// <summary>
    /// Tests users with rights to perform GetContext can query.
    /// </summary>
    [TestMethod]
    public void GetCxt_TestValidPermissions()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      string administrator = "admin01";
      Guid adminContextId = Guid.Empty;

      string physAdmin = "phyadmin01";
      Guid phyAdminContextId = Guid.Empty;

      string opAdmin = "opAdmin01";
      Guid opAdminContextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertUser(m_serviceContextId, administrator, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Administrator");

        adminContextId = SecurityGateway.AcquireContext(administrator, "tester01$");

        SecurityGateway.InsertUser(m_serviceContextId, physAdmin, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Phy-Admin");

        phyAdminContextId = SecurityGateway.AcquireContext(physAdmin, "tester01$");

        SecurityGateway.InsertUser(m_serviceContextId, opAdmin, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Op-Admin");

        opAdminContextId = SecurityGateway.AcquireContext(opAdmin, "tester01$");

        SecurityGateway.GetContext(adminContextId,    string.Format("<context login=\"{0}\" />", username));
        SecurityGateway.GetContext(phyAdminContextId, string.Format("<context login=\"{0}\" />", username));
        SecurityGateway.GetContext(opAdminContextId,  string.Format("<context login=\"{0}\" />", username));
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);

        SecurityGateway.ReleaseContext(adminContextId, administrator);
        SecurityGateway.DeleteUser(m_serviceContextId, administrator);

        SecurityGateway.ReleaseContext(phyAdminContextId, physAdmin);
        SecurityGateway.DeleteUser(m_serviceContextId, physAdmin);

        SecurityGateway.ReleaseContext(opAdminContextId, opAdmin);
        SecurityGateway.DeleteUser(m_serviceContextId, opAdmin);
      }
    } // GetCxt_TestValidPermissions()


    /// <summary>
    /// Tests querying multiple contexts <context id="context1|context2" />
    /// </summary>
    [TestMethod]
    public void GetCxt_TestMultiContextQuery()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      string adminUser = "admin01";
      Guid adminContext = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertUser(m_serviceContextId, adminUser, "He is Administrator Man!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Administrator");

        adminContext = SecurityGateway.AcquireContext(adminUser, "tester01$");

        string results = SecurityGateway.GetContext(adminContext, string.Format("<context id=\"{0}|{1}\" />", contextId, adminContext));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"context\"]/context").Count, 
          "The number of contexts retrned does not match expectations...");

        XmlNodeList user01Contexts = doc.DocumentElement.SelectNodes(string.Format(
          "/collection[@id=\"context\"]/context[@login=\"{0}\"]", username));

        Assert.AreEqual(1, user01Contexts.Count, "The number of returned context nodes for the physician doesn't match expectations.");
        Assert.AreEqual(contextId, Guid.Parse(user01Contexts[0].Attributes["id"].Value), "The expected context id couldn't be found.");

        XmlNodeList adminContexts = doc.DocumentElement.SelectNodes(string.Format(
          "/collection[@id=\"context\"]/context[@login=\"{0}\"]", adminUser));

        Assert.AreEqual(1, adminContexts.Count, "The number of returned context nodes for the admin doesn't match expectations.");
        Assert.AreEqual(adminContext, Guid.Parse(adminContexts[0].Attributes["id"].Value), "The expected context id couldn't be found.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);

        SecurityGateway.ReleaseContext(adminContext, adminUser);
        SecurityGateway.DeleteUser(m_serviceContextId, adminUser);
      }
    } // GetCxt_TestMultiContextQuery()


    /// <summary>
    /// Tests multiple licenses in the query.
    /// </summary>
    [TestMethod]
    public void GetCxt_TestMultiLicenseQuery()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;
      string usernameLicense = string.Empty;

      string adminUser = "admin01";
      Guid adminContext = Guid.Empty;
      string adminLicense = string.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertUser(m_serviceContextId, adminUser, "He is Administrator Man!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        adminContext = SecurityGateway.AcquireContext(adminUser, "tester01$");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          //
          // Username
          //

          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Contexts] WHERE [v3d].[Contexts].[id] = \'{0}\'", contextId.ToString()));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");

          usernameLicense = ds.Tables[0].Rows[0]["lic"].ToString();

          //
          // Administrator
          //

          cmd = database.GetSqlStringCommand(string.Format(
            "SELECT * FROM [v3d].[Contexts] WHERE [v3d].[Contexts].[id] = \'{0}\'", adminContext.ToString()));

          ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match expectations.");

          adminLicense = ds.Tables[0].Rows[0]["lic"].ToString();
        }

        //
        // Username
        //

        string results = SecurityGateway.GetContext(m_serviceContextId, string.Format("<context lic=\"{0}\" />",
          usernameLicense));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        XmlNodeList userNode = doc.DocumentElement.SelectNodes(string.Format(
          "/collection[@id=\"context\"]/context[@login=\"{0}\"]", username));

        Assert.AreEqual(contextId, Guid.Parse(userNode[0].Attributes["id"].Value), "The user's context did not match expectations.");

        //
        // Administrator
        //

        results = SecurityGateway.GetContext(m_serviceContextId, string.Format("<context lic=\"{0}\" />",
          adminLicense));

        doc = new XmlDocument();
        doc.LoadXml(results);

        XmlNodeList adminNode = doc.DocumentElement.SelectNodes(string.Format(
          "/collection[@id=\"context\"]/context[@login=\"{0}\"]", adminUser));

        Assert.AreEqual(contextId, Guid.Parse(userNode[0].Attributes["id"].Value), "The admin's context did not match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);

        SecurityGateway.ReleaseContext(adminContext, adminUser);
        SecurityGateway.DeleteUser(m_serviceContextId, adminUser);
      }
    } // GetCxt_TestMultiLicenseQuery()

    #endregion

  } // class GetContextTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
