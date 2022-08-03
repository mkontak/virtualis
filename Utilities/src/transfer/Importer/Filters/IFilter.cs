// $Id: IFilter.cs,v 1.1.2.2 2009/06/04 18:48:18 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;

using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Filters
{
  /// <summary>
  /// An interface to define file filters for the various types of directory structures that exist.
  /// </summary>
  public interface IFilter
  {

    /// <summary>
    /// Determines whether or not the folder can be processed.
    /// </summary>
    /// <param name="args">Defines where the ioArgs is located.</param>
    /// <returns>Whether or not the particular filter can process the folder's contents.</returns>
    bool CanParse(ImporterArgs ioArgs);

    /// <summary>
    /// Processes the folder to determine files fit into what series.
    /// </summary>
    /// <param name="ioArgs">Defines where the ioArgs is located.</param>
    /// <param name="importer">The object to hold what files fit into what series.</param>
    /// <returns>A Paitient object that was defined by the set of files found.</returns>
    IImporter Parse(ImporterArgs ioArgs);

  } // IFilter
} // namespace Viatronix.Utilities.Importer.Filters

#region revision history

// $Log: IFilter.cs,v $
// Revision 1.1.2.2  2009/06/04 18:48:18  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Filters/IFilter.cs,v 1.1.2.2 2009/06/04 18:48:18 kchalupa Exp $
// $Id: IFilter.cs,v 1.1.2.2 2009/06/04 18:48:18 kchalupa Exp $

#endregion
