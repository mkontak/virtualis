using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Viatronix.UI
{
  #region PopupClosedEventArgs
  
  public class PopupClosedEventArgs : EventArgs
  {    
    #region fields
    
    private Form m_popup = null;

    #endregion

    #region properties

    public Form Popup
    {
      get { return m_popup; }
    }


    #endregion

    #region initialization

    public PopupClosedEventArgs( Form popup )
    {
      m_popup = popup;
    }

    #endregion
  
  } // class PopupClosedEventArgs

 
  #endregion

  #region PopupCancelEventArgs

  public class PopupCancelEventArgs : EventArgs
  {
    #region fields
    
    private bool m_cancel = false;

    private Point m_location;

    private Form m_popup = null;

    #endregion

    #region properties

    public Form Popup
    {
      get{ return m_popup; }
    }

    
    public Point CursorLocation
    {
      get { return m_location; }
    }

    
    public bool Cancel
    {
      get{ return m_cancel; }
      set{ m_cancel = value; }
    }
  

    #endregion

    #region initialization
    
    public PopupCancelEventArgs( Form popup, Point location )
    {
      m_popup = popup;
      m_location = location;
      m_cancel = false;
    }


    #endregion
  
  } // class PopupCancelEventArgs
  
  #endregion

  #region PopupClosedEventHandler
  
  public delegate void PopupClosedEventHandler( object sender, PopupClosedEventArgs args );

  #endregion

  #region PopupCancelEventHandler

  public delegate void PopupCancelEventHandler( object sender, PopupCancelEventArgs args );
  
  #endregion

  #region PopupWindowHelper

  public class PopupWindowHelper : NativeWindow
  {
    #region interop

    [DllImport("user32")]
    private extern static void keybd_event(byte bVk, byte bScan, int dwFlags, int dwExtraInfo);

    private const int KEYEVENTF_KEYUP       = 0x0002;
    
    #endregion

    #region events

    public event PopupClosedEventHandler PopupClosed;
    
    public event PopupCancelEventHandler PopupCancel;
    
    #endregion

    #region fields

    private EventHandler m_popupClosedHandler = null;

    private PopupWindowHelperMessageFilter m_filter = null;

    private Form m_popup = null;

    private Form m_owner = null;

    private bool m_showing = false;

    private bool m_skipClose = false;
    
    #endregion

    #region initialization
    
    public PopupWindowHelper()
    {
      m_filter = new PopupWindowHelperMessageFilter( this );
      m_filter.PopupCancel += new PopupCancelEventHandler( OnPopupCancelHandler );
    }

    #endregion

    #region methods
    
    public void ShowPopup( Form owner, Form popup, Point location )
    {

      m_owner = owner;
      m_popup = popup;

      // start checking for the popup being cancelled
      Application.AddMessageFilter( m_filter );

      // set the location of the popup form:
      m_popup.StartPosition = FormStartPosition.Manual;
      m_popup.Location = location; 
      
      // make it owned by the window that's displaying it:
      m_owner.AddOwnedForm( m_popup );			
      
      // respond to the Closed event in case the popup is closed by its own internal means
      m_popupClosedHandler = new EventHandler( OnPopupClosedHandler );
      popup.Closed += m_popupClosedHandler;

      // show the popup
      this.m_showing = true;
      m_popup.Show();
      m_popup.Activate();
			
      // A little bit of fun  We've shown the popup, but because we've kept the main window's
      // title bar in focus the tab sequence isn't quite right.  This can be fixed by sending a tab,
      // but that on its own would shift focus to the second control in the form.  So send a tab,
      // followed by a reverse-tab.

      // send a Tab command
      keybd_event( (byte) Keys.Tab, 0, 0, 0 );
      keybd_event( (byte) Keys.Tab, 0, KEYEVENTF_KEYUP, 0 );

      // Send a reverse Tab command:
      keybd_event( (byte) Keys.ShiftKey, 0, 0, 0 );
      keybd_event( (byte) Keys.Tab, 0, 0, 0 );
      keybd_event( (byte) Keys.Tab, 0, KEYEVENTF_KEYUP, 0 );
      keybd_event( (byte) Keys.ShiftKey, 0, KEYEVENTF_KEYUP, 0 );

      // start filtering for mouse clicks outside the popup
      m_filter.Popup = m_popup;
    }

    
    protected override void WndProc( ref Message m )
    {
      base.WndProc( ref  m);
      if( this.m_showing )
      {
        if( m.Msg == ( int )Win32.Msgs.WM_NCACTIVATE )
        {
          if( ((int) m.WParam) == 0 )
            Win32.User32.SendMessage( this.Handle, (int)Win32.Msgs.WM_NCACTIVATE, 1, 0 );
        }
        else if( m.Msg == ( int )Win32.Msgs.WM_ACTIVATEAPP )
        {
          // is the application being deactivated
          if( (int)m.WParam == 0 )
          {
            ClosePopup();
            Win32.User32.PostMessage( this.Handle, ( int )Win32.Msgs.WM_NCACTIVATE, 0, 0 );
          }
        }
      } // if( this.m_showing )
    } // if( this.m_showing )

    
    public void ClosePopup()
    {
      if( m_showing )
      {
        if( m_skipClose )
        {
          // raise event to owner
          OnPopupClosed( new PopupClosedEventArgs( m_popup ) );					
        }
        m_skipClose = false;

        m_owner.RemoveOwnedForm( m_popup );
        m_showing = false;
        m_popup.Closed -= m_popupClosedHandler;
        m_popupClosedHandler = null;
        m_popup.Close();

        Application.RemoveMessageFilter( m_filter );
        m_owner.Activate();
        m_popup = null;
        m_owner = null;
      }
    }
    
    
    protected virtual void OnPopupClosed( PopupClosedEventArgs args )
    {
      if( this.PopupClosed != null )
        this.PopupClosed( this, args );
    }

    
    private void OnPopupClosedHandler( object sender, EventArgs args )
    {
      ClosePopup();
    }

    
    protected virtual void OnPopupCancel( PopupCancelEventArgs args )
    {
      if( this.PopupCancel != null )
      {
        PopupCancel( this, args );
        if( !args.Cancel )
          m_skipClose = true;
      }
    }


    private void OnPopupCancelHandler( object sender, PopupCancelEventArgs args )
    {
      OnPopupCancel( args );
    }

    #endregion
  
  }

  #endregion

  #region PopupWindowHelperMessageFilter

  public class PopupWindowHelperMessageFilter : IMessageFilter
  {
    #region events

    public event PopupCancelEventHandler PopupCancel;

    #endregion

    #region fields
		
    private Form m_popup = null;

    private PopupWindowHelper m_owner = null;

    #endregion

    #region properties

    public Form Popup
    {
      get{ return m_popup; }
      set{ m_popup = value; }
    }


    #endregion

    #region initialization

    public PopupWindowHelperMessageFilter( PopupWindowHelper owner )
    {
      m_owner = owner;
    }


    #endregion

    #region methods
    
    public bool PreFilterMessage( ref Message m )
    {
      if( m_popup != null)
      {
        switch (m.Msg)
        {				
          case (int)Win32.Msgs.WM_LBUTTONDOWN:
          case (int)Win32.Msgs.WM_RBUTTONDOWN:
          case (int)Win32.Msgs.WM_MBUTTONDOWN:
          case (int)Win32.Msgs.WM_NCLBUTTONDOWN:
          case (int)Win32.Msgs.WM_NCRBUTTONDOWN:
          case (int)Win32.Msgs.WM_NCMBUTTONDOWN:
            OnMouseDown();
            break;
        }
      }
      return false;
    }


    private void OnMouseDown()
    {
      Point cursorPos = Cursor.Position;
      if( !m_popup.Bounds.Contains( cursorPos ) )
        OnCancelPopup( new PopupCancelEventArgs( m_popup, cursorPos ) );
    }


    protected virtual void OnCancelPopup( PopupCancelEventArgs args )
    {
      if( this.PopupCancel != null )
        this.PopupCancel (this, args );

      if( !args.Cancel )
      {
        m_owner.ClosePopup();
        m_popup = null;
      }
    }


    #endregion

  }
  #endregion

} // namespace Viatronix.UI
