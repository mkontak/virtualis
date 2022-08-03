using System;
using System.Data;
using System.Data.Sql;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Data.Common;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Performs testing of the AddLicenses stored procedure.
  /// </summary>
  [TestClass]
  public sealed class AddLicensesTests
  {

    #region methods

    /// <summary>
    /// Performs initialization before each test.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();

      LicensingGateway.RegisterLicenseDatabase();
    } // TestInitialize()


    /// <summary>
    /// Cleans up after each test.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      LicensingGateway.ClearRegistration();
      LicensingGateway.ClearLicenses();
    } // TestCleanup()


    /// <summary>
    /// Tests the AddLicenses to add licenses and validate that they were added.
    /// </summary>
    [TestMethod]
    public void AddLic_TestSuccess()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);
      }
      catch (Exception e)
      {
        Assert.Fail(string.Format("An error occurred.  [ERROR=\"{0}\"]", e.Message));
      }
    } // AddLic_TestSuccess()


    /// <summary>
    /// Tests the AddLicenses on a system that is disabled.  
    /// </summary>
    /// <remarks>This test will succeed if error "900980" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestSystemDisabled()
    {
      try
      {
        Database database = LicensingGateway.CreateDatabase();
        using (IDbConnection conn = database.CreateConnection())
        {
          DbCommand cmd = database.GetSqlStringCommand("UPDATE [v3d].[Install] SET date = \'2011-11-21 12:00:15.9630366 -05:00\', sysdate = \'2011-11-21 12:00:15.9630366 -05:00\', expdate = \'2011-11-21 12:00:15.9630366 -05:00\'");
          database.ExecuteNonQuery(cmd);

          LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);
        }

        Assert.Fail("Database did not emmit an error when trying to add licenes on an expired system.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900980, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestSystemDisabled()


    /// <summary>
    /// Tests the AddLicenses on a system where Register was not called.  
    /// </summary>
    /// <remarks>This test will succeed if error "900981" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestNotInstalled()
    {
      try
      {
        LicensingGateway.ClearRegistration();
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 120);

        Assert.Fail("Database did not emmit an error when trying to add licenes without performing system registration.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900981, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestNotInstalled()


    /// <summary>
    /// Tests the AddLicenses on a system where no system id.
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestMissingSystemId()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", null, DateTimeOffset.Now, "kchalupa", 2, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an empty system id.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestEmptySystemId()


    /// <summary>
    /// Tests the AddLicenses on a system where an invalid system id or no system id was supplied.  
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestInvalidSystemId()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", string.Empty, DateTimeOffset.Now, "kchalupa", 2, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an invalid system id parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestInvalidSystemId()


    /// <summary>
    /// Tests the AddLicenses on a system where an invalid system id or no system id was supplied.  
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestBogusSystemId()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", "BOGUS SYSTEM ID", DateTimeOffset.Now, "kchalupa", 2, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an invalid system id parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestBogusSystemId()


    /// <summary>
    /// Tests the AddLicenses on a system where an missing issued by.  
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestMissingIssuedBy()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, null, 2, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an missing issued by parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestMissingIssuedBy()


    /// <summary>
    /// Tests the AddLicenses on a system where an invalid issued by.  
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestInvalidIssuedBy()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, string.Empty, 2, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an invalid issued by parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestMissingIssuedBy()


    /// <summary>
    /// Tests the AddLicenses on a system where an missing issued by.  
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestMissingLicenseCount()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", null, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an missing license count parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestMissingLicenseCount()


    /// <summary>
    /// Tests the AddLicenses on a system where an invalid license count.
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestInvalidLicenseCount()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 0, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an invalid license count parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestInvalidLicenseCount()


    /// <summary>
    /// Tests the AddLicenses on a system where an missing issued date.
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestMissingIssuedDate()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, null, "kchalupa", 2, 120);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an missing issued date parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestMissingIssuedDate()


    /// <summary>
    /// Tests the AddLicenses on a system where an invalid license count.
    /// </summary>
    /// <remarks>This test will succeed if error "900990" is returned.</remarks>
    [TestMethod]
    public void AddLic_TestInvalidTimeToLive()
    {
      try
      {
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 2, 0);
        Assert.Fail("Database did not emmit an error when trying to add licenes with an missing license ttl parameter.");
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900990, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // AddLic_TestInvalidTimeToLive()

    #endregion

  } // class AddLicensesTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
