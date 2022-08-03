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
  /// Validates the isAuthorized stored proc.
  /// </summary>
  [TestClass]
  public class IsAuthorizedTests
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
    /// Tests a context that is not found.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestContextNotFound()
    {
      try
      {
        SecurityGateway.IsAuthorized(Guid.Empty, SecurityGateway.Rights.ViewReport);
        Assert.Fail("No exception was thrown when trying to authorize a user that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // IsAuth_TestLoginNotFound()


    /// <summary>
    /// Tests an expired context for authorization.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestExpiredContext()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$", null, -15);

        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ReportingManagement);
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        // Don't release context, it's expired.
        //SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.CleanupExpiredContexts();

        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestExpiredContext()


    /// <summary>
    /// Tests a user not being authorized.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestNotAuthorized()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "he is superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Operator");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.AuditingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestNotAuthorized()
    

    /// <summary>
    /// Tests the administrator role.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestAdministratorRole()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Administrator");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Valid rights.
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.UserManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.StorageManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ProcessingManagement);

        // Restricted rights.
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.AuditingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.ReportingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.FindingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestAdministratorRole()


    /// <summary>
    /// Tests the operator role.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestOperatorRole()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Operator");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Valid rights.
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.StorageManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ProcessingManagement);

        // Restricted rights.
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.UserManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.AuditingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.ReportingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.FindingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestOperatorRole()


    /// <summary>
    /// Tests the physician role.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestPhysicianRole()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Valid rights.
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.StorageManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ProcessingManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ReportingManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.FindingManagement);

        // Restricted rights.
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.UserManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.AuditingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestPhysicianRole()


    /// <summary>
    /// Tests the service role.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestServiceRole()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Service");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Valid rights.
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.UserManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.StorageManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ProcessingManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.AuditingManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ReportingManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.FindingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestServiceRole()


    /// <summary>
    /// Tests the operator admin role.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestOpAdminRole()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Op-Admin");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Valid rights.
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.UserManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.StorageManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ProcessingManagement);

        // Restricted rights.
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.AuditingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.ReportingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.FindingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestOpAdminRole()


    /// <summary>
    /// Tests the physician admin role.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestPhyAdminRole()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Phy-Admin");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Valid rights.
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.UserManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.StorageManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ProcessingManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.ReportingManagement);
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.FindingManagement);

        // Restricted rights.
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.AuditingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestOpAdminRole()


    /// <summary>
    /// Tests the auditor role.
    /// </summary>
    [TestMethod]
    public void IsAuth_TestAuditorRole()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Auditor");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Valid rights.
        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.AuditingManagement);

        // Restricted rights.
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.UserManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.StorageManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.ProcessingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.ReportingManagement);
        TestNotAuthorizedRight(contextId, SecurityGateway.Rights.FindingManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // IsAuth_TestAuditorRole()


    /// <summary>
    /// Tests a failed right.
    /// </summary>
    /// <param name="context">The user's context.</param>
    /// <param name="right">The right to check.</param>
    private void TestNotAuthorizedRight(Guid context, SecurityGateway.Rights right)
    {
      try
      {
        SecurityGateway.IsAuthorized(context, right);
        Assert.Fail(string.Format("The user was allowed access to a right he sholdn't have.  [RIGHT=\"{0}\"]", right.ToString()));
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901201, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // TestNotAuthorizedRight( context, username, right )

    #endregion

  } // class IsAuthorizedTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
