// $Id: ViatronStudy.h,v 1.7 2007/03/07 22:33:53 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViatronStudy.h  
// Description: Definition of class/instances to handle various user modes in Colon application.
// Owner: Jeff Meade  jmeade@viatronix.net

#if !defined(ViatronStudy_h)
#define ViatronStudy_h

#include "BookmarkList.h" // Added by ClassView
#include "GLChildView.h"
#include "VtxViewModes.h"
#include "StudyLoad.h"

using namespace ColonViews::ChildViews;

class ViatronStudy
{
public:

  /// Resets all view modes.
  static bool Reset( bool bUserSpecific = true );

public:
  /// Sets the currently selected view.
  static void SetCurrentView(const GLChildView::ViewWndTypeEnum eViewType, const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrient);
  /// Gets the currently selected view modes.
  static VtxViewModes &GetCurrentViewModes() { return *(m_peCurrentViewModes); }

  /// Sets the currently selected view type.
  static GLChildView::ViewWndTypeEnum GetCurrentViewType()  { return (m_eCurrentViewType); }
  /// Sets the currently selected study dataset orientation.
  static ReaderLib::DatasetOrientations::TypeEnum GetCurrentViewOrientation() { return m_eCurrentOrientation; }

  /// Returns the view modes of the currently selected view.
  static VtxViewModes & GetViewModes(const GLChildView::ViewWndTypeEnum eViewType);

  /// Method invoked when view mode is changed.
  static void OnSetViewModeProc(const ViewModesValue& uOldMode, const ViewModesValue& uNewMode);

  /// Turns off autoflight
  static void TurnOffAutoflight();

public:
  /// View modes for overview
  static VtxViewModes m_modeOverView;
  /// View modes for orthogonal views
  static VtxViewModesWithNotify<OnSetViewModeProc> m_modeOrthogView;
  /// View modes for orthogonal views
  static VtxViewModesWithNotify<OnSetViewModeProc> m_modeObliqueView;
  /// View modes for the endoscopic view, with a change notify method.
  static VtxViewModesWithNotify<OnSetViewModeProc> m_modeEndoView;
  /// View modes for the 3D segments view
  static VtxViewModesWithNotify<OnSetViewModeProc> m_modeSegments3D;

  /// Current modes for each of the navigation's views
  static struct structVWMDS
  {
    /// Standard view.
    VtxViewModes m_modeStandardLayout;
    /// Endoscopic-Orthogonal view.
    VtxViewModes m_modeEndoOrthoLayout;
    /// Dual orthogonal view.
    VtxViewModes m_modeSPOrthoLayout;
    /// Dual dataset view.
    VtxViewModes m_modeSPOverViewLayout;
  } ViewWndModes;

private:

  /// Constructor
  ViatronStudy();

private:
  /// Valid mode for the current view
  static VtxViewModes * m_peCurrentViewModes;
  /// View type of current view.
  static GLChildView::ViewWndTypeEnum m_eCurrentViewType;
  /// Orientation of current view.
  static ReaderLib::DatasetOrientations::TypeEnum m_eCurrentOrientation;
}; // ViatronStudy

#include "ViatronStudy.inl"

#endif // ViatronStudy_h


// Revision History:
// $Log: ViatronStudy.h,v $
// Revision 1.7  2007/03/07 22:33:53  jmeade
// code standards.
//
// Revision 1.6  2007/02/02 21:29:36  jmeade
// Issue 5107: separate mode for 3dsegments child.
//
// Revision 1.5  2005/11/01 16:13:22  jmeade
// Issue 4491: Slice views trigger on-mode-changed processing.
//
// Revision 1.4  2005/09/13 13:05:46  geconomos
// Added StopAutoFlight() static member
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.3.2.3.4.1  2005/04/11 15:04:07  jmeade
// Issue 2815: Disable annotations for oblique views.
//
// Revision 3.3.2.3  2003/06/02 21:39:47  jmeade
// Comments.
//
// Revision 3.3.2.2  2003/02/27 03:33:46  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.3.2.1  2003/02/22 05:57:01  jmeade
// Issue 2837:  Recognize any generic orthogonal view for m_orthoModes.
//
// Revision 3.3  2002/12/12 22:39:58  jmeade
// Unused or unneeded view modes code cleanup; Comments, code standards.
//
// Revision 3.2  2002/05/09 20:54:16  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.1.4.1  2002/04/24 01:02:19  jmeade
// Issue 2044:  Set current view *dataset orientation* as well as type.
//
// Revision 3.1  2001/10/17 19:20:32  jmeade
// All Orthogonal views now share same view modes.  Issue 1183.
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:30   ingmar
// Initial revision.
// Revision 1.22  2001/05/12 00:01:00  jmeade
// Coding conventions
//
// Revision 1.21  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.20  2001/04/10 15:32:59  binli
// moved namespace 'ViewModes' and class 'VtxViewModes' to 1-utils
//
// Revision 1.19  2001/02/22 01:36:45  jeff
// removed unnecessary modes
//
// Revision 1.18  2001/02/09 00:10:45  jeff
// biopsy_valid mode for endoscopic view
//
// Revision 1.17  2001/02/05 21:22:27  jeff
// measure ==> line measure and roi measure; removed CL_GRADIENT mode
//
// Revision 1.16  2001/02/05 18:51:01  frank
// Removed the centerline gradient in all-but-verify view.
//
// Revision 1.15  2001/01/30 02:13:00  jeff
// changed PICKINFO and MARKING mode to MISSED_REGIONS
//
// Revision 1.14  2001/01/27 02:57:13  kevin
// Hooked up the display paintinfo to the overview.
// (note there are still some bugs with the grab image,
// but I can't seemm to get it to connect from readerlib
// into viatron. ARGGHH I am a coding dinosaur)
//
// Revision 1.13  2001/01/26 19:10:25  geconomos
// Removed CONNECT_SEGS view mode and combined with CENTERLINE.
// Added DRAW_DISTANCE view mode to enable drawing dictance from rectum
//
// Revision 1.12  2001/01/19 21:07:10  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.11  2000/12/27 23:11:35  jeff
// Store view modes for each navigation view (Standard, Endo-Ortho, etc)
// eGVT_TRANSVERSE ==> eGVT_AXIAL;
// eGVT_MPR ==> eGVT_CROSS_SECTION;
//
// Revision 1.10  2000/11/17 05:55:11  liwei
// Bugs fixed for VolumePro
//
// Revision 1.9  2000/11/16 19:46:40  binli
// added 'CENTERLINE2' to OverViewModes to control centerline rendering.
//
// Revision 1.8  2000/11/10 15:23:31  frank
// Added jumping to new segments in overview window
//
// Revision 1.7  2000/10/31 23:51:47  jeff
// ViatronStudy::Reset() function, to reset modes and the like
//
// Revision 1.6  2000/10/30 13:37:10  kevin
// Added Biopsy Mode for moving the magic window around
//
// Revision 1.5  2000/10/13 22:49:12  kevin
// Added Picking Verify mode
//
// Revision 1.4  2000/10/12 02:52:06  kevin
// Added CONNECT_SEGS render mode
//
// Revision 1.3  2000/10/09 23:51:49  kevin
// Added Picking Seg and Endpts modes
//
// Revision 1.2  2000/10/02 23:01:39  jeff
// Moved bookmark list to ReaderGlobal (instance)
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 9     9/08/00 6:07p Jeff
// changed IndependentModes() and MutexModes() to Get...Modes()
// 
// 8     9/01/00 9:14p Jeff
// Mode changes implemented throughout interface
// 
// 7     9/01/00 3:45p Jeff
// Can only use first 31 bits (32 causes overflow, but WHY??)
// GetCurrentViewModes() returns reference to mode (for changing the
// current view mode)
// Functions to return either independent modes only or mutex modes only
// 
// 6     9/01/00 12:10p Jeff
// Another 'modes' incremental checkin
// 
// 5     8/31/00 9:09a Jeff
// Incremental checkin (for view mode changes)
// 
// 4     8/30/00 6:32p Jeff
// One standardarized enum value class for all mode windows
// 
// 3     8/29/00 4:14p Jeff
// Overview mode variables added
// 
// 2     8/28/00 2:44p Antonio
// updated header and footer
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/14/00  Time:  1:53p
// Created ViatronStudy.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronStudy.h,v 1.7 2007/03/07 22:33:53 jmeade Exp $
// $Id: ViatronStudy.h,v 1.7 2007/03/07 22:33:53 jmeade Exp $
