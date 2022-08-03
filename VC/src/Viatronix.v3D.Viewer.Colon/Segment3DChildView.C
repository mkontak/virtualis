// $Id: Segment3DChildView.C,v 1.26.2.16 2010/09/30 16:29:07 dongqing Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Kevin Kreeger    kevin@viatronix.net


// includes
#include "stdafx.h"
#include "Segment3DChildView.h"
#include "MainAppFrame.h"
#include "ReaderGlobal.h"
#include "ViatronStudy.h"
#include "resource.h"
#include <sstream>
#include "RenderUtil.h"
#include "VCRenderUtils.h"
#include "SliceChildView.h"
#include "Quadrilateral.h"
#include "vxPlaneOblique.h"
#include "Plane.h"

// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.26.2.16 $"


// namespaces
using namespace WinUtils;
using namespace ReaderLib;
using namespace vxCAD;


const uint4 iSetScrollMessageID(SliceScrollBar::m_uScrollMessageID = WM_APP_CHILD_VIEW_MSG);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Constructor 
 * 
 * @param eOrientation   Axis orientation for this slice instance
 * @param hParent        Parent window
 * @param uControlID     ID of the window
 */
Segment3DChildView::Segment3DChildView(const DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID)
 : ViatronChildView(eGVT_3DSEGMENTS, hParent, eDatasetOrientation),
 m_hScrollbar(HWND(NULL)), m_bMeasureActive(false),
 m_bUpdateEndoluminalView(false), m_pDatasetLastRendered((Dataset*) NULL),
 m_bJustFinishInteracting(false), m_ptLastSynched3DPos(0,0,0)
{
  // SetDatasetType(eDatasetOrientation);
  m_3DsegmentViewer.SetDatasetType(eDatasetOrientation);

  m_point = CPoint(0,0);
} // Constructor


/**
 * Destructor 
 */
Segment3DChildView::~Segment3DChildView()
{
  m_pDatasetLastRendered = (Dataset*) NULL;
} // Destructor


/**
 * Initialize slice volume data
 * 
 * @param WPARAM   Unused
 * @param LPARAM   Unused
 * @return         Result
 */
LRESULT Segment3DChildView::OnVolumeLoaded(WPARAM, LPARAM)
{
  UpdateSliceScrollPos(true);
  return 0;
} // OnVolumeLoaded()


/**
 * Render the scene
 * @param void*   Unused
 */
void Segment3DChildView::GLRenderScene(void *)
{
  if (!m_3DsegmentViewer.GetDataset()->IsLoadingComplete())
  {
    ViatronChildView::GLRenderScene();
    return;
  }

  // Synchronize to the current 3D position *if necessary*; do not synchronize in certain cases which may create 'circular' updates.
  Point3D<float4> ptCurrPos = GetDatasetRef().m_submarine.GetPositionWorld();
  CPoint curpt; GetCursorPos(&curpt);
  if ( IsWindow(*this) )
    this->ScreenToClient(&curpt);
  else
    curpt = CPoint(0,0);
  CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  CRect rtClient = viewRect - viewRect.TopLeft();
TRACE("CurrPt: %d,%d\n", curpt.x, curpt.y);
  if ( !m_bJustFinishInteracting && (ptCurrPos != m_ptLastSynched3DPos) && !GetDatasetRef().m_submarine.IsFlying()
    && !(IsKeyStateDown(VK_LBUTTON) && rtClient.PtInRect(curpt)) )
  {
TRACE("segment selecting\n");
    m_3DsegmentViewer.SelectBestSegment();
    m_ptLastSynched3DPos = ptCurrPos;
  }

  if (m_pDatasetLastRendered != m_3DsegmentViewer.GetDataset())
  {
    UpdateSliceScrollPos(true);
    m_pDatasetLastRendered = m_3DsegmentViewer.GetDataset();
  }
  else
  {
    UpdateSliceScrollPos(false);
  }

  // render the image
  m_3DsegmentViewer.SetInvertImage( GetInverseImage() );
  m_3DsegmentViewer.RenderSegment(GetViewport().m_left, GetViewport().m_top);

} // GLRenderScene()


/**
 * Render text and other 2D objects 
 *
 * @param hDC   Device context for drawing
 */
void Segment3DChildView::GLRasterScene()
{
  if (!m_3DsegmentViewer.GetDataset()->IsLoadingComplete())
  {
    ViatronChildView::GLRasterScene();
    return;
  }

  glPushAttrib(GL_CURRENT_BIT);

  CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  CRect clientRect = viewRect - viewRect.TopLeft();

  // OpenGL font
  GLFont & glFont = GetNormalFont();

  // Color for window text & menus
  glDisable( GL_BLEND );
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHT0);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glColor3f(1,0,0);

  int1 sTmpStr[256];

#if 0 // no printing of angle (Issue #5781)
  // Angle
  const uint4 uAngle( m_3DsegmentViewer.GetCurrentPlaneAngle() * 20 );
  sprintf(sTmpStr, "%03d deg", uAngle);
  glColor3f(1,1,1);
  // At bottom-right
  glFont.DrawText(clientRect.right - 2, 4, sTmpStr, GDT_ABOVELEFT);
#endif

  // Segment number
  Dataset& rDataset = *m_3DsegmentViewer.GetDataset();
  sprintf(sTmpStr, "Segment: %d/%d", rDataset.m_currentSegment+1, rDataset.m_straightSegmentNodes.size()-1 ); // the m_straightSegmentNodes.size() is number of nodes rather than that of segments
  // At top-right
  glFont.DrawText(clientRect.right/2 - 2, clientRect.bottom - 2, sTmpStr, GDT_BELOWLEFT);

  if ( IsReportRendering() )
  {
    glColor3f(0,1,0);
    std::string sPatInfo = GetDatasetRef().m_studyinfo.m_sPatientID + ", " + GetDatasetRef().m_studyinfo.m_sExamTime;

    glFont.DrawText(clientRect.CenterPoint().x - clientRect.Height() + m_iTextPadding, clientRect.bottom - m_iTextPadding + m_iBottomExtraTextPadding, sPatInfo.c_str(), GDT_BELOW); // top-left
    glFont.DrawText(clientRect.CenterPoint().x + clientRect.Height() - m_iTextPadding, clientRect.bottom - m_iTextPadding + m_iBottomExtraTextPadding, sPatInfo.c_str(), GDT_BELOWLEFT); // top-right
  }

  GLRenderLogo();

  glPopAttrib();

  DrawCadMarkings();

  DrawIndicatorInOtherQuadrant();

} // GLRasterScene()


/**
 * Draws the CAD markings for the current segment
 */
void Segment3DChildView::DrawCadMarkings()
{
  // current dataset
  Dataset & dataset = *m_3DsegmentViewer.GetDataset();

  // break out early if cad findings aren't loaded
  if( !dataset.m_bCadFindingsLoaded || (!rdrGlobal.m_bDisplayCadFindings && !rdrGlobal.m_bAlwaysShowCadArrows) )
    return;

  /////////////////////////////////////////////////////////////////
  // locate all the visible cad findings within the current segment
  
  // get the skeleton indices for the current segment
  const uint4 uIndex1 = dataset.m_straightSegmentNodes[ dataset.m_currentSegment ];
  const uint4 uIndex2 = dataset.m_straightSegmentNodes[ dataset.m_currentSegment + 1 ];

  std::vector< int4 > visibleFindings;
  for( int4 iFindingIndex = 0; iFindingIndex < dataset.m_cadFindings.GetCount(); ++iFindingIndex )
  {
    const vxCadFinding & finding = dataset.m_cadFindings.GetFinding( iFindingIndex );
    
    const bool bFindingVisible = RenderUtil::IsFindingVisible( finding, dataset.m_session.m_mapCadFindingInfo,
                                                               dataset.m_cadFindings.m_iCadThresholdValue, false );
    if ( bFindingVisible )
    { 
      const ColonSession::FindingInfo & info = dataset.m_session.m_mapCadFindingInfo[ finding.GetId() ];
      
      // check if the skeleton index is included between [uIndex1,uIndex2]
      if( info.GetJoinedSkeletonIndex() >= uIndex1 && info.GetJoinedSkeletonIndex() <= uIndex2 )
        visibleFindings.push_back( iFindingIndex );
    }
  }  // for( int4 iFindingIndex = 0; iFindingIndex < dataset.m_cadFindings.GetCount(); ++iFindingIndex )


  /////////////////////////////////////////////////////////////////
  // render all visible findings
  
  // save existing opengl state
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  // setup an orthographic projection 2:1 viewing aspect ratio
  glMatrixMode( GL_PROJECTION );
  glPushMatrix(); 
  glLoadIdentity();
  gluOrtho2D( 0.0, 2.0, 0.0, 1.0 );

  // setup properties for cad marking
  if (rdrGlobal.m_iColorScheme == 0)
    glColor3ub( 255, 255, 0 );
  else
    glColor3ub( 255, 0, 0);
  glLineWidth( 3.0F );

  // radius for CAD marking
  const float4 fMarkingRadius = 0.03F;

  // render cad markings for each quadrant in the segment viewer
  const int4 iNumberOfAngles = 4;
  const float4 fAngles[] = { 0.0F, 90.0F, 180.0F, 270.0F };
  for( int4 iAngleIndex = 0; iAngleIndex < iNumberOfAngles; ++iAngleIndex )
  {
    // setup plane for specified quadrant angle
    m_3DsegmentViewer.SetupPlaneCorners( fAngles[ iAngleIndex ] );

    // create quadrilateral representing plane from segment renderer
    Quadrilateral quad( dataset.m_vObliqueTopLeft, dataset.m_vObliqueTopRight, dataset.m_vObliqueBottomLeft, dataset.m_vObliqueBottomRight );

    // create a vx plane from the oblique data
    vxPlaneOblique< float4 > plane( dataset.m_vObliqueOrientation, dataset.m_vObliqueLocation );
    
    CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
    CRect rtClient = viewRect - viewRect.TopLeft();

    float4 xmin,xmax,ymin,ymax;

    if( rtClient.Width() <= 2*rtClient.Height() )
    {
      const float4 fBlackStripeWidth = 0.5F - float4(rtClient.Width())/(4.0F*float4(rtClient.Height()));

      xmin = (iAngleIndex % 2) ? 1.0F : 0.0F;
      xmax = (iAngleIndex % 2) ? 2.0F : 1.0F;
      ymin = (iAngleIndex < 2) ? fBlackStripeWidth : 0.5F;
      ymax = (iAngleIndex < 2) ? 0.5F : 1.0F-fBlackStripeWidth;
    }
    else
    {
      const float4 fBlackStripeWidth = 1.0F - 2.0F*float4(rtClient.Height())/float4(rtClient.Width());

      xmin = (iAngleIndex % 2) ? 1.0F : fBlackStripeWidth;
      xmax = (iAngleIndex % 2) ? 2.0F-fBlackStripeWidth : 1.0F;
      ymin = (iAngleIndex < 2) ? 0.0F : 0.5F;
      ymax = (iAngleIndex < 2) ? 0.5F : 1.0F;
    }

    // render each visible cad finding in the current quadrant
    for( int4 iVisibleFindingIndex = 0; iVisibleFindingIndex < visibleFindings.size(); ++iVisibleFindingIndex )
    {
      const vxCadFinding & finding = dataset.m_cadFindings.GetFinding( visibleFindings[ iVisibleFindingIndex ] );

      Point3Df & position = Point3Df(finding.GetCentroidMM().GetVector());

      Point3Df intersection;
      plane.GetIntersection( position, -dataset.m_vObliqueOrientation, intersection );

      if( (dataset.m_vObliqueLocation.m_x-position.m_x)*dataset.m_vObliqueOrientation.X() +
          (dataset.m_vObliqueLocation.m_y-position.m_y)*dataset.m_vObliqueOrientation.Y() +
          (dataset.m_vObliqueLocation.m_z-position.m_z)*dataset.m_vObliqueOrientation.Z() > 0.0F )
        continue;

      // project the position of the finding onto the quadrilateral ( position is normalized )
      Point2D< float4 > point = quad.GetProjectedPosition( intersection );

      // adjust position to take into account the quadrant position and extra width
      point.m_x = Interpolate( 0.0F, point.m_x, 1.0F, xmin, xmax );
      point.m_y = Interpolate( 0.0F, point.m_y, 1.0F, ymin, ymax );
        
      // draw the cad marking
      RenderUtil::GLDrawCircle( point, fMarkingRadius );
    } // for( int4 i
  } // for( int4 iAngleIndex = 0; iAngleIndex < iNumberOfAngles; ++iAngleIndex )

  // restore previous opengl state
  glPopMatrix();
  glPopAttrib();  
} // DrawCadMarkings()


/**
 * Set up slice viewer rendering context
 */
void Segment3DChildView::GLSetupRC()
{
  if (!m_3DsegmentViewer.GetDataset()->IsLoadingComplete())
  {
    ViatronChildView::GLSetupRC();
    return;
  }
  m_3DsegmentViewer.SetupOpenGLContext();
} // GLSetupRC()


/**
 * Called on view window resize
 *
 * @param iWidth    Width of child view
 * @param iHeight   Height of child view
 */
void Segment3DChildView::GLResize(GLsizei iWidth, GLsizei iHeight)
{

  m_3DsegmentViewer.ResizeDisplay(iWidth, iHeight);

  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

  SliceScrollBar & scrollBar = GetScrollBar();
  if (scrollBar)
  {
    scrollBar.MoveWindow(CRect(CPoint(GetViewRect().m_left-2, GetViewRect().m_bottom+4),
      CSize(GetViewRect().Width()+4, 12)));
  }
} // GLResize()


/**
 * On idle handler, return true if any rendering on view should occur
 *
 * @return   True to render view, false otherwise
 */
bool Segment3DChildView::GLIdle()
{
  return false;
} // GLIdle()

//////////////////////////////////////////////////////////////////////////////////////


/**
 * Gets string IDs for mouse button use
 *
 * @param iLeftID    Left button use string resource ID
 * @param iMidID     Middle button use string resource ID
 * @param iRightID   Right button use string resource ID
 */
void Segment3DChildView::GetMouseKeyStringIDs(uint4 & iLeftID, uint4 & iMidID, uint4 & iRightID) const
{
  // left mouse synchronizes
  iLeftID = IDS_SYNCHRONIZE;
  // middle wheel always changes display angle
  iMidID = IDS_PLANE_ANGLE;

  if (GetAsyncKeyState(VK_SHIFT)&0x8000)
  {
    // iLeftID = ;
    //iRightID = ;
  }
  else if (GetAsyncKeyState(VK_CONTROL)&0x8000)
  {
    //iLeftID = ;
    //iRightID = ;
  }
  else
  {
    //iLeftID = ;
    //iRightID = ;
  }
} // GetMouseKeyStringIDs().

//////////////////////////////////////////////////////////////////////////////////////
// ... Methods


/**
 * Returns a temporary-use reference to the accompanying scrollbar instance
 *
 * @return    Temporary-use reference to the slice scrollbar
 */
SliceScrollBar & Segment3DChildView::GetScrollBar()
{
  SliceScrollBar & scrollBar = *(dynamic_cast<SliceScrollBar*>(CWnd::FromHandle(m_hScrollbar)));
  if (!scrollBar)
  {
    TRACE("Couldn't find scrollbar dialog item ID %d\n", GetClassInstanceID());
  }
  return scrollBar;
} // GetScrollBar()


/**
 * Updates the scrollbar position and range (if indicated)
 *
 * @param bSetRange   Whether or not to update scrollbar range as well
 */
void Segment3DChildView::UpdateSliceScrollPos(const bool bSetRange)
{
  SliceScrollBar & scrollBar = GetScrollBar();
  if (!scrollBar)
  {
    return;
  }

  //int4 iMaxSlice(m_sliceViewer.GetNumberOfSlices() - 1);
  int4 iMaxSlice(18);
  if (iMaxSlice <= 0)   iMaxSlice = 1;

  if (bSetRange)
  {
    scrollBar.SetScrollRange(0, iMaxSlice);
  }

  const int4 iScrollPos(m_3DsegmentViewer.GetCurrentPlaneAngle());
  if (scrollBar.GetScrollPos() != iScrollPos)
  {
    scrollBar.SetScrollPos(iScrollPos);
  }
} // UpdateSliceScrollPos().


/**
 * Returns ID of the popup menu to display
 *
 * @param uFlags   Mouse/key down flags
 * @param point    Point of mouse event triggering popup menu
 * @return         Resource ID of popup menu
 */
uint4 Segment3DChildView::GetPopupMenuID(UINT uFlags, CPoint point) const
{
  return 0;

  // Don't show a popup menu when control key is down
  if (uFlags & MK_CONTROL)
  {
    return 0;
  }

} // GetPopupMenuID().



/**
 * Prepares popup menu for display by enabling/disabling/removing items as appropriate
 *
 * @param menu      Popup menu
 * @param uMenuID   Id of menu
 */
void Segment3DChildView::AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const
{
} // AdjustPopupMenu()


/**
 * Message handler
 *
 * @param uMessage   Message
 * @param lParam     Message-dependent parameter
 * @return           Non-zero if message is handled
 */
int4 Segment3DChildView::OnMessage(uint2 uMessage, int4 lParam)
{
  switch (uMessage)
  {
    case WM_HSCROLL:    // Scroll to absolute position
    case WM_VSCROLL:    // Scroll by relative position
    {
      InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);

      uint4 uNewAngle  = LOWORD(lParam);
      uint4 uCurrAngle = m_3DsegmentViewer.GetCurrentPlaneAngle();

      if (uMessage == WM_HSCROLL)
      {
        // Scroll by difference with current slice
        m_3DsegmentViewer.Scroll(uNewAngle - uCurrAngle);
      }
      else // message == WM_VSCROLL
      {
        m_3DsegmentViewer.Scroll(LOWORD(lParam));
      }
      // get the actual slice scrolled to
      uNewAngle = m_3DsegmentViewer.GetCurrentPlaneAngle();

      m_3DsegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
      Invalidate(FALSE);
      InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);

      return (uNewAngle<<16) + (uCurrAngle);
    }
    break;

    case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      //m_sliceViewer.ResetCurrentVolumePosition();
      UpdateSliceScrollPos();
      InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);
      m_3DsegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
      break;

    default:
      return FALSE; // Command not handled
  } // endswitch(uMessage)

  Invalidate(FALSE);
  return TRUE;  // Command handled
} // OnMessage()


/**
 * Sets the orientation of the slice
 *
 * @param eOrientation   Slice orientation
 */
void Segment3DChildView::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  m_3DsegmentViewer.SetDatasetType(eOrientation);
  UpdateSliceScrollPos(true);
} // SetDatasetType()


/**
 * WM_COMMAND handler
 *
 * @param wParam   Command ID (LOWORD) / Type of control (HIGHWORD)
 * @param lParam   Control handle, or 0 if message not from a control
 * @return         Whether or not message was handled
 */
BOOL Segment3DChildView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
    // Reset slice zoom
    case ID_SLICEZOOM_100:
      break;

    case ID_VIEWWINDOW_SHOWCLEANSED: // Toggle display cleansed/uncleansed slice
      if (m_3DsegmentViewer.IsShowUncleansedSet())
        m_3DsegmentViewer.SetShowUncleansedFlag(false);
      else
        m_3DsegmentViewer.SetShowUncleansedFlag(true);
      m_3DsegmentViewer.SetDesiredPlaneAngle(-1);

      break;

    case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      //m_sliceViewer.ResetCurrentVolumePosition();
      UpdateSliceScrollPos();
      break;

    case ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW: // if update endoluminal view while flying (centerline)
      m_bUpdateEndoluminalView = 1 - m_bUpdateEndoluminalView;
      break;

    case ID_VIEWWINDOW_SNAPSHOT:
      /// See handler(s) in EndoOrthogonalViewFrame::ViewWnd and SupineProneViewWnd
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
      break;

    default:
      return FALSE; // Command not handled
  } // endif seln value

  Invalidate(FALSE);
  return TRUE;  // Command handled
} // OnCommand()


/**
 * Left mouse button down
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment3DChildView::OnLButtonDown(UINT uFlags, CPoint point)
{
  m_point = point;

  // All subsequent mouse input will be sent to this view
  SetCapture();

  SynchronizeEndoluminalView( point );
 
  m_bJustFinishInteracting = true;

} // OnLButtonDown()


/**
 * Left mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment3DChildView::OnLButtonUp(UINT uFlags, CPoint point)
{
  // release mouse input capture
  ReleaseCapture();

  Invalidate(FALSE);
  
  m_bJustFinishInteracting = true;

} // OnLButtonUp()


/**
 * Dbl click Left mouse button
 *
 * @param uFlags  Mouse/keyboard flags
 * @param point   Point of mouse click
 */
void Segment3DChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  m_point = point;

  SynchronizeEndoluminalView( point );
} // OnLButtonDblClk()


/**
 * synchronizes the submarine to the given 2D image point
 *
 * @param point   the location of the mouse cursor
 */
void Segment3DChildView::SynchronizeEndoluminalView( CPoint point )
{

  CRect rect;
  GetClientRect( & rect );

  float4 fPosX = float4( point.x )/float4( rect.Width() );
  float4 fPosY = float4( point.y )/float4( rect.Height() );
  Normal3D<float4> viewDirectionWorld, viewUpWorld;
  Point<float4> vPosWorldCoord = m_3DsegmentViewer.GetPickedLocationWorldCoord( fPosX, fPosY, false, viewDirectionWorld, viewUpWorld );
  //Point<float4> vPosWorldCoord = GetDatasetRef().m_submarine.GetPositionWorld();
  //Normal3D<float4> viewDirectionWorld = GetDatasetRef().m_submarine.GetOrientation(), viewUpWorld = GetDatasetRef().m_submarine.GetUpDirection();

  GetDatasetRef().SelectSegment( vPosWorldCoord );
  GetDatasetRef().m_submarine.SetView( vPosWorldCoord, viewDirectionWorld, viewUpWorld );
  GetDatasetRef().m_submarine.UpdatePosition();
  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();

  InvalidateViews( GLChildView::eGVT_NAVIGATION, 0 );

} // SynchronizeEndoluminalView()


/**
 * Right mouse button down
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment3DChildView::OnRButtonDown(UINT uFlags, CPoint point)
{
  SetCapture();

  // stop autoflying (centerline).
  if ( (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D) )
  {
    ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;

    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

    const long UPDATE_OVERVIEW = 1;
    const long RERENDER_OVERVIEW = 0;
    GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW ) );

    return;
  }

  VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());
  bool bMeasure = (sliceModes == ViewModes::LINEMEASURE) || (sliceModes == ViewModes::ROIMEASURE);

  GLChildView::OnRButtonDown(uFlags, point);
} // OnRButtonDown(). 


/**
 * Right mouse button up
 *
 * @param uFlags   Mouse/keyboard flags
 * @param point    Point of mouse click
 */
void Segment3DChildView::OnRButtonUp(UINT uFlags, CPoint point)
{
  ReleaseCapture();
  //RoiMeasurement(Segment3DChildView::MEASUREFUNC_UPDATE, CPoint(0,0));
  GLChildView::OnRButtonUp(uFlags, point);
} // OnRButtonUp()


/**
 * Sets mouse cursor
 *
 * @param pWnd       Pointer to current window
 * @param uHitTest   Identifies location of the mouse
 * @param uMessage   Type of message triggering set cursor message
 * @return           TRUE if cursor is set, FALSE otherwise (to set default cursor)
 */
BOOL Segment3DChildView::OnSetCursor(CWnd * pWnd, UINT uHitTest, UINT uMessage)
{
  UINT uCursorID = IDC_CROSSMC;

  // Show the measure icon if a main view window and mode is line or roi measure
  if (IsMainViewWindow())
  {
    switch (ViatronStudy::GetViewModes(GetViewWndType()))
    {
      case ViewModes::LINEMEASURE:
      case ViewModes::ROIMEASURE:
        uCursorID = IDC_MEASURE;
        break;
      default:
        break;
    } // endswitch(ViatronStudy::ViewModes)
  }

  if (uCursorID == IDC_CROSSMC)
  {
    switch (uMessage)
    {
      case WM_LBUTTONDOWN:
        break;

      case WM_RBUTTONDOWN:
        break;

      default:
        break;
    } // endswitch(uMessage)
  } // endif(uCursorID)

  SetCursor(AfxGetApp()->LoadCursor(uCursorID));
  return TRUE;
} // OnSetCursor()


/**
 * Mouse event handlers: mouse move.
 *
 * @param  uFlags   Indicates whether various virtual keys are down. 
 * @param  point    specifies the x- and y-coordinate of the cursor.
 */
void Segment3DChildView::OnMouseMove(UINT uFlags, CPoint point)
{
  m_point = point;

  if (!m_3DsegmentViewer.GetDataset()->IsLoadingComplete())
  {
    ViatronChildView::OnMouseMove(uFlags, point);
    return;
  }

  Point2D<int4> pt( point.x, point.y );
  m_3DsegmentViewer.SetupHalfPlaneShadow(pt);

  if ( uFlags & MK_LBUTTON )
    SynchronizeEndoluminalView( point );

  //////Invalidate(FALSE);
  GLChildView::OnMouseMove(uFlags, point);

} // OnMouseMove()


/**
 * Mouse wheel spin handler, scrolls slice
 *
 * @param uFlags    Mouse/keyboard flags
 * @param iZDelta   Amount of mouse wheel spin
 * @param pt        Point at which wheel event occurs
 * @return          TRUE to re-enable mouse scroll
 */
BOOL Segment3DChildView::OnMouseWheel(UINT uFlags, int2 iZDelta, CPoint pt)
{
  // each mouse wheel click is 120 (WHEEL_DELTA) delta values
  int4 iMove(iZDelta / WHEEL_DELTA);

  // go much faster if the user holds down the ctrl key
  static const int4 iFactor = 10;
  if (uFlags & MK_CONTROL) iMove *= iFactor;

  m_3DsegmentViewer.Scroll(iMove);
  UpdateSliceScrollPos();

  m_3DsegmentViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
  Invalidate(FALSE);
  InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);
  
  // return true to continue enabling mouse wheel scrolling
  return TRUE;
} // OnMouseWheel().


/**
 * Update window's visibility state.
 *
 * @param bShow   Whether or not the window is now visible.
 */
void Segment3DChildView::OnShowWindow(const bool bShow)
{
  SliceScrollBar & scroll = GetScrollBar();
  if (scroll)
  {
    scroll.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
  }
} // OnShowWindow()


/**
 * proc response to action that mouse leaves the window.
 */
void Segment3DChildView::OnMouseLeave()
{
  ////Invalidate(FALSE);
  GLChildView::OnMouseLeave();
  m_bJustFinishInteracting = false;
  m_ptLastSynched3DPos = GetDatasetRef().m_submarine.GetPositionWorld();
} // OnMouseLeave()


/////////////////////////////////////////////////////////////////////////////////////
//

/**
 * keyboard event handler: key press.
 *
 * @param uChar     specifies the virtual key code of the given key.
 * @param uRepCnt   specifies the repeat count.
 * @param uFlags    specifies the scan code, key-transition code, previous key state, and context code.
 */
void Segment3DChildView::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{ 
  // this needs to be done so that the CAD jumping works correctly
  m_bJustFinishInteracting = false;
  return;
  
  /// The following implementation is not for final product. It is for development purpose.
  switch (uChar)
  {
    case 'M': 
    { /// This appears to allow user adjusting the RecursiveDouglasPeucker algorithm setting.
      /// Increase the threshold make the partitioned straight segment longer
      if(rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold<250)
      rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold++;
      rdrGlobal.m_pCurrDataset->CreateSkeletonSegments();
      break;
    } 
    case 'L': 
    { /// This appears to allow user adjusting the RecursiveDouglasPeucker algorithm setting.
      /// Decrease the threshold make the partitioned straight segment shorter
      if(rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold>0)
      rdrGlobal.m_pCurrDataset->m_fSegmentErrorThreshold--;
      rdrGlobal.m_pCurrDataset->CreateSkeletonSegments();
      break;
    } 

    case 'T':
      break;

    default:
      GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
      return;
  }

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

  m_3DsegmentViewer.SetDesiredPlaneAngle( 0 );

  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  Invalidate(FALSE);

  GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown()


/**
 * Called when mode changes
 *
 * @param uOldMode   previous mode
 * @param uNewMode   new mode
 */
void Segment3DChildView::OnViewModeChange(const uint4 uOldMode, const uint4 uNewMode)
{
  // feign an unchanged scroll jump, which will set whatever dirty flags are needed to update the image
  OnMessage(WM_HSCROLL, LPARAM(m_3DsegmentViewer.GetCurrentPlaneAngle()));
} // OnViewModeChange()


/**
 * Invalidate (mark for redraw) view
 */
void Segment3DChildView::InvalidateView()
{
  m_3DsegmentViewer.SetImageDirty();
} // InvalidateView()


/**
 * draw a cross-hair at the other UDS quadrant to indicate the same anatomy spot
 *
 * @param point   the location of the mouse cursor
 */
void Segment3DChildView::DrawIndicatorInOtherQuadrant()
{
  CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  CRect rect = viewRect - viewRect.TopLeft();

  float4 fPosX = float4( m_point.x )/float4( rect.Width() );
  float4 fPosY = float4( m_point.y )/float4( rect.Height() );
  Normal3D<float4> viewDirectionWorld, viewUpWorld;
  Point<float4> vPosWorldCoord = m_3DsegmentViewer.GetPickedLocationWorldCoord( fPosX, fPosY, true, viewDirectionWorld, viewUpWorld );

  const float4 fSelectionX = Bound( 0.0F, Interpolate( m_3DsegmentViewer.m_viewport.GetMinX(), 
                                    fPosX * m_3DsegmentViewer.m_uDisplayWidth, 
                                    m_3DsegmentViewer.m_viewport.GetMaxX(), 0.0F, 1.0F ), 1.0F );
  const float4 fSelectionY = Bound( 0.0F, Interpolate( m_3DsegmentViewer.m_viewport.GetMinY(), 
                                    fPosY * m_3DsegmentViewer.m_uDisplayHeight, 
                                    m_3DsegmentViewer.m_viewport.GetMaxY(), 0.0F, 1.0F ), 1.0F );

  /// determine the quadrant the point should be in
  int4 iQuadrantIndex;
  if( fSelectionX > 0.5F && fSelectionY < 0.5F )
    iQuadrantIndex = 3;
  else if( fSelectionX < 0.5F && fSelectionY > 0.5F )
    iQuadrantIndex = 0;
  else if( fSelectionX > 0.5F && fSelectionY > 0.5F )
    iQuadrantIndex = 1;
  else
    iQuadrantIndex = 2;

  // save existing opengl state
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  // setup an orthographic projection 2:1 viewing aspect ratio
  glMatrixMode( GL_PROJECTION );
  glPushMatrix(); 
  glLoadIdentity();
  gluOrtho2D( 0.0, 2.0, 0.0, 1.0 );
  glColor3ub( 20, 40, 255);

  // current dataset
  Dataset & dataset = *m_3DsegmentViewer.GetDataset();

  // render indicator for each quadrant in the segment viewer
  const int4 iNumberOfAngles = 4;
  const float4 fAngles[] = { 0.0F, 90.0F, 180.0F, 270.0F };
  for( int4 iAngleIndex = 0; iAngleIndex < iNumberOfAngles; ++iAngleIndex )
  {
    if( (iQuadrantIndex == 0 && iAngleIndex != 0 && iAngleIndex != 2 ) ||
        (iQuadrantIndex == 1 && iAngleIndex != 1 && iAngleIndex != 3 ) ||
        (iQuadrantIndex == 2 && iAngleIndex != 0 && iAngleIndex != 2 ) ||
        (iQuadrantIndex == 3 && iAngleIndex != 1 && iAngleIndex != 3 ) )
    {
      // setup plane for specified quadrant angle
      m_3DsegmentViewer.SetupPlaneCorners( fAngles[ iAngleIndex ] );

      // create quadrilateral representing plane from segment renderer
      Quadrilateral quad( dataset.m_vObliqueTopLeft, dataset.m_vObliqueTopRight, dataset.m_vObliqueBottomLeft, dataset.m_vObliqueBottomRight );

      // create a vx plane from the oblique data
      vxPlaneOblique< float4 > plane( dataset.m_vObliqueOrientation, dataset.m_vObliqueLocation );
      
      float4 xmin,xmax,ymin,ymax;

      if( rect.Width() <= 2*rect.Height() )
      {
        const float4 fBlackStripeWidth = 0.5F - float4(rect.Width())/(4.0F*float4(rect.Height()));

        xmin = (iAngleIndex % 2) ? 1.0F : 0.0F;
        xmax = (iAngleIndex % 2) ? 2.0F : 1.0F;
        ymin = (iAngleIndex < 2) ? fBlackStripeWidth : 0.5F;
        ymax = (iAngleIndex < 2) ? 0.5F : 1.0F-fBlackStripeWidth;
      }
      else
      {
        const float4 fBlackStripeWidth = 1.0F - 2.0F*float4(rect.Height())/float4(rect.Width());

        xmin = (iAngleIndex % 2) ? 1.0F : fBlackStripeWidth;
        xmax = (iAngleIndex % 2) ? 2.0F-fBlackStripeWidth : 1.0F;
        ymin = (iAngleIndex < 2) ? 0.0F : 0.5F;
        ymax = (iAngleIndex < 2) ? 0.5F : 1.0F;
      }

      Point3Df intersection;
      plane.GetIntersection( vPosWorldCoord, -dataset.m_vObliqueOrientation, intersection );

      if( (dataset.m_vObliqueLocation.m_x-vPosWorldCoord.m_x)*dataset.m_vObliqueOrientation.X() +
          (dataset.m_vObliqueLocation.m_y-vPosWorldCoord.m_y)*dataset.m_vObliqueOrientation.Y() +
          (dataset.m_vObliqueLocation.m_z-vPosWorldCoord.m_z)*dataset.m_vObliqueOrientation.Z() > 0.0F )
        continue;

      // project the position of the finding onto the quadrilateral ( position is normalized )
      Point2D< float4 > point = quad.GetProjectedPosition( intersection );

      // adjust position to take into account the quadrant position and extra width
      point.m_x = Interpolate( 0.0F, point.m_x, 1.0F, xmin, xmax );
      point.m_y = Interpolate( 0.0F, point.m_y, 1.0F, ymin, ymax );
        
      // draw the cad marking
      RenderUtil::GLDrawRhombus( point, 0.03F, 1.0F, false );
    } // if panel side checking

  } // for( int4 iAngleIndex = 0; iAngleIndex < iNumberOfAngles; ++iAngleIndex )
  
  // restore previous opengl state
  glPopMatrix();
  glPopAttrib();  

} // DrawIndicatorInOtherPanel()



// $Log: Segment3DChildView.C,v $
// Revision 1.26.2.16  2010/09/30 16:29:07  dongqing
// removed debug code
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.15  2010/09/30 16:25:59  dongqing
// removed debug code
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.14  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.13  2010/03/10 03:09:10  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.12  2010/03/09 20:23:10  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.11  2010/02/09 17:38:51  dongqing
// add the side panel check for anatomy location indicator (rhombus)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.10  2010/02/08 22:52:00  dongqing
// Draw the location indicator rhombus line thinner
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.9  2010/02/01 19:33:53  dongqing
// change the RayCastObliqueSlice for handling the selection for both ray depth situations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.8  2010/01/29 22:57:30  dongqing
// add location indicator on other quadrent. Mouse left button down
// to trigger that.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.7  2010/01/25 22:23:42  dongqing
// add JoinedSkeletonIndex for correctly display cad mark in UDS view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.6  2009/12/16 15:14:17  dongqing
// fix the error display of UD segments total number. The m_straightNodes.size()
// is the number of nodes rather than number of segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.26.2.5  2009/10/07 16:13:15  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.26.2.4  2007/10/23 19:24:59  jmeade
// Issue 5852: move segment re-creation function down to segment child view.
//
// Revision 1.26.2.3  2007/08/20 19:44:44  jmeade
// Issue 5781: remove display of rotation angle in segments view.
//
// Revision 1.26.2.2  2007/05/03 17:41:16  jmeade
// spacing.
//
// Revision 1.26.2.1  2007/03/16 17:53:30  jmeade
// Issue 5474: mouse text and mouse cursor changes.
//
// Revision 1.26  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.25  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.24  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.23  2007/02/15 15:39:07  dongqing
// decoment the Invalidate(FALSE) for mouse wheel bug
//
// Revision 1.22  2007/02/08 19:10:22  dongqing
// disable all Invalidate(FALSE); This seems not necessary.
// It also fixes the 3D fly jig due to mouse move.
//
// Revision 1.21  2007/02/07 20:47:17  jmeade
// Issue 5222: different cad mark colours for different colour schemes.
//
// Revision 1.20  2007/02/06 16:40:27  jmeade
// user data in render method.
//
// Revision 1.19  2007/02/06 15:44:17  dongqing
// Add the CAD finding display option menu
//
// Revision 1.18  2007/02/05 17:07:59  dongqing
// add display CAD mark on UD segment view
//
// Revision 1.17  2007/01/30 16:58:49  geconomos
// initial work on draw cad markings
//
// Revision 1.16  2007/01/10 18:31:46  jmeade
// Issue 5217: flag image for dirty on InvalidateView().
//
// Revision 1.15  2006/11/30 20:02:48  jmeade
// comments.
//
// Revision 1.14  2006/11/24 16:53:42  jmeade
// Issue 5137: correlate 3d segment view back to submarine only when appropriate.
//
// Revision 1.13  2006/11/21 04:44:44  jmeade
// Issue 5137: less buggy version, will only circular-correlate (ud-3d-back-to-ud) in rare cases.  will fix soon.
//
// Revision 1.12  2006/11/19 00:20:25  jmeade
// Issue 5137: correlate 3d segment view back to submarine when appropriate.
//
// Revision 1.11  2006/10/26 21:07:57  frank
// removed transparency of viatronix logo
//
// Revision 1.10  2006/10/09 19:14:29  frank
// Issue #5048: Fixed scrollbar behavior of UD segments view
//
// Revision 1.9  2006/10/04 16:44:12  jmeade
// segment n/n; remove zoom factor/menu.
//
// Revision 1.8  2006/10/04 15:43:54  jmeade
// update view on mode change; mouse help.
//
// Revision 1.7  2006/10/04 04:17:17  jmeade
// reverse segment jump; render logo; remove popup menu.
//
// Revision 1.6  2006/10/03 20:50:52  frank
// synchronized navigation view on left button down
//
// Revision 1.5  2006/09/28 16:00:36  frank
// added image inversion (for taking snapshots)
//
// Revision 1.4  2006/09/27 19:19:29  frank
// fixed view selection
//
// Revision 1.3  2006/01/31 14:27:19  frank
// code review
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.8.2.3.4.6.2.1  2005/07/15 17:21:43  geconomos
// Issue # 4225: Drawing is choppy while switching views
//
// Revision 1.8.2.3.4.6  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 1.8.2.3.4.5  2005/06/10 18:20:38  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.8.2.3.4.4  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.8.2.3.4.3.2.1  2005/06/14 17:37:49  jmeade
// pass scrollbar code as part of WM_HSCROLL message from slicescrollbar.
//
// Revision 1.8.2.3.4.3  2005/04/21 12:57:27  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 1.8.2.3.4.2.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.8.2.3.4.2  2005/04/19 13:40:56  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 1.8.2.3.4.1  2005/03/21 19:54:00  frank
// Issue #4038: Sped up font drawing
//
// Revision 1.8.2.3  2003/04/24 20:32:44  jmeade
// Using equivalent draw text location enum values.
//
// Revision 1.8.2.2  2003/02/27 03:22:03  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.8.2.1  2003/02/12 01:02:53  jmeade
// Clarifying names for 2D flight modes.
//
// Revision 1.8  2002/11/14 19:38:06  kevin
// Clean up unused code
//
// Revision 1.7  2002/11/14 18:46:40  jmeade
// Ensure a dataset is loaded before rendering.
//
// Revision 1.6  2002/09/27 14:21:14  ingmar
// replaced Size2D with Point2D
//
// Revision 1.5  2002/09/10 15:47:56  jmeade
// Use one function to handle changing main dataset.
//
// Revision 1.4  2002/08/28 20:52:21  kevin
// Lots of updates to segmental viewing
//
// Revision 1.3  2002/08/22 13:50:00  jmeade
// Don't render when no dataset loaded.
//
// Revision 1.2  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.1  2002/08/20 19:15:33  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:54:23  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Segment3DChildView.C,v 1.26.2.16 2010/09/30 16:29:07 dongqing Exp $
// $Id: Segment3DChildView.C,v 1.26.2.16 2010/09/30 16:29:07 dongqing Exp $
