// $Id: ProgressCtrl.h,v 1.1.2.5 2010/11/01 18:51:33 dongqing Exp $
//
// Copyright © 2000-2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak [mkontak@viatronix.com]


#pragma once

namespace WinUtils
{

  class ProgressCtrl : public CProgressCtrl
  {
  public:
    /// default constructor
    ProgressCtrl();

    /// override:
    virtual int SetPos(int nPos);
    
	/// override: The flag has to be updated
    virtual void Invalidate(BOOL bErase);

	/// override and force to return FALSE always
	afx_msg BOOL OnEraseBkgnd( CDC * pDC );

	/// override paint to avoid Win 7 Aero conflict
    afx_msg void OnPaint();

    DECLARE_MESSAGE_MAP()

  private:

    /// flag for indicating need of background redraw
    bool m_bPaintBackground;

  }; // EndoCoverageProgressCtrl

} // WinUtils


// $Log: ProgressCtrl.h,v $
// Revision 1.1.2.5  2010/11/01 18:51:33  dongqing
// Fix the resize artifacts on background
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.4  2010/11/01 12:40:55  dongqing
// add comments on Mark's fix for the Win 7 progress bar of 3D fly thru
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.0  2010/10/29 13:07:27  mkontak
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ProgressCtrl.h,v 1.1.2.5 2010/11/01 18:51:33 dongqing Exp $
// $Id: ProgressCtrl.h,v 1.1.2.5 2010/11/01 18:51:33 dongqing Exp $
