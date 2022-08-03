// $Id: ControlsDialog.C,v 1.32.2.19 2010/10/20 20:47:33 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  @jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "Viatron.h"
#include "MainFrm.h"
#include "ReportViewWnd.h"
#include "NavigationView.h"
#include "ButtonMenus.h"
#include "User.h"
#include "FunctionProgress.h"
#include "ControlsDialog.h"
#include "ApplicationSettings.h"
#include "vcUtils.h"


// namespaces
using namespace ViewModes;
using namespace ColonViews::Dialogs;
using namespace ColonViews::Layouts;
using namespace WinUtils;
using namespace ReaderLib;

// defines
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define FILE_REVISION "$Revision: 1.32.2.19 $"

/////////////////////////////////////////////////////////////////////////////
// SegmentJumpCtrl methods


/**
 * Creates the 3 buttons for the segment jump/direction control
 * 
 * @param LPCTSTR sCaption      Caption (unused for the bitmap buttons)
 * @param DWORD dwStyle         Button style
 * @param const RECT & rect     Enclosing rectangle for all three buttons
 * @param CWnd * pParentWnd     Parent window
 * @param UINT uID              Base ID for buttons (range from uID+1 to uID+3)
 * @return BOOL                 Success of creation
 */
BOOL ControlsDialog::SegmentJumpCtrl::Create(LPCTSTR sCaption, DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT uID)
{
  dwStyle = dwStyle|WS_VISIBLE|WS_CHILD | BS_OWNERDRAW | BS_BITMAP;

  const int4 iThirdOfWidth(((rect.right - rect.left) * 0.333)-1);
  CRect btnRect = rect;

  btnRect.right = btnRect.left + iThirdOfWidth;
  m_prevBtn.Create(CString(), dwStyle, btnRect, pParentWnd, uID+1);

  btnRect.OffsetRect(iThirdOfWidth + 1, 0);
  m_dirBtn.Create(CString(), dwStyle, btnRect, pParentWnd, uID+2);

  btnRect.OffsetRect(iThirdOfWidth, 0);
  m_nextBtn.Create(CString(), dwStyle, btnRect, pParentWnd, uID+3);

  UpdateFlightDirn(Submarine::RECTUM_TO_CECUM);
  return TRUE;
} // Create()


/**
 * Updates button icons based on current flight direction
 * 
 * @param const Submarine::NavigationDirection eNavDirn   Flight direction
 */
void ControlsDialog::SegmentJumpCtrl::UpdateFlightDirn(const Submarine::NavigationDirection eNavDirn)
{
  static const uint4 uNumBtns(3);
  const bool bToCecum(eNavDirn == Submarine::RECTUM_TO_CECUM);

  const UINT uBaseID(m_prevBtn.GetDlgCtrlID() - 1);

  const UINT vuIconIds[uNumBtns][2] =
  {
    { uBaseID + 1, (bToCecum ? IDB_SEGJUMP_CECUM_NEXT : IDB_SEGJUMP_RECTUM_PREV) }  ,
    { uBaseID + 2, (bToCecum ? IDB_TO_CECUM_ARROW : IDB_TO_RECTUM_ARROW) } ,
    { uBaseID + 3, (bToCecum ? IDB_SEGJUMP_CECUM_PREV : IDB_SEGJUMP_RECTUM_NEXT) }  ,
  };
  SetButtonIcons(m_prevBtn.GetParent(), vuIconIds, uNumBtns);
} // UpdateFlightDirn()

/**
 * Reposition the window
 *
 * @param iX  the x position
 * @param iY  the y position
 */
void ControlsDialog::SegmentJumpCtrl::RepositionWindow( const int4 iX, const int4 iY )
{
  // get the current locations
  CRect prevButtonRect = WinUtils::GetWindowRect( m_prevBtn );
  CRect dirButtonRect = WinUtils::GetWindowRect( m_dirBtn );
  CRect nextButtonRect = WinUtils::GetWindowRect( m_nextBtn );

  // move to the new location
  const int4 iX2 = iX + prevButtonRect.Width();
  const int4 iX3 = iX2 + dirButtonRect.Width();
  m_prevBtn.SetWindowPos( NULL, iX, iY, prevButtonRect.Width(), prevButtonRect.Height(), SWP_NOZORDER );
  m_dirBtn.SetWindowPos( NULL, iX2, iY, dirButtonRect.Width(), dirButtonRect.Height(), SWP_NOZORDER );
  m_nextBtn.SetWindowPos( NULL, iX3, iY, nextButtonRect.Width(), nextButtonRect.Height(), SWP_NOZORDER );

} // RepositionWindow( x, y )


/////////////////////////////////////////////////////////////////////////////
// ControlsDialog dialog


/**
 * Constructor
 * 
 * @param CWnd* pParent   Parent window
 */
ControlsDialog::ControlsDialog(CWnd* pParent /* = NULL */, const uint4 uDlgBkgdResourceID /*= IDB_ENDOVIEW_HORIZ_DLGBAR*/)
: ViatronDialog(IDD, pParent, uDlgBkgdResourceID),
m_currentVtxViewModes(0),
m_eCurrNavDirection(Submarine::NavigationDirection(-1)),
m_eCurrOrient(DatasetOrientations::TypeEnum(-1)),
m_bWasFlying(false),
m_bHorizontal( uDlgBkgdResourceID == IDB_ENDOVIEW_HORIZ_DLGBAR ),
m_iCurrentToolID(-1)
{
  //{{AFX_DATA_INIT(ControlsDialog)
  //}}AFX_DATA_INIT
} // constructor


/**
 * Initialize child control variables
 * 
 * @param CDataExchange* pDX   
 */
void ControlsDialog::DoDataExchange(CDataExchange* pDX)
{
  ViatronDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ControlsDialog)
  DDX_Control(pDX, IDC_CDB_MOVIE, m_movieBtn);
  DDX_Control(pDX, IDC_CDB_SNAPSHOT, m_snapshotBtn);
  DDX_Control(pDX, IDC_CDB_AUTOFLY, m_autoFlyBtn);
  DDX_Control(pDX, IDC_CDB_SEGMENT_JUMP_CONTROL, m_segmentJumpBtn);
  DDX_Control(pDX, IDC_CDB_REGIONSEGMENT_BTN, m_btnRegionSegment);
  DDX_Control(pDX, IDC_CDB_FLIPVIEW_BTN, m_flipViewBtn);
  DDX_Control(pDX, IDC_CDB_ORTHOGONAL_SHADOWS, m_shadowsBtn);
  DDX_Control(pDX, IDC_CDB_TOOLS, m_toolsBtn);
  DDX_Control(pDX, IDC_CDB_PAINTING, m_showPaintingBtn);
  DDX_Control(pDX, IDC_CTRLS_SLIDER_VALUE_DISP, m_sliderValStatic);
  DDX_Control(pDX, IDC_CTRLS_SPEED_SLIDER, m_speedSlider);
  DDX_Control(pDX, IDC_CTRLS_PERCEIVED_SPEED_CHECK, m_checkPerceivedSpeed);
  DDX_Control(pDX, IDC_PID_SUPINEPRONE_BTN, m_supineProneBtn);
  DDX_Control(pDX, IDC_CDB_TOOL_LINEMEASURE, m_btnLineMeasure);
  DDX_Control(pDX, IDC_CDB_TOOL_LIGHTING, m_btnLighting);
  DDX_Control(pDX, IDC_CDB_TOOL_TRANSLUCENCY, m_btnTranslucency);
  DDX_Control(pDX, IDC_CDB_TOOL_2DROI, m_btn2DROI);
  DDX_Control(pDX, IDC_CDB_TOOL_ANNOTATION, m_btnAnnotation);
  SkinAutoRadioButton::DDX_Radio_IDs(pDX, IDC_CDB_TOOL_LINEMEASURE, m_iCurrentToolID);
  //}}AFX_DATA_MAP
} // DoDataExchange()

BEGIN_MESSAGE_MAP(ControlsDialog, ViatronDialog)
  //{{AFX_MSG_MAP(ControlsDialog)
  ON_WM_SIZE()
  ON_WM_HSCROLL()
  ON_BN_CLICKED(IDC_CDB_SEGMENT_JUMP_DIRECTION, OnViewDirectionBtnClicked)
  ON_WM_SETCURSOR()
  ON_WM_KEYDOWN()
  ON_WM_MOUSEMOVE()
  ON_BN_CLICKED(IDC_CDB_FLIPVIEW_BTN, OnFlipViewBtnClicked)
  ON_BN_CLICKED(IDC_CDB_PAINTING, OnShowPaintingClicked)
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_CDB_SEGMENT_JUMP_PREV, OnPrevSegmentBtnClicked)
  ON_BN_CLICKED(IDC_CDB_SEGMENT_JUMP_NEXT, OnNextSegmentBtnClicked)
  ON_BN_CLICKED(IDC_CDB_AUTOFLY, OnAutoflyClicked)
  ON_BN_CLICKED(IDC_CDB_SNAPSHOT, OnSnapshotClicked)
  ON_BN_CLICKED(IDC_CDB_MOVIE, OnMovieToggled)
  ON_BN_CLICKED(IDC_CTRLS_PERCEIVED_SPEED_CHECK, OnClickedPerceivedSpeedCheck)
  ON_BN_CLICKED(IDC_PID_SUPINEPRONE_BTN, OnClickedDatasetBtn)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ControlsDialog message handlers


/**
 * Init dialog procedure
 * 
 * @return BOOL  Create success
 */
BOOL ControlsDialog::OnInitDialog()
{
  ViatronDialog::OnInitDialog();

  // {{ Initialize speed slider
  const CSize speedRange(1, EndoluminalView::m_uMaxFlightSpeed);
  const int4 iSpeedRange( max(speedRange.cy - speedRange.cx + 1, 1) );

  float4 fSpeed(min(EndoluminalView::GetUserPreferredFlightSpeed() / float4(iSpeedRange), 1.0F));
  if (fSpeed <= 0.0F)
  {
    fSpeed = rdrGlobal.m_pCurrDataset->m_submarine.GetThrottle();
  }

  m_speedSlider.SetRange(speedRange.cx, speedRange.cy);
  m_speedSlider.SetPos(fSpeed * iSpeedRange);
  // }} Initialize speed slider

  // {{ Set button icons
  const uint4 vuBtnsNicons[][2] =
  {
    // Control pad buttons
    { IDC_CDB_AUTOFLY, IDB_AUTOFLY },    { IDC_CDB_FLIPVIEW_BTN, IDB_FLIPVIEW },
    { IDC_CDB_PAINTING, IDB_ENDOVIEW_MARKING },
    { IDC_CDB_ORTHOGONAL_SHADOWS, IDB_SLICEOPTIONS },
    { IDC_CDB_SNAPSHOT, IDB_CAMERA }, { IDC_CDB_MOVIE, IDB_MOVIE_BUTTON },
    { IDC_CDB_TOOL_LINEMEASURE, IDB_MEASURE },
    { IDC_CDB_TOOL_LIGHTING, IDB_HIGHLIGHTS },
    { IDC_CDB_TOOL_TRANSLUCENCY, IDB_TRANSPARENT_VIEW },
    { IDC_CDB_TOOL_2DROI, IDB_ROIMEASURE },
    { IDC_CDB_TOOL_ANNOTATION, IDB_ANNOTATIONS_BUTTON },
    { IDC_CDB_REGIONSEGMENT_BTN, IDB_SEGMENT_POLYP },
  };
  SetButtonIcons(this, vuBtnsNicons, sizeof(vuBtnsNicons)/(sizeof(uint4[2])));
  // }} Set button icons

  // Enable tool tips
  const BOOL bTipsEnabled(EnableToolTips(TRUE));

  // {{ Create segment jump control
  CRect segRect = WinUtils::GetWindowRect(m_segmentJumpBtn); ScreenToClient(segRect);
  m_segJumpCtrl.Create(CString(), WS_VISIBLE|WS_CHILD, segRect, this, IDC_CDB_SEGMENT_JUMP_CONTROL);
  // }} Create segment jump control

  // Orthogonal shadows button
  m_shadowsBtn.AddMenuItems(IDM_ENDOVIEW_INDICATORS, 0, MenuButton::MBS_CHECK);

  // {{ Tools button
  m_toolsBtn.AddMenuItems(IDM_TOOLS_PLUS, 0, MenuButton::MBS_RADIO);
  const UINT modeBmpIds[EndoscopicMenus::iCurrModePlusIds] =
  { IDB_MEASURE, IDB_TRANSPARENT_VIEW, IDB_HIGHLIGHTS, IDB_MEASURE, IDB_ANNOTATIONS_BUTTON, IDB_SEGMENT_POLYP };
  m_toolsBtn.AddItemIcons(modeBmpIds, EndoscopicMenus::iCurrModePlusIds);
  m_toolsBtn.SetDefaultIcon(IDB_TOOLS);
  // }} Tools button

  // {{ Endoscopic modes
  UpdateEndoModes();
  m_currentVtxViewModes = ViatronStudy::GetCurrentViewModes();
  SetTimer(1000, 1000, NULL);
  // }} Endoscopic modes

  UpdateState();

  m_supineProneBtn.EnableWindow(rdrGlobal.IsDualDataLoaded()
    && (GetParent()->SendMessage(WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE,0,0) != 0) ? TRUE : FALSE);

  return TRUE;
} // OnInitDialog()


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void ControlsDialog::OnSize( UINT uType, int iCX, int iCY )
{
  ViatronDialog::OnSize(uType, iCX, iCY);

  // Ensure it is safe to access dialog controls, as the OnSize() method is called
  //  once prior to the handling of WM_INITDIALOG, which causes bad things to happen
  if ( iCX == 0 || iCY == 0 || GetDlgItem( IDC_CTRLS_SPEED_SLIDER ) == NULL )
  {
    return;
  }

  CWnd *pWndSpeedSlider;
  CWnd *pWndContainer;
  if ( (pWndContainer = GetDlgItem(IDC_CONTAINER_GROUPBOX)) != NULL )
  {
    CenterChildControls(IDC_CONTAINER_GROUPBOX);
  }
  else if ( (pWndSpeedSlider = GetDlgItem( IDC_CTRLS_SPEED_SLIDER ) ) != NULL )
  {
    CRect rectS = WinUtils::GetChildRect(pWndSpeedSlider, this);
    rectS.right = iCX - 20;
    pWndSpeedSlider->SetWindowPos( NULL, 0,0, rectS.Width(), rectS.Height(), SWP_NOMOVE | SWP_NOZORDER );
  }


  // obsolete functionality follows
  return;

  //
  // custom resizing code for horizontal layout
  //
  if ( ! m_bHorizontal ) return;

  // declare some known sizes
  const int4 iListWidth = 285;
  const int4 iListHeight = 145;
  const int4 iButtonSize = 62;
  const int4 iStaticWidth = 2;
  const int4 iSegmentControlWidth = 230;

  // layout the list view
  const int4 iGapH = max( 0, ( iCX - iListWidth - iSegmentControlWidth - 2 * iStaticWidth - 6 * iButtonSize ) / 240 );
  const int4 iListY = max( 0, ( iCY - iListHeight ) / 2 );

  // layout the list view buttons
  const int4 iButtonGap = max( 0, ( iCY - 2 * iButtonSize ) / 3 );
  const int4 iListButtonsX = iGapH + iListWidth + iGapH;
  const int4 iBottomRowY = iButtonGap + iButtonSize + iButtonGap;
  GetDlgItem( IDC_CDB_DETECTMISSEDRGNS_BTN )->SetWindowPos( NULL, iListButtonsX, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  GetDlgItem( IDC_CDB_NEXTMISSEDRGN_BTN )->SetWindowPos( NULL, iListButtonsX, iBottomRowY, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );

  // layout the first divider
  const int4 iStatic1X = iListButtonsX + iButtonSize + iGapH;
  const int4 iStatic2X = iStatic1X + 2 * iGapH + iStaticWidth + iSegmentControlWidth;
  const int4 iStaticGapV = 4;
  const int4 iStaticHeight = iCY - 2 * iStaticGapV;
  GetDlgItem( IDC_DIVIDER_1 )->SetWindowPos( NULL, iStatic1X, iStaticGapV, iStaticWidth, iStaticHeight, SWP_NOZORDER );

  // layout the middle buttons
  const int4 iMiddleX = iStatic1X + iStaticWidth + iGapH;
  const int4 iMiddleWidth = iStatic2X - ( iStatic1X + iStaticWidth );
  const int4 iMiddleGap = max( 0, ( iMiddleWidth - 2 * iButtonSize ) / 3 );
  const int4 iMiddleButtonX1 = iStatic1X + iStaticWidth + iMiddleGap;
  const int4 iMiddleButtonX2 = iMiddleButtonX1 + iButtonSize + iMiddleGap;
  GetDlgItem( IDC_CDB_AUTOFLY )->SetWindowPos( NULL, iMiddleButtonX1, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  GetDlgItem( IDC_CDB_FLIPVIEW_BTN )->SetWindowPos( NULL, iMiddleButtonX2, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  m_segJumpCtrl.RepositionWindow( iMiddleX + 4, iBottomRowY );

  // layout the second divider
  GetDlgItem( IDC_DIVIDER_2 )->SetWindowPos( NULL, iStatic2X, iStaticGapV, iStaticWidth, iStaticHeight, SWP_NOZORDER );

  // layout the mode buttons
  const int4 iButtonX1 = iStatic2X + iStaticWidth + iGapH;
  const int4 iButtonX2 = iButtonX1 + iButtonSize + iGapH;
  const int4 iButtonX3 = iButtonX2 + iButtonSize + iGapH;
  const int4 iButtonX4 = iButtonX3 + iButtonSize + iGapH;
  const int4 iButtonX5 = iButtonX4 + iButtonSize + iGapH;
  const int4 iButtonX6 = iButtonX5 + iButtonSize + iGapH;
  GetDlgItem( IDC_CDB_TOOLS )->SetWindowPos( NULL, iButtonX1, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  GetDlgItem( IDC_CDB_SNAPSHOT )->SetWindowPos( NULL, iButtonX2, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  GetDlgItem( IDC_CDB_MOVIE )->SetWindowPos( NULL, iButtonX3, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  GetDlgItem( IDC_CDB_ORTHOGONAL_SHADOWS )->SetWindowPos( NULL, iButtonX4, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  GetDlgItem( IDC_CDB_PAINTING )->SetWindowPos( NULL, iButtonX5, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  GetDlgItem( IDC_CDB_REGIONSEGMENT_BTN )->SetWindowPos( NULL, iButtonX6, iButtonGap, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );

  // layout the speed controls
  const int4 iSpeedInset = 20;
  const int4 iSpeedRightX = iButtonX5 + iButtonSize;
  const int4 iCheckboxSize = 29;
  const int4 iSpeedTextWidth = 57;
  const int4 iSpeedTextHeight = iCheckboxSize;
  const int4 iSpeedGap = 5;
  const int4 iSliderHeight = 34;
  const int4 iSliderWidth = iSpeedRightX - iButtonX1 - 2 * iSpeedInset - iCheckboxSize -iSpeedTextWidth - 2 * iSpeedGap;
  const int4 iCheckboxX = iButtonX1 + iSpeedInset;
  const int4 iSpeedTextX = iCheckboxX + iCheckboxSize + iSpeedGap;
  const int4 iSliderX = iSpeedTextX + iSpeedTextWidth + iSpeedGap;
  const int4 iSpeedYCenter = iBottomRowY + iButtonSize / 2;
  const int4 iCheckboxY = iSpeedYCenter - iCheckboxSize / 2 - 4;
  const int4 iSpeedTextY = iSpeedYCenter - iSpeedTextHeight / 2;
  const int4 iSliderY = iSpeedYCenter - iSliderHeight / 2;
  GetDlgItem( IDC_CTRLS_PERCEIVED_SPEED_CHECK )->SetWindowPos( NULL, iCheckboxX, iCheckboxY, iCheckboxSize, iCheckboxSize, SWP_NOZORDER );
  GetDlgItem( IDC_SPEED_TEXT )->SetWindowPos( NULL, iSpeedTextX, iSpeedTextY, iSpeedTextWidth, iSpeedTextHeight, SWP_NOZORDER );
  GetDlgItem( IDC_CTRLS_SPEED_SLIDER )->SetWindowPos( NULL, iSliderX, iSliderY, iSliderWidth, iSliderHeight, SWP_NOZORDER );

} // OnSize()


/**
 * Empty override so that user cannot dismiss dialog
 */
void ControlsDialog::OnOK()
{
  // By default, the CDialog::OnOK() base class method will close
  //  the dialog, when, for example, the user hits the enter key
  // So, we override this and do nothing

  return;
} // OnOK()


/**
 * Empty override so that user cannot dismiss dialog
 */
void ControlsDialog::OnCancel()
{
  // By default, the CDialog::OnCancel() base class method will close
  //  the dialog, when, for example, the user hits the ESC-key
  // So, we override this and do nothing

  return;
} // OnCancel()


/**
 * Called when dialog is being destroyed.
 */
void ControlsDialog::OnDestroy()
{
  EndoluminalView::SetUserPreferredFlightSpeed(rdrGlobal.m_pCurrDataset->m_submarine.GetThrottle());
  ViatronDialog::OnDestroy();
} // OnDestroy()


/**
 * Sets text for the tool tip balloon for a control
 * 
 * @param UINT uID            ID for the child control (not to be used, use ID in pTTTStruct)
 * @param NMHDR *pTTTStruct   Tool tip event notify struct
 * @param LRESULT *pResult    Success/failure
 * @return BOOL               True tells framework to set tooltip
 */
BOOL ControlsDialog::OnToolTipNeedText(UINT uID, NMHDR *pTTTStruct, LRESULT *pResult)
{
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
  UINT uIDFromCtrl = pTTTStruct->idFrom;
  if (pTTT->uFlags & TTF_IDISHWND)
  {
    // pTTTStruct->idFrom is actually the HWND of the tool, not it's ID
    uIDFromCtrl = ::GetDlgCtrlID((HWND)uIDFromCtrl);
    if (uIDFromCtrl == 0)
    {
      return FALSE;
    }

    static char sStr[20];
    pTTT->lpszText = sStr;
    pTTT->hinst = NULL;

    switch (uIDFromCtrl)
    {
      case IDC_CTRLS_SPEED_SLIDER:
      {
        SetBuddyData(uIDFromCtrl);
        sprintf(sStr, std::string().c_str());
        break;
      }
      default:
      {
        pTTT->lpszText = MAKEINTRESOURCE(uIDFromCtrl);
        pTTT->hinst = AfxGetResourceHandle();
        break;
      }
    }

    return TRUE;
  }

  return FALSE;
} // OnToolTipNeedText()


/**
 * Event handler for speed slider scrolling, presently only handles IDC_CTRLS_SPEED_SLIDER
 * 
 * @param UINT uSBCode             Scroll event code
 * @param UINT uPos                Current slider position, validity depends on nSBCode
 * @param CScrollBar* pScrollBar   Pointer to control receiving the event
 */
void ControlsDialog::OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar)
{
  const int4 iCtrlID(pScrollBar->GetDlgCtrlID());

  if (iCtrlID != IDC_CTRLS_SPEED_SLIDER)
  {
    ViatronDialog::OnHScroll(uSBCode, uPos, pScrollBar);
    LogDbg("invalid scrollbar control ID argument", "ControlsDialog", "OnHScroll");
    return;
  }

  const CSliderCtrl * pSpeedSlider = (CSliderCtrl *) pScrollBar;

  float4 fPercent(0);
  switch (uSBCode)
  {
    // Scroll codes defined in <commctrl.h>
    case TB_THUMBTRACK:     // while user is moving slider

    case TB_ENDTRACK:       // called at the end of all movements
      // NOTE: Same code to get the value for all scroll events
      fPercent = float4(pSpeedSlider->GetPos()) / float4((pSpeedSlider->GetRangeMax()-pSpeedSlider->GetRangeMin()+1));
      break;

    default: // don't handle any other notification codes
      break;
  }

  rdrGlobal.m_pCurrDataset->m_submarine.SetThrottle(fPercent);
  rdrGlobal.m_pCurrDataset->m_submarine.SetPotentialStrength(fPercent);
  if (rdrGlobal.m_pSecondaryDataset->IsLoadingComplete())
  {
    rdrGlobal.m_pSecondaryDataset->m_submarine.SetThrottle(fPercent);
    rdrGlobal.m_pSecondaryDataset->m_submarine.SetPotentialStrength(fPercent);
  }

  // TODO: Notify other windows via messages of changes to scrollbars as appropriate

  SetBuddyData(iCtrlID);

  // TODO: Send message to update tooltip text

  ViatronDialog::OnHScroll(uSBCode, uPos, pScrollBar);
} // OnHScroll()


/**
 * To set slider value in a static text field
 * 
 * @param const int4 ctrlID    Control ID
 */
void ControlsDialog::SetBuddyData(const int4 iCtrlID)
{
  CSliderCtrl * pSlider = STATIC_DOWNCAST(CSliderCtrl, GetDlgItem(iCtrlID));
  CStatic * pDisp = STATIC_DOWNCAST(CStatic, GetDlgItem(IDC_CTRLS_SLIDER_VALUE_DISP));

  CString str; str.Format("%d", pSlider->GetPos());
  pDisp->SetWindowText(str);

  CPoint pt;  CRect slRect, rect;

  GetCursorPos(&pt);              ScreenToClient(&pt);
  pSlider->GetWindowRect(slRect);  ScreenToClient(slRect);

  static const int4 iBorderSz(2);
  pDisp->GetClientRect(rect);
  rect.OffsetRect(pt.x, slRect.top - rect.Height() - iBorderSz);
  rect.InflateRect(iBorderSz,iBorderSz);  // apparently, we need to give it room for the border?

  pDisp->MoveWindow(rect);
  pDisp->BringWindowToTop();
  pDisp->UpdateWindow();
  pDisp->ShowWindow(SW_SHOW);
} // SetBuddyData()


//////////////////////////////////////////////////////////////////////////
//


/**
 * Updates all buttons based on current view modes
 */
void ControlsDialog::UpdateEndoModes()
{
  bool bUpdateModes(false);
  const VtxViewModes currModes(ViatronStudy::GetCurrentViewModes());

  // endoscopic view alone has autofly, so enable these buttons based on that mode
  m_autoFlyBtn.EnableWindow((currModes.GetValid() & AUTOFLY_VALID)
    || ((currModes.GetValid() & AUTOFLY2D_VALID) && rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::Auto2DFlight())) );
  m_shadowsBtn.EnableWindow(currModes.GetValid() & AUTOFLY_VALID);
  m_showPaintingBtn.EnableWindow(currModes.GetValid() & AUTOFLY_VALID);

  m_toolsBtn.EnableWindow(currModes.GetValid() & (LIGHTING_VALID | BIOPSY_VALID | LINEMEASURE_VALID | ROIMEASURE_VALID | ANNOTATION_VALID | SEGMENTPOLYP_VALID));
  m_btnLighting.EnableWindow((currModes.GetValid() & LIGHTING_VALID) == LIGHTING_VALID);
  m_btnTranslucency.EnableWindow((rdrGlobal.m_pCurrDataset->GetModality() != Dataset::MR) && ((currModes.GetValid() & BIOPSY_VALID) == BIOPSY_VALID) );
  m_btnLineMeasure.EnableWindow((currModes.GetValid() & LINEMEASURE_VALID) == LINEMEASURE_VALID);
  m_btn2DROI.EnableWindow((currModes.GetValid() & ROIMEASURE_VALID) == ROIMEASURE_VALID);
  m_btnAnnotation.EnableWindow((currModes.GetValid() & ANNOTATION_VALID) == ANNOTATION_VALID);

#if !defined(REGION_SEGMENTATION_DISABLED)
  const bool bRptWriteable( rdrGlobal.m_pCurrDataset->IsLoadingComplete() && ReportLayout::HasPermissions() && ReportLayout::CanModifyReport() );
  const bool bEnableSRT(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT())
    && ((currModes.GetValid() & SEGMENTPOLYP_VALID) == SEGMENTPOLYP_VALID) && bRptWriteable && User::GetCurrentUser().IsAllowed("CompleteReport") );
  m_btnRegionSegment.EnableWindow( bEnableSRT );
  m_toolsBtn.EnableMenuItem(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, bEnableSRT);
#else
  m_btnRegionSegment.EnableWindow(FALSE);
#endif

//  m_autoFlyBtn.SetCheck(0);
  switch (currModes.GetMutexModes())
  {
    case LINEMEASURE:
      m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = IDC_CDB_TOOL_LINEMEASURE;
      break;
    case ROIMEASURE:
      m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = IDC_CDB_TOOL_2DROI;
      break;
    case LIGHTING:
      m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = IDC_CDB_TOOL_LIGHTING;
      break;
    case ANNOTATION:
      m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = IDC_CDB_TOOL_ANNOTATION;
      break;
    case SEGMENTPOLYP:
			m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = IDC_CDB_REGIONSEGMENT_BTN;
      break;
    case AUTOFLY:
      m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, 0, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = -1;
      if (rdrGlobal.m_pCurrDataset->m_submarine.IsFlying())
        m_autoFlyBtn.SetCheck(1);
      else
      {
        ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY;
        m_autoFlyBtn.SetCheck(0);
      }
      break;
    case AUTOFLY2D:
      m_autoFlyBtn.SetCheck(1);
      m_iCurrentToolID = -1;
      break;
    case BIOPSY:
      m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = IDC_CDB_TOOL_TRANSLUCENCY;
      break;
    default:  // No mode
      m_toolsBtn.CheckRadioItem(EndoscopicMenus::viIdCurrPlusModes, 0, EndoscopicMenus::iCurrModePlusIds);
      m_iCurrentToolID = -1;
      break;
  }

  // Check the appropriate shadow menu button items
  const bool vbShadowStates[EndoscopicMenus::iShadowIds] = { (currModes & CENTERLINE), (currModes & AXIAL),
                                    (currModes & SAGITTAL), (currModes & CORONAL),
                                    (currModes & CROSS_SECTION) };
  m_shadowsBtn.CheckMenuItems(EndoscopicMenus::viIdShadows, vbShadowStates, EndoscopicMenus::iShadowIds);

  // track the centerline setting in the roaming profile
  ApplicationSettings::UserSettings().SetBool( APP_SETTING_SHOW_CENTERLINE, (currModes & CENTERLINE) ? true : false );
    
  // Set the on/off toggle of the show painting button
  if ( (m_showPaintingBtn.GetCheck()==1) != (currModes & MISSED_REGIONS))
    m_showPaintingBtn.SetCheck((currModes & MISSED_REGIONS) ? 1 : 0);

  // perceived speed
  if ( (m_checkPerceivedSpeed.GetCheck()==1) != Submarine::m_bPerceivedSpeed)
    m_checkPerceivedSpeed.SetCheck(Submarine::m_bPerceivedSpeed ? 1 : 0);

  bUpdateModes = true;

  UpdateData(FALSE);
} // UpdateEndoModes()


/**
 * Timer event handler, periodically updates dialog
 * 
 * @param UINT uIDEvent   Timer event ID
 */
void ControlsDialog::OnTimer(UINT_PTR uIDEvent)
{
  UpdateState();
  
  ViatronDialog::OnTimer(uIDEvent);
} // OnTimer()


/**
 * Sets the current state for dialog buttons and menus
 */
void ControlsDialog::UpdateState()
{
  // Update Endoscopic mode toggles
  if ( (m_currentVtxViewModes != ViatronStudy::GetCurrentViewModes()) ||
    (m_currentVtxViewModes.GetValid() != ViatronStudy::GetCurrentViewModes().GetValid()) )
  {
    UpdateEndoModes();
    m_currentVtxViewModes = ViatronStudy::GetCurrentViewModes();
  }

  if (m_eCurrNavDirection != rdrGlobal.m_pCurrDataset->m_submarine.GetFlightDirection())
  {
    m_segJumpCtrl.UpdateFlightDirn(rdrGlobal.m_pCurrDataset->m_submarine.GetFlightDirection());
    m_eCurrNavDirection = rdrGlobal.m_pCurrDataset->m_submarine.GetFlightDirection();
  }
  // (end) Update Endoscopic mode toggles

  // enable snapshot and movie buttons when appropriate
  const bool bSnapshot = ( rdrGlobal.m_pCurrDataset->IsLoadingComplete() && ReportLayout::HasPermissions()
    && ReportLayout::CanModifyReport() && User::GetCurrentUser().IsAllowed("CompleteReport") );
  if ( m_snapshotBtn.IsWindowEnabled() && !bSnapshot )
    m_snapshotBtn.EnableWindow( FALSE );
  else if ( !m_snapshotBtn.IsWindowEnabled() && bSnapshot )
    m_snapshotBtn.EnableWindow(TRUE);

   if ( m_btnRegionSegment.IsWindowEnabled() && !bSnapshot )
     m_btnRegionSegment.EnableWindow( FALSE );
   else if ( !m_btnRegionSegment.IsWindowEnabled() && bSnapshot && rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT()) )
     m_btnRegionSegment.EnableWindow(TRUE);
   m_toolsBtn.EnableMenuItem(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, bSnapshot);

  const bool bFlyValid((m_currentVtxViewModes.GetValid() & ViewModes::AUTOFLY_VALID) || (m_currentVtxViewModes.GetValid() & ViewModes::AUTOFLY2D_VALID));
  const bool bMovie(bFlyValid && rdrGlobal.m_pCurrDataset->IsLoadingComplete() && ReportLayout::HasPermissions() );
  if (m_movieBtn.IsWindowEnabled() && !bMovie && (m_movieBtn.GetCheck() == 0))
    m_movieBtn.EnableWindow(FALSE);
  else if (!m_movieBtn.IsWindowEnabled() && bMovie)
    m_movieBtn.EnableWindow(TRUE);
  // (end) Snapshot and movie buttons

  // Update Autofly button and Lumen Coverage
  if (!rdrGlobal.m_pCurrDataset->m_submarine.IsFlying())
  {
    if (m_bWasFlying)
    {
      m_bWasFlying = false;
      GetParent()->SendMessage(WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0);
      AfxGetMainWnd()->SendMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
      m_autoFlyBtn.SetCheck(0);
    }
    const bool bAutofly2D =
      ( ViatronStudy::m_modeOrthogView.GetMutexModes() & ViewModes::AUTOFLY2D ) ||
      ( ViatronStudy::m_modeObliqueView.GetMutexModes() & ViewModes::AUTOFLY2D );
    if ( ! bAutofly2D )
    {
      m_autoFlyBtn.SetCheck(0);
    }
  }
  else
  {
    if (!m_bWasFlying)
    {
      m_bWasFlying = true;
      if (ViatronStudy::GetCurrentViewModes().GetMutexModes() == AUTOFLY)
      {
        m_autoFlyBtn.SetCheck(1);
      }
    }
  }
  // (end) Update Autofly button and Lumen Coverage

  // Update Movie button
  if (GetParent()->SendMessage(ColonViatronixWnd::WM_USER_MOVIE_RECORD_MODE_QUERY, 0, 0) != 0)
  {
    // Flash the button icon for effect when recording movie...
    m_movieBtn.SetCheck((m_movieBtn.GetCheck() == 1) ? 2 : 1);
    SetButtonIcon(m_movieBtn, (m_movieBtn.GetCheck() == 1) ? IDB_MOVIE_BUTTON : IDB_BUTTON_SQUARE);
  }
  else if (m_movieBtn.GetCheck() != 0)
  {
    // ...else reset the icon, only if necessary, if not recording a movie
    m_movieBtn.SetCheck(0);
    SetButtonIcon(m_movieBtn, IDB_MOVIE_BUTTON);
  } // (end) Update movie button

  m_btnRegionSegment.SetCheck((ViatronStudy::m_modeEndoView & ViewModes::SEGMENTPOLYP)
    || (ViatronStudy::m_modeOrthogView & ViewModes::SEGMENTPOLYP) );

  m_btnAnnotation.SetCheck(ViatronStudy::m_modeEndoView & ViewModes::ANNOTATION);

  SetButtonIcon(m_supineProneBtn, IDB_BUTTON_SMALL);
  m_supineProneBtn.SetWindowText(ColonViews::Layouts::ColonViatronixWnd::PatientOrientationToString(*rdrGlobal.m_pCurrDataset, true).c_str());
  int4 iCheck((rdrGlobal.m_pCurrDataset->GetOrientation() == DatasetOrientations::eSUPINE) ? 0 : 1);
  if ( m_supineProneBtn.GetCheck() != iCheck )
    m_supineProneBtn.SetCheck(iCheck);

  UpdateData(FALSE);
} // UpdateState()


/////////////////////////////////////////////////////////////////////////////
// ControlsDialog button event handlers


/**
 * Toggle autofly button clicked
 */
void ControlsDialog::OnAutoflyClicked()
{
  AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(IDM_TOGGLE_AUTOFLY, 0), 0);
} // OnAutoflyClicked()


/**
 * Flight direction toggled
 */
void ControlsDialog::OnViewDirectionBtnClicked()
{
  rdrGlobal.m_pCurrDataset->m_submarine.ToggleFlightDirection();
  if(rdrGlobal.IsDualDataLoaded()) rdrGlobal.m_pSecondaryDataset->m_submarine.ToggleFlightDirection();
  m_segJumpCtrl.UpdateFlightDirn(rdrGlobal.m_pCurrDataset->m_submarine.GetFlightDirection());
  AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
} // OnViewDirectionBtnClicked()


/**
 * Flip direction of view 180 degrees
 */
void ControlsDialog::OnFlipViewBtnClicked()
{
  rdrGlobal.m_pCurrDataset->m_submarine.SetOrientation(-rdrGlobal.m_pCurrDataset->m_submarine.GetOrientation());
  rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  CWnd *parent = GetParentOwner();
  if (parent)
  {
    parent->Invalidate(FALSE);
  }
  AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);
} // OnFlipViewBtnClicked()


/**
 * Take a snapshot of the current endoscopic view
 */
void ControlsDialog::OnSnapshotClicked()
{
  AfxGetMainWnd()->PostMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
} // OnSnapshotClicked()


/**
 * Toggle movie mode
 */
void ControlsDialog::OnMovieToggled()
{
  GetParent()->PostMessage(ColonViatronixWnd::WM_USER_MOVIE_RECORD_MODE_TOGGLE, WPARAM(0), LPARAM(0));
  m_movieBtn.SetCheck((m_movieBtn.GetCheck() == 0) ? 1 : 0);
  SetButtonIcon(m_movieBtn, IDB_MOVIE_BUTTON);
} // OnMovieToggled()


/**
 * Toggle region segment mode
 **/
void ControlsDialog::OnRegionSegmentClicked()
{
} // OnRegionSegmentClicked()


/**
 * Toggle missed region marking display
 */
void ControlsDialog::OnShowPaintingClicked()
{
  if (rdrGlobal.m_bRaycastPaintVol = (m_showPaintingBtn.GetCheck()==1))
  {
    ViatronStudy::GetCurrentViewModes() += ViewModes::MISSED_REGIONS;
  }
  else
  {
    ViatronStudy::GetCurrentViewModes() -= ViewModes::MISSED_REGIONS;
  }

  rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  MainAppFrame::GetMainFrame()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_NAVIGATION), 0);
} // OnShowPaintingClicked()


/////////////////////////////////////////////////////////////////////////////
// ControlsDialog windows message handlers


/**
 * CMenuButton menu item command handler, WPARAM differs from normal menu command handler! (see below)
 * 
 * @param WPARAM wParam   LOWORD - menu item ID; HIWORD - TRUE (1) item is set
 * @param LPARAM lParam   Identifies the control that sends the message, or 0 if message not from a control
 * @return BOOL           Non-zero if command was handled
 */
BOOL ControlsDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  ViewModes::VIEWMODES mode = ViewModes::VIEWMODES(0);
  const bool bSet = (HIWORD(wParam) != FALSE);

  switch (LOWORD(wParam))
  {
    case ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION:
      mode = ViewModes::ANNOTATION;
      break;

    case ID_ENDOSCOPICVIEW_SLICEINDICATORS_AXIAL:
      mode = ViewModes::AXIAL;
      rdrGlobal.m_bAxialShadow = bSet;
      break;
    case ID_ENDOSCOPICVIEW_SLICEINDICATORS_SAGITTAL:
      mode = ViewModes::SAGITTAL;
      rdrGlobal.m_bSagittalShadow = bSet;
      break;
    case ID_ENDOSCOPICVIEW_SLICEINDICATORS_CORONAL:
      mode = ViewModes::CORONAL;
      rdrGlobal.m_bCoronalShadow = bSet;
      break;
    case ID_ENDOSCOPICVIEW_SLICEINDICATORS_CROSSSECTIONAL:
      mode = ViewModes::CROSS_SECTION;
      rdrGlobal.m_bObliqueShadow = bSet;
      break;

    case ID_ENDOSCOPICVIEW_OPTIONS_CENTERLINE:
      mode = ViewModes::CENTERLINE;
      break;


    case IDC_CDB_TOOL_LINEMEASURE:
      UpdateData(TRUE);
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_CURRENTMODE_LINEMEASURE,0), 0);
      break;
    case IDC_CDB_TOOL_LIGHTING:
      UpdateData(TRUE);
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_CURRENTMODE_LIGHTING,0), 0);
      break;
    case IDC_CDB_TOOL_TRANSLUCENCY:
      UpdateData(TRUE);
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_OPTIONS_TRANSLUCENT_DISPLAY,0), 0);
      break;
    case IDC_CDB_TOOL_2DROI:
      UpdateData(TRUE);
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_CURRENTMODE_ROIMEASURE,0), 0);
      break;
    case IDC_CDB_TOOL_ANNOTATION:
      UpdateData(TRUE);
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEWWINDOW_MEASUREMENTOPTIONS_ADDANNOTATION,0), 0);
      break;
    case IDC_CDB_REGIONSEGMENT_BTN:
	  if ( rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT()) )
	  {
		  UpdateData(TRUE);
			rdrGlobal.m_bDisplaySRTroi2D = true;
			rdrGlobal.m_bDisplaySRTroi   = true;
		  SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, 0), 0);
	  }
      break;

    default:
      return ViatronDialog::OnCommand(wParam, lParam);
  }

  if (bSet)
  {
    ViatronStudy::GetCurrentViewModes() += mode;
  }
  else
  {
    ViatronStudy::GetCurrentViewModes() -= mode;
  }

  UpdateEndoModes();
  m_currentVtxViewModes = ViatronStudy::GetCurrentViewModes();

  rdrGlobal.m_supine.m_fields.m_volumeRender.FlushAccumulationBuffer();
  rdrGlobal.m_prone .m_fields.m_volumeRender.FlushAccumulationBuffer();
  AfxGetMainWnd()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_ALL), 0);

  return TRUE;
} // OnCommand()


/**
 * Called to set mouse cursor icon
 * 
 * @param CWnd* pWnd      Pointer to window containing cursor
 * @param UINT uHitTest   Area within dialog where cursor is location (e.g. HT_BORDER)
 * @param UINT uMessage   Mouse message number
 * @return BOOL           TRUE if cursor was set
 */
BOOL ControlsDialog::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage) 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  return TRUE;
} // OnSetCursor()


/**
 * Keyboard message handler; simply redirects message to parent
 * 
 * @param UINT uChar     Keyboard character identifier
 * @param UINT uRepCnt   Repeat count (the number of times the keystroke is repeated as a result of the user holding down the key)
 * @param UINT uFlags    Key codes, see documentation for WM_KEYDOWN
 */
void ControlsDialog::OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags)
{
  GetParentOwner()->PostMessage(WM_KEYDOWN, uChar, MAKELPARAM(uRepCnt, uFlags));
  return;
} // OnKeyDown()

/**
 * Mouse move handler; passes focus on to parent; hides slider "tooltip"
 * 
 * @param UINT uFlags    Identifies which mouse buttons and/or keyboards keys are currently held down
 * @param CPoint point   Location of mouse cursor
 */
void ControlsDialog::OnMouseMove(UINT uFlags, CPoint point) 
{
  if (m_sliderValStatic.IsWindowVisible())
  {
    m_sliderValStatic.ShowWindow(SW_HIDE);
  }

  ViatronDialog::OnMouseMove(uFlags, point);
} // OnMouseMove()


/////////////////////////////////////////////////////////////////////////////
// ControlsDialog application message handlers


/**
 * Jump to previous segment (opposite direction of flight)
 */
void ControlsDialog::OnPrevSegmentBtnClicked()
{
  MainAppFrame::GetMainFrame()->PostMessage(WM_COMMAND, IDM_JUMP_PREVSEGMENT, 0);
} // OnPrevSegmentBtnClicked()


/**
 * Jump to next segment (in direction of flight)
 */
void ControlsDialog::OnNextSegmentBtnClicked()
{
  MainAppFrame::GetMainFrame()->PostMessage(WM_COMMAND, IDM_JUMP_NEXTSEGMENT, 0);
} // OnNextSegmentBtnClicked()


/**
 * Toggle perceived speed option
 */
void ControlsDialog::OnClickedPerceivedSpeedCheck()
{
  Submarine::m_bPerceivedSpeed = m_checkPerceivedSpeed.GetCheck();
} // OnClickedPerceivedSpeedCheck()


/**
 * Sets the dataset orientation appropriately
 */
void ControlsDialog::OnClickedDatasetBtn() 
{
  if( rdrGlobal.IsDualDataLoaded() && Utilities::Datasets::IsLocked() ) // For Linked actions
    rdrGlobal.UpdateWorldPosition( *rdrGlobal.m_pCurrDataset, *rdrGlobal.m_pSecondaryDataset );

  AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM((m_supineProneBtn.GetCheck() == 0) ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE), 0);

} // OnClickedDatasetBtn()


// $Log: ControlsDialog.C,v $
// Revision 1.32.2.19  2010/10/20 20:47:33  dongqing
// The SRT tool availability and display switch will be only determined
// by the VCSetting.xml switch now.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.32.2.18  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.32.2.17  2010/04/21 13:57:48  dongqing
// Controls....ViatronDialog is your father.....*haaa, hissss*
//
// Revision 1.32.2.16  2010/04/17 11:06:19  dongqing
// srt tool button and menu button item disabled when report locked
//
// Revision 1.32.2.15  2010/04/11 11:40:02  dongqing
// centering child controls
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.32.2.14  2010/04/10 23:52:31  dongqing
// centering controls dialog children on the fly
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.32.2.13  2010/01/21 05:15:55  dongqing
// fit speed slider into widescreen layouts
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.32.2.12  2009/12/11 23:14:23  dongqing
// split tools menu button into separate buttons
//
// Revision 1.32.2.11  2009/12/11 22:32:39  dongqing
// split tools menu button into separate buttons
//
// Revision 1.32.2.10  2009/11/25 22:12:40  dongqing
// moved dataset switch button to controls
//
// Revision 1.32.2.9  2009/11/20 22:04:20  dongqing
// localized all missed regions list functionality to a single control class.
//
// Revision 1.32.2.8  2009/10/07 16:13:12  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.32.2.7  2008/12/31 21:45:37  jmeade
// moved voxel size selection control into the sliceview
//
// Revision 1.32.2.6  2008/12/20 00:39:13  jmeade
// roi voxel edit size.
//
// Revision 1.32.2.5  2008/07/02 21:12:42  jmeade
// Issue #6123: fixed bad code to enable/disable srt button.
//
// Revision 1.32.2.4  2008/07/02 21:08:37  jmeade
// Issue #6123: fixed bad code to enable/disable srt button.
//
// Revision 1.32.2.3  2008/03/20 19:17:18  jmeade
// Issue 5963: disable srt if report is locked.
//
// Revision 1.32.2.2  2008/01/24 21:47:48  jmeade
// fixing button sizing.
//
// Revision 1.32.2.1  2007/04/12 16:08:09  jmeade
// system setting for enabling srt tool.
//
// Revision 1.32  2007/03/08 22:21:49  romy
// code review - header files adjusted
//
// Revision 1.31  2007/03/01 01:18:55  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.30  2007/02/02 20:22:57  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.29  2007/01/25 16:30:09  romy
// Bug Fix created by fixing the EndoAxial SRT
//
// Revision 1.28  2007/01/24 22:45:41  romy
// OnRegionSegmentClicked modified for SRT tool
//
// Revision 1.27  2007/01/22 20:38:53  romy
// Checks the hasPermission option to enable/disable Movie button
//
// Revision 1.26  2007/01/11 02:36:11  jmeade
// turn on display of srt regions when tool is initiated.
//
// Revision 1.25  2007/01/02 20:58:42  jmeade
// enable segment region tool option for normal release.
//
// Revision 1.24  2006/12/21 21:59:40  jmeade
// disable segment region tool option for beta release.
//
// Revision 1.23  2006/10/04 20:49:10  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.22  2006/10/03 17:16:21  jmeade
// variable names.
//
// Revision 1.21  2006/10/03 02:04:12  jmeade
// segment region button on controls.
//
// Revision 1.20  2006/09/22 19:53:18  jmeade
// fixed code to enable segment_polyp mode for all valid views.
//
// Revision 1.19  2006/09/13 16:15:33  jmeade
// enable segment polyp mode.
//
// Revision 1.18  2006/07/11 19:17:11  jmeade
// Issue 4658: check 'take snapshot' permissions.
//
// Revision 1.17  2006/06/02 21:49:35  jmeade
// Issue 4814: sizing dialog elements.
//
// Revision 1.16  2006/06/02 20:31:21  jmeade
// Issue 4814: resize missed regions.
//
// Revision 1.15  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.14  2006/01/31 14:25:37  frank
// code review
//
// Revision 1.13  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.12  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.11  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.10.2.4  2006/02/28 17:28:20  geconomos
// modified for SkinButton changes
//
// Revision 1.10.2.3  2006/01/10 19:36:58  jmeade
// extra save of current flight speed on destroy, in case window is closed directly after updating speed.
//
// Revision 1.10.2.2  2005/12/23 16:49:15  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.10.2.1  2005/11/10 01:28:38  jmeade
// Fixed bug saving session values between views and across sessions.
//
// Revision 1.10  2005/11/04 00:57:27  jmeade
// Issue 4513: Recoded vcsettings xml functionality.
//
// Revision 1.9  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
//
// Revision 1.8  2005/10/13 18:11:32  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.7  2005/09/13 21:45:29  jmeade
// proper class derivation.
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
// Revision 3.31.2.12.2.9.2.5  2005/07/18 19:32:31  jmeade
// Issue 4285: When jumping to next missed region, ensure you can see the current as well as the next missed region.
//
// Revision 3.31.2.12.2.9.2.4  2005/07/08 18:58:04  jmeade
// privatize dataset modality variable.
//
// Revision 3.31.2.12.2.9.2.3  2005/07/01 17:09:45  jmeade
// Issue 4200: Disable translucency button for MR.
//
// Revision 3.31.2.12.2.9.2.2  2005/07/01 00:04:49  jmeade
// Issue 4230: Don't disable movie button while movie is recording.
//
// Revision 3.31.2.12.2.9.2.1  2005/06/27 17:39:10  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.31.2.12.2.9  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.31.2.12.2.8  2005/04/25 13:44:29  frank
// Issue #4148: Updated display after toggling pseudocolor and painting
//
// Revision 3.31.2.12.2.7.2.1  2005/06/08 13:59:53  geconomos
// added some refresh mechanisms
//
// Revision 3.31.2.12.2.7  2005/04/19 13:00:16  frank
// Issue #4097: Resetting the autofly button at the end of a 2D flight.
//
// Revision 3.31.2.12.2.6  2005/04/15 16:30:12  jmeade
// Issue 2815: Also update display when valid-view-modes change.
//
// Revision 3.31.2.12.2.5  2005/04/08 15:28:15  frank
// Issue #4097: turned off autfly button at end of centerline flight
//
// Revision 3.31.2.12.2.4  2005/03/26 19:46:04  frank
// revamped main application idle/processing loop - checkin for Jeff
//
// Revision 3.31.2.12.2.3  2005/03/26 16:42:25  frank
// Issue #4064: Updated 3D slice indicators immediately
//
// Revision 3.31.2.12.2.2  2005/03/22 17:01:57  frank
// Issue #:3982: Fixed distance numbers in missed region tool
//
// Revision 3.31.2.12.2.1  2005/03/14 23:01:42  jmeade
// Issue 4022: Removed beta tool.
//
// Revision 3.31.2.12  2004/09/30 15:16:40  jmeade
// Interaction for polyp segmentation.
//
// Revision 3.31.2.11  2004/09/27 20:53:33  jmeade
// Preference to disable 2d auto flight.
//
// Revision 3.31.2.10  2004/08/27 22:04:48  jmeade
// Issue 3538: Option to adjust perceived flight speed accordingly with colon region width/diameter.
//
// Revision 3.31.2.9  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.31.2.8  2003/07/25 16:26:45  jmeade
// Issue 3401: Update child views when flight stops.
//
// Revision 3.31.2.7  2003/03/13 01:11:54  jmeade
// Issue 2890: Mode change message handlers will flush accum buffer; Removed dead code.
//
// Revision 3.31.2.6  2003/03/10 23:48:55  jmeade
// Changed button ID.
//
// Revision 3.31.2.5  2003/02/26 17:40:57  jmeade
// More concise method for triggering child view updates.
//
// Revision 3.31.2.4  2003/02/22 11:54:48  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.31.2.3  2003/02/19 07:05:13  jmeade
// Issue 2848:  Localizing view mode changing code to ViatronViewWnd.
//
// Revision 3.31.2.2  2003/02/14 01:10:43  jmeade
// Issue 2847:  Flush accum buffer for both datasets when toggling tools modes.
//
// Revision 3.31.2.1  2003/02/12 00:59:46  jmeade
// Clarifying names for 2D flight modes.
//
// Revision 3.31  2003/01/31 20:02:57  jmeade
// Missed region toggle should update both studies (it is a global option).
//
// Revision 3.30  2003/01/30 16:57:08  jmeade
// Localized registry storage of all flight params to NavigationView.
//
// Revision 3.29  2003/01/29 20:28:25  jmeade
// Missing statement when initially setting flight speed.
//
// Revision 3.28  2003/01/21 19:31:49  jmeade
// Duplicate: Revision: 3.12.2.16 : Size lumen coverage list columns appopriately for any header text.
//
// Revision 3.27  2002/12/12 21:23:10  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.26  2002/11/24 17:43:22  kevin
// Removed the 90% limit on missed patches AND Added toggle flight direction on both datasets (for S/P registration)
//
// Revision 3.25  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.24  2002/09/03 21:04:27  jmeade
// Don't set the window region (to a rounded rect).
//
// Revision 3.23  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.22  2002/06/04 18:36:59  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.21  2002/05/15 19:26:10  jmeade
// Allowing for general use of registry (and not just Software\AppName\...).
//
// Revision 3.20  2002/05/09 20:54:15  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.19  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.18  2002/04/18 18:24:52  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.17  2002/04/12 02:12:32  jmeade
// Compile error.
//
// Revision 3.16  2002/04/12 02:10:17  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.15  2002/03/28 18:49:48  jmeade
// Allow run-time setting of maximum flight speeds.
//
// Revision 3.14  2002/03/25 21:23:20  jmeade
// Cleanup of log record code.
//
// Revision 3.13  2002/03/13 22:01:15  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.12.2.15  2002/08/13 00:46:03  jmeade
// Minor change: reusing stored modes value.
//
// Revision 3.12.2.14  2002/07/31 21:17:46  jmeade
// Issue 2585: Don't set focus to dialog on mouse move, hopefully this won't screw up anything else.
//
// Revision 3.12.2.13  2002/06/18 22:04:37  jmeade
// Issue 2454:  Only SetFocus() when no known processes, spawned by Viewer, as running.
//
// Revision 3.12.2.12  2002/05/28 20:02:08  jmeade
// Update dialog controls immediately after changing view mode.
//
// Revision 3.12.2.11  2002/05/23 16:48:42  jmeade
// Issue 2270:  Support for 2D annotations mode.
//
// Revision 3.12.2.10  2002/05/13 17:30:28  jmeade
// Slice indicators not valid on 2D slice views.
// CMenuButton reimplemented, better setup receives CMenu resource ID.
//
// Revision 3.12.2.9  2002/04/27 00:53:27  jmeade
// Issue 1559:  Enable movie button only for views that can fly.
//
// Revision 3.12.2.8  2002/04/26 21:10:17  jmeade
// Issue 2060:  Snapshot and movie button are *not* enabled from View modes, so take out of UpdateEndoModes() fn.
//
// Revision 3.12.2.7  2002/04/23 21:35:54  jmeade
// Issue 2044:  Auto-fly button should also reflect state of slice-auto-fly.
//
// Revision 3.12.2.6  2002/04/17 23:22:34  jmeade
// Remembers last flight speed setting.
//
// Revision 3.12.2.5  2002/04/10 18:53:55  jmeade
// Issue 2037:  Setting speed using slider position as a percentage of the slider range.
//
// Revision 3.12.2.4  2002/03/28 22:20:41  jmeade
// Allow run-time setting of maximum flight speeds.
//
// Revision 3.12.2.3  2002/02/28 21:08:12  jmeade
// Issue 1537:  Fixed issue introduced in 3.11, using wrong variable for control ID for tool tip text.
//
// Revision 3.12.2.2  2002/02/19 19:15:17  jmeade
// Issue 1848:  Disable movies on CD study.
//
// Revision 3.12.2.1  2002/02/04 20:37:33  jmeade
// Issue 1647:  Toggle view direction only for current (not both!) dataset.
//
// Revision 3.12  2002/01/17 04:02:22  jmeade
// Code standards.
//
// Revision 3.11  2002/01/09 16:14:59  jmeade
// Movie record button implemented; Coding standards.
//
// Revision 3.10  2002/01/04 18:22:15  jmeade
// Movie record button.
//
// Revision 3.9  2001/12/29 00:11:31  jmeade
// Merge from 1.1 branch.
//
// Revision 3.8  2001/12/21 01:08:45  jmeade
// Combined common code.
//
// Revision 3.7  2001/11/20 09:49:57  jmeade
// Allow caller to set background image resource; slice options button.
//
// Revision 3.6  2001/11/19 13:18:08  kevin
// Fixed some of the problems with S/P orientation when segments are flipped
//
// Revision 3.5  2001/11/17 03:53:16  jmeade
// Threw out the old carpet.
//
// Revision 3.4  2001/11/12 21:14:44  binli
// GUI: use autobtn to control 2d auto-fly
//
// Revision 3.3  2001/11/09 00:41:21  jmeade
// Added control bar to registration view; Removed unused strength slider
//
// Revision 3.2  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.1.2.1  2001/11/09 16:18:46  jmeade
// Standards.
//
// Revision 3.1  2001/10/17 20:21:17  jmeade
// Enable Tools button iff at least one of the tools is valid .  Issue 1181.
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:00   ingmar
// Initial revision.
// Revision 1.88  2001/08/29 16:47:51  jmeade
// Snapshot button
//
// Revision 1.87  2001/08/20 20:08:08  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.86  2001/07/25 22:51:53  binli
// ID 89: new exception handling.
//
// Revision 1.85  2001/07/24 21:48:47  binli
// ID 89: exception handling in v1k GUI
//
// Revision 1.84  2001/07/05 15:12:36  jmeade
// reordered mode menu options
//
// Revision 1.83  2001/06/21 21:15:32  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.82  2001/06/14 00:39:47  jmeade
// enum encapsulation
//
// Revision 1.81  2001/05/10 19:34:38  jmeade
// Coding conventions
//
// Revision 1.80  2001/05/02 20:25:29  ingmar
// .
//
// Revision 1.79  2001/04/30 12:33:37  frank
// Reflected changes in Study class.
//
// Revision 1.78  2001/04/30 02:20:03  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.77.2.1  2001/05/02 00:06:03  jmeade
// m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.77  2001/04/13 02:31:50  jmeade
// headers and footers
//
// Revision 1.76  2001/04/11 20:50:27  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.75  2001/04/11 13:27:32  frank
// Moved volume data to Fields class.
//
// Revision 1.74  2001/04/10 17:52:30  frank
// New coding conventions in Submarine class.
//
// Revision 1.73  2001/03/30 22:44:56  jmeade
// derive from ViatronDialog (instead of CDialog) for common skin code
//
// Revision 1.72  2001/03/16 16:24:34  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.71  2001/03/08 23:01:37  jmeade
// removed test-strings from listbox
//
// Revision 1.70  2001/03/08 02:09:23  jmeade
// Bug: turned on autofly when in normal fly; untabify; coding conventions
//
// Revision 1.69  2001/02/27 23:49:43  jmeade
// OnTimer(): update missed regions list when study orientation changes
//
// Revision 1.68  2001/02/27 19:33:47  jmeade
// Missed regions list (m_patchList) a member of Study class
//
// Revision 1.67  2001/02/15 23:45:53  jeff
// Missed regions: Size (voxels) now reads Size (mm2)
//
// Revision 1.66  2001/02/15 00:16:38  jeff
// more messages for keydowns; fix for auto painting update bug (check flying
// state and autofly button state on every timer message)
//
// Revision 1.65  2001/02/12 23:31:06  jeff
// enable/disable lighting and next/jump missed region options as appropriate
//
// Revision 1.64  2001/02/12 19:14:30  jeff
// only update modes and navigation direction when necessary,
// reduces bmp flashing over constant updating
//
// Revision 1.63  2001/02/12 17:36:43  jeff
// translucent/biopsy only in endoscopic view
//
// Revision 1.62  2001/02/09 18:41:11  jeff
// app messages for detect, jump to next missed region replace keyboard keys
//
// Revision 1.61  2001/02/08 17:04:44  jeff
// uncommented lines for enabling/disabling measurement menu items
//
// Revision 1.60  2001/02/06 21:45:20  jeff
// clarified segment jump operations
//
// Revision 1.59  2001/02/06 14:14:29  frank
// Revamped study load progress display.
//
// Revision 1.58  2001/02/05 20:49:06  jeff
// measure ==> line measure and roi measure
//
// Revision 1.57  2001/02/05 16:13:24  jeff
// en/dis-abling control buttons based on view modes
//
// Revision 1.56  2001/02/02 20:20:21  jeff
// green/red colored segment direction buttons; correct direction for cecum/rectum buttons
//
// Revision 1.55  2001/02/02 07:02:40  jeff
// Moving strings to resource table
//
// Revision 1.54  2001/02/01 00:55:00  jeff
// changed missed region list to a ListCtrl to give it headers and columns
//
// Revision 1.53  2001/01/31 18:16:47  jeff
// function for updating missed region list
//
// Revision 1.52  2001/01/30 04:41:31  kevin
// FIxed the live update of painted information
//
// Revision 1.51  2001/01/30 02:13:16  jeff
// changed PICKINFO and MARKING mode to MISSED_REGIONS
//
// Revision 1.50  2001/01/25 00:05:22  jeff
// no message
//
// Revision 1.49  2001/01/24 21:04:31  jeff
// autofly gets its own button; show painting is a toggle
//
// Revision 1.48  2001/01/24 15:57:10  jeff
// Aren't all bools supposed to start m_b...?; show painting toggle button
//
// Revision 1.47  2001/01/24 01:58:17  jeff
// removed "update missed regions" button; added "display of missed regions" button
//
// Revision 1.46  2001/01/13 00:31:43  jeff
// Windows controls a region after call to SetWindowRgn(), so detach it
//
// Revision 1.45  2001/01/08 18:43:11  frank
// Clarified Supine-Prone dataset distinction.
//
// Revision 1.44  2001/01/05 15:35:36  frank
// Used SegmentList to contain navigation direction.
//
// Revision 1.43  2000/12/27 23:06:18  jeff
// global GetViatronMainFrame() ==> static MainAppFrame::GetMainFrame()
//
// Revision 1.42  2000/12/27 01:17:37  jeff
// stretching the background bitmap slightly larger than window size
//  to leave room for offsetting brush, which accounts for borders
//
// Revision 1.41  2000/12/27 00:36:17  jeff
// Correcting background updates of listbox; rounded window region
//
// Revision 1.40  2000/12/22 23:57:35  jeff
// moved SegmentJumpCtrl to ControlsDialog class (class condensing)
//
// Revision 1.39  2000/12/22 02:17:10  jeff
// no message
//
// Revision 1.38  2000/12/22 00:22:56  jeff
// segment jump bitmaps
//
// Revision 1.37  2000/12/21 00:32:38  jeff
// specific arrow bitmaps for 'to cecum' and 'to rectum'
//
// Revision 1.36  2000/12/20 19:24:54  jeff
// OnCtlColor() to paint dialog background
//
// Revision 1.35  2000/12/12 18:10:46  jeff
// missed patch list --> controls dialog
//
// Revision 1.34  2000/12/12 18:00:40  jeff
// SegmentJumpCtrl (moving segment jump from overview dialog);
//  bitmaps for icons
//
// Revision 1.33  2000/12/08 00:52:11  jeff
// skin buttons
//
// Revision 1.32  2000/11/25 05:42:54  kevin
// Added Biopsy mode
//
// Revision 1.31  2000/11/25 02:20:46  jeff
// Fun with icons and buttons!
//
// Revision 1.30  2000/11/25 01:30:36  jeff
// Reflected change (from loooooong time ago) from using independent
//  Translucent mode to using mutually-exclusive Biopsy mode
//
// Revision 1.29  2000/11/24 19:34:13  jeff
// New flip view button
//
// Revision 1.28  2000/11/24 14:03:55  frank
// Plugged resource leak
//
// Revision 1.27  2000/11/23 03:02:07  jeff
// Moved around a couple of menubuttons/menubutton items
//
// Revision 1.26  2000/11/22 19:26:29  jeff
// New bitmaps for the arrows (thanks Frank!); removed some commented code
//
// Revision 1.25  2000/11/22 17:28:23  jeff
// Getting the precise color at a window point to determine
//  background color for child controls, dialogs, etc.
//
// Revision 1.24  2000/11/21 21:25:20  jeff
// Update view direction (along with other buttons)
//
// Revision 1.23  2000/11/21 04:25:01  jeff
// Removed unused CalcWindowRect() fn
//
// Revision 1.22  2000/11/20 17:28:17  jeff
// Setting (static) colors closer to dialog's color
//
// Revision 1.21  2000/11/20 00:52:28  jeff
// OnCtlControl() to custom color dialog
//
// Revision 1.20  2000/11/19 22:47:30  kevin
// Added FlushAcuclumlationbuffer calls
//
// Revision 1.19  2000/11/17 16:11:42  frank
// Flushed accumulation buffer upon mode changes
//
// Revision 1.18  2000/11/17 04:21:52  kevin
// Made old strangth slider work for renderSize (for adaptive sampling)
// Temporary for testing what size is optimal
//
// Revision 1.17  2000/11/14 19:26:49  frank
// Flushed accumulation buffer during jumps
//
// Revision 1.16  2000/11/14 18:56:21  frank
// Changed study reference to pointer
//
// Revision 1.15  2000/11/08 17:39:18  frank
// Added m_study to ReaderGlobal
//
// Revision 1.14  2000/11/08 12:40:11  frank
// Removed Strength slider
//
// Revision 1.13  2000/10/31 21:34:20  jeff
// Bug fix:  only set the endoscopic mode if that is the command option given
//
// Revision 1.12  2000/10/30 13:37:25  kevin
// Added Biopsy Mode for moving the magic window around
//
// Revision 1.11  2000/10/27 00:39:26  kevin
// Added 3D 2D Map COunting Makring
//
// Revision 1.10  2000/10/13 04:53:56  jeff
// no message
//
// Revision 1.9  2000/10/12 21:08:28  kevin
// Added Axial, Sagittal and Coronal Flags. Also cleaned up a little
//
// Revision 1.8  2000/10/12 02:49:56  kevin
// Moved Flight Direction to Submarine (Where it
// is better encapsualted)
//
// Revision 1.7  2000/10/04 23:57:42  jeff
// Changes for verification view
//
// Revision 1.6  2000/09/27 14:43:18  jeff
// Added button mode bitmaps for measure, highlights, autofly button
//
// Revision 1.5  2000/09/26 20:59:27  jeff
// Many changes for new enable popup menu buttons
//
// Revision 1.4  2000/09/22 23:15:42  jeff
// button menus
//
// Revision 1.3  2000/09/21 15:23:37  jeff
// Removed popup listbox
//
// Revision 1.2  2000/09/20 21:53:23  jeff
// Adding menu buttons
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 29    9/08/00 6:07p Jeff
// changed IndependentModes() and MutexModes() to Get...Modes()
// 
// 28    9/05/00 6:41p Jeff
// Removed code that resets current navigation mode (totally unnecessary,
// and in effect halts an autofly!!)
// 
// 27    9/01/00 3:46p Jeff
// More incremental changes for modes
// 
// 26    9/01/00 12:10p Jeff
// Another 'modes' incremental checkin
// 
// 25    8/30/00 6:32p Jeff
// One standardarized enum value class for all mode windows
// 
// 24    8/29/00 6:42p Jeff
// Mode changes
// 
// 23    8/29/00 4:17p Jeff
// A new class for protected access (using a callback function) to a
// value, typically used for mode value setting
// 
// 22    8/28/00 1:04p Antonio
// updated header and footer
//
//*****************  Version 21  *****************
//User: Jeff            Date:  8/25/00  Time:  1:06p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  More view toggles
//Lower value of speed range now 1 (instead of 0)
//
//*****************  Version 20  *****************
//User: Jeff            Date:  8/25/00  Time: 12:30p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Arrow cursors over dialog "bars"
//
//*****************  Version 19  *****************
//User: Jeff            Date:  8/25/00  Time: 12:19p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  More view mode toggles (controls dialog)
//
//*****************  Version 18  *****************
//User: Jeff            Date:  8/24/00  Time:  7:22p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Using SetButtonIcons function
//
//*****************  Version 17  *****************
//User: Jeff            Date:  8/24/00  Time:  6:42p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Removed extra static (strength) control, renamed speed static
//control
//
//*****************  Version 16  *****************
//User: Jeff            Date:  8/24/00  Time:  3:52a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Change View direction (button, etc) now specifies towards rectum or
//cecum
//Added flip/rotate view direction (180*) button
//
//*****************  Version 15  *****************
//User: Jeff            Date:  8/24/00  Time: 12:51a
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Show the "tooltip" when TTN_NEEDTEXT notification is received
//
//*****************  Version 14  *****************
//User: Jeff            Date:  8/23/00  Time:  4:53p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Moving window showing speed/strength value while updating
//
//*****************  Version 13  *****************
//User: Jeff            Date:  8/22/00  Time: 11:57p
//Checked in $/ViatronMDI/src/Viatron
//Comment:
//  Initial values of speed and strength sliders read from rdrGlobal
//No longer using two stationary buddy edits for speed/strength value display
//  (will use tooltips or close mockup in near future)
//OnSetCursor() overridden to set mouse cursor for dialog
//WM_KEYDOWN messages sent to parent window
//MouseMove() events set keyboard focus to parent window
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 12  *****************
//User: Jeff            Date:  8/14/00  Time:  2:49p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Created a new class ViatronStudy for study-specific info (bookmarks,
//narvigation)
//
//*****************  Version 11  *****************
//User: Jeff            Date:  7/31/00  Time:  6:26p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added view direction (cecum or rectum) button
//
//*****************  Version 10  *****************
//User: Jeff            Date:  7/31/00  Time:  5:36p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added an enum class to GuiNavigation (GuiEnum) to:
//(1) Control access to mode vals (can only use = operator from inside class)
//(2) Reduce duplication (one each of Get,Set,operator ==,etc. methods)
//
//*****************  Version 9  *****************
//User: Jeff            Date:  7/27/00  Time:  2:33p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added endoscopic view mode toggles to GUI
//Smoother updating of Overview dialog
//BroadcastSendMessage changed to BroadcastMessage(..., bWait)
//
//*****************  Version 8  *****************
//User: Jeff            Date:  7/26/00  Time:  9:44p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Implemented callbacks for controls dialog endoscopic mode selection
//
//*****************  Version 7  *****************
//User: Kevin           Date:  7/24/00  Time:  8:14p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added translucent rendering as a "magic window" with context in
//surface rendering
//
//*****************  Version 6  *****************
//User: Jeff            Date:  7/21/00  Time:  1:27p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Removed manual drawing of tooltip text (will try to use normal
//methods)
//
//*****************  Version 5  *****************
//User: Jeff            Date:  7/20/00  Time:  7:29p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  
//
//*****************  Version 4  *****************
//User: Jeff            Date:  7/20/00  Time: 11:07a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Overriding the OnOK() method to do nothing (by default it dismisses
//the dialog)
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/18/00  Time:  1:58p
//Branched at version 3
//Comment:
//  
//
//*****************  Version 2  *****************
//User: Jeff            Date:  7/18/00  Time:  1:53p
//Checked in $/v1k/src/Viatron
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created ControlsDialog.cpp
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ControlsDialog.C,v 1.32.2.19 2010/10/20 20:47:33 dongqing Exp $
// $Id: ControlsDialog.C,v 1.32.2.19 2010/10/20 20:47:33 dongqing Exp $
