using System;

namespace Viatronix.Imapi
{

  #region delegates

  /// <summary>
  /// Event to handle progress.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Arguments to take into account.</param>
  public delegate void ProgressEventHandler(object sender, ProgressEventArgs args);

  #endregion

  #region ProgressEventArgs class

  /// <summary>
  /// Arguments for progress.
  /// </summary>
  public class ProgressEventArgs
  {

    #region fields

    /// <summary>
    /// The message to display.
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// The progress, [0, total].
    /// </summary>
    private int m_progress = 0;

    /// <summary>
    /// The total.
    /// </summary>
    private int m_total = 0;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message to display.</param>
    /// <param name="progress">The progress.</param>
    /// <param name="total">The total progress.</param>
    public ProgressEventArgs(string message, int progress, int total)
    {
      m_message = message;
      m_progress = progress;
      m_total = total;
    } // ProgressEventArgs( message, progress, total )

    #endregion

    #region properties

    /// <summary>
    /// Gets the message.
    /// </summary>
    public string Message
    {
      get { return m_message; }
    } // Message


    /// <summary>
    /// Gets the progress.
    /// </summary>
    public int Progress
    {
      get { return m_progress; }
    } // Progress


    /// <summary>
    /// Gets the total.
    /// </summary>
    public int Total
    {
      get { return m_total; }
    } // Total

    #endregion

  } // class ProgressEventArgs

  #endregion

} // namespace Viatronix.Imapi
