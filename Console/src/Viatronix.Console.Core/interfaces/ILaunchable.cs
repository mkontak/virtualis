// $Id: ILaunchable.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
//
// Copyright � 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using Viatronix.v3D.Core;


namespace Viatronix.Console
{
	/// <summary>
	/// Defines methods that are necessary to launch a viewer.
	/// </summary>
	public interface ILaunchable
	{
		
    /// <summary>
    /// Gets or sets a LaunchManager object.
    /// </summary>
    LaunchManager Launcher
    {
      get;
      set;
    } // Launcher

	} // interface ILaunchable
} // namespace Viatronix.Console


#region revision history

// $Log: ILaunchable.cs,v $
// Revision 1.2  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.1  2006/10/30 15:35:22  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.1  2005/10/18 17:47:47  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/interfaces/ILaunchable.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
// $Id: ILaunchable.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $

#endregion