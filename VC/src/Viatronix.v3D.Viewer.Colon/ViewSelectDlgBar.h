// $Id: ViewSelectDlgBar.h,v 1.6 2007/03/07 22:35:00 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViewSelectDlgBar.h  
// Author: Jeff Meade  jmeade@viatronix.net


// pragma declarations
#pragma once

// include declarations
#include "TabStrip.h"
#include "MouseHelp.h"
#include "StatusIndicator.h"
#include "ObservableObject.h"
#include "ApplicationController.h"


OPEN_DIALOGS_NS

// class declaration
class ViewSelectDlgBar : public WinUtils::TabStrip, public IObserver< Colon::ReportArgs >
{
// enums and typedefs
public:

  /// number of layouts
  enum { NUM_VIEW_SCREENS = 5 };
  /// bitset for enabling/disabling layouts
  typedef std::bitset<NUM_VIEW_SCREENS> EnableViewsBitSet;

// member functions
public:
  
  /// constructor
  ViewSelectDlgBar();

  /// destructor
  virtual ~ViewSelectDlgBar();

  /// shows/hides all children
  void ShowAllChildren(const bool bShow);

  /// sets the current tab control selection
  void SetCurrentViewSelection(int4 iView);
  
  /// gets the current tab control selection
  int4 GetCurrentViewSelection();

  /// enables the specified tab selection
  bool EnableItem(const int4 iItem, const bool bEnable);
  
  /// enables/disables the specified tab selections by index
  bool EnableItems(const ViewSelectDlgBar::EnableViewsBitSet& enableTabBitSet);

  /// sets the position of the progress control (0-100)
  void SetProgress(const int4 iProgress);
  
  /// sets the state for the mouse help control
  void SetButtonFunctionText( const std::string & sLeft  = "", 
                              const std::string & sMid   = "", 
                              const std::string & sRight = "",
                              const bool bCtrl = false, 
                              const bool bShift = false );

  /// observer of report state changes
  void OnNotified( const Colon::ReportArgs & args );

  /// set visibility of the link state indicator
  void SetLinkStateDisplay( const bool & bShowLinkState );


// implementation
protected:
  
  /// called before the current tab changes
  virtual void OnItemChanging( int4 iPreviousItem, int4 iNewItem );

  /// called after the current tab changed
  virtual void OnItemChanged( int4 iPreviousItem, int4 iNewItem );
  
  /// mfc messge map
  DECLARE_MESSAGE_MAP()

  // WM_CREATE handler
  int4 OnCreate( LPCREATESTRUCT lpCreateStruct ) ;

  /// WM_DESTROY
  void OnDestroy();

  /// WM_SIZE handler
  void OnSize( UINT uType, int4 cx, int4 cy );
    
  /// Gets tool tip message text
  BOOL OnToolTipNeedTextHandler( UINT id, NMHDR *pTTTStruct, LRESULT *pResult );


private:

  /// repositions the child controls
  void RepositionChildren();

// member variables
private:
  
  /// progress bar
  CProgressCtrl m_progressbar;
  
  /// mouse help
  WinUtils::MouseHelp m_mouseHelp;

  /// status indicator
  WinUtils::StatusIndicator m_statusIndicator;
    
}; // class ViewSelectDlgBar


CLOSE_DIALOGS_NS



// $Log: ViewSelectDlgBar.h,v $
// Revision 1.6  2007/03/07 22:35:00  jmeade
// code standards.
//
// Revision 1.5  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/10/03 12:59:01  geconomos
// derived class from tab strip
//
// Revision 1.3  2005/08/16 21:53:47  jmeade
// namespaces for WinUtils.
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.4  2001/12/31 19:09:31  jmeade
// Hide all children function.
//
// Revision 3.3  2001/11/09 04:55:01  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2  2001/11/06 20:00:32  jmeade
// Removed the extra tab.
//
// Revision 3.1  2001/11/05 17:49:12  ana
// Registration view added.
//
// Revision 3.0.2.2  2001/11/06 23:50:52  jmeade
// Send new item selection as parameter to WM_APP_VIEW_SELECTION_CHANGING message;
//  Remaining comments
//
// Revision 3.0.2.1  2001/11/06 17:16:33  jmeade
// Cleaned up V1KMessages.h enum, comments.
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:36   ingmar
// Initial revision.
// Revision 1.12  2001/06/08 00:37:39  jmeade
// coding conventions
//
// Revision 1.11  2001/05/23 18:00:52  jmeade
// enable/disable view selection tab items (instead of hiding entire tab control);
// tabs to spaces
//
// Revision 1.10  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.9  2001/02/05 14:30:29  geconomos
// Added functions to allow change of tab text
//
// Revision 1.8  2001/01/17 01:48:28  jeff
// no functional changes
//
// Revision 1.7  2001/01/06 00:12:47  jeff
// control/shift indicators
//
// Revision 1.6  2000/11/20 15:30:25  jeff
// Highlighting the current view tab selection to improve visibility
//
// Revision 1.5  2000/11/14 08:48:26  jeff
// Mouse button use Key, function to set text for three mouse buttons in key
//
// Revision 1.4  2000/11/04 00:39:36  jeff
// Progress bar
//
// Revision 1.3  2000/10/13 17:31:05  jeff
// no message
//
// Revision 1.2  2000/10/04 23:57:42  jeff
// Changes for verification view
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 5     9/05/00 5:23p Jeff
// Added set current selection function
// 
// 4     9/05/00 4:50p Jeff
// Added a function for retrieving the current view selection
// 
// 3     8/28/00 2:45p Antonio
// updated header and footer
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  8/24/00  Time:  5:58a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Missing include file
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/14/00  Time:  9:25p
// Created ViewSelectDlgBar.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViewSelectDlgBar.h,v 1.6 2007/03/07 22:35:00 jmeade Exp $
// $Id: ViewSelectDlgBar.h,v 1.6 2007/03/07 22:35:00 jmeade Exp $
