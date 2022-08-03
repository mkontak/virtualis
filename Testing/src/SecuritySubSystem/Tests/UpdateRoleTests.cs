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
  /// Validates the UpdateRole stored proc.
  /// </summary>
  [TestClass]
  public class UpdateRoleTests
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
    /// Tests a valid update of a role.
    /// </summary>
    [TestMethod]
    public void UpdRol_TestValidRoleUpdate()
    {
      string roleName = "goofball";

      try
      {
        SecurityGateway.InsertRole(m_serviceContextId, roleName, "A test role", 2222);

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Roles] WHERE [name] = \'{0}\'", roleName));
          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of rows did not match expectations.");
          Assert.AreEqual(2222, Convert.ToInt32(ds.Tables[0].Rows[0]["prms"]), "The permissions  mask did not match expectations on insert.");
          Assert.AreEqual("A test role", ds.Tables[0].Rows[0]["desc"], "The description did not match expectations on insert.");
        }

        SecurityGateway.UpdateRole(m_serviceContextId, roleName, "New Description", 2468);

        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Roles] WHERE [name] = \'{0}\'", roleName));
          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of rows did not match expectations.");
          Assert.AreEqual(2468, Convert.ToInt32(ds.Tables[0].Rows[0]["prms"]), "The permissions  mask did not match expectations on update.");
          Assert.AreEqual("New Description", ds.Tables[0].Rows[0]["desc"], "The description did not match expectations on update.");
        }
      }
      finally
      {
        SecurityGateway.DeleteRole(m_serviceContextId, roleName);
      }
    } // UpdRol_TestValidUpdate()


    /// <summary>
    /// Tests invalid permissions when updating a role.
    /// </summary>
    [TestMethod]
    public void UpdRol_TestInvalidPermissions()
    {
      string username = "user01";
      string roleName = "goofball";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertRole(m_serviceContextId, roleName, "Original Description", 2222);
        SecurityGateway.UpdateRole(contextId, roleName, "New Description", 2468);
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

        SecurityGateway.DeleteRole(m_serviceContextId, roleName);
      }
    } // UpdRol_TestInvalidPermissions()

    #endregion

  } // class UpdateRoleTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
