using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Utilities.Transfer
{
  #region delegates

  /// <summary>
  /// Delegate to notify the device is empty.
  /// </summary>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="args">Arguments to take into account.</param>
  public delegate void InsufficientMediaCapacityEventHandler(object sender, InsufficientMediaCapacityEventArgs args);

  #endregion

  /// <summary>
  /// Event data to notify the device is empty or disc unreadable.
  /// </summary>
  public class InsufficientMediaCapacityEventArgs : DeviceEmptyEventArgs
  {

    #region fields

    /// <summary>
    /// The minimum capacity required.
    /// </summary>
    private Int64 m_minimumCapacity = 0;

    #endregion

    #region contstruction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="minimumCapacity">The minimum capacity required.</param>
    public InsufficientMediaCapacityEventArgs(Int64 minimumCapacity)
    {
      m_minimumCapacity = minimumCapacity;
    } // InsufficientMediaCapacityEventArgs( minimumCapacity )


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="minimumCapacity">The minimum capacity required.</param>
    /// <param name="abort">Abort?</param>
    public InsufficientMediaCapacityEventArgs(Int64 minimumCapacity, bool abort) : base(abort)
    {
      m_minimumCapacity = minimumCapacity;
    } // InsufficientMediaCapacityEventArgs( minimumCapacity, abort )

    #endregion

    #region properties

    /// <summary>
    /// Gets the minimum disc capacity required for transfer.
    /// </summary>
    public Int64 MinimumCapacity
    {
      get { return m_minimumCapacity; }
    } // MinimumCapacity

    #endregion

  } // class InsufficientMediaCapacityEventArgs
} // namespace Viatronix.Utilities.Transfer
