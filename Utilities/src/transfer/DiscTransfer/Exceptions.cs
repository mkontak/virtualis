using System;
using System.IO;

namespace Viatronix.Utilities.Transfer.DiscTransfer
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
    public LoadAbortException(string message) : base(message)
    {
    } // LoadAbortException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to display.</param>
    /// <param name="innerException">The exception that caused this one.</param>
    public LoadAbortException(string message, Exception innerException) : base(message, innerException)
    {
    } // LoadAbortException( message, innerException )

    #endregion

  } // class LoadAbortException

  #endregion

  #region InsufficientCapacityException class

  /// <summary>
  /// Notifies that the media is not blank.
  /// </summary>
  public class InsufficientCapacityException : IOException
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public InsufficientCapacityException() : base()
    {
    } // InsufficientCapacityException()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to display.</param>
    public InsufficientCapacityException(string message) : base(message)
    {
    } // InsufficientCapacityException( message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to display.</param>
    /// <param name="innerException">The exception that caused this one.</param>
    public InsufficientCapacityException(string message, Exception innerException) : base(message, innerException)
    {
    } // InsufficientCapacityException( message, innerException )

    #endregion

  } // class InsufficientCapacityException

  #endregion

} // namespace Viatronix.Utilities.Transfer.DiscTransfer
