// $Id: GradientCtrlImpl.h,v 1.3.2.1 2007/11/02 03:07:16 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: GradientCtrlImpl.h
// Description: 
// Owner: Jeff Meade [jmeade@viatronix.com]

#if !defined(AFX_GRADIENTCTRLIMPL_H__C28028E9_A49A_498F_8BC4_77DE5D266D04__INCLUDED_)
#define AFX_GRADIENTCTRLIMPL_H__C28028E9_A49A_498F_8BC4_77DE5D266D04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Gradient.h"

namespace WinUtils
{
  /// forward declaration
  class GradientCtrl;

  /// gradient colour display
  class GradientCtrlImpl  
  {
// Methods

  public:
    /// constructor
    GradientCtrlImpl(GradientCtrl &rOwner);

  protected:
    /// destructor
    virtual ~GradientCtrlImpl();

    /// draw control
    void Draw(CDC *pDC, const bool bEnabled = true);
    /// draw gradient field
    void DrawGradient(CDC *pDC);
    /// draw pegs
    void DrawPegs(CDC *pDC);
    /// draw a currently-selected peg
    void DrawSelPeg(CDC *pDC, const int4 iPeg);
    /// draw a currently-selected peg
    void DrawSelPeg(CDC *pDC, const CPoint& point, const int4 iDirection);
    /// draw a normal peg
    void DrawPeg(CDC *pDC, const CPoint& point, const COLORREF colour, const int4 iDirection);
    /// draw an end peg
    void DrawEndPegs(CDC *pDC);

    /// gets ???
    int4 PointFromPos(const float4 fPos) const;
    /// get ???
    float4 PosFromPoint(const int4 iPoint) const;
    /// get amount of peg indent
    int4 GetPegIndent(const int4 iIndex) const;
    /// get ???
    int4 PtInPeg(const CPoint& point) const;
    /// get a peg's bounding rectangle
    void GetPegRect(const int4 iIndex, CRect *pRect, const bool bRight) const;

    /// ???
    void ParseToolTipLine(CString& sTiptext, const Peg& peg) const;
    /// shows a tooltip
    void ShowTooltip(const CPoint& point, const CString& sText);
    /// ???
    CString ExtractLine(const CString& sSource, int4 iLine) const;
    /// set text of tooltip
    void SetTooltipText(const CString& sText);
    /// remove tooltip
    void DestroyTooltip();
    /// ???
    void SynchronizeTooltips();
    /// vertical or horizontal control
    bool IsVertical() const;
    /// width of gradient control
    int4 GetDrawWidth() const;

// Variables
  protected:
    /// tooltip window
    HWND m_hToolTip;
    /// tooltip information
    TOOLINFO m_toolInfo;
    /// gradient control
    GradientCtrl &m_rGradOwner;
    /// tooltip control
    CToolTipCtrl m_ctrlToolTip;
    /// ???
    int4 m_iRectCount;
    /// bounds
    bool m_bLeftDownSide, m_bRightUpSide;

    /// one base peg for error return
    Peg m_pegNull;

    /// friend access control
    friend class GradientCtrl;
  }; // class GradientCtrlImpl

} // namespace WinUtils

#endif // !defined(AFX_GRADIENTCTRLIMPL_H__C28028E9_A49A_498F_8BC4_77DE5D266D04__INCLUDED_)

// $Log: GradientCtrlImpl.h,v $
// Revision 1.3.2.1  2007/11/02 03:07:16  jmeade
// Issue 5583: allow for disabled/greyed gradient control.
//
// Revision 1.3  2007/03/13 18:36:38  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2004/05/05 23:23:28  jmeade
// Incremental update.
//
// Revision 1.1.2.1  2004/04/21 03:44:20  jmeade
// Gradient color edit control.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GradientCtrlImpl.h,v 1.3.2.1 2007/11/02 03:07:16 jmeade Exp $
// $Id: GradientCtrlImpl.h,v 1.3.2.1 2007/11/02 03:07:16 jmeade Exp $
