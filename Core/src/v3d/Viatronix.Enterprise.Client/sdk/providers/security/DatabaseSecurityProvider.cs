using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.Security.Principal;
using System.Runtime.InteropServices;
using Viatronix.Enterprise;
using Viatronix.Enterprise.SDK;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;
using System.Globalization;

namespace Viatronix.Enterprise.SDK.Providers
{
  /// <summary>
  /// Database Provider 
  /// </summary>
  public class DatabaseSecurityProvider : ProviderBase, ISecurityProvider, IRenewalProvider, IDisposable
  {

    #region fields


    /// <summary>
    /// Renewal timer
    /// </summary>
    private RenewalProvider m_renewalProvider = null;

    /// <summary>
    /// user 
    /// </summary>
    private User m_user = null;

    /// <summary>
    /// tracks disposal of object
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// Authentication context
    /// </summary>
    private string m_context = string.Empty;

    #endregion


    #region constructors


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="xml"></param>
    public DatabaseSecurityProvider(string xml)
      : base(xml)
    {



      /// Initilaize the timer
      m_renewalProvider = new RenewalProvider(xml, (ISecurityProvider)this);
      m_renewalProvider.RenewalFailed += new RenewalEventHandler(OnRenewalFailed);


 
 
    }

    #endregion


    #region properties

    #region IRenewalTimer

    /// <summary>
    /// Sets/Gets tghe renewal time
    /// </summary>
    public Int64 RenewalTime
    {
      get { return m_renewalProvider.RenewalTime; }
      set { m_renewalProvider.RenewalTime = value; }
    } // RenewlTime

    /// <summary>
    /// Gets the renewal error message 
    /// </summary>
    public string LastRenewalErrorMessage
    {
      get { return m_renewalProvider.LastRenewalErrorMessage; }
    }

    /// <summary>
    /// Renewal failed
    /// </summary>
    public bool IsRenewalFailed
    {
      get { return m_renewalProvider.IsRenewalFailed; }
    } // RenewalFailed

    /// <summary>
    /// Returns flag indicating if the renewl timer is running or not
    /// </summary>
    public bool IsRenewalTimerRunning
    {
      get { return m_renewalProvider.IsRenewalTimerRunning; }
    }   // IsRenewalTimerRunning

    #endregion

    /// <summary>
    /// Gets the authentication token/context
    /// </summary>
    public string Context
    { get { return m_context; } }

    /// <summary>
    /// Gets the user
    /// </summary>
    public User User
    { get { return m_user; } }

    /// <summary>
    /// Returns the available flag
    /// </summary>
    public override bool IsAvailable
    {  get {  return SecurityGateway.IsAvailable();  }  }


    /// <summary>
    /// Returns true if we are authenticated
    /// </summary>
    public  override bool IsAuthenticated
    {  get { return (! string.IsNullOrEmpty(m_context)); } }

    #endregion


    #region ISecurity Methods


    #region Domain

    /// <summary>
    /// Gets the domain we are on
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public string GetDomain()
    {
      return string.Empty;
      //return string.Format("<domain name=\"{0}\" ver=\"3.0\" />", Viatronix.Enterprise.Providers.SystemResourceProvider.GetDomain());
    } // GetDomain()

    #endregion

    #region Login/Logout methods

    /// <summary>
    /// Log into system
    /// </summary>
    /// <param name="login">User name</param>
    /// <param name="password">User password</param>
    /// <param name="context">COntext</param>
    /// <returns>Context/Authentication token</returns>
    public  User Login(string login, string password, string context)
    {

      // Make sure the provider is not already authenticated with a login
      if (IsAuthenticated && login != m_user.Login)
          throw new UserAlreadyHasContextException(string.Format("This provider is already logged in as {0}", m_user.Login));

      m_context = SecurityGateway.AcquireContext(context, login, Viatronix.Encryption.Crypto.Encrypt(password));



      List<User> users = new List<User>();

      EntityCollection.Load(users, SecurityGateway.Get(m_context, "user", XmlBuilder.Create("user", new Dictionary<string, string>() { { "login", login } })));


      if (users.Count == 0)
        throw new NotFoundException(string.Format("User {0} was not found", login));

      m_user = users[0];

      StartRenewalTimer();

      return m_user;

    } // Login(string user, string password)


    /// <summary>
    /// Log out of system
    /// </summary>
    /// <param name="context">Context/Authentication token</param>
    public  void Logout(string login = "")
    {


      try
      {


        if (string.IsNullOrEmpty(login))
        {
          Logging.Log.Information("Logging out", "DatabaseSecurityProvider", "Logout");

          if (!string.IsNullOrEmpty(m_context))
            SecurityGateway.ReleaseContext(m_context);
        }
        else
        {
          Logging.Log.Information("Logging out " + login, "DatabaseSecurityProvider", "Logout");

          SecurityGateway.ReleaseContext(login);
        }

      }
      catch (Exception e)
      {
        Logging.Log.Error(e.Message, "DatabaseSecurityProvider", "Logout");
        throw e;
      }
      finally
      {
        StopRenewalTimer();


        m_user = null;
        m_context = string.Empty;
      }

    } // Logout()

    /// <summary>
    /// Log out of system
    /// </summary>
    /// <param name="context">Context/Authentication token</param>
    public  void Renew()
    {
      if (!IsAuthenticated)
      {
        Logging.Log.Warning("Invalid operation, attempting to renew when not authorized", "DatabaseProvider", "Renew");
        return;
      }

      SecurityGateway.RenewContext(m_context);
    } // Renew()



    #endregion

    #region User methods

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  User GetUser(string login)
    {

      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");


      // ========================================
      // Make sure the user name is specified
      // =======================================
      if (!string.IsNullOrEmpty(login))
      {

        List<User> users = new List<User>();

        EntityCollection.Load(users, SecurityGateway.Get(m_context, "user", XmlBuilder.Create("user", new Dictionary<string, string>() { { "login", login } })));


        if (users.Count == 0)
          throw new NotFoundException(string.Format("User {0} was not found", login));

        return users[0];


      }
      else
        throw new Exception("No user name (login) specified");

    } // GetUser( XmlBuilder builder)

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  List<User> GetUsers()
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get users operation requires a login");

      List<User> users = new List<User>();

      EntityCollection.Load(users, SecurityGateway.Get(m_context, "user", "<user />"));

      return users;
    } // GetUsers( XmlBuilder builder)

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  List<User> GetUsers(Role role)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get users operation requires a login");

      List<User> users = new List<User>();

      EntityCollection.Load(users, SecurityGateway.Get(m_context, "user", XmlBuilder.Create("user", new Dictionary<string, string>() { { "role", role.Name } })));

      return users;

    } // GetUsers( XmlBuilder builder)


    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  List<User> GetUsers(XmlBuilder builder)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get users operation requires a login");

      List<User> users = new List<User>();

      EntityCollection.Load(users, SecurityGateway.Get(m_context, "user", builder));

      return users;

    } // GetUsers( XmlBuilder builder)


    /// <summary>
    /// Add the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public  void Insert(User user, string password)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The insert user operation requires a login");

      user.Password = password;

      SecurityGateway.Insert(m_context, "user", user);


    } // Insert( User user)

    /// <summary>
    /// Add the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public  void ChangePassword(User user, string password)
    {
      if (!IsAuthenticated)
      {
        Logging.Log.Warning("Must be an logged in to perform this action.", "DatabaseProvider", "ChangePassword");
        return;
      }


      SecurityGateway.Update(m_context, "user", XmlBuilder.Create("user", new Dictionary<string, string>() 
                                                    { { "login", user.Login }, { "pwd", Viatronix.Encryption.Crypto.Encrypt(password) } }));



    } // AddUser( User user)


    /// <summary>
    /// Delete the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public  void Delete(User user)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The delete user operation requires a login");


      SecurityGateway.DeleteUser(m_context, user.Login);
    } // DeleteUser( User user)

    /// <summary>
    /// Update the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public  void Update(User user)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The update user operation requires a login");


      SecurityGateway.Update(m_context, "user", user);



    } // UpdateUser( User user)

    #endregion

    #region Roles methods

    /// <summary>
    /// Gets the roles 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  Role GetRole(string name)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get roles operation requires a login");

      List<Role> roles = GetRoles(XmlBuilder.Create("role"));

      if (roles.Count == 0)
        throw new NotFoundException("Role " + name + " was not found");

      return roles[0];

    } // GetRole( XmlBuilder builder)



    /// <summary>
    /// Gets the roles using the xml query builder
    /// </summary>
    /// <param name="builder">Query</param>
    /// <returns></returns>
    public  List<Role> GetRoles(XmlBuilder builder)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get roles operation requires a login");

      List<Role> roles = new List<Role>();

      EntityCollection.Load(roles,  SecurityGateway.Get(m_context,"role", builder));

      return roles;

    } //GetRoles(XmlBuilder builder)


    /// <summary>
    /// Gets the roles using the xml query builder
    /// </summary>
    /// <param name="builder">Query</param>
    /// <returns></returns>
    public  List<Role> GetRoles()
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get roles operation requires a login");

      List<Role> roles = new List<Role>();

      EntityCollection.Load(roles, SecurityGateway.Get(m_context, "role", XmlBuilder.Create("role")));

      return roles;

    } //GetRoles(XmlBuilder builder)


    #endregion

    #region Authentication Types


    /// <summary>
    /// Gets the roles 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  List<AuthenticationType> GetAuthenticationTypes(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get authentication types operation requires a login");

      List<AuthenticationType> types = new List<AuthenticationType>();

      EntityCollection.Load(types, SecurityGateway.Get(m_context, "authenticationtype", XmlBuilder.Create("authenticationtype")));

      return types;
    } // GetAuthenticationType( )


    #endregion

    #region Rights methods

    /// <summary>
    /// Gets the rights 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  List<Right> GetRights()
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get rights operation requires a login");

      List<Right> rights = new List<Right>();

      EntityCollection.Load(rights, SecurityGateway.Get(m_context, "right", XmlBuilder.Create("right")));

      return rights;
    } // GetRight( XmlBuilder builder)



    #endregion



    #region UserProfiles methods

    /// <summary>
    /// Insert the user config
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user"></param>
    public void Insert(UserProfile profile)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      SecurityGateway.Insert(m_context, "userprofile", XmlBuilder.Create(profile));
    }

    /// <summary>
    /// Sets the lock on the study
    /// </summary>
    /// <param name="context"></param>
    /// <param name="study"></param>
    public void Delete(UserProfile profile)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      SecurityGateway.Delete(m_context, "userprofile", XmlBuilder.Create(profile));
    }


    /// <summary>
    /// Get the lock for the series specified
    /// </summary>
    /// <param name="context"></param>
    /// <param name="series"></param>
    /// <returns></returns>
    public Dictionary<string, UserProfile> GetUserProfiles(User user)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      List<UserProfile> profiles = new List<UserProfile>();


      string response = SecurityGateway.Get(m_context, "userprofile", XmlBuilder.Create("userprofile", new Dictionary<string, string>() { { "login", user.Login } }));

      EntityCollection.Load(profiles, response);


      user.Profiles.Clear();

      foreach (UserProfile profile in profiles)
      {
        user.Profiles.Add(profile.Name, profile);
      }


      return user.Profiles;


    }

    /// <summary>
    /// Get the lock for the series specified
    /// </summary>
    /// <param name="context"></param>
    /// <param name="series"></param>
    /// <returns></returns>
    public UserProfile GetUserProfile(string login, string name)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      List<UserProfile> profiles = new List<UserProfile>();


      string response = SecurityGateway.Get(m_context, "userprofile", XmlBuilder.Create("userprofile", new Dictionary<string, string>() { { "login", login }, { "name", name } }));

      EntityCollection.Load(profiles, response);

      if (profiles.Count == 0)
        throw new NotFoundException("Users profile " + name + " was not found");


      return profiles[0];
    }



    /// <summary>
    /// Sets the lock on the study
    /// </summary>
    /// <param name="context"></param>
    /// <param name="study"></param>
    public void Update(UserProfile profile)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      SecurityGateway.Update(m_context, "userprofile", XmlBuilder.Create(profile));
    }


    #endregion


    #region Settings


    /// <summary>
    /// Gets the security settings
    /// </summary>
    /// <returns></returns>
    public SystemSetting GetSetting(string name)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      List<SystemSetting> settings = GetSettings(XmlBuilder.Create("systemsetting", new Dictionary<string, string>() { { "subsystem", "Security" }, { "name", name } }));

      if (settings.Count == 0)
        throw new NotFoundException("Setting " + name + " was not found");

      return settings[0];

    } // GetSetting(string name)

    /// <summary>
    /// Gets the security settings
    /// </summary>
    /// <returns></returns>
    public List<SystemSetting> GetSettings(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      List<SystemSetting> settings = new List<SystemSetting>();

      string response = SystemGateway.Get(m_context, "systemsetting", (query == null ? XmlBuilder.Create("systemsetting") : query));

      EntityCollection.Load(settings, response);

      return settings;
    } // GetSettings()




    #endregion

    #region PasswordHints 

    /// <summary>
    /// Gets the password hints for the default password template
    /// </summary>
    /// <returns></returns>
    public List<PasswordHint> GetPasswordHints()
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("The get password hints operation requires a login");

      List<PasswordHint> hints = new List<PasswordHint>();

      EntityCollection.Load(hints, SecurityGateway.Get(m_context, "passwordhint", XmlBuilder.Create("passwordhint")));

      return hints;

    }

    #endregion


    #endregion


    #region IDisposable Members


    /// <summary>
    /// Disposes of object
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);

    } // Dispose()

    /// <summary>
    /// Disposes the objects resources
    /// </summary>
    /// <param name="disposing">true if managed object should be cleaned up; otherwise only cleanup unmanaged</param>
    private void Dispose(bool disposing)
    {
      if (!m_disposed)
      {
        if (disposing)
        {
          if (m_user != null)
            Logout();

        }

        m_disposed = true;
      }
    } // Dispose( disposing )

    #endregion

    #region IRenewalTimer

    /// <summary>
    /// Starts the renewal timer, using the specified time, if it is not already running and return true if started,
    /// false if not started.  
    /// </summary>
    /// <param name="renewlTime">New renewal time</param>
    /// <returns></returns>
    public bool StartRenewalTimer(Int64 renewlTime)
    {
      return m_renewalProvider.StartRenewalTimer(renewlTime);
    }

    /// <summary>
    /// Starts the renewal timer, using the renewal time already set in the object, if it is not already running and return true if started,
    /// false if not started.  
    /// </summary>
    /// <returns>True is started, flase if not</returns>
    public bool StartRenewalTimer()
    {
      return m_renewalProvider.StartRenewalTimer();
    } // StartRenewalTimer()


    /// <summary>
    /// Stops the renewal time if it is running
    /// </summary>
    public void StopRenewalTimer()
    {
      m_renewalProvider.StopRenewalTimer();
    } // StopRenewalTimer()


    /// <summary>
    /// Renewal Failed event
    /// </summary>
    public event RenewalEventHandler RenewalFailed;

    /// <summary>
    /// Handler for the renewal failed event
    /// </summary>
    /// <param name="args"></param>
    private void OnRenewalFailed(ProviderEventArgs args)
    {

      Logging.Log.Error("Renewal failed : " + args.Message, "ResetSecurityProvider", "OnRenewalFailed");

      if (RenewalFailed != null)
        RenewalFailed(args);

    } // OnRenewalFailed(ProviderEventArgs args)


    #endregion

 
  } // class SecurityProvider

} // namespace Viatronix.Console.Providers
