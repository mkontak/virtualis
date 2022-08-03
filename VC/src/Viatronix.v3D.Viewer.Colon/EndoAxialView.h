// $Id: EndoAxialView.h,v 1.11.2.4 2010/04/17 05:17:08 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: EndoAxialView.h  
// Description: Interface for view with one 3D and one 2D subview window.
// Owner: Jeffrey Meade  jmeade@viatronix.com


#if !defined(AFX_ENDOORTHOGONALVIEW_H__9BE12492_6FE8_11D4_B6DE_00D0B7BF4092__INCLUDED_)
#define AFX_ENDOORTHOGONALVIEW_H__9BE12492_6FE8_11D4_B6DE_00D0B7BF4092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"

namespace ColonViews
{

  namespace Layouts
  {

    /// frame window containing an endoscopic view and orthogonal view
    class EndoOrthogonalLayoutFrame : public ChildLayoutFrame  
    {
    public:
      /// Constructor
      EndoOrthogonalLayoutFrame();

    private:
      DECLARE_DYNCREATE(EndoOrthogonalLayoutFrame)
    };

    // Viatron view window for this child frame window
    class EndoOrthoLayout : public ColonViatronixWnd
    {
    public:
      /// Constructor
      EndoOrthoLayout();

      /// Is datasets locking tracked by this view
      virtual bool IsDatasetsLockingEnabled() const { return true; }

    protected:

      /// Renders the views
      virtual void GLRenderViews();

      /// Called when view is initialized
      virtual void OnStudyLoadStepNotify(const ReaderLib::StudyLoad::StepsEnum eCompletedStep,
        const ReaderLib::StudyLoad::StepsEnum eCurrentStep, const ReaderLib::DatasetOrientations::TypeEnum eOrientation);
      /// mouse move message
      virtual afx_msg void OnMouseMove(UINT uFlags, CPoint point);
      /// message to add a report snapshot    
      afx_msg void OnReportAddSnapshot();
      /// message to query whether toggle between datasets is available
      afx_msg LRESULT OnQueryEnableSupineProneSwitch(WPARAM, LPARAM);
      /// message to toggle autofly mode
      afx_msg void OnToggleAutofly();
      /// message to get the id of the main viewwindow
      afx_msg LRESULT OnGetMainViewWndID(WPARAM, LPARAM);
      /// message to update views during 2D flight
      afx_msg LRESULT OnUpdate2DAutoFlight(WPARAM, LPARAM);
      /// message to show popup menu for a viewwindow
      virtual uint4 OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const;

      /// called when the view is being resized
      afx_msg void OnSize( UINT uType, int iSizeX, int iSizeY );

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDT_ENDOSLICE_LANDSCAPE; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return (m_bToolbarAtBottom ? IDT_ENDOSLICE_WIDESCREEN : IDT_ENDOSLICE_WIDESCREEN_BARTOP); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDT_ENDOSLICE; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return IDT_ENDOSLICE_PORTRAIT_LARGE; }

      DECLARE_MESSAGE_MAP()
			/// flag for toolbar position
			bool m_bToolbarAtBottom;
    }; // class EndoOrthoLayout

  } // namespace Layouts

} // namespace ColonViews

#endif // !defined(AFX_ENDOORTHOGONALVIEW_H__9BE12492_6FE8_11D4_B6DE_00D0B7BF4092__INCLUDED_)


// Revision History:
// $Log: EndoAxialView.h,v $
// Revision 1.11.2.4  2010/04/17 05:17:08  dongqing
// small portrait display layouts
//
// Revision 1.11.2.3  2010/03/20 12:33:04  dongqing
// virtual
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.2  2009/12/09 20:26:01  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.11.2.1  2008/01/11 02:18:54  jmeade
// issue 5907.
//
// Revision 1.11  2007/03/01 01:18:55  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.10  2007/01/24 22:41:53  romy
// added code to handle the SRT region tool click
//
// Revision 1.9  2006/06/01 20:19:38  frank
// updated to visual studio 2005
//
// Revision 1.8  2006/02/07 21:21:46  jmeade
// code review: comments.
//
// Revision 1.7  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.6.2.1  2005/12/23 16:49:15  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.6  2005/10/25 15:37:28  jmeade
// ParentViews ==> Layouts
//
// Revision 1.5  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.4  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.14.2.3.4.4.2.1  2005/06/24 13:02:20  geconomos
// Issue #4221: Unable able to fly in 3d after flying in 2d
//
// Revision 3.14.2.3.4.4  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.14.2.3.4.3.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 3.14.2.3.4.3  2005/04/01 01:25:18  frank
// Issue #4079: fixed cross sectional image slicing
//
// Revision 3.14.2.3.4.2  2005/03/31 17:36:10  jmeade
// Issue 4038: Generalized idle time processing; fixed for other messages.
//
// Revision 3.14.2.3.4.1  2005/03/28 20:44:55  jmeade
// Issue 4066.
//
// Revision 3.14.2.3  2003/05/29 17:29:30  jmeade
// comments.
//
// Revision 3.14.2.2  2003/02/22 11:55:31  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.14.2.1  2003/02/22 05:50:59  jmeade
// Issue 2837:  Only 2D and 3D view are relevant as main views.
//
// Revision 3.14  2003/02/10 16:47:35  jmeade
// Issue 2832: Redundant, excess code for snapshot notifications.
//
// Revision 3.13  2002/11/21 22:53:19  jmeade
// Overview render mode cleanup; now-unneeded method overrides.
//
// Revision 3.12  2002/11/15 00:29:42  jmeade
// No hard-coding of # of child views.
//
// Revision 3.11  2002/09/06 16:10:26  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.10  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.8  2002/07/26 15:03:36  jmeade
// Fixes for Visual Studio class wizard database.
//
// Revision 3.7  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.6  2002/04/12 02:10:17  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.5  2002/03/13 22:01:15  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.4.2.5  2002/08/05 23:43:08  jmeade
// Issue 2591 (in previous revision):  Added default handlers for main view window identification in ViatronViewWnd.
//
// Revision 3.4.2.4  2002/08/05 23:39:09  jmeade
// Issue 2593:  Clarify the "object" of the measurements (3D) and annotations (2D) menus.
//
// Revision 3.4.2.3  2002/04/03 01:03:23  jmeade
// Don't move mouse cursor on snapshot, the user is not expecting it;
// Removed unnecessary override.
//
// Revision 3.4.2.2  2002/02/26 16:54:15  binli
// issue 1543: inverse image when taking snapshot.
//
// Revision 3.4.2.1  2002/02/22 20:40:38  binli
// issue 1543: 3> cursor in the center of mainviewwindow.
//
// Revision 3.4  2001/01/01 06:02:58  binli
// issue 1588: response to message 'WM_APP_2DAUTOFLIGHT_UPDATE'
//
// Revision 3.3  2002/01/23 15:58:31  jmeade
// 1591:  Support for movie recording; Coding standards.
//
// Revision 3.2  2001/11/17 03:30:50  jmeade
// Patient view stored in base class.
//
// Revision 3.1  2001/11/09 01:16:58  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.0  2001/10/14 23:01:57  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 03 2001 12:13:10   jmeade
// Snapshot menu handler; coding standards
// 
//    Rev 2.0   Sep 16 2001 23:41:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:08   ingmar
// Initial revision.
// Revision 1.13  2001/05/12 00:00:04  jmeade
// Coding conventions
//
// Revision 1.12  2001/04/13 02:33:20  jmeade
// headers and footers
//
// Revision 1.11  2001/02/06 14:14:29  frank
// Revamped study load progress display.
//
// Revision 1.10  2001/02/05 23:32:35  liwei
// Non-live views in EndoAxialView are also grayed out
//
// Revision 1.9  2001/01/25 04:29:38  jeff
// supine/prone toggle (combobox) in patient info dialog (will "sexy" it up later);
// doesn't yet change on command in verifyview
//
// Revision 1.8  2000/12/27 23:40:27  jeff
// EndoAxialView ==> EndoOrthogonalView;
// embedded child view class in view frame class
//
// Revision 1.7  2000/12/20 19:47:10  jeff
// Added small overview window; swapped position of endoscopic and orthogonal view
//
// Revision 1.6  2000/12/12 23:44:50  jeff
// Added Patient info dialog; selection of ortho view orientation now
//  embedded in slice view; popup menus
//
// Revision 1.5  2000/11/21 04:09:40  jeff
// Create one (set of) font(s) in MainAppFrame class and access throughout
//  system (instead of each view window creating their own fonts)
//
// Revision 1.4  2000/11/14 07:47:06  jeff
// allocate font in a member variable, delete in destructor (previous memory-leak fix)
//
// Revision 1.3  2000/10/02 21:56:46  jeff
// Changes for staggered study load (had to check in because my cheapo computer is crashing)
//
// Revision 1.2  2000/09/30 22:17:04  jeff
// ViatronViewWnd class for more study-specific, V1K specific, etc. things
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 5     9/07/00 1:32p Jeff
// Embedded control dialog ("bars") in the GLChildView class, resized in
// OnSize
// 
// 4     8/28/00 2:15p Antonio
// updated header and footer
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  8/24/00  Time: 12:55a
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Buttons on EndoOrthogonal view to change flip between the orthogonal
// views
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  8/15/00  Time:  5:39p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   Enable repeated changing of slice view orientation
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/14/00  Time: 10:35a
// Created EndoAxialView.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/EndoAxialView.h,v 1.11.2.4 2010/04/17 05:17:08 dongqing Exp $
// $Id: EndoAxialView.h,v 1.11.2.4 2010/04/17 05:17:08 dongqing Exp $
