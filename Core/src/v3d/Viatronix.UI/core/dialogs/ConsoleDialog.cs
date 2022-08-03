// $Id: Dialog.cs,v 1.5 2007/03/01 21:04:07 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using Viatronix.UI;

namespace Viatronix.UI
{
	/// <summary>
	/// Provides derived classes with the ability to be hidden when the console auto logs off.
	/// </summary>
	public class ConsoleDialog : Viatronix.UI.Dialog
  {

    #region fields

    /// <summary>
    /// Event handler for console visibility
    /// </summary>
    private EventHandler m_consoleVisibilityChangedHandler =  null;

    #endregion

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
    public ConsoleDialog()
    {
      m_consoleVisibilityChangedHandler =  new EventHandler( this.OnConsoleVisibilityChangedHandler );
    } // Dialog()

    #endregion

    #region event handlers
    
    /// <summary>
    /// Assigns a visibility event handler to the console.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnLoad( EventArgs args )
    {
      base.OnLoad( args );

      if( this.Owner != null )
        this.Owner.VisibleChanged += m_consoleVisibilityChangedHandler;// new EventHandler( this.OnConsoleVisibilityChangedHandler );
    } // OnLoad( args )


    /// <summary>
    /// Removes the visibility event handler.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnClosed( EventArgs args )
    {
      base.OnClosed( args );

      if( this.Owner != null )
        this.Owner.VisibleChanged -= m_consoleVisibilityChangedHandler;// new EventHandler( this.OnConsoleVisibilityChangedHandler );
    } // OnClosed


    /// <summary>
    /// Hides the dialog when the main form is not visible.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnConsoleVisibilityChangedHandler( object sender, EventArgs args )
    {
      if( this.Owner != null )
        this.Visible = this.Owner.Visible;
    } // OnConsoleVisibilityChangedHandler( sender, args )

    #endregion

	} // class Dialog
} // namespace Viatronix.Console

#region revision history

// $Log: Dialog.cs,v $
// Revision 1.5  2007/03/01 21:04:07  gdavidson
// Changed ownership
//
// Revision 1.4  2006/05/08 15:26:14  gdavidson
// Issue #4768: Check the dialogs's owner for null before accessing its properties
//
// Revision 1.3  2006/04/27 21:05:04  romy
// event handler fix
//
// Revision 1.2  2006/04/27 21:04:37  romy
// event handler fix
//
// Revision 1.1  2006/02/16 18:12:02  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/Dialog.cs,v 1.5 2007/03/01 21:04:07 gdavidson Exp $
// $Id: Dialog.cs,v 1.5 2007/03/01 21:04:07 gdavidson Exp $

#endregion
