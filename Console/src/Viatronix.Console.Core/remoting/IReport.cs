// $Id: IReport.cs,v 1.5 2007/03/01 20:58:19 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson( mailto:davidson@viatronix.com )

using System;
using Viatronix.Enterprise.Entities;

namespace Viatronix.v3D.Core
{
	/// <summary>
	/// Interface necessary to remotly control a viewer that displays a report.
	/// </summary>
	public interface IReport
	{
		
    /// <summary>
    /// Initializes the IReport remoting object.
    /// </summary>
    /// <param name="args">Initialization args</param>
    void Initialize( RemotingArgs args );

    /// <summary>
    /// Loads the supplies report.
    /// </summary>
    /// <param name="filename">The path to the report.</param>
    /// <param name="page">The page of the report.</param>
    void LoadReport( Series session, int page );

	} // interface IReport
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: IReport.cs,v $
// Revision 1.5  2007/03/01 20:58:19  gdavidson
// Changed ownership
//
// Revision 1.4  2006/01/18 16:05:38  gdavidson
// Modified Initialize to take RemotingArgs
//
// Revision 1.3  2005/12/14 20:03:26  gdavidson
// Corrected the spelling of the Initialize method.
//
// Revision 1.2  2005/10/06 15:32:02  gdavidson
// Modified the method parameters.
//
// Revision 1.1  2005/06/16 13:17:14  gdavidson
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/remoting/IReport.cs,v 1.5 2007/03/01 20:58:19 gdavidson Exp $
// $Id: IReport.cs,v 1.5 2007/03/01 20:58:19 gdavidson Exp $

#endregion