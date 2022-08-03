// $Id: Theme.h,v 1.7.2.1 2009/11/25 22:09:07 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (george@viatronix.com)


// pragma declarations
#pragma once



// namespaces
OPEN_WINUTILS_NS


// class declaration
class Theme
{
// member functions
public:

  /// initializes the  theme
  static void Initialize();

  /// draws the disclaimer
  static void DrawDisclaimer( Gdiplus::Graphics & gfx, Gdiplus::RectF rect );

private:

  /// initializes the logo
  static void InitializeLogo();

// data members for all to see and use
public:

  // logos
  static Gdiplus::Bitmap * LogoRegistrationView;

  // horizontal TabStrip images
  static Gdiplus::Bitmap * TabHorzActiveLeft;
  static Gdiplus::Bitmap * TabHorzActiveMiddle;
  static Gdiplus::Bitmap * TabHorzActiveRight;
  static Gdiplus::Bitmap * TabHorzActiveEnd;
  static Gdiplus::Bitmap * TabHorzInactiveLeft;
  static Gdiplus::Bitmap * TabHorzInactiveMiddle;
  static Gdiplus::Bitmap * TabHorzInactiveRight;
  static Gdiplus::Bitmap * TabHorzInactiveEnd;

  // vertical TabStrip images
  static Gdiplus::Bitmap * TabVertActiveLeft;
  static Gdiplus::Bitmap * TabVertActiveMiddle;
  static Gdiplus::Bitmap * TabVertActiveRight;
  static Gdiplus::Bitmap * TabVertActiveEnd;
  static Gdiplus::Bitmap * TabVertInactiveLeft;
  static Gdiplus::Bitmap * TabVertInactiveMiddle;
  static Gdiplus::Bitmap * TabVertInactiveRight;
  static Gdiplus::Bitmap * TabVertInactiveEnd;

  // tabstrip fonts and brush
  static Gdiplus::Font * TabFont;
  static Gdiplus::Font * TabSelectedFont;
  static Gdiplus::SolidBrush * TabBrush;
  static Gdiplus::SolidBrush * TabSelectedBrush;

  // menubar 
  static Gdiplus::Font * MenubarFont;
  static Gdiplus::Pen * MenubarSelectedPen;
  static Gdiplus::Bitmap * MenubarBkgndMiddle;
  static Gdiplus::Bitmap * MenubarBkgndRight;
  static Gdiplus::SolidBrush * MenubarTextBrush;
  static Gdiplus::SolidBrush * MenubarSelectedBrush;
  static Gdiplus::Bitmap * MenubarBkgndLeft;

  // button
  static Gdiplus::Font * ButtonFont;
  static Gdiplus::SolidBrush * ButtonBrush;
  static Gdiplus::SolidBrush * ButtonDisabledBrush;
  static Gdiplus::Pen * ButtonFocusPen;

  // background images
  static Gdiplus::Bitmap * BkgndSideLeft;
  static Gdiplus::Bitmap * BkgndSideRight;
  static Gdiplus::Bitmap * BkgndSideBottom;
  static Gdiplus::Bitmap * BkgndSideTop;
  static Gdiplus::Bitmap * BkgndCornerBottomLeft;
  static Gdiplus::Bitmap * BkgndCornerBottomRight;
  static Gdiplus::Bitmap * BkgndCornerTopLeft;
  static Gdiplus::Bitmap * BkgndHeaderLeft;
  static Gdiplus::Bitmap * BkgndHeaderMiddle;
  static Gdiplus::Bitmap * BkgndHeaderRight;

  static Gdiplus::Bitmap * BkgndHeaderRightWidescreen;

  // mouse help
  static Gdiplus::Font *       MouseHelpFont;
  static Gdiplus::Font *       MouseHelpKeyFont;
  static Gdiplus::SolidBrush * MouseHelpBrush;
  static Gdiplus::SolidBrush * MouseHelpKeyBrush;
  static Gdiplus::SolidBrush * MouseHelpKeyEnabledBrush;

  // patient info (horizontal version )
  static Gdiplus::Pen * PatientInfoPen;
  static Gdiplus::Font * PatientInfoFont;
  static Gdiplus::SolidBrush * PatientInfoBrush;

  // reporting background
  static Gdiplus::Bitmap *    ReportingBkgnd;

  // disclaimer
  static Gdiplus::Font * DisclaimerFont;
  static Gdiplus::SolidBrush * DisclaimerBrush;

  /// default dialog background brush
  static CBrush * DefaultBackgroundBrush;

  /// default dialog background color
  static COLORREF DefaultBackgroundColor;

}; // class Theme


// namespaces
CLOSE_WINUTILS_NS


// $Log: Theme.h,v $
// Revision 1.7.2.1  2009/11/25 22:09:07  dongqing
// widescreen
//
// Revision 1.7  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.6  2006/01/31 14:27:33  frank
// code review
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.3  2006/02/28 17:26:05  geconomos
// added theme elements for buttons
//
// Revision 1.4.2.2  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.4.2.1  2006/01/04 13:05:32  frank
// Moved default background brush to the theme class
// Pre-blended logo for supine/prone view to fix over-accumulation of white
//
// Revision 1.4  2005/10/07 13:12:01  geconomos
// added disclaimer related items
//
// Revision 1.3  2005/10/06 20:35:21  geconomos
// updated for menubar and patient information dialog
//
// Revision 1.2  2005/10/04 02:06:01  geconomos
// added backgound header and registration view logo images
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Theme.h,v 1.7.2.1 2009/11/25 22:09:07 dongqing Exp $
// $Id: Theme.h,v 1.7.2.1 2009/11/25 22:09:07 dongqing Exp $
