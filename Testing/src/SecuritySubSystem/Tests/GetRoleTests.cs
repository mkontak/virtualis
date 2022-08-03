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
  /// Validates the getRole stored proc.
  /// </summary>
  [TestClass]
  public class GetRoleTests
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

      LicensingGateway.RegisterLicenseDatabase();
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

      m_serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);
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
    /// Tests getting a role.
    /// </summary>
    [TestMethod]
    public void GtRole_TestValid()
    {
      string results = SecurityGateway.GetRole(m_serviceContextId, "<role name=\"Auditor\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The results root node name did not match expectations.");
      Assert.AreEqual("role", doc.DocumentElement.Attributes["id"].Value, "The results id did not match expectations.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"role\"]/role[@name=\"Auditor\"]").Count,
        "The number of nodes returned did not match expectations.");
    } // GtRole_TestValid()


    /// <summary>
    /// Tests multiple names in the query.
    /// </summary>
    public void GtRole_TestMultiName()
    {
      string results = SecurityGateway.GetRole(m_serviceContextId, "<role name=\"Administrator|Physician|Operator\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The results root name did not match expectations.");
      Assert.AreEqual("role", doc.DocumentElement.Attributes["id"].Value, "The results root id value did not match expectations.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"role\"/role[@name=\"Administrator\" and @mask=\"6271\"]"), "The returned results did not include an instance of the Administrator role.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"role\"/role[@name=\"Physician\" and @mask=\"19823\"]"), "The returned results did not include an instance of the Physician role.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"role\"/role[@name=\"Operator\" and @mask=\"111\"]"), "The returned results did not include an instance of the Operator role.");
    } // GtRole_TestMultiName()


    /// <summary>
    /// Tests retrieving a role by its permissions.
    /// </summary>
    [TestMethod]
    public void GtRole_TestPrms()
    {
      string results = SecurityGateway.GetRole(m_serviceContextId, "<role prms=\"6271\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The name of the root node returned did not match expectations.");
      Assert.AreEqual("role", doc.DocumentElement.Attributes["id"].Value, "The id value of the root node returned did not match expectations.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"role\"]/role[@name=\"Administrator\" and @prms=\"6271\"]").Count, "The results did not match expectations.");
    } // GtRole_TestPrms()


    /// <summary>
    /// Gets a role by it's description.
    /// </summary>
    [TestMethod]
    public void GtRole_TestDescription()
    {
      string results = SecurityGateway.GetRole(m_serviceContextId, "<role desc=\"Operator/Administrator role\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The name of the root node returned did not match expectations.");
      Assert.AreEqual("role", doc.DocumentElement.Attributes["id"].Value, "The id value of the root node returned did not match expectations.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"role\"]/role[@name=\"Op-Admin\" and @prms=\"6271\"]").Count, "The results did not match expectations.");
    } // GtRole_TestDescription()

    #endregion

  } // class GetRoleTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
