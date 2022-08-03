// $Id: ReportControls.h,v 1.3 2007/03/10 05:06:25 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Owner draw list control
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#ifndef ReportControls_h__
#define ReportControls_h__

#include "typedef.h"

namespace ColonReport
{
  /// list control for items
  class ReportListCtrl : public CListCtrl
  {
  public:
    /// constructor
    ReportListCtrl();
    
    //{{AFX_VIRTUAL(ReportEditCtrl)
    /// draw item
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    /// pre-processing
    void PreSubclassWindow();
    //}}AFX_VIRTUAL
    
  protected:

    /// common initialization routine
    BOOL CommonInit();

    /// offset of images in list control
    CSize m_szOffset;

    /// current tracking item
    int4 m_iTrackItem;

    /// flag indicating if a button is in the down state
    BOOL m_bItemDown;

    /// button down handler
    void ButtonDown(const CPoint& point);

    /// button up handler
    void ButtonUp(const CPoint& point);

    //{{AFX_MSG(ReportListCtrl)
    afx_msg void OnLButtonDown(UINT uFlags, CPoint point);
    afx_msg void OnMouseMove(UINT uFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT uFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT uFlags, CPoint point);
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR uIDEvent);
	  afx_msg void OnRButtonUp(UINT uFlags, CPoint point);
	  //}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()
  }; // class ReportListCtrl

} // namespace ColonReport

#endif

// Revision History:
// $Log: ReportControls.h,v $
// Revision 1.3  2007/03/10 05:06:25  jmeade
// code standards.
//
// Revision 1.2  2005/09/13 17:21:07  jmeade
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
//    Rev 2.0   Sep 16 2001 23:41:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:32   ingmar
// Initial revision.
// Revision 1.5  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.4  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.3  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportControls.h,v 1.3 2007/03/10 05:06:25 jmeade Exp $
// $Id: ReportControls.h,v 1.3 2007/03/10 05:06:25 jmeade Exp $
