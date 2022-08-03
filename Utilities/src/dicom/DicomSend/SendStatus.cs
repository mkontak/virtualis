// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Dicom.DicomSend
{
  /// <summary>
  /// The status of the send.
  /// </summary>
  public enum SendStatus
  {
    Pending,
    Completed,
    Failed
  } // enum SendStatus
} // namespace Viatronix.Utilities.Dicom.DicomSend
