// $Id: ReportWndBase.C,v 1.9.2.1 2008/05/14 01:29:50 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:gecono@viatronix.com)


// includes
#include "stdafx.h"
#include "viatron.h"
#include "ReportWndBase.h"
#include "Theme.h"
#include "resource.h"


// namespaces
using namespace WinUtils;
using namespace ColonReport;


/**
 * Constructor
 *
 * @param uID      dialog background image resource id
 * @param pParent  parent wnd
 */
ReportWndBase::ReportWndBase( UINT uID, CWnd * pParent ) :
CDialog( uID, pParent ),
m_pDrawBuffer( NULL ),
m_hPatternBrush( NULL ),
m_hBitmap( NULL )
{
} // ReportWndBase( uID, pParent )


/**
 * Destructor
 */
ReportWndBase::~ReportWndBase()
{
  if( m_pDrawBuffer )
  {
    delete m_pDrawBuffer;
    m_pDrawBuffer = NULL;
  }

  if( m_hPatternBrush )
  {
    DeleteObject( m_hPatternBrush );
    m_hPatternBrush = NULL;
  }

  if( m_hBitmap )
  {
    DeleteObject( m_hBitmap );
    m_hBitmap = NULL;
  }

} // ~ReportWndBase()


/**
 * Called by the framework to exchange and validate dialog data
 *
 * @param   pDX   CDataExchange instance
 */
void ReportWndBase::DoDataExchange( CDataExchange * pDX )
{
  CDialog::DoDataExchange(pDX);
} // DoDataExchange( CDataExchange * pDX )


/// mfc message map
BEGIN_MESSAGE_MAP(ReportWndBase, CDialog)
  ON_WM_PAINT()
  ON_WM_CTLCOLOR()
  ON_WM_SIZE()
  ON_WM_SETCURSOR()
  ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/**
 * WM_ERASEBKGND handler
 *
 * @param pDC device context for updating background
 *
 * @return TRUE if handled
 */
BOOL ReportWndBase::OnEraseBkgnd( CDC * pDC )
{
  return TRUE;
} // OnEraseBkgnd( pDC )


/**
 * WM_INITDIALOG handler
 *
 * @return non-zero to set focus to the first child control
 */
BOOL ReportWndBase::OnInitDialog()
{
  CDialog::OnInitDialog();

  VERIFY(m_fontName.CreateFont(
    32, 
    0, 
    0, 
    0, 
    FW_NORMAL, 
    FALSE, 
    FALSE, 
    0, 
    ANSI_CHARSET, 
    OUT_DEFAULT_PRECIS, 
    CLIP_DEFAULT_PRECIS, 
    DEFAULT_QUALITY, 
    DEFAULT_PITCH | FF_SWISS, 
    "Arial"));
  
  VERIFY(m_fontSmall.CreateFont(
    4,
    0,
    0,
    0,
    FW_NORMAL, 
    FALSE, 
    FALSE, 
    0, 
    ANSI_CHARSET, 
    OUT_DEFAULT_PRECIS, 
    CLIP_DEFAULT_PRECIS, 
    DEFAULT_QUALITY, 
    DEFAULT_PITCH | FF_SWISS, 
    "Arial"));
  
  VERIFY(m_fontNormal.CreateFont(
    16,
    0,
    0,
    0,
    FW_NORMAL,
    FALSE,
    FALSE,
    0,
    ANSI_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_SWISS,
    "Arial"));
  
  VERIFY(m_fontBold.CreateFont(
    14,
    0,
    0,
    0,
    FW_BOLD,
    FALSE,
    FALSE,
    0,
    ANSI_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_SWISS,
    "Arial"));

  
  VERIFY(m_fontUnderline.CreateFont(
    10,
    0,
    0,
    0,
    FW_NORMAL,
    FALSE,
    TRUE,
    0,
    ANSI_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_SWISS,
    "Arial"));


  // Set fonts for  child windows
  SendMessageToDescendants(WM_SETFONT, reinterpret_cast<WPARAM>(m_fontNormal.GetSafeHandle()));

  CRect rcClient;GetClientRect(&rcClient);//(WinUtils::GetClientRect(this));
  
  CRgn rgn;
  rgn.CreateRoundRectRgn(0, 0, rcClient.right, rcClient.bottom, 20, 20);
  SetWindowRgn(reinterpret_cast<HRGN>(rgn.Detach()),TRUE);

  const int4 iGray = 172;
  m_clrBack = RGB( iGray, iGray, iGray );
  m_bkEdit.CreateSolidBrush( m_clrBack );
 
  return TRUE;

} // OnInitDialog()


/**
 * WM_SIZE handler, Called when window is resized
 *
 * @param UType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void ReportWndBase::OnSize( UINT uType, int cx, int cy )
{
  CDialog::OnSize( uType, cx, cy );

  // free the previous background image
  if( m_pDrawBuffer != NULL )
  {
    delete m_pDrawBuffer;
    m_pDrawBuffer = NULL;
  }

  // create new drawing buffer based on new window size
  m_pDrawBuffer = new Gdiplus::Bitmap( cx+20, cy+20, PixelFormat24bppRGB );
  {
    Gdiplus::Graphics gfx( m_pDrawBuffer );
    gfx.SetInterpolationMode( Gdiplus::InterpolationModeHighQualityBicubic );
    gfx.DrawImage( Theme::ReportingBkgnd, -5, -5, cx+25, cy+25 );
  }
  
  if( m_hBitmap )
  {
    DeleteObject( m_hBitmap );
    m_hBitmap = NULL;
  }
  m_pDrawBuffer->GetHBITMAP( Gdiplus::Color::White, &m_hBitmap );

  if( m_hPatternBrush )
  {
    DeleteObject( m_hPatternBrush );
    m_hPatternBrush = NULL;
  }
  m_hPatternBrush = CreatePatternBrush( m_hBitmap );

} // OnSize( UINT uType, int cx, int cy )


/**
 * WM_PAINT handler
 */
void ReportWndBase::OnPaint()
{
  CPaintDC dc( this );

  // draw the buffer to the screen
  if( m_pDrawBuffer != NULL )
  {
    Gdiplus::Graphics gfx( dc.GetSafeHdc() );
    gfx.DrawImage( m_pDrawBuffer, 0, 0, m_pDrawBuffer->GetWidth(), m_pDrawBuffer->GetHeight() );
  } 
} // OnPaint()


/**
 * WM_CTLCOLOR handler
 *
 * @param pDC         display context for child window
 * @param pWnd        control asking for color
 * @param uCtlColor   type of control
 * @return            brush for paint background of control
 */
HBRUSH ReportWndBase::OnCtlColor( CDC * pDC, CWnd * pWnd, UINT uCtlColor ) 
{

  HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, uCtlColor );
  
  if( uCtlColor == CTLCOLOR_EDIT )
  {
    pDC->SetBkColor(m_clrBack);
    return m_bkEdit;
  }

  pDC->SetBkMode(TRANSPARENT);
  if( uCtlColor == CTLCOLOR_STATIC )
  {
    CRect rc; pWnd->GetWindowRect( &rc );
    ScreenToClient( &rc );
    pDC->SetBrushOrg( -rc.left, -rc.top );
    return m_hPatternBrush;
  }
  return hbr;

} // OnCtlColor( pDC, pWnd, uCtlColor )


/**
 * Get the background image of the specified child control
 *
 * @param uID     id of child window
 * @return        HBITMAP of background image
 */
HBITMAP ReportWndBase::GetChildBackground( UINT uID )
{
  CWnd * pWnd( GetDlgItem( uID ) );

  CRect rc; 
  pWnd->GetWindowRect(rc);
  ScreenToClient( &rc );
  
  Gdiplus::Bitmap destination( rc.Width(), rc.Height(), PixelFormat24bppRGB );
  
  Gdiplus::Graphics gfx( &destination );

  gfx.DrawImage( m_pDrawBuffer, 0, 0, rc.left, rc.top, rc.Width(), rc.Height(), Gdiplus::UnitPixel );

  HBITMAP hBackground;
  destination.GetHBITMAP( Gdiplus::Color::Black, &hBackground );
  return hBackground;
} // GetChildBackground( uID )


/**
 * Create a sping edit control
 *
 * @param uID           id of control   
 * @param wndSpin       will contain reference to subclass upon return
 * @param fMin          minimum spin value
 * @param fMax          maximum spin value
 * @param iInitial      initial value
 * @param fStep         step size
 * @param fPrecs        precision
 * @param bWrap         indicates if spin should wrap around when maximum is reached
 */
void ReportWndBase::CreateSpinEdit(const UINT uID, ReportSpinEdit& wndSpin, const float4 fMin, const float4 fMax,
                                   const float4 fInitial /* = 0.0F */, const float4 fStep /* = 1.0F */,
                                   const float4 fPrecs /* = 0.0F */, const bool bWrap /* = false */)
{

  CWnd * pWnd = GetDlgItem(uID);
  ASSERT(pWnd);

  CRect rcSpinEdit;
  pWnd->GetWindowRect(rcSpinEdit);
  ScreenToClient(rcSpinEdit);

  pWnd->DestroyWindow();

  wndSpin.Create(NULL, "", WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPSIBLINGS|WS_BORDER, rcSpinEdit, this, uID);
  wndSpin.SetFont(&m_fontNormal);
  wndSpin.SetDigits(fPrecs);
  wndSpin.SetRange(fMin, fMax, bWrap);
  wndSpin.SetStepSize(fStep);
  wndSpin.SetValue(fInitial);
  wndSpin.SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);  
  wndSpin.SetColor(m_clrBack);

} // CreateSpinEdit( ... )


/**
 * Displays the the specified reporting error
 *
 * @param uErrorIDS     resource of general error message
 * @param hr            HRESULT return from reporting library
 */   
void ReportWndBase::DisplayReportError(uint4 uErrorIDS, HRESULT hr)
{
  CString s, sfmt;
  sfmt.Format(LoadResourceString(IDS_RPTERR_FORMAT), LoadResourceString(uErrorIDS));
  s.Format("%s(0x%08X)", sfmt, hr);

  AfxMessageBox(s, MB_ICONSTOP|MB_OK);
} // DisplayReportError( uErrorIDS, hr )


/**
 * Selects an item in the specified combo box matching the specified data
 *
 * @param wndCombo  combo box 
 * @param dwData    data to match
 * @return          TRUE if item was found
 */
BOOL ReportWndBase::SelectComboItem( CComboBox & wndCombo, DWORD dwData )
{
  for(int4 iIndex(0); iIndex < wndCombo.GetCount(); iIndex++)
  {
    if(wndCombo.GetItemData(iIndex) == dwData)
    {
      wndCombo.SetCurSel(iIndex);
      return TRUE;
    }
  }

  return FALSE;

} // SelectComboItem( wndCombo, dwData )


/**
 * WM_SETCURSOR handler
 *    
 * @param   pWnd        window that contains the cursor
 * @param   uHitTest    hit test determines the cursor's location
 * @param   uMessage    specifies the mouse message number
 * @return             nonzero to halt further processing, or 0 to continue
 */
BOOL ReportWndBase::OnSetCursor( CWnd * pWnd, UINT uHitTest, UINT uMessage ) 
{
  SetCursor(LoadCursor(NULL, IDC_ARROW));
  return TRUE;
} // OnSetCursor( pWnd, uHitTest, uMessage )


/**
 * override this to avoid further calling of EndDialog()
 */
void ReportWndBase::OnOK() 
{
  NextDlgCtrl();

} // OnOK()


// $Log: ReportWndBase.C,v $
// Revision 1.9.2.1  2008/05/14 01:29:50  jmeade
// french version.
//
// Revision 1.9  2007/03/09 15:59:11  jmeade
// code standards.
//
// Revision 1.8  2006/01/31 15:44:52  geconomos
// updated for code review
//
// Revision 1.7  2006/01/31 14:02:31  frank
// code review
//
// Revision 1.6  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5  2005/11/07 20:38:18  geconomos
// modified headers
//
// Revision 1.4.2.1  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.4  2005/10/04 02:10:32  geconomos
// reworked drawing of background image
//
// Revision 1.3  2005/09/13 17:27:19  jmeade
// ColonReport namespace.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.1.10.1.12.2  2005/06/22 19:47:21  geconomos
// modified GetControlBackground() to use updates to background brush
//
// Revision 3.1.10.1.12.1  2005/06/17 13:15:10  geconomos
// using new BackgroundBrush class
//
// Revision 3.1.10.1  2003/04/09 21:50:32  jmeade
// Coding standards.
//
// Revision 3.1  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.0.4.1  2002/02/26 22:37:39  binli
// override OnOK() to avoid unnecessary pop-down of dialog view.
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
//    Rev 2.0   Sep 16 2001 23:41:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:50   ingmar
// Initial revision.
// Revision 1.10  2001/08/21 18:24:19  binli
// no message
//
// Revision 1.9  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.8  2001/02/05 16:24:17  jeff
// DisplayReportError(): static and public, so that it's accessible by non-derived (but still
// report-related) classes; corrected parameter to string format function
//
// Revision 1.7  2001/02/02 08:06:33  jeff
// Moving strings to resource table
//
// Revision 1.6  2001/02/01 17:07:03  geconomos
// Added comment headers and footers
//
// Revision 1.5  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportWndBase.C,v 1.9.2.1 2008/05/14 01:29:50 jmeade Exp $
// $Id: ReportWndBase.C,v 1.9.2.1 2008/05/14 01:29:50 jmeade Exp $
