using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// States of the Series/Studies
  /// </summary>
  [Flags]
  public enum States
  {
    None        = 0x0000,
    Dirty       = 0x0001,
    Receiving   = 0x0002,
    Processing  = 0x0004,
    Converting  = 0x0008,
    Completed   = 0x0010,
    Pending     = 0x0020,
    Failed      = 0x0040,
    Aborted     = 0x0080,
    Limited     = 0x0100,
    Open        = 0x1000
  };

}
