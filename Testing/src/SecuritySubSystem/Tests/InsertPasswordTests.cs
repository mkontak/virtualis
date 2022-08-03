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
  /// Validates the insert password stored proc.
  /// </summary>
  [TestClass]
  public class InsertPasswordTests
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
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a valid insertion of a password.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestValid()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tster01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "abcdefgh1!");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestValid()


    /// <summary>
    /// Tests invalid login parameter values.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestInvalidLogin()
    {
      try
      {
        SecurityGateway.InsertPassword(null, "superman1!");
        Assert.Fail("No exception was emmitted when trying to provide a null for the userame.");
      }
      catch(SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }

      try
      {
        SecurityGateway.InsertPassword(string.Empty, "superman1!");
        Assert.Fail("No exception was emmitted when trying to provide a \"\" for the userame.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsPwd_TestInvalidLogin()


    /// <summary>
    /// Tests invalid password parameter values.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestInvalidPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman1!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        try
        {
          SecurityGateway.InsertPassword(username, null);
          Assert.Fail("No exception was emmitted when trying to provide a null for the password.");
        }
        catch (SqlException e)
        {
          int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
          Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
        }

        try
        {
          SecurityGateway.InsertPassword(username, string.Empty);
          Assert.Fail("No exception was emmitted when trying to provide a \"\" for the password.");
        }
        catch (SqlException e)
        {
          int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
          Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
        }
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestInvalidPassword()


    /// <summary>
    /// Tests inserting a password on an account that doesn't exist.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestAccountNotFound()
    {
      try
      {
        SecurityGateway.InsertPassword("user01", "tester01$");
        Assert.Fail("No exception was emmitted when trying to insert a password on an account that doesn't exit.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900991, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // InsPwd_TestAccountNotFound()


    /// <summary>
    /// Tests inserting a password on a user with a password template that doesn't exist.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestPasswordTemplateNotFound()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand(string.Format(
            "UPDATE [v3d].[Users] SET [v3d].[Users].[ptmp] = \'invalidptmp\' WHERE [v3d].[Users].[login] = \'{0}\'",
            username));

          database.ExecuteNonQuery(cmd);
        }

        SecurityGateway.InsertPassword(username, "abcdefg1!");
        Assert.Fail("The database did not emmit an error when trying to change the password when the user had an invalid template.");
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
    } // InsPwd_TestPasswordTemplateNotFound()


    /// <summary>
    /// Tests inserting a reused password according to the standard template rules.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestValidReuse()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "abcdefg1!");
        SecurityGateway.InsertPassword(username, "tester01$");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestValidReuse()


    /// <summary>
    /// Tests that insertPassword detects when the user tries to reuse a previous password.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestCannotReusePassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "tester01$");
        Assert.Fail("The database did not emmit an error when trying to insert a password that doesn't enough different characters in the password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901221, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestCannotReusePassword()


    /// <summary>
    /// Tests that insert password notices that the password differs by at least x number of characters.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestPasswordDiffersByXNumberOfCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "abcdefg1!",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "standard");

        SecurityGateway.InsertPassword(username, "abcdefg2@");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestPasswordDoesNotDifferByXNumberOfCharacters()


    /// <summary>
    /// Tests that insert password notices that the password does not differ by x number of characters.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestPasswordDoesNotDifferByXNumberOfCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "abcdefg1!",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "standard");

        SecurityGateway.InsertPassword(username, "abcdefg2!");
        Assert.Fail("No exception was emitted when trying to set the password that does not differ by x chars.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901213, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestPasswordDoesNotDifferByXNumberOfCharacters()


    /// <summary>
    /// Tests exceeding the password change allowed.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestExceededFrequency()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "hippa");

        // It should error here becaue the initial change occurred when adding the user, 
        SecurityGateway.InsertPassword(username, "abcdefg1!");
        Assert.Fail("The database did not emmit an error when trying to change the password too often.");
     }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901222, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestExceededFrequency()


    /// <summary>
    /// Tests that the database notices when the login is embedded in the password.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestLoginCannotBeEmbeddedInPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "suser01$");
        Assert.Fail("The database did not emmit an error when trying to embed the username in the password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901214, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestLoginCannotBeEmbeddedInPassword()


    /// <summary>
    /// Test exceeding the maximum characters for the password template.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestExceededMaximumCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "abcdefghijklmnopqrstuvwxyz1!");
        Assert.Fail("The database did not emmit an error when trying to exceed the maximum number of characters in the password.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901226, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestExceededMaximumCharacters()


    /// <summary>
    /// Test having a password less than the minimum number of characters.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestLessThanMinimumCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "abc1!");
        Assert.Fail("The database did not emmit an error when trying to insert a password than has less than the minimum number of characters.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901225, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestLessThanMinimumCharacters()


    /// <summary>
    /// Tests inserting a password that has spaces and expect an exception.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestPasswordCannotContainSpaces()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "abcdef  g1!");
        Assert.Fail("The database did not emmit an error when trying to insert a password that has spaces.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901217, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestPasswordCannotContainSpaces()


    /// <summary>
    /// Tests inserting a password without enough special characters.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestNotEnoughSpecialCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "abcdefgh1");
        Assert.Fail("The database did not emmit an error when trying to insert a password that doesn't have enough special characters.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901225, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestNotEnoughSpecialCharacters()


    /// <summary>
    /// Tests inserting a password without enough numeric characters.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestNotEnoughNumericCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        SecurityGateway.InsertPassword(username, "abcdefgh!");
        Assert.Fail("The database did not emmit an error when trying to insert a password that doesn't have enough numeric characters.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901225, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestNotEnoughNumericCharacters()


    /// <summary>
    /// Tests not enough uppercase characters in the password.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestNotEnoughUppercaseCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "dod");

        SecurityGateway.InsertPassword(username, "abcdefgh1!");
        Assert.Fail("The database did not emmit an error when trying to insert a password that doesn't have enough uppercase characters.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901225, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestNotEnoughUppercaseCharacters()


    /// <summary>
    /// Tests not enough lower characters in the password.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestNotEnougLowercaseCharacters()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "dod");

        SecurityGateway.InsertPassword(username, "ABCDEFG1!");
        Assert.Fail("The database did not emmit an error when trying to insert a password that doesn't have enough uppercase characters.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901225, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestNotEnougLowercaseCharacters()


    /// <summary>
    /// Tests too many repititions in the password.
    /// </summary>
    [TestMethod]
    public void InsPwd_TestTooManyRepitionsInPassword()
    {
      string username = "user01";

      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, username, "He is Superman!", "tester01$",
          SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician", "dod");

        SecurityGateway.InsertPassword(username, "aabcdefg1!");
        Assert.Fail("The database did not emmit an error when trying to insert a password that doesn't have enough uppercase characters.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901225, errorCode, "The expected errorcode returned didn't match expectations.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, username);
      }
    } // InsPwd_TestTooManyRepitionsInPassword()

    #endregion

  } // class InsertPasswordTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
