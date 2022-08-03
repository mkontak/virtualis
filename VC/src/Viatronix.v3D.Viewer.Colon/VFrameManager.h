// $Id: VFrameManager.h,v 1.9.2.4 2009/12/10 20:06:37 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VFrameManager.h
// Description: Interface for the VFrameManager class
// Owner: Jeff Meade  jmeade@viatronix.net


#if !defined(AFX_VFRAMEMANAGER_H__5A22B314_5660_424E_817A_B037F5E7881C__INCLUDED_)
#define AFX_VFRAMEMANAGER_H__5A22B314_5660_424E_817A_B037F5E7881C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
typedef std::vector<std::string> stdStringVector;

#include "TrackBall.h"

namespace ColonViews
{

  namespace FramesManager
  {

    /// Helps to manage transitions between layouts
    class ViatronFrameManager
    {
    public:
      /// Whether the view is first gaining focus, losing focus, or changing subviews
      enum FocusChangeEnum { eFocusGain, eFocusLose, eFocusViewChange };

    public:
      /// Constructor
      ViatronFrameManager(HWND hMainFrameWnd = HWND(NULL));

      /// Create handler
      void Create(HWND hMainFrameWnd);

      /// Called when the frame is being initialized
      virtual void OnFrameInit(stdStringVector & vTabStrings) = 0;
      /// Called when a view is gain or losing focus, or when a subview selection is changing
      virtual CRuntimeClass * OnChangeViewSel(const int4 iPreviousSubView, int4& iNewSubView, const FocusChangeEnum eFocusChange) = 0;

    protected:
      /// Target for all frame window messages
      HWND m_hMainFrameWnd;

      /// Allowing calls to OnChangeViewSel to choose whether to use the enable tab bit vector
      static uint8 m_uGlobalDefaultParam;

      /// The last selected subview
      uint4 m_uLastSelectedSubView;

    protected:

      /// Class to save the state of the overview trackball between view changes
      class TrackBallsSaver
      {
      public:
        /// Constructor
        TrackBallsSaver();
        /// Initialize
        void Init();
        /// Save state
        void Save();
        /// Set state
        void Set();
      private:
        /// Global overview trackball saver
        ReaderLib::TrackBall m_global;
        /// Primary study overview trackball saver
        ReaderLib::TrackBall m_primary;
        /// Secondary study overview trackball saver
        ReaderLib::TrackBall m_secondary;
      }; // trackballsaver
    }; // ViatronFramesManager

    // the various frames
    namespace VFrameManager
    {
      // study browser layout
      class StudyBrowserFrameManager : public ViatronFrameManager
      {
      public:
        // NOTE: PACS not implemented in V3D Browser as of V1.2
        enum LocationFrameEnum {FILESYSTEM_FRAME=0, NUM_FRAMES, PACS_FRAME, REGISTRATION_FRAME};

      public:
        /// Constructor
        StudyBrowserFrameManager(HWND hMainFrameWnd = HWND(NULL));

        /// Called when the frame is being initialized
        void OnFrameInit(stdStringVector & vTabStrings);
        /// Called when a view is gain or losing focus, or when a subview selection is changing
        CRuntimeClass * OnChangeViewSel(const int4 iPreviousSubView, int4& iNewSubView, const FocusChangeEnum eFocusChange);
      }; // StudyBrowserFrameManager

      // verification layout
      class VerifyFrameManager : public ViatronFrameManager
      {
      public:
        /// Constructor
        VerifyFrameManager(HWND hMainFrameWnd = HWND(NULL));

        /// Called when the frame is being initialized
        void OnFrameInit(stdStringVector & vTabStrings);
        /// Called when a view is gain or losing focus, or when a subview selection is changing
        CRuntimeClass * OnChangeViewSel(const int4 iPreviousSubView, int4& iNewSubView, const FocusChangeEnum eFocusChange);

      private:
        /// Saves state of overview trackball
        TrackBallsSaver m_trackballsaver;
      }; // VerifyFrameManager

      // navigation layout
      class NavFrameManager : public ViatronFrameManager
      {
      public:
        /// Positions
        enum { ePOS_STANDARD = 0, ePOS_ENDOORTHO = 1, ePOS_3DSEGMENT = 2, ePOS_ORTHO = 3, ePOS_REGISTERED = 4,
          ePOS_UDSEGMENTS_MAUI = 5, ePOS_OBLIQUE = 6, ePOS_2DSEGMENT = 6, ePOS_CYLSEGMENT = 7, ePOS_MOVIE = 8, ePOS_FULLVIEW = 9, ePOS_BLANKVIEW = 10 };

      public:
        /// Constructor
        NavFrameManager(HWND hMainFrameWnd = HWND(NULL));

        /// Called when the frame is being initialized
        void OnFrameInit(stdStringVector & vTabStrings);
        /// Called when a view is gain or losing focus, or when a subview selection is changing
        CRuntimeClass * OnChangeViewSel(const int4 iPreviousSubView, int4& iNewSubView, const FocusChangeEnum eFocusChange);

      private:
        /// Saves state of overview trackball for navigation view
        TrackBallsSaver m_navigationTS;
        /// Saves state of overview trackball for endoaxial view
        TrackBallsSaver m_endoaxialTS;
      }; // NavFrameManager

      // report layout
      class ReportFrameManager : public ViatronFrameManager
      {
      public:
        /// Layout positions
        enum { eREPORTPAGE_FINDINGS = 0, eREPORTPAGE_PATIENTINFO = 1, eREPORTPAGE_PREVIEW = 2, eREPORTPAGE_OTHERFINDINGS = 3 };

      public:
        /// Constructor
        ReportFrameManager(HWND hMainFrameWnd = HWND(NULL));

        /// Called when the frame is being initialized
        void OnFrameInit(stdStringVector & vTabStrings);
        /// Called when a view is gain or losing focus, or when a subview selection is changing
        CRuntimeClass * OnChangeViewSel(const int4 iPreviousSubView, int4& iNewSubView, const FocusChangeEnum eFocusChange);
      }; // ReportFrameManager
    }; // namespace VFrameManager

    // helper typedef
    typedef ViatronFrameManager * VFrameManagerPtr;

  } // namespace FramesManager

} // namespace ColonViews


#endif // !defined(AFX_VFRAMEMANAGER_H__5A22B314_5660_424E_817A_B037F5E7881C__INCLUDED_)


// Revision History:
// $Log: VFrameManager.h,v $
// Revision 1.9.2.4  2009/12/10 20:06:37  dongqing
// cleaner reloading of layouts, for e.g. when changing scrollbar orientation
//
// Revision 1.9.2.3  2008/02/15 01:46:30  jmeade
// Issue 5924: remove otherfindings tab from release builds.
//
// Revision 1.9.2.2  2008/02/14 06:59:58  jmeade
// Issue 5924: identify report layout pages properly.
//
// Revision 1.9.2.1  2007/09/20 23:08:12  jmeade
// full screen view
//
// Revision 1.9  2007/03/08 17:09:48  jmeade
// code standards.
//
// Revision 1.8  2007/03/07 20:18:53  jmeade
// comments.
//
// Revision 1.7  2007/01/03 05:33:32  jmeade
// future ud segments layout.
//
// Revision 1.6  2006/12/16 04:52:01  jmeade
// Issue 5220: full oblique mode on for cross-sectional view mode.
//
// Revision 1.5  2006/10/03 01:23:17  jmeade
// UD segments.
//
// Revision 1.4  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.2.4.2.4.1.4.1  2005/07/12 15:55:55  jmeade
// Issue 4264: remove full oblique layout for this release.
//
// Revision 3.2.4.2.4.1  2005/05/25 01:56:41  jmeade
// oblique review.
//
// Revision 3.2.4.2  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.2.4.1  2003/02/14 15:37:20  jmeade
// Issue 2843:  Store current subview selection, and reset when views are first invoked.
//
// Revision 3.2  2002/08/15 15:37:31  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.1  2001/11/05 17:49:59  ana
// Registration view added.
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:14   ingmar
// Initial revision.
// Revision 1.9  2001/08/28 21:30:22  jmeade
// Removed (unimplemented) PACS option
//
// Revision 1.8  2001/05/23 17:56:09  jmeade
// code conventions
//
// Revision 1.7  2001/05/17 06:18:09  jmeade
// tabs to spaces; Enumerated tab selections in Study Browser Frame Mgr;
// Instances of ViatronFrameManager derivates must set app main window
// thru Create() method
//
// Revision 1.6  2001/04/13 02:35:22  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VFrameManager.h,v 1.9.2.4 2009/12/10 20:06:37 dongqing Exp $
// $Id: VFrameManager.h,v 1.9.2.4 2009/12/10 20:06:37 dongqing Exp $
