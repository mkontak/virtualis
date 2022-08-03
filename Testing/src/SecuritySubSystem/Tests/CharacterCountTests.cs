using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.SecuritySubSystem.Tests
{
  /// <summary>
  /// Tests the character count.
  /// </summary>
  [TestClass]
  public class CharacterCountTests
  {

    #region methods

    /// <summary>
    /// Tests a single occurance.
    /// </summary>
    [TestMethod]
    public void GtChrCnt_TestSingleOccurance()
    {
      Assert.AreEqual(1, SecurityGateway.GetCharacterCount("q", "questioning"), "The character count did not match expectations.");
    } // GtChrCnt_TestSingleOccurance()


    /// <summary>
    /// Tests no occurances.
    /// </summary>
    [TestMethod]
    public void GtChrCnt_TestNoOccurances()
    {
      Assert.AreEqual(0, SecurityGateway.GetCharacterCount("z", "questioning"), "The character count did not match expectations.");
    } // GtChrCnt_TestNoOccurances()


    /// <summary>
    /// Tests multiple occurances.
    /// </summary>
    [TestMethod]
    public void GtChrCnt_TestMultipleOccurances()
    {
      Assert.AreEqual(3, SecurityGateway.GetCharacterCount("io", "questioning"), "The character count did not match expectations.");
    } // GtChrCnt_TestMultipleOccurances()

    #endregion

  } // class CharacterCountTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
