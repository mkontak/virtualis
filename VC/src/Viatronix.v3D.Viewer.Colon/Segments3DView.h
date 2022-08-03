// $Id: Segments3DView.h,v 1.11.2.11 2010/04/17 05:16:53 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Kevin Kreeger (kevin@viatronix.com)


// pragmas
#pragma once


// defines to use when checking which view to update.
#define DO_NOT_UPDATE_VIEW 0
#define UPDATE_VIEW 1
// defines to use when checking which registration mode is active.
#define NOT_ACTIVE 0
#define ACTIVE 1


// includes
#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"
#include "ReaderGlobal.h"
#include "V1KMessages.h"


// namespaces
namespace ColonViews
{
  namespace Layouts
  {

    /**
     * 3D undistorted segments layout
     */
    class Segments3DLayoutFrame : public ChildLayoutFrame
    {

      DECLARE_DYNCREATE(Segments3DLayoutFrame)

    // construction
    protected:

      /// Constructor; protected constructor used by dynamic creation
      Segments3DLayoutFrame();

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(Segments3DLayoutFrame)
      //}}AFX_VIRTUAL

      // Generated message map functions
      //{{AFX_MSG(Segments3DLayoutFrame)
      // NOTE - the ClassWizard will add and remove member functions here.
      //}}AFX_MSG
    }; // class Segments3DLayoutFrame

    /////////////////////////////////////////////////////////////////////////////


    /**
     * 3D layout for segments
     */
    class Segments3DLayout : public ColonViatronixWnd
    {
    // construction
    public:

      /// Constructor
      Segments3DLayout();

    // member functions
    public:

      /// Is datasets locking tracked by this view
      virtual bool IsDatasetsLockingEnabled() const { return true; }

    protected:

      //Renderer viewers method
      virtual void GLRenderViews();

      /// Vector positions for this window's child views
      enum ePos { ePOS_UDVIEW = 0, ePOS_OVERVIEW, ePOS_NAVIGATION, ePOS_ORTHO, ePOS_OBLIQUE };

      /// store the last time the navigation view was updated (for optimizing interactivity during 2D slicing)
      Timer m_lastNavigationUpdateTimer;

      /// store the last time the overview was updated (for optimizing interactivity during 2D slicing)
      Timer m_lastOverviewUpdateTimer;

      /// store the last time the UD View was updated (for optimizing interactivity during 2D slicing)
      Timer m_lastUDViewUpdateTimer;

    protected:

      /// Keyboard event handler: key pressed.
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// called when the view is being resized
      afx_msg void OnSize( UINT uType, int iSizeX, int iSizeY );

      /// WM_PAINT handler
      void OnPaint();

      /// Jump to the previous segment
      afx_msg void OnJumpPrevSegment();

      /// Jump to the next segment
      afx_msg void OnJumpNextSegment();

      /// Add new report snapshot
      afx_msg void OnReportAddSnapshot();

      //{{AFX_MSG(Segments3DLayout)
      /// query whether to enable jumping between datasets
      LRESULT OnQueryEnableSupineProneSwitch(WPARAM, LPARAM);
      /// Toggle line measure mode
      afx_msg void OnToggleLineMeasure();

      /// Toggle lighting mode
      afx_msg void OnToggleLighting();

      /// Toggle roi measure mode
      afx_msg void OnToggleRoiMeasure();

      /// Toggle translucent mode
      afx_msg void OnToggleTranslucent();

      /// Updates the next missed region option
      afx_msg void OnUpdateNextMissedRegion(CCmdUI* pCmdUI);

      /// adjust the control pad display menu items appropriate
      afx_msg void OnUpdateControlPadMenuItemAdjust(CCmdUI* pCmdUI);

		  /// message to toggle autofly mode
      afx_msg void OnToggleAutofly();

      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDT_3DSEGMENTS_LANDSCAPE; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return (m_bToolbarAtBottom ? IDT_3DSEGMENTS_WIDESCREEN : IDT_3DSEGMENTS_WIDESCREEN_BARTOP); }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDT_3DSEGMENTS; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return IDT_3DSEGMENTS_PORTRAIT_LARGE; }

      /// message to show popup menu for a viewwindow
      virtual uint4 OnShowPopupMenu(const GLChildView& childView, const uint4 uPopupMenuID) const;
	  /// flag for the toolbar position
	  bool m_bToolbarAtBottom;

    }; // class Segments3DLayout
  } // namespace FramesManager
} // namespace ColonViews


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: Segments3DView.h,v $
// Revision 1.11.2.11  2010/04/17 05:16:53  dongqing
// small portrait display layouts
//
// Revision 1.11.2.10  2010/04/14 04:11:50  dongqing
// portrait changes
//
// Revision 1.11.2.9  2010/03/20 12:34:42  dongqing
// added menu item for dissection plane display in overview
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.8  2010/01/14 14:47:45  dongqing
// added an oblique view to 3d segments (to fill out empty space)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.7  2010/01/07 22:23:24  dongqing
// 3D segments portrait layout
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.6  2010/01/07 18:49:11  dongqing
// widescreen 3d segments view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.5  2009/12/09 20:22:49  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.11.2.4  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.11.2.3  2008/01/31 20:18:03  jmeade
// removing code checked in before it's time.
//
// Revision 1.11.2.2  2008/01/11 02:46:43  jmeade
// issue 5907.
//
// Revision 1.11.2.1  2007/10/23 00:38:24  jmeade
// Issue 5844: disable missed region jump.
//
// Revision 1.11  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.10  2007/02/09 19:08:20  romy
// Renderer conditions modify to fix Issue#5339.
//
// Revision 1.9  2007/02/02 21:51:40  romy
// added GLRendererViewers  method
//
// Revision 1.8  2006/10/12 20:48:58  jmeade
// Issue 5048: default text colour for layouts.
//
// Revision 1.7  2006/10/04 17:57:12  jmeade
// allow parent to handle view mode update.
//
// Revision 1.6  2006/10/04 15:43:08  jmeade
// patient info dialog; enable dataset toggling.
//
// Revision 1.5  2006/09/28 16:00:24  frank
// added controls to 3D segment view
//
// Revision 1.4  2006/09/19 16:12:11  frank
// fixing up the experimental layouts
//
// Revision 1.3  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.4.2.2  2003/02/25 17:57:05  jmeade
// Coding standards.
//
// Revision 1.4.2.1  2003/02/22 12:08:44  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.4  2002/11/15 00:37:24  jmeade
// No hard-coding of # of child views.
//
// Revision 1.3  2002/10/01 22:07:27  kevin
// This file does NOT need registration handler
//
// Revision 1.2  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 1.1  2002/08/20 19:15:33  kevin
// 2D and 3D segment views.
//
// Revision 1.1  2002/08/15 15:54:23  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 1.12  2001/11/23 17:26:44  jmeade
// Enable supine/prone toggle in Registration view (via style in resource).
//
// Revision 1.11  2001/11/21 20:26:25  jmeade
// Black text on the lighter registration view background.
//
// Revision 1.10  2001/11/21 01:37:10  kevin
// Jeff M. fixed some stuff with S/P 2D flight linking
//
// Revision 1.9  2001/11/17 03:51:14  jmeade
// Popup menus, coding standards.
//
// Revision 1.8  2001/11/15 22:13:48  binli
// switch updateview when dbl-click on overview.
//
// Revision 1.7  2001/11/14 02:29:10  kevin
// More CLenaup of SUpine/Prone and added in orientation
// according to  submarine "lookahaed" algrotihm
//
// Revision 1.6  2001/11/14 01:34:04  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 1.5  2001/11/09 04:18:22  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 1.4  2001/11/08 20:22:27  ana
// All the view updates are being made after the flying stops. To switch between
// the registration modes use S for Splenic Fleaxure mode or P for Centerline
// Percentage mode.
//
// Revision 1.3  2001/11/07 20:49:59  ana
// When stop flying update the other view accordingly with suzi's registration.
//
// Revision 1.2  2001/11/06 20:00:06  jmeade
// Override GLRenderViews() to only render view that is flying.
//
// Revision 1.1  2001/11/05 17:55:12  ana
// Registration view added.
//
// Revision 1.1.2.2  2001/10/30 21:17:16  ana
// Excluded registration initialization and computatio. Now, it just uses the
// registration computed at VerifyView. Reader Global has the registration
// instatiation.
//
// Revision 1.1.2.1  2001/08/21 00:27:29  jmeade
// Added registration view
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Segments3DView.h,v 1.11.2.11 2010/04/17 05:16:53 dongqing Exp $
// $Id: Segments3DView.h,v 1.11.2.11 2010/04/17 05:16:53 dongqing Exp $
