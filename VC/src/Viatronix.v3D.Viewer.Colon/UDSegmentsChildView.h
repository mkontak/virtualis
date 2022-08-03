//////////////////////////////////////////////////////////////////////
// $Id: UDSegmentsChildView.h,v 1.5.2.1 2009/10/07 16:13:16 dongqing Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "ViatronChildView.h"
#include "SliceScrollBar.h"
#include "GLPopupMenu.h"
#include "StripViewer.h"


// namespaces
namespace ColonViews
{
  namespace ChildViews
  {


    // continuous segments strip
    class UDSegmentsMauiChildView : public ViatronChildView
    {
    public:

      /// constructor
      UDSegmentsMauiChildView(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID);

      /// destructor
      virtual ~UDSegmentsMauiChildView();

      /// Sets dataset type (position)
      void SetDatasetType(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation);

      /// Stores the HWND of the accompanying scrollbar
      void SetSliceScrollBar(const CWnd & wnd) { m_hScrollbar = wnd; }

      /// Initialize
      afx_msg LRESULT OnVolumeLoaded(WPARAM wParam, LPARAM lParam);

    protected:

      /// Set up rendering context
      virtual void GLSetupRC();

      /// Render the slice
      void GLRenderScene(void * pData = NULL);

      /// raster scene: text, measurement line, etc.
      virtual void GLRasterScene();

      /// action response to resize window
      virtual void GLResize(GLsizei iWidth, GLsizei iHeight);

      /// idle function
      virtual bool GLIdle();

      /// Gets resource ID for popup menu
      uint4 GetPopupMenuID(UINT uFlags, CPoint point) const;

      /// Gets resource string IDs for mouse key
      virtual void GetMouseKeyStringIDs(uint4 & uLeftID, uint4 & uMidID, uint4 & uRightID) const;

      /// Adjust items in popup menu before display
      void AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const;

      /// event handler: message
      virtual int4 OnMessage(uint2 uMessage, int4 lParam);

      /// event handler: command
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

      /// synchronizes the submarine to the given 2D image point
      void SynchronizeEndoluminalView( CPoint point );

      //
      // a group of mouse event handlers below:
      //

      /// L-button down
      void OnLButtonDown(UINT uFlags, CPoint point);

      /// L-button up
      void OnLButtonUp(UINT uFlags, CPoint point) ;

      /// L-button double click
      virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

      /// R-button down
      void OnRButtonDown(UINT uFlags, CPoint point);

      /// R-button up
      void OnRButtonUp(UINT uFlags, CPoint point);

      /// mouse move
      void OnMouseMove(UINT uFlags, CPoint point);

      /// mouse wheel
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

    private:

      /// the horizontal scrollbar
      HWND m_hScrollbar;

      /// the viewer
      ReaderLib::StripViewer m_stripViewer;

    }; // class UDSegmentsMauiChildView


  } // namespace ColonViews
} // namespace ChildViews


// $Log: UDSegmentsChildView.h,v $
// Revision 1.5.2.1  2009/10/07 16:13:16  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.5  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.4  2007/02/14 20:58:56  frank
// fixed up scrolling and segment selection
//
// Revision 1.3  2007/02/02 23:42:37  jmeade
// variable type for vendor map.
//
// Revision 1.2  2007/01/16 10:49:11  frank
// working on scrolling
//
// Revision 1.1  2007/01/03 06:17:36  jmeade
// future ud segments layout.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/UDSegmentsChildView.h,v 1.5.2.1 2009/10/07 16:13:16 dongqing Exp $
// $Id: UDSegmentsChildView.h,v 1.5.2.1 2009/10/07 16:13:16 dongqing Exp $
