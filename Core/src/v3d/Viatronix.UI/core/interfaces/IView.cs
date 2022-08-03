// $Id: IView.cs,v 1.2 2005/10/05 12:30:33 mkontak Exp $
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
  /// Defines methods that plugin tabs must implement.
  /// </summary>
	public interface IView
	{    
    /// <summary>
    /// Initializes the plugin tab with the specified xml
    /// </summary>
    /// <param name="xml">Additional xml initialization data.</param>
    void Initialize( string xml );

    
  } // interface IView

} // namespace Viatronix::UI


#region revision history

// $Log: IView.cs,v $
// Revision 1.2  2005/10/05 12:30:33  mkontak
// Minor comment changes
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/IView.cs,v 1.2 2005/10/05 12:30:33 mkontak Exp $
// $Id: IView.cs,v 1.2 2005/10/05 12:30:33 mkontak Exp $

#endregion