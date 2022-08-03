// $Id: SlidingPaneView.h,v 1.5.2.1 2010/04/18 18:35:14 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SlidingPaneView.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#if !defined(AFX_SLIDINGPANEVIEW_H__DEE54A72_D3D9_4E9E_82AC_F4B645F2F16B__INCLUDED_)
#define AFX_SLIDINGPANEVIEW_H__DEE54A72_D3D9_4E9E_82AC_F4B645F2F16B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"
#include <list>
#include <map>
#include <vector>
#include "WinGDIUtils.h"
#include "WmTimer.h"

/// Forward declarations
namespace ColonViews
{
  namespace Layouts
  {
    class ViatronixWnd;
  } // namespace Layouts

  namespace Panes
  {
    class PaneHeader;
  } // namespace Panes
} // namespace ColonViews

namespace ColonViews
{

  namespace Panes
  {
    /**
     * View consisting of hidable pane child views.
     * In actuality, this is a groupbox that simply manages the panes and pane headers.
     * Note: The SlidingPaneManager instance, pane headers and panes should all be sibling windows.
     */
    class SlidingPaneManager : public CButton  
    {
      // Structures
    private:
      /// Maps HWNDs (for pane children) to the last time they were active
      class HWNDtoTimeMap : public std::map<HWND, COleDateTime>
      {
      public:
        /// Returns the last accessed time for the given pane, if any
        COleDateTime GetTime(HWND hWnd) const
        {
          const COleDateTime timeMaxValue(100,1,1,0,0,0);
          HWNDtoTimeMap::const_iterator itMapEntry;
          return ((itMapEntry = find(hWnd)) != end()) ? itMapEntry->second : timeMaxValue;
        }
      };

      /// Information on a pane
      class PaneInfo
      {
      public:
        /// Constructor
        PaneInfo(HWND hWndPaneHdr = NULL, const COleDateTime& axtime = COleDateTime(100,1,1,0,0,0))
          : m_hWndPaneHdr(hWndPaneHdr), m_timeLastActive(axtime) {  }

      public:
        /// Access routine, get the pane header handle
        HWND GetPaneHeader() const { return m_hWndPaneHdr; }

        /// Access routine, set the pane header handle
        void SetPaneHeader(const HWND& hWnd) { m_hWndPaneHdr = hWnd; }

        /// Access routine, get the last access time
        COleDateTime GetAccessTime() const { return m_timeLastActive; }

        /// Access routine, set the last access time
        void SetAccessTime(const COleDateTime& accessTime) { m_timeLastActive = accessTime; }

      private:
        /// The window handle of the pane
        HWND m_hWndPaneHdr;

        /// The last time the given pane was accessed by user
        COleDateTime m_timeLastActive;
      };

    public:
      /// Adds a pane to be managed
      void AddPane(HWND hPaneWnd);
      /// Shows or hides the pane corresponding to the pane header with the specified control ID
      void ShowPane(const uint4 uPaneHeaderControlID, const bool bShow);
      /// Returns true if the pane corresponding to the given pane header is visible, false otherwise
      bool IsPaneVisible(const uint4 uPaneHeaderControlID);
      /// Sets the text for the pane (and pane header, which displays the pane's window text)
      void SetPaneText(const uint4 uPaneHeaderControlID, const std::string& sText);

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(SlidingPaneManager)
    protected:
      //}}AFX_VIRTUAL

      // Generated message map functions
    protected:
      //{{AFX_MSG(SlidingPaneManager)
      /// non-client area double-click
      afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
      /// resize msg
      afx_msg void OnSize(UINT nType, int cx, int cy);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

    private:
      /// Places the child pane controls appropriately
      void StackPanes();

      /// Sort iteration routine, compares by window rect y positions
      static int ComparePaneInfo_HeaderYPos(const void *pVoid1, const void *pVoid2);
      /// Sort iteration routine, compares by last access time
      static int ComparePaneInfo_Time(const void *pVoid1, const void *pVoid2);

      /// Returns a vector of controls that are of typeid(PaneHeader)
      typedef std::vector<PaneInfo> PaneInfoVector;
      PaneInfoVector GetPanesInfo() const;

    private:
      /// List of managed panes
      typedef std::list<HWND> HWNDList;
      HWNDList m_paneList;
      /// MRU list, maps HWNDs (for pane children) to the last time they were active
      HWNDtoTimeMap m_mapLastActive;
    };

    /////////////////////////////////////////////////////////////////////////////
    // PaneHeader window

    /**
     * "Title" bar (header) for a hidable child pane view
     */
    class PaneHeader : public CButton
    {
    public:
      /// Message to show or hide a child pane
      enum { WM_USER_SHOWPANE = WM_USER + 100 }; // WPARAM = 0 to hide, show otherwise

    public:
      /// Constructor
      PaneHeader(HWND hWndChild = HWND(NULL));

      /// Show or hide the pane's child
      void ShowChild(const bool bShow) const;

      /// Returns true if the child is valid and visible
      inline bool IsChildVisible() const;

      /// Gets window rect of entire pane (pane header plus visible child rect)
      CRect GetPaneWindowRect() const;

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(PaneHeader)
    public:
      virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
      //}}AFX_VIRTUAL

      // Generated message map functions
    protected:
      //{{AFX_MSG(PaneHeader)
      afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
      afx_msg void OnMove(int x, int y);
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnSize(UINT nType, int cx, int cy);
      afx_msg BOOL OnEraseBkgnd( CDC * pdc );
      afx_msg void OnTimer(UINT_PTR uIDEvent);
      //}}AFX_MSG
      /// Handles the message to show or hide the pane
      afx_msg LRESULT OnMessageShowPane(WPARAM wShowOrHide, LPARAM);

      DECLARE_MESSAGE_MAP()

      /// Gets the pane header's manager
      inline HWND GetManager() const
      {
        return m_hPaneManager;
      }

      /// Sets the pane header's manager
      inline void SetManager(HWND hManagerWnd)
      {
        m_hPaneManager = hManagerWnd;
      }

      // Variables
    private:
      /// Sliding pane manager for this header
      HWND m_hPaneManager;

      /// Child window (the pane) for this header
      HWND m_hPaneChild;

      /// Drawing bitmap
      WinUtils::BitmapDC m_bmpDC;

      /// Bitmap DC for the closed pane header
      WinUtils::BitmapDC m_paneHdrBmpDC_Closed;
      /// Bitmap DC for the open pane header
      WinUtils::BitmapDC m_paneHdrBmpDC_Open;

      /// Timer for keeping title updated
      WinUtils::WmTimer m_tmrUpdateTitle;

      // Friends
      friend class ColonViews::Layouts::ViatronixWnd;
      friend class SlidingPaneManager;
    };


    /**
     * Returns true if the child is valid and visible
     *
     * @return   Whether or not the m_hPaneChild member is a valid, visible window
     */
    inline bool PaneHeader::IsChildVisible() const
    {
#ifdef _DEBUG
      const bool bIsWin(IsWindow(m_hPaneChild)!=FALSE);
      const bool bIsVis(::IsWindowVisible(m_hPaneChild)!=FALSE);
      return bIsWin && bIsVis;
#endif
      return (IsWindow(m_hPaneChild) && ::IsWindowVisible(m_hPaneChild));
    }

  } // namespace Panes

} // namespace ColonViews

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDINGPANEVIEW_H__DEE54A72_D3D9_4E9E_82AC_F4B645F2F16B__INCLUDED_)

// Revision History:
// $Log: SlidingPaneView.h,v $
// Revision 1.5.2.1  2010/04/18 18:35:14  dongqing
// timer to keep pane title updated
//
// Revision 1.5  2007/03/09 01:11:49  jmeade
// code standards.
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
// Revision 1.13.2.4.4.1  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.13.2.4.8.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.13.2.4  2003/05/05 19:40:02  geconomos
// Code walkthrough.
//
// Revision 1.13.2.3  2003/03/01 02:12:16  jmeade
// Cleanup on sliding pane header and notations.
//
// Revision 1.13.2.2  2003/02/22 17:59:36  jmeade
// Removing empty constructor/destructor methods.
//
// Revision 1.13.2.1  2003/02/22 12:20:27  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.13  2002/11/26 23:55:36  jmeade
// Cleaner sliding panes implementation with panes not embedded in another view, pane manager.
//
// Revision 1.12  2002/11/25 23:25:47  jmeade
// Is visible, set text pane convenience functions; Privatize member variables.
//
// Revision 1.11  2002/11/15 00:45:28  jmeade
// No hard-coding of # of child views.
//
// Revision 1.10  2002/09/09 21:14:43  jmeade
// Post message for Show/hide pane functionality, for proper handling show/update events.
//
// Revision 1.9  2002/09/06 20:15:51  jmeade
// Code for up arrow/down arrow pane header.
//
// Revision 1.8  2002/08/29 19:57:07  jmeade
// Close panes, by least recently active times, to conserve real estate.
//
// Revision 1.7  2002/08/29 00:22:07  jmeade
// Image for sliding pane header; Comments.
//
// Revision 1.6  2002/08/28 20:03:01  jmeade
// Pane header beautifying; Unused code removal.
//
// Revision 1.5  2002/08/28 16:11:26  jmeade
// User control for sliding pane header.
//
// Revision 1.4  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.2  2002/07/26 22:47:04  jmeade
// Commenting out resize handlers until I determine cause of the ViatronChildWnd crash on pane hide;
// Tabs to spaces.
//
// Revision 1.1  2002/07/26 22:31:23  jmeade
// Sliding pane view.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SlidingPaneView.h,v 1.5.2.1 2010/04/18 18:35:14 dongqing Exp $
// $Id: SlidingPaneView.h,v 1.5.2.1 2010/04/18 18:35:14 dongqing Exp $
