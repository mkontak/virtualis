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
  /// Validates the windows authentication stored proc.
  /// </summary>
  [TestClass]
  public class WindowsAuthenticationTests
  {

    #region fields

    /// <summary>
    /// The context id for service.
    /// </summary>
    private static Guid m_securityContextId = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization before each test is run.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();

      m_securityContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);
    } // TestInitialize()


    /// <summary>
    /// Performs clean after each test is run.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_securityContextId, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests a valid login against the local system.
    /// </summary>
    [TestMethod]
    public void WinAuth_TestValidLocalLogin()
    {
      SecurityGateway.WindowsAuthentication(SecurityGateway.Decrypt("NlkL+Gm0NYWp/dVDbc1rNQ=="), SecurityGateway.Decrypt("LyJSDnjkqSMIRzO3RsQESA=="));
    } // WinAuth_TestValidLocalLogin()


    /// <summary>
    /// Tests a valid login against the domain.
    /// </summary>
    [TestMethod]
    public void WinAuth_TestValidDomainLogin()
    {
      string username = SecurityGateway.Decrypt("Av9ALOybltXmPUfIrfG2Uw==");
      string password = SecurityGateway.Decrypt("lCU4i0kEIHucpljr2CtmKQ==");

      SecurityGateway.WindowsAuthentication(username, password, "viatronix");
    } // WinAuth_TestValidDomainLogin()


    /// <summary>
    /// Tests a invalid login against the local system, username doesn't exit.
    /// </summary>
    [TestMethod]
    public void WinAuth_TestInvalidUsernameLocal()
    {
      try
      {
        SecurityGateway.WindowsAuthentication("Martha", "Washington");
        Assert.Fail("No exception was emitted when attempting to logon with an invalid username.");
      }
      catch (SqlException e)
      {
        Assert.IsTrue(e.Message.IndexOf("901200") > -1, "The expected errorcode returned didn't match expectations.");
      }
    } // WinAuth_TestValidLocalLogin()


    /// <summary>
    /// Tests a invalid password against the local system, password doesn't exit.
    /// </summary>
    [TestMethod]
    public void WinAuth_TestInvalidPasswordLocal()
    {
      try
      {
        SecurityGateway.WindowsAuthentication(SecurityGateway.Decrypt("NlkL+Gm0NYWp/dVDbc1rNQ=="), "Martha Washington");
        Assert.Fail("No exception was emitted when attempting to logon with an invalid username.");
      }
      catch (SqlException e)
      {
        Assert.IsTrue(e.Message.IndexOf("901200") > -1, "The expected errorcode returned didn't match expectations.");
      }
    } // WinAuth_TestInvalidPasswordLocal()


    /// <summary>
    /// Tests a invalid login against the domain system, username doesn't exit.
    /// </summary>
    [TestMethod]
    public void WinAuth_TestInvalidUsernameDomain()
    {
      try
      {
        SecurityGateway.WindowsAuthentication("Martha", "Washington", "viatronix");
        Assert.Fail("No exception was emitted when attempting to logon with an invalid username.");
      }
      catch (SqlException e)
      {
        Assert.IsTrue(e.Message.IndexOf("901200") > -1, "The expected errorcode returned didn't match expectations.");
      }
    } // WinAuth_TestInvalidUsernameDomain()


    /// <summary>
    /// Tests a invalid password against the domain, password doesn't exit.
    /// </summary>
    [TestMethod]
    public void WinAuth_TestInvalidPasswordDomain()
    {
      try
      {
        SecurityGateway.WindowsAuthentication(SecurityGateway.Decrypt("Av9ALOybltXmPUfIrfG2Uw=="), "Martha Washington", "viatronix");
        Assert.Fail("No exception was emitted when attempting to logon with an invalid username.");
      }
      catch (SqlException e)
      {
        Assert.IsTrue(e.Message.IndexOf("901200") > -1, "The expected errorcode returned didn't match expectations.");
      }
    } // WinAuth_TestInvalidPasswordDomain()

    #endregion

  } // class WindowsAuthenticationTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
