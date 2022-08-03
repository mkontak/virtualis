//////////////////////////////////////////////////////////////////////
// $Id: StudyBrowserView.h,v 1.7 2007/03/09 01:11:49 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StudyBrowserViewWnd.h
// Description: Interface for the StudyBrowserViewWnd class.
// Created:
// Owner: Jeff Meade  jmeade@viatronix.net

#pragma once

#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"

namespace ColonViews
{

  namespace Layouts
  {
    /* original study browser dialog layout (obsolete) */
    class StudyBrowserFrame : public ChildLayoutFrame
    {
    public:
      /// layout window
      class ViewWnd : public GLWnd
      {
      public:
        /// constructor
        ViewWnd();
        /// destructor
        virtual ~ViewWnd();
        /// message map
        DECLARE_MESSAGE_MAP()

      protected:
        /// view creation
        void CreateChildViews(HWND hWndThis);
        /// calculate view rectangles
        void CalculateViewRects(const int4 iWidth, const int4 iHeight);

      private:
      }; // ViewWnd

    public:
      /// constructor
      StudyBrowserFrame ();
      /// destructor
      virtual ~StudyBrowserFrame();

    private:
      /// dynamic class creation
      DECLARE_DYNCREATE(StudyBrowserFrame)
    };

  } // namespace Layouts

} // namespace ColonViews


// Revision History:
// $Log: StudyBrowserView.h,v $
// Revision 1.7  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.6  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.5  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.4  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.6.2.1  2003/02/22 12:20:28  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.6  2002/11/15 00:45:28  jmeade
// No hard-coding of # of child views.
//
// Revision 3.5  2002/09/25 19:31:40  jmeade
// Unused study open accelerator message.
//
// Revision 3.4  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.2  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.1  2001/10/18 16:43:09  jmeade
// Coding standards (removing unused resource items, obsolete code)
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:42:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:02   ingmar
// Initial revision.
// Revision 1.7  2001/08/30 19:06:05  jmeade
// ID 120:  Show Deleted studies menu option
//
// Revision 1.6  2001/06/14 00:36:16  jmeade
// coding conventions
//
// Revision 1.5  2001/05/07 22:27:21  jmeade
// no message
//
// Revision 1.4  2001/04/13 02:34:36  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/StudyBrowserView.h,v 1.7 2007/03/09 01:11:49 jmeade Exp $
// $Id: StudyBrowserView.h,v 1.7 2007/03/09 01:11:49 jmeade Exp $
