//////////////////////////////////////////////////////////////////////
// $Id: Segment2DChildView.h,v 1.6.2.1 2009/10/07 16:13:15 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Segment2DChildView.h
// Description: Interface for the Segment2DChildView class.
// Created:
// Author: Kevin Kreeger    kevin@viatronix.net
//
// Complete History at bottom of file.
// 
//////////////////////////////////////////////////////////////////////

#ifndef Segment2DChildView_H
#define Segment2DChildView_H



// include declarations
#include "ViatronChildView.h"
#include "Segment2DViewer.h"
#include "SliceScrollBar.h"
#include "Triple.h"
#include "GLPopupMenu.h"


namespace ColonViews
{

  namespace ChildViews
  {

    // class declaration
    class Segment2DChildView : public ViatronChildView
    {
    public:
      /// Enums for line/area measurements
      enum MEASUREFUNCENUM { MEASUREFUNC_START, MEASUREFUNC_MOVE, MEASUREFUNC_END, MEASUREFUNC_UPDATE };

    public:
      /// Constructor
      Segment2DChildView(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent, const uint4 uControlID);
      /// Destructor
      virtual ~Segment2DChildView();

      /// Sets dataset type (position)
      void SetDatasetType(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation);

      /// Stores the HWND of the accompanying scrollbar
      void SetSliceScrollBar(const CWnd & wnd) { m_hScrollbar = wnd; }

      /// Initialize slice volume data.
      afx_msg LRESULT OnVolumeLoaded(WPARAM wParam, LPARAM lParam);
      /// set flag
      void SetMouseWheelActionFlag(const bool bMouse) { m_bMouseWheelCalculate = bMouse; }
      /// set flag
      void SetUpdateEndoluminalViewFlag(const bool bUpdataEndoluminalView) { m_bUpdateEndoluminalView = bUpdataEndoluminalView; }
      /// get flag
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

      /// mapping 2D point in window to its 3D world coordinats
      Point<float4> Calculate3DPoint(const Point<float4> & point);

      /// keyboard event handler: key pressed.
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// Called to set mouse cursor
      BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);

      /// Update the slice scrollbar position (and range)
      void UpdateSliceScrollPos(const bool bSetRange = false);

      /// Get the Hounsfield unit value at the specified point in the view
      int4 GetHounsfieldValue(const Point2D<int4>& pt, bool & bValidPosition) const;

    private:
      /// Calculates change in window/level as user drags mouse to specified point
      bool WindowLevel(const CPoint & point);

      /// Set up the start point for WindowLevel contrl.
      void SetStartPointWindowLevel(const CPoint & point);

#if 0
      /// Zooms the view as user drags mouse to specified point
      bool ZoomSelect(const CPoint & point);

      /// Pans the view
      bool Pan(SliceViewer & sliceViewer, const CPoint & point);

      /// Creates new/Adjusts current measurement
      bool RoiMeasurement(const MEASUREFUNCENUM eMeasureFunc, const CPoint point);

      /// Creates new/Adjusts current line measurement
      bool LineMeasurement(const MEASUREFUNCENUM eMeasureFunc, const CPoint point);

      /// Renders a measurement line
      void RenderLinearMeasurement(MeasureLineList::iterator & line, const CRect & rect);

      /// Renders a measurement area
      void RenderAreaMeasurement(MeasureAreaList::iterator & area, const CRect & rect, const Triple<float> & color) const;

      /// Deletes measurement line (if any) in proximity to given point
      void DeleteMeasurement(const bool bRoi, const CPoint & point);

      /// Calculates statistics for a region of interest
      void CalculateRoiStatistics(MeasureAreaList::iterator & pMeasureArea);

      /// Moves the current viewpoint
      bool MoveViewPoint(const CPoint & point);

      /// Moves the direction of view
      bool MoveViewDirection(const CPoint & point);

#endif

      /// Gets a temporary-use reference to the accompanying scrollbar instance
      WinUtils::SliceScrollBar & GetScrollBar();
      /// Transfer view coordinates to coordinates in a slice (volume)
      //Point2D<int> ViewToSlice(const Point2D<int> & Pt2DView, const CRect & rect);

      /// Transfer view coordinates to coordinates in a slice (volume)
      //Point2D<int> SliceToView(const Point2D<int> & Pt2DView, const CRect & rect);

    public:
      /// pointer to its neighbour slice view.(Supine/Prone)
      Segment2DChildView * m_pNeighbourSliceView;

      /// Viewer for this slice
      ReaderLib::Segment2DViewer m_2DsegmentViewer;

      // when jumping to new segmnet autoroate 180 degrees once
      bool m_bAutoRotate;

    protected:
      /// Current Hounsfield Unit value
      int4 m_iCurrHounsfieldValue;

      /// If the current Hounsfield value is valid
      bool m_bCurrHounsfieldValueValid;

      /// Last clicked point
      CPoint m_currPoint;

      /// Current window
      int4 m_iWindow;

      /// Current level
      int4 m_iLevel;

      /// Last window/level mouse point
      CPoint m_windowLevelLastPoint;

      /// Whether or not a measurement is currently being updated
      bool m_bMeasureActive;

      /// Current Window/Level preset 
      int4 m_iCurrentWLPreset;

      /// Menu of window/level presets, used to display localized strings
      CMenu m_presetsMenu;

      /// Window/Level preset menu
      WinUtils::GLPopupMenu m_winlevMenu;

      /// Zoom menu
      WinUtils::GLPopupMenu m_zoomMenu;

      /// the setting of this variable is for unnecessary calculation of HU value.
      bool m_bMouseWheelCalculate;

      /// if HU is updated response to MouseWheel, no further calculation in MouseMove.
      bool m_bMouseWheelUpdated; 

      /// if we need to update navigation view. 
      bool m_bUpdateEndoluminalView;

      /// update ROIMeasurement;
      bool m_bUpdateRoiMeasurement;

    private:
      /// Pointer to the last Dataset instance that was rendered, to detect the need to update the view
      ReaderLib::Dataset* m_pDatasetLastRendered;
      /// Handle to the accompanying scrollbar
      HWND m_hScrollbar;
    };


  } // namespace ColonViews

} // namespace ChildViews


#endif // !defined(AFX_Segment2DChildView_H)

// Revision History:
// $Log: Segment2DChildView.h,v $
// Revision 1.6.2.1  2009/10/07 16:13:15  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.6  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.5  2007/02/06 16:40:27  jmeade
// user data in render method.
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
// Revision 1.5.2.3.4.3  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.5.2.3.4.2.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 1.5.2.3.4.2  2005/04/13 19:25:01  frank
// split out GL popup menu class
//
// Revision 1.5.2.3.4.1  2005/03/22 20:57:27  frank
// working on current Hounsfield value display
//
// Revision 1.5.2.3  2004/07/12 20:02:18  jmeade
// removed declaration for undefined/unused method.
//
// Revision 1.5.2.2  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.5.2.1  2003/02/27 03:22:03  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.5  2003/01/22 22:33:41  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.4  2002/11/14 19:38:06  kevin
// Clean up unused code
//
// Revision 1.3  2002/08/28 20:52:21  kevin
// Lots of updates to segmental viewing
//
// Revision 1.2  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.1  2002/08/20 19:15:32  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:54:23  kevin
// Added Segmental Visualization prototype/test code.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Segment2DChildView.h,v 1.6.2.1 2009/10/07 16:13:15 dongqing Exp $
// $Id: Segment2DChildView.h,v 1.6.2.1 2009/10/07 16:13:15 dongqing Exp $
