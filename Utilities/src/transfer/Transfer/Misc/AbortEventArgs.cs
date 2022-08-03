using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Transfer
{
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
} // namespace Viatronix.Utilities.Transfer
