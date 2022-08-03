// $Id: StatusIndicator.cpp $
//
// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StatusIndicator.cpp  
//
// Owner(s): Dongqing Chen  dongqing@viatronix.net

#include "stdafx.h"
#include "StatusIndicator.h"
#include "GdiplusUtilities.h"
#include "Theme.h"
#include "Resource.h"
#include "WinUtils.h"
#include "ApplicationController.h"
#include "vcUtils.h"


// namespaces
using namespace WinUtils;


// StatusIndicator: status indicator to be displayed at the bottom status bar of the application

IMPLEMENT_DYNAMIC(StatusIndicator, CWnd)

/**
 * default constructor
 */
StatusIndicator::StatusIndicator() :
  m_linkedImage( AfxGetResourceHandle(), MAKEINTRESOURCEW( IDB_LINKED_UNLINKED )),
  m_reportCompletedImage( AfxGetResourceHandle(), MAKEINTRESOURCEW( IDB_CHECK_MARK )),
  m_bShowLinked( true )
{
} // StatusIndicator()


/**
 * destructor
 */
StatusIndicator::~StatusIndicator()
{
} // ~StatusIndicator()


/// windows message map
BEGIN_MESSAGE_MAP( StatusIndicator, CWnd )
  ON_WM_CREATE()
  ON_WM_ERASEBKGND()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_DESTROY()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/**
 * Called when the window is being created.
 *
 * @param lpCreateStruct  CREATESTRUCT structure that contains information about the CWnd object being created
 * @return                0 to continue creation, -1 to destroy the window  
 */
int StatusIndicator::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
  if( CWnd::OnCreate( lpCreateStruct ) == -1 )
    return -1;

  Colon::ApplicationController::Connect( dynamic_cast< IObserver< Colon::DatasetsLockedArgs > * >( this ));
  Colon::ApplicationController::Connect( dynamic_cast< IObserver< Colon::ReportArgs > * >( this ));

  return 0;
} // OnCreate()


/**
 * Called when the window is being destroyed
 */
void StatusIndicator::OnDestroy()
{
  Colon::ApplicationController::Disconnect( dynamic_cast< IObserver< Colon::DatasetsLockedArgs > * >( this ));
  Colon::ApplicationController::Disconnect( dynamic_cast< IObserver< Colon::ReportArgs > * >( this ));
} // OnDestroy()


/**
 * OnPaint message handler
 */
void StatusIndicator::OnPaint() 
{
  CPaintDC dc( this );

  CRect iSize( WinUtils::GetWindowRect( this ));
  int4 iRight( iSize.Width() - 20 );

  Gdiplus::Graphics gfx( GetSafeHwnd() );
	/// draw the link indicator icon unless it is a need
  if( ReaderLib::rdrGlobal.IsDualDataLoaded() && m_bShowLinked )
  {
    int4 iWidth = m_linkedImage.GetWidth();
    int4 iHeight = static_cast< int4 >(  m_linkedImage.GetHeight() / 2.0F  + 0.5F );
    int4 y = ( Utilities::Datasets::IsLocked() ? 0 : 1 ) * iHeight;

    Gdiplus::Color color;
    m_linkedImage.GetPixel( 0, 0, &color );

    Gdiplus::ImageAttributes imAtt;
    imAtt.SetColorKey( color, color, Gdiplus::ColorAdjustTypeBitmap );

    Gdiplus::Rect dst( iRight - iWidth, ( iSize.Height() - iHeight ) / 2, iWidth, iHeight );
    gfx.DrawImage( &m_linkedImage, dst, 0, y, iWidth, iHeight, Gdiplus::UnitPixel, &imAtt );

    iRight -= ( iWidth + 10 );
  } // if (m_bShowLinked)

	/// draw the report complete indicator icon unless it is a need
  if( Utilities::Reporting::IsComplete() )
  {
    int4 iWidth = m_reportCompletedImage.GetWidth();
    int4 iHeight = m_reportCompletedImage.GetHeight();
    int4 y = 0;

    Gdiplus::Color color;
    m_reportCompletedImage.GetPixel( 0, 0, &color );

    Gdiplus::ImageAttributes imAtt;
    imAtt.SetColorKey( color, color, Gdiplus::ColorAdjustTypeBitmap );

    CRect iSize( WinUtils::GetWindowRect( this ));

    Gdiplus::Rect dst( iRight - iWidth, ( iSize.Height() - iHeight ) / 2, iWidth, iHeight );
    gfx.DrawImage( &m_reportCompletedImage, dst, 0, y, iWidth, iHeight, Gdiplus::UnitPixel, &imAtt );

    iRight -= ( iWidth + 10 );
  } // if (report )

} // OnPaint()


/**
 * WM_ERASEBKGND handler, called when background needs erasing
 *
 * @param   pdc     Device context for window
 * @return          Non-zero to indicate NOT to erase, zero to let framework erase window
 */
BOOL StatusIndicator::OnEraseBkgnd( CDC * pdc )
{
  return FALSE;
} // OnEraseBkgnd( CDC * pdc )


/**
 * WM_SIZE handler, Called when window is resized
 *
 * @param uType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void StatusIndicator::OnSize( UINT uType, int iCX, int iCY )
{
  CWnd::OnSize( uType, iCX, iCY );
} // OnSize( UINT uType, int cx, int cy )


/**
 * Left button down handler to toggle the link state indicator status
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void StatusIndicator::OnLButtonDown(UINT uFlags, CPoint point) 
{
  CWnd::OnLButtonDown( uFlags, point );

	const bool b = m_bShowLinked;
	if( b )
	{
		CPoint point; 
		GetCursorPos( &point );
		ScreenToClient( &point );

		CRect bounds( WinUtils::GetWindowRect( this ));
		ScreenToClient( &bounds );

		int4 iRight( bounds.Width() - 20 );

		CRect linkBounds( iRight - m_linkedImage.GetWidth(), 0, iRight, bounds.Height() );
		if( linkBounds.PtInRect( point ) )
		{
			Utilities::Datasets::Lock( !ReaderLib::rdrGlobal.m_bSPLocked );
		} // if mouse cursor is on top of the icon
	} // if
} // OnLButtonDown


/**
 *  display tool's help tip while mouse hovering on top of the icon
 *
 *  @return the help tip string
 */
std::string StatusIndicator::GetToolTip()
{
  CPoint point; 
  GetCursorPos( &point );
  ScreenToClient( &point );

  CRect bounds( WinUtils::GetWindowRect( this ));
  ScreenToClient( &bounds );

  int4 iRight( bounds.Width() - 20 );

  std::string sToolTip;
  // for dataset link state indicator
  if( ReaderLib::rdrGlobal.IsDualDataLoaded() && m_bShowLinked )
  {
    CRect linkBounds( iRight - m_linkedImage.GetWidth(), 0, iRight, bounds.Height() );
    if( linkBounds.PtInRect( point ))
      sToolTip = LoadResourceString(( Utilities::Datasets::IsLocked() ) ? IDS_STATUS_INDICATOR_LINKED : IDS_STATUS_INDICATOR_UNLINKED );

    iRight -= ( m_linkedImage.GetWidth() + 10 );
  } // if

	/// for report complete indicator
  if( Utilities::Reporting::IsComplete() )
  {
    CRect completedBounds( iRight - m_reportCompletedImage.GetWidth(), 0, iRight, bounds.Height() );
    if( completedBounds.PtInRect( point ))
      sToolTip = LoadResourceString( IDS_REPORT_COMPLETED_STATUS );

    iRight -= ( m_reportCompletedImage.GetWidth() + 10 );
  } // if
	  
  return sToolTip;
} // GetToolTip()


/**
 * Called when the registered observable object is changed 
 *
 * @param       data          observable data
 */
void StatusIndicator::OnNotified( const Colon::DatasetsLockedArgs & args )
{
  m_bShowLinked = args.IsEnabled();
  GetParent()->Invalidate();
} // OnNotified( data )



/**
 * Called when the registered observable object is changed 
 *
 * @param       data          observable data
 */
void StatusIndicator::OnNotified( const Colon::ReportArgs & args )
{
  if( args.IsCompleted() )
  {
    GetParent()->Invalidate();
    GetParent()->UpdateWindow();
  }
} // OnNotified( data )


/**
 * Called when the visibility of the link indicator is changed 
 *
 * @param  bShowLinked
 */
void StatusIndicator::SetLinkIconVisibility( const bool & bShowLinked )
{
  m_bShowLinked = bShowLinked;
  GetParent()->Invalidate();
} // SetLinkIconVisibility()