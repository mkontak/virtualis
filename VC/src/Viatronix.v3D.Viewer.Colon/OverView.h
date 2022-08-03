// $Id: OverView.h,v 1.14.2.1 2009/01/12 21:58:38 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Bin Li (binli@viatronix.net); Kevin K. and Jeff.
//

/*
 * Description : a class to display the overview colon and support segments sorting 
 * This class 1> gives user a overview of the objects: colon/skeleton/bookmarks/
 *                    planes(A,S,C,Cross-Sectional)/framebox;
 *            2> supports multi-mode rendering: solid/translucent.
 *            2> supports interactive segments sorting.
 *            3> supports simply registration.
 */

#if !defined(Overview_h)
#define Overview_h

//#include "ViatronChildView.h"
//#include "ReaderGlobal.h"
//#include "Dataset.h"
//#include "ViatronStudy.h"
//#include "RenderContext.h"
#include <list>
#include "OverviewRender.h"
#include "ReportViewWnd.h"


namespace ColonViews
{

  namespace ChildViews
  {

    /**
     * Child view giving external view of colon surface.
     */
    class OverView : public ViatronChildView
    {
    public:
      /// flag to save image for report.
      enum SaveReportImageEnum { eSaveNone, eSaveVerification, eSaveStandard };

    public:
      /// constructor
      OverView(HWND hWndParent, const ReaderLib::DatasetOrientations::TypeEnum eOrientn = ReaderLib::DatasetOrientations::ePRIMARY,
        const bool bBufferImage = false);
      /// destructor
      virtual ~OverView();

      /// Initialize common rendering characteristics
      void Initialize(OverView * pNeighbour = NULL, ReaderLib::TrackBall * pTrackball = NULL,
        const ReaderLib::OverviewRender::RenderModeEnum eMode = ReaderLib::OverviewRender::eNormal);

      /// Refresh image.
      inline void RefreshImage(const uint4 iTimeClick=10);

      /// Sets a location for an arrow image that will display in a report snapshot
      inline void SetReportArrow(const Point<float4> & positionPt, const Normal<float4> & dirNormal);
      /// Sets a location for the colon segment to be highlighted in a report snapshot
      inline void SetReportSegment(const Point<float4> & beginPt, const Point<float4> & endPt);
      /// whether or not to display report arrow
      inline void SetRenderReportArrow(const bool bNewVal);
      /// whether or not report arrow is currently set/displayed
      inline bool GetRenderReportArrow() const;

      /// set current viewing mode: normal/verify/picking.
      inline void SetRenderMode(const ReaderLib::OverviewRender::RenderModeEnum eRenderMode);

      /// get current render viewing mode
      inline const ReaderLib::OverviewRender::RenderModeEnum GetRenderMode() const;

      /// Retrieve the pointer for its neighbour view.
      inline OverView * GetNeighbourView() const;

      /// Set neighbour view.
      inline void SetNeighbourView(OverView * pOverView);

      /// set up flag to update modelview matrix.
      inline void SetModelViewMatrixUpdateFlag(const bool bUpdateModelViewMatrix);

      /// Deletes the display lists (in overview render); previously named SwapDisplayLists()
      ///  which is a naming not of *what* it does but rather *where* it was being used.
      inline void DeleteDisplayLists();

      /// Turn on/off display list.
      inline void SetDisplayListRender(const bool bRefDisplayList);

      /// match current rendering mode with selected mode.
      inline bool IsMode(const ReaderLib::OverviewRender::RenderModeEnum eOVmode) const;

      /// test if the this slice: A/S/C/CS, shoule be rendered.
      inline bool IsRenderThisSlice(const VtxViewModes & viewModes) const;

      /// call this function to save overview image, when dataset is first loaded, on next render.
      void SetToSaveReportImage(int4 & iWidth, int4 & iHeight, const SaveReportImageEnum eSaveImageMode);

      /// Adds non-distended areas to the report
      bool AddNonDistendedAreas(ReaderLib::Dataset& dataset, ReportLib::IReportNonDistendedFindingsPtr spFindings, const bool bInitialAdd);

      /// Retrieves update buffer
      virtual const void far* GetUpdateBuffer(int4 & iWidth, int4 & iHeight, int4 & iWidthAlignedDWORD, const GLenum ePixelFormat = GL_BGR_EXT, const GLenum eDatatype = GL_UNSIGNED_BYTE, const bool bOnlyGLRenderScene = false);

      /// Set whether to display distance from rectum
      inline void SetDisplayDistanceFromRectum(const bool bDisplay);

      /// Force the view to redraw
      void ForceRedraw();

      /// invalidates this view
      virtual void InvalidateView();

      /// Whether to permit the display of slices
      void SetPermitSlices( const bool & bPermitSlices ) { m_bPermitSlices = bPermitSlices; }

      /// Whether to display the orientation arrow
      void ShowArrow( const bool bShow ) { m_bShowArrow = bShow; }

      /// gets the "govern frame rate while flying" flag
      void SetGovernFrameRateWhileFlying( const bool & bGovernFrameRateWhileFlying ) { m_bGovernFrameRateWhileFlying = bGovernFrameRateWhileFlying; }

      /// sets the "govern frame rate while flying" flag
      const bool & GetGovernFrameRateWhileFlying() const { return m_bGovernFrameRateWhileFlying; }

    protected:

      /// renders the view (includes render and raster)
      virtual void GLRender();

      /// render the scene
      virtual void GLRenderScene(void * pData = NULL);

      /// 2D scene rendering
      virtual void GLRasterScene();

      /// set up rendering context
      virtual void GLSetupRC();

      /// response to resize of window
      virtual void GLResize(GLsizei width, GLsizei height);

      /// idle function
      virtual bool GLIdle();
      /// whether or not to display a reverse-video image
      virtual void SetInverseImage(const bool bInverseImage );

      /// handlers in response to mouse/keyboard actions.
      /// left button down handler
      void OnLButtonDown(UINT uFlags, CPoint point);
      /// left button up handler
      void OnLButtonUp(UINT uFlags, CPoint point);
      /// right button down handler
      void OnRButtonDown(UINT uFlags, CPoint point);
      /// right button up handler
      void OnRButtonUp(UINT uFlags, CPoint point);
      /// middle button down handler
      void OnMButtonDown(UINT uFlags, CPoint point);
      /// middle button up handler
      void OnMButtonUp(UINT uFlags, CPoint point);
      /// mouse movement handler
      void OnMouseMove(UINT uFlags, CPoint point);
      /// keyboard press handler
      void OnKeyDown(UINT uChar, UINT nRepCnt, UINT nFlags);
      /// left button double-click handler
      void OnLButtonDblClk(UINT uFlags, CPoint point);
      /// resize message handler
      void OnSize(UINT uType, int4 iCX, int4 iCY);
      /// mouse wheel scroll handler
      virtual BOOL OnMouseWheel(UINT uFlags, int2 zDelta, CPoint pt);

      /// handler respnses to On_Set_Cursor(): set up different cursor icons.
      BOOL OnSetCursor(CWnd *pWnd, UINT uHitTest, UINT uMessage);

      /// command message handler
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

      /// render the object: colon(or volume), skeleton, camera position, bookmarks, connections...
      virtual void RenderObjects(const VtxViewModes& viewModes, const bool bIndexRender = false,
        const bool bRenderSelectedSegments = true);

      /// match the point to sorted segment.
      int4 GetSegmentHit(const CPoint & point, const bool& bSelected);

      /// Returns the index of the nearest skeleton node to the given point.
      int4 GetNearestSkeletonNode(const CPoint & point, Skeleton & skeleton) const;

      /// Returns the index of the nearest skeleton node to the given point.
      int4 GetNearestJoinedSkeletonNode(const CPoint & point, ReaderLib::SkeletonVCExt & skeleton) const;

      /// Returns a list of indices of bookmarks at the specified point.
      std::list<uint4> GetBookmarksHit(const CPoint & point, const ReaderLib::BookmarkList& bmList);

      /// Returns the index of the registration point at the specified point.
      int4 GetRegistrationPointHit(const CPoint & point);

      /// Returns a list of indices of cad findings at the specified point.
      std::list<std::string> GetCadFindingHit(const CPoint & point);

      /// get the popup menu ID.
      virtual uint4 GetPopupMenuID(UINT uFlags, CPoint point) const;

      /// change the popup menu.
      virtual void AdjustPopupMenu(CMenu & menu, const uint4 uMenuID) const;

      /// get the mouse keystring ID.
      virtual void GetMouseKeyStringIDs(uint4 & uLeftID, uint4 & uMidID, uint4 & uRightID) const;

      /// Timer control for image buffering.
      inline void ResetTimer(const uint4 uTime=0);

      /// Timer control for image buffering.
      inline void TimeClick();
      /// Timer control for image buffering.
      inline bool IsTimeOut() const;

    private:
      /// grap and buffer 2D image.
      void SaveBufferImage();

      /// updates the "distance from anal verge" static label
      void UpdateDistanceFromAnalVerge();

      /// cache of the drawn slice position
      Point3D<float4> m_vDrawnSlicePosition;

    /// Data members.
    protected:
      /// flag for whether the report arrow will be rendered.
      bool m_bRenderReportArrow;

      /// background color
      GLclampf m_bcR, m_bcG, m_bcB, m_bcA;
  
      /// OverviewRender instance
      ReaderLib::OverviewRender m_overviewRender;

      /// record the width and height of the window (required by trackball)
      uint4 m_uViewWth, m_uViewHgt;

    private:

      /// a buffer used for picking segments using OpenGL pick mechanism
      uint1 * m_puPickBuffer;

      /// member variables below are for Supine/Prone control.
      /// pointer to its neighbour view. (Supine/Prone)
      OverView* m_pNeighbourView;

      /// current selected bookmark.
      int4 m_iCurSelBookmark;

      /// current selected user registration point.
      int4 m_iCurSelRegistrationPoint;

      /// current selected cad finding.
      std::string m_idCurSelCadFinding;

      /// Stored formatting for primary dataset's distance-from-rectum text display (to save from repeated reloading during render)
      static std::string m_sDistFmtPrimary;
      /// Stored formatting for secondary dataset's distance-from-rectum text display (to save from repeated reloading during render)
      static std::string m_sDistFmtSecndry;

      /// position of report entry arrow
      Point<float4> m_reportPositionPt;
      /// direction of report entry arrow
      Normal<float4> m_reportDirectionNormal;
      /// startpoint of a connection between segments drawn for report image
      Point<float4> m_reportStartPt;
      /// endpoint of a connection between segments drawn for report image
      Point<float4> m_reportEndPt;

      /// Okay, this is an ad hoc hack to make the Overview class slightly more intelligent
      ///  about when it needs to re-render, regenerate display lists, and so on.  I'm not
      ///  totally convinced of its feasibility, but I'm going to take an implementation's shot at it
      ReaderLib::Dataset* m_pDatasetLastRendered;

      /// flag for whether this view should be buffered for faster drawing or not
      /// (on some large viewrects, the readback takes longer than the new render)
      bool m_bBufferImage;

      /// in ReportView, the display list will be turned off.
      bool m_bRefDisplayList;

      /// current (OpenGL) modelview matrix.
      GLdouble m_mModelViewMatrix[16];
      /// current (OpenGL) projection matrix.
      GLdouble m_mProjMatrix[16];

      /// flag for update Projection Matrix.
      bool m_bUpdateProjMatrix;
      /// flag for update ModelView Matrix.
      bool m_bUpdateModelViewMatrix;

      /// timer: calculate the frames per second.
      Timer m_frameTimer;

      /// used to regulate the frame rate during flying
      Timer m_governorTimer;

      /// a timer to control image buffering.
      int4 m_iTimeCounter;

      /// variable to keep track of the last right click position
      CPoint m_lastClickPoint;

      /// Whether user feature(s) have been added/changed (requiring re-registration)
      bool m_bUserFeaturesChanged;

      /// Whether to render simplified segments
      bool m_bRenderSimplifiedSegments;

      /// flag to save image for report.
      enum SaveReportImageEnum m_eSaveReportImage;

      /// pointer to NonDistendedAreaImage buffer.
      LPBYTE m_pNonDistendedAreaImage;

      /// Whether to display distance from rectum text in image
      bool m_bDisplayDistanceFromRectum;

      /// Whether to permit the display of slices
      bool m_bPermitSlices;

      /// Whether the mouse is currently positioned over a particular feature
      class OverviewFeature
      {
      public:
        enum TypeEnum { eNone, eUserFeature };
      }; // class OverviewFeature
      OverviewFeature::TypeEnum m_eMouseOverFeature;

      /// Whether or not to show the orientation arrow
      bool m_bShowArrow;

      /// indicates if the frame rate should be governed while flying
      bool m_bGovernFrameRateWhileFlying;
    }; // class Overview.


    #include "OverView.inl"


  } // namespace ColonViews

} // namespace ChildViews


#endif // Overview


// Revision History:
// $Log: OverView.h,v $
// Revision 1.14.2.1  2009/01/12 21:58:38  jmeade
// set pick buffer to be the exact size of the window, eliminating buffer overuns for large displays.
//
// Revision 1.14  2007/03/10 06:25:54  jmeade
// code standards.
//
// Revision 1.13  2007/03/09 23:38:54  jmeade
// code standards.
//
// Revision 1.12  2007/03/08 22:22:12  romy
// code review - header files adjusted
//
// Revision 1.11  2007/02/02 21:10:31  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.10  2006/07/06 15:59:55  jmeade
// Read patient position from volume header.
//
// Revision 1.9  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.8  2006/02/09 01:13:42  jmeade
// code review: comments.
//
// Revision 1.7  2005/11/21 23:16:18  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.6.2.2  2006/02/22 20:11:46  geconomos
// Issue #4604:  Overview snapshot not working while flying
//
// Revision 1.6.2.1  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.6  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.5  2005/09/27 01:10:16  jmeade
// context menu for cad findings.
//
// Revision 1.4  2005/09/09 14:31:21  geconomos
// Added a governor that regulates the fps while flying in 3d.
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
// Revision 3.34.2.2.4.8.2.4  2005/07/19 12:37:54  geconomos
// Issue #4289: fixed update mechanism
//
// Revision 3.34.2.2.4.8.2.3  2005/06/24 12:45:45  geconomos
// Issue#: 4220 Overview image staying inverted after snapshot
//
// Revision 3.34.2.2.4.8.2.2  2005/06/21 15:26:32  geconomos
// Corrected revision history
//
// Revision 3.34.2.2.4.8.2.1  2005/06/21 15:15:46  geconomos
// Broke out DrawOverView class into it's own .h and .C
//
// Revision 3.34.2.2.4.8  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.34.2.2.4.5.2.2  2005/06/08 14:01:10  geconomos
// continued effort ( sorry for lame comment! )
//
// Revision 3.34.2.2.4.5.2.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.34.2.2.4.5  2005/04/19 17:42:19  geconomos
// separated OpenGL and Windows resize functions
//
// Revision 3.34.2.2.4.4  2005/04/19 13:40:56  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.34.2.2.4.3  2005/04/05 14:46:51  frank
// Issue #4055: added ability to disable slice indicators on overview
// Issue $04065: drawing blank image if no data loaded
//
// Revision 3.34.2.2.4.2  2005/03/31 15:36:06  frank
// Issue #4077: Fixed update of verify view when button is pressed.
//
// Revision 3.34.2.2.4.5  2005/04/19 17:42:19  geconomos
// separated OpenGL and Windows resize functions
//
// Revision 3.34.2.2.4.4  2005/04/19 13:40:56  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.34.2.2.4.3  2005/04/05 14:46:51  frank
// Issue #4055: added ability to disable slice indicators on overview
// Issue $04065: drawing blank image if no data loaded
//
// Revision 3.34.2.2.4.2  2005/03/31 15:36:06  frank
// Issue #4077: Fixed update of verify view when button is pressed.
//
// Revision 3.34.2.2.4.1  2005/03/26 01:52:01  frank
// Issue #4029: Udated slice indicators on all views
//
// Revision 3.34.2.2  2003/02/27 02:22:55  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.34.2.1  2003/02/21 18:10:50  jmeade
// Fixed incorrect segment hit calculation when an overlapping segment is not selected.
//
// Revision 3.34  2003/02/05 23:50:09  jmeade
// Use numbered points instead of colors to differentiate user reg'n points within a dataset (and associate linked points between datasets)..
// Coding standards (const, variable naming).
//
// Revision 3.33  2003/01/29 00:43:28  jmeade
// Distance from rectum text display determined by separate flag (an i.e. a parent window), not by IsMainWindow();
// Code standard: All member vars private unless otherwise necessary.
//
// Revision 3.32  2003/01/03 23:07:18  jmeade
// Draw overview class.
//
// Revision 3.31  2002/11/22 03:11:18  jmeade
// IsMode() - more concise method name.
//
// Revision 3.30  2002/11/22 00:32:04  jmeade
// OverView::Initialize convenience function.
//
// Revision 3.29  2002/11/11 16:19:51  kevin
// Render the created "Straight segmetns"
//
// Revision 3.28  2002/10/31 19:56:00  kevin
// Redfactor Selecting of segments, jump-to-points etc since this got brooken
//
// Revision 3.27  2002/09/11 19:15:02  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.26  2002/09/10 20:50:54  jmeade
// Oops:  Each OverviewRender instance needs its own render mode variable.
//
// Revision 3.25  2002/09/10 19:07:40  jmeade
// One render mode for all OverviewRender instances; Code standards; Name clarifications.
//
// Revision 3.24  2002/09/10 15:47:10  jmeade
// Removed unused message.
//
// Revision 3.23  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.21  2002/07/18 20:25:38  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.20  2002/07/18 03:17:59  jmeade
// Correct picking of segment and node index for user registration points; Renamed ambiguous functions.
//
// Revision 3.19  2002/07/11 23:25:19  jmeade
// Implementing user-defined registration point specification.
//
// Revision 3.18  2002/07/09 20:13:26  jmeade
// Code standards.
//
// Revision 3.17  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.16  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.15  2002/04/12 02:24:03  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.14  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.13  2002/03/05 15:35:50  ana
// Using feature vectors.
//
// Revision 3.12  2002/02/26 16:39:31  ana
// changed a variable to boolean as it should be, still doesn't work :(
//
// Revision 3.11  2002/02/21 20:11:20  ana
// Displaying metrics
//
// Revision 3.10  2002/02/20 19:33:43  ana
// Part of metrics are being displayed already
//
// Revision 3.9  2002/02/07 18:13:47  ana
// GLRasterScene displays feature info. To display feature info
// Vverview Menu -> Registration Options -> Show feature info
//
// Revision 3.8  2002/02/06 21:20:04  ana
// Added one more option to the menu.
//
// Revision 3.7  2002/02/06 20:33:58  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
// (message WM_APP_2DFLIGHT_UPDATEVIEW)
//
// Revision 3.6.2.5  2002/06/27 17:35:33  jmeade
// GetUpdateBuffer is inherently a public-use function.
//
// Revision 3.6.2.4  2002/06/17 18:31:47  jmeade
// Code standards.
//
// Revision 3.6.2.3  2002/06/12 02:21:33  jmeade
// Issue 2423:  Use one GetUpdateBuffer method that returns private, temporary memory.
//
// Revision 3.6.2.2  2002/04/11 21:55:47  jmeade
// Code standards.
//
// Revision 3.6.2.1  2002/02/11 18:02:15  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
//
// Revision 3.6  2002/01/29 18:56:56  binli
// issue 1427: the image has already buffered. Adjusted the structure of control to make it worls.
//
// Revision 3.5  2002/01/03 02:23:35  kevin
// Cleanup related to new TTP work when ses, set and sec went away
//
// Revision 3.4  2001/12/22 00:08:57  jmeade
// Code standards.
//
// Revision 3.3  2001/10/24 16:16:33  jmeade
// Preliminary changes for identifying multiple bookmarks on selection
//
// Revision 3.2  2001/10/19 02:39:23  jmeade
// Code standards
//
// Revision 3.1  2001/10/17 22:02:33  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.4   Oct 08 2001 19:04:06   jmeade
// Code standards
// 
//    Rev 2.3   Oct 08 2001 17:03:46   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
// 
//    Rev 2.2   Oct 02 2001 16:50:00   binli
// report bug: grasp images
// 
//    Rev 2.1   Oct 01 2001 17:08:48   binli
// functions to ensure no bad/black images in report file: synchronous control
// 
//    Rev 2.0   Sep 16 2001 23:41:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:22   ingmar
// Initial revision.
// Revision 1.92  2001/08/22 01:19:15  jmeade
// Better encapsulation in OverView (ResetTimer has no meaning outside of class)
//
// Revision 1.91  2001/07/31 16:25:09  binli
// inline fucntion
//
// Revision 1.90  2001/07/30 22:40:59  binli
// bug fixed: update overview when change slices in StandardView/EndoAxialView.
//
// Revision 1.89  2001/06/27 14:09:10  binli
// bug 532: unnecessary redraw (rotate) of overview.
// (added  overriding functions: SetCapture(), ReleaseCapture(), IsCaptured())
//
// Revision 1.88  2001/06/26 22:43:41  binli
// removed unused variables: m_bInteracting/m_bInteractingOverView.
//
// Revision 1.87  2001/06/26 14:44:41  binli
// rewrote the control logic to rerender overview: made memory stable.
//
// Revision 1.86  2001/06/18 17:08:12  jmeade
// only need to set the study last rendered if it's changed
//
// Revision 1.85  2001/05/29 20:50:36  binli
// code conventions
//
// Revision 1.84  2001/05/15 18:55:43  jmeade
// Code conventions
//
// Revision 1.83  2001/05/15 17:49:38  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.82  2001/05/11 17:36:29  binli
// removed duplicated OVModes in OverView class.
//
// Revision 1.81  2001/05/08 18:35:28  binli
// Trackrecord ID 000377: green marked or not in S&PView and VerifyView.
// (No need to change snapshot in Report)
//
// Revision 1.80  2001/05/04 15:33:42  binli
// display lists: triangle/framebox.
//
// Revision 1.79  2001/04/18 22:19:21  binli
// set up flag for ModelView matrix updating in overview.
//
// Revision 1.78  2001/04/17 16:23:31  binli
// the current modelview matrix is saved for efficiency.
//
// Revision 1.77  2001/04/17 15:46:21  binli
// code cleaned
//
// Revision 1.76  2001/04/16 20:42:48  binli
// fixed the bug for picking node on skeleton.
//
// Revision 1.75  2001/04/16 18:45:34  binli
// 1. the current ProjMatrix and ModelViewMatrix are saved for reuse in RenderCamera(),
//     MatchSelectedSegents(), MatchSkeleton(), MatchSkeletonToSegment().
// 2. removed unused variable: m_bImageCreated in Overview.
//
// Revision 1.74  2001/04/13 15:33:50  binli
// moved out the render function: RayCastVolume() to OverviewRender class.
//
// Revision 1.73  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.72  2001/04/12 22:27:15  binli
// moved out the render function: RenderSegNum() (formal RenderMarkInSkel()) to OverviewRender class.
//
// Revision 1.71  2001/04/12 21:13:30  binli
// moved out the render function: RenderCamera() to OverviewRender class.
//
// Revision 1.70  2001/04/12 14:51:27  binli
// moved out the render function: RenderBookMarks() to OverviewRender class.
//
// Revision 1.69  2001/04/11 21:52:32  binli
// moved out the render function: RenderReportConnections to OverviewRender Class.
//
// Revision 1.68  2001/04/11 21:03:25  binli
// moved out the render function: RenderConnections to OverviewRender Class.
//
// Revision 1.67  2001/04/11 20:47:13  binli
// moved out the render function: RenderSkel to OverviewRender Class.
//
// Revision 1.66  2001/04/11 16:26:43  binli
// moved RenderSlices to OverviewRender class.
//
// Revision 1.65  2001/04/11 11:59:24  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.64  2001/04/10 22:17:08  binli
// moved render functions "RenderTri" to the OverviewRender class.
//
// Revision 1.63  2001/04/09 16:07:37  binli
// moved out render functions to OverviewRender class.
//
// Revision 1.62  2001/04/06 16:28:08  binli
// Used Matrix  to fulfill matrix multi
//
// Revision 1.61  2001/04/05 16:28:33  binli
// enhanced
//
// Revision 1.60  2001/04/03 21:13:24  binli
// 1st code walkthrough
//
// Revision 1.59  2001/04/03 17:07:47  binli
// added comments.
//
// Revision 1.58  2001/04/02 20:08:41  binli
// reflected "Typedef.h"
//
// Revision 1.57  2001/03/27 15:12:29  binli
// reflected Typedef.h
//
// Revision 1.56  2001/03/21 21:46:45  liwei
// Obsolete functions removed
//
// Revision 1.55  2001/03/21 15:28:18  liwei
// Remove the RenderObject from VLIOverView. VolumePro is invoked by a virtual member function: RayCastVolume
//
// Revision 1.54  2001/03/19 18:16:02  binli
// code walkthrough according to new code standard:
//   1> inline functions moved to .inl
//   2> no public/pretect data member variables.
//
// Revision 1.53  2001/03/16 22:37:46  binli
// renamed some variables:  according to new code standard
//
// Revision 1.52  2001/03/16 20:21:48  binli
// code walkthrough
//
// Revision 1.51  2001/03/12 20:18:38  liwei
// VLIOverview modified to be compatible with changes in other files
//
// Revision 1.50  2001/03/07 16:03:02  liwei
// Added some comments
//
// Revision 1.49  2001/02/28 23:30:31  liwei
// Overview rerendered when buttons on VerifyView clicked
//
// Revision 1.48  2001/02/28 17:53:55  liwei
// Overviews in VerifyView and Supine/ProneViews are set to buffer rendered image.
// That is, call RenderUpdateBuffer() rather than RenderObjects whenever possible
//
// Revision 1.47  2001/02/26 16:49:22  liwei
// Frame timer moved from GLRasterScene to Class
//
// Revision 1.46  2001/02/23 20:11:31  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.45  2001/02/22 19:17:22  binli
// fixed 'jump' bug. (skeleton point)
//
// Revision 1.44  2001/02/22 01:39:38  jeff
// coding conventions; unnecessary global viewmodes added to overview;
//
// Revision 1.43  2001/02/21 20:33:42  jeff
// COverview constructor now takes a StudyOrientation as a parameter (Study parameters unused/obsolete)
//
// Revision 1.42  2001/02/19 17:18:07  jeff
// OnKeyPressed() is obsoleete, identical to OnKeyDown()
//
// Revision 1.41  2001/02/07 15:23:35  frank
// Rendered centerline in continuous color gradient.
//
// Revision 1.40  2001/02/05 18:22:58  geconomos
// Added methods to draw a given segment connection line.
//
// Revision 1.39  2001/02/05 14:31:46  geconomos
// Started to work on report arrow
//
// Revision 1.38  2001/02/05 13:30:43  frank
// Made a single global null study.
//
// Revision 1.37  2001/02/02 07:28:46  jeff
// Moving strings to resource table
//
// Revision 1.36  2001/01/26 20:00:17  binli
// defect 000188: default Overview position
//
// Revision 1.35  2001/01/26 19:10:25  geconomos
// Removed CONNECT_SEGS view mode and combined with CENTERLINE.
// Added DRAW_DISTANCE view mode to enable drawing dictance from rectum
//
// Revision 1.34  2001/01/24 19:23:13  liwei
// Software volume rendering for overview
//
// Revision 1.33  2001/01/24 17:47:37  frank
// Improved supine/prone selection.
//
// Revision 1.32  2001/01/23 17:44:43  jeff
// app message handler
//
// Revision 1.31  2001/01/23 17:38:35  frank
// Clarified member name.
//
// Revision 1.30  2001/01/19 21:07:10  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.29  2001/01/16 15:48:59  binli
// Verification: 1. draw skeleton in arrows when a segment is picked.
//                    2. mark (number) the picked skeleton.
//
// Revision 1.28  2001/01/12 23:01:19  binli
// added a ball to represents the middle of skeleton (used in vertification).
// transparently render other segments when picking (used in verification view).
//
// Revision 1.27  2000/12/20 19:17:04  jeff
// Default parameters for study and bBufferImage
// Changed order of parameters in constructor, on the notion that
//  m_bBufferImage is the most likely default-value parameter
//
// Revision 1.26  2000/12/12 23:55:24  jeff
// AdjustPopupMenu() now indicates which popup menu was specified
//
// Revision 1.25  2000/11/23 08:15:46  jeff
// no message
//
// Revision 1.24  2000/11/22 18:38:46  binli
// added function for display list delete.
//
// Revision 1.23  2000/11/22 17:14:59  binli
// fixed display list for strips rendering.
//
// Revision 1.22  2000/11/21 18:59:24  binli
// added frame display list
//
// Revision 1.21  2000/11/20 01:51:05  jeff
// GetMouseKeyStringIDs() -- allows each view to set strings for mouse button use key
// AdjustPopupMmenu() takes a menu reference instead of a pointer
//
// Revision 1.20  2000/11/17 21:27:27  binli
// adjusted the rendering order of colon and other stuffs in translucent mode.
//
// Revision 1.19  2000/11/17 05:55:37  liwei
// Bugs fixed for VolumePro
//
// Revision 1.18  2000/11/15 19:12:30  liwei
// Adding features to read transfer functions from file
//
// Revision 1.17  2000/11/14 23:51:48  liwei
// Add support for VolumePro
//
// Revision 1.16  2000/11/14 18:23:02  binli
// changed references to pointers (to m_study and m_secondary in object 'rdrGlobal')
//
// Revision 1.15  2000/11/13 19:27:37  binli
// 1.changed construction for Supine/Prone application
// 2. followd adjustment of lock/unlock controls in VerifyView.
//
// Revision 1.14  2000/11/10 22:44:36  binli
// worked on both Supine/Prone overview rendering. In global variable 'rdrGlobal',
// two local trackballs are added. Some changes are made in TrackBall and OverView.
// Followed the change in OverView, VerifyView, EndoVisView, StandardView are
// changed a little.
//
// Revision 1.13  2000/11/10 20:14:12  frank
// Added MatchSkeleton function
//
// Revision 1.12  2000/11/10 15:22:10  frank
// Added jumping to new segments in overview window
//
// Revision 1.11  2000/11/01 22:43:58  jeff
// Flags and point of the last mouse click passed into GetMenuPopupID()
// (for the child view to use to decide which popup menu to show)
//
// Revision 1.10  2000/10/25 18:01:04  jeff
// OnCommand() overridden to handle bookmark/view window popup
// override other necessary popup menu functions
// Note current bookmark clicked on right mouse click
//
// Revision 1.9  2000/10/13 16:55:10  kevin
// Changed the logic for when to grab a new overview image
//
// Revision 1.8  2000/10/12 21:10:51  kevin
// Added bufferImage flag to ovwerview constructor
//
// Revision 1.7  2000/10/12 02:53:30  kevin
// Added CONNECT_SEGS render mode, and code to do it.
// Moved distance from rectum text here. Updated to properly
// compute distance from rectum across segments.
//
// Revision 1.6  2000/10/09 23:44:16  kevin
// Added Guadric member (instead of creating and killing al the time)
// and cleaned u p rendering of objects
//
// Revision 1.5  2000/10/04 14:23:21  kevin
// Added Endpoint picking (so far simple for no
// branches case untill skel is redone)
//
// Also added rdrGlobal units and dim for fast
// reading for study load speed
//
// Revision 1.4  2000/09/29 23:00:18  kevin
// FIxed the picking problem
//
// Revision 1.3  2000/09/21 19:28:20  kevin
// Added Raster Scene
//
// Revision 1.2  2000/09/13 16:31:18  jeff
// Removing unnecessary references to obsolete CChildView class
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 30    9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 29    9/03/00 6:44p Kevin
// Added update of ovreview window without redrawing all the
// ploygons by grabbing a texture image. Then the position
// can be updated ont he display constantly. Also added
// double click for bookmarks
// 
// 28    8/29/00 4:13p Jeff
// Mode variables now held in ViatronStudy
// 
// 27    8/28/00 2:35p Antonio
// updated header and footer
// 
// *****************  Version 26  *****************
// User: Binli           Date:  8/23/00  Time: 11:36a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 25  *****************
// User: Binli           Date:  8/23/00  Time: 11:35a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 24  *****************
// User: Kevin           Date:  8/22/00  Time:  4:30p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added centerline toggling
// 
// *****************  Version 23  *****************
// User: Binli           Date:  8/22/00  Time: 11:44a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 22  *****************
// User: Binli           Date:  8/22/00  Time: 11:40a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 21  *****************
// User: Binli           Date:  8/22/00  Time: 11:22a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 20  *****************
// User: Binli           Date:  8/22/00  Time: 11:15a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 19  *****************
// User: Kevin           Date:  8/21/00  Time:  3:41p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added segment length checking
// 
// *****************  Version 18  *****************
// User: Kevin           Date:  8/19/00  Time: 12:37p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added drawing of 2D planes on overview
// 
// *****************  Version 17  *****************
// User: Binli           Date:  8/18/00  Time:  3:07p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 16  *****************
// User: Binli           Date:  8/18/00  Time:  2:18p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 15  *****************
// User: Binli           Date:  8/18/00  Time:  2:11p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Add comments
// 
// *****************  Version 14  *****************
// User: Binli           Date:  8/18/00  Time: 10:44a
// Checked in $/ViatronMDI/src/Viatron
// 
// *****************  Version 13  *****************
// User: Jeff            Date:  8/15/00  Time:  7:26p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Moved OverviewTrackBall (class and instance) to ReaderGlobal so that
// overview position/orientation can persist throughout application
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 12  *****************
// User: Frank           Date:  8/08/00  Time:  6:40p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Jeff: (don't worry Frank, you didn't make this change!)
// SwapBuffers() moved back to CChildView::OnPaint(), new function override
// PostGLRender() can do any device context (HDC) drawing to the window
// 
// *****************  Version 11  *****************
// User: Kevin           Date:  8/06/00  Time:  7:02p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added Picking of segments for order. To accomplish this, I
// pulled the guts out of some data structures and replaced them
// 
// *****************  Version 10  *****************
// User: Binli           Date:  8/02/00  Time: 10:47a
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 9  *****************
// User: Kevin           Date:  8/02/00  Time:  1:33a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Aligned centerline with polygons and cleaned up overview a little
// 
// *****************  Version 8  *****************
// User: Binli           Date:  8/01/00  Time: 10:56a
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 7  *****************
// User: Binli           Date:  7/28/00  Time:  1:02p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 6  *****************
// User: Binli           Date:  7/26/00  Time:  2:00p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 5  *****************
// User: Binli           Date:  7/26/00  Time: 10:48a
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 4  *****************
// User: Binli           Date:  7/26/00  Time:  9:29a
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   
// 
// *****************  Version 3  *****************
// User: Binli           Date:  7/25/00  Time:  3:45p
// Checked in $/Viatron1000/src/Viatron
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/18/00  Time:  1:58p
// Branched at version 2
// Comment:
//   
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/12/00  Time:  9:14p
// Created OverView.h
// Comment:
//   
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/OverView.h,v 1.14.2.1 2009/01/12 21:58:38 jmeade Exp $
// $Id: OverView.h,v 1.14.2.1 2009/01/12 21:58:38 jmeade Exp $
