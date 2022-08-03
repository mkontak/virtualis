// $Id: SegmentsCylindricalView.h,v 1.6.2.2 2010/04/17 05:16:47 dongqing Exp $
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


// defines
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


/////////////////////////////////////////////////////////////////////////////
// SegmentsCylindricalViewFrame frame


// namespaces
namespace ColonViews
{
  namespace Layouts
  {

    /**
     * cylindrical segments
     */
    class SegmentsCylindricalLayoutFrame : public ChildLayoutFrame
    {
      DECLARE_DYNCREATE(SegmentsCylindricalLayoutFrame)

    public:

      // protected constructor used by dynamic creation
      SegmentsCylindricalLayoutFrame();           

      // Operations
    public:

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(SegmentsCylindricalLayoutFrame)
      //}}AFX_VIRTUAL

      // Implementation
    public:

      // Generated message map functions
      //{{AFX_MSG(SegmentsCylindricalLayoutFrame)
      // NOTE - the ClassWizard will add and remove member functions here.
      //}}AFX_MSG
    };

    /////////////////////////////////////////////////////////////////////////////

    /**
     * cylindrical segments
     */
    class SegmentsCylindricalLayout : public ColonViatronixWnd
    {
      // functions
    public:

      /// Constructor
      SegmentsCylindricalLayout();

    protected:

      /// Keyboard event handler: key pressed.
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// called when the view is being resized
      afx_msg void OnSize( UINT uType, int iSizeX, int iSizeY );

      /// Renders the views

      //{{AFX_MSG(SegmentsCylindricalLayout)
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDT_CYLINDRICAL_SEGMENTS; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return IDT_CYLINDRICAL_SEGMENTS; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDT_CYLINDRICAL_SEGMENTS; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return IDT_CYLINDRICAL_SEGMENTS; }

    }; // class SegmentsCylindricalLayout


    // namespaces

  } // Layouts

} // ColonViews


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: SegmentsCylindricalView.h,v $
// Revision 1.6.2.2  2010/04/17 05:16:47  dongqing
// small portrait display layouts
//
// Revision 1.6.2.1  2009/12/09 20:22:49  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.6  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.5  2006/10/12 20:48:59  jmeade
// Issue 5048: default text colour for layouts.
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
// Revision 1.2.2.1  2003/02/22 12:08:44  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 1.2  2002/11/15 00:37:24  jmeade
// No hard-coding of # of child views.
//
// Revision 1.1  2002/11/11 16:17:01  kevin
// Initial version of cylindrical -- still needs some polishing
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SegmentsCylindricalView.h,v 1.6.2.2 2010/04/17 05:16:47 dongqing Exp $
// $Id: SegmentsCylindricalView.h,v 1.6.2.2 2010/04/17 05:16:47 dongqing Exp $
