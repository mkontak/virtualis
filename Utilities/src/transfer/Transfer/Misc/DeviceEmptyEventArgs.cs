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
  public delegate void DeviceEmptyEventHandler(object sender, DeviceEmptyEventArgs args);

  #endregion

  /// <summary>
  /// Event data to notify the device is empty or disc unreadable.
  /// </summary>
  public class DeviceEmptyEventArgs : AbortEventArgs
  {

    #region contstruction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DeviceEmptyEventArgs()
    {
    } // LoadEventArgs()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="abort">Abort?</param>
    public DeviceEmptyEventArgs(bool abort) : base(abort)
    {
    } // LoadEventArgs( abort )

    #endregion

  } // class DeviceEmptyEventArgs
} // namespace Viatronix.Utilities.Transfer
