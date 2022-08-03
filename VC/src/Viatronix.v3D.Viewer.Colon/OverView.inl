// $Id: OverView.inl,v 1.4 2007/03/10 06:25:54 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File : Overview.inl
// Description : a class to display the overview colon and support segments sorting 
// OverView.inl: inline functions in Class OverView.
//
// Owner: Jeffrey Meade (jmeade@viatronix.net)


/*
 * Refreshes the overview buffered image (occurs on next GLRenderScene)
 *
 * @param iTimeClick     count of time clicks that control the update.
 */
inline void OverView::RefreshImage(const uint4 iTimeClick)
{
  ResetTimer(iTimeClick);
} // RefreshImage


/**
 * Retrieve the pointer for its neighbour view.
 * @return   neighbour view
 */
inline OverView * OverView::GetNeighbourView() const
{ 
  return m_pNeighbourView; 
} // GetNeighbourView()


/**
 *  Set neighbour view.
 *
 * @param pOverView   a pointer to its neighbour. 
 * @return            pointer for neighbour view.
 */
inline void OverView::SetNeighbourView(OverView * pOverView) 
{
  m_pNeighbourView = pOverView; 
} // SetNeighbourView()


/**
 * Set report arrow display value
 *
 * @param bNewVal   true to display report arrow
 */
inline void OverView::SetRenderReportArrow(const bool bNewVal) 
{
  m_bRenderReportArrow = bNewVal; 
} // SetRenderReportArrow()


/**
 * Get report arrow display value
 *
 * @return    true if displaying report arrow
 */
inline bool OverView::GetRenderReportArrow() const
{
  return m_bRenderReportArrow; 
} // GetRenderReportArrow()


/**
 * Set current viewing mode: normal/verify/picking.
 *
 * @param eOVmode  reference of the viewing mode to be set.
 */
inline void OverView::SetRenderMode(const ReaderLib::OverviewRender::RenderModeEnum eOVMode) 
{
  SetImageBuffered( false );
  ResetTimer();
  m_overviewRender.SetRenderMode(eOVMode);
} // SetMode()


/**
 * Get current viewing mode: normal/verify/picking.
 *
 * @return   the current render viewing mode.
 */
inline const ReaderLib::OverviewRender::RenderModeEnum OverView::GetRenderMode() const
{
  return m_overviewRender.GetRenderMode();
} // GetMode()


/**
 * check if current rendering mode matches selected mode
 *
 * @param eOVmode   OverviewRender::OVModes.
 * @return          true when match, else false.
 */
inline bool OverView::IsMode(const ReaderLib::OverviewRender::RenderModeEnum eOVMode) const
{
  return (m_overviewRender.GetRenderMode() == eOVMode);
} // IsMode()


/**
 * Set report arrow position
 *
 * @param positionPt   location
 * @param dirNormal    view normal
 */
inline void OverView::SetReportArrow(const Point<float4> & positionPt, const Normal<float4> & dirNormal)
{
  m_reportPositionPt = positionPt;
  m_reportDirectionNormal = dirNormal;
  m_overviewRender.SetArrowInReport(positionPt, dirNormal);
} // SetReportArrow()


/**
 * Set report arrow segment
 *
 * @param beginPt   segment start
 * @param endPt     segment end
 */
inline void OverView::SetReportSegment(const Point<float4> & beginPt, const Point<float4> & endPt)
{
  m_reportStartPt = beginPt;
  m_reportEndPt = endPt;
} // SetReportSegment()


/**
 * set up flag to update modelview matrix.
 *
 * @param bUpdateModelViewMatrix bool
 */
inline void OverView::SetModelViewMatrixUpdateFlag(const bool bUpdateModelViewMatrix)
{
  m_bUpdateModelViewMatrix = bUpdateModelViewMatrix;
} // SetModelViewMatrixUpdateFlag()


/**
 * Swap display lists when swap studies: 
 *    delete all display lists: triangle/frame display list
 *    the will be re-generated in OverviewRender.
 */
inline void OverView::DeleteDisplayLists()
{
  m_overviewRender.DeleteAllDisplayLists();
} // SwapTriDisplayList()


/**
 * turn on/off display list rendering. (OFF in report view)
 *
 * @param bRefDisplayList  ture:turn on/false:turn off.
 */
inline void OverView::SetDisplayListRender(const bool bRefDisplayList)
{
  m_bRefDisplayList = bRefDisplayList;
} // SetDisplayListRender


/**
 * Timer control for image buffering: reset.
 * @param   time value
 */
inline void OverView::ResetTimer(const uint4 uTime) 
{ 
  m_iTimeCounter = uTime; 
} // ResetTimer()


/**
 * Timer control for image buffering: one click.
 */
inline void OverView::TimeClick() 
{
  m_iTimeCounter++;
} // TimeClick()
  

/**
 * Timer control for image buffering: reset.
 *
 * @return true when time is out.
 */
inline bool OverView::IsTimeOut() const
{
  return (m_iTimeCounter >= 10);
} // IsTimeOut()


/*
 * test if the this slice should be rendered.
 * (this function is used to test if one kind of (axial/sagittal/coronal/cross-sectional) slice 
 * is rendered. Use this function for efficient rendering in StandardView/EndoSliceView.)
 *
 * @param videModes   current view mode. (ViewModes::AXIAL/SAGITTAL?CORONAL/CROSS-SECTION)
 * @return            true if the slice is rendered.
 */
inline bool OverView::IsRenderThisSlice(const VtxViewModes & viewModes) const
{
  return ((ViatronStudy::m_modeOverView & viewModes) != 0);
} // IsRenderThisSlice()


/**
 * Set whether to display distance from rectum
 * @param bDisplay   Whether to display distance from rectum
 */
inline void OverView::SetDisplayDistanceFromRectum(const bool bDisplay)
{
  m_bDisplayDistanceFromRectum = bDisplay;
} // SetDisplayDistanceFromRectum()


// $Log: OverView.inl,v $
// Revision 1.4  2007/03/10 06:25:54  jmeade
// code standards.
//
// Revision 1.3  2007/02/02 21:10:31  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.14.8.1  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.14.12.1  2005/05/25 15:50:23  geconomos
// initial backup
//
// Revision 3.14  2003/02/05 23:49:51  jmeade
// Coding standards (const, variable naming).
//
// Revision 3.13  2003/01/29 00:42:05  jmeade
// Distance from rectum text display determined by separate flag (an i.e. a parent window), not by IsMainWindow().
//
// Revision 3.12  2002/12/12 22:06:40  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.11  2002/11/22 03:11:19  jmeade
// IsMode() - more concise method name.
//
// Revision 3.10  2002/09/10 20:50:54  jmeade
// Oops:  Each OverviewRender instance needs its own render mode variable.
//
// Revision 3.9  2002/09/10 19:07:40  jmeade
// One render mode for all OverviewRender instances; Code standards; Name clarifications.
//
// Revision 3.8  2002/07/09 20:13:26  jmeade
// Code standards.
//
// Revision 3.7  2002/05/09 20:54:15  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.6  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.5  2002/02/06 20:33:58  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
// (message WM_APP_2DFLIGHT_UPDATEVIEW)
//
// Revision 3.4.2.3  2002/04/27 00:18:20  jmeade
// Code standard.
//
// Revision 3.4.2.2  2002/02/11 18:02:15  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
//
// Revision 3.4.2.1  2002/02/05 19:55:42  jmeade
// Refresh image when a view mode changes.
//
// Revision 3.4  2002/01/29 18:56:56  binli
// issue 1427: the image has already buffered. Adjusted the structure of control to make it worls.
//
// Revision 3.3  2001/12/28 20:33:34  jmeade
// Compiler warning.
//
// Revision 3.2  2001/10/19 02:28:59  jmeade
// GLChildView get current mouse capture convenience functions
//
// Revision 3.1  2001/10/17 22:01:01  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 08 2001 19:01:14   jmeade
// Code conventions
// 
//    Rev 2.1   Oct 08 2001 17:03:46   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
// 
//    Rev 2.0   Sep 16 2001 23:41:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:22   ingmar
// Initial revision.
// Revision 1.22  2001/08/22 01:19:16  jmeade
// Better encapsulation in OverView (ResetTimer has no meaning outside of class)
//
// Revision 1.21  2001/08/20 20:08:09  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.20  2001/07/31 16:25:10  binli
// inline fucntion
//
// Revision 1.19  2001/07/25 22:51:53  binli
// ID 89: new exception handling.
//
// Revision 1.18  2001/07/24 21:49:10  binli
// ID 89: exception handling in v1k GUI
//
// Revision 1.17  2001/06/27 14:09:10  binli
// bug 532: unnecessary redraw (rotate) of overview.
// (added  overriding functions: SetCapture(), ReleaseCapture(), IsCaptured())
//
// Revision 1.16  2001/06/26 14:44:41  binli
// rewrote the control logic to rerender overview: made memory stable.
//
// Revision 1.15  2001/05/29 20:50:36  binli
// code conventions
//
// Revision 1.14  2001/05/15 18:55:44  jmeade
// Code conventions
//
// Revision 1.13  2001/05/11 17:36:29  binli
// removed duplicated OVModes in OverView class.
//
// Revision 1.12  2001/05/08 18:35:28  binli
// Trackrecord ID 000377: green marked or not in S&PView and VerifyView.
// (No need to change snapshot in Report)
//
// Revision 1.11  2001/05/04 15:33:42  binli
// display lists: triangle/framebox.
//
// Revision 1.10  2001/05/03 15:23:22  binli
// made m_currOVMode in OverviewRender class private.
//
// Revision 1.9  2001/04/18 22:19:21  binli
// set up flag for ModelView matrix updating in overview.
//
// Revision 1.8  2001/04/17 15:46:21  binli
// code cleaned
//
// Revision 1.7  2001/04/16 18:45:34  binli
// 1. the current ProjMatrix and ModelViewMatrix are saved for reuse in RenderCamera(),
//     MatchSelectedSegents(), MatchSkeleton(), MatchSkeletonToSegment().
// 2. removed unused variable: m_bImageCreated in Overview.
//
// Revision 1.6  2001/04/12 21:13:31  binli
// moved out the render function: RenderCamera() to OverviewRender class.
//
// Revision 1.5  2001/04/10 22:17:08  binli
// moved render functions "RenderTri" to the OverviewRender class.
//
// Revision 1.4  2001/03/27 15:12:30  binli
// reflected Typedef.h
//
// Revision 1.3  2001/03/19 18:16:33  binli
// more inline functions for private variables.
//
// Revision 1.2  2001/03/19 16:16:18  binli
// more inline functions from .h to this .inl.
//
// Revision 1.1  2001/03/19 15:51:52  binli
// moved inline functions from .h to this .inl.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/OverView.inl,v 1.4 2007/03/10 06:25:54 jmeade Exp $
// $Id: OverView.inl,v 1.4 2007/03/10 06:25:54 jmeade Exp $
