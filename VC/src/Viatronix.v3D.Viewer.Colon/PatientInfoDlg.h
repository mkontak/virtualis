//////////////////////////////////////////////////////////////////////
// $Id: PatientInfoDlg.h,v 1.10.2.9 2010/04/11 11:56:19 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PatientInfoDlg.h
// Author: Jeff Meade  jmeade@viatronix.net

// paragmas
#pragma once

// includes
//#include "ViatronDialog.h"
#include "WmTimer.h"
#include "ReaderGlobal.h"
#include "resource.h"
#include "SkinButton.h"
#include "vxConsole.h"
#include "ViatronDialog.h"

namespace ColonViews
{
  namespace Dialogs
  {

    /// patient/study data
    class PatientInfoDlg : public ViatronDialog
    {
    public:
      /// private window message
      enum { WM_USER_INITIALIZE_STUDY = WM_USER + 100};

    public:
      /// Standard constructor
      PatientInfoDlg( CWnd* pParent = NULL, const uint4 uDlgBkgdResourceID = IDB_PATIENTINFO_HORIZ_BKGD, const bool bDualDataDisp = false );

      /// Destructor
      virtual ~PatientInfoDlg();

      /// Gets patient study information
      static ReaderLib::StudyInfo GetStudyInfo(const ReaderLib::Dataset & rDataset);

    // Dialog Data
      //{{AFX_DATA(PatientInfoDlg)
      enum { IDD = IDD_PATIENT_INFO_HORIZONTAL };
      /// patient name
      CString m_sPatientName;
      /// referring physician
      CString m_sPhysicianName;
      /// age/gender
      CString m_sPatientData;
      /// patient ID
      CString m_sPatientID;
      /// exam date
      CString m_sExamTime;
      /// exam location
      CString m_sExamSite;
      /// patient ID control
      CStatic m_patientIDStatic;
      /// button to change which dataset's info is displayed
      WinUtils::SkinAutoCheckButton m_btnToggleDisplay;
      //}}AFX_DATA

    // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(PatientInfoDlg)
      protected:
      /// DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      //}}AFX_VIRTUAL

    // Implementation
    protected:

      // Generated message map functions
      //{{AFX_MSG(PatientInfoDlg)
      /// initializations
      virtual BOOL OnInitDialog();
      /// control colouring
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
      /// set cursor
      afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
      /// timer expired msg
      afx_msg void OnTimer(UINT_PTR uIDEvent);
      /// toggle datasets
      afx_msg void OnClickedSupineProneBtn();
      /// study init stage
      afx_msg LRESULT OnMessageInitStudy(WPARAM, LPARAM);
      /// when the supine/prone info display button is clicked
      afx_msg void OnToggleSupineProneInfoDisplayButton();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
      
      /// WM_PAINT handler
      void OnPaint();

      /// WM_ERASEBKGND handler
      BOOL OnEraseBkgnd( CDC * pdc );

      /// WM_SIZE handler
      void OnSize( UINT uType, int4 iCX, int4 iCY );

    private:

      /// redraws the background image
      void RedrawBackground();

      /// draws a groupbox into the draw buffer
      void DrawGroupbox( Gdiplus::Graphics & gfx, const uint4 uId );

      /// draws static text into the draw buffer
      void DrawStatic( Gdiplus::Graphics & gfx, const uint4 uId, const Gdiplus::StringAlignment eAlignment );

      // updates the supine prone button
      void UpdateSupineProneButton();

    private:

      /// draw buffer
      Gdiplus::Bitmap * m_pDrawBuffer;

      /// Timer for keeping supine/prone combo updated
      WinUtils::WmTimer m_updateSPTimer;

      /// indicates if the info dialog is in a pane
      bool m_bContainedInPane;

      /// indicates that the current layout will display both datasets, so just show basic info of each dataset
      bool m_bDualDatasetDisplay;

    }; // PatientInfoDlg

  } // namespace Dialogs

} // namespace ColonViews


/**
 * get value of header entry
 * @param sHeaderVarIdentifier   id's header entry
 * @return                       entry's value
 **/
inline std::string GetVolumeHeaderVariable(const std::string & sHeaderVarIdentifer)
{
  return VxConsole::GetVolumeHeaderVariable(ReaderLib::rdrGlobal.m_pCurrDataset->m_volumeHeader, sHeaderVarIdentifer);
} // GetVolumeHeaderVariable


// Revision History:
// $Log: PatientInfoDlg.h,v $
// Revision 1.10.2.9  2010/04/11 11:56:19  dongqing
// centering child controls
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.8  2010/04/01 06:45:55  dongqing
// putting back the F4l/toggle datasets functionality I was previously told to remove.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.7  2010/03/03 08:58:55  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.6  2010/02/01 19:24:11  dongqing
// study info into the dataset class
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.5  2010/01/17 18:29:17  dongqing
// update
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.4  2010/01/16 04:03:54  dongqing
// only show data for loading/loaded datasets
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.3  2010/01/14 14:43:48  dongqing
// displaying info of both datasets on dual-dataset layouts
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.2  2010/01/07 18:44:37  dongqing
// identify studies associated with both datasets (for cases where datasets from different studies are loaded)
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.10.2.1  2009/11/25 22:11:02  dongqing
// moved dataset switch button to controls
//
// Revision 1.10  2007/03/10 06:09:14  jmeade
// code standards.
//
// Revision 1.9  2007/03/08 22:22:20  romy
// code review - header files adjusted
//
// Revision 1.8  2006/07/24 19:46:57  jmeade
// Issue 4657: Update patient info upon volume header info read.
//
// Revision 1.7  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.6  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.5  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.4.2.2  2006/02/28 17:29:39  geconomos
// modified for SkinButton changes
//
// Revision 1.4.2.1  2005/11/09 02:15:13  jmeade
// comments.
//
// Revision 1.4  2005/10/06 20:35:59  geconomos
// reimplemented drawing
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.11  2003/01/29 00:55:09  jmeade
// Removed link button (to be place elsewhere).
//
// Revision 3.10  2002/12/12 22:06:50  jmeade
// Unused or unneeded view modes code cleanup.
//
// Revision 3.9  2002/08/29 22:11:16  jmeade
// Changed supine-prone toggle combo to an auto check button; Dataset link button.
//
// Revision 3.8  2002/08/22 13:20:01  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.6  2002/07/18 20:25:38  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.5  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.4  2002/04/12 02:13:35  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.3.2.1  2002/04/05 18:19:06  jmeade
// Issue 2035: Split ID out of patient data to its own field
//
// Revision 3.3  2001/11/20 09:50:34  jmeade
// Allow caller to set background image resource.
//
// Revision 3.2  2001/11/18 01:12:31  jmeade
// Custom specification of patient info resource template.
//
// Revision 3.1  2001/11/17 03:34:46  jmeade
// Derive from ViatronDialog to handle looks (threw out the old carpet).
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Sep 27 2001 21:22:48   jmeade
// No more encrypted patientName/ID
// 
//    Rev 2.3   Sep 27 2001 21:01:32   jmeade
// No more encrypted patientName/ID
// 
//    Rev 2.2   Sep 19 2001 15:14:02   binli
// no change
// 
//    Rev 2.1   Sep 19 2001 15:07:14   binli
// (no track ID): bad patient name('0' ) and ID ('0')
// 
//    Rev 2.0   Sep 16 2001 23:41:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:26   ingmar
// Initial revision.
// Revision 1.13  2001/06/16 00:22:44  jmeade
// using WmTimer class
//
// Revision 1.12  2001/06/14 00:48:32  jmeade
// coding conventions; encrypted patient name
//
// Revision 1.11  2001/05/15 18:55:44  jmeade
// Code conventions
//
// Revision 1.10  2001/04/13 02:34:11  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/PatientInfoDlg.h,v 1.10.2.9 2010/04/11 11:56:19 dongqing Exp $
// $Id: PatientInfoDlg.h,v 1.10.2.9 2010/04/11 11:56:19 dongqing Exp $
