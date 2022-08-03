// $Id: SliceChildView.C,v 1.86.2.73 2010/12/08 21:57:11 dongqing Exp $
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
#include "ApplicationSettings.h"
#include "vcUtils.h"


// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.86.2.73 $"
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


// static member declarations
const uint4 iSetScrollMessageID(SliceScrollBar::m_uScrollMessageID = WM_APP_CHILD_VIEW_MSG);
SliceChildView::DbgShowEnum SliceChildView::m_eDbgShow(SliceChildView::SHOW_NONE);
bool SliceChildView::m_bUpdateEndoluminalView(false);
SliceChildView::CoordDisplay::Enum SliceChildView::m_eCoordDisplay(SliceChildView::CoordDisplay::eNone);
bool SliceChildView::m_bWindowWidthIncreaseRight( true );
ColonViews::TransSettingsMap SliceChildView::m_mapWLSettings;
bool SliceChildView::m_bSettingsInitialRead(false);
std::string SliceChildView::m_sQuickAnnotationName;
SliceChildView::SEGMENTROIMODE SliceChildView::m_eROIEditMode(SliceChildView::ROI_REGION_NEW);
uint4 SliceChildView::m_uROIEditVoxelSize(1);
SliceChildView::SCROLLBARLOCATIONENUM SliceChildView::m_eScrollLoc(SliceChildView::SBO_BOTTOM);
GLChildView::ViewWndTypeEnum SliceChildView::m_eSegmentPolypModeView(eGVT_NONE);


int4 SliceChildView::m_veWindowLevelPresets[SliceChildView::eWLP_Count][4] =
{
  { SliceChildView::eWLP_CTC,         2000,    0, ID_WINDOWLEVEL_RESET    },
  { SliceChildView::eWLP_Abdomen,      350,   40, ID_WINDOWLEVEL_ABDOMEN  },
  { SliceChildView::eWLP_TaggedFecal, 1349,  385, ID_WINDOWLEVEL_ABDOMEN2 },
  { SliceChildView::eWLP_Lung,        1500, -500, ID_WINDOWLEVEL_LUNG     },
  { SliceChildView::eWLP_Bone,        2000,  500, ID_WINDOWLEVEL_BONE     },
  { SliceChildView::eWLP_Liver,        363,  128, ID_WINDOWLEVEL_LIVER    },    
};


/**
 * Constructor
 *
 * @param eSliceOrientation     Axis orientation for this slice instance
 * @param eDatasetOrientation   Patient position
 * @param hParent               Parent window
 */
SliceChildView::SliceChildView(const SliceOrientations::TypeEnum eSliceOrientation,
                               const DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent)
 : ViatronChildView(ChooseAxialViewType(eSliceOrientation), hParent, eDatasetOrientation),
 m_hScrollbar(HWND(NULL)), m_bMouseOverWorldLoc(false), m_bCurrMovingWorldLoc(false), m_currClickPoint(-1,-1),
 m_wlNormal(make_pair<int4,int4>(2000,0)), m_wlSecondary(make_pair<int4,int4>(2000,0)), m_bMeasureActive(false),
 m_eCurrentWLPreset(eWLP_CTC), m_iCurrHounsfieldValue(0), m_currMousePoint(CPoint(9999,9999)), m_bRenderThis(true),
 m_bMouseWheelUpdated(false), m_bMouseWheelCalculate(true), m_bCurrHounsfieldValueValid(false), m_bFullOblique(false),
 m_bUpdateRoiMeasurement(true), m_fDDBScale(1.0), m_iSpeedAccum(0), m_pNeighbourSliceView(NULL),
 m_pDatasetLastRendered((Dataset*) NULL), m_bShowHUValue(true), m_bAutoflyMode( false ), m_bFirstMouseMove( true ),
 m_iRoiSlice( -1 ), m_bFullScreen( false )
{
  // Set slice orientation, rendering mode
  SetOrientation(eSliceOrientation);
  m_sliceViewer.m_sliceRenderer.SetRenderingMode(SliceViewer::m_eGlobalRenderMode);

  // SetDatasetType(eDatasetOrientation);
  m_sliceViewer.SetDatasetType(eDatasetOrientation);
  m_sliceViewer.InitVolumeLoaded();

  // Load the popup menus to retrieve text when needed
  m_menuWinLev.m_winMenu.LoadMenu(IDM_WLPRESETS);
  m_menuSliceOrientation.m_winMenu.LoadMenu(IDM_SLICE_ORIENTATION);
  m_menuRenderMode.m_winMenu.LoadMenu(IDM_SLICE_RENDERMODE);

  m_pCurrAnnotation = AnnotationArrowRef(m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end(),false);
  m_pMouseOverAnnotation = AnnotationArrowRef(m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end(),false);

  m_dragscrollPt = Point2D<int4>(-1,-1);

  OnVolumeLoaded( 0, 0 );

  if ((GetDatasetRef().GetModality() == Dataset::MR) == (SliceChildView::m_eDbgShow == SHOW_BIOPSY))
    SliceChildView::m_eDbgShow = SHOW_NONE;

  m_bWindowWidthIncreaseRight = rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::WindowWidthIncreaseRight());

  m_iDbgCounter=0;

  if ( !SliceChildView::m_bSettingsInitialRead )
  {
    SliceChildView::RetrieveUserPreferredDisplayOptions();
    SliceChildView::m_mapWLSettings.ReadSettings();
    SliceChildView::m_bSettingsInitialRead = true;
  }

  // setup cheesy button bars
  m_btnbarVoxelSize[1] = m_btnbarVoxelSize[2] = GLSButton();
  m_btnbarEditMode[ROI_REGION_NEW] = GLSButton(Rect<int4>(), "*");
  m_btnbarEditMode[ROI_REGION_REM] = GLSButton(Rect<int4>(), "x");
  m_btnbarEditMode[ROI_VOXEL_ADD] = GLSButton(Rect<int4>(), "+");
  m_btnbarEditMode[ROI_VOXEL_DEL] = GLSButton(Rect<int4>(), "-");

} // SliceChildView( eSliceOrientation, eDatasetOrientation, hParent)


/**
 * Destructor
 */
SliceChildView::~SliceChildView()
{
  m_pDatasetLastRendered = (Dataset*) NULL;
} // ~SliceChildView()


/**
 * Initialize slice volume data
 *
 * @param wParam   Unused
 * @param lParam   Unused
 * @return         Result
 */
LRESULT SliceChildView::OnVolumeLoaded(WPARAM wParam, LPARAM lParam) // wParam,lParam: unused
{
  m_sliceViewer.InitVolumeLoaded();

  UpdateSliceScrollPos(true);

  Fields& rFields = m_sliceViewer.GetDataset()->m_fields;
  m_wlNormal = std::make_pair(rFields.m_defaultWL.first, rFields.m_defaultWL.second);
  m_wlSecondary =std::make_pair(rFields.m_secondaryWL.first, rFields.m_secondaryWL.second);

  if (!SliceViewer::m_bShowUncleansed)
  {
    SliceChildView::m_veWindowLevelPresets[SliceChildView::eWLP_CTC][1] = m_wlNormal.first;
    SliceChildView::m_veWindowLevelPresets[SliceChildView::eWLP_CTC][2] = m_wlNormal.second;
    m_sliceViewer.m_sliceRenderer.SetDefaultWindowLevel( m_wlNormal.first, m_wlNormal.second );
  }
  else
  {
    SliceChildView::m_veWindowLevelPresets[SliceChildView::eWLP_CTC][1] = m_wlSecondary.first;
    SliceChildView::m_veWindowLevelPresets[SliceChildView::eWLP_CTC][2] = m_wlSecondary.second;
    m_sliceViewer.m_sliceRenderer.SetDefaultWindowLevel( m_wlSecondary.first, m_wlSecondary.second  );
  }
  m_sliceViewer.m_bForceSliceRender = true;

  return 0;
} // OnVolumeLoaded(wParam, lParam)


/**
 * Set up slice viewer rendering context
 */
void SliceChildView::GLSetupRC()
{
  m_sliceViewer.SetupOpenGLContext();
} // GLSetupRC()


/**
 * called before the current layout changes (save values)
 */
void SliceChildView::OnLayoutChanging()
{
  if ( (ViatronStudy::GetViewModes(GetViewWndType()) & ViewModes::SEGMENTPOLYP) != 0 )
  {
    m_eSegmentPolypModeView = GetViewWndType();
  }
  else if ( m_eSegmentPolypModeView == GetViewWndType() )
  {
    m_eSegmentPolypModeView = eGVT_NONE;
  }
} // OnLayoutChanging()


/**
 * called after the current layout changes (load values)
 */
void SliceChildView::OnLayoutChanged()
{
  if ( (m_eSegmentPolypModeView != eGVT_NONE) && ((ViatronStudy::GetViewModes(m_eSegmentPolypModeView) & ViewModes::SEGMENTPOLYP) == 0) )
  {
    ViatronStudy::GetViewModes(m_eSegmentPolypModeView).Toggle(ViewModes::SEGMENTPOLYP);
  }
} // OnLayoutChanged()


/**
 * Called on view window resize
 *
 * @param iWidth    Width of child view
 * @param iHeight   Height of child view
 */
void SliceChildView::GLResize(GLsizei iWidth, GLsizei iHeight)
{

  glViewport(GetViewport().m_left, GetViewport().m_top, iWidth, iHeight);
  m_sliceViewer.ResizeDisplay(iWidth, iHeight);

  // w/l on top-left
  m_menuWinLev.Initialize(this,Rect<int4>(Point2D<int4>(m_iTextPadding,0),Point2D<int4>(90,15)),IDM_WLPRESETS);

  // render mode on top-right
  m_menuRenderMode.Initialize(this, Rect<int4>(Point2D<int>(iWidth-m_iTextPadding,0),Point2D<int4>(40,15)), IDM_SLICE_RENDERMODE);

  // orientation on bottom-left
  m_menuSliceOrientation.Initialize(this,Rect<int4>(Point2D<int4>(m_iTextPadding,iHeight-m_iTextPadding-m_iBottomExtraTextPadding),Point2D<int4>(120,15)),IDM_SLICE_ORIENTATION);

  // zoom on bottom-right
  m_menuZoom.Initialize(this,Rect<int4>(Point2D<int4>(iWidth-m_iTextPadding,iHeight-m_iTextPadding-m_iBottomExtraTextPadding),Point2D<int4>(40,15)),IDM_SLICE_ZOOM);

} // GLResize( iWidth, iHeight )


/**
 * WM_SIZE handler
 *
 * @param uType     Type
 * @param iWidth    New width
 * @param iHieght   New height
 */
void SliceChildView::OnSize(UINT uType, int4 iWidth, int4 iHeight )
{
  ViatronChildView::OnSize( uType, iWidth, iHeight );
  m_sliceViewer.ResizeDisplay(iWidth, iHeight);

  SliceScrollBar & scrollBar = GetScrollBar();
  if ( scrollBar )
  {
    if ( IsWindowVisible() )
    {
      scrollBar.ShowWindow(SW_SHOW);
      InvalidateAsscociatedControl( scrollBar );
    }
  }

  // Reset pan/zoom
  m_sliceViewer.m_rZoom.Set(0.0F,0.0F,1.0F,1.0F);
  m_sliceViewer.m_rSelect.Set(9999,9999,9999,9999);
  
  // if necessary, update ROI statistics
  if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
  {
    m_bUpdateRoiMeasurement = true;
  }

  m_btnbarEditMode.m_szBtn = Point2D<int4>(18,18);
  m_btnbarVoxelSize.m_szBtn = Point2D<int4>(18,18);
} // OnSize( uType, iWidth, iHeight )


/**
 * Event handler form the Windows Position Changing event
 *
 * @param   pWndPos       Window position
 */
void SliceChildView::OnWindowPosChanging( WINDOWPOS * pWndPos )
{
  ViatronChildView::OnWindowPosChanging( pWndPos );

  SliceScrollBar & scrollBar = GetScrollBar();
  if( scrollBar )
  {
    InvalidateAsscociatedControl( scrollBar );
  }
} // OnWindowPosChanging( pWndPos )


/**
 * Event handler form the Windows Position Changed event
 *
 * @param   pWndPos       Window position
 */
void SliceChildView::OnWindowPosChanged( WINDOWPOS * pWndPos )
{
  ViatronChildView::OnWindowPosChanged( pWndPos );
  
  SliceScrollBar & scrollBar = GetScrollBar();
  if( scrollBar )
  {
    const Rect<int4> & viewrect = GetViewRect();
    if ( m_bHorzScroll )
    {
      scrollBar.MoveWindow( viewrect.m_left - 1, viewrect.m_bottom + 3, viewrect.Width() + 2, 12 );
    }
    else
    {
      scrollBar.MoveWindow( viewrect.m_right + 3, viewrect.m_top - 1, 12, viewrect.Height() + 2 );
    }

    if ( (this->IsWindowVisible()==TRUE) && (scrollBar.IsWindowVisible()==FALSE) )
      scrollBar.ShowWindow(SW_SHOW);
    else if ( (this->IsWindowVisible()!=TRUE) && (scrollBar.IsWindowVisible()!=FALSE) )
      scrollBar.ShowWindow(SW_HIDE);

    InvalidateAsscociatedControl( scrollBar );
  }

} // OnWindowPosChanged( pWndPos )


/**
 * On idle handler, return true if any rendering on view should occur
 *
 * @return         True to render view, false otherwise
 */
bool SliceChildView::GLIdle()
{
  //if( GetDatasetRef().m_submarine.IsFlying() && !SliceChildView::m_bUpdateEndoluminalView )
  //  return false;

  // stop flying if mode is off
  const bool bAutofly2D = GetViewModes() & ViewModes::AUTOFLY2D;
  if ( !bAutofly2D && GetAutoflyMode() )
  {
    SetAutoflyMode( false );
  }

  if( GetAutoflyMode() && bAutofly2D )
  {
    if ( m_sliceViewer.AutoFlight2D() == -1 )
    {
      GetViewModes() -= ViewModes::AUTOFLY2D;
      SetAutoflyMode( false );
      m_sliceViewer.SetSkeletonIndex( -1.0F );
      if ( m_pNeighbourSliceView != NULL )
      {
        m_pNeighbourSliceView->SetAutoflyMode( false );
        m_pNeighbourSliceView->m_sliceViewer.SetSkeletonIndex( -1.0F );
      }
    }

    // Get the Hounsfield units at the cursor position
    if ( m_bMouseWheelUpdated == false && m_bMouseWheelCalculate == true)
    {
      CPoint mousePt;
      GetCursorPos(&mousePt);
      ScreenToClient(&mousePt);
      m_iCurrHounsfieldValue = GetHounsfieldValue(Point2D<int4>(mousePt.x,mousePt.y), m_bCurrHounsfieldValueValid, m_currHounsfieldSamplePosition);
    }

    InvalidateView();    

    if( Utilities::Datasets::IsLocked() && m_pNeighbourSliceView != NULL )
    {
      SynchronizeNeighborSlice();
    }

    return true;
  }
  // not flying in this view, but are we flying in the neighbour view?
  else if ( bAutofly2D && IsLinkedWithNeighbor())
  {
    return true;
  }

  return m_sliceViewer.m_bForceSliceRender;

} // GLIdle()


//////////////////////////////////////////////////////////////////////////////////////
// ... Methods


/**
 * Returns a temporary-use reference to the accompanying scrollbar instance
 *
 * @return   Temporary-use reference to the slice scrollbar
 */
SliceScrollBar & SliceChildView::GetScrollBar()
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
void SliceChildView::UpdateSliceScrollPos(const bool bSetRange)
{
  try
  {
    SliceScrollBar & scrollBar = GetScrollBar();
    if (!scrollBar)
    {
      return;
    }

    int4 iMaxSlice(m_sliceViewer.GetNumberOfSlices() - 1);
    if (iMaxSlice <= 0)   iMaxSlice = 1;

    if( (m_sliceViewer.GetOrientation() == SliceOrientations::eCROSS_SECTION || m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR) &&
         rdrGlobal.IsLoadingComplete() )
    {
      if (GetFullObliqueMode())
      {
        // init scrollbar
        float4 fCount = m_sliceViewer.GetDataset()->m_joinedSkeleton.GetNumOfElements();
        scrollBar.SetScrollRange(0, fCount-1);
        if ( m_bHorzScroll )
        {
          scrollBar.SetScrollPos(m_sliceViewer.GetSliceNumber());  
        }
        else
        {
          scrollBar.SetScrollPos(iMaxSlice+1 - m_sliceViewer.GetSliceNumber());
        }
      }
      else
      {
        int4 iObliqueIndexNear = static_cast< float4 >( m_sliceViewer.GetDataset()->m_fields.GetClosestSkeletonIndex( m_sliceViewer.GetDataset()->m_submarine.GetPositionVolume() ) );
        if (iObliqueIndexNear < 0) iObliqueIndexNear = 0;
        {
          // scrollBar.SetScrollRange(0, Min(iObliqueIndexNear, iMaxSlice+1));
          scrollBar.SetScrollRange(0, m_sliceViewer.GetNumberOfSlices()-1);
        }
      }
    }
    else if (bSetRange)
    {
      scrollBar.SetScrollRange(0, iMaxSlice);
    }

    if (!GetFullObliqueMode())
    {
      int4 iScrollPos(m_sliceViewer.GetCurrentOrthoSlice());
      if ( !m_bHorzScroll )
      {
        iScrollPos = iMaxSlice - iScrollPos;
      }

      if (scrollBar.GetScrollPos() != iScrollPos)
      {
        scrollBar.SetScrollPos(iScrollPos);
      }
    }
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceChildView", "UpdateSliceScrollPos" );
  }
} // UpdateSliceScrollPos( bSetRange )


/**
 * Called when received the message 'WM_APP_2DAUTOFLIGHT_UPDATE'
 *
 * @param iSkeletonIndex   Index of the colon segment
 */
void SliceChildView::OnUpdate2DAutoFlight(int4 iSkeletonIndex)
{
  m_sliceViewer.SetSkeletonIndex((float4)iSkeletonIndex);
} // OnUpdate2DAutoFlight( iSkeletonIndex )


/**
 * Called when view modes have changed
 *
 * @param uOldMode   Old view mode
 * @param uNevMode   New view mode
 */
void SliceChildView::OnViewModeChange(const uint4 uOldMode, const uint4 uNewMode)
{
  m_sliceViewer.m_measureLineList.clear();
  m_sliceViewer.m_measureAreaList.clear();

  if (uNewMode == ViewModes::AUTOFLY2D)
  {
    if ((m_pNeighbourSliceView != NULL) && !GetAutoflyMode() && !m_pNeighbourSliceView->GetAutoflyMode())
    {
      SetAutoflyMode(true);
    }
  }
  else if (uOldMode == ViewModes::AUTOFLY2D)
  {
    SetAutoflyMode(false);
  }

  OnSetCursor(this, 0,0);
} // OnViewModeChange( uOldMode, uNewMode )


/**
 * Message handler
 *
 * @param uMessage   Message
 * @param lParam     Message-dependent parameter
 * @return           Non-zero if message is handled
 */
int4 SliceChildView::OnMessage(uint2 uMessage, int4 lParam)
{
  try
  {
    switch (uMessage)
    {
      case WM_HSCROLL:    // Scroll to absolute position
      case WM_VSCROLL:    // Scroll by relative position
      {
        m_bRenderThis = true;

        uint4 uNewSlice  = LOWORD(lParam);
        uint4 uCurrSlice = m_sliceViewer.GetCurrentOrthoSlice();

        if ( !m_bHorzScroll )
        {
          uint4 uMax = m_sliceViewer.GetNumberOfSlices();
          uNewSlice = Bound(int4(0), int4(uMax)-1 - int4(uNewSlice), int4(uMax-1));
        }

        if (m_sliceViewer.IsOrthogonal())
        {
          // uint4 uSlicePerc = (LOWORD(lParam) * 0.01 * (m_sliceViewer.GetNumberOfSlices() - 1));

          if (uMessage == WM_HSCROLL)
          {
            // Scroll by difference with current slice
            m_sliceViewer.Scroll(uNewSlice - uCurrSlice);
            SynchronizeNeighborSlice();
          }
          else // message == WM_VSCROLL
          {
            m_sliceViewer.Scroll(uNewSlice);
            SynchronizeNeighborSlice();
          }

          // get the actual slice scrolled to
          uNewSlice = m_sliceViewer.GetCurrentOrthoSlice();

          // if necessary, update ROI statistics
          if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
            RoiMeasurement(MEASUREFUNC_UPDATE, CPoint(0,0));

          ForceRedraw( true );

          // return previous and new slice values
          return (uNewSlice<<16) + (uCurrSlice);
        }
        else // cross sectional
        {
          if (GetFullObliqueMode() && rdrGlobal.IsLoadingComplete() )
          { 
            uint4 uCurrSliceX = m_sliceViewer.GetSliceNumber(); ///GetDatasetRef().m_iXsectionScrollBarLocationIndex;

            if (uMessage == WM_HSCROLL)
            {
              m_sliceViewer.Scroll(uNewSlice - uCurrSliceX);
          
              SynchronizeNeighborSlice();
            }
            else // message == WM_VSCROLL
            {
              m_sliceViewer.Scroll(uNewSlice);
              SynchronizeNeighborSlice();
            }

            // get the actual slice scrolled to
            uNewSlice = m_sliceViewer.GetSliceNumber(); ///GetDatasetRef().m_iXsectionScrollBarLocationIndex;
          }
          else
          {
            m_sliceViewer.ScrollTo(uNewSlice);
            // if necessary, update ROI statistics
            if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
            {
              RoiMeasurement(MEASUREFUNC_UPDATE, CPoint(0,0));
            }
            m_sliceViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();

           /*   const long UPDATE_OVERVIEW = 1;
            const long RERENDER_OVERVIEW = 1;
            GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW, UPDATE_OVERVIEW));*/
          }

          ForceRedraw(true);

          InvalidateViews( GLChildView::eGVT_OVERVIEW, 0 );

          // return new slice and previous slice
          return (m_sliceViewer.GetCurrentDisplayedSlice() << 16);          
        }

        break;
      }
      case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      {
          m_sliceViewer.ResetCurrentVolumePosition();
          UpdateSliceScrollPos();
          SynchronizeNeighborSlice();
          m_bRenderThis = true;
          ForceRedraw();
          m_sliceViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
      }
        break;

      default:
      {
        return FALSE; // Command not handled
      }
    } // endswitch(uMessage)

    return TRUE;  // Command handled
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceChildView", "OnMessage" );
    return FALSE;
  }

  return TRUE;
} // OnMessage( uMessage, lParam )


/**
 * Set slice orientation.
 *
 * @param eOrientation  Orientation of the slice.
 */
void SliceChildView::SetOrientation(const SliceOrientations::TypeEnum eOrientation)
{
  m_sliceViewer.SetOrientation(eOrientation);
  m_sliceViewer.m_measureLineList.clear();

  switch (eOrientation)
  {
    case SliceOrientations::eAXIAL:
    {
      SetViewWndType(GLChildView::eGVT_AXIAL);
      // others, except axial shahow, should be unset.
      rdrGlobal.m_bSagittalShadow = (ViatronStudy::m_modeEndoView & ViewModes::SAGITTAL);
      rdrGlobal.m_bCoronalShadow = (ViatronStudy::m_modeEndoView & ViewModes::CORONAL);
      rdrGlobal.m_bObliqueShadow = (ViatronStudy::m_modeEndoView & ViewModes::CROSS_SECTION);

      SetFullObliqueMode(false);
      break;
    }
    case SliceOrientations::eSAGITTAL:
    {
      SetViewWndType(GLChildView::eGVT_SAGITTAL);
      // others, except sagittal shadow, should be unset.
      rdrGlobal.m_bAxialShadow = (ViatronStudy::m_modeEndoView & ViewModes::AXIAL);
      rdrGlobal.m_bCoronalShadow = (ViatronStudy::m_modeEndoView & ViewModes::CORONAL);
      rdrGlobal.m_bObliqueShadow = (ViatronStudy::m_modeEndoView & ViewModes::CROSS_SECTION);

      SetFullObliqueMode(false);
      break;
    }
    case SliceOrientations::eCORONAL:
    {
      SetViewWndType(GLChildView::eGVT_CORONAL);
      // others, except coronal shadow, should be unset.
      rdrGlobal.m_bAxialShadow = (ViatronStudy::m_modeEndoView & ViewModes::AXIAL);
      rdrGlobal.m_bSagittalShadow = (ViatronStudy::m_modeEndoView & ViewModes::SAGITTAL);
      rdrGlobal.m_bObliqueShadow = (ViatronStudy::m_modeEndoView & ViewModes::CROSS_SECTION);

      SetFullObliqueMode(false);
      break;
    }
    case SliceOrientations::eCROSS_SECTION:
    case SliceOrientations::eVIEWDIR:
    {
      SetViewWndType(GLChildView::eGVT_CROSS_SECTION);
      // others, except cross-section shadow, should be unset.
      rdrGlobal.m_bAxialShadow = (ViatronStudy::m_modeEndoView & ViewModes::AXIAL);
      rdrGlobal.m_bSagittalShadow = (ViatronStudy::m_modeEndoView & ViewModes::SAGITTAL);
      rdrGlobal.m_bCoronalShadow = (ViatronStudy::m_modeEndoView & ViewModes::CORONAL);

      SetFullObliqueMode(eOrientation == SliceOrientations::eCROSS_SECTION);
      break;
    }
  } // endswitch(eOrientation)

  UpdateSliceScrollPos(true);
  m_bUpdateRoiMeasurement = true;
  m_sliceViewer.m_rZoom = Rect<float4>(0.0F, 0.0F, 1.0F, 1.0F);

} // SetOrientation( eOrientation )


/**
 * Sets the orientation of the slice
 *
 * @param eOrientation   Slice orientation
 */
void SliceChildView::SetDatasetType(const DatasetOrientations::TypeEnum eOrientation)
{
  m_sliceViewer.SetDatasetType(eOrientation);
  m_sliceViewer.InitVolumeLoaded();
  UpdateSliceScrollPos(true);
} // SetDatasetType( eOrientation )


/**
 * Links the view with its accompanying scrollbar
 */
void SliceChildView::SetSliceScrollBar(const CWnd & wnd)
{
  m_hScrollbar = wnd;
  m_bHorzScroll = (wnd.GetStyle() & SBS_VERT) != SBS_VERT;
} // SetSliceScrollBar( wnd )


/**
 * Show/hide render camera
 *
 * @param bShow   True to show position camera
 */
void SliceChildView::ShowCamera(const bool bShow)
{
  SliceViewer::SetStatusOfRenderCamera(bShow);
} // ShowCamera( bShow )


/**
 * Get Status of render camera visibility
 *
 * @return   True to show position camera
 */
bool SliceChildView::GetIsCameraShowing()
{
  return SliceViewer::GetStatusOfRenderCamera();
} // GetIsCameraShowing()


/**
 * Retrieve global user preference across sessions
 */
void SliceChildView::RetrieveUserPreferredDisplayOptions()
{
  UpdateUserPreferredDisplayOptions(true);
} // RetrieveUserPreferredDisplayOptions()


/**
 * Store global user preference across sessions
 */
void SliceChildView::StoreUserPreferredDisplayOptions()
{
  UpdateUserPreferredDisplayOptions(false);
} // StoreUserPreferredDisplayOptions()


/**
 * Updates the value (on or off) of the show uncleansed global option for the current user
 *
 * @param bRetrieve   True to get value from registry, false to store value back to registry
 */
void SliceChildView::UpdateUserPreferredDisplayOptions(const bool bRetrieve)
{
  if (bRetrieve)
  {
    ApplicationSettings::UserSettings().TryGet< bool >( APP_SETTING_SHOW_UNCLEANSED, SliceViewer::m_bShowUncleansed );
    ApplicationSettings::UserSettings().TryGet< SliceChildView::DbgShowEnum >( APP_SETTING_PSUEDO_COLOR, SliceChildView::m_eDbgShow );
    ApplicationSettings::UserSettings().TryGet< SliceChildView::CoordDisplay::Enum >( APP_SETTING_DISPLAY_COORD, SliceChildView::m_eCoordDisplay );
    ApplicationSettings::UserSettings().TryGet< bool >( APP_SETTING_SHOW_EXTRA_COLONIC_FINDINGS, rdrGlobal.m_bShowExternalCadFindings );
    ApplicationSettings::UserSettings().TryGet( APP_SETTING_QUICK_ANNOTATION_NAME, SliceChildView::m_sQuickAnnotationName );
    ApplicationSettings::UserSettings().TryGet< SliceChildView::SCROLLBARLOCATIONENUM >( APP_SETTING_SCROLLBAR_LOCATION, SliceChildView::m_eScrollLoc );
  }
  else
  {
    ApplicationSettings::UserSettings().SetBool( APP_SETTING_SHOW_UNCLEANSED, SliceViewer::m_bShowUncleansed );
    ApplicationSettings::UserSettings().SetInt( APP_SETTING_PSUEDO_COLOR, SliceChildView::m_eDbgShow );
    ApplicationSettings::UserSettings().SetInt( APP_SETTING_DISPLAY_COORD, SliceChildView::m_eCoordDisplay );
    ApplicationSettings::UserSettings().SetBool( APP_SETTING_SHOW_EXTRA_COLONIC_FINDINGS, rdrGlobal.m_bShowExternalCadFindings );
    ApplicationSettings::UserSettings().Set( APP_SETTING_QUICK_ANNOTATION_NAME, SliceChildView::m_sQuickAnnotationName );
    ApplicationSettings::UserSettings().SetInt( APP_SETTING_SCROLLBAR_LOCATION, SliceChildView::m_eScrollLoc );
  }

} // UpdateUserPreferredDisplayOptions( bRetrieve )


/**
 * WM_COMMAND handler
 *
 * @param wParam   Command ID (LOWORD) / Type of control (HIGHWORD)
 * @param lParam   Control handle, or 0 if message not from a control
 * @return         Whether or not message was handled
 */
BOOL SliceChildView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
  // Window/level presets
    case ID_WINDOWLEVEL_RESET:
    {
      SetWindowLevelPreset(eWLP_CTC);
      break;
    }
    case ID_WINDOWLEVEL_ABDOMEN:
    {
      SetWindowLevelPreset(eWLP_Abdomen);
      break;
    }
    case ID_WINDOWLEVEL_ABDOMEN2:
    {
      SetWindowLevelPreset(eWLP_TaggedFecal);
      break;
    }
    case ID_WINDOWLEVEL_LUNG:
    {
      SetWindowLevelPreset(eWLP_Lung);
      break;
    }
    case ID_WINDOWLEVEL_BONE:
    {
      SetWindowLevelPreset(eWLP_Bone);
      break;
    }
    case ID_WINDOWLEVEL_LIVER:
    {
      SetWindowLevelPreset(eWLP_Liver);
      break;
    }
    case ID_WINDOWLEVEL_SAVENEW:
    {
      pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;

      std::string sName = WinUtils::TextPromptWnd::GetText(LPCTSTR(LoadResourceString(IDS_ENTERNEWSETTINGSNAME)), std::string(), TextPromptWnd::eSingleLine);
      if (sName.empty())      break;
      SliceChildView::m_mapWLSettings.ReadSettings();

      int4 iCurrentStg(-1);
      int4 iReply = IDYES;
      while ( (iReply != IDCANCEL) && (iCurrentStg = SliceChildView::m_mapWLSettings.FindWLSetting(sName)) >= 0)
      {
        iReply = AfxMessageBox("Duplicate name found: replace settings with new?", MB_YESNOCANCEL);
        switch (iReply)
        {
          case IDYES:
          {
            SliceChildView::m_mapWLSettings.m_vWL[iCurrentStg].second = rWL;
            SliceChildView::m_mapWLSettings.WriteSettings();
            SetWindowLevelPreset(iCurrentStg + eWLP_Count);
            iReply = IDCANCEL;
            break;
          }
          case IDNO:
          {
            sName = WinUtils::TextPromptWnd::GetText(LPCTSTR(LoadResourceString(IDS_ENTERNEWSETTINGSNAME)), std::string(), TextPromptWnd::eSingleLine);
            if (sName.empty())      break;
            break;
          }
          case IDCANCEL:
          default:
          {
            iReply = IDCANCEL;
            break;
          }
        }
      }

      if (iCurrentStg < 0)
      {
        SliceChildView::m_mapWLSettings.m_vWL.push_back(std::make_pair(sName, std::make_pair(rWL.first,rWL.second)));
        SliceChildView::m_mapWLSettings.WriteSettings();
        SetWindowLevelPreset((SliceChildView::m_mapWLSettings.m_vWL.size() - 1) + eWLP_Count);
      }
      break;
    } // case ID_WINDOWLEVEL_SAVENEW

    case ID_CUSTOM_WINDOWLEVEL:
    case ID_CUSTOM_WINDOWLEVEL+1:
    case ID_CUSTOM_WINDOWLEVEL+2:
    case ID_CUSTOM_WINDOWLEVEL+3:
    case ID_CUSTOM_WINDOWLEVEL+4:
    case ID_CUSTOM_WINDOWLEVEL+5:
    case ID_CUSTOM_WINDOWLEVEL+6:
    case ID_CUSTOM_WINDOWLEVEL+7:
    case ID_CUSTOM_WINDOWLEVEL+8:
    case ID_CUSTOM_WINDOWLEVEL+9:
    {
      SetWindowLevelPreset(LOWORD(wParam) - ID_CUSTOM_WINDOWLEVEL + eWLP_Count);
      break;
    }

    case ID_WINDOWLEVEL_DELETESELECTED:
    {
      if (lParam != LPARAM(0))
      {
        if (HWND(lParam) != HWND(this))
        {
          m_eCurrentWLPreset = eWLP_CTC;
        }
        break;
      }

      SliceChildView::m_mapWLSettings.ReadSettings();
      ColonViews::WindowLevelVector::iterator it = SliceChildView::m_mapWLSettings.m_vWL.begin();
      for (int4 iCt(int4(m_eCurrentWLPreset)-ID_CUSTOM_WINDOWLEVEL); it != SliceChildView::m_mapWLSettings.m_vWL.end() && iCt > 0; it++, iCt--)
        ;
      if (it != SliceChildView::m_mapWLSettings.m_vWL.end())
      {
        SliceChildView::m_mapWLSettings.m_vWL.erase(it);
        SliceChildView::m_mapWLSettings.WriteSettings();
        m_eCurrentWLPreset = eWLP_CTC;
        if (m_pNeighbourSliceView != NULL)
          m_pNeighbourSliceView->m_eCurrentWLPreset = eWLP_CTC;
      }

      MainAppFrame::GetMainFrame()->BroadcastMessage(GLChildView::eGVT_SLICE, WM_COMMAND,
        MAKEWPARAM(ID_WINDOWLEVEL_DELETESELECTED, 0), LPARAM(HWND(this)) );
      break;
    }

    // Set Orientations
    case ID_SLICEORIENTATION_AXIAL:
    {
      SetOrientation(SliceOrientations::eAXIAL);
      if ( Utilities::Datasets::IsLocked() && m_pNeighbourSliceView != NULL )
      {
        m_pNeighbourSliceView->SetOrientation( SliceOrientations::eAXIAL );
        m_pNeighbourSliceView->ForceRedraw();
      }
      if( rdrGlobal.IsLoadingComplete() )
      {
        UpdateLinkButton();
      }
      ForceRedraw();
      break;
    }
    case ID_SLICEORIENTATION_SAGITTAL:
    {
      SetOrientation(SliceOrientations::eSAGITTAL);
      if ( Utilities::Datasets::IsLocked() && m_pNeighbourSliceView != NULL )
      {
        m_pNeighbourSliceView->SetOrientation( SliceOrientations::eSAGITTAL );
        m_pNeighbourSliceView->ForceRedraw();
      }
      if( rdrGlobal.IsLoadingComplete() )
      {
        UpdateLinkButton();
      }
      ForceRedraw();
      break;
    }
    case ID_SLICEORIENTATION_CORONAL:
    {
      SetOrientation(SliceOrientations::eCORONAL);
      if ( Utilities::Datasets::IsLocked() && m_pNeighbourSliceView != NULL )
      {
        m_pNeighbourSliceView->SetOrientation( SliceOrientations::eCORONAL );
        m_pNeighbourSliceView->ForceRedraw();
      }
      if( rdrGlobal.IsLoadingComplete() )
      {
        UpdateLinkButton();
      }
      ForceRedraw();
      break;
    }
    case ID_SLICEORIENTATION_CROSSSECTIONAL:
    {
      if( rdrGlobal.IsLoadingComplete() )
      {
        SetOrientation(SliceOrientations::eCROSS_SECTION);
        if ( Utilities::Datasets::IsLocked() && m_pNeighbourSliceView != NULL )
        {
          m_pNeighbourSliceView->SetOrientation( SliceOrientations::eCROSS_SECTION );
          m_pNeighbourSliceView->ForceRedraw();
        }
        UpdateLinkButton();
        ForceRedraw();
      }
      break;
    }
    case ID_SLICEORIENTATION_VIEWDIRECTION:
    {
      if( rdrGlobal.IsLoadingComplete() )
      {
        SetOrientation(SliceOrientations::eVIEWDIR);
        UpdateLinkButton();
        ForceRedraw();
      }
      break;
    }
    // Reset slice zoom
    case ID_SLICEZOOM_100:
    {
      // cleverness described: using a loop so as not to duplicate code
      SliceChildView* vpViews[3] = {this, m_pNeighbourSliceView, (SliceChildView*)NULL};
      if ( ! IsLinkedWithNeighbor() ) { vpViews[1] = (SliceChildView*)NULL; }

      for (SliceChildView** ppView = vpViews; (*ppView) != (SliceChildView*) NULL; ppView++)
      {
        (*ppView)->m_sliceViewer.m_rZoom.Set(0.0F,0.0F,1.0F,1.0F);
        (*ppView)->m_sliceViewer.m_rSelect.Set(9999,9999,9999,9999);
        (*ppView)->m_sliceViewer.m_measureLineList.clear();

        // if necessary, update ROI statistics
        (*ppView)->m_bUpdateRoiMeasurement = true;
        (*ppView)->ForceRedraw();
      }

      break;
    }

    case ID_VIEWWINDOW_SHOWPOSITION: // Toggle view position indicator
    {
      m_sliceViewer.ToggleRenderCamera();
      ForceRedraw();
      break;
    }
    case ID_VIEWWINDOW_SHOWCLEANSED: // Toggle display cleansed/uncleansed slice
      {
        const Fields & fd = m_sliceViewer.GetDataset()->m_fields;
        const bool bEC(fd.m_sliceRendererData.IsValid() ? fd.IsHQDiffVolLoaded() && fd.IsElecCleansed() : fd.IsDiffVolLoaded());
        if (bEC)
        {
          SliceViewer::m_bShowUncleansed = !SliceViewer::m_bShowUncleansed;

          // laziness cleverly disguised as cleverness 
          SliceChildView* vpViews[3] = {this, m_pNeighbourSliceView, (SliceChildView*)NULL};
          CPoint mousePt;
          GetCursorPos( & mousePt );
          ScreenToClient( &mousePt );

          for (SliceChildView** ppView = vpViews; (*ppView) != (SliceChildView*) NULL; ppView++)
          {
            (*ppView)->m_sliceViewer.SetCurrentSliceNo(-1);

            if( (*ppView)->m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
            {
              pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? (*ppView)->m_wlNormal : (*ppView)->m_wlSecondary;
              (*ppView)->m_sliceViewer.m_sliceRenderer.SetDefaultWindowLevel( rWL.first, rWL.second );
            }

            // update the HU display
            (*ppView)->m_iCurrHounsfieldValue = (*ppView)->GetHounsfieldValue( Point2D<int4>( mousePt.x, mousePt.y ), m_bCurrHounsfieldValueValid, m_currHounsfieldSamplePosition );

            (*ppView)->m_bUpdateRoiMeasurement = true;
            (*ppView)->ForceRedraw();
          }
          m_iCurrHounsfieldValue = GetHounsfieldValue( Point2D<int4>(mousePt.x,mousePt.y), m_bCurrHounsfieldValueValid, m_currHounsfieldSamplePosition );
          InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
          ForceRedraw();
        }
  
      } // is electronic cleansing available for this study

      break;

      case ID_VIEWWINDOW_PSEUDOCOLORDISPLAY:
      {
        m_eDbgShow = (m_eDbgShow != SHOW_BIOPSY) ? SHOW_BIOPSY : SHOW_NONE;
        InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
        ForceRedraw();
        break;
      }
    case ID_VIEWWINDOW_JUMPTOVIEWPOSITION:    // Jump to current view position, i.e. cancel scrolling
      {
        m_sliceViewer.ResetCurrentVolumePosition();
        UpdateSliceScrollPos();
        SynchronizeNeighborSlice();
        ForceRedraw();
        if (m_pNeighbourSliceView != NULL)
        {
          m_pNeighbourSliceView->ForceRedraw();
        }
      }
      break;

    case ID_VIEWWINDOW_LINEMEASUREMENT:
    {
      ViatronStudy::GetViewModes(GetViewWndType()).Toggle(ViewModes::LINEMEASURE);
      m_sliceViewer.m_measureAreaList.clear();
      m_sliceViewer.m_measureLineList.clear();
      if ( Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        m_pNeighbourSliceView->m_sliceViewer.m_measureAreaList.clear();
        m_pNeighbourSliceView->m_sliceViewer.m_measureLineList.clear();
        m_pNeighbourSliceView->ForceRedraw();
      }
      ForceRedraw();
    break;
    }
    case ID_VIEWWINDOW_ROIMEASUREMENT:
    {
      ViatronStudy::GetViewModes(GetViewWndType()).Toggle(ViewModes::ROIMEASURE);
      m_sliceViewer.m_measureAreaList.clear();
      m_sliceViewer.m_measureLineList.clear();
      if ( Utilities::Datasets::IsLocked() && (m_pNeighbourSliceView != NULL))
      {
        m_pNeighbourSliceView->m_sliceViewer.m_measureAreaList.clear();
        m_pNeighbourSliceView->m_sliceViewer.m_measureLineList.clear();
        m_pNeighbourSliceView->ForceRedraw();
      }
      ForceRedraw();
      break;
    }
    case ID_SLICEVIEW_MEASUREMENTTOOLS_VOLUMEMEASUREMENT:
    {
      ToggleROISegmentMode(m_eROIEditMode);
      break;
    }
    case ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION:
    {
      AddNewAnnotation(Point2D<int4>(m_currClickPoint.x, m_currClickPoint.y));
      ForceRedraw();
      break;
    }
    case ID_VIEWWINDOW_UPDATEENDOLUMINALVIEW: // if update navigation view while flying (centerline)
    {
      m_bUpdateEndoluminalView = !m_bUpdateEndoluminalView;
      break;
    }
    case ID_VIEWWINDOW_ARROWINSLICECENTER: // if keep the arrow in the slice center.
    {
      SliceViewer::m_bArrowInSliceCenter = !SliceViewer::m_bArrowInSliceCenter;
      m_bUpdateRoiMeasurement = true;
      ForceRedraw();
      break;
    }
    case ID_VIEWWINDOW_SNAPSHOT:
    {
      /// See handler(s) in EndoOrthogonalViewFrame::ViewWnd and SupineProneViewWnd
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
      break;
    }
    case ID_VIEWWINDOW_SWITCHFLIGHTDIRECTION: // change flight direction.
    {
      if ((Utilities::Datasets::IsLocked()) && (m_pNeighbourSliceView!=NULL))
      {
        rdrGlobal.m_supine.m_submarine.ToggleFlightDirection();
        rdrGlobal.m_prone.m_submarine.ToggleFlightDirection();
        InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
        ForceRedraw();
        m_pNeighbourSliceView->ForceRedraw();
      }
      else
      {
        Dataset& myDatasetRef = *m_sliceViewer.GetDataset();
        myDatasetRef.m_submarine.ToggleFlightDirection();
      }
      break;
    }
    // Annotation popup menu
    case ID_ANNOTATION_EDITTEXT:
    {
      if (m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
      {
        const std::string sEntry = TextPromptWnd::GetText(LPCTSTR(LoadResourceString(IDS_ANNOTATION_TEXT_PROMPT)),
          m_pCurrAnnotation.first->m_sName, TextPromptWnd::eSingleLine);
        if (!sEntry.empty())
        {
          m_pCurrAnnotation.first->m_sName = sEntry;
          ForceRedraw();
        }

        m_pCurrAnnotation.first = m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end();
      }

      break;
    }
    case ID_ANNOTATION_DELETE:
    {
      if (m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
      {
        CString sPrompt;
        sPrompt.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_ANNOTATION));
        if (AfxMessageBox(sPrompt, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
          m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.erase(m_pCurrAnnotation.first);
          InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
          ForceRedraw();
        }

        m_pCurrAnnotation.first = m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end();
      }

      break;
    }
    case ID_ANNOTATION_GOTO:
    {
      if (m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
      {
        m_pCurrAnnotation.first->GoTo(*(m_sliceViewer.GetDataset()));
        if (IsLinkedWithNeighbor())
        {
          m_pCurrAnnotation.first->GoTo(*(m_pNeighbourSliceView->m_sliceViewer.GetDataset()));
        }

        InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
        ForceRedraw();

        m_pCurrAnnotation.first = m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end();
      }

      break;
    }
    case ID_ANNOTATION_SHOWTEXT:
    {
      if (m_pCurrAnnotation.first != m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end())
      {
        m_pCurrAnnotation.first->m_bVisible = !m_pCurrAnnotation.first->m_bVisible;

        m_pCurrAnnotation.first = m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end();
      }
      ForceRedraw();

      break;
    }

    // Display type
    case ID_VIEWWINDOW_2DSLICE:
    {
      m_sliceViewer.m_sliceRenderer.SetRenderingMode(vxSliceRenderer::Default);
      SliceViewer::m_eGlobalRenderMode = vxSliceRenderer::Default;
      InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
      ForceRedraw();
      break;
    }
    case ID_VIEWWINDOW_THINSLAB:
    {
      m_sliceViewer.m_sliceRenderer.SetRenderingMode(vxSliceRenderer::ThinSlab);
      SliceViewer::m_eGlobalRenderMode = vxSliceRenderer::ThinSlab;
      InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
      ForceRedraw();
      break;
    }
    case ID_VIEWWINDOW_MIP:
    {
      m_sliceViewer.m_sliceRenderer.SetRenderingMode(vxSliceRenderer::MIP);
      SliceViewer::m_eGlobalRenderMode = vxSliceRenderer::MIP;
      InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
      ForceRedraw();
      break;
    }
    // Coordinates display
    case ID_VIEWWINDOW_SHOWVOLUMECOORDINATES:
    {
      SliceChildView::m_eCoordDisplay = (SliceChildView::m_eCoordDisplay != SliceChildView::CoordDisplay::eVolume)
        ? SliceChildView::CoordDisplay::eVolume : SliceChildView::CoordDisplay::eNone;
      break;
    }
    case ID_VIEWWINDOW_SHOWWORLDCOORDINATES:
    {
      SliceChildView::m_eCoordDisplay = (SliceChildView::m_eCoordDisplay != SliceChildView::CoordDisplay::eWorld)
        ? SliceChildView::CoordDisplay::eWorld : SliceChildView::CoordDisplay::eNone;
      break;
    }
    case ID_REGIONEDITINGTOOLS_REGIONNEW:
    {
      OnKeyDown('Q', 0, 0);
      break;
    }
    case ID_REGIONEDITINGTOOLS_REGIONREMOVE:
    {
      OnKeyDown('W', 0, 0);
      break;
    }
    case ID_REGIONEDITINGTOOLS_VOXELADD:
    {
      OnKeyDown('E', 0, 0);
      break;
    }
    case ID_REGIONEDITINGTOOLS_VOXELDELETE:
    {
      OnKeyDown('R', 0, 0);
      break;
    }
    case ID_SLICE_CHILD_FORCE_REDRAW:
    {
      // this is necessary since slice number is determined by rendering of the slice 
      SetRenderOnlyBackground( false );

      ForceRedraw();
      if( lParam && IsLinkedWithNeighbor() )
      {
        SynchronizeNeighborSlice();
        m_pNeighbourSliceView->ForceRedraw();
      }
      break;
    }
    default:
    {
      // Some other command not handled by this view
      return ViatronChildView::OnCommand(wParam, lParam);
    }
  } // endif seln value

  InvalidateView();
  return TRUE;  // Command handled
} // OnCommand( wParam, lParam )


/**
 * Updates the link button (enabling/disabling as appropriate)
 */
void SliceChildView::UpdateLinkButton()
{
  // get the button
  SkinButton * pLinkBtn = dynamic_cast< SkinButton * > ( GetParent()->GetDlgItem( IDC_TEMPLATE_LINKVIEWS ) );
  if ( pLinkBtn == NULL )
  {
    // no link button on this view
    return;
  }

  // check for a neighbor
  if ( m_pNeighbourSliceView == NULL )
  {
    // no neighbor - don't allow to use button
    pLinkBtn->EnableWindow( false );
  }
  else
  {
    // have a neighbor - only allow linking if the orientations match and they are axial, sagittal, coronal, or cross-sectional
    const bool bOrientationsMatch = m_sliceViewer.GetOrientation() == m_pNeighbourSliceView->m_sliceViewer.GetOrientation();
    const bool bOrientationsLinkable =
      m_sliceViewer.GetOrientation() == SliceOrientations::eAXIAL ||
      m_sliceViewer.GetOrientation() == SliceOrientations::eSAGITTAL ||
      m_sliceViewer.GetOrientation() == SliceOrientations::eCORONAL ||
      m_sliceViewer.GetOrientation() == SliceOrientations::eCROSS_SECTION;
    pLinkBtn->EnableWindow( bOrientationsMatch && bOrientationsLinkable );
  }

} // UpdateLinkButton()


/**
 * Invalidate (mark for redraw) view
 */
void SliceChildView::InvalidateView()
{
  
  m_sliceViewer.m_bForceSliceRender = true;

  //const long UPDATE_OVERVIEW = 1;
  //const long RERENDER_OVERVIEW = 0;
  //GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));

  m_sliceViewer.GetDataset()->m_fields.m_volumeRender.FlushAccumulationBuffer();
} // InvalidateView()


/**
 * Invalidates (mark for redraw) the navigation view
 */
void SliceChildView::InvalidateEndoluminalView()
{
  // Trigger an endoscopic view update if showing the relevant slice shadow
  // TODO: Make specific to dataset, Add similar code for Overview slice shadows
  switch (m_sliceViewer.GetOrientation())
  {
    case SliceOrientations::eAXIAL:
    {
      if ((ViatronStudy::m_modeEndoView & ViewModes::AXIAL) || rdrGlobal.m_bAxialShadow)
      {
        InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
      }
      break;
    }
    case SliceOrientations::eSAGITTAL:
    {
      if ((ViatronStudy::m_modeEndoView & ViewModes::SAGITTAL) || rdrGlobal.m_bSagittalShadow)
      {
        InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
      }
      break;
    }
    case SliceOrientations::eCORONAL:
    {
      if ((ViatronStudy::m_modeEndoView & ViewModes::CORONAL) || rdrGlobal.m_bCoronalShadow)
      {
        InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
      }
      break;
    }
    case SliceOrientations::eCROSS_SECTION:
    case SliceOrientations::eVIEWDIR:
    {
      if ((ViatronStudy::m_modeEndoView & ViewModes::CROSS_SECTION) || rdrGlobal.m_bObliqueShadow)
      {
        InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
      }
      break;
    }
    default:
    {
      break;
    }
  }

  if( !IsFlying2dOr3d() )
  {
    GetParentGLWnd()->GLRenderViews();  
  }
} // InvalidateView()


/**
 * Update window's visibility state.
 *
 * @param bShow   Whether or not the window is now visible.
 */
void SliceChildView::OnShowWindow(const bool bShow)
{
  SliceScrollBar & sliceScroll = GetScrollBar();
  if (sliceScroll)
  {
    sliceScroll.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
  }
} // OnShowWindow( bShow )


/////////////////////////////////////////////////////////////////////////////////////
//

/**
 * Get the Hounsfield unit value at the specified point in the view
 *
 * @param pt                       2D Point in the slice view rectangle
 * @param &bValidPosition          Returns whether the position is valid (within view rectangle)
 * @param &currentSamplePosition   Returns the 3D point corresponding to the give 2D point
 * @return                         Hounsfield unit value at the point specified
 */
int4 SliceChildView::GetHounsfieldValue( const Point2D<int4>& pt, bool & bValidPosition,
                                         Point3D<float4> & currentSamplePosition ) const
{
  try
  {
    static const int4 iDefaultValue(-1000);

    CRect rect;
    GetClientRect(&rect);

    if ( pt.m_x < 0 || pt.m_x >= rect.Width() ||
        pt.m_y < 0 || pt.m_y >= rect.Height() )
    {
      bValidPosition = false;
      return iDefaultValue;
    }
    bValidPosition = true;

    Dataset& myDatasetRef = *m_sliceViewer.GetDataset();
    const float4 fXPos(float4(pt.m_x)/float4(rect.Width()));
    const float4 fYPos(float4(pt.m_y)/float4(rect.Height()));
    currentSamplePosition = m_sliceViewer.GetPickedLocationVolumeCoord(fXPos,fYPos);
    Point<uint2> vuPosVolumeCoord = Point<uint2>(uint2(currentSamplePosition.m_x + 0.5F),uint2(currentSamplePosition.m_y + 0.5F),uint2(currentSamplePosition.m_z + 0.5F));
    const Triple<float4> dim(m_sliceViewer.m_bSliceRendererEnabled ? myDatasetRef.m_fields.m_sliceDimensions : myDatasetRef.m_vDim);

    // check if inside the volume
    bValidPosition = currentSamplePosition.m_x >= 0 && currentSamplePosition.m_x <= dim.m_x - 1 &&
                     currentSamplePosition.m_y >= 0 && currentSamplePosition.m_y <= dim.m_y - 1 &&
                     currentSamplePosition.m_z >= 0 && currentSamplePosition.m_z <= dim.m_z - 1;

    float4 fRawValue;
    if( myDatasetRef.m_fields.m_sliceRendererData.IsValid() )
    {
      if (m_sliceViewer.GetBlockPixels())
      {
        return myDatasetRef.m_fields.m_sliceRendererData.GetVoxel(vuPosVolumeCoord.m_x, vuPosVolumeCoord.m_y, vuPosVolumeCoord.m_z, SliceViewer::m_bShowUncleansed);
      }
      else
      {
        return myDatasetRef.m_fields.m_sliceRendererData.GetVoxel(currentSamplePosition.m_x + 0.5F, currentSamplePosition.m_y + 0.5F, currentSamplePosition.m_z + 0.5F, SliceViewer::m_bShowUncleansed);
      }
    }
    else
    {
      if (SliceViewer::m_bShowUncleansed)
      {
        if (m_sliceViewer.GetBlockPixels())
        {
          fRawValue = myDatasetRef.m_fields.m_volumeRender.GetUncleansedValue(vuPosVolumeCoord);
        }
        else
        {
          fRawValue = myDatasetRef.m_fields.m_volumeRender.GetUncleansedResampledValue(currentSamplePosition);
        } // endif interpolate or not
      }
      else
      {
        if(m_sliceViewer.GetBlockPixels())
        {
          fRawValue = myDatasetRef.m_fields.m_volumeRender.GetVoxel(vuPosVolumeCoord);
        }
        else
        {
          fRawValue = myDatasetRef.m_fields.m_volumeRender.GetResampledValue(currentSamplePosition);
        } //endif interpolate or not
      } // endif uncleansed or not
    }

    return HUconverter::Uchar2HU(fRawValue);
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceChildView", "GetHounsfieldValue" );
    return -1000;
  }
} // GetHounsfieldValue( pt, bValidPosition, currentSamplePosition )


/**
 * Scrolls the slice view, either by a relative (difference) value, or to an absolute slice number
 *
 * @param iScrollValue   Scroll value.
 * @param eScrollType    Relative or absolute scroll value.
 * @param pt             Point of scroll event (i.e. mouse wheel event).
 * @param uFlags         Scroll acceleration flags, MK_CONTROL to speed up scrolling.
 */
void SliceChildView::ScrollSlice(int4 iScrollValue, const ScrollTypeEnum eScrollType, const CPoint& pt, const UINT uFlags)
{
  try
  {
    Dataset& myDatasetRef = *m_sliceViewer.GetDataset();

    // go much faster if the user holds down the control key
    static const int4 iFactor = 10;
    if (uFlags & MK_CONTROL)
    {
      iScrollValue *= iFactor;
    }

    // This will not necessary for cross-section view for sure. I am not sure the View-Direction 
    // view need this or not? Dongqing
    //if (GetFullObliqueMode())
    //{
    //  const int4 iNewSkel = Bound(
    //    0,
    //    (eScrollType == eScrollDifference) ? GetScrollBar().GetScrollPos() + iScrollValue : iScrollValue,
    //    int4(myDatasetRef.m_joinedSkeleton.GetNumOfElements())-1
    //    );

    //  TRACE2("Scroll (%d), New value (%d)\n", iScrollValue, iNewSkel);

    //  GetScrollBar().SetScrollPos(iNewSkel);
    // 
    //  SkeletonNode skel = myDatasetRef.m_joinedSkeleton.GetSkelNode(iNewSkel);
    //  myDatasetRef.m_submarine.SetPositionWorld(skel.m_vWorldPos);
    //  myDatasetRef.SelectSegment(skel.m_vWorldPos);
    //  myDatasetRef.m_submarine.UpdatePosition();

    //  rdrGlobal.m_pCurrDataset->m_iXsectionScrollBarLocationIndex = iNewSkel;
    //}
    //else
    {
      if (eScrollType == eScrollDifference)
      {
        // disable flight along centerline if it isn't a main view window.
        m_sliceViewer.Scroll(iScrollValue, IsMainViewWindow());
      }
      else
      {
        m_sliceViewer.ScrollTo(iScrollValue);
      }
    }

    // Get the Hounsfield unit value at the cursor position
    CRect rect;
    GetClientRect(&rect);
    CPoint clientPt(pt); ScreenToClient(&clientPt);

    if (rect.PtInRect(clientPt) && m_bMouseWheelCalculate)
    {
      m_iCurrHounsfieldValue = GetHounsfieldValue( Point2D<int4>(clientPt.x, clientPt.y), m_bCurrHounsfieldValueValid, m_currHounsfieldSamplePosition );
    }
    m_bMouseWheelUpdated = true;

    myDatasetRef.m_fields.m_volumeRender.FlushAccumulationBuffer();



 /*   const long UPDATE_OVERVIEW = 1;
    const long RERENDER_OVERVIEW = 1;
    GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW, UPDATE_OVERVIEW));*/
    // if necessary, update ROI statistics
    try
    {
      if (ViatronStudy::GetViewModes(GetViewWndType()) == ViewModes::ROIMEASURE)
      {
        RoiMeasurement(MEASUREFUNC_UPDATE, CPoint(0,0));
      }
    }
    catch ( ... )
    {
      LogErr( "unhandled exception", "SliceChildView", "ScrollSlice" );
    }

    UpdateSliceScrollPos();
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceChildView", "ScrollSlice" );
  }
} // ScrollSlice( iScrollValue, eScrollType, pt, uFlags)


/**
 * Scrolls the slice view by a value relative to the current slice.
 *
 * @param iScrollValue   Scroll value.
 * @param uFlags         Scroll acceleration flags, MK_CONTROL to speed up scrolling.
 */
void SliceChildView::ScrollSlice(const int4 iValue, const UINT uFlags)
{
  // setting this flag prevents HU calculation in ScrollSlice() method.
  //SetMouseWheelActionFlag(false);

  ScrollSlice(iValue, eScrollDifference, CPoint(0,0), uFlags);

  // resetting...
  //SetMouseWheelActionFlag(true);

} // ScrollSlice( iValue, uFlags )


/**
 * Synchronizes the slice view to the other one
 */
void SliceChildView::SynchronizeNeighborSlice()
{

  try
  {
    // do not synchronize if not necessary
    if ( m_pNeighbourSliceView == NULL ||
        ! Utilities::Datasets::IsLocked() ||
        m_sliceViewer.GetOrientation() != m_pNeighbourSliceView->m_sliceViewer.GetOrientation() ||
        m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR ||
        m_pNeighbourSliceView->m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR )
    {
      return;
    }

    // multiply the slice offset by this factor to account for supine/prone
    SliceViewer & neighbor = m_pNeighbourSliceView->m_sliceViewer;

    // since rdrGlobal.m_vProneOffset is always governed by which dataset is opened first (the so-called supine dataset),
    //  the offset has to be added or subtracted accordingly
    int4 iLeftRightDeterminer = (GetDatasetPtr() == &rdrGlobal.m_supine) ? -1 : 1;

    // choose the new slice
    int4 iNewSlice( -1 );
    switch ( neighbor.GetOrientation() )
    {
      case SliceOrientations::eAXIAL:
      {
        //if( synchronize based on slice number )
        //  iNewSlice = m_sliceViewer.GetSliceNumber();
        //else if( synchronize based on the slice difference set by user )
          iNewSlice = m_sliceViewer.GetSliceNumber() - (rdrGlobal.m_vProneOffset.m_z * iLeftRightDeterminer);
        break;
      }
      case SliceOrientations::eSAGITTAL:
      {
        //if( synchronize based on slice number )
        //  iNewSlice = m_sliceViewer.GetSliceNumber();
        //else if( synchronize based on the slice difference set by user )
          iNewSlice = m_sliceViewer.GetSliceNumber() - (rdrGlobal.m_vProneOffset.m_x * iLeftRightDeterminer);
        break;
      }
      case SliceOrientations::eCORONAL:
      {
        //if( synchronize based on slice number )
        //  iNewSlice = m_sliceViewer.GetSliceNumber();
        //else if( synchronize based on the slice difference set by user )
          iNewSlice =  m_sliceViewer.GetSliceNumber() - (rdrGlobal.m_vProneOffset.m_y * iLeftRightDeterminer);
        break;
      }
      case SliceOrientations::eCROSS_SECTION:
      {
        iNewSlice = m_sliceViewer.GetDataset()->m_joinedSkeleton.GetRegistrationOtherIndex( m_sliceViewer.GetSliceNumber() );
        break;
      }
      default:
      {
        break;
      }
    }

    // bound to the volume dimensions
    if ( iNewSlice < 0 )
    {
      iNewSlice = 0;
    }
    if ( iNewSlice > neighbor.GetNumberOfSlices() - 1 )
    {
      iNewSlice = neighbor.GetNumberOfSlices() - 1;
    }

    // go!
    m_pNeighbourSliceView->ScrollSlice(iNewSlice, eScrollAbsolute, CPoint(0,0), 0);

    // Get the Hounsfield units at the cursor position
    {
      CPoint mousePt;
      GetCursorPos(&mousePt);
      m_pNeighbourSliceView->ScreenToClient(&mousePt);
      m_pNeighbourSliceView->m_iCurrHounsfieldValue
        = m_pNeighbourSliceView->GetHounsfieldValue( Point2D<int4>(mousePt.x,mousePt.y), m_pNeighbourSliceView->m_bCurrHounsfieldValueValid, m_pNeighbourSliceView->m_currHounsfieldSamplePosition );
    }
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceChildView", "SynchronizeNeighborSlice" );
  }

} // SynchronizeNeighborSlice()


/**
 * Calculates change in window/level as user drags mouse to specified point
 *
 * @param point   Drag point
 */
void SliceChildView::SetWindowLevel(const CPoint & point)
{
  const float4 fWinFactor(1000.0F / GetViewport().Width() * ( m_bWindowWidthIncreaseRight ? -1 : 1 ) );
  const float4 fLevFactor(1000.0F / GetViewport().Height());

  pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;
  rWL.first  += (m_windowLevelLastPoint.x - point.x) * fWinFactor;
  rWL.second += (point.y - m_windowLevelLastPoint.y) * fLevFactor;

  m_windowLevelLastPoint = point;

  if (rWL.first < 1) { rWL.first = 1; }
  //else if (rWL.first > 2000) { rWL.first = 2000; } This dose not match the XP setting
  else if (rWL.first > 3000) { rWL.first = 3000; }
  if (rWL.second < -1000) { rWL.second = -1000; }
  else if (rWL.second > 1000) { rWL.second = 1000; }

  m_eCurrentWLPreset = eWLP_CTC;
  
  if( m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
  {
    m_sliceViewer.m_sliceRenderer.SetDefaultWindowLevel( rWL.first, rWL.second );
  }

  m_sliceViewer.m_bForceSliceRender = true;
} // SetWindowLevel( point )


/**
 * Sets the window/level to the specified values
 *
 * @param fWin    window
 * @param fLev    level
 */
void SliceChildView::SetWindowLevel(const float4 fWin, const float4 fLev)
{
  pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;
  rWL.first = fWin;
  rWL.second = fLev;
  if ( IsLinkedWithNeighbor() )
  {
    pair<int4,int4>& rNeighbourWL = !SliceViewer::m_bShowUncleansed ? m_pNeighbourSliceView->m_wlNormal : m_pNeighbourSliceView->m_wlSecondary;
    rNeighbourWL.first  = rWL.first;
    rNeighbourWL.second = rWL.second;
  }
} // SetWindowLevel( fWin, fLev``` )


/**
 * set one of the window level preset values
 *
 * @param iPresetVal   identifies the WL preset
 */
void SliceChildView::SetWindowLevelPreset( const int4 iPresetVal )
{
  WindowLevelPresetsEnum eNewWLPreset((WindowLevelPresetsEnum)iPresetVal);

  // cleverness described: using a loop so as not to duplicate code
  SliceChildView* vpViews[3] = {this, IsLinkedWithNeighbor()?m_pNeighbourSliceView:(SliceChildView*)NULL, (SliceChildView*)NULL};

  if ( (eNewWLPreset >= 0) && (eNewWLPreset < eWLP_Count) )
  {
    for (SliceChildView** ppView = vpViews; (*ppView) != (SliceChildView*) NULL; ppView++)
    {
      (*ppView)->m_eCurrentWLPreset = eNewWLPreset;
      pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? (*ppView)->m_wlNormal : (*ppView)->m_wlSecondary;
      rWL.first = m_veWindowLevelPresets[eNewWLPreset][1];
      rWL.second = m_veWindowLevelPresets[eNewWLPreset][2];

      if ( (*ppView)->m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
      {
        (*ppView)->m_sliceViewer.m_sliceRenderer.SetDefaultWindowLevel( rWL.first, rWL.second );
      }
      (*ppView)->ForceRedraw();
    }
  }
  else
  {
    int4 iUserPreset(iPresetVal - eWLP_Count);
    if (iUserPreset < SliceChildView::m_mapWLSettings.m_vWL.size())
    {
      SetWindowLevel(SliceChildView::m_mapWLSettings.m_vWL[iUserPreset].second.first, SliceChildView::m_mapWLSettings.m_vWL[iUserPreset].second.second);
      m_eCurrentWLPreset = (WindowLevelPresetsEnum) (iUserPreset + ID_CUSTOM_WINDOWLEVEL);
      if ( IsLinkedWithNeighbor() )
      {
        m_pNeighbourSliceView->m_eCurrentWLPreset = (WindowLevelPresetsEnum) (iUserPreset + ID_CUSTOM_WINDOWLEVEL);
      }
    }
  }

} // SetWindowLevelPreset( iPresetVal )


/**
 * edit the user preference for quick drop annotations
 */
void SliceChildView::EditQuickDropAnnotationName()
{
  std::string sNewText = WinUtils::TextPromptWnd::GetText(LPCTSTR(WinUtils::LoadResourceString(IDS_QUICKDROP_PROMPT)), m_sQuickAnnotationName);
  if (!sNewText.empty())
  {
    SliceChildView::m_sQuickAnnotationName = sNewText;
    ApplicationSettings::UserSettings().Set( APP_SETTING_QUICK_ANNOTATION_NAME, SliceChildView::m_sQuickAnnotationName );
  }
} // EditQuickDropAnnotationName()


/**
 * Called when the current dataset has changed
 */
void SliceChildView::OnDatasetChanged()
{
  m_sliceViewer.m_measureLineList.clear();
  m_sliceViewer.m_measureAreaList.clear();
  m_bUpdateRoiMeasurement = true;

  m_pCurrAnnotation.first = m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end();

  CPoint point;
  GetCursorPos( &point );
  ScreenToClient( &point );
  // needs to be called twice for whatever reason to get the correct hu value
  OnMouseMove( 0, point );
  OnMouseMove( 0, point );
} // OnDatasetChanged()


/**
 * Set up the start point for WindowLevel Control
 *
 * @param point   start point
 */
void SliceChildView::SetStartPointWindowLevel(const CPoint & point)
{
  m_windowLevelLastPoint = point;
} // SetStartPointWindowLevel( point )


/**
 * Zooms the view as user drags mouse to specified point
 *
 * @param point   Drag point
 * @return        Success/failure
 */
bool SliceChildView::ZoomSelect(const CPoint & point)
{
  if ( m_sliceViewer.m_viStartMovePt.m_x == 9999 )
  {
    m_sliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
    m_sliceViewer.m_viStartMovePt.m_y = (uint4) point.y;
  }

  if (m_sliceViewer.m_vfAnchorPt.m_x < 0)
  {
    m_sliceViewer.m_vfAnchorPt.m_x = (float4)point.x / (float4)m_sliceViewer.m_uDisplayWidth;
    m_sliceViewer.m_vfAnchorPt.m_y = (float4)point.y / (float4)m_sliceViewer.m_uDisplayHeight;
  }

  const float4 fScalar(m_sliceViewer.m_rZoom.m_bottom - m_sliceViewer.m_rZoom.m_top);
  const float4 fDelta(((float4)point.y - (float4)m_sliceViewer.m_viStartMovePt.m_y) / (float4)m_sliceViewer.m_uDisplayHeight);

  float4 fCurrentZoomFactor(m_sliceViewer.GetZoomPercentage() / 100.0F);
  if (fCurrentZoomFactor < 1.0F)
  {
    fCurrentZoomFactor = 1.0F;
  }
  const float4 fZoomScalar(Bound(0.5F, fScalar - fDelta*5.0F * fCurrentZoomFactor, 30.0F));
  const float4 fNewX( ( m_sliceViewer.m_vfAnchorPt.m_x - m_sliceViewer.m_rZoom.m_left ) / fScalar );
  const float4 fNewY( ( m_sliceViewer.m_vfAnchorPt.m_y - m_sliceViewer.m_rZoom.m_top  ) / fScalar );

  m_sliceViewer.m_rZoom.m_left   = (0.0F - fNewX) * fZoomScalar + m_sliceViewer.m_vfAnchorPt.m_x;
  m_sliceViewer.m_rZoom.m_right  = (1.0F - fNewX) * fZoomScalar + m_sliceViewer.m_vfAnchorPt.m_x;
  m_sliceViewer.m_rZoom.m_top    = (0.0F - fNewY) * fZoomScalar + m_sliceViewer.m_vfAnchorPt.m_y;
  m_sliceViewer.m_rZoom.m_bottom = (1.0F - fNewY) * fZoomScalar + m_sliceViewer.m_vfAnchorPt.m_y;

  m_sliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
  m_sliceViewer.m_viStartMovePt.m_y = (uint4) point.y;

  m_bUpdateRoiMeasurement = true;

  // if coupled ...
  if ( IsLinkedWithNeighbor() )
  {
    SliceViewer & neighborSliceViewer = m_pNeighbourSliceView->m_sliceViewer;

    if ( neighborSliceViewer.m_viStartMovePt.m_x == 9999 )
    {
      neighborSliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
      neighborSliceViewer.m_viStartMovePt.m_y = (uint4) point.y;
    }

    if (neighborSliceViewer.m_vfAnchorPt.m_x < 0)
    {
      neighborSliceViewer.m_vfAnchorPt.m_x = (float4)point.x / (float4)neighborSliceViewer.m_uDisplayWidth;
      neighborSliceViewer.m_vfAnchorPt.m_y = (float4)point.y / (float4)neighborSliceViewer.m_uDisplayHeight;
    }

    const float4 fNeiScalar(neighborSliceViewer.m_rZoom.m_bottom - neighborSliceViewer.m_rZoom.m_top);
    const float4 fNeiDelta(((float4)point.y - (float4)neighborSliceViewer.m_viStartMovePt.m_y) / (float4)neighborSliceViewer.m_uDisplayHeight);

    float4 fNeiCurrentZoomFactor(neighborSliceViewer.GetZoomPercentage() / 100.0F);
    if (fNeiCurrentZoomFactor < 1.0F)
    {
      fNeiCurrentZoomFactor = 1.0F;
    }
    const float4 fNeiZoomScalor(Bound(0.5F, fNeiScalar - fNeiDelta*5.0F * fNeiCurrentZoomFactor, 20.0F));
    const float4 fNeiNewX((neighborSliceViewer.m_vfAnchorPt.m_x - neighborSliceViewer.m_rZoom.m_left)/ fNeiScalar);
    const float4 fNeiNewY((neighborSliceViewer.m_vfAnchorPt.m_y - neighborSliceViewer.m_rZoom.m_top) / fNeiScalar);

    neighborSliceViewer.m_rZoom.m_left   = (0 - fNeiNewX) * fNeiZoomScalor + neighborSliceViewer.m_vfAnchorPt.m_x;
    neighborSliceViewer.m_rZoom.m_right  = (1 - fNeiNewX) * fNeiZoomScalor + neighborSliceViewer.m_vfAnchorPt.m_x;
    neighborSliceViewer.m_rZoom.m_top    = (0 - fNeiNewY) * fNeiZoomScalor + neighborSliceViewer.m_vfAnchorPt.m_y;
    neighborSliceViewer.m_rZoom.m_bottom = (1 - fNeiNewY) * fNeiZoomScalor + neighborSliceViewer.m_vfAnchorPt.m_y;

    neighborSliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
    neighborSliceViewer.m_viStartMovePt.m_y = (uint4) point.y;

    m_pNeighbourSliceView->m_bUpdateRoiMeasurement = true;

  } // endif (coupled)

  return true;
} // ZoomSelect( point )


/*
 * Pans the slice, according to the difference specified by the last pan point.
 *
 * @param sliceViewer   Slice viewer
 * @param point         Drag point
 * @return              Success/failure
 */
bool SliceChildView::Pan(SliceViewer & sliceViewer, const CPoint & point)
{
  if (GetViewWndType() & eGVT_CROSS_SECTION)
  {
    return false;
  }

  sliceViewer.SetValueOfDisplayOverviewSliceGrasp(true);
  if (sliceViewer.m_viStartMovePt.m_x == 9999 )
  {
    sliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
    sliceViewer.m_viStartMovePt.m_y = (uint4) point.y;
  }

  int4 iDeltaX = point.x - sliceViewer.m_viStartMovePt.m_x;
  int4 iDeltaY = point.y - sliceViewer.m_viStartMovePt.m_y;

  float4 fMoveX = (float4)iDeltaX / (float4)sliceViewer.m_uDisplayWidth;
  float4 fMoveY = (float4)iDeltaY / (float4)sliceViewer.m_uDisplayHeight;

  sliceViewer.m_rZoom.Offset(fMoveX, fMoveY);
  sliceViewer.m_viStartMovePt.m_x = (uint4) point.x;
  sliceViewer.m_viStartMovePt.m_y = (uint4) point.y;

  // when zoom factor < 1.0, no offset.
  if ( (sliceViewer.m_rZoom.m_right - sliceViewer.m_rZoom.m_left)<1.0F
    || (sliceViewer.m_rZoom.m_bottom - sliceViewer.m_rZoom.m_top)<1.0F )
  {
    return true;
  }

  Rect<float4> boundsAdjust(0,0,0,0);

  if (sliceViewer.m_rZoom.m_left > 0.0F)
  {
    boundsAdjust.m_left = -sliceViewer.m_rZoom.m_left;
  }
  if (sliceViewer.m_rZoom.m_top > 0.0F)
  {
    boundsAdjust.m_top = -sliceViewer.m_rZoom.m_top;
  }
  sliceViewer.m_rZoom.Offset(boundsAdjust.m_left, boundsAdjust.m_top);

  if (sliceViewer.m_rZoom.m_right < 1.0F)
  {
    boundsAdjust.m_right = 1.0F - sliceViewer.m_rZoom.m_right;
  }
  if (sliceViewer.m_rZoom.m_bottom < 1.0F)
  {
    boundsAdjust.m_bottom = 1.0F - sliceViewer.m_rZoom.m_bottom;
  }
  sliceViewer.m_rZoom.Offset(boundsAdjust.m_right, boundsAdjust.m_bottom);

  m_bUpdateRoiMeasurement = true;

  return true;
} // Pan( sliceViewer, point )


/**
 * Moves the current viewpoint
 *
 * @param point   2D viewpoint to which to move
 * @return        Success/failure
 */
bool SliceChildView::MoveViewPoint(const CPoint & point)
{
  Dataset& myDatasetRef = *m_sliceViewer.GetDataset();
  if (!myDatasetRef.IsLoadingComplete())
  {
    return false;
  }

  if ( m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR )
  //if ((m_sliceViewer.GetOrientation() == SliceOrientations::eCROSS_SECTION) || (m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR))
  // I am not sure this change may affect other function or not. Dongqing
  {
    return false;
  }

  CRect rect;
  Point<float4> vPosWorldCoord;
  float4 fPosX, fPosY;

  GetClientRect(&rect);

  fPosX = float4(point.x)/float4(rect.Width());// - m_sliceViewer.m_rZoom.m_left;
  fPosY = float4(point.y)/float4(rect.Height());// - m_sliceViewer.m_rZoom.m_top;
  vPosWorldCoord = m_sliceViewer.GetPickedLocationWorldCoord(fPosX, fPosY);

  myDatasetRef.SelectSegment(vPosWorldCoord);
  myDatasetRef.m_submarine.SetPositionWorld(vPosWorldCoord);
  myDatasetRef.m_submarine.UpdatePosition();
  myDatasetRef.m_fields.m_volumeRender.FlushAccumulationBuffer();

  InvalidateViews( GLChildView::eGVT_ALL, 0 );

//  ForceRedraw();

  return true;
} // MoveViewPoint( point )


/**
 * Move the view direction
 *
 * @param point   Point *opposite* the direction the view will face
 * @return        Success/failure
 */
bool SliceChildView::MoveViewDirection(const CPoint & point)
{
  Dataset& myDatasetRef = *m_sliceViewer.GetDataset();
  if (!myDatasetRef.IsLoadingComplete())
  {
    return false;
  }

  CRect rect;
  float4 fPosX, fPosY;
  Point<float4> vPosWorldCoord;
  Point<float4> vLookAtWorldCoord;
  Normal<float4> vOrientation;

  GetClientRect(&rect);

  fPosX = float4(point.x)/float4(rect.Width());
  fPosY = float4(point.y)/float4(rect.Height());


  vPosWorldCoord = myDatasetRef.m_submarine.GetPositionWorld();
  vLookAtWorldCoord = m_sliceViewer.GetPickedLocationWorldCoord(fPosX, fPosY);
  vOrientation =  Normal<float4>(vPosWorldCoord - vLookAtWorldCoord);
  myDatasetRef.m_submarine.SetOrientation(vOrientation);
  myDatasetRef.m_submarine.UpdatePosition();
  myDatasetRef.m_fields.m_volumeRender.FlushAccumulationBuffer();

  return true;
} // MoveViewDirection( point )


/**
 * calculate the world coordinate: mapping a 2D point (screen coordinates) on the window to its world coordinates.
 *
 * @param vPoint    point at the window.
 * @return          a 3D point which is a world coordinate.
 */
Point<float4> SliceChildView::Calculate3DPoint(const Point<float4> & vPoint) const
{
  CRect rect;

  Point<float4> vPointWC;
  GetClientRect(&rect);
  float4 fXPos = float4(vPoint.m_x)/float4(rect.Width());
  float4 fYPos = float4(vPoint.m_y)/float4(rect.Height());
  vPointWC = m_sliceViewer.GetPickedLocationWorldCoord(fXPos,fYPos);

  return vPointWC;
} // Calculate3DPoint( vPoint )


/**
 * Transfer view coordinates to coordinates in a slice (volume).
 *
 * @param pt2DView   view coordinates.
 * @param rect       client area of the screen
 * @return           view coordinates translated to slice volume.
 */
Point2D<int> SliceChildView::GetViewToSlice(const Point2D<int4> & pt2DView, const CRect & rect) const
{
  Dataset& myDatasetRef = *m_sliceViewer.GetDataset();
  Point2D<int4> pt2DSlice;

  float4 fDeltaX = (float4)pt2DView.m_x / rect.Width() - m_sliceViewer.m_rZoom.m_left;
  float4 fDeltaY = (float4)pt2DView.m_y / rect.Height() - m_sliceViewer.m_rZoom.m_top;

  const Point<float4> dim(m_sliceViewer.GetOrientationDim());

  pt2DSlice.m_x = fDeltaX / m_sliceViewer.m_rZoom.Width() * dim.m_x + 0.5f;
  pt2DSlice.m_y = fDeltaY / m_sliceViewer.m_rZoom.Height() * dim.m_y + 0.5f;

  return pt2DSlice;
} // GetViewToSlice( pt2DView, rect )


/**
 * Transfer slice coordinates to view coordinates.
 *
 * @param pt2DSlice   Slice coordinates.
 * @param rect        client area of the screen.
 * @return            Slice coordinates translated to the view area.
 */
Point2D<int> SliceChildView::GetSliceToView(const Point2D<int4> & pt2DSlice, const CRect & rect) const
{
  Dataset& myDatasetRef = *m_sliceViewer.GetDataset();
  const Point<float4> dim(m_sliceViewer.GetOrientationDim());

  float4 fDeltaX = pt2DSlice.m_x / dim.m_x * m_sliceViewer.m_rZoom.Width();
  float4 fDeltaY = pt2DSlice.m_y / dim.m_y * m_sliceViewer.m_rZoom.Height();

  Point2D<int4> pt2DView;
  pt2DView.m_x = int4(((fDeltaX + m_sliceViewer.m_rZoom.m_left) * rect.Width()));
  pt2DView.m_y = int4(((fDeltaY + m_sliceViewer.m_rZoom.m_top) * rect.Height()));

  return pt2DView;
} // GetSliceToView( pt2DSlice, rect )


/**
 * Prompts for text and adds a new annotation at the specified 2D point
 *
 * @param point2D        2D view point at which to add annotation in current slice view
 * @param sDefaultName   name to be used for annotation; if empty, user will be queried
 * @return               Success/failure
 */
bool SliceChildView::AddNewAnnotation(const Point2D<int4>& point2D, const std::string& sDefaultName /* = std::string */)
{
  if (!IsMainViewWindow())
  {
    return false;
  }

  // Annotation text
  Bookmark antn;

  switch (m_sliceViewer.GetOrientation())
  {
    case SliceOrientations::eAXIAL:
    {
      antn.m_orientation = Normal<float4>(1,0,0);
      antn.m_upDirection = Normal<float4>(0,0,1);
      break;
    }
    case SliceOrientations::eSAGITTAL:
    {
      antn.m_orientation = Normal<float4>(0,0,-1);
      antn.m_upDirection = Normal<float4>(1,0,0);
      break;
    }
    case SliceOrientations::eCORONAL:
    {
      antn.m_orientation = Normal<float4>(1,0,0);
      antn.m_upDirection = Normal<float4>(0,0,1);
      break;
    }
    default:
    {
      AfxMessageBox("Adding annotations are currently allowed only in 2D orthogonal views.\n"
        "Annotations in the MPR view will be included in a future software release.\n\n"
        "Please contact service for more information.");
      return false;
    }
  } // endswitch(m_sliceViewer.Orientation).

  // prompt for name if necessary
  antn.m_sName = !sDefaultName.empty() ? sDefaultName :
    TextPromptWnd::GetText(LPCTSTR(LoadResourceString(IDS_ANNOTATION_TEXT_PROMPT)), std::string(), TextPromptWnd::eSingleLine);

  if (antn.m_sName.empty())
  {
    return true;
  }

  // Location
  CRect rect;
  GetClientRect(&rect);

  Dataset& thisDataset = *(m_sliceViewer.GetDataset());
  antn.m_position = m_sliceViewer.GetPickedLocationWorldCoord(float4(point2D.m_x)/float4(rect.Width()), float4(point2D.m_y)/float4(rect.Height()));

  // Slice orientation (over/re-using m_priority member of bookmark)
  antn.m_ePriority = Bookmark::PriorityEnum(m_sliceViewer.GetOrientation());

  thisDataset.m_session.m_2DAnnotationList.AddBookmark(antn);
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
  ForceRedraw();

  return true;
} // AddNewAnnotation( point2D )


/**
 * Finds the nearest proximal, if any, annotation to the point
 *
 * @param pt     Test point.
 * @param bTip   true to test for annotation arrow tip, false to test for arrow back.
 * @return       Closest proximal annotation, or NULL if there are no proximal annotations.
 */
SliceChildView::AnnotationArrowRef SliceChildView::GetProximalAnnotation(const Point<int4>& pt) const
{
  if ((m_sliceViewer.GetOrientation() == SliceOrientations::eCROSS_SECTION) || (m_sliceViewer.GetOrientation() == SliceOrientations::eVIEWDIR))
  {
    return AnnotationArrowRef(m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end(),false);
  }

  Dataset& rDataset = *m_sliceViewer.GetDataset();
  if (!rDataset.IsLoadingComplete())
  {
    return AnnotationArrowRef(m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end(),false);
  }

  CRect rect; GetClientRect( rect);
  float4 arrowLength = (m_sliceViewer.m_uCameraArrowLength / m_sliceViewer.m_rZoom.Width()) * 450 / rect.Width(); // 450 ??? Dont ask me, it works ?????

  BookmarkList& list = rDataset.m_session.m_2DAnnotationList;

  Point<float4> pt3D = m_sliceViewer.GetPickedLocationWorldCoord(float4(pt.m_x)/float4(rect.Width()), float4(pt.m_y)/float4(rect.Height()));

  const float4 fProxDist(8.0F / m_sliceViewer.m_rZoom.Width());

  for (BookmarkList::iterator it = list.begin(); it != list.end(); it++)
  {
    const Point<float4> itPos(it->m_position);
    if (m_sliceViewer.DistFromCurrentSlice(itPos.GetDividedAnIso(rDataset.m_fields.m_units2D)) < m_sliceViewer.m_uAnnotationSliceDiffThreshold &&
        m_sliceViewer.ManhattanDistInPlane(itPos,pt3D) < fProxDist )
    {
      return AnnotationArrowRef(it,true);
    }

    const Point<float4> itOrient(itPos + (Vector<float4>(it->m_orientation)*(-arrowLength)));
    if (m_sliceViewer.DistFromCurrentSlice(itOrient.GetDividedAnIso(rDataset.m_fields.m_units2D)) < m_sliceViewer.m_uAnnotationSliceDiffThreshold &&
        m_sliceViewer.ManhattanDistInPlane(itOrient,pt3D) < fProxDist )
    {
      return AnnotationArrowRef(it,false);
    }

  } // endfor all annotations

  return AnnotationArrowRef(m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList.end(),false);
} // GetProximalAnnotation( pt )


/**
 * Returns true if the given point is proximal to the current world location
 *
 * @param viewPt   Test point.
 * @return         true if the given point is proximal to the current world location, false otherwise.
 */
bool SliceChildView::IsProximalWorldLoc(const Point<int4>& viewPt) const
{
  Dataset& myDset = *m_sliceViewer.GetDataset();
  // False when no dataset loaded
  if (!myDset.IsLoadingComplete()) 
  {
    return false;
  }

  // World location indicator not valid for mpr slice views
  const SliceOrientations::TypeEnum eOrient(m_sliceViewer.GetOrientation());
  if ((eOrient == SliceOrientations::eVIEWDIR) || (eOrient == SliceOrientations::eCROSS_SECTION)) 
  {
    return false;
  }

  CRect rect; GetClientRect( rect );

  // TODO: Just use volume coordinates; the function below calculates in volume then changes to world, so double work is being done here.
  const Triple<float4> units( m_sliceViewer.m_bSliceRendererEnabled ? myDset.m_fields.m_units2D : myDset.m_vUnits );
  Point<float4> pt3D = m_sliceViewer.GetPickedLocationWorldCoord(float4(viewPt.m_x)/float4(rect.Width()), float4(viewPt.m_y)/float4(rect.Height()));
  pt3D = pt3D.GetDividedAnIso( units );

  Point<float4> volumePos(myDset.m_submarine.GetPositionWorld().GetDividedAnIso(units));

  const float4 fProxDist(8.0F / m_sliceViewer.m_rZoom.Width());

  return (m_sliceViewer.DistFromCurrentSlice(volumePos) < m_sliceViewer.m_uCameraSliceDiffThreshold &&
          m_sliceViewer.ManhattanDistInPlane(volumePos,pt3D) < fProxDist );
} // IsProximalWorldLoc( viewPt )



/**
 * Force a redraw of this view
 *
 * @param bForceNeighbor   Whether or not to force a neighbor (if present) redraw
 */
void SliceChildView::ForceRedraw( bool bForceNeighbor )
{
  if( IsFlying2dOr3d() )
  {
    return;
  }

  //if( GetDatasetRef().m_submarine.IsFlying() && !SliceChildView::m_bUpdateEndoluminalView )
  //  return;

  InvalidateView();
  m_sliceViewer.m_bForceSliceRender = true;
  GLRender();
  
  if( bForceNeighbor && IsLinkedWithNeighbor() )
  {
    m_pNeighbourSliceView->ForceRedraw();
  }
} // ForceRedraw( bForceNeighbor )


/**
 * Determines if this view has a neighbor and is linked to it
 *
 * @return if this view has a neighbor and is linked to it
 */
bool SliceChildView::IsLinkedWithNeighbor() const
{

  const bool bOrthogonal =
    m_sliceViewer.GetOrientation() == SliceOrientations::eAXIAL ||
    m_sliceViewer.GetOrientation() == SliceOrientations::eSAGITTAL ||
    m_sliceViewer.GetOrientation() == SliceOrientations::eCORONAL ||
    m_sliceViewer.GetOrientation() == SliceOrientations::eCROSS_SECTION;

  return
    Utilities::Datasets::IsLocked() &&
    bOrthogonal &&
    m_pNeighbourSliceView != NULL &&
    m_sliceViewer.GetOrientation() == m_pNeighbourSliceView->m_sliceViewer.GetOrientation();

} // IsLinkedWithNeighbor()


/**
 * timer event
 *
 * @param uIDEvent   event id
 */
void SliceChildView::OnTimer(const uint4 uIDEvent)
{
  if (uIDEvent == m_timerUpdateROI)
  {
    m_timerUpdateROI.KillTimer();
    //UpdateSuspiciousROI( m_ptUpdateROI );
  }
} // OnTimer()


/**
 * Returns the slicechildview state
 */
SliceChildView::SliceChildViewState SliceChildView::GetSliceChildViewState() const
{
  SliceChildViewState state;
  state.m_windowLevel = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;
  state.m_eOrientation = m_sliceViewer.GetOrientation();
  state.m_bFullScreen = m_bFullScreen;
  state.m_bSetOrientation = true;

  return state;
} // GetSliceChildViewState()



/**
 * sets the slicechildview state
 *
 * @param         state                 SliceChildViewState
 */
void SliceChildView::SetSliceChildViewState( SliceChildViewState & state )
{
  if( state.m_bSetOrientation )
  SetOrientation( state.m_eOrientation );

  if( m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
  {
    pair<int4, int4> & rWL = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;
    rWL = state.m_windowLevel;

    m_sliceViewer.m_sliceRenderer.SetDefaultWindowLevel( rWL.first, rWL.second );
    InvalidateViews( WPARAM(GLChildView::eGVT_SLICE), 0);
    ForceRedraw();
  }
} // SetSliceChildViewState( state )