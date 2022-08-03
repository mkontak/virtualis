// $Id: NavigationView.h,v 1.34.2.9 2010/11/01 12:40:55 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Jeffrey Meade (jmeade@viatronix.com)


/**
 * Interface for the EndoluminalView class.
 */


// pragmas
#pragma once


// includes
#include <vector>
#include <list>
#include <string>
#include "Vector.h"
#include "ViatronChildView.h"
#include "resource.h"
#include "Rect.h"
//#include "vxOverlayStrings.h"
#include "GLTimer.h"
#include "ProgressCtrl.h"

// forward declarations
template <class T> class Image;
template <class T> class PixelRGBA;


// namespaces
namespace ColonViews
{

  namespace ChildViews
  {

    /**
     * Progress bar control that includes a label and number text display
     */
    class EndoCoverageProgressCtrl : public WinUtils::ProgressCtrl
    {
    public:
      /// Creates the control.
      BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT uID);
      /// Sets the percentage (Note: uses basic types because it is an override).
      int SetPos(int iPos);

      /// show/hide control
      BOOL ShowWindow(int uCmdShow);

      /// Called when window is moved to place controls  (Note: uses basic types because it is an override).
      void MoveWindow(int x, int y, int iWidth, int iHeight, BOOL bRepaint = TRUE);
      /// Called when window is moved to place controls  (Note: uses basic types because it is an override).
      void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);

      /// Overrides
      /// ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(EndoCoverageProgressCtrl)
    protected:
      //}}AFX_VIRTUAL

      /// Generated message map functions
    protected:
      //{{AFX_MSG(EndoCoverageProgressCtrl)
      //}}AFX_MSG

      DECLARE_MESSAGE_MAP()


    private:
      /// Progress number (percentage) display
      CStatic m_buddyNumberStatic;
      /// Control label
      CStatic m_labelStatic;
      /// Format for the percent coverage numerical value display
      int1 m_sCvgValueFmt[11];

      friend class EndoluminalView;
    }; // EndoCoverageProgressCtrl


    /**
     * Information about a hotspot around a finding
     */
    struct FindingHotspot
    {
    public:

      /// the hot spot
      Rect<int4> m_hotspot;

      /// the number of this finding
      int4 m_iNumber;

      /// the opacity of the finding hotspot
      float4 m_fHotspotOpacity;

      /// the opacity of the arrow
      float4 m_fArrowOpacity;

      /// the visibility of the finding
      ColonSession::FindingInfo m_findingInfo;

      /// the distance of the finding from the eye (for z-sorting)
      float4 m_fDepth;

      /// the distance of the finding from the cursor
      float4 m_fCursorDistance;

      /// the ID of the finding
      std::string m_sID;

    public:

      /// comparison function for hotspots
      struct SortAscendingDepth : public std::binary_function< FindingHotspot, FindingHotspot, bool >
      {
        bool operator()( FindingHotspot & x, FindingHotspot & y );
      }; // comparison fn

      /// comparison function for hotspots
      struct SortAscendingCursorDistance : public std::binary_function< FindingHotspot, FindingHotspot, bool >
      {
        bool operator()( FindingHotspot & x, FindingHotspot & y );
      }; // comparison fn

    }; // struct FindingHotspot


    /**
     * V3D Colon Endoscopic View
     */
    class EndoluminalView : public ViatronChildView
    {
      /// inner classes
    private:

      /// class to hold the data to display a dimension for a measurement
      class MeasurementDisplayData
      {
      public:

        /// constructor for simply displaying a text label with no measurement
        MeasurementDisplayData( const std::string & sJustALabel ) : m_sLongLabel( sJustALabel ), m_bDisplayDimension( false ) { }

        /// constructor for a regular dimension display
        MeasurementDisplayData( const std::string & sLongLabel, const std::string & sShortLabel,
          const vxCAD::vxCadDimension & dimension, const float4 fOpacity )
          : m_sLongLabel( sLongLabel ),
          m_sShortLabel( sShortLabel ),
          m_dimension( dimension ),
          m_bDisplayDimension( true ),
          m_fOpacity( fOpacity )
        {
        } // constructor

        /// get if the dimension should be displayed
        const bool & GetDisplayDimension() const { return m_bDisplayDimension; }

        /// get the dimension
        const vxCAD::vxCadDimension & GetDimension() const { return m_dimension; }

        /// get the short label
        const std::string & GetShortLabel() const { return m_sShortLabel; }

        /// get the long label
        const std::string & GetLongLabel() const { return m_sLongLabel; }

        /// get the opacity
        const float4 GetOpacity() const { return m_fOpacity; }

      private:

        /// the long form of the label (e.g., "Maximum Dimension: " or "Sample Measurements")
        std::string m_sLongLabel;

        /// the short form of the label displayed next to the 3D measurement (e.g., "max")
        std::string m_sShortLabel;

        /// the dimension data, if any (leave at default if not measurement is desired)
        vxCAD::vxCadDimension m_dimension;

        /// if the dimension and short label should be displayed or not
        bool m_bDisplayDimension;

        /// the opacity of the dimension and text (0, 1)
        float4 m_fOpacity;

      }; // class MeasurementDisplayData

    // functions
    public:

      /// Constructor
      EndoluminalView(HWND hWndParent, const ReaderLib::DatasetOrientations::TypeEnum eOrient, const bool bVerticalLumenCoverageControl = false);

      /// Destructor
      virtual ~EndoluminalView();

      /// Retrieves update image; DO NOT DELETE DATA!
      virtual const void far * GetUpdateBuffer(int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat = GL_BGR_EXT,
        const GLenum eDatatype = GL_UNSIGNED_BYTE, const bool bOnlyGLRenderScene = false);

      /// Gets the user's preferred (saved) flight speed from the registry
      static float4 GetUserPreferredFlightSpeed();

      /// Saves the user's preferred flight speed to the registry
      static void SetUserPreferredFlightSpeed(float4 fSpeed);

      /// Show or hide lumen coverage fields
      void ShowLumenCoverage(const bool bShow);

    protected:

      /// called before the current layout changes (save values)
      virtual void OnLayoutChanging();

      /// called after the current layout changes (load values)
      virtual void OnLayoutChanged();

      /// Renders the view.
      virtual void GLRenderScene(void * pData = NULL);

      /// Sets up the rendering context
      virtual void GLSetupRC();

      /// Called during view resize, allows validation/alteration of new size
      virtual void OnSizing(const UINT uUnused, Rect<int4>& viewRect);

      /// Called on view resize.
      virtual void GLResize(GLsizei iWidth, GLsizei iHeight);

      /// Idle event update.
      virtual bool GLIdle();

      /// 2D rendering in view.
      virtual void GLRasterScene();
      virtual bool GLCreate();

      /// Key down handler.
      virtual void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// Key up handler
      virtual void OnKeyUp(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// Left-button double-click handler.
      virtual void OnLButtonDblClk(UINT uFlags, CPoint point);

      /// Left-button down handler.
      virtual void OnLButtonDown(UINT uFlags, CPoint point);

      /// Left-button up handler.
      virtual void OnLButtonUp(UINT uFlags, CPoint point);

      /// Middle button down handler.
      virtual void OnMButtonDown(UINT uFlags, CPoint point);

      /// Middle button up handler.
      virtual void OnMButtonUp(UINT uFlags, CPoint point);

      /// Mouse move handler.
      virtual void OnMouseMove(UINT uFlags, CPoint point);

      /// Mouse wheel handler.
      virtual BOOL OnMouseWheel(UINT uFlags, short iZDelta, CPoint pt);

      /// Right button down handler.
      virtual void OnRButtonDown(UINT uFlags, CPoint point);

      /// Right button up handler.
      virtual void OnRButtonUp(UINT uFlags, CPoint point);

      /// Right button double click handler.
      virtual void OnRButtonDblClk(UINT uFlags, CPoint point);

      /// size handler
      virtual void OnSize(UINT uType, int iCX, int iCY );
      /// called immediately before window pos change event
      virtual void OnWindowPosChanging( WINDOWPOS * pWndPos );
      /// called immediately after window pos change event
      virtual void OnWindowPosChanged( WINDOWPOS * pWndPos );

      /// Called when mode changes
      virtual void OnViewModeChange(const uint4 uOldMode, const uint4 uNewMode);

      /// Sets cursor for view.
      BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);

      /// Gets context-based popup menu ID.
      uint4 GetPopupMenuID(UINT uFlags, CPoint point) const;

      /// Retrieves context-based strings for mouse use.
      virtual void GetMouseKeyStringIDs(uint4 & uLeftID, uint4 & uMidID, uint4 & uRightID) const;

      /// handles timer event
      virtual void OnTimer(const uint4 uIDEvent);

    private:

      /// Returns a 3D coordinate from the specified 2-D point.
      Point3D<float4> Calculate3DPoint(const Point2D<float4> & point2D) const;

      /// Tries to return a 3D coordinate that is visible from the specified 2-D point.
      Point3D<float4> CalculateVisible3DPoint(const Point2D<float4> & point2D) const;

      /// Projects a 3D point to 2D coordinates.
      Point2D<int4> Project3DPoint(const Point3D<float4> & point) const;

      bool Is3DPtVisible(const Point3D<float4> & pt3D, CPoint & pt2D) const;

      /// draw all measurement lines in 3D.
      void Render3DMeasurementLines(const Point<float4> & viewpoint);

      /// draw a single measurement line in 3D
      void Render3DMeasurementLine(MeasureLine & line, const bool bCurrent, const bool bAdjustable,
        const Point<float4> viewpoint, const PixelRGBA<float4> & lineColor);

      /// Draws text measurements of all measurement lines.
      void RenderTextMeasurementLines(const Rect<int4> & clientRect);

      /// Draws annotations from 2D slices in the 3D view
      void DrawAnnotations(const float4 * pfDepthBuffer, const uint4 uRenderSize, const uint4 uRenderBufferSize, const Point<float4> viewpoint);

      /// Draws cad findings
      void DrawCadFindings(const float4 * pfDepthBuffer, const uint4 uRenderSize, const uint4 uRenderBufferSize, const Point<float4> viewpoint);

      /// Initializes navigation view parameters that are saved across VC sessions
      void InitSessionParams();

      /// pickup measurement line.
      bool PickupMeasurementLine(const CRect & rect, const CPoint & point, const bool bPickAndMove);

      /// get the nearest annotation proximal to the given point within the view
      ReaderLib::BookmarkList::iterator EndoluminalView::GetProximalAnnotation(const CRect & rect, const CPoint & point);

      /// Suspicious region segmentation method
      void SegmentRegion(const Point<float4> & selectPoint);

      /// Record a single point SROI. This is for clinical trial creating a truth
      void RecordAPointOnly(const Point<float4>& selectPoint);

      /// storing SRT ROI voxels for session
      void RecordROIVoxels( vxCAD::vxCadROI & roi, const uint2 & uROIlabel, vxBlockVolume<uint2> & labelVol );

      /// render a list of dimensions
      void RenderDimensions( const std::list< MeasurementDisplayData > & dataList, const Point<float4> & viewpoint );

      /// renders measurements for the given cad finding
      void RenderFindingMeasurements( const vxCAD::vxCadFinding & finding, const Point<float4> & viewpoint );

      /// search for the nearest supicious or CAD ROI
      uint2 SearchNearestROILabel( const Vector3D<int2> & viPos );


    public:

      /// Maximum endoscopic flight speed
      static uint4 m_uMaxFlightSpeed;

      /// kludge, to draw current ROI during snapshot
      static bool m_bForceDrawCurrentROI;

      /// true if user is currently changing the FOV
      bool m_bChangingFOV;

#if defined(FINAL_RELEASE)
      /// Whether or not to display render speed statistics (in release builds)
      static bool m_bDisplayRenderSpeed;
#endif

    private:

      /// Number of steps currently taken in the "wrong" direction
      uint4 m_uWrongWayCount;

      /// Number of steps before the centerline will change color
      static const uint4 m_uWrongWayFrameThreshold;

      /// distinguish pick - pick & move
      bool m_bPickAndMove;

      /// Last point at which the translucent window was displayed.
      Point2D<int4> m_lastBiopsyPt;

      /// Lumen coverage progress display
      EndoCoverageProgressCtrl m_progressCtrl;

      /// end point of measurement line (3D).
      GLUquadricObj * m_pQuadObject;

      GLFont m_medFont;

      /// the current percentage of colon seen
      float4 m_fPercentCoverage;

      /// a set of click targets on-screen to let the user enable or disable cad findings
      std::vector< FindingHotspot > m_findingHotspots;

      /// if the user is currently selecting a hotspot
      bool m_bSelectingHotspot;

      /// the distance of the near clipping plane
      static const float4 m_fNearClipPlane;

      /// the images to show finding options
      Image< PixelRGBA<uint1> > * m_pFindingImages[3];

      /// the last opacity of the hotspots
      float4 m_fAverageHotspotOpacity;

      /// measurement text to display
      std::list< std::string > m_vsMeasurementText;

#if !defined(FINAL_RELEASE)
      /// Current right-double-click animation
      uint4 m_iRdbcAnim;
      /// Current right-double-click point
      Point2D<int4> m_rdbcPoint;
#endif

      /// annotation selected for moving/editing
      ReaderLib::BookmarkList::iterator m_pCurrentAnnotation;

      /// display indicator for info of suspicious ROI
      bool m_bDisplaySuspiciousROI;

      /// timer to temporarily display slice position indicators
      WinUtils::GLTimer m_timerOrthogonalPtFix;

      /// temporarily saves the states of the slice shadows
      bool m_vbSliceShadowStateSave[4];

    }; // class EndoluminalView

  } // namespace ColonViews

} // namespace ChildViews


// $Log: NavigationView.h,v $
// Revision 1.34.2.9  2010/11/01 12:40:55  dongqing
// add comments on Mark's fix for the Win 7 progress bar of 3D fly thru
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.34.2.8  2010/10/29 13:07:27  mkontak
// New progress bar to fix the flashing that occurs in the NavigationView. Commented
// out the Invalidate of the entire Frame on the Verify view to fix the flashing
// during the volume loading.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.34.2.6  2009/11/18 23:17:21  dongqing
// put the hot spot right on top the finding surface
//
// Revision 1.34.2.5  2009/10/07 16:13:14  dongqing
// Changing class references for the endoluminal child view: EndoluminalView <== NavigationView
//
// Revision 1.34.2.4  2008/04/02 18:51:08  dongqing
// remove none use function definition
//
// Revision 1.34.2.3  2008/03/03 20:40:17  dongqing
// add function for record single point user seleced in the SROI list
//
// Revision 1.34.2.2  2007/11/08 16:54:44  jmeade
// temporarily display slice shadows after ctrl-clicking to set slice positions.
//
// Revision 1.34.2.1  2007/07/31 22:23:33  jmeade
// Hitting the 'F' key toggles between 90* and 120* rotation angles.
//
// Revision 1.34  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.33  2007/03/10 05:06:25  jmeade
// code standards.
//
// Revision 1.32  2007/03/09 22:01:43  jmeade
// Issue 5278: drawing srt measurement during snapshot.
//
// Revision 1.31  2007/03/08 22:22:04  romy
// code review - header files adjusted
//
// Revision 1.30  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.29  2007/02/06 16:43:14  jmeade
// user data in render method.
//
// Revision 1.28  2007/01/23 20:17:42  dongqing
// add functionto handle the SRT session save and load
//
// Revision 1.27  2006/10/04 03:10:04  jmeade
// Issue 4905: Print current and default field of view values during adjustment.
//
// Revision 1.26  2006/10/03 19:29:46  frank
// added FOV adjustment
//
// Revision 1.25  2006/09/28 20:30:04  jmeade
// segment region method.
//
// Revision 1.24  2006/09/22 20:48:02  dongqing
// adjust the annotation for flat lesion
//
// Revision 1.23  2006/09/18 19:47:41  jmeade
// Issue 4916: trigger mouse cursor change when view mode changes.
//
// Revision 1.22  2006/09/14 18:24:28  dongqing
// tune up the maximum diameter measurement
//
// Revision 1.21  2006/09/14 13:03:33  frank
// refactored rendering to allow polyp measurement text
//
// Revision 1.20  2006/06/01 20:21:45  frank
// updated to visual studio 2005
//
// Revision 1.19  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.18  2006/01/31 19:32:23  romy
// build errors fixed
//
// Revision 1.17  2006/01/31 18:21:32  romy
// code review-rerranged include files
//
// Revision 1.16  2006/01/31 14:50:38  frank
// split up volume measure file
//
// Revision 1.15  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.14  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.13  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.12.2.7  2006/09/18 19:58:01  jmeade
// Issue 4916: trigger mouse cursor change when view mode changes.
//
// Revision 1.12.2.6  2006/03/22 22:51:54  jmeade
// Issue 4667: restrict registration synchronization to views where both datasets are visible, and a link button is available.
//
// Revision 1.12.2.5  2006/03/22 21:01:55  frank
// moved the finding visibility and other information into a class
//
// Revision 1.12.2.4  2006/01/23 14:51:11  frank
// showed expanded information about CAD finding measurements
//
// Revision 1.12.2.3  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.12.2.2  2005/11/16 02:55:27  frank
// improved display of cad measurements - added labels
//
// Revision 1.12.2.1  2005/11/10 01:28:39  jmeade
// Fixed bug saving session values between views and across sessions.
//
// Revision 1.12  2005/11/03 21:28:12  frank
// added cad finding measurements if available
//
// Revision 1.11  2005/10/07 14:49:23  vxconfig
// stopped it from flying when selecting finding hotspots
//
// Revision 1.10  2005/10/06 13:40:05  vxconfig
// fixed visibility and opacity of arrows and numbers
//
// Revision 1.9  2005/10/06 05:00:31  vxconfig
// Fixed a number of issues
//
// Revision 1.8  2005/10/04 18:49:15  vxconfig
// updated view when not flying
//
// Revision 1.7  2005/10/04 17:38:25  vxconfig
// added hotspots in 3D for showing/hiding the findings
//
// Revision 1.6  2005/09/30 18:06:20  vxconfig
// formatting
//
// Revision 1.5  2005/09/22 18:26:25  jmeade
// draw cad findings.
//
// Revision 1.4  2005/09/09 14:31:01  geconomos
// Added a governor that regulates the update of the endoluminal coverge indicator.
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
// Revision 3.26.2.9.2.4.2.3  2005/07/15 18:25:44  geconomos
// Added methods to invalidate parent window where external controls are placed.
//
// Revision 3.26.2.9.2.4.2.2  2005/07/13 19:23:31  geconomos
// Issue# 4263: Endoluminal coverage label flickering while flying
//
// Revision 3.26.2.9.2.4.2.1  2005/06/30 16:56:16  geconomos
// Issue: #4234 not displaying warning when not using space leaping
//
// Revision 3.26.2.9.2.4  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.26.2.9.2.2.2.1  2005/05/25 15:50:22  geconomos
// initial backup
// Revision 3.26.2.9.2.2  2005/04/19 17:42:19  geconomos
// separated OpenGL and Windows resize functions
//
// Revision 3.26.2.9.2.1  2005/04/13 20:48:02  frank
// Issue #4108: Working on linear measurement
//
// Revision 3.26.2.9  2005/02/15 21:02:32  jmeade
// merge from vc1.4 branch.
//
// Revision 3.26.2.8  2004/11/20 15:28:00  dongqing
// back using the paint volume
//
// Revision 3.26.2.7  2004/09/30 15:17:31  jmeade
// Interaction for polyp segmentation.
//
// Revision 3.26.2.6.2.1  2005/01/27 22:47:05  jmeade
// Hide lumen coverage controls on movie view.
//
// Revision 3.26.2.6  2004/05/15 00:35:58  jmeade
// Allow storage/adjustment of all biopsy/transluncency values.
//
// Revision 3.26.2.5  2004/05/05 23:16:28  jmeade
// Incremental update.
//
// Revision 3.26.2.4  2004/04/21 03:59:09  jmeade
// Adjust translucent rending dialog.
//
// Revision 3.26.2.3  2003/06/02 21:47:44  jmeade
// Issue 3262: Route and handle view mode change message thru MainAppFrame to ensure it'll always get handled; comments.
//
// Revision 3.26.2.2  2003/04/15 21:55:10  jmeade
// Issue 3001: Always resize to square dimensions.
//
// Revision 3.26.2.1  2003/02/27 02:17:34  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.26  2003/01/30 16:56:25  jmeade
// Localized registry storage of all flight params to EndoluminalView.
//
// Revision 3.25  2003/01/21 19:34:26  jmeade
// Duplicate: Revision : 3.11.2.8: Size lumen coverage labels appropriately by (language-specific) text.
//
// Revision 3.24  2003/01/03 17:36:21  kevin
// Adjust Debug output for endoscopic frame rate to also include
// adaptive render size for the current frame and the average since the
// viewer started.
//
// Revision 3.23  2002/12/23 22:27:35  kevin
// Store the frame rate for adjusting the render-size
//
// Revision 3.22  2002/12/17 22:50:00  jmeade
// Override of GetUpdateBuffer, for crisper images for movies.
//
// Revision 3.21  2002/11/04 20:38:19  kevin
// Mad annotations 3D instead of 2D and added drawing of annotations in endoscopic view
//
// Revision 3.20  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.18  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.17  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.16  2002/04/18 18:27:45  jmeade
// More descriptive measure line render function names.
//
// Revision 3.15  2002/04/12 02:13:05  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.14  2002/03/28 18:50:48  jmeade
// Allow run-time setting of maximum flight speeds; Code standards.
//
// Revision 3.13  2002/03/21 02:12:30  jmeade
// Display render speed service menu option; Code standards.
//
// Revision 3.12  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.11.2.6  2002/06/12 02:29:56  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.11.2.5  2002/03/28 22:28:14  jmeade
// Display render speed service menu option; Code standards.
// Allow run-time setting of maximum flight speeds; Code standards.
//
// Revision 3.11.2.4  2002/03/05 20:17:26  binli
// adjustment of 3D measurement line drawing.
//
// Revision 3.11.2.3  2002/03/01 17:01:25  binli
// issue 1966: when measurement line is visible, default to 'polyp'
//
// Revision 3.11.2.2  2002/02/19 21:33:01  binli
// improvement of 3D measurement line drawing: accurate pickup
//
// Revision 3.11.2.1  2002/02/06 21:35:02  jmeade
// Comments.
//
// Revision 3.11  2001/12/31 16:03:09  binli
// 3D measurement line: adjustment:
// 1> L-Btn down: adjust exist line/start new line
// 2> R-Btn down: view a exist line (visible & invisble)
// 3> R-Btn dbl-click: delete line(s)
//
// Revision 3.10  2001/12/28 16:31:17  binli
// 3D measurement line: improve 2: picking on L-btn down: show invisible or move end-point
//
// Revision 3.9  2001/12/27 20:07:40  binli
// 3D measurement line: adjustment
//
// Revision 3.8  2001/12/22 00:04:35  jmeade
// Moved movie recording (for use in other views); Code standards.
//
// Revision 3.7  2001/12/21 23:02:49  binli
// 3D measurement line: basic function: improve 1: picking.
//
// Revision 3.6  2001/12/20 22:44:28  binli
// 3D measurement line: basic function (to be improved)
//
// Revision 3.5  2001/11/21 19:09:04  jmeade
// Handle view mode change in Navigation view.
//
// Revision 3.4  2001/11/21 00:30:09  jmeade
// Lumen coverage.
//
// Revision 3.3  2001/10/19 02:45:49  jmeade
// Code standards
//
// Revision 3.2  2001/10/18 15:11:05  binli
// issue 1123: added a flag to avoid unnecessary rotation.
//
// Revision 3.1  2001/10/17 21:57:03  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 10 2001 17:10:44   jmeade
// One frame rate timer per class instance
// 
//    Rev 2.1   Oct 08 2001 16:54:06   jmeade
// Consolidated code specifying study orientation; allow creator of EndoluminalView to specify study orientation
// 
//    Rev 2.0   Sep 16 2001 23:41:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:20   ingmar
// Initial revision.
// Revision 1.32  2001/08/14 18:23:18  jmeade
// replaced m_bButtonDown with proper AsyncKeyState check
//
// Revision 1.31  2001/05/29 17:14:47  jmeade
// tabs to spaces; no need for m_bMovingBiopsy
//
// Revision 1.30  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.29  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.28  2001/04/11 20:50:27  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.27  2001/03/08 02:49:28  jmeade
// more fun non-release animations (first cuts)
//
// Revision 1.26  2001/02/23 20:11:31  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.25  2001/02/19 17:18:07  jeff
// OnKeyPressed() is obsoleete, identical to OnKeyDown()
//
// Revision 1.24  2001/02/15 13:46:46  frank
// Added double click function.
//
// Revision 1.23  2001/01/31 22:20:32  geconomos
// Implemented virtual GetUpdateBuffer()
//
// Revision 1.22  2001/01/31 13:23:36  frank
// Upgraded to modern data structures.
//
// Revision 1.21  2001/01/13 00:40:32  jeff
// lines for classwizard (no functionality change)
//
// Revision 1.20  2001/01/12 21:49:27  frank
// Consolidate code.
//
// Revision 1.19  2001/01/12 19:52:37  frank
// Moved code to ReaderGlobal
//
// Revision 1.18  2001/01/12 17:59:36  frank
// Added linear profile rendering.
//
// Revision 1.17  2001/01/02 17:28:53  jeff
// removed unused CNavigationView::InfoDlgBar class
//
// Revision 1.16  2000/11/23 17:04:57  kevin
// Crutch to fix jumping on menu-leave (basically getting mouse
// move with a previous button-down event)
//
// Revision 1.15  2000/11/23 00:55:25  jeff
// Removed unused EndoCoverageProgressCtrl::OnMove() function; attempt to
//  create progress control *before* statics; call MoveWindow() from Create() function;
//  use global OpenGL font
//
// Revision 1.14  2000/11/21 02:41:34  jeff
// Added label to progress control;
// Overrode MoveWindow() to size label, progress, and buddy within specified
//  view rectangle for the progress control
//
// Revision 1.13  2000/11/20 17:59:48  jeff
// Code for static next to progress control
//
// Revision 1.12  2000/11/20 16:59:21  jeff
// Percent examined (progress) bar
//
// Revision 1.11  2000/11/20 01:07:18  jeff
// GetMouseKeyStringIDs() -- allows each view to set strings for mouse button use key
//
// Revision 1.10  2000/11/09 02:55:28  kevin
// Many changes to different methods of visible surface determination
//
// Revision 1.9  2000/11/01 22:44:59  jeff
// Flags and point of the last mouse click passed into GetMenuPopupID()
// (for the child view to use to decide which popup menu to show)
//
// Revision 1.8  2000/10/30 13:37:45  kevin
// Added Biopsy Mode for moving the magic window around
//
// Revision 1.7  2000/10/29 20:17:37  kevin
// CHanged CalculateMeasurePoint to Calculate 3DPoint
// and added jump orthogonal position to mouseclick
//
// Revision 1.6  2000/10/26 20:51:27  jeff
// Persistent list of measurements (stored in ReaderGlobal)
//
// Revision 1.5  2000/10/25 17:39:11  jeff
// New popup menu paradigm for view windows
//
// Revision 1.4  2000/09/15 01:58:25  jeff
// Moving to text draw in OpenGL
//
// Revision 1.3  2000/09/13 16:31:18  jeff
// Removing unnecessary references to obsolete CChildView class
//
// Revision 1.2  2000/09/12 20:47:03  jeff
// Resizing and repositioning overview and (coming soon) visible-surface views
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 20    9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 19    8/31/00 2:13p Frank
// Added double-click autofly start
// 
// 18    8/29/00 6:42p Jeff
// Mode changes
// 
// 17    8/28/00 2:31p Antonio
// updated header and footer
// 
// *****************  Version 16  *****************
// User: Jeff            Date:  8/25/00  Time:  7:08p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added popup menu
// 
// *****************  Version 15  *****************
// User: Kevin           Date:  8/20/00  Time:  1:48p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Fixed centerline coloring
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 14  *****************
// User: Kevin           Date:  8/14/00  Time:  8:33p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added RGB marked volume writing for Revli viewing
// 
// *****************  Version 13  *****************
// User: Frank           Date:  8/08/00  Time:  6:40p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Jeff: (don't worry Frank, you didn't make this change!)
// SwapBuffers() moved back to CChildView::OnPaint(), new function override
// PostGLRender() can do any device context (HDC) drawing to the window
// 
// *****************  Version 12  *****************
// User: Jeff            Date:  8/02/00  Time: 11:09a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added a highlight cursor; removed references to local navigation
// mode (now stored in GuiNavigation class)
// 
// *****************  Version 11  *****************
// User: Jeff            Date:  7/27/00  Time:  2:34p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added endoscopic view mode toggles to GUI
// Smoother updating of Overview dialog
// BroadcastSendMessage changed to BroadcastMessage(..., bWait)
// 
// *****************  Version 10  *****************
// User: Jeff            Date:  7/24/00  Time:  7:52p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added bookmark class and associated hooks,
// (initially) only for jumping to fixed precomputed positions
// 
// *****************  Version 9  *****************
// User: Kevin           Date:  7/22/00  Time: 10:30a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added hooks for adaptive Smapling
// 
// *****************  Version 8  *****************
// User: Kevin           Date:  7/21/00  Time:  8:37p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added CDC drawing to place symbology over the axial slices and the
// distance measurements
// 
// *****************  Version 7  *****************
// User: Kevin           Date:  7/20/00  Time: 10:17p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added Moving Lights and ifxed some distance measuring stuff
// 
// *****************  Version 6  *****************
// User: Jeff            Date:  7/20/00  Time: 12:43p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added a patient info header in navigation view
// 
// *****************  Version 5  *****************
// User: Kevin           Date:  7/20/00  Time:  9:33a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added Measurment and Moving Lights hooks into navigation
// 
// *****************  Version 4  *****************
// User: Jeff            Date:  7/18/00  Time:  1:58p
// Branched at version 4
// Comment:
//   
// 
// *****************  Version 3  *****************
// User: Frank           Date:  7/18/00  Time:  1:22p
// Checked in $/Viatron1000/Viatron
// Comment:
//   Implemented middle mouse lookaround
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/14/00  Time:  5:42p
// Checked in $/Viatron1000/Viatron
// Comment:
//   Filtering KEYDOWN events to the view window containing the mouse
// cursor
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/12/00  Time:  9:14p
// Created NavigationView.h
// Comment:
//   
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/NavigationView.h,v 1.34.2.9 2010/11/01 12:40:55 dongqing Exp $
// $Id: NavigationView.h,v 1.34.2.9 2010/11/01 12:40:55 dongqing Exp $
