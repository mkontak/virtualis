using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Transfer
{

  #region delegates

  /// <summary>
  /// A delegate to notifiy progress.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Argument to take into account.</param>
  public delegate void ProgressEventHandler(object sender, ProgressEventArgs args);

  #endregion

  /// <summary>
  /// Arguments that describe progress.
  /// </summary>
  public class ProgressEventArgs : EventArgs
  {

    #region fields

    /// <summary>
    /// The message.
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// The current progress.
    /// </summary>
    private int m_progress = 0;

    /// <summary>
    /// The max progress.
    /// </summary>
    private int m_maxProgress = 100;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="progress">The progress.</param>
    /// <param name="maxProgress">The highest progress.</param>
    public ProgressEventArgs(string message, int progress, int maxProgress)
    {
      m_message = message;
      m_progress = progress;
      m_maxProgress = maxProgress;
    } // ProgressEventArgs( message, progress, maxProgress )

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
    /// Gets the max progress.
    /// </summary>
    public int MaxProgress
    {
      get { return m_maxProgress; }
    } // MaxProgress

    #endregion

  } // class ProgressEventArgs
} // namespace Viatronix.Utilities.Transfer
