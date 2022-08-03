// $Id: ISearchView.cs,v 1.2 2007/03/06 19:15:00 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Text;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{

  /// <summary>
  /// Defines methods that allow a view to display search results.
  /// </summary>
  public interface ISearchView : Viatronix.UI.IView
  {

    /// <summary>
    /// Adds a single study result.
    /// </summary>
    /// <param name="study">A found study.</param>
    void AddSingleSearchResult( Study study );


    /// <summary>
    /// Adds all found studies.
    /// </summary>
    /// <param name="studies">Collection of studies.</param>
    void AddAllSearchResults( List<Study> studies );

  } // interface ISearchView
} // namespace Viatronix.Console


#region revision history

// $Log: ISearchView.cs,v $
// Revision 1.2  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.1  2006/10/30 15:35:32  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/interfaces/ISearchView.cs,v 1.2 2007/03/06 19:15:00 gdavidson Exp $
// $Id: ISearchView.cs,v 1.2 2007/03/06 19:15:00 gdavidson Exp $

#endregion
