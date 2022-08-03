// $Id: IPersistable.cs,v 1.2 2007/03/12 17:46:31 mkontak Exp $
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

namespace Viatronix.UI
{

  /// <summary>
  /// Defines the interface for persisting the viewes
  /// </summary>
  public interface IPersistable
  {

    /// <summary>
    /// Allows the view to load any persisted state.
    /// </summary>
    /// <param name="viewState">The view's persistable state.</param>
    void LoadViewState( Dictionary< string, object > viewState );

    /// <summary>
    /// Allows the view to persist its state.
    /// </summary>
    /// <param name="viewState">The view's persisted state.</param>
    void SaveViewState( Dictionary< string, object > viewState );

  } // interface IPersistable
} // namespace Viatronix::UI



#region revision history

// $Log: IPersistable.cs,v $
// Revision 1.2  2007/03/12 17:46:31  mkontak
// coding standards
//
// Revision 1.1  2006/10/30 16:05:48  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/IPersistable.cs,v 1.2 2007/03/12 17:46:31 mkontak Exp $
// $Id: IPersistable.cs,v 1.2 2007/03/12 17:46:31 mkontak Exp $

#endregion