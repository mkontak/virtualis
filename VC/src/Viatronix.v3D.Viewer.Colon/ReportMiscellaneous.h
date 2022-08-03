// $Id: ReportMiscellaneous.h,v 1.4.2.8 2010/03/30 07:50:28 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for miscellaneous section of report
// Author: George Economos

#if !defined(AFX_REPORTMISCELLANEOUS_H__BC1CBE04_0580_4CDB_B6FD_CF1D01F5E088__INCLUDED_)
#define AFX_REPORTMISCELLANEOUS_H__BC1CBE04_0580_4CDB_B6FD_CF1D01F5E088__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "ReportWndBase.h"
#include "ReportSpinEdit.h"
#include "ReportGroupCtrl.h"
// TODO: PUT the following in a new file ==> #include "HyperTextStatic.h"

namespace WinUtils
{
  // A clickable hypertext link
  class HyperTextStatic : public CStatic
  {
  public:
    // windows messages
    /**
     * Window message handler
     *
     * @param uMessage   Message
     * @param wParam     Message-dependent parameter
     * @param lParam     Message-dependent parameter
     * @return           Message-dependent.
     */
    virtual LRESULT WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam )
    {
      switch ( uMessage )
      {
        case WM_INITIALUPDATE:
          VERIFY(m_fontCold.CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, TRUE, 0, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial"));
          VERIFY(m_fontHot.CreateFont(-14, 0, 0, 0, FW_BOLD, TRUE, TRUE, 0, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial"));

          m_bFontHot = false;
          SetFont(&m_fontCold);
          break;

        case WM_LBUTTONDOWN:
        {  
          CWaitCursor crsr;
          SHELLEXECUTEINFO sei;
          memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

          // Open the picture file
          sei.cbSize = sizeof(SHELLEXECUTEINFO);
          sei.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_DDEWAIT;
          sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
          sei.lpFile = m_sTarget.c_str();
          sei.lpVerb = "open";
          sei.lpDirectory = ".";
          sei.nShow = SW_SHOW;

          ShellExecuteEx(&sei);
          break;
        }
        case WM_MOUSEMOVE:
        {    
          TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, GetSafeHwnd(), 0 };
          _TrackMouseEvent( &tme );
          if ( !m_bFontHot )
          {
            SetFont(&m_fontHot);
            m_bFontHot = true;
          }
          break;
        }
        case WM_MOUSELEAVE:
          SetFont(&m_fontCold);
          m_bFontHot = false;
          break;
      }

      return CWnd::WindowProc( uMessage, wParam, lParam );
    } // WindowProc()
  public:
    // hypertext link
    std::string m_sTarget;
  private:
    // indicates which font is currently being used
    bool m_bFontHot;
    // regular and hover fonts
    CFont m_fontCold, m_fontHot;
  }; // HyperTextStatic
} // namespace WinUtils


namespace ColonReport
{
  // General study information
  class ReportMiscellaneous : public ReportWndBase
  {
  public:
    /// standard constructor
    ReportMiscellaneous(CWnd* pParent = NULL);

    /// set report revision
    virtual void SetReport(ReportLib::IReportRevisionPtr& spRevision);
    /// when a view has been activated
    virtual void OnReportViewActivate();
    /// when a view has been inactivated
    virtual void OnReportViewUnactivate();

    /// update the E# text corresponding to the most relevant extra-colonic finding's C-RADS classification
    static void UpdateKludgedExtraColonicCRadsText(ReportLib::IReportRevisionPtr& spRevision);

    /// returns the highest c-rads category among the extra-colonic findings
    static const long GetExtraColonicCRADSCat( ReportLib::IReportRevisionPtr & spRevision, CString & sOutput = CString());

    /// removes legacy crads category from string
    static void RemoveLegacyCRADSCategory( CString & sMessage );

    //{{AFX_VIRTUAL(ReportMiscellaneous)
  protected:
    /// DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(ReportMiscellaneous)
    /// dialog init
    virtual BOOL OnInitDialog();
    afx_msg void OnSelChangeCRADSCategory();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    /// Enter/OnOK
    virtual void OnOK();
    /// Esc/OnCancel
    virtual void OnCancel();

  protected:
    /// gender
    enum
    {
      Male = 0,
      Female = 1,
    };

  public:
    /// dialog template ID
    static const int4 IDD;

  private:
    //{{AFX_DATA(ReportMiscellaneous)
    /// units
    CComboBox m_wndUnit;
    /// type
    CComboBox m_wndType;
    /// recommendations
    ReportGroupCtrl  m_wndRecommendations;
    /// recommendations
    ReportGroupCtrl  m_wndRecommendations2;
    /// indications
    ReportGroupCtrl  m_wndIndications;
    /// exam quality
    ReportGroupCtrl  m_wndExamQuality;
    /// age
    ReportNumEdit  m_wndAge;
    /// doctor's name
    ReportEdit m_wndPhysician;
    /// patient name
    ReportEdit m_wndName;
    /// patient ID
    ReportEdit m_wndID;
    /// impressions
    ReportEdit m_wndImpression;
    /// study date?
    CDateTimeCtrl m_wndDate;
    /// gender
    CComboBox m_wndSex;
    /// CRADS category
    CComboBox m_wndComboCRADS;
    /// age2
    ReportNumEdit  m_wndAge2;
    /// doctor 2's name
    ReportEdit m_wndPhysician2;
    /// patient2 name
    ReportEdit m_wndName2;
    /// patient2 ID
    ReportEdit m_wndID2;
    /// study date2
    CDateTimeCtrl m_wndDate2;
    /// gender2
    CComboBox m_wndSex2;
    //}}AFX_DATA
    /// time till next exam
    ReportNumEdit m_wndYears;
    /// clickable link to CRADS web description
    WinUtils::HyperTextStatic m_staticCRADS;
    /// clickable link to CRADS web description
    WinUtils::HyperTextStatic m_staticXCRADS;

    /// delimiter for c-rads and exam quality kludges
    static const char m_sHiddenTextDelimiter[3];
    /// newlines within the hidden text
    static const char m_sHiddenTextNewline[3];

  }; // ReportMiscellaneous

} // namespace ColonReport

#endif

// Revision History:
// $Log: ReportMiscellaneous.h,v $
// Revision 1.4.2.8  2010/03/30 07:50:28  dongqing
// prevent dialog from disappearing
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.7  2010/02/24 05:59:22  dongqing
// in the report, add the name of a second dataset opened if ID is dissimilar to the first
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.6  2010/02/18 12:41:45  dongqing
// link to c-rads webpage
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.5  2010/02/09 03:43:04  dongqing
// verbose c-rads text to the report miscellaneous dialog
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.4  2010/02/05 15:19:10  dongqing
// update the hidden extra-colonic text when an extra-colonic entry changes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.3  2010/02/05 09:48:38  dongqing
// C-RADS and Exam Quality
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.2  2007/06/08 21:28:01  jmeade
// Issue 5648: change the x-colonic crads misc field to a text box.
//
// Revision 1.4.2.1  2007/04/27 02:19:34  jmeade
// Issue 5608: one overall extra-colonic c-rads category in the miscellaneous view.
//
// Revision 1.4  2007/03/10 03:19:30  jmeade
// code standards.
//
// Revision 1.3  2007/02/09 03:59:58  jmeade
// Issue 5219: Implementing C-RADS categories.
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:44   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:36   ingmar
// Initial revision.
// Revision 1.7  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.6  2001/05/09 14:14:41  geconomos
// Addressed Item#000330 - Overall Impression
//
// Revision 1.5  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.4  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.3  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportMiscellaneous.h,v 1.4.2.8 2010/03/30 07:50:28 dongqing Exp $
// $Id: ReportMiscellaneous.h,v 1.4.2.8 2010/03/30 07:50:28 dongqing Exp $
