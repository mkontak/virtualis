// $Id: VerifyView.h,v 1.11.2.6 2010/04/17 05:16:34 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

#if !defined(AFX_VERIFYCOLONWND_H__09EBDE59_94B5_11D4_B6E9_00D0B7BF4092__INCLUDED_)
#define AFX_VERIFYCOLONWND_H__09EBDE59_94B5_11D4_B6E9_00D0B7BF4092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ViatronViewWnd.h"
#include "SkinButton.h"
#include "ChildViewFrame.h"
#include "SupineProneViewWnd.h"

namespace ColonViews
{

  namespace Layouts
  {

    /**
     * Overview verification stage
     */
    class VerifyLayoutFrame : public ChildLayoutFrame
    {
    public:
      /// Constructor.
      VerifyLayoutFrame();

    private:
      DECLARE_DYNCREATE(VerifyLayoutFrame)
    };


    /**
     * Slice verification stage
     */
    class VerifySliceLayoutFrame : public ChildLayoutFrame
    {
    public:
      /// Constructor.
      VerifySliceLayoutFrame();

    private:
      DECLARE_DYNCREATE(VerifySliceLayoutFrame)
    };


    /**
     * Slice verification view
     */
    class VerifySliceLayout : public SupineProneLayout
    {
    public:
      /// Constructor.
      VerifySliceLayout();

    protected:
      /// Call constructors for views
      void CreateChildViews(HWND hWndThis);

      // Message handlers
      //{{AFX_MSG(VerifySliceWnd)
      /// update during centerline flight
      afx_msg virtual void OnUpdateViewWindowFlightAlongCenterline(CCmdUI* pCmdUI);
      /// update after changing flight dir
      afx_msg virtual void OnUpdateViewWindowSwitchFlightDirection(CCmdUI* pCmdUI);
      /// update for arrow in center
      afx_msg virtual void OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI);
      /// update nav view
      afx_msg virtual void OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI);
      /// snapshot
      afx_msg void OnReportAddSnapshot();
      /// autofly
      afx_msg void OnToggleAutofly();
      /// segment jump
      afx_msg void OnJumpPrevSegment();
      /// segment jump
      afx_msg void OnJumpNextSegment();
      /// missed region jump
      afx_msg void OnNextMissedRegion();
      /// add bookmark
      afx_msg void OnBookmarksAddBookmark();
      /// toggle lighting
      afx_msg void OnToggleLighting();
      /// add annotation
      afx_msg void OnToggleAddAnnotation();
      /// toggle view dir
      afx_msg void OnToggleViewDirection();
      /// show next view
      afx_msg void OnShowNextView();
      /// toggle report complete
      afx_msg void OnToggleReportCompleteMark();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return (ReaderLib::rdrGlobal.m_bDualDataDetected ? IDT_DUALORTHOGONAL_LANDSCAPE : IDT_SINGLEORTHOGONAL_LANDSCAPE); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return (ReaderLib::rdrGlobal.m_bDualDataDetected ? IDT_DUAL_VERIFYORTHOGONAL_WIDESCREEN : IDT_SINGLEORTHOGONAL_WIDESCREEN); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return (ReaderLib::rdrGlobal.m_bDualDataDetected ? IDT_DUALORTHOGONAL : IDT_SINGLEORTHOGONAL); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID()   { return (ReaderLib::rdrGlobal.m_bDualDataDetected ? IDT_DUALORTHOGONAL_PORTRAIT_LARGE : IDT_SINGLEORTHOGONAL_PORTRAIT_LARGE); }

      /// Filter window messages before they are dispatched to the Windows functions
      virtual BOOL PreTranslateMessage( MSG * pMsg );
    }; // VerifySliceLayout


    /**
     * Overview verification view
     */
    class VerifyLayout : public ColonViatronixWnd
    {
    public:
      /// Constructor.
      VerifyLayout();

    protected:
      /// Call constructors for views
      void CreateChildViews(HWND hWndThis);
      /// Notifies view of study load progress.
      virtual void OnStudyLoadStepNotify(const ReaderLib::StudyLoad::StepsEnum eCompletedStep,
        const ReaderLib::StudyLoad::StepsEnum eCurrentStep, const ReaderLib::DatasetOrientations::TypeEnum eOrientation);

      /// Validate the popup menu to be shown.
      virtual uint4 OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const;

      // Overrides
    protected:
      /// command message handler
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      /// menu command handler for centerline-flight option
      afx_msg virtual void OnUpdateViewWindowFlightAlongCenterline(CCmdUI* pCmdUI);
      /// menu command handler for flight-direction toggle
      afx_msg virtual void OnUpdateViewWindowSwitchFlightDirection(CCmdUI* pCmdUI);
      /// menu command handler for arrow in slice center option
      afx_msg virtual void OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI);
      /// menu command handler for 2d-flight update of navigation view
      afx_msg virtual void OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI);
      /// menu command update handler for scrollbar orientation change
      afx_msg virtual void OnUpdateToggleScrollbarOrientation(CCmdUI* pCmdUI);

      // message handlers
      DECLARE_MESSAGE_MAP()

      /// window destroy message
      afx_msg void OnDestroy();
      /// mouse move handler
      afx_msg virtual void OnMouseMove( UINT uFlags, CPoint point );
      /// resize message handler
      afx_msg virtual void OnSize(UINT uType, int cx, int cy);
      /// Filter window messages before they are dispatched to the Windows functions
      virtual BOOL PreTranslateMessage( MSG * pMsg );
      /// idle time processing
      virtual void GLIdle();

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDT_VERIFICATION_LANDSCAPE; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return IDT_VERIFICATION_WIDESCREEN; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDT_VERIFICATION; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return IDT_VERIFICATION_PORTRAIT_LARGE; }

    private:
      /// Sets text prompts and button states as appropriate for the current state/stage of verification.
      void SetSPVerifyUIState();
      /// Background function to run study initialization.
      static UINT StudyInitializeVisualizationProc(LPVOID pParam);
      /// Handles 'back' button
      void OnVerifyBack(const bool bSupine);
      /// Handles 'next' button
      void OnVerifyNext();
      /// Handles 'finish' button
      void OnVerifyFinish();

      /// Sets the label text properly
      void SetLabelText();

    private:
      /// Whether to move directly to verification after study load completes.
      bool m_bFinishOnLoadCompletion;
      /// Whether initialization is currently being done.
      bool m_bInitializingVisualization;
      /// Whether registration has ever been done.
      bool m_bRegistrationRunFirstTime;
      /// Records whether the segment order has been changed at all throughout the session
      static bool m_bSegmentOrderChanged;
      /// Whether or not to update the window's controls
      bool m_bUpdateUI;

    }; // class VerifyLayout

  } // namespace Layouts

} // namespace ColonViews

#endif // !defined(AFX_VERIFYCOLONWND_H__09EBDE59_94B5_11D4_B6E9_00D0B7BF4092__INCLUDED_)

// Revision History:
// $Log: VerifyView.h,v $
// Revision 1.11.2.6  2010/04/17 05:16:34  dongqing
// small portrait display layouts
//
// Revision 1.11.2.5  2010/03/20 21:11:19  dongqing
// virtual
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.4  2010/02/25 22:43:49  dongqing
// removed overview controls from verify orthogonal layout
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.3  2010/01/18 09:49:16  dongqing
// vertical scrollbars and blank view handling
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.2  2009/12/09 20:25:22  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.11.2.1  2009/10/07 16:13:17  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.11  2007/03/09 22:04:05  jmeade
// code standards.
//
// Revision 1.10  2007/03/01 01:18:57  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.9  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.8  2006/06/08 18:21:28  frank
// working on showing decubitus left/right, etc
//
// Revision 1.7  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.6  2006/02/07 23:33:29  jmeade
// code review: comments.
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.3  2006/05/16 12:43:34  geconomos
// mand OnMouseMove virtual
//
// Revision 1.4.2.2  2006/03/08 00:17:33  jmeade
// Issue 4620: Shortcut key for annotation mode.
//
// Revision 1.4.2.1  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.4  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.12.2.3.4.6.2.1  2005/07/13 17:31:13  geconomos
// Issue #4272: Flying not permitted.
//
// Revision 3.12.2.3.4.6  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.12.2.3.4.5.2.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 3.12.2.3.4.5  2005/04/08 18:23:40  frank
// Issue #4113: Disabled menu items in verification view
//
// Revision 3.12.2.3.4.4  2005/04/08 16:02:10  frank
// Issue #4113: permitted toggling supine/prone in verify view
//
// Revision 3.12.2.3.4.3  2005/03/30 20:54:13  jmeade
// drawing framework changes.
//
// Revision 3.12.2.3.4.2  2005/03/30 20:37:10  frank
// Issue #3997: Disabled function keys in verify slice view
//
// Revision 3.12.2.3.4.1  2005/03/29 15:00:48  jmeade
// Issue 3997: No 2D flight in slice verification.
//
// Revision 3.12.2.3  2004/07/21 20:26:26  jmeade
// Issue 3630: Keep track of when the segment orderings have been changed.
//
// Revision 3.12.2.2  2004/07/17 03:03:58  jmeade
// Update colon finding distances when colon segments are changed; Separated functions for individual wm_command messages.
//
// Revision 3.12.2.1  2003/02/22 12:20:28  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.12  2002/12/24 03:45:21  jmeade
// Remove debug code (registering on 'r' key).
//
// Revision 3.11  2002/11/27 22:33:05  jmeade
// Reverse of paradigm for popup menus:  assume show popup menu on right-click in childview,
//  have view window instances validate/refuse popup as necessary.
//
// Revision 3.10  2002/11/15 00:45:28  jmeade
// No hard-coding of # of child views.
//
// Revision 3.9  2002/11/14 18:47:14  jmeade
// Moving declarations to satisfy ClassWizard.
//
// Revision 3.8  2002/11/08 01:34:06  kevin
// Cleaned up a couple of small issues in S/P registration
//
// Revision 3.7  2002/10/21 18:52:08  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.6  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.4  2002/04/12 02:16:25  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.3  2001/11/23 15:13:04  jmeade
// Set icons for buttons.
//
// Revision 3.2  2001/11/17 03:41:34  jmeade
// Patient view stored in base class.
//
// Revision 3.1  2001/11/09 01:17:50  jmeade
// Merge with 1.1 - 2001-11-08
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
//    Rev 2.0   Sep 16 2001 23:43:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:12   ingmar
// Initial revision.
// Revision 1.27  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.26  2001/04/13 02:35:22  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VerifyView.h,v 1.11.2.6 2010/04/17 05:16:34 dongqing Exp $
// $Id: VerifyView.h,v 1.11.2.6 2010/04/17 05:16:34 dongqing Exp $
