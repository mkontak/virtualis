// $Id: V1KMessages.h,v 1.2.2.2 2009/01/07 01:39:00 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: V1KMessages.h
// Description: Colon App Window messages; definition of all application-level messages (all WM_APP)
// Owner: Jeff Meade  jmeade@viatronix.net


#ifndef _VIATRON_1000_MESSAGES_
#define _VIATRON_1000_MESSAGES_

#define WM_APP_MESSAGE_HIGH     0xBFFF   // Windows Platform SDK states WM_APP message range as WM_APP(0X8000) to 0xBFFF
#define VIATRON_APP_MSG_COUNT   100

enum enumViatronWMAppMessages
{
  /// Start Viatron messages near end of the range of WM_APP messages, to hopefully avoid collision with menu and other IDs
  WM_APP_FIRST_MESSAGE = WM_APP_MESSAGE_HIGH - VIATRON_APP_MSG_COUNT,

  /// GL Idle message, for updating views as necessary when application is idle
  /// @param WPARAM   views to get message
  /// @param LPARAM   views NOT to get message
  /// @return INT     Unused
  WM_APP_GL_IDLE = WM_APP_FIRST_MESSAGE,

  /// Gets the unique ID for the child view
  /// @param WPARAM   0-based position of child view
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_GET_VIEW_WINDOW_CLASSID,

  /// Sets the main child view window
  /// @param WPARAM   view class instance ID
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_SET_MAIN_VIEW_WINDOW,

  /// Gets the unique ID of the main child view window
  /// @param WPARAM   ID of querying view window
  /// @param LPARAM   Unused
  /// @return INT     class id of main view
  WM_APP_GET_MAIN_VIEW_WINDOW_CLASSID,

  /// Indicates a load study background process has completed
  /// @param WPARAM   TRUE/FALSE indicating study load success
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_LOAD_VIATRON_STUDY_DONE,

  /// Called before change in view selection, to validate change
  /// @param WPARAM   new main view
  /// @param LPARAM   new subview, previous subview
  /// @return INT     TRUE to allow selection change, FALSE to deny selection change
  WM_APP_VIEW_SELECTION_CHANGING,

  /// Called after view selection change, to reflect change in interface
  /// @param WPARAM   new main view
  /// @param LPARAM   new subview, previous subview
  /// @return INT     Unused
  WM_APP_VIEW_SELECTION_CHANGED,

  /// Invalidates (sets for WM_PAINT refresh) a region for the specified child view
  /// @param WPARAM   class instance ID or 0 for all childview instances
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_GLWND_REGION_INVALIDATE,

  /// Flags all child views of a specified type to be updated on next render (WM_PAINT)
  /// @param WPARAM   GLChildView::ViewTypeEnum value, indicates the child view type to refresh
  /// @param LPARAM   Unused
  /// @param INT      Unused
  WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE,

  /// Child view message
  /// @param WPARAM   childID (low word), message (high word)
  /// @param LPARAM   message parameters)
  /// @return INT     Unused
  WM_APP_CHILD_VIEW_MSG,

  /// Sets the capture to a child view window
  /// @param WPARAM   Child view ID
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_GL_SET_CAPTURE,

  /// Releases the capture of a child view window
  /// @param WPARAM   Child view ID
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_GL_RELEASE_CAPTURE,

  /// Gets the current child view with the mouse capture if any
  /// @param WPARAM   Child view ID
  /// @param LPARAM   Unused
  /// @return INT     ID of the mouse view
  WM_APP_GL_GET_CAPTURE,

  /// Notifies of a change in view modes
  /// @param WPARAM   old mode
  /// @param LPARAM   new mode
  /// @return INT     Unused
  WM_APP_VIEWMODE_CHANGED,

  /// Notifies of a change in/completion of a step during study loading
  /// @param WPARAM   previous step (loword), next step (hiword)
  /// @param LPARAM   orientation: 1=supine, 2=prone
  /// @return INT     Unused
  WM_APP_STUDY_LOAD_STEP_NOTIFY,

  /// Displays help text
  /// @param WPARAM   string table ID
  /// @param LPARAM   non-zero to remove text after short delay
  /// @return INT     Unused
  WM_APP_DISPLAY_HELP_TEXT,

  /// Whether or not to enable toggling between supine and prone studies
  /// @param WPARAM   Unused
  /// @param LPARAM   Unused
  /// @return INT     0 (or 1) to disable (enable) toggling between supine and prone datasets
  WM_APP_QUERY_ENABLE_SUPINE_PRONE_TOGGLE,

  /// A missed region has been selected (to jump to)
  /// @param WPARAM   0-based selected region
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_MISSED_REGION_SELECTED,

  /// Jump to the next missed region
  /// @param WPARAM   0=previous or 1=next
  /// @param LPARAM   Unused
  /// @return INT     
  WM_APP_JUMP_NEXT_MISSED_REGION,

  /// Calculate missed regions
  /// @param WPARAM   Unused
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_DETECT_MISSED_REGIONS,

  /// Update the endolumen coverage marking display
  /// @param WPARAM   Unused
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_UPDATE_LUMEN_COVERAGE_MARKING,

  /// Trigger reconnection to the patient database
  /// @param WPARAM   Unused
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_CONNECT_TO_DATABASE,

  /// Set the main dataset to that with the given orientation
  /// @param WPARAM   Orientation of new main dataset (cast to DatasetOrientations::TypeEnum)
  /// @param LPARAM   Unused
  /// @return INT     Unused
  WM_APP_SET_MAIN_DATASET,

  /// 2D auto-flight: signal to set up new skeleton position.
  /// @param WPARAM   position on current skeleton.
  /// @param LPARAM   indicate update skeleton(0), or overview(1).
  /// @return INT     Unused
  WM_APP_2DAUTOFLIGHT_UPDATE,

  /// Set/reset/kill timer for a GLChildView, message to child's parent GLWnd
  /// @param WPARAM   Timer ID
  /// @param LPARAM   Non-zero: Child ID to set/reset timer; Zero indicates timer to be killed
  /// @return INT     Unused
  WM_APP_CHILDTIMER_STATUSCHANGE,

  /// Synchronizes the world locations of two (registered) datasets
  /// @param WAPARM   Source dataset orientation (or ID)
  /// @param LPARAM   Destination dataset orientation (or ID)
  /// @return INT     Unused
  WM_APP_SYNCRONIZE_DATASET_LOCATIONS,
};

/// WParam values for WM_APP_VIEW_SELECTION_CHANGED
enum enumViewSelections
{
  WM_APP_VIEWSEL_NOCHANGE = -1,
  WM_APP_VIEWSEL_STUDYBROWSER = 0,
  WM_APP_VIEWSEL_VERIFICATION = 1,
  WM_APP_VIEWSEL_NAVIGATION = 2,
  WM_APP_VIEWSEL_REPORT = 3,
  WM_APP_VIEWSEL_STUDYCOMPLETE = 4
};

#define APSTUDIO_INVOKED
#include "resource.h"

#ifndef _APS_NEXT_COMMAND_VALUE
#define _APS_NEXT_COMMAND_VALUE		WM_APP + 1000
#endif // #ifndef _APS_NEXT_COMMAND_VALUE


/// menu item constants
enum enumViatronExtraMenus
{
  IDM_GOTO_BOOKMARK_LIST_MAX = 1000,

  IDM_GOTO_BOOKMARK_FIRST	  =	_APS_NEXT_COMMAND_VALUE + 1000,
  IDM_GOTO_ANNOTATION_FIRST = IDM_GOTO_BOOKMARK_FIRST + IDM_GOTO_BOOKMARK_LIST_MAX + 1,
  IDM_GOTO_3DMEASUREMENTS_FIRST = IDM_GOTO_ANNOTATION_FIRST + IDM_GOTO_BOOKMARK_LIST_MAX + 1
};
#undef APSTUDIO_INVOKED

#endif // #ifndef _VIATRON_1000_MESSAGES_



// Revision History:
// $Log: V1KMessages.h,v $
// Revision 1.2.2.2  2009/01/07 01:39:00  jmeade
// volume measure interface changes.
//
// Revision 1.2.2.1  2007/09/25 21:27:12  jmeade
// comments.
//
// Revision 1.2  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.1.4.1  2006/03/22 22:54:24  jmeade
// Issue 4667: restrict registration synchronization to views where both datasets are visible, and a link button is available.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.13.2.4  2003/07/18 17:43:59  jmeade
// Child view timers.
//
// Revision 3.13.2.3  2003/02/27 03:29:05  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.13.2.2  2003/02/26 20:12:50  jmeade
// Issue 2853:  Start Colon app messages near high of WM_APP msg range to avoid collision with other messages.
//
// Revision 3.13.2.1  2003/02/21 17:54:14  jmeade
// Removed an essential identical app message.
//
// Revision 3.13  2003/02/10 17:13:07  jmeade
// Issue 2832: Redundant, excess code for snapshot notifications.
//
// Revision 3.12  2002/09/16 22:57:22  jmeade
// Moved messages to FunctionProgress class.
//
// Revision 3.11  2002/09/10 15:50:13  jmeade
// Removed unused msgs, added new.
//
// Revision 3.10  2002/08/28 16:05:15  jmeade
// Removed unused messages.
//
// Revision 3.9  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.8  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.7  2002/02/06 20:32:06  binli
// comments on message WM_APP_2DFLIGHT_UPDATEVIEW
//
// Revision 3.6.2.3  2002/05/25 05:36:32  jmeade
// Issue 2270:  Select annotation to jump to frorm (menu) list of annotations.
//
// Revision 3.6.2.2  2002/02/22 20:04:22  binli
// message 'WM_APP_PLAYSOUND'
//
// Revision 3.6.2.1  2002/02/11 18:02:15  binli
// issue 1796: update indicators when stop flying (3d & 2d flight)
//
// Revision 3.6  2001/01/01 06:00:00  binli
// issue 1588: new message: WM_APP_2DAUTOFLIGHT_UPDATE
//
// Revision 3.5  2001/11/21 19:12:56  jmeade
// View mode change now specifies previous and new mode.
//
// Revision 3.4  2001/11/15 22:13:17  binli
// added message WM_APP_SWITCH_UPDATEVIEW.
//
// Revision 3.3  2001/11/09 04:55:01  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2  2001/11/05 17:50:05  ana
// Registration view added.
//
// Revision 3.1.2.2  2001/11/06 17:16:33  jmeade
// Cleaned up V1KMessages.h enum, comments.
//
// Revision 3.1.2.1  2001/10/28 20:56:59  jmeade
// Choice of database provider implemented fully
//
// Revision 3.1  2001/10/19 02:49:59  jmeade
// GLChildView get current mouse capture convenience functions
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
//
//    Rev 2.2   Oct 01 2001 16:53:52   binli
// removed the message just added: no need any more.
//
//    Rev 2.1   Oct 01 2001 11:50:30   binli
// Added a message for grasp report image.
//
//    Rev 2.0   Sep 16 2001 23:43:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
//
//    Rev 1.0   Sep 16 2001 17:29:10   ingmar
// Initial revision.
// Revision 1.27  2001/08/21 02:25:58  jmeade
// Enums for view window positions (no hardcoded values)
//
// Revision 1.26  2001/06/26 14:44:41  binli
// rewrote the control logic to rerender overview: made memory stable.
//
// Revision 1.25  2001/06/05 22:57:58  binli
// bug 000357: fixed
// (different number of slices in S&P studies)
//
// Revision 1.24  2001/05/17 06:15:22  jmeade
// Transfer study app message
//
// Revision 1.23  2001/04/13 02:35:01  jmeade
// headers and footers
//
// Revision 1.22  2001/04/05 18:58:03  jmeade
// start/finish message for background process
//
// Revision 1.21  2001/03/08 03:10:44  jmeade
// definition of WM_APP_DISPLAY_HELP_TEXT changes slightly
//
// Revision 1.20  2001/02/28 18:53:11  jmeade
// removed unused frames-per-second
//
// Revision 1.19  2001/02/20 22:22:45  jeff
// unused message
//
// Revision 1.18  2001/02/15 00:23:42  jeff
// more messages for keydowns
//
// Revision 1.17  2001/02/09 18:49:18  jeff
// app messages for detect, jump to next missed region replace keyboard keys
//
// Revision 1.16  2001/02/09 16:31:19  jeff
// Bug fix: Loading Prone not cleared after study load complete
// WM_APP_STUDY_LOAD_NOTIFY: now indicates previous (loword) and
// next (hiword) step in wParam, and orientation in lParam
//
// Revision 1.15  2001/02/06 14:16:29  frank
// Revamped study load progress display.
//
// Revision 1.14  2001/01/25 18:38:05  jeff
// Message: WM_APP_DISPLAY_FUNCTION_TEXT - displays function/step text
//
// Revision 1.13  2001/01/25 04:30:41  jeff
// WM_APP_QUERY_ENABLE_SUPINE_PRONE_SWITCH
//
// Revision 1.12  2001/01/23 18:07:49  jeff
// specific childview messages (to replace so-called "hotkeys")
//
// Revision 1.11  2001/01/17 01:47:45  jeff
// navigation tab selection indicates old and new selection (with double-word int)
//
// Revision 1.10  2001/01/02 21:03:36  jeff
// get child view id by 0-based position
//
// Revision 1.9  2000/12/27 23:08:10  jeff
// removed obsolete messages
//
// Revision 1.8  2000/12/22 02:03:13  jeff
// message to display help text (from string table) when mouse over controls
//
// Revision 1.7  2000/12/15 18:00:22  jeff
// WM_APP_CHILD_VIEW_MSG, messages for specific child view instances
// Implemented navigation view tab selection
//
// Revision 1.6  2000/11/25 00:25:53  jeff
// OnAppEndoModeChanged() message
//
// Revision 1.5  2000/11/20 01:47:30  jeff
// WM_APP_GET_GLWIN_POINT_COLOR: gets RGB color to
//  display at specified point for view window children
//
// Revision 1.4  2000/11/04 00:43:55  jeff
// Set Progress (bar) message, handled by MainAppFrame()
//
// Revision 1.3  2000/10/13 15:57:20  kevin
// Added GrabOverview message
//
// Revision 1.2  2000/09/30 20:35:10  jeff
// Notifications to main application during study load process
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
//
// 14    8/28/00 7:25p Jeff
// Release capture message now passes GLChildViewPtr (as Set capture
// message does)
//
// 13    8/28/00 2:40p Antonio
// updated header and footer
//
// *****************  Version 12  *****************
// User: Jeff            Date:  8/24/00  Time: 10:09a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Changed WM_GL_..._INVALIDATE to WM_APP_GL_..._INVALIDATE
//
// *****************  Version 11  *****************
// User: Jeff            Date:  8/24/00  Time: 12:53a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   WM_APP_GET_MAIN_VIEW_WINDOW_ID expects the calling child view window
// ID in the WPARAM parameter
//
// *****************  Version 10  *****************
// User: Jeff            Date:  8/22/00  Time: 11:36p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   GLChildView invalidate app message added
//
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
//
// *****************  Version 9  *****************
// User: Jeff            Date:  8/14/00  Time:  5:55p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//
//
// *****************  Version 8  *****************
// User: Jeff            Date:  8/11/00  Time:  9:57p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//
//
// *****************  Version 7  *****************
// User: Jeff            Date:  7/28/00  Time:  4:00p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Extra ifndef's to ensure compilation under other environments
//
// *****************  Version 6  *****************
// User: Jeff            Date:  7/24/00  Time:  7:52p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added bookmark class and associated hooks,
// (initially) only for jumping to fixed precomputed positions
//
// *****************  Version 5  *****************
// User: Jeff            Date:  7/18/00  Time:  1:58p
// Branched at version 5
// Comment:
//
//
// *****************  Version 4  *****************
// User: Jeff            Date:  7/17/00  Time:  4:38p
// Checked in $/Viatron1000/Viatron
// Comment:
//   Added background thread to handle view window updates
// Reorganized messages (e.g. put view wnd msgs in view wnd class)
//
// *****************  Version 3  *****************
// User: Jeff            Date:  7/15/00  Time:  8:00p
// Checked in $/Viatron1000/Viatron
// Comment:
//   Incremental update:
// Attempting to have continuous updates to all views while handling OnMouseMove()
// msg in navigation view
//
// *****************  Version 2  *****************
// User: Jeff            Date:  7/14/00  Time:  5:35p
// Checked in $/Viatron1000/Viatron
// Comment:
//   Added event for passing keyboard events between view windows
//
// *****************  Version 1  *****************
// User: Jeff            Date:  7/12/00  Time:  9:14p
// Created V1KMessages.h
// Comment:
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/V1KMessages.h,v 1.2.2.2 2009/01/07 01:39:00 jmeade Exp $
// $Id: V1KMessages.h,v 1.2.2.2 2009/01/07 01:39:00 jmeade Exp $
