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
  /// Validates the getUser stored proc.
  /// </summary>
  [TestClass]
  public class GetUserTests
  {

    #region fields

    /// <summary>
    /// Service's context id.
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
    /// Tests retrieving users by login name.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestLoginQuery()
    {
      string username = "user01";
      string username2 = "user02";
      string username3 = "user03";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUser(m_serviceContextId, username2, "He is Superman2!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUser(m_serviceContextId, username3, "He is Superman3!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        string results = SecurityGateway.GetUser(m_serviceContextId, string.Format("<user login=\"{0}|{1}\" />",
          username, username2));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The results node name did not match expectations.");
        Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The results id attribute did not match expectations.");
        Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "the number of users returned did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username2);
        SecurityGateway.DeleteUser(m_serviceContextId, username3);
      }
    } // GtUsr_TestLoginQuery()


    /// <summary>
    /// Gets a user by the name.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestNameQuery()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        string results = SecurityGateway.GetUser(m_serviceContextId, "<user name=\"He is Superman!\" />");

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
        Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtUsr_TestNameQuery()


    /// <summary>
    /// Tests a query by the role.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestRoleQuery()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        string results = SecurityGateway.GetUser(m_serviceContextId, "<user role=\"Physician\" />");

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
        Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtUsr_TestRoleQuery()


    /// <summary>
    /// Tests getting users by authentication type.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestAuthQuery()
    {
      string results = SecurityGateway.GetUser(m_serviceContextId, "<user auth=\"Viatronix\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
      Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");

      results = SecurityGateway.GetUser(m_serviceContextId, "<user auth=\"Windows\" />");

      doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
      Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
      Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
    } // GtUsr_TestAuthQuery()


    /// <summary>
    /// Tests querying by domain.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestDomainQuery()
    {
      string results = SecurityGateway.GetUser(m_serviceContextId, "<user dom=\"\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
      Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
      Assert.AreEqual(3, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
    } // GtUsr_TestDomainQuery()


    /// <summary>
    /// Tests querying the database with state.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestStateQuery()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Disabled, DateTime.Now.AddYears(300));

        string results = SecurityGateway.GetUser(m_serviceContextId, string.Format("<user state=\"{0}\" />", (int)SecurityGateway.UserStates.Disabled));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
        Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtUsr_TestStateQuery()


    /// <summary>
    /// Tests getting a user by it's app value.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestAppsQuery()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.UpdateUser(m_serviceContextId, string.Format("<user login=\"{0}\" apps=\"512\" />", username));

        string results = SecurityGateway.GetUser(m_serviceContextId, "<user apps=\"256\" />");

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
        Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtUsr_TestAppsQuery()


    /// <summary>
    /// Tests querying by added date.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestAddDate()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        string results = SecurityGateway.GetUser(m_serviceContextId, string.Format("<user><add from=\"{0}\" to=\"{1}\" /></user>",
          DateTimeOffset.Now.Subtract(new TimeSpan(0, 5, 0)).ToString("o"), DateTimeOffset.Now.AddMinutes(5).ToString("o")));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
        Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtUsr_TestAddDate()


    /// <summary>
    /// Tests querying by expiration date.
    /// </summary>
    [TestMethod]
    public void GtUsr_TestExpDate()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        string results = SecurityGateway.GetUser(m_serviceContextId, string.Format("<user><exp from=\"{0}\" to=\"{1}\" /></user>",
          DateTimeOffset.Now.AddYears(300).Subtract(new TimeSpan(0, 5, 0)).ToString("o"), 
          DateTimeOffset.Now.AddYears(300).AddMinutes(5).ToString("o")));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The root node name did not match expectations.");
        Assert.AreEqual("user", doc.DocumentElement.Attributes["id"].Value, "The root node id value did not match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"user\"]/user").Count, "The number of users returned did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtUsr_TestExpDate()

    #endregion

  } // class GetUserTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
