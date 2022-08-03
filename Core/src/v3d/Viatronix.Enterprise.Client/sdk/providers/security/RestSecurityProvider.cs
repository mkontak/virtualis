using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Messaging;
using System.Globalization;

namespace Viatronix.Enterprise.SDK.Providers
{

  public sealed class RestSecurityProvider : ProviderBase, ISecurityProvider, IRenewalProvider, IDisposable
  {

    #region fields

 
    /// <summary>
    /// Renewal timer
    /// </summary>
    private RenewalProvider m_renewalProvider = null;
 
    /// <summary>
    /// Rest client
    /// </summary>
    private HttpClient m_client = null;

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

    #region construction



    /// <summary>
    /// private constructor
    /// </summary>
    public RestSecurityProvider(string xml)
      : base(xml)
    {


      /// Initilaize the timer
      m_renewalProvider = new RenewalProvider(xml, (ISecurityProvider)this);
      m_renewalProvider.RenewalFailed += new RenewalEventHandler(OnRenewalFailed);


      string url = string.Empty;

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      // Try to get the url necessary to connect
      if (!doc.DocumentElement.TryGetAttributeValue<string>("url", ref url))
        throw new MissingAttributeException("Missing url in the initialization xml");

      

      // Create a new rest client using the url
      m_client = new HttpClient(url);

      //====================================================================
      // Read in the client certificate which is embedded in the program
      //====================================================================
      using (System.IO.Stream stream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.Enterprise.res.client.pfx"))
      {
        try
        {
          // Allocate a buffer to read in the certificate
          byte[] certificateBuffer = new byte[stream.Length];

          // Read the entire certificate to the bufer
          stream.Read(certificateBuffer, 0, (int)stream.Length);

          // Set the private client certificate
          m_client.Certificate = new System.Security.Cryptography.X509Certificates.X509Certificate2(certificateBuffer);
        }
        catch (Exception ex)
        {
          throw new Exception("Failed to create enterprise client : " + ex.Message);
        }
      } // using

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
    /// Returns the authentication token/context
    /// </summary>
    public  string Context
    { get { return m_context; } }

    /// <summary>
    /// Returns the user
    /// </summary>
    public  User User
    { get { return m_user; }} // User

    /// <summary>
    /// Determines if we are authenticated
    /// </summary>
    public  override bool IsAuthenticated
    { get { return !string.IsNullOrEmpty(m_context); } }
    

    #endregion

    #region create methods

    /// <summary>
    /// Creates a new instance of RestSecurityProvider
    /// </summary>
    /// <returns>RestSecurityProvider object</returns>
    public static RestSecurityProvider Create(string xml)
    {

      // new client
      RestSecurityProvider provider = new RestSecurityProvider(xml);

      if (provider == null)
        throw new OutOfMemoryException("Failed to allocate RestSecurityProvider");


      return provider;

    } // Create()



    /// <summary>
    /// Sends an echo message
    /// </summary>
    /// <param name="message">Message to be echoed</param>
    /// <returns>The message echo by the server</returns>
    public string Echo(string message)
    {
      try
      {
        return m_client.Echo(message);
      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Echo");
        throw e;
      }
    } // Echo( message )


    /// <summary>
    /// Sends an xml requests to retrieve data
    /// </summary>
    /// <param name="input">xml request</param>
    /// <returns>xml response</returns>
    public string Get(string input)
    {

      try
      {
        if (!IsAuthenticated)
          throw new InvalidOperationException("This operation requires a login");


        XmlNode node = input.ToXmlNode();
        node.ValidateSchema(Schema.Settings);

        ResponseMessage response = m_client.Get(RequestMessage.Create(m_context,
                                                                        OperationTypes.Get,
                                                                        XmlBuilder.Create(node.Name.ToLower(), input)));


        if (response.Failed)
          throw ExceptionFactory.Create(response.Message, response.ErrorCode);

        return response.Message;


      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Get");
        throw e;

      }

    } // Get( input )

    /// <summary>
    /// Sends an xml requests to add data
    /// </summary>
    /// <param name="input">xml request</param>
    /// <returns>xml response</returns>
    public string Insert(string input)
    {

      try
      {
        if (!IsAuthenticated)
          throw new InvalidOperationException("This operation requires a login");


        XmlNode node = input.ToXmlNode();
        node.ValidateSchema(Schema.Settings);

        ResponseMessage response = m_client.Put(RequestMessage.Create(m_context,
                                                                        OperationTypes.Insert,
                                                                        XmlBuilder.Create(node.Name.ToLower(), input)));


        if (response.Failed)
          throw ExceptionFactory.Create(response.Message, response.ErrorCode);

        return response.Message;


      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Add");
        throw e;

      }

    } // Delete( input )



    /// <summary>
    /// Sends an xml requests to retrieve data
    /// </summary>
    /// <param name="input">xml request</param>
    /// <returns>xml response</returns>
    public string Update(string input)
    {

      try
      {
        if (!IsAuthenticated)
          throw new InvalidOperationException("This operation requires a login");


        XmlNode node = input.ToXmlNode();
        node.ValidateSchema(Schema.Settings);

        ResponseMessage response = m_client.Post(RequestMessage.Create(m_context,
                                                                        OperationTypes.Update,
                                                                        XmlBuilder.Create(node.Name.ToLower(), input)));


        if (response.Failed)
          throw ExceptionFactory.Create(response.Message, response.ErrorCode);

        return response.Message;


      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Update");
        throw e;

      }

    } // Get( input )

    /// <summary>
    /// Sends an xml requests to retrieve data
    /// </summary>
    /// <param name="input">xml request</param>
    /// <returns>xml response</returns>
    public string Delete(string input)
    {

      try
      {
        if (!IsAuthenticated)
          throw new InvalidOperationException("This operation requires a login");


        XmlNode node = input.ToXmlNode();
        node.ValidateSchema(Schema.Settings);

        ResponseMessage response = m_client.Delete(RequestMessage.Create(m_context,
                                                                        OperationTypes.Delete,
                                                                        XmlBuilder.Create(node.Name.ToLower(), input)));


        if (response.Failed)
          throw ExceptionFactory.Create(response.Message, response.ErrorCode);

        return response.Message;


      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Delete");
        throw e;

      }

    } // Delete( input )


    #endregion


    #region ISecurity Methods


    #region Login/Logout/Renew methods

    /// <summary>
    /// Sends a login
    /// </summary>
    /// <param name="username">username</param>
    /// <param name="password">password</param>
    /// <param name="context">Context</param>
    /// <returns>user xml</returns>
    public  User Login(string username, string password, string context = "")
    {

      // Make sure the provider is not already authenticated with a login
      if (IsAuthenticated && ( m_user != null && username != m_user.Login))
        throw new UserAlreadyHasContextException(string.Format("This provider is already logged in as {0}", m_user.Login));


      ResponseMessage response = m_client.Post(
          RequestMessage.Create(string.Empty, OperationTypes.Acquire,
                                XmlBuilder.Create("context", new Dictionary<string, string>() { { "login", username }, 
                                                                                                { "pwd", Viatronix.Encryption.Crypto.Encrypt(password) }, 
                                                                                                { "id", context } } )));

      if (response.Failed)
        throw ExceptionFactory.Create(response.Message, response.ErrorCode);

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(response.Message);

      m_context = doc.DocumentElement.Attributes["id"].Value;

      response = m_client.Get(RequestMessage.Create(m_context, OperationTypes.Get, XmlBuilder.Create("user", new Dictionary<string, string>() { { "login", username } })));

      if (response.Failed)
        throw ExceptionFactory.Create(response.Message, response.ErrorCode);



      List<User> users = new List<User>();

      EntityCollection.Load(users, response.Message);


      if (users.Count == 0)
        throw new LoginFailedException("Failed to log into the enterprise system, no user returned");

      m_user = users[0];

      StartRenewalTimer();

      return m_user;

    } // Login( username, password )


    /// <summary>
    /// Sends a logout
    /// </summary>
    public  void Logout(string login = "")
    {
      try
      {

        if (!IsAuthenticated)
        {
          Log.Debug("Must be an logged in to perform this action.", "RestSecurityProvider", "Logout");
          return;
        }


        // ==========================================================================
        // If the login is specified then it is an administrator logging out a user
        // ==========================================================================
        if (!string.IsNullOrEmpty(login))
        {

          // =========================================================================
          // Cannot logout yourself just call Logout() without the login specified
          // =========================================================================
          if (m_user.Login == login)
            throw new InvalidOperationException("This method cannot be used with the \"login\" parameter to logout your account. Please use Logout()");

          Log.Information(string.Format("Logging out {0}", login), "RestSecurityProvider", "Logout");

          // =========================================================
          // Release the context associated with the login specified
          // =========================================================
          m_client.Post(RequestMessage.Create(m_context,
                                              OperationTypes.Release,
                                              XmlBuilder.Create("context", new Dictionary<string, string>() { { "login", login } })));

        }
        else
        {

          Log.Information("Logging out", "RestSecurityProvider", "Logout");

          // ======================================================
          // Check context just in case of multiple logout calls
          // ======================================================
          if (!string.IsNullOrEmpty(m_context))
          {


            // ===================================================
            // Release the context associated with this instance
            // ===================================================
            m_client.Post(RequestMessage.Create(m_context,
                                                OperationTypes.Release,
                                                XmlBuilder.Create("context", new Dictionary<string, string>() { { "id", m_context } })));
          }
        }


      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Logout");
        throw e;
      }
      finally
      {

        /// Stop the renewal 
        StopRenewalTimer();

        m_user = null;
        m_context = string.Empty;
      }
    } // Logout()



    /// <summary>
    /// Sends a renew
    /// </summary>
    public  void Renew()
    {
      try
      {

        ResponseMessage response = m_client.Post(RequestMessage.Create(m_context, OperationTypes.Renew, XmlBuilder.Create("context", new Dictionary<string, string>() { { "id", m_context } })));


        if (response.Failed)
          throw ExceptionFactory.Create(response.Message, response.ErrorCode);

      }
      catch (NullReferenceException)
      {
        Log.Debug("Must be an logged in to perform this action.", "RestSecurityProvider", "Renew");
        m_user = null;
      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Renew");
        m_user = null;
        throw e;
      }
    } // Renew()


    /// <summary>
    /// Sends a change password
    /// </summary>
    /// <param name="password">new password</param>
    public void ChangePassword(string password)
    {
      if (!IsAuthenticated)
      {
        Log.Debug("Must be an logged in to perform this action.", "RestSecurityProvider", "ChangePassword");
        return;
      }

      ChangePassword(m_user, password);
    } // ChangePassword( password )

  /// <summary>
    /// Sends a change password
    /// </summary>
    /// <param name="login">login</param>
    /// <param name="password">new password</param>
    public void ChangePassword(User user, string password)
    {
      try
      {
        if (!IsAuthenticated)
        {
          Log.Debug("Must be an logged in to perform this action.", "RestSecurityProvider", "ChangePassword");
          return;
        }


        ResponseMessage response = m_client.Post(
              RequestMessage.Create(m_context, OperationTypes.Delete,
                                    XmlBuilder.Create("user", new Dictionary<string, string>() { { "login", user.Login }, { "pwd", Viatronix.Encryption.Crypto.Encrypt(password) } })));


        if (response.Failed)
          throw ExceptionFactory.Create(response.Message, response.ErrorCode);

      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "ChangePassword");
        throw e;
      }

    }
 
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

      // ========================================
      // Make sure the user name is specified
      // =======================================
      if (!string.IsNullOrEmpty(login))
      {


        ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("user", (new Dictionary<string, string>() { { "login", login } })));

        if (response.Failed)
          throw ExceptionFactory.Create(response.Message, response.ErrorCode);

        return User.Create(response.Message);

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

      return GetUsers(XmlBuilder.Create("user"));

    } // GetUsers( XmlBuilder builder)

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public  List<User> GetUsers(Role role)
    {

      return GetUsers(XmlBuilder.Create("user", new Dictionary<string, string>() { { "role", role.Name } }));
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
        throw new InvalidOperationException("This operation requires a login");

      List<User> users = new List<User>();

      ResponseMessage response = m_client.Get(m_context, builder);

      if (response.Failed)
        throw ExceptionFactory.Create(response.Message, response.ErrorCode);

      EntityCollection.Load(users, response.Message);

      return users;
    } // GetUsers( XmlBuilder builder)


    /// <summary>
    /// Insert the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public void Insert(User user, string password)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      user.Password = password;

      m_client.Insert(m_context, XmlBuilder.Create(user));

    } // AddUser( User user)




    /// <summary>
    /// Sends an add user request
    /// </summary>
    /// <param name="xml">user xml</param>
    /// <returns>user xml</returns>
    public string Insert(User user)
    {

      try
      {

        if (!IsAuthenticated)
          throw new InvalidOperationException("This operation requires a login");

        ResponseMessage response = m_client.Insert(m_context, XmlBuilder.Create(user));


        return response.Message;

      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Insert");
        throw e;
      }
    } // AddUser( xml )


    /// <summary>
    /// Sends a remove user request
    /// </summary>
    /// <param name="id">user xml</param>
    public void Delete(User user)
    {
      try
      {

        if (!IsAuthenticated)
          throw new InvalidOperationException("This operation requires a login");

        m_client.Delete(m_context, XmlBuilder.Create("user", new Dictionary<string, string>() { { "login", user.Login } }));

      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Delete");
        throw e;
      }
    } // RemoveUser( xml )


    /// <summary>
    /// Sends an edit user request
    /// </summary>
    /// <param name="xml">user xml</param>
    /// <returns>user xml</returns>
    public void Update(User user)
    {
      try
      {

        if (!IsAuthenticated)
          throw new InvalidOperationException("This operation requires a login");

        ResponseMessage response = m_client.Update(m_context, XmlBuilder.Create(user));


      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RestSecurityProvider", "Update");
        throw e;
      }
    } // EditUser( xml )

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
        throw new InvalidOperationException("This operation requires a login");

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
        throw new InvalidOperationException("This operation requires a login");

      List<Role> roles = new List<Role>();

      ResponseMessage response = m_client.Get(m_context, builder);

      EntityCollection.Load(roles, response.Message);

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
        throw new InvalidOperationException("This operation requires a login");

      List<Role> roles = new List<Role>();

      ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("role"));

      EntityCollection.Load(roles, response.Message);

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
        throw new InvalidOperationException("This operation requires a login");

      List<AuthenticationType> types = new List<AuthenticationType>();

      ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("authenticationtype"));

      EntityCollection.Load(types, response.Message);

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
        throw new InvalidOperationException("This operation requires a login");

      List<Right> rights = new List<Right>();

      ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("right"));

      EntityCollection.Load(rights, response.Message);

      return rights;
    } // GetRight( XmlBuilder builder)



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
    public  List<SystemSetting> GetSettings(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      List<SystemSetting> settings = new List<SystemSetting>();

      ResponseMessage response = m_client.Get(m_context, (query == null ? XmlBuilder.Create("systemsetting") : query));

      EntityCollection.Load(settings, response.Message);

      return settings;
    } // GetSettings()

    #endregion

    #region Domain

    /// <summary>
    /// Gets the domain we are on
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public  string GetDomain()
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");


      ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("domain"));

      string xml = response.Message;

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return doc.DocumentElement.Attributes["name"].Value;

    } // GetDomain()

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

      ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("passwordhint"));

      EntityCollection.Load(hints, response.Message);
      return hints;

    }

    #endregion


    #region UserProfiles methods

    /// <summary>
    /// Insert the user config
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user"></param>
    public  void Insert(UserProfile profile)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      m_client.Insert(m_context, XmlBuilder.Create(profile));
    }

    /// <summary>
    /// Sets the lock on the study
    /// </summary>
    /// <param name="context"></param>
    /// <param name="study"></param>
    public  void Delete(UserProfile profile)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      m_client.Delete(m_context, XmlBuilder.Create(profile));
    }


    /// <summary>
    /// Get the lock for the series specified
    /// </summary>
    /// <param name="context"></param>
    /// <param name="series"></param>
    /// <returns></returns>
    public  Dictionary<string, UserProfile> GetUserProfiles(User user)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      List<UserProfile> profiles = new List<UserProfile>();


      ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("userprofile", new Dictionary<string, string>() { { "login", user.Login } }));

      EntityCollection.Load(profiles, response.Message);


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
    public  UserProfile GetUserProfile(string login, string name)
    {
      if (!IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      List<UserProfile> profiles = new List<UserProfile>();


      ResponseMessage response = m_client.Get(m_context, XmlBuilder.Create("userprofile", new Dictionary<string, string>() { { "login", login }, { "name", name } }));

      EntityCollection.Load(profiles, response.Message);

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

      m_client.Update(m_context, XmlBuilder.Create(profile));
    }


    #endregion

    #endregion


  
    #region IDisposable Members


    /// <summary>
    /// Disposes of object
    /// </summary>
    public  void Dispose()
    {
      Dispose(true);
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

          m_client = null;
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

      Log.Error("Renewal failed : " + args.Message, "ResetSecurityProvider","OnRenewalFailed");

      if ( RenewalFailed != null )
        RenewalFailed(args);

    } // OnRenewalFailed(ProviderEventArgs args)


    #endregion


  } // class RestProvider

} // namespace Viatronix
