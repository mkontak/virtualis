// $Id: SliceChildView.h,v 1.22.2.31 2010/04/17 10:22:15 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net


/**
 * Interface for the SliceChildView class.
 */


// pragmas
#pragma once


// includes
#include <utility>
#include "ViatronChildView.h"
#include "SliceViewer.h"
#include "SliceScrollBar.h"
#include "Triple.h"
#include "ViatronStudy.h"
#include "GLPopupMenu.h"
#include "AdjustTranslucencyDlg.h"
#include "GLTimer.h"


// namespaces
namespace ColonViews
{
  namespace ChildViews
  {
    /**
     * Slice child view class
     */
    class SliceChildView : public ViatronChildView
    {
    public:

      struct SliceChildViewState
      {
        /// Current window/level
        std::pair<int4,int4> m_windowLevel;

        /// orientation
        ReaderLib::SliceOrientations::TypeEnum m_eOrientation;

        /// full screen
        bool m_bFullScreen;

        /// set orientation
        bool m_bSetOrientation;
      };

    // enums and constants
    public:

      /// Enums for line/area measurements
      enum MEASUREFUNCENUM { MEASUREFUNC_START, MEASUREFUNC_MOVE, MEASUREFUNC_END, MEASUREFUNC_UPDATE };

      /// Pointer to annotation; .second indicates position (true) or orientation (false)
      typedef std::pair<ReaderLib::BookmarkList::iterator,bool> AnnotationArrowRef;

      /// Sub-modes for ROI segmentation
      enum SEGMENTROIMODE { ROI_REGION_NEW = 1, ROI_REGION_REM = 2, ROI_VOXEL_ADD = 4, ROI_VOXEL_DEL = 8 };

      /// Location of scrollbar in relation to window (currently, only bottom and right implemented)
      enum SCROLLBARLOCATIONENUM { SBO_BOTTOM, SBO_RIGHT, SBO_TOP, SBO_LEFT };

    // construction
    public:

      /// Constructor
      SliceChildView(const ReaderLib::SliceOrientations::TypeEnum eSliceOrientation, const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation, HWND hParent);

      /// Destructor
      virtual ~SliceChildView();

    // member functions
    public:

      /// Retrieve global user preference across sessions
      static void RetrieveUserPreferredDisplayOptions();

      /// Store global user preference across sessions
      static void StoreUserPreferredDisplayOptions();

      /// Sets slice orientation
      void SetOrientation(const ReaderLib::SliceOrientations::TypeEnum eSliceOrientation);
      
      /// Sets dataset type (position)
      void SetDatasetType(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation);

      /// Links the view with its accompanying scrollbar
      void SetSliceScrollBar(const CWnd & wnd);

      /// Whether or not to show 3D camera position indicator.
      static void ShowCamera(const bool bShow);
      
      /// Whether or not the 3D camera position is visible.
      static bool GetIsCameraShowing();

      /// Initialize slice volume data.
      afx_msg LRESULT OnVolumeLoaded(WPARAM wParam, LPARAM lParam);

      /// Whether or not to perform any recalculations on mouse wheel movement?
      void SetMouseWheelActionFlag(const bool bMouse) { m_bMouseWheelCalculate = bMouse; }

      /// Set up new skeleton position if received the message 'WM_APP_2DAUTOFLIGHT_UPDATE'.
      void OnUpdate2DAutoFlight(int4 iSkeletonIndex);

      /// Scrolls the slice view, by the relative value
      void ScrollSlice(const int4 iValue, const UINT uFlags = 0);

      /// Whether or not to display the HU value at the current mouse cursor point
      void ShowHUValue(const bool bShow) { m_bShowHUValue = bShow; }

      /// Whether or not the HU value at the current mouse cursor point is displayed
      bool IsHUValueVisible() const { return m_bShowHUValue; }

      /// Get current auto-fly mode status
      const bool & GetAutoflyMode() const { return m_bAutoflyMode; }

      /// Set current auto-fly mode status
      void SetAutoflyMode( const bool & bValue ) { m_bAutoflyMode = bValue; }

      /// Gets a temporary-use reference to the accompanying scrollbar instance
      WinUtils::SliceScrollBar & GetScrollBar();

      /// Get current full oblique mode
      const bool GetFullObliqueMode() const { return m_bFullOblique; }

      /// Set current full oblique mode
      void SetFullObliqueMode( const bool bVal ) { m_bFullOblique = bVal; }

      /// set one of the window level preset values
      void SetWindowLevelPreset( const int4 iPresetVal );

      inline bool IsROIVoxelEditMode() const { return ( IsMainViewWindow() && (GetViewModes() & ViewModes::SEGMENTPOLYP) &&
          (GetSegmentROIEditMode() == ROI_VOXEL_ADD || GetSegmentROIEditMode() == ROI_VOXEL_DEL || GetSegmentROIEditMode() == ROI_REGION_REM) ); }

      /// edit the user preference for quick drop annotations
      static void EditQuickDropAnnotationName();

      /// returns the current ROI segment editing mode
      static const SEGMENTROIMODE GetSegmentROIEditMode() { return m_eROIEditMode; }

      /// gets the current ROI segment editing voxel size
      static const uint4 GetSegmentROIVoxelSize() { return m_uROIEditVoxelSize; }
      
      /// sets the current ROI segment editing voxel size
      static void SetSegmentROIVoxelSize(uint4 iValue) { m_uROIEditVoxelSize = Bound(uint4(1), iValue, uint4(5)); }

      /// returns the scrollbar location
      static const SCROLLBARLOCATIONENUM GetScrollBarLocationEnum() { return m_eScrollLoc; }

      /// sets the scrollbar location
      static void SetScrollBarLocationEnum (const SCROLLBARLOCATIONENUM eLoc) { m_eScrollLoc = eLoc; }

      /// gets the slicechildview state
      SliceChildViewState GetSliceChildViewState() const;

      /// sets the slicechildview state
      void SetSliceChildViewState( SliceChildViewState & state );

    protected:

      /// Set up rendering context
      virtual void GLSetupRC();

      /// called before the current layout changes (save values)
      virtual void OnLayoutChanging();

      /// called after the current layout changes (load values)
      virtual void OnLayoutChanged();

      /// Render the slice
      void GLRenderScene(void * pData = NULL);

      /// raster scene: text, measurement line, etc.
      virtual void GLRasterScene();

      /// Temporarily moved from the months-long requested obsolete CAxialSliceView
      void CAxialSliceView_GLRasterScene();

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

      /// Adjust items in popup menu before display
      void AdjustSliceViewWindowMenu(CMenu & menu, const uint4 uMenuID) const;

      /// called when the current dataset has changed
      virtual void OnDatasetChanged();

      /// Called when mode changes
      virtual void OnViewModeChange(const uint4 uOldMode, const uint4 uNewMode);

      /// event handler: message.
      virtual int4 OnMessage(uint2 uMessage, int4 lParam);

      /// event handler: command.
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

      /// Invalidate (mark for redraw) view
      virtual void InvalidateView();

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

      /// M-button down.
      void OnMButtonDown(UINT uFlags, CPoint point);

      /// M-button up.
      void OnMButtonUp(UINT uFlags, CPoint point);
      
      /// mouse move
      void OnMouseMove(UINT uFlags, CPoint point);
      
      /// mouse wheel
      virtual BOOL OnMouseWheel(UINT uFlags, int2 iZDelta, CPoint pt);

      /// window position changed
      virtual void OnWindowPosChanged( WINDOWPOS * pWndPos );

      /// window position changing
      virtual void OnWindowPosChanging( WINDOWPOS * pWndPos );
      
      /// mouse leave (a window)
      virtual void OnMouseLeave();

      /// resize msg
      virtual void OnSize(UINT uType, int4 iWidth, int4 iHeight );

      /// handles timer event
      virtual void OnTimer(const uint4 uIDEvent);

      /// Visibility state change
      virtual void OnShowWindow(const bool bShow);

      /// keyboard event handler: key pressed.
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);
      
      /// Called to set mouse cursor
      BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
      
      /// map 2D point in window to its 3D world coordinate
      Point<float4> Calculate3DPoint(const Point<float4> & point) const;

      /// map 2D point in window to it 3D world coordinate
      Point<float4> Calculate3DPoint(const CPoint& pt) const
      {
        return Calculate3DPoint(Point<float4>(pt.x,pt.y,0));
      } // Calculate3DPoint(const CPoint& pt)

      /// Synchronizes the neighbor slice view to the other one
      void SynchronizeNeighborSlice();
      
      /// Update the slice scrollbar position (and range)
      void UpdateSliceScrollPos(const bool bSetRange = false);
      
      /// Get the Hounsfield unit value at the specified point in the view
      int4 GetHounsfieldValue(const Point2D<int4>& pt, bool & bValidPosition, Point3D<float4> & samplePosition) const;

      /// handles toggling mode on or off as necessary
      void ToggleROISegmentMode(const SEGMENTROIMODE eMode);

    private:
      
      /// Zooms the view as user drags mouse to specified point
      bool ZoomSelect(const CPoint & point);

      /// Pans the view
      bool Pan(ReaderLib::SliceViewer & sliceViewer, const CPoint & point);

      /// Types of slice scroll events.
      enum ScrollTypeEnum { eScrollAbsolute, eScrollDifference };
      /// Scrolls the slice view
      void ScrollSlice(const int4 iValue, const ScrollTypeEnum eScrollType, const CPoint& pt, const UINT uFlags = 0);

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

      /// Finds the nearest *proximal* measurement line to the point
      MeasureLineList::iterator GetNearestProximalMeasurement(MeasureLineList& lMeasureLine, const Point2D<int4>& point2D) const;

      /// Finds the nearest *proximal* measurement line to the point
      MeasureLineList::const_iterator GetNearestProximalMeasurement(const MeasureLineList& lMeasureLine,
        const Point2D<int4>& point2D) const;
      
      /// Calculates statistics for a region of interest
      void CalculateRoiStatistics(MeasureAreaList::iterator & pMeasureArea);

      /// Calculates change in window/level as user drags mouse to specified point
      void SetWindowLevel(const CPoint & point);

      /// sets window and level
      void SetWindowLevel(const float4 fWin, const float4 fLev);

      /// Set up the start point for WindowLevel control.
      void SetStartPointWindowLevel(const CPoint & point);
      
      /// Moves the current viewpoint
      bool MoveViewPoint(const CPoint & point);
      
      /// Moves the direction of view
      bool MoveViewDirection(const CPoint & point);

      /// Transfer view coordinates to coordinates in a slice (volume)
      Point2D<int> GetViewToSlice(const Point2D<int> & pt2DView, const CRect & rect) const;

      /// Transfer view coordinates to coordinates in a slice (volume)
      Point2D<int> GetSliceToView(const Point2D<int> & pt2DView, const CRect & rect) const;

      /// Prompts for text and adds a new annotation at the specified 2D point
      bool AddNewAnnotation(const Point2D<int4>& point2D, const std::string& sDefaultName = std::string());

      /// adds a new annotation at the specified 2D point
      bool AddNewSuspiciousROIinfo(const Point2D<int4>& point2D, const float4 & fVolume, const float4 & fMaximumDiameter);

      /// Finds the nearest proximal, if any, annotation to the point
      AnnotationArrowRef GetProximalAnnotation(const Point<int4>& pt) const;

      /// Returns true if the given point is proximal to the current world location
      bool IsProximalWorldLoc(const Point<int4>& volPos) const;

      /// Retrieves or updates the user preference across sessions
      static void UpdateUserPreferredDisplayOptions(const bool bRetrieve);

      /// Updates the link button (enabling/disabling as appropriate)
      void UpdateLinkButton();

      /// if needed invalidates the navigation view
      void InvalidateEndoluminalView();

      /// Force a redraw of this view
      void ForceRedraw( bool bForceNeighbor = false );

      /// Determines if this view has a neighbor and is linked to it
      bool IsLinkedWithNeighbor() const;

      inline VtxViewModes& GetViewModes() const
      { return (GetViewWndType() == GLChildView::eGVT_CROSS_SECTION) ? ViatronStudy::m_modeObliqueView : ViatronStudy::m_modeOrthogView; };

      /// indicates if we are flying in either 2d and/or 3d
      inline bool IsFlying2dOr3d() const { return
        ( ViatronStudy::m_modeOrthogView  & ViewModes::AUTOFLY2D ) ||
        ( ViatronStudy::m_modeObliqueView & ViewModes::AUTOFLY2D ) ||
        ( ViatronStudy::m_modeEndoView    & ViewModes::AUTOFLY   ) ||
        ReaderLib::rdrGlobal.m_supine.m_submarine.IsFlying()       ||
        ReaderLib::rdrGlobal.m_prone .m_submarine.IsFlying();
      }

      /// Get the full oblique mode slice number
      float4 GetFullObliqueSlice() const
      {
        return float4(GetDatasetRef().m_joinedSkeleton.GetNumOfElements())
          * (GetDatasetRef().GetCurrentDistanceFromRectumMM() / GetDatasetRef().m_segmentList.GetTotalLengthMM());
      } // GetFullObliqueSlice()

      /// polyp segmentation
      void SegmentRegion(const Point3D<float4>& selectPoint3D, const Point2D<int4>& mouseClickPt);

      ///  Record a single point SROI. This is for clinical trial creating a truth
      void RecordAPointOnly(const Point<float4>& selectPoint);

      /// storing SRT ROI voxels for session
      void RecordROIVoxels( vxCAD::vxCadROI & roi, const uint2 & uROIlabel, vxBlockVolume<uint2> & labelVol );

      /// finds the component label of the closest SRT ROI to the given point
      uint2 FindProximalROILabel(const CPoint & ptClient);

      /// returns the position of the closest SRT ROI to the given point, if any
      int4 FindProximalROI(const CPoint & ptClient);

      /// Update the suspicious ROI in the label and paint volume after user editing
      void UpdateSuspiciousROI( const int4 & iROIindex, const uint2 & uROIlabel );

      /// edit an existing segmented roi
      void EditROIRegion(const SEGMENTROIMODE eMode, const Point<float4> & selectPoint);

      /// convert volume coordinate to DICOM RCS world coordinate
      void ConvertVol2DicomWorldCoordinate( const Vector3D<int4> & viVolCoordinate, Vector3D<float4> & vfWorldCoordinate,
             const Triple<int4> & viDim, const Triple<float4> & vfUnits, const std::string & sPatientOrientation  );

      /// Remove a suspicious ROI from the list 
      void RemoveSingleSROIFromList( const uint2 & uLabelIndex, const int4 & iIndex );

      /// add voxel to selected ROI with brush size of the 1st order neighborhood
      void AddVoxelIn2ndOrderNeighbors( vxCAD::vxCadFinding & rFinding, vxBlockVolumeIterator<uint2> & itl, 
                     const uint2 & uROIlabel, vxCAD::vxCadVoxelPosition & poss );

      /// remove a single voxel from the VolumeROI list of a CAD finding
      void RemoveVoxelFromVolumeROI( vxCAD::vxCadFinding & rFinding, const vxCAD::vxCadVoxelPosition & pos );

      /// delete voxel to selected ROI with brush size of the 1st order neighborhood
      void DeleteVoxelIn2ndOrderNeighbors( vxCAD::vxCadFinding & rFinding, vxBlockVolumeIterator<uint2> & itl, 
                     const uint2 & uROIlabel, vxCAD::vxCadVoxelPosition & poss );

    // member variables
    public:

      /// pointer to its neighbour slice view.(Supine/Prone)
      SliceChildView * m_pNeighbourSliceView;
      
      /// Viewer for this slice
      ReaderLib::SliceViewer m_sliceViewer;

      /// if we need to update navigation view. 
      static bool m_bUpdateEndoluminalView;

      /// Debug views
      enum DbgShowEnum { SHOW_NONE = 0, SHOW_INS = 1, SHOW_DDB = 2, SHOW_CSI = 3, SHOW_PDR = 4, SHOW_BIOPSY = 5 };
      static DbgShowEnum m_eDbgShow;

      /// Coordinates display
      struct CoordDisplay
      {
        enum Enum { eNone = 0, eVolume, eWorld };
      };

      /// Coordinate display
      static CoordDisplay::Enum m_eCoordDisplay;

      /// Render flag
      bool m_bRenderThis;

    protected:

      /// Current Hounsfield Unit value (named after Sir Godfrey N. Hounsfield)
      int4 m_iCurrHounsfieldValue;

      /// If the current Hounsfield value is valid
      bool m_bCurrHounsfieldValueValid;
      
      /// The location of the current Hounsfield sample
      Point3D<float4> m_currHounsfieldSamplePosition;
      
      /// Last mouse-move point
      CPoint m_currMousePoint;

      /// Last clicked 2D Point
      CPoint m_currClickPoint;
      
      /// Current debugging scale
      float4 m_fDDBScale;
      
      /// Current window/level
      std::pair<int4,int4> m_wlNormal;
      /// Uncleansed/other window/level
      std::pair<int4,int4> m_wlSecondary;

      /// Last window/level mouse point
      CPoint m_windowLevelLastPoint;

      /// Whether or not a measurement is currently being updated
      bool m_bMeasureActive;

      /// Window/Level presets (Custom should always be the last entry
      enum WindowLevelPresetsEnum
      {
        eWLP_CTC, eWLP_First = 1, eWLP_Abdomen = 1, eWLP_TaggedFecal, eWLP_Lung, eWLP_Bone, eWLP_Liver, eWLP_Count
      };
      /// Current Window/Level preset
      WindowLevelPresetsEnum m_eCurrentWLPreset;

      /// Holds values of system window/level presets (0-preset value, 1-window, 2-level, 3-menu ID)
      static int4 m_veWindowLevelPresets[SliceChildView::eWLP_Count][4];

      /// Window/Level preset menu
      WinUtils::GLPopupMenu m_menuWinLev;

      /// Slice axis orientation menu
      WinUtils::GLPopupMenu m_menuSliceOrientation;

      /// Zoom menu
      WinUtils::GLPopupMenu m_menuZoom;

      /// Render mode
      WinUtils::GLPopupMenu m_menuRenderMode;

      /// the setting of this variable is for unnecessary calculation of HU value.
      bool m_bMouseWheelCalculate;

      /// if HU is updated response to MouseWheel, no further calculation in MouseMove.
      bool m_bMouseWheelUpdated; 

      /// update ROIMeasurement;
      bool m_bUpdateRoiMeasurement;

      /// control the speed of 2D flight;
      int4 m_iSpeedAccum;

      /// whether in full oblique mode
      bool m_bFullOblique;

    private:

      /// Pointer to the last Dataset instance that was rendered, to detect the need to update the view
      ReaderLib::Dataset* m_pDatasetLastRendered;

      /// the slice on which the ROI was computed
      int4 m_iRoiSlice;

      /// Handle to the accompanying scrollbar
      HWND m_hScrollbar;

      /// true if the scrollbar is horizontal, false if vertical
      /// (note: this is needed b/c a bug is causing the scroll orientation to change, this will force the orientation
      bool m_bHorzScroll;

      /// Currently-selected annotation (for moving, renaming, deleting, etc.)
      AnnotationArrowRef m_pCurrAnnotation;

      /// Check for mouse over an annotation during mouse-move event
      AnnotationArrowRef m_pMouseOverAnnotation;

      /// Whether the world view location is currently selected (for moving of location and orientation)
      bool m_bCurrMovingWorldLoc;

      /// Check for mouse over world view location
      bool m_bMouseOverWorldLoc;

      /// Current screen point during click-and-move-to-scroll-slice operation
      Point2D<int4> m_dragscrollPt;

      /// Whether or not to display the HU value at the current mouse cursor point
      bool m_bShowHUValue;

      /// Timer for rendering frame rate display
      Timer m_timerFrameRate;

      /// Counter to help debug multiple image redraws
      int m_iDbgCounter;

      /// Timer to help debug rendering problems
      Timer m_debugTimerOutside;

      /// If this viewer is in autofly mode
      bool m_bAutoflyMode;

      /// Updates the endoluminal view
      bool m_bFirstMouseMove;

      /// cached value of whether the window width increases to the right
      static bool m_bWindowWidthIncreaseRight;

      /// for window/level user settings to be shared by all instances
      static ColonViews::TransSettingsMap m_mapWLSettings;
      /// whether user settings have been initialized (window level as well as other slice settings)
      static bool m_bSettingsInitialRead;

      /// text for quick-drop annotations
      static std::string m_sQuickAnnotationName;

      /// the current ROI segmentation editing mode
      static SEGMENTROIMODE m_eROIEditMode;
      /// current voxel size used for ROI editing
      static uint4 m_uROIEditVoxelSize;

      /// simple button bar class
      class GLSButton { public: GLSButton(){}; GLSButton(Rect<int4> r, std::string s):m_rect(r),m_text(s){}; Rect<int4> m_rect; std::string m_text; };
      class GLSButtonBar : public std::map<int4, GLSButton >
      {
      public:
        GLSButtonBar() {}
        /// returns the button in which the point is located
        int4 HitTest(const Point2D<int4>& ptTest) const;
      public:
        /// size of each button
        Point2D<int4> m_szBtn;
      }; // ButtonBar

      /// the display rectangles of the voxel size selection buttons
      GLSButtonBar m_btnbarVoxelSize;
      /// the voxel edit mode selection buttons
      GLSButtonBar m_btnbarEditMode;

      /// timer to update roi after editing
      WinUtils::GLTimer m_timerUpdateROI;
      /// ROI update location
      Point<float4> m_ptUpdateROI;

      /// location of the scrollbar in relation to the window
      static SCROLLBARLOCATIONENUM m_eScrollLoc;

      /// used to reinstate current segment polyp mode when switching views
      static ViewWndTypeEnum m_eSegmentPolypModeView;

      /// is fullscreen
      bool m_bFullScreen;

    }; // class SliceChildView
  } // namespace ColonViews
} // namespace ChildViews
