// $Id: Segment3DChildView.h,v 1.14.2.2 2010/01/29 22:57:31 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger    kevin@viatronix.net


// pragmas
#pragma once


// includes
#include "ViatronChildView.h"
#include "Segment3DViewer.h"
#include "SliceScrollBar.h"
#include "GLPopupMenu.h"


// namespaces
namespace ColonViews
{
  namespace ChildViews
  {

    // class declaration
    class Segment3DChildView : public ViatronChildView
    {
    public:

      /// Enums for line/area measurements
      enum MEASUREFUNCENUM { MEASUREFUNC_START, MEASUREFUNC_MOVE, MEASUREFUNC_END, MEASUREFUNC_UPDATE };

    public:

      /// Constructor
      Segment3DChildView(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID);

      /// Destructor
      virtual ~Segment3DChildView();

      /// Sets dataset type (position)
      void SetDatasetType(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation);

      /// Stores the HWND of the accompanying scrollbar
      void SetSliceScrollBar(const CWnd & wnd) { m_hScrollbar = wnd; }

      /// Initialize slice volume data.
      afx_msg LRESULT OnVolumeLoaded(WPARAM wParam, LPARAM lParam);

      /// sets the flag to update the endoluminal view
      void SetUpdateEndoluminalViewFlag(const bool bUpdateEndoluminalView) { m_bUpdateEndoluminalView = bUpdateEndoluminalView; }

    protected:

      /// Set up rendering context
      virtual void GLSetupRC();

      /// Render the slice
      void GLRenderScene(void * pData = NULL);

      /// raster scene: text, measurement line, etc.
      virtual void GLRasterScene();

      /// action response to resize window.
      virtual void GLResize(GLsizei iWidth, GLsizei iHeight);

      /// idle function.
      virtual bool GLIdle();

      /// Gets resource ID for popup menu
      uint4 GetPopupMenuID(UINT uFlags, CPoint point) const;

      /// Gets resource string IDs for mouse key
      virtual void GetMouseKeyStringIDs(uint4 & uLeftID, uint4 & uMidID, uint4 & uRightID) const;

      /// Adjust items in popup menu before display
      void AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const;

      /// event handler: message.
      virtual int4 OnMessage(uint2 uMessage, int4 lParam);

      /// event handler: command.
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

      /// a group of mouse event handlers below:
      /// L-button down.
      void OnLButtonDown(UINT uFlags, CPoint point);

      /// L-button up.
      void OnLButtonUp(UINT uFlags, CPoint point) ;

      /// L-button dbl click
      virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

      /// R-button down.
      void OnRButtonDown(UINT uFlags, CPoint point);

      /// R-button up.
      void OnRButtonUp(UINT uFlags, CPoint point);

      /// mouse move
      void OnMouseMove(UINT uFlags, CPoint point);

      /// mouse wheel.
      virtual BOOL OnMouseWheel(UINT uFlags, int2 iZDelta, CPoint pt);

      /// mouse leave (a window)
      virtual void OnMouseLeave();

      /// Visibility state change
      virtual void OnShowWindow(const bool bShow);

      /// keyboard event handler: key pressed.
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// Called to set mouse cursor
      BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);

      /// Update the slice scrollbar position (and range)
      void UpdateSliceScrollPos(const bool bSetRange = false);

      /// synchronizes the submarine to the given 2D image point
      void SynchronizeEndoluminalView( CPoint point );

      /// draw a cross-hair at the other UDS panel to indicate the same anatomy spot
      void DrawIndicatorInOtherQuadrant();

      /// Called when view modes change
      virtual void OnViewModeChange(const uint4 uOldMode, const uint4 uNewMode);

      /// Invalidate (mark for redraw) view
      virtual void InvalidateView();

    private:

      /// Gets a temporary-use reference to the accompanying scrollbar instance
      WinUtils::SliceScrollBar & GetScrollBar();

      /// draw cad indicators
      void DrawCadMarkings();

    public:

      /// Viewer for this slice
      ReaderLib::Segment3DViewer m_3DsegmentViewer;

    protected:

      /// Whether or not a measurement is currently being updated
      bool m_bMeasureActive;

      /// if we need to update navigation view. 
      bool m_bUpdateEndoluminalView;

    private:

      /// Pointer to the last Dataset instance that was rendered, to detect the need to update the view
      ReaderLib::Dataset* m_pDatasetLastRendered;

      /// Handle to the accompanying scrollbar
      HWND m_hScrollbar;

      /// set to true after (left-)mouse-up events
      bool m_bJustFinishInteracting;

      /// the last 3D point to which this 3D segments view was synched
      Point3D<float4> m_ptLastSynched3DPos;

      /// record the mouse cursor coordinate on screen
      CPoint m_point;

    }; // class Segment3DChildView


    // namespaces
  } // namespace ColonViews
} // namespace ChildViews


// $Log: Segment3DChildView.h,v $
// Revision 1.14.2.2  2010/01/29 22:57:31  dongqing
// add location indicator on other quadrent. Mouse left button down
// to trigger that.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.14.2.1  2009/10/07 16:13:15  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.14  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.13  2007/02/06 16:40:27  jmeade
// user data in render method.
//
// Revision 1.12  2007/01/30 16:58:49  geconomos
// initial work on draw cad markings
//
// Revision 1.11  2007/01/10 18:31:46  jmeade
// Issue 5217: flag image for dirty on InvalidateView().
//
// Revision 1.10  2006/11/24 16:53:48  jmeade
// Issue 5137: correlate 3d segment view back to submarine only when appropriate.
//
// Revision 1.9  2006/11/21 04:44:44  jmeade
// Issue 5137: less buggy version, will only circular-correlate (ud-3d-back-to-ud) in rare cases.  will fix soon.
//
// Revision 1.8  2006/11/19 00:20:32  jmeade
// Issue 5137: correlate 3d segment view back to submarine when appropriate.
//
// Revision 1.7  2006/10/04 16:44:12  jmeade
// segment n/n; remove zoom factor/menu.
//
// Revision 1.6  2006/10/04 15:43:54  jmeade
// update view on mode change; mouse help.
//
// Revision 1.5  2006/09/27 19:17:34  frank
// formatting
//
// Revision 1.4  2006/01/31 14:50:38  frank
// split up volume measure file
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
// Revision 1.5.8.2  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.5.8.1.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.5.8.1  2005/04/13 19:25:01  frank
// split out GL popup menu class
//
// Revision 1.5  2002/11/14 19:38:06  kevin
// Clean up unused code
//
// Revision 1.4  2002/11/11 21:15:35  kevin
// Cylindirical works now
//
// Revision 1.3  2002/11/11 16:18:07  kevin
// Initial version of cylindrical -- still needs some polishing
//
// Revision 1.2  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.1  2002/08/20 19:15:33  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:54:23  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Segment3DChildView.h,v 1.14.2.2 2010/01/29 22:57:31 dongqing Exp $
// $Id: Segment3DChildView.h,v 1.14.2.2 2010/01/29 22:57:31 dongqing Exp $
