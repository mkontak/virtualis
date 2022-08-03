using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{
  /// <summary>
  /// An exception that is generic and starts with an object that it applies to.
  /// </summary>
  public class EntityException : ViatronixException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="errorCode">The error code.</param>
    public EntityException(string message, ErrorCodes errorCode) : base(message, errorCode)
    {
    } // EntityException( message, errorCode )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    /// <param name="errorCode">The error code.</param>
    public EntityException(string message, Exception innerException, ErrorCodes errorCode) : base(message, innerException, errorCode)
    {
    } // EntityException( message, innerException, errorCode )

    #endregion

    #region properties

    /// <summary>
    /// Gets the entity.
    /// </summary>
    public string Entity
    {
      get { return Message.Substring(0, Message.IndexOf(":")); }
    } // Entity

    #endregion

  } // class EntityException
} // namespace Viatronix.Enterprise
