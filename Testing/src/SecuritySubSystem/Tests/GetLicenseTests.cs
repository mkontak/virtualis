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
  /// Validates the get license stored proc.
  /// </summary>
  [TestClass]
  public class GetLicenseTests
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
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      LicensingGateway.ClearRegistration();

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests retrieving all licenses.
    /// </summary>
    [TestMethod]
    public void GetLic_TestRetrieveAll()
    {
      int licensesAdded = 2;
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", licensesAdded, 120);

      try
      {
        string results = SecurityGateway.GetLicense(m_serviceContextId, "<license />");

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(results);

        Assert.AreEqual("collection", doc.DocumentElement.Name, "The xml node name of the results did not match expectations.");
        Assert.AreEqual(licensesAdded, doc.DocumentElement.SelectNodes("/collection[@id=\"license\"]/license").Count,
          "The xml node name of the results did not match expectations.");
      }
      finally
      {
        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestRetrieveAll()


    /// <summary>
    /// Tests retrieving licenses without the valid permissions.
    /// </summary>
    [TestMethod]
    public void GetLic_TestInvalidPermissions()
    {
      int licensesAdded = 2;
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", licensesAdded, 120);
      
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        string results = SecurityGateway.GetLicense(contextId, "<license />");
        Assert.Fail("No exception was emmitted when trying to call getLicenses without the proper authorization.");
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

        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestInvalidPermissions()


    /// <summary>
    /// Tests retrieving licenses without the valid permissions.
    /// </summary>
    [TestMethod]
    public void GetLic_TestValidPermissions()
    {
      int licensesAdded = 2;
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", licensesAdded, 120);

      string username = "admin01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Administrator");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        string results = SecurityGateway.GetLicense(contextId, "<license />");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);

        LicensingGateway.ClearLicenses();
      }
    } // GetLic_TestInvalidPermissions()

    #endregion

  } // class GetLicenseTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
