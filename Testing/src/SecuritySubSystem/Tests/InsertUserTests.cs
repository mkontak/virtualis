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
  /// Validates the InsertUser stored proc.
  /// </summary>
  [TestClass]
  public class InsertUserTests
  {

    #region fields

    /// <summary>
    /// The service context id.
    /// </summary>
    private static Guid m_serviceContextid = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Perfroms initialization before each test is run.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();

      m_serviceContextid = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextid, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a valid user insert.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestValidInsert()
    {
      string username = "userA";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextid, username, "dfadsfdf adsfdf", "abcdefghij1!",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          // Verify that the user was added.

          DbCommand cmd = database.GetSqlStringCommand(string.Format("SELECT * FROM [v3d].[Users] WHERE [v3d].[Users].[login] = \'{0}\'", username));
          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.IsTrue(ds.Tables.Count > 0, "The dataset does not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The row count does not match expectations");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextid, username);
      }
    } // InsUsr_TestValidInsert()


    /// <summary>
    /// Tests a double insert of the user and expects a user already exists error.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestDoubleInsert()
    {
      int attempts = 0;
      string username = "user01";

      try
      {
        // First insert, we expect no problem on this guy.
        ++attempts;
        SecurityGateway.InsertUser(m_serviceContextid, username, "userA", "tester01!",
          SecurityGateway.UserStates.Available, DateTime.Now, "Physician");
        
        // Second insert, we expect a problem here.
        ++attempts;
        SecurityGateway.InsertUser(m_serviceContextid, username, "userA", "tester01!",
          SecurityGateway.UserStates.Available, DateTime.Now, "Physician");

        Assert.Fail("No exception was emitted from the database when trying to add the same user twice.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(2, attempts, "The number of attempts did not match expectations.");
        Assert.AreEqual(900992, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextid, username);
      }
    } // InsUsr_TestDoubleInsert()


    /// <summary>
    /// Tests inserting a user with a role that doesn't exist.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestRoleNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextid, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Goofy!");

        Assert.Fail("Databse did not emmit an error when inserting a user with a role that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsUsr_TestRoleNotFound()


    /// <summary>
    /// Tests inserting a user with out specifying the password.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestInsertWithoutPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextid, username, "He is Superman!", null,
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Assert.Fail("Databse did not emmit an error when inserting a user without a password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsUsr_TestInsertWithoutPassword()


    /// <summary>
    /// Tests inserting a user with a password template that doesn't exist.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestInvalidPasswordTemplate()
    {
      try
      {
        SecurityGateway.InsertUser(m_serviceContextid, "user01", "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "goofypasswordssss");

        Assert.Fail("Database did not emmit an exception when trying to insert a user with a password template that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsUsr_TestInvalidPasswordTemplate()


    /// <summary>
    /// Tests a user being inserted with the base password template, with a password that violates that template.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestPasswordInvalidAgainstBaseSecurityTemplate()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextid, username, "He is Superman!", "abcdefg!",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Assert.Fail("Database did not emmitt an exception when attempting to insert a user that doesn't have any numeric characters.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901225, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextid, username);
      }
    } // InsUsr_TestPasswordInvalidAgainstBaseSecurityTemplate()


    /// <summary>
    /// Tests a successful insert of a user and then checks the audit login for a sucess.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestAuditingLog()
    {
      string username = string.Empty;

      try
      {
        username = "user01";

        DateTimeOffset start = LicensingGateway.GetDatabaseDateTime().AddYears(300).Subtract(new TimeSpan(0, 0, 1));

        SecurityGateway.InsertUser(m_serviceContextid, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        DateTimeOffset end = LicensingGateway.GetDatabaseDateTime().AddYears(300).AddSeconds(1);

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "SELECT TOP 1 * FROM [Auditing].[v3d].[UserHistory] WHERE [login] = \'{0}\' AND [action]=\'{1}\' ORDER BY [date] DESC",
            username, "insert"));

          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of tables did not match expectations.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of rows did not match expectations.");

          string results = ds.Tables[0].Rows[0]["ext"].ToString();

          XmlDocument doc = new XmlDocument();
          doc.LoadXml(results);

          DateTimeOffset date = DateTimeOffset.Parse(doc.DocumentElement.Attributes["exp"].Value);
          Assert.IsTrue(start < date && date < end, "The expiration date did not match expectations.");

          Assert.AreEqual(username, doc.DocumentElement.Attributes["login"].Value, "The username did not match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextid, username);
      }
    } // InsUsr_TestAuditingLog()


    /// <summary>
    /// Tests inserting a user without valid user permissions.
    /// </summary>
    [TestMethod]
    public void InsUsr_TestInvalidPermissions()
    {
      string username = "user01";
      string username2 = "user02";

      Guid contextId = Guid.Empty;

      try
      {
        LicensingGateway.RegisterLicenseDatabase();
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 30);

        SecurityGateway.InsertUser(m_serviceContextid, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        SecurityGateway.InsertUser(contextId, username2, "He is Superman2!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Assert.Fail("No error was emmitted when trying insert a user without the proper permissions");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901201, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextid, username);

        LicensingGateway.ClearLicenses();
        LicensingGateway.ClearRegistration();
      }
    } // InsUsr_TestInvalidPermissions()

    #endregion

  } // class InsertUserTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
