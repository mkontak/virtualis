// $Id: IConsole.cs,v 1.13 2007/03/12 17:46:31 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson( mailto:davidson@viatronix.com )

using System;
using System.Windows.Forms;
using Viatronix.Enterprise.Entities;


namespace Viatronix.v3D.Core
{
  /// <summary>
  /// Defines the console interface
  /// </summary>
  public interface IConsole
  {

    /// <summary>
    /// Returns if the console supports save operations
    /// </summary>
    bool CanSave { get; }

    /// <summary>
    /// Locks the application.
    /// </summary>
    void LockApplication();

    /// <summary>
    /// Creates a series of the type specified adding the series collection as references.
    /// </summary>
    /// <param name="args">Series information</param>
    /// <returns>Newly created series</returns>
    void Save( Series session );

 
    /// <summary>
    /// Handles the viewer closing.
    /// </summary>
    void ViewerClosing();

    /// <summary>
    /// Allows the viewer to login to the console.
    /// </summary>
    /// <param name="username">user name</param>
    /// <param name="password">password</param>
    /// <returns>true if the user can login; otherwise false</returns>
    //bool Login( string username, string password );

  } // interface IConsole
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: IConsole.cs,v $
// Revision 1.13  2007/03/12 17:46:31  mkontak
// coding standards
//
// Revision 1.12  2007/03/01 20:58:19  gdavidson
// Changed ownership
//
// Revision 1.11  2006/12/21 16:45:11  gdavidson
// Added LockApplication
//
// Revision 1.10  2006/10/30 16:02:55  gdavidson
// Added CanSave property
//
// Revision 1.9  2006/04/20 19:44:54  gdavidson
// Changed the parameters for the CreateSeries method
//
// Revision 1.8  2006/01/18 16:06:53  gdavidson
// Added a Login method
//
// Revision 1.7  2005/11/23 03:58:32  geconomos
// viewer launching revisted
//
// Revision 1.6  2005/11/09 16:12:25  gdavidson
// Modified the interface methods.
//
// Revision 1.5  2005/10/18 17:34:03  gdavidson
// Added a WindowState property.
//
// Revision 1.4  2005/10/06 15:30:59  gdavidson
// Added Hide and Show methods.
//
// Revision 1.3  2005/10/05 12:29:16  mkontak
// Added CreateSeries()
//
// Revision 1.2  2005/09/26 13:51:57  gdavidson
// Added Save method.
//
// Revision 1.1  2005/05/05 17:33:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/remoting/IConsole.cs,v 1.13 2007/03/12 17:46:31 mkontak Exp $
// $Id: IConsole.cs,v 1.13 2007/03/12 17:46:31 mkontak Exp $

#endregion