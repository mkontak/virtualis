// $Id: OverViewDlgBar.C,v 1.12.2.1 2010/04/17 16:00:58 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Control-pad like bar for common OverView controls
// Owner: Jeff Meade jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "MainFrm.h"
//#include "GLChildView.h"
#include "OverViewDlgBar.h"
//#include "WinUtils.h"

//#include "ReaderGlobal.h"
#include "ButtonMenus.h"
//#include "V1KMessages.h"


// defines
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define FILE_REVISION "$Revision: 1.12.2.1 $"


// namespaces
using namespace ViewModes;
using namespace OverviewMenus;
using namespace ColonViews::Dialogs;
using namespace ColonViews::Layouts;
using namespace WinUtils;
using namespace ReaderLib;

/////////////////////////////////////////////////////////////////////////////
// OverViewDlgBar dialog


/**
 * Constructor 
 *
 * @param pParent   Parent window
 */
OverViewDlgBar::OverViewDlgBar(CWnd* pParent /*=NULL*/)
  : ViatronDialog(OverViewDlgBar::IDD, pParent, IDB_OVERVIEW_DLGBAR)
{
  //{{AFX_DATA_INIT(OverViewDlgBar)
  //}}AFX_DATA_INIT

  m_currentModes = 0;

  m_iWinWidth = 0;

  m_iLastWorldLocationTenthOfMM = 0;
} // OverViewDlgBar()


/**
 * Initializes dialog data variables
 *
 * @param pDX   Pointer to a CDataExchange object
 */
void OverViewDlgBar::DoDataExchange(CDataExchange* pDX)
{
  ViatronDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(OverViewDlgBar)
  DDX_Control(pDX, IDC_ODB_LOCATION_DISPLAY, m_locationStatic);
  DDX_Control(pDX, IDC_ODB_TOGGLE_OVERVIEW_MARKING, m_toggleMarkingBtn);
  DDX_Control(pDX, IDC_ODB_SHOWHIDE, m_showHideBtn);
  DDX_Control(pDX, IDC_ODB_RESET_OVERVIEW, m_resetBtn);
  DDX_Control(pDX, IDC_ODB_TRANSLUCENT, m_translucentBtn);
  DDX_Control(pDX, IDC_ODB_SLICE_INDICATORS, m_sliceIndBtn);
  //}}AFX_DATA_MAP
} // DoDataExchange()


BEGIN_MESSAGE_MAP(OverViewDlgBar, ViatronDialog)
  //{{AFX_MSG_MAP(OverViewDlgBar)
  ON_WM_MOUSEMOVE()
  ON_BN_CLICKED(IDC_ODB_RESET_OVERVIEW, OnResetOverview)
  ON_WM_SETCURSOR()
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_ODB_SHOWHIDE, OnAnimateShowHide)
  ON_WM_SIZE()
  ON_BN_CLICKED(IDC_ODB_TOGGLE_OVERVIEW_MARKING, OnToggleOverviewMarking)
  ON_BN_CLICKED(IDC_ODB_TRANSLUCENT, OnToggleTranslucent)
  ON_WM_SHOWWINDOW()
  //}}AFX_MSG_MAP
  ON_MESSAGE(WM_UPDATE_LOCATION, UpdateWorldLocation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OverViewDlgBar message handlers


/**
 * Handles dialog initialization
 *
 * @return   FALSE if you set focus to a control, TRUE otherwise
 */
BOOL OverViewDlgBar::OnInitDialog() 
{
  ViatronDialog::OnInitDialog();

  const int4 uBtns(4);
  const UINT vuBtnsNicons[uBtns][2] =
  {
    { IDC_ODB_SLICE_INDICATORS, IDB_SLICEOPTIONS }, { IDC_ODB_TRANSLUCENT, IDB_TRANSLUCENT_OVERVIEW },
    { IDC_ODB_RESET_OVERVIEW, IDB_RESET_OVERVIEW }, { IDC_ODB_TOGGLE_OVERVIEW_MARKING, IDB_OVERVIEW_MARKING }
  };
  SetButtonIcons(this, vuBtnsNicons, uBtns);

  EnableToolTips();

  UpdateWorldLocation(0,0);
  m_updateModesTimer.SetTimer(this, 1000);

  m_sliceIndBtn.AddMenuItems(IDM_OVERVIEW_INDICATORS, 0);

  UpdateModeButtons();
  m_currentModes = ViatronStudy::m_modeOverView;

  m_locationStatic.SetWindowText(CString());
  m_locationStatic.ShowWindow(SW_HIDE);
//  m_locationStatic.SetParent(GetParentOwner());

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
} // OnInitDialog()


/**
 * Mouse move event handler
 *
 * @param uFlags   Indicates whether various virtual keys are down
 * @param point    Location of the mouse cursor
 */
void OverViewDlgBar::OnMouseMove(UINT uFlags, CPoint point)
{
  GetParentOwner()->SendMessage(WM_MOUSEMOVE, uFlags, MAKELPARAM(point.x, point.y));
  ViatronDialog::OnMouseMove(uFlags, point);
} // OnMouseMove()


/**
 * Called when the window is shown or hidden.
 *
 * @param bShow     TRUE if window is being shown, FALSE if hidden.
 * @param uStatus   Status of window.  SW_PARENTCLOSING, SW_PARENTOPENING,
 *                        or 0 (ShowWindow function triggered the message).
 */
void OverViewDlgBar::OnShowWindow(BOOL bShow, UINT uStatus) 
{
  ViatronDialog::OnShowWindow(bShow, uStatus);
} // OnShowWindow()


/**
 * Menu command handler
 *
 * @param wParam   Specifies menu command
 * @param lParam   Unused
 * @return         TRUE if command is handled
 */
BOOL OverViewDlgBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  ViewModes::VIEWMODES eMode(ViewModes::VIEWMODES(0));

  switch (LOWORD(wParam))
  {
    case ID_OVERVIEW_OPTIONS_CENTERLINE:
      eMode = CENTERLINE;
      break;
    case ID_OVERVIEW_OPTIONS_TRANSPARENT:
      eMode = TRANSLUCENT;
      break;
    case ID_OVERVIEW_SLICEINDICATORS_AXIAL:
      eMode = AXIAL;
      break;
    case ID_OVERVIEW_SLICEINDICATORS_SAGITTAL:
      eMode = SAGITTAL;
      break;
    case ID_OVERVIEW_SLICEINDICATORS_CORONAL:
      eMode = CORONAL;
      break;
    case ID_OVERVIEW_SLICEINDICATORS_CROSSSECTIONAL:
      eMode = CROSS_SECTION;
      break;
    default:
      return ViatronDialog::OnCommand(wParam, lParam);
  }

  if (HIWORD(wParam))
  {
    ViatronStudy::m_modeOverView += eMode;
  }
  else
  {
    ViatronStudy::m_modeOverView -= eMode;
  }

  return FALSE;
} // OnCommand()


/**
 * On OK handler, overridden to keep user from dismissing dialog
 */
void OverViewDlgBar::OnOK()
{
  // By default, the CDialog::OnOK() base class method will close
  //  the dialog, when, for example, the user hits the enter key
  // So, we override this and do nothing

  return;
}// OnOK()


/**
 * On Cancel handler, overridden to keep user from dismissing dialog
 */
void OverViewDlgBar::OnCancel()
{
  // By default, the CDialog::OnCancel() base class method will close
  //  the dialog, when, for example, the user hits the ESC-key
  // So, we override this and do nothing

  return;
} // OnCancel()


/**
 * Sets cursor for the dialog
 *
 * @param CWnd*   Unused
 * @param UINT    Unused
 * @param UINT    Unused
 * @return        TRUE, indicating cursor was set
 */
BOOL OverViewDlgBar::OnSetCursor(CWnd*, UINT, UINT) 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  return TRUE;
} // OnSetCursor()


/**
 * Timer event handler
 *
 * @param uIDEvent   ID of timer event
 */
void OverViewDlgBar::OnTimer(UINT_PTR uIDEvent)
{
  // Update mode buttons
  if (uIDEvent == m_updateModesTimer) 
  {
    if (m_currentModes != ViatronStudy::m_modeOverView)
    {
      UpdateModeButtons();
      m_currentModes = ViatronStudy::m_modeOverView;
      // update overview right now.
      const long UPDATE_OVERVIEW = 1;
      const long RERENDER_OVERVIEW = 0;
      MainAppFrame::GetMainFrame()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_OVERVIEW), 0);
      MainAppFrame::GetMainFrame()->SendMessage(WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));
    }
  }
  else if (uIDEvent == m_showHideAnimationTimer)
  {
    OnAnimateShowHide();
  }

  // obsolete: text field for distance from rectum display no longer being used
  if ( 0 )
  {
    const int4 iCurrWorldLocTenthOfMM(int4(rdrGlobal.m_pCurrDataset->GetCurrentDistanceFromRectumMM()*10));
    if (iCurrWorldLocTenthOfMM != m_iLastWorldLocationTenthOfMM)
    {
      UpdateWorldLocation(0,0);
      m_iLastWorldLocationTenthOfMM = iCurrWorldLocTenthOfMM;
    }
  }

  ViatronDialog::OnTimer(uIDEvent);
} // OnTimer()


/**
 * Updates text display of world location
 *
 * @param WPARAM    Unused
 * @param LPARAM    Unused
 * @return          Zero
 */
LRESULT OverViewDlgBar::UpdateWorldLocation(WPARAM, LPARAM)
{
  CWnd * pWnd = GetParent()->GetDlgItem(IDC_TEMPLATE_DISTFROMRECTUM_DISPLAY);
  if (!pWnd)
  {
    return 0;
  }

  char sTmpStr[101];
  sprintf(sTmpStr, "%.1f", rdrGlobal.m_pCurrDataset->GetCurrentDistanceFromRectumMM() / 10);
  pWnd->SetWindowText(sTmpStr);
  return 0;
} // UpdateWorldLocation()


/**
 * Window resize handler
 *
 * @param uType   Type of resize event
 * @param iCX     New width of window
 * @param iCY     New height of window
 */
void OverViewDlgBar::OnSize(UINT uType, int iCX, int iCY) 
{
  ViatronDialog::OnSize(uType, iCX, iCY);

  // TODO: Add your message handler code here
  if (m_iWinWidth <= 0)
  {
    m_iWinWidth = iCX;
  }

  // Ensure it is safe to access dialog controls, as the OnSize() method is called
  //  once prior to the handling of WM_INITDIALOG, which causes bad things to happen
  if (!IsWindow(m_resetBtn.GetSafeHwnd()))
  {
    return;
  }

  ResizeWindow( iCX, iCY );
} // OnSize()


/**
 * Resizes the contents of the dialog
 *
 * @param iWidth    Width of dialog
 * @param iHeight   Height of dialog
 */
void OverViewDlgBar::ResizeWindow( const int4 iWidth, const int4 iHeight )
{

  // ---------------------------------------------
  // Evenly space the buttons across in the dialog
  // ---------------------------------------------
  const int4 iNumButtons = 4;
  CRect buttonRect = WinUtils::GetWindowRect( m_resetBtn );
  const int4 iButtonWidth = buttonRect.Width();
  const int4 iButtonHeight = buttonRect.Height();
  const int4 iGapHorizontal = max( 0, ( iWidth - iNumButtons * iButtonWidth ) / ( iNumButtons + 1 ) );
  const int4 iGapVertical = max( 0, ( iHeight - iButtonHeight ) / 2 );

  // lay out the buttons
  const int4 iButtonInterval = iButtonWidth + iGapHorizontal;
  const int4 iButtonX1 = iGapHorizontal;
  const int4 iButtonX2 = iButtonX1 + iButtonWidth + iGapHorizontal;
  const int4 iButtonX3 = iButtonX2 + iButtonWidth + iGapHorizontal;
  const int4 iButtonX4 = iButtonX3 + iButtonWidth + iGapHorizontal;
  m_resetBtn        .SetWindowPos( NULL, iButtonX1, iGapVertical, iButtonWidth, iButtonHeight, SWP_NOZORDER );
  m_sliceIndBtn     .SetWindowPos( NULL, iButtonX2, iGapVertical, iButtonWidth, iButtonHeight, SWP_NOZORDER );
  m_translucentBtn  .SetWindowPos( NULL, iButtonX3, iGapVertical, iButtonWidth, iButtonHeight, SWP_NOZORDER );
  m_toggleMarkingBtn.SetWindowPos( NULL, iButtonX4, iGapVertical, iButtonWidth, iButtonHeight, SWP_NOZORDER );
  
} // ResizeWindow()



/**
 * Updates indicators of current modes
 */
void OverViewDlgBar::UpdateModeButtons()
{
  m_translucentBtn.SetCheck((ViatronStudy::m_modeOverView & TRANSLUCENT) ? 1 : 0);

  const bool vbSliceStates[OverviewMenus::iOverviewSlices] =
  {
    (ViatronStudy::m_modeOverView & CENTERLINE), (ViatronStudy::m_modeOverView & AXIAL),
    (ViatronStudy::m_modeOverView & SAGITTAL), (ViatronStudy::m_modeOverView & CORONAL),
    (ViatronStudy::m_modeOverView & CROSS_SECTION)
  };
  m_sliceIndBtn.CheckMenuItems(OverviewMenus::viOverviewSliceIds, vbSliceStates, OverviewMenus::iOverviewSlices);

  m_toggleMarkingBtn.SetCheck((ViatronStudy::m_modeOverView & MISSED_REGIONS) ? 1 : 0);
} // UpdateModeButtons()


/////////////////////////////////////////////////////////////////////////////
// OverViewDlgBar button handlers


/**
 * Animates showing or hiding the dialog (Unused)
 */
void OverViewDlgBar::OnAnimateShowHide()
{
  CRect rect;  GetWindowRect(rect);
  GetParent()->ScreenToClient(rect);

  CRect btnRect;  m_showHideBtn.GetWindowRect(btnRect);
  ScreenToClient(btnRect);

  const int hiddenWidth = 25;

  if (rect.Width() == hiddenWidth)
  {
    rect.right = rect.left + m_iWinWidth;
  }
  else
  {
    rect.right = rect.left + hiddenWidth;
  }

  MoveWindow(rect);
} // OnAnimateShowHide()


/**
 * Called when Reset button clicked
 */
void OverViewDlgBar::OnResetOverview()
{
  MainAppFrame::GetMainFrame()
    ->BroadcastMessage(GLChildView::eGVT_OVERVIEW, WM_COMMAND, ID_OVERVIEW_RESETDISPLAY, 0 );
} // OnResetOverview()


/**
 * Called when Translucency toggle button clicked
 */
void OverViewDlgBar::OnToggleTranslucent() 
{
  if (m_translucentBtn.GetCheck() == 1)
  {
    ViatronStudy::m_modeOverView += TRANSLUCENT;
  }
  else
  {
    ViatronStudy::m_modeOverView -= TRANSLUCENT;
  }

  const long UPDATE_OVERVIEW = 1;
  const long RERENDER_OVERVIEW = 0;
  MainAppFrame::GetMainFrame()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_OVERVIEW), 0);
  MainAppFrame::GetMainFrame()->SendMessage(WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));

} // OnToggleTranslucent() 


/**
 * Called when Overview painting toggle button clicked
 */
void OverViewDlgBar::OnToggleOverviewMarking()
{
  if (m_toggleMarkingBtn.GetCheck() == 1)
  {
    ViatronStudy::m_modeOverView += MISSED_REGIONS;
  }
  else
  {
    ViatronStudy::m_modeOverView -= MISSED_REGIONS;
  }

  const long UPDATE_OVERVIEW = 1;
  const long RERENDER_OVERVIEW = 0;
  MainAppFrame::GetMainFrame()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_OVERVIEW), 0);
  MainAppFrame::GetMainFrame()->SendMessage(WM_APP_2DAUTOFLIGHT_UPDATE, WPARAM(0), MAKELPARAM(RERENDER_OVERVIEW,UPDATE_OVERVIEW));
} // OnToggleOverviewMarking()


// undefines
#undef FILE_REVISION


// $Log: OverViewDlgBar.C,v $
// Revision 1.12.2.1  2010/04/17 16:00:58  dongqing
// text field used to display distance to rectum has long since been removed
//
// Revision 1.12  2007/03/10 06:14:31  jmeade
// code standards.
//
// Revision 1.11  2007/03/08 22:22:12  romy
// code review - header files adjusted
//
// Revision 1.10  2007/02/02 21:10:31  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.9  2006/10/03 17:16:21  jmeade
// variable names.
//
// Revision 1.8  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.7.2.1  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.7  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.6  2005/10/04 18:07:17  jmeade
// no separate distance-to-rectum text field, text only in overview.
//
// Revision 1.5  2005/09/16 15:40:56  jmeade
// Issue 4430: Let mouse-clicks and mouse dialog-leave-event hide popup (instead of mouse move)
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
// Revision 3.12.2.4.4.2.2.2  2005/07/06 19:45:52  geconomos
// commented out RedrawWindow call when showing/hiding the overview dialog ( let's see what happens )
//
// Revision 3.12.2.4.4.2.2.1  2005/06/27 17:39:10  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.12.2.4.4.2  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.12.2.4.4.1.2.2  2005/06/08 14:01:10  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.12.2.4.4.1.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.12.2.4.4.1  2005/04/01 16:35:42  frank
// Issue #4096: Fixed update issues on changing modes
//
// Revision 3.12.2.4  2003/02/27 23:14:44  jmeade
// Put common tooltip text get method in base class for use by all derivatives.
//
// Revision 3.12.2.3  2003/02/27 02:22:55  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.12.2.2  2003/02/22 11:58:20  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.12.2.1  2003/02/21 17:54:34  jmeade
// Removed an essential identical app message.
//
// Revision 3.12  2002/12/12 22:06:40  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.11  2002/08/27 18:05:44  jmeade
// Unhandled control color message.
//
// Revision 3.10  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.9  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.8  2002/04/18 18:27:53  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.7  2002/04/12 02:13:25  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.6.2.2  2002/06/18 22:04:37  jmeade
// Issue 2454:  Only SetFocus() when no known processes, spawned by Viewer, as running.
//
// Revision 3.6.2.1  2002/05/13 17:29:11  jmeade
// CMenuButton reimplemented, better setup receives CMenu resource ID.
//
// Revision 3.6  2001/11/23 15:11:10  jmeade
// Show/Hide distance from rectum display when dialog visibility changes as appropriate.
//
// Revision 3.5  2001/11/21 01:38:37  jmeade
// Distance from rectum.
//
// Revision 3.4  2001/11/21 01:20:06  jmeade
// Distance from rectum.
//
// Revision 3.3  2001/11/20 09:50:11  jmeade
// Slice options button.
//
// Revision 3.2  2001/11/19 16:03:33  jmeade
// Standard view form.
//
// Revision 3.1  2001/11/17 03:34:18  jmeade
// Derive from ViatronDialog to handle looks (threw out the old carpet).
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 12 2001 12:46:56   jmeade
// Only need to reset distance text (and not entire string); Coding standardsResolution for 1042: suggestion
// 
//    Rev 2.0   Sep 16 2001 23:41:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:24   ingmar
// Initial revision.
// Revision 1.42  2001/06/26 14:44:41  binli
// rewrote the control logic to rerender overview: made memory stable.
//
// Revision 1.41  2001/05/08 19:17:58  jmeade
// User doesn't need framebox menu
//
// Revision 1.40  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.39  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.38  2001/03/30 22:46:38  jmeade
// codings standards; tabs to spaces
//
// Revision 1.37  2001/03/28 22:25:23  jmeade
// evenly space the (four) buttons across the dialog
//
// Revision 1.36  2001/03/01 16:47:31  jmeade
// send WM_APP_GRAB_OVERVIEW message when appropriate
//
// Revision 1.35  2001/02/27 17:19:26  jmeade
// member variable for world locn
//
// Revision 1.34  2001/02/27 17:17:06  jmeade
// Fix, Bug 000278:  Periodically update world location if necessary
//
// Revision 1.33  2001/02/07 21:05:22  jeff
// message/function to update location
//
// Revision 1.32  2001/02/07 19:34:26  jeff
// update location using timer when flying
//
// Revision 1.31  2001/02/06 22:41:32  jeff
// view location text display (needs to be refreshed more frequently)
//
// Revision 1.30  2001/01/30 02:14:11  jeff
// changed PICKINFO and MARKING mode to MISSED_REGIONS;
// translucent button is an auto checkbox
//
// Revision 1.29  2001/01/27 02:57:13  kevin
// Hooked up the display paintinfo to the overview.
// (note there are still some bugs with the grab image,
// but I can't seemm to get it to connect from readerlib
// into viatron. ARGGHH I am a coding dinosaur)
//
// Revision 1.28  2001/01/24 21:05:13  jeff
// button for toggling overview marking
//
// Revision 1.27  2001/01/24 01:58:17  jeff
// removed "update missed regions" button; added "display of missed regions" button
//
// Revision 1.26  2001/01/23 17:59:40  jeff
// enums should be capitalized
//
// Revision 1.25  2001/01/09 15:21:02  frank
// Cleaned up study orientations in slice view.
//
// Revision 1.24  2001/01/09 13:29:55  frank
// Removed extra toggle.
//
// Revision 1.23  2001/01/08 18:43:42  frank
// Clarified Supine-Prone dataset distinction.
//
// Revision 1.22  2000/12/27 23:47:22  jeff
// global GetViatronMainFrame() ==> static MainAppFrame::GetMainFrame()
//
// Revision 1.21  2000/12/12 18:07:25  jeff
// Removed segment jump buttons; added show/hide bar button (but will this stay?)
//
// Revision 1.20  2000/12/08 00:52:33  jeff
// check run-time type before casting buttons in SetButtonIcons
//
// Revision 1.19  2000/12/07 22:22:55  jeff
// skin buttons
//
// Revision 1.18  2000/11/25 05:41:48  kevin
// CHange to make 'V' not change supine/prone
// toggle m_selected properly
//
// Revision 1.17  2000/11/25 02:21:12  jeff
// Fun with icons and buttons!
//
// Revision 1.16  2000/11/25 03:05:21  kevin
// Fixed supine/prone button
//
// Revision 1.15  2000/11/24 17:18:52  jeff
// Fixed code for setting supine/prone button icons
//
// Revision 1.14  2000/11/24 14:04:21  frank
// Plugged resource leak
//
// Revision 1.13  2000/11/23 08:16:13  jeff
// Supine/prone bitmap icons
//
// Revision 1.12  2000/11/23 03:02:07  jeff
// Moved around a couple of menubuttons/menubutton items
//
// Revision 1.11  2000/11/22 19:22:35  jeff
// New bitmaps for the arrows (thanks Frank!)
//
// Revision 1.10  2000/11/22 17:30:27  jeff
// Getting the precise color at a window point to determine
//  background color for child controls, dialogs, etc.
//
// Revision 1.9  2000/11/20 17:28:17  jeff
// Setting (static) colors closer to dialog's color
//
// Revision 1.8  2000/11/20 01:08:16  jeff
// OnCtlControl() to custom color dialog
//
// Revision 1.7  2000/11/14 20:21:44  binli
// added switch rendering of Supine/Prone
//
// Revision 1.6  2000/11/14 20:07:27  binli
// added Supine/Prone switch rendering button
//
// Revision 1.5  2000/10/09 23:43:16  kevin
// Added Cneterlin Icon to overview Dialog buttons
//
// Revision 1.4  2000/09/26 20:58:45  jeff
// CheckMenuItem() and CheckRadioItem() functions instead of ModifyMenuItem()
//
// Revision 1.3  2000/09/22 23:15:42  jeff
// button menus
//
// Revision 1.2  2000/09/20 21:53:23  jeff
// Adding menu buttons
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 9     8/31/00 9:09a Jeff
// Incremental checkin (for view mode changes)
// 
// 8     8/30/00 6:32p Jeff
// One standardarized enum value class for all mode windows
// 
// 7     8/29/00 4:15p Jeff
// Overview modes now stored in ViatronStudy
// 
// 6     8/28/00 1:45p Antonio
// updated header and footer
// 
// *****************  Version 5  *****************
// User: Jeff            Date:  8/25/00  Time: 12:30p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Arrow cursors over dialog "bars"
// 
// *****************  Version 4  *****************
// User: Jeff            Date:  8/24/00  Time:  9:40p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Icons for bar buttons
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  8/24/00  Time:  7:22p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Using SetButtonIcons function
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  8/24/00  Time:  8:20a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   First good cut at OverView shortcut command dialog (bar)
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/24/00  Time:  6:28a
// Created OverViewDlgBar.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/OverViewDlgBar.C,v 1.12.2.1 2010/04/17 16:00:58 dongqing Exp $
// $Id: OverViewDlgBar.C,v 1.12.2.1 2010/04/17 16:00:58 dongqing Exp $
