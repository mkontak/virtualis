using System;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.SecuritySubSystem.Tests
{
  /// <summary>
  /// Tests the decrypt stored procedure.
  /// </summary>
  [TestClass]
  public class DecryptTests
  {

    #region methods

    /// <summary>
    /// Tests a decrypt and verifies the value.
    /// </summary>
    [TestMethod]
    public void Dcrpt_TestSuccess()
    {
      Assert.AreEqual("password1!", SecurityGateway.Decrypt("9GNiloAASGzFwPXmXAFD4g=="), "The decrypted value did not match expectations.");
    } // Dcrpt_TestSuccess()

    #endregion

  } // class DecryptTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
