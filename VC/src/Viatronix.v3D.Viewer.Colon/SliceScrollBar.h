// $Id: SliceScrollBar.h,v 1.3.2.1 2009/11/27 19:59:30 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: SliceScrollBar.h: interface for the SliceScrollBar class
// Description: 
// Created: 
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_SLICESCROLLBAR_H__90F95E31_4B89_4650_A4F2_D30064EA316A__INCLUDED_)
#define AFX_SLICESCROLLBAR_H__90F95E31_4B89_4650_A4F2_D30064EA316A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"

namespace WinUtils
{
  /// scrollbar for orthogonal slices
  class SliceScrollBar : public CScrollBar  
  {
  // Methods
  public:
    /// Constructor
    SliceScrollBar();
    /// Constructor
    SliceScrollBar(const uint4 uBitmapResID) : m_uParentViewID(0) { ; }
    /// Destructor
    virtual ~SliceScrollBar();
    /// Handles window movement; resizes internal parent and window
    void MoveWindow( int x, int y, int iWidth, int iHeight, BOOL bRepaint = TRUE );
    /// Handles window movement; resizes internal parent and window
    void MoveWindow( LPCRECT lpRect, BOOL bRepaint = TRUE );
    /// Sets the ID of the view window that will receive scroll messages from this scrollbar
    void SetParentView(const uint4 uParentID) { m_uParentViewID = uParentID; }

  // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(SliceScrollBar)
    //}}AFX_VIRTUAL

    // Generated message map functions
  protected:
    //{{AFX_MSG(SliceScrollBar)
    afx_msg void HScroll(UINT uSBCode, UINT uPos);
    afx_msg void VScroll(UINT uSBCode, UINT uPos);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
    afx_msg void OnShowWindow(BOOL bShow, UINT uStatus);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

  public:
    /// ID of message to post to parent window when user changes scroll position
    static uint4 m_uScrollMessageID;

  private:
    /// ID of Parent view
    uint4 m_uParentViewID;
    /// The actual parent of the control
    CButton m_myRealParent;
  };

} // namespace WinUtils

#endif // !defined(AFX_SLICESCROLLBAR_H__90F95E31_4B89_4650_A4F2_D30064EA316A__INCLUDED_)

// Revision History:
// $Log: SliceScrollBar.h,v $
// Revision 1.3.2.1  2009/11/27 19:59:30  dongqing
// scroll bars
//
// Revision 1.3  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.5  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.4.2.1  2002/05/07 22:39:28  jmeade
// Issue 2046:  Reverting to hidden-CScrollBar derived method.  (Generic looks, proper operation.)
//
// Revision 3.4  2001/12/06 20:33:06  binli
// adjustment:
// 1> m_iScrollMax = sliceNum: avoid flahing problem.
// 2> one press (down) on left/right arrow: scroll pos adjusted once at least.
// 3> pagesize ~ slicenum.
//
// Revision 3.3  2001/11/23 15:06:56  jmeade
// Use a separate parent view identifier to ssend scroll messages.
//
// Revision 3.2  2001/11/20 02:59:12  jmeade
// More picturesque bitmapped scrollbars.
//
// Revision 3.1  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/06 17:12:50  jmeade
// Removed message dependency on Viatron project
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:00   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:46   ingmar
// Initial revision.
// Revision 1.4  2001/05/11 22:46:59  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SliceScrollBar.h,v 1.3.2.1 2009/11/27 19:59:30 dongqing Exp $
// $Id: SliceScrollBar.h,v 1.3.2.1 2009/11/27 19:59:30 dongqing Exp $
