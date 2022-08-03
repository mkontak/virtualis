//////////////////////////////////////////////////////////////////////
// $Id: SegmentCylindricalChildView.h,v 1.7.2.1 2009/10/07 16:13:15 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SegmentCylindricalChildView.h
// Description: Interface for the SegmentCylindricalChildView class.
// Created:
// Author: Kevin Kreeger    kevin@viatronix.net
//
// Complete History at bottom of file.
// 
//////////////////////////////////////////////////////////////////////

#ifndef SegmentCylindricalChildView_H
#define SegmentCylindricalChildView_H



// include declarations
#include "ViatronChildView.h"
#include "SegmentCylindricalViewer.h"
#include "SliceScrollBar.h"
#include "GLPopupMenu.h"


namespace ColonViews
{

  namespace ChildViews
  {

    // class declaration
    class SegmentCylindricalChildView : public ViatronChildView
    {
    public:
      /// Enums for line/area measurements
      enum MEASUREFUNCENUM { MEASUREFUNC_START, MEASUREFUNC_MOVE, MEASUREFUNC_END, MEASUREFUNC_UPDATE };

    public:
      /// Constructor
      SegmentCylindricalChildView(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID);
      /// Destructor
      virtual ~SegmentCylindricalChildView();

      /// Sets dataset type (position)
      void SetDatasetType(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation);

      /// Stores the HWND of the accompanying scrollbar
      void SetSliceScrollBar(const CWnd & wnd) { m_hScrollbar = wnd; }

      /// Initialize slice volume data.
      afx_msg LRESULT OnVolumeLoaded(WPARAM wParam, LPARAM lParam);

      /// set whether to update the endoluminal view
      void SetUpdateEndoluminalViewFlag(const bool bUpdateEndoluminalView) { m_bUpdateEndoluminalView = bUpdateEndoluminalView; }
      /// get whether to update the navigation view
      const bool UpdateEndoluminalView() const { return m_bUpdateEndoluminalView; }

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

    private:
      /// Gets a temporary-use reference to the accompanying scrollbar instance
      WinUtils::SliceScrollBar & GetScrollBar();

    public:

      /// Viewer for this slice
      ReaderLib::SegmentCylindricalViewer m_cylindricalSegmentViewer;

    protected:
      /// Whether or not a measurement is currently being updated
      bool m_bMeasureActive;

      /// Zoom menu
      WinUtils::GLPopupMenu m_zoomMenu;

      /// if we need to update navigation view. 
      bool m_bUpdateEndoluminalView;

    private:
      /// Pointer to the last Dataset instance that was rendered, to detect the need to update the view
      ReaderLib::Dataset* m_pDatasetLastRendered;
      /// Handle to the accompanying scrollbar
      HWND m_hScrollbar;
    };

  } // namespace ColonViews

} // namespace ChildViews


#endif // !defined(AFX_SegmentCylindricalChildView_H)

// $Log: SegmentCylindricalChildView.h,v $
// Revision 1.7.2.1  2009/10/07 16:13:15  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.7  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.6  2007/02/06 16:40:55  jmeade
// user data in render method.
//
// Revision 1.5  2006/09/27 19:20:12  frank
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
// Revision 1.2.8.2  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.2.8.1.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.2.8.1  2005/04/13 19:25:06  frank
// split out GL popup menu class
//
// Revision 1.2  2002/11/14 19:38:06  kevin
// Clean up unused code
//
// Revision 1.1  2002/11/11 16:17:01  kevin
// Initial version of cylindrical -- still needs some polishing
//
