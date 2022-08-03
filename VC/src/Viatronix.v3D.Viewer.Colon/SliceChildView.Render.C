// $Id: SliceChildView.Render.C,v 1.1.2.27 2010/12/07 14:20:11 dongqing Exp $
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
#include "GDIPlusUtilities.h"

// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.1.2.27 $"
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


/**
 * Render slice
 * @param void*   unused
 */
void SliceChildView::GLRenderScene(void *)
{

  try
  {    

#ifdef RENDERSTATS
    LogDbg( std::string( "Outside: " ) + m_debugTimerOutside.AsString(), "", "" );
    Timer renderSceneTimer;
#endif

    if (!m_sliceViewer.IsVolumeLoaded())
    {
      GLChildView::GLRenderScene();
      return;
    }
    
    vxShareableObject< vxComponentArray > * pComponentArray( m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.GetComponents() );

    register int m = m_sliceViewer.GetDataset()->m_suspiciousRegions.GetCount();

    if( pComponentArray->GetCount() > 1 ) /// This is necessary since we allow the user to switch to orthorgonal view while data is loading. At that time, the Components is not loaded yet.
    {    
      register int i;
      if( m > 0 )
      {
        for( i=0; i<m; i++ )
        { // check the SRT display switch ON/OFF  TODO: This may be not the most efficient spot to do this. Dongqing
          if( !rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT())  )
          { // disable display SRT roi
            pComponentArray->ArrayByLabel()[m_sliceViewer.GetDataset()->m_suspiciousRegions.GetFinding(i).GetVolumeROI().GetLabelIndex()]->SetLabelVisible(false);
          } // if the SRT display is turn off
          else 
          { // enable display SRT roi
            if( rdrGlobal.m_bDisplaySRTroi2D )
			        pComponentArray->ArrayByLabel()[m_sliceViewer.GetDataset()->m_suspiciousRegions.GetFinding(i).GetVolumeROI().GetLabelIndex()]->SetLabelVisible(true);
						else
			        pComponentArray->ArrayByLabel()[m_sliceViewer.GetDataset()->m_suspiciousRegions.GetFinding(i).GetVolumeROI().GetLabelIndex()]->SetLabelVisible(false);
          }
        } // for i
      } // if component exists
      
      m = m_sliceViewer.GetDataset()->m_cadFindings.GetCount();

      if( m > 0 )
      {
        for( i=0; i<m; i++ )
        { // check the CAD display switch ON/OFF
          if( !rdrGlobal.m_bDisplayCadFindings && !rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT()) )
          { // disable display CAD roi
            pComponentArray->ArrayByLabel()[m_sliceViewer.GetDataset()->m_cadFindings.GetFinding(i).GetVolumeROI().GetLabelIndex()]->SetLabelVisible(false);
          } // if the CAD display is turn off
          else
          { // enable display CAD roi
			      if( RenderUtil::IsFindingVisible( m_sliceViewer.GetDataset()->m_cadFindings.GetFinding(i), m_sliceViewer.GetDataset()->m_session.m_mapCadFindingInfo, 
				        m_sliceViewer.GetDataset()->m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings ) && rdrGlobal.m_bDisplaySRTroi2D )
			      {
              pComponentArray->ArrayByLabel()[m_sliceViewer.GetDataset()->m_cadFindings.GetFinding(i).GetVolumeROI().GetLabelIndex()]->SetLabelVisible(true);
			      }
			      else
			      {
              pComponentArray->ArrayByLabel()[m_sliceViewer.GetDataset()->m_cadFindings.GetFinding(i).GetVolumeROI().GetLabelIndex()]->SetLabelVisible(false);
			      }
          }
        } // for i
      } // if component exists
    } // if( pComponentArray->GetCount() > 1 )

    m_sliceViewer.m_sliceRenderer.SetSlabThickness(SliceViewer::m_fGlobalSlabThickness);
    m_sliceViewer.m_sliceRenderer.SetRenderingMode(SliceViewer::m_eGlobalRenderMode);

    if (m_pDatasetLastRendered != m_sliceViewer.GetDataset())
    {
      m_sliceViewer.InitVolumeLoaded();
      UpdateSliceScrollPos(true);
      m_pDatasetLastRendered = m_sliceViewer.GetDataset();
      m_sliceViewer.m_measureLineList.clear();
      m_sliceViewer.m_measureAreaList.clear();
    }
    else
    {
      UpdateSliceScrollPos(false);
    }

    Dataset& rDataset = *m_sliceViewer.GetDataset();
    if (!SliceViewer::m_bShowUncleansed && !rDataset.m_fields.m_sliceRendererData.IsValid() && !rDataset.m_fields.IsDiffVolLoaded())
    {
      rDataset.m_fields.LazyLoadDiffVolume();
    }

    glClearColor( 0, 0, 0, 0 );
    glClear(GL_COLOR_BUFFER_BIT);
    // set up color mapping either biopsy or window-iLevel
    if (SliceChildView::m_eDbgShow == SHOW_BIOPSY)
    {
      // biopsy color mapping
      m_sliceViewer.GetDataset()->m_biopsyTransferFunction.PrepareOpenGLPixelMapEnvironment();
    }
    else
    {
      // window leveling for old slice renderers
      pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;
      if( !m_sliceViewer.m_bSliceRendererEnabled )
      {
        float4 fScale = 2000.0F/float4(rWL.first);
        int4 iLevel = rWL.second + 1000 - (rWL.first/2);
        float4 fBias = 0.0F-((float4(iLevel)/2000.0F)*fScale);
        
        glPixelTransferf(GL_RED_SCALE,fScale);
        glPixelTransferf(GL_GREEN_SCALE,fScale);
        glPixelTransferf(GL_BLUE_SCALE,fScale);
        glPixelTransferf(GL_RED_BIAS,fBias);
        glPixelTransferf(GL_GREEN_BIAS,fBias);
        glPixelTransferf(GL_BLUE_BIAS,fBias);
      }
    } // endif (SHOW_BIOPSY)

    // rendering the slice
    switch (SliceChildView::m_eDbgShow)
    {
      case SHOW_INS:
      {
        m_sliceViewer.RenderIns(GetViewport().m_left, GetViewport().m_top, GetInverseImage());
        break;
      }
      case SHOW_DDB:
      {
        m_sliceViewer.RenderDdb(m_fDDBScale, GetViewport().m_left, GetViewport().m_top, GetInverseImage());
        break;
      }
      case SHOW_CSI:
      {
        m_sliceViewer.RenderCsi(GetViewport().m_left, GetViewport().m_top, GetInverseImage());
        break;
      }
      case SHOW_PDR:
      {
        m_sliceViewer.RenderPdr(GetViewport().m_left, GetViewport().m_top, GetInverseImage());
        break;
      }
      default:
      {
#ifdef RENDERSTATS
        renderSceneTimer.Stop();
        Timer renderSliceTimer;
#endif

        pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;
        m_sliceViewer.m_sliceRenderer.SetDefaultWindowLevel( rWL.first, rWL.second );

        Rect<int4> rect = GetViewRect();
        m_sliceViewer.RenderSlice(  GetDatasetRef().m_submarine.IsFlying(),
                                    IsMainViewWindow(), 
                                    rect, 
                                    GetViewport().m_left, 
                                    GetViewport().m_top,
                                    SliceChildView::m_eDbgShow == SHOW_BIOPSY,
                                    GetAutoflyMode(), 
                                    GetInverseImage() );


#ifdef RENDERSTATS
        LogDbg( std::string( "RenderSlice: " ) + renderSliceTimer.AsString(), "", "" );
        renderSceneTimer.Continue();
#endif
        break;
      }
    } // endswitch(m_eDbgShow)
		
    // reset window-iLevel
    glPixelTransferf(GL_RED_SCALE,1.0);
    glPixelTransferf(GL_GREEN_SCALE,1.0);
    glPixelTransferf(GL_BLUE_SCALE,1.0);
    glPixelTransferf(GL_RED_BIAS,0.0);
    glPixelTransferf(GL_GREEN_BIAS,0.0);
    glPixelTransferf(GL_BLUE_BIAS,0.0);

    m_sliceViewer.m_bForceSliceRender = false;

#ifdef RENDERSTATS
    LogDbg( std::string( "RenderScene: " ) + renderSceneTimer.AsString(), "", "" );
#endif
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceChildView", "GLRenderScene" );
  }
} // GLRenderScene()


/**
 * Render text, measurement lines, etc.
 */
void SliceChildView::CAxialSliceView_GLRasterScene()
{
  if (!m_sliceViewer.IsVolumeLoaded())
  {
    return;
  }

  CRect viewRect;
  GetClientRect(&viewRect);
  const CRect clientRect = viewRect - viewRect.TopLeft();

  GLFont & font = GetNormalFont();
  const int4 iTextHt(15);

  //{{ Slice Number and render mode
  const uint4 uSlice(!GetFullObliqueMode() ? m_sliceViewer.GetSliceNumber() : int4(GetFullObliqueSlice()));
  const uint4 uMaxSlice(!GetFullObliqueMode() ? m_sliceViewer.GetNumberOfSlices()-1 : int4(GetDatasetRef().m_joinedSkeleton.GetNumOfElements()-1));
  const int4 iTmpStrSz(200);
  int1 sTmpStr[iTmpStrSz+1];

  if (m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid())
  {
    uint4 uRenderModeStrID(0);
    switch (m_sliceViewer.m_sliceRenderer.GetRenderingMode())
    {
      case vxSliceRenderer::ThinSlab:
      {
        if ((m_sliceViewer.GetOrientation() != eCROSS_SECTION) && (m_sliceViewer.GetOrientation() != eVIEWDIR))
        {
          uRenderModeStrID = IDS_SLAB;
          int4 iLoSlice = Bound(int4(0), int4(floor(uSlice-m_sliceViewer.m_sliceRenderer.GetSlabThickness()/2)), int4(uMaxSlice));
          int4 iHiSlice = Bound(int4(0), int4(ceil(uSlice+m_sliceViewer.m_sliceRenderer.GetSlabThickness()/2)), int4(uMaxSlice));
          sprintf(sTmpStr, "%03d-%03d", iLoSlice, iHiSlice);
        }
        else
        {
          // NOTE: Identical blocks: A1
          uRenderModeStrID = IDS_SLICE;
          if ( m_sliceViewer.GetOrientation() == eCROSS_SECTION )
          {
            sprintf( sTmpStr, "%.1f cm", GetDatasetRef().m_vObliqueDistanceFromRectumMM / 10.0F );
          }
          else
          {
            sprintf( sTmpStr, "%03d", uSlice );
          }
        }
        break;
      }
      case vxSliceRenderer::MIP:
      {
        uRenderModeStrID = IDS_MIP;
        int4 iLoSlice = Bound(int4(0), int4(floor(uSlice-m_sliceViewer.m_sliceRenderer.GetSlabThickness()/2)), int4(uMaxSlice));
        int4 iHiSlice = Bound(int4(0), int4(ceil(uSlice+m_sliceViewer.m_sliceRenderer.GetSlabThickness()/2)), int4(uMaxSlice));
        sprintf(sTmpStr, "%03d-%03d", iLoSlice, iHiSlice);
        break;
      }
      case vxSliceRenderer::XRay:
      {
        break;
      }
      case vxSliceRenderer::Default:
      default:
      {
        // NOTE: Identical blocks: A1
        uRenderModeStrID = IDS_SLICE;
        //sprintf(sTmpStr, "%03d (%d)", uSlice, ++m_iDbgCounter); for debugging use
        if ( m_sliceViewer.GetOrientation() == eCROSS_SECTION )
        {
          if ( GetDatasetRef().m_vObliqueDistanceFromRectumMM >= 0.0F )
          {
            sprintf( sTmpStr, "%.1f cm", GetDatasetRef().m_vObliqueDistanceFromRectumMM / 10.0F );
          }
          else
          {
            sprintf( sTmpStr, " --- cm" );
          }
        }
        else
        {
          sprintf( sTmpStr, "%03d", uSlice );
        }
        break;
      }
    }

    glColor3f(1,0,0);
    m_menuRenderMode.Draw(LPCTSTR(LoadResourceString(uRenderModeStrID)), font, GDT_BELOWLEFT);
    //font.DrawText(clientRect.right - m_iTextPadding, clientRect.bottom, LPCTSTR(LoadResourceString(uRenderModeStrID)), GDT_BELOWLEFT);
    glColor3f(0,1,0);
    font.DrawText(clientRect.right - m_iTextPadding, clientRect.bottom - iTextHt, sTmpStr, GDT_BELOWLEFT);
  }
  else
  {
    //sprintf(sTmpStr, "%03d (%d)", uSlice, ++m_iDbgCounter); for debugging use
    if ( m_sliceViewer.GetOrientation() == eCROSS_SECTION )
    {
      sprintf( sTmpStr, "%.1f cm", GetDatasetRef().m_vObliqueDistanceFromRectumMM / 10.0F );
    }
    else
    {
      sprintf( sTmpStr, "%03d", uSlice );
    }
    glColor3f(0,1,0);
    font.DrawText(clientRect.right - m_iTextPadding, clientRect.bottom, LPCTSTR(LoadResourceString(IDS_SLICE)), GDT_BELOWLEFT);
    font.DrawText(clientRect.right - m_iTextPadding, clientRect.bottom - iTextHt, sTmpStr, GDT_BELOWLEFT);
  }
  //}} Slice Number


  //{{ display the frame rate in the lower right corner
#if defined(FINAL_RELEASE)
  if (EndoluminalView::m_bDisplayRenderSpeed)
#endif
  {
    const float4 fTime(m_timerFrameRate.ElapsedTime()); m_timerFrameRate.Reset();
    sprintf(sTmpStr, "%.1f Hz", fTime>0.0F ? 1.0F/fTime : 0.0F);
    font.DrawText(clientRect.right, clientRect.bottom - iTextHt*2, sTmpStr, GDT_BELOWLEFT); // At bottom-right
  }
  //}} display the frame rate in the lower right corner

  //{{ display the SRT measurements only when the mouse cursor touch the ROI
  if ( ( rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT()) ) && 
	  !( (GetViewModes() & ViewModes::SEGMENTPOLYP) && GetSegmentROIEditMode() & (ROI_VOXEL_ADD|ROI_VOXEL_DEL|ROI_REGION_REM)) )
  {
    CPoint pt;
    GetCursorPos( &pt );
    ScreenToClient( &pt );

    uint2 uComponentLabel(FindProximalROILabel(pt));

		if ( uComponentLabel > 0 && rdrGlobal.m_bDisplaySRTroi2D )
    { /// This point is close to a component, draw the ROI measurements
      Dataset * pDataset = m_sliceViewer.GetDataset();
      int4 n = pDataset->m_suspiciousRegions.GetCount();

      for( int4 i(0); i<n; i++ )
      {
        vxCadFinding finding( pDataset->m_suspiciousRegions.GetFinding(i) );
        vxCadROI     roi( finding.GetVolumeROI() );

        uint2 uLabel( roi.GetLabelIndex() );

        if( uLabel == uComponentLabel )
        {
          vxCadDimension largest( finding.GetDimensions().GetLargestDimension() );
          float fVolume( roi.GetVolumeValue() );

          float fsize(sqrt((largest.GetEndPoint().GetVector().m_x-largest.GetStartPoint().GetVector().m_x) *
                           (largest.GetEndPoint().GetVector().m_x-largest.GetStartPoint().GetVector().m_x) +
                           (largest.GetEndPoint().GetVector().m_y-largest.GetStartPoint().GetVector().m_y) *
                           (largest.GetEndPoint().GetVector().m_y-largest.GetStartPoint().GetVector().m_y) +
                           (largest.GetEndPoint().GetVector().m_z-largest.GetStartPoint().GetVector().m_z) *
                           (largest.GetEndPoint().GetVector().m_z-largest.GetStartPoint().GetVector().m_z)) );

          sprintf( sTmpStr, "ROI #%2d of %2d", (i+1), n);
          font.DrawText( pt.x, clientRect.Height()-pt.y+30, sTmpStr, GDT_BELOW );
          sprintf( sTmpStr, "Vol: %.0f mm^3", fVolume);
          font.DrawText( pt.x, clientRect.Height()-pt.y, sTmpStr, GDT_ABOVE );
          sprintf( sTmpStr, "Diam: %.1f mm", fsize);
          font.DrawText( pt.x, clientRect.Height()-pt.y, sTmpStr, GDT_BELOW );

          if( rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::OutputSuspiciousROI()) )
          { /// This turn on the CAD partner specific display for DICOM RCS world coordinate. 
            /// It is not for end user release product
            Vector3D<float4> vf = finding.GetCentroidMM().GetVector();
            Vector3D<int4>   vi = Vector3D<int4>( int4(vf.m_x/pDataset->m_vUnits.m_x+0.5F), int4(vf.m_y/pDataset->m_vUnits.m_y+0.5F), int4(vf.m_z/pDataset->m_vUnits.m_z+0.5F) ); 

            vf = pDataset->m_fields.m_vfShiftOffsetForDICOMWorldCoordinate ;
            ConvertVol2DicomWorldCoordinate( vi, vf, pDataset->m_vDim, pDataset->m_vUnits, pDataset->m_fields.m_sPatientOrientation );
            
            sprintf( sTmpStr, "Pt: (%.1f, %.1f, %.1f)  ", vf.m_x, vf.m_y, vf.m_z );
            font.DrawText( pt.x, clientRect.Height()-pt.y, sTmpStr, GDT_LEFT );           
          } // if

          break;
        } // if the label match 
      } // for all suspicious ROIs
    } // cursor near a suspicious ROI
  }
  //}} display SRT measurements =======================================================================

  //{{ Hounsfield Unit Value
  if (IsHUValueVisible())
  {
    m_bMouseWheelUpdated = false;
    if ( m_bCurrHounsfieldValueValid )
    {
      sprintf( sTmpStr, "%s: %03d", LoadResourceString(IDS_VALUE), m_iCurrHounsfieldValue);
    }
    else
    {
      sprintf( sTmpStr, "%s: --", LoadResourceString(IDS_VALUE) );
    }
    font.DrawText( int4( ( clientRect.Width() - font.GetPixelWidth( sTmpStr ) ) * 0.5F ), clientRect.bottom, sTmpStr, GDT_BELOW );
  }
  //}} Hounsfield Unit Value

  //{{ display electronic cleansing (or not)
  const Fields & fd = m_sliceViewer.GetDataset()->m_fields;
  const bool bECAvailable(fd.m_sliceRendererData.IsValid() ? fd.IsHQDiffVolLoaded() && fd.IsElecCleansed() : fd.IsDiffVolLoaded());
  if ( bECAvailable && !m_sliceViewer.m_bShowUncleansed )
  {
    if ( m_sliceViewer.GetDataset()->GetModality() != Dataset::MR )
    {
      sprintf( sTmpStr, "%s", LoadResourceString( IDS_ELEC_CLEANSING ) );
      font.DrawText( clientRect.Width() - m_iTextPadding, m_iTextPadding + iTextHt + m_iBottomExtraTextPadding, sTmpStr, GDT_LEFT );
    }
  }
  else
  {
    if ( m_sliceViewer.GetDataset()->GetModality() == Dataset::MR )
    {
      sprintf( sTmpStr, "%s", LoadResourceString( IDS_SUBTRACTED ) );
      font.DrawText( clientRect.Width() - m_iTextPadding, m_iTextPadding + iTextHt + m_iBottomExtraTextPadding, sTmpStr, GDT_LEFT );
    }
  }
  //}} display electronic cleansing (or not)

  //{{ draw voxel values when zoomed in greater then 2900%
  if( m_sliceViewer.IsOrthogonal()   && m_sliceViewer.GetZoomPercentage() > 2900 && 
      m_sliceViewer.GetBlockPixels() && !m_sliceViewer.m_bDisplayOverviewSliceGrasp )
  {
    uint4 uYStep = (clientRect.Height() * (m_sliceViewer.GetZoomPercentage()/100)) / m_sliceViewer.GetOrientationDim().m_y;
    uint4 uXStep = (clientRect.Width()  * (m_sliceViewer.GetZoomPercentage()/100)) / m_sliceViewer.GetOrientationDim().m_x;
	      
		if( 0 < m_currMousePoint.x && m_currMousePoint.x < clientRect.Width() &&
			  0 < m_currMousePoint.y && m_currMousePoint.y < clientRect.Height() )
		{
			for(uint4 y=m_currMousePoint.y-3*uYStep; y<=m_currMousePoint.y+5*uYStep; y+=uYStep) // don't ask me, but I verifiied doing this makes an even distribution with the PROPER hu values atop the PROPER pixels
			{
				for(uint4 x=m_currMousePoint.x-3*uXStep; x<=m_currMousePoint.x+5*uXStep; x+=uXStep)
				{
					float4 fXPos(float4(x)/float4(clientRect.Width()));
					float4 fYPos(float4(y)/float4(clientRect.Height()));
					m_sliceViewer.MovePickedLocationToVoxelCenter(fXPos,fYPos);
					uint4 uDrawX = fXPos*clientRect.Width();
					uint4 uDrawY = fYPos*clientRect.Height();
					bool bValidPosition( false );
					Point3Df unusedPosition;
					sprintf(sTmpStr, "%3d", GetHounsfieldValue(Point2D<int4>(uDrawX,uDrawY), bValidPosition, unusedPosition));
					if ( ! bValidPosition ) sprintf( sTmpStr, "--" );
					glColor3f(0,0,0);
					font.DrawText(uDrawX, clientRect.Height()-uDrawY, sTmpStr, GDT_ABOVELEFT);
					glColor3f(0,1,0);
					font.DrawText(uDrawX+1, clientRect.Height()-uDrawY+1, sTmpStr, GDT_ABOVELEFT);
				} // for x
			} // for y
		} // if the mouse cursor is in the window area
  } // if the 2D view is zoomed in the extreme level and in blockPixel mode
  //}} draw voxel values

  //{{ Right/Left/Superior/Inferior/Anterior/Posterior
  if (m_sliceViewer.IsOrthogonal())
  {
    int4 iPixelWidth( font.GetPixelWidth( m_sliceViewer.TopText() ));
    font.DrawText(int4( ( clientRect.Width() - iPixelWidth ) * 0.5F), int4(clientRect.bottom - iTextHt), m_sliceViewer.TopText(), GDT_BELOW);
    font.DrawText(int4( ( clientRect.Width() - iPixelWidth ) * 0.5F), m_iTextPadding + iTextHt + m_iBottomExtraTextPadding, m_sliceViewer.BottomText(), GDT_ABOVE);
    font.DrawText(int4(clientRect.Width()-m_iTextPadding), int4(clientRect.bottom * 0.5F), m_sliceViewer.RightText(), GDT_LEFT);
    font.DrawText(m_iTextPadding, int4(clientRect.bottom * 0.5F), m_sliceViewer.LeftText(), GDT_RIGHT);
  } // IsOrthogonal().
  //}} Right/Left/Superior/Inferior/Anterior/Posterior

  // Draw the centerline intersection point marker
  if (m_sliceViewer.IsCrossSectional() && rdrGlobal.m_bAlignCrossSectional)
  {
    glColor3f(1,0,0);
    CPoint centerPt = clientRect.CenterPoint();
    glBegin(GL_LINES);
    {
      glVertex2i(centerPt.x, centerPt.y - 5); glVertex2i(centerPt.x, centerPt.y + 5);
      glVertex2i(centerPt.x - 5, centerPt.y); glVertex2i(centerPt.x + 5, centerPt.y);
    }
    glEnd();
  } // endif (draw_intersection_point_marker)

	/// draw the 2D ruler for measuring reference
	if (rdrGlobal.m_bDisplay2DRuler) 
	{
		/// draw ruler line
		glLineWidth(1.0F);
		glBegin(GL_LINES);

		glColor3f(0, 0, 1);

    CPoint centerPt = clientRect.CenterPoint();
		float4 fd(clientRect.Width() / 16), fdd(fd*2.0f);
		int4 idd(centerPt.y + fdd), id(centerPt.y + fd), imdd(centerPt.y - fdd), imd(centerPt.y - fd);

    glVertex2i(19, idd);
    glVertex2i(19, imdd);

    glVertex2i(24, idd);
    glVertex2i(26, idd);
    glVertex2i(24, id);
    glVertex2i(26, id);
    glVertex2i(24, centerPt.y);
    glVertex2i(26, centerPt.y);
    glVertex2i(24, imd);
    glVertex2i(26, imd);
    glVertex2i(24, imdd);
    glVertex2i(26, imdd);

		glColor3f(0, 1, 0);
    glVertex2i(20, idd);
    glVertex2i(20, imdd);

    glVertex2i(20, idd);
    glVertex2i(23, idd);
    glVertex2i(20, id);
    glVertex2i(23, id);
    glVertex2i(20, centerPt.y);
    glVertex2i(23, centerPt.y);
    glVertex2i(20, imd);
    glVertex2i(23, imd);
    glVertex2i(20, imdd);
    glVertex2i(23, imdd);

		glEnd();

		Point3Df p1( Calculate3DPoint(Point<float>(20, id, 0)) );
		Point3Df p2( Calculate3DPoint(Point<float>(20, centerPt.y, 0)) );
		/// calculate unit length for the ruler
		float4 fUnitLength(p1.GetEuclideanDistance(p2));

		char cc[5] = "Unit";
		std::string ss = ToAscii(RoundDecimals(fUnitLength, 1));
		ss = ss + " mm";
		/// draw the unit length text information
		font.DrawText(2, idd + m_iTextPadding + iTextHt + m_iBottomExtraTextPadding, cc, GDT_ABOVE);
		font.DrawText(2, idd + m_iBottomExtraTextPadding, ss.c_str(), GDT_ABOVE);

	} // if
} // CAxialSliceView_GLRasterScene()


/**
 * Render text and other 2D objects
 */
void SliceChildView::GLRasterScene()
{
  try
  {
    
#ifdef RENDERSTATS
    Timer rasterTimer;
#endif

    if (!m_sliceViewer.IsVolumeLoaded())
    {
      GLChildView::GLRasterScene();
      return;
    }

    glPushAttrib(GL_CURRENT_BIT);

    Dataset * pDataset = m_sliceViewer.GetDataset();
    SliceOrientations::TypeEnum myOrientation(m_sliceViewer.GetOrientation());

    CRect viewRect;
    GetClientRect(&viewRect);
    CRect clientRect = viewRect - viewRect.TopLeft();
    int1 sTmpStr[101];

    const int4 iTextHt(15);

    // Positions where window text will be drawn
    const CPoint sliceOrtnPos(6,5);

    // OpenGL font
    GLFont & glFont = GetNormalFont();
    const uint4 uIdMenuMouseOver(GetPopupMenuID(0, CPoint(abs(m_currMousePoint.x),abs(m_currMousePoint.y))));

    //{{ Window/Level Preset
    {
      // TODO: Might be useful to make a hash table of the window/level values, each entry hashing to the preset and its name...might be.
      CString sWindowLevel;
      pair<int4,int4>& rWL = !SliceViewer::m_bShowUncleansed ? m_wlNormal : m_wlSecondary;

      glColor3f(1,0,0);
      for (int4 iWL(0); iWL < SliceChildView::eWLP_Count; iWL++)
      {
        if ( m_veWindowLevelPresets[iWL][1] == rWL.first && m_veWindowLevelPresets[iWL][2] == rWL.second )
        {
          m_menuWinLev.m_winMenu.GetMenuString(m_veWindowLevelPresets[iWL][3], sWindowLevel, MF_BYCOMMAND);
          break;
        }
      }
      if ( sWindowLevel.IsEmpty() )
      {
        for ( int4 iWL(0); iWL < m_mapWLSettings.m_vWL.size(); iWL++ )
        {
          if ( m_mapWLSettings.m_vWL[iWL].second.first == rWL.first && m_mapWLSettings.m_vWL[iWL].second.second == rWL.second )
          {
            sWindowLevel = m_mapWLSettings.m_vWL[iWL].first.c_str();
            break;
          }
        }
      }
      if ( sWindowLevel.IsEmpty() )
      {
        sWindowLevel.LoadString(IDS_CUSTOM);
      }

      // window/level preset name
      m_menuWinLev.Draw(LPCTSTR(sWindowLevel), glFont, GDT_BELOW);

      // window/level value
      sWindowLevel.Format("%d/%d", rWL.first, rWL.second);
      glColor3f(0,1,0);
      glFont.DrawText(m_iTextPadding, clientRect.bottom-iTextHt, LPCTSTR(sWindowLevel), GDT_BELOW); // Second Line At top-left
    }
    //}} endblock(Window/Level Preset)

    //{{ Display 3d position x,y,z value
    glColor3f(0,1,0);
    switch (SliceChildView::m_eCoordDisplay)
    {
      case SliceChildView::CoordDisplay::eWorld:
      { /// the following shifting offset will convert VX world to the standardized DICOM world coordinates.
        const Triple<float4> units( m_sliceViewer.m_bSliceRendererEnabled ? pDataset->m_fields.m_units2D : pDataset->m_vUnits );
        const Triple<int4>   vDim ( pDataset->m_vDim );
        
        Vector3D<float4> vfWorldRCSdicom( pDataset->m_fields.m_vfShiftOffsetForDICOMWorldCoordinate );
        Vector3D<int4>   viVolCoordinate(m_currHounsfieldSamplePosition.m_x, m_currHounsfieldSamplePosition.m_y, m_currHounsfieldSamplePosition.m_z);

        ConvertVol2DicomWorldCoordinate( viVolCoordinate, vfWorldRCSdicom, vDim, units, pDataset->m_fields.m_sPatientOrientation );

        if ( m_bCurrHounsfieldValueValid )
        {
          sprintf( sTmpStr, "W(%.1f, %.1f, %.1f)", vfWorldRCSdicom.m_x, vfWorldRCSdicom.m_y, vfWorldRCSdicom.m_z );
        }
        else
        {
          sprintf( sTmpStr, "W(--,--,--)" );
        }
        glFont.DrawText(m_iTextPadding, clientRect.bottom-iTextHt*2, sTmpStr, GDT_BELOW);
      }
      break;
      case SliceChildView::CoordDisplay::eVolume:
      {
        if ( m_bCurrHounsfieldValueValid )
        {
          sprintf(sTmpStr, "V(%d, %d, %d)", int4(m_currHounsfieldSamplePosition.m_x + 0.5F),
          int4(m_currHounsfieldSamplePosition.m_y + 0.5F), int4(m_currHounsfieldSamplePosition.m_z + 0.5F));
        }
        else
        {
          sprintf( sTmpStr, "V(--,--,--)" );
        }
        glFont.DrawText(m_iTextPadding, clientRect.bottom-iTextHt*2, sTmpStr, GDT_BELOW);
      }
      break;
      case SliceChildView::CoordDisplay::eNone:
      default:
      {
        break;
      }
    }
    //}} Display 3d position value

    //{{ Slice orientation
    {
      CString sOrientation;
      CMenu * pSubMenu = m_menuSliceOrientation.m_winMenu.GetSubMenu(0);
      if (pSubMenu != (CMenu*) NULL)
      {
        switch (myOrientation)
        {
          case SliceOrientations::eAXIAL:
          {
            pSubMenu->GetMenuString(ID_SLICEORIENTATION_AXIAL, sOrientation, MF_BYCOMMAND);
            break;
          }
          case SliceOrientations::eSAGITTAL:
          {
            pSubMenu->GetMenuString(ID_SLICEORIENTATION_SAGITTAL, sOrientation, MF_BYCOMMAND);
            break;
          }
          case SliceOrientations::eCORONAL:
          {
            pSubMenu->GetMenuString(ID_SLICEORIENTATION_CORONAL, sOrientation, MF_BYCOMMAND);
            break;
          }
          case SliceOrientations::eCROSS_SECTION:
          {
            pSubMenu->GetMenuString(ID_SLICEORIENTATION_CROSSSECTIONAL, sOrientation, MF_BYCOMMAND);
            break;
          }
          case SliceOrientations::eVIEWDIR:
          {
            pSubMenu->GetMenuString(ID_SLICEORIENTATION_VIEWDIRECTION, sOrientation, MF_BYCOMMAND);
            break;
          }
          default:
          {
            break;
          }
        } // endswitch(m_sliceViewer.Orientation).
      }

      sOrientation.Remove('&');
      
      #ifndef FINAL_RELEASE
        if( m_sliceViewer.m_bSliceRendererEnabled && m_sliceViewer.GetDataset()->m_fields.m_sliceRendererData.IsValid() )
          sOrientation += " 12bit";
      #endif

      // slice orientation
      glColor3f(1,0,0);
      m_menuSliceOrientation.Draw(LPCTSTR(sOrientation), glFont, GDT_ABOVE,
        IsMainViewWindow() || (GetViewWndType() & eGVT_CROSS_SECTION));
    }
    //}} Slice orientation

    //{{ Voxel edit size
    if ( !IsReportRendering() && IsROIVoxelEditMode() )
    {
      glPushAttrib(GL_CURRENT_BIT);
      {
        // at third line at lower left
        static Image< PixelRGBA<uint1> > * pImageSizeBar = NULL;
        if ( pImageSizeBar == NULL )
        {
          pImageSizeBar = GdiplusUtilities::LoadImageFromResource( MAKEINTRESOURCE( IDB_REGIONSIZE_BTNBAR ), "PNG" );
          pImageSizeBar->FlipVertical();
        }

        if ( pImageSizeBar != NULL )
        {
          glPixelStorei( GL_UNPACK_ROW_LENGTH, pImageSizeBar->GetDim().m_x );
          glPixelTransferf( GL_ALPHA_SCALE, 1.0F );

          Rect<int4> btnRect(Point2D<int4>(m_iTextPadding, (iTextHt*3)), Point2D<int4>(pImageSizeBar->GetDim().m_x/2, pImageSizeBar->GetDim().m_y/2));
          glRasterPos2i( btnRect.m_left, btnRect.m_top );

          m_btnbarVoxelSize[1].m_rect = btnRect;
          btnRect.Offset(pImageSizeBar->GetDim().m_x/2, 0); 
          m_btnbarVoxelSize[2].m_rect = btnRect;

          const int4 iOneLineSz((GetSegmentROIVoxelSize() == 2) ? sizeof(PixelRGBA<uint1>)*((pImageSizeBar->GetDim().m_y/2)*pImageSizeBar->GetDim().m_x) : 0);
          glDrawPixels( pImageSizeBar->GetDim().m_x, pImageSizeBar->GetDim().m_y/2, GL_RGBA, GL_UNSIGNED_BYTE,
            reinterpret_cast< GLubyte * >( pImageSizeBar->GetDataPtr() ) + iOneLineSz );
        }
      }
      glPopAttrib();
    }
    //}} Voxel edit size

    //{{ Region edit mode
    if ( !IsReportRendering() && IsMainViewWindow() && (GetViewModes() & ViewModes::SEGMENTPOLYP) )
    {
      glPushAttrib(GL_CURRENT_BIT);
      {
        // at second line at lower left
        static Image< PixelRGBA<uint1> > * pImageEditBtnBar = NULL;
        if ( pImageEditBtnBar == NULL )
        {
          pImageEditBtnBar = GdiplusUtilities::LoadImageFromResource( MAKEINTRESOURCE( IDB_REGIONEDIT_BTNBAR ), "PNG" );
          pImageEditBtnBar->FlipVertical();
        }
				
        if ( pImageEditBtnBar != NULL )
        {
          glPixelStorei( GL_UNPACK_ROW_LENGTH, pImageEditBtnBar->GetDim().m_x );
          glPixelTransferf( GL_ALPHA_SCALE, 1.0F );

          Rect<int4> btnRect(Point2D<int4>(m_iTextPadding, (m_iTextPadding*3 + iTextHt)), Point2D<int4>(pImageEditBtnBar->GetDim().m_x/4, pImageEditBtnBar->GetDim().m_y/4) );
          glRasterPos2i( btnRect.m_left, btnRect.m_top );

          int4 iSel(-1), iCount(0);
          for ( GLSButtonBar::iterator it = m_btnbarEditMode.begin(); it != m_btnbarEditMode.end(); it++, btnRect.Offset(pImageEditBtnBar->GetDim().m_x/4, 0), iCount++ )
          {
            if (it->first == GetSegmentROIEditMode()) // highlight the selected mode
            {
              iSel = iCount;
            }
            it->second.m_rect = btnRect;
          }

          const int4 iOneSetSz(sizeof(PixelRGBA<uint1>)*((pImageEditBtnBar->GetDim().m_y/4)*pImageEditBtnBar->GetDim().m_x));
          glDrawPixels( pImageEditBtnBar->GetDim().m_x, pImageEditBtnBar->GetDim().m_y/4, GL_RGBA, GL_UNSIGNED_BYTE,
            reinterpret_cast< GLubyte * >( pImageEditBtnBar->GetDataPtr() ) + iOneSetSz*(3-iSel));
        }
      }
      glPopAttrib();
    }
    //}} Region edit mode

    if ( IsReportRendering() )
    {
      glPushAttrib(GL_CURRENT_BIT);
      glColor3f(0,1,0);
      CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
      CRect clientRect = viewRect - viewRect.TopLeft();
      std::string sPatInfo = pDataset->m_studyinfo.m_sPatientID + ", " + pDataset->m_studyinfo.m_sExamTime;
      const int4 iAdjFontSz(min(-1 * viewRect.Width() * 0.03, -11));
      GLFont fontAdj; fontAdj.Create( GetHDC(), iAdjFontSz, 0, "arial", FW_SEMIBOLD, false, false, true );
      fontAdj.DrawText(m_iTextPadding, clientRect.bottom - 2*(m_iTextPadding + iTextHt) + m_iBottomExtraTextPadding, sPatInfo.c_str(), GDT_BELOW); // Third Line At top-left
      glPopAttrib();
    }

    // Zoom factor; assume that it is square (and it should be!)
    {
      sprintf(sTmpStr, "%d%%", m_sliceViewer.GetZoomPercentage());
      // zoom factor
      m_menuZoom.Draw(sTmpStr, glFont, GDT_ABOVELEFT, IsMainViewWindow());
    } // end draw zoom factor

    // Draw measurement from m_measureList
    const VtxViewModes viewModes(ViatronStudy::GetViewModes(GetViewWndType()));
    if ( (viewModes & ViewModes::LINEMEASURE) && (!m_sliceViewer.m_measureLineList.empty()) )
    {
      static const float4 fGray(0.7F);
      // draw all of them in gray
      glColor3f(fGray, fGray, fGray);
      for (MeasureLineList::iterator listCurr = m_sliceViewer.m_measureLineList.begin(); listCurr != m_sliceViewer.m_measureLineList.end(); listCurr++)
      {
        RenderLinearMeasurement(listCurr, clientRect);
      } // end for loop thru measure list

      Dataset& myDatasetRef = *m_sliceViewer.GetDataset();

      // draw the primary line in white
      if (m_sliceViewer.m_measureLineList.m_currentIter != m_sliceViewer.m_measureLineList.end())
      {
        // glow primary line if more than one
        static Timer glowTimer;
        const float4 fFlashInterval(1.5F); // seconds
        const float4 fDistFromOne(1.0F - fabs(( float4)fmod( (float4)glowTimer.ElapsedTime(), fFlashInterval) - fFlashInterval / 2.0F));
        const float4 fIntensity(Bound(0.0F, fGray + (1.0F - fGray) * fDistFromOne, 1.0F));
        glColor3f(1,1,1);

        RenderLinearMeasurement(m_sliceViewer.m_measureLineList.m_currentIter, clientRect);
      } // glow primary line if more than one

      // 2D Line Measurement profile
      if (ColonViatronixWnd::IsDisplaying2DMeasurementProfile())
      {
        // Draw the profile view
        try
        {
          VCRenderUtils::RenderMeasurementProfile(m_sliceViewer.m_measureLineList,
            Rect<uint4>(clientRect.left, clientRect.top, clientRect.right, clientRect.bottom),
            myDatasetRef.m_biopsyTransferFunction,
            myDatasetRef.m_fields.m_volumeRender,
            myDatasetRef.m_fields.m_volumeRender.GetWorldToVolumeMatrix());
        }
        catch (...)
        {
          // Error::WarnLog("<SliceChildView::GLRasterScene> Unhandled exception.");
          ex::RuntimeException(LogRec("<SliceChildView::GLRasterScene> Unhandled exception.", "SliceChildView", "GLRasterScene"));
        }
      } // if displaying 2D measure profile
    } // end draw measurement list

    // 2D annotations
    BookmarkList& list = m_sliceViewer.GetDataset()->m_session.m_2DAnnotationList;
    if (!list.empty() && (m_sliceViewer.GetOrientation() != SliceOrientations::eCROSS_SECTION) && (m_sliceViewer.GetOrientation() != SliceOrientations::eVIEWDIR) )
    {
      glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
      glLineWidth(4.0F);

      float4 arrowLength = (m_sliceViewer.m_uCameraArrowLength / m_sliceViewer.m_rZoom.Width()) * 450 / clientRect.Width(); // 450 ??? Dont ask me, it works ?????

      GLFont & annTextFont = GetNormalFont(); // IsMainViewWindow() ? GetLargeFont() : 
      for (BookmarkList::iterator it = list.begin(); it != list.end(); it++)
      {
        Point<float4> vPosVolumeCoord = it->m_position.GetDividedAnIso(pDataset->m_fields.m_units2D);
        if (/*(SliceOrientations::TypeEnum(it->m_ePriority) == myOrientation) &&*/ (m_sliceViewer.DistFromCurrentSlice(vPosVolumeCoord) < m_sliceViewer.m_uAnnotationSliceDiffThreshold) )
        {
          // Calculate the position of the tip and back of the arrow in GL coords.
          Point<float4> tip = m_sliceViewer.Calc2DGLPosFromVolumeCoord(vPosVolumeCoord);
          Point<float4> backWC = it->m_position + (Vector<float4>(it->m_orientation)*(-arrowLength));
          Point<float4> back = m_sliceViewer.Calc2DGLPosFromVolumeCoord(backWC.GetDividedAnIso(pDataset->m_fields.m_units2D));
          Point2D<float4> pos2DTip(tip.m_x * clientRect.Width(),tip.m_y * clientRect.Height());
          Point2D<float4> pos2DBack(back.m_x * clientRect.Width(),back.m_y * clientRect.Height());

            // draw text if visible
          if( it->m_bVisible )
          {
            // position the text to the right of the rightmost point
            Point2D<float4> leftPoint( pos2DBack );
            Point2D<float4> rightPoint( pos2DTip );
            if ( leftPoint.m_x > rightPoint.m_x ) 
              Swap( leftPoint, rightPoint );

            Point2D<float4> textPos( leftPoint.m_x - 2 * m_iTextPadding, (float4)( leftPoint.m_y - annTextFont.GetPixelHeight() / 2 + m_iTextPadding ) );

            char sTok[] = {13,10,0};
            CString sResTok;
            CString sMultiLineString = it->m_sName.c_str();
            int4 iCurTokPos(0);  int4 iLineY(0);
            for (CString sResTok = sMultiLineString.Tokenize(sTok, iCurTokPos); !sResTok.IsEmpty();
              sResTok = sMultiLineString.Tokenize(sTok, iCurTokPos), iLineY += 20)
            {
              // make sure it fits on the screen
              if( (textPos.m_x - annTextFont.GetPixelWidth( it->m_sName ) - 2*m_iTextPadding) >= 0 )
              {
                glColor3f(0.0F, 0.0F, 0.0F);
                annTextFont.DrawText( textPos.m_x+1, textPos.m_y-iLineY-1, LPCTSTR(sResTok), GDT_LEFT );
                glColor3f(1.0F, 1.0F, 1.0F);
                annTextFont.DrawText( textPos.m_x, textPos.m_y-iLineY, LPCTSTR(sResTok), GDT_LEFT );
              }
              else
              {
                textPos = Point2D<float4>( rightPoint.m_x + 2 * m_iTextPadding, leftPoint.m_y - annTextFont.GetPixelHeight() / 2 + m_iTextPadding - iLineY);

                glColor3f(0.0F, 0.0F, 0.0F);
                annTextFont.DrawText( textPos.m_x+1, textPos.m_y-1, LPCTSTR(sResTok), GDT_RIGHT );            
                glColor3f(1.0F, 1.0F, 1.0F);
                annTextFont.DrawText( textPos.m_x, textPos.m_y, LPCTSTR(sResTok), GDT_RIGHT );            
              }
            }
          }

          // Highlight the current annotation, or, if no current, that which is under the mouse, if any
          enum HighlightTypeEnum { eHTENone, eHTETip, eHTEBack };
          HighlightTypeEnum eHighlight(eHTENone);

          if (m_pCurrAnnotation.first == it)
          {
            eHighlight = m_pCurrAnnotation.second ? eHTETip : eHTEBack;
          }
          else if (!m_bCurrMovingWorldLoc && (m_pCurrAnnotation.first == list.end()))
          {
            if (m_pMouseOverAnnotation.first == it)
            {
              eHighlight = m_pMouseOverAnnotation.second ? eHTETip : eHTEBack;
            }
          } // endif

          if (eHighlight == eHTETip)
          { // Highlight position point
            glPushAttrib(GL_CURRENT_BIT);
              glColor3f(0.0F, 0.90F, 0.0F);
              RenderUtil::GLDrawCircle(pos2DTip, 5, false);
            glPopAttrib();
          }
          else if (eHighlight == eHTEBack)
          { // Highlight orientation point
            glPushAttrib(GL_CURRENT_BIT);
              glColor3f(0.80F, 0.90F, 0.0F);
              RenderUtil::GLDrawCircle(pos2DBack, 3, true);
              RenderUtil::GLDrawCircle(pos2DBack, 5, false);
            glPopAttrib();
          }
        } // endif display the current annotation
      } // endfor all annotations
      glPopAttrib();
    } // endif annotations to display

    // display text for 2D CAD findings
    if ( rdrGlobal.m_bDisplayCadFindings &&
         pDataset->m_cadFindings.GetCount() > 0 &&
         ( m_sliceViewer.GetOrientation() == SliceOrientations::eAXIAL ||
           m_sliceViewer.GetOrientation() == SliceOrientations::eSAGITTAL ||
           m_sliceViewer.GetOrientation() == SliceOrientations::eCORONAL ) )
    {
      glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);

      float4 fArrowLength = ( m_sliceViewer.m_uCameraArrowLength / m_sliceViewer.m_rZoom.Width() ) * 450 / clientRect.Width(); // 450 ??? Dont ask me, it works ?????

      GLFont & annTextFont = GetNormalFont();
      int4 iFindingCounter(0);
      for ( int4 i(0); i < pDataset->m_cadFindings.GetCount(); ++i )
      {
        const vxCadFinding & cadFinding = pDataset->m_cadFindings.GetFinding( i );
        if ( cadFinding.GetCentroidMM().GetUnits() != vxCadVector::MILLIMETER )
        {
          LogErr( "Expected CAD finding centroid to be in world units", "SliceChildView", "GLRasterScene" );
          continue;
        }
        Point<float4> vPosVolumeCoord = Point3Df( cadFinding.GetCentroidMM().GetVector().GetDividedAnIso( pDataset->m_fields.m_units2D ) );

        const bool bFindingVisible = RenderUtil::IsFindingVisible( cadFinding, pDataset->m_session.m_mapCadFindingInfo, pDataset->m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings);
        if ( bFindingVisible )
          iFindingCounter++;
        const bool bNearCurrentSlice = m_sliceViewer.DistFromCurrentSlice( vPosVolumeCoord ) < m_sliceViewer.m_uAnnotationSliceDiffThreshold;
        if ( bNearCurrentSlice )
        {
          // draw text if visible
          const bool bFindingNotDenied = RenderUtil::GetFindingDecision( cadFinding, pDataset->m_session.m_mapCadFindingInfo ) != ColonSession::FindingInfo::eDecisionDenied;
          if ( bFindingVisible && bFindingNotDenied )
          {

            // calculate the text position
            Point<float4> center = m_sliceViewer.Calc2DGLPosFromVolumeCoord( vPosVolumeCoord );
            const Point2D<float4> fTextPlacement( 30, -1 );
            Point2D<float4> center2D( center.m_x * clientRect.Width() + fTextPlacement.m_x, center.m_y * clientRect.Height() + fTextPlacement.m_y );
            Point2D<float4> textPos( center2D.m_x - 2 * m_iTextPadding, (float4)( center2D.m_y - annTextFont.GetPixelHeight() / 2 + m_iTextPadding ) );

            // draw
            std::string sText = ToAscii( iFindingCounter );
            glColor3f( 0, 0, 0 );
            annTextFont.DrawText( textPos.m_x+1, textPos.m_y-1, sText, GDT_LEFT );
            glColor3f( 1, 1, 1 );
            annTextFont.DrawText( textPos.m_x, textPos.m_y, sText, GDT_LEFT );

          } // if cad annotation visible
        } // endif display the current cad annotation
      } // endfor all cad annotations
      glPopAttrib();
    } // endif CAD findings to display

    //{{ Hovering/Captured world location
    if (m_bCurrMovingWorldLoc || m_bMouseOverWorldLoc)
    {
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
      Point<float4> tip = m_sliceViewer.Calc2DGLPosFromVolumeCoord(pDataset->m_submarine.GetPositionWorld().GetDividedAnIso(pDataset->m_fields.m_units2D));
      Point2D<float4> pos2DTip(tip.m_x * clientRect.Width(),tip.m_y * clientRect.Height());
      if (IsKeyStateDown(VK_RBUTTON))
      { // Highlight orientation point
        glColor3f(0.80F, 0.90F, 0.0F);
        RenderUtil::GLDrawCircle(pos2DTip, 3, true);
        RenderUtil::GLDrawCircle(pos2DTip, 5, false);
      }
      else
      { // Highlight position point
        glColor3f(0.0F, 0.90F, 0.0F);
        RenderUtil::GLDrawCircle(pos2DTip, 5, false);
      }
    glPopAttrib();
    }
    //}} Hovering/Captured world location


    //{{ ROI measurements
    if ( viewModes & ViewModes::ROIMEASURE )
    {
      // Draw roi measurement
      MeasureAreaList::iterator listCurr = m_sliceViewer.m_measureAreaList.begin();
      MeasureAreaList::iterator listEnd = m_sliceViewer.m_measureAreaList.end();
      for (; listCurr != listEnd; listCurr++)
      {
        Triple<float4> vColor(1.0F, 1.0F, 1.0F);
        if (m_sliceViewer.m_measureAreaList.size() > 1)
        {
          // Highlight the current line
          if (listCurr == m_sliceViewer.m_measureAreaList.m_currentIter)
          {
            vColor = Triple<float4>(1.0F, 1.0F, 1.0F);
          }
          else
          {
            vColor = Triple<float4>(0.8F, 0.8F, 0.8F);
          }
        } // if more than one line

        RenderAreaMeasurement(listCurr, clientRect, vColor);
      } // for loop thru measure area list

      glColor3f(0,1,0);
      // ROI Measurement Histogram
      if (m_sliceViewer.m_measureAreaList.m_currentIter != m_sliceViewer.m_measureAreaList.end())
      {
        if ( m_iRoiSlice != m_sliceViewer.GetCurrentOrthoSlice() )
          m_bUpdateRoiMeasurement = true;
        if ( m_bUpdateRoiMeasurement )
        {
          CalculateRoiStatistics( m_sliceViewer.m_measureAreaList.m_currentIter );
          m_bUpdateRoiMeasurement = false;
          m_iRoiSlice = m_sliceViewer.GetCurrentOrthoSlice();
        }

        // Display histogram statistics
        CString sDiam, sArea, sAvgHu, sMaxHu, sStdHu;

		// The major radius is not an intuitive size indicator. Don't show it. DQ 20120104
        //sDiam.Format(IDS_ROI_MAJOR_DIAMETER_FORMAT, int4(m_sliceViewer.m_measureAreaList.m_currentIter->m_fMajorDiameterMM / 2 + 0.5F));
        //glFont.DrawText(m_iTextPadding, clientRect.Height()-iTextHt*4, LPCTSTR(sDiam), GDT_ABOVE); // third line at upper left

        sArea.Format(IDS_ROI_AREA_FORMAT, int4(m_sliceViewer.m_measureAreaList.m_currentIter->m_fAreaSquareMM + 0.5F));
        glFont.DrawText(m_iTextPadding, clientRect.Height()-iTextHt*5, LPCTSTR(sArea), GDT_ABOVE); // fourth line at upper left

        sAvgHu.Format(IDS_ROI_AVERAGE_HU_FORMAT, m_sliceViewer.m_measureAreaList.m_currentIter->m_fAverageHU);
        glFont.DrawText(m_iTextPadding, clientRect.Height()-iTextHt*6, LPCTSTR(sAvgHu), GDT_ABOVE); // fifth line at upper left

        sMaxHu.Format(IDS_ROI_MAX_HU_FORMAT, m_sliceViewer.m_measureAreaList.m_currentIter->m_fMaximumHU);
        glFont.DrawText(m_iTextPadding, clientRect.Height()-iTextHt*7, LPCTSTR(sMaxHu), GDT_ABOVE); // sixth line at upper leftt

        sStdHu.Format(IDS_ROI_STD_HU_FORMAT, m_sliceViewer.m_measureAreaList.m_currentIter->m_fStandardDeviationHU);
        glFont.DrawText(m_iTextPadding, clientRect.Height()-iTextHt*8, LPCTSTR(sStdHu), GDT_ABOVE); // seventh line at upper left

      } // if render histogram for the current roi
    } // endif (viewModes & ViewModes::ROIMEASURE)
    //}} ROI measurements

    CAxialSliceView_GLRasterScene();

    GLRenderLogo();

    glPopAttrib();


#ifdef RENDERSTATS
    LogDbg( std::string( "RasterScene: " ) + rasterTimer.AsString(), "", "" );
    m_debugTimerOutside.Reset();
#endif
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "SliceChildView", "GLRasterScene" );
  }

} // GLRasterScene()


/**
 * Renders a measurement line
 *
 * @param lineIter   Line to render
 * @param rect       View rectangle in which to draw
 */
void SliceChildView::RenderLinearMeasurement(MeasureLineList::iterator & lineIter, const CRect & rect)
{
  glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);

  Point2D<int> drawPt1 = lineIter->m_drawEnds.m_start; // GetSliceToView(lineIter->m_drawEnds.m_start, rect);
  Point2D<int> drawPt2 = lineIter->m_drawEnds.m_end; // GetSliceToView(lineIter->m_drawEnds.m_end, rect);

  Dataset& myDatasetRef = *m_sliceViewer.GetDataset();

  // flip the points for OpenGL drawing
  drawPt1.m_y = rect.Height() - drawPt1.m_y;
  drawPt2.m_y = rect.Height() - drawPt2.m_y;

  glLineWidth(2.0);
  glBegin(GL_LINES);
    glVertex2i(drawPt1.m_x, drawPt1.m_y);
    glVertex2i(drawPt2.m_x, drawPt2.m_y);
  glEnd();

  // Circles to mark endpoints
  // Perry doesn't like them!
  //RenderUtil::GLDrawCircle(Point2D<float4>(drawPt1.m_x, drawPt1.m_y), 5.0f);
  //RenderUtil::GLDrawCircle(Point2D<float4>(drawPt2.m_x, drawPt2.m_y), 5.0f);

  // text
  const float4 fDist(lineIter->m_f3Ddistance);

  if ( (int4(fDist * 10.0f) != 0) && (rect.PtInRect(CPoint(drawPt2.m_x,drawPt2.m_y))) )
  {
    int1 sTmpStr[1000];
    static const CString sMeasureFormat(LoadResourceString(IDS_LINEMEASUREMENT_FORMAT));
    sprintf(sTmpStr, sMeasureFormat, fDist, /*int4(myDatasetRef.m_vUnits.m_z + 0.5),*/ LoadResourceString(IDS_MM));

    Point2D<int4> textPt;

    if (lineIter->m_ePosn == MeasureLine::START)
    {
      textPt = drawPt1;
      textPt.m_y += (drawPt1.m_y > drawPt2.m_y) ? 5 : -15;
    }
    else if (lineIter->m_ePosn == MeasureLine::END)
    {
      textPt = drawPt2;
      textPt.m_y += (drawPt2.m_y > drawPt1.m_y) ? 5 : -15;
    }
    else
    {
      textPt = (drawPt1 + drawPt2) / 2;
      if(abs(drawPt1.m_x - drawPt2.m_x) > abs(drawPt1.m_y - drawPt2.m_y))
      {
        textPt.m_y -= 20;
      }
      else
      {
        textPt.m_x += 10;
      }
    } // endif(lineIter->m_ePosn)

    uint4 uTextPos = 0;
    uTextPos |= (textPt.m_x >= rect.right - 100) ? GDT_LEFT : GDT_RIGHT;
    uTextPos |= (textPt.m_y >= rect.bottom - 20) ? GDT_BELOW: GDT_ABOVE;

    glColor3f(0.0F, 0.0F, 0.0F);
    GetNormalFont().DrawText(textPt.m_x+1, textPt.m_y-1, sTmpStr, GDT_TEXTLOC(uTextPos));
    glColor3f(1.0F, 1.0F, 1.0F);
    GetNormalFont().DrawText(textPt.m_x, textPt.m_y, sTmpStr, GDT_TEXTLOC(uTextPos));
  } // endif (dist != 0)

  glPopAttrib();
} // RenderLinearMeasurement( lineIter, rect )


/**
 * Render an area (ROI) measurement
 *
 * @param areaIter   ROI to render
 * @param rect       View window rectangle
 * @param color      ROI Color
 */
void SliceChildView::RenderAreaMeasurement(MeasureAreaList::iterator & areaIter, const CRect & rect, const Triple<float> & color) const
{
  glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);

  Matrix<float4> mZoom;
  mZoom.Scale(Vector<float4>(m_sliceViewer.m_rZoom.Width(), m_sliceViewer.m_rZoom.Height(), 1.0F));
  mZoom.Scale(Vector<float4>(1.0F / m_sliceViewer.m_uDisplayWidth, 1.0F / m_sliceViewer.m_uDisplayHeight, 1.0F));
  mZoom.Translate(Vector<float4>(m_sliceViewer.m_rZoom.m_left, m_sliceViewer.m_rZoom.m_top, 0.0F));
  mZoom.Scale(Vector<float4>(m_sliceViewer.m_uDisplayWidth, m_sliceViewer.m_uDisplayHeight, 1.0F));

  // draw the ellipse
  const uint4 uSteps = 100;
  glLineWidth(2.0);
  const float4 fLighter = 0.8F;
  glColor3f(fLighter * color.m_x, fLighter * color.m_y, fLighter * color.m_z);
  glBegin(GL_LINE_STRIP);
  for (uint4 i=0; i < uSteps; i++)
  {
    const float4 fAngleRadians = 2.0F * M_PI * float4(i) / float4(uSteps-2); // steps-2 to complete the circle
    Point<float4> positionPt(areaIter->m_centerDrawPt + areaIter->m_majorAxisDrawVector * cos(fAngleRadians) + areaIter->m_minorAxisDrawVector * sin(fAngleRadians));
    positionPt.m_y = rect.Height() - positionPt.m_y;
    glVertex2f(positionPt.m_x, positionPt.m_y);
  } // loop over all angles of the ellipse
  glEnd();

  // draw the handles as little squares
  glColor3f(color.m_x, color.m_y, color.m_z);
  typedef Point<float4> PointFloat;
  std::vector<PointFloat> vPoints = areaIter->GetDrawPoints();
  for (uint4 i=0; i < vPoints.size(); i++)
  {
    Point<float4> & point = vPoints[i];
    point.m_y = rect.Height() - point.m_y;
    point.m_x -= 1.0F; // move for easier picking
    point.m_y += 3.5F;
    glLineWidth(1.0);
    glBegin(GL_LINE_STRIP);
    glVertex2f(point.m_x - 2, point.m_y - 2);
    glVertex2f(point.m_x + 2, point.m_y - 2);
    glVertex2f(point.m_x + 2, point.m_y + 2);
    glVertex2f(point.m_x - 2, point.m_y + 2);
    glVertex2f(point.m_x - 2, point.m_y - 2);
    glEnd();
  } // loop over points of area

  glPopAttrib();

} // RenderAreaMeasurement( areaIter, rect, color )


//////////////////////////////////////////////////////////////////////////////////////
// ... Methods


/**
 * measure in 2D slices
 *
 * @param eMeasureFunc  a enum value to choose measure function.
 * @param point         coordinate of a point in the window.
 * @return              false when measure action not applied, else, true.
 */
bool SliceChildView::LineMeasurement(const MEASUREFUNCENUM eMeasureFunc, const CPoint point)
{
  bool bUpdated = false;
  CRect rect;
  GetClientRect(&rect);

  switch (eMeasureFunc)
  {
    case MEASUREFUNC_START:
    {
      MeasureLineList::iterator listCurr = m_sliceViewer.m_measureLineList.begin();
      MeasureLineList::iterator listEnd = m_sliceViewer.m_measureLineList.end();

      // Find the closest proximal line (if any) to the clicked point
      m_sliceViewer.m_measureLineList.m_currentIter = m_sliceViewer.m_measureLineList.end();

      float4 fLeastDist = 20.0f / m_sliceViewer.m_rZoom.Height();
      bool bSwapEnds = false;

      Point2D<int4> point2D(point.x, point.y);
      for (; listCurr != listEnd; listCurr++)
      {
        const float4 vfDist[2] = { Distance(/*GetSliceToView(*/listCurr->m_drawEnds.m_start/*, rect)*/, point2D),
                                   Distance(/*GetSliceToView(*/listCurr->m_drawEnds.m_end/*, rect)*/, point2D) };

        if (vfDist[0] < fLeastDist)
        {
          bSwapEnds = true;
          m_sliceViewer.m_measureLineList.m_currentIter = listCurr;
          fLeastDist = vfDist[0];
        }
        if (vfDist[1] < fLeastDist)
        {
          bSwapEnds = false;
          m_sliceViewer.m_measureLineList.m_currentIter = listCurr;
          fLeastDist = vfDist[1];
        }
      } // endloop(listCurr)

      if (m_sliceViewer.m_measureLineList.m_currentIter != m_sliceViewer.m_measureLineList.end())
      {
        if (bSwapEnds)
        {
          m_sliceViewer.m_measureLineList.m_currentIter->SwapEnds();
        }
        m_sliceViewer.m_measureLineList.m_currentIter->m_ePosn = m_sliceViewer.m_measureLineList.GetUniquePos(m_sliceViewer.m_measureLineList.m_currentIter);
      }
      else
      {  // A start point for a new line has been clicked
        MeasureLine measureLine;
        measureLine.m_measureEnds.m_start = measureLine.m_measureEnds.m_end = Calculate3DPoint(Point<float>(point.x, point.y, 0));
        measureLine.m_drawEnds.m_start = measureLine.m_drawEnds.m_end = point2D; // GetViewToSlice(point2D, rect);
        measureLine.StoreDistance();
        m_sliceViewer.m_measureLineList.push_front(measureLine);
        m_sliceViewer.m_measureLineList.m_currentIter = m_sliceViewer.m_measureLineList.begin();
      }
      bUpdated = true;
    } // case:MEASUREFUNC_START
    break;

    case MEASUREFUNC_END:
    {
      m_bMeasureActive = false;
      bUpdated = true;
    } // case:MEASUREFUNC_END
    break;

    case MEASUREFUNC_MOVE:
    {
      if (m_sliceViewer.m_measureLineList.m_currentIter == m_sliceViewer.m_measureLineList.end())
      {
        return bUpdated;
      }

      // Get the "client" rectangle
      Rect<int4> viewClientRect(GetViewRect());
      viewClientRect -= viewClientRect.LeftTop();

      // Expand client rect by 1 pixel; then, if endpoint is outside the view,
      // the drawing code will detect this (and not display a measurement)
      viewClientRect.Inflate(1, 1);

      // Restrict measure line to the client rectangle
      Point2D<int4> mousePt(point.x, point.y);
      viewClientRect.ConstrainPt(mousePt.m_x,mousePt.m_y);

      m_sliceViewer.m_measureLineList.m_currentIter->m_drawEnds.m_end = mousePt; // GetViewToSlice(mousePt, rect);
      m_sliceViewer.m_measureLineList.m_currentIter->m_measureEnds.m_end = Calculate3DPoint(Point<float4>(point.x, point.y, 0.0F));
      m_sliceViewer.m_measureLineList.m_currentIter->StoreDistance();
      m_sliceViewer.m_measureLineList.m_currentIter->m_ePosn = m_sliceViewer.m_measureLineList.GetUniquePos(m_sliceViewer.m_measureLineList.m_currentIter);

      bUpdated = true;
    } // case:MEASUREFUNC_MOVE
    break;

    case MEASUREFUNC_UPDATE:
    {
      if (m_sliceViewer.m_measureLineList.m_currentIter == m_sliceViewer.m_measureLineList.end())
      {
        return bUpdated;
      }

      // get the 2D start&end points;
      Point2D<int4> start = m_sliceViewer.m_measureLineList.m_currentIter->m_drawEnds.m_start;
      Point2D<int4> end = m_sliceViewer.m_measureLineList.m_currentIter->m_drawEnds.m_end;

      // re-calculate the corresponding 3D points.
      m_sliceViewer.m_measureLineList.m_currentIter->m_measureEnds.m_start = Calculate3DPoint(Point<float4>(start.m_x, start.m_y, 0.0F));
      m_sliceViewer.m_measureLineList.m_currentIter->m_measureEnds.m_end = Calculate3DPoint(Point<float4>(end.m_x, end.m_y, 0.0F));
      m_sliceViewer.m_measureLineList.m_currentIter->StoreDistance();
      m_sliceViewer.m_measureLineList.m_currentIter->m_ePosn = m_sliceViewer.m_measureLineList.GetUniquePos(m_sliceViewer.m_measureLineList.m_currentIter);

      bUpdated = true;
    }
    break;
    default:
    {
      break;
    }
  } // endswitch(eMeasureFunc)

  if ( bUpdated )
  {
    ForceRedraw();
  }

  return bUpdated;
} // LineMeasurement(eMeasureFunc, point)


/**
 * Deletes the measurement line proximal to the specified point (if any)
 *
 * @param bRoi    True to delete ROI, false for line measurement
 * @param point   Point selected
 */
void SliceChildView::DeleteMeasurement(const bool bRoi, const CPoint & point)
{
  // either delete a roi or a linear measurement
  if (bRoi)
  {
    const Point<float4> vPoint(point.x, point.y, 0);
    bool bResult = m_sliceViewer.m_measureAreaList.SelectHandle(vPoint);
    if (bResult == true)
    {
      CString sDeletePrompt;
      sDeletePrompt.Format(IDS_VERIFY_USER_OPERATION_FORMAT,
                 LoadResourceString(IDS_DELETE), LoadResourceString(IDS_MEASURE));
      if (AfxMessageBox(sDeletePrompt, MB_YESNO | MB_ICONQUESTION) == IDYES)
      {
        m_sliceViewer.m_measureAreaList.erase(m_sliceViewer.m_measureAreaList.m_currentIter);
        m_sliceViewer.m_measureAreaList.m_currentIter = m_sliceViewer.m_measureAreaList.end();
      }
    }
  }
  else
  {
    CRect rc;
    GetClientRect( rc );

    // TODO: Correct for non-squared images.  Is it enough to change the GetClientRect() based on aspect ratio (and zoom)?
    MeasureLineList::iterator iterLineClose = GetNearestProximalMeasurement(m_sliceViewer.m_measureLineList,
      GetViewToSlice(Point2D<int4>(point.x, point.y), rc));
    if (iterLineClose != m_sliceViewer.m_measureLineList.end())
    {
      CString sDeletePrompt;
      sDeletePrompt.Format(IDS_VERIFY_USER_OPERATION_FORMAT,
                 LoadResourceString(IDS_DELETE), LoadResourceString(IDS_MEASURE));
      if (AfxMessageBox(sDeletePrompt, MB_YESNO | MB_ICONQUESTION) == IDYES)
      {
        m_sliceViewer.m_measureLineList.erase(iterLineClose);
        m_sliceViewer.m_measureLineList.m_currentIter = m_sliceViewer.m_measureLineList.end();
      }
    } // END ... if (m_sliceViewer)

  } // END ... if (bRoi)

} // DeleteMeasurement( bRoi, point )


/**
 * Returns the iterator for the nearest line to the given point that is proximal to the point.
 * NOTE:  The return value for this function necessitated exact duplication in a const and non-const override;
 *         ANY code changes should be applied to both variants.
 *
 * @param lMeasureLine   Measurement line list.
 * @param slicePoint     Test point in slice coordinates (i.e. as given by GetViewToSlice method).
 * @return               Nearest line proxmial to the point.
 */
MeasureLineList::iterator SliceChildView::GetNearestProximalMeasurement(MeasureLineList& lMeasureLine,
                                                                        const Point2D<int4>& slicePoint) const
{
  float4 fLeastDist(0.5f);

  MeasureLineList::iterator iterLeastDist = lMeasureLine.end();
  MeasureLineList::iterator listCurr = lMeasureLine.begin();
  MeasureLineList::iterator listEnd = lMeasureLine.end();

  for (; listCurr!=listEnd; ++listCurr)
  {
    MeasureLine::LineEnds< Point2D<int4> > ends;
    ends.m_start = listCurr->m_drawEnds.m_start;
    ends.m_end   = listCurr->m_drawEnds.m_end;

    // TODO: If necessary, replace this with the proper equation for point-to-line distance
    // Find the closest line proximal to the point
    float4 fPtDist = Distance(ends.m_start, slicePoint) + Distance(ends.m_end, slicePoint);
    float4 fLineDist = Distance(ends.m_start, ends.m_end);

    if ( (fPtDist = fabs(fPtDist - fLineDist)) < fLeastDist)
    {
      iterLeastDist = listCurr;
      fLeastDist = fPtDist;
    }
  } // endfor (loop thru lMeasureLine list)

  return iterLeastDist;
} // GetNearestProximalMeasurement( lMeasureLine, slicePoint )


/**
 * Returns the iterator for the nearest line to the given point that is proximal to the point.
 * NOTE:  The return value for this function necessitated exact duplication in a const and non-const override;
 *         ANY code changes should be applied to both variants.
 *
 * @param lMeasureLine   Measurement line list.
 * @param slicePoint     Test point in slice coordinates (i.e. as given by GetViewToSlice method).
 * @return               Nearest line proxmial to the point.
 */
MeasureLineList::const_iterator SliceChildView::GetNearestProximalMeasurement(const MeasureLineList& lMeasureLine,
                                                                              const Point2D<int4>& slicePoint) const
{
  float4 fLeastDist(0.5f);

  MeasureLineList::const_iterator iterLeastDist = lMeasureLine.end();
  MeasureLineList::const_iterator listCurr = lMeasureLine.begin();
  MeasureLineList::const_iterator listEnd = lMeasureLine.end();

  for (; listCurr!=listEnd; ++listCurr)
  {
    MeasureLine::LineEnds< Point2D<int4> > ends;
    ends.m_start = listCurr->m_drawEnds.m_start;
    ends.m_end   = listCurr->m_drawEnds.m_end;

    // TODO: If necessary, replace this with the proper equation for point-to-line distance
    // Find the closest line proximal to the point
    float4 fPtDist = Distance(ends.m_start, slicePoint) + Distance(ends.m_end, slicePoint);
    float4 fLineDist = Distance(ends.m_start, ends.m_end);

    if ( (fPtDist = fabs(fPtDist - fLineDist)) < fLeastDist)
    {
      iterLeastDist = listCurr;
      fLeastDist = fPtDist;
    }
  } // endfor (loop thru lMeasureLine list)

  return iterLeastDist;
} // GetNearestProximalMeasurement( lMeasureLine, slicePoint )


/**
 * Calculates ROI statistics along the given region
 *
 * @param pMeasureArea   Region for which to calculate statistics
 */
void SliceChildView::CalculateRoiStatistics(MeasureAreaList::iterator & pMeasureArea)
{
  try
  {
    // early return if there is no current area
    if (pMeasureArea == m_sliceViewer.m_measureAreaList.end())
    {
      return;
    }

    Timer timer;
    Dataset& myDatasetRef = *m_sliceViewer.GetDataset();

    // recalculate the 3D position (properly depending on the volume)
    const Triple<int4> dim( m_sliceViewer.m_bSliceRendererEnabled ? myDatasetRef.m_fields.m_sliceDimensions : myDatasetRef.m_vDim );
    const Triple<float4> units( m_sliceViewer.m_bSliceRendererEnabled ? myDatasetRef.m_fields.m_units2D : myDatasetRef.m_vUnits );
    Point<float4> centerMeasurePt = Calculate3DPoint(pMeasureArea->m_centerDrawPt).GetDividedAnIso(units);

    // recalculate axis vectors
    Vector<float4> majorAxisVector = Vector<float4>(Calculate3DPoint(pMeasureArea->m_centerDrawPt + pMeasureArea->m_majorAxisDrawVector).GetDividedAnIso(units));
    Vector<float4> majorAxisMeasureVector = majorAxisVector - Vector<float4>(centerMeasurePt);
    Point<float4> minorAxisPoint = Calculate3DPoint(pMeasureArea->m_centerDrawPt + pMeasureArea->m_minorAxisDrawVector).GetDividedAnIso(units);
    Vector<float4> minorAxisMeasureVector = minorAxisPoint - centerMeasurePt;
    Vector<float4> majorAxisMeasureWorldVector = majorAxisMeasureVector.GetScaledAnIso(units);
    Vector<float4> minorAxisMeasureWorldVector = minorAxisMeasureVector.GetScaledAnIso(units);

    if ( m_sliceViewer.m_bSliceRendererEnabled )
    {
      // calculate the ellipse bounding box
      Point3Df lowerLeftWorld = Calculate3DPoint( pMeasureArea->m_centerDrawPt - pMeasureArea->m_majorAxisDrawVector - pMeasureArea->m_minorAxisDrawVector );
      Point3Df upperLeftWorld = Calculate3DPoint( pMeasureArea->m_centerDrawPt - pMeasureArea->m_majorAxisDrawVector + pMeasureArea->m_minorAxisDrawVector );
      Point3Df lowerRightWorld = Calculate3DPoint( pMeasureArea->m_centerDrawPt + pMeasureArea->m_majorAxisDrawVector - pMeasureArea->m_minorAxisDrawVector );
      Vector3Df verticalEdgeWorld = lowerLeftWorld - upperLeftWorld;
      Vector3Df horizontalEdgeWorld = lowerRightWorld - lowerLeftWorld;

      // calculate the statistics
      vxIntensityStatistic statistics;
      m_sliceViewer.m_sliceRenderer.GetIntensityStatistics( statistics, lowerLeftWorld, verticalEdgeWorld, horizontalEdgeWorld );

      // save statistics for subsequent display
      pMeasureArea->m_fAverageHU = m_sliceViewer.m_sliceRenderer.ConvertRawToModality( statistics.GetAverage() );
      pMeasureArea->m_fMaximumHU = m_sliceViewer.m_sliceRenderer.ConvertRawToModality( statistics.GetMaximum() );
      pMeasureArea->m_fStandardDeviationHU = m_sliceViewer.m_sliceRenderer.ConvertRawToModality( statistics.GetStandardDeviation() ) - m_sliceViewer.m_sliceRenderer.ConvertRawToModality( 0 );
    }
    else
    {
      // sample the bounding box
      const float4 fSamplingRate = 0.5F; // linear samples per voxel
      const int4 iSamplesMajor = int4(majorAxisMeasureVector.GetLength() * 2.0F / fSamplingRate) + 10;
      const int4 iSamplesMinor = int4(majorAxisMeasureVector.GetLength() * 2.0F / fSamplingRate) + 10;
      const float4 fSampleArea = majorAxisMeasureWorldVector.GetLength() * 2.0F / iSamplesMajor * minorAxisMeasureWorldVector.GetLength() * 2.0F / iSamplesMinor;
      uint4 uTotalSamples = 0;
      float8 fValueSumHU = 0;
      pMeasureArea->m_fMaximumHU = -10000;
      // Loop over the entire trapezoidal bounding box
      for (uint4 uMajor=0; uMajor< iSamplesMajor; uMajor++)
      {
        for (uint4 uMinor=0; uMinor < iSamplesMinor; uMinor++)
        {
          // Decide if the sample point is inside the ellipse
          const float4 fPercentMajor = Interpolate(0.0F, float4(uMajor), float4(iSamplesMajor-1), -1.0F, 1.0F);
          const float4 fPercentMinor = Interpolate(0.0F, float4(uMinor), float4(iSamplesMinor-1), -1.0F, 1.0F);
          if (Vector<float4>(fPercentMajor, fPercentMinor, 0.0F).GetLength() < 1.0F)
          {
            // inside ellipse, sample and add to statistics
            const Point<float4> positionPt(centerMeasurePt + majorAxisMeasureVector * fPercentMajor + minorAxisMeasureVector * fPercentMinor);
            float8 fSampledValueHU;
            uint4 uHistogramIndex;
            float8 fSampledValueRaw;
            if ( SliceViewer::m_bShowUncleansed )
            {
              fSampledValueRaw = myDatasetRef.m_fields.m_volumeRender.GetUncleansedResampledValue(positionPt);
            }
            else
            {
              fSampledValueRaw = float8(myDatasetRef.m_fields.m_volumeRender.GetResampledValue(positionPt));
            }
            uHistogramIndex = uint4( fSampledValueRaw );
            fSampledValueHU = HUconverter::Uchar2HU( fSampledValueRaw );

            // update the average
            fValueSumHU += fSampledValueHU;
            uTotalSamples++;

            // keep track of the maximum
            if ( fSampledValueHU > pMeasureArea->m_fMaximumHU )
            {
              pMeasureArea->m_fMaximumHU = fSampledValueHU;
            }
          } // if inside ellipse
        } // for minor axis samples
      } // for major axis samples

      float8 fAverageValueHU = fValueSumHU / uTotalSamples;
      float8 fSumOfSquaredDifferences = 0;

      // Repeat entire sampling procedure again but this time compute standard deviation now that we have the average value.
      // Loop over the entire trapezoidal bounding box
      for (uint4 uMajor=0; uMajor < iSamplesMajor; uMajor++)
      {
        for (uint4 uMinor=0; uMinor < iSamplesMinor; uMinor++)
        {
          // Decide if the sample point is inside the ellipse
          const float4 fPercentMajor = Interpolate(0.0F, float4(uMajor), float4(iSamplesMajor-1), -1.0F, 1.0F);
          const float4 fPercentMinor = Interpolate(0.0F, float4(uMinor), float4(iSamplesMinor-1), -1.0F, 1.0F);
          if (Vector<float4>(fPercentMajor, fPercentMinor, 0.0F).GetLength() < 1.0F)
          {
            // inside ellipse, sample and add to statistics
            const Point<float4> positionPt(centerMeasurePt + majorAxisMeasureVector * fPercentMajor + minorAxisMeasureVector * fPercentMinor);
            float8 fSampledValueHU;
            float8 fSampledValueRaw;
            if ( SliceViewer::m_bShowUncleansed )
            {
              fSampledValueRaw = myDatasetRef.m_fields.m_volumeRender.GetUncleansedResampledValue(positionPt);
            }
            else
            {
              fSampledValueRaw = float8(myDatasetRef.m_fields.m_volumeRender.GetResampledValue(positionPt));
            }

            fSampledValueHU = HUconverter::Uchar2HU( fSampledValueRaw );

            // compute the sum of squared differences
            float8 fDifference = fSampledValueHU - fAverageValueHU;
            fSumOfSquaredDifferences += fDifference * fDifference;

          } // if inside ellipse
        } // for minor axis samples
      } // for major axis samples

      // save statistics for subsequent display
      pMeasureArea->m_fAverageHU = fAverageValueHU;
      pMeasureArea->m_fMaximumHU = static_cast< int4 >( pMeasureArea->m_fMaximumHU );
      pMeasureArea->m_fStandardDeviationHU = sqrt(fSumOfSquaredDifferences / uTotalSamples);
    }
    // same for both
    pMeasureArea->m_fMajorDiameterMM = 2.0F * Max(majorAxisMeasureWorldVector.GetLength(), minorAxisMeasureWorldVector.GetLength());
    pMeasureArea->m_fAreaSquareMM = M_PI * majorAxisMeasureWorldVector.GetLength() * minorAxisMeasureWorldVector.GetLength();
  } // try
  catch (...)
  {
    ex::RuntimeException(LogRec("<SliceChildView::CalculateRoiStatistics> Unhandled exception.", "SliceChildView", "CalculateRoiStatistics"));
  }

} // CalculateRoiStatistics( pMeasureArea )


/**
 * Creates new/Adjusts current ROI measurement
 *
 * @param eMeasureFunc   Specifies action on the current measure line
 * @param point          Current mouse point
 * @return               Success of measure adjustment
 */
bool SliceChildView::RoiMeasurement(const MEASUREFUNCENUM eMeasureFunc, const CPoint point)
{

  // translation after zooming.
  CPoint scaledPoint;
  scaledPoint.x = ((float4)point.x/(float4)m_sliceViewer.m_uDisplayWidth - m_sliceViewer.m_rZoom.m_left) * 
                  ((float4)m_sliceViewer.m_uDisplayWidth/(m_sliceViewer.m_rZoom.m_right - m_sliceViewer.m_rZoom.m_left));

  scaledPoint.y = ((float4)point.y/(float4)m_sliceViewer.m_uDisplayHeight - m_sliceViewer.m_rZoom.m_top) * 
                  ((float4)m_sliceViewer.m_uDisplayHeight/(m_sliceViewer.m_rZoom.m_bottom - m_sliceViewer.m_rZoom.m_top));

  ////////////////////////////////////////////////////////
  // swicth on the measure function enumerator parameter
  ///////////////////////////////////////////////////////
  switch (eMeasureFunc)
  {
    case MEASUREFUNC_START:
    {
      Point<float4> handlePoint(point.x, point.y, 0);
      bool bResult = m_sliceViewer.m_measureAreaList.SelectHandle(handlePoint);
      if (bResult == true)
      {
        // we have selected the current handle and stored it in
        // (see m_measureAreaList.m_current and m_measureAreaList.currentHandle)
      }
      else
      {
        // start a new roi for now
        MeasureArea area;
        area.m_centerDrawPt.m_x = point.x;
        area.m_centerDrawPt.m_y = point.y;
        area.m_majorAxisDrawVector = Vector<float4>(30,0,0);
        area.m_minorAxisDrawVector = Vector<float4>(0,30,0);
        m_sliceViewer.m_measureAreaList.push_front(area);
        m_sliceViewer.m_measureAreaList.m_currentIter = m_sliceViewer.m_measureAreaList.begin();
        m_sliceViewer.m_measureAreaList.m_currentIter->SelectHandle(Point<float4>(point.x, point.y, 0.0F));
        m_bUpdateRoiMeasurement = true;
      }
      return true;
    } // case:MEASUREFUNC_START

    case MEASUREFUNC_END:
    {
      m_bMeasureActive = false;
      return true;
    }

    case MEASUREFUNC_MOVE:
    {
      if (m_sliceViewer.m_measureAreaList.m_currentIter == m_sliceViewer.m_measureAreaList.end() ||
          m_sliceViewer.m_measureAreaList.m_currentIter->m_iCurrentHandle < 0)
      {
        return false;
      }
      // Get the "client" rectangle
      Rect<int4> viewClientRect(GetViewRect());
      viewClientRect -= viewClientRect.LeftTop();
      // Expand left by 1 pixel; then, if outside of this rectangle, will
      // (below) set point to -1, so the drawing code detect outside to the left
      viewClientRect.m_left -= 1;
      // Restrict measure line to the client rectangle
      Point2D<int4> mousePt(point.x, point.y);
      viewClientRect.ConstrainPt(mousePt.m_x,mousePt.m_y);
      if (mousePt.m_x == viewClientRect.m_left)
      {
        mousePt.m_x = -1;
      }

      // manipulate a handle
      m_sliceViewer.m_measureAreaList.m_currentIter->MoveHandle(Point<float4>(mousePt.m_x, mousePt.m_y, 0.0F));
      m_bUpdateRoiMeasurement = true;
      break;
    } //case:MEASUREFUNC_MOVE

    case MEASUREFUNC_UPDATE:
    {
      m_bUpdateRoiMeasurement = true;
      break;
    }

    default:
    {
      break;
    }
  } // END ... switch(eMeasureFunc).

  return true;

} // RoiMeasurement( eMeasureFunc, point )


/**
 * Prompts for text and adds a new annotation at the specified 2D point
 *
 * @param point2D   2D view point at which to add annotation in current slice view
 * @return          Success/failure
 */
bool SliceChildView::AddNewSuspiciousROIinfo(const Point2D<int4>& point2D, const float4 & fVolume, const float4 & fMaximumDiameter)
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
  } // end switch(m_sliceViewer.Orientation).

  antn.m_sName = "Volume: " + ToAscii(RoundDecimals( fVolume, 0 )) + "mm^3\n" + "Max: "
    + ToAscii(RoundDecimals( fMaximumDiameter, 0 )) + " +/- 0.8mm";

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
} // AddNewSuspiciousROIinfo( point2D, fVolume, fMaximumDiameter )


// undefines
#undef FILE_REVISION


// $Log: SliceChildView.Render.C,v $
// Revision 1.1.2.27  2010/12/07 14:20:11  dongqing
// Add mouse cursor coordinate check to make sure it is within the current
// window. This fix problem with freeze when cursor in the mortal dialog
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.26  2010/10/21 14:49:44  dongqing
// Fix the extra-colonic finding display toggle fails to toggle the ROI color
// display in 2D.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.25  2010/10/20 20:47:33  dongqing
// The SRT tool availability and display switch will be only determined
// by the VCSetting.xml switch now.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.24  2010/03/09 20:20:50  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.23  2010/02/02 23:02:44  dongqing
// add Components number check to allow the Slice View display while
// the component is not loaded. That fixes the Verification /Orthogonal
// layout crash while load the session of SRT
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.22  2009/12/18 22:01:13  dongqing
// buttons for segmented region voxel size modes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.21  2009/10/07 16:13:16  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.1.2.20  2009/07/30 17:58:21  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.19  2009/01/06 03:32:53  jmeade
// volume measure interface changes.
//
// Revision 1.1.2.18  2008/12/31 21:44:34  jmeade
// moved voxel size selection control into the sliceview
//
// Revision 1.1.2.17  2008/12/23 17:00:13  dongqing
// add bigger brush size option for the SRT ROI region editing tool
//
// Revision 1.1.2.16  2008/10/20 23:12:55  jmeade
// moved up checking on an empty dataset.
//
// Revision 1.1.2.15  2008/07/03 14:56:44  dongqing
// add global CAD display switch check for 2D display CAD ROI
//
// Revision 1.1.2.14  2008/06/24 20:05:06  dongqing
// Fix the bug for toggling the SRT ROI display causing CAD 2D ROI off
//
// Revision 1.1.2.13  2008/03/25 02:26:19  jmeade
// Issue 6012: Copy code for change from Branch: CON_3-0_RC407_Patch080212
//
// Revision 1.1.2.12  2008/02/29 20:57:32  dongqing
// add code for CAD partner to see the user click points dicom RCS
// world coordinate
//
// Revision 1.1.2.11  2008/02/29 15:52:54  dongqing
// Create a new function for converting Vol Coordinate to Dicom RCS world
// coordinate
//
// Revision 1.1.2.10  2008/02/28 23:10:25  dongqing
// fix the computation bug in DICOM RCS display
//
// Revision 1.1.2.9  2008/02/28 19:19:09  dongqing
// the DICOM RCS convertion using Image Orientation Patient Cosine
// only now. The HFS stuff is no longer used for that purpose.
//
// Revision 1.1.2.8  2008/02/05 22:06:09  jmeade
// Issue 5962: correct all problems hashing current window/level to displayed preset name.
//
// Revision 1.1.2.7  2008/02/04 22:19:30  jmeade
// Issue 5999: ctc text for custom.
//
// Revision 1.1.2.6  2008/01/30 22:31:19  dongqing
// change the name and W/L value in the 2D view W/L list
//
// Revision 1.1.2.5  2008/01/17 19:35:31  dongqing
// complete the manual tool for suspicious ROI editing
//
// Revision 1.1.2.4  2008/01/10 18:34:14  jmeade
// moving methods btw files.
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SliceChildView.Render.C,v 1.1.2.27 2010/12/07 14:20:11 dongqing Exp $
// $Id: SliceChildView.Render.C,v 1.1.2.27 2010/12/07 14:20:11 dongqing Exp $
