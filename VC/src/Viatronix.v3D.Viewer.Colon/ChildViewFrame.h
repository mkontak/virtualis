// $Id: ChildViewFrame.h,v 1.7 2007/03/01 01:18:55 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ChildViewFrame.h  
// Description: Interface to MDI Child Wnd encapsulating a GLWnd
// Owner: Jeff Meade  @jmeade@viatronix.net

// pragmas
#pragma once

// inclues
#include "GLWnd.h"

// namespaces
OPEN_LAYOUTS_NS

// class declaration
class ChildLayoutFrame : public CMDIChildWnd
{
// member functions
public:
  
  /// constructor
  ChildLayoutFrame( GLWnd * pWnd );
  
  /// destructor
  virtual ~ChildLayoutFrame();

  /// gets pointer the active (enclosed) GLWnd
  inline virtual GLWnd * GetActiveLayout() const {  return IsWindow( GetSafeHwnd() ) ? m_pWndView : NULL; }

protected:
  
  /// called by the framework before the creation of the Windows window attached to this CWnd object
  virtual BOOL PreCreateWindow( CREATESTRUCT & cs );

  // activates and restores the frame window so that it is visible and available to the user
  virtual void ActivateFrame( int nShowCmd );

  /// called by the framework to route and dispatch command messages and to handle the update of command user-interface objects
  virtual BOOL OnCmdMsg( UINT uID, int iCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo );

  /// wndproc
  virtual LRESULT WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam );

  /// mfc message map
  DECLARE_MESSAGE_MAP()

  /// WM_CREATE handler
  int OnCreate( LPCREATESTRUCT lpCreateStruct );

  /// WM_ERASEBKGND handler 
  BOOL OnEraseBkgnd( CDC* pDC );

  /// WM_SIZE handler
  void OnSize( UINT uType, int cx, int cy ) ;

  /// WM_SIZE handler
  void OnSetFocus(CWnd* pOldWnd);

  /// WM_COMMAND, ID_FILE_CLOSE handler
  void OnFileClose();

  /// WM_DESTORY handler
  void OnDestroy();

// member variables
private:

  // view for the client area of the frame.
  GLWnd * m_pWndView;

}; // class ChildViewFrame

CLOSE_LAYOUTS_NS

// Revision History:
// $Log: ChildViewFrame.h,v $
// Revision 1.7  2007/03/01 01:18:55  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.6  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.5.2.1  2006/05/16 12:39:46  geconomos
// proper destruction of contained view window in WM_DESTROY handler
//
// Revision 1.5  2005/10/28 17:48:23  geconomos
// renamed files from GLViewWnd => GLWnd
//
// Revision 1.4  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/10/05 15:23:49  geconomos
// Modified child frame such that it is always maximized within the parent frame
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2003/02/22 11:54:48  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:00   ingmar
// Initial revision.
// Revision 1.4  2001/04/13 02:31:49  jmeade
// headers and footers
//
// Revision 1.3  2000/12/20 19:21:18  jeff
// OnSize() handler to resize view window
//
// Revision 1.2  2000/09/30 02:34:51  jeff
// Added common view frame code to PreCreateWindow()
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 6     8/28/00 2:08p Antonio
// updated header and footer
// 
// *****************  Version 5  *****************
// User: Jeff            Date:  8/23/00  Time:  2:50p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Added GetActiveViewWnd() function (returns the GLViewWnd * member)
// 
// *****************  Version 4  *****************
// User: Jeff            Date:  8/22/00  Time: 11:35p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   WindowProc messages (e.g. WM_USER, WM_APP) redirected through the
// child frame window to its child view window
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  8/12/00  Time:  8:01p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   First errorless compile after changes to class architecture
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  8/12/00  Time:  6:47p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Changing class architecture
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/11/00  Time:  3:44p
// Created MainViewFrame.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ChildViewFrame.h,v 1.7 2007/03/01 01:18:55 jmeade Exp $
// $Id: ChildViewFrame.h,v 1.7 2007/03/01 01:18:55 jmeade Exp $
