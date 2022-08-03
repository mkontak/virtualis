// $Id: IViewer.cs,v 1.11 2007/03/01 20:58:19 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson( mailto:davidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.ComponentModel;
using Viatronix.Enterprise.Entities;


namespace Viatronix.v3D.Core
{
	/// <summary>
	/// Interface necessary to remotly control a viewer.
	/// </summary>
	public interface IViewer
	{
		
    /// <summary>
    /// Initializes the IVewer remoting object.
    /// </summary>
    /// <param name="args">Initialization args</param>
    void Initialize( RemotingArgs args );

    /// <summary>
    /// Creates a new session.
    /// </summary>
    /// <param name="seriesCollection"></param>
    void New(List<Series>[] seriesCollection);

    /// <summary>
    /// Loads an existing session.
    /// </summary>
    /// <param name="session"></param>
    /// <param name="collection"></param>
    void Load(Series session, List<Series> collection);

    /// <summary>
    /// Saves the session.
    /// </summary>
    void Save();

    /// <summary>
    /// Saves and closes the viewer.
    /// </summary>
    /// <returns></returns>
    bool SaveAndClose();

    /// <summary>
    /// Determines if the viewer is closing.
    /// </summary>
    bool IsClosing { get; }

	} // interface IViewer
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: IViewer.cs,v $
// Revision 1.11  2007/03/01 20:58:19  gdavidson
// Changed ownership
//
// Revision 1.10  2006/01/18 16:05:38  gdavidson
// Modified Initialize to take RemotingArgs
//
// Revision 1.9  2005/12/14 20:03:26  gdavidson
// Corrected the spelling of the Initialize method.
//
// Revision 1.8  2005/11/23 03:58:32  geconomos
// viewer launching revisted
//
// Revision 1.7  2005/10/26 19:54:31  gdavidson
// Added a SaveAndClose method
//
// Revision 1.6  2005/10/11 14:42:21  gdavidson
// Added a Save method
//
// Revision 1.5  2005/10/06 15:32:02  gdavidson
// Modified the method parameters.
//
// Revision 1.4  2005/09/26 13:51:45  gdavidson
// Added New and Load methods.
//
// Revision 1.3  2005/06/02 13:35:55  gdavidson
// Commented code.
//
// Revision 1.2  2005/05/26 19:36:48  gdavidson
// Added AddDataSet method.
//
// Revision 1.1  2005/05/05 17:33:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/remoting/IViewer.cs,v 1.11 2007/03/01 20:58:19 gdavidson Exp $
// $Id: IViewer.cs,v 1.11 2007/03/01 20:58:19 gdavidson Exp $

#endregion