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
  /// Validates the delete role stored proc.
  /// </summary>
  [TestClass]
  public class DeleteRoleTests
  {

    #region fields

    /// <summary>
    /// The context id for the service user.
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
    /// Tests a valid delete and validate the role was deleted.
    /// </summary>
    [TestMethod]
    public void DelRol_TestValidDelete()
    {
      string roleName = "TestRole";

      SecurityGateway.InsertRole(m_serviceContextId, roleName, "This is a test role.", 222);
      Database database = SecurityGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format(
          "SELECT COUNT(*) FROM [v3d].[Roles] WHERE [name] = \'{0}\'", roleName));
        Assert.AreEqual(1, Convert.ToInt32(database.ExecuteScalar(cmd).ToString()), "The role deletion failed...");
      }
      
      SecurityGateway.DeleteRole(m_serviceContextId, roleName);
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(string.Format(
          "SELECT COUNT(*) FROM [v3d].[Roles] WHERE [name] = \'{0}\'", roleName));
        Assert.AreEqual(0, Convert.ToInt32(database.ExecuteScalar(cmd).ToString()), "The role deletion failed...");
      }
    } // DelRol_TestValidDelete()


    /// <summary>
    /// Tests with a context that doesn't exist.
    /// </summary>
    [TestMethod]
    public void DelRol_TestContextNotFound()
    {
      try
      {
        SecurityGateway.InsertRole(m_serviceContextId, "GoofBall", "New Role", 222);
        SecurityGateway.DeleteRole(new Guid(), "GoofBall");

        Assert.Fail("No exception was emmitted when trying to delete a role with a context id that doesn't exist in the system.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteRole(m_serviceContextId, "GoofBall");
      }
    } // DelRol_TestContextNotFound()


    /// <summary>
    /// Tests invalid permissions when trying to delete a role.
    /// </summary>
    [TestMethod]
    public void DelRol_TestInvalidPermissions()
    {
      string username = "user01";
      string roleName = "goofball";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertRole(m_serviceContextId, roleName, "desc", 2222);
        SecurityGateway.DeleteRole(contextId, roleName);

        Assert.Fail("No exception was emmitted when trying to delete a role with invalid permissions");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901201, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteRole(m_serviceContextId, roleName);
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // DelRol_TestInvalidPermissions()

    #endregion

  } // class DeleteRoleTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
