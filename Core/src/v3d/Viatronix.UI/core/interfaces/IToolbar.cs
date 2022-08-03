// $Id: IToolbar.cs,v 1.1 2005/01/26 19:07:05 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;

namespace Viatronix.UI
{
  /// <summary>
  /// Defines methods that toolbar must implement.
  /// </summary>
  public interface IToolbar
	{
    /// <summary>
    /// Initializes the plugin toolbar with the specified xml
    /// </summary>
    /// <param name="xml">Additional xml initialization data.</param>
    void Initialize( string xml );
	
  } // interface IToolbar
} // namespace Viatronix.Viewer.Plugins

#region revision history

// $Log: IToolbar.cs,v $
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.1  2005/01/12 17:10:08  geconomos
// moved from viatronix.v3d.viewer.plugins
//
// Revision 1.1  2004/09/15 17:49:38  geconomos
// moved files
//
// Revision 1.1  2004/05/13 13:28:42  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/IToolbar.cs,v 1.1 2005/01/26 19:07:05 geconomos Exp $
// $Id: IToolbar.cs,v 1.1 2005/01/26 19:07:05 geconomos Exp $

#endregion