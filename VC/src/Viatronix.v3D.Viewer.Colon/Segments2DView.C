// $Id: Segments2DView.C,v 1.10.2.1 2009/12/09 20:22:48 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Segments2DView.C
// Owner(s): Kevin Kreeger   kevin@viatronix.com

#include "stdafx.h"

#include "Segments2DView.h"
#include "NavigationView.h"
#include "SliceChildView.h"
#include "OverView.h"
#include "V1KMessages.h"
#include "Plane.h"
#include "Segment2DChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonViews::Layouts;
using namespace ReaderLib;

/////////////////////////////////////////////////////////////////////////////
// Segments2DViewFrame

IMPLEMENT_DYNCREATE(Segments2DLayoutFrame, CMDIChildWnd)

/**
 * constructor
 */
Segments2DLayoutFrame::Segments2DLayoutFrame() : ChildLayoutFrame(new Segments2DLayout)
{
} // constructor

//{{AFX_MSG_MAP(Segments2DLayoutFrame)
//}}AFX_MSG_MAP

//////////////////////////////////////////////////////////////////////
// Segments2DLayout Class

/// Positions of views and controls in vectors
enum SegmentLayoutVectorPosEnum { eSeg2D_SEGMENT_2DVIEW = 2 };

/**
 * Constructor
 */
Segments2DLayout::Segments2DLayout() : ColonViatronixWnd()
{
} // Constructor

BEGIN_MESSAGE_MAP(Segments2DLayout, ColonViatronixWnd)
  //{{AFX_MSG_MAP(Segments2DLayout)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Keyboard event handler: key pressed
 * @param uChar     key pressed
 * @param uRepCnt   repeat count
 * @param uFlags    current-state flage
 */
void Segments2DLayout::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  switch (uChar)
  {
    case 'N': 
    {
      if(rdrGlobal.m_pCurrDataset->m_currentSegment<rdrGlobal.m_pCurrDataset->m_straightSegmentNodes.size()-1)
      rdrGlobal.m_pCurrDataset->m_currentSegment++;
      break;
    } 
    case 'P': 
    {
      if(rdrGlobal.m_pCurrDataset->m_currentSegment>0)
      rdrGlobal.m_pCurrDataset->m_currentSegment--;
      break;
    } 
    case 'M': 
    {
      if(rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold<250)
      rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold++;
      rdrGlobal.m_pCurrDataset->CreateSkeletonSegments();
      break;
    } 
    case 'L': 
    {
      if(rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold>0)
      rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold--;
      rdrGlobal.m_pCurrDataset->CreateSkeletonSegments();
      break;
    } 
    default:
    {
      // Let the base class do all the dirty work
      ColonViatronixWnd::OnKeyDown(uChar, uRepCnt, uFlags);
      return;
    }
  } // end of switch

  // Set the viewpoint to be viewing along the axis
  uint4 uIndex1 = rdrGlobal.m_pCurrDataset->m_straightSegmentNodes[rdrGlobal.m_pCurrDataset->m_currentSegment];
  uint4 uIndex2 = rdrGlobal.m_pCurrDataset->m_straightSegmentNodes[rdrGlobal.m_pCurrDataset->m_currentSegment+1];
  Point<float4> Point1 = rdrGlobal.m_pCurrDataset->m_joinedSkeleton.GetWorldPos(uIndex1);
  Point<float4> Point2 = rdrGlobal.m_pCurrDataset->m_joinedSkeleton.GetWorldPos(uIndex2);
  Normal<float4> nViewDirection(Point2-Point1);
  Plane rotationPlane(Point1,nViewDirection);
  Normal<float4> nUp = Normal<float4>(rotationPlane.GetProjectionParallelToPlane(Vector<float4>(rdrGlobal.m_pCurrDataset->m_joinedSkeleton.GetSkelNode(uIndex1).m_vWorldUp)));
  rdrGlobal.m_pCurrDataset->m_submarine.SetView(Point1, nViewDirection, nUp);

  // re-render all images
  rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  Invalidate(FALSE);  

  Segment2DChildView& rView = ((Segment2DChildView&)(GetChildViewByPos(eSeg2D_SEGMENT_2DVIEW)));
  rView.m_2DsegmentViewer.ResetAngle();
  rView.m_bAutoRotate = true;

  GLRenderViews();
} // OnKeyDown


/**
 * renders views in the layout
 */
void Segments2DLayout::GLRenderViews()
{
  Segment2DChildView& rView = ((Segment2DChildView&)(GetChildViewByPos(eSeg2D_SEGMENT_2DVIEW)));
  if (GetCaptureViewPtr() != NULL)
  {
    GLRenderView( *GetCaptureViewPtr() );
  }
  else if (rView.m_bAutoRotate)
  {
    GLRenderView( eSeg2D_SEGMENT_2DVIEW ); // 2D Segment View
  } 
  else
  {
    ColonViatronixWnd::GLRenderViews();
  }
  return;
} // GLRenderViews()


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void Segments2DLayout::OnSize( UINT uType, int iCX, int iCY ) 
{
  ColonViatronixWnd::OnSize(uType, iCX, iCY);

  if ( iCX < 1 || iCY < 1 )
    return;

  //
  // custom resizing code
  //

  // layout the controls
  const int4 iGap = LAYOUT_GAP;
  const int4 iControlsWidth = 330;
  const int4 iControlsHeight = 190;
  const int4 iControlsX = iGap;
  const int4 iControlsY = 150;
  GetDlgItem( IDC_TEMPLATE_CONTROLS )->SetWindowPos( NULL, iControlsX, iControlsY, iControlsWidth, iControlsHeight, SWP_NOZORDER );

  // layout the main segment view
  const int4 iScrollbarHeight = LAYOUT_SCROLLBAR_THICKNESS;
  const int4 iSegmentWidth = iCX - 2 * iGap;
  const int4 iSegmentHeight = iCY - ( 2 * iGap + iControlsHeight + iControlsY + iScrollbarHeight );
  const int4 iSegmentY = iControlsY + iControlsHeight + iGap;
  GetDlgItem( IDC_TEMPLATE_SLICEVIEW_SUPINE )->SetWindowPos( NULL, iGap, iSegmentY, iSegmentWidth, iSegmentHeight, SWP_NOZORDER );
  const int4 iScrollbarY = iSegmentY + iSegmentHeight;
  GetDlgItem( IDC_CUSTOM1 )->SetWindowPos( NULL, iGap, iScrollbarY, iSegmentWidth, iScrollbarHeight, SWP_NOZORDER );

  // layout the endoscopic view and overview
  const int4 iFullWidth = iCX - iGap - iControlsWidth;
  const int4 iPossibleWidth = ( iFullWidth - 3 * iGap ) / 2;
  const int4 iBuddyHeight = LAYOUT_BUDDY_HEIGHT;
  const int4 iPossibleHeight = iSegmentY - 2 * iGap - iBuddyHeight;
  const int4 iSelectedSize = min( iPossibleWidth, iPossibleHeight );
  const int4 iBigGap = ( iFullWidth - 2 * iSelectedSize ) / 3;
  const int4 iOverviewX = iGap + iControlsWidth + iBigGap;
  const int4 iEndoX = iOverviewX + iSelectedSize + iBigGap;
  // put the endoscopic first because it is closer to the controls (moving over the overview disables them so we put that out of the way)
  GetDlgItem( IDC_TEMPLATE_ENDOSCOPIC_SUPINE )->SetWindowPos( NULL, iOverviewX, iGap, iSelectedSize, iSelectedSize, SWP_NOZORDER );
  GetDlgItem( IDC_TEMPLATE_OVERVIEW_SUPINE )->SetWindowPos( NULL, iEndoX, iGap, iSelectedSize, iSelectedSize, SWP_NOZORDER );

} // OnSize( UINT uType, int iCX, int iCY ) 


// $Log: Segments2DView.C,v $
// Revision 1.10.2.1  2009/12/09 20:22:48  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.10  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.9  2007/02/06 18:58:18  dongqing
// Set back the UD segments go through the last segment
// for 'N' key down
//
// Revision 1.8  2006/11/16 19:17:01  dongqing
// 'N' key stroke ends at # of segments-2 instead of # of segments-1
//
// Revision 1.7  2006/10/12 20:48:58  jmeade
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
// Revision 1.11.2.5.2.2  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.11.2.5.2.1  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.11.2.5.4.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.11.2.5  2005/02/15 19:58:46  jmeade
// merge from vc1.4 branch.
//
// Revision 1.11.2.4.2.1  2004/11/18 02:02:52  jmeade
// Render only captured view when appropriate.
//
// Revision 1.11.2.4  2003/02/27 03:22:03  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.11.2.3  2003/02/25 17:55:27  jmeade
// Enabling recognition via Visual Studio classwizard.
//
// Revision 1.11.2.2  2003/02/22 12:08:44  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.11.2.1  2003/02/21 18:05:37  jmeade
// Line measurement mode same for both 2D and 3D subviews in this view window.
//
// Revision 1.11  2003/01/24 20:26:25  jmeade
// Plane is no longer a template class.
//
// Revision 1.10  2003/01/22 22:34:13  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.9  2002/11/27 22:27:54  jmeade
// Reverse of paradigm for popup menus:  assume show popup menu on right-click in childview,
//  have view window instances validate/refuse popup as necessary; Cleanup of unneeded code.
//
// Revision 1.8  2002/11/24 17:41:53  kevin
// Renamed ProjectOntoPlane
//
// Revision 1.7  2002/11/15 00:34:20  jmeade
// No hard-coding of # of child views.
//
// Revision 1.6  2002/11/11 16:17:42  kevin
// Initial version of cylindrical -- still needs some polishing
//
// Revision 1.5  2002/08/28 19:31:26  kevin
// Lots of updates to segmental viewing
//
// Revision 1.4  2002/08/27 18:28:49  jmeade
// Class wizard specification.
//
// Revision 1.3  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.1  2002/08/15 15:54:23  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Segments2DView.C,v 1.10.2.1 2009/12/09 20:22:48 dongqing Exp $
// $Id: Segments2DView.C,v 1.10.2.1 2009/12/09 20:22:48 dongqing Exp $
