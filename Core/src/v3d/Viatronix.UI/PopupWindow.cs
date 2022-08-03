// $Id: PopupWindow.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright (c) 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Viatronix.UI
{
	/// <summary>
	/// Represents a modal popup window.
	/// </summary>
  public class PopupWindow : NativeWindow
	{
    #region constants

    /// <summary>
    /// Animation speed for fading popups.
    /// </summary>
    private const int m_animationInterval = 10;
 
    #endregion
    
    #region fields

    /// <summary>
    /// Window location in screen coordinates.
    /// </summary>
    private Point m_location = new Point();

    /// <summary>
    /// Window size in pixels.
    /// </summary>
    private Size m_size = new Size();

    /// <summary>
    /// Indicates if the modal loop should be terminated.
    /// </summary>
    private bool m_terminateLoop = false;

    /// <summary>
    /// Parent control that owns popup window.
    /// </summary>
    private Control m_parent = null;

    /// <summary>
    /// Parent popup window.
    /// </summary>
    private PopupWindow m_parentPopup = null;
    
    /// <summary>
    /// Child popup window.
    /// </summary>
    private PopupWindow m_childPopup = null;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the window's location in screen coordinates.
    /// </summary>
    public Point Location
    {
      get { return m_location; }
      set { m_location = value; SetBounds( m_location, m_size ); }
    } // Location

    
    /// <summary>
    /// Gets or sets the window's size in pixels.
    /// </summary>
    public Size Size
    {
      get { return m_size; }
      set { m_size = value; SetBounds( m_location, m_size ); }
    } // Size


    /// <summary>
    /// Gets the window's client area.
    /// </summary>
    public Rectangle ClientRectangle
    {
      get { return new Rectangle( 0, 0, m_size.Width, m_size.Height ); }
    } // ClientRectangle

    
    /// <summary>
    /// Gets the window's bounds in screen cooridnates.
    /// </summary>
    public Rectangle Bounds
    {
      get { return new Rectangle( m_location, m_size ); }
    } // WindowRectangle


    /// <summary>
    /// Gets or sets the parent control.
    /// </summary>
    public Control Parent
    {
      get { return m_parent; }
      set { m_parent = value; }
    } // Parent
    
    /// <summary>
    /// Gets or sets the parent popup window.
    /// </summary>
    public PopupWindow ParentPopup
    {
      get { return m_parentPopup; }
      set { m_parentPopup = value; }
    } // ParentPopup
    

    /// <summary>
    /// Gets or sets the child popup window.
    /// </summary>
    public PopupWindow ChildPopup
    {
      get { return m_childPopup; }
      set { m_childPopup = value; }
    } // ChildPopup
  
    
    #endregion

    #region methods

    /// <summary>
    /// Shows the PopupWindow at the specified location.
    /// </summary>
    /// <param name="parent">Parent control that PopupWindow is associated.<./param>
    /// <param name="position">Screen coordinates to display the PopupWindow.</param>
    /// <returns>0 on suceess, non-zero otherwise</returns>
    public virtual int Show( Control parent, Point position )
    {
      // default the orientation to down
      return Show( parent, position, PopupDirection.Down );
    } // Show( parent, position )
    
    
    /// <summary>
    /// Shows the PopupMenu at the specified location.
    /// </summary>
    /// <param name="parent">Parent control that PopupWindow is associated.</param>
    /// <param name="position">Screen coordinates to display the PopupWindow.</param>
    /// <param name="direction">Direction releative to position that PopupWindow is displayed.</param>
    /// <returns>0 on suceess, non-zero otherwise</returns>
    public virtual int Show( Control parent, Point position, PopupDirection direction )
    {  
      // associate a window for this menu
      Create( parent, position, this.Size, direction );
      
      // enter modal loop
      BeginModal();
      
      // close it down
      Dismiss();

      // return 
      return 0;
    } // Show( parent, position, direction )
 
    
    /// <summary>
    /// Sets the window's location and size.
    /// </summary>
    /// <param name="location">location of window in screen coordinates</param>
    /// <param name="size">size of window in pixels</param>
    public void SetBounds( Point location, Size size )
    {
      // call overload
      SetBounds( location.X, location.Y, size.Width, size.Height );
    } // SetBounds( x, y, width, height )

   
    /// <summary>
    /// Sets the window's location and size.
    /// </summary>
    /// <param name="x">x location of window in screen coordinates.</param>
    /// <param name="y">y location of window in screen coordinates.</param>
    /// <param name="width">width of window in pixles</param>
    /// <param name="height">height of window in pixles</param>
    public void SetBounds( int x, int y, int width, int height )
    {
      // store the size an  location
      m_location = new Point( x, y );
      m_size = new Size( width, height );

      Win32.User32.SetWindowPos( this.Handle, IntPtr.Zero, x, y, width, height, ( int )Win32.SetWindowPosFlags.SWP_NOZORDER );
    } // SetBounds( x, y, width, height )


    /// <summary>
    /// Creates the underlying window.
    /// </summary>
    public void Create( Control parent, Point location, Size size, PopupDirection direction )
    {
      // make sure everything fits on the screen
      m_location = CorrectLocationForScreen( location, size, direction );
      
      m_size = size;
      m_parent = parent;

      if( this.Handle == IntPtr.Zero )
      {
        // set the window creation parameters
        CreateParams cp = new CreateParams();
        cp.Caption = string.Empty;
        cp.X = m_location.X;
        cp.Y = m_location.Y;
        cp.Height = m_size.Height;
        cp.Width = m_size.Width;
        cp.Parent = IntPtr.Zero;
        cp.Style = unchecked( ( int ) Win32.WindowStyles.WS_POPUP );
        cp.ExStyle =  ( int )Win32.WindowExStyles.WS_EX_TOPMOST  + 
          ( int )Win32.WindowExStyles.WS_EX_TOOLWINDOW;

        // create the window
        this.CreateHandle( cp );
      }
      else
        SetBounds( m_location, m_size );

      // fade the window into view 
      Win32.User32.AnimateWindow( this.Handle, ( uint )m_animationInterval, 0x00080000 ); 
    } // CreateWindow( parent, location, size, direction )


    /// <summary>
    /// Forces the window to redraw.
    /// </summary>
    public void Refresh()
    {
      Win32.RECT clientRect = new Win32.RECT();

      clientRect.left = 0;
      clientRect.top = 0;
      clientRect.right = m_size.Width;
      clientRect.bottom = m_size.Height;

      Win32.User32.RedrawWindow( this.Handle, ref clientRect, IntPtr.Zero, (int)Win32.RedrawWindowFlags.RDW_ERASE | ( int )Win32.RedrawWindowFlags.RDW_INVALIDATE );
    } // Refresh()
    

    /// <summary>
    /// Starts the modal loop.
    /// </summary>
    public void BeginModal()
    {
      // restart the loop
      m_terminateLoop = false;
      
      // create an object for storing windows message information
      Win32.MSG msg = new Win32.MSG();

      // always use the arrow cursor
      Win32.User32.SetCursor( Win32.User32.LoadCursor(IntPtr.Zero, (uint)Win32.Cursors.IDC_ARROW));

      // must hide caret so user thinks focus has changed
      bool hideCaret = Win32.User32.HideCaret( IntPtr.Zero );

      // Process messages until exit condition recognised
      while( !m_terminateLoop )
      {
        // Suspend thread until a windows message has arrived
        if( Win32.User32.WaitMessage() )
        {
          // Take a peek at the message details without removing from queue
          while(! m_terminateLoop && Win32.User32.PeekMessage(ref msg, 0, 0, 0, Win32.PeekMessageFlags.PM_NOREMOVE))
          {

            bool eatMessage = false;
            
            bool leaveMsg = false;
            
            // Mouse was pressed in a window of this application
            if( ( msg.message == ( int )Win32.Msgs.WM_LBUTTONUP ) ||
                ( msg.message == ( int )Win32.Msgs.WM_MBUTTONUP ) ||
                ( msg.message == ( int )Win32.Msgs.WM_RBUTTONUP ) )
            {
              if( this.Bounds.Contains( Control.MousePosition ) )
              {
                OnMouseUp( GetMouseEventArgs( this.Handle ) );
                eatMessage = true;								
              }
              else
              {
                PopupWindow target = ParentPopupWantsMouseMessage( Control.MousePosition );
                if( target != null )
                {
                  target.OnMouseUp( GetMouseEventArgs( m_parentPopup.Handle ) );

                  eatMessage = true;								
                } // if( target != null )
              } 
            } // if( mouse button up )
                                          
            // Mouse was pressed in a window of this application
            if( ( msg.message == (int)Win32.Msgs.WM_LBUTTONDOWN ) ||
                ( msg.message == (int)Win32.Msgs.WM_MBUTTONDOWN ) ||
                ( msg.message == (int)Win32.Msgs.WM_RBUTTONDOWN ) )
            {
              if( this.Bounds.Contains( Control.MousePosition ) )
              {
                OnMouseDown( GetMouseEventArgs( this.Handle ) );
                // Eat the message to prevent the intended destination getting it
                eatMessage = true;								
              }
              else
              {                  
                // does the parent want to handle the message?
                PopupWindow target = ParentPopupWantsMouseMessage( Control.MousePosition );
                if( target != null )
                {
                  target.OnMouseDown( GetMouseEventArgs( m_parentPopup.Handle ) );

                  eatMessage = true;								
                }
                else
                {
                  // No, then we need to exit the popup window tracking
                  m_terminateLoop = true;

                  // DO NOT process the message, leave it on the queue
                  // and let the real destination window handle it.
                  leaveMsg = true;

                  // Is a parent control specified?
                  if( m_parentPopup != null )
                  {
                    // Is the mouse event destination the parent control?
                    if( msg.hwnd == m_parentPopup.Handle )
                    {
                      //leaveMsg = false;
                    } 
                  } // m_parentPopup != null
                } // if( ParentPopupWantsMouseMessage( Control.MousePosition ) == null )
              } // !windows.Contin()
            } // mouse down

            // Mouse move occurred
            if (msg.message == (int)Win32.Msgs.WM_MOUSEMOVE ) 
            {
              if( this.Bounds.Contains( Control.MousePosition ) )
              {
                OnMouseMove( GetMouseEventArgs( this.Handle ) );
                
                // Eat the message to prevent the intended destination getting it
                eatMessage = true;								
              }
              else
              {
                PopupWindow target = ParentPopupWantsMouseMessage( Control.MousePosition );
                if( target != null )
                {
                  target.OnMouseMove( GetMouseEventArgs( m_parentPopup.Handle ) );

                  eatMessage = true;								
                }
              }
            } // if (msg.message == (int)Win32.Msgs.WM_MOUSEMOV E) 


            // We consume all keyboard input
            if( ( msg.message == ( int )Win32.Msgs.WM_KEYDOWN     ) ||
                ( msg.message == ( int )Win32.Msgs.WM_KEYUP       ) ||
                ( msg.message == ( int )Win32.Msgs.WM_SYSKEYDOWN  ) ||
                ( msg.message == ( int )Win32.Msgs.WM_SYSKEYUP    ) )					
            {
              // Eat the message to prevent the intended destination getting it
              eatMessage = true;								
            }						

            // Should the message be eaten to prevent intended destination getting it?
            if( eatMessage )
            {
              Win32.MSG eat = new Win32.MSG();
              Win32.User32.GetMessage(ref eat, 0, 0, 0);
            }
            else
            {	
              // Should the message we pulled from the queue?
              if (!leaveMsg)
              {
                if (Win32.User32.GetMessage(ref msg, 0, 0, 0))
                {
                  Win32.User32.TranslateMessage(ref msg);
                  Win32.User32.DispatchMessage(ref msg);
                }
              }
              else
                leaveMsg = false;
            }
          }
        } //Win32.WaitMessage()
      } // !m_exitloop

      // If caret was hidden then show it again now
      if (hideCaret)
        Win32.User32.ShowCaret(IntPtr.Zero);

    } // DoModal()


    public void EndModal()
    {
      m_terminateLoop = true;
    } // EndModal()


    /// <summary>
    /// Closes the PopupWindow.
    /// </summary>
    public void Dismiss()
    {
      // close any open child popups
      if( m_childPopup != null )
      {
        m_childPopup.Dismiss();
        m_childPopup = null;
      }

      // indicate that we want to break out of the modal loop
      EndModal();

      // hide the window first before destroying( reduces artifacts )
      Win32.User32.ShowWindow( this.Handle, (short)Win32.ShowWindowStyles.SW_HIDE );
      
      // free the resources for this popup
      DestroyHandle();
    } // Dismiss()

    /// <summary>
    /// Dismisses all popups in the chain.
    /// </summary>
    public void DismissAll()
    {
      PopupWindow popupToDismiss = this;
      PopupWindow nextPopup = popupToDismiss.ParentPopup;
      while( nextPopup != null )
      {
        popupToDismiss = nextPopup;
        nextPopup = nextPopup.ParentPopup;
      } // while( nextPopup != null )

      popupToDismiss.Dismiss();
    } // DismissAll()


    
    #endregion

    #region initialization
		
    /// <summary>
    /// Initializes a PopupWindow object with default values.
    /// </summary>
    public PopupWindow()
		{
		} // PopupWindow()


    #endregion
    
    #region overrideables

    /// <summary>
    /// Called when the windows needs painting.
    /// </summary>
    /// <param name="args">PaintEventArgs containing data related to this method call.</param>
    protected virtual void OnPaint( PaintEventArgs args )
    {
    } // OnPaint( args )

    
    /// <summary>
    /// Called when mouse button is down.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to method call.</param>
    protected virtual void OnMouseDown( MouseEventArgs args )
    {
    } // OnMouseDown( args )
    
    
    /// <summary>
    /// Called after a mouse button is released.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to method call.</param>
    protected virtual void OnMouseUp( MouseEventArgs args )
    {
    } // OnMouseUp( args )
    
    
    /// <summary>
    /// Called whenever the mouse position changed.
    /// </summary>
    /// <param name="args">MouseEventArgs containing data related to method call.</param>
    protected virtual void OnMouseMove( MouseEventArgs args )
    {
    } // OnMouseMove( args )
    
    
    #endregion

    #region wndproc
    
    /// <summary>
    /// Wnd Proc method.
    /// </summary>
    /// <param name="msg">Message to process.</param>
    protected override void WndProc( ref Message msg )
    {
      // WM_PAINT
      if( msg.Msg == ( int ) Win32.Msgs.WM_PAINT )
      {
        // create a bitmap to render into
        Bitmap bitmap = new Bitmap( m_size.Width, m_size.Height, PixelFormat.Format32bppArgb );
        
        // create a graphics object based on the bitmap
        using( Graphics gfx = Graphics.FromImage( bitmap ) )
        {
          // Create paint event args class
          PaintEventArgs args = new PaintEventArgs( gfx, new Rectangle( 0, 0, m_size.Width, m_size.Height ) );

          // do the painting
          OnPaint( args );
        } // if( msg.Msg == ( int ) Win32.Msgs.WM_PAINT )
      
        // create a good ole' fashioned paint sturcture
        Win32.PAINTSTRUCT ps = new Win32.PAINTSTRUCT();

        // start the legendary painting process
        IntPtr hdc = Win32.User32.BeginPaint( msg.HWnd, ref ps );

        // create a tempoary graphics object to render the bitmap
        using( Graphics gfx = Graphics.FromHdc( hdc ) )
          gfx.DrawImageUnscaled( bitmap, 0, 0 );

        // done painting
        Win32.User32.EndPaint( msg.HWnd, ref ps );  
      } // if( msg.Msg == ( int ) Win32.Msgs.WM_PAINT )
      
      // WM_PRINT
      else if( msg.Msg == ( int ) Win32.Msgs.WM_PRINT )
      {
        Bitmap bitmap = new Bitmap( m_size.Width, m_size.Height, PixelFormat.Format32bppArgb );
        using( Graphics gfx = Graphics.FromImage( bitmap ) )
          OnPaint( new PaintEventArgs( gfx, this.ClientRectangle ) );

        using( Graphics gfx = Graphics.FromHdc( msg.WParam ) )
          gfx.DrawImageUnscaled( bitmap, 0, 0 );
      } // else if( msg.Msg == ( int ) Win32.Msgs.WM_PRINT )
      
      
      // WM_MOUSEACTIVATE
      else if( msg.Msg == ( int ) Win32.Msgs.WM_MOUSEACTIVATE )
      {
        msg.Result = ( IntPtr ) Win32.MouseActivateFlags.MA_NOACTIVATE;
      } // else if( msg.Msg == ( int ) Win32.Msgs.WM_MOUSEACTIVATE )

      // WM_SECURSOR
      else if( msg.Msg == ( int ) Win32.Msgs.WM_SETCURSOR )
      {
        Win32.User32.SetCursor( Win32.User32.LoadCursor( IntPtr.Zero, ( uint ) Win32.Cursors.IDC_ARROW ) );
      } // else if( msg.Msg == ( int ) Win32.Msgs.WM_SETCURSOR )

        // default processing
      else
        base.WndProc( ref msg ); 
    }  // WndProc( msg )


    #endregion

    #region implementation

    /// <summary>
    /// Checks if parent window wants to process the current mouse message.
    /// </summary>
    /// <param name="position">Current mouse position in screen coordinates.</param>
    /// <returns>PopupWindow object wanting to handle message, null if none</returns>
    internal PopupWindow ParentPopupWantsMouseMessage( Point position )
    {
      // is the mouse over our parent popup?
      if( m_parentPopup != null )
        return m_parentPopup.WantsMouseMessage( position );
      
      // nonbody wants it...
      return null;
    } // ParentPopupWantsMouseMessage()

    
    /// <summary>
    /// Checks if this window wants to process the current mouse message.
    /// </summary>
    /// <param name="position">Current mouse position in screen coordinates.</param>
    /// <returns>PopupWindow object wanting to handle message, null if none.</returns>
    protected PopupWindow WantsMouseMessage( Point position )
    {
      // is the mouse over this popup?
      if( this.Bounds.Contains( position ) )
        return this;

      // maybe it is for our parent popup
      if( m_parentPopup != null )
        return m_parentPopup.WantsMouseMessage( position );

      // we don't want it
      return null;
    } // WantsMouseMessage()
 

    /// <summary>
    /// Returns a MousEventArgs based on the current mouse settings.
    /// </summary>
    /// <returns></returns>
    private MouseEventArgs GetMouseEventArgs( IntPtr hWnd )
    {
      // get mouse location in client coordinates
      Point pos = GetClientPosition( hWnd );
      
      // return the MouseEventArgs object
      return new MouseEventArgs( Control.MouseButtons, 1, pos.X, pos.Y, 0 );
    } // GetMouseEventArgs()


    /// <summary>
    /// Updates the position so that the menu can be entirely displayed on the screen.
    /// </summary>
    /// <param name="position">Position to check.</param>
    /// <param name="size">Size of menu in pixels.</param>
    /// <param name="direction">Menu popup direction.</param>
    /// <returns></returns>
    private Point CorrectLocationForScreen( Point position, Size size, PopupDirection direction )
    {
      Rectangle screen = Screen.GetWorkingArea( position );
      
      // calculate the downward position first
      if( direction == PopupDirection.Down )
      {
        // ensure the end of the menu is not off the bottom of the screen
        if( ( position.Y + size.Height) > screen.Bottom )
          position.Y = screen.Bottom - size.Height - 1;
      } // if( direction == PopupDirection.Down )
      else
      {
        // ensure the end of the menu is not off the top of the screen
        if( ( position.Y - size.Height) < screen.Top )
        {
          // is there space below the required position?
          if( ( position.Y + size.Height ) > screen.Bottom )
            position.Y = screen.Bottom - size.Height - 1;
        }
        else
          position.Y -= size.Height;      
      } // else

      // ensure that the menu is not off the right of the screen
      if( ( position.X + size.Width) > screen.Right )
        position.X = screen.Right - size.Width - 1;

      // return the corrected position
      return position;
    } // CorrectLocationForScreen( position, size, direction )



    /// <summary>
    /// Gets the current mouse position in client coordinates.
    /// </summary>
    /// <returns>Mouse position in client coordinates.</returns>
    private Point GetClientPosition( IntPtr hWnd )
    {
      Win32.POINT screenPos;
      screenPos.x = Control.MousePosition.X;
      screenPos.y = Control.MousePosition.Y;

      // Convert the mouse position to screen coordinates
      Win32.User32.ScreenToClient( hWnd, ref screenPos );

      return new Point( screenPos.x, screenPos.y );
    } // GetClientPosition
    

    #endregion
  
	} // class PopupWindow
} // namespace Viatronix.UI

#region revision history

// $Log: PopupWindow.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/11/21 19:44:50  frank
// Changed the speed of fade-in so that the main menu is more responsive
//
// Revision 1.5  2003/08/20 14:23:25  michael
// added "mailto:"
//
// Revision 1.4  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.3  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.2  2003/04/24 14:34:29  frank
// code review
//
// Revision 1.1  2003/04/14 13:08:54  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/PopupWindow.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: PopupWindow.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion