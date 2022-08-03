using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise
{
  /// <summary>
  /// Viatronix exception class.
  /// </summary>
  public class ViatronixException : Exception
  {

    #region fields

    /// <summary>
    /// Error code.
    /// </summary>
    private ErrorCodes m_errorCode = ErrorCodes.None;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="errorCode">The error code.</param>
    public ViatronixException(string message, ErrorCodes errorCode) : base(message)
    {
      m_errorCode = errorCode;
    } // ViatronixException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="innerException">The inner exception.</param>
    /// <param name="errorCode">The error code.</param>
    public ViatronixException(string message, Exception innerException, ErrorCodes errorCode) : base(message, innerException)
    {
      m_errorCode = errorCode;
    } // ViatronixException( message, innerException, errorCode )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the errorcode.
    /// </summary>
    public ErrorCodes ErrorCode
    {
      get { return m_errorCode; }
      set { m_errorCode = value; }
    } // ErrorCode

    #endregion

  } // class ViatronixException
} // namespace Viatronix.Enterprise
