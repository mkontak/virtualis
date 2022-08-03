using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.SqlTypes;
using System.Data.SqlClient;
using System.Collections.Generic;
using System.Data.Common;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.LicensingSubSystem.Tests
{
  /// <summary>
  /// Validates the validate license stored procedure.
  /// </summary>
  [TestClass]
  public class ValidateLicenseTests
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
    /// Tests the validation of a valid license.
    /// </summary>
    [TestMethod]
    public void ValLic_TestValidLicense()
    {
      // Add one license and expire it in the database.
      LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 1, 120);
      Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Available);

      if (licenses.Length > 0)
      {
        LicensingGateway.ValidateLicense(licenses[0].Item1);
      }
      else
      {
        Assert.Fail("No licenses could be retrieved.");
      }
    } // ValLic_TestValidLicense()


    /// <summary>
    /// Tests the validation of an expired license.
    /// </summary>
    [TestMethod]
    public void ValLic_TestExpiredLicense()
    {
      try
      {
        // Add one license and expire it in the database.
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 1, 120);
        Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.Available);

        if (licenses.Length > 0)
        {
          Database database = LicensingGateway.CreateDatabase();
          using (DbConnection conn = database.CreateConnection())
          {
            DbCommand cmd = database.GetSqlStringCommand(string.Format(
              "UPDATE [v3d].[Licenses] SET [v3d].[Licenses].[exp] = \'{0}\' WHERE [v3d].[Licenses].[key] = \'{1}\'",
              DateTimeOffset.Now.Subtract(new TimeSpan(0, 1, 0)).ToString("o"), licenses[0].Item1));

            database.ExecuteNonQuery(cmd);
          }

          LicensingGateway.ValidateLicense(licenses[0].Item1, true, true);
          Assert.Fail("The database did not emmitt an error as expected when trying to validate an expired license.");
        }
        else
        {
          Assert.Fail("The database did not return the expected number of licenses.");
        }
      }
      catch(SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(900993, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ValLic_TestValidateLicense()


    /// <summary>
    /// Tests the validation of an in use license.
    /// </summary>
    [TestMethod]  
    public void ValLic_TestInuseLicense()
    {
      try
      {
        // Add one license and expire it in the database.
        LicensingGateway.AddLicenses("12345", LicensingGateway.SYSTEM_ID, DateTimeOffset.Now, "kchalupa", 1, 120);
        LicensingGateway.AcquireLicense(LicensingGateway.SYSTEM_ID);

        Tuple<string, LicensingGateway.LicenseStates>[] licenses = LicensingGateway.GetLicenses(LicensingGateway.LicenseStates.InUse);

        if (licenses.Length > 0)
        {
          LicensingGateway.ValidateLicense(licenses[0].Item1);
        }
        else
        {
          Assert.Fail("No licenses could be retrieved.");
        }
      }
      catch (SqlException e)
      {
        int errorCode = Convert.ToInt32(e.Message.Substring(0, e.Message.IndexOf(":")));
        Assert.AreEqual(901002, errorCode, "The expected errorcode returned didn't match expectations.");
      }
    } // ValLic_TestInuseLicense()

    #endregion

  } // class ValidateLicenseTests
} // namespace Viatronix.UnitTests.LicensingSubSystem.Tests
