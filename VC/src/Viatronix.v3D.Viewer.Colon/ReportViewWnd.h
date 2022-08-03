// $Id: ReportViewWnd.h,v 1.14.2.3 2010/04/17 05:16:59 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Jeff Meade (Author: George Economos)


// pragmas
#pragma once


// includes
#include "ViatronStudy.h"
#include "ViatronViewWnd.h"
#include "trackball.h"
#include "VxVCReport.h"
#include <string>

#if defined(_DEBUG)
# import "..\..\..\..\Libraries\com\ReportLibd.dll" named_guids
#elif defined(FINAL_RELEASE)
# import "..\..\..\..\Libraries\com\ReportLib.dll" named_guids
#else
# import "..\..\..\..\Libraries\com\ReportLibdr.dll" named_guids
#endif

#include <atlbase.h>
#include "ReportColonFindings.h"
#include "ReportOtherFindings.h"
#include "ReportNonDistendedAreas.h"
#include "ReportMiscellaneous.h"
#include <vector>
#include <list>


// namespaces
namespace ColonViews
{
  namespace Layouts
  {


    /**
     * Work area for colon findings reporting
     */
    class ReportLayout : public ColonViatronixWnd
    {
    public:

      /// Constructor
      ReportLayout();

      /// Destructor
      virtual ~ReportLayout();

      /// Creates child view windows
      virtual void CreateChildViews(HWND hWndThis);

      /// Initial size of child views
      virtual void CalculateViewRects(const int4 iWidth, const int4 iHeight);

      /// sets the marked completed flag for the current revision
      static bool SetMarkCompleteFlag(const bool bCompleted);

      /// Returns the name of the currently opened report file
      static std::string GetReportFileName() { return m_report.m_sReportFile; }

      /// Returns TRUE if the report file exists
      static BOOL ReportFileExists();

      /// Replaces patient information in the report file
      static BOOL ReplacePatientInfo(LPCTSTR pszReport, LPCTSTR pszEnPatientName, LPCTSTR pszEnPatientID);

      /// Adds a snapshot of the specified views to the report
      static BOOL AddReportEntry(const std::list<GLChildViewPtr>& childViewList, const MeasureLineList& currMeasureList /* = MeasureLineList()*/);

      /// Updates the *displayed* distance to rectum in the list of report entries
      static void UpdateEntryDistances(const bool bSegmentOrderChanged);

      /// Gets a pointer to the report COM interface
      static ReportLib::IReportRevisionPtr GetReport() { return m_report.m_spRevision; }

      /// Generates a listing of non-distended areas in the report
      static bool AddNonDistendedAreas(GLChildViewRef childView, const bool bSupine, const bool bInitialAdd = false);

      /// Returns TRUE if the report can be modified (i.e. is not read-only)
      static BOOL CanModifyReport();

      /// Returns TRUE if the current user has proper access level to modify any report
      static BOOL CanOpenReport();

      /// Returns TRUE if the current user has proper access level to modify any report
      static BOOL HasPermissions();

      /// Sets the database location of the current report
      /// saves the report and all study session data
      static void SaveSessionData();

    protected:

      /// Activates the specified view
      BOOL ActivateView(ColonReport::ReportWndBase* pViewWnd);

      /// Adds an image to the given entry
      static BOOL AddImageToEntry(GLChildViewRef view, ReportLib::IReportEntryPtr& spEntry);

      /// derived classes can override to remove areas from the parent region
      virtual void OnUpdateBackgroundRegion( CRgn & parentRgn );

      // Message handlers
      //{{AFX_MSG(ReportLayout)
      afx_msg void OnPaint();
      afx_msg BOOL OnEraseBkgnd( CDC * pdc );
      afx_msg void OnSize( UINT uType, int4 cx, int4 cy );
      afx_msg void OnOpenReport();
      afx_msg void OnColonFindings();
      afx_msg void OnNonDistendedAreas();
      afx_msg void OnOtherFindings();
      afx_msg void OnRecommendations();
      afx_msg void OnMiscellaneous();
      afx_msg void OnReportSave();
      afx_msg void OnPreviewReport();
      afx_msg virtual void OnMouseMove(UINT uFlags, CPoint point);
      afx_msg void OnToggleReportCompleteMark();
      afx_msg void OnDestroy();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

    protected:

      /// Colon findings window
      ColonReport::ReportColonFindings m_wndColonFindings;

      /// Other findings window
      ColonReport::ReportOtherFindings m_wndOtherFindings;

      /// Non-distended areas window
      ColonReport::ReportNonDistendedAreas m_wndNonDistendedAreas;

      /// Miscellaneous findings window
      ColonReport::ReportMiscellaneous m_wndMiscellaneous;

      /// Report instance
      static VxVCReport m_report;

      /// Current report view window
      ColonReport::ReportWndBase* m_pCurrentView;

      /// Saves the current dataset trackball at the time the report view is instantiated (reset on view close)
      ReaderLib::TrackBall m_datasetTrackBall;

      /// Saves the secondary dataset trackball at the time the report view is instantiated (reset on view close)
      ReaderLib::TrackBall m_secondaryTrackBall;

      /// Saves the current overview modes at the time the report view is instantiated (reset on view close)
      VtxViewModes m_overViewMode;

      /// Saves the current dataset orientation so that it can be reset upon exiting the view
      ReaderLib::DatasetOrientations::TypeEnum m_eRestoreOrientation;

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return 0; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return 0; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return 0; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return 0; }

    private:

      /// draw buffer
      Gdiplus::Bitmap * m_pDrawBuffer;

    }; // class ReportLayout


    // namespaces
  } // Layouts

} // namespace ColonViews


// $Log: ReportViewWnd.h,v $
// Revision 1.14.2.3  2010/04/17 05:16:59  dongqing
// small portrait display layouts
//
// Revision 1.14.2.2  2009/12/09 20:20:30  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.14.2.1  2007/03/23 02:00:18  jmeade
// Issue 5502: save changes for current view when destroying reportlayout win.
//
// Revision 1.14  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.13  2007/02/01 21:35:08  romy
// added CanOpenReport method
//
// Revision 1.12  2006/01/31 14:02:08  frank
// code review
//
// Revision 1.11  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.10  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.9.2.1  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.9  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.8  2005/10/04 02:07:20  geconomos
// added drawing code for header images
//
// Revision 1.7  2005/09/13 17:27:19  jmeade
// ColonReport namespace.
//
// Revision 1.6  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.5  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.4  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.3  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/05 14:14:10  geconomos
// updated #import for location of reporting library
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.19.2.6.2.1.6.1  2005/07/01 15:05:06  jmeade
// comment.
//
// Revision 3.19.2.6.2.1  2005/03/12 14:36:03  jmeade
// Snapshots wit 2d measurements interpreted as polyps (as with 3d measurements).
//
// Revision 3.19.2.5.2.2  2005/02/23 03:16:29  jmeade
// Snapshots wit 2d measurements interpreted as polyps (as with 3d measurements).
//
// Revision 3.19.2.5.2.1  2004/10/23 00:47:06  jmeade
// Return to navigation screen after marking study complete.
//
// Revision 3.19.2.5  2004/07/21 20:25:40  jmeade
// Issue 3630: Note where appropriate when the rectum-distance is unable to be calculated.
//
// Revision 3.19.2.4  2004/07/17 03:12:42  jmeade
// Issue 3630: Update colon finding distances when colon segments are changed.
//
// Revision 3.19.2.3  2004/03/30 23:45:47  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.19.2.2  2003/02/22 11:59:03  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.19.2.1  2003/02/19 22:24:32  jmeade
// Issue 2842:  Implemented variable number of snapshots per report entry.
//
// Revision 3.19  2002/12/12 22:07:52  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.18  2002/11/15 00:37:36  jmeade
// No hard-coding of # of child views.
//
// Revision 3.17  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.15  2002/07/18 20:25:39  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.14  2002/06/26 17:35:44  jmeade
// Moving more hard-coded strings to the string table.
//
// Revision 3.13  2002/05/09 20:54:15  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 3.12  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.11  2002/04/12 02:14:56  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.10  2002/03/15 03:27:47  jmeade
// Project configuration changes: Release ==> Debug-Release, Final Release ==> Release.
//
// Revision 3.9  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.8.2.5  2002/04/18 01:14:00  jmeade
// Issue 2043:  Return success of SetMarkCompletedFlag; return string class instances, not pointers.
//
// Revision 3.8.2.4  2002/02/27 22:09:04  jmeade
// Cosmetic changes.
//
// Revision 3.8.2.3  2002/02/26 22:36:32  binli
// code cleaned
//
// Revision 3.8.2.2  2002/02/26 21:42:43  binli
// issue 1900: override OnMouseMove(): keep the focus & no need to call fucntions defined in ViatronViewWnd & GLViewWnd
//
// Revision 3.8.2.1  2002/02/19 18:32:31  jmeade
// Code standards.
//
// Revision 3.8  2002/01/30 23:11:31  jmeade
// Issue 1721:  Can only change information (done in OpenReport) on a writable file,
//  so copy read-only report to temporary before generating pdf.
//
// Revision 3.7  2002/01/25 21:04:58  binli
// issue 1652: underlying function: File::CreateTempFile() creates a temp file now.
//     (as compared with old version, which just creates a temp file NAME)
//
// Revision 3.6  2002/01/23 21:46:12  binli
// issue 1589: Endo-SliceView: take snapshot from all 3 views now.
//
// Revision 3.5  2002/01/17 02:23:50  jmeade
// Generate PDF Function; comments.
//
// Revision 3.4  2001/11/17 03:35:18  jmeade
// Patient view stored in base class.
//
// Revision 3.3  2001/11/09 04:50:21  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2  2001/10/30 00:44:27  geconomos
// Added methods for getting/setting MarkedComplete flag
//
// Revision 3.1.2.5  2001/11/06 01:42:57  jmeade
// Coding standards.
//
// Revision 3.1.2.4  2001/11/04 05:14:59  jmeade
// Code standards.
//
// Revision 3.1.2.3  2001/10/31 00:33:38  jmeade
// Code conventions
//
// Revision 3.1.2.2  2001/10/31 00:30:19  jmeade
// From Revision 3.3  2001/10/30 00:44:27  geconomos:  Added methods for getting/setting MarkedComplete flag
//
// Revision 3.1.2.1  2001/10/28 11:11:27  jmeade
// Coding standards; Safer references instead of pointers
//
// Revision 3.1  2001/10/17 21:08:37  binli
// issue 1184: 2 solutions: a> hide 3DViewer when the report is opened from CD (Jeff M did)
//                                       b> set up the flag DBLocation, and ensure only a ReadOnly reportview can created. (BinLi did)
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
//    Rev 2.4   Oct 03 2001 18:12:32   binli
// bug fix: error when open study from CD
// fix: readonly for all level custom.
// (.vrx file exists, open it, else, create new temporary file, but no save it)
// 
//    Rev 2.3   Oct 03 2001 11:10:24   binli
// report bug: ensure to create all report images.
// fixed: temporay lock of NavigationBar.
// 
//    Rev 2.2   Oct 01 2001 17:09:34   binli
// functions to ensure no bad/black images in report file: synchronous control.
// 
//    Rev 2.1   Sep 27 2001 11:06:48   jmeade
// Use static header read function in PatientInfoDlg
// 
//    Rev 2.0   Sep 16 2001 23:41:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:50   ingmar
// Initial revision.
// Revision 1.32  2001/09/14 23:06:10  jmeade
// Localized calculation of report filename (to reduce future errors),
// Code conventions, topmost for debug only, save only with proper permission
//
// Revision 1.31  2001/09/10 08:36:57  soeren
// new dirs
//
// Revision 1.30  2001/08/09 17:11:39  binli
// bug 509: close .vrx file after the study is closed.
//
// Revision 1.29  2001/07/09 18:21:13  jmeade
// Bug fix 522: preview report message (not a direct call), like other handlers; delete temp pdf
// after preview; redraw windows and switch views (or close study) as appropriate
// after preview
//
// Revision 1.28  2001/05/30 14:23:21  jmeade
// coding standards to fix name clashes
//
// Revision 1.27  2001/05/17 20:42:05  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.26  2001/05/10 21:04:40  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.25  2001/05/07 00:51:33  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.24  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.23  2001/03/21 15:47:27  geconomos
// Fixed bug with read only report based on user level
//
// Revision 1.22  2001/03/08 16:34:55  geconomos
// Added read only only report functionailty.
//
// Revision 1.21  2001/02/28 16:37:01  geconomos
// no message
//
// Revision 1.20  2001/02/12 16:44:39  geconomos
// Fixe supine and prone dilema in non distended areas
//
// Revision 1.19  2001/02/07 16:25:04  geconomos
// Added revisions to report file
//
// Revision 1.18  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportViewWnd.h,v 1.14.2.3 2010/04/17 05:16:59 dongqing Exp $
// $Id: ReportViewWnd.h,v 1.14.2.3 2010/04/17 05:16:59 dongqing Exp $
