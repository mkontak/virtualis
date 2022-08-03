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
  /// Tests the encryption stored procedure.
  /// </summary>
  [TestClass]
  public class EncryptTests
  {

    #region methods

    /// <summary>
    /// Performs initialization before each test is run.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests an encrypt and validates that the encrypted value matches expectations.
    /// </summary>
    [TestMethod]
    public void Encpt_TestSuccess()
    {
      Assert.AreEqual("9GNiloAASGzFwPXmXAFD4g==", SecurityGateway.Encrypt("password1!"), "The encrypted value did not match expectations.");
    } // Encpt_TestSuccess()


    /// <summary>
    /// Tests the encrypt against the passwords table.
    /// </summary>
    [TestMethod]
    public void Encrpt_TestAgainstPasswordTable()
    {
      Guid serviceContextId = Guid.Empty;

      string username = "user01";

      try
      {
        serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);

        SecurityGateway.InsertUser(serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Passwords] WHERE [v3d].[Passwords].[login] = \'{0}\' ORDER BY [v3d].[Passwords].[date]", username));

          DataSet ds = database.ExecuteDataSet(cmd);
          Assert.AreNotEqual(0, ds.Tables.Count, "The dataset result does not match expectations.");
          Assert.AreNotEqual(0, ds.Tables[0].Rows.Count, "The dataset result does not match expectations.");
          Assert.AreEqual(SecurityGateway.Encrypt("tester01$"), ds.Tables[0].Rows[0]["pwd"], "The dataset result does not match expectations.");
        }
      }
      finally
      {
        // Delete the user.
        SecurityGateway.DeleteUser(serviceContextId, username);

        // Release the service context.
        SecurityGateway.ReleaseContext(serviceContextId, SecurityGateway.SRVCUSERNAME);
      }
    } // Encrpt_TestAgainstPasswordTable()

    #endregion

  } // class EncryptTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
