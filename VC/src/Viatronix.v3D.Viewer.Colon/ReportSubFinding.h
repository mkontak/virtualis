// $Id: ReportSubFinding.h,v 1.4.2.3 2010/02/05 15:19:11 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for other findings
// Author: George Economos

#if !defined(AFX_REPORTSUBFINDING_H__6178904D_347D_4068_82AE_2CE4671C5E93__INCLUDED_)
#define AFX_REPORTSUBFINDING_H__6178904D_347D_4068_82AE_2CE4671C5E93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 


#include "ReportGroupCtrl.h"
#include "WinUtils.h"
#include "ReportWndBase.h"

namespace ColonReport
{
  /**
   * extra-colonic findings 
   */
  class ReportSubFinding : public ReportWndBase
  {
  // construction
  public:
    /// constructor
    ReportSubFinding(CWnd* pParent = NULL);

    //{{AFX_DATA(ReportSubFinding)
    //}}AFX_DATA

  // member functions
  public:

    /// sets the finding
    void SetFinding(ReportLib::IReportOtherFindingPtr& spFinding);

    /// show dialog
    void SlideOut(const CPoint& ptFinal, CWnd* pWnd);

    /// hide dialog
    void SlideIn(const CPoint& ptFinal, CWnd* pWnd);

    /// when view inactivated
    virtual void OnReportViewUnactivate();

    /// gets the current finding being displayed
    inline const ReportLib::IReportOtherFindingPtr GetCurrentFinding() const { return m_spFinding; }

    //{{AFX_VIRTUAL(ReportSubFinding)
  protected:
  
    /// dialog data initialize
    virtual void DoDataExchange(CDataExchange* pDX);

    /// dialog initialized
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

  protected:

    /// finding location control
    ReportGroupCtrl m_wndGroupLocation;
    
    /// finding size control
    ReportGroupCtrl m_wndGroupSize;
    
    /// finding level control
    ReportGroupCtrl m_wndGroupLevel;
    
    /// CRADS category
    CComboBox m_wndComboCRADS;
    
    /// maximum diameter
    ReportSpinEdit m_wndMaximumSize;
    
    /// diameter perpendicular to max
    ReportSpinEdit m_wndPerpendicularSize;
    
    /// length
    ReportSpinEdit m_wndLength;
    
    /// contains the subfindings information
    ReportLib::IReportOtherFindingPtr m_spFinding;

    //{{AFX_MSG(ReportSubFinding)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    /// initialize fn
    void OnInitLung1();
    
    /// initialize fn
    void OnInitLung2();
    
    /// initialize fn
    void OnInitLiver1();
    
    /// initialize fn
    void OnInitAdrenal1();
    
    /// initialize fn
    void OnInitRenal1();
    
    /// initialize fn
    void OnInitRenal2();
    
    /// initialize fn
    void OnInitRenal3();
    
    /// initialize fn
    void OnInitVascular1();
    
    /// initialize fn
    void OnInitVascular2();
    
    /// initialize fn
    void OnInitSpleen1();
    
    /// initialize fn
    void OnInitGI1();

    friend class ReportColonFindings;

  }; // class ReportSubFinding

} // using namespace ColonReport

#endif


// Revision History:
// $Log: ReportSubFinding.h,v $
// Revision 1.4.2.3  2010/02/05 15:19:11  dongqing
// update the hidden extra-colonic text when an extra-colonic entry changes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.2  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.4.2.1  2008/02/14 06:55:19  jmeade
// Issue 5924: allow inquiry to current displayed finding.
//
// Revision 1.4  2007/03/09 23:38:54  jmeade
// code standards.
//
// Revision 1.3  2007/02/09 00:11:49  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.1  2002/04/18 18:32:21  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// Revision 1.9  2001/05/10 21:04:40  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.8  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.7  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// Revision History:
// $Log: ReportSubFinding.h,v $
// Revision 1.4.2.3  2010/02/05 15:19:11  dongqing
// update the hidden extra-colonic text when an extra-colonic entry changes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4.2.2  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.4.2.1  2008/02/14 06:55:19  jmeade
// Issue 5924: allow inquiry to current displayed finding.
//
// Revision 1.4  2007/03/09 23:38:54  jmeade
// code standards.
//
// Revision 1.3  2007/02/09 00:11:49  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.1  2002/04/18 18:32:21  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:42   ingmar
// Initial revision.
// Revision 1.9  2001/05/10 21:04:40  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.8  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.7  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportSubFinding.h,v 1.4.2.3 2010/02/05 15:19:11 dongqing Exp $
// $Id: ReportSubFinding.h,v 1.4.2.3 2010/02/05 15:19:11 dongqing Exp $
