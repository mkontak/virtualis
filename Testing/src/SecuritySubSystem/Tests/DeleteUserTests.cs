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
  /// Validates the delete user stored proc.
  /// </summary>
  [TestClass]
  public class DeleteUserTests
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
    /// Tests deleting a user in a normal situation.
    /// </summary>
    [TestMethod]
    public void DelUsr_TestValidDelete()
    {
      string username = "user01";

      //
      // Insert the user and ensure that the database reflects the change.
      //

      SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
        SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

      Database database = SecurityGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(
          string.Format("SELECT * FROM [v3d].[USers] WHERE [login] = \'{0}\'", username));

        DataSet ds = database.ExecuteDataSet(cmd);

        Assert.AreEqual(1, ds.Tables.Count, "The number of tables returned did not match expectations.");
        Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of rows returned did not match expectations.");
        Assert.AreEqual(username, ds.Tables[0].Rows[0]["login"], "The username did not match expectations.");
      }

      //
      // Delete the user and ensure that the database reflects the change.
      //

      SecurityGateway.DeleteUser(m_serviceContextId, username);

      database = SecurityGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(
          string.Format("SELECT * FROM [v3d].[USers] WHERE [login] = \'{0}\'", username));

        DataSet ds = database.ExecuteDataSet(cmd);

        Assert.AreEqual(1, ds.Tables.Count, "The number of tables returned did not match expectations.");
        Assert.AreEqual(0, ds.Tables[0].Rows.Count, "The number of rows returned did not match expectations.");
      }
    } // DelUsr_TestValidDelete()


    /// <summary>
    /// Tests deleting a user without the valid permissions.
    /// </summary>
    [TestMethod]
    public void DelUsr_TestInvalidPermissions()
    {
      string username = "user01";
      string username2 = "user02";

      Guid contextId = Guid.Empty;

      try
      {
        //
        // Insert the user and acquire a context.
        //

        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 VIATRONIX", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        // Delay a second to prevent duplicate key in user history.
        System.Threading.Thread.Sleep(1000);

        SecurityGateway.InsertUser(m_serviceContextId, username2, "USER02 VIATRONIX", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        // Delay a second to prevent duplicate key in user history.
        System.Threading.Thread.Sleep(1000);

        //
        // Try to delete the second user with the context of the first.
        //

        SecurityGateway.DeleteUser(contextId, username2);

        Assert.Fail("No exception was emmitted when attempting to delete a user without permission.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901201, errorCode, "The expected error code did not match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);

        SecurityGateway.DeleteUser(m_serviceContextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username2);
      }
    } // DelUsr_TestInvalidPermissions()


    /// <summary>
    /// Tests deleting a user that has a context.
    /// </summary>
    [TestMethod]
    public void DelUsr_TestUserHasContext()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

        contextId = SecurityGateway.AcquireContext(username, "tester01$");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(2, 4));
        Assert.AreEqual(901213, errorCode, "The expected error code did not match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // DelUsr_TestUserHasContext()


    /// <summary>
    /// Tests deleting a user with the valid permissions.
    /// </summary>
    [TestMethod]
    public void DelUsr_TestValidPermissions()
    {
      string username = "user01";
      string administrator = "admin01";
      Guid contextId = Guid.Empty;

      SecurityGateway.InsertUser(m_serviceContextId, username, "USER01 Viatronix", "tester01$",
        SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300));

      SecurityGateway.InsertUser(m_serviceContextId, administrator, "USER02 Viatronix", "tester01$",
        SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Administrator");

      contextId = SecurityGateway.AcquireContext(administrator, "tester01$");

      SecurityGateway.DeleteUser(contextId, username);

      SecurityGateway.ReleaseContext(contextId, administrator);
      SecurityGateway.DeleteUser(m_serviceContextId, administrator);
    } // DelUsr_TestValidPermissions()

    #endregion

  } // class DeleteUserTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
