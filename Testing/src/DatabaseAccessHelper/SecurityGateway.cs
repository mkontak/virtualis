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

namespace Viatronix.UnitTests.DatabaseAccessHelper
{
  /// <summary>
  /// A gateway to support the unit tests.
  /// </summary>
  public static class SecurityGateway
  {

    #region enums

    /// <summary>
    /// The list of available user states.
    /// </summary>
    public enum UserStates : int
    {
      Available = 0,
      Locked = 1,
      Expired = 4,
      Disabled = 8,
      PasswordChange = 256
    } // UserStates


    /// <summary>
    /// The list of rights.
    /// </summary>
    public enum Rights
    {
      Any,
      Add,
      Delete,
      Modify,
      View,
      Security,
      Storage,
      Processing,
      Auditing,
      Finding,
      Reporting,
      Transferring,
      Administration,
      Service,
      Launching,

      // Combined rights.
      UserManagement,
      StorageManagement,
      ProcessingManagement,
      AuditingManagement,
      ViewReport,
      ReportingManagement,
      FindingManagement
    } // enum Rights


    /// <summary>
    /// The authentication modes.
    /// </summary>
    public enum AuthenticationModes
    {
      Viatronix,
      Windows
    } // enum AuthenticationModes

    #endregion

    #region exceptions

    /// <summary>
    /// An exception to denote that cleanup wasn't done or that the database wasn't left in a way we expect.
    /// </summary>
    public class DatabaseCleanupValidationException : Exception
    {

      #region construction

      /// <summary>
      /// Constructor.
      /// </summary>
      public DatabaseCleanupValidationException() : base()
      {
      } // DatabaseCleanupValidationException()


      /// <summary>
      /// Constructor.
      /// </summary>
      /// <param name="message">The exception message.</param>
      public DatabaseCleanupValidationException(string message) : base(message)
      {
      } // DatabaseCleanupValidationException( message )



      /// <summary>
      /// Constructor.
      /// </summary>
      /// <param name="message">The exception message.</param>
      /// <param name="inner">The inner exception.</param>
      public DatabaseCleanupValidationException(string message, Exception inner) : base(message, inner)
      {
      } // DatabaseCleanupValidationException( message, inner )

      #endregion

    } // class DatabaseCleanupValidationException

    #endregion

    #region constants

    /// <summary>
    /// The connection string to the security database.
    /// </summary>
    private static string SECURITY_CONNECTION_STRING = "Server=(local)\\V3DENT;Database=Security;User Id=sa;Password=VXPUSHPIN89!;";

    /// <summary>
    /// The system id.
    /// </summary>
    public static readonly string SYSTEM_ID = "251A316F04FFDC3A41139E4425386545";

    /// <summary>
    /// The service username.
    /// </summary>
    public static readonly string SRVCUSERNAME = "vxservice";

    /// <summary>
    /// The service password.
    /// </summary>
    public static readonly string SRVCPASSWORD = "stonybrook89!";

    #endregion

    #region methods
        
    /// <summary>
    /// Acquires a context.
    /// </summary>
    /// <param name="login">The login to authenticate with.</param>
    /// <param name="password">The password to authenticate with.</param>
    /// <param name="newPassword">The new password if a password change is required.  Default value will generate an error.</param>
    /// <param name="ttl">The time to live.</param>
    /// <param name="context">The existing context to bump up the ref count.</param>
    /// <returns>The context id to prove authentication has occurred.</returns>
    public static Guid AcquireContext(string login, string password, string newPassword = null, int ttl = 30, Guid? context = null)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_acquireContext]");

        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@PASSWORD", DbType.AnsiString, Encrypt(password));
        database.AddInParameter(cmd, "@NEW_PASSWORD", DbType.AnsiString, newPassword);
        database.AddInParameter(cmd, "@TTL", DbType.Int32, ttl);
        
        if(context != null && context != Guid.Empty)
        {
          database.AddParameter(cmd, "@CONTEXT_ID", DbType.AnsiString, ParameterDirection.InputOutput, string.Empty, DataRowVersion.Default, context.ToString());
        }
        else
        {
          database.AddOutParameter(cmd, "@CONTEXT_ID", DbType.AnsiString, 256);
        }
        
        database.ExecuteNonQuery(cmd);

        return Guid.Parse(database.GetParameterValue(cmd, "@CONTEXT_ID").ToString());
      }
    } // AcquireContext( login, password, newPassword, ttl )


    /// <summary>
    /// Attempts to authenticate...?
    /// </summary>
    /// <param name="contextId">The context id to authenticate against.</param>
    /// <param name="username">The username to authenticate against.</param>
    /// <param name="password">The password to authenticate against.</param>
    public static void Authenticate(Guid contextId, string username, string password)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_authenticate]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, username);
        database.AddInParameter(cmd, "@PASSWORD", DbType.AnsiString, Encrypt(password));

        database.ExecuteNonQuery(cmd);
      }
    } // Authenticate( contextId, username, password )


    /// <summary>
    /// Attempts to authenticate the user.
    /// </summary>
    /// <param name="login">The username.</param>
    /// <param name="password">The password.</param>
    public static void AuthenticateUser(string login, string password)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_authenticateUser]");

        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@PASSWORD", DbType.AnsiString, password);

        database.ExecuteNonQuery(cmd);
      }
    } // AuthenticateUser( login, password )


    /// <summary>
    /// Changes the user's password.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="login">The username.</param>
    /// <param name="oldPassword">The old password.</param>
    /// <param name="newPassword">The new password.</param>
    public static void ChangePassword(Guid? contextId, string login, string oldPassword, string newPassword)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_changePassword]");

        if(contextId != null && contextId != Guid.Empty)
        {
          database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        }

        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@OLD_PASSWORD", DbType.AnsiString, oldPassword);
        database.AddInParameter(cmd, "@NEW_PASSWORD", DbType.AnsiString, newPassword);

        database.ExecuteNonQuery(cmd);
      }
    } // ChangePassword( contextId, login, oldPassword, newPassword )


    /// <summary>
    /// Cleans up expired contexts.
    /// </summary>
    public static void CleanupExpiredContexts()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_cleanupExpiredContexts]");
        database.ExecuteNonQuery(cmd);
      }
    } // CleanupExpiredContexts()


    /// <summary>
    /// Creates the database ready for use.
    /// </summary>
    /// <returns>A database object representing the security db.</returns>
    public static Database CreateDatabase()
    {
      return new SqlDatabase(SECURITY_CONNECTION_STRING);
    } // CreateDatabase()


    /// <summary>
    /// Decrypts the provided value.
    /// </summary>
    /// <param name="value">The value to encrypt.</param>
    /// <returns>The decrypted value.</returns>
    public static string Decrypt(string value)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_decrypt]");

        database.AddInParameter(cmd, "@value", DbType.AnsiString, value);
        database.AddOutParameter(cmd, "@decryptedValue", DbType.AnsiString, 8192);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@decryptedValue").ToString();
      }
    } // Decrypt( value )


    /// <summary>
    /// Decrypts the provided value.
    /// </summary>
    /// <param name="value">The value to encrypt.</param>
    /// <returns>The decrypted value.</returns>
    public static string DecryptPassword(string value)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_decryptPassword]");

        database.AddInParameter(cmd, "@ENCRYPTED", DbType.AnsiString, value);
        database.AddOutParameter(cmd, "@DECRYPTED", DbType.AnsiString, 8192);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@DECRYPTED").ToString();
      }
    } // DecryptPassword( value )


    /// <summary>
    /// Deletes the specified role.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="roleName">The name of the role to delete.</param>
    public static void DeleteRole(Guid contextId, string roleName)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_deleteRole]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@NAME", DbType.AnsiString, roleName);

        database.ExecuteNonQuery(cmd);
      }
    } // DeleteRole( roleName )


    /// <summary>
    /// Deletes the provided user.
    /// </summary>
    /// <param name="contextId">The authentication token.</param>
    /// <param name="login">The user to remove.</param>
    public static void DeleteUser(Guid contextId, string login, bool forceReleaseContext = false)
    {
      Database database = SecurityGateway.CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_deleteUser]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@RELEASE_CONTEXT", DbType.Int32, forceReleaseContext ? 1 : 0);

        database.ExecuteNonQuery(cmd);
      }
    } // DeleteUser( contextId, login )


    /// <summary>
    /// Deletes the user profile.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="login">The user, whose profile we will delete.</param>
    /// <param name="profileName">The prpfile name.</param>
    /// <param name="record">The xml containing the login and profile name.</param>
    public static void DeleteUserProfile(Guid contextId, string login, string profileName, string record = null)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_deleteUserProfile]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@NAME", DbType.AnsiString, profileName);

        if (record == null)
        {
          database.AddInParameter(cmd, "@RECORD", DbType.AnsiString,
            string.Format("<userprofile login=\"{0}\" profileName=\"{1}\" />", login, profileName));
        }
        else
        {
          database.AddInParameter(cmd, "@RECORD", DbType.AnsiString, record);
        }

        database.ExecuteNonQuery(cmd);
      }
    } // DeleteUserProfile( contextId, login, profileName, record )


    /// <summary>
    /// Encrypts the provided value.
    /// </summary>
    /// <param name="value">The value to encrypt.</param>
    /// <returns>The encrypted value.</returns>
    public static string Encrypt(string value)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_encrypt]");

        database.AddInParameter(cmd, "@value", DbType.AnsiString, value);
        database.AddOutParameter(cmd, "@encryptedValue", DbType.AnsiString, 8192);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@encryptedValue").ToString();
      }
    } // Encrypt( value )


    /// <summary>
    /// Forces a user to change his password.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="login">The login of the user we are forcing to change his password.</param>
    public static void ForcePasswordChange(Guid contextId, string login)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_forcePasswordChange]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);

        database.ExecuteNonQuery(cmd);
      }
    } // ForcePasswordChange()


    /// <summary>
    /// Gets the authentication type.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="query">What to look for.</param>
    /// <returns>The result of the query.</returns>
    public static string GetAuthenticationType(Guid contextId, string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getAuthenticationType]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@QUERY", DbType.Xml, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetAuthenticationType( contextId, query )


    /// <summary>
    /// Gets a list of contexts conforming to either the login or the query xml.
    /// </summary>
    /// <param name="contextId">The context id to authenticate against.</param>
    /// <param name="query">The query xml.</param>
    /// <returns>Any contexts that match the provided query xor the login.</returns>
    public static string GetContext(Guid contextId, string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getContext]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@QUERY", DbType.AnsiString, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetContext( contextId, query, login )


    /// <summary>
    /// Gets the number of occurances that the characters in needle (each character), occurs in the haystack string.
    /// </summary>
    /// <param name="needle">What to look for.</param>
    /// <param name="haystack">Where to look.</param>
    /// <returns>How many times needle was found in the haystack.</returns>
    public static int GetCharacterCount(string needle, string haystack)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_characterCount]");

        database.AddInParameter(cmd, "@CHARACTERS", DbType.AnsiString, needle);
        database.AddInParameter(cmd, "@STRING", DbType.AnsiString, haystack);
        database.AddParameter(cmd, "@COUNT", DbType.Int32, ParameterDirection.Output, string.Empty, DataRowVersion.Current, -1);

        database.ExecuteNonQuery(cmd);

        return Convert.ToInt32(database.GetParameterValue(cmd, "@COUNT").ToString());
      }
    } // GetCharacterCount( needle, haystack )


    /// <summary>
    /// Gets the passwords for the supplied user(s).
    /// </summary>
    /// <param name="logins">The users to retrieve passwords.</param>
    /// <returns>The passwords for the provided login(s).</returns>
    public static string GetPasswords(string logins = null)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getPasswords]");

        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, logins);
        database.AddOutParameter(cmd, "@PASSWORDS", DbType.Xml, 8192);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@PASSWORDS").ToString();
      }
    } // GetPasswords( logins )


    /// <summary>
    /// Gets the number of password templates.
    /// </summary>
    /// <returns>The number of password templates.</returns>
    public static int GetPasswordCount()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand("SELECT COUNT(*) FROM [v3d].[PasswordTemplates]");
        return Convert.ToInt32(database.ExecuteScalar(cmd).ToString());
      }
    } // GetPasswordCount()


    /// <summary>
    /// Gets the rights specified in the query.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="query">The query.</param>
    public static string GetRight(Guid contextId, string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getRight]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@QUERY", DbType.Xml, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetRight( contextId, query, rightName )


    /// <summary>
    /// Gets the roles associated with the query.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="query">The query.</param>
    /// <returns>The roles that match the query.</returns>
    public static string GetRole(Guid contextId, string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getRole]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@QUERY", DbType.Xml, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetRole( contextId, query )


    /// <summary>
    /// Gets the users associated with the query.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="query">The query.</param>
    /// <returns>The users that match the query.</returns>
    public static string GetUser(Guid contextId, string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getUser]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@QUERY", DbType.Xml, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetUser( contextId, query )


    /// <summary>
    /// Gets the user profiles associated with the query.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="query">The query.</param>
    /// <returns>The user profiles that match the query.</returns>
    public static string GetUserProfile(Guid contextId, string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getUserProfile]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@QUERY", DbType.Xml, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetUserProfile( contextId, query )


    /// <summary>
    /// Gets the licenses associated with the query.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="query">The query.</param>
    /// <returns>The licenses that match the query.</returns>
    public static string GetLicense(Guid contextId, string query)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_getLicense]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@QUERY", DbType.Xml, query);
        database.AddParameter(cmd, "@RESULTS", DbType.Xml, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@RESULTS").ToString();
      }
    } // GetLicense( contextId, query )


    /// <summary>
    /// Insert a new password into the password table.
    /// </summary>
    /// <param name="login">The username.</param>
    /// <param name="password">The new password to set.</param>
    /// <param name="ignoreFrequency">Should the user be able to bypass the frequency check?</param>
    /// <param name="validateOnly">Validation phase only.</param>
    public static void InsertPassword(string login, string password, bool ignoreFrequency = false, bool validateOnly = false)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_insertPassword]");

        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@PASSWORD", DbType.AnsiString, password);
        database.AddInParameter(cmd, "@IGNORE_FREQUENCY", DbType.Int32, ignoreFrequency ? 1 : 0);
        database.AddInParameter(cmd, "@VALIDATE_ONLY", DbType.Int32, validateOnly ? 1 : 0);

        database.ExecuteNonQuery(cmd);
      }
    } // InsertPassword( login, password, ignoreFrequency, validateOnly )


    /// <summary>
    /// Inserts the role.
    /// </summary>
    /// <param name="contextId">The context id to autorize the add.</param>
    /// <param name="name">The name of the role.</param>
    /// <param name="description">The description of the role.</param>
    /// <param name="prmsMask">The mask of permissions.</param>
    public static void InsertRole(Guid contextId, string name, string description, int prmsMask)
    {
      string role = string.Format("<role name=\"{0}\" desc=\"{1}\" prms=\"{2}\" />", name, description, prmsMask);

      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_insertRole]");

        database.AddInParameter(cmd, "@ID", DbType.Guid, contextId);
        database.AddInParameter(cmd, "@RECORD", DbType.Xml, role);

        database.ExecuteNonQuery(cmd);
      }
    } // InsertRole( contextId, name, description, prmsMask )


    /// <summary>
    /// Inserts a user to the database.
    /// </summary>
    /// <param name="contextId">The context id to access the database's functions.</param>
    /// <param name="login">The database login.</param>
    /// <param name="password">The user's password.</param>
    /// <param name="name">The user's name.</param>
    /// <param name="state">The user state.</param>
    /// <param name="expDate">The user's expiration date.</param>
    /// <param name="role">The user's role.</param>
    /// <param name="passwordTemplate">The password template.</param>
    public static void InsertUser(Guid contextId, string login, string name, string password, UserStates state, DateTimeOffset expDate, string role = "Physician", string passwordTemplate = null)
    {
      string userXml = string.Format("<user login=\"{0}\" name=\"{1}\" state=\"{2}\" exp=\"{3}\" role=\"{4}\" {5} {6} />",
        login, name, (int)state, expDate.ToString("o"), role, string.IsNullOrEmpty(password) ? string.Empty : string.Format("pwd=\"{0}\"", 
        Encrypt(password)), !string.IsNullOrEmpty(passwordTemplate) ? string.Format("ptmp=\"{0}\"", passwordTemplate) : string.Empty);

      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_insertUser]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@RECORD", DbType.AnsiString, userXml);

        database.ExecuteNonQuery(cmd);
      }
    } // InsertUser( contextId, login, name, password, state, expDate, role, passwordTemplate )


    /// <summary>
    /// Inserts the user profile.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="username">The user this profile applies to.</param>
    /// <param name="profileName">The name of the profile.</param>
    /// <param name="profileXml">The profile xml.</param>
    public static void InsertUserProfile(Guid contextId, string username, string profileName, string profileXml)
    {
      string record = string.Format("<userprofile login=\"{0}\" name=\"{1}\">{2}</userprofile>", username, profileName, profileXml);

      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_insertUserProfile]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@RECORD", DbType.AnsiString, record);

        database.ExecuteNonQuery(cmd);
      }
    } // InsertUserProfile(Guid contextId, string username, string profileName, string profileXml)


    /// <summary>
    /// Tests to see if the user is authorized to perform the action in mask.
    /// </summary>
    /// <param name="contextId">The context id of the user to test for.</param>
    /// <param name="login">The login of the user to test for.</param>
    /// <param name="mask">The permission mask.</param>
    /// <returns>?</returns>
    public static void IsAuthorized(Guid contextId, Rights right, string login = null)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_isAuthorized]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@MASK", DbType.AnsiString, right.ToString());

        if (login != null)
        {
          database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        }
        
        database.ExecuteNonQuery(cmd);
      }
    } // IsAuthorized( contextId, right )


    /// <summary>
    /// Releases the context.
    /// </summary>
    /// <param name="contextId">The context held by the user with provided login.</param>
    /// <param name="login">The logged in user.</param>
    /// <param name="force">True If we should force the context to release, regardless of the reference count, false otheriwse.</param>
    public static void ReleaseContext(Guid contextId, string login, bool force = false)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_releaseContext]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId != Guid.Empty ? contextId.ToString() : null);
        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@FORCE", DbType.AnsiString, force ? 1 : 0);

        database.ExecuteNonQuery(cmd);
      }
    } // ReleaseContext( contextId, login )


    /// <summary>
    /// Renews the provided context.
    /// </summary>
    /// <param name="contextId">The context id.</param>
    /// <param name="ttl">The time to live.</param>
    /// <param name="checkExpiration">Should test if the user or license is expired.</param>
    public static void RenewContext(Guid contextId, int ttl = 30, bool checkExpiration = true)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_renewContext]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@TTL", DbType.Int32, ttl);
        database.AddInParameter(cmd, "@CHECK_EXPIRATION", DbType.AnsiString, checkExpiration);

        database.ExecuteNonQuery(cmd);
      }
    } // RenewContext( contextId, ttl, checkExpiration )


    /// <summary>
    /// Updates the specified role.
    /// </summary>
    /// <param name="contextId">The context id of the authorizing user.</param>
    /// <param name="roleName">The name of the role to update.</param>
    /// <param name="description">The description of the role.</param>
    /// <param name="prms">The new permissions mask.</param>
    public static void UpdateRole(Guid contextId, string roleName, string description, int prms)
    {
      string record = string.Format("<role name=\"{0}\" desc=\"{1}\" prms=\"{2}\" />",
        roleName, description, prms);
      
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_updateRole]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@RECORD", DbType.AnsiString, record);

        database.ExecuteNonQuery(cmd);
      }
    } // UpdateRole( contextId, roleName, description, prms )


    /// <summary>
    /// Updates the user with the parameters supplied.
    /// </summary>
    /// <param name="contextId">The context to authenticate with.</param>
    /// <param name="record">The user record to update the fields with.</param>
    public static void UpdateUser(Guid contextId, string record)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_updateUser]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, contextId.ToString());
        database.AddInParameter(cmd, "@RECORD", DbType.Xml, record);

        database.ExecuteNonQuery(cmd);
      }
    } // UpdateUser( contextId, record )


    /// <summary>
    /// Validates the provided context.
    /// </summary>
    /// <param name="login">The login name.</param>
    /// <param name="context">The context to be validated.</param>
    public static void ValidateContext(string login, Guid context)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_validateContext]");

        database.AddInParameter(cmd, "@ID", DbType.AnsiString, context.ToString());
        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, login);
        database.AddInParameter(cmd, "@CHECK_EXPIRATION", DbType.Int32, 1);

        database.ExecuteNonQuery(cmd);
      }
    } // ValidateContext( login, context )


    /// <summary>
    /// Validates the database.
    /// </summary>
    public static void ValidateDatabase()
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetSqlStringCommand(
          "SELECT * FROM [v3d].[Users] WHERE [login] <> 'administrator' AND [login] <> 'vxservice' AND [login] <> 'auditor'");

        DataSet ds = database.ExecuteDataSet(cmd);

        if (ds.Tables.Count < 1 || ds.Tables[0].Rows.Count > 0)
        {
          throw new DatabaseCleanupValidationException("The users table did not match exceptions.");
        }

        cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[ROLES] WHERE [name] <> 'Administrator' AND " +
          "[name] <> 'Auditor' AND [name] <> 'Op-Admin' AND [name] <> 'Operator' AND [name] <> 'Phy-Admin' AND " +
          "[name] <> 'Physician' AND [name] <> 'Service'");

        ds = database.ExecuteDataSet(cmd);

        if (ds.Tables.Count < 1 || ds.Tables[0].Rows.Count > 0)
        {
          throw new DatabaseCleanupValidationException("The roles table did not match exceptions.");
        }

        cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[Passwords] WHERE [login] <> 'auditor'");

        ds = database.ExecuteDataSet(cmd);

        if (ds.Tables.Count < 1 || ds.Tables[0].Rows.Count > 0)
        {
          throw new DatabaseCleanupValidationException("The passwords table did not match exceptions.");
        }

        cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[Contexts] WHERE [lic] <> ''");

        ds = database.ExecuteDataSet(cmd);

        if (ds.Tables.Count < 1 || ds.Tables[0].Rows.Count > 0)
        {
          throw new DatabaseCleanupValidationException("The contexts table did not match exceptions.");
        }
      }
    } // ValidateDatabase()


    /// <summary>
    /// Verifies the password has less character repititions than the max.
    /// </summary>
    /// <param name="password">The password was validated.</param>
    /// <param name="maxRepititions">The maximum number of repititions.</param>
    public static void ValidateMaximumCharacterRepititons(string password, int maxRepititions)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_validateMaximumRepititionsAllowed]");

        database.AddInParameter(cmd, "@password", DbType.AnsiString, password);
        database.AddInParameter(cmd, "@maximumRepititions", DbType.Int32, maxRepititions);

        database.ExecuteNonQuery(cmd);
      }
    } // ValidateMaximumCharacterRepititons( password, maxRepititions )


    /// <summary>
    /// Validates the password.
    /// </summary>
    /// <param name="username">The username.</param>
    /// <param name="password">The password.</param>
    public static void ValidatePassword(string username, string password)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_validatePassword]");

        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, username);
        database.AddInParameter(cmd, "@PASSWORD", DbType.AnsiString, password);

        database.ExecuteNonQuery(cmd);
      }
    } // ValidatePassword(string username, string password)


    /// <summary>
    /// Validates the user's password against the user's history.
    /// </summary>
    /// <param name="username">The username the password.</param>
    /// <param name="password">The password to validate.</param>
    /// <returns>The encrypted password.</returns>
    public static string ValidateUserPassword(string username, string password)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_validateUserPassword]");

        database.AddInParameter(cmd, "@LOGIN", DbType.AnsiString, username);
        database.AddInParameter(cmd, "@PASSWORD", DbType.AnsiString, password);
        database.AddParameter(cmd, "@ENCRYPTED_PASSWORD", DbType.AnsiString, ParameterDirection.Output, string.Empty, DataRowVersion.Default, string.Empty);

        database.ExecuteNonQuery(cmd);

        return database.GetParameterValue(cmd, "@ENCRYPTED_PASSWORD").ToString();
      }
    } // ValidateUserPassword( username, password )


    /// <summary>
    /// Authenticates the winodws account.
    /// </summary>
    /// <param name="username">The username to authenticate with.</param>
    /// <param name="password">The password to authenticate with.</param>
    /// <param name="domain">The domain to authenticate against.</param>
    public static void WindowsAuthentication(string username, string password, string domain = null)
    {
      Database database = CreateDatabase();
      using (DbConnection conn = database.CreateConnection())
      {
        DbCommand cmd = database.GetStoredProcCommand("[v3d].[sp_windowsAuthentication]");

        database.AddInParameter(cmd, "@login", DbType.AnsiString, username);
        database.AddInParameter(cmd, "@password", DbType.AnsiString, password);

        if (!string.IsNullOrEmpty(domain))
        {
          database.AddInParameter(cmd, "@domain", DbType.AnsiString, domain);
        }
        else
        {
          database.AddInParameter(cmd, "@domain", DbType.AnsiString, string.Empty);
        }

        database.ExecuteNonQuery(cmd);
      }
    } // WindowsAuthentication( username, password, domain )

    #endregion

  } // class SecurityGateway
} // namespace Viatronix.UnitTests.DatabaseAccessHelper
