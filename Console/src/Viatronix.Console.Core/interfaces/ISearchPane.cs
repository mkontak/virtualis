// $Id: ISearchPane.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using Viatronix.v3D.Core;
using Viatronix.Enterprise;


namespace Viatronix.Console
{
	/// <summary>
	/// Defines methods that are necessary for a pane to perform a launch.
	/// </summary>
	public interface ISearchPane
	{
		
    /// <summary>
    /// Creates a where close for a dataprovider.
    /// </summary>
    /// <returns>A XmlBuilder object.</returns>
    XmlBuilder CreateQuery();

    /// <summary>
    /// Clears the child controls.
    /// </summary>
    void ClearControls();

	} // interface ISearchPane
} // namespace Viatronix.Console

#region revision history

// $Log: ISearchPane.cs,v $
// Revision 1.2  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.1  2006/10/30 15:35:22  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.1  2006/02/23 21:03:58  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/interfaces/ISearchPane.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
// $Id: ISearchPane.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $

#endregion