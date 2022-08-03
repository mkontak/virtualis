using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.SDK.Providers
{

  /// <summary>
  /// Security provider interface. 
  /// </summary>
  public interface ISecurityProvider : IProvider
  {

    #region properties

    /// <summary>
    /// Current user
    /// </summary>
    User User
    { get; }

    /// <summary>
    /// Is Authenticated
    /// </summary>
    bool IsAuthenticated
    { get;  }

    /// <summary>
    /// Gets the context
    /// </summary>
    string Context
    { get; }

    #endregion


    #region methods

    #region domain


    /// <summary>
    /// Gets the domain we are joined to
    /// </summary>
    /// <returns></returns>
    string GetDomain();

    #endregion


    #region login/logout

    /// <summary>
    /// Log into system
    /// </summary>
    /// <param name="user">User name</param>
    /// <param name="password">User password</param>
    /// <param name="context">Context</param>
    /// <returns>User object with context</returns>
    User Login(string login, string password, string context = "");

    /// <summary>
    /// Logs out of the system using the context or the user
    /// <param name="context">Authentication Context/Token</param>
    /// <param name="login">User to logout</param>
    /// </summary>
    void Logout(string login = null);

    /// <summary>
    /// Renews the specified context
    /// <param name="context">Context to renew</param>
    /// </summary>
    void Renew();

    #endregion


    #region user


    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    User GetUser( string username);

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    List<User> GetUsers();

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    List<User> GetUsers( Role role);


    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    List<User> GetUsers( XmlBuilder builder);

    /// <summary>
    /// Insert the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    void Insert(User user, string password);

    /// <summary>
    /// Add the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    void ChangePassword( User user, string password);


    /// <summary>
    /// Delete the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    void Delete( User user);

    /// <summary>
    /// Update the user
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    void Update( User user);

    #endregion


    #region role


    /// <summary>
    /// Get the roles 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    Role GetRole( string name);

    /// <summary>
    /// Get the roles 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    List<Role> GetRoles(XmlBuilder  query);

    /// <summary>
    /// Get the roles 
    /// </summary>
    /// <param name="context"></param>
    /// <returns>Xml of the users</returns>
    List<Role> GetRoles();

    #endregion


    #region rights


    /// <summary>
    /// Get the rights 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    List<Right> GetRights();

    #endregion


    #region authentication types

    /// <summary>
    ///  Gets the this of valid authentication types
    /// </summary>
    /// <returns></returns>
    List<AuthenticationType> GetAuthenticationTypes(XmlBuilder query = null);

    #endregion


    #region UserProfiles methods

    /// <summary>
    /// Adds the user profile
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user"></param>
    void Insert(UserProfile profile);

    /// <summary>
    /// Sets the lock on the study
    /// </summary>
    /// <param name="context"></param>
    /// <param name="user"></param>
    void Delete(UserProfile profile);


    /// <summary>
    /// Get all the user profiles
    /// </summary>
    /// <param name="context"></param>
    /// <param name="series"></param>
    /// <returns></returns>
    Dictionary<string, UserProfile> GetUserProfiles(User user);

    /// <summary>
    /// Get a single user profile
    /// </summary>
    /// <param name="context"></param>
    /// <param name="series"></param>
    /// <returns></returns>
    UserProfile GetUserProfile(string login, string name);




    #endregion


    #region password hints

    /// <summary>
    /// Gets the password hints for the current default password temnplate
    /// </summary>
    /// <returns></returns>
    List<PasswordHint> GetPasswordHints();

    #endregion


    #region Setting

    /// <summary>
    /// Get a single setting
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    SystemSetting GetSetting(string name);

    /// <summary>
    /// Gets a list of settings
    /// </summary>
    /// <param name="query"></param>
    /// <returns></returns>
    List<SystemSetting> GetSettings(XmlBuilder query = null);


    #endregion

    #endregion

  } // interface ISecurityProvider

} // namespace
