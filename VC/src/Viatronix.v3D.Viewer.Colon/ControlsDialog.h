// $Id: ControlsDialog.h,v 1.11.2.7 2010/04/14 03:53:21 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey R. Meade  jeff@viatronix.net


// pragmas
#pragma once


// includes
#include "resource.h"
#include "ViatronStudy.h"
#include "ViatronDialog.h"
#include "MenuButton.h"
#include "Submarine.h"
#if OTHER_SLIDER
#include "SliderCtrlEx.h"
#include "MySliderControl.h"
#endif

// namespaces
namespace ColonViews
{

namespace Dialogs
{

/**
 * (description of class)
 */
class ControlsDialog : public ViatronDialog
{
  class SegmentJumpCtrl : public CButton
  {
  public:

    /// Creates segment jump control buttons
    BOOL Create(LPCTSTR sCaption, DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT uID);

    /// Update button icons based on flight direction
    void UpdateFlightDirn(const ReaderLib::Submarine::NavigationDirection eNavDirn);

    /// reposition the window
    void RepositionWindow( const int4 iX, const int4 iY );

  public:

    /// Button variables
    WinUtils::SkinButton m_prevBtn, m_dirBtn, m_nextBtn;

  }; // class SegmentJumpCtrl

/// Methods
public:

  /// standard constructor
  ControlsDialog(CWnd* pParent = NULL, const uint4 uDlgBkgdResourceID = IDB_ENDOVIEW_HORIZ_DLGBAR);

  /// Updates buttons based on current view modes
  void UpdateEndoModes();

private:

  /// Dialog Data
  //{{AFX_DATA(ControlsDialog)
  enum { IDD = IDD_CONTROLS_HORIZONTAL };
  WinUtils::SkinButton	m_movieBtn;
  WinUtils::SkinButton  m_snapshotBtn;
  WinUtils::SkinAutoCheckButton m_autoFlyBtn;
  CButton m_segmentJumpBtn;
  WinUtils::SkinButton  m_flipViewBtn;
  WinUtils::MenuButton m_shadowsBtn;
  WinUtils::MenuButton m_toolsBtn;
  WinUtils::SkinAutoCheckButton m_showPaintingBtn;
  WinUtils::SkinAutoCheckButton m_supineProneBtn;
  CStatic m_sliderValStatic;
#if OTHER_SLIDER
  CSliderCtrlEx /*CMySliderControl*/ m_speedSlider;
#else
  CSliderCtrl m_speedSlider;
#endif
  CButton m_checkPerceivedSpeed;

  WinUtils::SkinAutoRadioButton  m_btnLineMeasure;
  WinUtils::SkinAutoRadioButton  m_btnLighting;
  WinUtils::SkinAutoRadioButton  m_btnTranslucency;
  WinUtils::SkinAutoRadioButton  m_btn2DROI;
  WinUtils::SkinAutoRadioButton  m_btnAnnotation;
  WinUtils::SkinAutoRadioButton  m_btnRegionSegment;
  //}}AFX_DATA

/// Overrides
  /// ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(ControlsDialog)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

/// Implementation
protected:

  /// Enter/OnOK
  virtual void OnOK();

  /// Esc/OnCancel
  virtual void OnCancel();

  void UpdateState();

  /// Generated message map functions
  //{{AFX_MSG(ControlsDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(UINT uType, int iSizeX, int iSizeY);
  afx_msg void OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar);
  afx_msg void OnViewDirectionBtnClicked();
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
  afx_msg void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);
  afx_msg void OnMouseMove(UINT uFlags, CPoint point);
  afx_msg void OnFlipViewBtnClicked();
  afx_msg void OnShowPaintingClicked();
  afx_msg void OnTimer(UINT_PTR uIDEvent);
  afx_msg void OnPrevSegmentBtnClicked();
  afx_msg void OnNextSegmentBtnClicked();
  afx_msg void OnAutoflyClicked();
  afx_msg void OnSnapshotClicked();
  afx_msg void OnMovieToggled();
  afx_msg void OnRegionSegmentClicked();
  afx_msg void OnDestroy();
  afx_msg void OnClickedPerceivedSpeedCheck();
  afx_msg void OnClickedDatasetBtn();
  //}}AFX_MSG

  /// Gets text for help balloons
  virtual BOOL OnToolTipNeedText(UINT uID, NMHDR *pTTTStruct, LRESULT *pResult);

  DECLARE_MESSAGE_MAP()

private:

  /// Set slider value text field
  void SetBuddyData(const int4 iCtrlID = IDC_CTRLS_SPEED_SLIDER);

// enumerations
public:

  enum { IDD_HORIZONTAL = IDD_CONTROLS_HORIZONTAL , IDD_VERTICAL = IDD_CONTROLS_VERTICAL };

private:

  /// Stores current view modes for less frequent updating
  VtxViewModes m_currentVtxViewModes;

  /// Stores current navigation direction for less frequent updating
  ReaderLib::Submarine::NavigationDirection m_eCurrNavDirection;

  /// Stores current dataset orientation
  ReaderLib::DatasetOrientations::TypeEnum m_eCurrOrient;

  /// Used to indicate when flying has *just* stopped or *just* started
  bool m_bWasFlying;

  /// class instance for (3) segment control buttons
  SegmentJumpCtrl m_segJumpCtrl;

  /// Window region
  CRgn m_windRgn;

  /// If this dialog is laid out horizontally
  bool m_bHorizontal;

  /// currently selected radio button for tools
  int4 m_iCurrentToolID;

}; // class ControlsDialog

} // namespace Dialogs

} // namespace ColonViews

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: ControlsDialog.h,v $
// Revision 1.11.2.7  2010/04/14 03:53:21  dongqing
// compile error
//
// Revision 1.11.2.6  2010/04/10 23:52:52  dongqing
// centering controls dialog children on the fly
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.5  2009/12/11 22:32:39  dongqing
// split tools menu button into separate buttons
//
// Revision 1.11.2.4  2009/11/25 22:12:40  dongqing
// moved dataset switch button to controls
//
// Revision 1.11.2.3  2009/11/20 22:04:21  dongqing
// localized all missed regions list functionality to a single control class.
//
// Revision 1.11.2.2  2008/12/31 21:45:37  jmeade
// moved voxel size selection control into the sliceview
//
// Revision 1.11.2.1  2008/12/20 00:39:13  jmeade
// roi voxel edit size.
//
// Revision 1.11  2007/03/08 22:21:49  romy
// code review - header files adjusted
//
// Revision 1.10  2006/12/22 19:48:35  jmeade
// Issue 5160: region segment button should be an autocheck button.
//
// Revision 1.9  2006/10/03 02:04:12  jmeade
// segment region button on controls.
//
// Revision 1.8  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.7  2006/01/31 14:25:37  frank
// code review
//
// Revision 1.6  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5.4.2  2006/02/28 17:28:20  geconomos
// modified for SkinButton changes
//
// Revision 1.5.4.1  2005/12/23 16:49:15  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
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
// Revision 3.12.2.2  2004/08/27 22:04:48  jmeade
// Issue 3538: Option to adjust perceived flight speed accordingly with colon region width/diameter.
//
// Revision 3.12.2.1  2003/03/13 01:14:10  jmeade
// Comments, dead code.
//
// Revision 3.12  2003/01/30 16:57:08  jmeade
// Localized registry storage of all flight params to NavigationView.
//
// Revision 3.11  2003/01/21 19:29:50  jmeade
// Duplicate: Revision : 3.5.2.3: Size lumen coverage list columns appopriately for any header text.
//
// Revision 3.10  2002/12/12 21:22:52  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.9  2002/05/15 19:26:10  jmeade
// Allowing for general use of registry (and not just Software\AppName\...).
//
// Revision 3.8  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.7  2002/04/12 02:10:17  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.6  2002/03/13 22:01:15  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.5.2.2  2002/04/17 23:22:35  jmeade
// Remembers last flight speed setting.
//
// Revision 3.5.2.1  2002/02/19 19:27:41  jmeade
// Code standards.
//
// Revision 3.5  2002/01/09 16:14:59  jmeade
// Movie record button implemented; Coding standards.
//
// Revision 3.4  2002/01/04 18:22:15  jmeade
// Movie record button.
//
// Revision 3.3  2001/11/20 09:49:57  jmeade
// Allow caller to set background image resource; slice options button.
//
// Revision 3.2  2001/11/17 02:55:37  jmeade
// Removed unused function.
//
// Revision 3.1  2001/11/09 00:41:43  jmeade
// Added control bar to registration view; Removed unused strength slider
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
//    Rev 1.0   Sep 16 2001 17:28:02   ingmar
// Initial revision.
// Revision 1.27  2001/08/29 16:48:20  jmeade
// Snapshot button
//
// Revision 1.26  2001/05/10 19:34:38  jmeade
// Coding conventions
//
// Revision 1.25  2001/04/13 02:31:50  jmeade
// headers and footers
//
// Revision 1.24  2001/04/10 17:52:30  frank
// New coding conventions in Submarine class.
//
// Revision 1.23  2001/03/30 22:44:57  jmeade
// derive from ViatronDialog (instead of CDialog) for common skin code
//
// Revision 1.22  2001/02/12 19:14:30  jeff
// only update modes and navigation direction when necessary,
// reduces bmp flashing over constant updating
//
// Revision 1.21  2001/02/02 20:20:21  jeff
// green/red colored segment direction buttons; correct direction for cecum/rectum buttons
//
// Revision 1.20  2001/02/01 00:55:00  jeff
// changed missed region list to a ListCtrl to give it headers and columns
//
// Revision 1.19  2001/01/31 18:16:47  jeff
// function for updating missed region list
//
// Revision 1.18  2001/01/30 04:41:31  kevin
// FIxed the live update of painted information
//
// Revision 1.17  2001/01/25 00:05:22  jeff
// no message
//
// Revision 1.16  2001/01/24 21:04:31  jeff
// autofly gets its own button; show painting is a toggle
//
// Revision 1.15  2001/01/24 15:57:22  jeff
// show painting toggle button
//
// Revision 1.14  2001/01/24 01:58:17  jeff
// removed "update missed regions" button; added "display of missed regions" button
//
// Revision 1.13  2000/12/27 00:36:17  jeff
// Correcting background updates of listbox; rounded window region
//
// Revision 1.12  2000/12/22 23:57:35  jeff
// moved SegmentJumpCtrl to ControlsDialog class (class condensing)
//
// Revision 1.11  2000/12/20 19:24:54  jeff
// OnCtlColor() to paint dialog background
//
// Revision 1.10  2000/12/12 18:00:41  jeff
// SegmentJumpCtrl (moving segment jump from overview dialog);
//  bitmaps for icons
//
// Revision 1.9  2000/12/08 00:52:11  jeff
// skin buttons
//
// Revision 1.8  2000/11/24 17:19:53  jeff
// Made destructor virtual (because class allocates memory)
//
// Revision 1.7  2000/11/24 14:03:55  frank
// Plugged resource leak
//
// Revision 1.6  2000/11/21 04:25:01  jeff
// Removed unused CalcWindowRect() fn
//
// Revision 1.5  2000/11/20 00:52:28  jeff
// OnCtlControl() to custom color dialog
//
// Revision 1.4  2000/10/12 02:49:56  kevin
// Moved Flight Direction to Submarine (Where it
// is better encapsualted)
//
// Revision 1.3  2000/09/26 20:59:34  jeff
// Many changes for new enable popup menu buttons
//
// Revision 1.2  2000/09/22 23:15:42  jeff
// button menus
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 17    9/01/00 12:10p Jeff
// Another 'modes' incremental checkin
// 
// 16    8/29/00 6:42p Jeff
// Mode changes
// 
// 15    8/28/00 2:11p Antonio
// updated header and footer
// 
// *****************  Version 14  *****************
// User: Jeff            Date:  8/25/00  Time: 12:19p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   More view mode toggles (controls dialog)
// 
// *****************  Version 13  *****************
// User: Jeff            Date:  8/24/00  Time:  6:42p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Removed extra static (strength) control, renamed speed static
// control
// 
// *****************  Version 12  *****************
// User: Jeff            Date:  8/24/00  Time:  3:52a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Change View direction (button, etc) now specifies towards rectum or
// cecum
// Added flip/rotate view direction (180*) button
// 
// *****************  Version 11  *****************
// User: Jeff            Date:  8/23/00  Time:  4:55p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Remove VxSliderCtrl (a quick debug thing, wasnt' meant for
// submittal)
// 
// *****************  Version 10  *****************
// User: Jeff            Date:  8/23/00  Time:  4:53p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Moving window showing speed/strength value while updating
// 
// *****************  Version 9  *****************
// User: Jeff            Date:  8/22/00  Time: 11:57p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Initial values of speed and strength sliders read from rdrGlobal
// No longer using two stationary buddy edits for speed/strength value display
//   (will use tooltips or close mockup in near future)
// OnSetCursor() overridden to set mouse cursor for dialog
// WM_KEYDOWN messages sent to parent window
// MouseMove() events set keyboard focus to parent window
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 8  *****************
// User: Jeff            Date:  7/31/00  Time:  6:26p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added view direction (cecum or rectum) button
// 
// *****************  Version 7  *****************
// User: Jeff            Date:  7/27/00  Time:  2:33p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added endoscopic view mode toggles to GUI
// Smoother updating of Overview dialog
// BroadcastSendMessage changed to BroadcastMessage(..., bWait)
// 
// *****************  Version 6  *****************
// User: Jeff            Date:  7/26/00  Time:  9:44p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Implemented callbacks for controls dialog endoscopic mode selection
// 
// *****************  Version 5  *****************
// User: Jeff            Date:  7/21/00  Time:  1:28p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Removed manual drawing of tooltip text (will try to use normal
// methods)
// 
// *****************  Version 4  *****************
// User: Jeff            Date:  7/20/00  Time:  7:29p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  7/20/00  Time: 11:07a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Overriding the OnOK() method to do nothing (by default it dismisses
// the dialog)
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/18/00  Time:  1:58p
// Branched at version 2
// Comment:
//   
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/12/00  Time:  9:14p
// Created ControlsDialog.h
// Comment:
//   
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ControlsDialog.h,v 1.11.2.7 2010/04/14 03:53:21 dongqing Exp $
// $Id: ControlsDialog.h,v 1.11.2.7 2010/04/14 03:53:21 dongqing Exp $
