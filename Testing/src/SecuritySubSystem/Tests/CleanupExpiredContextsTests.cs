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
  /// Tests the cleanupExpiredContexts stored proc.
  /// </summary>
  [TestClass]
  public class CleanupExpiredContextsTests
  {

    #region fields

    /// <summary>
    /// The service context Id.
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
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 3, 120);
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
    /// Tests having no contexts and calling cleanup.
    /// </summary>
    [TestMethod]
    public void ClnExpCxt_TestNoContexts()
    {
      SecurityGateway.CleanupExpiredContexts();
    } // ClnExpCxt_TestNoContexts()


    /// <summary>
    /// Tests having active contexts in the system and calling the cleanup.
    /// </summary>
    [TestMethod]
    public void ClnExpCxt_TestNoExpiredContexts()
    {
      Guid contextUser01 = Guid.Empty;
      Guid contextUser02 = Guid.Empty;
      Guid contextUser03 = Guid.Empty;

      try
      {
        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
        SecurityGateway.DeleteUser(m_serviceContextId, "user02");
        SecurityGateway.DeleteUser(m_serviceContextId, "user03");

        SecurityGateway.InsertUser(m_serviceContextId, "user01", "USER 01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUser(m_serviceContextId, "user02", "USER 02 Viatronix", "tester02$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUser(m_serviceContextId, "user03", "USER 03 Viatronix", "tester03$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        contextUser01 = SecurityGateway.AcquireContext("user01", "tester01$");
        contextUser02 = SecurityGateway.AcquireContext("user02", "tester02$");
        contextUser03 = SecurityGateway.AcquireContext("user03", "tester03$");

        SecurityGateway.CleanupExpiredContexts();

        int activeContexts = 0;

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("SELECT COUNT(*) FROM [v3d].[Contexts]");
          activeContexts = Convert.ToInt32(database.ExecuteScalar(cmd).ToString());
        }

        Assert.AreEqual(4, activeContexts, "The number of contexts in the database did not match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextUser01, "user01");
        SecurityGateway.ReleaseContext(contextUser02, "user02");
        SecurityGateway.ReleaseContext(contextUser03, "user03");

        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
        SecurityGateway.DeleteUser(m_serviceContextId, "user02");
        SecurityGateway.DeleteUser(m_serviceContextId, "user03");
      }
    } // ClnExpCxt_TestNoExpiredContexts()


    /// <summary>
    /// Tests having no active contexts in the system and calling the cleanup.
    /// </summary>
    [TestMethod]
    public void ClnExpCxt_TestAllExpiredContexts()
    {
      Guid contextIdUser01 = Guid.Empty;
      Guid contextIdUser02 = Guid.Empty;
      Guid contextIdUser03 = Guid.Empty;

      try
      {
        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
        SecurityGateway.DeleteUser(m_serviceContextId, "user02");
        SecurityGateway.DeleteUser(m_serviceContextId, "user03");

        SecurityGateway.InsertUser(m_serviceContextId, "user01", "USER 01 Viatronix", "tester01$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUser(m_serviceContextId, "user02", "USER 02 Viatronix", "tester02$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUser(m_serviceContextId, "user03", "USER 03 Viatronix", "tester03$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        contextIdUser01 = SecurityGateway.AcquireContext("user01", "tester01$", null, -1);
        contextIdUser02 = SecurityGateway.AcquireContext("user02", "tester02$", null, -1);
        contextIdUser03 = SecurityGateway.AcquireContext("user03", "tester03$", null, -1);

        SecurityGateway.CleanupExpiredContexts();

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[Contexts]");
          DataSet ds = database.ExecuteDataSet(cmd);

          // The only context that should come back is the service context.
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of contexts in the database did not match expectations.");
          Assert.AreEqual(m_serviceContextId, Guid.Parse(ds.Tables[0].Rows[0]["id"].ToString()), "The number of contexts in the database did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.ReleaseContext(Guid.Empty, "user01");
        SecurityGateway.ReleaseContext(Guid.Empty, "user02");
        SecurityGateway.ReleaseContext(Guid.Empty, "user03");

        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
        SecurityGateway.DeleteUser(m_serviceContextId, "user02");
        SecurityGateway.DeleteUser(m_serviceContextId, "user03");
      }
    } // ClnExpCxt_TestNoExpiredContexts()


    /// <summary>
    /// Tests having a single expired context in the system and calling the cleanup.
    /// </summary>
    [TestMethod]
    public void ClnExpCxt_TestSingleExpiredContexts()
    {
      Guid contextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);

      try
      {
        SecurityGateway.ReleaseContext(Guid.Empty, "user01");
        SecurityGateway.ReleaseContext(Guid.Empty, "user02");
        SecurityGateway.ReleaseContext(Guid.Empty, "user03");

        SecurityGateway.DeleteUser(contextId, "user01");
        SecurityGateway.DeleteUser(contextId, "user02");
        SecurityGateway.DeleteUser(contextId, "user03");

        SecurityGateway.InsertUser(contextId, "user01", "USER 01 Viatronix", "tester01$", 
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        SecurityGateway.InsertUser(contextId, "user02", "USER 02 Viatronix", "tester02$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");
        
        SecurityGateway.InsertUser(contextId, "user03", "USER 03 Viatronix", "tester03$",
          SecurityGateway.UserStates.Available, DateTimeOffset.Now.AddYears(300), "Physician");

        Guid expiredContextId = SecurityGateway.AcquireContext("user01", "tester01$", null, -1);
        Guid activeContextId  = SecurityGateway.AcquireContext("user02", "tester02$", null, 5);
        Guid activeContextId2 = SecurityGateway.AcquireContext("user03", "tester03$", null, 5);

        SecurityGateway.CleanupExpiredContexts();

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("SELECT COUNT(*) FROM [v3d].[Contexts]");
          Assert.AreEqual(3, Convert.ToInt32(database.ExecuteScalar(cmd).ToString()),
            "The number of contexts in the database did not match expectations.");

          cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Contexts] WHERE [id] = \'{0}\'", activeContextId));
          DataSet ds = database.ExecuteDataSet(cmd);
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of contexts returned for user02 did not match expectations.");
          Assert.AreEqual(activeContextId, Guid.Parse(ds.Tables[0].Rows[0]["id"].ToString()), "The context id did not match expectations for user02");

          cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Contexts] WHERE [id] = \'{0}\'", activeContextId2));
          ds = database.ExecuteDataSet(cmd);
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of contexts returned for user03 did not match expectations.");
          Assert.AreEqual(activeContextId2, Guid.Parse(ds.Tables[0].Rows[0]["id"].ToString()), "The context id did not match expectations for user03");

          cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Contexts] WHERE [id] = \'{0}\'", expiredContextId));
          ds = database.ExecuteDataSet(cmd);
          Assert.AreEqual(0, ds.Tables[0].Rows.Count, "The number of contexts returned for user01 did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.ReleaseContext(Guid.Empty, "user01");
        SecurityGateway.ReleaseContext(Guid.Empty, "user02");
        SecurityGateway.ReleaseContext(Guid.Empty, "user03");

        SecurityGateway.DeleteUser(contextId, "user01");
        SecurityGateway.DeleteUser(contextId, "user02");
        SecurityGateway.DeleteUser(contextId, "user03");
      }
    } // ClnExpCxt_TestSingleExpiredContexts()

    #endregion

  } // class CleanupExpiredContextsTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
