// $Id: WindowUtilities.cs,v 1.8 2007/03/12 17:47:08 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson( mailto:davidson@viatronix.com )

using System;
using System.Windows.Forms;

namespace Viatronix.Console
{
	
  /// <summary>
  /// Implements utilities used by our console/viewer windows
  /// </summary>
  public sealed class WindowUtilities
  {

    #region ShowWindow
    
    /// <summary>
    /// Contains information necessary to display a window.
    /// </summary>
    private struct ShowWindowArgs
    {

      /// <summary>
      /// The form being shown
      /// </summary>
      public System.Windows.Forms.Form m_form;

      /// <summary>
      /// The new bounds for the form.
      /// </summary>
      public System.Drawing.Rectangle m_bounds;

      /// <summary>
      /// The new window state for the form.
      /// </summary>
      public System.Windows.Forms.FormWindowState m_state;

    } // struct ShowWindowArgs
    

    /// <summary>
    /// Represents the method which executes the ShowWindow method.
    /// </summary>
    private delegate void ShowWindowHandler( ShowWindowArgs args );
    
    /// <summary>
    /// handler for the ShowWindowHandler delegate
    /// </summary>
    static private ShowWindowHandler m_showWindowHandler = new ShowWindowHandler( WindowUtilities.OnShowWindowHandler );

    /// <summary>
    /// Shows the provided window with the specifeid bounds and state.
    /// </summary>
    /// <param name="form">Form being displayed</param>
    /// <param name="bounds">bounds of the window</param>
    /// <param name="state">state of the window</param>
    static public void ShowWindow( System.Windows.Forms.Form form, System.Drawing.Rectangle bounds, System.Windows.Forms.FormWindowState state )
    {
      ShowWindowArgs args = new ShowWindowArgs();

      args.m_form = form;
      args.m_bounds = bounds;
      args.m_state = state;

      if( form.InvokeRequired )
        form.Invoke( WindowUtilities.m_showWindowHandler, new object[] { args } );
      else
        OnShowWindowHandler( args );
    } // ShowWindow( form, bounds, state )
    

    /// <summary>
    /// Handles the method invoke on the main ui thread.
    /// </summary>
    /// <param name="args">ShowWindowArgs struct</param>
    static private void OnShowWindowHandler( ShowWindowArgs args )
    {
      Win32.WINDOWPLACEMENT wp = new Win32.WINDOWPLACEMENT();
      wp.length = (uint) System.Runtime.InteropServices.Marshal.SizeOf( typeof( Win32.WINDOWPLACEMENT ) );
      
      // get the current WIDNOWPLACEMENT struct
      Win32.User32.GetWindowPlacement( args.m_form.Handle, ref wp );
      
      wp.flags = (uint)Win32.WPF.WPF_ASYNCWINDOWPLACEMENT;
      
      // update teh restore window position
      wp.rcNormalPosition = new Win32.RECT();
      wp.rcNormalPosition.left = args.m_bounds.Left;
      wp.rcNormalPosition.top = args.m_bounds.Top;
      wp.rcNormalPosition.right = args.m_bounds.Right;
      wp.rcNormalPosition.bottom  = args.m_bounds.Bottom;
      
      wp.showCmd = ( uint ) ( args.m_state == FormWindowState.Normal? Win32.ShowWindowStyles.SW_RESTORE : Win32.ShowWindowStyles.SW_SHOWMAXIMIZED );

      // update the WINDOWPLACEMENT
      Win32.User32.SetWindowPlacement( args.m_form.Handle, ref wp );    
      
      Win32.User32.SetForegroundWindow( args.m_form.Handle );
      Win32.User32.PostMessage( args.m_form.Handle, ( int ) Win32.Msgs.WM_NULL, 0, 0);

      args.m_form.Visible = true;
    } // OnShowWindowHandler( args )

    #endregion

    #region SwitchApplication
      

    /// <summary>
    /// Contains information necessary to switch between applications.
    /// </summary>
    private struct SwitchApplicationArgs
    {

      /// <summary>
      /// reference to the source window
      /// </summary>
      public System.Windows.Forms.Form m_sourceForm;

      /// <summary>
      /// bounds of the source window
      /// </summary>
      public System.Drawing.Rectangle m_sourceBounds;

      /// <summary>
      /// state of the source window
      /// </summary>
      public System.Windows.Forms.FormWindowState m_sourceState;

      /// <summary>
      /// reference to the destination window
      /// </summary>
      public Viatronix.v3D.Core.IWindow m_destinationWindow;

    } // struct SwitchApplicationArgs


    /// <summary>
    /// Represents the method which executes the SwitchApplication method.
    /// </summary>
    //private delegate void SwitchApplicationHandler( SwitchApplicationArgs args );


    /// <summary>
    /// handler for the SwitchApplicationHandler delegate
    /// </summary>
    //private static SwitchApplicationHandler m_switchApplicationHandler = new SwitchApplicationHandler( OnSwitchApplicationHandler );


    /// <summary>
    /// Switchs the from one application to another using the remoting interfaces.
    /// </summary>
    /// <param name="source">Source form</param>
    /// <param name="destination">Destination remoting interface</param>
    static public void SwitchApplication( System.Windows.Forms.Form source, Viatronix.v3D.Core.IWindow destination )
    {
      SwitchApplicationArgs args = new SwitchApplicationArgs();
      args.m_sourceForm = source;
      args.m_sourceState = source.WindowState;
      args.m_destinationWindow = destination;

      Win32.WINDOWPLACEMENT wp = new Win32.WINDOWPLACEMENT();
      wp.length = (uint) System.Runtime.InteropServices.Marshal.SizeOf( typeof( Win32.WINDOWPLACEMENT ) );

      uint ret = Win32.User32.GetWindowPlacement( source.Handle, ref wp );

      args.m_sourceBounds = new System.Drawing.Rectangle( 
        wp.rcNormalPosition.left, 
        wp.rcNormalPosition.top, 
        wp.rcNormalPosition.right -  wp.rcNormalPosition.left, 
        wp.rcNormalPosition.bottom -  wp.rcNormalPosition.top );

      // remain on the current thread 
      OnSwitchApplicationHandler( args );
      // (crashes console when using VC COM object)
      //m_switchApplicationHandler.EndInvoke( m_switchApplicationHandler.BeginInvoke( args, null, null ));

      args.m_sourceForm.Hide();
    } // SwitchApplication( source, destination )
   

    /// <summary>
    /// Handles the asynchronous calls that switches the application.
    /// </summary>
    /// <param name="args">SwitchApplicationArgs struct</param>
    static private void OnSwitchApplicationHandler( SwitchApplicationArgs args )
    {
      Win32.User32.AllowSetForegroundWindow( args.m_destinationWindow.ProcessId );
      args.m_destinationWindow.Show( args.m_sourceBounds, args.m_sourceState );
    } // OnSwitchApplicationHandler( args )

    #endregion

  } // class WindowUtilities
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: WindowUtilities.cs,v $
// Revision 1.8  2007/03/12 17:47:08  mkontak
// coding standards
//
// Revision 1.7  2007/03/08 21:30:33  gdavidson
// Issue #5469: modified the idle timer
//
// Revision 1.6  2007/03/08 15:11:57  gdavidson
// Issue #5463: Added check to avoid unnecessary invokes
//
// Revision 1.5  2007/03/05 16:13:39  gdavidson
// Issue #5338: Removed asynchronous call
//
// Revision 1.4  2007/03/01 21:01:26  gdavidson
// Changed ownership
//
// Revision 1.3  2006/10/30 16:04:59  gdavidson
// Modified to work with a regular winform
//
// Revision 1.2  2006/05/05 20:28:38  gdavidson
// Issue #4679: Changed the Idle timer
//
// Revision 1.1  2006/03/09 21:25:40  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/remoting/WindowUtilities.cs,v 1.8 2007/03/12 17:47:08 mkontak Exp $
// $Id: WindowUtilities.cs,v 1.8 2007/03/12 17:47:08 mkontak Exp $

#endregion