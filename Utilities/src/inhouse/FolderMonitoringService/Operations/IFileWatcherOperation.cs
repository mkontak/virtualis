// Copyright © 2011, Viatronix Inc., All Rights Reserved.
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

namespace Viatronix.Operations
{
  /// <summary>
  /// An operation run every so often by the Server.
  /// </summary>
  interface IFileWatcherOperation
  {

    /// <summary>
    /// Executes the operation.
    /// </summary>
    /// <param name="folders">The list of folders the operation needs to be applied to.</param>
    void Execute(Dictionary<string, Folder> folders);

  } // interface IFileWatcherOperation
} // namespace Viatronix.Operations
