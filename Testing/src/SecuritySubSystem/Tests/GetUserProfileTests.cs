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
  /// Validates the getUserProfile stored proc.
  /// </summary>
  [TestClass]
  public class GetUserProfileTests
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
    /// Gets the user profile by login.
    /// </summary>
    [TestMethod]
    public void GtUsrPrf_GetByLogin()
    {
      string username = "user01";
      string username2 = "user02";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUser(m_serviceContextId, username2, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUserProfile(m_serviceContextId, username, "profile", "<profile lanuguage=\"English\" />");
        SecurityGateway.InsertUserProfile(m_serviceContextId, username2, "profile2", "<profile lanuguage=\"French\" />");

        string results = SecurityGateway.GetUserProfile(m_serviceContextId, string.Format("<userprofile login=\"{0}\" />",
          username));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The returned node name didn't match expectations.");
        Assert.AreEqual("userprofile", doc.DocumentElement.Attributes["id"].Value,
          "The returned node id attribute value didn't match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"userprofile\"]/userprofile").Count, 
          "The number of returned user profiles did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUserProfile(m_serviceContextId, username, "profile");
        SecurityGateway.DeleteUserProfile(m_serviceContextId, username2, "profile");

        SecurityGateway.DeleteUser(m_serviceContextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username2);
      }
    } // GtUsrPrf_GetByLogin()


    /// <summary>
    /// Tests getting a user profile by the profile name.
    /// </summary>
    [TestMethod]
    public void GtUsrPrf_GetByProfileName()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        SecurityGateway.InsertUserProfile(m_serviceContextId, username, "profile", "<profile lanuguage=\"English\" />");
        SecurityGateway.InsertUserProfile(m_serviceContextId, username, "profile2", "<profile lanuguage=\"French\" />");

        string results = SecurityGateway.GetUserProfile(m_serviceContextId, string.Format("<userprofile login=\"{0}\" name=\"{1}\" />",
          username, "profile2"));

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The returned node name didn't match expectations.");
        Assert.AreEqual("userprofile", doc.DocumentElement.Attributes["id"].Value,
          "The returned node id attribute value didn't match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"userprofile\"]/userprofile").Count,
          "The number of returned user profiles did not match expectations.");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(
          "/collection[@id=\"userprofile\"]/userprofile[@login=\"user01\" and @name=\"profile2\"]").Count,
          "The number of returned user profiles did not match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUserProfile(m_serviceContextId, username, "profile");
        SecurityGateway.DeleteUserProfile(m_serviceContextId, username, "profile2");

        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtUsrPrf_GetByProfileName()

    #endregion

  } // class GetUserProfileTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
