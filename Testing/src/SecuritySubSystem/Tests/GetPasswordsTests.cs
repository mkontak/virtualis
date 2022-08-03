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
  /// Tests the get passwords stored proc.
  /// </summary>
  [TestClass]
  public class GetPasswordsTests
  {

    #region contstants

    /// <summary>
    /// The service context id.
    /// </summary>
    private static Guid m_serviceContextId = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before this class's tests are run.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();

      m_serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup after this class's tests are run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a state in which user has no passwords and try to get the passwords.
    /// </summary>
    [TestMethod]
    public void GetPwd_TestNoPasswords()
    {
      try
      {
        SecurityGateway.InsertUser((Guid)m_serviceContextId, "user01", "a", "tester01$", SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("DELETE FROM [v3d].[Passwords]");
          database.ExecuteNonQuery(cmd);
        }

        Assert.AreEqual("<passwords />", SecurityGateway.GetPasswords("user01"));
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
      }
    } // GetPwd_TestNoPasswords()


    /// <summary>
    /// Tests a state in which user has a single passwords and try to get the passwords.
    /// </summary>
    [TestMethod]
    public void GetPwd_TestSinglePassword()
    {
      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, "user01", "a", "tester01$", SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");

        string expectedPassword = SecurityGateway.Encrypt("tester01$");
        string passwords = SecurityGateway.GetPasswords("user01");

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(passwords);

        Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/passwords/password").Count);
        Assert.AreEqual(expectedPassword, doc.DocumentElement.SelectNodes("/passwords/password")[0].Attributes["pwd"].Value);
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
      }
    } // GetPwd_TestSinglePassword()


    /// <summary>
    /// Tests multiple passwords.
    /// </summary>
    [TestMethod]
    public void GetPwd_TestMultiplePasswords()
    {
      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, "user01", "a", "taster01$", SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");
        SecurityGateway.ChangePassword(m_serviceContextId, "user01", "taster01$", "tbster02$");
        SecurityGateway.ChangePassword(m_serviceContextId, "user01", "tbster02$", "tcster03$");

        string expectedPassword  = SecurityGateway.Encrypt("taster01$");
        string expectedPassword2 = SecurityGateway.Encrypt("tbster02$");
        string expectedPassword3 = SecurityGateway.Encrypt("tcster03$");

        string passwords = SecurityGateway.GetPasswords("user01");

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(passwords);

        Assert.AreEqual(3, doc.DocumentElement.SelectNodes("/passwords/password").Count);
        Assert.IsTrue(doc.DocumentElement.SelectSingleNode(string.Format("/passwords/password[@pwd=\"{0}\"]", expectedPassword)) != null, "The first password for user could not be found in the results.");
        Assert.IsTrue(doc.DocumentElement.SelectSingleNode(string.Format("/passwords/password[@pwd=\"{0}\"]", expectedPassword2)) != null, "The second password for user could not be found in the results.");
        Assert.IsTrue(doc.DocumentElement.SelectSingleNode(string.Format("/passwords/password[@pwd=\"{0}\"]", expectedPassword3)) != null, "The third password for user could not be found in the results.");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
      }
    } // GetPwd_TestMultiplePasswords()


    /// <summary>
    /// Tests retrieving all passwords for all users.
    /// </summary>
    [TestMethod]
    public void GetPwd_TestAllUsers()
    {
      try
      {
        SecurityGateway.InsertUser(m_serviceContextId, "user01", "USER01 Viatronix", "taster01$", SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");
        SecurityGateway.ChangePassword(m_serviceContextId, "user01", "taster01$", "tbster02$");
        SecurityGateway.ChangePassword(m_serviceContextId, "user01", "tbster02$", "tcster03$");
        
        SecurityGateway.InsertUser(m_serviceContextId, "user02", "USER02 Viatronix", "taster01$", SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");
        SecurityGateway.ChangePassword(m_serviceContextId, "user02", "taster01$", "tbster02$");
        SecurityGateway.ChangePassword(m_serviceContextId, "user02", "tbster02$", "tcster03$");
        
        SecurityGateway.InsertUser(m_serviceContextId, "user03", "USER03 Viatronix", "taster01$", SecurityGateway.UserStates.Available, DateTime.Now.AddYears(300), "Physician");
        SecurityGateway.ChangePassword(m_serviceContextId, "user03", "taster01$", "tbster02$");
        SecurityGateway.ChangePassword(m_serviceContextId, "user03", "tbster02$", "tcster03$");
        
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(SecurityGateway.GetPasswords());
        
        Assert.AreEqual(3, doc.DocumentElement.SelectNodes("/passwords/password[@login=\"user01\"]").Count, "The number of returned passwords for user01 did not match expectations");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user01\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("taster01$"))).Count, "The original ");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user01\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("tbster02$"))).Count, "The original ");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user01\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("tcster03$"))).Count, "The original ");

        Assert.AreEqual(3, doc.DocumentElement.SelectNodes("/passwords/password[@login=\"user02\"]").Count, "The number of returned passwords for user02 did not match expectations");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user02\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("taster01$"))).Count, "The original ");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user02\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("tbster02$"))).Count, "The original ");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user02\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("tcster03$"))).Count, "The original ");

        Assert.AreEqual(3, doc.DocumentElement.SelectNodes("/passwords/password[@login=\"user03\"]").Count, "The number of returned passwords for user03 did not match expectations");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user03\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("taster01$"))).Count, "The original ");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user03\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("tbster02$"))).Count, "The original ");
        Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format("/passwords/password[@login=\"user03\" and @pwd=\"{0}\"]", SecurityGateway.Encrypt("tcster03$"))).Count, "The original ");
      }
      finally
      {
        SecurityGateway.DeleteUser(m_serviceContextId, "user01");
        SecurityGateway.DeleteUser(m_serviceContextId, "user02");
        SecurityGateway.DeleteUser(m_serviceContextId, "user03");
      }
    } // GetPwd_TestAllUsers()

    #endregion

  } // class GetPasswordsTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
