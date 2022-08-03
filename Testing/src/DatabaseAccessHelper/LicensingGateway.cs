using System;
using System.IO;
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

namespace Viatronix.UnitTests.DatabaseAccessHelper
{
  /// <summary>
  /// Global constants/functions used by this project.
  /// </summary>
  public static class LicensingGateway
  {

    #region enums

    /// <summary>
    /// The license states.
    /// </summary>
    public enum LicenseStates : int
    {
      Any = 0,
      Available = 1,
      InUse = 2,
      Disabled = 4,
      Locked = 8,
      Expired = 16
    } // enum LicenseStates

    #endregion

    #region constants

    /// <summary>
    /// The connection string to the licensing database.
    /// </summary>
    private static readonly string LICENSING_CONNECTION_STRING = "Server=(local)\\V3DENT;Database=Licensing;User Id=sa;Password=VXPUSHPIN89!;";

    /// <summary>
    /// The system id.
    /// </summary>
    public static readonly string SYSTEM_ID = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    static LicensingGateway()
    {
      SYSTEM_ID = LicensingGateway.GetSystemId();
    } // LicensingGateway()

    #endregion

    #region methods

    /// <summary>
    /// Creates a database to point to the licensing database on the local system.
    /// </summary>
    /// <returns>A database to point to the licensing database on the local system</returns>
    public static Database CreateDatabase()
    {
      return new SqlDatabase(LICENSING_CONNECTION_STRING);
    } // CreateDatabase()


    /// <summary>
    /// Clears the registration.
    /// </summary>
    public static void ClearRegistration()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("DELETE FROM [v3d].[Install]");
        database.ExecuteNonQuery(cmd);
      }
    } // ClearRegistration()
    

    /// <summary>
    /// Clears the licenses.
    /// </summary>
    public static void ClearLicenses()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("DELETE FROM [v3d].[Licenses]");
        database.ExecuteNonQuery(cmd);
      }
    } // ClearLicenses()


    /// <summary>
    /// Gets the database date time.
    /// </summary>
    /// <returns>Gets the database date time.</returns>
    public static DateTimeOffset GetDatabaseDateTime()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("SELECT SYSDATETIMEOFFSET()");
        return (DateTimeOffset)database.ExecuteScalar(cmd);
      }
    } // GetDatabaseDateTime()


    /// <summary>
    /// Unregisters the system with the specified system id.
    /// </summary>
    /// <param name="systemId">The system id of the server to unregister.</param>
    public static void UnregisterSystem(string systemId)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_unregisterSystem]");
        database.AddInParameter(cmd, "@SYSID", DbType.AnsiString, systemId);
        database.ExecuteNonQuery(cmd);
      }
    } // UnregisterSystem()


    /// <summary>
    /// Registers the databse.
    /// </summary>
    /// <param name="expirationDate">The expiration date.</param>
    public static int RegisterLicenseDatabase(DateTimeOffset? expirationDate = null)
    {
      if (expirationDate == null)
      {
        expirationDate = DateTimeOffset.Now.AddDays(120);
      }
      
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_register]");

        database.AddInParameter(cmd, "@ENABLE", DbType.Int16, 1);
        database.AddInParameter(cmd, "@EXPDATE", DbType.DateTimeOffset, expirationDate);
        database.AddParameter(cmd, "@RESULT", DbType.Int32, ParameterDirection.ReturnValue, string.Empty, DataRowVersion.Current, 0);

        database.ExecuteNonQuery(cmd);
        return Convert.ToInt32(database.GetParameterValue(cmd, "@RESULT").ToString());
      }
    } // RegisterLicenseDatabase( expirationDate )


    /// <summary>
    /// Registers the databse.
    /// </summary>
    public static int AddLicenses(string transactionId, string systemId, DateTimeOffset? dateIssued, string issuedBy, int? licenseCount, int? ttl)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_addLicenses]");

        string date = DateTimeOffset.Now.AddDays(30).ToString("o");

        database.AddInParameter(cmd, "@TRANSACTION_ID", DbType.AnsiString, transactionId);
        database.AddInParameter(cmd, "@SYSTEM_ID", DbType.AnsiString, systemId);
        database.AddInParameter(cmd, "@DATE_ISSUED", DbType.DateTimeOffset, dateIssued);
        database.AddInParameter(cmd, "@ISSUED_BY", DbType.AnsiString, issuedBy);
        database.AddInParameter(cmd, "@LICENSE_COUNT", DbType.Int32, licenseCount);
        database.AddInParameter(cmd, "@TTL", DbType.Int32, ttl);
        database.AddParameter(cmd, "@RESULT", DbType.Int32, ParameterDirection.ReturnValue, string.Empty, DataRowVersion.Current, 0);

        database.ExecuteNonQuery(cmd);
        return Convert.ToInt32(database.GetParameterValue(cmd, "@RESULT").ToString());
      }
    } // AddLicenses( transactionId, systemId, dateIssued, issuedBy, licenseCount, ttl )


    /// <summary>
    /// Acquires a license from the database.
    /// </summary>
    /// <param name="systemId">The processor system id making the request.</param>
    /// <returns>Xml containing the license.</returns>
    public static string AcquireLicense(string systemId)
    {
      Database database = CreateDatabase();
      using(DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_acquireLicense]");

        database.AddInParameter(cmd, "@SYSID", DbType.AnsiString, systemId);
        database.AddOutParameter(cmd, "@LICENSE", DbType.AnsiString, 256);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@LICENSE").ToString();
      }
    } // AcquireLicense( systemId )


    /// <summary>
    /// Releases the provided license.
    /// </summary>
    /// <param name="licenseKey">The license to release.</param>
    public static void ReleaseLicense(string licenseKey)
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_releaseLicense]");

        databse.AddInParameter(cmd, "@KEY", DbType.AnsiString, licenseKey);

        databse.ExecuteNonQuery(cmd);
      }
    } // ReleaseLicense( licenseKey )


    /// <summary>
    /// Checks to see if the system has expired.
    /// </summary>
    /// <returns>True if the system is not expired, false otherwise.</returns>
    public static bool CheckExpirationDate()
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_checkExpirationDate]");
        databse.AddParameter(cmd, "@RESULT", DbType.Int32, ParameterDirection.ReturnValue, string.Empty, DataRowVersion.Current, 0);
        databse.ExecuteNonQuery(cmd);

        return Convert.ToInt32(databse.GetParameterValue(cmd, "@RESULT").ToString()) == 1 ? true : false;
      }
    } // CheckExpirationDate()


    /// <summary>
    /// Checks to see if the system date is valid against the database.
    /// </summary>
    /// <returns>True if the system is date is valid, false otherwise.</returns>
    public static bool CheckSystemDate()
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_checkSystemDate]");
        databse.AddParameter(cmd, "@RESULT", DbType.Int32, ParameterDirection.ReturnValue, string.Empty, DataRowVersion.Current, 0);
        databse.ExecuteNonQuery(cmd);

        int result = Convert.ToInt32(databse.GetParameterValue(cmd, "@RESULT").ToString());
        return result == 1 ? true : false;
      }
    } // CheckSystemDate()


    /// <summary>
    /// Checks to see if the stored system id is valid against the database.
    /// </summary>
    /// <returns>True if the stored system id is valid, false otherwise.</returns>
    public static bool CheckSystemId()
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_checkSystemId]");
        databse.AddParameter(cmd, "@RESULT", DbType.Int32, ParameterDirection.ReturnValue, string.Empty, DataRowVersion.Current, 0);
        databse.ExecuteNonQuery(cmd);

        int result = Convert.ToInt32(databse.GetParameterValue(cmd, "@RESULT").ToString());
        return result == 1 ? true : false;
      }
    } // CheckSystemId()


    /// <summary>
    /// Checks to see if the stored system is valid.
    /// </summary>
    /// <returns>True if the stored system is valid, false otherwise.</returns>
    public static bool CheckSystem()
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_checkSystem]");
        databse.AddInParameter(cmd, "@CHECK_ID", DbType.Boolean, true);
        databse.AddParameter(cmd, "@RESULT", DbType.Int32, ParameterDirection.ReturnValue, string.Empty, DataRowVersion.Current, 0);
        databse.ExecuteNonQuery(cmd);

        int result = Convert.ToInt32(databse.GetParameterValue(cmd, "@RESULT").ToString());
        return result == 1 ? true : false;
      }
    } // CheckSystem()


    /// <summary>
    /// Disables the specified license.
    /// </summary>
    /// <param name="licenseKey">The license to disable.</param>
    public static void DisableLicense(string licenseKey)
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_disableLicense]");
        databse.AddInParameter(cmd, "@KEY", DbType.AnsiString, licenseKey);
        databse.ExecuteNonQuery(cmd);
      }
    } // DisableLicense( licenseKey )


    /// <summary>
    /// Enables the license specified.
    /// </summary>
    /// <param name="licenseKey">The license to enable.</param>
    public static void EnableLicense(string licenseKey)
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_enableLicense]");
        databse.AddInParameter(cmd, "@KEY", DbType.AnsiString, licenseKey);
        databse.ExecuteNonQuery(cmd);
      }
    } // EnableLicense( licenseKey )


    /// <summary>
    /// Retrives the licenses with the specified state.
    /// </summary>
    /// <param name="state">The desired license state.</param>
    /// <returns>All available licenses in the database.</returns>
    public static Tuple<string, LicenseStates>[] GetLicenses(LicenseStates state = LicenseStates.Any)
    {
      List<Tuple<string, LicenseStates>> licenses = new List<Tuple<string, LicenseStates>>();

      Database database = LicensingGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[Licenses]");
        DataSet ds = database.ExecuteDataSet(cmd);

        if (ds.Tables.Count > 0)
        {
          foreach (DataRow row in ds.Tables[0].Rows)
          {
            // Only include if the license is available.
            int licenseState = Convert.ToInt32(row["state"].ToString());
            if (state == LicenseStates.Any || licenseState == (int)state)
            {
              licenses.Add(new Tuple<string, LicenseStates>(row["key"].ToString(), (LicenseStates)licenseState));
            }
          }
        }
      }

      return licenses.ToArray();
    } // GetLicenses( state )


    /// <summary>
    /// Gets the state for the license key specified.
    /// </summary>
    /// <param name="key">The license to check.</param>
    /// <returns>The state for the specified license, or null if the license key doesn't exist.</returns>
    public static LicenseStates? GetLicenseState(string key)
    {
      Tuple<string, LicenseStates>[] licenses = GetLicenses();

      foreach (Tuple<string, LicenseStates> license in licenses)
      {
        if (license.Item1 == key)
        {
          return license.Item2;
        }
      }

      return null;
    } // GetLicenseState( key )


    /// <summary>
    /// Locks a license.
    /// </summary>
    /// <param name="licenseKey">The license to lock.</param>
    public static void LockLicense(string licenseKey)
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_lockLicense]");
        databse.AddInParameter(cmd, "@KEY", DbType.AnsiString, licenseKey);
        databse.ExecuteNonQuery(cmd);
      }
    } // LockLicense( licenseKey )


    /// <summary>
    /// Unlocks a license.
    /// </summary>
    /// <param name="licenseKey">The license to unlock.</param>
    public static void UnlockLicense(string licenseKey)
    {
      Database databse = CreateDatabase();
      using (DbConnection conn = databse.CreateConnection())
      {
        DbCommand cmd = databse.GetStoredProcCommand("[v3d].[sp_unlockLicense]");
        databse.AddInParameter(cmd, "@KEY", DbType.AnsiString, licenseKey);
        databse.ExecuteNonQuery(cmd);
      }
    } // UnlockLicense( licenseKey )


    /// <summary>
    /// Gets the system id from the hardware.
    /// </summary>
    /// <returns>The system id.</returns>
    public static string GetSystemId()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getSystemId]");
        database.AddOutParameter(cmd, "@systemid", DbType.AnsiString, 4000);
        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@systemid").ToString();
      }
    } // GetSystemId()


    /// <summary>
    /// Gets the licenses requested by the parameters.
    /// </summary>
    /// <param name="query">Represents an xml formatted query.</param>
    /// <returns>Information about the licenses.</returns>
    public static string GetLicense(string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getLicense]");

        database.AddInParameter(cmd, "@QUERY", DbType.AnsiString, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetLicense( key, query )


    /// <summary>
    /// Registers a system with the licensing database.
    /// </summary>
    /// <param name="systemid">The system to register.</param>
    public static string RegisterSystem(string systemid, string name, string ipAddress, DateTimeOffset installDate)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_registerSystem]");

        database.AddInParameter(cmd, "@SYSID", DbType.AnsiString, systemid);
        database.AddInParameter(cmd, "@NAME", DbType.AnsiString, name);
        database.AddInParameter(cmd, "@IP", DbType.AnsiString, ipAddress);
        database.AddInParameter(cmd, "@INS", DbType.DateTimeOffset, installDate);
        database.AddOutParameter(cmd, "@LS", DbType.AnsiString, 4000);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@LS").ToString();
      }
    } // RegisterSystem( systemid, name, ipAddress, offset )


    /// <summary>
    /// Updates expired licenses.
    /// </summary>
    public static void UpdateExpiredLicenses()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_updateExpiredLicenses]");
        database.ExecuteNonQuery(cmd);
      }
    } // UpdateExpiredLicenses()


    /// <summary>
    /// Updates a role.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="name">The name of the role.</param>
    /// <param name="desc">The description of the role.</param>
    /// <param name="prms">The permission mask.</param>
    public static void UpdateRole(Guid contextId, string name, string desc, int prms)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_updateRole]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@RECORD", DbType.AnsiString, string.Format(
          "<role name=\"{0}\" desc=\"{1}\" prms=\"{2}\"", name, desc, prms));

        database.ExecuteNonQuery(cmd);
      }
    } // UpdateRole( contextId, name, desc, prms )


    /// <summary>
    /// Validates the license.
    /// </summary>
    /// <param name="key">The license key to validate.</param>
    /// <param name="checkExpired">Should check the expired state.</param>
    /// <param name="checkInUse">Should check if it is in use.</param>
    public static void ValidateLicense(string key, bool checkExpired = true, bool checkInUse = true)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_validateLicense]");

        database.AddInParameter(cmd, "@KEY", DbType.AnsiString, key);
        database.AddInParameter(cmd, "@CHECK_EXPIRED", DbType.Boolean, checkExpired);
        database.AddInParameter(cmd, "@CHECK_INUSE", DbType.Boolean, checkInUse);

        database.ExecuteNonQuery(cmd);
      }
    } // ValidateLicense( key, checkExpired, checkInUse )


    /// <summary>
    /// Validates that the licenses keys in the licensing table.
    /// </summary>
    public static void ValidateLicenseKeys()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_validateLicenseKeys]");
        database.ExecuteNonQuery(cmd);
      }
    } // ValidateLicenseKeys()

    #endregion

  } // class LicensingGateway
} // namespace Viatronix.UnitTests.DatabaseAccessHelper
