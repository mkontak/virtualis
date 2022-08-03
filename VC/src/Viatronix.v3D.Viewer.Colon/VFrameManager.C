// $Id: VFrameManager.C,v 1.25.2.15 2010/04/17 05:16:24 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VFrameManager.C
// Description: Implementation of the NavFrameManager and related classes
// Owner: Jeff Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "VFrameManager.h"

#include "StudyBrowserView.h"

#include "VerifyView.h"
#include "RegistrationView.h"
#include "Segments2DView.h"
#include "Segments3DView.h"
#include "SegmentsCylindricalView.h"
#include "UDSegmentsColonWnd.h"

#include "StandardView.h"
#include "EndoAxialView.h"
#include "SupineProneViewWnd.h"
#include "MovieView.h"
#include "FullObliqueView.h"

#include "ReportViewFrame.h"
#include "ReportViewWnd.h"

#include "MainAppFrame.h"

#include "ViatronStudy.h"
#include "resource.h"
#include "V1KMessages.h"

#include "Viatron.h"
#include "User.h"
#include "vcUtils.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#ifndef _DEBUG
#define CHECK_REPORT_PERMISSIONS 1
//#endif


using namespace ColonViews::FramesManager;
using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;

//////////////////////////////////////////////////////////////////////////

/**
 * Constructor 
 */
ViatronFrameManager::TrackBallsSaver::TrackBallsSaver()
{
  Init();
} // TrackBallsSaver()


/**
 * Initialize trackballs.
 */
void ViatronFrameManager::TrackBallsSaver::Init()
{
  m_global.Reset(); m_primary.Reset(); m_secondary.Reset();
} // Init()


/**
 * Save state.
 */
void ViatronFrameManager::TrackBallsSaver::Save()
{
  if (rdrGlobal.m_pCurrDataset)     
    m_primary = rdrGlobal.m_pCurrDataset->m_trackballStandard;
  if (rdrGlobal.m_pSecondaryDataset) 
    m_secondary = rdrGlobal.m_pSecondaryDataset->m_trackballStandard;
} // Save()


/**
 * Reset state.
 */
void ViatronFrameManager::TrackBallsSaver::Set()
{
  if (rdrGlobal.m_pCurrDataset)     
    rdrGlobal.m_pCurrDataset->m_trackballStandard = m_primary;
  if (rdrGlobal.m_pSecondaryDataset)
    rdrGlobal.m_pSecondaryDataset->m_trackballStandard = m_secondary;
} // Set()


//////////////////////////////////////////////////////////////////////
// BlankLayout - used as an inbetween for reloading a layout
//////////////////////////////////////////////////////////////////////

namespace ColonViews
{
  namespace Layouts
  {
    /**
     * A blank layout
     */
    class BlankLayout : public ColonViatronixWnd
    {
    // member functions
    private:
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDD_BLANK; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return IDD_BLANK; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDD_BLANK; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return IDD_BLANK; }
    }; // class BlankLayout

    /**
     * Frame for a blank layout
     */
    class BlankLayoutFrame : public ChildLayoutFrame
    {
      DECLARE_DYNCREATE(BlankLayoutFrame)
    protected:
      /// constructor
      BlankLayoutFrame() : ChildLayoutFrame(new BlankLayout) {}
    }; // class BlankLayoutFrame
  } //namespace Layouts
} // namespace ColonViews

IMPLEMENT_DYNCREATE(BlankLayoutFrame, ChildLayoutFrame)


//////////////////////////////////////////////////////////////////////
// ViatronFrameManager
//////////////////////////////////////////////////////////////////////


/**
 * Constructor 
 *
 * @param hMainFrameWnd   Main frame window handle.
 */
ViatronFrameManager::ViatronFrameManager(HWND hMainFrameWnd)
 : m_hMainFrameWnd(hMainFrameWnd), m_uLastSelectedSubView(0)
{
} // constructor


/**
 * Create handler
 *
 * @param hMainFrameWnd   Main frame window handle.
 */
void ViatronFrameManager::Create(HWND hMainFrameWnd)
{
  m_hMainFrameWnd = hMainFrameWnd;
} // Create()

//////////////////////////////////////////////////////////////////////
// VFrameManager::StudyBrowserFrameManager
//////////////////////////////////////////////////////////////////////


/**
 * Constructor 
 *
 * @param hMainFrameWnd   Main frame window handle.
 */
VFrameManager::StudyBrowserFrameManager::StudyBrowserFrameManager(HWND hMainFrameWnd) : ViatronFrameManager(hMainFrameWnd)
{

} // StudyBrowserFrameManager()


/**
 * Called when the frame is being initialized.
 *
 * @param &vTabStrings    Receives the name of the subview selections.
 */
void VFrameManager::StudyBrowserFrameManager::OnFrameInit(stdStringVector &vTabStrings)
{
  uint4 iTabStringIDs[NUM_FRAMES];
  iTabStringIDs[FILESYSTEM_FRAME] = IDS_FILESYSTEM;

  // No file system tab for non-service users
  uint1 uNumTabs((User::GetCurrentUser().GetType() == User::UserType_Service) ? NUM_FRAMES : NUM_FRAMES - 1);

  vTabStrings.resize(uNumTabs);
  for (uint1 i(0); i < uNumTabs; i++)
  {
    vTabStrings[i] = LoadResourceString(iTabStringIDs[i]);
  }

} // OnFrameInit()


/**
 * Called when a view is gain or losing focus, or when a subview selection is changing.
 *
 * @param iPrevSubView   Previous subview.
 * @param iNewSubView    New subview.
 * @param eFocusChange   Identifies the view change.
 * @return               New ChildFrameWindow runtime class instance, if (sub) view is changing.
 */
CRuntimeClass * VFrameManager::StudyBrowserFrameManager::OnChangeViewSel(const int4 iPrevSubView, int4& iNewSubView,
                                                                         const FocusChangeEnum eFocusChange)
{
  switch (iNewSubView)
  {
    case FILESYSTEM_FRAME: // File
      ::PostMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_SOPEN_FILE, 0), LPARAM(NULL));
      break;
    default: // File
      ::PostMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_SOPEN_FILE, 0), LPARAM(NULL));
      break;
  }

  return RUNTIME_CLASS(StudyBrowserFrame);
} // OnChangeViewSel()

//////////////////////////////////////////////////////////////////////
// VFrameManager::VerifyFrameManager
//////////////////////////////////////////////////////////////////////


/**
 * Constructor 
 *
 * @param hMainFrameWnd   Main frame window handle.
 */
VFrameManager::VerifyFrameManager::VerifyFrameManager(HWND hMainFrameWnd) : ViatronFrameManager(hMainFrameWnd)
{
  m_trackballsaver.Init();
} // constructor


/**
 * Called when the frame is being initialized.
 *
 * @param &vTabStrings   Receives the name of the subview selections.
 */
void VFrameManager::VerifyFrameManager::OnFrameInit(stdStringVector &vTabStrings)
{
	if( rdrGlobal.m_bEnableDualDisplayTabs )
    vTabStrings.resize(2);
	else
    vTabStrings.resize(1);

  vTabStrings[0] = "3D Colon"; //LoadResourceString(IDS_SUPINE) + '/' + LoadResourceString(IDS_PRONE);
	if( rdrGlobal.m_bEnableDualDisplayTabs )
    vTabStrings[1] = LoadResourceString(IDS_ORTHOGONAL);
} // OnFrameInit()


/**
 * Called when a view is gain or losing focus, or when a subview selection is changing.
 *
 * @param iPrevSubView   Previous subview.
 * @param iNewSubView    New subview.
 * @param eFocusChange   Identifies the view change.
 * @return               New ChildFrameWindow runtime class instance, if (sub) view is changing.
 */
CRuntimeClass *VFrameManager::VerifyFrameManager::OnChangeViewSel(const int4 iPrevSubView, int4& iNewSubView,
                                                                  const FocusChangeEnum eFocusChange)
{
  if ( (iPrevSubView == VFrameManager::NavFrameManager::ePOS_FULLVIEW) || (iPrevSubView == VFrameManager::NavFrameManager::ePOS_BLANKVIEW) )
  {
    iNewSubView = m_uLastSelectedSubView;
  }

  switch (iNewSubView)
  {
    case 0:
      m_uLastSelectedSubView = iNewSubView;
      return RUNTIME_CLASS(VerifyLayoutFrame);
      break;

    case VFrameManager::NavFrameManager::ePOS_FULLVIEW: // Full-"screen" child view
      return RUNTIME_CLASS(FullObliqueLayoutFrame);
      break;

    case VFrameManager::NavFrameManager::ePOS_BLANKVIEW: // Do-nothing view, used as an inbetween for reloading a layout
      return RUNTIME_CLASS(BlankLayoutFrame);
      break;

    default:
      m_uLastSelectedSubView = iNewSubView;
      return RUNTIME_CLASS(VerifySliceLayoutFrame);
      break;
  } // switch (...)
} // OnChangeViewSel()

//////////////////////////////////////////////////////////////////////
// VFrameManager::NavFrameManager
//////////////////////////////////////////////////////////////////////


/**
 * Constructor 
 *
 * @param hMainFrameWnd   Main frame window handle.
 */
VFrameManager::NavFrameManager::NavFrameManager(HWND hMainFrameWnd) : ViatronFrameManager(hMainFrameWnd)
{
  m_navigationTS.Init();
  m_endoaxialTS.Init();
} // constructor


/**
 * Called when the frame is being initialized.
 *
 * @param &vTabStrings   Receives the name of the subview selections.
 */
void VFrameManager::NavFrameManager::OnFrameInit(stdStringVector &vTabStrings)
{
  const bool bDebugViews(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::AlternateLayouts()));
  std::list<int4> liTabStrID;
  liTabStrID.push_back(IDS_STANDARD);
  liTabStrID.push_back(IDS_ENDOSLICE);
  liTabStrID.push_back(IDS_3DSEGMENTS);
#ifdef FINAL_RELEASE
	//// m_bEnableDualDisplayTabs is for IP going around solution
	if (rdrGlobal.m_bDualDataLoaded && rdrGlobal.m_bEnableDualDisplayTabs)
#else
  if (1)
#endif
  {
    liTabStrID.push_back(IDS_ORTHOGONAL);
    liTabStrID.push_back(IDS_DUAL2D3D);
    if (bDebugViews)
    {
      liTabStrID.push_back(IDS_UDSEGMENTS_MAUI);
      liTabStrID.push_back(IDS_2DSEGMENTS);
      liTabStrID.push_back(IDS_CYLINDRICAL_SEGMENTS);
    }
  }
  else
  {
    if (bDebugViews)
    {
      liTabStrID.push_back(IDS_DUAL2D3D);
      liTabStrID.push_back(IDS_UDSEGMENTS_MAUI);
      liTabStrID.push_back(IDS_2DSEGMENTS);
      liTabStrID.push_back(IDS_CYLINDRICAL_SEGMENTS);
    }
  }

  vTabStrings.resize(liTabStrID.size());
  std::list<int4>::const_iterator it = liTabStrID.begin();
  for (uint4 iCurrTab(0); it != liTabStrID.end(); vTabStrings[iCurrTab] = LoadResourceString(*it), it++, iCurrTab++);
} // OnFrameInit()


/**
 * Called when a view is gain or losing focus, or when a subview selection is changing.
 *
 * @param iPrevSubView   Previous subview.
 * @param iNewSubView    New subview.
 * @param eFocusChange   Identifies the view change.
 * @return               New ChildFrameWindow runtime class instance, if (sub) view is changing.
 */
CRuntimeClass *VFrameManager::NavFrameManager::OnChangeViewSel(const int4 iPreviousSubView, int4& iNewSubView,
                                                               const FocusChangeEnum eViewFocus)
{
  { // Save state information (e.g. current view, view modes)
    GLWndRef currLayout = MainAppFrame::GetMainFrame()->GetCurrentLayout();
    {
      // Main view window meaningful only to StandardView
      GLChildView::GLVID
        iViewID(GLChildView::GLVID(currLayout.SendMessage(WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID, 0,0)));
      
      if (iViewID <= 0)
      {
        // non-standard (endo-orthogonal and dual-orthogonal) views
        iViewID = GLChildView::GLVID(currLayout.SendMessage(WM_APP_GET_VIEW_WINDOW_CLASSID, 0, 0));
      }

      if (iViewID > 0)
      {
        const GLChildView::Info childViewInfo(currLayout.GetChildViewInfo(iViewID));
        ViewModes::VIEWMODES viewMode = ViewModes::VIEWMODES(0);

        switch (childViewInfo.m_eType)
        {
          case GLChildView::eGVT_AXIAL:
            viewMode = ViewModes::AXIAL;
            break;
          case GLChildView::eGVT_SAGITTAL:
            viewMode = ViewModes::SAGITTAL;
            break;
          case GLChildView::eGVT_CORONAL:
            viewMode = ViewModes::CORONAL;
            break;
          case GLChildView::eGVT_CROSS_SECTION:
            viewMode = ViewModes::CROSS_SECTION;
            break;

          default:
            break;
        }

        if (viewMode != ViewModes::VIEWMODES(0))
        {
          ViatronStudy::ViewWndModes.m_modeSPOrthoLayout -= ViewModes::SLICE;
          ViatronStudy::ViewWndModes.m_modeSPOrthoLayout += viewMode;

          ViatronStudy::ViewWndModes.m_modeEndoOrthoLayout -= ViewModes::SLICE;
          ViatronStudy::ViewWndModes.m_modeEndoOrthoLayout += viewMode;
        }
      }
    }
  } // (end) Save state information (e.g. current view, view modes)

  { // Save trackballs
    switch (iPreviousSubView)
    {
      case ePOS_STANDARD: // navigation
        m_navigationTS.Save();
        break;

      case ePOS_ENDOORTHO: // endo-axial
        m_endoaxialTS.Save();
        break;

      case ePOS_FULLVIEW: // Full screen ortho view
        Utilities::FullScreen::ToggleOff();
      case ePOS_MOVIE: // Movie Creation View
      case ePOS_BLANKVIEW: // Temporary go-between view
        iNewSubView = m_uLastSelectedSubView;
        break;

      default:
        break;
    }
  } // (end) Save trackballs

  // current view starts out as endoscopic
  ViatronStudy::SetCurrentView(GLChildView::eGVT_NAVIGATION, rdrGlobal.m_pCurrDataset->GetOrientation());

  CRuntimeClass *pRtnNewClass = (CRuntimeClass *) NULL;

  switch (eViewFocus)
  {
    case eFocusLose:
      break;

    case eFocusGain:
      iNewSubView = m_uLastSelectedSubView;
      // Intended drop through to next case

    case eFocusViewChange:
    default:
    {
      uint4 uFlags( MF_BYPOSITION );
      // Set new navigation view
      switch (iNewSubView)
      {
        case ePOS_STANDARD: // Standard layout
          pRtnNewClass = RUNTIME_CLASS(StandardLayoutFrame);
          uFlags |= MF_ENABLED;
          //m_navigationTS.Set();
          break;

        case ePOS_ENDOORTHO: // Endo-Orthogonal
          pRtnNewClass = RUNTIME_CLASS(EndoOrthogonalLayoutFrame);
          uFlags |= MF_ENABLED;
          //m_endoaxialTS.Set();
          break;

        case ePOS_ORTHO: // Supine-Prone Orthogonal
          pRtnNewClass = RUNTIME_CLASS(DualOrthogonalLayoutFrame);
          uFlags |= MF_ENABLED;
          break;

        case ePOS_REGISTERED: // Registered Supine/Prone View
          pRtnNewClass = RUNTIME_CLASS(RegistrationLayoutFrame);
          uFlags |= MF_GRAYED;
          break;  

        case ePOS_UDSEGMENTS_MAUI:
          pRtnNewClass = RUNTIME_CLASS(UDSegmentsLayoutFrame);
          uFlags |= MF_ENABLED;
          break;

        case ePOS_2DSEGMENT: // 2D Segments
          pRtnNewClass = RUNTIME_CLASS(Segments2DLayoutFrame);
          uFlags |= MF_ENABLED;
          break;  

        case ePOS_3DSEGMENT: // 3D Segments
          pRtnNewClass = RUNTIME_CLASS(Segments3DLayoutFrame);
          uFlags |= MF_ENABLED;
          break;  

        case ePOS_CYLSEGMENT: // Cylindrical Segments
          pRtnNewClass = RUNTIME_CLASS(SegmentsCylindricalLayoutFrame);
          uFlags |= MF_ENABLED;
          break;

        case ePOS_MOVIE: // Movie Creation Layout
          iNewSubView = m_uLastSelectedSubView; // ignore this view as a valid selection, so that we can jump back to previous
          pRtnNewClass = RUNTIME_CLASS(MovieLayoutFrame);
          uFlags |= MF_GRAYED;
          break;

        case ePOS_FULLVIEW: // Full-"screen" child layout
          iNewSubView = m_uLastSelectedSubView; // ignore this view as a valid selection, so that we can jump back to previous
          pRtnNewClass = RUNTIME_CLASS(FullObliqueLayoutFrame);
          uFlags |= MF_GRAYED;
          Utilities::FullScreen::ToggleOn();
          break;

        case ePOS_BLANKVIEW: // Do-nothing view, used as an inbetween for reloading a layout
          iNewSubView = m_uLastSelectedSubView; // ignore this view as a valid selection, so that we can jump back to previous
          pRtnNewClass = RUNTIME_CLASS(BlankLayoutFrame);
          uFlags |= MF_GRAYED;
          break;

        default:
          return (CRuntimeClass *) NULL;
      }

      m_uLastSelectedSubView = iNewSubView;
      break;
    } // (end) eFocusChange and default case
  } // (end) switch(eFocusView)

  return pRtnNewClass;
} // OnChangeViewSel()


//////////////////////////////////////////////////////////////////////
// VFrameManager::ReportFrameManager
//////////////////////////////////////////////////////////////////////


/**
 * Constructor 
 *
 * @param hMainFrameWnd   Main frame window handle.
 */
VFrameManager::ReportFrameManager::ReportFrameManager(HWND hMainFrameWnd) : ViatronFrameManager(hMainFrameWnd)
{
} // constructor


/**
 * Called when the frame is being initialized.
 *
 * @param &vTabStrings   Receives the name of the subview selections.
 */
void VFrameManager::ReportFrameManager::OnFrameInit(stdStringVector &vTabStrings)
{
#ifdef CHECK_REPORT_PERMISSIONS
  if (ReportLayout::HasPermissions() && ReportLayout::CanModifyReport())
#else
  if (1)
#endif
  {
    int4 viTabs[] = { IDS_FINDINGS, IDS_PATIENTINFO, IDS_PREVIEW, IDS_OTHER_FINDINGS };
    int4 iNumTabs(sizeof(viTabs)/sizeof(viTabs[0]));
#ifdef FINAL_RELEASE
    iNumTabs--; // The Other-Findings tab is now obsolete, use it in debug builds for testing if necessary
#endif
    vTabStrings.resize(iNumTabs);
    for (int4 i(0); i<iNumTabs; vTabStrings[i] = LoadResourceString(viTabs[i]), i++);
  }
  else
  {
    vTabStrings.resize(1);
    vTabStrings[0] = LoadResourceString(IDS_PREVIEW);
  }

  if (ReportLayout::HasPermissions() && ReportLayout::CanModifyReport())
    ReportLayout::SaveSessionData();
} // OnFrameInit()


/**
 * Called when a view is gain or losing focus, or when a subview selection is changing.
 *
 * @param iPrevSubView   Previous subview.
 * @param iNewSubView    New subview.
 * @param eFocusChange   Identifies the view change.
 * @return               New ChildFrameWindow runtime class instance, if (sub) view is changing.
 */
CRuntimeClass *VFrameManager::ReportFrameManager::OnChangeViewSel(const int4 iPrevSubView, int4& iNewSubView,
                                                                  const FocusChangeEnum eFocusChange)
{
  ::SendMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_RPT_SAVE, 0), LPARAM(NULL));

  CRuntimeClass* pFrameClass = RUNTIME_CLASS(ReportLayoutFrame);

  if ( iPrevSubView == VFrameManager::NavFrameManager::ePOS_BLANKVIEW )
  {
    iNewSubView = m_uLastSelectedSubView;
  }

  switch (eFocusChange)
  {
    case eFocusLose:
      break;

    case eFocusGain:
      #ifdef CHECK_REPORT_PERMISSIONS
      if (ReportLayout::HasPermissions() && ReportLayout::CanModifyReport())
      #else
      if (1)
      #endif
      {
        // Reset previous view
        iNewSubView = m_uLastSelectedSubView; 
      }
      
      // Intended drop through to next case

    case eFocusViewChange:
    default:
    {
      switch (iNewSubView)
      {    
        case eREPORTPAGE_FINDINGS:
          #ifdef CHECK_REPORT_PERMISSIONS
          if (ReportLayout::HasPermissions() && ReportLayout::CanModifyReport())
          #else
          if (1)
          #endif
          {
            ::PostMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_RPT_COLON_FINDINGS, 0), LPARAM(NULL));
          }
          else
          {
            ::PostMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_RPT_PREVIEW,0), LPARAM(NULL));
          }
          break;

        case eREPORTPAGE_OTHERFINDINGS:
          ::PostMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_RPT_OTHER_FINDINGS,0), LPARAM(NULL));
          break;

        case eREPORTPAGE_PATIENTINFO:
          ::PostMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_RPT_MISCELLANEOUS,0), LPARAM(NULL));
          break;

        case VFrameManager::NavFrameManager::ePOS_BLANKVIEW: // Do-nothing view, used as an inbetween for reloading a layout
          return RUNTIME_CLASS(BlankLayoutFrame);
          break;

        default:
        case eREPORTPAGE_PREVIEW:
          ::PostMessage(m_hMainFrameWnd, WM_COMMAND, MAKEWPARAM(IDM_RPT_PREVIEW,0), LPARAM(NULL));
          iNewSubView = m_uLastSelectedSubView; // ignore this view as a valid selection, so that we can jump back to previous
          break;
      }

      m_uLastSelectedSubView = iNewSubView;
      break;
    }
  }

  return pFrameClass;
} // OnChangeViewSel()

// Revision History:
// $Log: VFrameManager.C,v $
// Revision 1.25.2.15  2010/04/17 05:16:24  dongqing
// small portrait display layouts
//
// Revision 1.25.2.14  2010/02/09 05:59:12  dongqing
// report layout and verify layout will return to currently selected subview after going to blank/full-screen views
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.25.2.13  2010/01/18 09:49:16  dongqing
// vertical scrollbars and blank view handling
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.25.2.12  2010/01/07 18:48:02  dongqing
// fixed issue where blank view switching not returning to intended layout
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.25.2.11  2009/12/21 17:56:42  dongqing
// disambiguating views and layouts
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.25.2.10  2009/12/10 20:06:37  dongqing
// cleaner reloading of layouts, for e.g. when changing scrollbar orientation
//
// Revision 1.25.2.9  2009/04/03 14:21:57  mkontak
// Fix so that the Console tab is not enabled when launched stand-alone or from a
// launcher
//
// Revision 1.25.2.8  2008/11/18 00:59:16  jmeade
// checking report permissions in debug mode (as well as in release mode).
//
// Revision 1.25.2.7  2008/02/15 01:46:30  jmeade
// Issue 5924: remove otherfindings tab from release builds.
//
// Revision 1.25.2.6  2008/02/14 06:59:58  jmeade
// Issue 5924: identify report layout pages properly.
//
// Revision 1.25.2.5  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.25.2.4  2007/10/03 20:34:04  jmeade
// Issue 5359: jump back to previous layout after exiting full-sreen layout.
//
// Revision 1.25.2.3  2007/09/28 00:55:45  jmeade
// Issue 5359: fullview in verification view.
//
// Revision 1.25.2.2  2007/09/20 23:08:12  jmeade
// full screen view
//
// Revision 1.25.2.1  2007/04/12 16:08:09  jmeade
// system setting for enabling srt tool.
//
// Revision 1.25  2007/03/08 17:09:48  jmeade
// code standards.
//
// Revision 1.24  2007/03/07 20:49:26  jmeade
// code standards.
//
// Revision 1.23  2007/03/01 01:18:57  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.22  2007/02/02 21:56:21  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.21  2007/01/03 05:33:32  jmeade
// future ud segments layout.
//
// Revision 1.20  2006/12/16 04:52:01  jmeade
// Issue 5220: full oblique mode on for cross-sectional view mode.
//
// Revision 1.19  2006/11/24 21:25:31  jmeade
// alternate projection layouts entry.
//
// Revision 1.18  2006/10/04 20:50:21  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.17  2006/10/04 03:46:44  jmeade
// mistakenly added an extra view.
//
// Revision 1.16  2006/10/03 01:23:17  jmeade
// UD segments.
//
// Revision 1.15  2006/08/23 20:43:39  jmeade
// Issue 4946: new strings for decubitus studies.
//
// Revision 1.14  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.13  2006/02/13 16:36:39  jmeade
// allow alternate views in debug mode.
//
// Revision 1.12  2005/11/22 19:32:33  jmeade
// AlternateProjectionLayouts viewer settings value.
//
// Revision 1.11  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.10  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.9  2005/11/07 20:40:23  geconomos
// initial com server implementation
//
// Revision 1.8.2.1  2006/02/16 21:30:26  geconomos
// issue# 4630: error open report from cd
//
// Revision 1.8  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.7  2005/09/07 13:13:57  geconomos
// Issue# 4413: Notations menu not grayed out properly for movie view. The code for the handling of the menu state has been moved to VFrameManager::OnChangeViewSel
//
// Revision 1.6  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.5  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.4  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 3.20.2.7.2.5.2.1  2005/07/12 15:55:55  jmeade
// Issue 4264: remove full oblique layout for this release.
//
// Revision 3.20.2.7.2.5  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.20.2.7.2.4  2005/05/25 01:56:41  jmeade
// oblique review.
//
// Revision 3.20.2.7.2.3  2005/04/20 16:50:01  jmeade
// Issue 4065: Don't display registration tab for single-dataset studies.
//
// Revision 3.20.2.7.2.2.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.20.2.7.2.2  2005/04/11 21:36:53  jmeade
// Issue 4065: Change dual-orthogonal view to simply "orthogonal".
//
// Revision 3.20.2.7.2.1  2005/03/29 15:00:48  jmeade
// Issue 3997: No 2D flight in slice verification.
//
// Revision 3.20.2.7  2005/02/15 19:58:46  jmeade
// merge from vc1.4 branch.
//
// Revision 3.20.2.6.2.3  2005/01/28 00:43:48  jmeade
// Return to previous view from movie-record-view.
//
// Revision 3.20.2.6.2.2  2005/01/21 20:50:55  jmeade
// Separate movie-creation view, incremental checkin.
//
// Revision 3.20.2.6.2.1  2005/01/20 21:28:13  jmeade
// Separate movie view, incremental checkin.
//
// Revision 3.20.2.6  2004/03/30 23:45:47  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.20.2.5  2003/05/27 19:34:38  jmeade
// Comments.
//
// Revision 3.20.2.4  2003/02/22 17:57:25  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.20.2.3  2003/02/22 12:20:28  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.20.2.2  2003/02/14 16:41:22  jmeade
// Issue 2843:  Don't attempt to restore previous report view when only preview is available.
//
// Revision 3.20.2.1  2003/02/14 15:37:20  jmeade
// Issue 2843:  Store current subview selection, and reset when views are first invoked.
//
// Revision 3.20  2002/12/12 22:09:39  jmeade
// Supine/Prone instead of Registration in tab text.
//
// Revision 3.19  2002/11/26 00:38:09  kevin
// Changed surface initialization logic
//
// Revision 3.18  2002/11/22 00:27:07  jmeade
// Coding standard on m_trackball* variables.
//
// Revision 3.17  2002/11/16 22:35:48  kevin
// Dont enter the Registration VIew if only a single dataset
//
// Revision 3.16  2002/11/15 00:40:56  jmeade
// Unused, one of Kevin's newfangled ideas ;-)
//
// Revision 3.15  2002/11/11 16:16:14  kevin
// Initial version of cylindrical -- still needs some polishing
//
// Revision 3.14  2002/09/18 20:07:37  jmeade
// Moved UserAdmin to ConsoleLib.
//
// Revision 3.13  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.11  2002/08/15 15:37:31  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.10  2002/05/09 20:54:16  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.9  2002/04/12 02:16:25  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.8  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.7.2.3  2002/04/24 01:02:28  jmeade
// Issue 2044:  Set current view *dataset orientation* as well as type.
//
// Revision 3.7.2.2  2002/02/27 22:26:34  jmeade
// Issue 1629:  Toggle report complete mark.
//
// Revision 3.7.2.1  2002/02/05 23:47:48  jmeade
// Issue 1629:  Check IsMarkedComplete status of report when switching to report view.
//
// Revision 3.7  2002/01/15 00:57:29  jmeade
// Default switch cases.
//
// Revision 3.6  2002/01/15 00:07:56  jmeade
// StudyBrowserFrame should post search location message when first switching to frame.
//
// Revision 3.5  2002/01/08 22:44:20  jmeade
// Users to database; Code standards.
//
// Revision 3.4  2001/12/31 19:09:12  jmeade
// Returned dual views; Registration view not in final release
//
// Revision 3.3  2001/11/21 03:41:50  jmeade
// Supine/prone screens removed.
//
// Revision 3.2  2001/11/09 04:51:04  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1  2001/11/05 17:49:59  ana
// Registration view added.
//
// Revision 3.0.2.3  2001/11/02 21:52:10  binli
// issue 1295/1128/... : solved problem from base (by Jeff M)
//
// Revision 3.0.2.2  2001/11/02 21:18:52  binli
// issue 1295: When 'ReadOnly' (login as user/administrator), GetCurrentView from ReportView, get NULL, must check it
//
// Revision 3.0.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 05 2001 02:54:34   jmeade
// Coding standards
// 
//    Rev 2.1   Oct 03 2001 19:38:20   jmeade
// Coding conventions in UserAdmin.C classes
// 
//    Rev 2.0   Sep 16 2001 23:43:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:14   ingmar
// Initial revision.
// Revision 1.38  2001/08/28 21:30:22  jmeade
// Removed (unimplemented) PACS option
//
// Revision 1.37  2001/08/15 15:36:16  jmeade
// V2000 DB location changed to "Network"
//
// Revision 1.36  2001/07/09 18:21:13  jmeade
// Bug fix 522: preview report message (not a direct call), like other handlers; delete temp pdf
// after preview; redraw windows and switch views (or close study) as appropriate
// after preview
//
// Revision 1.35  2001/05/25 08:44:14  jmeade
// user account types to UserAccountData class
//
// Revision 1.34  2001/05/23 17:55:45  jmeade
// code conventions
//
// Revision 1.33  2001/05/17 20:42:05  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.32  2001/05/17 06:18:08  jmeade
// tabs to spaces; Enumerated tab selections in Study Browser Frame Mgr;
// Instances of ViatronFrameManager derivates must set app main window
// thru Create() method
//
// Revision 1.31  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.30  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.29  2001/04/13 02:35:22  jmeade
// headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VFrameManager.C,v 1.25.2.15 2010/04/17 05:16:24 dongqing Exp $
// $Id: VFrameManager.C,v 1.25.2.15 2010/04/17 05:16:24 dongqing Exp $
