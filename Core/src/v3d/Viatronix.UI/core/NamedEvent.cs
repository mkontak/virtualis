// $Id: NamedEvent.cs,v 1.3 2007/03/12 17:46:31 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Threading;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace Viatronix.Core
{
	/// <summary>
	/// Wraps the Win32 named events
	/// </summary>
	public class NamedEvent : WaitHandle
	{

    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name">identifier of the event</param>
    /// <param name="create">true creates the event; otherwise open the event</param>
		public NamedEvent( string name, bool create )
		{ 
      if( create )
        this.SafeWaitHandle = CreateEvent( IntPtr.Zero, true, false, name );
      else
        this.SafeWaitHandle = OpenEvent( 0x1F0003, 0, name );
		} // NamedEvent( name, create )

    #endregion

    #region methods

    /// <summary>
    /// Signals the event
    /// </summary>
    public void Signal()
    {
      SetEvent( this.SafeWaitHandle );
    } // Signal()

    #endregion

    #region Kernel32

    /// <summary>
    /// Creates a neamed event
    /// </summary>
    /// <param name="lpEventAttributes">event attributes</param>
    /// <param name="bManualReset">is it manual reset</param>
    /// <param name="bInitialState">sets the initial state</param>
    /// <param name="lpName">name of the event</param>
    /// <returns>SafeWaitHandle to the event</returns>
    [ DllImport( "Kernel32.dll" ) ]
    private static extern SafeWaitHandle CreateEvent( IntPtr lpEventAttributes, bool bManualReset, bool bInitialState, string lpName );

    /// <summary>
    /// Opens the named event
    /// </summary>
    /// <param name="dwDesiredAccess">access</param>
    /// <param name="bInheritHandle"></param>
    /// <param name="lpName">name of event</param>
    /// <returns>SafeWaitHandle to the event</returns>
    [ DllImport( "Kernel32.dll" ) ]
    private static extern SafeWaitHandle OpenEvent( int dwDesiredAccess, int bInheritHandle, string lpName );

    /// <summary>
    /// Signals the event
    /// </summary>
    /// <param name="hEvent">reference to event</param>
    /// <returns></returns>
    [ DllImport( "Kernel32.dll" ) ]
    private static extern bool SetEvent( SafeWaitHandle hEvent );

    #endregion

  } // class NamedEvent
} // namespace Viatronix.Core


#region revision history

// $Log: NamedEvent.cs,v $
// Revision 1.3  2007/03/12 17:46:31  mkontak
// coding standards
//
// Revision 1.2  2007/03/01 19:12:52  gdavidson
// Commented code
//
// Revision 1.1  2006/10/30 16:06:00  gdavidson
// Initial revision
//
// Revision 1.5  2006/08/23 18:22:08  gdavidson
// Updated to SafeWaitHandle
//
// Revision 1.4  2006/08/23 18:05:19  gdavidson
// Updated to SafeWaitHandle
//
// Revision 1.3  2006/03/09 21:11:11  gdavidson
// Removed commented unused struct
//
// Revision 1.2  2005/11/23 03:58:32  geconomos
// viewer launching revisted
//
// Revision 1.1  2005/05/05 17:33:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/NamedEvent.cs,v 1.3 2007/03/12 17:46:31 mkontak Exp $
// $Id: NamedEvent.cs,v 1.3 2007/03/12 17:46:31 mkontak Exp $

#endregion