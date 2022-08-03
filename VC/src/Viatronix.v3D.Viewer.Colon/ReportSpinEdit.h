// $Id: ReportSpinEdit.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Owner drawn spin edit control that allows only numeric input
// Author: George Economos

#if !defined(AFX_REPORTSPINEDIT_H__0F66878D_344E_47F6_882E_6A154ECE41A3__INCLUDED_)
#define AFX_REPORTSPINEDIT_H__0F66878D_344E_47F6_882E_6A154ECE41A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

namespace ColonReport
{
  /** edit control in report **/
  class ReportEdit : public CEdit  
  {
    /// dynamic instantiation
    DECLARE_DYNAMIC(ReportEdit)
  public:
    /// constructor
    ReportEdit();
    /// set colour of field
    void SetColor(const COLORREF clr) { m_clr = clr; }

  protected:
    //{{AFX_MSG(ReportEdit)
	  //}}AFX_MSG

  protected:
    /// field's colour
    COLORREF m_clr;

    /// message map
    DECLARE_MESSAGE_MAP()    
  }; // class ReportEdit

  /// number edit field
  class ReportNumEdit : public ReportEdit  
  {
    DECLARE_DYNAMIC(ReportNumEdit)
  public:
    /// constructor
    ReportNumEdit ();
    /// set number of digits allowed
    void SetDigits (const int4 iDigits);    
    /// get number of digits allowed
    int4 GetDigits() const { return m_iDigits; };

    /// get value
    float4 GetValue() const;
    /// set value
    void SetValue(float4 fVal);

    /// set value min and max
    void SetRange(const float4 fMin, const float4 fMax);
    /// get range max
    float4 GetMax() const { return m_fMax; };
    /// get range min
    float4 GetMin() const { return m_fMin; };

    /// whether text will wrap
    void SetWrap(const bool bWrap); 
    /// whether text will wrap
    bool GetWrap() const { return m_bWrap; }

  protected:
    /// when key is pressed
    virtual void OnChar (UINT uChar, UINT uRepCnt, UINT uFlags);
    /// validate input
    virtual bool CheckInput (UINT uChar);

    /// number of allowed digits
    int4 m_iDigits;
    /// range max
    float4 m_fMax;
    /// range min
    float4 m_fMin;
    /// word wrap allowed?
    bool m_bWrap;
    /// initial value
    CString m_sInitial;
    
    //{{AFX_MSG(ReportNumEdit)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()    
  }; // class ReportNumEdit

  /// up-down spin control
  class CSpin : public CSpinButtonCtrl  
  {
    /// dynamic instantiation
    DECLARE_DYNAMIC(CSpin)

  public:
    /// constructor
    CSpin();
    /// change value
    void Change (const int4 iSteps);
    /// set the increment for the change buttons
    void SetDelta (const float4 fDelta);
    /// when value changed
    void OnDeltaPos (NMHDR* pNMHDR, LRESULT* pResult);
    
  protected:
    //{{AFX_MSG(CSpin)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  private:
    /// increment when buttons are pressed
    float4 m_fDelta;
  }; // class CSpin


  /** grouped spin and edit **/
  class ReportSpinEdit: public CWnd  
  {
    /// dynamic instantiation
    DECLARE_DYNAMIC(ReportSpinEdit)
      
  public:
    /// constructor
    ReportSpinEdit ();
    /// destructor
    virtual ~ReportSpinEdit ();

    /// get the spin control
    CSpin* GetSpinner () const;
    /// get the edit control
    ReportNumEdit* GetEdit () const;

    /// message handling
    virtual BOOL PreTranslateMessage (MSG* pMsg);

    /// set number of digits allowed
    void SetDigits (const int4 iDigCount);

    /// set value min and max
    void SetRange (const float4 iMin, const float4 iMax, const bool bWrap);

    /// set the increment for the change buttons
    void SetStepSize (const float4 dx);

    /// set font
    void SetFont (CFont* pFont, BOOL bRedraw = TRUE);

    /// text colour
    void SetTextColor (const int4 r, const int4 b, const int4 g);

    /// get value
    float4 GetValue() const;

    /// set value
    void SetValue (const float4 fVal);

    /// create validation
    virtual int PreCreateWindow(CREATESTRUCT& cs);
    /// colour set
    void SetColor(const COLORREF clr);

  protected:
    /// create routine
    int OnCreate (LPCREATESTRUCT lpCreateStruct);
    /// resize handler
    void OnSize (UINT uType, int iCX, int iCY);
    /// key press
    void OnKeyDown (UINT uChar, UINT uRepCnt, UINT uFlags);
    /// window receiving focus
    void OnSetFocus (CWnd* pOldWnd);
    /// control colours
    HBRUSH OnCtlColor (CDC* pDC, CWnd* pWnd, UINT uCtlColor);

  protected:
    /// background brush
    CBrush m_bkBrush;
    /// control colour
    COLORREF m_clrBrush;

    /// spin control
    CSpin* m_pSpin;
    /// edit control
    ReportNumEdit* m_pEdit;
    //{{AFX_MSG(ReportSpinEdit)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  private:
    /// text colour
    COLORREF m_clrText;
  }; // class ReportSpinEdit

} // namespace ColonReport

#endif


// Revision History:
// $Log: ReportSpinEdit.h,v $
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
// Revision 1.6  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.5  2001/03/30 18:13:20  geconomos
// Partially fixed bug with reportspinedit control
//
// Revision 1.4  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// Revision History:
// $Log: ReportSpinEdit.h,v $
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
// Revision 1.6  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.5  2001/03/30 18:13:20  geconomos
// Partially fixed bug with reportspinedit control
//
// Revision 1.4  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportSpinEdit.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
// $Id: ReportSpinEdit.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
