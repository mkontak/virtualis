// $Id: MovieView.C,v 1.25.2.10 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MovieView.C
// Owner(s): Jeff Meade jmeade@viatronix.com

#include "stdafx.h"


#include "NavigationView.h"
#include "BackgroundProcess.h"
#include "User.h"
#include "MovieView.h"
#include "VFrameManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonViews::Layouts;
using namespace Movies;
using namespace ReaderLib;
using namespace WinUtils;

#define FILE_REVISION "$Revision: 1.25.2.10 $"

const uint4 MovieLayout::m_uMidBorder(0);

/////////////////////////////////////////////////////////////////////////////
// MovieLayoutFrame

IMPLEMENT_DYNCREATE(MovieLayoutFrame, CMDIChildWnd)

/**
 * Constructor
 */
MovieLayoutFrame::MovieLayoutFrame() : ChildLayoutFrame(new MovieLayout)
{
} // MovieLayoutFrame constructor

/////////////////////////////////////////////////////////////////////////////
// MovieLayout

/**
 * Constructor
 */
MovieLayout::MovieLayout() : 
  ColonViatronixWnd(),
  m_pAVI( NULL ), 
  m_uTotalFrames( 0 ),
  m_uCurrentFrame( 0 )
{
  // wait handle for movie completion
  m_hMovieWait = CreateEvent( NULL, TRUE, FALSE, NULL );

  // render size
  m_iRenderSize = rdrGlobal.GetSettings()->GetIntValue(VCSettings::ViewerXML(), VCSettings::MovieRenderSize());

  // image size
  m_iImageSize =  rdrGlobal.GetSettings()->GetIntValue(VCSettings::ViewerXML(), VCSettings::MovieImageSize());

  // frame rate
  m_iFrameRate = rdrGlobal.GetSettings()->GetIntValue(VCSettings::ViewerXML(), VCSettings::MovieFrameRate());

  // milliseconds per frame
  m_uMillisecondsPerFrame = 1.0F / m_iFrameRate * 1000;
  
} // MovieLayout()


/**
 * Destructor
 */
MovieLayout::~MovieLayout()
{
  if( m_pAVI != NULL )
  {
    delete m_pAVI;
    m_pAVI = NULL;
  }

  if( m_hMovieWait )
  {
    SetEvent(m_hMovieWait);
    CloseHandle( m_hMovieWait );
    m_hMovieWait = NULL;
  }

  rdrGlobal.m_supine.m_fields.m_volumeRender.SetRenderSize( VolumeRender::GetMaxRenderSize() );
  rdrGlobal.m_supine.m_fields.m_volumeRender.SetImageMode( VolumeRender::ADAPTIVE );
  rdrGlobal.m_prone.m_fields.m_volumeRender.SetRenderSize( VolumeRender::GetMaxRenderSize() );
  rdrGlobal.m_prone.m_fields.m_volumeRender.SetImageMode( VolumeRender::ADAPTIVE );

} // ~MovieLayout()


/// MFC message map
BEGIN_MESSAGE_MAP(MovieLayout, ColonViatronixWnd)
  ON_COMMAND(IDM_BOOKMARKS_ADDBOOKMARK, OnDisableCommand)
  ON_COMMAND(IDM_REPORT_ADDSNAPSHOT, OnDisableCommand)
  ON_COMMAND(IDM_TOGGLE_SUPINE_PRONE, OnDisableCommand)

  ON_COMMAND(IDM_TOGGLE_AUTOFLY, OnDisableCommand)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE, OnDisableCommand)
  ON_COMMAND(ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING, OnDisableCommand)
  ON_COMMAND(ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY, OnDisableCommand)

  ON_COMMAND(IDM_TOGGLE_VIEW_DIRECTION, OnDisableCommand)
  ON_COMMAND(IDM_JUMP_PREVSEGMENT, OnDisableCommand)
  ON_COMMAND(IDM_JUMP_NEXTSEGMENT, OnDisableCommand)
  ON_COMMAND(IDM_NEXT_MISSED_REGION, OnDisableCommand)

  ON_COMMAND(IDM_SHOW_NEXT_VIEW, OnDisableCommand)
  ON_COMMAND(ID_REPORT_MARKCOMPLETE, OnDisableCommand)
END_MESSAGE_MAP()


/**
 * Create the child views for this window
 *
 * @param hWndThis   Handle to this window
 */
void MovieLayout::CreateChildViews(HWND hWndThis)
{
  // create child view
  ColonViatronixWnd::CreateChildViews( hWndThis );

  if( !BackgroundProcess::Run(WaitOnMovieCompletion, (void*) m_hMovieWait, "WaitOnMovieCompletion", HideParamsEnum(eBKHIDE_TABS | eBKHIDE_MENU), THREAD_PRIORITY_LOWEST) )
    LogErr("BackgroundProcess run failed.","MovieLayout","CreateChildViews");

  // initialize progress bar
  CProgressCtrl *pProgress = (CProgressCtrl*) GetDlgItem(IDC_MOVIEVIEW_PROGRESS);
  pProgress->SetRange(0, 10000);
  pProgress->SetPos(0);
  const float4 fProgressStep(10000.0F / m_movie.m_frames.size());
  pProgress->SetStep(int4(fProgressStep));

  if( m_movie.m_format.m_bIncludeOverview )
  {
    GLChildViewRef ovChildView = GetChildViewDlgItem(IDC_TEMPLATE_OVERVIEW);
    ovChildView.EnableWindow(FALSE);
  }

  // initialize navigation view parameters
  EndoluminalView& rNavView = (EndoluminalView&) GetChildViewDlgItem(IDC_TEMPLATE_ENDOVIEW);
  rNavView.EnableWindow(FALSE);
  rNavView.ShowWindow(m_movie.m_eType == GLChildView::eGVT_NAVIGATION);
  rNavView.ShowLumenCoverage(false);
  rdrGlobal.m_supine.m_fields.m_volumeRender.SetRenderSize( m_iRenderSize );
  rdrGlobal.m_supine.m_fields.m_volumeRender.SetImageMode( VolumeRender::FIXED );
  rdrGlobal.m_prone.m_fields.m_volumeRender.SetRenderSize( m_iRenderSize );
  rdrGlobal.m_prone.m_fields.m_volumeRender.SetImageMode( VolumeRender::FIXED );

  // initialize slice view parameters
  SliceChildView::m_eCoordDisplay = SliceChildView::CoordDisplay::eNone;
  SliceChildView& rSliceView = (SliceChildView&) GetChildViewDlgItem(IDC_TEMPLATE_SLICEVIEW);
  rSliceView.EnableWindow(FALSE);
  rSliceView.ShowHUValue(false);
  rSliceView.ShowWindow(m_movie.m_eType != GLChildView::eGVT_NAVIGATION);
  rSliceView.SetFullObliqueMode(m_movie.m_bFullOblique);
  switch( m_movie.m_eType )
  {
    case GLChildView::eGVT_AXIAL:
      rSliceView.SetOrientation(SliceOrientations::eAXIAL);
      break;
    case GLChildView::eGVT_SAGITTAL:
      rSliceView.SetOrientation(SliceOrientations::eSAGITTAL);
      break;
    case GLChildView::eGVT_CORONAL:
      rSliceView.SetOrientation(SliceOrientations::eCORONAL);
      break;
    case GLChildView::eGVT_CROSS_SECTION:
      rSliceView.SetOrientation(SliceOrientations::eCROSS_SECTION);
      break;
    default:
      break;
  }
} // CreateChildViews(HWND hWndThis)


/**
 * Message: WM_APP_SET_MAIN_VIEW_WINDOW: Called when sizing window to resize child views
 *
 * @param uType   Identifies size event
 * @param iCX     Width of window
 * @param iCY     Height of view window
 */
void MovieLayout::OnSize( UINT uType, int iCX, int iCY ) 
{
  ColonViatronixWnd::OnSize( uType, iCX, iCY );

  return;
  
  //
  // custom resizing code
  //

  if ( iCX < 1 || iCY < 1 )
    return;

  // header fixed height at top
  const bool bOverview = m_movie.m_format.m_bIncludeOverview;
  const int4 iHeaderHeight = WinUtils::GetClientRect(GetDlgItem( IDC_TEMPLATE_PATIENTINFO )).Height();
  GetDlgItem( IDC_TEMPLATE_PATIENTINFO )->SetWindowPos( NULL, 0, 0, iCX, iHeaderHeight, SWP_NOZORDER );

  // determine sizes
  const int4 iTextHeight = 24;
  const int4 iProgressHeight = 24;
  const int4 iButtonHeight = 32;
  const int4 iGap = 10;
  const int4 iImageSize = min( bOverview ? ( ( iCX - 3 * iGap ) / 2 ) : ( iCX - 2 * iGap ),
                               iCY - iHeaderHeight - 2 * iTextHeight - iProgressHeight - iButtonHeight - 6 * iGap );
  int4 iGapV = ( iCY - iHeaderHeight - 2 * iTextHeight - iImageSize - iProgressHeight - iButtonHeight ) / 6;

  // layout the title
  const int4 iTitleWidth = 200;
  const int4 iCenterX = iCX / 2;
  const int4 iTitleX = iCenterX - iTitleWidth / 2;
  const int4 iTitleY = iHeaderHeight + iGapV;
  GetDlgItem( IDC_STATIC_MOVIEVIEW_TITLE )->SetWindowPos( NULL, iTitleX, iTitleY, iTitleWidth, iTextHeight, SWP_NOZORDER );

  // layout the image
  const int4 iSingleImageX = iCenterX - iImageSize / 2;
  const int4 iDualGapH = ( iCX - 2 * iImageSize ) / 3;
  const int4 iLeftImageX = iDualGapH;
  const int4 iRightImageX = 2 * iDualGapH + iImageSize;
  const int4 iImageY = iTitleY + iTextHeight + iGapV;
  if ( bOverview )
  {
    if ( GetDlgItem( IDC_TEMPLATE_OVERVIEW ) != NULL ) GetDlgItem( IDC_TEMPLATE_OVERVIEW )->SetWindowPos( NULL, iLeftImageX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
    if ( GetDlgItem( IDC_TEMPLATE_SLICEVIEW ) != NULL ) GetDlgItem( IDC_TEMPLATE_SLICEVIEW )->SetWindowPos( NULL, iRightImageX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
    if ( GetDlgItem( IDC_TEMPLATE_ENDOVIEW ) != NULL ) GetDlgItem( IDC_TEMPLATE_ENDOVIEW )->SetWindowPos( NULL, iRightImageX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
  }
  else
  {
    if ( GetDlgItem( IDC_TEMPLATE_SLICEVIEW ) != NULL ) GetDlgItem( IDC_TEMPLATE_SLICEVIEW )->SetWindowPos( NULL, iSingleImageX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
    if ( GetDlgItem( IDC_TEMPLATE_ENDOVIEW ) != NULL ) GetDlgItem( IDC_TEMPLATE_ENDOVIEW )->SetWindowPos( NULL, iSingleImageX, iImageY, iImageSize, iImageSize, SWP_NOZORDER );
  }

  // layout the bottom text
  const int4 iRemainingWidth = 200;
  const int4 iRemainingX = iCenterX - iRemainingWidth - iGap / 2;
  const int4 iTimeX = iCenterX + iGap / 2;
  const int4 iTimeWidth = 100;
  const int4 iRemainingY = iImageY + iImageSize + iGapV;
  GetDlgItem( IDC_STATIC_MOVIEVIEW_PROGRESS )->SetWindowPos( NULL, iRemainingX, iRemainingY, iRemainingWidth, iTextHeight, SWP_NOZORDER );
  GetDlgItem( IDC_STATIC_MOVIEVIEW_TIMELEFT )->SetWindowPos( NULL, iTimeX, iRemainingY, iTimeWidth, iTextHeight, SWP_NOZORDER );

  // layout the progress bar
  const int4 iProgressWidth = iCX - 2 * iGap;
  const int4 iProgressY = iRemainingY + iTextHeight + iGapV;
  GetDlgItem( IDC_MOVIEVIEW_PROGRESS )->SetWindowPos( NULL, iGap, iProgressY, iProgressWidth, iProgressHeight, SWP_NOZORDER );

  // layout the cancel button
  const int4 iButtonWidth = 150;
  const int4 iButtonX = iCenterX - iButtonWidth / 2;
  const int4 iButtonY = iProgressY + iProgressHeight + iGapV;
  GetDlgItem( IDCANCEL )->SetWindowPos( NULL, iButtonX, iButtonY, iButtonWidth, iButtonHeight, SWP_NOZORDER );

  // flush 3D images
  rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone.m_fields.m_volumeRender.FlushAccumulationBuffer();

} // OnSize()


/**
 * Initialize movie
 */
void MovieLayout::InitializeMovie()
{
  // compute total number uf required frames using the sample time of the last acquired frame
  m_uTotalFrames = static_cast< uint4 >( m_movie.m_frames.back().m_uSampleTime / static_cast< float4 >( m_uMillisecondsPerFrame ) + 0.5F );
  
  // reset the current frame
  m_uCurrentFrame = 0;
  
  // initialize progress bar
  CProgressCtrl * pProgress = (CProgressCtrl*) GetDlgItem(IDC_MOVIEVIEW_PROGRESS);
  pProgress->SetRange( 0, 10000 );
  pProgress->SetPos( 0 );
  const float4 fProgressStep(10000.0F / m_uTotalFrames);
  pProgress->SetStep(int4(fProgressStep));
  
  // size movie frames appropriately (i.e. double the size if overviews are to be included)
  m_frameSize.m_x = m_frameSize.m_y = m_iImageSize;
  if( m_movie.m_format.m_bIncludeOverview )
    m_frameSize.m_x += m_iImageSize + m_uMidBorder;
  
  m_pAVI = new WriteAVI( m_movie.m_ssAVIFile.str(), m_frameSize.m_x, m_frameSize.m_y, m_iFrameRate );

  HDC hDC = GetDC()->GetSafeHdc();
  CDC *pDC = CDC::FromHandle(hDC);
  m_movieFrameBDC.m_bmp.CreateCompatibleBitmap( pDC, m_frameSize.m_x, m_frameSize.m_y );
  m_movieFrameBDC.CreateBmpDC( pDC );

  CBitmap titleFrameBmp;
  titleFrameBmp.LoadBitmap( IDB_MOVIEHEADER );
  BitmapDC titleBDC(titleFrameBmp, pDC);
  
  if( ( m_frameSize.m_x < titleBDC.m_info.bmWidth ) || ( m_frameSize.m_y < titleBDC.m_info.bmHeight ) )
  {
    BitmapDC resizeBDC;
    CRect resizeRect(0,0,Min(m_frameSize.m_x,uint4(titleBDC.m_info.bmWidth)),Min(m_frameSize.m_y,uint4(titleBDC.m_info.bmHeight)));
    resizeBDC.StretchFit(*pDC, resizeRect, titleBDC);
    Point2D<int4> pos((m_frameSize.m_x - resizeRect.Width())/2, (m_frameSize.m_y - resizeRect.Height())/2);
    m_movieFrameBDC.m_dc.BitBlt(pos.m_x,pos.m_y,resizeRect.Width(),resizeRect.Height(),&resizeBDC.m_dc,0,0,SRCCOPY);
  }
  else
  {
    Point2D<int4> pos((m_frameSize.m_x - titleBDC.m_info.bmWidth)/2, (m_frameSize.m_y - titleBDC.m_info.bmHeight)/2);
    m_movieFrameBDC.m_dc.BitBlt(pos.m_x,pos.m_y,titleBDC.m_info.bmWidth,titleBDC.m_info.bmHeight, &titleBDC.m_dc,0,0,SRCCOPY);
  }

  // add title frame (opening credits) :-)
  m_pAVI->AddFrame(m_movieFrameBDC.m_bmp, eOpeningCreditsLength);

  // reset blank movie frame
  m_movieFrameBDC.m_dc.FillSolidRect(CRect(0,0,m_frameSize.m_x,m_frameSize.m_y), RGB(0,0,0));

  // set the appropriate dataset
  AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM( m_movie.m_frames.front().m_eDatasetOrientation), 0);

  m_progressTimer.Reset();
} // InitializeMovie()


/**
 * Render single movie frame
 */
void MovieLayout::GLRenderViews()
{
  if( m_pAVI == NULL )
  {
    ColonViatronixWnd::GLRenderViews();
    InitializeMovie();
    return;
  }

  if( m_uCurrentFrame == m_uTotalFrames )
  {
    ColonViatronixWnd::GLRenderViews();
    return;
  }

  UpdateProgress();

  // compute time for this frame ( in milliseconds )
  int4 iFrameTime( m_uCurrentFrame * m_uMillisecondsPerFrame );

  // find the nearest samples
  uint4 uFrameIndex( m_movie.m_frames.size() - 1 );
  for( uint4 i( 1 ); i < m_movie.m_frames.size(); ++i )
  {
    if( m_movie.m_frames[ i ].m_uSampleTime >= iFrameTime )
    {
      uFrameIndex = i;
      break;
    }
  }

  MovieFrame frame1( m_movie.m_frames[ uFrameIndex - 1 ] );  
  MovieFrame frame2( m_movie.m_frames[ uFrameIndex ] );

  const float4 fAlpha( static_cast<float4>( ( frame2.m_uSampleTime - iFrameTime ) ) / ( frame2.m_uSampleTime - frame1.m_uSampleTime ) );
  const float4 fOneMinusAlpha( 1.0F - fAlpha );

  Normal3Df vpn(( static_cast< Vector3Df > ( frame1.m_camera.GetVpn() ) * fAlpha ) + ( static_cast< Vector3Df > ( frame2.m_camera.GetVpn() ) * fOneMinusAlpha  )  );
  Normal3Df vup(( static_cast< Vector3Df > ( frame1.m_camera.GetVup() ) * fAlpha ) + ( static_cast< Vector3Df > ( frame2.m_camera.GetVup() ) * fOneMinusAlpha  )  );
  Point3Df vrp (( static_cast< Vector3Df > ( frame1.m_camera.GetVrp() ) * fAlpha ) + ( static_cast< Vector3Df > ( frame2.m_camera.GetVrp() ) * fOneMinusAlpha  )  );

  // Update the current camera position
  GLChildViewRef childView = GetChildViewDlgItem((m_movie.m_eType == GLChildView::eGVT_NAVIGATION) ? IDC_TEMPLATE_ENDOVIEW : IDC_TEMPLATE_SLICEVIEW);

  Dataset & dataset = ((ViatronChildView&)childView).GetDatasetRef();

  dataset.m_submarine.SetPositionWorld( vrp );  
  dataset.m_submarine.SetOrientation( vpn );
  dataset.m_submarine.SetUpDirection( vup );
  dataset.m_vOrthogonalPtWorld = dataset.m_submarine.GetPositionWorld();
  // update the cross-section sliding bar position    
  dataset.m_iXsectionScrollBarLocationIndex = dataset.m_joinedSkeleton.ConvertToJoinedIndex( dataset.m_segmentList.GetCurrentSegment(), 
                                                       dataset.m_fields.GetClosestSkeletonIndex( dataset.m_submarine.GetPositionVolume()) );

  dataset.m_fields.m_volumeRender.m_magicWindowStartPt = Point<int4>(frame2.m_ptTransLoc.m_x,frame2.m_ptTransLoc.m_y,0);

  rdrGlobal.m_bBiopsyMode = (frame2.m_eFrameType == MovieFrame::TRANSLUCENCY);
  rdrGlobal.m_bSpaceLeaping = frame2.m_bSpaceLeaping;

  SliceChildView::ShowCamera(frame2.m_bShow2DCamera);

  // render views
  for ( int4 iView( 0 ); iView < NumGLViews(); iView++ )
    GLRenderView( iView );

  // create current frame
  CDC *pDC = GetDC();
  
  BitmapDC childBDC;
  childBDC.m_bmp.CreateCompatibleBitmap( pDC,  m_iImageSize, m_iImageSize );
  
  Point2D<int4> bufferSz;
  int4 iWidthAlignedDWORD( 0 );

  LPBYTE pImageBuffer = LPBYTE(childView.GetUpdateBuffer(bufferSz.m_x, bufferSz.m_y, iWidthAlignedDWORD, GL_BGR_EXT, GL_UNSIGNED_BYTE, false ));
  ResizeBmp(pDC, pImageBuffer, bufferSz, Point2D< uint4>( m_iImageSize, m_iImageSize ), childBDC );

  // add an overview image to movie (if the child is not an overview)
  if ( m_movie.m_format.m_bIncludeOverview )
  {
    GLChildViewRef ovChildView = GetChildViewDlgItem(IDC_TEMPLATE_OVERVIEW);
    BitmapDC overviewBDC;
    overviewBDC.m_bmp.CreateCompatibleBitmap(pDC, m_iImageSize, m_iImageSize );

    // render overview image for the movie frame and get the update buffers
    pImageBuffer = LPBYTE(ovChildView.GetUpdateBuffer(bufferSz.m_x, bufferSz.m_y, iWidthAlignedDWORD, GL_BGR_EXT, GL_UNSIGNED_BYTE));
    ResizeBmp(pDC, pImageBuffer, bufferSz, Point2D<uint4>(m_iImageSize,m_iImageSize), overviewBDC);

    // put the two images side by side
    m_movieFrameBDC.m_dc.BitBlt( m_iImageSize + m_uMidBorder, 0, m_iImageSize, m_iImageSize, &childBDC.m_dc, 0, 0, SRCCOPY );
    m_movieFrameBDC.m_dc.BitBlt( 0, 0, m_iImageSize, m_iImageSize, &overviewBDC.m_dc, 0, 0, SRCCOPY );
  }
  else
  {
    // add image to the movie frame
    m_movieFrameBDC.m_dc.BitBlt( 0, 0, m_iImageSize, m_iImageSize, &childBDC.m_dc, 0, 0, SRCCOPY );
  }

  // Create preview image: a jpg with the 1st image in the movie
  if (m_uCurrentFrame == 0)
  {
    ULONG_PTR pToken;
    Gdiplus::GdiplusStartupInput gsi;
    Gdiplus::GdiplusStartup(&pToken, &gsi, NULL);
    Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromHBITMAP(m_movieFrameBDC.m_bmp, HPALETTE(0));

    if (pImage != NULL)
    {
      CLSID imgCLSID;
      if (ReaderGlobal::GetCodecClsid(L"image/jpeg", imgCLSID))
      {
        extern std::wstring widestring(const std::string &);
        std::string sImgFile = vx::File(m_movie.m_ssAVIFile.str()).GetFullTitle() + vx::JPEGExt;
        pImage->Save((wchar_t*)widestring(sImgFile).c_str(), &imgCLSID, NULL);
      }
    }

    Gdiplus::GdiplusShutdown(pToken);
  }

  // add frame to movie
  try
  {
    m_pAVI->AddFrame( m_movieFrameBDC.m_bmp );
  }
  catch(...)
  {
    LogErr("Error adding frame to avi file","ColonViatronixWnd","CreateMovieFile");
    throw;
  }

  // advance to next frame
  ++m_uCurrentFrame;

  // are we done?
  if ( m_uCurrentFrame >= m_uTotalFrames )
  {
    // write movie file
    m_pAVI->Close();

    // converts and moves movie if necessary
    ConvertMovie();

    // TODO: Jump back to calling view
    AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE, MAKELPARAM(0, FramesManager::VFrameManager::NavFrameManager::ePOS_MOVIE));
  }

} // GLRenderViews()


/**
 * Convert movie
 */
bool MovieLayout::ConvertMovie()
{
  bool bSuccess(false);

  if (!m_pAVI->IsOK())
  {
    LogErr("Error detected in avi file after creation","ColonViatronixWnd","CreateMovieFile");
    bSuccess = false;
  }
  else
  {
    SHELLEXECUTEINFO sei;
    memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

    // Convert the movie to wmv
    if (m_movie.m_format.m_sFileTypeExt == vx::WindowsMediaVideoExt)
    {
      // {{ Create command line for movie conversion application
      std::strstream ssConvertParams;
      ssConvertParams << "-input \"" << m_movie.m_ssAVIFile.str() << "\" -output \"" << m_movie.m_ssTempWMVFile.str() << "\"";
      ssConvertParams << " -profile v32 -v_mode 2 -v_quality 100";
      ssConvertParams << " -v_width " << m_frameSize.m_x << " -v_height " << m_frameSize.m_y;
      ssConvertParams << " -v_framerate " << float4(eMovie_FrameRate);
      ssConvertParams << " -title \"" << "v3D Colon Fly-Thru "
        << "Created: "<< LPCTSTR(COleDateTime::GetCurrentTime().Format()) << "\"";
      ssConvertParams << " -author \"" << User::GetCurrentUser().GetFullName() << "\"";
      ssConvertParams << " -copyright \"2002 Viatronix, Inc.\"";
      ssConvertParams << std::ends;
      TRACE0(ssConvertParams.str());
      // }} (end) Create command line for movie conversion application

      // {{ Run the movie conversion utility
      sei.cbSize = sizeof(SHELLEXECUTEINFO);
      sei.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_DDEWAIT;
      sei.hwnd = HWND(*this);
      sei.lpVerb = "open";
      sei.lpFile = "wm8eutil.exe";
      sei.lpParameters = ssConvertParams.str();
      sei.lpDirectory = vx::File(m_movie.m_ssTempWMVFile.str()).GetParent().c_str();
      sei.nShow = SW_SHOW;
      sei.nShow = SW_SHOWMINNOACTIVE;

      ShellExecuteEx(&sei);
      // }} (end) Run the movie conversion utility

      ssConvertParams.freeze(false);
    }

    // Monitor utility and copy converted file in background process
    std::string sMovieFile = vx::File( rdrGlobal.m_pCurrDataset->GetSessionPrefix() ).GetParent() + "/Movies/" + m_movie.m_format.m_sFileTitle + vx::WindowsMediaVideoExt;
    vx::File(sMovieFile).GetParentFile().Mkdirs();
    MovieConvertProcess * pMCP = new MovieConvertProcess(vx::File(m_movie.m_ssTempWMVFile.str()),
      vx::File(sMovieFile), sei.hProcess, m_movie.m_format);

    if (!BackgroundProcess::Run(MoveConvertedMovie, LPVOID(pMCP), "MoveConvertedMovie", eBKHIDE_NONE, THREAD_PRIORITY_BELOW_NORMAL))
    {
      MoveConvertedMovie(LPVOID(pMCP));
    }

    bSuccess = true;
  }

  return bSuccess;
} // ConvertMovie()


/**
 * Background thread function, waits for event to complete
 *
 * @param pParam   Event handle
 */
UINT MovieLayout::WaitOnMovieCompletion(LPVOID pParam)
{
  WaitForSingleObject((HANDLE)pParam, INFINITE);
  return 1;
} // WaitOnMovieCompletion()


/**
 * Updates the progress
 */
void MovieLayout::UpdateProgress()
{
  // advance progressbar
  CProgressCtrl *pProgress = static_cast< CProgressCtrl * >( GetDlgItem( IDC_MOVIEVIEW_PROGRESS ) );
  pProgress->StepIt();

  // get the current range
  int4 iLower( 0 ), iUpper( 0 );
  pProgress->GetRange( iLower, iUpper );
  
  // calculate and display remaining time
  float4 fPercDone   = pProgress->GetPos() / static_cast< float4 >( iUpper );
  float4 fFramesDone = m_uTotalFrames * fPercDone;
  float4 fFramesLeft = m_uTotalFrames- m_uCurrentFrame;
  float4 fTimeLeft = ( m_progressTimer.ElapsedTime() / fFramesDone ) * fFramesLeft + 5.0F;
  
  std::string sTimeLeft = ToAscii(int4(fTimeLeft/60.0F)) + "m " + ToAscii(int4(fTimeLeft) % 60) + "s";
  SetDlgItemText(IDC_STATIC_MOVIEVIEW_TIMELEFT, sTimeLeft.c_str());

} // UpdateProgress()


/**
 * WM_COMMAND message
 *
 * @param wParam    message-dependent parameter
 * @param lParam    message-dependent parameter
 * @return          result of message processing
 */
BOOL MovieLayout::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch (wParam)
  {
    case IDCANCEL:
      AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NOCHANGE, MAKELPARAM(0, FramesManager::VFrameManager::NavFrameManager::ePOS_MOVIE));
      break;

    default:
      break;
  }

  return ColonViatronixWnd::OnCommand(wParam, lParam);
} // OnCommand()


/**
 * disabled message
 */
void MovieLayout::OnKeyDown(UINT, UINT, UINT)
{
} // OnKeyDown()


/**
 * disabled message
 **/
void MovieLayout::OnLButtonDown(UINT, CPoint)
{
} // OnLButtonDown()


/**
 * disabled message
 **/
void MovieLayout::OnMButtonDown(UINT, CPoint)
{
} // OnMButtonDown()


/**
 * disabled message
 **/
void MovieLayout::OnRButtonDown(UINT, CPoint)
{
} // OnRButtonDown()


/**
 * disabled message
 **/
void MovieLayout::OnLButtonUp(UINT, CPoint)
{
} // OnLButtonUp()


/**
 * disabled message
 **/
void MovieLayout::OnMButtonUp(UINT, CPoint)
{
} // OnMButtonUp()


/**
 * disabled message
 **/
void MovieLayout::OnRButtonUp(UINT, CPoint)
{
} // OnRButtonUp()


/**
 * disabled message
 **/
void MovieLayout::OnLButtonDblClk(UINT, CPoint)
{
} // OnLButtonDblClk()


/**
 * disabled message
 **/
void MovieLayout::OnMButtonDblClk(UINT, CPoint)
{
} // OnMButtonDblClk()


/**
 * disabled message
 **/
void MovieLayout::OnRButtonDblClk(UINT, CPoint)
{
} // OnRButtonDblClk()


/**
 * disabled message
 **/
void MovieLayout::OnMouseMove(UINT, CPoint)
{
} // OnMouseMove()


/**
 * empty method, used to disable menu and other commands
 **/
void MovieLayout::OnDisableCommand()
{
  // empty block
} // OnDisableCommand()


// $Log: MovieView.C,v $
// Revision 1.25.2.10  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.25.2.9  2010/02/26 15:38:46  dongqing
// removing another relic of the hardcoded-window-sizing headache
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.25.2.8  2009/12/09 20:26:59  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.25.2.7  2009/10/07 16:13:13  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.25.2.6  2009/01/21 22:02:59  jmeade
// Issue 6223: making 2d camera location a global option; reset after completing movie creation.
//
// Revision 1.25.2.5  2009/01/21 06:02:34  jmeade
// show/hide position camera in 2d movie.
//
// Revision 1.25.2.4  2008/02/07 19:33:00  jmeade
// removed hard-coded dialog sizes.
//
// Revision 1.25.2.3  2007/10/23 19:09:48  jmeade
// disable commands.
//
// Revision 1.25.2.2  2007/09/25 21:27:49  jmeade
// using defined enums instead of constants.
//
// Revision 1.25.2.1  2007/03/22 20:40:46  dongqing
// change the m_uXsectionScrollBarLocation to m_iXsectionScrollBarLocation
// The type is changed from uint4 to int4
//
// Revision 1.25  2007/03/12 20:47:11  jmeade
// code standards.
//
// Revision 1.24  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.23  2007/03/08 22:22:05  romy
// code review - header files adjusted
//
// Revision 1.22  2007/03/05 23:17:13  dongqing
// add new m_iXsectionScrollBarLocationIndex
//
// Revision 1.21  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.20  2006/10/30 20:25:39  jmeade
// Issue 5074: reset render size after creating movie.
//
// Revision 1.19  2006/10/04 20:49:10  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.18  2006/05/12 20:33:15  jmeade
// comment (removed a TODO since it's been done).
//
// Revision 1.17  2006/05/09 20:17:50  frank
// Issue #4737: Wrote movie files into the proper directory when just a prone dataset is present
//
// Revision 1.16  2006/02/23 19:15:16  jmeade
// movies in their own separate directory.
//
// Revision 1.15  2006/02/17 18:32:11  jmeade
// add preview image along with movie file.
//
// Revision 1.14  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.13  2005/11/22 16:38:40  jmeade
// movie creation reflecting new directory structure.
//
// Revision 1.12  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.11  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.10.2.2  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.10.2.1  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.10  2005/11/04 01:14:07  jmeade
// updated
//
// Revision 1.9  2005/10/28 13:28:15  geconomos
// now interploating captured frame positions
//
// Revision 1.8  2005/10/25 15:47:37  jmeade
// ParentViews ==> Layouts
//
// Revision 1.7  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.6  2005/09/16 15:08:06  jmeade
// Issue 4438: Disable the child views to prevent mouse interactions.
//
// Revision 1.5.2.1  2005/09/16 18:27:15  jmeade
// Issue 4438: Disable the child views to prevent mouse interactions.
//
// Revision 1.5  2005/09/07 13:39:38  geconomos
// Issue# 4417:  Removed some unwanted code from the rendering loop. Needs more investigating to figure out why performace halved between 1.5 and 1.6
//
// Revision 1.4  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
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
// Revision 1.1.4.1.2.6.2.2  2005/07/20 14:10:48  jmeade
// Issue 4295: Disable commands for F-keys.
//
// Revision 1.1.4.1.2.6.2.1  2005/07/14 18:39:05  jmeade
// Issue 4253: Only set current dataset once (movie creation for only one view).
//
// Revision 1.1.4.1.2.6  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 1.1.4.1.2.5  2005/06/10 18:20:49  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.1.4.1.2.4  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.4.1.2.3.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 1.1.4.1.2.3.4.1  2005/06/13 19:50:15  jmeade
// Set full oblique in movie view.
//
// Revision 1.1.4.1.2.3  2005/04/28 17:20:00  jmeade
// Issue 4147: Fix bugs to movie frame capture introduced by new framework changes.
//
// Revision 1.1.4.1.2.2  2005/04/22 04:28:34  jmeade
// Issue 4140: Propagate paint/update window call framework changes.
//
// Revision 1.1.4.1.2.1  2005/04/21 19:18:50  jmeade
// Issue 3851: Add debugging info for process exceptions.
//
// Revision 1.1.4.1  2005/02/15 18:50:04  jmeade
// merge from vc1.4 branch.
//
// Revision 1.1.2.12  2005/02/11 15:40:58  jmeade
// Issue 3881: Store current dataset with each frame.
//
// Revision 1.1.2.11  2005/02/10 00:40:48  jmeade
// Issue 3882: Turn off 2D HU and Vol/World coordinate display in movie frames.
//
// Revision 1.1.2.10  2005/02/07 16:50:33  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 1.1.2.9  2005/01/28 00:43:48  jmeade
// Return to previous view from movie-record-view.
//
// Revision 1.1.2.8  2005/01/27 22:47:05  jmeade
// Hide lumen coverage controls on movie view.
//
// Revision 1.1.2.7  2005/01/27 18:18:58  jmeade
// Only show appropriate views during movie creation;
// Display time remaining for movie creation.
//
// Revision 1.1.2.6  2005/01/26 17:11:36  jmeade
// Disable all tabs during movie creation.
//
// Revision 1.1.2.5  2005/01/25 00:23:52  jmeade
// Separate movie-creation view: bug fix, identify type of view (and slice orientation for 2d) for movie
//
// Revision 1.1.2.4  2005/01/24 16:36:00  jmeade
// Separate movie-creation view, incremental checkin.
//
// Revision 1.1.2.3  2005/01/21 20:50:55  jmeade
// Separate movie-creation view, incremental checkin.
//
// Revision 1.1.2.2  2005/01/20 21:26:06  jmeade
// not ready for half-3d-full-2d changes.
//
// Revision 1.1.2.1  2005/01/20 21:19:30  jmeade
// Separate movie view, incremental checkin.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieView.C,v 1.25.2.10 2010/09/20 18:41:34 dongqing Exp $
// $Id: MovieView.C,v 1.25.2.10 2010/09/20 18:41:34 dongqing Exp $
