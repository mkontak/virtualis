// $Id: GLWnd.C,v 1.10.2.3 2009/11/18 19:14:34 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: GLViewWnd.C  


/**
 * Implementation of an OpenGL-enabled CWnd
 */ 


// includes
#include "stdafx.h"
#include "GLWnd.h"
#include "Viatron.h"
#include "V1KMessages.h"
#include "Exception.h"
#include "WinUtils.h"
#include "MainAppFrame.h"
#include "VCSettings.h"
#include "Theme.h"
#include "ViatronDialog.h"

// defines
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define FILE_REVISION "$Revision: 1.10.2.3 $"


// namespaces
using namespace ColonViews::Layouts;

// static initialization
GLChildView GLWnd::m_nullChildView;
const int4 GLWnd::m_iInitialUpdateTimerId = 45678;
const int4 GLWnd::m_iInitialUpdateTimerFrequency = 10;


/**
 * Constructor 
 *
 * @param uViewBkgdResID   Resource ID of the background image
 */
GLWnd::GLWnd( const uint4 uViewBkgdResID ) : 
  m_uViewBkgdResID( uViewBkgdResID ),
  m_iMouseInViewWnd( 0 ),
  m_pCaptureView( NULL ),
  m_lastMousePosition( 0, 0 )
{ 
  m_uViewBkgdResID <= 0 ? IDB_BLUEMETAL : m_uViewBkgdResID;
} // GLWnd()


/**
 * Destructor
 */
GLWnd::~GLWnd()
{
} // ~GLWnd()



BEGIN_MESSAGE_MAP(GLWnd, CWnd)
  //{{AFX_MSG_MAP(GLWnd)
  ON_WM_CREATE()
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  ON_WM_CTLCOLOR()
  ON_WM_PAINT()
  ON_WM_TIMER()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
  ON_MESSAGE(WM_APP_GET_VIEW_WINDOW_CLASSID, OnAppGetChildViewID)
  ON_MESSAGE(WM_APP_GL_IDLE, OnAppGLIdle)
  ON_MESSAGE(WM_APP_CHILD_VIEW_MSG, OnAppChildViewMessage)
  ON_MESSAGE(WM_APP_CHILDTIMER_STATUSCHANGE, OnAppChildViewTimerStatusChange)
  ON_MESSAGE(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, OnAppGLChildViewTypeInvalidate)
END_MESSAGE_MAP()


/**
 * WM_DESTROY handler
 */
void GLWnd::OnDestroy()
{
  CWnd::OnDestroy();

  // delete GLChildViews
  for( uint4 i(0); i < m_vpViews.size(); i++ )
  {
    if( m_vpViews[ i ] )
    {
      GLChildView * pChildView = m_vpViews[ i ];
      pChildView->DestroyWindow();
      delete pChildView;
    }
  }
  m_vpViews.clear();
} // OnDestroy()


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pDC     Device context for window
 * @return  Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL GLWnd::OnEraseBkgnd( CDC * pdc ) 
{
  // get the client area for this window
  CRect rcClient;
  GetClientRect( &rcClient );

  // create a region based on the client area
  CRgn rgnParent;
  rgnParent.CreateRectRgnIndirect( &rcClient );

  // initialize the structure that is to be passed to EnumChildWindows
  EnumChildWindowsData data;
  data.m_pParentWnd = this;
  data.m_pParentRgn = &rgnParent;

  // enumerate all the child windows a subtract their area from the parent region
  EnumChildWindows( GetSafeHwnd(), &GLWnd::UpdateBackgroundRegionCallback, reinterpret_cast< LPARAM >( &data ) );

  // let the derived classes update the parent region
  OnUpdateBackgroundRegion( rgnParent );

  // set the updated region for this window
  // NOTE: the HRGN handle is now owned by the window so we can detach from the mfc wrapper
  SetWindowRgn( reinterpret_cast< HRGN >( rgnParent.Detach() ), false );

  // select the theme background brush into the supplied dc
  CBrush * pOldBrush = pdc->SelectObject( GetViewBackgroundBrush() );

  // get the area that needs updating
  CRect rcClipBox; 
  pdc->GetClipBox( &rcClipBox );     

  // fill the background with the theme brush
  pdc->PatBlt( rcClipBox.left, rcClipBox.top, rcClipBox.Width(), rcClipBox.Height(), PATCOPY );

  // restore original brush
  pdc->SelectObject( pOldBrush );

  // remove the region
  SetWindowRgn( NULL, false );

  // indicate that we have handled WM_ERASEBKGND
  return TRUE;
} // OnEraseBkgnd( CDC * pdc )


/**
 * Callback function for EnumChldWindows ( used when calculating HRGN for background updates )
 *
 * @param     hWnd        window handle to child
 * @param     lParam      user specific data supplied to callback
 * @return    TRUE to continue enumerating; FALSE otherwise
 */
BOOL CALLBACK GLWnd::UpdateBackgroundRegionCallback( HWND hWnd, LPARAM lParam )
{
  /// cast the lParam to appropriate data type
  EnumChildWindowsData * pData( reinterpret_cast< EnumChildWindowsData * >( lParam ) );

  /// retrieve temporary CWnd * for supplied child window handle
  CWnd * pChildWnd = CWnd::FromHandle( hWnd );

  // only exlude region if the child window is visible and not an immediate child of this window
  if( pChildWnd->IsWindowVisible() && pChildWnd->GetParent()->GetSafeHwnd() == pData->m_pParentWnd->GetSafeHwnd() )
  {
    // get the window rectangle for the child window ( screen coordinates )
    CRect rcChild;
    pChildWnd->GetWindowRect( &rcChild );

    // convert from screen coordinates to client (with respect to the childs parent)
    pData->m_pParentWnd->ScreenToClient( &rcChild );

    // if the view is a GLChildView expand the bounds by 1 pixel to accomodate for the white border
    if( dynamic_cast< GLChildView * >( pChildWnd ) )
      rcChild.InflateRect( 1, 1 );
    
    /// create a region based on the rectangle
    CRgn rgnChild;
    rgnChild.CreateRectRgnIndirect( &rcChild );

    // subtract region from parent region
    if( pData->m_pParentRgn->CombineRgn( pData->m_pParentRgn, &rgnChild, RGN_DIFF ) == ERROR )
    {
      LogErr( "Unable remove child window HRGN from parent HRGN", "GLWnd", "UpdateBackgroundRegionCallback" );        
      return TRUE; 
    }
  } // if( pChildWnd->IsWindowVisible() )
  return TRUE;
} // UpdateBackgroundRegionCallback( HWND hWnd, LPARAM lParam )


/**
 * callback function for EnumChildWindows, used to send LayoutChanging/Changed events to dialogs
 *
 * @param     hWnd        window handle to child
 * @param     lParam      user specific data supplied to callback
 * @return    TRUE to continue enumerating; FALSE otherwise
 */
BOOL CALLBACK GLWnd::Callback_SendLayoutChangeEvent( HWND hWnd, LPARAM lParam )
{
  ::SendMessage(hWnd, ColonViews::Dialogs::ViatronDialog::WM_USER_LAYOUT_CHANGE, lParam, lParam);
  return TRUE;
} // Callback_SendLayoutChangeEvent( HWND hWnd, LPARAM lParam )


/**
 * Called by the framework before the creation of the Windows window attached to this CWnd object
 *
 * @param     cs      Create parameters
 * @return    Nonzero if the window creation should continue; 0 to indicate creation failure
 */
BOOL GLWnd::PreCreateWindow( CREATESTRUCT & cs ) 
{
  if( !CWnd::PreCreateWindow( cs ) )
    return FALSE;

  cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
  cs.style &= ~WS_BORDER;
  cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC,
    ::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);
  
  return TRUE;
} // PreCreateWindow()


/**
 * WM_CREATE, called during the creation phase of the window.
 *
 * @param   lpCreateStruct   Info on the window being created
 * @return  Return 0 to continue the creation of the CWnd object. If –1, the window will be destroyed.
 */
int GLWnd::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
  if( CWnd::OnCreate( lpCreateStruct ) == -1 )
    return -1;

  // remove the client edge from the mdi client window
  SetWindowLong( m_hWnd, GWL_EXSTYLE, GetWindowLong( m_hWnd, GWL_EXSTYLE ) & ~WS_EX_CLIENTEDGE );

  
  // create child view windows
  CreateChildViews( GetSafeHwnd() );

  // if no views created in CreateChildViews method, call set child views function with empty list for initializations
  if( m_vpViews.empty() )
    SetChildViews( std::list<GLChildViewPtr>() );

  EnableWindow( false );

  return 0;
} // OnCreate() 


/**
 * WM_PAINT handler, called when window needs repainting
 */
void GLWnd::OnPaint() 
{
  try
  {
    CPaintDC dc( this );

    Gdiplus::Graphics gfx( m_hWnd );

    Gdiplus::Pen pen( Gdiplus::Color::White );
   
    // Draw frames around each view window
    for( int4 iView( 0 ); iView < NumGLViews(); iView++)
    {
      if( !m_vpViews[iView]->IsWindowVisible() )
        continue;

      Rect< int > bounds = m_vpViews[iView]->GetViewRect();
      bounds.Inflate( 1, 1, 0, 0 );

      gfx.DrawRectangle( &pen, bounds.m_left, bounds.m_top, bounds.Width(), bounds.Height() );
    }

    // process idle routines
    for( IdleRoutineToDataMap::iterator itMap = m_mapIdleRoutines.begin(); itMap != m_mapIdleRoutines.end(); ++itMap )
      itMap->first( itMap->second );
  }
  catch( ... )
  {
    ex::RuntimeException( LogRec( "<GLWnd::OnPaint> Unhandled exception.", "GLWnd", "OnPaint" ) );
  }
} // OnPaint() 


/**
 * WM_CTLCOLOR, colors window controls as appropriate
 *
 * @param   pDC         Window device context
 * @param   pWnd        Pointer to control being colored
 * @param   nCtlColor   Identifies type of control
 * @return  Handle to brush that will be used to color the control
 */
HBRUSH GLWnd::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT uCtlColor )
{
  HBRUSH hbr = CWnd::OnCtlColor( pDC, pWnd, uCtlColor );

  pDC->SetBkMode( TRANSPARENT );

  if( uCtlColor != CTLCOLOR_BTN )
    pDC->SetTextColor( 0xffffff );

  if( uCtlColor != CTLCOLOR_EDIT )
    hbr = (HBRUSH) ( * WinUtils::Theme::DefaultBackgroundBrush );

  return hbr;
} // OnCtlColor()


/**
 * WM_SIZE, Called when window is resized
 *
 * @param nType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void GLWnd::OnSize( UINT uType, int iCX, int iCY ) 
{

  CWnd::OnSize( uType, iCX, iCY );

  m_viewWndSz = CSize( iCX, iCY );
  
  // custom layouts are now done in each view separately
  CalculateViewRects( iCX, iCY );

  // Inform the views that they've been resized
  for( int iView = 0; iView < NumGLViews(); iView++)
  {
    GLChildViewPtr pView = m_vpViews[iView];
    pView->OnSize(uType, pView->GetViewRect().Width(), pView->GetViewRect().Height());
  }

} // OnSize()


/**
 * Windows procedure (WindowProc) for a CWnd object.
 *
 * @param   uMessage  Specifies the Windows message to be processed.
 * @param   wParam    The parameter value depends on the message
 * @param   lParam    The parameter value depends on the message
 * @return  The return value depends on the message.
 */
LRESULT GLWnd::WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
  if( uMessage == WM_MOUSEWHEEL )
  {
    DWORD dwPos = GetMessagePos();
    CPoint point( LOWORD( dwPos ), HIWORD( dwPos ) );
    ScreenToClient( &point );

    CWnd * pWnd = ChildWindowFromPoint( point , CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT );

    Dialogs::ViatronDialog * pViatronDialog = dynamic_cast< Dialogs::ViatronDialog*> ( pWnd );

    //If the Child window is of type 'ViatronDialog' let us handle the scroll wheel
    //in the normal way. This fixes the issue Scroll wheel in Dialogs
    if( pViatronDialog == NULL && pWnd && (HWND(*pWnd) != HWND( *this ) ) )
      return pWnd->PostMessage(WM_MOUSEWHEEL, wParam, lParam );
  }
  return CWnd::WindowProc( uMessage, wParam, lParam );
} // WindowProc()


/**
 *  Translate window messages before they are dispatched 
 *
 * @param     pMsg    message to process
 * @return    Nonzero if the message was translated and should not be dispatched;0 otherwise.
 */
BOOL GLWnd::PreTranslateMessage( MSG * pMsg )
{
  if( pMsg->message == WM_KEYDOWN )
  {
    OnKeyDown( pMsg->wParam, LOWORD( pMsg->lParam  ), HIWORD( pMsg->lParam ) );
    return FALSE;
  }
  else if( pMsg->message == WM_KEYUP )
  {
    OnKeyUp( pMsg->wParam, LOWORD( pMsg->lParam  ), HIWORD( pMsg->lParam ) );
    return FALSE;
  }
  else if( pMsg->message == WM_MOUSEMOVE )
  {
    if( NeedToRerouteMouseMove( pMsg->hwnd ) )
    {
      CPoint position( pMsg->lParam );
      ScreenToClient( &position );
      if( position != m_lastMousePosition )
      {
        m_lastMousePosition = position;
        CWnd::FromHandle( pMsg->hwnd )->ClientToScreen( &position );
        OnMouseMove( pMsg->wParam,  position );
      }
      return TRUE;
    }
  }
  return CWnd::PreTranslateMessage( pMsg );
} // PreTranslateMessage()


/**
 * Determines if hWnd refers to "this" wnd or to any of it's children
 *
 * @return true if hWnd refers to "this" wnd or to any of it's children
 */
bool GLWnd::NeedToRerouteMouseMove( HWND hWnd )
{
  if( GetSafeHwnd() == hWnd )
    return true;

  for( int4 i(0); i < NumGLViews(); i++ )
  {
    if( m_vpViews[ i ]->GetSafeHwnd() == hWnd )
      return true;
  }
  return false;
} // NeedToRerouteMouseMove( HWND hWnd )


/**
 * return the brush to be used for the view's background
 *
 * @return brush used for the view's background
 */
CBrush * GLWnd::GetViewBackgroundBrush()
{
  return WinUtils::Theme::DefaultBackgroundBrush;
} // GetViewBackgroundBrush()


/**
 * WM_KEYDOWN, called from PreTranslateMessage() in order to dispatch to the message the correct child view.
 *
 * @param   uChar     Specifies the virtual key code of the given key.
 * @param   uRepCnt   Repeat count.
 * @param   uFlags    Specifies the scan code, key-transition code, previous key state, and context code.
 */
void GLWnd::OnKeyDown( UINT uChar, UINT uRepCnt, UINT uFlags )
{
  m_vpViews[ m_iMouseInViewWnd ]->OnKeyDown( uChar, uRepCnt, uFlags );
  CWnd::OnKeyDown( uChar, uRepCnt, uFlags );
} // OnKeyDown()


/**
 * WM_KEYUP, called from PreTranslateMessage() in order to dispatch the message to the correct child view.
 *
 * @param   uChar     Specifies the virtual key code of the given key.
 * @param   uRepCnt   Repeat count.
 * @param   uFlags    Specifies the scan code, key-transition code, previous key state, and context code.
 */
void GLWnd::OnKeyUp( UINT uChar, UINT uRepCnt, UINT uFlags )
{
  m_vpViews[ m_iMouseInViewWnd ]->OnKeyUp( uChar, uRepCnt, uFlags );
  CWnd::OnKeyUp( uChar, uRepCnt, uFlags );
} // OnKeyUp()


/**
 * WM_MOUSEMOVE, called from PreTranslateMessage() in order to dispatch the message the correct child view.
 *
 * @param   uFlags    Indicates whether various virtual keys are down( i.e. shift, alt )
 * @param   point     Specifies the x- and y-coordinate of the cursor (client coordinates)
 */
void GLWnd::OnMouseMove( UINT uFlags, CPoint point )
{
  
  m_iMouseInViewWnd = NumGLViews();
  for( int4 i(0); i < NumGLViews(); i++ )
  {
    if( m_vpViews[ i ]->IsWindowVisible() && m_vpViews[ i ]->GetViewRect().PtInRect( point.x, point.y ) )
    {
      if( i !=  m_iMouseInViewWnd )
      {
        TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE,  m_vpViews[ i ]->GetSafeHwnd(), HOVER_DEFAULT };
        _TrackMouseEvent( &tme );
      }
      m_iMouseInViewWnd = i;
      break;
    }
  }   
  
  
  GLChildView * pView =  GetCapture() != NULL? dynamic_cast< GLChildView * >( GetCapture() ) : m_vpViews[ m_iMouseInViewWnd ];
  if( pView != NULL )
  {
    Rect< int4 > viewrect = pView->GetViewRect();
    point.Offset( -viewrect.m_left, -viewrect.m_top );
    pView->OnMouseMove( uFlags, point );
  }  
} // OnMouseMove()


/**
 * Menu item, control notify, or accelerator message handler
 *
 * @param wParam   LOWORD: Command ID; HIGHWORD Message (control), 1 (accelerator) or 0 (menu) 
 * @param lParam   Control handle (control) or zero
 * @return BOOL    Nonzero if message is processed; otherwise 0
 */
BOOL GLWnd::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
  BOOL bReturn = CWnd::OnCommand(wParam, lParam);

  GLRenderViews();

  return bReturn;
} // OnCommand()


/**
 * Initially updates the child views
 */
void GLWnd::BeginInitialUpdate()
{
  SetTimer( m_iInitialUpdateTimerId, m_iInitialUpdateTimerFrequency, NULL );
} // BeginInitialUpdate()


/**
 * Called when te view is initialzed
 */
void GLWnd::OnInitialized()
{
  for( int i( 0 ); i < NumGLViews(); ++i )
    GetChildViewByPos( i ).SetRenderOnlyBackground( false );
    
  if( IsWindow( GetSafeHwnd() ) )
  {
    InvalidateViews( GLChildView::eGVT_ALL, 0 );
  
    GLRenderViews();
  }
  EnableWindow( true );
} // OnInitialized()


/**
 * Timer event triggered.
 *
 * @param uIDEvent   ID of timer event.
 */
void GLWnd::OnTimer( UINT_PTR uIDEvent )
{
  
  if( uIDEvent == m_iInitialUpdateTimerId )
  {
    KillTimer( m_iInitialUpdateTimerId );
    OnInitialized();    
  }
  else
  {
    std::map<uint4,uint4>::iterator itTimer = m_mapTimerToChildCtrlID.find(uIDEvent);
    if (itTimer != m_mapTimerToChildCtrlID.end())
    {
      GetChildViewDlgItem(itTimer->second).OnTimer(uIDEvent);
    }
  }

  CWnd::OnTimer( uIDEvent );
} // OnTimer()


/**
 * Called before the current layout changes
 */
void GLWnd::OnLayoutChanging()
{
  for( int i( 0 ); i < m_vpViews.size(); ++i )
    m_vpViews[ i ]->OnLayoutChanging();

  EnumChildWindows( GetSafeHwnd(), &GLWnd::Callback_SendLayoutChangeEvent,
    LPARAM( ColonViews::Dialogs::ViatronDialog::eWMUserWPLayoutChanging ) );
} // OnLayoutChanging()


/**
 * Called after the current layout changes
 */
void GLWnd::OnLayoutChanged()
{  
  for( int i( 0 ); i < m_vpViews.size(); ++i )
    m_vpViews[ i ]->OnLayoutChanged();

  EnumChildWindows( GetSafeHwnd(), &GLWnd::Callback_SendLayoutChangeEvent,
    LPARAM( ColonViews::Dialogs::ViatronDialog::eWMUserWPLayoutChanged ) );
} // OnLayoutChanged()


/**
 * update all views
 * @param wParam   GLChildView::ViewTypeEnum value, indicates the child view type to refresh
 * @param lParam   GLVID value, the unique ID of the child view
 */
void GLWnd::InvalidateViews( WPARAM wParam, LPARAM lParam )
{
  OnAppGLChildViewTypeInvalidate( wParam, lParam );
}

/**
 * Renders a view
 *
 * @param iView   View vector position
 */
void GLWnd::GLRenderView(const int4 iView)
{
  GLRenderView( *m_vpViews[iView] );
} // GLRenderView()


/**
 * Renders a view
 *
 * @param hDC         Device context
 * @param childView   View to render
 */
void GLWnd::GLRenderView( GLChildViewRef childView )
{
  bool bIsWindow( IsWindow( childView.m_hWnd ) );
  if( !bIsWindow || ( bIsWindow &&  childView.IsWindowVisible() ))
    childView.GLRender();
} // GLRenderView


/**
 * Idle-time processing
 */
void GLWnd::GLIdle()
{
  GLRenderViews();
}


/**
 * Renders all child views
 */
void GLWnd::GLRenderViews()
{
  for( int4 iView( 0 ); iView < NumGLViews(); iView++ )
    GLRenderView( iView );
} // GLRenderViews()




/**
 * Sets the list of child views for this parent view instance (called at most once)
 *
 * @param childViewPtrList   List of child views.
 */
void GLWnd::SetChildViews(const std::list<GLChildViewPtr>& childViewPtrList)
{
  if (!m_vpViews.empty())
    throw ex::UnsupportedOperationException(LogRec("Only one call to SetChildViews currently allowed per view instance", "GLWnd", "SetChildViews"));

  // Resize vector for child view pointers, with an extra entry for the null-event view
  m_vpViews.resize(childViewPtrList.size() + 1);
  uint4 uView(0);
  for (std::list<GLChildViewPtr>::const_iterator it = childViewPtrList.begin();
    it != childViewPtrList.end(); m_vpViews[uView++] = *it, it++);

  // Create the extra (null-event) view
  m_vpViews[NumGLViews()] = new GLChildView(GLChildView::eGVT_NONE );

  // Mouse defaults to not in any view
  m_iMouseInViewWnd = NumGLViews();
}



/**
 * Gets the GLChildView identified by uDlgControlID (analogous to CWnd::GetDlgItem())
 *
 * @param     uDlgControlID     The dialog control ID for the GLChildView to lookup.
 * @return    Reference to child view window
 */
GLChildViewRef GLWnd::GetChildViewDlgItem( const uint4 uDlgControlID )
{
  for( std::vector< GLChildViewPtr >::iterator it = m_vpViews.begin(); it != m_vpViews.end(); ++it )
  {
    if( (*it)->GetControlID() == uDlgControlID )
    {
      return **it;
    }
  }
  return m_nullChildView;
} // GetChildViewDlgItem()


/**
 * Gets a child view pointer by ID
 *
 * @param   id    ID of the child view
 * @return  Reference to child view window
 */
GLChildViewRef GLWnd::GetChildView( const GLChildView::GLVID & id ) const
{
  for( int i( 0 ); i < NumGLViews(); ++i )
  {
    if( m_vpViews[ i ]->GetClassInstanceID() == id )
    {
      return *m_vpViews[ i ];
    }
  }
  return m_nullChildView;
} // GetChildView()


/**
 * Gets a child view vector position (i.e. the i-th view) by id
 *
 * @param     id      Child view ID
 * @return    Position of the child view
 */
int4 GLWnd::GetChildViewPos( const GLChildView::GLVID & id ) const
{
  for( int i( 0 ); i < NumGLViews(); ++i )
  {
    if( m_vpViews[ i ]->GetClassInstanceID() == id )
    {
      return i;
    }
  }  
  return -1;
} // GetChildViewPos()


/**
 * Gets the current view window
 *
 * @return  Reference to child view window
 */
GLChildViewRef GLWnd::GetCurrentView() const
{
  return ( ( m_pCaptureView != GLChildViewPtr( NULL ) ) ? *m_pCaptureView : *m_vpViews[ m_iMouseInViewWnd ] );
} // GetCurrentView()


/**
 * Gets the childview at the given point
 *
 * @param   pt                Point to test
 * @param   eFindFlags        Flags restricting search
 * @return  Reference to child view window
 */
GLChildViewRef GLWnd::GetChildViewFromPoint( const Point2D< int4 > & pt, const CVPFlagsEnum & eFindFlags ) const
{
  for( std::vector< GLChildViewPtr >::const_iterator it = m_vpViews.begin(); it != m_vpViews.end(); ++it )
  {
    GLChildViewPtr pView = *it;
    if ( (((eFindFlags&eCVP_OnlyVisible)==0) || pView->IsWindowVisible()) && pView->GetViewRect().PtInRect(pt))
    {
      return *pView;
    }
  }
  return GetNullEventView();
} // GetChildViewFromPoint()


/**
 * Gets view rectangle for the specified view (by position)
 *
 * @param   iView        Position of the child view in view window vector
 * @return  View rectangle for the child view
 */
Rect<int> GLWnd::GetViewRect( const int4 iView ) const
{
  return m_vpViews[iView]->GetViewRect();
} // GetViewRect()


/**
 * Gets view rectangle for the specified view
 *
 * @param   view         Child view
 * @return  View rectangle for the child view
 */
Rect<int> GLWnd::GetViewRect( const GLChildView & view ) const
{
  return view.GetViewRect();
} // GetViewRect()


/**
 * Sets the view rectangle for the specified view
 *
 * @param iView      Child view index
 * @param viewRect   View rectangle for child view
 */
void GLWnd::SetViewRect( const int4 iView, const Rect<int4> & viewRect )
{
  SetViewRect( GetChildViewByPos(iView), viewRect );
} // SetViewRect()


/**
 * Sets the view rectangle for the specified view
 *
 * @param childView   Child view index
 * @param viewRect    View rectangle for child view
 */
void GLWnd::SetViewRect(GLChildViewRef childView, const Rect<int4> & viewRect)
{
  childView.SetWindowPos( NULL, viewRect.m_left, viewRect.m_top, viewRect.Width(), viewRect.Height(),  SWP_NOZORDER | SWP_NOREDRAW  | SWP_NOACTIVATE );
} // SetViewRect()


/**
 * Swaps the view rectangles of the two specified views
 *
 * @param view1   View 1
 * @param view2   View 2
 */
void GLWnd::SwapViewRects( GLChildView & view1, GLChildView & view2 )
{
  Rect<int> view1rect(view1.GetViewRect());
  Rect<int> view2rect(view2.GetViewRect());

  SetViewRect( view1, view2rect );
  SetViewRect( view2, view1rect );

  view1.OnSize(SIZE_MAXHIDE, view1.GetViewRect().Width(), view1.GetViewRect().Height());
  view2.OnSize(SIZE_MAXHIDE, view2.GetViewRect().Width(), view2.GetViewRect().Height());
} // SwapViewRects()



/**
 * Returns the captured view if any (or NULL if no captured view)
 *
 * @return   The captured view if any (or NULL if no captured view)
 */
const GLChildViewPtr GLWnd::GetCaptureViewPtr()
{
  return m_pCaptureView;
} // GetCaptureViewPtr()


/**
 * Get info on the child view with specified ID
 *
 * @param uChildID             ID for the child view
 * @return GLChildView::Info   Info on the child view
 */
GLChildView::Info GLWnd::GetChildViewInfo( const GLChildView::GLVID & uChildID ) const
{
  GLChildView::Info info;
  GLChildViewRef view = GetChildView(uChildID);
  info.m_uId = view.GetClassInstanceID();
  info.m_eType = view.GetViewWndType();  
  return info;
} // GetChildViewInfo()


/**
 * Broadcasts a message to multiple childviews.
 *
 * @param iWndTypes   Indicates GLChildView::ViewTypeEnum view types.
 * @param uMsg        Message to broadcast.
 * @param wParam      Message-dependent parameter.
 * @param lParam      Message-dependent parameter.\
 * @param bWait       True to send, or false to post, the message.
 */
void GLWnd::BroadcastMessage(int4 iWndTypes, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, const bool bWait)
{
  // Allow derived classes to redirect the message as they see fit
  RedirectBroadcastMessage(iWndTypes, uMsg);

  // Store the current captured view
  GLChildView * pCaptureView = m_pCaptureView;
  int4 iCaptureView(m_iMouseInViewWnd);

  bool bSent(false);
  
  if (iWndTypes > 0)
  {
    for (int4 iView(0); iView < NumGLViews(); iView++)
    {
      if (m_vpViews[iView]->GetViewWndType() & iWndTypes)
      {
        // Set the mouse capture to this view so it will receive the event
        m_pCaptureView = m_vpViews[iView];
        m_iMouseInViewWnd = iView;
        
        m_vpViews[iView]->SendMessage(uMsg, wParam, lParam);
        bSent = true;
      }
    }
  }
  
  // Restore the current captured view
  m_pCaptureView = pCaptureView;
  m_iMouseInViewWnd = iCaptureView;
  
  if (!bSent)
  {
    if (bWait)
    {
      SendMessage(uMsg, wParam, lParam);
    }
    else
    {
      PostMessage(uMsg, wParam, lParam);
    }
  }
}


/**
 * Message: WM_APP_GL_IDLE: Idle-time processing.
 *
 * @param WPARAM    Unused.
 * @param LPARAM    Unused.
 * @return LRESULT  True.
 */
LRESULT GLWnd::OnAppGLIdle(WPARAM, LPARAM)
{
  for( IdleRoutineToDataMap::iterator itMap = m_mapIdleRoutines.begin(); itMap != m_mapIdleRoutines.end(); itMap++ )
    itMap->first( itMap->second );
  
  GLIdle();
  return TRUE;
}


/**
 * Flags all child views of a specified type to be updated on next render (WM_PAINT)
 *
 * @param wpGLChildViewType   GLChildView::ViewTypeEnum value, indicates the child view type to refresh
 * @param lpGLChildViewID     GLVID value, the unique ID of the child view
 * @return                    TRUE
 */
LRESULT GLWnd::OnAppGLChildViewTypeInvalidate(WPARAM wpGLChildViewType, LPARAM lpGLChildViewID)
{
  if( wpGLChildViewType != 0 )
  {
    for( int iView( 0 ); iView < NumGLViews(); ++iView )
    {
      if( wpGLChildViewType & m_vpViews[ iView ]->GetViewWndType() )
        m_vpViews[ iView ]->InvalidateView();
    }
    
  }
  return TRUE;
} // OnAppGLChildViewTypeInvalidate()


/**
 * Gets the class instance ID for a childview at the given index
 *
 * @param wParam     Index of child view.
 * @param lParam     Unused.
 * @return LRESULT   ID of child or 0 if index out of range.
 */
LRESULT GLWnd::OnAppGetChildViewID( WPARAM wParam, LPARAM lParam )
{
  if ( (wParam >= 0) && (wParam < NumGLViews()) )
  {
    return LRESULT(m_vpViews[wParam]->GetClassInstanceID());
  }
  else
  {
    return 0;
  }
} // OnAppGetChildViewID()


/**
 * App child view message
 *
 * @param wParam     (loword) Destination child ID, (hiword) message.
 * @param lParam     Message-dependent parameter.
 * @return LRESULT   Zero.
 */
LRESULT GLWnd::OnAppChildViewMessage( WPARAM wParam, LPARAM lParam )
{
  // LOWORD(wParam) == child view ID for destination of message
  for (int4 iView(0); iView < NumGLViews(); iView++)
  {
    if (m_vpViews[iView]->GetClassInstanceID() == LOWORD(wParam))
    {
      return m_vpViews[iView]->OnMessage(HIWORD(wParam), lParam);
    }
  }
  
  return 0;
} // OnAppChildViewMessage()


/**
 * Change in a child view timer
 *
 * @param iTimerID      ID for the win timer
 * @param uChildCtlID   Non-zero indicates set timer, is control ID for the child view; zero indicates kill timer
 * @return LRESULT      Zero.
 */
LRESULT GLWnd::OnAppChildViewTimerStatusChange(WPARAM iTimerID, LPARAM uChildCtlID)
{
  if (uChildCtlID != 0)
  {
    // Set/reset timer event
    m_mapTimerToChildCtrlID.insert(std::map<uint4,uint4>::value_type(iTimerID, uChildCtlID));
  }
  else
  {
    // kill timer event
    std::map<uint4,uint4>::iterator itTimer = m_mapTimerToChildCtrlID.find(iTimerID);
    if (itTimer != m_mapTimerToChildCtrlID.end())
    {
      m_mapTimerToChildCtrlID.erase(iTimerID);
    }
  }
  return 0;
} // OnAppChildViewTimerStatusChange()


/**
 * Override to validate the popup menu being displayed.
 *
 * @param childView      Child view whose popup menu will be displayed.
 * @param uPopupMenuID   ID Popup menu requested to be displayed.
 * @return uint4         ID of popup to display, or 0 to refuse display of popup menu.
 */
uint4 GLWnd::OnShowPopupMenu( const GLChildView& childView, const uint4 uPopupMenuID ) const
{
  return uPopupMenuID;
}


// $Log: GLWnd.C,v $
// Revision 1.10.2.3  2009/11/18 19:14:34  dongqing
// Issue Cad09-03: second cad pane to registration-cad layout
//
// Revision 1.10.2.2  2008/02/04 22:35:59  jmeade
// comment.
//
// Revision 1.10.2.1  2007/10/03 02:28:57  jmeade
// allow childviews to set their own window background colour.
//
// Revision 1.10  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.9  2007/02/08 17:21:16  romy
// Issue # 5306 is fixed by checking the childWindow type is that of ViatronDlg. If so let him take the normal message.
//
// Revision 1.8  2007/01/09 19:12:31  jmeade
// layout changed/changing events.
//
// Revision 1.7  2006/08/01 19:16:53  jmeade
// comments.
//
// Revision 1.6  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.3  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.2  2005/11/04 18:06:24  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.1.2.9  2006/05/22 12:41:10  geconomos
// making call to DestoryWindow() on all child controls during cleanup
//
// Revision 1.1.2.8  2006/05/16 12:42:33  geconomos
// proper destruction of child views in WM_DESTROY handler
//
// Revision 1.1.2.7  2006/05/09 13:54:25  geconomos
// initialization of m_lastMousePosition
//
// Revision 1.1.2.6  2006/02/28 17:25:15  geconomos
// reworked WM_MOUSEMOVE handling
//
// Revision 1.1.2.5  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.1.2.4  2006/01/04 13:05:32  frank
// Moved default background brush to the theme class
// Pre-blended logo for supine/prone view to fix over-accumulation of white
//
// Revision 1.1.2.3  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.1.2.2  2005/11/17 23:01:24  geconomos
// more efficient method for painting the background on WM_ERASEBKGND
//
// Revision 1.1.2.1  2005/11/04 18:21:53  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.2  2005/11/04 18:06:24  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.1  2005/10/28 17:47:34  geconomos
// renamed files from GLViewWnd => GLWnd
//
// Revision 1.9  2005/10/25 15:47:37  jmeade
// ParentViews ==> Layouts
//
// Revision 1.8  2005/10/03 13:22:54  geconomos
// removed references to brushes
//
// Revision 1.7  2005/10/03 13:00:21  geconomos
// drawing background with solid brush
//
// Revision 1.6  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.5.2.2  2005/09/21 20:05:15  geconomos
// removed info logging messages
//
// Revision 1.5.2.1  2005/09/21 13:27:15  geconomos
// modified code to make non-window calls during rendering
//
// Revision 1.5  2005/08/30 16:01:44  geconomos
// Fixed crash when DrawBackgroundImage is set to -1 in the config file.
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/17 18:58:44  geconomos
// Added support to render the background using a default solid brush instead of an image. ( vcsettings.xml - DrawBackgroundImage )
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.31.2.13.4.13.2.7  2005/07/15 17:21:43  geconomos
// Issue # 4225: Drawing is choppy while switching views
//
// Revision 3.31.2.13.4.13.2.6  2005/07/13 15:58:42  geconomos
// Issue #4268: Added a check to ensure that a child view is visible before drawing it's border
//
// Revision 3.31.2.13.4.13.2.5  2005/07/06 18:00:42  geconomos
// Renamed Invalidate to invalidateView ( clashing with CWnd::Invalidate() )
//
// Revision 3.31.2.13.4.13.2.4  2005/07/06 13:26:59  geconomos
// Issue #4236: Added call to GLChildView::Invalidate when invalidating child views.
//
// Revision 3.31.2.13.4.13.2.3  2005/06/23 19:08:53  geconomos
// Issue #4222: Overview not showing when prone only is loaded.
//
// Revision 3.31.2.13.4.13.2.2  2005/06/22 19:47:58  geconomos
// added suppression of WM_MOUSEMOVE if mouse position hasn't changed
//
// Revision 3.31.2.13.4.13.2.1  2005/06/17 13:16:09  geconomos
// using new BackgroundBrush class
//
// Revision 3.31.2.13.4.13  2005/06/14 19:13:26  geconomos
// Un-did changes to high resolution 3d rendering.
//
// Revision 3.31.2.13.4.12  2005/06/14 13:48:42  geconomos
// Added a check to ensure view pointer is not null in OnMouseMove()
//
// Revision 3.31.2.13.4.11  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 3.31.2.13.4.10  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.31.2.13.4.8.2.2  2005/06/08 14:01:10  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.31.2.13.4.8.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLWnd.C,v 1.10.2.3 2009/11/18 19:14:34 dongqing Exp $
// $Id: GLWnd.C,v 1.10.2.3 2009/11/18 19:14:34 dongqing Exp $
