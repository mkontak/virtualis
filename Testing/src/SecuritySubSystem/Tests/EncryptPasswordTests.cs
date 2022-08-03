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
  /// Tests the encrypt password.
  /// </summary>
  [TestClass]
  public class EncryptPasswordTests
  {

    #region contstants

    /// <summary>
    /// The service context.
    /// </summary>
    private static Guid m_serviceContext = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before the class's tests are run.
    /// </summary>
    /// <param name="context">The test context.</param>
    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
      m_serviceContext = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);
    } // ClassInitialize( context )


    /// <summary>
    /// Performs cleanup after the class's tests are run.
    /// </summary>
    [ClassCleanup]
    public static void ClassCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContext, SecurityGateway.SRVCUSERNAME);
    } // ClassCleanup()


    /// <summary>
    /// Tests a successful encrypt.
    /// </summary>
    [TestMethod]
    public void EncptPwd_TestAgainstPasswordTable()
    {
      try
      {
        SecurityGateway.InsertUser(m_serviceContext, "user01", "He is Superman", "tester01$", SecurityGateway.UserStates.Active, DateTime.Now.AddYears(300), "Physician");

        string encrptPassword = SecurityGateway.EncryptPassword("user01", "tester01$");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[Passwords]");
          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.IsTrue(ds.Tables.Count > 0, "The table count coming back from the password select did not match expectations.");
          Assert.IsTrue(ds.Tables[0].Rows.Count > 0, "The row count coming back from the password select did not match expectations.");
          Assert.AreEqual(ds.Tables[0].Rows[0]["pwd"], encrptPassword, "The encrypted password did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContext, "user01");
      }
    } // EncptPwd_TestAgainstPasswordTable()

    #endregion

  } // class EncryptPasswordTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
