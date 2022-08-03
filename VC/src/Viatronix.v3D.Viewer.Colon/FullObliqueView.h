// $Id: FullObliqueView.h,v 1.5.2.14 2010/04/17 05:16:17 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: FullObliqueView.h
// Owner(s): Kevin Kreeger   kevin@viatronix.com

#if !defined(AFX_FULLOBLIQUECOLONWND_H__C583B9A6_9374_4281_8D9F_B7A8DE44AF0E__INCLUDED_)
#define AFX_FULLOBLIQUECOLONWND_H__C583B9A6_9374_4281_8D9F_B7A8DE44AF0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"
#include "SliceScrollBar.h"

#include "ReaderGlobal.h"
#include "V1KMessages.h"

/////////////////////////////////////////////////////////////////////////////
// FullObliqueViewFrame frame

namespace ColonViews
{

  namespace Layouts
  {

    /**
     * Frame for a view with a single large orthogonal view
     */
    class FullObliqueLayoutFrame : public ChildLayoutFrame
    {
      DECLARE_DYNCREATE(FullObliqueLayoutFrame)

    // construction
    protected:

      /// Constructor; protected constructor used by dynamic creation
      FullObliqueLayoutFrame();

    // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(FullObliqueLayoutFrame)
      //}}AFX_VIRTUAL

      // Generated message map functions
      //{{AFX_MSG(FullObliqueLayoutFrame)
        // NOTE - the ClassWizard will add and remove member functions here.
      //}}AFX_MSG
    }; // class FullObliqueLayoutFrame
  
    /////////////////////////////////////////////////////////////////////////////

    /**
     * A view with a single large orthogonal view
     */
    class FullObliqueLayout : public ColonViatronixWnd
    {
    // construction
    public:

      /// Constructor
      FullObliqueLayout();
      
      /// Destructor
      ~FullObliqueLayout();
      
    // member functions
    public:

      /// Perform view-specific initialization
      void CreateChildViews(HWND hWndThis);
      
      /// Gets the resource ID for the popup menu for this window
      virtual uint4 OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const;

    protected:

      /// Idle time processing
      //virtual void GLIdle();
      /// renders all views
      virtual void GLRenderViews();

      /// Keyboard event handler: key pressed.
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      //{{AFX_MSG(FullObliqueLayout)
      HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
      
      /// handle resize
      afx_msg void OnSize(UINT uType, int cx, int cy);
      
      /// handle the main dataset changed
      afx_msg LRESULT OnChangeMainDataset(WPARAM, LPARAM);

      /// handle App Child view message
      virtual afx_msg LRESULT OnAppChildViewMessage(WPARAM wParam, LPARAM lParam);

      /// handle timer event
      afx_msg void OnTimer(UINT uIDEvent);

      /// handle toggle of autofly
      afx_msg void OnToggleAutofly();

      /// handle flight direction toggle
      afx_msg void OnToggleFlightDir();

      /// handle mouse wheel event
      afx_msg BOOL OnMouseWheel(UINT uFlags, short zDelta, CPoint pt);

      /// handle mouse move event
      afx_msg void OnMouseMove(UINT uFlags, CPoint point);

      /// handle left button double click event
      afx_msg void OnLButtonDblClk(UINT uFlags, CPoint origPt);

      /// handle report add snapshot event
      afx_msg void OnReportAddSnapshot();

      /// handle child window invalidate
      virtual afx_msg LRESULT OnAppGLChildViewTypeInvalidate(WPARAM wpGLChildViewType, LPARAM lpGLChildViewID);

      /// handle update window arrow
      afx_msg virtual void OnUpdateViewWindowArrowInSliceCenter(CCmdUI* pCmdUI);

      /// handle update vieiew window update
      afx_msg virtual void OnUpdateViewWindowUpdateEndoluminalView(CCmdUI* pCmdUI);

      /// handle update view window flight event
      afx_msg virtual void OnUpdateViewWindowFlightAlongCenterline(CCmdUI* pCmdUI);

      /// handle update next missed region event
      afx_msg virtual void OnUpdateNextMissedRegion(CCmdUI* pCmdUI);

      /// handle movie record event
      afx_msg LRESULT OnMovieRecordGetViewID(WPARAM, LPARAM);

      /// handle toggle of body position event
      afx_msg void OnToggleSupineProne();

      /// Accelerator message, toggles line measure mode on/off
      afx_msg void OnToggleLineMeasure();

      /// disable this menu option in fullscreen view
      afx_msg virtual void OnUpdateToggleScrollbarOrientation(CCmdUI* pCmdUI);
      //}}AFX_MSG

      DECLARE_MESSAGE_MAP()

      /// oncommand handler
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

      /// return the brush to be used for the view's background
      virtual CBrush * GetViewBackgroundBrush();

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDT_FULLOBLIQUE_LANDSCAPE; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return IDT_FULLOBLIQUE_WIDESCREEN; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDT_FULLOBLIQUE_VIEW; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID()   { return IDT_FULLOBLIQUE_VIEW; }

    private:

      /// Waits on event for view completion
      static UINT WaitOnViewCompletion(LPVOID pParam);

    // member variables
    public:

      /// type of child view window to invoke
      static GLChildView::ViewWndTypeEnum m_eViewWndType;
      /// dataset position
      static ReaderLib::DatasetOrientations::TypeEnum m_eDatasetOrientation;

    private:
      /// progressive refinement timer
      WinUtils::WmTimer m_timerProgRefine;

      /// Handle of wait event 
      HANDLE m_hViewWait;

      /// slice scrollbar (NOTE: this is because for some God-unknown reason I can't get the scrollbar to appear thru the resource file)
      WinUtils::SliceScrollBar m_scroll;

      /// Brush for the black background
      CBrush m_brushBlack;
    }; // class FullObliqueLayout

  } //namespace Layouts

} // namespace ColonViews



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FULLOBLIQUECOLONWND_H__C583B9A6_9374_4281_8D9F_B7A8DE44AF0E__INCLUDED_)

// Revision History:
// $Log: FullObliqueView.h,v $
// Revision 1.5.2.14  2010/04/17 05:16:17  dongqing
// small portrait display layouts
//
// Revision 1.5.2.13  2010/04/09 02:30:34  dongqing
// portrait and widescreen layouts
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.2.12  2010/03/20 12:33:31  dongqing
// virtual
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.2.11  2009/12/09 20:26:02  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.5.2.10  2009/10/07 16:13:13  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.5.2.9  2009/07/30 17:58:19  kchalupa
// Code Walkthrough
//
// Revision 1.5.2.8  2008/01/16 01:41:37  jmeade
// Issue 5917: only vertical scrollbars on the fullscreen view.
//
// Revision 1.5.2.7  2007/10/03 20:03:50  jmeade
// Issue 5359: enable line-measure functionality via f6.
//
// Revision 1.5.2.6  2007/10/03 02:28:57  jmeade
// allow childviews to set their own window background colour.
//
// Revision 1.5.2.5  2007/10/02 01:00:09  jmeade
// scrollbar implemented by hand.
//
// Revision 1.5.2.4  2007/09/28 01:25:40  jmeade
// set current dataset in fullview mode.
//
// Revision 1.5.2.3  2007/09/25 23:15:08  jmeade
// Issue 5359: disable the tabs.
//
// Revision 1.5.2.2  2007/09/25 22:45:16  jmeade
// Issue 5359: set the child view type.
//
// Revision 1.5.2.1  2007/09/25 21:34:06  jmeade
// Issue 5359: use for full-screen single view functionality.
//
// Revision 1.5  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.4  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
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
// Revision 1.1.2.13.2.1  2005/07/07 00:53:19  jmeade
// Issue 4242: Ignore F4 with only one dataset.
//
// Revision 1.1.2.13  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 1.1.2.12  2005/06/10 18:20:49  geconomos
// Added direct call to invalidate one or more views in lieu of using the SendMessage approach
//
// Revision 1.1.2.11  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.2.10.2.1  2005/06/08 18:46:20  jmeade
// override to specify the movie record view.
//
// Revision 1.1.2.10  2005/06/06 18:21:32  jmeade
// snapshot and next-region-jump disable....turn off autofly at colon end.
//
// Revision 1.1.2.9  2005/06/03 23:24:09  jmeade
// progressive refinement when scrolling.
//
// Revision 1.1.2.8  2005/06/03 15:02:24  jmeade
// oblique view incremental.
//
// Revision 1.1.2.7  2005/06/02 21:07:11  jmeade
// paint event.
//
// Revision 1.1.2.6  2005/06/01 23:06:02  jmeade
// oblique view: controls pad.
//
// Revision 1.1.2.5  2005/06/01 01:36:02  jmeade
// slice count update.
//
// Revision 1.1.2.4  2005/05/31 22:48:23  jmeade
// double-click flight, snapshot, r-key in overview.
//
// Revision 1.1.2.3  2005/05/27 15:57:39  jmeade
// oblique review: flight direction.
//
// Revision 1.1.2.2  2005/05/25 21:32:42  jmeade
// more oblique review functionality.
//
// Revision 1.1.2.1  2005/05/25 01:53:36  jmeade
// oblique review.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/FullObliqueView.h,v 1.5.2.14 2010/04/17 05:16:17 dongqing Exp $
// $Id: FullObliqueView.h,v 1.5.2.14 2010/04/17 05:16:17 dongqing Exp $
