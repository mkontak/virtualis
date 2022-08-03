// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.IO;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Operations
{
  /// <summary>
  /// Checks every so often to clear out old files.
  /// </summary>
  public class OldFileCleanupOperation : IFileWatcherOperation
  {

    #region IFileWatcherOperation implementation

    /// <summary>
    /// Executes the operation.
    /// </summary>
    /// <param name="folders">The list of folders the operation needs to be applied to.</param>
    public void Execute(Dictionary<string, Folder> folders)
    {
      int cutoff = Convert.ToInt32(ConfigurationManager.AppSettings["cutOffDays"]);
      foreach (KeyValuePair<string, Folder> folder in folders)
      {
        foreach (string file in Directory.GetFiles(folder.Value.Path))
        {
          if ((DateTime.Now - File.GetCreationTime(file)).TotalDays >= cutoff)
          {
            Server.LogMessage(string.Format("Cleaning up files created over {0} days ago.  File: {1}", file, cutoff));
            File.Delete(file);
          }
        }
      }
    } // Execute( folders )

    #endregion

  } // class OldFileCleanupOperation
} // namespace Viatronix.Operations
