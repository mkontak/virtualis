// $Id: COMViewer.C,v 1.2 2007/02/20 14:35:40 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "COMViewer.h"
#include "vxRenderingContextGL.h"
#include "GLFontGeorge.h"
#include "VolumeRender.h"
#include "vxLogFactory.h"

/**
 * Constructor
 */
COMViewer::COMViewer() : 
  m_fPercent( 0.0F ),
  m_pDevice( NULL ),
  m_pContext( NULL ),
  m_pDataset( &ReaderLib::rdrGlobal.m_supine ),
  m_uTimerResolution( 0 ),
  m_uTimerPeriod( 50 ),
  m_bAutoFlyEnabled( false ),
  m_bNagMessageEnabled( true ),
  m_bTimerRunning( false )
{
  m_bWindowOnly = true;
} // COMViewer()


/**
 * WM_CREATE handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  TIMECAPS tc;
  timeGetDevCaps( &tc, sizeof( TIMECAPS ) );
  m_uTimerResolution = min( max( tc.wPeriodMin, m_uTimerResolution ), tc.wPeriodMax );

  m_pDevice = new GLPaintDevice;
  m_pDevice->SetHDC( GetDC() );
  m_pDevice->SetHWND( m_hWnd );

  m_pContext = new vxRenderingContextGL( GLFormat(), m_pDevice );
  m_pContext->Create();
  m_pContext->SetInitialized( true );   

  m_pContext->MakeCurrent();

  glClearColor( 0.0F, 0.0F, 0.0F, 0.0F );

  m_pFont = new GLFontGeorge();
  m_pFont->Create( m_pContext, "Arial", 16, 0, 0 );
  m_pFont->SetStyle( GLFontGeorge::SHADOW_AROUND );

  m_pDataset->m_fields.m_volumeRender.SetRenderSize( ReaderLib::VolumeRender::GetMinRenderSize() );
  m_pDataset->m_fields.m_volumeRender.SetImageMode( ReaderLib::VolumeRender::ADAPTIVE );

  const float4 fSpeed = 1.0F;
  m_pDataset->m_submarine.SetThrottle( fSpeed );
  m_pDataset->m_submarine.SetPotentialStrength( fSpeed );

  vxLogFactory::CreateDefaultLogger( "c:\\vclog" );

  return 0;
} // OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * WM_DESTROY handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  if( m_pFont != NULL )
  {
    delete m_pFont;
    m_pFont = NULL;
  }

  if( m_pContext != NULL )
  {
    m_pContext->DoneCurrent();
    m_pContext->Reset();
    delete m_pContext;
    m_pContext = NULL;
  }

  if ( m_pDevice != NULL )
  {
    delete m_pDevice;
    m_pDevice = NULL;
  }

  return 0;
} // OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * WM_SIZE handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */

LRESULT COMViewer::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  m_pContext->MakeCurrent();

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0.0f,  LOWORD( lParam ), 0.0f, HIWORD( lParam ) );
      
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
  return 0;
} // OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * WM_LBUTTONDOWN handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  if( ReaderLib::rdrGlobal.IsLoadingComplete() )
  {
    if( m_bAutoFlyEnabled )
    {
      m_pDataset->m_submarine.StopFlying();
      m_pDataset->m_submarine.SwitchToManualMode();
      m_bAutoFlyEnabled = false;
    }
    
    CRect rc;
    GetClientRect( &rc );

    const float4 fHeading = Interpolate( 0.0F, LOWORD( lParam ), rc.Width()  - 1,  1.0F, -1.0F );
    const float4 fPitch   = Interpolate( 0.0F, HIWORD( lParam ), rc.Height() - 1, -1.0F,  1.0F );
    const float4 fThrust  = Bound( 0.0F, static_cast< float4 >( 1.0F - sqrt( fHeading * fHeading + fPitch * fPitch ) ), 1.0F );
    
    m_pDataset->m_submarine.FullStop();
    m_pDataset->m_submarine.StartFlying();
    m_pDataset->m_submarine.SetForces( fThrust, fHeading, fPitch );

    StartTimer();
  }
  else
  {
    Load(  m_bsInitialDirectory.Length() > 0? _com_util::ConvertBSTRToString( m_bsInitialDirectory ) :  "C:\\vcds" );
  }

  return 0;
} // OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * WM_LBUTTONDOWN handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  if( ReaderLib::rdrGlobal.IsLoadingComplete() )
  {
      m_pDataset->m_submarine.SwitchToAutomaticMode();
      m_pDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
      m_bAutoFlyEnabled = true;
      StartTimer();
  }
  return 0;
} // OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )

/**
 * WM_LBUTTONUP handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  if( !m_bAutoFlyEnabled )
  {
    m_pDataset->m_submarine.StopFlying();
    m_pDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
    StopTimer();
  }
  return 0;
} // OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * WM_MOUSEMOVE handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  if( m_bAutoFlyEnabled )
    return 0;

  CRect rc;
  GetClientRect( &rc );

  const float4 fHeading = Interpolate( 0.0F, LOWORD( lParam ), rc.Width()  - 1,  1.0F, -1.0F );
  const float4 fPitch   = Interpolate( 0.0F, HIWORD( lParam ), rc.Height() - 1, -1.0F,  1.0F );
  const float4 fThrust  = Bound( 0.0F, static_cast< float4 >( 1.0F - sqrt( fHeading * fHeading + fPitch * fPitch ) ), 1.0F );
  
  m_pDataset->m_submarine.SetForces( fThrust, fHeading, fPitch );

  return 0;
} // OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * WM_ERASEBKGND handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  bHandled = true;
  return 0;
} // OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * WM_KEYDOWN handler
 *
 * @param   uMsg        message id
 * @param   wParam      wparam value
 * @param   lParam      lparam value
 * @param   bHandled    indicates if message was handled
 * @return  lresult
 */
LRESULT COMViewer::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  return 0;
} // OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )


/**
 * Handles drawing for the control
 *
 * @param   di  drawing info
 * @rertun  S_OK on sucess
 */
HRESULT COMViewer::OnDraw( ATL_DRAWINFO & di )
{
  RECT& rc = *(RECT*)di.prcBounds;

  m_pContext->MakeCurrent();

  glClear( GL_COLOR_BUFFER_BIT );

  if( ReaderLib::rdrGlobal.IsLoadingComplete() )
  {    
    m_pDataset->m_submarine.UpdatePosition();
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
      m_pDataset->m_fields.m_volumeRender.Render( *m_pDataset, min( width, height ), false );
    glPopAttrib();

    if( m_bNagMessageEnabled )
      m_pFont->PutFormattedString( "For Evaluation Purposes Only", GLFontGeorge::ALIGN_CENTER, Point2D<float4>( width / 2, height / 2 ), Triplef( 1, 1, 0 ));
 }
  else
  {
    if( m_fPercent > 0.0F )
    {
      std::string s = "Loading " + ToAscii( m_fPercent ) + "%%";
      m_pFont->PutFormattedString( s, GLFontGeorge::ALIGN_CENTER, Point2D<float4>((rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2 ), Triplef( 1, 1, 0 ));
    }
    else
    {
      m_pFont->PutFormattedString( "V3D Viewer", GLFontGeorge::ALIGN_CENTER, Point2D<float4>((rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2 ), Triplef( 1, 1, 0 ));
    }
  }
  m_pContext->SwapBuffers();
  
  return S_OK;
} // OnDraw( ATL_DRAWINFO & di )


/**
 * Loads the specified dataset
 *
 * @param   sDirectory  dataset directory
 */
void COMViewer::Load( const std::string & sDirectory )
{ 
  try
  {
    ReaderLib::StudyLoadParams params;    
    params.GetPrimaryDataset().SetVolumeDirectory( sDirectory );
    params.GetPrimaryDataset().SetPreprocessedDirectory( sDirectory );
    params.GetPrimaryDataset().SetPrefix(  ReaderLib::DatasetLoadParams::GeneratePrefix( sDirectory, "s" ) );    
    params.SetFlags( ReaderLib::StudyLoadParams::PRIMARY_DATASET_VALID );
    params.SetSessionDirectory( sDirectory );

    ReaderLib::rdrGlobal.LoadStudy2( params, ReaderLib::StudyLoad::LoadCallback( StudyLoadNotify, this ) );

    ReaderLib::StudyLoad::LoadCallback loadCallback( StudyLoadNotify, this );
    loadCallback.NextStep(ReaderLib::StudyLoad::eINITIALIZE, 90, ReaderLib::DatasetOrientations::eSUPINE );
    loadCallback.NextStep(ReaderLib::StudyLoad::eINITIALIZE, 95, ReaderLib::DatasetOrientations::ePRONE );
    loadCallback.NextStep(ReaderLib::StudyLoad::eINITIALIZE_COMPLETE, 0, ReaderLib::DatasetOrientations::eDEFAULT );

    if( !ReaderLib::rdrGlobal.m_bSpaceLeaping && !m_pDataset->m_fields.IsDiffVolLoaded() )
      m_pDataset->m_fields.LazyLoadDiffVolume();

  }
  catch( ... )
  {
  }
} // Load( const std::string & sDirectory )


/**
 * Load study callback procedure
 *
 * @param   pData             user specific data
 * @param   ePrevStep         previous load step
 * @param   eNextStep         next load step
 * @param   fPercentComplete  total percent complete
 * @param   eOrientation      dataset orientation
 * @return  0 for sucess
 */
int4 COMViewer::StudyLoadNotify( void * pData, const ReaderLib::StudyLoad::StepsEnum ePrevStep,
                                 const ReaderLib::StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete,
                                 const ReaderLib::DatasetOrientations::TypeEnum eOrientation )
{
  COMViewer * pViewer = reinterpret_cast< COMViewer* >( pData );
  pViewer->m_fPercent = fPercentComplete;
  pViewer->RedrawWindow();
  return 0;
} // StudyLoadNotify()


/**
 * Starts the animation timer
 */
void  COMViewer::StartTimer()
{
  if( !m_bTimerRunning )
  {
    timeBeginPeriod( m_uTimerResolution );

    m_uTimerId = timeSetEvent( m_uTimerResolution, m_uTimerResolution,  TimerProc, reinterpret_cast< DWORD_PTR >( this ), TIME_PERIODIC );
    
    m_bTimerRunning = true;
  }
} // StartTimer()


/**
 * Stops the animation timer
 */
void COMViewer::StopTimer()
{
  if( m_bTimerRunning )
  {
    timeKillEvent( m_uTimerId );
    timeEndPeriod( m_uTimerResolution );
    m_bTimerRunning = false;
  }
} // StopTimer()


/**
 * Timer procedure
 * 
 * @param   uId         timer id
 * @param   uMsg        timer message
 * @param   dwUser      user specific data
 * @param   dw1         unused
 * @param   dw2         unused
 */
void __stdcall COMViewer::TimerProc( UINT uId, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
  reinterpret_cast< COMViewer* >( dwUser )->RedrawWindow();
} // TimerProc( UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2 )


// $Log: COMViewer.C,v $
// Revision 1.2  2007/02/20 14:35:40  geconomos
// including logging
//
// Revision 1.1  2007/02/16 14:43:41  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/misc/activex/COMViewer.C,v 1.2 2007/02/20 14:35:40 geconomos Exp $
// $Id: COMViewer.C,v 1.2 2007/02/20 14:35:40 geconomos Exp $
