// $Id: OverViewDlgBar.h,v 1.7 2007/03/10 06:14:31 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade (jmeade@viatronix.com)


// pragmas
#pragma once


// includes
//#include "ViatronDialog.h"
//#include "ViatronStudy.h"
#include "resource.h"
//#include "MenuButton.h"
//#include "SkinButton.h"
//#include "WmTimer.h"


// namespaces
namespace ColonViews
{

  namespace Dialogs
  {
    /// controls/options for overview
    class OverViewDlgBar : public ViatronDialog
    {
    public:

      /// standard constructor
      OverViewDlgBar(CWnd* pParent = NULL);

      /// Default destructor
      /// Create the dialog
      BOOL Create(CWnd *pParentWnd = NULL)  { return CDialog::Create(IDD, pParentWnd); }

    private:
      // Dialog Data
      //{{AFX_DATA(OverViewDlgBar)
      enum { IDD = IDDB_OVERVIEW };
      /// 3d location
      CStatic m_locationStatic;
      /// missed region markings
      WinUtils::SkinAutoCheckButton m_toggleMarkingBtn;
      /// show/hide dialog
      WinUtils::SkinButton  m_showHideBtn;
      /// reset view
      WinUtils::SkinButton  m_resetBtn;
      /// translucent rendering
      WinUtils::SkinAutoCheckButton m_translucentBtn;
      /// slice position indicators
      WinUtils::MenuButton m_sliceIndBtn;
      //}}AFX_DATA

    protected:

      /// OnOK override
      virtual void OnOK();

      /// OnCancel override
      virtual void OnCancel();

      /// reflects current modes for all buttons
      void UpdateModeButtons();

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(OverViewDlgBar)
    protected:
      /// DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      /// command msg handler
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      //}}AFX_VIRTUAL

      // Implementation
    protected:

      // Generated message map functions
      //{{AFX_MSG(OverViewDlgBar)
      /// init
      virtual BOOL OnInitDialog();
      /// mouse move
      afx_msg void OnMouseMove(UINT uFlags, CPoint point);
      /// reset overview
      afx_msg void OnResetOverview();
      /// set cursor
      afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
      /// timer event
      afx_msg void OnTimer(UINT_PTR uIDEvent);
      /// show/hide dialog animation
      afx_msg void OnAnimateShowHide();
      /// resize
      afx_msg void OnSize(UINT uType, int iCX, int iCY);
      /// missed regions
      afx_msg void OnToggleOverviewMarking();
      /// transparent view
      afx_msg void OnToggleTranslucent();
      /// show window handler
      afx_msg void OnShowWindow(BOOL bShow, UINT uStatus);
      /// window moving
      afx_msg void OnWindowPosChanging( WINDOWPOS * lpwndpos );
      //}}AFX_MSG

      /// Message to update world location display
      LRESULT UpdateWorldLocation(WPARAM, LPARAM);
      DECLARE_MESSAGE_MAP()

    public:

      /// Update location private message
      enum { WM_UPDATE_LOCATION = WM_APP };

      /// Dialog bar ID
      enum { IDDB = IDDB_OVERVIEW };

      /// moves the window to the specified location
      void ResizeWindow( const int4 iWidth, const int4 iHeight );

    private:

      /// Current view modes
      ViewModesValue m_currentModes;

      /// Update modes timer
      WinUtils::WmTimer m_updateModesTimer;

      /// Show/hide dialog animation timer
      WinUtils::WmTimer m_showHideAnimationTimer;

      /// Update global location timer
      WinUtils::WmTimer m_updateGlobalLocationTimer;

      /// Stores last displayed world location
      int4 m_iLastWorldLocationTenthOfMM;

      /// width of window after receipt of first WM_SIZE event
      int4 m_iWinWidth;

    }; // class OverViewDlgBar

  } // namespace Dialogs

} // namespace ColonViews


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: OverViewDlgBar.h,v $
// Revision 1.7  2007/03/10 06:14:31  jmeade
// code standards.
//
// Revision 1.6  2007/03/08 22:22:12  romy
// code review - header files adjusted
//
// Revision 1.5  2006/01/31 14:27:03  frank
// code review
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3.4.1  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.3.4.1.12.1  2005/06/27 17:39:10  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.3.4.1  2003/02/27 23:14:44  jmeade
// Put common tooltip text get method in base class for use by all derivatives.
//
// Revision 3.3  2002/08/27 18:05:44  jmeade
// Unhandled control color message.
//
// Revision 3.2  2001/11/23 15:11:10  jmeade
// Show/Hide distance from rectum display when dialog visibility changes as appropriate.
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
//    Rev 2.1   Oct 12 2001 12:42:52   jmeade
// Coding standards
// 
//    Rev 2.0   Sep 16 2001 23:41:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:24   ingmar
// Initial revision.
// Revision 1.19  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.18  2001/02/27 17:19:26  jmeade
// member variable for world locn
//
// Revision 1.17  2001/02/07 21:05:22  jeff
// message/function to update location
//
// Revision 1.16  2001/02/07 19:34:32  jeff
// update location using timer when flying
//
// Revision 1.15  2001/02/06 22:41:32  jeff
// view location text display (needs to be refreshed more frequently)
//
// Revision 1.14  2001/01/30 02:14:11  jeff
// changed PICKINFO and MARKING mode to MISSED_REGIONS;
// translucent button is an auto checkbox
//
// Revision 1.13  2001/01/24 21:05:13  jeff
// button for toggling overview marking
//
// Revision 1.12  2001/01/24 01:58:17  jeff
// removed "update missed regions" button; added "display of missed regions" button
//
// Revision 1.11  2001/01/09 13:29:55  frank
// Removed extra toggle.
//
// Revision 1.10  2000/12/12 18:07:25  jeff
// Removed segment jump buttons; added show/hide bar button (but will this stay?)
//
// Revision 1.9  2000/12/07 22:22:55  jeff
// skin buttons
//
// Revision 1.8  2000/11/25 03:05:21  kevin
// Fixed supine/prone button
//
// Revision 1.7  2000/11/24 17:15:11  jeff
// Made destructor virtual (because class allocates memory)
//
// Revision 1.6  2000/11/24 14:04:21  frank
// Plugged resource leak
//
// Revision 1.5  2000/11/23 08:16:13  jeff
// Supine/prone bitmap icons
//
// Revision 1.4  2000/11/20 01:08:17  jeff
// OnCtlControl() to custom color dialog
//
// Revision 1.3  2000/11/14 20:21:44  binli
// added switch rendering of Supine/Prone
//
// Revision 1.2  2000/09/20 21:53:23  jeff
// Adding menu buttons
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 7     9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 6     8/31/00 9:09a Jeff
// Incremental checkin (for view mode changes)
// 
// 5     8/29/00 4:15p Jeff
// Overview modes now stored in ViatronStudy
// 
// 4     8/28/00 2:36p Antonio
// updated header and footer
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  8/25/00  Time: 12:30p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Arrow cursors over dialog "bars"
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  8/24/00  Time:  8:20a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   First good cut at OverView shortcut command dialog (bar)
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/24/00  Time:  6:28a
// Created OverViewDlgBar.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/OverViewDlgBar.h,v 1.7 2007/03/10 06:14:31 jmeade Exp $
// $Id: OverViewDlgBar.h,v 1.7 2007/03/10 06:14:31 jmeade Exp $
