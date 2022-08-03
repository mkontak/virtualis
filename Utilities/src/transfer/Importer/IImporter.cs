// $Id: IImporter.cs,v 1.1.2.2 2009/06/04 19:02:17 kchalupa Exp $
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

using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// Defines an importer's basic necessities.
  /// </summary>
  public interface IImporter
  {

    /// <summary>
    /// Initiates a copy command cascade catastrophe to the individual parsed series
    /// objects contained herein.  Both the regular m_seriesList and m_sessionSeries are copied.
    /// </summary>
    /// <param name="ioArgs">Determines the output directory and whether copy and database records are
    /// performed and also whether the session is included.</param>
    void Copy(ImporterArgs ioArgs);

  } // IImporter
} // namespace Viatronix.Utilities.Importer

#region revision history

// $Log: IImporter.cs,v $
// Revision 1.1.2.2  2009/06/04 19:02:17  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/IImporter.cs,v 1.1.2.2 2009/06/04 19:02:17 kchalupa Exp $
// $Id: IImporter.cs,v 1.1.2.2 2009/06/04 19:02:17 kchalupa Exp $

#endregion
