using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Dicom.CreateVolume.Model
{

  /// <summary>
  /// Delegate for the create volume events
  /// </summary>
  /// <param name="args"></param>
  public delegate void ConversionDelegate(ConversionEventArgs args);

  /// <summary>
  /// Evenet args for the create volume model
  /// </summary>
  public class ConversionEventArgs : System.EventArgs
  {

    #region fields

    /// <summary>
    /// Remaining
    /// </summary>
    private int m_remaining = 0;

    /// <summary>
    /// Completed
    /// </summary>
    private int m_completed = 0;

    /// <summary>
    /// Message
    /// </summary>
    private string m_message = string.Empty;

    #endregion

    #region coinstructor

    /// <summary>
    /// Constrctor
    /// </summary>
    /// <param name="remaining"></param>
    /// <param name="completed"></param>
    /// <param name="message"></param>
    public ConversionEventArgs(int remaining, int completed, string message)
    {
      m_remaining = remaining;
      m_completed = completed;
      m_message = message;
    } // ConversionEventArgs( int remaining, int completed, string message )
    

    #endregion

    #region properties

    /// <summary>
    /// Gets the message
    /// </summary>
    public string Message
    {
      get { return m_message; }
    } // Message

    /// <summary>
    /// Gets the remaining value
    /// </summary>
    public int Remaining
    {
      get { return m_remaining; }
    } // Remaining

    /// <summary>
    /// Gets the completed value
    /// </summary>
    public int Completed
    {
      get { return m_completed; }
    }// Completed
    #endregion

  }
}
