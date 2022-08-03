// $Id: ViatronixWnd.C,v 1.21.2.27 2010/04/17 08:12:43 dongqing Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViatronixWnd.C
// Description: Implementation of GLWnd with Viatronix-built controls
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "ViatronixWnd.h"
#include "Overview.h"
#include "OverviewDlgBar.h"
#include "SliceChildView.h"
#include "ControlsDialog.h"
#include "NotationsDialog.h"
#include "NavigationView.h"
#include "Segment2DChildView.h"
#include "Segment3DChildView.h"
#include "SegmentCylindricalChildView.h"
#include "UDSegmentsChildView.h"
#include "SlidingPaneView.h"
#include "MainAppFrame.h"
#include "Viatron.h"
#include "WinUtils.h"
#include "ColorProgressCtrl.h"
#include "AdjustTranslucencyDlg.h"
#include "CadResultsDlg.h"
#include "MonitorDisplay.h"
#include "MissedRegionsDlg.h"

#define FILE_REVISION "$Revision: 1.21.2.27 $"

using namespace ColonViews::Layouts;
using namespace ColonViews::Dialogs;
using namespace ColonViews::Panes;
using namespace ReaderLib;
using namespace WinUtils;

/// globals
const CSize ViatronixWnd::m_szSmallThreshold(1560,1120);
CSize ViatronixWnd::m_szAppWindow(0,0);


//////////////////////////////////////////////////////////////////////
// ViatronixWnd::CreateChildEnumStruct methods

/**
 * Creates a V3D control.
 *
 * @param hwnd     Handle to the template window identifying the new control.
 * @param lParam   Pointer to CreateChildEnumStruct, which stores information about created child controls.
 * @return         Success.
 */
BOOL CALLBACK ViatronixWnd::CreateChildEnumStruct::CreateProc(HWND hwnd, LPARAM lParam)
{
  if (!lParam)
  {
    return FALSE;
  }
  if (!IsWindow(hwnd))
  {
    LogErr("template entry hwnd invalid","ViatronixWnd::CreateChildEnumStruct","CreateProc");
  }

  CreateChildEnumStruct & enumParams = *((CreateChildEnumStruct *)lParam);

  CWnd & templateWnd = *CWnd::FromHandle(hwnd);
  CWnd & parentWnd = *CWnd::FromHandle(enumParams.m_hParent);
  if (!templateWnd)
  {
    return TRUE;
  }

  // View window class name
  int1 sTempClassName[MAX_PATH+1];
  GetClassName(templateWnd, sTempClassName, MAX_PATH);
  const std::string sViewWndClassName(sTempClassName);

  // View window styles
  uint4 uViewStyle(templateWnd.GetStyle());
  uint4 uViewExStyle(templateWnd.GetExStyle());

  // display resolution
  const int4 cx(MonitorDisplay::GetCurrentDisplayRect().Width());
  const int4 cy(MonitorDisplay::GetCurrentDisplayRect().Height());

  const bool bWidescreen( cx >= 1600 && WinUtils::MonitorDisplay::IsWideScreen() );
  const bool bPortrait(WinUtils::MonitorDisplay::IsPortrait());
  const bool bLargeView( bWidescreen || (m_szAppWindow.cx > m_szSmallThreshold.cx && m_szAppWindow.cy > m_szSmallThreshold.cy ) );

  // Get the dataset and slice orientation (meaningful only to view windows)
  DatasetOrientations::TypeEnum eDataset(DatasetOrientations::eSUPINE);
  switch (uViewExStyle & V3DS::eDatasetOrientBits)
  {
    case V3DS::eSUPINE:     eDataset = DatasetOrientations::eSUPINE;    break;
    case V3DS::ePRONE:      eDataset = DatasetOrientations::ePRONE;     break;
    case V3DS::ePRIMARY:    eDataset = DatasetOrientations::ePRIMARY;   break;
    case V3DS::eSECONDARY:  eDataset = DatasetOrientations::eSECONDARY; break;
    default:      break;
  }

  SliceOrientations::TypeEnum eSlice(SliceOrientations::eAXIAL);
  switch (uViewExStyle & V3DS::eSliceOrientBits)
  {
    case V3DS::eSAGITTAL:       eSlice = SliceOrientations::eSAGITTAL;        break;
    case V3DS::eCORONAL:        eSlice = SliceOrientations::eCORONAL;         break;
    case V3DS::eCROSS_SECTION:  eSlice = SliceOrientations::eVIEWDIR;   break;
    default:      break;
  }

  const DWORD uAtomID(GetClassLong(templateWnd, GCW_ATOM));

  // Create the new view window or control based on its type
  GLChildViewPtr pNewView(NULL);
  CWnd * pNewControl = NULL;

  V3DS::ATOMtoV3DViewClassTypeEnumMap::const_iterator atomMapIter = V3DS::v3DClassAtomIDMap.find(uAtomID);
  if (atomMapIter != V3DS::v3DClassAtomIDMap.end())
  {
    switch (atomMapIter->second)
    {
      case V3DS::eV3DCLASSPOS_ENDOLUMINAL:
      {
        const bool bVerticalLC((uViewExStyle & V3DS::eVERTICAL_LUMENCOVERAGE)==V3DS::eVERTICAL_LUMENCOVERAGE);
        pNewView = new EndoluminalView(enumParams.m_hParent, eDataset, bVerticalLC);
        break;
      }
      case V3DS::eV3DCLASSPOS_OVERVIEW:
      {
        pNewView = new OverView(enumParams.m_hParent, eDataset, uViewExStyle & V3DS::eBUFFERIMAGE);
        break;
      }
      case V3DS::eV3DCLASSPOS_SLICEVIEW:
      {
        pNewView = new SliceChildView(eSlice, eDataset, enumParams.m_hParent);
        break;
      }      
      case V3DS::eV3DCLASSPOS_SEGMENT2DVIEW:
      {
        pNewView = new Segment2DChildView(eDataset, enumParams.m_hParent, 0);
        break;
      }
      case V3DS::eV3DCLASSPOS_SEGMENT3DVIEW:
      {
        pNewView = new Segment3DChildView(eDataset, enumParams.m_hParent, 0);
        break;
      }
      case V3DS::eV3DCLASSPOS_SEGMENTCYLINDRICALVIEW:
      {
        pNewView = new SegmentCylindricalChildView(eDataset, enumParams.m_hParent, 0);
        break;
      }
      case V3DS::eV3DCLASSPOS_UDSEGMENTSMAUI:
      {
        pNewView = new UDSegmentsMauiChildView(eDataset, enumParams.m_hParent, 0);
        break;
      }
      case V3DS::eV3DCLASSPOS_PATIENTINFO:
      case V3DS::eV3DCLASSPOS_PATIENTINFO_HORIZONTAL_SMALL:
      {
        const bool bVertical((uViewExStyle & V3DS::eVERTICAL)==V3DS::eVERTICAL);
        const int4 uBkgd = bVertical ? IDB_PATIENTINFO_VERT_BKGD : (bWidescreen?IDB_PATIENTINFO_HORIZ_BKGD_WIDESCREEN:IDB_PATIENTINFO_HORIZ_BKGD);

        const bool bDualDatasetDisplay( (uViewExStyle & V3DS::eSINGLE_DATASET_DISPLAY) == 0 );
        PatientInfoDlg * pNewPID = new PatientInfoDlg(NULL, uBkgd, bDualDatasetDisplay );

        int4 iDlgID(0);
        if ( atomMapIter->second == V3DS::eV3DCLASSPOS_PATIENTINFO_HORIZONTAL_SMALL)
          iDlgID = IDD_PATIENT_INFO_HORIZONTAL_SMALL;
        else if (bVertical)
          iDlgID = /*bLargeView ? IDD_PATIENT_INFO_VERTICAL_LARGE :*/ IDD_PATIENT_INFO_VERTICAL;
        else
          iDlgID = bWidescreen ? IDD_PATIENT_INFO_WIDESCREEN : IDD_PATIENT_INFO_HORIZONTAL;

        pNewPID->Create(iDlgID, &parentWnd);
        pNewControl = pNewPID;
        break;
      }
      case V3DS::eV3DCLASSPOS_CONTROLS:
      case V3DS::eV3DCLASSPOS_CONTROLS_WIDESCREEN_SMALL:
      case V3DS::eV3DCLASSPOS_CONTROLS_WIDESCREEN_LARGE:
      {
        const bool bForcePortrait = ((uViewExStyle & V3DS::ePORTRAIT)==V3DS::ePORTRAIT);
        const bool bWideControls = bWidescreen || ((uViewExStyle & V3DS::eWIDESCREEN)==V3DS::eWIDESCREEN);

        const bool bVertical((uViewExStyle & V3DS::eVERTICAL)==V3DS::eVERTICAL);
        const bool bAbbrev((uViewExStyle & V3DS::eABBREVIATED_CONTROLS));
  
        int4 iBkgdID(0);
        const uint4 iControlsDlgBkgdStyleBit(0xF00000);
        switch ( uViewExStyle & iControlsDlgBkgdStyleBit )
        {
          case 0x100000:
            iBkgdID = IDB_CADRES_DLG; // vertical colon-coloured dialog bkgd
            break;
          case 0x400000:
            iBkgdID = IDB_ENDOVIEW_VERT_DLGBAR; // i.e. the pane background
            break;
          case 0x500000:
            iBkgdID = IDB_DASHBOARD_BKGD; // long, blue bkgd
            break;
          default:
            iBkgdID = IDB_ENDOVIEW_HORIZ_DLGBAR; // horizontal colon-coloured dialog
            break;
        }
        uViewExStyle &= ~iControlsDlgBkgdStyleBit;

        ControlsDialog * pNewCCD = new ControlsDialog(NULL, iBkgdID);

        int4 iDlgID(0);
        if ( atomMapIter->second == V3DS::eV3DCLASSPOS_CONTROLS_WIDESCREEN_SMALL)
        {
          iDlgID = IDD_CONTROLS_HORIZONTAL_WIDESMALL;
        }
        else if (bAbbrev)
        {
          iDlgID = IDD_CONTROLS_ABBREV;
        }
        else
        {
          if (bVertical)
            iDlgID = bLargeView ? IDD_CONTROLS_VERTICAL_LARGE : IDD_CONTROLS_VERTICAL;
          else if (bForcePortrait)
            iDlgID = IDD_CONTROLS_HORIZONTAL_WIDESCREEN;
          else
            iDlgID = bLargeView ? ( bWideControls ? IDD_CONTROLS_HORIZONTAL_WIDESCREEN : IDD_CONTROLS_HORIZONTAL_LARGE )
                                : ( bWideControls ? IDD_CONTROLS_HORIZONTAL_WIDESMALL  : IDD_CONTROLS_HORIZONTAL );
        }

        pNewCCD->Create(iDlgID, &parentWnd);
        pNewControl = pNewCCD;
        break;
      }
      case V3DS::eV3DCLASSPOS_MISSEDREGIONS:
      {
        MissedRegionsDlg * pMissedDlg = new MissedRegionsDlg;
        pMissedDlg->Create(MissedRegionsDlg::IDD, &parentWnd);
        pNewControl = pMissedDlg;
        break;
      }
      case V3DS::eV3DCLASSPOS_NOTATIONS:
      {
        NotationsDialog * pNewNtd = new NotationsDialog;
        pNewNtd->Create(IDD_NOTATIONS, &parentWnd);
        pNewControl = pNewNtd;
        break;
      }
      case V3DS::eV3DCLASSPOS_OVERVIEWTOOLBAR:
      {
        OverViewDlgBar * pNewODB = new OverViewDlgBar;
        pNewODB->Create(&parentWnd);
        pNewControl = pNewODB;
        break;
      }
      case V3DS::eV3DCLASSPOS_SLICESCROLLBAR:
      {
        const int4 iOrientnStyle(SliceChildView::GetScrollBarLocationEnum()==SliceChildView::SBO_BOTTOM?SBS_HORZ:SBS_VERT);
        SliceScrollBar * pNewSSB = new SliceScrollBar(IDB_SCROLLBARIMAGES);
        pNewSSB->Create((WS_VISIBLE&uViewStyle) | WS_CHILD | iOrientnStyle, CRect(), &parentWnd, templateWnd.GetDlgCtrlID());
        uViewStyle = pNewSSB->GetStyle();
        pNewControl = pNewSSB;
        break;
      }
      case V3DS::eV3DCLASSPOS_VIATRONCHILDWND:
      {
        pNewControl = new ViatronixChildWnd(templateWnd.GetDlgCtrlID());
        pNewControl->Create(NULL, TEXT(""), WS_CHILD, CRect(0,0,1,1), &parentWnd, templateWnd.GetDlgCtrlID());
        break;
      }
      case V3DS::eV3DCLASSPOS_PANEHEADER:
      {
        // Temporarily store id in child window variable
        PaneHeader * pNewPane = new PaneHeader(HWND(uViewExStyle & V3DS::ePANEHEADERIDPOS));
        uViewStyle |= BS_OWNERDRAW;
        uViewExStyle &= ~V3DS::ePANEHEADERIDPOS;
        pNewPane->Create(CString(), uViewStyle, CRect(0,0,1,1), &parentWnd, templateWnd.GetDlgCtrlID());

        pNewControl = pNewPane;
        break;
      }
      case V3DS::eV3DCLASSPOS_ADJUSTTRANSLUCENCY:
      {
        const bool bWideControls = bWidescreen || ((uViewExStyle & V3DS::eWIDESCREEN)==V3DS::eWIDESCREEN);
        AdjustTranslucencyDlg * pNewAdjustDlg = new AdjustTranslucencyDlg(NULL, bWideControls ? IDB_DASHBOARD_BKGD : IDB_ENDOVIEW_HORIZ_DLGBAR);
        pNewAdjustDlg->Create(!bWideControls ? (!bLargeView ? AdjustTranslucencyDlg::IDD : AdjustTranslucencyDlg::IDD_LARGE)
                                             : (!bLargeView ? AdjustTranslucencyDlg::IDD_WIDESMALL : AdjustTranslucencyDlg::IDD_WIDE), &parentWnd);

        pNewControl = pNewAdjustDlg;
        break;
      }
      case V3DS::eV3DCLASSPOS_CADRESULTS:
      {
        const bool bVertical((uViewExStyle & V3DS::eVERTICAL)==V3DS::eVERTICAL);
        const bool bBkgdPane((uViewExStyle & V3DS::eBKGD_PANE)==V3DS::eBKGD_PANE);
        CadResultsDlg * pNewCadDlg = new CadResultsDlg(&parentWnd, bBkgdPane ? IDB_ENDOVIEW_VERT_DLGBAR : IDB_CADRES_DLG, eDataset);
        pNewCadDlg->Create(bLargeView ? IDD_CAD_RESULTS_PANE_LARGE : (!bVertical ? IDD_CAD_RESULTS : IDD_CAD_RESULTS_PANE), &parentWnd);

        pNewControl = pNewCadDlg;
        break;
      }
      case V3DS::eV3DCLASSPOS_BUTTON:
      {
        // Bit 1 on in extended style specifies autocheckbutton
        SkinButton * pNewSB = (uViewExStyle & V3DS::eSKINAUTOCHECKBUTTON) ? new SkinAutoCheckButton : new SkinButton;

        // Set button style by hand since the styles from the custom controls not meaningful to framework
        uViewStyle = (WS_CHILD | WS_GROUP | (WS_VISIBLE&uViewStyle) | BS_OWNERDRAW | BS_BITMAP);

        pNewSB->Create(CString(), uViewStyle, CRect(), &parentWnd, templateWnd.GetDlgCtrlID());
        pNewControl = pNewSB;
        break;
      }
      case V3DS::eV3DCLASSPOS_COLORPROGRESSCTRL:
      {
        ColorProgressCtrl * pNewProgress = new ColorProgressCtrl;
        if (uViewExStyle & V3DS::eCPS_VERTICAL)
          uViewStyle |= PBS_VERTICAL;
        else
          uViewStyle &= ~PBS_VERTICAL;
        pNewProgress->Create(uViewStyle, CRect(0,0,1,1), &parentWnd, templateWnd.GetDlgCtrlID());
        pNewControl = pNewProgress;
        break;
      }
      default:
      {
        break;
      }
    }
  }
  else if (sViewWndClassName == "Button")
  {
    CString sViewWndText;
    templateWnd.GetWindowText(sViewWndText);

    if ((uViewStyle & BS_GROUPBOX) && (sViewWndText == V3DS::sV3DClassNames[V3DS::eV3DCLASSPOS_SLIDINGPANEVIEW]))
    {
      // TODO: Use some unique characteristic to identify pane view (to allow for an actual group box)
      SlidingPaneManager * pNewPaneView = new SlidingPaneManager;
      pNewPaneView->Create(sViewWndText, uViewStyle, CRect(), &parentWnd, templateWnd.GetDlgCtrlID());
      pNewControl = pNewPaneView;

      if (enumParams.m_pPaneManager != NULL)
      {
        throw ex::UnsupportedOperationException(LogRec("Only one managed sliding pane per view window currently implemented",
          "ViatronViewWnd", "CreateChildEnumProc"));
      }

      enumParams.m_pPaneManager = pNewPaneView;
    }
    else
    {
      SkinButton * pNewSB = new SkinButton;
      pNewSB->Create(sViewWndText, uViewStyle, CRect(), &parentWnd, templateWnd.GetDlgCtrlID());
      pNewControl = pNewSB;
    }
  }
  else if (sViewWndClassName == "Static")
  {
    CString sViewWndText;
    templateWnd.GetWindowText(sViewWndText);

    CStatic * pNewStatic = new CStatic;
    pNewStatic->Create(sViewWndText, uViewStyle, CRect(), &parentWnd, templateWnd.GetDlgCtrlID());
    pNewControl = pNewStatic;
  }
  else
  {
    CString sViewWndText;
    templateWnd.GetWindowText(sViewWndText);
    pNewControl = new CWnd;
    pNewControl->Create(sViewWndClassName.c_str(), sViewWndText, uViewStyle, CRect(),
      &parentWnd, templateWnd.GetDlgCtrlID(), NULL);
  }

  // Set window styles
  if ((uViewExStyle & V3DS::ePANEHEADERIDPOS) && pNewControl)
  {
    enumParams.m_mapPaneChildID[uViewExStyle & V3DS::ePANEHEADERIDPOS] = HWND(*pNewControl);
    uViewExStyle &= ~V3DS::ePANEHEADERIDPOS;
    uViewExStyle &= ~V3DS::eSLIDINGPANEID_BITPOS;
  }

  // Set the view/control in the associated vector
  if (pNewView != NULL)
  {
    DWORD dwStyle = WS_CHILD;
    dwStyle |= ( templateWnd.GetStyle() & WS_VISIBLE );
    
    pNewView->Create( NULL, NULL, dwStyle, CRect( 0, 0, 1, 1 ), CWnd::FromHandle( enumParams.m_hParent ), templateWnd.GetDlgCtrlID() );
    
    // Set dialog control ID
    pNewView->SetControlID(templateWnd.GetDlgCtrlID());
    // Set visibility flag
    pNewView->ShowWindow((templateWnd.GetStyle() & WS_VISIBLE) == WS_VISIBLE);

    // Add child view to list
    enumParams.m_viewsList.push_back(pNewView);
  }

  if (pNewControl != NULL)
  {
    HFONT hFont = MainAppFrame::GetMainFrame()->m_hMainVtxFont;
    pNewControl->SendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));

    // Set dialog control ID
    pNewControl->SetDlgCtrlID(templateWnd.GetDlgCtrlID());

    uViewExStyle &= ~(0x1 + V3DS::eCAPTION);
    pNewControl->ModifyStyle(pNewControl->GetStyle(), uViewStyle);
    pNewControl->ModifyStyleEx(pNewControl->GetExStyle(), uViewExStyle);

    // Add control to list
    enumParams.m_controlsList.push_back(pNewControl);
  }

  return TRUE;
} // ViatronixWnd::CreateChildEnumStruct::CreateProc


//////////////////////////////////////////////////////////////////////
// ViatronixWnd methods

uint4 ViatronixWnd::m_uInvalidChildViews = (uint4(GLChildView::eGVT_NONE));

/**
 * Empty WindowProc procedure for V3D View class registration.
 *
 * @param HWND     Unused.
 * @param UINT     Unused.
 * @param WPARAM   Unused.
 * @param LPARAM   Unused.
 * @return         Unused.
 */
LRESULT CALLBACK ViatronixWnd::DoNothingWNDPROC(HWND, UINT, WPARAM, LPARAM)
{
  return 1;
} // DoNothingWNDPROC()


/**
 * Registers a V3D Window class.
 *
 * @param sName       Input name of the class.
 * @param outAtomID   On success, output identifier of the new window class.
 * @return            Success.
 */
bool ViatronixWnd::RegisterV3DClass(const std::string & sName, ATOM & outAtomID)
{
  WNDCLASSEX wndClass;
  memset((void *) &wndClass, 0, sizeof(wndClass));
  wndClass.cbSize = sizeof(wndClass);
  wndClass.lpszClassName = sName.c_str();
  wndClass.hInstance = AfxGetInstanceHandle();
  wndClass.lpfnWndProc = DoNothingWNDPROC;
  wndClass.cbClsExtra = 40;
  wndClass.cbWndExtra = 40;
  ATOM atomRegRtn = RegisterClassEx(&wndClass);
  if (atomRegRtn != 0)
  {
    outAtomID = atomRegRtn;
    return true;
  }

  return false;
} // RegisterV3DClass()


/**
 * Constructor
 *
 * @param uBkgdBmpID        Resource ID of the background image.
 */
ViatronixWnd::ViatronixWnd(const uint4 uBkgdBmpID) : GLWnd(uBkgdBmpID)
{
  for (uint4 i(0); i < V3DS::eNUM_V3D_CLASSES; i++)
  {
    ATOM newAtom(ATOM(0));
    RegisterV3DClass(V3DS::sV3DClassNames[i], newAtom);
    V3DS::v3DClassAtomIDMap[newAtom] = V3DS::V3DViewClassTypeEnum(i);
  }

  m_bIsLeafClass = dynamic_cast< ColonViatronixWnd *>( this ) == NULL;
} // constructor

BEGIN_MESSAGE_MAP(ViatronixWnd, GLWnd)
END_MESSAGE_MAP()

/**
 * WM_DESTROY handler
 */
void ViatronixWnd::OnDestroy()
{
  LogDbg( "ViatronixWnd::OnDestroy()", "", "" );

  GLWnd::OnDestroy();

  for( CWndPtrList::iterator iter = m_controlPtrList.begin() ; iter != m_controlPtrList.end(); ++iter )
  {
    (*iter)->DestroyWindow();
    delete (*iter);
  }
  m_controlPtrList.clear();
  
} // OnDestroy()


/**
 * Message WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE: Flags all child views of a specified type to be updated on next render (WM_PAINT)
 *
 * @param wpGLChildViewType   GLChildView::ViewTypeEnum value, indicates the child view type to refresh
 * @param lpGLChildViewID     GLVID value, the unique ID of the child view
 * @return                    TRUE
 */
LRESULT ViatronixWnd::OnAppGLChildViewTypeInvalidate(WPARAM wpGLChildViewType, LPARAM lpGLChildViewID)
{
  if (wpGLChildViewType != WPARAM(0))
  {
    m_uInvalidChildViews = (m_uInvalidChildViews & ~wpGLChildViewType) | wpGLChildViewType;
  }
  else if (lpGLChildViewID != LPARAM(0))
  {
    GLChildView::ViewWndTypeEnum eType = GetChildView(GLChildView::GLVID(lpGLChildViewID)).GetViewWndType();
    m_uInvalidChildViews = (m_uInvalidChildViews & ~eType) | eType;
  }

  return GLWnd::OnAppGLChildViewTypeInvalidate(wpGLChildViewType, lpGLChildViewID);
} // OnAppGLChildViewTypeInvalidate()


/**
 * Creates the child views for this window.
 *
 * @param hWndThis   Handle to this window.
 */
void ViatronixWnd::CreateChildViews(HWND hWndThis)
{
  // View window layout template 
  CDialog layoutTemplateDlg;

  uint4 uViewTemplateID(0);
  if (WinUtils::MonitorDisplay::IsWideScreen())
    uViewTemplateID = GetWidescreenLayoutID();
  else if (WinUtils::MonitorDisplay::IsPortrait())
  {
    CRect rctApp = WinUtils::GetClientRect(AfxGetMainWnd());
    TRACE("\n\nmain wnd: %dx%d\n\n", rctApp.Width(), rctApp.Height());

    if ( rctApp.Height() < 1900 )
      uViewTemplateID = GetPortraitLayoutID();
    else
      uViewTemplateID = GetPortraitLargeLayoutID();
  }
  else
    uViewTemplateID = GetLandscapeLayoutID();

  SetDlgCtrlID(uViewTemplateID);

  if ((uViewTemplateID == 0) || (!layoutTemplateDlg.Create(uViewTemplateID)))
  {
    LogWrn("Failed to load layout template window.  Check custom control in template window.", "ViatronViewWnd", "CreateChildViews");
    return;
  }

  std::list<GLChildView *> childViewList;
  // Create child windows
  CreateChildEnumStruct enumParams(childViewList, m_controlPtrList, hWndThis);
  EnumChildWindows(layoutTemplateDlg, CreateChildEnumStruct::CreateProc, LPARAM(&enumParams));

  // Set child views
  SetChildViews(childViewList);

  //{{ Store the template rectangles for each child
  layoutTemplateDlg.GetClientRect(m_layoutClientRect);
  m_childToOptionsMap.clear();

  CWnd * pLayoutItem = NULL;
  for (uint4 i(0); i < NumGLViews(); i++)
  {
    GLChildViewRef rChild = GetChildViewByPos(i);
    if ((pLayoutItem = layoutTemplateDlg.GetDlgItem(rChild.GetControlID())) != NULL)
    {
      CRect rect(WinUtils::GetWindowRect(pLayoutItem));
      layoutTemplateDlg.ScreenToClient(rect);
      m_childToOptionsMap[rChild.GetControlID()].m_rctDisplay = Rect<int4>(rect.left,rect.top,rect.right,rect.bottom);
      m_childToOptionsMap[rChild.GetControlID()].m_bSquare = ((pLayoutItem->GetExStyle() & V3DS::eWINDOWSZ_OPTION_BITS) == V3DS::eFIXED_DIMENSIONS);

    }
  }

  // Initialize controls
  for (CWndPtrList::iterator it = m_controlPtrList.begin(); it != m_controlPtrList.end(); it++)
  {
    CWnd & rControl = *(*it);
    const type_info& rControlType = typeid(rControl);

    // Set view rectangles for the controls
    if (pLayoutItem = layoutTemplateDlg.GetDlgItem(rControl.GetDlgCtrlID()))
    {
      CRect rect(WinUtils::GetWindowRect(pLayoutItem));
      layoutTemplateDlg.ScreenToClient(rect);
      m_childToOptionsMap[rControl.GetDlgCtrlID()].m_rctDisplay = Rect<int4>(rect.left,rect.top,rect.right,rect.bottom);
      m_childToOptionsMap[rControl.GetDlgCtrlID()].m_bSquare = false;
    }

    if (rControlType == typeid(PaneHeader))
    {
      // Link pane header to its intended child pane window
      PaneHeader& rPane = ((PaneHeader&) rControl);
      CreateChildEnumStruct::MapPaneIDToWnd::const_iterator itFind
        = enumParams.m_mapPaneChildID.find(uint4(rPane.m_hPaneChild));
      if (itFind != enumParams.m_mapPaneChildID.end())
      {
        rPane.m_hPaneChild = itFind->second;
        rPane.ShowChild(false);
      }

      if (enumParams.m_pPaneManager)
      {
        enumParams.m_pPaneManager->AddPane(HWND(rControl));
        rPane.SetManager(HWND(*enumParams.m_pPaneManager));
      }
    }
    else if (typeid(rControl) == typeid(SliceScrollBar))
    {
      // Slice scrollbars need to have their control IDs set to its associated SliceChildView
      const uint4 uExStyle(rControl.GetExStyle());
      const uint4 uParentPos(rControl.GetExStyle() >> 4);

      if (uParentPos < NumGLViews())
      {
        GLChildViewRef rParentView = GetChildViewByPos(uParentPos);
        if (typeid(rParentView) == typeid(SliceChildView))
        {
          ((SliceChildView&)rParentView).SetSliceScrollBar(rControl);
          ((SliceScrollBar&)rControl).SetParentView(rParentView.GetClassInstanceID());
          rControl.ModifyStyleEx(uExStyle, 0);
        }
        else if (typeid(rParentView) == typeid(Segment2DChildView))
        {
          ((Segment2DChildView&)rParentView).SetSliceScrollBar(rControl);
          ((SliceScrollBar&)rControl).SetParentView(rParentView.GetClassInstanceID());
          rControl.ModifyStyleEx(uExStyle, 0);
        }
        else if (typeid(rParentView) == typeid(Segment3DChildView))
        {
          ((Segment3DChildView&)rParentView).SetSliceScrollBar(rControl);
          ((SliceScrollBar&)rControl).SetParentView(rParentView.GetClassInstanceID());
          rControl.ModifyStyleEx(uExStyle, 0);
        }
        else if (typeid(rParentView) == typeid(SegmentCylindricalChildView))
        {
          ( ( SegmentCylindricalChildView & ) rParentView ).SetSliceScrollBar( rControl );
          ( ( SliceScrollBar & ) rControl ).SetParentView( rParentView.GetClassInstanceID() );
          rControl.ModifyStyleEx( uExStyle, 0 );
        }
      }
    }
  }
  //}} (end) Store the template rectangles for each child

  // Destroy the layout window now that we're done using it
  layoutTemplateDlg.DestroyWindow();
} // CreateChildViews()


/**
 * Scales <scaledRect> from <parentRect> down to <windowRect> coordinates.
 *
 * @param windowRect   Destination coordinates.
 * @param parentRect   Source coordinates.
 * @param scaledRect   Rectangle to scale.
 */
void ViatronixWnd::ScaleMyRect(const CRect & windowRect, const CRect & parentRect, Rect<int4> & scaledRect)
{
  scaledRect = Rect<int4>(
    windowRect.left + ((((float4)windowRect.Width()  * scaledRect.m_left   / float4(parentRect.Width()  )))+0.5F),
    windowRect.top  + ((((float4)windowRect.Height() * scaledRect.m_top    / float4(parentRect.Height() )))+0.5F),
    windowRect.left + ((((float4)windowRect.Width()  * scaledRect.m_right  / float4(parentRect.Width()  )))+0.5F),
    windowRect.top  + ((((float4)windowRect.Height() * scaledRect.m_bottom / float4(parentRect.Height() )))+0.5F)
  );

  float4 fRatio = float4(scaledRect.Width()) / scaledRect.Height();
  if ((fRatio > 0.9) && (fRatio < 1.1))
  {
    if (scaledRect.Width() > scaledRect.Height())
    {
      scaledRect.Deflate( ((scaledRect.Width()-scaledRect.Height())/2.0F)+0.5f, 0 );
    }
    else
    {
      scaledRect.Deflate(0, ((scaledRect.Height()-scaledRect.Width())/2.0F)+0.5f);
    }
  }  
} // ScaleMyRect()


/**
 * Calculate children view rectangles.
 *
 * @param iWidth    Width of this window.
 * @param iHeight   Height of this window.
 */
void ViatronixWnd::CalculateViewRects(const int4 iWidth, const int4 iHeight)
{
  if ((iWidth <= 0) || (iHeight <= 0) || (GetLandscapeLayoutID() == 0))
  {
    return;
  }

  CRect thisClientRect(0,0,iWidth,iHeight);
  int4 shiftX(0), shiftY(0);

  float4 fTemplateRatio = float4(m_layoutClientRect.Width()) / m_layoutClientRect.Height();
  float4 fParentRatio = float4(thisClientRect.Width()) / thisClientRect.Height();
  if (fTemplateRatio > fParentRatio)
  {
    float4 fTempHeight = thisClientRect.Width() / fTemplateRatio;
    float4 fExcess = thisClientRect.Height() - fTempHeight;

    shiftY = fExcess * 0.5F;
    thisClientRect.top    -= shiftY;
    thisClientRect.bottom += shiftY;

    // SetWindowPos(NULL, thisClientRect.left, thisClientRect.top, thisClientRect.Width(), thisClientRect.Height(), SWP_NOMOVE | SWP_NOZORDER);
  }

  // Set the child view rectangles
  for (uint4 i(0); i < NumGLViews(); i++)
  {
    DlgCtlID_To_ChildOptionsMap::const_iterator it = m_childToOptionsMap.find(GetChildViewByPos(i).GetControlID());
    if (it == m_childToOptionsMap.end())
    {
      continue;
    }

    Rect<int4> viewRect = it->second.m_rctDisplay;
    ScaleMyRect(thisClientRect, m_layoutClientRect, viewRect);

    if ( it->second.m_bSquare )
    {
      int4 iDimDiff( viewRect.Width() - viewRect.Height() );
      if ( iDimDiff < 0 )
      {
        viewRect.Inflate(0, iDimDiff/2);
      }
      else if ( iDimDiff > 0 )
      {
        viewRect.Deflate(iDimDiff/2, 0);
      }
    }

    SetViewRect(i, viewRect);
  }

  // Set view rectangles for the controls
  for (CWndPtrList::iterator it = m_controlPtrList.begin(); it != m_controlPtrList.end(); it++)
  {
    CWnd & rControl = *(*it);
    if ( typeid(rControl) == typeid(SliceScrollBar) ) // let slice children place their own scrollbars; doing it here proved buggy
      continue;

    DlgCtlID_To_ChildOptionsMap::const_iterator it2 = m_childToOptionsMap.find(rControl.GetDlgCtrlID());
    if (it2 == m_childToOptionsMap.end())
    {
      continue;
    }

    Rect<int4> viewRect = it2->second.m_rctDisplay;
    ScaleMyRect(thisClientRect, m_layoutClientRect, viewRect);
    rControl.MoveWindow(viewRect.m_left, viewRect.m_top, viewRect.Width()-2, viewRect.Height()-2);
    rControl.PostMessage(WM_SIZE, WPARAM(SIZE_RESTORED), MAKELPARAM(viewRect.Width()-2,viewRect.Height()-2));
  }
} // CalculateViewRects()


/**
 * gets the layout rectangle
 * @param uChildViewControlID   id of control to get rectangle for
 * @return                      bounding rect of control
 */
Rect<int4> ViatronixWnd::GetLayoutRect(const uint4 uChildViewControlID) const
{
  DlgCtlID_To_ChildOptionsMap::const_iterator it = m_childToOptionsMap.find(uChildViewControlID);
  if (it == m_childToOptionsMap.end())
  {
    return Rect<int4>();
  }

  CRect clientRect = WinUtils::GetClientRect(this);
  Rect<int4> viewRect = it->second.m_rctDisplay;
  ScaleMyRect(clientRect, m_layoutClientRect, viewRect);

  return viewRect;
} // GetLayoutRect()


/**
 * WM_MOUSEMOVE handler
 *
 * @param uFlags   Current key flags
 * @param point    Current point of mouse move
 */
void ViatronixWnd::OnMouseMove(UINT uFlags, CPoint point)
{
  GLChildViewRef prevView = GetCurrentView();

  GLWnd::OnMouseMove(uFlags, point);

  GLChildViewRef currView = GetCurrentView();
  if (prevView.GetClassInstanceID() != currView.GetClassInstanceID())
  {
    // Current view, as a result of ViatronixWnd::OnMouseMove() call, has changed
    // (i.e. mouse cursor moved between view windows)
    SetChildViewMisc(prevView, false);
    SetChildViewMisc(currView, true);
    InvalidateViews( WPARAM(GLChildView::eGVT_NAVIGATION), 0);
  }
  else
  {
    SetChildViewMisc(prevView, true);
  }
  
  if( m_bIsLeafClass )
  {
    const uint4 uViewModes( ViewModes::AUTOFLY | ViewModes::AUTOFLY2D );
    if( !rdrGlobal.m_supine.m_submarine.IsFlying() && !rdrGlobal.m_prone.m_submarine.IsFlying() && !(ViatronStudy::GetCurrentViewModes() & uViewModes ))
      GLIdle();

    GLWnd * pParentWnd = dynamic_cast< GLWnd * >( GetParent() );
    if( pParentWnd != NULL )
    {
      pParentWnd->GLIdle();
      pParentWnd->GLRenderViews();
    }
  }
} // OnMouseMove()


/**
 * WM_KEYDOWN handler
 *
 * @param uChar     Key code identifier.
 * @param uRepCnt   Key repeat count.
 * @param uFlags    Specifies the scan code, key-transition code, previous key state, and context code.
 */
void ViatronixWnd::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags) 
{
  // Notify current child view for modifier keys
  switch (uChar)
  {
    case VK_F1: case VK_F2: case VK_F3: case VK_F4:  case VK_F5:  case VK_F6:
    case VK_F7: case VK_F8: case VK_F9: case VK_F10: case VK_F11:  case VK_F12:
    case VK_CONTROL:
    case VK_SHIFT:
      SetChildViewMisc(GetCurrentView(), true);
      break;
    default:
      break;
  }

  GLWnd::OnKeyDown(uChar, uRepCnt, uFlags);
} // OnKeyDown()


/**
 * Set miscellaneous information/displays for child views:  cross-sectional shadows,
 *  mouse key strings, CTRL/SHIFT indicators
 *
 * @param view     Reference to the view
 * @param bEnter   True if you wish to set values as if entering view window, false otherwise
 */
void ViatronixWnd::SetChildViewMisc(const GLChildView &view, bool bEnter)
{
  bool bCtrl(false), bShift(false);

  switch (view.GetViewWndType())
  {
    case GLChildView::eGVT_CROSS_SECTION:
      if (bEnter)
      {
        rdrGlobal.m_bObliqueShadow = true;
      }
      else
      {
        rdrGlobal.m_bObliqueShadow = (ViatronStudy::m_modeEndoView & ViewModes::CROSS_SECTION);
      }
      rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();

      bCtrl = bShift = true;
      break;

    case GLChildView::eGVT_AXIAL:
      if (bEnter)
      {
        rdrGlobal.m_bAxialShadow = true;
      }
      else
      {
        rdrGlobal.m_bAxialShadow = (ViatronStudy::m_modeEndoView & ViewModes::AXIAL);
      }
      rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();

      bCtrl = bShift = true;
      break;

    case GLChildView::eGVT_SAGITTAL:
      if (bEnter)
      {
        rdrGlobal.m_bSagittalShadow = true;
      }
      else
      {
        rdrGlobal.m_bSagittalShadow = (ViatronStudy::m_modeEndoView & ViewModes::SAGITTAL);
      }
      rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();

      bCtrl = bShift = true;
      break;

    case GLChildView::eGVT_CORONAL:
      if (bEnter)
      {
        rdrGlobal.m_bCoronalShadow = true;
      }
      else
      {
        rdrGlobal.m_bCoronalShadow = (ViatronStudy::m_modeEndoView & ViewModes::CORONAL);
      }
      rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();

      bCtrl = bShift = true;
      break;

    case GLChildView::eGVT_3DSEGMENTS:
      if (bEnter)
      {
        rdrGlobal.m_bSegmentHalfPlaneShadow = true;
      }
      else
      {
        rdrGlobal.m_bSegmentHalfPlaneShadow = false;
      }
      rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
      rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();

      bCtrl = bShift = false;
      break;

    case GLChildView::eGVT_REPORTENTRY:
      bCtrl = bShift = false;
      break;

    case GLChildView::eGVT_OVERVIEW:
      bCtrl = false; bShift = false;
      break;
    case GLChildView::eGVT_NAVIGATION:
      bCtrl = true; bShift = false;
#ifndef FINAL_RELEASE
      bShift = true;
#endif
      break;

    default:
      bEnter = false;     bCtrl = bShift = false;
      break;
  }

  // IDs for the string resources for the left, middle, right button text
  CString strs[3];
  unsigned int strIds[3] = {0,0,0};

  if (bEnter)
  {
    view.GetMouseKeyStringIDs(strIds[0], strIds[1], strIds[2]);
    for (int i=0; i<3; strs[i].LoadString(strIds[i]), i++);
  }
  MainAppFrame::GetMainFrame()->m_viewSelBar.SetButtonFunctionText(
    LPCTSTR(strs[0]),LPCTSTR(strs[1]),LPCTSTR(strs[2]), bCtrl&&bEnter,bShift&&bEnter);
} // SetChildViewMisc()


/**
 * Idle time processing
 */
void ViatronixWnd::GLIdle()
{
  for( std::vector<GLChildViewPtr>::iterator it = m_vpViews.begin(); it != m_vpViews.end(); it++ )
  {
    GLChildViewRef child = **it;
    if( child.GLIdle() )
    {
      InvalidateViews( 0, LPARAM( child.GetClassInstanceID() ) );
    }
  }

  if( m_uInvalidChildViews != GLChildView::eGVT_NONE )
  {
    GLRenderViews();
  }
} // GLIdle()



// $Log: ViatronixWnd.C,v $
// Revision 1.21.2.27  2010/04/17 08:12:43  dongqing
// small portrait display layouts
//
// Revision 1.21.2.26  2010/04/17 05:15:59  dongqing
// small portrait display layouts
//
// Revision 1.21.2.25  2010/04/17 04:52:53  dongqing
// small portrait display layouts
//
// Revision 1.21.2.24  2010/04/16 18:01:23  dongqing
// specify pane backgrounds in the resource file
//
// Revision 1.21.2.23  2010/04/09 21:59:33  dongqing
// color progress control creation error
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.22  2010/04/09 02:34:29  dongqing
// portrait changes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.21  2010/04/07 00:21:29  dongqing
// created a user control for progress ctrl, seeing as how -- strangely -- the basic control ignores the 'vertical' setting
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.20  2010/03/24 07:22:25  dongqing
// use selected monitor for calculations
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.19  2010/03/03 09:00:56  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.18  2010/02/01 19:21:55  dongqing
// reload small/larger layouts if necessary while user is resizing
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.17  2010/01/31 02:13:55  dongqing
// support for vertical lumen coverage progress bar; toggle missed regions/cad dialogs
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.16  2010/01/18 13:25:31  dongqing
// 1280x1024 provisions
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.15  2010/01/18 11:02:22  dongqing
// 1280x1024 provisions
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.14  2010/01/17 18:24:13  dongqing
// 1680x1050 and 1920x1200 widescreen resolutions
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.13  2010/01/14 14:48:35  dongqing
// displaying info of both datasets on dual-dataset layouts
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.12  2010/01/07 21:50:37  dongqing
// proper layout of 3D segments view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.11  2010/01/07 18:46:47  dongqing
// translucency control for widescreen
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.10  2009/12/09 19:39:45  dongqing
// layouts will specify various IDs for different orientations, and the baseclass will decide which orientation is needed.
// somewhat similar to the IDD= enum using in mfc dialogs
//
// Revision 1.21.2.9  2009/12/04 18:23:50  dongqing
// specifying fixed-square-size views in resource
//
// Revision 1.21.2.8  2009/11/25 22:07:32  dongqing
// i don't even know
//
// Revision 1.21.2.7  2009/11/24 23:01:27  dongqing
// localized all missed regions list functionality to a single control class.
//
// Revision 1.21.2.6  2009/11/17 19:22:27  dongqing
// fixed long-time issue of scrollbars not showing up during automated window resizing!
//
// Revision 1.21.2.5  2009/11/17 19:21:31  dongqing
// fixed long-time issue of scrollbars not showing up during automated window resizing!
//
// Revision 1.21.2.4  2009/11/16 22:08:12  dongqing
// widescreen detection
//
// Revision 1.21.2.3  2009/11/12 23:13:07  dongqing
// Issue Cad09-03: second cad pane to registration-cad layout
//
// Revision 1.21.2.2  2009/10/07 16:13:17  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.21.2.1  2008/01/16 01:22:14  jmeade
// Issue 5917: support for vertical scrollbars throughout application.
//
// Revision 1.21  2007/03/07 23:07:03  jmeade
// code standards.
//
// Revision 1.20  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.19  2007/02/02 21:29:35  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.18  2007/01/03 06:22:27  jmeade
// future ud segments layout.
//
// Revision 1.17  2006/09/19 19:16:46  frank
// fixed cylindrical view scrollbar
//
// Revision 1.16  2006/07/11 22:58:29  jmeade
// Issue 4865: Don't need large-size versions of patient info and controls dialog on S/P view.
//
// Revision 1.15  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.14  2005/11/23 18:37:09  jmeade
// resizing controls for 16x12 resolution.
//
// Revision 1.13  2005/11/23 17:06:59  jmeade
// resizing horizontal controls pane for 16x12.
//
// Revision 1.12  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.11.2.4  2006/05/22 12:39:47  geconomos
// making call to DestoryWindow() on all child controls during cleanup
//
// Revision 1.11.2.3  2006/05/16 12:38:28  geconomos
// add WM_DESTROY handler to destroy non "view" based  children
//
// Revision 1.11.2.2  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.11.2.1  2005/11/09 02:15:00  jmeade
// cad pane on supine/prone view.
//
// Revision 1.11  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.10  2005/10/05 15:25:31  geconomos
// comment out call to setwindowpos in CalculateViewRects
//
// Revision 1.9  2005/09/14 14:41:53  jmeade
// cad results dialog, initial checkin.
//
// Revision 1.8  2005/09/06 17:11:17  geconomos
// Issue #4412: Movement in 2d causing hiccups during endoluminal flight
//
// Revision 1.7  2005/08/25 13:37:43  geconomos
// Issue# 4387: Enable Supine/Prone toggle for orthogonal view
//
// Revision 1.6  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.5  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.4  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.3  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.8.4.10.2.1  2005/07/18 18:28:34  geconomos
// Issue #4284: moved aspect ratio adjustment of child views to ScaleRect method
//
// Revision 1.1.2.8.4.10  2005/06/10 18:20:49  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.1.2.8.4.9  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.2.8.4.8  2005/06/03 15:05:26  jmeade
// abbreviated controls pane.
//
// Revision 1.1.2.8.4.7.2.2  2005/06/08 14:01:11  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 1.1.2.8.4.7.2.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 1.1.2.8.4.7  2005/04/11 15:40:24  frank
// removed debug message
//
// Revision 1.1.2.8.4.6  2005/04/08 13:34:10  frank
// Issue #4049: Added mouse text for overview images in sliding panes
//
// Revision 1.1.2.8.4.5  2005/04/04 19:28:26  jmeade
// force square images only for near-square images.
//
// Revision 1.1.2.8.4.4  2005/04/04 16:48:35  jmeade
// forcing square images.
//
// Revision 1.1.2.8.4.3  2005/03/31 23:06:09  jmeade
// Issue 4093: Idle routine call in a PAINT EVENT! :O yuck.
//
// Revision 1.1.2.8.4.2  2005/03/31 02:08:54  jmeade
// Issue 4038: eliminating some multiple redraws due to dependence on GLChildViewType.
//
// Revision 1.1.2.8.4.1  2005/03/26 19:46:05  frank
// revamped main application idle/processing loop - checkin for Jeff
//
// Revision 1.1.2.8  2004/08/19 18:21:09  jmeade
// Allow child windows to handle their own "Invalidate" calls.
//
// Revision 1.1.2.7  2004/05/05 23:20:44  jmeade
// Create adjust-translucency-dlg from resource.
//
// Revision 1.1.2.6  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.1.2.5  2003/07/18 17:43:42  jmeade
// Comments.
//
// Revision 1.1.2.4  2003/04/25 20:48:48  jmeade
// Issue ???:  Must call GLRenderViews() to render other views as necessary when one or more views are invalidated.
//
// Revision 1.1.2.3  2003/04/15 21:56:54  jmeade
// Issue 3001: Method to return the original scaled view rect from the layout resource.
//
// Revision 1.1.2.2  2003/02/27 03:34:50  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.1.2.1  2003/02/22 12:21:10  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronixWnd.C,v 1.21.2.27 2010/04/17 08:12:43 dongqing Exp $
// $Id: ViatronixWnd.C,v 1.21.2.27 2010/04/17 08:12:43 dongqing Exp $
