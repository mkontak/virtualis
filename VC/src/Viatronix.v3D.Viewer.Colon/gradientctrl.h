// $Id: gradientctrl.h,v 1.4 2007/03/14 23:32:33 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: GradientCtrl.h
// Description: 
// Owner: Jeff Meade [jmeade@viatronix.com]

#if !defined(AFX_GRADIENTCTRL_H__BA457C5B_AA8B_4E1F_8399_57B9E3C1A406__INCLUDED_)
#define AFX_GRADIENTCTRL_H__BA457C5B_AA8B_4E1F_8399_57B9E3C1A406__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GradientCtrl.h : header file
//

#include "Gradient.h"

#define GCW_AUTO -1

namespace WinUtils
{
  /// GradientCtrl messages
  enum
  {
    GC_SELCHANGE       = 1,
    GC_PEGMOVE        = 2,
    GC_PEGMOVED       = 3,
    GC_PEGREMOVED     = 4,
    GC_CREATEPEG      = 5,
    GC_EDITPEG        = 6,
    GC_CHANGE         = 7
  }; // enum
  /// peg notification message info structure
  struct PegNMHDR
  {
    NMHDR nmhdr;
    Peg peg;
    int4 iIndex;
  }; // PegNHMDR
  /// peg creation notify message structure
  struct PegCreateNMHDR
  {
    NMHDR nmhdr;
    float4 fPosition;
    COLORREF colour;
  }; // PegCreateNMHDR

  /// forward declarations
  class GradientCtrlImpl;

  /////////////////////////////////////////////////////////////////////////////
  // GradientCtrl window
  class GradientCtrl : public CWnd
  {
  public:
    /// Constructor
    GradientCtrl();
    /// create method
    BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT uID);
    /// horizontal or vertical orientation
    enum OrientationEnum
    {
      FORCEHORIZONTAL,
      FORCEVERTICAL,
      AUTO
    }; // OrientationEnum

    // Attributes
  public:
    /// get width of gradient window
    inline int4 GetGradientWidth() const { return m_iWidth; };
    /// set width of gradient window
    inline void SetGradientWidth(const int iWidth) { ASSERT(iWidth < -1); m_iWidth = iWidth; };

    /// get selected peg index
    int4 GetSelIndex() const { return m_iSelected; };
    /// set selected peg index
    int4 SetSelIndex(const int4 iSel);

    /// get selected peg
    const Peg GetSelPeg() const;

    /// get gradient
    GradientList& GetGradient() {return m_gradient;};
    void SetGradient(GradientList src) {m_gradient = src;};

    OrientationEnum GetOrientation() const {return m_eOrientation;};
    void SetOrientation(OrientationEnum eOrientation) {m_eOrientation = eOrientation;};

    float GetPosAtPoint(const CPoint& pt);

    bool GetPegSide(bool bRightOrUp) const;
    void SetPegSide(bool bSetRightOrUp, bool bEnable);

    void ShowTooltips(bool bShow = true);
    void SetTooltipFormat(const CString format);
    CString GetTooltipFormat() const;

    void EnableKeyboardEdit(const bool bEnable = true);
    bool IsEnabledKeyboardEdit() const;

    // Operations
  public:
    void DeleteSelected(bool bUpdate);
    int MoveSelected(float fNewpos, bool bUpdate);
    COLORREF SetColourSelected(COLORREF crNewColour, bool bUpdate);

    // Internals
  protected:
    BOOL RegisterWindowClass();
    void GetPegRgn(CRgn *rgn);
    void SendBasicNotification(UINT code, Peg peg, int iIndex);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGradientCtrl)
    public:
      virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
      virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

    // Implementation
  public:
    /// destructor
    virtual ~GradientCtrl();

    // Generated message map functions
  protected:

    //{{AFX_MSG(CGradientCtrl)
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  private:
    /// get the peg at the specified point
    int4 GetPegAtPoint(const CPoint& point);

  private:
    GradientList m_gradient;
    /// width of control
    int4 m_iWidth;

    /// selected peg
    int4 m_iSelected;
    /// last selected position
    int4 m_iLastPos;
    /// mouse click location
    CPoint m_ptMouseDown;

    /// whether to show tooltip control
    bool m_bShowToolTip;
    /// format of tooltip
    CString m_sToolTipFormat;

    /// horizontal or vertical
    enum OrientationEnum m_eOrientation;

    /// gradient drawing
    GradientCtrlImpl *m_pGradImpl;

    /// editing pegs via keyboard
    bool m_bKeyboardEditEnabled;

    /// friend access declarations
    friend class GradientCtrlImpl;
  }; // class GradientCtrl

} // namespace WinUtils

/////////////////////////////////////////////////////////////////////////////

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRADIENTCTRL_H__BA457C5B_AA8B_4E1F_8399_57B9E3C1A406__INCLUDED_)

// $Log: gradientctrl.h,v $
// Revision 1.4  2007/03/14 23:32:33  jmeade
// code standards.
//
// Revision 1.3  2007/03/13 18:36:38  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.3  2004/07/02 00:45:35  jmeade
// Allow parent to handle mouse move msgs; set cursor to indicate hover over pegs.
//
// Revision 1.1.2.2  2004/05/05 23:23:28  jmeade
// Incremental update.
//
// Revision 1.1.2.1  2004/04/21 03:44:20  jmeade
// Gradient color edit control.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/gradientctrl.h,v 1.4 2007/03/14 23:32:33 jmeade Exp $
// $Id: gradientctrl.h,v 1.4 2007/03/14 23:32:33 jmeade Exp $
