// $Id: ReportPreview.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: HTML view to view generated PDF file(requires AcrobatReader)
// Author: George Economos


#if !defined(AFX_REPORTPREVIEW_H__768ACA97_48D3_487C_90CD_C7F83FAB2242__INCLUDED_)
#define AFX_REPORTPREVIEW_H__768ACA97_48D3_487C_90CD_C7F83FAB2242__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

namespace ColonReport
{
  /* preview pdf report */ 
  class ReportPreview : public CHtmlView
  {
  protected:
    /// constructor
    ReportPreview();
    /// destructor
    ~ReportPreview(){}

    /// pre-validate creation
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    /// first update routine
    virtual void OnInitialUpdate();

    /// destroy handler
    afx_msg void OnDestroy();

    /// message map
    DECLARE_MESSAGE_MAP();
    DECLARE_DYNCREATE(ReportPreview)

  protected:
    /// name of temp report preview file
    CString m_sTempFileName;
  }; // class ReportPreview

} // namespace ColonReport

#endif


// Revision History:
// $Log: ReportPreview.h,v $
// Revision 1.3  2007/03/09 23:38:54  jmeade
// code standards.
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
//    Rev 2.0   Sep 16 2001 23:41:48   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:40   ingmar
// Initial revision.
// Revision 1.5  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.4  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportPreview.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
// $Id: ReportPreview.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
