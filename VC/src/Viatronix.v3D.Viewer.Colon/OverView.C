// $Id: OverView.C,v 1.44.2.16 2010/11/16 16:09:05 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Kevin Kreeger, Jeff Meade, and Bin Li (binli@viatronix.net).


// includes
#include "StdAfx.h"
#include "OverView.h"
#include "OverViewDlgBar.h"
#include "vxIntensityConverter.h"
//#include "OverviewRender.h"
//#include "ReaderGlobal.h"
#include "Viatron.h"
//#include "V1KMessages.h"
//#include "Dataset.h"
//#include "MainFrm.h"
//#include "WinUtils.h"
//#include "glFont.h"

#include "RenderUtil.h"
#include "resource.h"
//#include "SystemInfo.h"
#include "ButtonMenus.h"
#include "ReportVariant.h"
#include "NavigationView.h"
#include "ApplicationSettings.h"
#include "vcUtils.h"
#include "CadUtils.h"

//defines
#define FILE_REVISION "$Revision: 1.44.2.16 $"


using namespace std;
using namespace ColonViews::Layouts;
using namespace ColonViews::Dialogs;
using namespace ReaderLib;
using namespace WinUtils;
using namespace ColonReport;
using namespace vxCAD;

/// size of bookmark circle
enum
{
  eBOOKMARK_SIZE = 2500
};

/// list of colors used for segment picking and other selection functions
Triple<uint1> BlankColor = Triple<uint1>(1,1,1);
#ifdef FINAL_RELEASE
const
#endif
  static Triple<uint1> PickSegtColors[256] =
{
  Triple<uint1>(  19,  91,  19 ),
  Triple<uint1>(  39, 119, 216 ),
  Triple<uint1>( 155,  50,   0 ),
  Triple<uint1>( 128, 128,   0 ),

  Triple<uint1>(  77,  77,  77 ),
  Triple<uint1>( 166,  66,  51 ),
  Triple<uint1>(   1,  92,  92 ),
  Triple<uint1>(  24,  41, 199 ),

  Triple<uint1>( 140,  70,   0 ),
  Triple<uint1>( 201,  85,  35 ),
  Triple<uint1>(   2, 125,  54 ),
  Triple<uint1>(   0,   0, 128 ),

  Triple<uint1>( 128, 128,   5 ),
  Triple<uint1>( 128,   0, 128 ),
  Triple<uint1>(   0, 128, 128 ),
  Triple<uint1>( 255, 255, 255 ),

  Triple<uint1>(  64, 192,   0 ),
  Triple<uint1>( 192,   0,   0 ),
  Triple<uint1>( 192, 192,   0 ),
  Triple<uint1>(   0,   0, 255 ),

  Triple<uint1>( 192,  64,   0 ),
  Triple<uint1>(   0, 192,   0 ),
  Triple<uint1>( 192,   0, 192 ),
  Triple<uint1>(   0, 255,   0 ),

  // 24 up to (not including) here

  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,

  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,

  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,

  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,

  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,

  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,

  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
  BlankColor,BlankColor,BlankColor,BlankColor,  BlankColor,BlankColor,BlankColor,BlankColor,
};


///////////////////////////////////////////////////////////////////////////

std::string OverView::m_sDistFmtPrimary;
std::string OverView::m_sDistFmtSecndry;

/**
 * Constructor
 *
 * @param hWndParent     Parent window
 * @param eOrientn       Overview dataset orientation
 * @param bBufferImage   Whether to save an image for quick refresh
 */
OverView::OverView(HWND hWndParent, const DatasetOrientations::TypeEnum eOrientn, const bool bBufferImage)
  : ViatronChildView(eGVT_OVERVIEW, hWndParent, eOrientn), m_bBufferImage(bBufferImage), m_iCurSelBookmark(-1),
  m_iCurSelRegistrationPoint(-1), m_bcR(0.1F), m_bcG(0.1F), m_bcB(0.1F), m_bcA(0.0F),
  m_bRenderReportArrow(false), m_bUpdateProjMatrix(true), m_bUpdateModelViewMatrix(true), m_bRefDisplayList(true),
  m_eSaveReportImage(eSaveNone), m_pNonDistendedAreaImage(NULL), m_pDatasetLastRendered((Dataset*) NULL),
  m_pNeighbourView(NULL), m_bUserFeaturesChanged(false), m_bRenderSimplifiedSegments(false),
  m_bDisplayDistanceFromRectum(true), m_eMouseOverFeature(OverviewFeature::eNone), m_bPermitSlices( true ), m_bShowArrow(true),m_bGovernFrameRateWhileFlying(true),
  m_puPickBuffer(NULL)
{
  // Load IDS_DISTANCE_FROM_RECTUM_FORMAT string once for faster processing
  if (rdrGlobal.m_supine.IsLoadingComplete() && m_sDistFmtPrimary.empty())
  {
    const std::string sDistFmt = LPCSTR(LoadResourceString(IDS_DISTANCE_FROM_RECTUM_FORMAT));
    m_sDistFmtPrimary = sDistFmt + "-" + ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_supine, true);
  }
  if (rdrGlobal.m_prone.IsLoadingComplete() && m_sDistFmtSecndry.empty())
  {
    const std::string sDistFmt = LPCSTR(LoadResourceString(IDS_DISTANCE_FROM_RECTUM_FORMAT));
    m_sDistFmtSecndry = sDistFmt + "-" + ColonViatronixWnd::PatientOrientationToString(rdrGlobal.m_prone, true);
  }

  SetRenderMode(OverviewRender::eNormal);
} // OverView()


/**
 * Destructor.
 */
OverView::~OverView()
{
  m_pDatasetLastRendered = (Dataset*) NULL;
  if (m_pNonDistendedAreaImage)
  {
    delete []m_pNonDistendedAreaImage;
    m_pNonDistendedAreaImage = NULL;
  }

  if ( m_puPickBuffer != NULL )
  {
    delete m_puPickBuffer;
    m_puPickBuffer = NULL;
  }

} // ~OverView()


/**
 * Initialize common rendering characteristics
 *
 * @param pNeighbour   Neighbour view (okay to be NULL, which indicates there is no neighbour).
 * @param pTrackbal    Current overview view position.
 * @param eMode        Current render mode.
 */
void OverView::Initialize(OverView * pNeighbour, TrackBall * pTrackball, const OverviewRender::RenderModeEnum eMode)
{
  SetNeighbourView(pNeighbour);

  if( pTrackball ) 
    GetDatasetRef().m_pTrackball = pTrackball; 

  SetRenderMode(eMode);
} // Initialize


/**
 * set up Resource Context
 */
void OverView::GLSetupRC()
{
  // Initialize drawing context here
  glClearColor(m_bcR, m_bcG, m_bcB, m_bcA);
  glDisable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
} // GLSetupRC()


/**
 * idle function
 *
 * @return    whether or not image needs updating during idle time
 */
bool OverView::GLIdle()
{
   return true;
} // GLIdle()


/**
 * Updates the "distance fomr anal verge" label
 */
void OverView::UpdateDistanceFromAnalVerge()
{
  //if( !::IsWindow( m_hWnd ) )
    return;

  HWND hDlgBar( GetParent()->GetDlgItem( IDC_TEMPLATE_OVERVIEWTOOLS )->GetSafeHwnd() );
  if( GetDatasetRef().m_submarine.IsFlying() || (ViatronStudy::m_modeOverView & ViewModes::SLICE ) )
    ::SendMessage(hDlgBar, OverViewDlgBar::WM_UPDATE_LOCATION, 0, 0);
} // UpdateDistanceFromAnalVerge()


/**
 * Renders the view (includes render and raster)
 */
void OverView::GLRender()
{
  const float4 fMaxOverviewFrameRate( 4.0F );
  if( rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() && m_bGovernFrameRateWhileFlying )
  {
    if(  m_governorTimer.ElapsedTime() > 1.0F / fMaxOverviewFrameRate )
    {
      ViatronChildView::GLRender();
      m_governorTimer.Reset();
      UpdateDistanceFromAnalVerge();
    }
  }
  else
  {
    ViatronChildView::GLRender();
    UpdateDistanceFromAnalVerge();
  }
} // GLRender()


/**
 * render the scene
 * @param void*   Unused
 */
void OverView::GLRenderScene(void *)
{
  Dataset& myDataset = GetDatasetRef();
  if (!myDataset.m_segmentList.IsLoaded())
  {
    GLChildView::GLRenderScene();
    return;
  }

  // render a fresh image or use cached image
#ifdef FINAL_RELEASE
  try
#endif
  {
    // If a different dataset was rendered the last time around, some entities need to be reinitialized
    if (m_pDatasetLastRendered != GetDatasetPtr())
    {
      m_overviewRender.DeleteAllDisplayLists();
      m_overviewRender.BuildAllDisplayLists(myDataset);
      SetModelViewMatrixUpdateFlag(true);
      m_pDatasetLastRendered = GetDatasetPtr();
      ResetTimer();
    }
    else if (!m_overviewRender.AreDisplayListsGenerated())
    {
      // Rebuild display lists as need be
      m_overviewRender.BuildAllDisplayLists(myDataset);
    }

    // grasp a report image: initialize report::NonIndistendedAreas Images
    if ( m_eSaveReportImage != eSaveNone )
    {
      CRect rc( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
      Rect<int4> rect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );

      try
      {
        RenderObjects(ViatronStudy::m_modeOverView);
        if (m_eSaveReportImage == eSaveVerification)
        {
          if (m_bShowArrow)
            m_overviewRender.RenderCamera(myDataset, rect, m_mProjMatrix, m_mModelViewMatrix,
            /* m_bRenderReportArrow= */ true, /* scalor to control arrow size */ 2);
        }
        else if (m_eSaveReportImage == eSaveStandard)
        {
          if (m_bShowArrow)
            m_overviewRender.RenderCamera(myDataset, rect, m_mProjMatrix, m_mModelViewMatrix,
            /* m_bRenderReportArrow= */ false, /* scalor to control arrow size */ 1);
        }
      }
      catch (...)
      {
      } // just catch it. The exception has been logged already.

      // ... grab NonIndistendedArea images
      const int4 iReadWidth = (rc.Width()+3) & ~3;
      const int4 iReadHeight =  rc.Height();
      if ( (iReadWidth > 0) && (iReadHeight > 0) )
      {
        LPBYTE pPixels = new BYTE[iReadWidth*iReadHeight*3];
        glPushAttrib(GL_PIXEL_MODE_BIT);
        {
          glReadBuffer(GL_BACK);
          glPixelStorei(GL_PACK_ALIGNMENT, 4);
          glPixelStorei(GL_PACK_ROW_LENGTH, 0);
          glPixelStorei(GL_PACK_SKIP_ROWS, 0);
          glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
          glReadPixels(GetViewport().m_left, GetViewport().m_top, iReadWidth, iReadHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, (LPVOID)pPixels);
          LogFyi("OverView::GLRenderScene reading pixels.","OverView","GLRenderScene" );
        }
        glPopAttrib();
        m_pNonDistendedAreaImage = pPixels;
        m_eSaveReportImage = eSaveNone;
      }
    }

    // render objects: colon/skeleton/framebox/planes.
    if ( !IsTimeOut() || !GetImageBuffered() ||  GetInverseImage() )
    {
      try
      {
        RenderObjects(ViatronStudy::m_modeOverView);
      }
      catch (...)
      {
      } // just catch it. The exception has logged already.
      SetImageBuffered( false );
      TimeClick();
      if (IsTimeOut() && !GetInverseImage()) //  && m_bBufferImage)
      {
        if (IsMode(OverviewRender::eSegmentPicking))
          ResetTimer();
        else
          SaveBufferImage();
      }

      // called when inverse image.
      if (GetInverseImage())
      {
        GLSaveUpdateBuffer(/* inverse image = */ true);
        SetImageBuffered( true );
        glColor3f( 1, 1, 1 );
        GLRenderUpdateBuffer();
      }
    }
    else
    {
      // use cached image
      glColor3f( 1, 1, 1 );
      GLRenderUpdateBuffer();

      // check the cache the slice locations
      const bool bRenderingSlices( ViatronStudy::m_modeOverView & ViewModes::SLICE );
      const bool bFlying2D( (ViatronStudy::m_modeOrthogView & ViewModes::AUTOFLY2D) || (ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D) );
      const bool bFlying3D( ViatronStudy::m_modeEndoView & ViewModes::AUTOFLY );
      if ( bRenderingSlices && ! bFlying2D && ! bFlying3D && ! myDataset.m_submarine.IsFlying() )
      {
        if ( m_vDrawnSlicePosition != rdrGlobal.m_pCurrDataset->m_vOrthogonalPtWorld )
        {
          // Abracadabra!
          ResetTimer( 3 );
        }
      }

    } // endif currently interacting or not

    // if not picking, always render the camera above the image.
    if ( !IsMode(OverviewRender::eSegmentPicking) && !IsMode(OverviewRender::eSegmentVerify) && !IsMode(OverviewRender::eRegistrationVerify) )
    {
      // if picking, do not render the camera/
      // Will RenderCamera need depth test? If so, the depth buffer need to be cleared.
      Rect<int4> rect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
      glClear(GL_DEPTH_BUFFER_BIT);
      if (m_bShowArrow)
        m_overviewRender.RenderCamera(myDataset, rect, m_mProjMatrix, m_mModelViewMatrix, m_bRenderReportArrow);
    } // render Camera.
    glFlush();
  }
#ifdef FINAL_RELEASE
  catch (...)
  {
    // already loged, nothing to do now, just ignore errors.
  }
#endif
} // GLRenderScene()


/**
 * Maps a colon segment to a string name
 *
 * @param eSegment   colon segment identifier enum
 * @return           corresponding string name of segment
 */
std::string MapToString(const ColonSegmentEnum eSegment)
{
  uint4 uResID(0);
  switch (eSegment)
  {
    case eRECTUM:
      uResID = IDS_RECTUM;
      break;
    case eSIGMOID:
      uResID = IDS_SIGMOID;
      break;
    case eDESCENDING:
      uResID = IDS_DESCENDING;
      break;
    case eSPLENIC_FLEXURE:
      uResID = IDS_SPLENIC_FLEXURE;
      break;
    case eTRANSVERSE:
      uResID = IDS_TRANSVERSE;
      break;
    case eHEPATIC_FLEXURE:
      uResID = IDS_HEPATIC_FLEXURE;
      break;
    case eASCENDING:
      uResID = IDS_ASCENDING;
      break;
    case eCECUM:
      uResID = IDS_CECUM;
      break;
    default:
      break;
  }

  return LPCSTR(LoadResourceString(uResID));
} // MapToString()


/**
 * Raster (text output)
 */
void OverView::GLRasterScene()
{
  Dataset& myDataset = GetDatasetRef();
  if (!myDataset.m_segmentList.IsLoaded())
  {
    GLChildView::GLRasterScene();
    return;
  }

  if ( !IsTimeOut() || !GetImageBuffered() || GetInverseImage() )
    GLRenderLogo();

  int1 sTmpStr[101], sTmpStr1[101]; 
  GLFont & rTextdrawObject = GetNormalFont();
  glColor3f(0,1,0);

#ifdef FINAL_RELEASE  
 
 // Calculate the frames per second (time from last cycle)
 if (EndoluminalView::m_bDisplayRenderSpeed)
 {
    float8 fElapsed = m_frameTimer.ElapsedTime();
    m_frameTimer.Reset();
    if( fElapsed != 0 )
    {
      CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
      CRect clientRect = viewRect - viewRect.TopLeft();
      char sFrameRate[10] = {"0 Hz"};
      sprintf(sFrameRate, "%.1f Hz", float4(1.0 / fElapsed ) );
      glClear(GL_DEPTH_BUFFER_BIT /*| GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/);
      rTextdrawObject.DrawText(clientRect.right, 2, sFrameRate, GDT_ABOVELEFT); // At bottom-right
    }
 }

#endif

#ifndef FINAL_RELEASE
  if (IsMode(OverviewRender::eRegistrationVerify))
  {
    int r = rdrGlobal.m_supine.m_iFeatureToDisplayInfo;
    int c = rdrGlobal.m_prone.m_iFeatureToDisplayInfo;
    if(r<rdrGlobal.m_supine.m_ivFeatures.size() && c<rdrGlobal.m_prone.m_ivFeatures.size())
    {
      sprintf(sTmpStr, "inXout %0.2f", rdrGlobal.m_mCrossProducts(r,c));      rTextdrawObject.DrawText(0, 30, sTmpStr);
      sprintf(sTmpStr, "out    %0.2f", rdrGlobal.m_mOutVectors(r,c));         rTextdrawObject.DrawText(0, 45, sTmpStr);
      sprintf(sTmpStr, "in     %0.2f", rdrGlobal.m_mInVectors(r,c));          rTextdrawObject.DrawText(0, 60, sTmpStr);
      sprintf(sTmpStr, "dist   %0.2f", rdrGlobal.m_mEuclideanDistances(r,c)); rTextdrawObject.DrawText(0, 75, sTmpStr);
      sprintf(sTmpStr, "match  %0.2f", rdrGlobal.m_mMatchQuality(r,c));       rTextdrawObject.DrawText(0, 90, sTmpStr);
      sprintf(sTmpStr, "Hconf. %0.2f", rdrGlobal.m_mHorizConfidence(r,c));    rTextdrawObject.DrawText(0,105, sTmpStr);
      sprintf(sTmpStr, "Vconf. %0.2f", rdrGlobal.m_mVertConfidence(r,c));     rTextdrawObject.DrawText(0,120, sTmpStr);
      sprintf(sTmpStr, "conf.  %0.2f", rdrGlobal.m_mConfidence(r,c));         rTextdrawObject.DrawText(0,135, sTmpStr);
    }
  }
#endif

  // Draw text: distance from rectum.

  if (!IsMode(OverviewRender::eSegmentPicking) &&
      !IsMode(OverviewRender::eSegmentVerify) &&
      !IsMode(OverviewRender::eRegistrationVerify) &&
      myDataset.IsDistanceValid() && m_bDisplayDistanceFromRectum)
  {
    CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
    CRect clientRect = viewRect - viewRect.TopLeft();
    if (!myDataset.IsPointNearSelectedLumen(myDataset.m_submarine.GetPositionVolume()))
    {
      rTextdrawObject.DrawText(0, clientRect.bottom, LPCTSTR(LoadResourceString(IDS_DISTANCE_FROM_RECTUM_UNCERTAIN)), GDT_BELOW);
    }
    else
    {
      sprintf(sTmpStr, ((&myDataset==&rdrGlobal.m_supine)?m_sDistFmtPrimary:m_sDistFmtSecndry).c_str(),
        myDataset.GetCurrentDistanceFromRectumMM() / 10);
      rTextdrawObject.DrawText(0, clientRect.bottom, sTmpStr, GDT_BELOW); // At top-left
    }
  } // endif (IsMainViewWindow)

	/// Draw text : total volume of selected segments. This is for quick checking of quality of colon distention.
	/// It is not recommended to keep the record in the report.
	else if( IsMode(OverviewRender::eSegmentPicking) && !IsMode(OverviewRender::eSegmentVerify) &&
           !IsMode(OverviewRender::eRegistrationVerify) )
	{ 
    CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
    CRect clientRect = viewRect - viewRect.TopLeft();        	
		
    CRect viewRect1( GetViewport().m_left+20, GetViewport().m_top+20, GetViewport().m_right, GetViewport().m_bottom );
    CRect clientRect1 = viewRect1 - viewRect1.TopLeft();  

    bool bIsAirVolume = ApplicationSettings::UserSettings().Get<bool>( APP_SETTING_VOLUME_MEASURE_AIR_ONLY );   

		float4 fAirVolume = 0.0F;
		float4 fTotalVolume = 0.0F;

		double fAverageHU( 0.0 );
		uint4 iTaggedVoxelCount( 0 );

		if( !myDataset.IsLoadingComplete() )
		{
      sprintf( sTmpStr, "Volume data is still loading ...", fAirVolume );
			sprintf( sTmpStr1, "" );
		}
		else
		{
		  SegmentList::SelectedIterator itSeg( myDataset.m_segmentList );

			for( ; !itSeg.IsAtEnd(); itSeg++ )
			{
				fAirVolume   = fAirVolume   + (*itSeg).GetAirVolume();				
				fTotalVolume = fTotalVolume + (*itSeg).GetTotalVolume();

				fAverageHU        = fAverageHU        + (*itSeg).GetAverageTaggedHU();
				iTaggedVoxelCount = iTaggedVoxelCount + (*itSeg).GetTaggedVoxelCount();
			} // for

			int4 iAverageHU(0);
			if( iTaggedVoxelCount > 0 )
			{
				fAverageHU = fAverageHU / double( iTaggedVoxelCount );
				iAverageHU = vxIntensityConverter::RawToModality( static_cast<int4>(fAverageHU), myDataset.m_fields.m_sliceRendererData.GetUncleansedVolume()->GetHeader() );
			} // if

			float4 fTaggedVolumeRate(0.0f);
			if( fTotalVolume > 0.0f )
			{
				fTaggedVolumeRate = (1.0f - (fAirVolume/fTotalVolume))*100.0f;
			}

			if( bIsAirVolume )
			{
			  if( iAverageHU < 200 )
				{
          sprintf( sTmpStr, "Total air volume of selected segment(s): %0.2f cc.", fAirVolume );
          sprintf( sTmpStr1, "No tagged region found." );
				}
				else
				{
          sprintf( sTmpStr, "Total air volume of selected segment(s): %0.2f cc.", fAirVolume );
          sprintf( sTmpStr1, "Average value of tagged regions: %4d HU. Tagged region volume percentage: %0.1f %%", iAverageHU, fTaggedVolumeRate );
				}
			}
			else
			{
			  if( iAverageHU < 200 )
				{
          sprintf( sTmpStr, "Total volume of selected segment(s): %0.2f cc.", fTotalVolume );
          sprintf( sTmpStr1, "No tagged region found." );
				}
				else
				{					
          sprintf( sTmpStr, "Total volume of selected segment(s): %0.2f cc.", fTotalVolume );
          sprintf( sTmpStr1, "Average value of tagged regions: %4d HU. Tagged region volume percentage: %0.1f %%", iAverageHU, fTaggedVolumeRate );
				}
			}
		} // else

    rTextdrawObject.DrawText(0, clientRect.bottom, sTmpStr, GDT_BELOW); // At top-left

    rTextdrawObject.DrawText(0, clientRect1.bottom, sTmpStr1, GDT_BELOW); // At top-left
	} // else if


#if !defined(FINAL_RELEASE)
  // draw text: percentage.
  if (!IsMode(OverviewRender::eSegmentPicking) &&
      !IsMode(OverviewRender::eSegmentVerify) &&
      !IsMode(OverviewRender::eRegistrationVerify) &&
      myDataset.IsDistanceValid())
  {
    CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
    CRect clientRect = viewRect - viewRect.TopLeft();

    if (ViatronStudy::m_modeOverView & ViewModes::DRAW_DISTANCE)
    {
      if (myDataset.m_segmentList.GetTotalLengthMM())
      {
        const float4 fPercent( (myDataset.GetCurrentDistanceFromRectumMM() * 100.f) / (myDataset.m_segmentList.GetTotalLengthMM()) );
        sprintf(sTmpStr, "%s(%.1f)", MapToString(myDataset.GetCurrentColonSegment()).c_str(), fPercent);
        rTextdrawObject.DrawText(0, 2, sTmpStr);
      }
    }

  } // endif (percentage)

  sprintf(sTmpStr,
    (IsMode(OverviewRender::eNormal)) ? "eNormal" :
    (IsMode(OverviewRender::eSegmentVerify)) ? "eVerify" :
    (IsMode(OverviewRender::eSegmentPicking)) ? "ePicking" :
    (IsMode(OverviewRender::eRegistrationVerify)) ? "eRegistrationVerify" :
    (IsMode(OverviewRender::eRegisteredViews)) ? "eRegisteredViews" :
    "ERROR"
    );
  rTextdrawObject.DrawText(0, 18, sTmpStr);

  // Calculate the frames per second (time from last cycle)
  float8 fElapsed = m_frameTimer.ElapsedTime();
  m_frameTimer.Reset();
  CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  CRect clientRect = viewRect - viewRect.TopLeft();

  // display the frames rate in the lower right corner
  int1 sFrameRate[101] = {"0 Hz"};
  if (fElapsed != 0)
  {
    sprintf(sFrameRate, "%d Segs %.0f Err %.1f Hz",myDataset.m_straightSegmentNodes.size() ,myDataset.m_fSegmentErrorThreshold ,float4(1.0 / fElapsed));
  }
  glClear(GL_DEPTH_BUFFER_BIT /*| GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/);
  rTextdrawObject.DrawText(clientRect.right, 2, sFrameRate, GDT_ABOVELEFT); // At bottom-right
#endif

  if ( IsReportRendering() )
  {
    glPushAttrib(GL_CURRENT_BIT);
    CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
    CRect clientRect = viewRect - viewRect.TopLeft();
    glColor3f(0,1,0);
    std::string sPatInfo = GetDatasetRef().m_studyinfo.m_sPatientID + ", " + GetDatasetRef().m_studyinfo.m_sExamTime;
    GetNormalFont().DrawText(m_iTextPadding, clientRect.bottom, sPatInfo.c_str(), GDT_BELOW); // At bottom-left
    glPopAttrib();
  }

  glFlush();

} // GLRasterScene()


/*
 * render objects: triangle/bookmark/skeleton/connections/...
 *
 * @param viewModes      current view mode.
 * @param bIndexRender   when true: non-shaded index-rendering for "pick" operation.
 * @param bSelected      whether to render all segments or just the selected
 */
void OverView::RenderObjects(const VtxViewModes& viewModes, const bool bIndexRender, const bool bSelected)
{
  Dataset& myDataset = GetDatasetRef();
  CRect rect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  
  if ((!myDataset.m_segmentList.IsLoaded()) || rect.IsRectEmpty())
  {
    return;
  }

  GLfloat vfOGLMatrix[16];
  GLfloat vfAmbient[] = {0.5F,0.5F,0.5F,1.0F};
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, vfAmbient);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  const float4 fFOV = myDataset.m_pTrackball->GetZoomFactor();
  // update projection matrix or not.
  if (m_bUpdateProjMatrix)
  {
    // update projection matrix.
    glLoadIdentity();
    GLfloat fAspect = (GLfloat) m_uViewWth / (GLfloat) m_uViewHgt;
    gluPerspective( fFOV, fAspect, 1.0, 2400.0 );
    glGetDoublev(GL_PROJECTION_MATRIX, m_mProjMatrix);
    m_bUpdateProjMatrix = false;
  }
  else
  {
    // use current projection matrix.
    glLoadMatrixd(m_mProjMatrix);
  } // end-if (m_bUpdateProjMatrix)

  // update modelview matrix or not.
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  if (m_bUpdateModelViewMatrix)
  {
    // the matrix need update.
    glTranslatef((float4)(myDataset.m_pTrackball->GetTransferX()), -(float4)(myDataset.m_pTrackball->GetTransferY()), -1024);
    // build the rotation matrix
    myDataset.m_pTrackball->GetCurrentMatrix().SetOpenGLMatrix(vfOGLMatrix);
    glMultMatrixf(vfOGLMatrix);
    // translate the original point to the center of the data volume
    glTranslatef(-float4(myDataset.m_vDim.m_x * myDataset.m_vUnits.m_x)/2.0,
      -float4(myDataset.m_vDim.m_y * myDataset.m_vUnits.m_y)/2.0,
      -float4(myDataset.m_vDim.m_z * myDataset.m_vUnits.m_z)/2.0);
    // save this modelview matrix so as to reduce redundant calculation (for render camera, ...).
    glGetDoublev(GL_MODELVIEW_MATRIX, m_mModelViewMatrix);
    m_bUpdateModelViewMatrix = false;
  }
  else
  {
    // using current matrix.
    glLoadMatrixd(m_mModelViewMatrix);
  } // end-if (m_bUpdateModelViewMatrix)

  if (bIndexRender || IsMode(OverviewRender::eSegmentPicking) || IsMode(OverviewRender::eSegmentVerify)
    || IsMode(OverviewRender::eRegistrationVerify) || !(viewModes & ViewModes::TRANSLUCENT))
  {
    // Solid Rendering

    // render colon.
    if (IsKeyStateDown(VK_SHIFT))
	  {
      // Segment selections preview mode
      OverviewRender::RenderModeEnum eRenderMode = GetRenderMode();
      VtxViewModes currVtxMode = viewModes; currVtxMode -= ViewModes::TRANSLUCENT; currVtxMode -= ViewModes::MISSED_REGIONS;
      SetRenderMode(OverviewRender::eNormal);
      m_overviewRender.RenderTris(myDataset, currVtxMode, bIndexRender, bSelected, PickSegtColors, m_bRefDisplayList);
      SetRenderMode(eRenderMode);
	  }
	  else
    {
      m_overviewRender.RenderTris(myDataset, viewModes, bIndexRender, bSelected, PickSegtColors, m_bRefDisplayList);
    }

    if (viewModes & ViewModes::SLICE)
    {
      m_overviewRender.RenderSlices(myDataset, viewModes);

      // cache where the slices were drawn
      m_vDrawnSlicePosition = rdrGlobal.m_pCurrDataset->m_vOrthogonalPtWorld;
    }

    // Want the rest always visible (drawn over the colon)
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // improve the reality a little.
    if(viewModes & ViewModes::SLICE)
    {
      // get the depth info, but not to write to color buffer.
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      m_overviewRender.RenderSlices(myDataset, viewModes);
      // cache where the slices were drawn
      m_vDrawnSlicePosition = rdrGlobal.m_pCurrDataset->m_vOrthogonalPtWorld;
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    // render skeleton & connections.
    if(!bIndexRender)
    {
      if ((m_eSaveReportImage == eSaveNone) &&
        (IsMode(OverviewRender::eSegmentPicking) || IsMode(OverviewRender::eSegmentVerify) || (viewModes & ViewModes::CENTERLINE)))
      {
        if (m_bRenderSimplifiedSegments)
        {
          m_overviewRender.RenderSimplifiedSegments(myDataset, viewModes, 2.0, GetViewport(), float4(eBOOKMARK_SIZE/rect.Width()));
        }
        else
        {
          m_overviewRender.RenderSkel(myDataset, viewModes, 2.0, GetViewport());
        }
      }

      if (m_bRenderReportArrow)
      {
        m_overviewRender.RenderReportConnection(m_reportStartPt, m_reportEndPt);
      }

      if (IsMode(OverviewRender::eSegmentPicking) || IsMode(OverviewRender::eSegmentVerify))
      {
        m_overviewRender.RenderSegNum(myDataset, & GetNormalFont());
      }

      if ( (IsMode(OverviewRender::eNormal) || IsMode(OverviewRender::eRegisteredViews)) && (viewModes & ViewModes::BOOK_MARK))
      {
        m_overviewRender.RenderBookMarks(myDataset.m_session.m_bookmarkList, float4(eBOOKMARK_SIZE/rect.Width()));
        
        if (rdrGlobal.m_bDisplayCadFindings)
        {
          m_overviewRender.RenderCadFindings( myDataset.m_cadFindings, myDataset.m_session.m_mapCadFindingInfo,
                                              myDataset.m_cadFindings.m_iCadThresholdValue, float4(eBOOKMARK_SIZE/rect.Width()) );
        }
      }

      GLFont& glFont = GetLargeFont();
      if (IsMode(OverviewRender::eRegistrationVerify))
      {
        glPushAttrib(GL_CURRENT_BIT);
          const float4 fPointSize(eBOOKMARK_SIZE / rect.Width());

          int1 sFeatureIndex[21] = "1";
          const Point3D<float4> originPt(0,0,0);
          const Triple<float4> colorBlack(0,0,0), colorWhite(1,1,1);
          
          for (uint4 i(0); i < myDataset.m_session.m_vUserFeatures.size(); i++)
          {
            glPushMatrix();
              Point<float4> pt(myDataset.m_joinedSkeleton.GetWorldPos(myDataset.m_session.m_vUserFeatures[i]));
              glTranslatef(pt.m_x, pt.m_y, pt.m_z);
              sprintf(sFeatureIndex, "%d", i+1);
              RenderUtil::GLDrawPoint2f(Point2D<float4>(0,0), PixelRGBA<float4>(1,0,0,1), 7);
              glColor3f(0,0,1);
              glFont.DrawText(originPt.m_x, originPt.m_y, originPt.m_z, sFeatureIndex);
            glPopMatrix();
          }

          if (IsMode(OverviewRender::eRegistrationVerify) && (m_eMouseOverFeature != OverviewFeature::eUserFeature))
          {
            glPushMatrix();
            Point<float4> pt(myDataset.m_joinedSkeleton.GetWorldPos(m_overviewRender.m_uRegistrationVerificationPreviewIndex));
            glTranslatef(pt.m_x, pt.m_y, pt.m_z);
            glColor3f(1.0F,1.0F,1.0F);
            m_overviewRender.RenderSingleBookmark(fPointSize*0.75);
            glPopMatrix();
          }
        glPopAttrib();
      }

      if (rdrGlobal.m_bAllFeatures)
      {
        m_overviewRender.RenderRegistrationAllFeatures(myDataset, float4(eBOOKMARK_SIZE/rect.Width()), &GetNormalFont());
      }

      if (rdrGlobal.m_bMatchedFeatures)
      {
        m_overviewRender.RenderRegistrationMatchedFeatures(myDataset, float4(eBOOKMARK_SIZE/rect.Width()), PickSegtColors, &GetNormalFont());
      }
    } // if (!bIndexRender)
  } // if (bIndexRender || IsMode(OverviewRender::eSegmentPicking) || !(viewModes & ViewModes::TRANSLUCENT))
  else 
  {
    // push attributes (mainly for the GL_BLEND enable bit and blend src/dest functions)
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /// volume render transparent over view
      if ( myDataset.IsLoadingComplete() && myDataset.m_fields.GetOpacityVolumePossible() )
      {
        static bool bFirstTranslucentRender(true);
        CWaitCursor * pWaitCrsr(NULL);
        if (bFirstTranslucentRender)
        {
          pWaitCrsr = new CWaitCursor;
        }
        bFirstTranslucentRender = false;

        bool bHighResolution = IsMainViewWindow() && (m_iTimeCounter >= 8);
        m_overviewRender.RayCastVolume2(myDataset, bHighResolution, 1.0, fFOV, m_uViewWth, m_uViewHgt);

        if (pWaitCrsr != NULL)
          delete pWaitCrsr;
      } // endif (draw colon: surface or volume rendering)

      // render skeleton&connections
      if ((m_eSaveReportImage == eSaveNone) && (IsMode(OverviewRender::eSegmentVerify) || (viewModes & ViewModes::CENTERLINE)))
      {
        m_overviewRender.RenderSkel(myDataset, viewModes, 2.0, GetViewport());
        m_overviewRender.RenderConnections(myDataset, viewModes);
      }

      if (m_bRenderReportArrow)
      {
        m_overviewRender.RenderReportConnection(m_reportStartPt, m_reportEndPt);
      }
      if (viewModes & ViewModes::BOOK_MARK)
      {
        m_overviewRender.RenderBookMarks(myDataset.m_session.m_bookmarkList, float4(eBOOKMARK_SIZE/rect.Width()));

        if (rdrGlobal.m_bDisplayCadFindings)
        {
          m_overviewRender.RenderCadFindings( myDataset.m_cadFindings, myDataset.m_session.m_mapCadFindingInfo,
                                              myDataset.m_cadFindings.m_iCadThresholdValue, float4(eBOOKMARK_SIZE/rect.Width()) );
        }
      }

      if (rdrGlobal.m_bAllFeatures)
      {
        m_overviewRender.RenderRegistrationAllFeatures(myDataset, float4(eBOOKMARK_SIZE/rect.Width()),
          & GetNormalFont());
      }

      if (rdrGlobal.m_bMatchedFeatures)
      {
        m_overviewRender.RenderRegistrationMatchedFeatures(myDataset, float4(eBOOKMARK_SIZE/rect.Width()),
          PickSegtColors, & GetNormalFont());
      }


      // draw the triangles in the end in translucent mode.
      // Ray cast the volume. If failed, switched to polygon rendering
      if ( ! myDataset.IsLoadingComplete() || ! myDataset.m_fields.GetOpacityVolumePossible() )
      {
        m_overviewRender.RenderTris(myDataset, viewModes, bIndexRender, bSelected, PickSegtColors, m_bRefDisplayList);
      }
    
      // Want the rest always visible (drawn over the colon)
      glClear(GL_DEPTH_BUFFER_BIT);
      if (viewModes & ViewModes::SLICE)
      {
        m_overviewRender.RenderSlices(myDataset, viewModes);

        // cache where the slices were drawn
        m_vDrawnSlicePosition = rdrGlobal.m_pCurrDataset->m_vOrthogonalPtWorld;
      }
    }
    glPopAttrib();
  } // translucent rendering.

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
} // RenderObjects()


/**
 * response to resize message.
 *
 * @param iWidth  width of the window.
 * @param iHeight height of the window.
 */
void OverView::GLResize(GLsizei iWidth, GLsizei iHeight)
{
  glViewport(0, 0, m_uViewWth, m_uViewHgt);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  const float4 fFactor(float4(GetDatasetRef().m_pTrackball->GetZoomFactor()) / 1.0F);
  GLfloat fAspect(GLfloat(m_uViewWth) / GLfloat(m_uViewHgt));
  gluPerspective(fFactor, fAspect, 1.0, 2400.0);
  glGetDoublev(GL_PROJECTION_MATRIX, m_mProjMatrix);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
} // GLResize()


/**
 * WM_SIZE handler, Called when window is resized
 *
 * @param uType   Type of resizing (e.g. maximize, restore)
 * @param iCX     Width
 * @param iCY     Height
 */
void OverView::OnSize(UINT uType, int4 iCX, int4 iCY)
{
  ViatronChildView::OnSize( uType, iCX, iCY );

  if ( m_puPickBuffer != NULL )
  {
    delete m_puPickBuffer;
  }
  m_puPickBuffer = new uint1[GetViewport().Width() * GetViewport().Height() * 3];

  int4 iHeight = GetViewport().Height();

  if (iHeight==0)
  {
    iHeight = 1;
  }

  m_uViewWth = GetViewport().Width();
  m_uViewHgt = iHeight;

  HWND hDlgBar(GetParent()->GetDlgItem( IDC_TEMPLATE_OVERVIEWTOOLS )->GetSafeHwnd() );
  if( IsWindow( hDlgBar ) )
    ::ShowWindow(hDlgBar, IsMainViewWindow() ? SW_HIDE : SW_SHOW );
  
  SetImageBuffered( false );

  ResetTimer(5);
} // GLResize()


/**
 * Match the point to the sorted segment.
 *
 * @param point                    Point of selection.
 * @param bMatchSelectedSegments   Whether to use the list of selected or unselected segments for matches
 * @return                         >0 if the matched segment is found, -1: no match.
 */
int4 OverView::GetSegmentHit( const CPoint & point, const bool & bMatchSelectedSegments )
{
  wglMakeCurrent( GetHDC(), GetRenderingContext() );
  GLResize( GetViewport().Width(), GetViewport().Height() );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  RenderObjects( ViatronStudy::m_modeOverView, true, bMatchSelectedSegments );
  glFinish();

  CRect rect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  if ( (rect.Width() <= 0) || (rect.Height() <= 0) )
  {
    return -1;
  }

  glPixelStorei( GL_PACK_ROW_LENGTH, rect.Width() );
  glPixelStorei( GL_PACK_ALIGNMENT, 1 );
  glReadPixels( GetViewport().m_left,GetViewport().m_top,rect.Width(),rect.Height(),GL_RGB,GL_UNSIGNED_BYTE,(void*)m_puPickBuffer );
  uint1 uRed   = m_puPickBuffer[((rect.Height()-point.y)*rect.Width() + point.x)*3  ];
  uint1 uGreen = m_puPickBuffer[((rect.Height()-point.y)*rect.Width() + point.x)*3+1];
  uint1 uBlue  = m_puPickBuffer[((rect.Height()-point.y)*rect.Width() + point.x)*3+2];
  uint4 x = point.x;
  uint4 y = (rect.Height()-point.y);

  Dataset& myDataset = GetDatasetRef();
  for (int4 s=0; s < myDataset.m_segmentList.GetNumUnsortedSegments(); s++)
  {
    if (myDataset.m_segmentList.GetUnsortedSegment(s).IsSelected() == bMatchSelectedSegments)
    {
      if (uRed   == PickSegtColors[s].m_x &&
          uGreen == PickSegtColors[s].m_y &&
          uBlue  == PickSegtColors[s].m_z)
      {
        return s;
      } // if segment is matched
    } // if segment is selected
  } // for all unsorted segments
  return -1;
} // GetSegmentHit()


/**
 * Returns the index of the nearest skeleton node to the given point.
 *
 * @param point      CPoint on the view window.
 * @param skeleton   skeleton node to test
 * @return           -1: no match, otherwise, a node in a skeleton.
 */
int4 OverView::GetNearestSkeletonNode(const CPoint & point, Skeleton & skeleton) const
{
  Dataset& myDataset = GetDatasetRef();
  int4 iSelectedNode = skeleton.GetNumOfElements()/2;

  CRect rect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  GLint viewport[4];
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = rect.Width();
  viewport[3] = rect.Height();

  Point<float8> vWinCoords(0.0,0.0,0.0);
  float4 fMinimumDistance = 1e10F;
  const Point<float8> vPickCoords(point.x, rect.Height() - point.y, 0);

  for (uint4 i=0; i<skeleton.GetNumOfElements(); i++)
  {
    // get the current point in world coords and window coords
    Point<float4> vNodePoint = skeleton.GetSkelNode(i).m_vWorldPos;
    gluProject((float8)vNodePoint.m_x, (float8)vNodePoint.m_y, (float8)vNodePoint.m_z,
               m_mModelViewMatrix, m_mProjMatrix, viewport,
               &vWinCoords.m_x, &vWinCoords.m_y, &vWinCoords.m_z);
    vWinCoords.m_z = 0;

    // compute the distance from the cursor
    const float4 fDistance = Vector<float8>(vPickCoords - vWinCoords).GetLength();

    // store it if it is closer than the others
    if (fDistance < fMinimumDistance)
    {
      fMinimumDistance = fDistance;
      iSelectedNode = i;
    } // if found a closer point
  } // loop over all skeleton nodes

  return iSelectedNode;
} // GetNearestSkeletonNode()


/**
 * Returns the index of the nearest skeleton node to the given point.
 *
 * @param point      CPoint on the view window.
 * @param skeleton   skeleton node to test
 * @return           -1: no match, otherwise, a node in a skeleton.
 */
int4 OverView::GetNearestJoinedSkeletonNode(const CPoint & point, SkeletonVCExt & skeleton) const
{
  Dataset& myDataset = GetDatasetRef();
  int4 iSelectedNode = skeleton.GetNumOfElements()/2;

  CRect rect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );  GLint viewport[4];
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = rect.Width();
  viewport[3] = rect.Height();

  Point<float8> vWinCoords(0.0,0.0,0.0);
  float4 fMinimumDistance = 1e10F;
  const Point<float8> vPickCoords(point.x, rect.Height() - point.y, 0);

  for (uint4 i=0; i<skeleton.GetNumOfElements(); i++)
  {
    // get the current point in world coords and window coords
    Point<float4> vNodePoint = skeleton.GetWorldPos(i);
    gluProject((float8)vNodePoint.m_x, (float8)vNodePoint.m_y, (float8)vNodePoint.m_z,
               m_mModelViewMatrix, m_mProjMatrix, viewport,
               &vWinCoords.m_x, &vWinCoords.m_y, &vWinCoords.m_z);
    vWinCoords.m_z = 0;

    // compute the distance from the cursor
    const float4 fDistance = Vector<float8>(vPickCoords - vWinCoords).GetLength();

    // store it if it is closer than the others
    if (fDistance < fMinimumDistance)
    {
      fMinimumDistance = fDistance;
      iSelectedNode = i;
    } // if found a closer point
  } // loop over all skeleton nodes

  return iSelectedNode;
} // GetNearestJoinedSkeletonNode()


/**
 * Returns a list of indices of bookmarks at the specified point.
 *
 * @param point   A CPoint on the window.
 * @param list    List of bookmarks to search.
 * @return        List of selected bookmarks, empty list if none located at point.
 */
list<uint4> OverView::GetBookmarksHit(const CPoint & point, const BookmarkList& bmList)
{
  if (bmList.empty())
  {
    return list<uint4>();
  }

  wglMakeCurrent( GetHDC(), GetRenderingContext() );

  glPushAttrib(GL_TRANSFORM_BIT | GL_CURRENT_BIT);

  GLResize( GetViewport().Width(), GetViewport().Height() );

  // render
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glLoadMatrixd(m_mProjMatrix);
  glMatrixMode(GL_MODELVIEW);

  CRect clientRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  const int4 iClientWidth(clientRect.Width());
  const int4 iClientHeight(clientRect.Height());

  glPushMatrix();
  {
    glLoadMatrixd(m_mModelViewMatrix);
    // draw bookmarks
    uint4 uBk(0);
    for (BookmarkList::const_iterator itBk = bmList.begin(); itBk != bmList.end(); itBk++, uBk++)
    {
      // TODO: Use an accumulation or stencil buffer (or whatever might work)
      //        to distinguish possible multiple bookmarks drawn at same pixel locations
      glColor3ub(PickSegtColors[uBk].m_x, PickSegtColors[uBk].m_y, PickSegtColors[uBk].m_z);
      glPushMatrix();
      {
        Point<float4> pos = (*itBk).m_position;
        glTranslated(pos.m_x,pos.m_y,pos.m_z);
        m_overviewRender.RenderSingleBookmark(float4(eBOOKMARK_SIZE / iClientWidth));
      }
      glPopMatrix();
    } // endfor all bookmarks
  }
  glPopMatrix();
  glFinish();

  glPixelStorei(GL_PACK_ROW_LENGTH, iClientWidth);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  if ( (iClientWidth <= 0) || (iClientHeight <= 0) )
  {
    return list<uint4>();
  }

  glReadBuffer( GL_BACK );
  glReadPixels(GetViewport().m_left,GetViewport().m_top,iClientWidth,iClientHeight,GL_RGB,GL_UNSIGNED_BYTE,(void*)m_puPickBuffer);

  const int4 iPtStart(( ((iClientHeight-point.y) * iClientWidth) + point.x) * 3);
  const Triple<uint1>
    clickColor(m_puPickBuffer[iPtStart], m_puPickBuffer[iPtStart+1], m_puPickBuffer[iPtStart+2]);

  list<uint4> matchIdxList;
  {
    uint4 uBk(0);
    for (uBk=0; uBk < bmList.size(); uBk++)
    {
      // TODO: In keeping with draw method above, distinguish the possible one of many
      //        bookmarks that may be selected at/near same point.
      if ( (clickColor == PickSegtColors[uBk]) )
      {
        matchIdxList.push_back(uBk);
      }
    } // endfor all bookmarks
  }

  glPopAttrib();

  return matchIdxList;
} // GetBookmarksHit()


/**
 * Returns a list of indices of bookmarks at the specified point.
 *
 * @param point   A CPoint on the window.
 * @return        List of selected bookmarks, empty list if none located at point.
 */
int4 OverView::GetRegistrationPointHit(const CPoint & point)
{
  Dataset& myDataset = GetDatasetRef();

  if (myDataset.m_session.m_vUserFeatures.empty())
  {
    return -1;
  }

  // render
  glPushAttrib(GL_TRANSFORM_BIT | GL_CURRENT_BIT);
  wglMakeCurrent( GetHDC(), GetRenderingContext());
  GLResize( GetViewport().Width(), GetViewport().Height() );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glLoadMatrixd(m_mProjMatrix);
  glMatrixMode(GL_MODELVIEW);

  CRect clientRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  const int4 iClientWidth(clientRect.Width());
  const int4 iClientHeight(clientRect.Height());
  if ( (iClientWidth <= 0) || (iClientHeight <= 0) ) return -1;

  glPushMatrix();
  {
    glLoadMatrixd(m_mModelViewMatrix);
    // draw User Features
    for (uint4 f(0); f < myDataset.m_session.m_vUserFeatures.size(); f++)
    {
      glColor3ub(PickSegtColors[f].m_x, PickSegtColors[f].m_y, PickSegtColors[f].m_z);
      glPushMatrix();
      {
        Point<float4> pos = myDataset.m_joinedSkeleton.GetWorldPos(myDataset.m_session.m_vUserFeatures[f]);
        glTranslated(pos.m_x,pos.m_y,pos.m_z);
        m_overviewRender.RenderSingleBookmark(float4(eBOOKMARK_SIZE / iClientWidth));
      }
      glPopMatrix();
    } // endfor all User Features
  }
  glPopMatrix();
  glFinish();

  glPixelStorei(GL_PACK_ROW_LENGTH, iClientWidth);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(GetViewport().m_left,GetViewport().m_top,iClientWidth,iClientHeight,GL_RGB,GL_UNSIGNED_BYTE,(void*)m_puPickBuffer);

  const int4 iPtStart(( ((iClientHeight-point.y) * iClientWidth) + point.x) * 3);
  const Triple<uint1> clickColor(m_puPickBuffer[iPtStart], m_puPickBuffer[iPtStart+1], m_puPickBuffer[iPtStart+2]);

  for (uint4 f=0; f < myDataset.m_session.m_vUserFeatures.size(); f++)
  {
    if (clickColor == PickSegtColors[f]) return f;
  } // endfor all User Features

  glPopAttrib();
  return -1;
} // GetRegistrationPointHit()


/**
* Returns a list of GUIDs/list-indices of CAD Findings at the specified point.
*
* @param point   A CPoint on the window.
* @return        List of selected cad findings, empty list if none located at point.
*/
list<string> OverView::GetCadFindingHit(const CPoint & point)
{
  if (GetDatasetRef().m_cadFindings.GetCount() <= 0)
  {
    return list<string>();
  }

  wglMakeCurrent( GetHDC(), GetRenderingContext() );

  glPushAttrib(GL_TRANSFORM_BIT | GL_CURRENT_BIT);

  GLResize( GetViewport().Width(), GetViewport().Height() );

  // render
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glLoadMatrixd(m_mProjMatrix);
  glMatrixMode(GL_MODELVIEW);

  CRect clientRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  const int4 iClientWidth(clientRect.Width());
  const int4 iClientHeight(clientRect.Height());

  const Dataset& thisDS = GetDatasetRef();
  const vxCadFindings findList = thisDS.m_cadFindings;
  map< string, ColonSession::FindingInfo >::const_iterator it;

  glPushMatrix();
  {
    glLoadMatrixd(m_mModelViewMatrix);
    // draw bookmarks
    for (int4 i(0); i < findList.GetCount(); i++)
    {
      const vxCadFinding& finding = findList.GetFinding(i);
      it = thisDS.m_session.m_mapCadFindingInfo.find( finding.GetId() );
      if (it == thisDS.m_session.m_mapCadFindingInfo.end() || it->second.GetDecision() == ColonSession::FindingInfo::eDecisionDenied )
      {
        continue;
      }

      // TODO: Use an accumulation or stencil buffer (or whatever might work)
      //        to distinguish possible multiple bookmarks drawn at same pixel locations
      glColor3ub(0,0,i+1);
      glPushMatrix();
      {
        Vector3D<float4> pos(finding.GetCentroidMM().GetVector());
        glTranslated(pos.m_x,pos.m_y,pos.m_z);
        m_overviewRender.RenderSingleBookmark(float4(eBOOKMARK_SIZE / iClientWidth));
      }
      glPopMatrix();
    } // endfor all bookmarks
  }
  glPopMatrix();
  glFinish();

  glPixelStorei(GL_PACK_ROW_LENGTH, iClientWidth);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  CRect rect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );  
  if ( (iClientWidth <= 0) || (iClientHeight <= 0) )
  {
    return list<string>();
  }

  glReadBuffer( GL_BACK );
  glReadPixels(GetViewport().m_left,GetViewport().m_top,iClientWidth,iClientHeight,GL_RGB,GL_UNSIGNED_BYTE,(void*)m_puPickBuffer);

  const int4 iPtStart(( ((iClientHeight-point.y) * iClientWidth) + point.x) * 3);
  const Triple<uint1>
    clickColor(m_puPickBuffer[iPtStart], m_puPickBuffer[iPtStart+1], m_puPickBuffer[iPtStart+2]);

  list<string> matchIdxList;
  if (clickColor.m_z > 0)
  {
    matchIdxList.push_back(findList.GetFinding(clickColor.m_z-1).GetId());
  }

  glPopAttrib();

  return matchIdxList;
} // GetCadFindingHit()


/**
 *  save a snapshot of the current overview image to the rendering buffer.
 */
void OverView::SaveBufferImage()
{
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  GLRasterResize( GetViewport().Width(), GetViewport().Height() );
  GLRenderLogo();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();

  GLSaveUpdateBuffer();
  SetImageBuffered( true );
} // SaveBufferImage()


/**
 * handler responses to double-click left mouse button.
 *
 * @param  uFlags Indicates whether various virtual keys are down.
 * @param  point  specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnLButtonDblClk(UINT uFlags, CPoint point)
{
  try
  {
    Dataset& myDataset = GetDatasetRef();
    if (!myDataset.m_segmentList.IsLoaded())
    {
      GLChildView::OnLButtonDblClk(uFlags, point);
      return;
    }

    if (IsMode(OverviewRender::eSegmentPicking))
    {
      int4 iSegment = GetSegmentHit(point,false); // get from "as yet unselected" list of segments
      // if we matched any segment
      if (iSegment != -1)
      {
        myDataset.m_segmentList.AddUnsortedSegmentToSortedSegments(iSegment);

        Segment & segment = myDataset.m_segmentList.GetUnsortedSegment(iSegment);
        Skeleton & skeleton = segment.GetCurrentSkeleton();

        // if we selected near the after end of the skeleton
        segment.SelectDirection(GetNearestSkeletonNode(point,skeleton) < skeleton.GetNumOfElements() / 2);
      } // endif found a match
    }
    else if (m_overviewRender.GetRenderMode() == OverviewRender::eRegistrationVerify)
    {
      int4 iSegment = GetSegmentHit(point,true); // get from list of selected segments
      if (iSegment != -1)
      {
        if (m_pNeighbourView)
        {
          Segment & segment = myDataset.m_segmentList.GetUnsortedSegment(iSegment);
          int4 iLocalIndexNode = GetNearestSkeletonNode(point,segment.GetCurrentSkeleton());

          bool bTooClose = false;
          uint4 uJoinedIndexNode = myDataset.m_joinedSkeleton.ConvertToJoinedIndex(segment,iLocalIndexNode);
          uint4 uOtherJoinedIndexNode = myDataset.m_joinedSkeleton.GetRegistrationOtherIndex(uJoinedIndexNode);

          if ( uJoinedIndexNode      >= myDataset              .m_joinedSkeleton.GetNumOfElements() ||
              uOtherJoinedIndexNode >= GetOppositeDatasetRef().m_joinedSkeleton.GetNumOfElements() )
          {
            bTooClose = true;
            LogWrn( "joined index out of range: " + ToAscii( int4( uJoinedIndexNode ) ) + "/" +
              ToAscii( int4( myDataset.m_joinedSkeleton.GetNumOfElements() ) ) + "  other: " +
              ToAscii( int4( uOtherJoinedIndexNode ) ) + "/" + 
              ToAscii( int4( GetOppositeDatasetRef().m_joinedSkeleton.GetNumOfElements() ) ), "OverView", "OnLButtonDblClk" );
          }

          // check that either supine or prone feature is not "too close" to another node
          for (uint4 i=0; ! bTooClose && i<myDataset.m_session.m_vUserFeatures.size(); i++)
          {
            if (uJoinedIndexNode < myDataset.m_session.m_vUserFeatures[i]+5 &&
                uJoinedIndexNode > myDataset.m_session.m_vUserFeatures[i]-5)
            {
              bTooClose = true;
            }
            if (uOtherJoinedIndexNode < GetOppositeDatasetRef().m_session.m_vUserFeatures[i]+5 &&
                uOtherJoinedIndexNode > GetOppositeDatasetRef().m_session.m_vUserFeatures[i]-5)
            {
              bTooClose = true;
            }
          } // endfor all previous user features

          if (!bTooClose)
          {
            // Add feature and sort
            myDataset.m_session.m_vUserFeatures.push_back(uJoinedIndexNode);
            GetOppositeDatasetRef().m_session.m_vUserFeatures.push_back(uOtherJoinedIndexNode);
            std::sort(myDataset.m_session.m_vUserFeatures.begin(),myDataset.m_session.m_vUserFeatures.end());
            std::sort(GetOppositeDatasetRef().m_session.m_vUserFeatures.begin(),GetOppositeDatasetRef().m_session.m_vUserFeatures.end());
          } // endif feature not too close to another feature to add it

          m_bUserFeaturesChanged = true;
        }
      }
    }
    else
    {
      if (rdrGlobal.m_pCurrDataset->GetOrientation() != myDataset.GetOrientation())
      {
        AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM(myDataset.GetOrientation()), 0);
      }

      // first: check for a click on a bookmark.
      std::list<uint4> listMarks;
      std::list<std::string> listFindings;
      if ( !(listMarks = GetBookmarksHit(point, myDataset.m_session.m_bookmarkList)).empty() )
      {

        if (ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY)
        {
          ColonViatronixWnd::ToggleAutoflyMode();
        }
        else if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
        {
          ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
          AfxGetMainWnd()->PostMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
        }

        // Go to first bookmark
        myDataset.m_session.m_bookmarkList.StopAtBookmark(listMarks.front(), myDataset);
        ViatronStudy::TurnOffAutoflight();
        InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

      } // if a cad finding matched
      else if ( !(listFindings = GetCadFindingHit(point)).empty() )
      {
        Utilities::CAD::JumpTo( GetDatasetRef(), listFindings.front() );
      }
      else
      {
        // get from list of selected segments
        int4 iSegment = GetSegmentHit(point, true);
        // if we matched any segment
        if (iSegment != -1)
        {

          Segment & segment = myDataset.m_segmentList.GetUnsortedSegment(iSegment);
          Skeleton & skeleton = segment.GetCurrentSkeleton();
          int4 iIndexNode = GetNearestSkeletonNode(point,skeleton);

          bool bRegular = segment.IsRegularDirection();
          if (myDataset.m_submarine.GetFlightDirection() == Submarine::RECTUM_TO_CECUM)
          {
            bRegular = !bRegular;
            // if regular   & R_TO_C -> flip = false;
            // if irregular & R_TO_C -> flip = true;
            // if regular   & C_TO_R -> flip = true;
            // if irregular & C_TO_R -> flip = false;
          }

          myDataset.m_segmentList.SetCurrentSegmentIndex(myDataset.m_segmentList.GetSortedSegmentIndex(iSegment));
          myDataset.m_submarine.SetViewSkeleton(skeleton.GetSkelNode(iIndexNode), bRegular);
          if (m_bRenderSimplifiedSegments)
          {
            uint4 uJoinedSkelIndex = myDataset.m_joinedSkeleton.ConvertToJoinedIndex(myDataset.m_segmentList.GetCurrentSegment(),iIndexNode);
            for(uint4 s=0;s<myDataset.m_straightSegmentNodes.size();s++)
            {
              if(iIndexNode > myDataset.m_straightSegmentNodes[s]) myDataset.m_currentSegment = s;
            }
          } // endif also update the current segments

          // if in 2d-auto-flight, update skeleton position.
          if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
          {
            const long UPDATE_SKELETON = 0;
            GetParent()->SendMessage( WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(iIndexNode), LPARAM(UPDATE_SKELETON) );
          }

          // send message to synchronize datasets if necessary; handle case of (single-level) nested view if need be
          CWnd *pParent = GetParent();
          if (pParent->SendMessage( WM_APP_SYNCRONIZE_DATASET_LOCATIONS, WPARAM(GetDatasetRef().GetOrientation()),
            LPARAM(GetOppositeDatasetRef().GetOrientation())) == 0 )
          {
            pParent->GetParent()->SendMessage( WM_APP_SYNCRONIZE_DATASET_LOCATIONS, WPARAM(GetDatasetRef().GetOrientation()), LPARAM(GetOppositeDatasetRef().GetOrientation()) );
          }

          // stop flying
          if (ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY)
          {
            ColonViatronixWnd::ToggleAutoflyMode();
          }
          else if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
          {
            ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
            AfxGetMainWnd()->PostMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
          }

          ViatronStudy::m_modeOrthogView -= ViewModes::AUTOFLY;
          ViatronStudy::m_modeOrthogView -= ViewModes::AUTOFLY2D;
          ViatronStudy::m_modeObliqueView -= ViewModes::AUTOFLY;
          ViatronStudy::m_modeObliqueView -= ViewModes::AUTOFLY2D;
          ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY;
          ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;

          // come to a complete stop because we jumped to a completely new position
          rdrGlobal.m_pCurrDataset->m_submarine.FullStop();
          rdrGlobal.m_pSecondaryDataset->m_submarine.FullStop();

          myDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
          InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);

        }
        else
        {
          // no match...
          GLChildView::OnLButtonDblClk(uFlags, point);
        } // endif found a segment match

      } // if no bookmark or cadfinding matched
    } // if registration, picking or default mode
    ResetTimer(8);
    SetImageBuffered( false );

    wglMakeCurrent( NULL, NULL );
  } // try
  catch ( ... )
  {
    LogErr( "unhandled exception", "OverView", "OnLButtonDblClk" );
  }
} // OnLButtonDblClk()


/**
 * handler responses to left button press
 *
 * @param  uFlags Indicates whether various virtual keys are down.
 * @param  point  specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnLButtonDown(UINT uFlags, CPoint point)
{
  Dataset& myDataset = GetDatasetRef();
  if (!myDataset.m_segmentList.IsLoaded())
  {
    GLChildView::OnLButtonDown(uFlags, point);
    return;
  }

  SetCapture();
  myDataset.m_pTrackball->SetRotateStartPoint(Point2D<int4>(point.x, point.y));

  if (IsMode(OverviewRender::eRegistrationVerify))
  {
    m_iCurSelRegistrationPoint = GetRegistrationPointHit(point);
  }

  if ( Utilities::Datasets::IsLocked() && (m_pNeighbourView != NULL))
  {
    m_pNeighbourView->ResetTimer();
  }
  ResetTimer();
///  Invalidate(FALSE);
} // OnLButtonDown()


/**
 * get the popup menu ID.
 *
 * @param  uFlags Indicates whether various virtual keys are down.
 * @param  point  specifies the x- and y-coordinate of the cursor.
 * @return  a menu ID.
 */
uint4 OverView::GetPopupMenuID(UINT uFlags, CPoint point) const
{
  if (m_overviewRender.GetRenderMode() == OverviewRender::eRegistrationVerify)
  {
    return (!GetDatasetRef().m_session.m_vUserFeatures.empty() ? IDM_USERREGISTRATION_POPUP : 0);
  }
  else if (m_iCurSelBookmark >= 0)
  {
    return IDM_BOOKMARK_POPUP;
  }
  else if (!m_idCurSelCadFinding.empty())
  {
    return IDM_CADFINDING_POPUP;
  }
  else
  {
    return IDM_OVERVIEW;
  }
} // GetPopupMenuID()


/*
 * adjust the items in popup-menu (response to right mouse click)
 *
 * @param menu  reference to pop-up menu.
 * @param uMenuID reference to a menu-id.
 */
void OverView::AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const
{
  switch (uMenuID)
  {
    case IDM_BOOKMARK_POPUP:
    {
      const BookmarkList& bmList = GetDatasetRef().m_session.m_bookmarkList;
      if ((m_iCurSelBookmark < 0) || (m_iCurSelBookmark >= bmList.size()))
      {
        return;
      }

      menu.ModifyMenu(ID_BOOKMARKS_GOTO, MF_BYCOMMAND, ID_BOOKMARKS_GOTO,
        LoadResourceString(IDS_GOTO) + ' ' + bmList[m_iCurSelBookmark].m_sName.c_str());
      menu.SetDefaultItem(ID_BOOKMARKS_GOTO, MF_BYCOMMAND);

      switch (bmList[m_iCurSelBookmark].m_ePriority)
      {
        case Bookmark::HIGH:
          menu.CheckMenuItem(ID_BOOKMARKS_LEVEL_HIGH, MF_CHECKED | MF_BYCOMMAND);
          break;
        case Bookmark::MEDIUM:
          menu.CheckMenuItem(ID_BOOKMARKS_LEVEL_MEDIUM, MF_CHECKED | MF_BYCOMMAND);
          break;
        case Bookmark::LOW:
          menu.CheckMenuItem(ID_BOOKMARKS_LEVEL_LOW, MF_CHECKED | MF_BYCOMMAND);
          break;
        default:
          break;
      } // endswitch(m_ePriority)

      break;
    } // case IDM_BOOKMARK_POPUP
    case IDM_USERREGISTRATION_POPUP:
    {
      menu.EnableMenuItem(ID_USERREGISTRATION_DELETE, (m_iCurSelRegistrationPoint >= 0) ? MF_ENABLED : MF_GRAYED);
      break;
    }
    case IDM_CADFINDING_POPUP:
    {
      menu.SetDefaultItem(ID_BOOKMARKS_GOTO, MF_BYCOMMAND);
      if (!m_idCurSelCadFinding.empty())
      {
        switch ( GetDatasetRef().m_session.m_mapCadFindingInfo[ m_idCurSelCadFinding ].GetDecision() )
        {
          case ColonSession::FindingInfo::eDecisionAccepted:
            menu.CheckMenuItem(ID_SHOWCAD_SHOW, MF_CHECKED | MF_BYCOMMAND);
            break;
          case ColonSession::FindingInfo::eDecisionDenied:
            menu.CheckMenuItem(ID_SHOWCAD_HIDE, MF_CHECKED | MF_BYCOMMAND);
            break;
          case ColonSession::FindingInfo::eDecisionUndecided:
          default:
            menu.CheckMenuItem(ID_SHOWCAD_UNDECIDED, MF_CHECKED | MF_BYCOMMAND);
            break;
        }
      }
      break;
    }
    case IDM_OVERVIEW:
    case IDM_OVERVIEW_ABBREV:
    case IDM_OVERVIEW_ABBREV_PLUS_DISSECTION:
    {
      const bool vbSlcStates[OverviewMenus::iOverviewSlices] =
      {
        (ViatronStudy::m_modeOverView & ViewModes::CENTERLINE),
        (ViatronStudy::m_modeOverView & ViewModes::AXIAL),
        (ViatronStudy::m_modeOverView & ViewModes::SAGITTAL),
        (ViatronStudy::m_modeOverView & ViewModes::CORONAL),
        (ViatronStudy::m_modeOverView & ViewModes::CROSS_SECTION),
        (ViatronStudy::m_modeOverView & ViewModes::DISSECTIONAL)
      };

      menu.CheckMenuItem(ID_OVERVIEW_OPTIONS_TRANSPARENT,
        ((ViatronStudy::m_modeOverView & ViewModes::TRANSLUCENT) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
      menu.CheckMenuItem(ID_OVERVIEW_DISPLAYOPTIONS_MARKING,
        ((ViatronStudy::m_modeOverView & ViewModes::MISSED_REGIONS) ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

      for (int4 i=0; i<OverviewMenus::iOverviewSlices; i++)
      {
        menu.CheckMenuItem( OverviewMenus::viOverviewSliceIds[i], ( vbSlcStates[i] ? MF_CHECKED : MF_UNCHECKED ) | MF_BYCOMMAND );
        if ( ! m_bPermitSlices )
          menu.EnableMenuItem( OverviewMenus::viOverviewSliceIds[i], MF_GRAYED | MF_DISABLED | MF_BYCOMMAND );
      } // endloop(i)

      if (GetDatasetRef().m_session.m_bookmarkList.size() <= 0)
      {
        menu.EnableMenuItem(IDM_BOOKMARKS_DELETEALLBOOKMARKS, MF_GRAYED | MF_BYCOMMAND);
      } // endif no bookmarks
    break;
    } // case IDM_OVERVIEW
  } // switch (uMenuID)
} // AdjustPopupMenu()


/**
 * Returns the help text for each of the mouse buttons
 *
 * @param uLeftID    String resource ID for use of the left mouse button
 * @param uMidID     String resource ID for use of the middle mouse button
 * @param uRightID   String resource ID for use of the right mouse button
 */
void OverView::GetMouseKeyStringIDs(uint4 & uLeftID, uint4 & uMidID, uint4 & uRightID) const
{
  uLeftID = IDS_ROTATE_VIEW;
  uMidID = IDS_ZOOM_VIEW;

  if ( m_overviewRender.GetRenderMode() == OverviewRender::eSegmentPicking )
  {
    uRightID = IDS_SELECT;
  }
  else if ( m_overviewRender.GetRenderMode() == OverviewRender::eSegmentVerify )
  {
    uRightID = 0;
  }
  else
  {
    uRightID = IDS_POPUP_MENU;
  }
} // GetMouseKeyStringIDs()


/**
 * command message handler
 *
 * @param wParam  identifies the command ID of the menu item, control, or accelerator.
 * @param lParam  identifies the control that sends the message if the message is from a control. Otherwise, lParam is 0.
 * @return        nonzero if it processes this message; otherwise 0.
 */
BOOL OverView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  try
  {
    Dataset& myDataset = GetDatasetRef();
    if (m_iCurSelBookmark >= 0)
    {
      switch (LOWORD(wParam))
      {
        case ID_BOOKMARKS_DELETE:
        {
          CString sPromptString;

          sPromptString.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE),
            LoadResourceString(IDS_BOOKMARK) + '<'
            + myDataset.m_session.m_bookmarkList[m_iCurSelBookmark].m_sName.c_str() + '>');

          if (AfxMessageBox(sPromptString, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
          {
            myDataset.m_session.m_bookmarkList.DeleteBookmark(m_iCurSelBookmark);
          }
          break;
        }
        case ID_BOOKMARKS_LEVEL_HIGH:
          myDataset.m_session.m_bookmarkList[m_iCurSelBookmark].m_ePriority = Bookmark::HIGH;
          break;
        case ID_BOOKMARKS_LEVEL_MEDIUM:
          myDataset.m_session.m_bookmarkList[m_iCurSelBookmark].m_ePriority = Bookmark::MEDIUM;
          break;
        case ID_BOOKMARKS_LEVEL_LOW:
          myDataset.m_session.m_bookmarkList[m_iCurSelBookmark].m_ePriority = Bookmark::LOW;
          break;
        case ID_BOOKMARKS_GOTO:
          if (ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY)
          {
            ColonViatronixWnd::ToggleAutoflyMode();
          }
          else if (ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D)
          {
            ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
            AfxGetMainWnd()->PostMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
          }
          myDataset.m_session.m_bookmarkList.StopAtBookmark(m_iCurSelBookmark, myDataset);
          ViatronStudy::TurnOffAutoflight();
          InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
          break;

        default:
          // Some other command not handled by this view
          return ViatronChildView::OnCommand(wParam, lParam);
      } // endswitch(LOWORD(wParam))
    }
    else if (!m_idCurSelCadFinding.empty())
    {
      switch(LOWORD(wParam))
      {
        case ID_SHOWCAD_UNDECIDED:
          Utilities::CAD::SetDisplayDecision( GetDatasetRef(), m_idCurSelCadFinding, ColonSession::FindingInfo::eDecisionUndecided );
          break;
        case ID_SHOWCAD_SHOW:
          Utilities::CAD::SetDisplayDecision( GetDatasetRef(), m_idCurSelCadFinding, ColonSession::FindingInfo::eDecisionAccepted );
          break;
        case ID_SHOWCAD_HIDE:
          Utilities::CAD::SetDisplayDecision( GetDatasetRef(), m_idCurSelCadFinding, ColonSession::FindingInfo::eDecisionDenied );
          break;
        case ID_BOOKMARKS_GOTO:
        {
          // jump to cad finding
          Utilities::CAD::JumpTo( GetDatasetRef(), m_idCurSelCadFinding );
          break;
        }
        default:
          break;
      } // switch(LOWORD(wParam))
    } // !m_idCurSelCadFinding.isempty
    else if (m_iCurSelRegistrationPoint >= 0)
    {
      switch (LOWORD(wParam))
      {
      case ID_USERREGISTRATION_DELETE:
        {
          std::vector<uint4>::iterator iterPrimary( myDataset.m_session.m_vUserFeatures.begin() );
          std::advance( iterPrimary, m_iCurSelRegistrationPoint );
          myDataset.m_session.m_vUserFeatures.erase( iterPrimary );
          std::vector<uint4>::iterator iterOpposite( GetOppositeDatasetRef().m_session.m_vUserFeatures.begin() );
          std::advance( iterOpposite, m_iCurSelRegistrationPoint );
          GetOppositeDatasetRef().m_session.m_vUserFeatures.erase( iterOpposite );
          m_iCurSelRegistrationPoint = -1;
          rdrGlobal.Register();
        }
        break;

      case ID_USERREGISTRATION_CLEARALLUSERFEATURES:
        myDataset.m_session.m_vUserFeatures.clear();
        GetOppositeDatasetRef().m_session.m_vUserFeatures.clear();
        m_iCurSelRegistrationPoint = -1;
        rdrGlobal.Register();
        break;

      default:
        // Some other command not handled by this view
        return ViatronChildView::OnCommand(wParam, lParam);
      } // endswitch(LOWORD(wParam))
    }
    else
    {
      ViewModes::VIEWMODES mode = ViewModes::VIEWMODES(0);

      switch (LOWORD(wParam))
      {
        case ID_OVERVIEW_RESETDISPLAY:
          OnKeyDown('R',0,0);
          return TRUE;

        case IDM_BOOKMARKS_DELETEALLBOOKMARKS:
          ColonViatronixWnd::DeleteAllBookmarks(myDataset.GetOrientation());
          break;

        case ID_USERREGISTRATION_CLEARALLUSERFEATURES:
          myDataset.m_session.m_vUserFeatures.clear();
          GetOppositeDatasetRef().m_session.m_vUserFeatures.clear();
          rdrGlobal.Register();
          break;

        // Mode change
        case ID_OVERVIEW_OPTIONS_CENTERLINE:
          mode = ViewModes::CENTERLINE;
          break;
        case ID_OVERVIEW_OPTIONS_TRANSPARENT:
          mode = ViewModes::TRANSLUCENT;
          break;
        case ID_OVERVIEW_DISPLAYOPTIONS_MARKING:
          mode = ViewModes::MISSED_REGIONS;
          break;
        case ID_OVERVIEW_OPTIONS_BOOKMARK:
          mode = ViewModes::BOOK_MARK;
          break;
        case ID_OVERVIEW_SLICEINDICATORS_AXIAL:
          mode = ViewModes::AXIAL;
          break;
        case ID_OVERVIEW_SLICEINDICATORS_SAGITTAL:
          mode = ViewModes::SAGITTAL;
          break;
        case ID_OVERVIEW_SLICEINDICATORS_CORONAL:
          mode = ViewModes::CORONAL;
          break;
        case ID_OVERVIEW_SLICEINDICATORS_CROSSSECTIONAL:
          mode = ViewModes::CROSS_SECTION;
          break;
        case ID_OVERVIEW_SLICEINDICATORS_DISSECTION:
          mode = ViewModes::DISSECTIONAL;
          break;

        default:
          // Some other command not handled by this view
          return ViatronChildView::OnCommand(wParam, lParam);
      } // endswitch(LOWORD(wParam))

      ViatronStudy::m_modeOverView.Toggle(mode);
    } // endif (m_iCurSelBookmark)

//  ResetTimer();
    InvalidateViews( WPARAM(GLChildView::eGVT_OVERVIEW), 0);
    //ForceRedraw();
    GLRender();
    return TRUE;  // Command handled
  } // try
  catch ( ... )
  {
    LogErr( "unhandled exception", "OverView", "OnCommand" );
  }
  return TRUE;
} // OnCommand()


/**
 * invalidates this view
 */
void OverView::InvalidateView()
{
  ResetTimer();
  ForceRedraw();
} // InvalidateView()


/**
 * handler responses to left mouse button release.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnLButtonUp(UINT uFlags, CPoint point)
{
  m_iCurSelRegistrationPoint = -1;
  ReleaseCapture();
  ResetTimer(8);
  if ( Utilities::Datasets::IsLocked() && (m_pNeighbourView != NULL))
  {
    m_pNeighbourView->ResetTimer(8);
  }

  if (m_bUserFeaturesChanged)
  {
    rdrGlobal.Register();
    m_bUserFeaturesChanged = false;
  }
} // OnLButtonUp()


/**
 * handler responses to right mouse button press.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnRButtonDown(UINT uFlags, CPoint point)
{
  m_lastClickPoint = point;
  ResetTimer();

  if ( IsMode(OverviewRender::eRegistrationVerify) )
  {
    m_iCurSelRegistrationPoint = GetRegistrationPointHit(point);
  }
  else if ( !IsMode(OverviewRender::eSegmentPicking) )
  {
    // TODO: Allow user to select from list of bookmarks
    std::list<uint4> matchList(GetBookmarksHit(point, GetDatasetRef().m_session.m_bookmarkList));
    m_iCurSelBookmark = (!matchList.empty()) ? matchList.front() : -1;

    list<string> idList = GetCadFindingHit(point);
    m_idCurSelCadFinding = !idList.empty() ? idList.front() : string();
  }
  else if( IsMode(OverviewRender::eSegmentPicking))
  { 
    OnLButtonDblClk( MK_LBUTTON, point );
  }
  
  if( !IsMode( OverviewRender::eSegmentPicking ) && !IsMode( OverviewRender::eSegmentVerify ) )
  {
    ViatronChildView::OnRButtonDown( uFlags, point );
  }
} // OnRButtonDown()


/**
 * handler responses to right mouse button release.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnRButtonUp(UINT uFlags, CPoint point)
{
  m_iCurSelBookmark = -1;
  m_iCurSelRegistrationPoint = -1;
  m_idCurSelCadFinding.clear();
} // OnRButtonUp()


/**
 * handler responses to middle mouse button press.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnMButtonDown(UINT uFlags, CPoint point)
{
  ResetTimer();
#ifndef FINAL_RELEASE
  if ((uFlags & MK_CONTROL) && (uFlags & MK_SHIFT))
  {
    CColorDialog clrdlg;
    COLORREF vColors[16];
    for (int4 i=0; i<16; i++)
    {
      vColors[i] = RGB(PickSegtColors[i].m_x, PickSegtColors[i].m_y, PickSegtColors[i].m_z);
    }
    clrdlg.m_cc.lpCustColors = vColors;
    clrdlg.DoModal();
    for (int4 i=0; i<16; i++)
    {
      PickSegtColors[i] = Triple<uint1>(GetRValue(vColors[i]),GetGValue(vColors[i]),GetBValue(vColors[i]));
    }
    return;
  } // endif (flags)
#endif

  SetCapture();
  GetDatasetRef().m_pTrackball->StartZoom(point.y);
  if( Utilities::Datasets::IsLocked() )
  {
    GetOppositeDatasetRef().m_pTrackball->StartZoom(point.y);
    if (m_pNeighbourView != NULL)
    {
      m_pNeighbourView->ResetTimer();
    }
  }
} // OnMButtonDown()


/**
 * handler responses to middle mouse button release.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnMButtonUp(UINT uFlags, CPoint point)
{
  ReleaseCapture();
  ResetTimer(8);
  if ( Utilities::Datasets::IsLocked() && (m_pNeighbourView != NULL))
  {
    m_pNeighbourView->ResetTimer(8);
  }
} // OnMButtonUp()


/**
 * handler responses to mouse movement.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    specifies the x- and y-coordinate of the cursor.
 */
void OverView::OnMouseMove(UINT uFlags, CPoint point)
{
  Dataset& myDataset = GetDatasetRef();
  if (!myDataset.m_segmentList.IsLoaded())
  {
    ViatronChildView::OnMouseMove(uFlags, point);
    return;
  }
  Dataset& otherDataset = GetOppositeDatasetRef();

  if (!(uFlags&MK_LBUTTON) && IsMode(OverviewRender::eRegistrationVerify))
  {
    m_eMouseOverFeature = (m_iCurSelRegistrationPoint >= 0) || (GetRegistrationPointHit(point) >= 0) ? OverviewFeature::eUserFeature : OverviewFeature::eNone;
  }

  bool bUpdate = false;
  if (uFlags&MK_LBUTTON && !(uFlags&MK_CONTROL))
  {
    int4 iIndexNode(-1);
    if (IsMode(OverviewRender::eRegistrationVerify) && m_iCurSelRegistrationPoint >= 0 && m_iCurSelRegistrationPoint < myDataset.m_session.m_vUserFeatures.size())
    {
      uint4 uMatchedIndexNode = GetNearestJoinedSkeletonNode(point,myDataset.m_joinedSkeleton);
      // Force the selected point to stay betweenthe surrounding user points
      // (e.g. "crossing" user features is BAD) and that it does not get "too close" to another node.
      if (m_iCurSelRegistrationPoint > 0)
      {
        if (uMatchedIndexNode < myDataset.m_session.m_vUserFeatures[m_iCurSelRegistrationPoint-1]+5) uMatchedIndexNode = myDataset.m_session.m_vUserFeatures[m_iCurSelRegistrationPoint-1]+5;
      }
      if (m_iCurSelRegistrationPoint < myDataset.m_session.m_vUserFeatures.size()-1)
      {
        if (uMatchedIndexNode > myDataset.m_session.m_vUserFeatures[m_iCurSelRegistrationPoint+1]-5) uMatchedIndexNode = myDataset.m_session.m_vUserFeatures[m_iCurSelRegistrationPoint+1]-5;
      }
      myDataset.m_session.m_vUserFeatures[m_iCurSelRegistrationPoint] = uMatchedIndexNode;
      m_bUserFeaturesChanged = true;
    }
    else if (IsCaptured())
    {
      // rotate
      Matrix<float4> mAccMat;
      TrackBall * pTrackball = myDataset.m_pTrackball;
      pTrackball->SetRotateCurrentPoint(Point2D<int4>(point.x, point.y));
      pTrackball->InitCalc(m_uViewWth, m_uViewHgt);
      pTrackball->SetRotateStartPoint(Point2D<int4>(point.x, point.y));
      pTrackball->AddQuats();
      pTrackball->BuildRotMatrix(mAccMat);
      pTrackball->SetCurrentMatrix(mAccMat * pTrackball->GetCurrentMatrix());
      pTrackball->PartialReset();
      m_bUpdateModelViewMatrix = true;

      // if current status is 'lock', the current rotation matrix should be passed to the coupled view.
      if (Utilities::Datasets::IsLocked())
      {
        otherDataset.m_pTrackball->SetCurrentMatrix(mAccMat * otherDataset.m_pTrackball->GetCurrentMatrix());
        if (m_pNeighbourView != NULL)
        {
          m_pNeighbourView->m_bUpdateModelViewMatrix = true;
          m_pNeighbourView->ResetTimer();
          //m_pNeighbourView->RedrawWindow();
        }
      }
    }
    bUpdate = true;
    ResetTimer();
  } // end-if (rotate)
  else if (uFlags&MK_MBUTTON && !(uFlags&MK_CONTROL) )
  {
    // scale
    myDataset.m_pTrackball->SetZoom(point.y);
    myDataset.m_pTrackball->StartZoom(point.y);
    m_bUpdateProjMatrix = true;
    ResetTimer();
    if (Utilities::Datasets::IsLocked())
    {
      otherDataset.m_pTrackball->SetZoom(point.y);
      otherDataset.m_pTrackball->StartZoom(point.y);
      if (m_pNeighbourView != NULL)
      {
        m_pNeighbourView->m_bUpdateProjMatrix = true;
        m_pNeighbourView->ResetTimer();
      }
    }
    bUpdate = true;
  } // end-if (scale)
  else if (!uFlags && IsMode(OverviewRender::eRegistrationVerify))
  {
    m_overviewRender.m_uRegistrationVerificationPreviewIndex = GetNearestJoinedSkeletonNode(point,myDataset.m_joinedSkeleton);
    ResetTimer();
    if (m_pNeighbourView != NULL)
    {
      m_pNeighbourView->m_overviewRender.m_uRegistrationVerificationPreviewIndex = myDataset.m_joinedSkeleton.GetRegistrationOtherIndex(m_overviewRender.m_uRegistrationVerificationPreviewIndex);
      m_pNeighbourView->ResetTimer();
    }
    bUpdate = true;
  }



  // Note: Always pass FALSE to Invalidate() so that the framework
  //        doesn't automatically erase the window area (which slows
  //        window updates, since we're redrawing the window anyway)
  if (bUpdate)
  {
///    Invalidate(FALSE);  // Invalidates the entire client area
   // UpdateWindow();
  }

  ViatronChildView::OnMouseMove(uFlags, point);
} // OnMouseMove()


/**
 * Force the view to redraw
 */
void OverView::ForceRedraw()
{
  ResetTimer();
  ///Invalidate(false);
 // UpdateWindow();
} // ForceRedraw()


/**
 * handler responses to mouse wheel action.
 *
 * @param uFlags   Indicates whether various virtual keys are down.
 * @param point    specifies the x- and y-coordinate of the cursor.
 * @return         nonzero(TRUE) if it processes this message; otherwise FALSE.
 */
BOOL OverView::OnMouseWheel(UINT uFlags, int2 iDeltaZ, CPoint point)
{
  GetDatasetRef().m_pTrackball->MouseWheelZoom(iDeltaZ / WHEEL_DELTA);
  m_bUpdateProjMatrix = true;
  SetImageBuffered( false );

  if ( Utilities::Datasets::IsLocked() )
  {
    GetOppositeDatasetRef().m_pTrackball->MouseWheelZoom(iDeltaZ / WHEEL_DELTA);
    if (m_pNeighbourView != NULL)
    {
      m_pNeighbourView->m_bUpdateProjMatrix = true;
      m_pNeighbourView->ResetTimer(5);
    }
  } // endif (Utilities::Datasets::IsLocked())

  ResetTimer(5);
///  Invalidate(FALSE);
  return true;
} // OnMouseWheel()


/**
 * handler responses to keyboard press.
 *
 * @param uChar     specifies the virtual key code of the given key.
 * @param uRepCnt   specifies the repeat count.
 * @param uFlags    specifies the scan code, key-transition code, previous key state, and context code.
 */
void OverView::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  Dataset& myDataset = GetDatasetRef();
  if (!myDataset.m_segmentList.IsLoaded())
  {
    GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
    return;
  }

  switch (uChar)
  {
#ifndef FINAL_RELEASE
    case '8':
        rdrGlobal.m_bAllFeatures = !(rdrGlobal.m_bAllFeatures);
        rdrGlobal.m_bMatchedFeatures = false;
        break;
    case '9':
        rdrGlobal.m_bAllFeatures = false;
        rdrGlobal.m_bMatchedFeatures = !(rdrGlobal.m_bMatchedFeatures);
        break;
    case 'N':
        if(GetDatasetRef().m_iFeatureToDisplayInfo<GetDatasetRef().m_ivFeatures.size()-1) GetDatasetRef().m_iFeatureToDisplayInfo++;
        break;
    case 'B':
        if(GetDatasetRef().m_iFeatureToDisplayInfo>0) GetDatasetRef().m_iFeatureToDisplayInfo--;
        break;
#endif
    case VK_SHIFT:
      // trigger refresh for segment picking preview
      RefreshImage();
      break;
#ifndef FINAL_RELEASE
    case 'S':
        // toggle simplified measurements display
        m_bRenderSimplifiedSegments = !m_bRenderSimplifiedSegments;
        break;
#endif
    case 'R':
        // reset rendering view, not parameters.
        myDataset.m_pTrackball->Init(myDataset.MatchOrientation(myDataset.GetOrientation()));
        myDataset.m_pTrackball->ResetZoomFactor();

        m_bUpdateProjMatrix = true;
        m_bUpdateModelViewMatrix = true;

        if ( Utilities::Datasets::IsLocked() )
        {
          Dataset& otherDataset = GetOppositeDatasetRef();
          otherDataset.m_pTrackball->Init(otherDataset.MatchOrientation(otherDataset.GetOrientation()));
          otherDataset.m_pTrackball->ResetZoomFactor();
          OverView * pNeighbourView = GetNeighbourView();
          if ( pNeighbourView != NULL )
          {
            pNeighbourView->m_bUpdateProjMatrix = true;
            pNeighbourView->m_bUpdateModelViewMatrix = true;
            pNeighbourView->ResetTimer(5);
          }
        } // endif(m_bSPLocked)
        break;
    case 'A':
        // reset display as air lumen volume.
        ApplicationSettings::UserSettings().SetBool( APP_SETTING_VOLUME_MEASURE_AIR_ONLY, true );
        break;
    case 'T':
        // reset display as total lumen volume.
        ApplicationSettings::UserSettings().SetBool( APP_SETTING_VOLUME_MEASURE_AIR_ONLY, false );
        break;

    default:
      GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
      return;
  } // endswitch(uChar)
  SetImageBuffered( false );
  ResetTimer(5);
  OverView * pNeighbourView = GetNeighbourView();
  if ( pNeighbourView != NULL )
  {
    pNeighbourView->ResetTimer(5);
  }
///  Invalidate(FALSE);
  GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown()


/**
 * handler to message ON_WM_SETCURSOR.
 *
 * @param pWnd       specifies a pointer to the window that contains the cursor.
 * @param uHitTest   specifies the hit-test area code.
 * @param message    specifies the mouse message number.
 * @return           nonzero to halt further processing, or 0 to continue.
 */
BOOL OverView::OnSetCursor(CWnd *pWnd, UINT uHitTest, UINT uMessage)
{
  if (m_eMouseOverFeature != OverviewFeature::eNone)
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }
  else
  {
    SetCursor(AfxGetApp()->LoadCursor(IDC_INDEXHAND));
  }

  return TRUE;
} // OnSetCursor()


/**
 * Returns a snapshot of the view
 *
 * @param iWidth    Width of the update buffer to return
 * @param iHeight   Height of the buffer to return
 * @return          Snapshot buffer of the current view
 */
const void far* OverView::GetUpdateBuffer(int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat, const GLenum eDatatype, const bool bOnlyGLRenderScene)
{
  VtxViewModes ovMode = ViatronStudy::m_modeOverView;

  // ensure use of the unbuffered, high-quality, overview image.
  ResetTimer(10);
  LPBYTE pByte = LPBYTE(GLChildView::GetUpdateBuffer(iWidth, iHeight, iWidthAlignedDWORD, ePixelFormat, eDatatype, true));
  ViatronStudy::m_modeOverView = ovMode;

  return pByte;
} // GetUpdateBuffer()


/**
 * call this function to grasp a overview image from VerificationView
 *
 * @param iWidth           Width of update buffer.
 * @param iHeight          Height of update buffer.
 * @param eSaveImageMode   Save mode (specifying Verify or Standard view)
 */
void OverView::SetToSaveReportImage(int4 & iWidth, int4 & iHeight, const SaveReportImageEnum eSaveImageMode)
{
  CRect rc( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
  iWidth = (rc.Width()+3) & ~3;
  iHeight =  rc.Height();
  m_eSaveReportImage = eSaveImageMode;
} // SetToSaveReportImage()


/**
 * Adds non-distended areas to the report.
 *
 * @param dataset       Dataset for the non-distended areas
 * @param spFindings    Report findings (to which to add areas)
 * @param bInitialAdd   Whether or not this call is initializing non-distended areas (on study load)
 * @return bool         Success/failure.
 */
bool OverView::AddNonDistendedAreas(Dataset& dataset, ReportLib::IReportNonDistendedFindingsPtr spFindings, const bool bInitialAdd)
{
  try
  {
    if (!dataset.IsLoadingComplete())
    {
      return TRUE;
    }

    SegmentList segments = dataset.m_segmentList;
    const UINT uNumSegments(segments.GetNumSelectedSegments());

    for (UINT uSegment(1); uSegment <= (uNumSegments-1); uSegment++)
    {
      Segment& segPrev = segments.GetSegment(uSegment-1);
      Segment& segNext = segments.GetSegment(uSegment);
      int4 iWidth(0), iHeight(0), iWidthAlignedDWORD( 0 );
      Point<float> mp = (segPrev.GetEndPointMM() + Vector<float>(segNext.GetStartPointMM()) ) / 2;
      Normal<float> dir = Normal<float>(segPrev.GetEndPointMM() - segNext.GetStartPointMM());

      // set up arrow position and direction.
      SetReportArrow(mp, dir);
      if (bInitialAdd)
      {
        SetToSaveReportImage(iWidth, iHeight, eSaveVerification);
      }

      // always render arrow in NonIndustendedArea image.
      SetRenderReportArrow(true);
      // set up start point and end point of report connection.
      SetReportSegment(segPrev.GetEndPointMM(), segNext.GetStartPointMM());

      LPBYTE pBuffer(0);
      pBuffer = LPBYTE(GetUpdateBuffer(iWidth, iHeight, iWidthAlignedDWORD, GL_BGR_EXT, GL_UNSIGNED_BYTE, true ) );

      if (pBuffer != NULL)
      {
        const float fStartMM(segPrev.GetOffsetDistanceFromRectumMM() + segPrev.GetEuclideanLengthMM());
        const float fEndMM(segNext.GetOffsetDistanceFromRectumMM());
        CString s;
        s.Format("%.1f %s - %.1f %s", fStartMM / 10, LoadResourceString(IDS_CM), fEndMM / 10, LoadResourceString(IDS_CM));

        ReportLib::IReportNonDistendedFindingPtr spFinding = spFindings->Add();

        spFinding->Text = static_cast<LPCTSTR>(s);
        spFinding->ZoomFactor = dataset.m_trackballStandard.GetZoomFactor();

        extern ReportLib::DatasetPositionEnumProp DicomPatientOrientationToDatasetPosition(const SerializedDicom::PatientOrientation eOrient);
        spFinding->_DatasetPositionEnumProp = DicomPatientOrientationToDatasetPosition(dataset.GetPatientOrientation());

        spFinding->Offset = (fStartMM + fEndMM) / 2.0f;
        spFinding->Location = VxVCReport::MapColonSegment(dataset.GetColonSegmentFromOffset(spFinding->GetOffset()));
        spFinding->RotationMatrix = ReportVariant(dataset.m_trackballStandard.GetCurrentMatrix());
        spFinding->Quaternion = ReportVariant(dataset.m_trackballStandard.GetCurrentQuat());
        spFinding->TransferX  = dataset.m_trackballStandard.GetTransferX();
        spFinding->TransferY  = dataset.m_trackballStandard.GetTransferY();
        spFinding->ViewMode   = ViatronStudy::m_modeOverView;
        spFinding->StartPoint = ReportVariant(segPrev.GetEndPointMM());
        spFinding->EndPoint   = ReportVariant(segNext.GetStartPointMM());
        spFinding->Position   = ReportVariant(mp);
        spFinding->Direction  = ReportVariant(dir);

        {
          COleSafeArray sa;
					DWORD dwElements = iWidthAlignedDWORD*iHeight;
          sa.CreateOneDim(VT_UI1, dwElements, pBuffer);
          ReportLib::IImagePtr spImage(__uuidof(ReportLib::Image));
          spImage->Create(iWidth, iHeight, 24);
          spImage->SetPixels(sa);
          
					if (rdrGlobal.m_bNormalizeSnapshotDim)
            spImage->Resize( 512, 512 );

          spFinding->Image = spImage;
        }
      }
    }
  }
  catch(_com_error& e)
  {
    ReportWndBase::DisplayReportError(IDS_ADD_NONDISTENDED_AREA, e.Error());
    return FALSE;
  }

  return TRUE;
} // AddNonDistendedAreas()


/**
 * whether or not to display a reverse-video image
 *
 * @param bInverseImage   true for reverse video, false for normal image
 */
void OverView::SetInverseImage(const bool bInverseImage ) 
{ 
  ViatronChildView::SetInverseImage( bInverseImage );
  ForceRedraw();
} // SetInverseImage()


#undef FILE_REVISION

// Revision History:
// $Log: OverView.C,v $
// Revision 1.44.2.16  2010/11/16 16:09:05  dongqing
// remove #include "Timer.h"
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.44.2.15  2010/11/16 15:36:10  dongqing
// change the wording for air volume measurement of segment
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.44.2.14  2010/11/15 20:38:12  dongqing
// add codes for measuring air lumen volume of selected segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.44.2.13  2010/06/14 20:18:44  dongqing
// Render translucent overview at high resolution unless it is the main view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.44.2.12  2010/03/20 21:13:12  dongqing
// added menu item for dissection plane display in overview
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.44.2.11  2010/03/10 00:55:17  dongqing
// don't create font for inline snapshot drawing (will crash)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.44.2.10  2010/03/09 20:13:36  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.44.2.9  2009/11/24 20:04:41  dongqing
// reset the high resolution overview transparency display
//
// Revision 1.44.2.8  2009/10/07 16:13:14  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.44.2.7  2009/01/12 21:58:38  jmeade
// set pick buffer to be the exact size of the window, eliminating buffer overuns for large displays.
//
// Revision 1.44.2.6  2009/01/10 07:25:24  jmeade
// comments
//
// Revision 1.44.2.5  2008/11/13 02:17:18  jmeade
// fix bug where go-to-bookmark functionality failed when prone loaded as 1st dataset.
//
// Revision 1.44.2.4  2008/09/25 00:32:41  jmeade
// Issue 6154: use high-quality translucent image for snapshot/preview.
//
// Revision 1.44.2.3  2008/01/31 23:21:22  dongqing
// fix the series toggling bug. When toggling series, transparency overview does not
// get updated.
//
// Revision 1.44.2.2  2008/01/22 19:58:58  geconomos
// Changed order of translucent rendering.  Image generation is done first.
//
// Revision 1.44.2.1  2007/06/27 01:33:52  jmeade
// comment.
//
// Revision 1.44  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.43  2007/03/10 06:25:54  jmeade
// code standards.
//
// Revision 1.42  2007/03/08 22:22:12  romy
// code review - header files adjusted
//
// Revision 1.41  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.40  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.39  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.38  2007/02/02 21:46:57  romy
// Utilizes the IsRayCastVolume2InitialRun flag to show the wait cursor
//
// Revision 1.37  2007/02/02 21:10:31  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.36  2007/01/09 19:17:59  jmeade
// Issue 5197: don't calculate distance-to-rectum when current location outside of lumen.
//
// Revision 1.35  2006/10/26 21:05:17  frank
// fixed transparency of logo when displaying update buffer
//
// Revision 1.34  2006/10/03 17:16:21  jmeade
// variable names.
//
// Revision 1.33  2006/09/26 20:42:41  jmeade
// Issue 4946: display of decubitus patient positions in report.
//
// Revision 1.32  2006/09/01 18:14:42  jmeade
// Issue 4946: text in non-distended regions.
//
// Revision 1.31  2006/08/24 01:29:41  jmeade
// Issue 4946: strings for decubitus studies.
//
// Revision 1.30  2006/07/06 15:59:55  jmeade
// Read patient position from volume header.
//
// Revision 1.29  2006/06/28 18:21:43  frank
// used the new rendering engine for multi-threading
//
// Revision 1.28  2006/06/01 20:22:08  frank
// updated to visual studio 2005
//
// Revision 1.27  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.26  2006/02/09 01:13:42  jmeade
// code review: comments.
//
// Revision 1.25  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.24  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.23  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.22.2.10  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.22.2.9.2.1  2007/01/18 22:05:39  jmeade
// display cad findings only when appropriate.
//
// Revision 1.22.2.9  2006/03/28 20:22:14  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.22.2.8  2006/03/22 22:52:44  jmeade
// Issue 4667: restrict registration synchronization to views where both datasets are visible, and a link button is available.
//
// Revision 1.22.2.7  2006/03/22 20:58:07  frank
// moved the finding visibility and other information into a class
//
// Revision 1.22.2.6  2006/02/22 20:11:46  geconomos
// Issue #4604:  Overview snapshot not working while flying
//
// Revision 1.22.2.5  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.22.2.4  2005/11/17 20:08:31  jmeade
// Issue 4545: Check visibility of cad findings before doing hit-check.
//
// Revision 1.22.2.3  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.22.2.2  2005/11/15 14:51:02  frank
// Issue #4514: Fixed crash jumping to a finding not in the current dataset
//
// Revision 1.22.2.1  2005/11/04 02:45:03  jmeade
// Issue 4520: turn off transparent and marking during segment-picked-preview.
//
// Revision 1.22  2005/11/02 18:43:15  frank
// Issue #4461: Fixed some problems with registration of points near the end.
//
// Revision 1.21  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.20  2005/10/05 22:48:37  jmeade
// update cad findings display after changes.
//
// Revision 1.19  2005/10/04 18:08:48  jmeade
// update cad list after changes.
// no separate distance-to-rectum text field, text only in overview.
//
// Revision 1.18  2005/09/29 18:32:21  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.17  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.16  2005/09/28 00:35:49  jmeade
// context menu functionality in overview.
//
// Revision 1.15  2005/09/27 01:09:55  jmeade
// context menu for cad findings.
//
// Revision 1.14  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.13  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.12  2005/09/23 03:19:04  vxconfig
// hid cad findings on verification view
//
// Revision 1.11  2005/09/22 16:56:58  jmeade
// render cad findings in overview.
//
// Revision 1.10  2005/09/13 17:21:07  jmeade
// ColonReport namespace.
//
// Revision 1.9.2.2  2005/09/21 13:27:15  geconomos
// modified code to make non-window calls during rendering
//
// Revision 1.9.2.1  2005/09/14 17:48:39  geconomos
// issue# 4434: Crash taking snapshot
//
// Revision 1.9  2005/09/13 13:06:21  geconomos
// Making calls to ViatronStudy::StopAutoFlight()
//
// Revision 1.8  2005/09/09 14:31:21  geconomos
// Added a governor that regulates the fps while flying in 3d.
//
// Revision 1.7  2005/08/30 17:34:29  geconomos
// Issue#4 391: Removed registration context menu items
//
// Revision 1.6  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.5  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.4  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.3  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.81.2.20.2.28.2.10  2005/07/22 19:00:40  geconomos
// Issue #4299: Fixed out of bounds problem with registration when user features are near the end.
//
// Revision 3.81.2.20.2.28.2.9  2005/07/19 12:37:54  geconomos
// Issue #4289: fixed update mechanism
//
// Revision 3.81.2.20.2.28.2.8  2005/07/14 19:01:55  geconomos
// Issue # 4277: Registration features numbers flicker
//
// Revision 3.81.2.20.2.28.2.7  2005/07/14 14:16:31  geconomos
// Refactored logo renering code
//
// Revision 3.81.2.20.2.28.2.6  2005/07/06 19:46:17  geconomos
// forced a redraw when turning on/off slice indicators
//
// Revision 3.81.2.20.2.28.2.5  2005/07/06 18:00:51  geconomos
// Renamed Invalidate to invalidateView ( clashing with CWnd::Invalidate() )
//
// Revision 3.81.2.20.2.28.2.4  2005/06/27 17:39:10  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.81.2.20.2.28.2.3  2005/06/24 12:45:45  geconomos
// Issue#: 4220 Overview image staying inverted after snapshot
//
// Revision 3.81.2.20.2.28.2.2  2005/06/21 15:26:31  geconomos
// Corrected revision history
//
// Revision 3.34.2.2.4.8.2.1  2005/06/21 15:15:46  geconomos
// Broke out DrawOverView class into it's own .h and .C
//
// Revision 3.34.2.2.4.8  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.34.2.2.4.5.2.2  2005/06/08 14:01:10  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.34.2.2.4.5.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.34.2.2.4.5  2005/04/19 17:42:19  geconomos
// separated OpenGL and Windows resize functions
//
// Revision 3.34.2.2.4.4  2005/04/19 13:40:56  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.34.2.2.4.3  2005/04/05 14:46:51  frank
// Issue #4055: added ability to disable slice indicators on overview
// Issue $04065: drawing blank image if no data loaded
//
// Revision 3.34.2.2.4.2  2005/03/31 15:36:06  frank
// Issue #4077: Fixed update of verify view when button is pressed.
//
// Revision 3.34.2.2.4.5  2005/04/19 17:42:19  geconomos
// separated OpenGL and Windows resize functions
//
// Revision 3.34.2.2.4.4  2005/04/19 13:40:56  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.34.2.2.4.3  2005/04/05 14:46:51  frank
// Issue #4055: added ability to disable slice indicators on overview
// Issue $04065: drawing blank image if no data loaded
//
// Revision 3.34.2.2.4.2  2005/03/31 15:36:06  frank
// Issue #4077: Fixed update of verify view when button is pressed.
//
// Revision 3.34.2.2.4.1  2005/03/26 01:52:01  frank
// Issue #4029: Udated slice indicators on all views
//
// Revision 3.34.2.2  2003/02/27 02:22:55  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.34.2.1  2003/02/21 18:10:50  jmeade
// Fixed incorrect segment hit calculation when an overlapping segment is not selected.
//
// Revision 3.34  2003/02/05 23:50:09  jmeade
// Use numbered points instead of colors to differentiate user reg'n points within a dataset (and associate linked points between datasets)..
// Coding standards (const, variable naming).
//
// Revision 3.33  2003/01/29 00:43:28  jmeade
// Distance from rectum text display determined by separate flag (an i.e. a parent window), not by IsMainWindow();
// Code standard: All member vars private unless otherwise necessary.
//
// Revision 3.32  2003/01/03 23:07:18  jmeade
// Draw overview class.
//
// Revision 3.31  2002/11/22 03:11:18  jmeade
// IsMode() - more concise method name.
//
// Revision 3.30  2002/11/22 00:32:04  jmeade
// OverView::Initialize convenience function.
//
// Revision 3.29  2002/11/11 16:19:51  kevin
// Render the created "Straight segmetns"
//
// Revision 3.28  2002/10/31 19:56:00  kevin
// Redfactor Selecting of segments, jump-to-points etc since this got brooken
//
// Revision 3.27  2002/09/11 19:15:02  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.26  2002/09/10 20:50:54  jmeade
// Oops:  Each OverviewRender instance needs its own render mode variable.
//
// Revision 3.25  2002/09/10 19:07:40  jmeade
// One render mode for all OverviewRender instances; Code standards; Name clarifications.
//
// Revision 3.24  2002/09/10 15:47:10  jmeade
// Removed unused message.
//
// Revision 3.23  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.21  2002/07/18 20:25:38  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.20  2002/07/18 03:17:59  jmeade
// Correct picking of segment and node index for user registration points; Renamed ambiguous functions.
//
// Revision 3.19  2002/07/11 23:25:19  jmeade
// Implementing user-defined registration point specification.
//
// Revision 3.18  2002/07/09 20:13:26  jmeade
// Code standards.
//
// Revision 3.17  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.16  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.15  2002/04/12 02:24:03  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.14  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.13  2002/03/05 15:35:50  ana
// Using feature vectors.
//
// Revision 3.12  2002/02/26 16:39:31  ana
// changed a variable to boolean as it should be, still doesn't work :(
//
// Revision 3.11  2002/02/21 20:11:20  ana
// Displaying metrics
//
// Revision 3.10  2002/02/20 19:33:43  ana
// Part of metrics are being displayed already
//
// Revision 3.9  2002/02/07 18:13:47  ana
// GLRasterScene displays feature info. To display feature info
// Vverview Menu -> Registration Options -> Show feature info
//
// Revision 3.8  2002/02/06 21:20:04  ana
// Added one more option to the menu.
//
// Revision 3.7  2002/02/06 20:33:58  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
// (message WM_APP_2DFLIGHT_UPDATEVIEW)
//
// Revision 3.6.2.5  2002/06/27 17:35:33  jmeade
// GetUpdateBuffer is inherently a public-use function.
//
// Revision 3.6.2.4  2002/06/17 18:31:47  jmeade
// Code standards.
//
// Revision 3.6.2.3  2002/06/12 02:21:33  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.6.2.2  2002/04/11 21:55:47  jmeade
// Code standards.
//
// Revision 3.6.2.1  2002/02/11 18:02:15  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
//
// Revision 3.6  2002/01/29 18:56:56  binli
// issue 1427: the image has already buffered. Adjusted the structure of control to make it worls.
//
// Revision 3.5  2002/01/03 02:23:35  kevin
// Cleanup related to new TTP work when ses, set and sec went away
//
// Revision 3.4  2001/12/22 00:08:57  jmeade
// Code standards.
//
// Revision 3.3  2001/10/24 16:16:33  jmeade
// Preliminary changes for identifying multiple bookmarks on selection
//
// Revision 3.2  2001/10/19 02:39:23  jmeade
// Code standards
//
// Revision 3.1  2001/10/17 22:02:33  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.4   Oct 08 2001 19:04:06   jmeade
// Code standards
// 
//    Rev 2.3   Oct 08 2001 17:03:46   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
// 
//    Rev 2.2   Oct 02 2001 16:50:00   binli
// report bug: grasp images
// 
//    Rev 2.1   Oct 01 2001 17:08:48   binli
// functions to ensure no bad/black images in report file: synchronous control
// 
//    Rev 2.0   Sep 16 2001 23:41:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:22   ingmar
// Initial revision.
// Revision 1.92  2001/08/22 01:19:15  jmeade
// Better encapsulation in OverView (ResetTimer has no meaning outside of class)
//
// Revision 1.91  2001/07/31 16:25:09  binli
// inline fucntion
//
// Revision 1.90  2001/07/30 22:40:59  binli
// bug fixed: update overview when change slices in StandardView/EndoAxialView.
//
// Revision 1.89  2001/06/27 14:09:10  binli
// bug 532: unnecessary redraw (rotate) of overview.
// (added  overriding functions: SetCapture(), ReleaseCapture(), IsCaptured())
//
// Revision 1.88  2001/06/26 22:43:41  binli
// removed unused variables: m_bInteracting/m_bInteractingOverView.
//
// Revision 1.87  2001/06/26 14:44:41  binli
// rewrote the control logic to rerender overview: made memory stable.
//
// Revision 1.86  2001/06/18 17:08:12  jmeade
// only need to set the study last rendered if it's changed
//
// Revision 1.85  2001/05/29 20:50:36  binli
// code conventions
//
// Revision 1.84  2001/05/15 18:55:43  jmeade
// Code conventions
//
// Revision 1.83  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.82  2001/05/11 17:36:29  binli
// removed duplicated OVModes in OverView class.
//
// Revision 1.81  2001/05/08 18:35:28  binli
// Trackrecord ID 000377: green marked or not in S&PView and VerifyView.
// (No need to change snapshot in Report)
//
// Revision 1.80  2001/05/04 15:33:42  binli
// display lists: triangle/framebox.
//
// Revision 1.79  2001/04/18 22:19:21  binli
// set up flag for ModelView matrix updating in overview.
//
// Revision 1.78  2001/04/17 16:23:31  binli
// the current modelview matrix is saved for efficiency.
//
// Revision 1.77  2001/04/17 15:46:21  binli
// code cleaned
//
// Revision 1.76  2001/04/16 20:42:48  binli
// fixed the bug for picking node on skeleton.
//
// Revision 1.75  2001/04/16 18:45:34  binli
// 1. the current ProjMatrix and ModelViewMatrix are saved for reuse in RenderCamera(),
//     MatchSelectedSegents(), MatchSkeleton(), MatchSkeletonToSegment().
// 2. removed unused variable: m_bImageCreated in Overview.
//
// Revision 1.74  2001/04/13 15:33:50  binli
// moved out the render function: RayCastVolume() to OverviewRender class.
//
// Revision 1.73  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.72  2001/04/12 22:27:15  binli
// moved out the render function: RenderSegNum() (formal RenderMarkInSkel()) to OverviewRender class.
//
// Revision 1.71  2001/04/12 21:13:30  binli
// moved out the render function: RenderCamera() to OverviewRender class.
//
// Revision 1.70  2001/04/12 14:51:27  binli
// moved out the render function: RenderBookMarks() to OverviewRender class.
//
// Revision 1.69  2001/04/11 21:52:32  binli
// moved out the render function: RenderReportConnections to OverviewRender Class.
//
// Revision 1.68  2001/04/11 21:03:25  binli
// moved out the render function: RenderConnections to OverviewRender Class.
//
// Revision 1.67  2001/04/11 20:47:13  binli
// moved out the render function: RenderSkel to OverviewRender Class.
//
// Revision 1.66  2001/04/11 16:26:43  binli
// moved RenderSlices to OverviewRender class.
//
// Revision 1.65  2001/04/11 11:59:24  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.64  2001/04/10 22:17:08  binli
// moved render functions "RenderTri" to the OverviewRender class.
//
// Revision 1.63  2001/04/09 16:07:37  binli
// moved out render functions to OverviewRender class.
//
// Revision 1.62  2001/04/06 16:28:08  binli
// Used Matrix  to fulfill matrix multi
//
// Revision 1.61  2001/04/05 16:28:33  binli
// enhanced
//
// Revision 1.60  2001/04/03 21:13:24  binli
// 1st code walkthrough
//
// Revision 1.59  2001/04/03 17:07:47  binli
// added comments.
//
// Revision 1.58  2001/04/02 20:08:41  binli
// reflected "Typedef.h"
//
// Revision 1.57  2001/03/27 15:12:29  binli
// reflected Typedef.h
//
// Revision 1.56  2001/03/21 21:46:45  liwei
// Obsolete functions removed
//
// Revision 1.55  2001/03/21 15:28:18  liwei
// Remove the RenderObject from VLIOverView. VolumePro is invoked by a virtual member function: RayCastVolume
//
// Revision 1.54  2001/03/19 18:16:02  binli
// code walkthrough according to new code standard:
//   1> inline functions moved to .inl
//   2> no public/pretect data member variables.
//
// Revision 1.53  2001/03/16 22:37:46  binli
// renamed some variables:  according to new code standard
//
// Revision 1.52  2001/03/16 20:21:48  binli
// code walkthrough
//
// Revision 1.51  2001/03/12 20:18:38  liwei
// VLIOverview modified to be compatible with changes in other files
//
// Revision 1.50  2001/03/07 16:03:02  liwei
// Added some comments
//
// Revision 1.49  2001/02/28 23:30:31  liwei
// Overview rerendered when buttons on VerifyView clicked
//
// Revision 1.48  2001/02/28 17:53:55  liwei
// Overviews in VerifyView and Supine/ProneViews are set to buffer rendered image.
// That is, call RenderUpdateBuffer() rather than RenderObjects whenever possible
//
// Revision 1.47  2001/02/26 16:49:22  liwei
// Frame timer moved from GLRasterScene to Class
//
// Revision 1.46  2001/02/23 20:11:31  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.45  2001/02/22 19:17:22  binli
// fixed 'jump' bug. (skeleton point)
//
// Revision 1.44  2001/02/22 01:39:38  jeff
// coding conventions; unnecessary global viewmodes added to overview;
//
// Revision 1.43  2001/02/21 20:33:42  jeff
// COverview constructor now takes a StudyOrientation as a parameter (Study parameters unused/obsolete)
//
// Revision 1.42  2001/02/19 17:18:07  jeff
// OnKeyPressed() is obsoleete, identical to OnKeyDown()
//
// Revision 1.41  2001/02/07 15:23:35  frank
// Rendered centerline in continuous color gradient.
//
// Revision 1.40  2001/02/05 18:22:58  geconomos
// Added methods to draw a given segment connection line.
//
// Revision 1.39  2001/02/05 14:31:46  geconomos
// Started to work on report arrow
//
// Revision 1.38  2001/02/05 13:30:43  frank
// Made a single global null study.
//
// Revision 1.37  2001/02/02 07:28:46  jeff
// Moving strings to resource table
//
// Revision 1.36  2001/01/26 20:00:17  binli
// defect 000188: default Overview position
//
// Revision 1.35  2001/01/26 19:10:25  geconomos
// Removed CONNECT_SEGS view mode and combined with CENTERLINE.
// Added DRAW_DISTANCE view mode to enable drawing dictance from rectum
//
// Revision 1.34  2001/01/24 19:23:13  liwei
// Software volume rendering for overview
//
// Revision 1.33  2001/01/24 17:47:37  frank
// Improved supine/prone selection.
//
// Revision 1.32  2001/01/23 17:44:43  jeff
// app message handler
//
// Revision 1.31  2001/01/23 17:38:35  frank
// Clarified member name.
//
// Revision 1.30  2001/01/19 21:07:10  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.29  2001/01/16 15:48:59  binli
// Verification: 1. draw skeleton in arrows when a segment is picked.
//                    2. mark (number) the picked skeleton.
//
// Revision 1.28  2001/01/12 23:01:19  binli
// added a ball to represents the middle of skeleton (used in vertification).
// transparently render other segments when picking (used in verification view).
//
// Revision 1.27  2000/12/20 19:17:04  jeff
// Default parameters for study and bBufferImage
// Changed order of parameters in constructor, on the notion that
//  m_bBufferImage is the most likely default-value parameter
//
// Revision 1.26  2000/12/12 23:55:24  jeff
// AdjustPopupMenu() now indicates which popup menu was specified
//
// Revision 1.25  2000/11/23 08:15:46  jeff
// no message
//
// Revision 1.24  2000/11/22 18:38:46  binli
// added function for display list delete.
//
// Revision 1.23  2000/11/22 17:14:59  binli
// fixed display list for strips rendering.
//
// Revision 1.22  2000/11/21 18:59:24  binli
// added frame display list
//
// Revision 1.21  2000/11/20 01:51:05  jeff
// GetMouseKeyStringIDs() -- allows each view to set strings for mouse button use key
// AdjustPopupMmenu() takes a menu reference instead of a pointer
//
// Revision 1.20  2000/11/17 21:27:27  binli
// adjusted the rendering order of colon and other stuffs in translucent mode.
//
// Revision 1.19  2000/11/17 05:55:37  liwei
// Bugs fixed for VolumePro
//
// Revision 1.18  2000/11/15 19:12:30  liwei
// Adding features to read transfer functions from file
//
// Revision 1.17  2000/11/14 23:51:48  liwei
// Add support for VolumePro
//
// Revision 1.16  2000/11/14 18:23:02  binli
// changed references to pointers (to m_study and m_secondary in object 'rdrGlobal')
//
// Revision 1.15  2000/11/13 19:27:37  binli
// 1.changed construction for Supine/Prone application
// 2. followd adjustment of lock/unlock controls in VerifyView.
//
// Revision 1.14  2000/11/10 22:44:36  binli
// worked on both Supine/Prone overview rendering. In global variable 'rdrGlobal',
// two local trackballs are added. Some changes are made in TrackBall and OverView.
// Followed the change in OverView, VerifyView, EndoVisView, StandardView are
// changed a little.
//
// Revision 1.13  2000/11/10 20:14:12  frank
// Added MatchSkeleton function
//
// Revision 1.12  2000/11/10 15:22:10  frank
// Added jumping to new segments in overview window
//
// Revision 1.11  2000/11/01 22:43:58  jeff
// Flags and point of the last mouse click passed into GetMenuPopupID()
// (for the child view to use to decide which popup menu to show)
//
// Revision 1.10  2000/10/25 18:01:04  jeff
// OnCommand() overridden to handle bookmark/view window popup
// override other necessary popup menu functions
// Note current bookmark clicked on right mouse click
//
// Revision 1.9  2000/10/13 16:55:10  kevin
// Changed the logic for when to grab a new overview image
//
// Revision 1.8  2000/10/12 21:10:51  kevin
// Added bufferImage flag to ovwerview constructor
//
// Revision 1.7  2000/10/12 02:53:30  kevin
// Added CONNECT_SEGS render mode, and code to do it.
// Moved distance from rectum text here. Updated to properly
// compute distance from rectum across segments.
//
// Revision 1.6  2000/10/09 23:44:16  kevin
// Added Guadric member (instead of creating and killing al the time)
// and cleaned u p rendering of objects
//
// Revision 1.5  2000/10/04 14:23:21  kevin
// Added Endpoint picking (so far simple for no
// branches case untill skel is redone)
//
// Also added rdrGlobal units and dim for fast
// reading for study load speed
//
// Revision 1.4  2000/09/29 23:00:18  kevin
// FIxed the picking problem
//
// Revision 1.3  2000/09/21 19:28:20  kevin
// Added Raster Scene
//
// Revision 1.2  2000/09/13 16:31:18  jeff
// Removing unnecessary references to obsolete CChildView class
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 30    9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 29    9/03/00 6:44p Kevin
// Added update of ovreview window without redrawing all the
// ploygons by grabbing a texture image. Then the position
// can be updated ont he display constantly. Also added
// double click for bookmarks
// 
// 28    8/29/00 4:13p Jeff
// Mode variables now held in ViatronStudy
// 
// 27    8/28/00 2:35p Antonio
// updated header and footer
// 
// *****************  Version 26  *****************
// User: Binli           Date:  8/23/00  Time: 11:36a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 25  *****************
// User: Binli           Date:  8/23/00  Time: 11:35a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 24  *****************
// User: Kevin           Date:  8/22/00  Time:  4:30p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added centerline toggling
// 
// *****************  Version 23  *****************
// User: Binli           Date:  8/22/00  Time: 11:44a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 22  *****************
// User: Binli           Date:  8/22/00  Time: 11:40a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 21  *****************
// User: Binli           Date:  8/22/00  Time: 11:22a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 20  *****************
// User: Binli           Date:  8/22/00  Time: 11:15a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 19  *****************
// User: Kevin           Date:  8/21/00  Time:  3:41p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added segment length checking
// 
// *****************  Version 18  *****************
// User: Kevin           Date:  8/19/00  Time: 12:37p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added drawing of 2D planes on overview
// 
// *****************  Version 17  *****************
// User: Binli           Date:  8/18/00  Time:  3:07p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 16  *****************
// User: Binli           Date:  8/18/00  Time:  2:18p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 15  *****************
// User: Binli           Date:  8/18/00  Time:  2:11p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Add comments
// 
// *****************  Version 14  *****************
// User: Binli           Date:  8/18/00  Time: 10:44a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 13  *****************
// User: Jeff            Date:  8/15/00  Time:  7:26p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Moved OverviewTrackBall (class and instance) to ReaderGlobal so that
// overview position/orientation can persist throughout application
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 12  *****************
// User: Frank           Date:  8/08/00  Time:  6:40p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Jeff: (don't worry Frank, you didn't make this change!)
// SwapBuffers() moved back to CChildView::OnPaint(), new function override
// PostGLRender() can do any device context (HDC) drawing to the window
// 
// *****************  Version 11  *****************
// User: Kevin           Date:  8/06/00  Time:  7:02p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added Picking of segments for order. To accomplish this, I
// pulled the guts out of some data structures and replaced them
// 
// *****************  Version 10  *****************
// User: Binli           Date:  8/02/00  Time: 10:47a
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 9  *****************
// User: Kevin           Date:  8/02/00  Time:  1:33a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Aligned centerline with polygons and cleaned up overview a little
// 
// *****************  Version 8  *****************
// User: Binli           Date:  8/01/00  Time: 10:56a
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 7  *****************
// User: Binli           Date:  7/28/00  Time:  1:02p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 6  *****************
// User: Binli           Date:  7/26/00  Time:  2:00p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 5  *****************
// User: Binli           Date:  7/26/00  Time: 10:48a
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 4  *****************
// User: Binli           Date:  7/26/00  Time:  9:29a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   
// 
// *****************  Version 3  *****************
// User: Binli           Date:  7/25/00  Time:  3:45p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/18/00  Time:  1:58p
// Branched at version 2
// Comment:
//   
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/12/00  Time:  9:14p
// Created OverView.h
// Comment:
//   
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/OverView.C,v 1.44.2.16 2010/11/16 16:09:05 dongqing Exp $
// $Id: OverView.C,v 1.44.2.16 2010/11/16 16:09:05 dongqing Exp $
