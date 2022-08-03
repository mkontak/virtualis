// $Id: UDSegmentsColonWnd.C,v 1.4.2.1 2009/12/09 20:25:21 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Kevin Kreeger (kevin@viatronix.com)


// includes
#include "stdafx.h"
#include "UDSegmentsColonWnd.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "OverView.h"
#include "V1KMessages.h"
#include "Plane.h"
//#include "UDSegmentsChildView.h"


// defines
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// namespaces
using namespace ColonViews::Layouts;
using namespace ReaderLib;


/////////////////////////////////////////////////////////////////////////////
// UDSegmentsLayoutFrame

IMPLEMENT_DYNCREATE(UDSegmentsLayoutFrame, CMDIChildWnd)


/**
 * constructor
 */
UDSegmentsLayoutFrame::UDSegmentsLayoutFrame() : ChildLayoutFrame(new UDSegmentsLayout)
{
} // constructor

//{{AFX_MSG_MAP(UDSegmentsLayoutFrame)
//}}AFX_MSG_MAP

//////////////////////////////////////////////////////////////////////
// UDSegmentsLayout Class


/**
 * constructor
 */
UDSegmentsLayout::UDSegmentsLayout() : ColonViatronixWnd()
{
} // Constructor


BEGIN_MESSAGE_MAP(UDSegmentsLayout, ColonViatronixWnd)
  //{{AFX_MSG_MAP(UDSegmentsLayout)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WM_APP_CHILD_VIEW_MSG, OnAppChildViewMessage)
END_MESSAGE_MAP()


/**
 * Keyboard event handler: key pressed
 *
 * @param uChar     Keyboard character identifier
 * @param uRepCnt   Repeat count (the number of times the keystroke is repeated as a result of the user holding down the key)
 * @param uFlags    Key codes, see documentation for WM_KEYDOWN
 */
void UDSegmentsLayout::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
} // OnKeyDown


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void UDSegmentsLayout::OnSize( UINT uType, int iCX, int iCY ) 
{
  ColonViatronixWnd::OnSize(uType, iCX, iCY);
  CalculateViewRects( iCX, iCY );

  if ( iCX < 1 || iCY < 1 )
    return;
  return;
  //
  // custom resizing code
  //

  // layout the main segment view
  const int4 iGap = LAYOUT_GAP;
  const int4 iScrollbarHeight = LAYOUT_SCROLLBAR_THICKNESS;
  const int4 iSegmentWidth = iCX - 2 * iGap;
  const int4 iSegmentHeight = iCY / 2 - 1.5 * iGap - iScrollbarHeight;
  const int4 iSegmentY = iCY / 2 + iGap / 2;
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW_SUPINE )->SetWindowPos( NULL, iGap, iSegmentY, iSegmentWidth, iSegmentHeight, SWP_NOZORDER );
  const int4 iScrollbarY = iSegmentY + iSegmentHeight;
  GetDlgItem( IDC_CUSTOM1 )->SetWindowPos( NULL, iGap, iScrollbarY, iSegmentWidth, iScrollbarHeight, SWP_NOZORDER );

  // layout the endoscopic view, sliceview, and overview
  const int4 iPossibleWidth = ( iCX - 4 * iGap ) / 3;
  const int4 iBuddyHeight = LAYOUT_BUDDY_HEIGHT;
  const int4 iPossibleHeight = iCY / 2 - 1.5 * iGap - iBuddyHeight;
  const int4 iSelectedSize = min( iPossibleWidth, iPossibleHeight );
  const int4 iBigGap = ( iCX - 3 * iSelectedSize ) / 4;
  const int4 iSliceX = iBigGap + iSelectedSize + iBigGap;
  const int4 iOverviewX = iSliceX + iSelectedSize + iBigGap;
  // put the endoscopic first because it is closer to the controls (moving over the overview disables them so we put that out of the way)
  GetDlgItem( IDC_TEMPLATE_ENDOSCOPIC_SUPINE )->SetWindowPos( NULL, iBigGap, iGap, iSelectedSize, iSelectedSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW2 )->SetWindowPos( NULL, iSliceX, iGap, iSelectedSize, iSelectedSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_OVERVIEW_SUPINE )->SetWindowPos( NULL, iOverviewX, iGap, iSelectedSize, iSelectedSize, SWP_NOZORDER );

} // OnSize( UINT uType, int iCX, int iCY ) 


// $Log: UDSegmentsColonWnd.C,v $
// Revision 1.4.2.1  2009/12/09 20:25:21  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.4  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.3  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.2  2007/01/03 05:48:20  jmeade
// used default window sizing to get window up and running.
//
// Revision 1.1  2007/01/03 05:34:42  jmeade
// future ud segments layout.
//
// Revision 1.8  2006/11/16 19:17:01  dongqing
// 'N' key stroke ends at # of segments-2 instead of # of segments-1
//
// Revision 1.7  2006/10/12 20:48:59  jmeade
// Issue 5048: default text colour for layouts.
//
// Revision 1.6  2006/09/19 16:12:11  frank
// fixing up the experimental layouts
//
// Revision 1.5  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.4  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.5.2.4.4.1  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.5.2.4  2003/02/27 03:22:03  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.5.2.3  2003/02/25 17:54:46  jmeade
// Enabling recognition via Visual Studio classwizard.
//
// Revision 1.5.2.2  2003/02/22 12:08:44  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.5.2.1  2003/02/21 18:05:37  jmeade
// Line measurement mode same for both 2D and 3D subviews in this view window.
//
// Revision 1.5  2003/01/24 20:26:25  jmeade
// Plane is no longer a template class.
//
// Revision 1.4  2003/01/22 22:34:38  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.3  2002/11/24 17:41:38  kevin
// Renamed ProjectOntoPlane
//
// Revision 1.2  2002/11/15 00:34:20  jmeade
// No hard-coding of # of child views.
//
// Revision 1.1  2002/11/11 16:17:01  kevin
// Initial version of cylindrical -- still needs some polishing
//
