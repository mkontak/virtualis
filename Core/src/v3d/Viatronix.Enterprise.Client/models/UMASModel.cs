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
  /// Back end for adding/modifying/deleting users.
  /// </summary>
  public class UMASModel
  {

    #region events



    #endregion

    #region fields

    /// <summary>
    /// Security provider.
    /// </summary>
    private ISecurityProvider m_security = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="security">A security provider to perform the requisite operations.</param>
    public UMASModel(ISecurityProvider security)
    {
      m_security = security;
    } // UMASModel( security )

    #endregion

    #region properties

    /// <summary>
    /// Gets the list of users in the system.
    /// </summary>
    public List<User> Users
    {
      get { return m_security.GetUsers(); }
    } // Users


    /// <summary>
    /// Gets the list of roles in the system.
    /// </summary>
    public List<Role> Roles
    {
      get { return m_security.GetRoles(); }
    } // Roles

    #endregion

    #region methods

    /// <summary>
    /// Gets the user.
    /// </summary>
    /// <param name="username">The username.</param>
    /// <returns>The user with username.</returns>
    public User GetUser(string username)
    {
      return m_security.GetUser(username);
    } // GetUser( username )


    /// <summary>
    /// Determines if the UMAS tab is available to a given user.
    /// </summary>
    /// <param name="user">The user to whom access is to be granted/denied.</param>
    /// <returns>True if the UMAS module should be available to the user, false otherwise.</returns>
    public bool IsUMASAvailable(User user)
    {
      if (user.IsService || user.IsAdministrator)
      {
        return true;
      }

      return false;
    } // IsUMASAvailable( user )


    /// <summary>
    /// Deletes the user.
    /// </summary>
    /// <param name="user">The user to delete.</param>
    public void Delete(User user)
    {
      m_security.Delete(user);
    } // Delete( user )


    /// <summary>
    /// Unlocks the user.
    /// </summary>
    /// <param name="user">The user to be unlocked.</param>
    public void UnlockUser(User user)
    {
      m_security.Logout(user.Login);
    } // UnlockUser( user )

    #endregion

  } // class UMASModel
} // namespace Viatronix.Enterprise.SDK.Models
