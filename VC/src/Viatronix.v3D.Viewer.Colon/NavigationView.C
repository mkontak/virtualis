// $Id: NavigationView.C,v 1.101.2.35 2010/12/21 22:55:10 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade [jmeade@viatronix.com]


/**
 * Endoluminal Child view
 */


// pragmas
#pragma warning (disable : 4251)


// includes
#include "stdafx.h"
#include <mmsystem.h>
#include "MainAppFrame.h"
#include "Viatron.h"
#include "glFont.h"
#include "VolumeRender.inl"
#include "RenderUtil.h"
#include "TextPromptWnd.h"
#include "GdiplusUtilities.h"
#include "vxPolypSegmentation.h"
#include "vxHistogramUpdaterLabel.h"
#include "ApplicationSettings.h"
#include "CadUtils.h"

//#include "ReaderGlobal.h"
//#include "V1KMessages.h"
//#include "ViatronStudy.h"
//#include "VolumeRender.inl"
//#include "WinUtils.h"
//#include "Dataset.h"
//#include "ImageUtilities.h"
//#include "PixelRGBA.h"
//#include "Overview.h"

#include "NavigationView.h"


// defines
#define FILE_REVISION "$Revision: 1.101.2.35 $"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// namespaces
using namespace std;
using namespace ColonViews::ChildViews;
using namespace WinUtils;
using namespace ReaderLib;
using namespace ColonViews::Dialogs;
using namespace vxCAD;

/// static member declarations
const uint4 EndoluminalView::m_uWrongWayFrameThreshold(10);
const int4 iPickupShiftY(6);

#ifdef FINAL_RELEASE
bool EndoluminalView::m_bDisplayRenderSpeed(false);
#endif

uint4 EndoluminalView::m_uMaxFlightSpeed(200);
const float4 EndoluminalView::m_fNearClipPlane( 7.0F );

bool EndoluminalView::m_bForceDrawCurrentROI(false);

//////////////////////////////////////////////////////////////////////
// EndoCoverageProgress methods
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(EndoCoverageProgressCtrl, ProgressCtrl)
  //{{AFX_MSG_MAP(EndoCoverageProgressCtrl)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * Creates the endoscopic coverage progress control.
 *
 * @param dwStyle      Window style flags.
 * @param rect         Window rectangle.
 * @param pParentWnd   Parent window.
 * @param uID          ID for the new control.
 * @return             TRUE/FALSE indicating success of creation.
 */
BOOL EndoCoverageProgressCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT uID)
{
  if (!ProgressCtrl::Create(dwStyle, rect, pParentWnd, uID))
  {
    return FALSE;
  }
  
  if ( (dwStyle & PBS_VERTICAL) == 0 )
  {
    DWORD dwStaticStyle = (dwStyle & WS_VISIBLE) | WS_CHILD;
    HFONT hFont = MainAppFrame::GetMainFrame()->m_hMainVtxFont;

    m_labelStatic.Create(LoadResourceString(IDS_LUMEN_COVERAGE), dwStaticStyle | SS_LEFT, CRect(), pParentWnd);
    m_labelStatic.SendMessage(WM_SETFONT, WPARAM(hFont), LPARAM(TRUE));

    m_buddyNumberStatic;
  }

  strcpy(m_sCvgValueFmt, LPCTSTR(LoadResourceString(IDS_LUMEN_COVERAGE)));

  MoveWindow(CRect(rect), TRUE);


  return TRUE;
} // Create()


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local helper functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * gets the size of the text drawn in a window/control
 *
 * @param wnd               the window/control
 * @param sText             the text
 * @param uDrawTextFormat   format for drawn text
 * @return                  size of drawn text
 */
CSize GetTextSize(CWnd& wnd, const CString& sText, UINT uDrawTextFormat = 0)
{
  CClientDC dcWnd(&wnd);
  CFont * pOldFont = dcWnd.SelectObject(wnd.GetFont());
  
  CRect textRect(0,0,0,0);
  dcWnd.DrawText(sText, textRect, (uDrawTextFormat & ~DT_CALCRECT) | DT_CALCRECT);
  
  dcWnd.SelectObject( pOldFont );
  
  return textRect.Size();
} // GetTextSize()


/**
 * gets the size of a dialog/window's text
 *
 * @param wnd               window to query
 * @param uDrawTextFormat   format of window's text
 * @return                  size of the drawn text
 */
CSize GetWindowTextSize(CWnd& wnd, UINT uDrawTextFormat = 0)
{
  CString sText; wnd.GetWindowText(sText);
  return GetTextSize(wnd, sText, uDrawTextFormat);
} // GetWindowTextSize()


/**
 * Compute how opaque a 'hotspot' should be depending on its distance from the cursor.
 * It should be fully opaque when the cursor is near and fade away as the cursor moves away.
 *
 * @param rect       the location of the hotspot
 * @param cursorPt   the cursor position
 *
 * @return            desired opacity in the range [0,1] 
 */
float4 GetHotspotOpacity( Rect<int4> & rect, CPoint cursorPt )
{

  const float4 fDistance = sqrt( powf( ( rect.GetMinX() + rect.GetMaxX() ) / 2 - cursorPt.x, 2 ) +
                                 powf( ( rect.GetMinY() + rect.GetMaxY() ) / 2 - cursorPt.y, 2 ) );

  return Interpolate( 20.0F, Bound( 20.0F, fDistance, 100.0F), 100.0F, 1.0F, 0.0F );

} // GetHotspotOpacity( rect, cursorPt )


/**
 * show/hide window
 * @param uCmdShow   The type of 'show' operation
 * @return           Result of show window command
 */
BOOL EndoCoverageProgressCtrl::ShowWindow(int uCmdShow)
{
  if ( IsWindow(m_labelStatic) )
    m_labelStatic.ShowWindow(uCmdShow);
  if ( IsWindow(m_buddyNumberStatic) )
    m_buddyNumberStatic.ShowWindow(uCmdShow);

  return ProgressCtrl::ShowWindow(uCmdShow);
} // ShowWindow()


/**
 * Override of window move to properly place controls.
 *
 * @param x          X-coordinate of upper-left of control.
 * @param y          Y-coordinate of upper-left of control.
 * @param iWidth     Width of control.
 * @param iHeight    Height of control.
 * @param bRepaint   Whether or not to repaint the control.
 */
void EndoCoverageProgressCtrl::MoveWindow(int x, int y, int iWidth, int iHeight, BOOL bRepaint)
{
  if ( (GetStyle() & PBS_VERTICAL) == 0 )
  {
    CRect rect(CPoint(x, y + iHeight + 5), CSize(iWidth, 14));

    // Place label
    CRect labelRect(CPoint(rect.left, rect.top - 3), GetWindowTextSize(m_labelStatic));
    m_labelStatic.MoveWindow(labelRect, bRepaint);
    m_labelStatic.RedrawWindow();

    // Place progress
    rect.left = labelRect.right + 4;
    //rect.right -= iBuddyWidth + 4;
    ProgressCtrl::MoveWindow(CRect(rect.left,rect.top-2,rect.right,rect.bottom+2), bRepaint);

    // No more buddy window
    m_buddyNumberStatic;
  }
  else
  {
    const int4 iParentBorder(2); // the parent window has a border
    CRect rect(CPoint(x + iWidth + 2, y-iParentBorder), CSize(14, iHeight+iParentBorder));

    // No label
    m_labelStatic;

    // Place progress
    ProgressCtrl::MoveWindow(rect, bRepaint);

    // No buddy
    m_buddyNumberStatic;
  }

} // MoveWindow()


/**
 * Override of window move to properly place controls.
 *
 * @param lpRect     Coordinate of the window.
 * @param bRepaint   Whether the window should be redrawn after moving.
 */
void EndoCoverageProgressCtrl::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{
  MoveWindow(lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
} // MoveRect()


/**
 * Sets the position and text in the progress control.
 *
 * @param iPos   Percentage completion (0-100).
 * @return       Previous position of progress bar control.
 */
int EndoCoverageProgressCtrl::SetPos(int iPos)
{
  if (iPos != GetPos() || iPos == 0)
  {
    if ( IsWindow(m_buddyNumberStatic) )
    {
      static char sPosTxt[21]; sprintf(sPosTxt, m_sCvgValueFmt, iPos);
      m_buddyNumberStatic.SetWindowText(sPosTxt);
    }
    return ProgressCtrl::SetPos(iPos);
  }
  return GetPos();
} // SetPos()


//////////////////////////////////////////////////////////////////////
// EndoluminalView Construction/Destruction
//////////////////////////////////////////////////////////////////////


/**
 * Constructor 
 *
 * @param hWndParent       Parent window.
 * @param eOrient          Study for this endoscopic view.
 * @param bVertLumenCvrg   Whether to set the lumen coverage control along the right-hand side of the view
 */
EndoluminalView::EndoluminalView(HWND hWndParent, const DatasetOrientations::TypeEnum eOrient, const bool bVertLumenCvrg)
: ViatronChildView(eGVT_NAVIGATION, hWndParent, eOrient),
m_uWrongWayCount( 0 ),
m_fPercentCoverage( -1.0F ),
m_fAverageHotspotOpacity( 0.0F ),
m_bChangingFOV( false )
#if !defined(FINAL_RELEASE)
  , m_iRdbcAnim(0)
#endif
{
  CWnd* pParent = CWnd::FromHandle(hWndParent);
  m_progressCtrl.Create(WS_VISIBLE | WS_CHILD | PBS_SMOOTH | (bVertLumenCvrg?PBS_VERTICAL:0), CRect(), pParent, GetClassInstanceID());
  m_pQuadObject = gluNewQuadric();

  // initialize the images
  int4 iResources[3] = { IDB_FINDING_UNDECIDED, IDB_FINDING_ACCEPT, IDB_FINDING_REJECT };
  for ( int4 i(0); i<3; i++ )
  {
    try
    {
      m_pFindingImages[i] = GdiplusUtilities::LoadImageFromResource( MAKEINTRESOURCE( iResources[i] ), "PNG" );
      try
      {
        // flip the image
        for ( int4 y(0); y<m_pFindingImages[i]->GetDim().m_y / 2; ++y )
        {
          for ( int4 x(0); x<m_pFindingImages[i]->GetDim().m_x; ++x )
          {
            const Point2D< uint2 > pos1( x, y );
            const Point2D< uint2 > pos2( x, m_pFindingImages[i]->GetDim().m_y - 1 - y );
            const PixelRGBA< uint1 > tempPixel( m_pFindingImages[i]->GetPixel( pos1 ) );
            m_pFindingImages[i]->SetPixel( pos1, m_pFindingImages[i]->GetPixel( pos2 ) );
            m_pFindingImages[i]->SetPixel( pos2, tempPixel );
          } // for x
        } // for y

        // increase alpha to at least 32/255
        const uint1 uMinAlpha( 32 );
        for ( int4 y(0); y<m_pFindingImages[i]->GetDim().m_y; ++y )
        {
          for ( int4 x(0); x<m_pFindingImages[i]->GetDim().m_x; ++x )
          {
            const Point2D< uint2 > pos( x, y );
            PixelRGBA< uint1 > pixel( m_pFindingImages[i]->GetPixel( pos ) );
            if ( pixel.A() < uMinAlpha )
              pixel.A() = uMinAlpha;
            m_pFindingImages[i]->SetPixel( pos, pixel );
          } // for x
        } // for y
      } // try
      catch (ex::VException & ep)
      {
        LogErr( ep.GetLogRecord().GetTextMessage(),"EndoluminalView","EndoluminalView" );
      }
      catch ( ... )
      {
        LogDbg( "exception flipping image", "EndoluminalView", "EndoluminalView" );
      }
    }
    catch ( ... )
    {
      LogDbg( "exception loading png image", "EndoluminalView", "EndoluminalView" );
    }
  }
  InitSessionParams();

  m_pCurrentAnnotation = GetDatasetRef().m_session.m_2DAnnotationList.end();
} // EndoluminalView()


/**
 * Destructor 
 */
EndoluminalView::~EndoluminalView()
{
  gluDeleteQuadric(m_pQuadObject);

  // release finding images
  for ( int4 i(0); i<3; i++ )
  {
    if ( m_pFindingImages[i] != NULL )
      delete m_pFindingImages[i];
    m_pFindingImages[i] = NULL;
  }
} // destructor


/**
 * Initializes navigation view parameters that are saved across VC sessions
 */
void EndoluminalView::InitSessionParams()
{
  static const std::string sDefaultFlightParams(TEXT("200,180.0,0.0015,0.015"));
  std::string sSettings( sDefaultFlightParams );
  //{{ Initialize flight parameters (max manual speed, ideal frame rate (max auto speed), turn rate, thrust input
  ApplicationSettings::UserSettings().TryGet( APP_SETTING_FLIGHT_PARAMS, sSettings );

  bool bValidSpeedometerParams(false);
  std::pair<std::string::size_type,std::string::size_type> findIndex(0,-1);
  if ((findIndex.second = sSettings.find(',',findIndex.second+1)) != std::string::npos)
  {
    EndoluminalView::m_uMaxFlightSpeed = atoi(sSettings.substr(findIndex.first).c_str());
    findIndex.first = findIndex.second+1;

    if ((findIndex.second = sSettings.find(',',findIndex.second+1)) != std::string::npos)
    {
      Submarine::m_fIdealFrameRate = atof(sSettings.substr(findIndex.first).c_str());
      findIndex.first = findIndex.second+1;

      if ((findIndex.second = sSettings.find(',',findIndex.second+1)) != std::string::npos)
      {
        Submarine::m_fTurnRateScale = atof(sSettings.substr(findIndex.first).c_str());
        findIndex.first = findIndex.second+1;

        Submarine::m_fThrustScale = atof(sSettings.substr(findIndex.first).c_str());
        findIndex.first = findIndex.second+1;

        bValidSpeedometerParams = true;
      }
    }
  }

  // Reset default settings if that in the registry is invalid
  if (!bValidSpeedometerParams)
    ApplicationSettings::UserSettings().Set( APP_SETTING_FLIGHT_PARAMS, sDefaultFlightParams );
  //}} (end) Initialize flight parameters

  //{{ Initialize current flight speed
  const float4 fMaxSpeed(Max(EndoluminalView::m_uMaxFlightSpeed, uint4(1)));

  float4 fSpeed(Min(EndoluminalView::GetUserPreferredFlightSpeed() / fMaxSpeed, 1.0F));
  if (fSpeed > 0.0F)
  {
    rdrGlobal.m_supine.m_submarine.SetThrottle(fSpeed);
    rdrGlobal.m_supine.m_submarine.SetPotentialStrength(fSpeed);
    rdrGlobal.m_prone.m_submarine.SetThrottle(fSpeed);
    rdrGlobal.m_prone.m_submarine.SetPotentialStrength(fSpeed);
  }
  //}} end Initialize current flight speed

  //{{ Initialize translucent view
  ApplicationSettings::UserSettings().TryGet( APP_SETTING_TRANSLUCENCY_SETTINGS, sSettings );
  std::vector<TransferFunctionVC::LinearTFParams>* vptrans[2]
  = { &rdrGlobal.m_supine.m_biopsyTransferFunction.m_vfBiopsyThresholds,
    &rdrGlobal.m_prone.m_biopsyTransferFunction.m_vfBiopsyThresholds };

  for (int4 c(0); c < 2; c++)
  {
    std::vector<TransferFunctionVC::LinearTFParams>& vt = *(vptrans[c]);
    findIndex.first=0; findIndex.second=-1;
    for (int4 i(0); i < vt.size(); i++)
    {
      // threshold
      if ((findIndex.second = sSettings.find(' ',findIndex.second+1)) == std::string::npos) { break; }
      vt[i].m_fThreshold = atof(sSettings.substr(findIndex.first).c_str());
      findIndex.first = findIndex.second+1;
      // opacity first
      if ((findIndex.second = sSettings.find(' ',findIndex.second+1)) == std::string::npos) { break; }
      vt[i].m_prevColor.A() = atof(sSettings.substr(findIndex.first).c_str());
      findIndex.first = findIndex.second+1;
      // opacity second
      if ((findIndex.second = sSettings.find(' ',findIndex.second+1)) == std::string::npos) { break; }
      vt[i].m_nextColor.A() = atof(sSettings.substr(findIndex.first).c_str());
      findIndex.first = findIndex.second+1;

      COLORREF clr;
      // color first
      if ((findIndex.second = sSettings.find(' ',findIndex.second+1)) == std::string::npos) { break; }
      clr = atoi(sSettings.substr(findIndex.first).c_str());
      vt[i].m_prevColor.R() = GetRValue(clr); vt[i].m_prevColor.G() = GetGValue(clr); vt[i].m_prevColor.B() = GetBValue(clr);
      findIndex.first = findIndex.second+1;
      // color second
      if ((findIndex.second = sSettings.find(' ',findIndex.second+1)) == std::string::npos) { break; }
      clr = atoi(sSettings.substr(findIndex.first).c_str());
      vt[i].m_nextColor.R() = GetRValue(clr); vt[i].m_nextColor.G() = GetGValue(clr); vt[i].m_nextColor.B() = GetBValue(clr);
      findIndex.first = findIndex.second+1;
    }
  }

  if (rdrGlobal.m_supine.IsLoadingComplete())
  {
    rdrGlobal.m_supine.m_biopsyTransferFunction.SetBiopsy();
    rdrGlobal.m_supine.UpdateBiopsyColormap();
    rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  }
  if (rdrGlobal.m_prone.IsLoadingComplete())
  {
    rdrGlobal.m_prone.m_biopsyTransferFunction.SetBiopsy();
    rdrGlobal.m_prone.UpdateBiopsyColormap();
    rdrGlobal.m_prone.m_fields.m_volumeRender.FlushAccumulationBuffer();
  }

  GetDatasetRef().m_fields.m_volumeRender.SetRenderSize( VolumeRender::GetMinRenderSize() );
  GetDatasetRef().m_fields.m_volumeRender.SetImageMode( VolumeRender::ADAPTIVE );

  //}} (end) Initialize translucent view
} // InitSessionParams()


/**
 * called before the current layout changes (save values)
 */
void EndoluminalView::OnLayoutChanging()
{
  // save flight speed to registry
  SetUserPreferredFlightSpeed(rdrGlobal.m_pCurrDataset->m_submarine.GetThrottle());

  ViatronChildView::OnLayoutChanging();
} // OnLayoutChanging()


/**
 * called after the current layout changes (load values)
 */
void EndoluminalView::OnLayoutChanged()
{
  InitSessionParams();
  ViatronChildView::OnLayoutChanged();
} // OnLayoutChanged()


/**
 * Gets the user's preferred (saved) flight speed from the registry
 * @return   The currently saved flight speed for the logged-in user
 */
float4 EndoluminalView::GetUserPreferredFlightSpeed()
{
  return ApplicationSettings::UserSettings().Get<float4>( APP_SETTING_FLIGHT_SPEED );
} // GetUserPreferredFlightSpeed()


/**
 * Saves the user's preferred flight speed to the registry
 *
 * @param fSpeed   the flight speed to be saved
 */
void EndoluminalView::SetUserPreferredFlightSpeed(float4 fSpeed)
{
  if( fSpeed <= 0.0F )
    fSpeed = rdrGlobal.m_pCurrDataset->m_submarine.GetThrottle();

  ApplicationSettings::UserSettings().SetFloat( APP_SETTING_FLIGHT_SPEED, fSpeed * float4(EndoluminalView::m_uMaxFlightSpeed));
} // SetUserPreferredFlightSpeed()


/**
 * Show or hide lumen coverage fields
 *
 * @param bShow   Whether to show or hide controls
 */
void EndoluminalView::ShowLumenCoverage(const bool bShow)
{
  m_progressCtrl.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
} // ShowLumenCoverage()


/**
 * Sets up the rendering context.
 */
void EndoluminalView::GLSetupRC()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
} // GLSetupRC()


/**
 * Background process to play specified sound.
 *
 * @param iSoundID   Sound resource ID.
 * @return           TRUE.
 */
UINT ThreadPlaySoundProc(LPVOID iSoundID)
{
  PlaySound(LPCTSTR(iSoundID), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
  return 1;
} // ThreadPlaySoundProc()


/**
 * handles any initializations
 * @return   success of creation
 */
bool EndoluminalView::GLCreate()
{
  if( !ViatronChildView::GLCreate() )
    return false;

  m_medFont.Create(GetHDC(), -18, 0, "arial", FW_NORMAL, false, false, true);

  return true;
} // GLCreate()


/**
 * sizing event
 * @param uType   type of size event
 * @param iCX     width
 * @param iCY     height
 **/
void EndoluminalView::OnSize(UINT uType, int iCX, int iCY )
{
  ViatronChildView::OnSize( uType, iCX, iCY );

  GetDatasetRef().m_fields.m_volumeRender.SetDisplaySize(iCX); // set size for forward view

  if (GetOppositeDatasetRef().IsLoadingComplete() == true)
  {
    //int4 KEEP_AN_EYE_ON_THIS;
    GetOppositeDatasetRef().m_fields.m_volumeRender.SetRenderSize(iCX);
    GetOppositeDatasetRef().m_fields.m_volumeRender.SetDisplaySize(iCX);
  }

} // OnSize()


/**
 * Called just prior to window pos change event
 *
 * @param pWndPos   information on pos change
 */
void EndoluminalView::OnWindowPosChanging( WINDOWPOS * pWndPos )
{
  ViatronChildView::OnWindowPosChanging( pWndPos );
  InvalidateAsscociatedControl( m_progressCtrl );
} // OnWindowPosChanging()


/**
 * Called just after window pos change event
 *
 * @param pWndPos   information on pos change
 */
void EndoluminalView::OnWindowPosChanged( WINDOWPOS * pWndPos )
{
  ViatronChildView::OnWindowPosChanged( pWndPos );
  
  CRect rc;
  GetWindowRect( &rc );
  GetParent()->ScreenToClient( &rc );

  m_progressCtrl.MoveWindow( rc );
  
  InvalidateAsscociatedControl( m_progressCtrl );
} // OnWindowPosChanged()


/**
 * Called when mode changes
 *
 * @param uOldMode   previous mode
 * @param uNewMode   current mode
 */
void EndoluminalView::OnViewModeChange(const uint4 uOldMode, const uint4 uNewMode)
{
  OnSetCursor(this, 0, 0);
} // OnViewModeChange()


/**
 * Draw annotations
 *
 * @param pfDepthBuffer       image depth buffer
 * @param uRenderSize         size to render annotations (unused)
 * @param uRenderBufferSize   size of render buffer
 * @param viewpoint           current viewpoint
 */
void EndoluminalView::DrawAnnotations(const float4 * pfDepthBuffer, const uint4 uRenderSize, const uint4 uRenderBufferSize, const Point<float4> viewpoint)
{
  const GLFont& rGLFont = GetNormalFont();
  
  // get the current OpenGL matrices
  GLdouble mModelMatrix[16]; glGetDoublev(GL_MODELVIEW_MATRIX,mModelMatrix);
  GLdouble mProjMatrix[16]; glGetDoublev(GL_PROJECTION_MATRIX,mProjMatrix);
  const GLint vViewport[4] = { 0, 0, uRenderSize, uRenderSize };
  const Rect<GLint> viewportRect(vViewport[0], vViewport[1], vViewport[2], vViewport[3]);

  // OpenGL setup
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
  
  Point<float8> winCoordsPoint(0.0,0.0,0.0);
  Point<float4> point(0.0,0.0,0.0);

  PixelRGBA<float4> colorNormal(1.0F, 0.0F, 0.0F, 0.75F);
  PixelRGBA<float4> colorHighlight(0.0F, 1.0F, 0.0F, 0.75F);

  Dataset& rDataset = GetDatasetRef();

  BookmarkList& atnList = rDataset.m_session.m_2DAnnotationList;
  for (BookmarkList::const_iterator bmIt = atnList.begin(); bmIt != atnList.end(); bmIt++)
  {
    gluProject(float8(bmIt->m_position.m_x), float8(bmIt->m_position.m_y), float8(bmIt->m_position.m_z),
               mModelMatrix, mProjMatrix, vViewport,
               &winCoordsPoint.m_x, &winCoordsPoint.m_y, &winCoordsPoint.m_z);
    
    // determine whether to draw the current marker
    if (viewportRect.PtInRect(winCoordsPoint.m_x, winCoordsPoint.m_y) /*&& winCoordsPoint.m_z > 0.0f*/)
    {
      float4 fDepthZ(pfDepthBuffer[((int4)(winCoordsPoint.m_y))*uRenderBufferSize + ((int4)(winCoordsPoint.m_x))]);
      float4 fDistSquared(Vector<float4>(bmIt->m_position-viewpoint).GetLengthSquared());
      if (fDistSquared <= fDepthZ*fDepthZ)
      {
        RenderUtil::RenderArrow(bmIt->m_position, bmIt->m_orientation, 2.5, false, -1,
          (bmIt == m_pCurrentAnnotation) ? colorHighlight : colorNormal, true);
        Point<float4> textPt(bmIt->m_position + Vector<float4>(rDataset.m_submarine.GetUpDirection()));
        rGLFont.DrawText(textPt.m_x, textPt.m_y, textPt.m_z, bmIt->m_sName);
      } // if in front of organ wall
    } // if within viewport
  } // endfor all annotations

  glDisable(GL_LIGHT0);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
} // DrawAnnotations()


/**
 * Comparison function for depth of a hotspot
 *
 * @param x    the first hotspot to compare
 * @param y    the second hotspot to compare
 *
 * @return     if depth of first is less than depth of second
 */
bool FindingHotspot::SortAscendingDepth::operator()( FindingHotspot & x, FindingHotspot & y )
{
  return x.m_fDepth < y.m_fDepth;
} // SortAscendingDepth( x, y )


/**
 * Comparison function for distance of a hotspot from the cursor
 *
 * @param x    the first hotspot to compare
 * @param y    the second hotspot to compare
 *
 * @return     if cursor distance of first is less than depth of second
 */
bool FindingHotspot::SortAscendingCursorDistance::operator()( FindingHotspot & x, FindingHotspot & y )
{
  return x.m_fCursorDistance < y.m_fCursorDistance;
} // SortAscendingCursorDistance( x, y )


/**
 * Draw cad findings.
 *
 * @param pfDepthBuffer     the depth buffer
 * @param uRenderSize       the size of the active portion of the image buffer
 * @param uRenderBufferSize the size of the full power-of-two image buffer
 * @param viewpoint         the viewpoint
 */
void EndoluminalView::DrawCadFindings( const float4 * pfDepthBuffer, const uint4 uRenderSize, const uint4 uRenderBufferSize,
                                      const Point<float4> viewpoint )
{
  const GLFont& rGLFont = GetNormalFont();
  Dataset& rDataset = GetDatasetRef();

  // get the current OpenGL matrices
  GLdouble mModelMatrix[16]; glGetDoublev(GL_MODELVIEW_MATRIX,mModelMatrix);
  GLdouble mProjMatrix[16]; glGetDoublev(GL_PROJECTION_MATRIX,mProjMatrix);
  const GLint vViewport[4] = { 0, 0, uRenderSize, uRenderSize };
  const Rect<GLint> viewportRect(vViewport[0], vViewport[1], vViewport[2], vViewport[3]);

  // OpenGL setup
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_1D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Point<float8> winCoordsPoint(0.0,0.0,0.0);
  Point<float4> point(0.0,0.0,0.0);

  // get the cursor position
  CPoint cursorPt( GetMessagePos() );
  ScreenToClient( & cursorPt );
  CRect clientRect;
  GetClientRect( & clientRect );
  cursorPt.y = clientRect.Height() - cursorPt.y;

  // clear the hotspots
  m_findingHotspots.clear();

  // loop over all the cad findings
  vxCadFindings & cadList = rDataset.m_cadFindings;
  int4 iFindingCounter(0);
  for (int4 i(0); i < cadList.GetCount(); i++)
  {
    const vxCadFinding & cadFinding = cadList.GetFinding(i);
    bool bShow = RenderUtil::IsFindingVisible( cadFinding,rDataset.m_session.m_mapCadFindingInfo, rDataset.m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings );
    if ( bShow )
      iFindingCounter++;
    std::map< std::string, ColonSession::FindingInfo >::iterator visIter( rDataset.m_session.m_mapCadFindingInfo.find( cadFinding.GetId() ) );
    if ( visIter == rDataset.m_session.m_mapCadFindingInfo.end() )
    {
      LogErr( "finding not in map", "EndoluminalView", "DrawCadFindings" );
      continue;
    }
    ColonSession::FindingInfo::DecisionEnum eDecision = RenderUtil::GetFindingDecision( cadFinding, rDataset.m_session.m_mapCadFindingInfo );

    if( bShow && (eDecision != ColonSession::FindingInfo::eDecisionDenied) )
    {
     // project the arrow tip to the screen
      Point3D<float4> pos = rDataset.GetNearestSkeletonPosition( cadFinding );
      const float4 fArrowLength( 2.5F );
      const Point3Df cadCentroid( cadFinding.GetCentroidMM().GetVector() );
      Normal<float4> orientation( cadCentroid - pos );

      gluProject( float8(pos.m_x), float8(pos.m_y), float8(pos.m_z),
                  mModelMatrix, mProjMatrix, vViewport,
                  &winCoordsPoint.m_x, &winCoordsPoint.m_y, &winCoordsPoint.m_z);

      // determine whether to draw the current marker
      if (viewportRect.PtInRect(winCoordsPoint.m_x, winCoordsPoint.m_y) /*&& winCoordsPoint.m_z > 0.0f*/)
      {
        // project to the screen and determine Z-order
        const float4 tipScreenPosX( Interpolate( 0.0F, static_cast< float4 >( winCoordsPoint.m_x ), static_cast< float4 >( uRenderSize ),
                                                 0.0F, static_cast< float4 >( clientRect.Width() ) ) );
        const float4 tipScreenPosY( Interpolate( 0.0F, static_cast< float4 >( winCoordsPoint.m_y ), static_cast< float4 >( uRenderSize ),
                                                 0.0F, static_cast< float4 >( clientRect.Height() ) ) );
        float4 fDepthZ( pfDepthBuffer[((int4)(winCoordsPoint.m_y))*uRenderBufferSize + ((int4)(winCoordsPoint.m_x))] );
        float4 fDist( Vector<float4>(pos - viewpoint ).GetLength() );
        const bool bInFront( rDataset.m_submarine.GetOrientation().Dot( Vector3Df(pos - rDataset.m_submarine.GetPositionWorld() ) ) > 0 );
        const bool bVisible( bInFront && fDist > m_fNearClipPlane && fDist <= fDepthZ );
        const bool bNearlyVisible( bInFront && fDist > m_fNearClipPlane && fDist > fDepthZ && fDist <= fDepthZ * 2 );

        // modulate the color and opacity depending on visibility
        PixelRGBA<float4> colorFinding;

        if ( bVisible )
        {
          colorFinding = eDecision == ColonSession::FindingInfo::eDecisionUndecided ? PixelRGBA<float4>( 1.0F, 0.4F, 0.0F, 0.75F ) : PixelRGBA<float4>( 0.0F, 0.2F, 0.0F, 0.75F );

        } // if
        else
        {
          colorFinding = eDecision == ColonSession::FindingInfo::eDecisionUndecided ? PixelRGBA<float4>( 0.5F, 0.2F, 0.0F, 0.60F ) : PixelRGBA<float4>( 0.0F, 0.2F, 0.0F, 0.60F );
        } // else

        // TODO do something if not within lumen!

        // only proceed if visible
        if ( bVisible || bNearlyVisible )
        {
          Point<float4> textPt( pos );//+ Vector<float4>( rDataset.m_submarine.GetUpDirection() ) );
          float4 fArrowOpacity = bNearlyVisible ? 0.6F : 1.0F;
          if ( bNearlyVisible )
          {
            glColor4f( 1, 1, 1, 0.35F );
          }
          else
          {
            glColor4f( 1, 1, 1, 1 );
          }
          colorFinding.A() = fArrowOpacity;
          RenderUtil::RenderArrow( pos, orientation, fArrowLength, true, -1, colorFinding, false );
        } // if nearby enough to show

      } // if within viewport


      // determine the hot spot along the viewing ray to the finding centroid
      Vector3D<float4> vfCurrent(pos);
      Vector3D<float4> vfCentroid( cadCentroid );
      Vector3D<float4> vfNormal( orientation );
      vfNormal = vfNormal.GetNormalized();

      vxBlockVolumeIterator<uint1> iti(rDataset.m_fields.m_ins);
      Triple<float4> vfUnits( rDataset.m_fields.m_sliceRendererData.GetCleansedVolume()->GetHeader().GetVoxelUnits() );
      Triple<int2>   viDim  ( rDataset.m_fields.m_sliceRendererData.GetCleansedVolume()->GetHeader().GetVolDim() );

      Vector3D<int4> viAirVoxelLocation(int4(vfCurrent.m_x/vfUnits.m_x + 0.5F),
                                        int4(vfCurrent.m_y/vfUnits.m_y + 0.5F),
                                        int4(vfCurrent.m_z/vfUnits.m_z + 0.5F));
      iti.SetPos(viAirVoxelLocation);
      const uint1 uu = iti.GetVoxel();

      /// search away from the cammara location along the vfNormal direction and 
      /// stop at the first voxel outside the lumen
      for( int4 j=0; j<200; j++ )
      {
        vfCurrent = vfCurrent + vfNormal;

        viAirVoxelLocation.m_x = int4(vfCurrent.m_x/vfUnits.m_x + 0.5F);
        viAirVoxelLocation.m_y = int4(vfCurrent.m_y/vfUnits.m_y + 0.5F);
        viAirVoxelLocation.m_z = int4(vfCurrent.m_z/vfUnits.m_z + 0.5F);

        if( 0 < viAirVoxelLocation.m_x && viAirVoxelLocation.m_x < viDim.m_x &&
            0 < viAirVoxelLocation.m_y && viAirVoxelLocation.m_y < viDim.m_y &&
            0 < viAirVoxelLocation.m_z && viAirVoxelLocation.m_z < viDim.m_z )
        {
          iti.SetPos(viAirVoxelLocation);          

          if( iti.GetVoxel() != uu )
          { /// reach outside of the colon lumen
            vfCurrent = vfCurrent + vfNormal;
            j = 99999;
          } // if
        } // if
        else
        {
          vfCurrent = Vector3D<float4>(pos);
          j = 99999;
        } // else

      } // for j

      gluProject( float8(vfCurrent.m_x), float8(vfCurrent.m_y), float8(vfCurrent.m_z),
                  mModelMatrix, mProjMatrix, vViewport,
                  &winCoordsPoint.m_x, &winCoordsPoint.m_y, &winCoordsPoint.m_z);

      // determine whether to draw the hot spot
      if (viewportRect.PtInRect(winCoordsPoint.m_x, winCoordsPoint.m_y) /*&& winCoordsPoint.m_z > 0.0f*/)
      {
        // project to the screen and determine Z-order
        const float4 tipScreenPosX( Interpolate( 0.0F, static_cast< float4 >( winCoordsPoint.m_x ), static_cast< float4 >( uRenderSize ),
                                                 0.0F, static_cast< float4 >( clientRect.Width() ) ) );
        const float4 tipScreenPosY( Interpolate( 0.0F, static_cast< float4 >( winCoordsPoint.m_y ), static_cast< float4 >( uRenderSize ),
                                                 0.0F, static_cast< float4 >( clientRect.Height() ) ) );
        float4 fDepthZ( pfDepthBuffer[((int4)(winCoordsPoint.m_y))*uRenderBufferSize + ((int4)(winCoordsPoint.m_x))] );
        float4 fDist( (vfCurrent - Vector<float4>( viewpoint )).GetLength() );
        const bool bInFront( rDataset.m_submarine.GetOrientation().Dot( vfCurrent - Vector3Df( rDataset.m_submarine.GetPositionWorld() ) ) > 0 );
        const bool bVisible( bInFront && fDist > 0.01 && fDist <= fDepthZ*2.0F );

        // modulate the color and opacity depending on visibility
        PixelRGBA<float4> colorFinding;

        if ( bVisible )
        {
          colorFinding = eDecision == ColonSession::FindingInfo::eDecisionUndecided ? PixelRGBA<float4>( 1.0F, 0.4F, 0.0F, 0.75F ) : PixelRGBA<float4>( 0.0F, 0.2F, 0.0F, 0.75F );
        } // if
        else
        {
          colorFinding = eDecision == ColonSession::FindingInfo::eDecisionUndecided ? PixelRGBA<float4>( 0.5F, 0.2F, 0.0F, 0.60F ) : PixelRGBA<float4>( 0.0F, 0.2F, 0.0F, 0.60F );
        } // else

        // TODO do something if not within lumen!

        // only proceed if visible
        if ( bVisible ) //|| bNearlyVisible )
        {
          Point<float4> textPt( vfCurrent.m_x, vfCurrent.m_y, vfCurrent.m_z );//+ Vector<float4>( rDataset.m_submarine.GetUpDirection() ) );

          // draw the text
          float distance = sqrt( powf( tipScreenPosX - cursorPt.x, 2 ) + powf( tipScreenPosY - cursorPt.y, 2 ) );
          float opacity = Interpolate( 0.0F, Bound( 0.0F, distance, 200.0F), 200.0F, 1.0F, 0.35F );
          glColor4f( 1, 1, 1, opacity );
          std::string sText = ToAscii( iFindingCounter ) + " ";
          int4 iPixels = rGLFont.GetPixelWidth( sText );
          int4 iItemWidth = 0;
          if ( visIter->second.GetDecision() == ColonSession::FindingInfo::eDecisionUndecided )
            iItemWidth = m_pFindingImages[1]->GetDim().m_x + m_pFindingImages[2]->GetDim().m_x; // Y and N
          else if ( visIter->second.GetDecision() == ColonSession::FindingInfo::eDecisionAccepted )
            iItemWidth = m_pFindingImages[0]->GetDim().m_x + m_pFindingImages[2]->GetDim().m_x; // ? and N

          // create the hotspots for subsequent interaction
          const bool bWindowEnabled = IsWindowEnabled();
          if ( bWindowEnabled ) 
          {
            FindingHotspot hotspot;
            hotspot.m_hotspot = Rect<int4>( tipScreenPosX + iPixels,              tipScreenPosY,
                                            tipScreenPosX + iPixels + iItemWidth, tipScreenPosY + m_pFindingImages[0]->GetDim().m_y );
            hotspot.m_iNumber = iFindingCounter;
            hotspot.m_fHotspotOpacity = GetHotspotOpacity( hotspot.m_hotspot, cursorPt );
            hotspot.m_fArrowOpacity = 1.0F;
            hotspot.m_findingInfo = visIter->second;
            hotspot.m_fDepth = ( Point3D<float4>(vfCurrent.m_x, vfCurrent.m_y, vfCurrent.m_z) - rDataset.m_submarine.GetPositionWorld() ).GetLength();
            hotspot.m_fCursorDistance = sqrt( powf( hotspot.m_hotspot.CenterPoint().m_x - cursorPt.x, 2 ) + powf( hotspot.m_hotspot.CenterPoint().m_y - cursorPt.y, 2 ) );
            hotspot.m_sID = cadFinding.GetId();
            m_findingHotspots.push_back( hotspot );
          } // if
        } // if nearby enough to show

      } // if within viewport

    } // if finding is visible
  } // endfor all cad findings

  // sort hotspots by distance from cursor
  std::sort( m_findingHotspots.begin(), m_findingHotspots.end(), FindingHotspot::SortAscendingCursorDistance() );

  glDisable(GL_LIGHT0);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable( GL_BLEND );
} // DrawCadFindings()
  

/**
 * Render the endoscopic view.
 *
 * @param pData   Application-defined data (unused).
 */
void EndoluminalView::GLRenderScene(void * pData)
{
  Dataset& myDatasetRef = GetDatasetRef();
  Dataset& oppDatasetRef = GetOppositeDatasetRef();

  if (!myDatasetRef.IsLoadingComplete() || myDatasetRef.m_segmentList.GetCurrentSkeleton() == NULL)
  {
    GLChildView::GLRenderScene();
    return;
  }

  //if (saveCamera==currCamera)  return;
  /*
  bool bAutoFly = ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY;
  if(!GetDatasetRef().m_submarine.IsFlying() && !bAutoFly && m_pUpdateBuffer!= NULL && m_bImageBuffered == true){
  RenderUpdateBuffer();
  return;
  }
  */

  // If space leaping is now off, ensure the uncleansed volume is loaded
  if (!rdrGlobal.m_bSpaceLeaping && !myDatasetRef.m_fields.IsDiffVolLoaded())
  {
    myDatasetRef.m_fields.LazyLoadDiffVolume();
  }

  myDatasetRef.m_fields.m_volumeRender.Render( myDatasetRef, min(GetViewRect().Width(),GetViewRect().Height()), GetInverseImage() );

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {

    // set up camera view
    Point<float> center(GetDatasetRef().m_submarine.GetPositionWorld());
    Normal<float> orientation(myDatasetRef.m_submarine.GetOrientation());
    Vector<float> vUp(myDatasetRef.m_submarine.GetUpDirection());
    Point<float> focusPoint(center + Vector<float>(orientation)*10.0);
    gluLookAt(center.m_x, center.m_y, center.m_z,
              focusPoint.m_x, focusPoint.m_y, focusPoint.m_z,
              vUp.m_x, vUp.m_y, vUp.m_z);

    if ( ViatronStudy::m_modeEndoView == ViewModes::CENTERLINE && 
         ( ( myDatasetRef.m_fields.m_volumeRender.m_eProjectionType == VolumeRender::PERSPECTIVE ) ||
           ( myDatasetRef.m_fields.m_volumeRender.m_eProjectionType == VolumeRender::FRANKS ) ) )
    {
      // decide if the centerline should be red signifying
      // flight in the wrong direction
      const uint2 iCSI = myDatasetRef.m_fields.GetClosestSkeletonIndex( myDatasetRef.m_submarine.GetPositionVolume() );
      Segment & segment = myDatasetRef.m_segmentList.GetCurrentSegment();
      const bool bRegularDirection( segment.IsRegularDirection() );

      const Submarine::NavigationDirection eFlightDirection = myDatasetRef.m_submarine.GetFlightDirection();
      const Normal<float> nSkelDirection(myDatasetRef.m_segmentList.GetCurrentSkeleton().GetSkelNode(iCSI).m_vWorldTan);
      Normal<float> nProperDirection;
      
      if( (eFlightDirection == Submarine::RECTUM_TO_CECUM && bRegularDirection == true ) ||
          (eFlightDirection == Submarine::CECUM_TO_RECTUM && bRegularDirection == false)   )
      {
        nProperDirection = nSkelDirection;
      }
      else
      {
        nProperDirection = -nSkelDirection;
      } // endif start-to-end or end-to-start
      
      if( nProperDirection.Dot(myDatasetRef.m_submarine.GetOrientation()) > -0.707)
      {
        // right way
        if (m_uWrongWayCount > 0) m_uWrongWayCount--;
      }
      else
      {
        // wrong way
        if (m_uWrongWayCount < 2*m_uWrongWayFrameThreshold) m_uWrongWayCount++;
      } // endif right or wrong way
      
       // Default centerline color is green, wrong direction is red
      Triple<uint1> centerLineColor = (m_uWrongWayCount > m_uWrongWayFrameThreshold) ? Triple<uint1>(255,0,0) : Triple<uint1>(0, 255, 0);

      // Draw main centerline
      //Skeleton & skeleton = myDatasetRef.m_segmentList.GetCurrentSkeleton();
      Skeleton & skeleton = myDatasetRef.m_joinedSkeleton; 
      SkeletonRender * pSkelRender = reinterpret_cast<SkeletonRender *> (& skeleton);
      pSkelRender->DrawWithHiddenLines(myDatasetRef.m_fields.m_volumeRender.GetDepthBuffer(),
                                       myDatasetRef.m_fields.m_volumeRender.GetRenderSize(),
                                       myDatasetRef.m_fields.m_volumeRender.GetRenderBufferSize(),
                                       myDatasetRef.m_submarine.GetPositionWorld(),
                                       myDatasetRef.m_vObliqueLocation,
                                       !(myDatasetRef.m_submarine.IsFlying() || oppDatasetRef.m_submarine.IsFlying()),
                                       centerLineColor);
    } // endif draw centerline

    DrawAnnotations(myDatasetRef.m_fields.m_volumeRender.GetDepthBuffer(),
      myDatasetRef.m_fields.m_volumeRender.GetRenderSize(),
      myDatasetRef.m_fields.m_volumeRender.GetRenderBufferSize(),
      myDatasetRef.m_submarine.GetPositionWorld());

    if ( rdrGlobal.m_bDisplayCadFindings || rdrGlobal.m_bAlwaysShowCadArrows )
    {
      DrawCadFindings(myDatasetRef.m_fields.m_volumeRender.GetDepthBuffer(),
        myDatasetRef.m_fields.m_volumeRender.GetRenderSize(),
        myDatasetRef.m_fields.m_volumeRender.GetRenderBufferSize(),
        myDatasetRef.m_submarine.GetPositionWorld());
    }
  
    Render3DMeasurementLines(myDatasetRef.m_submarine.GetPositionWorld());

    if ( EndoluminalView::m_bForceDrawCurrentROI && (GetDatasetRef().m_suspiciousRegions.GetCount() > 0) ) // kludge, to draw current ROI during snapshot
    {
      const vxCadFindings suspiciousRois = GetDatasetRef().m_suspiciousRegions;
      RenderFindingMeasurements(suspiciousRois.GetFinding(suspiciousRois.GetCount()-1), myDatasetRef.m_submarine.GetPositionWorld() );
    }


#if !defined(FINAL_RELEASE)
    // Just for fun, Virtual Laser Polypectomy
    if (m_iRdbcAnim > 0)
    {
      glPushMatrix();
      Point<float> focalPoint(center + Vector<float>(orientation) * m_iRdbcAnim * 200.0);
      Point<float> thisPoint(center + Vector<float>(vUp) * 100.0);
      
      if (m_iRdbcAnim < 3)
      {
        // Ray
        glColor3f(1.0,0.25f,0);
        glBegin(GL_LINES);
        glVertex3f(thisPoint.m_x, thisPoint.m_y, thisPoint.m_z);
        Point3D<float> positionWorld = Calculate3DPoint(m_rdbcPoint);
        glVertex3f(positionWorld.m_x, positionWorld.m_y, positionWorld.m_z);
        glEnd();
      }
      
      if (m_iRdbcAnim == 1)
      {
        // Play blaster sound
        AfxBeginThread(ThreadPlaySoundProc, LPVOID(IDW_BLASTER));
        
        // Erosion
        Point3D<float> positionWorld = Calculate3DPoint(m_rdbcPoint);
        LinVolUC & volume = myDatasetRef.m_fields.m_volumeRender;
        Point<float> positionVolume = volume.GetWorldToVolumeMatrix() * positionWorld;
        const int4 iRadius(25);
        for (int4 z = -iRadius; z <= iRadius; z++)
        {
          for (int4 y = -iRadius; y <= iRadius; y++)
          {
            for (int4 x = -iRadius; x <= iRadius; x++)
            {
              Point<unsigned short> posPt(positionVolume.m_x + x, positionVolume.m_y + y, positionVolume.m_z + z);
              const float fIntensity = Bound(0.0F, Interpolate(0.0F, float(Vector<float>(x,y,z).GetLength()), float(iRadius), 0.0F, 1.0F), 1.0F);
              const float fNewValue = Bound(0.0F, fIntensity * volume.GetVoxel(posPt), 255.0F);
              volume.SetVoxel(posPt, fNewValue);
            }
          }
        }
      }
      
      // advance the animation
      m_iRdbcAnim = (++m_iRdbcAnim % (5+1));
      
      if (m_iRdbcAnim == 5)
      {
        // Explosion
        AfxBeginThread(ThreadPlaySoundProc, LPVOID(IDW_EXPLOSION));
      }
      glPopMatrix();
    }
#endif
    
  } glPopMatrix();
  
  SetImageBuffered( false );
/*
  if(!myDatasetRef.m_submarine.IsFlying() && !bAutoFly && (m_pUpdateBuffer== NULL || m_bImageBuffered == false)){
    SaveUpdateBuffer();
  }
*/
} // GLRenderScene


/**
 * 2-D scene rendering.
 *
 * @param hDC   Device context for rendering.
 */
void EndoluminalView::GLRasterScene()
{
  Dataset& myDatasetRef = GetDatasetRef();
  if (!myDatasetRef.IsLoadingComplete())
  {
    GLChildView::GLRasterScene();
    return;
  }

  // Draw measurement lines
  const Rect<int4> viewRect = GetViewRect();
  const Rect<int4> clientRect = viewRect - viewRect.LeftTop();
  RenderTextMeasurementLines(clientRect);
  const GLFont& rGLFont = GetNormalFont();

  // Set current percentage examined
  glColor3f(1,1,1);

  // only update the lumen coverage progress control if we have exceeded the specified threshold
  const float4 fPercentageThreshold( 0.01F );
  const float4 fCurrentLumenCoverage( myDatasetRef.GetPercentExamined() );
  const float4 fPerc( fCurrentLumenCoverage * 100.0F + 0.5F);  
  if( abs( fCurrentLumenCoverage - m_fPercentCoverage ) > fPercentageThreshold )
  {
    m_progressCtrl.SetPos((int4(fPerc) < 100) ? fPerc : fPerc - 0.5F);
    m_fPercentCoverage = fCurrentLumenCoverage;
  }

  char sCoverage[50]; sprintf(sCoverage, "Lumen:%d%%", int4(fPerc));
  rGLFont.DrawText(clientRect.m_right-2, clientRect.m_top+2, sCoverage, GDT_ABOVELEFT);

  const int4 iTextHeight = 15; // move these to base class
  const int4 iSidePadding = 3;
  const int4 iBottomExtraTextPadding = 2;
#if defined(FINAL_RELEASE)
  if (EndoluminalView::m_bDisplayRenderSpeed)
#endif
  {
    // Display render size
    char sMsg[101];
    //sprintf(sMsg,"%d:%d rbs:%d cnt:%d acc:%d", myDatasetRef.m_fields.m_volumeRender.GetRenderSize(), 
    //                     myDatasetRef.m_fields.m_volumeRender.m_uRenderSizeSum/myDatasetRef.m_fields.m_volumeRender.m_uRenderSizeCount,
    //                     myDatasetRef.m_fields.m_volumeRender.GetRenderBufferSize(),
    //                     myDatasetRef.m_fields.m_volumeRender.m_uRenderSizeCount,
    //                     myDatasetRef.m_fields.m_volumeRender.m_iAccumulationCount);
    sprintf(sMsg,"%d:%d:%d", myDatasetRef.m_fields.m_volumeRender.GetRenderSize(),  myDatasetRef.m_fields.m_volumeRender.m_uRenderSizeSum/myDatasetRef.m_fields.m_volumeRender.m_uRenderSizeCount, myDatasetRef.m_fields.m_volumeRender.GetNumberOfRenderThreads() );
    rGLFont.DrawText(clientRect.m_right-2, clientRect.m_bottom, sMsg, GDT_BELOWLEFT);
    sprintf(sMsg, "%.1fHz", myDatasetRef.m_fields.m_volumeRender.m_fFramesPerSecond);
    rGLFont.DrawText(clientRect.m_right-2, clientRect.m_bottom-2 - iTextHeight, sMsg, GDT_BELOWLEFT);
  }
  
  if (!rdrGlobal.m_bSpaceLeaping)
  {
    static const std::string sMsg(LPCTSTR(LoadResourceString(myDatasetRef.GetModality() == Dataset::MR ? IDS_DISPLAYING_UNSUBTRACTED : IDS_DISPLAYING_UNCLEANSED)));
    static const std::string sMsgVerbose(LPCTSTR(LoadResourceString(myDatasetRef.GetModality() == Dataset::MR ? IDS_DISPLAYING_UNSUBTRACTED_VERBOSE : IDS_DISPLAYING_UNCLEANSED_VERBOSE)));

    int4 iPixelWidth( m_medFont.GetPixelWidth( sMsgVerbose) );
    if (iPixelWidth > clientRect.Width())
      m_medFont.DrawText(clientRect.m_left, clientRect.m_top+3, sMsg, GDT_ABOVERIGHT);
    else
      m_medFont.DrawText((clientRect.Width() - iPixelWidth)*0.5F, clientRect.m_top+3, sMsgVerbose, GDT_ABOVERIGHT);
  }
  else
  {
    GLRenderLogo();
  }

  float4 fMaxOpacity = 0.0F;

  if ( rdrGlobal.m_bDisplayCadFindings )
  {
    // get cursor location
    CPoint cursorPt( GetMessagePos() );
    ScreenToClient( & cursorPt );
    CRect clientRect2;
    GetClientRect( & clientRect2 );
    cursorPt.y = clientRect2.Height() - cursorPt.y;

    // render hotspots
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for ( std::vector< FindingHotspot >::iterator hotspotIter = m_findingHotspots.begin();
          hotspotIter != m_findingHotspots.end();
          ++ hotspotIter )
    {

      FindingHotspot hotspot = * hotspotIter;
      Rect<int4> rect = hotspot.m_hotspot;

      // compute opacity based on distance from cursor
      const float4 fOpacity = GetHotspotOpacity( rect, cursorPt );

      // determine the text string to display
      std::string sNumber = ToAscii( hotspot.m_iNumber );
      int4 iTextPos = rect.GetMinX() - m_medFont.GetPixelWidth( sNumber );

      // draw text with a shadow
      const float4 fNudgeY( 10 );
      if ( rdrGlobal.m_bDisplayCadFindings )
      {
        glColor4f( 0, 0, 0, hotspot.m_fArrowOpacity );
        m_medFont.DrawText( iTextPos + 1, rect.GetMinY() - 1 + fNudgeY, sNumber );
        glColor4f( 1, 1, 1, hotspot.m_fArrowOpacity );
        m_medFont.DrawText( iTextPos, rect.GetMinY() + fNudgeY, sNumber );
      }

      // only display hotspot if it is the closest one to the cursor
      // (this is determined previously and encoded by the opacity
      // (only one has opacity > 0)
      if ( IsMainViewWindow() && hotspot.m_fArrowOpacity == 1.0F && hotspotIter == m_findingHotspots.begin() )
      {
        // select the images to display
        int4 iImagesToDisplay[2];
        if ( hotspot.m_findingInfo.GetDecision() == ColonSession::FindingInfo::eDecisionUndecided )
        {
          iImagesToDisplay[0] = 1;
          iImagesToDisplay[1] = 2;
        }
        else if ( hotspot.m_findingInfo.GetDecision() == ColonSession::FindingInfo::eDecisionAccepted )
        {
          iImagesToDisplay[0] = 0;
          iImagesToDisplay[1] = 2;
        }

        // display the images
        int4 iRasterX = rect.GetMinX();
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glPixelStorei( GL_PACK_ALIGNMENT, 1 );
        for ( int4 iImageNumber(0); iImageNumber<2; ++iImageNumber )
        {

          // prepare the image
          const int4 iImageToDisplay = iImagesToDisplay[ iImageNumber ];
          const int4 iImageWidth( m_pFindingImages[iImageToDisplay]->GetDim().m_x );
          const int4 iImageHeight( m_pFindingImages[iImageToDisplay]->GetDim().m_y );

          // highlight the click area for clarity
          Rect<int4> imageRect( iRasterX, rect.GetMinY(), iRasterX + iImageWidth, rect.GetMinY() + iImageHeight );
          const float4 fHighlightOpacity( Bound( 0.0F, imageRect.PtInRect( cursorPt.x, cursorPt.y ) ? fOpacity * 0.6F : fOpacity * 0.2F, 1.0F ) );
          glLineWidth( 1.0F );
          glColor4f( 1, 1, 1, fHighlightOpacity );
          glBegin( GL_QUADS );
          glVertex2i( imageRect.GetMinX(), imageRect.GetMinY() );
          glVertex2i( imageRect.GetMaxX(), imageRect.GetMinY() );
          glVertex2i( imageRect.GetMaxX(), imageRect.GetMaxY() );
          glVertex2i( imageRect.GetMinX(), imageRect.GetMaxY() );
          glEnd();

          glPixelStorei( GL_UNPACK_ROW_LENGTH, iImageWidth );
          glPixelTransferf( GL_ALPHA_SCALE, fOpacity );
          glRasterPos2i( iRasterX, rect.GetMinY() );
          try
          {
            glDrawPixels( iImageWidth, iImageHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                          reinterpret_cast< GLubyte * >( m_pFindingImages[iImageToDisplay]->GetDataPtr() ) );
          }
          catch ( ... )
          {
            LogDbg( "unhanded exception when rendering bitmap", "EndoluminalView", "GLRasterScene" );
          }
          iRasterX += m_pFindingImages[iImageToDisplay]->GetDim().m_x;
        }

      } // if ( IsMainViewWindow() )

      fMaxOpacity = max( fMaxOpacity, fOpacity );

    } // foreach hotspot

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glPixelTransferf( GL_ALPHA_SCALE, 1.0F );
  } // if displaying CAD marks

  // render the measurement text at the lower left, if any
  if ( ! m_vsMeasurementText.empty() )
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    if ( fMaxOpacity == 0.0F ) fMaxOpacity = 1.0F;
    glColor4f( 1, 1, 1, fMaxOpacity );
    int y = clientRect.m_top + m_iTextPadding + m_iBottomExtraTextPadding + m_medFont.GetPixelHeight();
    for ( std::list< std::string >::reverse_iterator iter = m_vsMeasurementText.rbegin();
        iter != m_vsMeasurementText.rend();
        ++iter )
    {
      if ( iter->length() > 0 )
      {
        //glColor4f( 1, 1, 1, 1 );
        glColor4f( 0, 0, 0, 1 );
        m_medFont.DrawText( m_iTextPadding, y, * iter );
        y += m_medFont.GetPixelHeight();
      }
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glPixelTransferf( GL_ALPHA_SCALE, 1.0F );
  } // if any measurement text

  if (IsKeyStateDown('F') )
  {
    std::string sMsg;
    sMsg = "FOV: " + ToAscii(GetDatasetRef().m_fields.m_volumeRender.m_fFovDegrees) + " (90) deg";
    rGLFont.DrawText(2, clientRect.m_bottom, sMsg, GDT_BELOWRIGHT);
  }

  if ( IsReportRendering() )
  {
    glPushAttrib(GL_CURRENT_BIT);
    CRect viewRect( GetViewport().m_left, GetViewport().m_top, GetViewport().m_right, GetViewport().m_bottom );
    CRect clientRect = viewRect - viewRect.TopLeft();
    glColor3f(0,1,0);
    std::string sPatInfo = GetDatasetRef().m_studyinfo.m_sPatientID + ", " + GetDatasetRef().m_studyinfo.m_sExamTime;

    const int4 iAdjFontSz(min(-1 * viewRect.Width() * 0.03, -11));
    GLFont fontAdj; fontAdj.Create( GetHDC(), iAdjFontSz, 0, "arial", FW_SEMIBOLD, false, false, true );
    fontAdj.DrawText(m_iTextPadding, clientRect.bottom - m_iTextPadding, sPatInfo.c_str(), GDT_BELOW); // top-left
    glPopAttrib();
  }

} // GLRasterScene()


/**
 * Called during view resize, allows validation/alteration of new size
 *
 * @param uUnsued    Placeholder to keep similarity to the analogous WM_SIZING event parameters (for possible future use)
 * @param viewRect   The proposed new view rectangle
 */
void EndoluminalView::OnSizing(const UINT uUnused, Rect<int4>& viewRect)
{
  const int4 iWidth(viewRect.Width());
  const int4 iHeight(viewRect.Height());

  if (iHeight > iWidth)
  {
    const int4 iDiff(iHeight - iWidth);
    viewRect.Deflate(0, iDiff/2);
  }
  else if (iWidth > iHeight)
  {
    const int4 iDiff(iWidth - iHeight);
    viewRect.Deflate(iDiff/2, 0);
  }
} // OnSizing()


/**
 * Called when view is resized.
 *
 * @param iWidth    Width of view.
 * @param iHeight   Height of view.
 */
void EndoluminalView::GLResize(GLsizei iWidth, GLsizei iHeight)
{
 
  if (iHeight == 0)
  {
    iHeight = 1;
  }
  glOrtho(0, 1, 0, 1, 0, -1);
  glViewport(0,0,iWidth,iHeight);

  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  const GLfloat fAspect(GLfloat(iWidth) / GLfloat(iHeight));
  // modify the new clip plane to adjust the amount of tail wagging
  gluPerspective(GetDatasetRef().m_fields.m_volumeRender.m_fFovDegrees, fAspect, m_fNearClipPlane, 886.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

} // GLResize()


/**
 * Idle-time processing.
 *
 * @return   Whether or not to re-render the scene.
 */
bool EndoluminalView::GLIdle()
{
  Dataset& myDatasetRef = GetDatasetRef();
  if (!myDatasetRef.IsLoadingComplete())
    return false;

#if !defined(FINAL_RELEASE)
    if( m_iRdbcAnim > 0 )
      return true;
#endif

  myDatasetRef.m_submarine.UpdatePosition();
  if (myDatasetRef.m_submarine.IsFlying())
  {
    InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
    return true;
  }
 /* else if ( myDatasetRef.m_fields.m_volumeRender.GetRenderSize() < VolumeRender::GetMaxRenderSize() )
  {
    InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
    return true;
  }
*/
  return false;
} // GLIdle()


/////////////////////////////////////////////////////////////////////////////////////////////////
// EndoluminalView mouse message handlers


/**
 * Left button down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnLButtonDown(UINT uFlags, CPoint point) 
{
  // turn off 2d flying
  if (ViatronStudy::m_modeOrthogView & ViewModes::AUTOFLY2D)
    ViatronStudy::m_modeOrthogView -= ViewModes::AUTOFLY2D;
  if (ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D)
    ViatronStudy::m_modeObliqueView -= ViewModes::AUTOFLY2D;

  if (!IsMainViewWindow())
  {
    GLChildView::OnLButtonDown(uFlags, point);
    return;
  }

  // get the cursor position
  CPoint cursorPt( GetMessagePos() );
  ScreenToClient( & cursorPt );
  CRect clientRect;
  GetClientRect( & clientRect );
  cursorPt.y = clientRect.Height() - cursorPt.y;

  if (rdrGlobal.m_pCurrDataset->GetOrientation() != GetDatasetRef().GetOrientation())
  {
    AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM(GetDatasetRef().GetOrientation()), 0);
  }

  // All subsequent mouse input will be sent to this view
  SetCapture();
  
  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
    case ViewModes::LINEMEASURE:
    {
      if (PickupMeasurementLine(clientRect, CPoint(point.x, point.y-iPickupShiftY), true) == true)
      {
        // a exist measurement line is picked up and will be adjusted.
      }
      else
      {
        // A start point for a new line has been clicked.
        MeasureLine measLine;
        Point2D<int4> point2D(point.x, point.y-iPickupShiftY);
        measLine.m_measureEnds.m_start = measLine.m_measureEnds.m_end = CalculateVisible3DPoint(point2D);
        measLine.m_drawEnds.m_start = measLine.m_drawEnds.m_end = point2D;
        measLine.StoreDistance();
        
        GetDatasetRef().m_session.m_measureLineList.push_front(measLine);
        GetDatasetRef().m_session.m_measureLineList.m_currentIter = GetDatasetRef().m_session.m_measureLineList.begin();
      }

      break;
    }
    case ViewModes::LIGHTING:
      // Do nothing, as opposed to default case which begins manual flight.
      break;
    
    case ViewModes::WINDOWLEVEL:
      // Do nothing, as opposed to default case which begins manual flight.
      break;
    
    case ViewModes::BIOPSY:
       m_lastBiopsyPt = Point2D<int4>(point.x, point.y);
      break;

    case ViewModes::ANNOTATION:
    {
      Bookmark antn;
      antn.m_sName = TextPromptWnd::GetText(LPCTSTR(LoadResourceString(IDS_ANNOTATION_TEXT_PROMPT)), std::string(), TextPromptWnd::eSingleLine);
      antn.m_position = Calculate3DPoint(Point2D<float>(point.x, point.y));
      if (antn.m_sName.empty())
      {
        //ViatronStudy::m_modeEndoView -= ViewModes::ANNOTATION;
        AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,0), 0);
        break;
      }

      // {{ annotation position (note: hack to locate it 25% from viewpoint to the wall)
      Vector<float4> eyePos = Vector<float4>(GetDatasetRef().m_submarine.GetPositionWorld());
      Vector<float4> vecEyeToNote;
      vecEyeToNote = eyePos + Vector<float4>(antn.m_position);
      eyePos = vecEyeToNote*0.5F;
      vecEyeToNote = eyePos + Vector<float4>(antn.m_position);
      antn.m_position = Point<float4>(vecEyeToNote*0.5F);
      // }} annotation position

      antn.m_orientation = Normal<float4>((Vector<float4>(antn.m_position) - eyePos).GetNormalized());
      //antn.m_orientation = GetDatasetRef().m_submarine.GetOrientation();
      GetDatasetRef().m_session.m_2DAnnotationList.AddBookmark(antn);
      //ViatronStudy::m_modeEndoView -= ViewModes::ANNOTATION;
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,0), 0);
      GetDatasetRef().m_vOrthogonalPtWorld = antn.m_position;
      break;
    }
    
    case ViewModes::SEGMENTPOLYP:
    {
      if( uFlags & MK_CONTROL && rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::OutputSuspiciousROI()))
      {
        RecordAPointOnly(Calculate3DPoint(Point2D<float>(point.x, point.y)));
      }
      else
      {
        SegmentRegion(Calculate3DPoint(Point2D<float>(point.x, point.y)));
      }
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, 0), 0);
			ViatronStudy::m_modeEndoView -= ViewModes::SEGMENTPOLYP;
      break;
    }

    case ViewModes::AUTOFLY:
    {
      // Turn off autofly mode, then begin manual flying by dropping down to default case below
      ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
    
      // DO NOT break!!  Continue on to the default (interactive fly) case
    }

    // FALL THRU FROM AUTOFLY BLOCK TO DEFAULT/INTERACTIVE FLIGHT MODE...LEAVE EMPTY

    default:
    {
      // test all the hotspots before starting flight
      m_bSelectingHotspot = false;
      for ( std::vector< FindingHotspot >::reverse_iterator hotspotIter = m_findingHotspots.rbegin();
            hotspotIter != m_findingHotspots.rend() && ! m_bSelectingHotspot;
            ++ hotspotIter )
      {
        FindingHotspot hotspot = * hotspotIter;
        Rect<int4> rect = hotspot.m_hotspot;
        if ( rect.PtInRect( cursorPt.x, cursorPt.y ) )
        {
          m_bSelectingHotspot = true;
          if ( hotspot.m_findingInfo.GetDecision() == ColonSession::FindingInfo::eDecisionAccepted )
          {
            if ( cursorPt.x < rect.GetMinX() + m_pFindingImages[0]->GetDim().m_x )
              Utilities::CAD::SetDisplayDecision( GetDatasetRef(), hotspot.m_sID, ColonSession::FindingInfo::eDecisionUndecided );
            else
              Utilities::CAD::SetDisplayDecision( GetDatasetRef(), hotspot.m_sID, ColonSession::FindingInfo::eDecisionDenied );
          }
          else if ( hotspot.m_findingInfo.GetDecision() == ColonSession::FindingInfo::eDecisionUndecided )
          {
            if ( cursorPt.x < rect.GetMinX() + m_pFindingImages[1]->GetDim().m_x )
              Utilities::CAD::SetDisplayDecision( GetDatasetRef(), hotspot.m_sID, ColonSession::FindingInfo::eDecisionAccepted );
            else
              Utilities::CAD::SetDisplayDecision( GetDatasetRef(), hotspot.m_sID, ColonSession::FindingInfo::eDecisionDenied );
          }
        }
      } // for all the hotspots

      if ( ! m_bSelectingHotspot )
      {
        if ( uFlags & MK_CONTROL )
        {
          GetDatasetRef().m_vOrthogonalPtWorld = Calculate3DPoint(Point2D<float>(point.x, point.y));
              
          // update the cross-section sliding bar position    
          GetDatasetRef().m_iXsectionScrollBarLocationIndex = GetDatasetRef().m_joinedSkeleton.ConvertToJoinedIndex( GetDatasetRef().m_segmentList.GetCurrentSegment(), 
                                                       GetDatasetRef().m_fields.GetClosestSkeletonIndex(GetDatasetRef().m_submarine.GetPositionVolume()) );

          // TRACE("3dpt:(%.02f,%.02f,%.02f\n", pt3d.m_x,pt3d.m_y,pt3d.m_z);

          if (m_timerOrthogonalPtFix.IsSet())
          {
            OnTimer(m_timerOrthogonalPtFix);
          }

          m_timerOrthogonalPtFix.SetTimer(*this, 2000);
          m_vbSliceShadowStateSave[0] = rdrGlobal.m_bAxialShadow;
          m_vbSliceShadowStateSave[1] = rdrGlobal.m_bSagittalShadow;
          m_vbSliceShadowStateSave[2] = rdrGlobal.m_bCoronalShadow;
          m_vbSliceShadowStateSave[3] = rdrGlobal.m_bObliqueShadow;
          rdrGlobal.m_bAxialShadow = rdrGlobal.m_bSagittalShadow = rdrGlobal.m_bCoronalShadow = rdrGlobal.m_bObliqueShadow = true;
        }
        else if ( ! rdrGlobal.m_bShowHomogenity )
        {
          const float fHeading = Interpolate( 0.0F, point.x, clientRect.Width()  - 1,  1.0F, -1.0F );
          const float fPitch   = Interpolate( 0.0F, point.y, clientRect.Height() - 1, -1.0F,  1.0F );
          float fThrust  = Bound( 0.0F, static_cast< float4 >( 1.0F - sqrt( fHeading * fHeading + fPitch * fPitch ) ), 1.0F );
          //GetDatasetRef().m_submarine.FullStop();
          GetDatasetRef().m_submarine.StartFlying();
          GetDatasetRef().m_submarine.SetForces( fThrust, fHeading, fPitch );
        } // endif set orthopos
      }
      break;
    }
  } // endswitch m_currMode;

  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnLButtonDown()


/**
 * Left button up handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnLButtonUp(UINT uFlags, CPoint point) 
{
  m_bSelectingHotspot = false;

  if (!IsMainViewWindow())
  {
    GLChildView::OnLButtonUp(uFlags, point);
    return;
  }

  // release mouse input capture
  ReleaseCapture();
  
  // get the cursor position
  CPoint cursorPt( GetMessagePos() );
  ScreenToClient( & cursorPt );
  CRect clientRect;
  GetClientRect( & clientRect );
  cursorPt.y = clientRect.Height() - cursorPt.y;

  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
  case ViewModes::LINEMEASURE:
    // remove zero-length measurement lines
    if ( GetDatasetRef().m_session.m_measureLineList.m_currentIter != GetDatasetRef().m_session.m_measureLineList.end() )
    {
      // if start is within a few pixels of the end point, remove it
      Point2D<int4> start = GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_drawEnds.m_start;
      Point2D<int4> end = GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_drawEnds.m_end;
      const float4 fThresholdDistance( 3 );
      if ( abs( pow( start.m_x - end.m_x, 2.0 ) + pow( start.m_y - end.m_y, 2.0 ) ) < fThresholdDistance )
      {
        GetDatasetRef().m_session.m_measureLineList.erase( GetDatasetRef().m_session.m_measureLineList.m_currentIter );
      }
			else
			{
			  if (GetDatasetRef().m_session.GetVersion() > 4.1F)
			  { /// this infor is available for session version 4.2 or later
			  	GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_position = GetDatasetRef().m_submarine.GetPositionWorld();
				  GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_orientation = GetDatasetRef().m_submarine.GetOrientation();
			  	GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_upDirection = GetDatasetRef().m_submarine.GetUpDirection();
			  }// if
			}
    }
    GetDatasetRef().m_session.m_measureLineList.m_currentIter = GetDatasetRef().m_session.m_measureLineList.end();
    break;
  case ViewModes::LIGHTING:
    // Do nothing, as opposed to default case which ends manual flight.
    break;
    
  case ViewModes::WINDOWLEVEL:
    // Do nothing, as opposed to default case which ends manual flight.
    break;
    
  case ViewModes::BIOPSY:
    // Do nothing, as opposed to default case which ends manual flight.
    break;
    
  default:
    // stop flying only if we are the only button left held down
    if (!(uFlags & (MK_MBUTTON | MK_RBUTTON)))
    {
      GetDatasetRef().m_submarine.StopFlying();
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
    }
    break;
    
  } // endswitch m_currMode;
  
  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnLButtonUp()


/**
 * Left button double-click handler.
 *
 * @param uFlags   Mouse-button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnLButtonDblClk(UINT uFlags, CPoint point)
{
  if (!IsMainViewWindow())
  {
    LogDbg( "Not main window!", "EndoluminalView", "OnLButtonDblClk" );
    GLChildView::OnLButtonDblClk(uFlags, point);
    return;
  }

  Dataset& myDataset = GetDatasetRef();
  if (rdrGlobal.m_pCurrDataset->GetOrientation() != myDataset.GetOrientation())
  {
    LogDbg( "Not right dataset!", "EndoluminalView", "OnLButtonDblClk" );
    AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM(myDataset.GetOrientation()), 0);
  }

  CRect rect;
  GetClientRect(&rect);

  // Start autofly when appropriate
  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
  case ViewModes::AUTOFLY:
    break;
    
  default:
    ViatronStudy::m_modeEndoView += ViewModes::AUTOFLY;
    break;
    
  } // endswitch m_currMode;

  myDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnLButtonDblClk()


/**
 * Returns a 3-D coordinate from the specified 2-D point.
 *
 * @param point   X and y coordinates of the 2-D point.
 * @return        Corresponding 3-D point.
 */
Point<float4> EndoluminalView::Calculate3DPoint(const Point2D<float4> & point) const
{
  const uint4 uRenderSize(GetDatasetRef().m_fields.m_volumeRender.GetRenderSize());
  const uint4 uRenderBufferSize(GetDatasetRef().m_fields.m_volumeRender.GetRenderBufferSize());

  CRect rect;
  GetClientRect(&rect);

  float4 xScl = float4(uRenderSize)/float4(rect.Width());
  float4 yScl = float4(uRenderSize)/float4(rect.Height());

  uint4 x = point.m_x*xScl;
  uint4 y = (rect.Height()-point.m_y)*yScl;

  float4 fovRadians = GetDatasetRef().m_fields.m_volumeRender.m_fFovDegrees*3.14159F/180.0F;
  float4 twoOverSize = 2.0f / float4(uRenderSize);
  float4 tanFov = float4( tan(fovRadians/2) );

  Point<float> viewpoint = GetDatasetRef().m_submarine.GetPositionWorld();
  Normal<float> vViewDirection = GetDatasetRef().m_submarine.GetOrientation();
  Vector<float> vUpDirection = Vector<float>(GetDatasetRef().m_submarine.GetUpDirection());
  Normal<float> vRight = Normal<float>(vViewDirection.Cross(vUpDirection));

  float dv = float(y) * twoOverSize - 1.0F;
  Vector<float> vRowDirection = Vector<float>(vViewDirection) + vUpDirection * dv * tanFov;

  // adjust the mapping, ???
  float4 fFactor = (float4)rect.Width() / (float4)rect.Height();
  float du = (float(x) * twoOverSize - 1.0F) * fFactor;
  Vector<float> vRayDirection = vRowDirection + vRight * du * tanFov;

  // calculate the z-depth by casting a ray and finding the maximum opacity point
  Point3D<float4> opaquePoint;
  bool bCleansed( rdrGlobal.m_bSpaceLeaping );
	if( !bCleansed && rdrGlobal.m_bUseULDvolume )
	{
		bCleansed = true;
	}
  Point3D<float4> startPointVolume( viewpoint.GetDividedAnIso( GetDatasetRef().m_fields.m_volumeRender.GetUnits() ) );
  Point3D<float4> farPointWorld( viewpoint + Vector3Df( Normal3Df( vRayDirection ) ) * 100000 );
  Point3D<float4> endPointVolume( farPointWorld.GetDividedAnIso( GetDatasetRef().m_fields.m_volumeRender.GetUnits() ) );
  GetDatasetRef().m_fields.m_volumeRender.CalculateAccumulatedOpacity(
    startPointVolume, endPointVolume, GetDatasetRef().m_surfaceTransferFunction.m_pfOpacity, bCleansed, opaquePoint );

  return opaquePoint.GetScaledAnIso( GetDatasetRef().m_fields.m_volumeRender.GetUnits() );
} // Calculate3DPoint()


/**
 * Returns a 2-D coordinate from the 3-D point: inverse of Calculate3DPoint.
 *
 * @param point   X and y coordinates of the 3D point.
 * @return        Corresponding 2D point.
 */
Point2D<int4> EndoluminalView::Project3DPoint(const Point<float4> & point) const
{
  CRect rect;
  GetClientRect(&rect);

  const uint4 uRenderSize(GetDatasetRef().m_fields.m_volumeRender.GetRenderSize());
  float4 xScl = float4(uRenderSize)/float4(rect.Width());
  float4 yScl = float4(uRenderSize)/float4(rect.Height());

  float4 fovRadians = GetDatasetRef().m_fields.m_volumeRender.m_fFovDegrees*3.14159F/180.0F;
  float4 twoOverSize = 2.0f / float4(uRenderSize);
  float4 tanFov = float4( tan(fovRadians/2) );

  Point<float>  viewpoint = GetDatasetRef().m_submarine.GetPositionWorld();
  Normal<float> vViewDirection = GetDatasetRef().m_submarine.GetOrientation();
  Vector<float> vUpDirection = Vector<float>(GetDatasetRef().m_submarine.GetUpDirection());
  Normal<float> vRight = Normal<float>(vViewDirection.Cross(vUpDirection));

  // 1> Pt = viewpoint + vRayDirection * ZDepth; 
  // 2> vRayDirection = vRowDirection + vRight * du * tanFov;
  // 3> vRowDirection = vViewDirection + vUpDirection * dv * tanFov;
  // 1>+2>+3>=>4> Pt = viewpoint + (vViewDirection + vUpDirection * dv * tanFov + vRight * du * tanFov) * ZDepth; 
  //   Pt.m_x = viewpoint.m_x + (vViewDirection.m_x + vUpDirection.m_x * dv * tanFov + vRight.m_x * du * tanFov) * ZDepth;
  //   Pt.m_y = viewpoint.m_y + (vViewDirection.m_y + vUpDirection.m_y * dv * tanFov + vRight.m_y * du * tanFov) * ZDepth;
  //   Pt.m_z = viewpoint.m_z + (vViewDirection.m_z + vUpDirection.m_z * dv * tanFov + vRight.m_z * du * tanFov) * ZDepth;

  float x0 = point.m_x - viewpoint.m_x;   // x0 = Pt.m_x - viewpoint.m_x
  float y0 = point.m_y - viewpoint.m_y;   // y0 = Pt.m_y - viewpoint.m_y
  float z0 = point.m_z - viewpoint.m_z;   // z0 = Pt.m_y - viewpoint.m_z

  float a0 = vViewDirection.X();
  float b0 = vViewDirection.Y();
  float c0 = vViewDirection.Z();

  // test if the point is outside of view volume.
  float t0 = y0 * c0 - z0 * b0;
  float t1 = z0 * a0 - x0 * c0;
  float t2 = x0 * b0 - y0 * a0;
  
  float fCrossProduct = sqrt(t0*t0 + t1*t1 + t2*t2);
  float fDotProduct = x0*a0 + y0*b0 + z0*c0;
  float fDistanceX = sqrt(x0*x0 + y0*y0 + z0*z0);
  float fDistanceA = sqrt(a0*a0 + b0*b0 + c0*c0);

  float Sin = fCrossProduct / (fDistanceX * fDistanceA);
  float Cos = fDotProduct / (fDistanceX * fDistanceA);

  // FOV = 90. Sin(90/2)=0.707F, Cos(90/2)=0.707F
  if ((Sin> 0.707F) || (Cos < 0.707F))
  {
    // not in view volume.
    return Point2D<int4>(-99, -99);
  }

  float a1 = vUpDirection.m_x * tanFov;
  float b1 = vUpDirection.m_y * tanFov;
  float c1 = vUpDirection.m_z * tanFov;

  float a2 = vRight.X() * tanFov;
  float b2 = vRight.Y() * tanFov;
  float c2 = vRight.Z() * tanFov;

  //                               x0                 y0              z0
  // remove ZDepth: ZDepth = --------------  +  -------------- + --------------
  //                         a0+a1*dv+a2*du     a0+a1*dv+a2*du   c0+c1*dv+c2*du

  //      c0*y0 - b0*z0 + (v1*y0 - b1*z0)*dv   x0*b0 - a0*y0 + (x0*b1 - a1*y0)*dv
  // du = ---------------------------------- = ----------------------------------
  //              b2*z0 - c2*y0                        a2*y0 - b2*x0

  //      m1 + m2*dv   n1 + n2*dv
  // du = ---------- = ----------
  //          m0          n0     
  
  float m0 = b2 * z0 - c2 * y0;
  float m1 = c0 * y0 - b0 * z0;
  float m2 = c1 * y0 - b1 * z0;

  float n0 = a2 * y0 - b2 * x0;
  float n1 = b0 * x0 - a0 * y0;
  float n2 = b1 * x0 - a1 * y0;

  float dv = (m0 * n1 - n0 * m1) / (m2 * n0 - m0 * n2);
  float du = (m1 + m2 * dv ) / m0;

  float y = rect.Height() - (dv + 1.0F) / twoOverSize / yScl; // correction needed because of non-unique W & H
  float x = (du*0.9805F + 1.0F) / (twoOverSize) / xScl + 2.0F;

  return Point2D<int4>(x, y);
} // Project3DPoint()


/**
 * test if a 3D point is visible at current viewing.
 *
 * @param Pt3D    reference to a 3D coordinates
 * @param Pt2D    if visible, return the projection of 3D pt.
 * @return        true when visible.
 */
bool EndoluminalView::Is3DPtVisible(const Point<float4> & Pt3D, CPoint & Pt2D)const
{

  // convert the viewpoint into volume coordinates
  const Point<float4> volumePosViewpoint(
    GetDatasetRef().m_submarine.GetPositionWorld().GetDividedAnIso(
    GetDatasetRef().m_fields.m_volumeRender.GetUnits() ) );

  // move the target point about 3mm closer to the viewpoint
  const float4 fCloser( 3 );
  const Normal<float4> viewFromTargetDir( GetDatasetRef().m_submarine.GetPositionWorld() - Pt3D );
  const Point<float4> newTargetWorld( Pt3D + Vector3Df( viewFromTargetDir ) * fCloser );
  const Point<float4> volumePosTarget( newTargetWorld.GetDividedAnIso( GetDatasetRef().m_fields.m_volumeRender.GetUnits() ) );

  // calculate the accumulated opacity
  Point<float4> opaquePoint;
  bool bCleansed( rdrGlobal.m_bSpaceLeaping );
	if( !bCleansed && rdrGlobal.m_bUseULDvolume )
	{
		bCleansed = true;
	}

  const float4 fAccumulatedOpacity( GetDatasetRef().m_fields.m_volumeRender.CalculateAccumulatedOpacity(
    volumePosViewpoint, volumePosTarget, GetDatasetRef().m_surfaceTransferFunction.m_pfOpacity, bCleansed, opaquePoint ) );

  // return visible if less then fully opaque
  const bool bVisible = fAccumulatedOpacity < 1;

  // compute the projected position
  if ( bVisible )
  {
    Point2D<int4> projectedPosition = Project3DPoint( Pt3D );
    Pt2D.x = projectedPosition.m_x;
    Pt2D.y = projectedPosition.m_y;
  }

  return bVisible;

} // Is3DPtVisible()


/**
 * Middle button down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnMButtonDown(UINT uFlags, CPoint point) 
{
  if (!IsMainViewWindow())
  {
    GLChildView::OnMButtonDown(uFlags, point);
    return;
  }
  
  // All subsequent mouse input will be sent to this view
  SetCapture();
  
  CRect rect;
  GetClientRect(&rect);
  
  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
  case ViewModes::LINEMEASURE:
    // Do nothing, as opposed to default case which begins manual flight.
    break;
    
  case ViewModes::LIGHTING:
    // Do nothing, as opposed to default case which begins manual flight.
    break;
    
  case ViewModes::WINDOWLEVEL:
    // Do nothing, as opposed to default case which begins manual flight.
    break;
    
  case ViewModes::BIOPSY:
    // Do nothing, as opposed to default case which begins manual flight.
    break;
    
  default:
    // Pan if ctrl key is down, otherwise rotate
    GetDatasetRef().m_submarine.SetPanMode((uFlags & MK_CONTROL) > 0);
    float heading = Interpolate(0.0F, point.x,rect.Width() -1.0F,  1.0F, -1.0F);
    float pitch   = Interpolate(0.0F, point.y,rect.Height()-1.0F, -1.0F,  1.0F);
    GetDatasetRef().m_submarine.SetHeading(heading);
    GetDatasetRef().m_submarine.SetPitch(pitch);
    if (GetDatasetRef().m_submarine.IsFlying() == false)
      GetDatasetRef().m_submarine.StartFlying();
    break;
    
  } // endswitch m_currMode;

  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnMButtonDown()


/**
 * Middle button up handler.
 *
 * @param uFlags    Mouse button and key flags.
 * @param point     Point of click.
 */
void EndoluminalView::OnMButtonUp(UINT uFlags, CPoint point) 
{

  // release mouse input capture
  ReleaseCapture();
  
  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
  case ViewModes::LINEMEASURE:
    // Do nothing, as opposed to default case which ends manual flight.
    break;

  case ViewModes::LIGHTING:
    // Do nothing, as opposed to default case which ends manual flight.
    break;

  case ViewModes::WINDOWLEVEL:
    // Do nothing, as opposed to default case which ends manual flight.
    break;

  case ViewModes::BIOPSY:
    // Do nothing, as opposed to default case which ends manual flight.
    break;

  default:
    // stop flying only if we are the only button left held down
    if (!(uFlags & (MK_LBUTTON | MK_RBUTTON)))
    {
      GetDatasetRef().m_submarine.StopFlying();
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
    }
    GetDatasetRef().m_submarine.SetPanMode(false);
    break;
    
  } // endswitch m_currMode;

  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnMButtonUp()


/**
 * Returns the popup menu ID.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Coordinates of mouse cursor.
 * @return         Popup menu ID for the view (or zero to not show menu).
 */
uint4 EndoluminalView::GetPopupMenuID(UINT uFlags, CPoint point) const
{
  return (!IsMainViewWindow() ? IDM_VIEW_WINDOW : 0);
} // GetPopupMenuID()


/**
 * Gets the string IDs for the mouse button use key.
 *
 * @param uLeftID    String ID for use of the left mouse button.
 * @param uMidID     String ID for use of the middle mouse button.
 * @param uRightID   String ID for use of the right mouse button.
 */
void EndoluminalView::GetMouseKeyStringIDs(uint4 & uLeftID, uint4 & uMidID, uint4 & uRightID) const
{
  bool bCtrlDown( IsKeyStateDown(VK_CONTROL) );
  
  switch (ViatronStudy::m_modeEndoView.GetMutexModes())
  {
    case ViewModes::LINEMEASURE:
      uLeftID = IDS_MEASURE;
      uMidID = 0;
      uRightID = bCtrlDown? 0 : IDS_MEASURE_DELETE;
      break;

    case ViewModes::LIGHTING:
      uLeftID = IDS_MOVELIGHTSOURCE;
      uMidID = 0;
      uRightID = 0;
      break;

    case ViewModes::BIOPSY:
      uLeftID = IDS_MOVEBIOPSY;
      uMidID = 0;
      uRightID = 0;
      break;

    default:
      uLeftID = IsKeyStateDown(VK_CONTROL) ? IDS_ALIGN_CROSS_SECTIONAL : IDS_FLIGHT_FORWARD;
      uMidID = IsKeyStateDown(VK_CONTROL) ? IDS_PAN_REWIND : IDS_SPIN_REWIND;

#ifndef FINAL_RELEASE
      if (IsKeyStateDown(VK_SHIFT))
      {
        uMidID = IDS_MOVEBIOPSY;
      }
#endif
      uRightID = IDS_FLIGHT_REVERSE;
      break;
  } // switch (mutex modes)

} // GetMouseKeyStringIDs()


/**
 * timer event
 *
 * @param uIDEvent   event id
 */
void EndoluminalView::OnTimer(const uint4 uIDEvent)
{
  if ( m_timerOrthogonalPtFix == uIDEvent)
  {
    rdrGlobal.m_bAxialShadow = m_vbSliceShadowStateSave[0];
    rdrGlobal.m_bSagittalShadow = m_vbSliceShadowStateSave[1];
    rdrGlobal.m_bCoronalShadow = m_vbSliceShadowStateSave[2];
    rdrGlobal.m_bObliqueShadow = m_vbSliceShadowStateSave[3];
    GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
    InvalidateViews( WPARAM(GetViewWndType()), 0);
    GLRender();
    m_timerOrthogonalPtFix.KillTimer();
  }
} // OnTimer()


/**
 * Right button down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnRButtonDown(UINT uFlags, CPoint point) 
{
  // Determine if this is the main view window
  if (!IsMainViewWindow())
  {
    GLChildView::OnRButtonDown(uFlags, point);
    return;
  }

  // All subsequent mouse input will be sent to this view
  SetCapture();
  
  CRect rect;
  GetClientRect(&rect);
  Point2D<int4> point2D(point.x, point.y);

  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
    case ViewModes::LINEMEASURE:
      PickupMeasurementLine(rect, CPoint(point.x, point.y-iPickupShiftY), false);
      
      // Delete measure line if control not held down
      if ((uFlags & MK_CONTROL) == 0)
      {
        if (GetDatasetRef().m_session.m_measureLineList.m_currentIter != GetDatasetRef().m_session.m_measureLineList.end())
        {
          CString str;
          str.Format(IDS_VERIFY_USER_OPERATION_FORMAT,
            LoadResourceString(IDS_DELETE), LoadResourceString(IDS_MEASUREMENT));
          if (AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION) == IDYES)
          {
            GetDatasetRef().m_session.m_measureLineList.erase(GetDatasetRef().m_session.m_measureLineList.m_currentIter);
            GetDatasetRef().m_session.m_measureLineList.m_currentIter = GetDatasetRef().m_session.m_measureLineList.end();
            InvalidateViews( WPARAM(GetViewWndType()), 0);
          }
          else
          {
            GetDatasetRef().m_session.m_measureLineList.m_currentIter = GetDatasetRef().m_session.m_measureLineList.end();
          }
        }
      }
      break;

    case ViewModes::LIGHTING:
      // Do nothing, as opposed to default case which begins manual flight.
      break;

    case ViewModes::WINDOWLEVEL:
      // Do nothing, as opposed to default case which begins manual flight.
      break;

    case ViewModes::BIOPSY:
      // Do nothing, as opposed to default case which begins manual flight.
      break;

    case ViewModes::ANNOTATION:
      // Cancel annotation mode
      ViatronStudy::m_modeEndoView -= ViewModes::ANNOTATION;
      break;

    case ViewModes::AUTOFLY:
    {
      // Turn off autofly mode, then begin manual flying by dropping down to default case below
      ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
      // DO NOT break!!  Continue on to the default (interactive fly) case
    }

    default:
      if (!rdrGlobal.m_bShowHomogenity)
      {
        const float fHeading = Interpolate( 0.0F, point.x, rect.Width()  - 1,  1.0F, -1.0F );
        const float fPitch   = Interpolate( 0.0F, point.y, rect.Height() - 1, -1.0F,  1.0F );
        float fThrust  = Bound( 0.0F, static_cast< float4 >( 1.0F - sqrt( fHeading * fHeading + fPitch * fPitch ) ), 1.0F );
        GetDatasetRef().m_submarine.StartFlying();
        GetDatasetRef().m_submarine.SetForces( -fThrust, fHeading, fPitch );
      }
      break;

  } // endswitch m_currMode;

  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnRButtonDown()


/**
 * Right button up handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnRButtonUp(UINT uFlags, CPoint point) 
{
  if (!IsMainViewWindow())
  {
    GLChildView::OnRButtonUp(uFlags, point);
    return;
  }
  
  // release mouse input capture
  ReleaseCapture();

  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
  case ViewModes::LINEMEASURE:
    GetDatasetRef().m_session.m_measureLineList.m_currentIter = GetDatasetRef().m_session.m_measureLineList.end();
    break;

  case ViewModes::LIGHTING:
    // Do nothing, as opposed to default case which ends manual flight.
    break;

  case ViewModes::WINDOWLEVEL:
    // Do nothing, as opposed to default case which ends manual flight.
    break;
    
  case ViewModes::BIOPSY:
    // Do nothing, as opposed to default case which ends manual flight.
    break;

  default:
    // stop flying only if we are the only button left held down
    if (!(uFlags & (MK_LBUTTON | MK_MBUTTON)))
    {
      GetDatasetRef().m_submarine.StopFlying();
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
    }
    break;

  } // endswitch m_currMode;
  
  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnRButtonUp()


/**
 * Right button double-click handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnRButtonDblClk(UINT uFlags, CPoint point)
{
  // Determine if this is the main view window
  if (!IsMainViewWindow())
  {
    GLChildView::OnRButtonDown(uFlags, point);
    return;
  }

  // All subsequent mouse input will be sent to this view
  SetCapture();
  
  CRect rect;
  GetClientRect(&rect);

#if !defined(FINAL_RELEASE)
  // start the animation
  m_iRdbcAnim = 1;
  m_rdbcPoint = Point2D<int4>(point.x, point.y);
#endif
} // OnRButtonDblClk()


/**
 * Calculate a 3D point and try to make sure that it is visible by moving it toward the eyepoint.
 *
 * @param mousePoint    2D point input
 * @return              the corresponding 3D volume (world?) point
 */
Point3D<float4> EndoluminalView::CalculateVisible3DPoint( const Point2D<float4> & mousePoint ) const
{
  const Point3D<float4> initialPoint = Calculate3DPoint( mousePoint );
  const float4 fMaxMovement( 100 );
  for ( float4 fDistance(0); fDistance < fMaxMovement; fDistance += 1.0F )
  {
    Normal3D<float4> directionToViewpoint( GetDatasetRef().m_submarine.GetPositionWorld() - initialPoint );
    const Point3D<float4> movedPoint = initialPoint + Vector3D<float4>( directionToViewpoint ) * fDistance;
    CPoint projectedPoint;
    if ( Is3DPtVisible( movedPoint, projectedPoint ) )
    {
      return movedPoint;
    }
  }
  return initialPoint;
} // CalculateVisible3DPoint()


/**
 * Mouse move down handler.
 *
 * @param uFlags   Mouse button and key flags.
 * @param point    Point of click.
 */
void EndoluminalView::OnMouseMove(UINT uFlags, CPoint point) 
{
  if (!IsMainViewWindow())
  {
    GLChildView::OnMouseMove(uFlags, point);
    return;
  }

  if(rdrGlobal.m_bShowHomogenity)
  {
    if (uFlags & MK_LBUTTON)
    {
      GetDatasetRef().m_fields.m_volumeRender.MoveWindow(point.x - m_lastBiopsyPt.m_x, m_lastBiopsyPt.m_y - point.y);
      GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
      InvalidateViews( WPARAM(GetViewWndType()), 0);
    }
    m_lastBiopsyPt = Point2D<int4>(point.x, point.y);
    return;
  }

  bool bUpdate = false;

  CPoint cursorPt( GetMessagePos() );
  ScreenToClient( & cursorPt );
  CRect rct;  GetClientRect( & rct );
  const CRect clientRect(rct);
  cursorPt.y = clientRect.Height() - cursorPt.y;

  // render hotspots
  int4 iHotspotCount = 0;
  float4 fAverageHotspotOpacity = 0;
  for ( std::vector< FindingHotspot >::iterator hotspotIter = m_findingHotspots.begin();
        hotspotIter != m_findingHotspots.end();
        ++ hotspotIter, ++ iHotspotCount )
  {

    // compute opacity based on distance from cursor
    fAverageHotspotOpacity += GetHotspotOpacity( hotspotIter->m_hotspot, cursorPt );

    // hash in some number for when the cursor is changed within a hotspot
    if ( hotspotIter->m_hotspot.PtInRect( cursorPt.x, cursorPt.y ) )
    {
      fAverageHotspotOpacity += Interpolate( static_cast< float4 >( hotspotIter->m_hotspot.GetMinX() ), static_cast< float4 >( cursorPt.x ),
                                             static_cast< float4 >( hotspotIter->m_hotspot.GetMaxX() ), 0.9F, 1.0F );
    }
  }
  if ( iHotspotCount > 0 )
    fAverageHotspotOpacity /= iHotspotCount;
  if ( fAverageHotspotOpacity != m_fAverageHotspotOpacity )
  {
    m_fAverageHotspotOpacity = fAverageHotspotOpacity;
    if ( ! rdrGlobal.m_pCurrDataset->m_submarine.IsFlying() )
    {
      InvalidateViews( WPARAM(GetViewWndType()), 0);
      GLRender();
    }
  }

  //KAK BUGFIX
  if (! (IsKeyStateDown(VK_LBUTTON) || IsKeyStateDown(VK_MBUTTON) || IsKeyStateDown(VK_RBUTTON)) )
  {
    return;
  }

  // don't do any other actions if we are currently selecting a hotspot
  if ( m_bSelectingHotspot )
  {
    return;
  }

  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
    case ViewModes::LINEMEASURE:
      if (uFlags & MK_LBUTTON)
      {
        if (GetDatasetRef().m_session.m_measureLineList.m_currentIter != GetDatasetRef().m_session.m_measureLineList.end())
        {
          if (m_bPickAndMove == true)
          {
            // Get the "client" rectangle
            Rect<int4> viewClientRect(GetViewRect());
            viewClientRect -= viewClientRect.LeftTop();
      
            // Shrink client rect by 10 pixels; then, if endpoint is outside the view, 
            // the drawing code will detect this (and not display a measurement)
            viewClientRect.Inflate(-5, -5);

            // Restrict measure line to the client rectangle
            Point2D<int4> mousePt(point.x, point.y-iPickupShiftY);
            // If the mouse is outside the client rectangle, then drop the current measurement
            if ( viewClientRect.PtInRect( mousePt ) == false )
            {
              // if start is within a few pixels of the end point, remove it
              Point2D<int4> start = GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_drawEnds.m_start;
              Point2D<int4> end = GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_drawEnds.m_end;
              const float4 fThresholdDistance( 3 );
              if ( abs( pow( start.m_x - end.m_x, 2.0 ) + pow( start.m_y - end.m_y, 2.0 ) ) < fThresholdDistance )
              {
                GetDatasetRef().m_session.m_measureLineList.erase( GetDatasetRef().m_session.m_measureLineList.m_currentIter );
              }
              GetDatasetRef().m_session.m_measureLineList.m_currentIter = GetDatasetRef().m_session.m_measureLineList.end();
              InvalidateViews( WPARAM(GetViewWndType()), 0);
            }
            else
            {
              GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_drawEnds.m_end = mousePt;
              GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_measureEnds.m_end = CalculateVisible3DPoint(mousePt);
              GetDatasetRef().m_session.m_measureLineList.m_currentIter->StoreDistance();
              GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_ePosn = GetDatasetRef().m_session.m_measureLineList.GetUniquePos(GetDatasetRef().m_session.m_measureLineList.m_currentIter);

              InvalidateViews( WPARAM(GetViewWndType()), 0);
            }
          }
        }
      }
      break;

    case ViewModes::LIGHTING:
      if(uFlags & MK_LBUTTON)
      {
        float dv = float(point.y) * (2.0f / float(clientRect.Height())) - 1.0F;
        float du = float(clientRect.Width()-point.x) * (2.0f / float(clientRect.Width())) - 1.0F;
        float tanFov = float( tan(90.0F*3.14159F/180.0F/2) );
        GetDatasetRef().m_fields.m_volumeRender.m_fMoveLightVertDirection = dv * tanFov;
        GetDatasetRef().m_fields.m_volumeRender.m_fMoveLightHorizDirection = du * tanFov;
        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
 
        InvalidateViews( WPARAM(GetViewWndType()), 0);
      } // endif left mouse button down
      break;
 
    case ViewModes::WINDOWLEVEL:
      break;
    
    case ViewModes::BIOPSY:
      if (uFlags & MK_LBUTTON)
      {
        GetDatasetRef().m_fields.m_volumeRender.MoveWindow(point.x - m_lastBiopsyPt.m_x, m_lastBiopsyPt.m_y - point.y);
        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
 
        InvalidateViews( WPARAM(GetViewWndType()), 0);
      }
      m_lastBiopsyPt = Point2D<int4>(point.x, point.y);
      break;

    case ViewModes::ANNOTATION:
      m_pCurrentAnnotation = GetProximalAnnotation(clientRect, point);
      break;

    default:
    {
      const float fHeading = Interpolate( 0.0F, point.x, clientRect.Width()  - 1.0F,  1.0F, -1.0F );
      const float fPitch   = Interpolate( 0.0F, point.y, clientRect.Height() - 1.0F, -1.0F,  1.0F );
      float fThrust  = Bound( 0.0F, static_cast< float4 >( 1.0F - sqrt( fHeading * fHeading + fPitch * fPitch ) ), 1.0F );

      if (uFlags & MK_LBUTTON)
      {
        // Handle left mouse button
        if (uFlags & MK_RBUTTON)
        {
          // both buttons, just turn
          GetDatasetRef().m_submarine.SetForces(0,fHeading,fPitch);
          GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
        }
        else
        {
          // just left button, move forward
          // issue 1123: to avoid unnecessary rotation, add this test.
          if (IsCaptured()) // added this test
          {
            GetDatasetRef().m_submarine.SetForces(fThrust,fHeading,fPitch);
          }
        }

        bUpdate = true;
      }

      // Handle middle mouse button change
      if (uFlags & MK_MBUTTON)
      {
        // Pan if ctrl key is down, otherwise rotate
        GetDatasetRef().m_submarine.SetPanMode((uFlags & MK_CONTROL) > 0);
        GetDatasetRef().m_submarine.SetHeading(fHeading);
        GetDatasetRef().m_submarine.SetPitch(fPitch);
        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();

        bUpdate = true;
      }

      if (uFlags & MK_RBUTTON)
      {
        // just right button, move backward
        // added this test to avoid unnecessary rotation.
        if (IsCaptured())
        {
          GetDatasetRef().m_submarine.SetForces(-fThrust,fHeading,fPitch);
        }

        bUpdate = true;
      }

      // TODO: Update this view and the other view windows
      if (bUpdate)
      {
        InvalidateViews( WPARAM(GetViewWndType()), 0);
      }
      break;
    } // end default
  } // end switch
  
  ///Invalidate(NULL);
  GLChildView::OnMouseMove(uFlags, point);
} // OnMouseMove()


/**
 * Mouse-wheel rotate handler
 *
 * @param uFlags    Identifier keys/buttons currently held down
 * @param iZDelta   Indicates amount of rotation
 * @param pt        Coordinates of the cursor
 * @return          Non-zero to continue enabling mouse wheel scrolling
 */
BOOL EndoluminalView::OnMouseWheel(UINT uFlags, short iZDelta, CPoint pt)
{
  // TODO: Remove restriction on scrolling and moving when drawing measurements in 3D is implemented

  // Don't allow movement via replay mode when measuring
  if (ViatronStudy::m_modeEndoView.GetMutexModes() == ViewModes::LINEMEASURE)
  {
    return true;
  }

  // each mouse wheel click is 120 (WHEEL_DELTA) delta values
  int4 iFramesReplay(-iZDelta / WHEEL_DELTA);

  // maybe adjust the field of view
  if ( IsKeyStateDown('F') )
  {
    m_bChangingFOV = true;
    const float4 fFovMin = 45;
    const float4 fFovMax = 150;
    const float4 fChangeFov = 5 * iFramesReplay;
    GetDatasetRef().m_fields.m_volumeRender.m_fFovDegrees = Bound( fFovMin, GetDatasetRef().m_fields.m_volumeRender.m_fFovDegrees + fChangeFov, fFovMax );
    GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
    GLRender();
    TRACE0("Changing FOV in Navigation View\n");
    return true;
  }

  // undo more frames if the user holds down the ctrl key
  static const int4 iReplayJumpFactor(10);
  if (uFlags & MK_CONTROL)
  {
    iFramesReplay *= iReplayJumpFactor;
  }

  uint4 uType = uint4(GLChildView::eGVT_SLICE);

  // replay (akin to undo) mode in submarine
  GetDatasetRef().m_submarine.ReplayPosition(iFramesReplay);
  InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);  // This allows the UDS view updated simutaneously.
  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
  GetParent()->SendMessage( WM_APP_SYNCRONIZE_DATASET_LOCATIONS, WPARAM(GetDatasetRef().GetOrientation()), LPARAM(GetOppositeDatasetRef().GetOrientation()) );

  // update slices
  InvalidateViews( WPARAM(uType), 0);

  GLRender();

  // return true since mouse wheel scrolling is enabled
  return true;

} // OnMouseWheel()


//////////////////////////////////////////////////////////////////////////////////
//

/**
 * Retrieves update image; DO NOT DELETE DATA!
 *
 * @param iWidth                (output) width of buffer
 * @param iHeight               (output) height of buffer
 * @param iWidthAlignedDWORD    (output) width of buffer aligned to DWORD boundary
 * @param ePixelFormat          requested pixel format
 * @param eDatatype             requested datatype
 * @param bOnlyGLRenderScene    whether to only perform 3D rendering (without 2D text/lines/etc rendering)
 * @return                      rendered scene
 */
const void far * EndoluminalView::GetUpdateBuffer(int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat,
                                                         const GLenum eDatatype, const bool bOnlyGLRenderScene)
{
  uint1 * pBuffer = NULL;
  if (bOnlyGLRenderScene)
  {
    Dataset& dset = GetDatasetRef();

    dset.m_fields.m_volumeRender.RenderIntoBitmap(dset);
    iWidth = iHeight = dset.m_fields.m_volumeRender.GetRenderBufferSize();

    pBuffer = dset.m_fields.m_volumeRender.GetRenderBuffer();

    if ( (ePixelFormat == GL_BGR_EXT) || 0)//(ePixelFormat == GL_BGRA_EXT) )
    {
      uint1 * pBufferEnd = pBuffer + (iWidth*iHeight*3);
      for (uint1 * pPtr = pBuffer; pPtr < pBufferEnd; pPtr+=3)
      {
        std::swap(*pPtr, *(pPtr+2));
      }
    }
  }
  else
  {
    pBuffer = (uint1 *) ViatronChildView::GetUpdateBuffer(iWidth, iHeight, iWidthAlignedDWORD, ePixelFormat, eDatatype, bOnlyGLRenderScene);
  }

  return pBuffer;
} // GetUpdateBuffer()


/**
 * Called when a keyboard key is pressed.
 *
 * @param uChar     Code of the key
 * @param uRepCnt   Number of times the key has been repeated.
 * @param uFlags    Key codes.
 */
void EndoluminalView::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags) 
{
  if (!IsMainViewWindow())
  {
    GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
    return;
  }
  
  bool bUpdate = true;
  
  switch (uChar)
  {
#ifndef FINAL_RELEASE
    case 'B':
      GetDatasetRef().m_submarine.SetBoundary(!GetDatasetRef().m_submarine.GetBoundary());
      break;
#endif
#ifndef FINAL_RELEASE
    case 'G':
      rdrGlobal.m_bGridShadow = !rdrGlobal.m_bGridShadow;
      break;
    case 'H':
      GetDatasetRef().m_fields.m_volumeRender.m_eProjectionType = VolumeRender::FRANKS;
      break;
    case 'I':
      GetDatasetRef().m_fields.m_volumeRender.m_eProjectionType = VolumeRender::MERCATUR;
      break;
    case 'J':
      GetDatasetRef().m_fields.m_volumeRender.m_eProjectionType = VolumeRender::SPHERICAL;
      break;
    case 'K':
      GetDatasetRef().m_fields.m_volumeRender.m_eProjectionType = VolumeRender::PERSPECTIVE;
      break;
    case 'L':
      GetDatasetRef().m_submarine.SetLeveling(!GetDatasetRef().m_submarine.GetLeveling());
      break;
    case 'N':
      {
      //rdrGlobal.m_navigationTest = !rdrGlobal.m_navigationTest;
      break;
      }
#endif
    case 'S':
    {
      const Fields & fd = GetDatasetRef().m_fields;
      const bool bECAvailable(fd.m_sliceRendererData.IsValid() ? fd.IsHQDiffVolLoaded() && fd.IsElecCleansed() : fd.DoesDiffVolExist());
      if ( bECAvailable )
      {
        rdrGlobal.m_bSpaceLeaping = !rdrGlobal.m_bSpaceLeaping;
        rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
        rdrGlobal.m_prone.m_fields.m_volumeRender.FlushAccumulationBuffer();
        bUpdate = true;
      }
      break;
    }

		case 'V':
		{ // toggle the SRT color code ROI display, but not wack out the 2D SRT tool state
			if( ViatronStudy::GetCurrentViewType() == GLChildView::ViewWndTypeEnum::eGVT_NAVIGATION &&
				  ViatronStudy::m_modeEndoView != ViewModes::SEGMENTPOLYP )
			{
				if( !rdrGlobal.m_bDisplaySRTroi )
				{
					rdrGlobal.m_bDisplaySRTroi = true;
				} // if
				else
				{
				  rdrGlobal.m_bDisplaySRTroi   = false;
				} // else

				rdrGlobal.m_bDisplaySRTroi2D = false;
			}
			break;
		}

#ifndef FINAL_RELEASE
    case 'T':
      GetDatasetRef().m_submarine.SetNeighborSearching(!GetDatasetRef().m_submarine.GetNeighborSearching());
      if (GetDatasetRef().m_submarine.GetNeighborSearching())
        AfxMessageBox("Submarine searching neighbors for auto-navigation.");
      else
        AfxMessageBox("Submarine using DF* field for auto-navigation.");
      break;
    case 'X':
      // double the image resolution
      if (GetDatasetRef().m_fields.m_volumeRender.GetRenderSize() < 255)
      {
        GetDatasetRef().m_fields.m_volumeRender.SetRenderSize(255);
      }
      else
      {
        GetDatasetRef().m_fields.m_volumeRender.SetRenderSize(129);
      }
      break;
#endif

    case '5':
    {
      rdrGlobal.m_pCurrDataset->BuildHomogenityVolume();
      break;
    }
    case '6':
    {
      rdrGlobal.m_bShowHomogenity = !rdrGlobal.m_bShowHomogenity & rdrGlobal.m_bHomogenityCreated;
      break;
    }

    // Navigation keys
    case VK_LEFT:
    {
#ifndef FINAL_RELEASE
      if (IsKeyStateDown(VK_SHIFT))
      {
        GetDatasetRef().m_biopsyTransferFunction.m_fTranslucentScale /= 1.01F;
        GetDatasetRef().m_biopsyTransferFunction.SetBiopsy();
      }
      else
#endif
      if (ViatronStudy::m_modeEndoView.GetMutexModes() == ViewModes::BIOPSY)
      {
        GetDatasetRef().m_fields.m_volumeRender.MoveWindow(-10, 0);
        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
      }
      else
      {
        if ( GetDatasetRef().m_submarine.IsFlying() )
        {
          if (ViatronStudy::m_modeEndoView & ViewModes::AUTOFLY)
            ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
        }
        if ( GetDatasetRef().m_submarine.GetReplayMode() )
        {
          GetDatasetRef().m_submarine.ExitReplayMode();
          GetDatasetRef().m_submarine.StartFlying();
          GetDatasetRef().m_submarine.SetForces( 0, 0, 0 );
        }
        GetDatasetRef().m_submarine.SetPanMode(IsKeyStateDown(VK_CONTROL));
        GetDatasetRef().m_submarine.AddHeading(0.1F);
        GetDatasetRef().m_submarine.UpdatePosition();
      }
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      break;
    }
    case VK_RIGHT:
    {
#ifndef FINAL_RELEASE
      if (IsKeyStateDown(VK_SHIFT))
      {
        GetDatasetRef().m_biopsyTransferFunction.m_fTranslucentScale *= 1.01F;
        GetDatasetRef().m_biopsyTransferFunction.SetBiopsy();
      }
      else
#endif
      if (ViatronStudy::m_modeEndoView.GetMutexModes() == ViewModes::BIOPSY)
      {
        GetDatasetRef().m_fields.m_volumeRender.MoveWindow(10, 0);
        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
      }
      else
      {
        if ( GetDatasetRef().m_submarine.IsFlying() )
        {
          if (ViatronStudy::m_modeEndoView & ViewModes::AUTOFLY)
            ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
        }
        if ( GetDatasetRef().m_submarine.GetReplayMode() )
        {
          GetDatasetRef().m_submarine.ExitReplayMode();
          GetDatasetRef().m_submarine.StartFlying();
          GetDatasetRef().m_submarine.SetForces( 0, 0, 0 );
        }
        GetDatasetRef().m_submarine.SetPanMode(IsKeyStateDown(VK_CONTROL));
        GetDatasetRef().m_submarine.AddHeading(-0.1F);
        GetDatasetRef().m_submarine.UpdatePosition();
      }
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      break;
    }
    case VK_UP:
    {
#ifndef FINAL_RELEASE
      if (IsKeyStateDown(VK_SHIFT))
      {
        GetDatasetRef().m_fColonWallThreshold++;
        GetDatasetRef().m_surfaceTransferFunction.SetColonWall(GetDatasetRef().m_fColonWallThreshold);
      }
      else
#endif
      if (ViatronStudy::m_modeEndoView.GetMutexModes() == ViewModes::BIOPSY)
      {
        GetDatasetRef().m_fields.m_volumeRender.MoveWindow(0, 10);
        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
      }
      else
      {
        if ( GetDatasetRef().m_submarine.IsFlying() )
        {
          if (ViatronStudy::m_modeEndoView & ViewModes::AUTOFLY)
            ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
        }
        if ( GetDatasetRef().m_submarine.GetReplayMode() )
        {
          GetDatasetRef().m_submarine.ExitReplayMode();
          GetDatasetRef().m_submarine.StartFlying();
          GetDatasetRef().m_submarine.SetForces( 0, 0, 0 );
        }
        GetDatasetRef().m_submarine.SetPanMode(IsKeyStateDown(VK_CONTROL));
        GetDatasetRef().m_submarine.AddPitch(-0.1F);
        GetDatasetRef().m_submarine.UpdatePosition();
      }
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      break;
    }
    case VK_DOWN:
    {
#ifndef FINAL_RELEASE
      if (IsKeyStateDown(VK_SHIFT))
      {
        GetDatasetRef().m_fColonWallThreshold--;
        GetDatasetRef().m_surfaceTransferFunction.SetColonWall(GetDatasetRef().m_fColonWallThreshold);
      }
      else
#endif
      if (ViatronStudy::m_modeEndoView.GetMutexModes() == ViewModes::BIOPSY)
      {
        GetDatasetRef().m_fields.m_volumeRender.MoveWindow(0, -10);
        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
      }
      else
      {
        if ( GetDatasetRef().m_submarine.IsFlying() )
        {
          if (ViatronStudy::m_modeEndoView & ViewModes::AUTOFLY)
            ViatronStudy::m_modeEndoView -= ViewModes::AUTOFLY;
        }
        if ( GetDatasetRef().m_submarine.GetReplayMode() )
        {
          GetDatasetRef().m_submarine.ExitReplayMode();
          GetDatasetRef().m_submarine.StartFlying();
          GetDatasetRef().m_submarine.SetForces( 0, 0, 0 );
        }
        GetDatasetRef().m_submarine.SetPanMode(IsKeyStateDown(VK_CONTROL));
        GetDatasetRef().m_submarine.AddPitch(0.1F);
        GetDatasetRef().m_submarine.UpdatePosition();
      }
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      break;
    }
    default:
    {
      GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
      bUpdate = false;
      return;
    }
  }

  if (bUpdate)
  {
    InvalidateViews( WPARAM(GetViewWndType()), 0);
//    AfxGetMainWnd()->PostMessage(WM_APP_GL_IDLE, eGVT_ALL, 0);
  }

  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
///  Invalidate(FALSE);
  GLChildView::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown()


/**
 * Called when a keyboard key is released.
 *
 * @param uChar     Code of the key
 * @param uRepCnt   Number of times the key has been repeated.
 * @param uFlags    Key codes.
 */
void EndoluminalView::OnKeyUp(UINT uChar, UINT uRepCnt, UINT uFlags) 
{
  if (!IsMainViewWindow())
  {
    GLChildView::OnKeyUp(uChar, uRepCnt, uFlags);
    return;
  }
  
  bool bUpdate = true;
  
  // TODO: Add your message handler code here and/or call default
  switch (uChar) 
  {
    case 'F':
      if (!IsKeyStateDown('F'))
      {
        if (!m_bChangingFOV)
        {
          float4 &rFov = GetDatasetRef().m_fields.m_volumeRender.m_fFovDegrees;
          rFov = (rFov != 90) ? 90 : 120;
          GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
          GLRender();
          TRACE0("Toggle FOV in Navigation View\n");
        }
        else
          m_bChangingFOV = false;
      }
      break;

    // Navigation keys
    // For each of these keys, just want to stop the flying
    case VK_LEFT:
    case VK_RIGHT:
      GetDatasetRef().m_submarine.SetHeading(0);
      break;
    
    case VK_UP:
    case VK_DOWN:
      GetDatasetRef().m_submarine.SetPitch(0);
      break;
    
    case VK_SPACE:
      GetDatasetRef().m_submarine.StopFlying();
      InvalidateViews( WPARAM(GLChildView::eGVT_ALL), 0);
      //GetDatasetRef().m_volumeRender.Transfer3DMarking();
      break;

    default:
      bUpdate = false;
      break;
  }
  
  if (bUpdate)
  {
//    AfxGetMainWnd()->PostMessage(WM_APP_GL_IDLE, eGVT_ALL, 0);
  }
  
  GLChildView::OnKeyUp(uChar, uRepCnt, uFlags);
  GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
} // OnKeyUp()


/**
 * Called to set mouse pointer cursor.
 *
 * @param pWnd       Window that contains the mouse.
 * @param uHitTest   Identifies the cursor's location in the window.
 * @param uMessage   Mouse message that caused the set cursor message.
 * @return           True if cursor is set, false otherwise (to set default cursor).
 */
BOOL EndoluminalView::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage)
{
  int4 iCursorID(0);

  switch(ViatronStudy::m_modeEndoView.GetMutexModes())
  {
    case ViewModes::LIGHTING:
      iCursorID = IDC_HIGHLIGHTS;
      break;
    case ViewModes::LINEMEASURE:
      iCursorID = IDC_MEASURE;
      break;
    case ViewModes::SEGMENTPOLYP:
      iCursorID = IDC_POINTER_PAINTCAN;
      break;
    case ViewModes::ANNOTATION:
      iCursorID = IDC_ANNOTATION;
      break;
    case ViewModes::AUTOFLY:
    default:
    {
      SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
      return TRUE;
    }
  }

  SetCursor(AfxGetApp()->LoadCursor(iCursorID));
  
  return TRUE;
} // OnSetCursor()


/**
 * Renders a list of measurement dimensions
 * @param dataList    list of measurements
 * @param viewpoint   current viewpoint
 */
void EndoluminalView::RenderDimensions( const std::list< EndoluminalView::MeasurementDisplayData > & dataList,
                                       const Point<float4> & viewpoint )
{

  // loop over all the data
  for ( std::list< MeasurementDisplayData >::const_iterator iter = dataList.begin();
        iter != dataList.end();
        ++iter )
    {

      // check for a valid measurement
      if ( iter->GetDisplayDimension() )
      {
        const vxCadDimension & dimension = iter->GetDimension();
        if ( dimension.GetStartPoint().GetVector().GetLength() > 0.1F )
        {

          // create the measurement data structure
          MeasureLine line;
          line.m_measureEnds.m_start = Point3Df( dimension.GetStartPoint().GetVector() );
          line.m_measureEnds.m_end = Point3Df( dimension.GetEndPoint().GetVector() );
          Point2D<int4> projectedStart = Project3DPoint( line.m_measureEnds.m_start );
          Point2D<int4> projectedEnd = Project3DPoint( line.m_measureEnds.m_end );
          line.m_drawEnds.m_start = projectedStart;
          line.m_drawEnds.m_end = projectedEnd;
          line.StoreDistance();

          // render it
          const PixelRGBA<float4> lineColor( 1, 1, 0, iter->GetOpacity() );
          Render3DMeasurementLine( line, false, false, viewpoint, lineColor );

          // render some text
          Point3Di rightmost = ( projectedStart.m_x > projectedEnd.m_x ) ? line.m_measureEnds.m_start : line.m_measureEnds.m_end;
          glColor4f( lineColor.R(), lineColor.G(), lineColor.B(), lineColor.A() );
          m_medFont.DrawText( rightmost.m_x, rightmost.m_y, rightmost.m_z, iter->GetShortLabel() );

          // store it for display in the 2D raster method
          char sMeasureDist[101];
          static const CString sMeasureFormat( LoadResourceString( IDS_LINEMEASUREMENT_FORMAT ) );
          const int4 iUncertainty( GetDatasetRef().m_vUnits.GetMaxAbsComponent() + 0.5F );
          sprintf( sMeasureDist, sMeasureFormat, float4( ( line.m_measureEnds.m_start - line.m_measureEnds.m_end ).GetLength() ), /*iUncertainty,*/ LoadResourceString( IDS_MM ) );
          m_vsMeasurementText.push_back( iter->GetLongLabel() + sMeasureDist );

        } // if a valid measurement
      }
      else
      {
        m_vsMeasurementText.push_back( iter->GetLongLabel() );
      }

    } // for all dimensions
      
} // RenderDimensions( )


/**
 * renders measurements for the given cad finding
 * @param finding     cad finding for measurements
 * @param viewpoint   rendering 3D viewpoint
 */
void EndoluminalView::RenderFindingMeasurements( const vxCadFinding & finding, const Point<float4> & viewpoint )
{
  vxCadROI     roi( finding.GetVolumeROI() );
  uint2 uLabel( roi.GetAverageHU() );

  vxCadDimension largest( finding.GetDimensions().GetLargestDimension() );
  float fVolume( roi.GetVolumeValue() );

  // display setting
  std::list< MeasurementDisplayData > roiInfo;
  roiInfo.push_back( MeasurementDisplayData( LPCTSTR(LoadResourceString(IDS_ROI_VOLUME_TXT)) + ToAscii( RoundDecimals( fVolume, 0 ) )
    + std::string(" ") + LPCTSTR(LoadResourceString(IDS_MM)) + std::string("^3" ) ) );
  roiInfo.push_back( MeasurementDisplayData( LPCTSTR(LoadResourceString(IDS_MAX_DIAMETER)), "", largest, 0.0F ) );

  RenderDimensions( roiInfo, viewpoint );

} // RenderFindingMeasurements


/**
 * Draw 3D measurement lines 
 *
 * @param viewpoint   camera position.
 */
void EndoluminalView::Render3DMeasurementLines( const Point<float4> & viewpoint )
{
  try
  {
    // OpenGL setup
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    {
      glEnable(GL_DEPTH_TEST);

      // render all the measurement lines
      for ( MeasureLineList::iterator listIter = GetDatasetRef().m_session.m_measureLineList.begin();
            listIter != GetDatasetRef().m_session.m_measureLineList.end();
            listIter++ )
      {

        Render3DMeasurementLine( * listIter, listIter == GetDatasetRef().m_session.m_measureLineList.m_currentIter, true, viewpoint, PixelRGBA<float4>( 1, 1, 1, 1 ) );

      } // for all measurement lines

      // get the cursor position
      CPoint cursorPt( GetMessagePos() );
      ScreenToClient( & cursorPt );
      CRect clientRect;
      GetClientRect( & clientRect );
      cursorPt.y = clientRect.Height() - cursorPt.y;

      m_vsMeasurementText.clear();

      if ( rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT()) && 
				!EndoluminalView::m_bForceDrawCurrentROI && rdrGlobal.m_bDisplaySRTroi )
      {/// display current suspicious region characteristics info
        CPoint pt( GetMessagePos() );
        ScreenToClient( & pt );

        // Get the volume coordinate
        Vector3D<float> vfCurrent( Calculate3DPoint(Point2D<float>(pt.x, pt.y)) );

        // get the label volume
        Triple<float> vfUnits( GetDatasetRef().m_fields.m_sliceRendererData.GetCleansedVolume()->GetHeader().GetVoxelUnits() );
    
        Vector3D<int2> viCurrent(8,8,8);
        // compute the volume coordinate for the center of sub-volume
        viCurrent.m_x = int2(vfCurrent.m_x/vfUnits.m_x+0.5F);
        viCurrent.m_y = int2(vfCurrent.m_y/vfUnits.m_y+0.5F);
        viCurrent.m_z = int2(vfCurrent.m_z/vfUnits.m_z+0.5F);
        
        uint2 uComponentLabel = SearchNearestROILabel( viCurrent );

        if ( uComponentLabel > 0 )
        { /// This point is close to a component, draw the ROI measurements
          const vxCadFindings suspiciousRois = GetDatasetRef().m_suspiciousRegions;
          int n = suspiciousRois.GetCount();

          if( n > 0 )
          {
            for( int i=0; i<n; i++ )
            {
              vxCadFinding finding( suspiciousRois.GetFinding(i) );
              vxCadROI     roi( finding.GetVolumeROI() );
              uint2 uLabel( roi.GetLabelIndex() );

              if( uLabel == uComponentLabel )
              {
                RenderFindingMeasurements( finding, viewpoint );
                break;
              } // if the label match 
            } // for all suspicious ROIs
          } // there is at lease one suspicious ROI
        } // cursor near a suspicious ROI
      } // end of the display of SRT measurements

      //render all the visible CAD measurements of the hotspot nearest
      //to the cursor (which is the first hotspot in the sorted list)
      if ( m_findingHotspots.size() > 0 && rdrGlobal.m_bDisplayCadFindings )
      {
        glEnable( GL_BLEND );

        FindingHotspot hotspot = m_findingHotspots.front();

        // compute opacity based on distance from cursor
        const float4 fOpacity = GetHotspotOpacity( hotspot.m_hotspot, cursorPt );

        if ( fOpacity > 0.0F )
        {

          // get the finding
          const vxCadFinding & cadFinding = GetDatasetRef().m_cadFindings.GetFinding( hotspot.m_sID );

          // prepare the dimension display data structures
          std::list< MeasurementDisplayData > dimensions;
          dimensions.push_back( MeasurementDisplayData( "Largest: ", "L", cadFinding.GetDimensions().GetLargestDimension(), fOpacity ) );
          dimensions.push_back( MeasurementDisplayData( "Perp: ", "P", cadFinding.GetDimensions().GetPerpendicularDimension(), fOpacity ) );
          dimensions.push_back( MeasurementDisplayData( "Depth: ", "D", cadFinding.GetDimensions().GetDepthDimension(), fOpacity ) );

          // render
          RenderDimensions( dimensions, viewpoint );

        } // if visible

      } // if any hotspots

    } // glPushAttrib
    glPopAttrib();

  } // try
  catch ( ... )
  {
    LogErr( "unhandled exception", "EndoluminalView", "Render3DMeasurementLines" );
  }

} // Render3DMeasurementLines()


/**
 * Draw a single 3D measurement line
 *
 * @param line        the line to draw.
 * @param bCurrent    if this line is the current line.
 * @param viewpoint   camera position.
 */
void EndoluminalView::Render3DMeasurementLine( MeasureLine & line, const bool bCurrent,
                                              const bool bAdjustable, const Point<float4> viewpoint,
                                              const PixelRGBA<float4> & lineColor )
{
  CPoint Pt2D0, Pt2D1;
  Point<float4> startPt = line.m_measureEnds.m_start;
  Point<float4> endPt   = line.m_measureEnds.m_end;

  bool bStartPtVisible = Is3DPtVisible(startPt, Pt2D0);
  bool bEndPtVisible   = Is3DPtVisible(endPt, Pt2D1);

  if (!bStartPtVisible && !bEndPtVisible)
  {
    // set up some flag here...
    line.m_ePosn = MeasureLine::NODISPLAY;
    line.m_drawEnds.m_start = Point2D<int4>(-1,-1);
    line.m_drawEnds.m_end   = Point2D<int4>(-1,-1);
    return;
  }
  else
  {
    if ( bAdjustable )
    {
      // draw start&(/)end points.
      glEnable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);
      glEnable(GL_LIGHT0);
      
      // if start Pt is visible...
      if ( bStartPtVisible || bCurrent )
      {
        float4 fOldDepth = (float4)(Vector<float4>(startPt-viewpoint).GetLength());
        float4 fRadius = fOldDepth * 0.01F;
        glColor3f(float4(!bStartPtVisible), float4(!bStartPtVisible), 1.0F);
        glPushMatrix(); 
          glTranslated(startPt.m_x, startPt.m_y, startPt.m_z);
          gluSphere(m_pQuadObject, fRadius, 10, 10);
        glPopMatrix();
      }

      // if end Pt is visible...
      if ( bEndPtVisible || bCurrent )
      {
        float4 fOldDepth = (float4)(Vector<float4>(endPt-viewpoint).GetLength());
        float4 fRadius = fOldDepth * 0.01F;
        glColor3f(float4(!bEndPtVisible), float4(!bEndPtVisible), 1.0F);
        glPushMatrix(); 
          glTranslated(endPt.m_x, endPt.m_y, endPt.m_z);
          gluSphere(m_pQuadObject, fRadius, 10, 10);
        glPopMatrix();
      }

      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
    } // if adjustable

    // set up the postion to display measure text.
    if ( bCurrent )
    {
      // position is set when pick up...
    }
    else
    {
      // start pt is visible...
      if (bStartPtVisible && !bEndPtVisible)
      {
        line.m_ePosn = MeasureLine::START;
        line.m_drawEnds.m_start = Point2D<int4>((int4)Pt2D0.x, (int4)Pt2D0.y);
      }
      
      // end point is visible...
      if (!bStartPtVisible && bEndPtVisible)
      {
        line.m_ePosn = MeasureLine::END;
        line.m_drawEnds.m_end = Point2D<int4>((int4)Pt2D1.x, (int4)Pt2D1.y);
      }

      // both are visible...
      if (bStartPtVisible && bEndPtVisible) 
      {
        // no set, choose start point...
        if (line.m_ePosn == MeasureLine::NODISPLAY)
        {
          line.m_ePosn = MeasureLine::START;
        }
        // else: keep the original 'm_ePosn' setting.
        line.m_drawEnds.m_start = Point2D<int4>((int4)Pt2D0.x, (int4)Pt2D0.y);
        line.m_drawEnds.m_end   = Point2D<int4>((int4)Pt2D1.x, (int4)Pt2D1.y);
      }
    } // end-else
  }

  // draw line segment...
  if ( bCurrent )
    glLineWidth(3.0F);
  else
    glLineWidth(2.0F);

  if ( bAdjustable )
  {
    float4 fDistance = (float4)(Vector<float4>(startPt - endPt).GetLength());
    uint4 uStepSize;
    float4 fDelta;
    if (fDistance <= 3.0F)
    {
      uStepSize = 1;
      fDelta = fDistance;
    }
    else
    {
      uStepSize = uint4(fDistance/1.0F);
      fDelta = fDistance / uStepSize;
    }
    if (fDelta<=0.01F) return;

    bool bPt0Visible = bStartPtVisible;
    bool bPt1Visible(false);
    CPoint Pt2D;
    Point<float4> Pt0 = startPt;
    for (float4 fCurrDist=fDelta; fCurrDist<(fDistance+fDelta/2); fCurrDist += fDelta)
    {      
      Point<float4> Pt1;
      Pt1.m_x = startPt.m_x * (1.0F - fCurrDist/fDistance) + endPt.m_x * (fCurrDist/fDistance);
      Pt1.m_y = startPt.m_y * (1.0F - fCurrDist/fDistance) + endPt.m_y * (fCurrDist/fDistance);
      Pt1.m_z = startPt.m_z * (1.0F - fCurrDist/fDistance) + endPt.m_z * (fCurrDist/fDistance);
      bPt1Visible = Is3DPtVisible(Pt1, Pt2D);

      if (bPt0Visible && bPt1Visible)
      {
        glColor4f( lineColor.R(), lineColor.G(), lineColor.B(), lineColor.A() );
        glBegin(GL_LINES);
          RenderUtil::GlVertex(Pt0);
          RenderUtil::GlVertex(Pt1);
        glEnd();
      } // if draw == true
      else
      {
        if ( ( bStartPtVisible && bEndPtVisible ) || bCurrent )
        {
          glColor4f( 1.0f, 0.0f, 1.0f, lineColor.A() );
          glBegin(GL_LINES);
            RenderUtil::GlVertex(Pt0);
            RenderUtil::GlVertex(Pt1);
          glEnd();
        }
      }

      /// Pt1 -> Pt0
      Pt0 = Pt1;
      bPt0Visible = bPt1Visible;
    }
  } // if adjustable
  else
  {
    glColor4f( lineColor.R(), lineColor.G(), lineColor.B(), lineColor.A() );
    glBegin(GL_LINES);
      RenderUtil::GlVertex(startPt);
      RenderUtil::GlVertex(endPt);
    glEnd();
  } // if not adjustable

} // Render3DMeasurementLine()


/**
 * Renders end points of all visible measurement lines.
 *
 * @param clientRect   Client rectangle.
 */
void EndoluminalView::RenderTextMeasurementLines(const Rect<int4> & clientRect)
{
  char sMeasureDist[101];
  const GLFont& rGLFont = GetNormalFont();

  const int4 iClientHeight(clientRect.Height());

  MeasureLineList::iterator listCurr = GetDatasetRef().m_session.m_measureLineList.begin();
  MeasureLineList::iterator listEnd = GetDatasetRef().m_session.m_measureLineList.end();
  for (; listCurr != listEnd; listCurr++)
  {
    // if neither is visible, no draw of this line.
    if (listCurr->m_ePosn == MeasureLine::NODISPLAY)
    {
      continue;
    }

    Point2D<int4> drawPt1 = listCurr->m_drawEnds.m_start;
    Point2D<int4> drawPt2 = listCurr->m_drawEnds.m_end;

    // if (drawPt1.m_x == -1 && drawPt1.m_y == -1)
      // continue;

    drawPt1.m_y = iClientHeight - drawPt1.m_y;
    drawPt2.m_y = iClientHeight - drawPt2.m_y;

    glColor3f(1.0F, 1.0F, 1.0F);
    const float4 fDist(listCurr->m_f3Ddistance);
    // Display measurement text n.n mm +/- 1mm
    if ( (int4(fDist * 10.0f) != 0) && (clientRect.PtInRect(drawPt2)) && drawPt2.m_y > 0 )
    {
      static const CString sMeasureFormat(LoadResourceString(IDS_LINEMEASUREMENT_FORMAT));
      sprintf(sMeasureDist, sMeasureFormat, fDist, /*int4(GetDatasetRef().m_vUnits.m_z+0.5),*/ LoadResourceString(IDS_MM));

      Point2D<int4> textPt;
      
      if (listCurr->m_ePosn == MeasureLine::START)
      {
        textPt = drawPt1;
        textPt.m_y += (drawPt1.m_y > drawPt2.m_y) ? 5 : -15;
      }
      else if (listCurr->m_ePosn == MeasureLine::END)
      {
        textPt = drawPt2;
        textPt.m_y += (drawPt2.m_y > drawPt1.m_y) ? 5 : -15;
      }
      else
      {
        // if (listCurr->m_posn == MeasureLine::MID)
        textPt = Point2D<int4>((drawPt1.m_x+drawPt2.m_x)/2, (drawPt1.m_y+drawPt2.m_y)/2);

        if(abs(drawPt1.m_x-drawPt2.m_x) > abs(drawPt1.m_y-drawPt2.m_y))
        {
          textPt.m_y -= 20;
        }
        else
        {
          textPt.m_x += 10;
        }
      }

      uint4 eTextPos = GDT_TEXTLOCNONE;
      eTextPos |= (textPt.m_x >= clientRect.m_right - 100) ? GDT_LEFT : GDT_RIGHT;
      eTextPos |= (textPt.m_y >= clientRect.m_bottom - 20) ? GDT_BELOW : GDT_ABOVE;
      rGLFont.DrawText(textPt.m_x, textPt.m_y, sMeasureDist, GDT_TEXTLOC(eTextPos));
    } // endif (dist != 0)
  }
} // RenderTextMeasurementLines()


/**
 * pickup measurement line
 *
 * @param rect             reference to window coordinates
 * @param point            2D point
 * @param bPickupAndMove   whether line is being moved
 * @return                 true when picked up a line.
 */
bool EndoluminalView::PickupMeasurementLine(const CRect & rect, const CPoint & point, const bool bPickAndMove)
{
  if (bPickAndMove) m_bPickAndMove = true;
  MeasureLineList::iterator listCurr = GetDatasetRef().m_session.m_measureLineList.begin();
  MeasureLineList::iterator listEnd  = GetDatasetRef().m_session.m_measureLineList.end();
  
  // Find the closest proximal line (if any) to the clicked point
  GetDatasetRef().m_session.m_measureLineList.m_currentIter = GetDatasetRef().m_session.m_measureLineList.end();

  bool bSwapEnds = false;
  Point<float4> pt3D = Calculate3DPoint(Point2D<float>(point.x, point.y));

  float4 fLeastDist2D = 10.0F;
  std::vector<uint1> vbEndPtsVisible;
  for (; listCurr != listEnd; listCurr++)
  {
    CPoint pt2D;
    uint1 iTest = 0;
    if (Is3DPtVisible(listCurr->m_measureEnds.m_start, pt2D))
    {
      iTest |= 0x01;
      float4 fDist = sqrt(static_cast<float4>((pt2D.x-point.x)*(pt2D.x-point.x) + (pt2D.y-point.y)*(pt2D.y-point.y)));
      if (fDist < fLeastDist2D)
      {
        bSwapEnds = true;
        GetDatasetRef().m_session.m_measureLineList.m_currentIter = listCurr;
        fLeastDist2D = fDist;
      }
    }

    if (Is3DPtVisible(listCurr->m_measureEnds.m_end, pt2D))
    {
      iTest |= 0x02;
      float4 fDist = sqrt(static_cast<float4>((pt2D.x-point.x)*(pt2D.x-point.x) + (pt2D.y-point.y)*(pt2D.y-point.y)));
      if (fDist < fLeastDist2D)
      {
        bSwapEnds = false;
        GetDatasetRef().m_session.m_measureLineList.m_currentIter = listCurr;
        fLeastDist2D = fDist;
      }
    }

    vbEndPtsVisible.push_back(iTest);
  }

  // if not picked up the line at the end points, try to pick it up through the the point on it for view:
  bool bLinePickup = false;
  float4 fLeastDistLine = 5.0F;
  if (GetDatasetRef().m_session.m_measureLineList.m_currentIter == GetDatasetRef().m_session.m_measureLineList.end())
  {
    uint4 iPtr = 0;
    Point<float4> currentPt;
    MeasureLineList::iterator listCurr = GetDatasetRef().m_session.m_measureLineList.begin();
    for (; listCurr!=listEnd; ++listCurr)
    {
      uint1 value = vbEndPtsVisible[iPtr++];
      if (value == 0x0) continue;

      Point<float4> ptStart = listCurr->m_measureEnds.m_start;
      Point<float4> ptEnd   = listCurr->m_measureEnds.m_end;

      float4 fLength = (float4)(Vector<float4>(ptStart - ptEnd).GetLength());
      float4 fDelta = (fLength <= 1.0F) ? (fLength/2.0) : (fLength/uint4(fLength));
      if (fDelta<=0.1F) continue;

      Point<float4> pt0 = ptStart;
      CPoint pt2D0, pt2D1;
      bool bPt0Visible = Is3DPtVisible(pt0, pt2D0);
      bool bPt1Visible;

      for (float4 fCurrDist=fDelta; fCurrDist<(fLength+1.0F); fCurrDist += fDelta)
      {
        Point<float4> pt1(
          ptStart.m_x * (1.0F-fCurrDist/fLength) + ptEnd.m_x * (fCurrDist/fLength),
          ptStart.m_y * (1.0F-fCurrDist/fLength) + ptEnd.m_y * (fCurrDist/fLength),
          ptStart.m_z * (1.0F-fCurrDist/fLength) + ptEnd.m_z * (fCurrDist/fLength)
          );
        bPt1Visible = Is3DPtVisible(pt1, pt2D1);

        if ((value == 0x03) || (bPt0Visible && bPt1Visible))
        {
          float4 fLength2D = sqrt(static_cast<float4>((pt2D0.x-pt2D1.x)*(pt2D0.x-pt2D1.x) + (pt2D0.y-pt2D1.y)*(pt2D0.y-pt2D1.y)));
          if (fLength2D > 0)
          {
            const float tIncr = 1.0 / fLength2D;
            float t(0);
            for (int4 i(0); i < fLength2D; i++)
            {
              CPoint pt2D;
              t += tIncr;
              pt2D.x = pt2D0.x * (1.0F-t) + pt2D1.x * t;
              pt2D.y = pt2D0.y * (1.0F-t) + pt2D1.y * t;
              float4 fDist = sqrt( static_cast<float4>((pt2D.x-point.x) * (pt2D.x-point.x) + (pt2D.y-point.y) * (pt2D.y-point.y)));

              if (fDist < fLeastDistLine)
              {
                bLinePickup = true;
                m_bPickAndMove = false;
                bSwapEnds = (fCurrDist < fLength/2.0F) ? true : false;
                GetDatasetRef().m_session.m_measureLineList.m_currentIter = listCurr;
                fLeastDistLine = fDist;
              }
            }
          }
        }

        bPt0Visible = bPt1Visible;
        pt2D0.x = pt2D1.x;
        pt2D0.y = pt2D1.y;
        pt0 = pt1;
      } // end-for (fCurrDist)
    } // end-for (listCurr)
  } // calculate the distance from the line.

  if (GetDatasetRef().m_session.m_measureLineList.m_currentIter != GetDatasetRef().m_session.m_measureLineList.end())
  {
    if (bSwapEnds)
    {
      GetDatasetRef().m_session.m_measureLineList.m_currentIter->SwapEnds();
    }
    GetDatasetRef().m_session.m_measureLineList.m_currentIter->m_ePosn = 
      GetDatasetRef().m_session.m_measureLineList.GetUniquePos(GetDatasetRef().m_session.m_measureLineList.m_currentIter);

    return true;
  }

  return false;
} // PickupMeasurementLine()


/**
 * Get the nearest annotation that is proximal to the given 2D test point
 *
 * @param rect    reference to window coordinates.
 * @param point   2D point.
 * @return        nearest proximal annotation or NULL if none.
 */
ReaderLib::BookmarkList::iterator EndoluminalView::GetProximalAnnotation(const CRect & rect, const CPoint & testPoint)
{
  BookmarkList::iterator itClosest = GetDatasetRef().m_session.m_2DAnnotationList.end();

  float4 fLeastDist2D = 100000.0F;
  CRect hotRect(testPoint, CSize(0,0)); hotRect.InflateRect(6,6);

  BookmarkList::iterator iter = GetDatasetRef().m_session.m_2DAnnotationList.begin();
  BookmarkList::iterator listEnd = GetDatasetRef().m_session.m_2DAnnotationList.end();
  for (; iter != listEnd; iter++)
  {
    CPoint pt2D;
    if (Is3DPtVisible(iter->m_position, pt2D))
    {
      Point2D<int4> pt2d = Project3DPoint(iter->m_position);
      CPoint center = hotRect.CenterPoint();
      CPoint vecktor(center.x - pt2d.m_x, center.y - pt2d.m_y);
      float4 fdist = sqrt(float4(vecktor.x * vecktor.x) + float4(vecktor.y * vecktor.y));

      if (hotRect.PtInRect(CPoint(pt2d.m_y, pt2d.m_y)) && fdist < fLeastDist2D)
      {
        fLeastDist2D = fdist;
        itClosest = iter;
      }
    }
  }

  return itClosest;
} // GetProximalAnnotation()


/**
 * Segments a (user-)selected region
 *
 * @param selectPoint   The (user-)selected point to begin region segmentation
 */
void EndoluminalView::SegmentRegion(const Point<float4> & selectPoint)
{
  CWaitCursor crsr;

  /////////////////////////////////////////////////////////////////////
  /// This is for polyp segmentation

  vxShareableObject< vxComponentArray > * pComponentArray( GetDatasetRef().m_fields.m_sliceRendererData.GetComponents() );
  uint2 uHistoryIndex = pComponentArray->AddComponent("Suspicious ROI", PixelRGBA<uint1>(255, 0, 0, 255) );
  const uint2 uNewLabel = (*pComponentArray)[uHistoryIndex]->GetLabel();

  vxPolypSegmentation polypSeg( GetDatasetRef().m_fields.m_volumeRender,
    *GetDatasetRef().m_fields.m_sliceRendererData.GetCleansedVolume(),
    *GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume(), uNewLabel,
    GetDatasetRef().m_fields.m_ins, GetDatasetRef().m_fields.m_paintVol, selectPoint );

  Point3D<float4> ptEnd1;
  Point3D<float4> ptEnd2;
  float fMaximumDiameter;
  float fPolypVolume;

  try
  {
    int iIndicator = polypSeg.Run(); 

    vxHistogramUpdaterLabel<uint2>::Update( *GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume() );

    if ( iIndicator < 0 )
    { // ROI does not look like a polyp
      AfxMessageBox(IDS_REGIONSEGMENT_NONEFOUND);
      pComponentArray->RemoveComponent( uHistoryIndex );
    }
    else if ( iIndicator == 1 )
    { // ROI seems like a small polyp!

      if (AfxMessageBox(IDS_REGIONSEGMENT_SMALLREGION, MB_ICONQUESTION | MB_YESNO) == IDYES)
      { // User selects to keep the ROI
        fMaximumDiameter = polypSeg.GetMaxDiameter( ptEnd1, ptEnd2 );
        fPolypVolume = polypSeg.GetVolume();

        vxCadVector pt0(      0.0F,       0.0F,       0.0F );
        vxCadVector pt1(ptEnd1.m_x, ptEnd1.m_y, ptEnd1.m_z );
        vxCadVector pt2(ptEnd2.m_x, ptEnd2.m_y, ptEnd2.m_z );

        vxCadDimension largest(pt1, pt2);
        vxCadDimension depth  (pt0, pt0);
        vxCadDimension width  (pt0, pt0);

        vxCadDimensions regionDim(largest, width, depth);

        vxCadFinding suspiciousRegion;
        suspiciousRegion.SetDimensions( regionDim );

        pt0.SetVector( Vector3D<float4>(selectPoint.X(), selectPoint.Y(), selectPoint.Z()) );
        suspiciousRegion.SetCentroidMM( pt0 );

        vxCadROI roi;
        roi.SetVolumeValue( fPolypVolume );
        roi.SetLabelIndex( uNewLabel );

        // record ROI voxels for session
        RecordROIVoxels( roi, uNewLabel, *GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume() );

        suspiciousRegion.SetVolumeROI(roi);

        GetDatasetRef().m_suspiciousRegions.AddFinding( suspiciousRegion );

        GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
        AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
      }
      else
      {
        polypSeg.ResetPaintVolPerUserRequest();
        pComponentArray->RemoveComponent( uHistoryIndex );
      }
    }
    else if ( iIndicator == 0 )
    { // ROI does like a regular polyp
      fMaximumDiameter = polypSeg.GetMaxDiameter( ptEnd1, ptEnd2 );
      fPolypVolume = polypSeg.GetVolume();

      vxCadVector pt0(      0.0F,       0.0F,       0.0F );
      vxCadVector pt1(ptEnd1.m_x, ptEnd1.m_y, ptEnd1.m_z );
      vxCadVector pt2(ptEnd2.m_x, ptEnd2.m_y, ptEnd2.m_z );

      vxCadDimension largest(pt1, pt2);
      vxCadDimension depth  (pt0, pt0);
      vxCadDimension width  (pt0, pt0);

      vxCadDimensions regionDim(largest, width, depth);

      vxCadFinding suspiciousRegion;
      suspiciousRegion.SetDimensions( regionDim );

      pt0.SetVector( Vector3D<float4>(selectPoint.X(), selectPoint.Y(), selectPoint.Z()) );
      suspiciousRegion.SetCentroidMM( pt0 );

      vxCadROI roi;
      roi.SetVolumeValue( fPolypVolume );
      roi.SetLabelIndex( uNewLabel );// TODO: should not use the average HU field

       // record ROI voxels for session
      RecordROIVoxels( roi, uNewLabel, *GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume() );

      suspiciousRegion.SetVolumeROI(roi);

      GetDatasetRef().m_suspiciousRegions.AddFinding( suspiciousRegion );

      GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
    }
    else if ( iIndicator == 3 )
    {
      AfxMessageBox("This ROI has been segmented already.");
      pComponentArray->RemoveComponent( uHistoryIndex );
    }
    else
    {
      AfxMessageBox(IDS_REGIONSEGMENT_ERROROCCURRED);
      pComponentArray->RemoveComponent( uHistoryIndex );
    }
  } // try
  catch( ... )
  {
    AfxMessageBox(IDS_REGIONSEGMENT_ERROROCCURRED);
    pComponentArray->RemoveComponent( uHistoryIndex );
    throw;
  }

} // SegmentRegion()


/**
 * Record a single point SROI. This is for clinical trial creating a truth
 *
 * @param selectPoint    The (user-)selected point to begin region segmentation in 3D
 */
void EndoluminalView::RecordAPointOnly(const Point<float4>& selectPoint)
{
  CWaitCursor crsr;

  vxShareableObject< vxComponentArray > * pComponentArray( GetDatasetRef().m_fields.m_sliceRendererData.GetComponents() );
  uint2 uHistoryIndex = pComponentArray->AddComponent("Suspicious ROI", PixelRGBA<uint1>(255, 0, 0, 255) );
  const uint2 uNewLabel = (*pComponentArray)[uHistoryIndex]->GetLabel();

  Point3D<float4> ptEnd1(selectPoint);
  Point3D<float4> ptEnd2(selectPoint);
  float4 fMaximumDiameter;
  float4 fPolypVolume;

  try
  {
    Triple<float4> vfUnits( GetDatasetRef().m_vUnits );
    Triple<int4>   viDim  ( GetDatasetRef().m_vDim );

    Vector3D<int4> posVol(selectPoint.m_x/vfUnits.m_x+0.5F, selectPoint.m_y/vfUnits.m_y+0.5F, selectPoint.m_z/vfUnits.m_z+0.5F);

    /// record one point in the label volume
    vxBlockVolumeIterator<uint2> itl(*GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume());
    itl.SetPos(posVol);
    itl.SetVoxel(uNewLabel);
    vxHistogramUpdaterLabel<uint2>::Update( *GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume() );

    /// paint the surface point in endoview
    uint1 * pPaintVol = GetDatasetRef().m_fields.m_paintVol.GetDataPtr();
    const int4 iS( viDim.m_x*viDim.m_y );
    const int4 ip( posVol.m_x + posVol.m_y*viDim.m_x + posVol.m_z*iS );
    pPaintVol[ip] = SUSPICIOUSROI;

    fMaximumDiameter = 0;
    fPolypVolume = 0;

    vxCadVector pt0(      0.0F,       0.0F,       0.0F );
    vxCadVector pt1(ptEnd1.m_x, ptEnd1.m_y, ptEnd1.m_z );
    vxCadVector pt2(ptEnd2.m_x, ptEnd2.m_y, ptEnd2.m_z );

    vxCadDimension largest(pt1, pt2);
    vxCadDimension depth  (pt0, pt0);
    vxCadDimension width  (pt0, pt0);

    vxCadDimensions regionDim(largest, width, depth);

    vxCadFinding suspiciousRegion;
    suspiciousRegion.SetDimensions( regionDim );

    pt0.SetVector( Vector3D<float4>(selectPoint.X(), selectPoint.Y(), selectPoint.Z()) );
    suspiciousRegion.SetCentroidMM( pt0 );

    vxCadROI roi;
    roi.SetVolumeValue( fPolypVolume );
    roi.SetLabelIndex( uNewLabel );

    // record ROI voxels for session
    RecordROIVoxels( roi, uNewLabel, *GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume() );

    suspiciousRegion.SetVolumeROI(roi);

    GetDatasetRef().m_suspiciousRegions.AddFinding( suspiciousRegion );

    GetDatasetRef().m_fields.m_volumeRender.FlushAccumulationBuffer();
    AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
  } // try
  catch( ... )
  {
    AfxMessageBox(IDS_REGIONSEGMENT_ERROROCCURRED);
    pComponentArray->RemoveComponent( uHistoryIndex );
    throw;
  }
} // RecordAPointOnly


/**  storing SRT ROI voxels for session
 * 
 *  @param roi         the working vxCadROI
 *  @param uROIlabel   the label of the ROI in the label volume
 *  @param labelVol    the label volume
 */
void EndoluminalView::RecordROIVoxels( vxCadROI & roi, const uint2 & uROIlabel,
                           vxBlockVolume<uint2> & labelVol )
{
  vxBlockVolumeIterator<uint2> it( labelVol );

  register vxCadVoxelPosition pos;
  for( it.SetPos(0,0,0); it.IsNotAtEnd(); it.NextBlockZYX() )
  {
    for( ; it.IsNotAtEndOfBlock(); it.NextZYXinsideBlock() )
    {
      if( it.GetVoxel() == uROIlabel )
      {
        pos = vxCadVoxelPosition(it.GetPos().m_x, it.GetPos().m_y, it.GetPos().m_z);
        
        roi.AddVoxel( pos );
      }
    } // for inside a block
  } // for all blocks
} // RecordROIVoxels


/**  search for the nearest supicious or CAD ROI
 * 
 *  @param viPos         the current mouse position in the volume
 *  @return the non-zero label that was found nearest to the mouse position
 */
uint2 EndoluminalView::SearchNearestROILabel( const Vector3D<int2> & viPos )
{
  uint2 uComponentLabel(0);

  // check if the current point is close to a suspicious or cad ROI
  vxShareableObject< vxComponentArray > * pComponentArray( GetDatasetRef().m_fields.m_sliceRendererData.GetComponents() );
  const uint2 uMaxLabel( pComponentArray->GetMaxLabel() );
  
  vxBlockVolume<uint2> *pLabelVolume( GetDatasetRef().m_fields.m_sliceRendererData.GetLabelVolume() );
  vxBlockVolumeIterator<uint2> itl(*pLabelVolume);

  itl.SetPos( viPos );

  if( uMaxLabel > 0 )
  {
    if( itl.GetVoxel() > 0 )
      uComponentLabel = itl.GetVoxel();
    else if( itl.PeekRel(1,0,0) > 0 )
      uComponentLabel = itl.PeekRel(1,0,0);
    else if( itl.PeekRel(2,0,0) > 0 )
      uComponentLabel = itl.PeekRel(2,0,0);
    else if( itl.PeekRel(3,0,0) > 0 )
      uComponentLabel = itl.PeekRel(3,0,0);
    else if( itl.PeekRel(-1,0,0) > 0 )
      uComponentLabel = itl.PeekRel(-1,0,0);
    else if( itl.PeekRel(-2,0,0) > 0 )
      uComponentLabel = itl.PeekRel(-2,0,0);
    else if( itl.PeekRel(-3,0,0) > 0 )
      uComponentLabel = itl.PeekRel(-3,0,0);
    else if( itl.PeekRel(0,1,0) > 0 )
      uComponentLabel = itl.PeekRel(0,1,0);
    else if( itl.PeekRel(0,2,0) > 0 )
      uComponentLabel = itl.PeekRel(0,2,0);
    else if( itl.PeekRel(0,3,0) > 0 )
      uComponentLabel = itl.PeekRel(0,3,0);
    else if( itl.PeekRel(0,-3,0) > 0 )
      uComponentLabel = itl.PeekRel(0,-3,0);
    else if( itl.PeekRel(0,-2,0) > 0 )
      uComponentLabel = itl.PeekRel(0,-2,0);
    else if( itl.PeekRel(0,-1,0) > 0 )
      uComponentLabel = itl.PeekRel(0,-1,0);
    else if( itl.PeekRel(0,0,1) > 0 )
      uComponentLabel = itl.PeekRel(0,0,1);
    else if( itl.PeekRel(0,0,2) > 0 )
      uComponentLabel = itl.PeekRel(0,0,2);
    else if( itl.PeekRel(0,0,3) > 0 )
      uComponentLabel = itl.PeekRel(0,0,3);
    else if( itl.PeekRel(0,0,-1) > 0 )
      uComponentLabel = itl.PeekRel(0,0,-1);
    else if( itl.PeekRel(0,0,-2) > 0 )
      uComponentLabel = itl.PeekRel(0,0,-2);
    else if( itl.PeekRel(0,0,-3) > 0 )
      uComponentLabel = itl.PeekRel(0,0,-3);
  } // if there exists suspicious or cad ROI

  return(uComponentLabel);

} // SearchNearestROILabel





// undefines
#undef FILE_REVISION


// $Log: NavigationView.C,v $
// Revision 1.101.2.35  2010/12/21 22:55:10  dongqing
// clean up ULD volume tranisition and space leaping flag
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.34  2010/11/01 12:40:55  dongqing
// add comments on Mark's fix for the Win 7 progress bar of 3D fly thru
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.33  2010/10/29 18:09:42  mkontak
// Fix for swapping Supine/Prone datasets and the progress control nbot refereshing
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.32  2010/10/29 13:07:27  mkontak
// New progress bar to fix the flashing that occurs in the NavigationView. Commented
// out the Invalidate of the entire Frame on the Verify view to fix the flashing
// during the volume loading.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.31  2010/10/20 20:47:33  dongqing
// The SRT tool availability and display switch will be only determined
// by the VCSetting.xml switch now.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.30  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.29  2010/08/17 21:05:58  dongqing
// Fix the CAD hotspot missed rendering. Enlarge the zDepth bounds
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.28  2010/05/17 19:13:50  dongqing
// Fix the Mouse wheel replay did not refresh the UDS view.
//
// Revision 1.101.2.27  2010/04/23 23:47:21  dongqing
// shortened % display to say: lumen:nn%
//
// Revision 1.101.2.26  2010/04/23 21:34:48  dongqing
// add lumen coverage label to the % display
//
// Revision 1.101.2.25  2010/04/19 18:15:50  dongqing
// turn off annotation mode after cancelling an annotation add
//
// Revision 1.101.2.24  2010/03/10 00:43:40  dongqing
// don't add annotation on cancel
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.23  2010/03/09 20:23:55  dongqing
// print study info on every snapshot image
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.22  2010/03/03 11:07:52  dongqing
// added "measurement" to the string table
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.21  2010/02/12 16:32:14  dongqing
// change the wording for the Delete a measurement. Note, the incorrect wording
// is due to the usage of String Table. Now, it uses the hard coded string.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.20  2010/01/31 02:08:24  dongqing
// support for vertical lumen coverage progress bar
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.101.2.19  2009/11/18 23:17:21  dongqing
// put the hot spot right on top the finding surface
//
// Revision 1.101.2.18  2009/11/10 19:20:48  dongqing
// Issue Cad09-02: giving overview cad marks colours to indicate visibility setting.
//
// Revision 1.101.2.17  2009/10/07 16:13:13  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.101.2.16  2008/05/27 20:43:42  dongqing
// Fix the SRT new ROI getting the wrong label index bug
// The componoent mechanism works based on HistoryLabelList
//
// Revision 1.101.2.15  2008/03/04 15:43:29  dongqing
// simplify world -> volume coordinate computation
//
// Revision 1.101.2.14  2008/03/03 20:40:17  dongqing
// add function for record single point user seleced in the SROI list
//
// Revision 1.101.2.13  2008/01/17 19:35:31  dongqing
// complete the manual tool for suspicious ROI editing
//
// Revision 1.101.2.12  2008/01/03 16:04:50  dongqing
// fix the change of GetROIVolume() for polyp segmentation
//
// Revision 1.101.2.11  2007/11/08 16:54:43  jmeade
// temporarily display slice shadows after ctrl-clicking to set slice positions.
//
// Revision 1.101.2.10  2007/10/22 23:35:34  jmeade
// resource string.
//
// Revision 1.101.2.9  2007/09/20 17:07:14  jmeade
// unicode char set for all fonts.
//
// Revision 1.101.2.8  2007/09/14 17:13:21  jmeade
// Issue 5779, 5780: remove the +/- units in childview display of measurement text.
//
// Revision 1.101.2.7  2007/08/17 21:08:21  jmeade
// Issue 5783: allow double-click to start autofly while in measurement mode.
//
// Revision 1.101.2.6  2007/08/17 19:43:24  jmeade
// Issue 5782: default FOV at 90*, i.e. first 'f' keypress returns to 90.
//
// Revision 1.101.2.5  2007/07/31 22:23:33  jmeade
// Hitting the 'F' key toggles between 90* and 120* rotation angles.
//
// Revision 1.101.2.4  2007/06/08 22:14:36  jmeade
// Issue 5680: change the translucency settings for both datasets.
//
// Revision 1.101.2.3  2007/06/06 20:38:24  jmeade
// Issue 5672: update vxviewer color scheme when initializing from past session.
//
// Revision 1.101.2.2  2007/04/04 18:51:03  jmeade
// Issue 5551: ensure current settings are saved if app is closed while AT pane is active.
//
// Revision 1.101.2.1  2007/03/22 20:40:46  dongqing
// change the m_uXsectionScrollBarLocation to m_iXsectionScrollBarLocation
// The type is changed from uint4 to int4
//
// Revision 1.101  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.100  2007/03/12 17:09:43  jmeade
// code standards.
//
// Revision 1.99  2007/03/12 16:19:26  jmeade
// Issue 5278: drawing srt measurement during snapshot.
//
// Revision 1.98  2007/03/09 22:01:43  jmeade
// Issue 5278: drawing srt measurement during snapshot.
//
// Revision 1.97  2007/03/08 22:22:04  romy
// code review - header files adjusted
//
// Revision 1.96  2007/03/08 18:32:22  jmeade
// code standards.
//
// Revision 1.95  2007/03/06 01:47:46  jmeade
// comments.
//
// Revision 1.94  2007/03/05 23:17:13  dongqing
// add new m_iXsectionScrollBarLocationIndex
//
// Revision 1.93  2007/02/16 22:00:16  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.92  2007/02/15 00:08:21  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.91  2007/02/06 16:36:43  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.90  2007/01/23 20:17:42  dongqing
// add functionto handle the SRT session save and load
//
// Revision 1.89  2007/01/12 16:38:04  dongqing
// change the SRT measurement text color to black
//
// Revision 1.88  2007/01/10 15:13:14  dongqing
// change the text color for CAD or SRT measurement
//
// Revision 1.87  2007/01/09 15:14:55  dongqing
// fix the bug of re-enable SRT ROI display causing 2D ROI disappear
//
// Revision 1.86  2007/01/05 15:41:16  dongqing
// Add exception catch in SegmentRegion()
//
// Revision 1.85  2006/12/21 16:21:40  dongqing
// Add the switch for display SRT ROI on/off
//
// Revision 1.84  2006/12/18 19:42:08  dongqing
// add checking for the ROI being already segmented
//
// Revision 1.83  2006/12/06 19:00:41  jmeade
// Issue 5047: better organization of translucency settings structure.
//
// Revision 1.82  2006/11/16 00:10:58  dongqing
// fix both 2D and 3D display of SRT measurements
//
// Revision 1.81  2006/11/15 20:32:17  dongqing
// the SRT measurements in 3D is based on the mouse cursor locatioin now
//
// Revision 1.80  2006/11/15 16:44:55  dongqing
// comments out test codes for display measurement temporarily
//
// Revision 1.79  2006/11/14 23:07:29  dongqing
// display SRT measurement only when mouse cursor at the ROI
//
// Revision 1.78  2006/11/14 16:28:53  dongqing
// take off the "size" display in 3D view when polyp segmentatioin happens
//
// Revision 1.77  2006/11/02 18:43:38  jmeade
// Issue 5075: handle mouse-up event regardless of main-window status.
//
// Revision 1.76  2006/10/26 21:04:49  frank
// removed transparency from logo
// added FOV: to field of view text
//
// Revision 1.75  2006/10/13 18:53:22  dongqing
// change the "maximum diam." to "size"
//
// Revision 1.74  2006/10/11 19:09:24  jmeade
// Issue 5056: flush accumulation buffer to ensure up-to-date image on snapshot.
//
// Revision 1.73  2006/10/10 20:02:23  dongqing
// replace SendMessage to PostMessage for fixing the bug of
// wrong color for polyp region
//
// Revision 1.72  2006/10/10 18:02:54  frank
// reduced the significant digits displayed for volume measurement
//
// Revision 1.71  2006/10/09 18:53:14  dongqing
// using cleansed volume for polyp segmentation
//
// Revision 1.70  2006/10/04 23:34:51  jmeade
// text to resources.
//
// Revision 1.69  2006/10/04 23:10:20  jmeade
// colour of render measurement text.
//
// Revision 1.68  2006/10/04 17:52:09  jmeade
// allow parent to handle view mode update.
//
// Revision 1.67  2006/10/04 03:12:02  jmeade
// typo.
//
// Revision 1.66  2006/10/04 03:10:04  jmeade
// Issue 4905: Print current and default field of view values during adjustment.
//
// Revision 1.65  2006/10/03 19:29:46  frank
// added FOV adjustment
//
// Revision 1.64  2006/10/03 02:06:58  jmeade
// wait cursor during region segmentation.
//
// Revision 1.63  2006/09/29 16:08:26  dongqing
// change the ray detection to 2-level iteration.
//
// Revision 1.62  2006/09/28 20:30:03  jmeade
// segment region method.
//
// Revision 1.61  2006/09/28 16:01:04  frank
// removed dead code
//
// Revision 1.60  2006/09/26 21:08:50  dongqing
// No matter success or fail of the polyp segmentation, exit the
// polyp segment mode
//
// Revision 1.59  2006/09/26 19:33:53  dongqing
// Add the component color in the 2D view.
//
// Revision 1.58  2006/09/26 15:55:51  dongqing
// pass in the bool label volume
//
// Revision 1.57  2006/09/22 20:48:02  dongqing
// adjust the annotation for flat lesion
//
// Revision 1.56  2006/09/21 21:17:05  dongqing
// allow to output flat polyp
//
// Revision 1.55  2006/09/19 01:18:03  jmeade
// Issue 4917: protect against unnecessary view mode changes.
//
// Revision 1.54  2006/09/18 19:47:41  jmeade
// Issue 4916: trigger mouse cursor change when view mode changes.
//
// Revision 1.53  2006/09/14 19:58:09  dongqing
// add flat/diminish polyp detection
//
// Revision 1.52  2006/09/14 18:24:28  dongqing
// tune up the maximum diameter measurement
//
// Revision 1.51  2006/09/14 17:17:12  frank
// removed debug
//
// Revision 1.50  2006/09/14 13:03:32  frank
// refactored rendering to allow polyp measurement text
//
// Revision 1.49  2006/09/13 19:32:19  dongqing
// activate the PolypSeg mode
//
// Revision 1.48  2006/08/25 21:38:43  romy
// expanded the measurement neighbourhood area for an easy pickup
//
// Revision 1.47  2006/08/02 23:05:55  jmeade
// Issue 4609: 'c' key toggles visibility of cad findings.
//
// Revision 1.46  2006/07/19 19:21:13  geconomos
// Issue# 4815: problems with resolution when recording movie from supine prone
//
// Revision 1.45  2006/06/01 20:21:36  frank
// updated to visual studio 2005
//
// Revision 1.44  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.43  2006/01/31 19:32:23  romy
// build errors fixed
//
// Revision 1.42  2006/01/31 18:21:10  romy
// code review-rerranged include files
//
// Revision 1.41  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.40  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.39  2005/11/16 15:57:29  frank
// re-fixed slight speed problem
//
// Revision 1.38  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.37  2005/11/04 18:07:51  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.36.2.29  2006/09/21 00:00:52  jmeade
// Issue #4925 (duplicating fix done in #4815, revision 1.46): set appropriate render size.
//
// Revision 1.36.2.28  2006/09/18 19:58:01  jmeade
// Issue 4916: trigger mouse cursor change when view mode changes.
//
// Revision 1.36.2.27  2006/05/09 13:55:31  geconomos
// + initialization of fAverageHotspotOpacity
// + restoring old font when selecting font into dcWbd
//
// Revision 1.36.2.26  2006/04/24 23:46:35  jmeade
// Issue 4712: Ensure a mutex mode is on b4 attempting to turn off (else, any current mutex mode will be turned off).
//
// Revision 1.36.2.25  2006/04/04 21:30:25  jmeade
// Issue 4693: Do not adjust cad numbers based on decision value, only on visibility.
//
// Revision 1.36.2.24  2006/03/29 20:53:03  frank
// read the segment number from the cad findings file
//
// Revision 1.36.2.23  2006/03/28 20:22:14  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.36.2.22  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.36.2.21  2006/03/22 22:51:54  jmeade
// Issue 4667: restrict registration synchronization to views where both datasets are visible, and a link button is available.
//
// Revision 1.36.2.20  2006/03/22 21:01:54  frank
// moved the finding visibility and other information into a class
//
// Revision 1.36.2.19  2006/03/07 19:20:25  jmeade
// Issue 4620: Locate annotation inside colon lumen.
//
// Revision 1.36.2.18  2006/03/03 01:56:25  jmeade
// Issue 4620: Add annotations in 3D.
//
// Revision 1.36.2.17  2006/01/23 14:51:11  frank
// showed expanded information about CAD finding measurements
//
// Revision 1.36.2.16  2006/01/16 21:17:32  jmeade
// Issue 4599: registration update when necessary during rewind mode.
//
// Revision 1.36.2.15  2006/01/07 04:35:20  frank
// fixed slider functionality and adjusted finding number display
//
// Revision 1.36.2.14  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.36.2.13  2006/01/03 18:08:10  frank
// Issue #4583: Fixed problem with spinning continuing after you leave the window while holding middle mouse on non-standard view.
//
// Revision 1.36.2.12  2005/12/30 20:48:52  jmeade
// Issue 4581: Accounted for strange mangling of CONSTANT *this data in Rect::GetLeftTop().
//
// Revision 1.36.2.11  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.36.2.10  2005/12/06 21:59:21  jmeade
// don't show slice indicators when flying in either dataset.
//
// Revision 1.36.2.9  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.36.2.8  2005/11/16 02:55:27  frank
// improved display of cad measurements - added labels
//
// Revision 1.36.2.7  2005/11/15 23:28:58  frank
// Issue #4533: Fixed flight speed
//
// Revision 1.36.2.6  2005/11/15 19:41:55  frank
// allowed reversing through the cad findings
//
// Revision 1.36.2.5  2005/11/15 14:50:17  frank
// Issue #4514: Fixed crash jumping to a finding not in the current dataset
//
// Revision 1.36.2.4  2005/11/11 02:39:44  jmeade
// Shift-Space in EndoView jumps to previous cad finding.
//
// Revision 1.36.2.3  2005/11/10 01:28:38  jmeade
// Fixed bug saving session values between views and across sessions.
//
// Revision 1.36.2.2  2005/11/10 00:54:46  jmeade
// Issue 4042: Disable EC shortcut and text display when EC not available on dataset.
//
// Revision 1.36.2.1  2005/11/04 18:21:53  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.37  2005/11/04 18:07:51  frank
// Issue #4515: Removed black strip along edge of image
//
// Revision 1.36  2005/11/03 21:28:12  frank
// added cad finding measurements if available
//
// Revision 1.35  2005/11/02 15:58:16  frank
// decreased the radius of the 3D hotspot
//
// Revision 1.34  2005/10/28 13:28:50  geconomos
// modified for changes to volume render
//
// Revision 1.33  2005/10/27 15:18:37  vxconfig
// Issue #4481: Fixed looking around with arrow keys after rewinding.
//
// Revision 1.32  2005/10/25 13:50:35  geconomos
// comments
//
// Revision 1.31  2005/10/25 13:49:17  geconomos
// now displaying the number of render threads when viewing the frame rates
//
// Revision 1.30  2005/10/13 18:11:32  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.29  2005/10/12 22:38:37  jmeade
// option to always show cad findings arrows in 3D
//
// Revision 1.28  2005/10/07 14:49:23  vxconfig
// stopped it from flying when selecting finding hotspots
//
// Revision 1.27  2005/10/06 18:06:25  vxconfig
// additional fixes
//
// Revision 1.26  2005/10/06 13:59:22  vxconfig
// hid CAD mark text and hotspots
//
// Revision 1.25  2005/10/06 13:40:05  vxconfig
// fixed visibility and opacity of arrows and numbers
//
// Revision 1.24  2005/10/06 05:00:31  vxconfig
// Fixed a number of issues
//
// Revision 1.23  2005/10/05 22:58:35  jmeade
// jump to next finding on spacebar; notify cad dialog of visibility changes.
//
// Revision 1.22  2005/10/04 19:05:08  vxconfig
// removed debug info
//
// Revision 1.21  2005/10/04 18:49:15  vxconfig
// updated view when not flying
//
// Revision 1.20  2005/10/04 17:38:25  vxconfig
// added hotspots in 3D for showing/hiding the findings
//
// Revision 1.19  2005/10/03 12:59:37  geconomos
// removed border style from progress control
//
// Revision 1.18  2005/09/29 18:32:21  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.17  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.16  2005/09/28 17:00:17  vxconfig
// name change: element -> index
//
// Revision 1.15  2005/09/28 12:04:12  vxconfig
// formatting
//
// Revision 1.14  2005/09/26 14:12:48  vxconfig
// hid invisible cad findings
//
// Revision 1.13  2005/09/23 20:48:32  jmeade
// softer colour for findings arrows.
//
// Revision 1.12  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.11  2005/09/23 03:22:21  vxconfig
// removed debug message
//
// Revision 1.10  2005/09/23 03:18:44  vxconfig
// working on display of cad findings
//
// Revision 1.9  2005/09/22 18:26:25  jmeade
// draw cad findings.
//
// Revision 1.8  2005/09/09 14:31:01  geconomos
// Added a governor that regulates the update of the endoluminal coverge indicator.
//
// Revision 1.7  2005/09/06 17:11:17  geconomos
// Issue #4412: Movement in 2d causing hiccups during endoluminal flight
//
// Revision 1.6  2005/08/25 14:13:42  geconomos
// Issue# 4379: Removed "Delete Measurement" from help option when control key is down
//
// Revision 1.5  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.4  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.55.2.16.2.19.2.11  2005/07/20 14:02:08  geconomos
// Issue #4293:  Moved volumerender.SetRenderSize out of rendering loop into WM_SIZE handler
//
// Revision 3.55.2.16.2.19.2.10  2005/07/15 18:25:44  geconomos
// Added methods to invalidate parent window where external controls are placed.
//
// Revision 3.55.2.16.2.19.2.9  2005/07/14 14:16:31  geconomos
// Refactored logo renering code
//
// Revision 3.55.2.16.2.19.2.8  2005/07/13 19:23:31  geconomos
// Issue# 4263: Endoluminal coverage label flickering while flying
//
// Revision 3.55.2.16.2.19.2.7  2005/07/13 19:03:03  geconomos
// fixed laser for non-released modes
//
// Revision 3.55.2.16.2.19.2.6  2005/07/08 18:58:23  jmeade
// privatize dataset modality variable.
//
// Revision 3.55.2.16.2.19.2.5  2005/07/06 18:00:42  geconomos
// Renamed Invalidate to invalidateView ( clashing with CWnd::Invalidate() )
//
// Revision 3.55.2.16.2.19.2.4  2005/07/06 15:36:11  geconomos
// Issue #4251: Scroll wheel not forcing update of view fixed
//
// Revision 3.55.2.16.2.19.2.3  2005/07/01 16:59:14  jmeade
// Issue 4199: Unsubtracted text for MR.
//
// Revision 3.55.2.16.2.19.2.2  2005/06/30 16:56:16  geconomos
// Issue: #4234 not displaying warning when not using space leaping
//
// Revision 3.55.2.16.2.19.2.1  2005/06/30 13:50:20  geconomos
// Issue #4221: Endoluminal not  flying on double click when flyin 2d
//
// Revision 3.55.2.16.2.19  2005/06/14 19:13:27  geconomos
// Un-did changes to high resolution 3d rendering.
//
// Revision 3.55.2.16.2.18  2005/06/10 18:31:28  geconomos
// Removed uneeded debugging information from view.
//
// Revision 3.55.2.16.2.17  2005/06/10 18:20:49  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 3.55.2.16.2.16  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.55.2.16.2.15  2005/04/23 01:38:17  frank
// adjusted the text position of Viatronix
//
// Revision 3.55.2.16.2.14  2005/04/22 16:48:31  frank
// cleaned up field class interface
//
// Revision 3.55.2.16.2.13  2005/04/22 05:19:21  jmeade
// Issue 4108: refresh image when deleting measurement.
//
// Revision 3.55.2.16.2.12  2005/04/22 03:30:19  frank
// Issue #4108: Improving measurement drawing and handling.
//
// Revision 3.55.2.16.2.11  2005/04/21 12:57:04  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 3.55.2.16.2.10  2005/04/20 10:52:06  frank
// fixed computation of string width so horizontal alignment is fixed in all views
//
// Revision 3.55.2.16.2.9  2005/04/19 17:42:19  geconomos
// separated OpenGL and Windows resize functions
//
// Revision 3.55.2.16.2.8  2005/04/19 13:40:55  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.55.2.16.2.7  2005/04/13 20:48:02  frank
// Issue #4108: Working on linear measurement
//
// Revision 3.55.2.16.2.6  2005/04/08 19:12:26  frank
// Issue #4108: Prevented dragging 3D measurements outside the view
//
// Revision 3.55.2.16.2.5  2005/04/04 19:29:07  jmeade
// IsLoadingComplete() checks before operations.
//
// Revision 3.55.2.16.2.4  2005/03/30 21:55:01  jmeade
// Issue 4038: framework changes.
//
// Revision 3.55.2.16.2.3  2005/03/30 21:04:18  frank
// Issue #4070: Changed align function to ctrl-left click
//
// Revision 3.55.2.16.2.2  2005/03/22 15:26:38  frank
// Issue #3860: Fixed hesitation when starting 3D reverse flight
//
// Revision 3.55.2.16.2.1  2005/03/21 19:53:59  frank
// Issue #4038: Sped up font drawing
//
// Revision 3.55.2.16  2005/02/15 20:49:20  jmeade
// merge from vc1.4 branch.
//
// Revision 3.55.2.15  2004/11/20 15:28:00  dongqing
// back using the paint volume
//
// Revision 3.55.2.14  2004/09/30 15:17:31  jmeade
// Interaction for polyp segmentation.
//
// Revision 3.55.2.13.2.7  2005/02/07 16:50:33  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.55.2.13.2.6  2005/01/27 22:47:05  jmeade
// Hide lumen coverage controls on movie view.
//
// Revision 3.55.2.13.2.5  2005/01/21 20:50:55  jmeade
// Separate movie-creation view, incremental checkin.
//
// Revision 3.55.2.13.2.4  2004/11/24 14:54:05  jmeade
// 's' key toggles uncleansed in 2D, toggles space leaping in 3D.
//
// Revision 3.55.2.13.2.3  2004/11/19 16:36:14  jmeade
// Along with space leaping, also toggle 2D uncleansed display with 's' key.
//
// Revision 3.55.2.13.2.2  2004/11/12 22:48:57  jmeade
// Default max flight speed of 200.
//
// Revision 3.55.2.13.2.1  2004/11/11 22:54:43  jmeade
// update 2d views when in 'rewind' mode.
//
// Revision 3.55.2.13  2004/06/25 04:57:00  jmeade
// Update inter-session storage of translucency values to reflect color selections.
//
// Revision 3.55.2.12  2004/05/15 00:35:57  jmeade
// Allow storage/adjustment of all biopsy/transluncency values.
//
// Revision 3.55.2.11  2004/05/05 23:16:28  jmeade
// Incremental update.
//
// Revision 3.55.2.10  2004/04/21 03:59:09  jmeade
// Adjust translucent rending dialog.
//
// Revision 3.55.2.9  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.55.2.8  2003/07/26 04:02:58  jmeade
// Issue 3405: Ensure the uncleansed volume is loaded when displaying uncleansed in 3D.
//
// Revision 3.55.2.7  2003/07/24 17:48:02  jmeade
// Issue 3402:  Switch to autofly mode on double-click when most other
// independent modes are active (this behavior was inadvertently changed/introduced
// in a previous version).
//
// Revision 3.55.2.6  2003/06/02 21:47:44  jmeade
// Issue 3262: Route and handle view mode change message thru MainAppFrame to ensure it'll always get handled; comments.
//
// Revision 3.55.2.5  2003/04/25 18:03:57  jmeade
// Issue ???:  Space leaping toggle state indicated in 3D view, movies, report.
//
// Revision 3.55.2.4  2003/04/15 21:55:10  jmeade
// Issue 3001: Always resize to square dimensions.
//
// Revision 3.55.2.3  2003/03/26 23:19:19  jmeade
// Issue 2849:  Removed unnecessary non-const pointer parameter.
//
// Revision 3.55.2.2  2003/03/05 21:25:27  jmeade
// Issue 2817:  Must set throttle for both datasets.
//
// Revision 3.55.2.1  2003/02/27 02:17:34  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.55  2003/01/30 16:56:25  jmeade
// Localized registry storage of all flight params to NavigationView.
//
// Revision 3.54  2003/01/22 22:32:28  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.53  2003/01/21 19:33:52  jmeade
// Duplicate: Revision : 3.28.2.14: Size lumen coverage labels appropriately by (language-specific) text.
//
// Revision 3.52  2003/01/07 15:02:38  kevin
// Use odd-sized render sub-buffers so that the in-plane adaptive sampling lines up better
//
// Revision 3.51  2001/01/09 17:36:21  kevin
// Adjust Debug output for endoscopic frame rate to also include
// adaptive render size for the current frame and the average since the
// viewer started.
//
// Revision 3.50  2002/12/24 16:25:17  ingmar
// removed "if !defined(FINAL_RELEASE)" to allow compilation
//
// Revision 3.49  2002/12/24 03:43:30  jmeade
// Bug: Measure list not saved in session b/c there were two copies of it!;
//  Override of GetUpdateBuffer() allows rendering of original 3D only, or of all data.
//
// Revision 3.48  2002/12/23 22:27:35  kevin
// Store the frame rate for adjusting the render-size
//
// Revision 3.47  2002/12/17 22:50:00  jmeade
// Override of GetUpdateBuffer, for crisper images for movies.
//
// Revision 3.46  2002/12/12 23:29:06  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.45  2002/11/24 17:42:23  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.44  2002/11/04 20:38:19  kevin
// Mad annotations 3D instead of 2D and added drawing of annotations in endoscopic view
//
// Revision 3.43  2002/09/30 17:10:22  jmeade
// Allow Auto-fly directly out of replay mode.
//
// Revision 3.42  2002/09/18 20:06:15  jmeade
// Moved UserAdmin to ConsoleLib.
//
// Revision 3.41  2002/09/12 18:07:17  jmeade
// No need for typedef'd iterator.
//
// Revision 3.40  2002/09/10 15:45:08  jmeade
// Use one function to handle changing main dataset.
//
// Revision 3.39  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.37  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.36  2002/06/04 18:36:59  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.35  2002/05/15 20:33:44  jmeade
// Allowing for general use of registry (and not just Software\AppName\...).
//
// Revision 3.34  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.33  2002/04/18 18:27:44  jmeade
// More descriptive measure line render function names.
//
// Revision 3.32  2002/04/12 02:13:05  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.31  2002/03/28 18:50:48  jmeade
// Allow run-time setting of maximum flight speeds; Code standards.
//
// Revision 3.30  2002/03/21 02:12:30  jmeade
// Display render speed service menu option; Code standards.
//
// Revision 3.29  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.28.2.12  2002/06/12 02:29:56  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.28.2.11  2002/05/31 16:19:29  jmeade
// Issue 2331:  Moved snapshot of a view to parent class.
//
// Revision 3.28.2.10  2002/03/28 22:27:41  jmeade
// Display render speed service menu option; Code standards.
// Allow run-time setting of maximum flight speeds; Code standards.
//
// Revision 3.28.2.9  2002/03/07 19:54:14  binli
// moved the global GL render function GLDrawCircle() from 10-math::Point2D to RenderUtil
//
// Revision 3.28.2.8  2002/03/05 20:17:26  binli
// adjustment of 3D measurement line drawing.
//
// Revision 3.28.2.7  2002/03/01 17:01:25  binli
// issue 1966: when measurement line is visible, default to 'polyp'
//
// Revision 3.28.2.6  2002/02/26 16:54:15  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.28.2.5  2002/02/20 15:34:45  binli
// comments on Revision 3.28
//
// Revision 3.28.2.4  2002/02/19 21:33:01  binli
// improvement of 3D measurement line drawing: accurate pickup
//
// Revision 3.28.2.3  2002/02/15 17:21:39  binli
// issue 1866: shift point
//
// Revision 3.28.2.2  2002/02/06 21:38:46  jmeade
// Oops, not all control paths...
//
// Revision 3.28.2.1  2002/02/06 21:35:41  jmeade
// Issue 1804:  EndoProgressCtrl:  Only set progress display when necessary.
//
// Revision 3.28  2002/01/09 16:50:30  binli
// 3D measurement line: text output
//
// Revision 3.27  2002/01/08 23:42:36  jmeade
// Right-click selects a line for delete (Right-button-double-click is not standard);
// Ctrl-Right-click toggles line visibility.
//
// Revision 3.26  2001/12/31 16:03:17  binli
// 3D measurement line: adjustment:
// 1> L-Btn down: adjust exist line/start new line
// 2> R-Btn down: view a exist line (visible & invisble)
// 3> R-Btn dbl-click: delete line(s)
//
// Revision 3.25  2001/12/28 22:04:34  binli
// 3D measurement line: improve 3: the radius/color of end-point sphere
//
// Revision 3.24  2001/12/28 16:37:59  binli
// 3D measurement line: improve 2: picking on L-btn down: show invisible or move end-point
//
// Revision 3.23  2001/12/27 20:07:40  binli
// 3D measurement line: adjustment
//
// Revision 3.22  2001/12/22 00:42:13  jmeade
// Fixed compile error checked in revision 3.19.
//
// Revision 3.21  2001/12/22 00:04:35  jmeade
// Moved movie recording (for use in other views); Code standards.
//
// Revision 3.20  2001/12/21 23:03:38  binli
// 3D measurement line: basic function: improve 1: picking.
//
// Revision 3.19  2001/12/20 22:44:28  binli
// 3D measurement line: basic function (to be improved)
//
// Revision 3.18  2001/12/14 21:50:48  jmeade
// Error checking and reporting to user on movie creation.
//
// Revision 3.17  2001/12/12 20:04:09  jmeade
// Movies now save frames via current modes (translucent and flight currently).
//
// Revision 3.16  2001/12/07 18:44:31  kevin
// Added "entertainment" bitmap to movies
//
// Revision 3.15  2001/11/23 16:24:11  jmeade
// Only start autofly on the current dataset (study orientation).
//
// Revision 3.14  2001/11/21 19:09:04  jmeade
// Handle view mode change in Navigation view.
//
// Revision 3.13  2001/11/21 16:58:17  jmeade
// UI feedback while recording and saving movie.
//
// Revision 3.12  2001/11/21 00:30:08  jmeade
// Lumen coverage.
//
// Revision 3.11  2001/11/19 15:55:45  jmeade
// Standard view form.
//
// Revision 3.10  2001/11/11 19:56:09  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.9  2001/11/09 04:50:02  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.8  2001/11/09 00:42:36  jmeade
// Ensure the proper study is set for double-click autofly navigation.
//
// Revision 3.7  2001/10/29 14:14:40  kevin
// Added Jpeg Picture saving
//
// Revision 3.6  2001/10/25 13:17:45  kevin
// Added hooks for avi/mpeg saving
//
// Revision 3.5.2.3  2001/11/05 21:45:27  jmeade
// Coding standards.
//
// Revision 3.5.2.2  2001/11/04 04:34:35  jmeade
// Issue 1275; Coding standards.
//
// Revision 3.5.2.1  2001/10/28 20:54:43  jmeade
// Coding standards
//
// Revision 3.5  2001/10/24 15:23:51  binli
// issue 1253: refresh all views.
//
// Revision 3.4  2001/10/24 00:15:05  jmeade
// Issue 1251: Question mark icon for delete confirmation message box
//
// Revision 3.3  2001/10/19 02:45:49  jmeade
// Code standards
//
// Revision 3.2  2001/10/18 15:15:17  binli
// issue 1123: test flag to see if it is captured, if yes, SetForces().
//
// Revision 3.1  2001/10/17 21:57:03  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.7   Oct 10 2001 17:10:44   jmeade
// One frame rate timer per class instance
// 
//    Rev 2.6   Oct 08 2001 21:37:04   jmeade
// Specify study to be rendered (removed legacy assumption of rdrGlobal.m_pStudy)
// Resolution for 1078: Registration support
// 
//    Rev 2.5   Oct 08 2001 17:16:26   jmeade
// Removed hard-coded references to the "other" study (as per change in revision 2.4)Resolution for 1078: Registration support
// 
//    Rev 2.4   Oct 08 2001 16:54:04   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
// 
//    Rev 2.3   Oct 05 2001 19:08:28   jmeade
// Don't allow replay when measuring
// 
//    Rev 2.2   Oct 04 2001 13:36:46   jmeade
// Only display 100% percent lumen coverage if we are indeed at 100.0% (i.e. don't round up for > 99.4%)
// 
//    Rev 2.1   Sep 27 2001 20:20:46   jmeade
// Corrected mouse use key strings for Navigation view
// 
//    Rev 2.0   Sep 16 2001 23:41:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:20   ingmar
// Initial revision.
// Revision 1.152  2001/08/14 18:23:18  jmeade
// replaced m_bButtonDown with proper AsyncKeyState check
//
// Revision 1.151  2001/06/29 23:08:27  jmeade
// expand measurement constraint rect so that points outside rect won't be drawn
//
// Revision 1.150  2001/06/28 19:21:31  jmeade
// Bug fix 539: removed non-useful 3D measure profile
//
// Revision 1.149  2001/06/26 16:23:35  ana
// Results of code review of Fields and Jitter classes
//
// Revision 1.148  2001/06/21 21:15:32  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.147  2001/06/05 16:02:01  jmeade
// arrow keys for panning, rotating, and moving translucent window
//
// Revision 1.146  2001/05/29 17:22:40  jmeade
// don't use RenderUtil::IndexColors for centerline; no need for m_bMovingBiopsy;
// Bug 497: fixed error with biopsy window disappearing once moved
//
// Revision 1.145  2001/05/24 22:18:30  jmeade
// Bug 127:  Remove Hot-keys (keyboard actions for child views)
//
// Revision 1.144  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.143  2001/05/15 14:23:45  binli
// type cast: 4th parameter in GLFont::DrawText();
//
// Revision 1.142  2001/05/15 13:52:16  binli
// bad work: whose code conversion made it not compilable!
//
// Revision 1.141  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.140  2001/05/07 00:51:33  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.139  2001/05/02 20:25:29  ingmar
// .
//
// Revision 1.138  2001/04/30 12:33:31  frank
// Reflected changes in Study class.
//
// Revision 1.137  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.136.2.1  2001/05/02 00:09:17  jmeade
// m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.136  2001/04/19 19:22:05  jmeade
// CglFont class renamed to GLFont
//
// Revision 1.135  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.134  2001/04/11 20:50:27  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.133  2001/04/11 13:27:32  frank
// Moved volume data to Fields class.
//
// Revision 1.132  2001/04/10 17:52:30  frank
// New coding conventions in Submarine class.
//
// Revision 1.131  2001/04/06 13:53:35  frank
// Reflected changes in SegmentList class.
//
// Revision 1.130  2001/04/05 18:47:58  jmeade
// tabs to spaces; EnableWindow() override
//
// Revision 1.129  2001/03/28 13:59:19  frank
// Reflected changes in SegmentList class.
//
// Revision 1.128  2001/03/26 16:03:54  frank
// Reflected changes in Segment class.
//
// Revision 1.127  2001/03/21 14:59:11  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.126  2001/03/19 12:00:55  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.125  2001/03/16 16:24:34  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.124  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.123  2001/03/08 02:49:13  jmeade
// more fun non-release animations (first cuts); coding standards
//
// Revision 1.122  2001/03/07 15:10:43  jmeade
// pan==>spin; sound for easter egg
//
// Revision 1.121  2001/02/28 23:30:30  liwei
// Overview rerendered when buttons on VerifyView clicked
//
// Revision 1.120  2001/02/27 19:26:27  jmeade
// my bad: must/should advance the "physical simulation" after each render
//
// Revision 1.119  2001/02/27 00:31:27  jmeade
// CPoint ==> Point2D<>; circles to mark measure line ends;
// tabs to spaces
//
// Revision 1.118  2001/02/23 20:11:31  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.117  2001/02/22 01:35:33  jeff
// code without coding conventions is gettting harder and harder to comprehend
//
// Revision 1.116  2001/02/19 17:18:07  jeff
// OnKeyPressed() is obsoleete, identical to OnKeyDown()
//
// Revision 1.115  2001/02/15 13:47:18  frank
// Viatronix...the game.
//
// Revision 1.114  2001/02/05 20:51:07  jeff
// measure ==> line measure and roi measure
//
// Revision 1.113  2001/02/02 07:28:19  jeff
// Moving strings to resource table
//
// Revision 1.112  2001/01/31 22:20:32  geconomos
// Implemented virtual GetUpdateBuffer()
//
// Revision 1.111  2001/01/31 19:15:02  jeff
// redraw text windows in progress control after moving
//
// Revision 1.110  2001/01/31 19:04:34  kevin
// Moved transferfunction to datastruct so triangle rendering could get
// correct colors
//
// Revision 1.109  2001/01/31 13:23:51  frank
// Upgraded to modern data structures.
//
// Revision 1.108  2001/01/30 21:12:39  jeff
// render size only for debugging purposes
//
// Revision 1.107  2001/01/30 04:41:32  kevin
// FIxed the live update of painted information
//
// Revision 1.106  2001/01/27 02:57:12  kevin
// Hooked up the display paintinfo to the overview.
// (note there are still some bugs with the grab image,
// but I can't seemm to get it to connect from readerlib
// into viatron. ARGGHH I am a coding dinosaur)
//
// Revision 1.105  2001/01/26 20:56:31  frank
// Adjusted wagging tail.
//
// Revision 1.104  2001/01/26 02:05:56  jeff
// use LoadResourceString() helper function
//
// Revision 1.103  2001/01/25 00:05:45  jeff
// frame rate only for debugging
//
// Revision 1.102  2001/01/24 15:57:50  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.101  2001/01/24 12:52:25  frank
// Tail waggling.
//
// Revision 1.100  2001/01/23 17:41:28  jeff
// only the high bit of return value of GetAsyncKeyState() signifies key down
//
// Revision 1.99  2001/01/19 21:07:10  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.98  2001/01/13 00:56:56  jeff
// Using new Rect<> and Tuple<> classes; abbreviating code
//
// Revision 1.97  2001/01/12 21:49:27  frank
// Consolidate code.
//
// Revision 1.96  2001/01/12 19:52:23  frank
// Moved code to ReaderGlobal
//
// Revision 1.95  2001/01/12 17:59:54  frank
// Added linear profile rendering.
//
// Revision 1.94  2001/01/08 18:43:20  frank
// Clarified Supine-Prone dataset distinction.
//
// Revision 1.93  2001/01/05 19:27:40  frank
// Moved marking functions into Study class.
//
// Revision 1.92  2001/01/05 15:36:02  frank
// Used SegmentList to contain navigation direction.
//
// Revision 1.91  2001/01/02 22:58:25  frank
// Removed old keyboard hotkey.
//
// Revision 1.90  2001/01/02 17:28:53  jeff
// removed unused CNavigationView::InfoDlgBar class
//
// Revision 1.89  2001/01/02 17:12:59  frank
// no message
//
// Revision 1.88  2000/12/28 00:00:29  jeff
// global GetViatronMainFrame() ==> static MainAppFrame::GetMainFrame()
//
// Revision 1.87  2000/12/27 17:49:36  kevin
// CHanges to remove Mprviewer
//
// Revision 1.86  2000/12/26 23:54:59  jeff
// remove posting of now-obsolete elapsed time message
//
// Revision 1.85  2000/12/22 23:57:56  jeff
// no message
//
// Revision 1.84  2000/12/22 19:31:41  frank
// Changed the style of panning back to relative mode from absolute.
//
// Revision 1.83  2000/12/22 16:49:11  jeff
// MoveWindow() for dialog bar, strangely, doesn't preserve size set by parameters,
//  so use SetWindowPos() with SWP_NOSIZE
//
// Revision 1.82  2000/12/19 02:30:26  kevin
// Added Grid Shadow
//
// Revision 1.81  2000/12/18 23:05:01  kevin
// Removed Unwrapped view and Hybrid View Since they
// won't be active in the final product
//
// Revision 1.80  2000/12/12 23:54:09  jeff
// popup menu only when not main view window
//
// Revision 1.79  2000/12/12 18:02:46  jeff
// turn off autofly when a right mouse click received (as with left mouse)
//
// Revision 1.78  2000/12/11 14:45:15  frank
// Added toggle for main image display
//
// Revision 1.77  2000/11/24 18:26:16  kevin
// Hack to fix measureing outside window (Should be fixed properly later)
//
// Revision 1.76  2000/11/23 17:04:56  kevin
// Crutch to fix jumping on menu-leave (basically getting mouse
// move with a previous button-down event)
//
// Revision 1.75  2000/11/23 00:55:25  jeff
// Removed unused EndoCoverageProgressCtrl::OnMove() function; attempt to
//  create progress control *before* statics; call MoveWindow() from Create() function;
//  use global OpenGL font
//
// Revision 1.74  2000/11/22 23:16:25  liwei
// 2D measuring now stays in view
//
// Revision 1.73  2000/11/22 19:17:40  kevin
// Made Progress number be round instead of trunc
//
// Revision 1.72  2000/11/21 04:20:22  jeff
// Removed border from, set font in, and resized, coverage static text
//
// Revision 1.71  2000/11/21 02:41:34  jeff
// Added label to progress control;
// Overrode MoveWindow() to size label, progress, and buddy within specified
//  view rectangle for the progress control
//
// Revision 1.70  2000/11/20 17:59:48  jeff
// Code for static next to progress control
//
// Revision 1.69  2000/11/20 19:04:11  kevin
// Fixed the overview buffer on patch jump
//
// Revision 1.68  2000/11/20 16:59:21  jeff
// Percent examined (progress) bar
//
// Revision 1.67  2000/11/20 15:57:01  frank
// Enabled toggling accumulation buffer
// Fixed centerline coloring
//
// Revision 1.66  2000/11/20 03:34:21  kevin
// Clean-Up
//
// Revision 1.65  2000/11/20 02:02:17  jeff
// Oh, the horror!  I checked in a compile error! (an extra parenthesis)
//
// Revision 1.64  2000/11/20 01:07:18  jeff
// GetMouseKeyStringIDs() -- allows each view to set strings for mouse button use key
//
// Revision 1.63  2000/11/19 23:46:56  kevin
// Changed RGBvol to PaintVol to reflect new meaning
//
// Revision 1.62  2000/11/19 22:47:15  kevin
// Fixed Measureing with new non-power-of-2 renderin buffer
// and aded FlushAcuclumlationbuffer calls
//
// Revision 1.61  2000/11/18 18:34:24  kevin
// Temp change for debug, basically no change since previous version but CVS thinks so for some reason
//
// Revision 1.60  2000/11/17 17:46:27  kevin
// Added rendersize debug info
//
// Revision 1.59  2000/11/17 16:11:32  frank
// Flushed accumulation buffer upon mode changes
//
// Revision 1.58  2000/11/17 04:43:50  kevin
// Made Adaptive Sampling the default
//
// Revision 1.57  2000/11/17 04:21:52  kevin
// Made old strangth slider work for renderSize (for adaptive sampling)
// Temporary for testing what size is optimal
//
// Revision 1.56  2000/11/16 20:45:46  kevin
// Made some volume render stuff inline
//
// Revision 1.55  2000/11/16 17:41:54  kevin
// Added some FLushAccumulationBUffer Calls
//
// Revision 1.54  2000/11/15 14:04:42  frank
// Updated to reflect new ReaderGlobal inlines
//
// Revision 1.53  2000/11/14 18:56:21  frank
// Changed study reference to pointer
//
// Revision 1.52  2000/11/14 14:30:02  frank
// Added some more debug info
//
// Revision 1.51  2000/11/14 07:56:41  jeff
// Pan only with control and middle mouse (instead of *either* left or right)
//
// Revision 1.50  2000/11/14 02:16:50  kevin
// Changed the projection enums
//
// Revision 1.49  2000/11/10 20:13:54  frank
// Added some debug info
//
// Revision 1.48  2000/11/10 20:33:54  kevin
// Cleanedup SaveMasekdRGB from VSD
//
// Revision 1.47  2000/11/09 20:11:29  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.46  2000/11/09 14:53:27  frank
// Fixed centerline coloring
//
// Revision 1.45  2000/11/09 02:55:28  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.44  2000/11/08 17:39:18  frank
// Added m_study to ReaderGlobal
//
// Revision 1.43  2000/11/08 04:41:33  frank
// Moved ecv into Study class
//
// Revision 1.42  2000/11/02 17:42:09  kevin
// Moved Transfer marking to detecting missed regions
//
// Revision 1.41  2000/11/01 22:44:59  jeff
// Flags and point of the last mouse click passed into GetMenuPopupID()
// (for the child view to use to decide which popup menu to show)
//
// Revision 1.40  2000/10/31 17:13:35  frank
// Syntax changes due to new skeleton structure
//
// Revision 1.39  2000/10/30 13:37:44  kevin
// Added Biopsy Mode for moving the magic window around
//
// Revision 1.38  2000/10/29 20:17:37  kevin
// CHanged CalculateMeasurePoint to Calculate 3DPoint
// and added jump orthogonal position to mouseclick
//
// Revision 1.37  2000/10/29 19:40:08  kevin
// Added Missed area jump to patches
//
// Revision 1.36  2000/10/28 01:16:31  ingmar
// render call syntax
//
// Revision 1.35  2000/10/28 00:54:35  jeff
// CPoint distance function
// Right-click delete measurement line (after verify); simple equation used to
//   determine if click point is near measurement line
//
// Revision 1.34  2000/10/27 20:39:09  jeff
// Calculate (and store) measurement distances only when user
//  is moving endpoints, read when drawing (cuts down on constant recalculations)
// Find the *nearest* endpoint in list for moving (and not just the first
//  proximal point)
//
// Revision 1.33  2000/10/27 01:14:44  kevin
// CHanges to Marking and some code cleanup
//
// Revision 1.32  2000/10/26 23:07:38  jeff
// Detect proximity to the top and right edge of the view window
//  (and adjust accordingly) when drawing text
// Set the (text draw) position when choosing an existing measure line
// Use GDT_LEFT when drawing at right edge
//
// Revision 1.31  2000/10/26 20:51:26  jeff
// Persistent list of measurements (stored in ReaderGlobal)
//
// Revision 1.30  2000/10/25 19:32:41  jeff
// Removed resetting of (moved) light position on button up (allowing
// for persistent light position during one light mode session)
//
// Revision 1.29  2000/10/25 17:39:11  jeff
// New popup menu paradigm for view windows
//
// Revision 1.28  2000/10/20 17:09:54  kevin
// Added multiple different projection methods
//
// Revision 1.27  2000/10/14 18:57:51  kevin
// Temporary comment of paitent Info and Logo for frame rate
//
// Revision 1.26  2000/10/13 04:57:58  jeff
// Pushing the controls dialog further down in the display
//
// Revision 1.25  2000/10/12 21:09:59  kevin
// Added Axial, Sagittal and Coronal Flags.
//
// Revision 1.24  2000/10/12 02:51:35  kevin
// Added  proper directional checking that considers flight direction
// as well as current segment direction. ALso removed some
// debug text
//
// Revision 1.23  2000/10/11 14:57:53  kevin
// Changes for rdrGloabl current seg/skel pointers
//
// Revision 1.22  2000/10/09 23:50:37  kevin
// Removed Color Change Debug
//
// Revision 1.21  2000/10/04 14:24:17  kevin
// Added  rdrGlobal units and dim for fast
// reading for study load speed
//
// (also some temp debug for coloring stuff)
//
// Revision 1.20  2000/10/03 00:53:36  kevin
// Updated Angle and Volume Measurement
//
// Revision 1.19  2000/09/30 13:11:06  kevin
// Removed performance measurement
//
// Revision 1.18  2000/09/30 05:48:51  jeff
// no message
//
// Revision 1.17  2000/09/27 16:26:19  frank
// Moved segment data structures to v2k
//
// Revision 1.16  2000/09/22 17:48:09  kevin
// Added perfromance measuremnet
//
// Revision 1.15  2000/09/21 20:53:54  kevin
// Returned to Submarine naming post-code-walkthrough
//
// Revision 1.14  2000/09/21 19:48:19  kevin
// I dind't make any changes, why does this stupid program think that I
// need to check this back in
//
// Revision 1.13  2000/09/21 19:35:56  frank
// New Submarine function names
//
// Revision 1.12  2000/09/20 19:07:09  binli
// followed the change in class Submarine (member functions: Update(), Panto(), Replay())
//
// Revision 1.11  2000/09/19 17:28:43  binli
// Followed Frank's names
//
// Revision 1.10  2000/09/19 16:17:09  kevin
// Commented grey-screen debug (After 4 days with no success, I give up)
// AND MOVED UNWRAPPED COLON DRAWING TO IT'S OWN PLACE
//
// Revision 1.9  2000/09/18 15:03:03  kevin
// Added GLDebug to find grey-screen error
//
// Revision 1.8  2000/09/15 16:09:28  jeff
// All rendering/drawing now done in OpenGL (no more GDI draw calls)
//
// Revision 1.7  2000/09/15 01:58:25  jeff
// Moving to text draw in OpenGL
//
// Revision 1.6  2000/09/13 19:24:16  jeff
// Only place controls dialog bar under a main view window instance
//
// Revision 1.5  2000/09/13 16:31:18  jeff
// Removing unnecessary references to obsolete CChildView class
//
// Revision 1.4  2000/09/12 20:47:03  jeff
// Resizing and repositioning overview and (coming soon) visible-surface views
//
// Revision 1.3  2000/09/12 14:54:51  frank
// Changes for coding standard compliance
//
// Revision 1.2  2000/09/11 20:04:39  kevin
// Updated hooks for volume measurement
//
// 
// 90    9/09/00 1:55p Kevin
// Added angular measurement and hooks for volume measurement
// 
// 89    9/08/00 6:07p Jeff
// changed IndependentModes() and MutexModes() to Get...Modes()
// 
// 88    9/07/00 4:14p Frank
// Improved examined region overlay on unwrapped colon
// 
// 87    9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 86    9/06/00 3:11p Frank
// Added ctrl-key panning to navigation
// 
// 85    9/06/00 12:46p Jeff
// GLRenderScene() and PostGLRender() call base class implementations when
// there is no data to draw
// 
// 84    9/02/00 4:19p Kevin
// Added drawing of the unwrapped colon
// 
// 83    9/01/00 4:56p Frank
// Worked on unwrapping the colon wall
// 
// 82    9/01/00 3:46p Jeff
// More incremental changes for modes
// 
// 81    8/31/00 4:19p Frank
// Added spherical projection visualization mode
// 
// 80    8/31/00 2:32p Frank
// Slowed down travel during turns
// 
// 79    8/31/00 2:13p Frank
// Added double-click autofly start
// 
// 78    8/30/00 6:32p Jeff
// One standardarized enum value class for all mode windows
// 
// 77    8/29/00 6:42p Jeff
// Mode changes
// 
// 76    8/29/00 4:17p Jeff
// A new class for protected access (using a callback function) to a
// value, typically used for mode value setting
// 
// 75    8/28/00 1:30p Antonio
// updated header and footer
//
//*****************  Version 74  *****************
//User: Jeff            Date:  8/25/00  Time:  7:08p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added popup menu
//
//*****************  Version 73  *****************
//User: Jeff            Date:  8/25/00  Time:  5:32p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added calls to base class methods where necessary
//
//*****************  Version 72  *****************
//User: Jeff            Date:  8/25/00  Time: 11:21a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Removed GuiNavEnum::Get(), use (automatic type conversion) operator
//int() instead
//
//*****************  Version 71  *****************
//User: Jeff            Date:  8/24/00  Time:  2:24a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Removed some extra unnecessary sprintf's to char[] intermediate
//steps (use CString::Format())
//Restrict measure line drawing to client area
//
//*****************  Version 70  *****************
//User: Kevin           Date:  8/21/00  Time:  5:00p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added scaleing (window/level-like functionality) of the translucent
//transfer function
//
//*****************  Version 69  *****************
//User: Kevin           Date:  8/21/00  Time:  3:41p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added segment length checking
//
//*****************  Version 68  *****************
//User: Kevin           Date:  8/20/00  Time:  1:48p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Fixed centerline coloring
//
//*****************  Version 67  *****************
//User: Kevin           Date:  8/19/00  Time:  1:22p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Updated screen text
//
//*****************  Version 66  *****************
//User: Kevin           Date:  8/18/00  Time:  3:35p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added resolution to measurement
//
//*****************  Version 65  *****************
//User: Kevin           Date:  8/18/00  Time:  2:59p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added direction to skeleton coloring
//
//*****************  Version 64  *****************
//User: Frank           Date:  8/18/00  Time:  2:58p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added frame/sec display
//
//*****************  Version 63  *****************
//User: Frank           Date:  8/18/00  Time:  1:33p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Added possible neighbor searching to navigation
//
//*****************  Version 62  *****************
//User: Jeff            Date:  8/18/00  Time: 12:23a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Removed m_flying from ReaderGlobal
//
//*****************  Version 61  *****************
//User: Jeff            Date:  8/17/00  Time: 11:37p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Fixed bug in determining whether or not to draw measure distance (if
//in/out of view rectangle)
//
//*****************  Version 60  *****************
//User: Kevin           Date:  8/17/00  Time:  9:48p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Fixed window jumping around
//
//*****************  Version 59  *****************
//User: Frank           Date:  8/17/00  Time: 11:54a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added navigation test mode
//
//*****************  Version 58  *****************
//User: Kevin           Date:  8/16/00  Time:  7:54p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added dfs/dfe (BUT, it is not in world coordinates)
//
//*****************  Version 57  *****************
//User: Kevin           Date:  8/16/00  Time:  1:55p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added read of threshold value from .thr file and adjust to 8 bit
//value
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 56  *****************
//User: Kevin           Date:  8/14/00  Time:  8:33p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added RGB marked volume writing for Revli viewing
//
//*****************  Version 55  *****************
//User: Jeff            Date:  8/14/00  Time:  2:33p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  
//
//*****************  Version 54  *****************
//User: Kevin           Date:  8/13/00  Time:  4:30p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added Visible Rendering Marking
//
//*****************  Version 53  *****************
//User: Frank           Date:  8/11/00  Time:  4:24p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Synchronized MPR slice location with MprViewer
//
//*****************  Version 52  *****************
//User: Jeff            Date:  8/11/00  Time: 12:47p
//Checked in $/Viatron1KGUI/src/Viatron
//Comment:
//  Fixing a compile error (variable out of scope)
//
//*****************  Version 51  *****************
//User: Frank           Date:  8/11/00  Time:  8:41a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Worked on debugging x-sectional view
//
//*****************  Version 50  *****************
//User: Jeff            Date:  8/09/00  Time:  7:50p
//Checked in $/Viatron1KGUI/src/Viatron
//Comment:
//  Fixing the bug on reverse (RButtonDown) with a stroke of the minus
//key!
//(changed a positive one to a negative one)
//
//*****************  Version 49  *****************
//User: Jeff            Date:  8/09/00  Time:  6:10p
//Checked in $/Viatron1KGUI/src/Viatron
//Comment:
//  Lots and lots of changes
//
//*****************  Version 48  *****************
//User: Kevin           Date:  8/09/00  Time:  4:02p
//Checked in $/Viatron1KGUI/src/Viatron
//Comment:
//  Added update of preipheral views only when not flying live
//
//*****************  Version 47  *****************
//User: Jeff            Date:  8/08/00  Time: 10:29p
//Checked in $/Viatron1KGUI/src/Viatron
//Comment:
//  Now rendering the endoscopic view
//
//*****************  Version 46  *****************
//User: Kevin           Date:  8/08/00  Time:  9:02p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Removed RearView rendering hooks
//
//*****************  Version 45  *****************
//User: Frank           Date:  8/08/00  Time:  6:40p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Jeff: (don't worry Frank, you didn't make this change!)
//SwapBuffers() moved back to CChildView::OnPaint(), new function override
//PostGLRender() can do any device context (HDC) drawing to the window
//
//*****************  Version 44  *****************
//User: Kevin           Date:  8/07/00  Time:  6:00p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  FIxed CSI problems with new segment data structures
//
//*****************  Version 43  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 42  *****************
//User: Kevin           Date:  8/04/00  Time: 11:02a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Made the directional indicator be centerline color
//
//*****************  Version 41  *****************
//User: Kevin           Date:  8/03/00  Time:  7:43p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  fixed orientation up/right descrepencies
//
//*****************  Version 40  *****************
//User: Kevin           Date:  8/02/00  Time: 11:47a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added colors
//
//*****************  Version 39  *****************
//User: Jeff            Date:  8/02/00  Time: 11:09a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added a highlight cursor; removed references to local navigation
//mode (now stored in GuiNavigation class)
//
//*****************  Version 38  *****************
//User: Kevin           Date:  8/02/00  Time:  2:33a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Begining of multi-segmnet handling
//
//*****************  Version 37  *****************
//User: Jeff            Date:  8/01/00  Time:  1:18p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  glFlush() and SwapBuffers() when no volume is loaded (and then
//return) from GLRenderScene()
//
//*****************  Version 36  *****************
//User: Jeff            Date:  7/31/00  Time:  6:26p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added an extra check (which, if need be, can be removed)
// to ensure a volume is loaded before rendering (ReaderGlobal::IsLoadingComplete())
//
//*****************  Version 35  *****************
//User: Jeff            Date:  7/31/00  Time:  5:38p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added an enum class to GuiNavigation (GuiEnum) to:
//(1) Control access to mode vals (can only use = operator from inside class)
//(2) Reduce duplication (one each of Get,Set,operator ==,etc. methods)
//
//*****************  Version 34  *****************
//User: Kevin           Date:  7/31/00  Time:  5:22p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added vpn debug
//
//*****************  Version 33  *****************
//User: Kevin           Date:  7/31/00  Time:  3:20p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added leaps/ray and samples/ray debug
//
//*****************  Version 32  *****************
//User: Jeff            Date:  7/27/00  Time:  2:34p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added endoscopic view mode toggles to GUI
//Smoother updating of Overview dialog
//BroadcastSendMessage changed to BroadcastMessage(..., bWait)
//
//*****************  Version 31  *****************
//User: Frank           Date:  7/27/00  Time: 11:21a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added debug display
//Added B to toggle boundary field
//Renamed RGB volume debug to R
//Rename MPR shadow display to W
//
//*****************  Version 30  *****************
//User: Jeff            Date:  7/26/00  Time:  9:45p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Implemented callbacks for controls dialog endoscopic mode selection
//
//*****************  Version 29  *****************
//User: Kevin           Date:  7/26/00  Time:  6:31p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Got MPR forward positioning to work (NOTE: what about MPR shadow in
//endoscopic view?)
//
//*****************  Version 28  *****************
//User: Frank           Date:  7/26/00  Time:  4:21p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added MPR slice debug
//
//*****************  Version 27  *****************
//User: Frank           Date:  7/25/00  Time:  9:13a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added SkeletonRender class.
//Updated with STL string class.
//
//*****************  Version 26  *****************
//User: Jeff            Date:  7/24/00  Time:  7:52p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added bookmark class and associated hooks,
//(initially) only for jumping to fixed precomputed positions
//
//*****************  Version 25  *****************
//User: Kevin           Date:  7/22/00  Time: 10:30a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added hooks for adaptive Smapling
//
//*****************  Version 24  *****************
//User: Kevin           Date:  7/21/00  Time:  8:37p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added CDC drawing to place symbology over the axial slices and the
//distance measurements
//
//*****************  Version 23  *****************
//User: Kevin           Date:  7/20/00  Time: 10:17p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added Moving Lights and ifxed some distance measuring stuff
//
//*****************  Version 22  *****************
//User: Jeff            Date:  7/20/00  Time:  1:02p
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 21  *****************
//User: Jeff            Date:  7/20/00  Time: 12:44p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added a patient info header in navigation view
//
//*****************  Version 20  *****************
//User: Kevin           Date:  7/20/00  Time:  9:33a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added Measurment and Moving Lights hooks into navigation
//
//*****************  Version 19  *****************
//User: Kevin           Date:  7/19/00  Time:  3:57p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added global Field-Of-View for rear and forward view
//
//*****************  Version 18  *****************
//User: Frank           Date:  7/18/00  Time:  4:58p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Reversed orientation and allowed control key for faster
//
//*****************  Version 17  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 16  *****************
//User: Jeff            Date:  7/18/00  Time:  1:58p
//Branched at version 16
//Comment:
//  
//
//*****************  Version 15  *****************
//User: Jeff            Date:  7/18/00  Time:  1:53p
//Checked in $/v1k/src/Viatron
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 14  *****************
//User: Frank           Date:  7/18/00  Time:  1:22p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Implemented middle mouse lookaround
//
//*****************  Version 13  *****************
//User: Jeff            Date:  7/17/00  Time:  4:38p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Added background thread to handle view window updates
//Reorganized messages (e.g. put view wnd msgs in view wnd class)
//
//*****************  Version 12  *****************
//User: Jeff            Date:  7/15/00  Time:  8:00p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Incremental update:
//Attempting to have continuous updates to all views while handling OnMouseMove()
//msg in navigation view
//
//*****************  Version 11  *****************
//User: Jeff            Date:  7/15/00  Time:  2:51p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Bug fix: Removed cyclic OnKeyDown function calls (which caused stack
//overflow)
//
//*****************  Version 10  *****************
//User: Jeff            Date:  7/14/00  Time:  5:42p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Filtering KEYDOWN events to the view window containing the mouse
//cursor
//
//*****************  Version 9  *****************
//User: Kevin           Date:  7/14/00  Time:  4:36p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Made the rear view point backwards and render at half the resolution
//
//*****************  Version 8  *****************
//User: Jeff            Date:  7/14/00  Time:  2:45p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Reflects the selection between forward/rear volume view
//
//*****************  Version 7  *****************
//User: Kevin           Date:  7/14/00  Time:  2:20p
//Checked in $/Viatron1000/Viatron
//Comment:
//  Changed RGBA of floats to RGB of char and moved
//so it doesn't always grab large memory untill needed
//
//*****************  Version 6  *****************
//User: Kevin           Date:  7/14/00  Time: 10:24a
//Checked in $/Viatron1000/Viatron
//Comment:
//  temp fix for message boxes (for the moment we have to clik in the
//inwdow to get key commands)
//
//*****************  Version 5  *****************
//User: Kevin           Date:  7/14/00  Time:  9:26a
//Checked in $/Viatron1000/Viatron
//Comment:
//  Added debug rendering toggles
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/13/00  Time: 10:21p
//Checked in $/NewViatron1000/Viatron
//Comment:
//  Added ability to double resolution with 'X' key
//
//*****************  Version 3  *****************
//User: Kevin           Date:  7/13/00  Time:  9:37a
//Checked in $/NewViatron1000/Viatron
//Comment:
//  new boolean toggle fields for rendering debug
//
//*****************  Version 2  *****************
//User: Kevin           Date:  7/13/00  Time:  9:06a
//Checked in $/NewViatron1000/Viatron
//Comment:
//  Added temporary bookmarks for full_stuy4_crop dataset
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created NavigationView.cpp
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/NavigationView.C,v 1.101.2.35 2010/12/21 22:55:10 dongqing Exp $
// $Id: NavigationView.C,v 1.101.2.35 2010/12/21 22:55:10 dongqing Exp $
