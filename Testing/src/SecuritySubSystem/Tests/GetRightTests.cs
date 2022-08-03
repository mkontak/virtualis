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
  /// Validates the getRight stored proc.
  /// </summary>
  [TestClass]
  public class GetRightTests
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

      LicensingGateway.RegisterLicenseDatabase();
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
    /// Tests a valid query.
    /// </summary>
    [TestMethod]
    public void GtRght_TestValidQuery()
    {
      string results = SecurityGateway.GetRight(m_serviceContextId, "<right name=\"UserManagement\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The results root name did not match expectations.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes(
        "/collection[@id=\"right\"]/right[@name=\"UserManagement\" and @mask=\"31\"]").Count,
        "The number of rights returned in the collection did not match expectations.");
    } // GtRght_TestValidQuery()


    /// <summary>
    /// Tests getting a right with valid permissions.
    /// </summary>
    [TestMethod]
    public void GtRght_TestValidPermissions()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Administrator");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        string results = SecurityGateway.GetRight(contextId, "<right />");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // GtRght_TestValidPermissions()


    /// <summary>
    /// Attempts to get a right without valid permissions.
    /// </summary>
    [TestMethod]
    public void GtRght_TestInvalidPermissions()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.GetRight(contextId, "<right name=\"UserManagement\" />");
        Assert.Fail("The database did not emmit an error when trying to get a right without proper authorization.");
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
    } // GtRght_TestInvalidPermissions()
    
    #endregion

  } // class GetRightTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
