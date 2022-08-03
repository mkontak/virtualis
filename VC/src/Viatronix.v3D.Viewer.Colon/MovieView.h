// $Id: MovieView.h,v 1.10.2.5 2010/09/20 18:41:34 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeff Meade         jmeade@viatronix.com

// pagmas
#pragma once

// includes
#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"
#include "WriteAVI.h"
//#include "Timer.h"
#include "MovieSaveStateOptions.h"

/////////////////////////////////////////////////////////////////////////////
// MovieLayoutFrame frame

namespace ColonViews
{

  namespace Layouts
  {
    /// movie preview layout
    class MovieLayoutFrame : public ChildLayoutFrame
    {
      DECLARE_DYNCREATE(MovieLayoutFrame)

    protected:

      /// constructor; 
      MovieLayoutFrame();

    };

  } // namespace Layouts

} // namespace ColonViews

/////////////////////////////////////////////////////////////////////////////

namespace ColonViews
{

  namespace Layouts
  {

    /// movie preview layout
    class MovieLayout : public ColonViatronixWnd  
    {
      // member functions
    public:

      /// Constructor
      MovieLayout();

      /// Destructor
      virtual ~MovieLayout();

    protected:

      // Initialization
      void CreateChildViews(HWND hWndThis);

      /// Renders the views
      void GLRenderViews();

      /// Initializes movie file
      void InitializeMovie();

      /// Converts movie file
      bool ConvertMovie();

      /// Waits on event for movie completion
      static UINT WaitOnMovieCompletion(LPVOID pParam);

      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

      DECLARE_MESSAGE_MAP()
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);
      void OnLButtonDown(UINT uFlags, CPoint point);
      void OnMButtonDown(UINT uFlags, CPoint point);
      void OnRButtonDown(UINT uFlags, CPoint point);
      void OnLButtonUp(UINT uFlags, CPoint point);
      void OnMButtonUp(UINT uFlags, CPoint point);
      void OnRButtonUp(UINT uFlags, CPoint point);
      void OnLButtonDblClk(UINT uFlags, CPoint point);
      void OnMButtonDblClk(UINT uFlags, CPoint point);
      void OnRButtonDblClk(UINT uFlags, CPoint point);
      void OnMouseMove(UINT uFlags, CPoint point);
      void OnDisableCommand();
      afx_msg virtual void OnSize(UINT uType, int cx, int cy);

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return !m_movie.m_format.m_bIncludeOverview ? IDT_MOVIEVIEW_SINGLE : IDT_MOVIEVIEW_DUAL; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return !m_movie.m_format.m_bIncludeOverview ? IDT_MOVIEVIEW_SINGLE_WIDESCREEN : IDT_MOVIEVIEW_DUAL_WIDESCREEN; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return !m_movie.m_format.m_bIncludeOverview ? IDT_MOVIEVIEW_SINGLE_PORTRAIT : IDT_MOVIEVIEW_DUAL_PORTRAIT; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID()   { return !m_movie.m_format.m_bIncludeOverview ? IDT_MOVIEVIEW_SINGLE_PORTRAIT_LARGE : IDT_MOVIEVIEW_DUAL_PORTRAIT_LARGE; }

    private:

      /// updates the progress 
      void UpdateProgress();

    private:

      /// AVI file
      WriteAVI * m_pAVI;

      /// milliseconds per frame
      uint4 m_uMillisecondsPerFrame;

      /// total number of frames 
      uint4 m_uTotalFrames;

      /// current frame
      uint4 m_uCurrentFrame;

      /// render size
      int4 m_iRenderSize;

      /// image size
      int4 m_iImageSize;

      /// frame rate
      int4 m_iFrameRate;

      /// render size
      Point2D< uint4 > m_frameSize;

      /// Device context for storing movie frames
      WinUtils::BitmapDC m_movieFrameBDC;

      /// border between the two images in each frame
      static const uint4 m_uMidBorder;

      /// Handle of wait event 
      HANDLE m_hMovieWait;

      /// Time elapsed for movie creation
      Timer m_progressTimer;

      /// Save and return state of software after movie completion
      Movies::MovieSaveStateOptions m_saveState;
    };

  } // namespace Layouts

} // namespace ColonViews


// $Log: MovieView.h,v $
// Revision 1.10.2.5  2010/09/20 18:41:34  dongqing
// Jeff's bug fix for his various monitor aspect ratio work
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.4  2010/05/19 18:59:24  dongqing
// Fix the Portray movie view stretching problem
//
// Revision 1.10.2.3  2010/04/17 05:16:16  dongqing
// small portrait display layouts
//
// Revision 1.10.2.2  2010/02/26 13:32:40  dongqing
// widescreen movieview
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.1  2009/12/09 20:26:59  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.10  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.9  2007/03/08 22:22:05  romy
// code review - header files adjusted
//
// Revision 1.8  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.7  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.6.2.1  2005/12/23 16:49:16  frank
// Issue #4537: Reworked layout to accomodate varying desktop sizes
//
// Revision 1.6  2005/10/28 13:28:15  geconomos
// now interploating captured frame positions
//
// Revision 1.5  2005/10/25 15:47:37  jmeade
// ParentViews ==> Layouts
//
// Revision 1.4  2005/09/07 13:39:38  geconomos
// Issue# 4417:  Removed some unwanted code from the rendering loop. Needs more investigating to figure out why performace halved between 1.5 and 1.6
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.1.4.1.2.3.2.1  2005/07/20 14:10:48  jmeade
// Issue 4295: Disable commands for F-keys.
//
// Revision 1.1.4.1.2.3  2005/06/08 19:45:27  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.4.1.2.2.2.1  2005/05/25 15:50:22  geconomos
// initial backup
//
// Revision 1.1.4.1.2.2  2005/04/28 17:20:00  jmeade
// Issue 4147: Fix bugs to movie frame capture introduced by new framework changes.
//
// Revision 1.1.4.1.2.1  2005/04/22 04:28:34  jmeade
// Issue 4140: Propagate paint/update window call framework changes.
//
// Revision 1.1.4.1  2005/02/15 18:50:04  jmeade
// merge from vc1.4 branch.
//
// Revision 1.1.2.6  2005/02/10 00:40:48  jmeade
// Issue 3882: Turn off 2D HU and Vol/World coordinate display in movie frames.
//
// Revision 1.1.2.5  2005/01/27 18:18:58  jmeade
// Only show appropriate views during movie creation;
// Display time remaining for movie creation.
//
// Revision 1.1.2.4  2005/01/26 17:11:36  jmeade
// Disable all tabs during movie creation.
//
// Revision 1.1.2.3  2005/01/25 00:23:52  jmeade
// Separate movie-creation view: bug fix, identify type of view (and slice orientation for 2d) for movie
//
// Revision 1.1.2.2  2005/01/24 16:36:00  jmeade
// Separate movie-creation view, incremental checkin.
//
// Revision 1.1.2.1  2005/01/20 21:19:30  jmeade
// Separate movie view, incremental checkin.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieView.h,v 1.10.2.5 2010/09/20 18:41:34 dongqing Exp $
// $Id: MovieView.h,v 1.10.2.5 2010/09/20 18:41:34 dongqing Exp $
