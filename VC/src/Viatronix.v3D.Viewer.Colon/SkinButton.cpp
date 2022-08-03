// $Id: SkinButton.cpp,v 1.10.2.1 2009/12/11 22:32:32 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

/// includes
#include "stdafx.h"
#include "SkinButton.h"
#include "Theme.h"

#define FILE_REVISION "$Revision: 1.10.2.1 $"

/// namespaces
using namespace WinUtils;

/// SkinButton static members
uint4 SkinButton::m_uGlobalHelpTextMessageID = 0;
uint4 SkinButton::m_uDefaultButtonResID = 0;

/**
 * Constructor
 */
SkinButton::SkinButton() : 
  m_pButtonImages( NULL ),
  m_pBackbrush( NULL ),
  m_eButtonState( SkinButton::UP ),
  m_bMouseOver( false ),
  m_iCheck( 0 ),
  m_dwResourceId( 0 ),
  m_bHasFocus( false )
{
} // SkinButton()


/**
 * Destructor
 */
SkinButton::~SkinButton()
{
} // ~SkinButton()



/**
 * MFC message map
 */
BEGIN_MESSAGE_MAP( SkinButton, CWnd )
  ON_WM_DESTROY()
  ON_WM_PAINT()
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_ENABLE()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_MESSAGE( BM_GETCHECK, OnGetCheck )
  ON_MESSAGE( BM_SETCHECK, OnSetCheck )
END_MESSAGE_MAP()


/**  
 * Creates the button
 */
 bool SkinButton::Create( LPCTSTR sText, DWORD dwStyle, CRect & rc, CWnd * pParent, DWORD dwId )
{
  if( !CWnd::Create( NULL, sText, dwStyle, rc, pParent, dwId ) )
    return false;

  return true;
} // Create( LPCTSTR sText, DWORD dwStyle, CRect & rc, CWnd * pParent, DWORD dwId )


/////////////////////////////////////////////////////////////////////////////
// SkinButton


/**
 * WM_DESTROY handler
 */
void SkinButton::OnDestroy()
{
  CWnd * pMainWnd = AfxGetMainWnd();
  if( pMainWnd != NULL )
    pMainWnd->PostMessage( m_uGlobalHelpTextMessageID, 0, 0 );

  if( m_pButtonImages != NULL )
  {
    delete m_pButtonImages;
    m_pButtonImages = NULL;
  }
} // OnDestroy()


/**
 * WM_PAINT handler
 */
void SkinButton::OnPaint()
{ 
  CPaintDC dc( this );

  if( m_pButtonImages == NULL )
    SetBitmap( SkinButton::m_uDefaultButtonResID );

  const bool bButtonEnabled( IsWindowEnabled() );
  int4 iWidth = m_pButtonImages->GetWidth();
  int4 iHeight = static_cast< int4 >(  m_pButtonImages->GetHeight() / 4.0F  + 0.5F );
  int4 x = 0; 
  int4 y = static_cast< int4 >( bButtonEnabled? m_eButtonState : SkinButton::DISABLED ) * iHeight;

  Gdiplus::Graphics gfx( GetSafeHwnd() );

  Gdiplus::Color color;
  m_pButtonImages->GetPixel( 0, 0, &color );

  Gdiplus::ImageAttributes imAtt;
  imAtt.SetColorKey(color, color, Gdiplus::ColorAdjustTypeBitmap);

  Gdiplus::Rect dst( 0, 0, iWidth, iHeight );
  gfx.DrawImage( m_pButtonImages, dst, x, y, iWidth, iHeight, Gdiplus::UnitPixel, &imAtt );

  CString sButtonText;
  GetWindowText( sButtonText );

  if ( !sButtonText.IsEmpty() && ((GetStyle() & BS_BITMAP) != BS_BITMAP))
  {
    USES_CONVERSION;

    // draw the text
    Gdiplus::StringFormat format;
    format.SetAlignment( Gdiplus::StringAlignmentCenter );
    format.SetLineAlignment( Gdiplus::StringAlignmentCenter );
    format.SetHotkeyPrefix( Gdiplus::HotkeyPrefixShow  );

    Gdiplus::RectF layout( 0, 0, iWidth, iHeight );
    gfx.DrawString( A2W( sButtonText ), 
                    sButtonText.GetLength(), 
                    Theme::ButtonFont,  
                    layout,                    
                    &format,
                    bButtonEnabled? Theme::ButtonBrush : Theme::ButtonDisabledBrush );
  }

  if( m_bHasFocus )
  {
    dst.Inflate( -6, -6 );
    gfx.DrawRectangle( Theme::ButtonFocusPen, dst );
  }
} // OnPaint()


/**
 * WM_ERASEBKGND handler
 * 
 * @param  pdc   device context for button
 * @return       whether to erase background
 */
BOOL SkinButton::OnEraseBkgnd( CDC * pdc )
{

  if( m_pBackbrush )
  {
    // select the theme background brush into the supplied dc
    CBrush * pOldBrush = pdc->SelectObject( const_cast< CBrush * >( m_pBackbrush ) );

    // get the area that needs updating
    CRect rcClipBox; 
    GetClientRect( rcClipBox );
  
    // fill the background with the theme brush
    pdc->PatBlt( rcClipBox.left, rcClipBox.top, rcClipBox.Width(), rcClipBox.Height(), PATCOPY );
  
    // restore original brush
    pdc->SelectObject( pOldBrush );

  }
  return TRUE;
} // OnEraseBkgnd( CDC * pdc )


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void SkinButton::OnSize( UINT uType, int iCX, int iCY ) 
{
  CWnd::OnSize( uType, iCX, iCY );
} // OnSize(UINT uType, int iCX, int iCY) 


/**
 * Left mouse click handler; pauses for 25ms to ensure animation (button drawn in down position) will be visible
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse click point
 */
void SkinButton::OnLButtonDown( UINT uFlags, CPoint point )
{
  SetCapture();
  SetButtonState( SkinButton::DOWN );
} // OnLButtonDown( UINT uFlags, CPoint point )


/**
 * Left mouse click handler; pauses for 25ms to ensure animation (button drawn in down position) will be visible
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse click point
 */
void SkinButton::OnLButtonUp( UINT uFlags, CPoint point)
{
  if( IsMouseOverButton() )
  {
    SetButtonState( SkinButton::HOVER );
    GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), BN_CLICKED ), 0 );
    OnClicked();
  }
  else
    SetButtonState( SkinButton::UP );

  ReleaseCapture();
} // OnLButtonUp( UINT uFlags, CPoint point)


/**
 * Mouse move handler; displays help text if available for button, tracks mouse for mouse leave event
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse move point
 */
void SkinButton::OnMouseMove( UINT uFlags, CPoint point ) 
{
  CWnd::OnMouseMove( uFlags, point );

  SkinButtonStateEnum eNewState;
  if( uFlags & MK_LBUTTON )
    eNewState = SkinButton::DOWN;
  else
    eNewState = SkinButton::HOVER;

  SetButtonState( eNewState );

} // OnMouseMove( UINT uFlags, CPoint point ) 


/**
 * Called when mouse enters the window
 */
void SkinButton::OnMouseEnter()
{
} // OnMouseEnter()


/**
 * Called when mouse leaves window
 */
void SkinButton::OnMouseLeave()
{
  m_eButtonState = SkinButton::UP;
  
  RedrawWindow();

} // OnMouseLeave()


/**
 * WM_ENABLED handler
 *
 * @param   bEnable   indicates if window is enable or not
 */
void SkinButton::OnEnable( BOOL bEnable )
{
  RedrawWindow();
} // OnEnable( BOOL bEnable )


/**
 * gain focus
 */
void SkinButton::OnSetFocus( CWnd* pOldWnd )
{
  m_bHasFocus = true;
  RedrawWindow();
} // OnSetFocus( pOldWnd )


/**
 * lose focus
 */
void SkinButton::OnKillFocus( CWnd* pNewWnd )
{
  m_bHasFocus = false;
  RedrawWindow();
} // OnKillFocus( pNewWnd )


/**
 * Window message procedure; resets help text when appropriate
 * 
 * @param uMessage   Window message
 * @param wParam     Message parameter
 * @param lParam     Message parameter
 * @return           Result of processed message
 */
LRESULT SkinButton::WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
  switch ( uMessage )
  {
  case WM_MOUSEMOVE:
  {
    if( !m_bMouseOver )
    {
      m_bMouseOver = true;
      TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, GetSafeHwnd(), 0 };
      _TrackMouseEvent( &tme );  
      AfxGetMainWnd()->PostMessage( m_uGlobalHelpTextMessageID, WPARAM(GetDlgCtrlID()), 0 );
      OnMouseEnter();
    }
    break;
  }
  case WM_MOUSELEAVE:
    OnMouseLeave();
    m_bMouseOver = false;
    AfxGetMainWnd()->PostMessage( m_uGlobalHelpTextMessageID, 0, 0 );
    break;
  case WM_KEYDOWN:
  case WM_KEYUP:
  case WM_CHAR:
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
    GetParentOwner()->PostMessage(uMessage, wParam, lParam);
    break;
  }

  return CWnd::WindowProc( uMessage, wParam, lParam );
} // WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam )


/**
 * Gets if the mouse cursor is over the button
 *
 * @return    true if mouse is over button
 */
bool SkinButton::IsMouseOverButton() const
{
  CRect rc;
  GetWindowRect( &rc );

  CPoint point( GetMessagePos() );
  return rc.PtInRect( point );
} // bool IsMouseOverButton() 


/**
 * Sets the current button state
 *
 * @param   eState    new button state
 */ 
void SkinButton::SetButtonState( const SkinButtonStateEnum & eNewState )
{
  if( eNewState != m_eButtonState )
  {
    m_eButtonState = eNewState;
    RedrawWindow();
  }
} // SetButtonState( const SkinButtonState & eState )


/**
 * Set bitmap for the button, initializes brushes, etc. for drawing the button
 * 
 * @param hBitmap   Bitmap handle for button picture
 * @return          Previous bitmap handle (not stored in this implementation, so always NULL)
 */
void SkinButton::SetBitmap( DWORD dwResourceId )
{
  if( m_dwResourceId != dwResourceId )
  {
    m_pButtonImages = new Gdiplus::Bitmap( AfxGetResourceHandle(), MAKEINTRESOURCEW( dwResourceId ) );
    if( m_pButtonImages == NULL )
      throw ex::ResourceImageException( LogRec( "Unable to load button images", "SetBitmap", "SkinButton" ) );

    m_dwResourceId = dwResourceId;
  }

  Invalidate();
} // SetBitmap( DWORD dwResourceId )


/**
 * Sets the check state for the button*
 * 
 * @param   iCheck    new check state
 */
void SkinButton::SetCheck( int4 iCheck )
{
  if( iCheck != m_iCheck )
  {
    switch( m_iCheck = iCheck )
    {
    case 0:
      m_eButtonState = SkinButton::UP;
      break;
    case 1:
    case 2:
      m_eButtonState = SkinButton::DOWN;
      break;
    default:
      break;
    }
    RedrawWindow();
  }
} // SetCheck( int4 iCheck )


/**
 * Returns button toggle state
 * 
 * @param WPARAM    Unused
 * @param LPARAM    Unused
 * @return          Returns toggle state
 */
LRESULT SkinButton::OnGetCheck( WPARAM wParam, LPARAM lParam )
{
  return static_cast< LRESULT >( m_iCheck );
} // OnGetCheck( WPARAM wParam, LPARAM lParam )


/**
 * Sets button toggle state
 * 
 * @param fCheck   Toggle state
 * @param LPARAM   Unused
 * @return         Zero (success)
 */
LRESULT SkinButton::OnSetCheck( WPARAM wParam, LPARAM lParam )
{
  SetCheck( wParam );
  return 0;
} // OnSetCheck( WPARAM wParam, LPARAM lParam )


/**
 * MFC message map
 */
BEGIN_MESSAGE_MAP( SkinAutoCheckButton, SkinButton )
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/**
 * Left mouse click handler; pauses for 25ms to ensure animation (button drawn in down position) will be visible
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse click point
 */
void SkinAutoCheckButton::OnLButtonDown( UINT uFlags, CPoint point )
{
  SetCapture();

  switch( GetCheck() )
  {  
  // button is up
  case 0:
    SetButtonState( SkinButton::DOWN );
    break;
  
  // button is down
  case 1:
  case 2:
    SetButtonState( SkinButton::UP );
    break;
    
  // unknown
  default:
    LogWrn( "Unknown check state", "OnLButtonDown", "SkinAutoCheckButton" );
    break;
  } // switch( m_iPreviousCheckState )
  
} // OnLButtonDown( UINT uFlags, CPoint point )


/**
 * Left mouse click handler; pauses for 25ms to ensure animation (button drawn in down position) will be visible
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse click point
 */
void SkinAutoCheckButton::OnLButtonUp( UINT uFlags, CPoint point)
{
  if( IsMouseOverButton() )
  { 
    switch( GetCheck() )
    {  
    // button is up
    case 0:
      SetCheck( 1 );
      break;
  
    // button is down
    case 1:
      SetCheck( 0 );
      break;
    
    case 2:
      SetCheck( 1 );
      break;
    
    // unknown
    default:
      LogWrn( "Unknown check state", "OnLButtonUp", "SkinAutoCheckButton" );
      break;
    } // switch( m_iPreviousCheckState )

    SetButtonState( SkinButton::HOVER );
    GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), BN_CLICKED ), 0 );
    OnClicked();
    OnChecked();
  }

  ReleaseCapture();
} // OnLButtonUp( UINT uFlags, CPoint point)


/**
 * Called when mouse Enter window
 */
void SkinAutoCheckButton::OnMouseEnter()
{
  SetButtonState( SkinButton::HOVER );
} // OnMouseEnter()


/**
 * Called when mouse leaves window
 */
void SkinAutoCheckButton::OnMouseLeave()
{
  switch( GetCheck() )
  {  
  // button is up
  case 0:
    SetButtonState( SkinButton::UP );
    break;
  
  // button is down or inderteminate
  case 1:
  case 2:
    SetButtonState( SkinButton::DOWN );
    break;
  
  // unknown
  default:
    LogWrn( "Unknown check state", "OnMouseLeave", "SkinAutoCheckButton" );
    break;
  } // switch( m_iPreviousCheckState )

  
  RedrawWindow();

} // OnMouseLeave()


/**
 * Mouse move handler; displays help text if available for button, tracks mouse for mouse leave event
 * 
 * @param uFlags   Control key/mouse key flags
 * @param point    Mouse move point
 */
void SkinAutoCheckButton::OnMouseMove( UINT uFlags, CPoint point ) 
{
  CWnd::OnMouseMove( uFlags, point );

} // OnMouseMove( UINT uFlags, CPoint point ) 


/**
 * Dialog data exchange
 *
 * @param pDX      Dialog data exchange instance.
 * @param iIDC     Dialog control ID of first radio in group.
 * @param iValue   [In/out] n-th radio in group that is the one checked, if any.
 */
void AFXAPI SkinAutoRadioButton::DDX_Radio( CDataExchange* pDX, int iControlId, int & iValue )
{
  CWnd * pWnd = pDX->m_pDlgWnd->GetDlgItem( iControlId );
  uint4 uCtl(0);
  do
  {
    if( pDX->m_bSaveAndValidate )
    {
      if( pWnd->SendMessage( BM_GETCHECK ) == BST_CHECKED )
      {
        iValue = uCtl;
        return;
      }
      else
      {
        iValue = -1;
      }
    }
    else
    {
      pWnd->SendMessage( BM_SETCHECK, ( iValue == uCtl ) ? BST_CHECKED : BST_UNCHECKED );
    }
  }
  while ( ((pWnd = pWnd->GetWindow( GW_HWNDNEXT ) ) != NULL) && ( ( pWnd->GetStyle() & WS_GROUP) != WS_GROUP) && (++uCtl));
} // DDX_Radio( CDataExchange* pDX, int uIDC, int & iValue )


/**
 * Dialog data exchange
 *
 * @param pDX      Dialog data exchange instance.
 * @param uIDC     Dialog control ID of first radio in group.
 * @param iValue   [In/out] n-th radio in group that is the one checked, if any.
 */
void AFXAPI SkinAutoRadioButton::DDX_Radio_IDs(CDataExchange *pDX, int iIDFirst, int &iSelectedID)
{
  CWnd * pWnd = pDX->m_pDlgWnd->GetDlgItem( iIDFirst );
  uint4 uCtl(0);
  do
  {
    if( pDX->m_bSaveAndValidate )
    {
      if( pWnd->SendMessage( BM_GETCHECK ) == BST_CHECKED )
      {
        iSelectedID = pWnd->GetDlgCtrlID();
        return;
      }
      else
      {
        iSelectedID = -1;
      }
    }
    else
    {
      pWnd->SendMessage( BM_SETCHECK, ( iSelectedID == pWnd->GetDlgCtrlID() ) ? BST_CHECKED : BST_UNCHECKED );
    }
  }
  while ( ((pWnd = pWnd->GetWindow( GW_HWNDNEXT ) ) != NULL) && ( ( pWnd->GetStyle() & WS_GROUP) != WS_GROUP) && (++uCtl));
} // DDX_Radio_IDs( CDataExchange* pDX, int iIDFirst, int & iSelectedID )


BEGIN_MESSAGE_MAP( SkinAutoRadioButton, SkinAutoCheckButton )
  ON_MESSAGE( BM_SETCHECK, OnSetCheck )
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/**
 * Sets button toggle state
 */
void SkinAutoRadioButton::OnChecked()
{
  OnSetCheck( GetCheck(), 0 );
} // OnChecked()


/**
 * Sets button toggle state
 * If fCheck == BST_CHECKED, unchecks all other items in group
 * 
 * @param fCheck   Toggle state
 * @param lParam   message's LPARAM value
 * @return         Zero (success)
 */
LRESULT SkinAutoRadioButton::OnSetCheck( WPARAM fCheck, LPARAM lParam)
{
  if ( fCheck != BST_CHECKED )
    return SkinButton::OnSetCheck(fCheck, lParam);

  // Find the first of the group
  CWnd* pWnd = this;
  for (pWnd = this; (pWnd != NULL) && ((pWnd->GetStyle() & WS_GROUP) != WS_GROUP); pWnd = pWnd->GetWindow(GW_HWNDPREV))
  { /* No for-loop body */ }

  //////////////////////////////////////////////////////
  // Set all other buttons of the group to unchecked
  /////////////////////////////////////////////////////
  do
  {
    if (HWND(*pWnd) != HWND(*this))
    {
      pWnd->SendMessage(BM_SETCHECK, WPARAM(BST_UNCHECKED), 0);
    }
  } while ( ((pWnd = pWnd->GetWindow(GW_HWNDNEXT)) != NULL) && ((pWnd->GetStyle() & WS_GROUP) != WS_GROUP) );

  return SkinButton::OnSetCheck(fCheck, lParam);

} // OnSetCheck(WPARAM fCheck, LPARAM lParam)

// Revision History:
// $Log: SkinButton.cpp,v $
// Revision 1.10.2.1  2009/12/11 22:32:32  dongqing
// allow DDX set/get of radio buttons by ID in addition to cardinality
//
// Revision 1.10  2007/03/09 17:03:49  jmeade
// code standards.
//
// Revision 1.9  2007/03/09 15:57:25  jmeade
// code standards.
//
// Revision 1.8  2007/03/09 15:00:11  romy
// fixed the build error
//
// Revision 1.7  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.6  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.5  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.4  2006/02/01 13:56:51  mkontak
// Coding standards
//
// Revision 1.3  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2.4.7  2006/05/22 12:30:33  geconomos
// fixed memory leak (deleting button bitmaps in destructor)
//
// Revision 1.2.4.6  2006/03/16 19:35:56  jmeade
// Issue 4648: Set '&' as a hotkey.
//
// Revision 1.2.4.5  2006/03/15 22:34:54  jmeade
// Don't draw text for BS_BITMAP button styles.
//
// Revision 1.2.4.4  2006/03/13 16:27:22  jmeade
// Header and footer info, comments.
//
// Revision 1.2.4.3  2006/2/28 17:30:13  geconomos
// reworked SkinButton as custom control
//
// Revision 1.2.4.2  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.9.4.2.4.1  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.9.4.2.8.1  2005/06/06 16:36:55  geconomos
// added a hack to ensure that the parent is notified when the button is clicked
//
// Revision 3.9.4.2  2003/03/10 18:43:27  jmeade
// Issue 2841: Skinned auto radio buttons.
//
// Revision 3.9.4.1  2003/02/26 17:10:32  jmeade
// Remove dependency on Viatron resource file by allowing lib user to set default button.
//
// Revision 3.9  2002/09/25 18:59:01  jmeade
// Update window region on button resize.
//
// Revision 3.8  2002/02/01 00:55:34  jmeade
// Clarified code a bit (no real changes).
//
// Revision 3.7  2002/01/20 00:56:45  jmeade
// Ensure main wnd pointers are non-NULL before access.
//
// Revision 3.6  2002/01/10 23:54:19  jmeade
// Reset help text in destructor to ensure it's cleared.
//
// Revision 3.5  2002/01/09 21:02:45  jmeade
// GetBitmap() override.
//
// Revision 3.4  2001/11/23 15:07:27  jmeade
// Default to medium size button.
//
// Revision 3.3  2001/11/21 00:26:28  jmeade
// Removed obsolete variable.
//
// Revision 3.2  2001/11/17 19:06:48  jmeade
// Added back drawing of button labels for non BS_BITMAP styles.
//
// Revision 3.1  2001/11/17 01:50:52  jmeade
// Multi-state button bitmaps
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:00   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:46   ingmar
// Initial revision.
// Revision 1.20  2001/05/17 06:14:31  jmeade
// Allow caller of LoadSkin() function to set the Help Text message ID,
// rather than using the Viatron message
//
// Revision 1.19  2001/05/11 22:36:26  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SkinButton.cpp,v 1.10.2.1 2009/12/11 22:32:32 dongqing Exp $
// $Id: SkinButton.cpp,v 1.10.2.1 2009/12/11 22:32:32 dongqing Exp $
