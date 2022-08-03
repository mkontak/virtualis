using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Enterprise.Models
{
  /// <summary>
  /// Model to add/modify a user.
  /// </summary>
  public class UserManagementModel
  {

    #region fields

    /// <summary>
    /// The security provider.
    /// </summary>
    private ISecurityProvider m_security = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="security">The security provider.</param>
    public UserManagementModel(ISecurityProvider security)
    {
      m_security = security;
    } // UserManagementModel( security )

    #endregion

    #region properties

    /// <summary>
    /// Gets the list of authentication types.
    /// </summary>
    public AuthenticationType[] AuthenticationTypes
    {
      get { return m_security.GetAuthenticationTypes().ToArray(); }
    } // AuthenticationTypes


    /// <summary>
    /// Gets the authentication type.
    /// </summary>
    public AuthenticationType DefaultAuthenticationType
    {
      get { return m_security.GetAuthenticationTypes().FindDefault(); }
    } // DefaultAuthenticationType


    /// <summary>
    /// Gets the default expiration for a user.
    /// </summary>
    public int DefaultUserExpiration
    {
      get { return Convert.ToInt32(GetSetting("DefaultUserExpiration")); }
    } // DefaultUserExpiration

    #endregion

    #region methods

    /// <summary>
    /// Gets the password hints.
    /// </summary>
    /// <returns>The list of password hints.</returns>
    public string GetPasswordHints()
    {
      return string.Empty;
    } // GetPasswordHints()


    /// <summary>
    /// Gets a setting.
    /// </summary>
    /// <param name="name">The name of the setting to retrieve.</param>
    /// <returns>The setting's value.</returns>
    public string GetSetting(string name)
    {
      return m_security.GetSetting(name) != null ? m_security.GetSetting(name).Value : null;
    } // GetSetting( name )


    /// <summary>
    /// Inserts the user.
    /// </summary>
    /// <param name="user">The user to insert.</param>
    /// <param name="password">The user's password.</param>
    public void Insert(User user, string password)
    {
      m_security.Insert(user, password);
    } // Insert( user, password )


    /// <summary>
    /// Updates the user.
    /// </summary>
    /// <param name="user">The new user data.</param>
    public void Update(User user)
    {
      m_security.Update(user);
    } // Update( user )

    #endregion

  } // class UserManagementModel
} // namespace Viatronix.Enterprise.Models
