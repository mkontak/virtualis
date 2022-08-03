// $Id: MouseHelp.h,v 1.2 2007/03/01 21:40:09 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragma declarations
#pragma once

OPEN_WINUTILS_NS

// class declaration
class MouseHelp : public CWnd 
{
// member functions
public:
  
  /// constructor
  MouseHelp();
  
  /// desturctor
  virtual ~MouseHelp();

  /// prevents the control from being redrawn as items are add and removed
  void BeginInit() { m_bInitializing = true; }

  /// sets initialization as completed and redraws the controls
  void EndInit();
    
  /// gets the left button text
  const std::string & GetLeftButtonText() const { return m_sLeftButtonText; }

  /// sets the left button text
  void SetLeftButtonText(  const std::string & sLeftButtonText );

  /// gets the middle button text
  const std::string & GetMiddleButtonText() const { return m_sMiddleButtonText; }

  /// sets the middle button text
  void SetMiddleButtonText(  const std::string & sMiddleButtonText );

  /// gets the right button text
  const std::string & GetRightButtonText() const { return m_sRightButtonText; }

  /// sets the right button text
  void SetRightButtonText(  const std::string & sRightButtonText );

  /// gets the control key enabled flag
  const bool & GetControlKeyEnabled() const { return m_bControlKeyEnabled; }

  /// sets the control key enabled flag
  void SetControlKeyEnabled( const bool & bEnabled );

  /// gets the shift key enabled flag
  const bool & GetShiftKeyEnabled() const { return m_bShiftKeyEnabled; }

  /// sets the shift key enabled flag
  void SetShiftKeyEnabled( const bool & bEnabled );

protected:

    /// mfc message map
    DECLARE_MESSAGE_MAP()

    /// WM_PAINT handler
    void OnPaint();

    /// WM_ERASEBKGND handler
    BOOL OnEraseBkgnd( CDC * pdc );

    /// WM_SIZE handler
    void OnSize( UINT uType, int cx, int cy );

private:

    /// redraws the control into the draw buffer
    void RedrawControl();

 /// member variables
private:

  /// left button text
  std::string m_sLeftButtonText;
  
  /// middle button text
  std::string m_sMiddleButtonText;
  
  /// right button text
  std::string m_sRightButtonText;

  /// indicates if the control key is enabled
  bool m_bControlKeyEnabled;

  /// indicates if the shift key is enabled
  bool m_bShiftKeyEnabled;

  /// draw buffer
  Gdiplus::Bitmap * m_pDrawBuffer;

  /// indicates if the control is being initialized
  bool m_bInitializing;

  /// left mouse image 
  Gdiplus::Bitmap m_leftMouseImage;
  
  /// middle mouse image 
  Gdiplus::Bitmap m_middleMouseImage;
  
  /// right mouse image 
  Gdiplus::Bitmap m_rightMouseImage;
  
  /// left mouse image rect
  Gdiplus::Rect m_rectLeftImage;
  
  /// left mouse text rect
  Gdiplus::RectF m_rectLeftText;
  
  /// middle mouse image rect
  Gdiplus::Rect m_rectMiddleImage;
  
  /// middle mouse text rect
  Gdiplus::RectF m_rectMiddleText;
  
  /// right mouse image rect
  Gdiplus::Rect m_rectRightImage;
  
  /// right mouse text rect
  Gdiplus::RectF m_rectRightText;
  
  /// control key text rect
  Gdiplus::RectF m_rectControlKeyText;
  
  /// shift key text rect
  Gdiplus::RectF m_rectShiftKeyText;

}; // class MouseHelp

CLOSE_WINUTILS_NS

// $Log: MouseHelp.h,v $
// Revision 1.2  2007/03/01 21:40:09  geconomos
// code review preparation
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MouseHelp.h,v 1.2 2007/03/01 21:40:09 geconomos Exp $
// $Id: MouseHelp.h,v 1.2 2007/03/01 21:40:09 geconomos Exp $
