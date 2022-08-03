// $Id: SkinButton.h,v 1.7.2.1 2009/12/11 22:32:32 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 

#pragma once


/// includes
#include "WinGDIUtils.h"
#include "typedef.h"

namespace WinUtils
{

  /**
   * Skinned button, a button with four state images in one bmp (up, down, focus, disabled)
   */
  class SkinButton : public CWnd
  {
  
  /// enums
  public:
    /// current button state
    enum SkinButtonStateEnum { UP = 0, DOWN, HOVER, DISABLED };

  /// member functions
  public:
    
    /// constructor
    SkinButton();
    
    /// destructor
    virtual ~SkinButton();

    /// creates the button
    bool Create( LPCTSTR sText, DWORD dwStyle, CRect & rc, CWnd * pParent, DWORD dwId );

    /// gets the button state
    SkinButtonStateEnum GetButtonState() const { return m_eButtonState; }

    /// sets the button bitmaps
    void SetBitmap( DWORD dwResourceId );
    
    /// sets the background brush
    void SetBackBrush( const CBrush * pBackBrush ) { m_pBackbrush = pBackBrush; }

    /// gets the button check state
    int4 GetCheck() { return m_iCheck; }

    /// sets the button check state
    void SetCheck( int4 iCheck );

    /// sets identifier for message that will be posted to display help text
    static void SetHelpTextMessageID( const uint4 uHelpTextMessageID = 0 ) { m_uGlobalHelpTextMessageID = uHelpTextMessageID; }
    
    /// sets the resource ID for the default button bitmap image
    static void SetDefaultButtonResourceID( const uint4 uDefaultBtnResID ) { m_uDefaultButtonResID = uDefaultBtnResID; }

  protected:

    /// MFC message map
    DECLARE_MESSAGE_MAP()

    /// WM_DESTORY handler
    afx_msg void OnDestroy();

    /// WM_ERASEBKGND handler
    afx_msg BOOL OnEraseBkgnd( CDC * pDC );

    /// WM_PAINT handler
    afx_msg void OnPaint();

    /// WM_LBUTTONDOWN handler
    afx_msg void OnLButtonDown( UINT uFlags, CPoint point);
    
    /// WM_LBUTTONUP handler
    afx_msg void OnLButtonUp( UINT uFlags, CPoint point );

    /// WM_MOUSEMOVE handler
    afx_msg void OnMouseMove( UINT uFlags, CPoint point );
    
    /// WM_SIZE
    afx_msg void OnSize( UINT uType, int iCX, int iCY );

    /// WM_ENABLE handler
    afx_msg void OnEnable( BOOL bEnable );

    /// gets the toggle state (up or down)
    afx_msg LRESULT OnGetCheck( WPARAM wParam, LPARAM lParam );

    /// sets toggle state
    afx_msg LRESULT OnSetCheck( WPARAM wParam, LPARAM lParam );

    /// gain focus
    afx_msg void OnSetFocus( CWnd* pOldWnd );

    /// lose focus
    afx_msg void OnKillFocus( CWnd* pNewWnd );
    
    /// window procedure
    virtual LRESULT WindowProc( UINT uMessage, WPARAM wParam, LPARAM lParam );

    /// called when the mouse enters the control
    virtual void OnMouseEnter();

    /// called when the mouse leaves the control
    virtual void OnMouseLeave();

    /// sets the current button state
    void SetButtonState( const SkinButton::SkinButtonStateEnum & eState );

    /// called when the button is clicked
    virtual void OnClicked() {}

    /// gets th if the mouse cursor is over the button
    bool IsMouseOverButton() const;

  private:

    /// button images
    Gdiplus::Bitmap * m_pButtonImages;

    /// button state
    SkinButtonStateEnum m_eButtonState;

    /// true when mouse cursor over button
    bool m_bMouseOver;

    /// indicates if the button is checked
    int4 m_iCheck;

    /// background brush
    const CBrush * m_pBackbrush;

    /// focus
    bool m_bHasFocus;

    /// current bitmap resource id
    DWORD m_dwResourceId;

    /// Used to post messages to applications that wish to use popup or other help text
    static uint4 m_uGlobalHelpTextMessageID;

    /// Resource ID of default button, should be defined by library user
    static uint4 m_uDefaultButtonResID;

  }; // class SkinButton


  /**
   * Skinned autocheckbox class
   * Since a button cannot have both BS_AUTOCHECKBOX and BS_OWNERDRAW styles, a class is needed
   */
  class SkinAutoCheckButton : public SkinButton
  {

  protected:

    /// MFC message map
    DECLARE_MESSAGE_MAP()

    /// WM_LBUTTONDOWN handler
    afx_msg void OnLButtonDown( UINT uFlags, CPoint point);
    
    /// WM_LBUTTONUP handler
    afx_msg void OnLButtonUp( UINT uFlags, CPoint point );

    /// WM_MOUSEMOVE handler
    afx_msg void OnMouseMove( UINT uFlags, CPoint point );

    /// called when the mouse leaves the control
    virtual void OnMouseEnter();

    /// called when the mouse leaves the control
    virtual void OnMouseLeave();

    /// called when the button is checked
    virtual void OnChecked() {}

  }; // class SkinAutoCheckButton 


  /**
   * Skinned auto radio button class
   * Since a button cannot have both BS_AUTORADIOBUTTON and BS_OWNERDRAW styles, a class is needed
   */
  class SkinAutoRadioButton : public SkinAutoCheckButton
  {
  public:

    /// dialog data exchange
    static void AFXAPI DDX_Radio( CDataExchange * pDX, int iIDFirst, int & iValue );
    /// dialog data exchange, using control IDs
    static void AFXAPI DDX_Radio_IDs( CDataExchange * pDX, int iIDFirst, int & iSelectedID );

  protected:

    /// mfc message map
    DECLARE_MESSAGE_MAP()

    /// called when the button is checked
    virtual void OnChecked();

    /// sets toggle state
    afx_msg LRESULT OnSetCheck( WPARAM wParam, LPARAM lParam );

    /// WM_MOUSEMOVE handler
    afx_msg void OnMouseMove( UINT uFlags, CPoint point ) {}

    /// called when the mouse leaves the control
    virtual void OnMouseEnter() {}

  }; // class SkinAutoRadioButton 

} // namespace WinUtils


// $Log: SkinButton.h,v $
// Revision 1.7.2.1  2009/12/11 22:32:32  dongqing
// allow DDX set/get of radio buttons by ID in addition to cardinality
//
// Revision 1.7  2007/03/10 03:19:31  jmeade
// code standards.
//
// Revision 1.6  2007/03/09 01:11:49  jmeade
// code standards.
//
// Revision 1.5  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.4  2006/02/01 13:56:51  mkontak
// Coding standards
//
// Revision 1.3  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2.4.2  2006/02/28 17:30:13  geconomos
// reworked SkinButton as custom control
//
// Revision 1.2.4.1  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.4.4.2.4.1  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.4.4.2.8.1  2005/06/06 16:36:55  geconomos
// added a hack to ensure that the parent is notified when the button is clicked
//
// Revision 3.4.4.2  2003/03/10 18:43:27  jmeade
// Issue 2841: Skinned auto radio buttons.
//
// Revision 3.4.4.1  2003/02/26 17:10:32  jmeade
// Remove dependency on Viatron resource file by allowing lib user to set default button.
//
// Revision 3.4  2002/09/25 18:59:01  jmeade
// Update window region on button resize.
//
// Revision 3.3  2002/01/09 21:02:45  jmeade
// GetBitmap() override.
//
// Revision 3.2  2001/11/21 00:26:28  jmeade
// Removed obsolete variable.
//
// Revision 3.1  2001/11/17 01:50:52  jmeade
// Multi-state button bitmaps
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
// Revision 1.10  2001/05/17 06:14:31  jmeade
// Allow caller of LoadSkin() function to set the Help Text message ID,
// rather than using the Viatron message
//
// Revision 1.9  2001/05/11 22:36:26  jmeade
// Coding conventions
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SkinButton.h,v 1.7.2.1 2009/12/11 22:32:32 dongqing Exp $
// $Id: SkinButton.h,v 1.7.2.1 2009/12/11 22:32:32 dongqing Exp $
