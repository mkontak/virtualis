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
  /// Validates the addRole stored proc.
  /// </summary>
  [TestClass]
  public class InsertRoleTests
  {

    #region fields

    /// <summary>
    /// Service's context id.
    /// </summary>
    private static Guid m_serviceContextId = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before the class' tests are run.
    /// </summary>
    /// <param name="context">The test context.</param>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();

      m_serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);

      LicensingGateway.RegisterLicenseDatabase();
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);
    } // TestInitialize( context )


    /// <summary>
    /// Performs cleanup after the class' tests are run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      m_serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);

      LicensingGateway.ClearLicenses();
      LicensingGateway.ClearRegistration();

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a valid insertion of a role.
    /// </summary>
    [TestMethod]
    public void InsRole_TestValidInsert()
    {
      string roleName = "Goofball";
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertRole(m_serviceContextId, roleName, "This is a test role...", 47);
        SecurityGateway.InsertUser(m_serviceContextId, username, "This is a test user...", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), roleName);

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.IsAuthorized(contextId, SecurityGateway.Rights.StorageManagement);
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);

        SecurityGateway.DeleteRole(m_serviceContextId, roleName);
      }
    } // InsRole_TestValidInsert()


    /// <summary>
    /// Tests inserting a role with a user that does not have the required permissions.
    /// </summary>
    [TestMethod]
    public void InsRole_TestInvalidPermissions()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertRole(contextId, "goofball", "Goofball is a role", 2222);
        Assert.Fail("No exception was emmitted when trying to insert a role without proper permissions.");
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
    } // InsRole_TestInvalidPermissions()


    /// <summary>
    /// Tests a double insertion of a role.
    /// </summary>
    [TestMethod]
    public void InsRole_TestDoubleInsert()
    {
      string roleName = "goofball";

      try
      {
        SecurityGateway.InsertRole(m_serviceContextId, roleName, "This is a test role...", 2222);
        SecurityGateway.InsertRole(m_serviceContextId, roleName, "This is a test role...", 2222);

        Assert.Fail("No exception was emmitted when trying to insert the same role twice.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900992, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteRole(m_serviceContextId, roleName);
      }
    } // InsRole_TestDoubleInsert()

    #endregion

  } // class InsertRoleTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
