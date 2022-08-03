using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Imapi;

namespace Viatronix.Utilities.Transfer
{

  #region LoadAbortException class

  /// <summary>
  /// Notifies that the media is not blank.
  /// </summary>
  public class LoadAbortException : AbortException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to display.</param>
    public LoadAbortException(string message)
      : base(message)
    {
    } // LoadAbortException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to display.</param>
    /// <param name="innerException">The exception that caused this one.</param>
    public LoadAbortException(string message, Exception innerException)
      : base(message, innerException)
    {
    } // LoadAbortException( message, innerException )

    #endregion

  } // class LoadAbortException

  #endregion

} // namespace Viatronix.Utilities.Transfer
