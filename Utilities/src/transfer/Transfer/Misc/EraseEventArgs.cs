using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Transfer
{

  #region delegates

  /// <summary>
  /// Event to handle erasing.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Arguments to take into account.</param>
  public delegate void EraseEventHandler(object sender, EraseEventArgs args);

  #endregion
  
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
} // namespace Viatronix.Utilities.Transfer
