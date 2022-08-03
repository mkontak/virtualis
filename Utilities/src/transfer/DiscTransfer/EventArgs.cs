using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Transfer.DiscTransfer
{

  #region delegates

  /// <summary>
  /// Event Handler to handle an exception.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">The error that occurred.</param>
  public delegate void ExceptionEventHandler(object sender, Exception args);

  /// <summary>
  /// Event to handle progress.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Arguments to take into account.</param>
  public delegate void ProgressEventHandler(object sender, ProgressEventArgs args);

  /// <summary>
  /// Event to handle loading.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Arguments to take into account.</param>
  public delegate void LoadEventHandler(object sender, LoadEventArgs args);

  /// <summary>
  /// Event to handle erasing.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Arguments to take into account.</param>
  public delegate void EraseEventHandler(object sender, EraseEventArgs args);

  #endregion

  #region AbortEventArgs class

  /// <summary>
  /// Represents arguments for an abort event.
  /// </summary>
  public class AbortEventArgs
  {

    #region fields

    /// <summary>
    /// Are we clear to abort?
    /// </summary>
    private bool m_abort = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public AbortEventArgs()
    {
    } // AbortEventArgs()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="abort">The abort flag.</param>
    public AbortEventArgs(bool abort)
    {
      m_abort = abort;
    } // AbortEventArgs( abort )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the abort.
    /// </summary>
    public bool Abort
    {
      get { return m_abort; }
      set { m_abort = value; }
    } // Abort

    #endregion

  } // class AbortEventArgs

  #endregion

  #region EraseEventArgs class

  /// <summary>
  /// Information about the erase operation.
  /// </summary>
  public class EraseEventArgs : AbortEventArgs
  {

    #region fields

    /// <summary>
    /// Should the disk be erased?
    /// </summary>
    private bool m_erase = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="erase">Should we erase the disk?</param>
    public EraseEventArgs(bool erase)
    {
      m_erase = erase;
    } // EraseEventArgs( erase )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets if the disk should be erased.
    /// </summary>
    public bool Erase
    {
      get { return m_erase; }
      set { m_erase = value; }
    } // Erase

    #endregion

  } // class EraseEventArgs

  #endregion

  #region LoadEventArgs class

  /// <summary>
  /// Event data for load operations.
  /// </summary>
  public class LoadEventArgs : AbortEventArgs
  {

    #region fields

    /// <summary>
    /// The load message.
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// If we should eject.
    /// </summary>
    private bool m_eject = false;

    #endregion

    #region contstruction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="index">The disk index.</param>
    /// <param name="message">A message to display.</param>
    public LoadEventArgs(string message)
    {
      m_message = message;
    } // LoadEventArgs( index, message )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">A message to display.</param>
    /// <param name="eject">Force eject?</param>
    public LoadEventArgs(string message, bool eject)
    {
      m_message = message;
      m_eject = eject;
    } // LoadEventArgs( message, eject )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">A message to display.</param>
    /// <param name="eject">Force eject?</param>
    /// <param name="abort">Abort?</param>
    public LoadEventArgs(string message, bool eject, bool abort) : base(abort)
    {
      m_message = message;
      m_eject = eject;
    } // LoadEventArgs( message, eject )

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
    /// Gets or sets if we should eject.
    /// </summary>
    public bool Eject
    {
      get { return m_eject; }
      set { m_eject = value; }
    } // Eject

    #endregion

  } // class LoadEventArgs

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

} // namespace Viatronix.Utilities.Transfer.DiscTransfer
