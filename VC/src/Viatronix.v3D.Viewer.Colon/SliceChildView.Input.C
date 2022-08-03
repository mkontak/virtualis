// $Id: SliceChildView.Input.C,v 1.1.2.24 2010/12/08 21:57:11 dongqing Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Viewer for 2D slices
// Owners: Jeffrey Meade(jmeade@viatronix.com), Frank Dachille(frank@viatronix.com)


// includes
#include "stdafx.h"
#include "SliceChildView.h"
#include <sstream>
#include "MainAppFrame.h"
#include "ReaderGlobal.h"
#include "resource.h"
#include "HUconverter.h"
#include "Rect.h"
#include "RenderUtil.h"
#include "VCRenderUtils.h"
#include "ReportViewWnd.h"
#include "TextPromptWnd.h"
#include "Viatron.h"
#include "VCSettings.h"
#include "user.h"
#include "NavigationView.h"
#include "vxPolypSegmentation.h"
#include "vxHistogramUpdaterLabel.h"
#include "vcUtils.h"


// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.1.2.24 $"
#define ChooseAxialViewType(a)    ((a==eSAGITTAL)  ? eGVT_SAGITTAL : ((a==eCORONAL) ? eGVT_CORONAL : ((a==eAXIAL) ? eGVT_AXIAL : eGVT_CROSS_SECTION)))
#define ID_CUSTOM_WINDOWLEVEL (ID_WINDOWLEVEL_SAVENEW+1000)

//#define RENDERSTATS


// namespaces
using namespace std;
using namespace ColonViews::Layouts;
using namespace WinUtils;
using namespace ReaderLib;
using namespace vxCAD;
using namespace ReaderLib::SliceOrientations;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal button bar class

int4 SliceChildView::GLSButtonBar::HitTest(const Point2D<int4> & ptTest) const
{
  for ( GLSButtonBar::const_iterator it = begin(); it != end(); it++ )
  {
    if (it->second.m_rect.PtInRect(Point2D<int4>(ptTest.m_x,ptTest.m_y)))
      return it->first;
  }
  return -1;
} // HitTest()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SliceChildView input methods

/**
* Left mouse button down
*
* @param uFlags   Mouse/keyboard flags
* @param point    Point of mouse click
*/
void SliceChildView::OnLButtonDown(UINT uFlags, CPoint point)
{
  // All subsequent mouse input will be sent to this view
  SetCapture();

  Dataset& thisDataset  = *(m_sliceViewer.GetDataset());

  thisDataset.m_bUserInteracting = true;

  if( /*IsMainViewWindow() &&*/ (ViatronStudy::m_modeEndoView & ViewModes::AUTOFLY) )
  {
    ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
  }

  // stop autoflying (centerline).
  if ( (GetViewModes() & ViewModes::AUTOFLY2D) )
  {
    GetViewModes() -= ViewModes::AUTOFLY2D;
    SetAutoflyMode(false);
    m_sliceViewer.SetSkeletonIndex(-1.0F);
    if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
    {
      m_pNeighbourSliceView->SetAutoflyMode(false);
      m_pNeighbourSliceView->m_sliceViewer.SetSkeletonIndex(-1.0F);
    }
    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
    ForceRedraw();

    //const long UPDATE_OVERVIEW = 1;
    //const long RERENDER_OVERVIEW = 0;
    //GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));

    return;
  }

  // set up this point every time!
  SetStartPointWindowLevel(point);
  // pass it to the coupled view.
  if ( IsLinkedWithNeighbor() )
  {
    m_pNeighbourSliceView->SetStartPointWindowLevel(point);
  }

  m_sliceViewer.m_vfAnchorPt = Point2D<float4>( point.x, point.y ) /
    Point2D<float4>( m_sliceViewer.m_uDisplayWidth, m_sliceViewer.m_uDisplayHeight );
  m_sliceViewer.m_viStartMovePt = Point2D<uint4>( point.x, point.y );

  // need to pass the zooming parameters to the coupled slice view.
  if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
  {
    m_pNeighbourSliceView->m_sliceViewer.m_vfAnchorPt = m_sliceViewer.m_vfAnchorPt;
    m_pNeighbourSliceView->m_sliceViewer.m_viStartMovePt = m_sliceViewer.m_viStartMovePt;
  } // endif (m_bSPLocked).

  // ??? Left-click on option popup menus acts like right-click
  switch (GetPopupMenuID(0, point))
  {
    case IDM_WLPRESETS:
    case IDM_SLICE_ORIENTATION:
    case IDM_SLICE_ZOOM:
    case IDM_SLICE_RENDERMODE:
    {
        GLChildView::OnLButtonDown(uFlags, point);
        GLChildView::OnRButtonDown(uFlags, point);
        //ChildToView(point);
        //PostMessage( WM_RBUTTONDOWN, WPARAM(uFlags), MAKELPARAM(point.x,point.y));
        return;
    }
    default:
    {
      break;
    }
  } // switch(PopupMenuID)

  if ( IsMainViewWindow() && (GetViewModes() & ViewModes::SEGMENTPOLYP) )
  {
    CRect rct;  GetClientRect(&rct);
    if ( GetSegmentROIEditMode() == ROI_VOXEL_ADD || GetSegmentROIEditMode() == ROI_VOXEL_DEL || GetSegmentROIEditMode() == ROI_REGION_REM )
    {
      int4 iTest = m_btnbarVoxelSize.HitTest(Point2D<int4>(point.x,rct.Height()-point.y));
      if ( iTest >= 0 )
      {
        SetSegmentROIVoxelSize(iTest);
        return;
      }
    } // if voxel add or delete

    // then test for all edit modes
    int4 iEdit = m_btnbarEditMode.HitTest(Point2D<int4>(point.x,rct.Height()-point.y));
    if ( iEdit >= 0 )
    {
      ToggleROISegmentMode(SEGMENTROIMODE(iEdit));
      return;
    }
  }

  if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
  {
    if (IsMainViewWindow())
    {
      RoiMeasurement(MEASUREFUNC_START, point);
    }
  }
  else if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::LINEMEASURE)
  {
    if (IsMainViewWindow())
    {
      LineMeasurement(MEASUREFUNC_START, point);
    }
  } // end line measure mode;
  else if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ANNOTATION)
  {
    if (AddNewAnnotation(Point2D<int4>(point.x,point.y)))
    {
      ;
    }
    //ViatronStudy::GetViewModes(GetViewWndType()).Toggle(ViewModes::ANNOTATION);
    AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,0), 0);
  }
  else if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::SEGMENTPOLYP )
  {
    Point<float4> selectPoint = Calculate3DPoint(point);

    switch ( GetSegmentROIEditMode() )
    {
      case ROI_VOXEL_ADD:
      { 
        EditROIRegion(ROI_VOXEL_ADD, selectPoint);
        break;
      }
      case ROI_VOXEL_DEL:
      {
        EditROIRegion(ROI_VOXEL_DEL, selectPoint);
        break;
      }
      case ROI_REGION_REM:
      {
        EditROIRegion(ROI_REGION_REM, selectPoint);
        break;
      }
      case ROI_REGION_NEW:
      default:
      {
        if( uFlags & MK_CONTROL && rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::OutputSuspiciousROI()))
        {
          RecordAPointOnly(selectPoint);
        }
        else
        {
          SegmentRegion(selectPoint, Point2D<int4>(point.x,point.y));
        }
        break;
      }
    } // switch (roi editing mode)
  } // if in segment editing mode
  else if (uFlags & MK_CONTROL)
  {
    if (IsMainViewWindow() && (!(GetViewWndType() & eGVT_CROSS_SECTION)))
    {
      m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(true);
      m_sliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
      m_sliceViewer.m_viStartMovePt.m_y = (uint4) point.y;

      // need to pass the zooming parameters to the coupled slice view.
      if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        m_pNeighbourSliceView->m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(true);
        m_pNeighbourSliceView->m_sliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
        m_pNeighbourSliceView->m_sliceViewer.m_viStartMovePt.m_y = (uint4) point.y;
      }
    } // endif (IsMainViewWindow())
  }
  else if ((uFlags & (MK_CONTROL|MK_SHIFT)) == 0)   // Move world loc or annotation, zoom to point or zoom to selection
  {
    const Point<int4> clickLoc(point.x,point.y,m_sliceViewer.GetCurrentDisplayedSlice());
    if (m_sliceViewer.GetStatusOfRenderCamera() && (m_bCurrMovingWorldLoc = IsProximalWorldLoc(clickLoc)))
    {
      // Nothing more to do
    }
    else if ((m_pCurrAnnotation = GetProximalAnnotation(clickLoc)).first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
    {
      // Nothing more to do
    }
    else if (IsMainViewWindow()) // zoom only in main views
    {
      if (m_sliceViewer.m_vfAnchorPt.m_x < 0 || m_sliceViewer.m_vfAnchorPt.m_x > 1)
      {
        m_sliceViewer.m_vfAnchorPt
          = Point2D<float4>(point.x,point.y) / Point2D<float4>(m_sliceViewer.m_uDisplayWidth,m_sliceViewer.m_uDisplayHeight);
        m_sliceViewer.m_viStartMovePt = Point2D<uint4>(point.x, point.y);

        // need to pass the zooming parameters to the coupled slice view.
        if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
        {
          m_pNeighbourSliceView->m_sliceViewer.m_vfAnchorPt = m_sliceViewer.m_vfAnchorPt;
          m_pNeighbourSliceView->m_sliceViewer.m_viStartMovePt = m_sliceViewer.m_viStartMovePt;
        } // END ... if (m_bSPLocked).
      } // END ... if (m_sliceViewer.m_vfAnchorPt)
    } // END .. if zoom
  } // END ... if (ViatronStudy::ViewModes)

} // OnLButtonDown(UINT uFlags, CPoint point)


/**
* Left mouse button up
*
* @param uFlags   Mouse/keyboard flags
* @param point    Point of mouse click
*/
void SliceChildView::OnLButtonUp(UINT uFlags, CPoint point)
{
  // release mouse input capture
  ReleaseCapture();

  // Reset currently selected annotation if any
  m_pCurrAnnotation.first = m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end();

  // if were moving the world location, update all
  if ( m_bCurrMovingWorldLoc )
  {
    InvalidateViews( WPARAM(GLChildView::eGVT_SLICE | GLChildView::eGVT_NAVIGATION), 0);
  }
  m_bCurrMovingWorldLoc = false;

  switch(ViatronStudy::GetViewModes(GetViewWndType()))
  {
    case ViewModes::ROIMEASURE:
    {
      if (IsMainViewWindow())
      {
        RoiMeasurement(MEASUREFUNC_END, point);
      }
      break;
    }
    case ViewModes::LINEMEASURE:
    {
      if (IsMainViewWindow())
      {
        LineMeasurement(MEASUREFUNC_END, point);
      }
      break;
    }
    case ViewModes::SEGMENTPOLYP:
    {
      ForceRedraw( true );
      InvalidateEndoluminalView();
    }
    break;
    default:
    {
      m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
      m_sliceViewer.m_vfAnchorPt.m_x = -1;
      m_sliceViewer.m_vfAnchorPt.m_y = -1;
      m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
      if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        m_pNeighbourSliceView->m_sliceViewer.m_vfAnchorPt.m_x = -1;
        m_pNeighbourSliceView->m_sliceViewer.m_vfAnchorPt.m_y = -1;
        m_pNeighbourSliceView->m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
      }
      break;
    }
  } // endswitch m_currMode;

  // invalidate the start move point
  m_sliceViewer.m_viStartMovePt.m_x = 9999;
  m_sliceViewer.m_viStartMovePt.m_y = 9999;
  if ( m_pNeighbourSliceView != NULL )
  {
    m_pNeighbourSliceView->m_sliceViewer.m_viStartMovePt.m_x = 9999;
    m_pNeighbourSliceView->m_sliceViewer.m_viStartMovePt.m_y = 9999;
  }

  InvalidateView();

  GetDatasetRef().m_bUserInteracting = false;
} // OnLButtonUp( uFlags, point )


/**
* Dbl click Left mouse button
*
* @param uFlags   Mouse/keyboard flags
* @param point    Point of mouse click
*/
void SliceChildView::OnLButtonDblClk(UINT uFlags, CPoint point)
{
  if (!IsMainViewWindow())
  {
    GLChildView::OnLButtonDblClk(uFlags, point);
    return;
  }

  const Point<int4> clickLoc(point.x,point.y,m_sliceViewer.GetCurrentDisplayedSlice());
  if ((m_pCurrAnnotation = GetProximalAnnotation(clickLoc)).first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
  {
    // Jump to annotation
    OnCommand(MAKEWPARAM(ID_ANNOTATION_GOTO,0), 0);
  }
  else if ( IsMainViewWindow() )
  {
#if 0
    if (rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight()))
    {
      GetViewModes() += ViewModes::AUTOFLY2D;
      SetAutoflyMode(true);
      if (m_pNeighbourSliceView != NULL)
      {
        m_pNeighbourSliceView->SetAutoflyMode(false);
      }
    }
#else
    m_currClickPoint = point;

    const VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());

    if ( sliceModes == ViewModes::SEGMENTPOLYP )
    {
      CRect rct; GetClientRect(rct);
      Point2D< int4 > ptTest(point.x,rct.Height()-point.y);
      if ( (m_btnbarVoxelSize.HitTest(ptTest) >= 0) || (m_btnbarEditMode.HitTest(ptTest) >= 0) )
      {
        return;
      }
    }

    if ( sliceModes != ViewModes::LINEMEASURE && sliceModes != ViewModes::ROIMEASURE)
    {
      if (m_sliceViewer.GetDatasetType() != rdrGlobal.m_pCurrDataset->GetOrientation())
      {
        // Set the current orientation
        AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM(m_sliceViewer.GetDatasetType()), 0);
      }

      MoveViewPoint(point);
      OnMouseMove(0, point);
      if ( IsLinkedWithNeighbor() )
      {
        m_pNeighbourSliceView->MoveViewPoint(point);
        m_pNeighbourSliceView->OnMouseMove(0, point);
      }
      ForceRedraw( true );
    }
#endif

    m_sliceViewer.m_vfAnchorPt.m_x = -1;
    m_sliceViewer.m_vfAnchorPt.m_y = -1;
    m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
    if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
    {
      m_pNeighbourSliceView->m_sliceViewer.m_vfAnchorPt.m_x = -1;
      m_pNeighbourSliceView->m_sliceViewer.m_vfAnchorPt.m_y = -1;
      m_pNeighbourSliceView->m_sliceViewer.SetValueOfDisplayOverviewSliceGrasp(false);
    }
  }

} // OnLButtonDblClk( uFlags, point )


/**
* Right mouse button down
*
* @param uFlags   Mouse/keyboard flags
* @param point    Point of mouse click
*/
void SliceChildView::OnRButtonDown(UINT uFlags, CPoint point)
{

  SetCapture();

  m_currClickPoint = point;

  // stop autoflying (centerline).
  if ( (GetViewModes() & ViewModes::AUTOFLY2D) )
  {
    GetViewModes() -= ViewModes::AUTOFLY2D;
    SetAutoflyMode(false);
    m_sliceViewer.SetSkeletonIndex(-1.0F);
    if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
    {
      m_pNeighbourSliceView->SetAutoflyMode(false);
      m_pNeighbourSliceView->m_sliceViewer.SetSkeletonIndex(-1.0F);
    }

    InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
    ForceRedraw();

    //const long UPDATE_OVERVIEW = 1;
    //const long RERENDER_OVERVIEW = 0;
    //GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));

    return;
  }

  const VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());
  const bool bMeasure = (sliceModes == ViewModes::LINEMEASURE) || (sliceModes == ViewModes::ROIMEASURE);

  if (bMeasure)
  {
    if (IsMainViewWindow() && (uFlags & MK_CONTROL))
    {
      // CTRL-Right == Delete Measurement
      DeleteMeasurement(sliceModes == ViewModes::ROIMEASURE, point);
      RoiMeasurement(SliceChildView::MEASUREFUNC_UPDATE, CPoint(0,0));
    }
  }
  else
  {
    const Point<int4> clickLoc(point.x,point.y,m_sliceViewer.GetCurrentDisplayedSlice());
    if (m_sliceViewer.GetStatusOfRenderCamera() && (m_bCurrMovingWorldLoc = IsProximalWorldLoc(clickLoc)))
    {
      // Nothing else to do
    }
    else if (IsMainViewWindow() && (m_pCurrAnnotation = GetProximalAnnotation(clickLoc)).first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
    {
      // Nothing else to do
    }
  } // endif (bMeasure)

  if ( !bMeasure && uFlags & MK_CONTROL)
  {
    MoveViewPoint(point);
    if ( IsLinkedWithNeighbor() )
    {
      m_pNeighbourSliceView->MoveViewPoint(point);
    }
    ForceRedraw( true );
  }

  GLChildView::OnRButtonDown(uFlags, point);

} // OnRButtonDown( uFlags, point )


/**
* Right mouse button up
*
* @param uFlags   Mouse/keyboard flags
* @param point    Point of mouse click
*/
void SliceChildView::OnRButtonUp(UINT uFlags, CPoint point)
{
  ReleaseCapture();

  ////////////////////////////////////////////////////////
  // if were moving the world location, update all
  ////////////////////////////////////////////////////////
  if ( m_bCurrMovingWorldLoc )
  {
    InvalidateViews( WPARAM(GLChildView::eGVT_SLICE | GLChildView::eGVT_NAVIGATION), 0);
  } // END ... if we are moving the world location

  m_bCurrMovingWorldLoc = false;

  RoiMeasurement(SliceChildView::MEASUREFUNC_UPDATE, CPoint(0,0));

  GLChildView::OnRButtonUp(uFlags,point);

} // OnRButtonUp( uFlags, point )


/**
* Middle mouse button down
*
* @param uFlags   Mouse/keyboard flags
* @param point    Point of mouse click
*/
void SliceChildView::OnMButtonDown(UINT uFlags, CPoint point)
{
  GLChildView::OnMButtonDown(uFlags, point);
  SetCapture();

  GetDatasetRef().m_bUserInteracting = true;
	SetMouseWheelActionFlag(false);
} // OnMButtonDown( uFlags, point )


/**
* Middle mouse button up
*
* @param uFlags   Mouse/keyboard flags
* @param point    Point of mouse click
*/
void SliceChildView::OnMButtonUp(UINT uFlags, CPoint point)
{
  ReleaseCapture();
  m_dragscrollPt = Point2D<int4>(-1,-1);
  GetDatasetRef().m_bUserInteracting = false;
  GLChildView::OnMButtonUp(uFlags, point);
	SetMouseWheelActionFlag(true);
  InvalidateEndoluminalView();
} // OnMButtonUp( uFlags, point )


/**
* Mouse event handlers: mouse move.
*
* @param uFlags   Indicates whether various virtual keys are down.
* @param point    specifies the x- and y-coordinate of the cursor.
*/
void SliceChildView::OnMouseMove(UINT uFlags, CPoint point)
{
  if ( !m_sliceViewer.IsVolumeLoaded() )
  {
    return;
  }

  VtxViewModes sliceModes = ViatronStudy::m_modeOrthogView | ViatronStudy::m_modeObliqueView;
  if ( !IsMainViewWindow() && ( sliceModes & ViewModes::AUTOFLY2D ) && !GetAutoflyMode() )
  {
    return;
  }

  sliceModes = GetViewModes();
  if( m_bFirstMouseMove )
  {
    InvalidateEndoluminalView();
    m_bFirstMouseMove = false;
  }

  m_currMousePoint = point;

  // Get the Hounsfield units at the cursor position
  if ( m_bMouseWheelUpdated == false && m_bMouseWheelCalculate == true)
  {
    CPoint mousePt;
    GetCursorPos(&mousePt);
    ScreenToClient(&mousePt);
    m_iCurrHounsfieldValue = GetHounsfieldValue( Point2D<int4>(mousePt.x,mousePt.y), m_bCurrHounsfieldValueValid, m_currHounsfieldSamplePosition );
  }

  Dataset& thisDataset = *(m_sliceViewer.GetDataset());
  m_bRenderThis = true;

  TRACE2("PtWorld: %s; Saved: %s\n", ToAscii(thisDataset.m_submarine.GetPositionWorld()).c_str(),
    ToAscii(thisDataset.m_vLastUpdatedOrthoPtWorld).c_str());

  if (IsMainViewWindow() && (!(uFlags & MK_LBUTTON)))
  {
    m_sliceViewer.m_vfAnchorPt.m_x = static_cast< float4 >( point.x ) / static_cast< float4 >( m_sliceViewer.m_uDisplayWidth );
    m_sliceViewer.m_vfAnchorPt.m_y = static_cast< float4 >( point.y ) / static_cast< float4 >( m_sliceViewer.m_uDisplayHeight );
    if (Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
    {
      m_pNeighbourSliceView->m_sliceViewer.m_vfAnchorPt = m_sliceViewer.m_vfAnchorPt;
    }
  } // endif (!(uFlags & MK_LBUTTON))

  // added here to prevent from bad operation.
  if (IsMainViewWindow() && m_sliceViewer.GetValueOfDisplayOverviewSliceGrasp() )
  {
    Pan(m_sliceViewer, point);
    if ( IsLinkedWithNeighbor() )
    {
      m_pNeighbourSliceView->Pan(m_pNeighbourSliceView->m_sliceViewer, point);
    }
  }

  // Identify the current location of the mouse pointer w.r.t. world loc and annotations
  const Point<int4> mouseLoc(point.x,point.y,m_sliceViewer.GetCurrentDisplayedSlice());
  if (m_bMouseOverWorldLoc = IsProximalWorldLoc(mouseLoc))
  {
    m_pMouseOverAnnotation = AnnotationArrowRef(m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end(),false);
  }
  else //if (IsMainViewWindow()) // KAK you can grab the viewpoint in small window, why not the annotations also
  {
    m_pMouseOverAnnotation = GetProximalAnnotation(mouseLoc);
  }

  if (uFlags & MK_LBUTTON)
  {
    if (sliceModes == ViewModes::ROIMEASURE)
    {
      if (IsMainViewWindow())
      {
        RoiMeasurement(MEASUREFUNC_MOVE, point);
      }
    }
    else if (sliceModes == ViewModes::LINEMEASURE)
    {
      if (IsMainViewWindow())
      {
        LineMeasurement(MEASUREFUNC_MOVE, point);
      }
    }
    else if ((sliceModes == ViewModes::ANNOTATION) || (sliceModes == ViewModes::AUTOFLY2D))
    {
      // Nothing to do
    }
    else if ( (sliceModes == ViewModes::SEGMENTPOLYP) && ((uFlags&MK_SHIFT)==0) && ((uFlags&MK_CONTROL)==0) )
    {
      // TODO:
      switch ( SliceChildView::GetSegmentROIEditMode() )
      {
        case ROI_VOXEL_ADD:
        {
          EditROIRegion(ROI_VOXEL_ADD, Calculate3DPoint(point));
          break;
        }
        case ROI_VOXEL_DEL:
        {
          EditROIRegion(ROI_VOXEL_DEL, Calculate3DPoint(point));
          break;
        }
      }
    }
    else
    {
      if (uFlags & MK_SHIFT)
      {
        if (SliceChildView::m_eDbgShow == SHOW_NONE)
        {
          SetWindowLevel(point);
          if ( IsLinkedWithNeighbor() )
          {
            m_pNeighbourSliceView->SetWindowLevel(point);
          }
        }
      }
      else if (uFlags & MK_CONTROL)
      {
        if (IsMainViewWindow())
        {
          Pan(m_sliceViewer, point);
          if ( IsLinkedWithNeighbor() )
          {
            m_pNeighbourSliceView->Pan(m_pNeighbourSliceView->m_sliceViewer, point);
          }
        }
      }
      else
      {
        if (IsCaptured())
        {
          if (m_bCurrMovingWorldLoc)
          {
            MoveViewPoint(point);
            if ( IsLinkedWithNeighbor() )
            {
              m_pNeighbourSliceView->MoveViewPoint(point);
            }
          }
          else if (/*IsMainViewWindow() && */(m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end()))
          {
            CRect r;
            GetClientRect(&r);
            if (m_pCurrAnnotation.second)
            { // arrow tip selected
              m_pCurrAnnotation.first->m_position = m_sliceViewer.GetPickedLocationWorldCoord(float4(point.x)/float4(r.Width()),
                float4(point.y)/float4(r.Height()));
            }
            else
            { // arrow stem selected
              Point<float4> lookAtWorldLoc = m_sliceViewer.GetPickedLocationWorldCoord(float4(point.x)/float4(r.Width()),
                float4(point.y)/float4(r.Height()));

              m_pCurrAnnotation.first->m_orientation = Normal<float4>(m_pCurrAnnotation.first->m_position - lookAtWorldLoc);
            }

            GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
            InvalidateViews( GLChildView::eGVT_ALL, 0 );
          }
          else if (IsMainViewWindow())
          {
            ZoomSelect(point);
          }
        } // endif captured
      } // endif MK_CONTROL or MK_SHIFT
    } // Not Measure mode
  }
  else if (uFlags & MK_MBUTTON)
  {
    const int4 iPtChange(m_dragscrollPt.Y() - point.y);
    if (uFlags & MK_CONTROL)
    {
      if ( ((m_sliceViewer.m_sliceRenderer.GetRenderingMode() == vxSliceRenderer::ThinSlab) || (m_sliceViewer.m_sliceRenderer.GetRenderingMode() == vxSliceRenderer::MIP))
        && (m_dragscrollPt.Y() >= 0))
      {
        const float4 fMinSlabThickness( 1.0F );
        const float4 fMaxSlabThickness( 100.0F );
        float4 fNewSlabThickness( Bound( fMinSlabThickness, m_sliceViewer.m_sliceRenderer.GetSlabThickness() + iPtChange * 0.1F, fMaxSlabThickness ) );
        m_sliceViewer.m_sliceRenderer.SetSlabThickness( fNewSlabThickness );
        SliceViewer::m_fGlobalSlabThickness = m_sliceViewer.m_sliceRenderer.GetSlabThickness();
        InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
      }
      else if (m_dragscrollPt.Y() >= 0)
      {
        // scroll through dataset one slice per pixel of mouse movement
        ScrollSlice( iPtChange );
        SynchronizeNeighborSlice();
      }
    }
    else
    {
      // scroll slice
      if (m_dragscrollPt.Y() >= 0)
      {
        // scroll through dataset one slice per pixel of mouse movement
        ScrollSlice( iPtChange );
        SynchronizeNeighborSlice();
      }
    }

    m_dragscrollPt = Point2D<int4>(point.x, point.y);
  }
  else if (uFlags & MK_RBUTTON)
  {
    if ((sliceModes != ViewModes::LINEMEASURE) && (sliceModes != ViewModes::ROIMEASURE) && (sliceModes != ViewModes::ANNOTATION))
    {
      if (m_bCurrMovingWorldLoc || (uFlags & MK_SHIFT))
      {
        MoveViewDirection(point);
        if ( IsLinkedWithNeighbor() )
        {
          m_pNeighbourSliceView->MoveViewDirection(point);
        }
        InvalidateViews( WPARAM(GLChildView::eGVT_SLICE | GLChildView::eGVT_NAVIGATION ), 0);
      }
      else if (uFlags & MK_CONTROL)
      {
        MoveViewPoint(point);
        if ( IsLinkedWithNeighbor() )
        {
          m_pNeighbourSliceView->MoveViewPoint(point);
        }
      }
    } // endif (MoveViewDirection)
  } // endif (uFlags & MK_RBUTTON)

  ForceRedraw( true );
  GLChildView::OnMouseMove(uFlags, point);

} // OnMouseMove( uFlags, point )


/**
* Mouse wheel spin handler, scrolls slice
*
* @param uFlags   Mouse/keyboard flags
* @param zDelta   Amount of mouse wheel spin
* @param pt       Point at which wheel event occurs
* @return         TRUE
*/
BOOL SliceChildView::OnMouseWheel(UINT uFlags, int2 iZDelta, CPoint pt)
{
  // each mouse wheel click is 120 (WHEEL_DELTA) delta values
  const int4 iMove = iZDelta / WHEEL_DELTA;

  // indicate that the HU value should be updated
  SetMouseWheelActionFlag( true );

  ScrollSlice(iMove, eScrollDifference, pt, uFlags);

  // reset mouse wheel action flag
  // SetMouseWheelActionFlag( true );

  SynchronizeNeighborSlice();

  if (GetViewModes() & ViewModes::AUTOFLY2D)
  {
    GetViewModes() -= ViewModes::AUTOFLY2D;
  }
  SetAutoflyMode( false );

  m_sliceViewer.SetSkeletonIndex( -1.0F );

  ForceRedraw( true );
  InvalidateEndoluminalView();

  /** I think this code was intended to update the HU value in some round about way.
  *  By properly setting the mouse wheel action flag above, the HU is updated properly
  *  -george 4/25/2007

  CPoint point;
  GetCursorPos( &point );
  ScreenToClient( &point );
  OnMouseMove( 0, point );
  OnMouseMove( 0, point );
  */

  // return true to continue enabling mouse wheel scrolling
  return TRUE;

} // OnMouseWheel( uFlags, iZDelta, pt )


/**
* Called when mouse leaves the window.
*/
void SliceChildView::OnMouseLeave()
{
  VtxViewModes sliceModes = ViatronStudy::m_modeOrthogView | ViatronStudy::m_modeObliqueView;
  if( !IsMainViewWindow() &&( sliceModes & ViewModes::AUTOFLY2D )&& !GetAutoflyMode() )
  {
    return;
  }

  CPoint cpt; GetCursorPos(&cpt);
  ScreenToClient(&cpt);

  if( Rect<int4>(0,0, GetViewRect().Width(), GetViewRect().Height() ).PtInRect( cpt.x,cpt.y ) )
  {
    m_currMousePoint = CPoint(-cpt.x,-cpt.y);
  }
  else
  {
    m_currMousePoint = CPoint(9999,9999);
  }
  m_bCurrHounsfieldValueValid = false;

  m_bMouseOverWorldLoc = false;
  m_pMouseOverAnnotation = AnnotationArrowRef(m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end(),false);

  InvalidateEndoluminalView();
  ForceRedraw();
  GLChildView::OnMouseLeave();
  m_bFirstMouseMove = false;
} // OnMouseLeave()



/**
* keyboard event handler: key press.
*
* @param uChar     specifies the virtual key code of the given key.
* @param uRepCnt   specifies the repeat count.
* @param uFlags    specifies the scan code, key-transition code, previous key state, and context code.
*/
void SliceChildView::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  const bool bServiceUser(User::GetCurrentUser().GetType() == User::UserType_Service);

  switch (uChar)
  {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      SetWindowLevelPreset(uChar - '1' + eWLP_First);
      break;
    }
    case '0':
    {
      SetWindowLevelPreset(eWLP_First + 9);
      break;
    }

    case 192: // This is the `/~ key.  Don't ask why, I wish I knew.
    {
      SetWindowLevelPreset(eWLP_CTC);
      break;
    }

    case VK_LEFT:
    case VK_DOWN:
    case VK_UP:
    case VK_RIGHT:
    {
      CPoint mousePt;
      GetCursorPos(&mousePt);
      const int2 iDelta((uChar==VK_LEFT)||(uChar==VK_DOWN) ? -WHEEL_DELTA : +WHEEL_DELTA);
      OnMouseWheel(IsKeyStateDown(VK_CONTROL) ? MK_CONTROL : 0, iDelta, mousePt);
      if (GetViewModes() & ViewModes::AUTOFLY2D)
      {
        GetViewModes() -= ViewModes::AUTOFLY2D;
      }
      SetAutoflyMode(false);
      m_sliceViewer.SetSkeletonIndex(-1.0F);
      break;
    }

    case VK_PRIOR:  // Page Up   (increase)
    case VK_NEXT:   // Page Down (decrease)
    {
      CPoint mousePt;
      GetCursorPos(&mousePt);
      const int2 iDelta((uChar == VK_NEXT) ? -WHEEL_DELTA : +WHEEL_DELTA);
      OnMouseWheel(MK_CONTROL, iDelta, mousePt);
      if (GetViewModes() & ViewModes::AUTOFLY2D)
      {
        GetViewModes() -= ViewModes::AUTOFLY2D;
      }
      SetAutoflyMode(false);
      m_sliceViewer.SetSkeletonIndex(-1.0F);
      break;
    }

    case VK_HOME:
    {
      CPoint mousePt;
      GetCursorPos(&mousePt);
      ScrollSlice(0, eScrollAbsolute, mousePt);
      SynchronizeNeighborSlice();
      if (GetViewModes() & ViewModes::AUTOFLY2D)
      {
        GetViewModes() -= ViewModes::AUTOFLY2D;
      }
      SetAutoflyMode(false);
      m_sliceViewer.SetSkeletonIndex(-1.0F);
      InvalidateEndoluminalView();
      break;
    }
    case VK_END:
    {
      CPoint mousePt;
      GetCursorPos(&mousePt);
      ScrollSlice(m_sliceViewer.GetNumberOfSlices() - 1, eScrollAbsolute, mousePt);
      SynchronizeNeighborSlice();
      if (GetViewModes() & ViewModes::AUTOFLY2D)
      {
        GetViewModes() -= ViewModes::AUTOFLY2D;
      }
      SetAutoflyMode(false);
      m_sliceViewer.SetSkeletonIndex(-1.0F);
      InvalidateEndoluminalView();
      break;
    }

    case VK_ESCAPE:
    {
      if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ANNOTATION)
      {
        //ViatronStudy::GetViewModes(GetViewWndType()).Toggle(ViewModes::ANNOTATION);
        AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,0), 0);
      }
      break;
    }
#ifndef FINAL_RELEASE
    case 'D':
      if (bServiceUser)
      {
        SliceChildView::m_eDbgShow = (SliceChildView::m_eDbgShow != SHOW_DDB) ? SHOW_DDB : SHOW_NONE;
      }
      break;
    case 'I':
      if (bServiceUser)
      {
        SliceChildView::m_eDbgShow = (SliceChildView::m_eDbgShow != SHOW_INS) ? SHOW_INS : SHOW_NONE;
      }
      break;
    case 'K':
      if (bServiceUser)
      {
        SliceChildView::m_eDbgShow = (SliceChildView::m_eDbgShow != SHOW_CSI) ? SHOW_CSI : SHOW_NONE;
      }
      break;
    case 'P':
      if (bServiceUser)
      {
        SliceChildView::m_eDbgShow = (SliceChildView::m_eDbgShow != SHOW_PDR) ? SHOW_PDR : SHOW_NONE;
      }
      break;
    case 'R':
      if (GetViewModes() & ViewModes::SEGMENTPOLYP)
      {
        ToggleROISegmentMode(ROI_VOXEL_DEL);
      }
      else
      {
        OnCommand(MAKEWPARAM(ID_WINDOWLEVEL_RESET, 0), 0);

        m_sliceViewer.m_rZoom.Set(0.0F,0.0F,1.0F,1.0F);
        m_sliceViewer.m_rSelect.Set(9999,9999,9999,9999);
        SliceChildView::m_eDbgShow = SHOW_NONE;
        m_bUpdateRoiMeasurement = true;

        if ( IsLinkedWithNeighbor() )
        {
          m_pNeighbourSliceView->m_sliceViewer.m_rZoom.Set(0.0F,0.0F,1.0F,1.0F);
          m_pNeighbourSliceView->m_sliceViewer.m_rSelect.Set(9999,9999,9999,9999);
          m_pNeighbourSliceView->m_bUpdateRoiMeasurement = true;
        }
      }
      break;
    case 'T':
      if (bServiceUser)
      {
        SliceChildView::m_eDbgShow = (SliceChildView::m_eDbgShow != SHOW_BIOPSY) ? SHOW_BIOPSY : SHOW_NONE;
      }
      break;
    case 'V':
      if (bServiceUser)
      {
        if( m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
        {
          m_sliceViewer.m_bSliceRendererEnabled = !m_sliceViewer.m_bSliceRendererEnabled;
          m_sliceViewer.m_bForceSliceRender = true;
        }
      }
      break;
#endif

    case 'A': // see 'Z' for step backwards
    {
      if ( m_sliceViewer.StepFlight2D(/* step forward= */ true) == -1 )
      {
        // -1 return indicates to stop flying
        if (GetViewModes() & ViewModes::AUTOFLY2D)
          GetViewModes() -= ViewModes::AUTOFLY2D;
        SetAutoflyMode(false);
        m_sliceViewer.SetSkeletonIndex(-1.0F);

        ForceRedraw();
      }

      if (IsLinkedWithNeighbor())
      {
        if (m_pNeighbourSliceView->m_sliceViewer.StepFlight2D(true) == -1 )
        {
          m_pNeighbourSliceView->m_sliceViewer.SetSkeletonIndex(-1.0F);
          m_pNeighbourSliceView->ForceRedraw();
        }
      }

      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      break;
    }
    case 'B':
    {
      m_sliceViewer.GetDataset()->BuildHomogenityVolume();
      break;
    }
    case 'E':
    {
      ToggleROISegmentMode(ROI_VOXEL_ADD);
      break;
    }
    case 'F':
    {
      m_bFullScreen = !m_bFullScreen;
      OnCommand(MAKEWPARAM(IDM_VIEWWINDOW_FULLSCREEN,0), LPARAM(0));
      break;
    }
    case 'H':
    {
      rdrGlobal.m_bShowHomogenity = !rdrGlobal.m_bShowHomogenity & rdrGlobal.m_bHomogenityCreated;
      break;
    }
    case 'L': // labeling (quick-drop annotation)
    {
        CPoint mousePt;
        GetCursorPos(&mousePt);
        ScreenToClient(&mousePt);

        Dataset& thisDataset = *(m_sliceViewer.GetDataset());
        AddNewAnnotation(Point2D<int4>(mousePt.x,mousePt.y), m_sQuickAnnotationName + ToAscii(thisDataset.m_session.m_2DAnnotationList.size()+1));
        break;
    }

		case 'M':
		{
		  rdrGlobal.m_bDisplay2DRuler = !rdrGlobal.m_bDisplay2DRuler;
			break;
		}
    case 'Q':
    {
      ToggleROISegmentMode(ROI_REGION_NEW);
      break;
    }

#ifdef FINAL_RELEASE
    case 'R':
    {
      ToggleROISegmentMode(ROI_VOXEL_DEL);
      break;
    }
#endif

    case 'S':
    {
      OnCommand(MAKEWPARAM(ID_VIEWWINDOW_SHOWCLEANSED,0), LPARAM(0));
      break;
    }
    case 'W':
    {
      ToggleROISegmentMode(ROI_REGION_REM);
      break;
    }
    case 'X':
    {
      m_sliceViewer.SwitchBlockAndSmooth();
      if ( IsLinkedWithNeighbor() )
      {
        m_pNeighbourSliceView->m_sliceViewer.SwitchBlockAndSmooth();
      }
      break;
    }
    case 'Z': // see 'A' for step forward
    {
      if ( m_sliceViewer.StepFlight2D(/* step forward= */ false) == -1 )
      {
        // -1 return indicates to stop flying
        if (GetViewModes() & ViewModes::AUTOFLY2D)
          GetViewModes() -= ViewModes::AUTOFLY2D;
        SetAutoflyMode(false);
        m_sliceViewer.SetSkeletonIndex(-1.0F);

        ForceRedraw();
      }

      if (IsLinkedWithNeighbor())
      {
        if (m_pNeighbourSliceView->m_sliceViewer.StepFlight2D(false) == -1 )
        {
          m_pNeighbourSliceView->m_sliceViewer.SetSkeletonIndex(-1.0F);
          m_pNeighbourSliceView->ForceRedraw();
        }
      }

      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      break;
    }
    case VK_SPACE:
    {
      break;
    }
    default:
    {
      GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
      return;
    }
  }

  InvalidateViews( WPARAM(GLChildView::eGVT_ORTHOGONAL), 0);
  ForceRedraw();
  m_bRenderThis = true;

#ifndef FINAL_RELEASE
  switch (uChar)
  {
    // switch between rendering of 'blocked' or 'smoothed' pixels.
  case 189: // 'Ctrl-'
  case 109:
    if (GetAsyncKeyState(VK_CONTROL)&0x8000)
    {
      if (GetAsyncKeyState(VK_SHIFT)&0x8000)
      {
        // JRM note: Works to do NOTHING, as the value is never checked and never used
        // GetDatasetRef().CenterArrowInSliceView(false);
      }

      const float4 fScaler = 1.20f;
      float4 fx0 = m_sliceViewer.m_vfAnchorPt.m_x / (float4)m_sliceViewer.m_uDisplayWidth;
      float4 fy0 = m_sliceViewer.m_vfAnchorPt.m_y / (float4)m_sliceViewer.m_uDisplayHeight;
      m_sliceViewer.m_rZoom.m_left   = (m_sliceViewer.m_rZoom.m_left   - fx0) / fScaler + fx0;
      m_sliceViewer.m_rZoom.m_top    = (m_sliceViewer.m_rZoom.m_top    - fy0) / fScaler + fy0;
      m_sliceViewer.m_rZoom.m_right  = (m_sliceViewer.m_rZoom.m_right  - fx0) / fScaler + fx0;
      m_sliceViewer.m_rZoom.m_bottom = (m_sliceViewer.m_rZoom.m_bottom - fy0) / fScaler + fy0;
      m_bUpdateRoiMeasurement = true;

      // if locked, pass the zooming parameters to its neighbour.
      if ( IsLinkedWithNeighbor() )
      {
        SliceViewer * pSliceViewer = & m_pNeighbourSliceView->m_sliceViewer;
        float4 fx0 = pSliceViewer->m_vfAnchorPt.m_x / (float4)pSliceViewer->m_uDisplayWidth;
        float4 fy0 = pSliceViewer->m_vfAnchorPt.m_y / (float4)pSliceViewer->m_uDisplayHeight;
        pSliceViewer->m_rZoom.m_left   = (pSliceViewer->m_rZoom.m_left   - fx0) / fScaler + fx0;
        pSliceViewer->m_rZoom.m_top    = (pSliceViewer->m_rZoom.m_top    - fy0) / fScaler + fy0;
        pSliceViewer->m_rZoom.m_right  = (pSliceViewer->m_rZoom.m_right  - fx0) / fScaler + fx0;
        pSliceViewer->m_rZoom.m_bottom = (pSliceViewer->m_rZoom.m_bottom - fy0) / fScaler + fy0;
        m_pNeighbourSliceView->m_bUpdateRoiMeasurement = true;
      }
    }
    break;

  case 107: // 'Ctrl+'
  case 187:
    if (GetAsyncKeyState(VK_CONTROL)&0x8000)
    {
      if (GetAsyncKeyState(VK_SHIFT)&0x8000)
      {
        // JRM note: Works to do NOTHING, as the value is never checked and never used
        // GetDatasetRef().CenterArrowInSliceView(false);
      }

      if ( !IsMainViewWindow() ) break;

      const float4 fScaler = 1.20f;
      float4 fx0 = m_sliceViewer.m_vfAnchorPt.m_x / (float4)m_sliceViewer.m_uDisplayWidth;
      float4 fy0 = m_sliceViewer.m_vfAnchorPt.m_y / (float4)m_sliceViewer.m_uDisplayHeight;
      m_sliceViewer.m_rZoom.m_left   = (m_sliceViewer.m_rZoom.m_left   - fx0) * fScaler + fx0;
      m_sliceViewer.m_rZoom.m_top    = (m_sliceViewer.m_rZoom.m_top    - fy0) * fScaler + fy0;
      m_sliceViewer.m_rZoom.m_right  = (m_sliceViewer.m_rZoom.m_right  - fx0) * fScaler + fx0;
      m_sliceViewer.m_rZoom.m_bottom = (m_sliceViewer.m_rZoom.m_bottom - fy0) * fScaler + fy0;
      m_bUpdateRoiMeasurement = true;

      // if locked, pass the zooming parameters to its neighbour.
      if ( IsLinkedWithNeighbor() )
      {
        SliceViewer * pSliceViewer = & m_pNeighbourSliceView->m_sliceViewer;
        float4 fx0 = pSliceViewer->m_vfAnchorPt.m_x / (float4)pSliceViewer->m_uDisplayWidth;
        float4 fy0 = pSliceViewer->m_vfAnchorPt.m_y / (float4)pSliceViewer->m_uDisplayHeight;
        pSliceViewer->m_rZoom.m_left   = (pSliceViewer->m_rZoom.m_left   - fx0) * fScaler + fx0;
        pSliceViewer->m_rZoom.m_top    = (pSliceViewer->m_rZoom.m_top    - fy0) * fScaler + fy0;
        pSliceViewer->m_rZoom.m_right  = (pSliceViewer->m_rZoom.m_right  - fx0) * fScaler + fx0;
        pSliceViewer->m_rZoom.m_bottom = (pSliceViewer->m_rZoom.m_bottom - fy0) * fScaler + fy0;
        m_pNeighbourSliceView->m_bUpdateRoiMeasurement = true;
      }
      break;
    } // end-if (m_bCtrlPressed)

    // select an area, then zoom.
    if (m_sliceViewer.m_rSelect.m_left != 9999)
    {
      float fStartX, fEndX, fStartY, fEndY;
      // make sure the start point is on the left-top corner,
      // and the end point is on the right-bottom corner.
      if (m_sliceViewer.m_rSelect.m_left < m_sliceViewer.m_rSelect.m_right)
      {
        fStartX = (float4)m_sliceViewer.m_rSelect.m_left;
        fEndX   = (float4)m_sliceViewer.m_rSelect.m_right;
      }
      else
      {
        fStartX = (float4)m_sliceViewer.m_rSelect.m_right;
        fEndX   = (float4)m_sliceViewer.m_rSelect.m_left;
      }
      if (m_sliceViewer.m_rSelect.m_top < m_sliceViewer.m_rSelect.m_bottom)
      {
        fStartY = (float4)m_sliceViewer.m_rSelect.m_top;
        fEndY   = (float4)m_sliceViewer.m_rSelect.m_bottom;
      }
      else
      {
        fStartY = (float4)m_sliceViewer.m_rSelect.m_bottom;
        fEndY   = (float4)m_sliceViewer.m_rSelect.m_top;
      }

      Triple<float4> vAreaCenter((fStartX + fEndX)/2.0F, (fStartY + fEndY)/2.0F, 0.0F);
      // make sure the zooming keeps the same ratio.
      float4 fAspectRatio1 = (float4)m_sliceViewer.m_uDisplayHeight / (float4)m_sliceViewer.m_uDisplayWidth;
      float4 fAspectRatio2 = (fEndY-fStartY) / (fEndX-fStartX);
      if ( fAspectRatio2 > fAspectRatio1 )
      {
        float4 fDy = (vAreaCenter.m_x-fStartX) * fAspectRatio1;
        fStartY = vAreaCenter.m_y - fDy;
        fEndY   = vAreaCenter.m_y + fDy;
      }
      else
      {
        float4 fDx = (vAreaCenter.m_y-fStartY) / fAspectRatio1;
        fStartX = vAreaCenter.m_x - fDx;
        fEndX   = vAreaCenter.m_x + fDx;
      } // endif().

      float4 fScalerX = 1.0f / (float4)m_sliceViewer.m_uDisplayWidth;
      float4 fScalerY = 1.0f / (float4)m_sliceViewer.m_uDisplayHeight;
      Triple<float4> vNewAreaCenter(vAreaCenter.m_x * fScalerX, vAreaCenter.m_y * fScalerY, 0.0F);
      float4 fScaler = 1.0f / (fScalerX * (fEndX-fStartX));
      m_sliceViewer.m_rZoom.m_left   = (m_sliceViewer.m_rZoom.m_left   - vNewAreaCenter.m_x) * fScaler + 0.5;
      m_sliceViewer.m_rZoom.m_top    = (m_sliceViewer.m_rZoom.m_top    - vNewAreaCenter.m_y) * fScaler + 0.5;
      m_sliceViewer.m_rZoom.m_right  = (m_sliceViewer.m_rZoom.m_right  - vNewAreaCenter.m_x) * fScaler + 0.5;
      m_sliceViewer.m_rZoom.m_bottom = (m_sliceViewer.m_rZoom.m_bottom - vNewAreaCenter.m_y) * fScaler + 0.5;
    } // end-if (m_sliceViewer.m_rSelect.m_left != 9999)
    m_sliceViewer.m_rSelect.Set(9999,9999,9999,9999);
    break;

  default:
    GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
    return;
  } // endswitch(uChar).

  InvalidateView();
#endif

  // Update the Hounsfield units at the cursor position
  // This seems necessacy since the <SPACE> change the slice number.
  if ( m_bMouseWheelUpdated == false && m_bMouseWheelCalculate == true)
  {
    CPoint mousePt;
    GetCursorPos(&mousePt);
    ScreenToClient(&mousePt);
    m_iCurrHounsfieldValue = GetHounsfieldValue( Point2D<int4>(mousePt.x,mousePt.y), m_bCurrHounsfieldValueValid, m_currHounsfieldSamplePosition );
  }

  GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown( uChar, uRepCnt, uFlags )


/**
 * Sets mouse cursor
 *
 * @param pWnd       Pointer to current window
 * @param uHitTest   Identifies location of the mouse
 * @param uMessage   Type of message triggering set cursor message
 * @return           TRUE if cursor is set, FALSE otherwise (to set default cursor)
 */
BOOL SliceChildView::OnSetCursor(CWnd * pWnd, UINT uHitTest, UINT uMessage)
{
  UINT uCursorID = IDC_CROSSMC;
  LPCTSTR sStandardCursorID = NULL;

  // Show the measure icon if a main view window and mode is line or roi measure
  switch (ViatronStudy::GetViewModes(GetViewWndType()))
  {
    case ViewModes::LINEMEASURE:
    case ViewModes::ROIMEASURE:
    {
      uCursorID = IDC_MEASURE;
      break;
    }
    case ViewModes::ANNOTATION:
    {
      uCursorID = IDC_ANNOTATION;
      break;
    }
    case ViewModes::SEGMENTPOLYP:
    {
      switch (SliceChildView::GetSegmentROIEditMode())
      {
        case ROI_REGION_REM:
        {
          uCursorID = IDC_POINTER_PAINTCAN_REM;
          break;
        }
        case ROI_VOXEL_ADD:
        {
          uCursorID = IDC_POINTER_PAINTCAN_ADD;
          break;
        }
        case ROI_VOXEL_DEL:
        {
          uCursorID = IDC_POINTER_PAINTCAN_DEL;
          break;
        }
        case ROI_REGION_NEW:
        default:
        {
          uCursorID = IDC_POINTER_PAINTCAN_NEW;
          break;
        }
      }
      break;
    }
    default:
    {
      switch (uMessage)
      {
        case WM_LBUTTONDOWN:
        {
          if (GetAsyncKeyState(VK_CONTROL)&0x8000)
          {
            if (IsMainViewWindow() && !(GetViewWndType() & eGVT_CROSS_SECTION))
            {
              // CTRL-LEFT: Pan in main view
              uCursorID = IDC_PAN;
            }
          }
          else if (IsKeyStateDown(VK_SHIFT)) // SHIFT-LEFT: Adjust Window/Level
          {
            uCursorID = IDC_WINDOWLEVEL;
          }
          break;
        }
        case WM_RBUTTONDOWN:
        {
          if (IsKeyStateDown(VK_CONTROL))
          { ; }
          else if (IsKeyStateDown(VK_SHIFT))
          { ; }
          break;
        }
        case WM_MOUSEMOVE:
        default:
        {
          if (m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
          {
//              uCursorID = IDC_INDEXHAND;
          }
          else if (IsKeyStateDown(VK_CONTROL)
            && ((m_sliceViewer.m_sliceRenderer.GetRenderingMode() == vxSliceRenderer::ThinSlab) || (m_sliceViewer.m_sliceRenderer.GetRenderingMode() == vxSliceRenderer::MIP)) )
          {
            sStandardCursorID = IDC_SIZENS;
            break;
          }
          else
          {
            Rect<uint4> selRect(m_sliceViewer.m_rSelect);
            selRect.Normalize();
            CWnd * pParent = GetParent();
            CPoint currPt; GetCursorPos(&currPt);
            pParent->ScreenToClient(&currPt);
            currPt -= CPoint( GetViewRect().m_left, GetViewRect().m_top );

            if (selRect.PtInRect(currPt.x,currPt.y))
            {
              uCursorID = IDC_MAGNIFY;
            }
          }

          break;
        }
      } // endswitch(uMessage)
    } // default
    break;
  } // endswitch(ViatronStudy::ViewModes)

  if (sStandardCursorID != NULL)
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(sStandardCursorID));
  }
  else
  {
    SetCursor(AfxGetApp()->LoadCursor(uCursorID));
  }

  return TRUE;
} // OnSetCursor( pWnd, uHitTest, uMessage )


/**
 * Gets string IDs for mouse button use
 *
 * @param iLeftID    Left button use string resource ID
 * @param iMidID     Middle button use string resource ID
 * @param iRightID   Right button use string resource ID
 */
void SliceChildView::GetMouseKeyStringIDs(uint4 & iLeftID, uint4 & iMidID, uint4 & iRightID) const
{
  VtxViewModes sliceModes = ViatronStudy::GetViewModes(GetViewWndType());
  const bool bMeasure = (sliceModes == ViewModes::LINEMEASURE) || (sliceModes == ViewModes::ROIMEASURE);

  // middle wheel usually scrolls
  iMidID = IDS_SCROLL_SLICE;

  if (IsKeyStateDown(VK_SHIFT))
  {
    iLeftID = bMeasure ? IDS_MEASURE : IDS_WINDOWLEVEL;
    iRightID = bMeasure ? 0 : IDS_MOVE_VIEW_DIRECTION;
  }
  else if (IsKeyStateDown(VK_CONTROL))
  {
    // Pan allowed only on main view & non-cross-sectional view/non-view-dir view.
    iLeftID = (IsMainViewWindow() && !(GetViewWndType() & eGVT_CROSS_SECTION) ? IDS_PAN_VIEW : 0);

    // Delete allowed only when main view and measure option selected
    if (GetViewWndType() & eGVT_CROSS_SECTION)
    {
      iLeftID = bMeasure ? IDS_MEASURE : 0;
      iRightID = 0;
    }
    else
    {
      iLeftID = IsMainViewWindow() ? (bMeasure ? IDS_MEASURE : IDS_PAN_VIEW) : 0;
      iRightID = (bMeasure && IsMainViewWindow()) ? IDS_MEASURE_DELETE : IDS_MOVE_VIEW;
    }

    if (IsKeyStateDown(VK_CONTROL) && ((m_sliceViewer.m_sliceRenderer.GetRenderingMode() == vxSliceRenderer::ThinSlab)
                                        || (m_sliceViewer.m_sliceRenderer.GetRenderingMode() == vxSliceRenderer::MIP))
      )
    {
      iMidID = IDS_ADJUST_THICKNESS;
    }
  }
  else
  {
    if (m_bMouseOverWorldLoc || m_bCurrMovingWorldLoc)
    {
      iLeftID = IDS_MOVE_VIEW;
      iRightID = IDS_MOVE_VIEW_DIRECTION;
    }
    else if (sliceModes == ViewModes::SEGMENTPOLYP)
    {
      iLeftID = IDS_ROITOOL;
      iRightID = IDS_POPUP_MENU;
    }
    else
    {
      iLeftID = IsMainViewWindow() ? (bMeasure ? IDS_MEASURE : IDS_ZOOM_VIEW) : 0;
      iRightID = IDS_POPUP_MENU;
    }

  }
} // GetMouseKeyStringIDs( iLeftID, iMidID, iRightID )

/**
 * Returns ID of the popup menu to display
 *
 * @param uFlags   Mouse/key down flags
 * @param point    Point of mouse event triggering popup menu
 * @return         Resource ID of popup menu
 */
uint4 SliceChildView::GetPopupMenuID(UINT uFlags, CPoint point) const
{
  // Don't show a popup menu when control key is down
  if ((uFlags & MK_CONTROL) || !m_sliceViewer.IsVolumeLoaded())
  {
    return 0;
  }

  const Point2D<int4> pt2D(point.x, point.y);

  // Window/level presets on top/left
  if ( m_menuWinLev.PtInMenu(pt2D) )
  {
    return m_menuWinLev.GetID();
  }

  // Slice orientation on bottom/left
  if ( ((GetViewWndType() & eGVT_CROSS_SECTION) || IsMainViewWindow()) && m_menuSliceOrientation.PtInMenu(pt2D) )
  {
    return m_menuSliceOrientation.GetID();
  }

  // Zoom rect on bottom/right
  if ( IsMainViewWindow() && m_menuZoom.PtInMenu(pt2D) )
  {
    return m_menuZoom.GetID();
  }

  // render mode
  if ( m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() && m_menuRenderMode.PtInMenu(pt2D) )
  {
    return m_menuRenderMode.GetID();
  }

  if (m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
  {
    return IDM_ANNOTATION_POPUP;
  }

  if (m_bMouseOverWorldLoc)
  {
    return 0;
  }

  if ( IsMainViewWindow() && (GetViewModes() & ViewModes::SEGMENTPOLYP) )
  {
    return IDM_ROI_EDIT_TOOLS;
  }

  return IDM_SLICE_VIEW_WINDOW;

} // GetPopupMenuID( uFlags, point )


/**
 * Prepares popup menu for display by enabling/disabling/removing items as appropriate
 *
 * @param menu      Popup menu
 * @param uMenuID   Id of menu
 */
void SliceChildView::AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const
{
  switch (uMenuID)
  {
    case IDM_SLICE_VIEW_WINDOW:
    {
      AdjustSliceViewWindowMenu(menu, uMenuID);
      break;
    }
    case IDM_SLICE_ORIENTATION:
    case IDM_SLICE_CROSS_SECTION_ORIENTATION:
    {
      int4 iMenuCmd(0);
      switch (m_sliceViewer.GetOrientation())
      {
        case SliceOrientations::eAXIAL:
        {
          iMenuCmd = ID_SLICEORIENTATION_AXIAL;
          break;
        }
        case SliceOrientations::eSAGITTAL:
        {
          iMenuCmd = ID_SLICEORIENTATION_SAGITTAL;
          break;
        }
        case SliceOrientations::eCORONAL:
        {
          iMenuCmd = ID_SLICEORIENTATION_CORONAL;
          break;
        }
        case SliceOrientations::eCROSS_SECTION:
        {
          iMenuCmd = ID_SLICEORIENTATION_CROSSSECTIONAL;
          break;
        }
        case SliceOrientations::eVIEWDIR:
        {
          iMenuCmd = ID_SLICEORIENTATION_VIEWDIRECTION;
          break;
        }
        default:
        {
          iMenuCmd = 0;
          break;
        }
      } // endswitch(m_sliceViewer.orientation)
      menu.CheckMenuItem(iMenuCmd, MF_CHECKED|MF_BYCOMMAND);
      if ( (uMenuID != IDM_SLICE_CROSS_SECTION_ORIENTATION)
        && (m_sliceViewer.m_sliceRenderer.GetRenderingMode() == vxSliceRenderer::ThinSlab) )
      {
        menu.EnableMenuItem(ID_SLICEORIENTATION_CROSSSECTIONAL, MF_BYCOMMAND | MF_GRAYED);
        menu.EnableMenuItem(ID_SLICEORIENTATION_VIEWDIRECTION, MF_BYCOMMAND | MF_GRAYED);
      }

      break;
    } // case:IDM_SLICE_ORIENTATION, IDM_SLICE_CROSS_SECTION_ORIENTATION.

    case IDM_WLPRESETS:
    {
      // Prepare the current window/level selection
      for (int4 n(0), iNumPresets(menu.GetMenuItemCount()); n < iNumPresets; n++)
      {
        if ( (m_eCurrentWLPreset - eWLP_First) == n)
        {
          menu.SetDefaultItem(n, TRUE /*by position*/);
          menu.CheckMenuItem(n, MF_CHECKED  | MF_BYPOSITION);
        }
        else
        {
          menu.CheckMenuItem(n, MF_UNCHECKED | MF_BYPOSITION);
        }
      }

      int4 iID(ID_CUSTOM_WINDOWLEVEL);

      //{{ add user-custom presets
      for (int4 iCt(0); iCt < SliceChildView::m_mapWLSettings.m_vWL.size(); iCt++, iID++)
      {
        menu.InsertMenu(ID_WINDOWLEVEL_CUSTOM, MF_STRING | MF_BYCOMMAND, iID, SliceChildView::m_mapWLSettings.m_vWL[iCt].first.c_str());
        if (m_eCurrentWLPreset == iID)
        {
          menu.CheckMenuItem(iID, MF_CHECKED | MF_BYCOMMAND);
          menu.SetDefaultItem(iID, FALSE /*by command*/);
        }
      }
      menu.DeleteMenu(ID_WINDOWLEVEL_CUSTOM, MF_BYCOMMAND);
      //}} add user-custom presets

      if ( ((m_eCurrentWLPreset >= 0) && (m_eCurrentWLPreset < eWLP_Count)) || (menu.GetDefaultItem(0, FALSE) == -1) )
      {
        menu.EnableMenuItem(ID_WINDOWLEVEL_DELETESELECTED, MF_GRAYED | MF_BYCOMMAND);
      }

      if (SliceChildView::m_mapWLSettings.m_vWL.size() >= TransSettingsMap::iMaxWLVectorSize)
      {
        menu.EnableMenuItem(ID_WINDOWLEVEL_SAVENEW, MF_GRAYED | MF_BYCOMMAND);
      }

      break;
    } // case:IDM_WLPRESETS

    case IDM_SLICE_ZOOM:
      if (m_sliceViewer.GetZoomPercentage() == 100)
      {
        menu.EnableMenuItem(ID_SLICEZOOM_100, MF_GRAYED | MF_BYCOMMAND);
      }
      break;

    case IDM_ANNOTATION_POPUP:
    {
      menu.CheckMenuItem(ID_ANNOTATION_SHOWTEXT, ((m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
        && m_pCurrAnnotation.first->m_bVisible) ? MF_CHECKED : MF_UNCHECKED);

      // Disable Goto item for x-sec orientation, make default item for other orientations
      if ((m_sliceViewer.GetOrientation() == eCROSS_SECTION) || (m_sliceViewer.GetOrientation() == eVIEWDIR))
      {
        menu.EnableMenuItem(ID_ANNOTATION_GOTO, MF_GRAYED | MF_BYCOMMAND);
      }
      else
      {
        menu.SetDefaultItem(ID_ANNOTATION_GOTO, FALSE);
      }

      break;
    }

    case IDM_SLICE_RENDERMODE:
    {
      // Selected Rendering mode (MIP/Thin Slab/2D)
      switch (m_sliceViewer.m_sliceRenderer.GetRenderingMode())
      {
        case vxSliceRenderer::MIP:
        {
          menu.CheckMenuItem(ID_VIEWWINDOW_MIP, MF_CHECKED | MF_BYCOMMAND);
          break;
        }
        case vxSliceRenderer::ThinSlab:
        {
          menu.CheckMenuItem(ID_VIEWWINDOW_THINSLAB, MF_CHECKED | MF_BYCOMMAND);
          break;
        }
        case vxSliceRenderer::Default:
        default:
        {
          menu.CheckMenuItem(ID_VIEWWINDOW_2DSLICE, MF_CHECKED | MF_BYCOMMAND);
          break;
        }
      }

      // Available rendering modes
      if( !m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
      {
        menu.EnableMenuItem(ID_VIEWWINDOW_MIP, MF_GRAYED | MF_BYCOMMAND);
        menu.EnableMenuItem(ID_VIEWWINDOW_THINSLAB, MF_GRAYED | MF_BYCOMMAND);
      }
      if ((m_sliceViewer.GetOrientation() == SliceOrientations::eCROSS_SECTION) || (m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR))
      {
        menu.EnableMenuItem(ID_VIEWWINDOW_THINSLAB, MF_GRAYED | MF_BYCOMMAND);
      }
    }

    case IDM_ROI_EDIT_TOOLS:
    {
      const bool bRegionsExist( GetDatasetRef().m_suspiciousRegions.GetCount() > 0 );
      menu.EnableMenuItem( ID_REGIONEDITINGTOOLS_REGIONREMOVE, MF_BYCOMMAND | ( bRegionsExist ? MF_ENABLED : MF_GRAYED ) );
      menu.EnableMenuItem( ID_REGIONEDITINGTOOLS_VOXELADD, MF_BYCOMMAND | ( bRegionsExist ? MF_ENABLED : MF_GRAYED ) );
      menu.EnableMenuItem( ID_REGIONEDITINGTOOLS_VOXELDELETE, MF_BYCOMMAND | ( bRegionsExist ? MF_ENABLED : MF_GRAYED ) );

      switch (GetSegmentROIEditMode())
      {
        case ROI_REGION_NEW:
        {
          menu.CheckMenuItem(ID_REGIONEDITINGTOOLS_REGIONNEW, MF_CHECKED | MF_BYCOMMAND);
          break;
        }
        case ROI_REGION_REM:
        {
          menu.CheckMenuItem(ID_REGIONEDITINGTOOLS_REGIONREMOVE, MF_CHECKED | MF_BYCOMMAND);
          break;
        }
        case ROI_VOXEL_ADD:
        {
          menu.CheckMenuItem(ID_REGIONEDITINGTOOLS_VOXELADD, MF_CHECKED | MF_BYCOMMAND);
          break;
        }
        case ROI_VOXEL_DEL:
        {
          menu.CheckMenuItem(ID_REGIONEDITINGTOOLS_VOXELDELETE, MF_CHECKED | MF_BYCOMMAND);
          break;
        }
        default:
        {
          break;
        }
      }
    }
    break;

    default:
    {
      break;
    }
  } // endswitch(uMenuID)
} // AdjustPopupMenu( menu, uMenuID )


/**
 * Prepares IDM_SLICE_VIEW_WINDOW popup menu for display by enabling/disabling/removing items as appropriate
 *
 * @param menu      Popup menu
 * @param uMenuID   Id of menu
 */
void SliceChildView::AdjustSliceViewWindowMenu(CMenu & menu, const uint4 uMenuID) const
{
  // Check pseudo-color display if enabled
  menu.CheckMenuItem(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY,
    ((SliceChildView::m_eDbgShow==SHOW_BIOPSY) ? MF_CHECKED: MF_UNCHECKED) | MF_BYCOMMAND);
  if (m_sliceViewer.GetDataset()->GetModality() == Dataset::MR)
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_PSEUDOCOLORDISPLAY, MF_GRAYED | MF_BYCOMMAND);
  }
	
  // check/uncheck the popup submenu
  if (IsMainViewWindow())
  {
    menu.CheckMenuItem(ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW,
      (m_bUpdateEndoluminalView ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

    menu.EnableMenuItem(ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW, !GetFullObliqueMode() ? MF_ENABLED : MF_GRAYED);

    menu.CheckMenuItem(ID_VIEWWINDOW_ARROWINSLICECENTER,
      (SliceViewer::m_bArrowInSliceCenter ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

    menu.EnableMenuItem(ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW,  MF_ENABLED);
  }

	// Enable show-electronic-cleaning option
  const Fields & thisFd = m_sliceViewer.GetDataset()->m_fields;
  if (thisFd.m_sliceRendererData.IsValid())
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_SHOWCLEANSED, thisFd.IsHQDiffVolLoaded() && thisFd.IsElecCleansed() ? MF_ENABLED : MF_GRAYED);
  }
  else
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_SHOWCLEANSED, thisFd.DoesDiffVolExist() ? MF_ENABLED : MF_GRAYED);
  }

  // Check show-electronic-cleaning option
  menu.CheckMenuItem(ID_VIEWWINDOW_SHOWCLEANSED,
    (m_sliceViewer.m_bShowUncleansed ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

  Dataset& rDataset = *m_sliceViewer.GetDataset();
  if (rDataset.GetModality() == Dataset::MR)
  {
    menu.ModifyMenu(ID_VIEWWINDOW_SHOWCLEANSED, MF_BYCOMMAND | MF_STRING, ID_VIEWWINDOW_SHOWCLEANSED, LoadResourceString(IDS_SHOWSUBTRACTED));
    menu.CheckMenuItem(ID_VIEWWINDOW_SHOWCLEANSED,
      (m_sliceViewer.m_bShowUncleansed ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
  }

  // Show view position indicator only for non-orthogonal slices
  if (!(GetViewWndType() & GLChildView::eGVT_ORTHOGONAL))
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_SHOWPOSITION, MF_GRAYED|MF_BYCOMMAND);
  }
  else
  {
    menu.CheckMenuItem(ID_VIEWWINDOW_SHOWPOSITION,
      ((m_sliceViewer.GetStatusOfRenderCamera()) ? (MF_CHECKED) : (MF_UNCHECKED)) | MF_BYCOMMAND);
  }

  const uint4 vuAnnMeasOptions[] = { ID_VIEWWINDOW_LINEMEASUREMENT, ID_VIEWWINDOW_ROIMEASUREMENT,
    ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION, ID_SLICEVIEW_MEASUREMENTTOOLS_VOLUMEMEASUREMENT };
  const uint4 uCtAnnMeasOptions = sizeof(vuAnnMeasOptions) / sizeof(uint4);

  // Line/ROI measurement only when the view is a main view
  if (!IsMainViewWindow())
  {
    for (int4 iGray(0); iGray < uCtAnnMeasOptions; iGray++)
    {
      menu.EnableMenuItem(vuAnnMeasOptions[iGray], MF_GRAYED|MF_BYCOMMAND);
    }
  }

  // Check Line/ROI measurement options if selected
  const VtxViewModes viewModes(ViatronStudy::GetViewModes(GetViewWndType()));
  menu.CheckMenuItem(ID_VIEWWINDOW_LINEMEASUREMENT,
    ((viewModes & ViewModes::LINEMEASURE) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
  menu.CheckMenuItem(ID_VIEWWINDOW_ROIMEASUREMENT,
    ((viewModes & ViewModes::ROIMEASURE) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
  menu.CheckMenuItem(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,
    ((viewModes & ViewModes::ANNOTATION) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
  menu.CheckMenuItem(ID_SLICEVIEW_MEASUREMENTTOOLS_VOLUMEMEASUREMENT,
    ((viewModes & ViewModes::SEGMENTPOLYP) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

  // Gray out add annotation option when appropriate
  if ((m_sliceViewer.GetOrientation() == eCROSS_SECTION) || (m_sliceViewer.GetOrientation() == eVIEWDIR))
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION, MF_GRAYED | MF_BYCOMMAND);
  }

  // Snapshot option
  const bool bEnableReport(ReportLayout::HasPermissions() && ReportLayout::CanModifyReport());
  if (!IsMainViewWindow() || !m_sliceViewer.GetDataset()->IsLoadingComplete() || !bEnableReport)
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_SNAPSHOT, MF_GRAYED | MF_BYCOMMAND);
  }

  // Selected Rendering mode (MIP/Thin Slab/2D)
  switch (m_sliceViewer.m_sliceRenderer.GetRenderingMode())
  {
    case vxSliceRenderer::MIP:
    {
      menu.CheckMenuItem(ID_VIEWWINDOW_MIP, MF_CHECKED | MF_BYCOMMAND);
      break;
    }
    case vxSliceRenderer::ThinSlab:
    {
      menu.CheckMenuItem(ID_VIEWWINDOW_THINSLAB, MF_CHECKED | MF_BYCOMMAND);
      break;
    }
    case vxSliceRenderer::Default:
    default:
    {
      menu.CheckMenuItem(ID_VIEWWINDOW_2DSLICE, MF_CHECKED | MF_BYCOMMAND);
      break;
    }
  }

  // Coordinates display
  switch (SliceChildView::m_eCoordDisplay)
  {
    case SliceChildView::CoordDisplay::eVolume:
    {
      menu.CheckMenuItem(ID_VIEWWINDOW_SHOWVOLUMECOORDINATES, MF_CHECKED | MF_BYCOMMAND);
      break;
    }
    case SliceChildView::CoordDisplay::eWorld:
    {
      menu.CheckMenuItem(ID_VIEWWINDOW_SHOWWORLDCOORDINATES, MF_CHECKED | MF_BYCOMMAND);
      break;
    }
    case SliceChildView::CoordDisplay::eNone:
    default:
    {
      break;
    }
  }

  // Disable flight options menu when 2dflight not allowed
  if (! rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight()))
  {
    // NOTE: WARNING!!: HARD-CODED MENU POSITION FOLLOWS (popup-submenus can only be identified by position)
    menu.EnableMenuItem(6, MF_GRAYED | MF_BYPOSITION);
  }

  // Available rendering modes
  if( !m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_MIP, MF_GRAYED | MF_BYCOMMAND);
    menu.EnableMenuItem(ID_VIEWWINDOW_THINSLAB, MF_GRAYED | MF_BYCOMMAND);
  }
  if ((m_sliceViewer.GetOrientation() == SliceOrientations::eCROSS_SECTION) || (m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR))
  {
    menu.EnableMenuItem(ID_VIEWWINDOW_THINSLAB, MF_GRAYED | MF_BYCOMMAND);
  }
} // AdjustSliceViewWindow()

// undefines
#undef FILE_REVISION


// $Log: SliceChildView.Input.C,v $
// Revision 1.1.2.24  2010/12/08 21:57:11  dongqing
// Disable the HU value update when middle mouse button
// is depressed.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.23  2010/12/07 20:17:38  dongqing
// Remove the InvalidateEndoluminalView() for middle mouse down and
// slice slider bar drag for perfomance boosting.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.22  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.21  2010/03/30 21:31:16  dongqing
// Fix the bug that the coordinate and HU value display did not update
// when hit <space> to jump to next CAD findings
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.20  2009/12/18 22:01:13  dongqing
// buttons for segmented region voxel size modes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.19  2009/10/07 16:13:16  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.1.2.18  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.17  2009/01/23 22:02:58  jmeade
// Issue 6224: proper deletion of single/all volume measurement
//
// Revision 1.1.2.16  2009/01/23 19:21:53  jmeade
// Issue 6224: proper deletion of single/all volume measurement
//
// Revision 1.1.2.15  2009/01/21 22:02:59  jmeade
// Issue 6223: making 2d camera location a global option; reset after completing movie creation.
//
// Revision 1.1.2.14  2009/01/06 03:32:53  jmeade
// volume measure interface changes.
//
// Revision 1.1.2.13  2008/12/31 21:44:34  jmeade
// moved voxel size selection control into the sliceview
//
// Revision 1.1.2.12  2008/12/23 21:12:35  dongqing
// add ForceRedraw and InvalidateNavigationView when LeftButtonUp
// for PolypSegmentation mode
//
// Revision 1.1.2.11  2008/12/23 20:46:02  dongqing
// fix the bug that 3D view ROI did not refresh after 2D ROI editing
//
// Revision 1.1.2.10  2008/12/23 17:00:13  dongqing
// add bigger brush size option for the SRT ROI region editing tool
//
// Revision 1.1.2.9  2008/03/05 17:45:09  dongqing
// add RemoveSingleSROIFromList() for fixing the bug that user may
// delete all voxels in a ROI
//
// Revision 1.1.2.8  2008/03/03 20:40:17  dongqing
// add function for record single point user seleced in the SROI list
//
// Revision 1.1.2.7  2008/02/04 21:37:14  jmeade
// Issue 5999: ctc text for custom.
//
// Revision 1.1.2.6  2008/02/01 15:55:28  dongqing
// decomments
//
// Revision 1.1.2.5  2008/01/31 17:33:40  jmeade
// Issue 5928: limit of 5 user-defined window/level presets; polyp preset to ellipsis (~) key.
//
// Revision 1.1.2.4  2008/01/16 01:13:15  jmeade
// Issue 5959: put back removed code for scrolling during measure; timer for roi update after editing.
//
// Revision 1.1.2.3  2008/01/10 18:30:14  jmeade
// moving methods btw files.
//
// Revision 1.1.2.2  2008/01/09 22:57:23  jmeade
// splitting off functionality of SliceChildView into smaller files.
//
// Revision 1.1.2.1  2008/01/09 22:24:56  jmeade
// splitting off functionality of SliceChildView into smaller files.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SliceChildView.Input.C,v 1.1.2.24 2010/12/08 21:57:11 dongqing Exp $
// $Id: SliceChildView.Input.C,v 1.1.2.24 2010/12/08 21:57:11 dongqing Exp $
