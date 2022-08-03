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
  /// Validatation of the ValidateContext stored proc.
  /// </summary>
  [TestClass]
  public class ValidateContextTests
  {

    #region fields

    /// <summary>
    /// The service user's context id.
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
    } // TestInitialize( context )


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
    /// Validate that ValidateContext with a context that doesn't exist.
    /// </summary>
    [TestMethod]
    public void ValCxt_TestContextNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now, "Physician");

        SecurityGateway.ValidateContext(username, Guid.Empty);
        Assert.Fail("No exception was emmitted by the database when trying to validate a context that doesn't exist.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // ValCxt_TestContextNotFound()


    /// <summary>
    /// Tests a valid context, but an invalid login.
    /// </summary>
    [TestMethod]
    public void ValCxt_TestLoginNotFound()
    {
      try
      {
        SecurityGateway.ValidateContext("user01", m_serviceContextId);
        throw new Exception("No exception was emmitted by the database when trying to validate a context with no login provided.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ValCxt_TestLoginNotFound()


    /// <summary>
    /// Tests an expired user.
    /// </summary>
    [TestMethod]
    public void ValCxt_TestExpiredUser()
    {
      string username = "user01";
      Guid contextId = Guid.Empty;

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is superman!", "tester01$", 
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        contextId = SecurityGateway.AcquireContext(username, "tester01$");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "UPDATE [v3d].[Users] SET [v3d].[Users].[exp] = \'{0}\' WHERE [v3d].[Users].[login] = \'{1}\'",
            DateTime.Now.Subtract(new TimeSpan(1, 0, 0)).ToString("o"), username));

          database.ExecuteNonQuery(cmd);
        }

        SecurityGateway.ValidateContext(username, contextId);
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.ReleaseContext(contextId, username);
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // ValCxt_TestExpiredUser()

    #endregion

  } // class ValidateContextTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
