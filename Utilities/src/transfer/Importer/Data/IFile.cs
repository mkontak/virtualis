// $Id: IFile.cs,v 1.1.2.3 2009/06/04 18:50:03 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.Collections.Generic;
using System.Text;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// Provides an interface for dealing with files that should be written out.
  /// </summary>
  public interface IFile
  {

    /// <summary>
    /// Performs a copy from the source to a destination.
    /// </summary>
    /// <param name="baseDirectory">
    /// The absolute path that the file should be written out to, this will prepend the file's
    /// relative path.
    /// </param>
    void Copy(string baseDirectory);

    /// <summary>
    /// Gets the source path for the file.
    /// </summary>
    /// <returns></returns>
    string GetSourceFileName();

    /// <summary>
    /// Get the FileName of the destination.
    /// </summary>
    string GetDestinationFileName();

  } // IFile
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: IFile.cs,v $
// Revision 1.1.2.3  2009/06/04 18:50:03  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/IFile.cs,v 1.1.2.3 2009/06/04 18:50:03 kchalupa Exp $
// $Id: IFile.cs,v 1.1.2.3 2009/06/04 18:50:03 kchalupa Exp $

#endregion
