// $Id: IWindow.cs,v 1.8 2007/03/12 17:46:31 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson( mailto:davidson@viatronix.com )

using System;
using System.Drawing;
using System.Windows.Forms;

namespace Viatronix.v3D.Core
{

  #region delegates

  /// <summary>
  /// Window show delgate
  /// </summary>
  /// <param name="form">Form</param>
  /// <param name="bounds">bounds</param>
  /// <param name="state">windows state</param>
  public delegate void WindowShowDelegate( Form form, Rectangle bounds, FormWindowState state );

  #endregion

  /// <summary>
  /// Interface for controling the GUI of a remoting object.
  /// </summary>
  public interface IWindow
  {

    /// <summary>
    /// Shows the remoting object.
    /// </summary>
    void Show( Rectangle bounds, FormWindowState state );

    /// <summary>
    /// Closes the remoting object.
    /// </summary>
    void CloseWindow();

    /// <summary>
    /// Gets the process if
    /// </summary>
    int ProcessId { get; }

  } // interface IWindow
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: IWindow.cs,v $
// Revision 1.8  2007/03/12 17:46:31  mkontak
// coding standards
//
// Revision 1.7  2007/03/01 20:58:19  gdavidson
// Changed ownership
//
// Revision 1.6  2006/03/09 21:25:25  gdavidson
// Removed SetWindowsBounds and GetWindowsBounds and added ProcessId property
//
// Revision 1.5  2005/11/16 14:09:43  gdavidson
// Added WindowState property
//
// Revision 1.4  2005/11/09 16:12:25  gdavidson
// Modified the interface methods.
//
// Revision 1.3  2005/10/18 17:31:43  gdavidson
// Added a WindowState property.
//
// Revision 1.2  2005/06/02 13:35:55  gdavidson
// Commented code.
//
// Revision 1.1  2005/05/05 17:33:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/remoting/IWindow.cs,v 1.8 2007/03/12 17:46:31 mkontak Exp $
// $Id: IWindow.cs,v 1.8 2007/03/12 17:46:31 mkontak Exp $

#endregion