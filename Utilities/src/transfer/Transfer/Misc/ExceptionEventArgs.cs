using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Transfer
{

  #region delegates

  /// <summary>
  /// Handler for events that notify that an error has occurred.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Arguments to take into account.</param>
  public delegate void ExceptionEventHandler(object sender, ExceptionEventArgs args);

  #endregion

  /// <summary>
  /// Arguments for an event to notify an exception occurs.
  /// </summary>
  public class ExceptionEventArgs : EventArgs
  {

    #region fields

    /// <summary>
    /// The exception.
    /// </summary>
    private Exception m_exception;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="exception">The exception to pass along.</param>
    public ExceptionEventArgs(Exception exception)
    {
      m_exception = exception;
    } // ExceptionEventArgs( exception )

    #endregion

    #region properties

    /// <summary>
    /// Gets the exception.
    /// </summary>
    public Exception Exception
    {
      get { return m_exception; }
    } // Exception

    #endregion

  } // class ExceptionEventArgs
} // namespace Viatronix.Utilities.Transfer
