using System;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Enterprise.Models
{
  /// <summary>
  /// Password change model.
  /// </summary>
  public class PasswordChangeModel
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
    public PasswordChangeModel(ISecurityProvider security)
    {
      m_security = security;
    } // PasswordChangeModel( security )

    #endregion

    #region properties

    #endregion

    #region methods

    /// <summary>
    /// Changes the password.
    /// </summary>
    /// <param name="user">The user, whose password to change.</param>
    /// <param name="originalPassword">The original password.</param>
    /// <param name="newPassword">The new password.</param>
    public void ChangePassword(User user, string originalPassword, string newPassword)
    {
      m_security.ChangePassword(user, newPassword);
    } // ChangePassword( user, originalPassword, newPassword )


    /// <summary>
    /// Gets the password hints.
    /// </summary>
    /// <returns>The list of password hints.</returns>
    public string GetPasswordHints()
    {
      return string.Empty;
    } // GetPasswordHints()

    #endregion

  } // class PasswordChangeModel
} // namespace Viatronix.Enterprise.Models
