// $Id: ViatronDialog.h,v 1.8.2.2 2010/04/11 11:40:53 dongqing Exp $
// 
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: ViatronDialog dialog helper class (e.g. for skinned-looking dialogs)
// Owner: Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_VIATRONDIALOG_H__D6836607_2065_4E30_A935_2170202F4EBC__INCLUDED_)
#define AFX_VIATRONDIALOG_H__D6836607_2065_4E30_A935_2170202F4EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"
#include "resource.h"
#include "BackgroundBrush.h"


namespace ColonViews
{

  namespace Dialogs
  {

    /**
     * Contains handlers for skinning/background images, common convenience functions, etc.
     */
    class ViatronDialog : public CDialog
    {
    public:
      /// action to take on each window found in an EnumWindows call
      enum eEnumChildrenProc { eShow, eHide, eEnable, eDisable };

      /// layout changing or layout changed event
      enum { WM_USER_LAYOUT_CHANGE = WM_USER + 10, eWMUserWPLayoutChanging = 0, eWMUserWPLayoutChanged = 1 };

    public:
      /// Standard constructor.
      ViatronDialog(UINT uIDTemplate, CWnd* pParent = NULL, const uint4 uDlgBkgdResourceID = IDB_BLUEMETAL);

      /// Default constructor.
      ViatronDialog();

      /// Performs a function, e.g. show, hide, enable, etc. on all children of this window
      void ProcAllChildWindows(const eEnumChildrenProc eProc);

      /// Enables tracking of mouse leave and mouse hover events
      void EnableMouseTracking();

      /// called before the current layout changes
      virtual void OnLayoutChanging() {}

      /// called after the current layout changes
      virtual void OnLayoutChanged() {}

      // Dialog Data
        //{{AFX_DATA(ViatronDialog)
        // NOTE: the ClassWizard will add data members here
        //}}AFX_DATA


      // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(ViatronDialog)
        protected:
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        //}}AFX_VIRTUAL
        /// When mouse stays within rectangle defined by SPI_GETMOUSEHOVERWIDTH/HEIGHT for period of time defined by SPI_GETMOUSEHOVERTIME
        virtual void OnMouseHover(UINT uFlags, CPoint origPt);
        /// When mouse leaves dialog
        virtual void OnMouseLeave();

        // Implementation
    protected:

      // Generated message map functions
      //{{AFX_MSG(ViatronDialog)
      /// size handler
      afx_msg void OnSize(UINT uType, int iCX, int iCY);
      /// control colour handler
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
      /// mouse move handler
      afx_msg void OnMouseMove(UINT uFlags, CPoint point);
      /// left double click handler
      afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
      /// layout change event handler
      afx_msg LRESULT OnMessageLayoutChangeEvent(WPARAM wParam, LPARAM lParam);
      /// ww_destroy handler
      afx_msg void OnDestroy();
      //}}AFX_MSG

      /// Gets tool tip message text
      virtual BOOL OnToolTipNeedText(UINT id, NMHDR *pTTTStruct, LRESULT *pResult);
  
      /// message map
      DECLARE_MESSAGE_MAP()

      /// called when background needs erasing
      BOOL OnEraseBkgnd( CDC * pdc );

      /// centers all children of the dialog box within the specified groupbox
      void CenterChildControls(const uint4 uControlContainerBoxID);

    private:
      /// Performs operation specified in lParam parameter on the indicated window
      static BOOL CALLBACK ChildWindowEnumProc(HWND hwnd, LPARAM lParam);

      /// Moves a single child window during enumeration of CenterChildControls method
      static BOOL CALLBACK MoveChildWinEnumProc(HWND hWndChild, LPARAM lParam);

    protected:
      /// typedef for background brushes 
      typedef std::map< uint4, WinUtils::BackgroundBrush * >  BackgroundBrushesCache;

      /// Brush to color controls.
      WinUtils::BackgroundBrush * m_pBackgroundBrush;
  
      /// Background image resource ID.
      uint4 m_uDlgBackgroundResourceID;

      /// number of times that WM_SIZE has been handled
      int4 m_iSizeCount;

      /// information on a move-children operation
      struct MoveChildWinStruct
      {
        CSize szDiff;
        HWND hParent;
        uint4 uContainerID;
      }; // struct MoveChildWinStruct

    private:
      /// true to reset mouse tracking events
      bool m_bResetMouseTrackingEvents;
    };

  } // namespace Dialogs

} // namespace ColonViews



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIATRONDIALOG_H__D6836607_2065_4E30_A935_2170202F4EBC__INCLUDED_)

// Revision History:
// $Log: ViatronDialog.h,v $
// Revision 1.8.2.2  2010/04/11 11:40:53  dongqing
// centering child controls
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.8.2.1  2010/04/10 23:56:38  dongqing
// tool tip text override
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.8  2007/03/07 20:19:17  jmeade
// comments.
//
// Revision 1.7  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.6  2007/01/09 19:16:43  jmeade
// layout changed/changing events for dialogs.
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.4.2  2006/05/05 12:06:47  geconomos
// correctly freeing brushes when windows ins destroyed
//
// Revision 1.4.4.2  2006/05/05 12:06:47  geconomos
// correctly freeing brushes when windows ins destroyed
//
// Revision 1.4.4.1  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/16 22:56:12  jmeade
// namespaces for WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.6.4.1.4.1.2.1  2005/06/17 13:05:52  geconomos
// using new BackgroundBrush class
//
// Revision 3.6.4.1.4.1  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.6.4.1.8.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 3.6.4.1  2003/02/27 23:14:19  jmeade
// Put common tooltip text get method in base class for use by all derivatives.
//
// Revision 3.6  2002/07/26 22:43:25  jmeade
// Pass non-client area double-click (i.e. activate/deactivate) message to parent for handling.
//
// Revision 3.5  2002/04/26 15:57:32  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.4.2.1  2002/04/10 20:14:44  jmeade
// Implemented mouse tracking.
//
// Revision 3.4  2002/01/29 03:02:54  jmeade
// Function to perform operations on all child windows.
//
// Revision 3.3  2001/11/20 09:55:38  jmeade
// Comments.
//
// Revision 3.2  2001/11/18 01:10:40  jmeade
// Variable naming.
//
// Revision 3.1  2001/11/17 03:47:58  jmeade
// Allow descendants to specify background (threw out the old carpet).
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 26 2001 23:56:46   jmeade
// Default CDialog-derived constructor
// 
//    Rev 2.0   Sep 16 2001 23:43:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:20   ingmar
// Initial revision.
// Revision 1.2  2001/04/13 02:35:22  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronDialog.h,v 1.8.2.2 2010/04/11 11:40:53 dongqing Exp $
// $Id: ViatronDialog.h,v 1.8.2.2 2010/04/11 11:40:53 dongqing Exp $
