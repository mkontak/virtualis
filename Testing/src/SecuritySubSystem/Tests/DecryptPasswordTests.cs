using System;
using System.Data;
using System.Data.Common;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Data.SqlTypes;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.SecuritySubSystem.Tests
{
  /// <summary>
  /// Tests the decrypt password stored proc.
  /// </summary>
  [TestClass]
  public class DecryptPasswordTests
  {

    #region methods

    /// <summary>
    /// Tests a valid decrypt.
    /// </summary>
    [TestMethod]
    public void DcrptPwd_TestValidDecrypt()
    {
      Assert.AreEqual("password1!", SecurityGateway.DecryptPassword("9GNiloAASGzFwPXmXAFD4g=="), "The decrypted value did not match expectations.");
    } // DcrptPwd_TestValidDecrypt()


    /// <summary>
    /// Tests a invalid decrypt input value.
    /// </summary>
    [TestMethod]
    public void DcrptPwd_TestInvalidDecryptParam()
    {
      try
      {
        SecurityGateway.DecryptPassword("a");
        Assert.Fail("No error was emmitted when trying to decrypt an invalid value.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // DcrptPwd_TestInvalidDecryptParam()

    #endregion

  } // class DecryptPasswordTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
