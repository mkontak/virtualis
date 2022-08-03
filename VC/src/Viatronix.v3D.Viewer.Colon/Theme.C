// $Id: Theme.C,v 1.8.2.1 2009/11/25 22:09:06 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (george@viatronix.com)


// includes
#include "StdAfx.h"
#include "Theme.h"
#include "Resource.h"
#include "GdiplusUtilities.h"


// namespaces
using namespace WinUtils;


// static initialization
Gdiplus::Font * Theme::TabFont = NULL;
Gdiplus::Font * Theme::TabSelectedFont = NULL;
Gdiplus::SolidBrush * Theme::TabBrush = NULL;
Gdiplus::SolidBrush * Theme::TabSelectedBrush = NULL;
Gdiplus::Bitmap * Theme::TabHorzActiveLeft      = NULL;
Gdiplus::Bitmap * Theme::TabHorzActiveMiddle    = NULL;
Gdiplus::Bitmap * Theme::TabHorzActiveRight     = NULL;
Gdiplus::Bitmap * Theme::TabHorzActiveEnd       = NULL;
Gdiplus::Bitmap * Theme::TabHorzInactiveLeft    = NULL;
Gdiplus::Bitmap * Theme::TabHorzInactiveMiddle  = NULL;
Gdiplus::Bitmap * Theme::TabHorzInactiveRight   = NULL;
Gdiplus::Bitmap * Theme::TabHorzInactiveEnd     = NULL;    
Gdiplus::Bitmap * Theme::TabVertActiveLeft      = NULL;
Gdiplus::Bitmap * Theme::TabVertActiveMiddle    = NULL;
Gdiplus::Bitmap * Theme::TabVertActiveRight     = NULL;
Gdiplus::Bitmap * Theme::TabVertActiveEnd       = NULL;
Gdiplus::Bitmap * Theme::TabVertInactiveLeft    = NULL;
Gdiplus::Bitmap * Theme::TabVertInactiveMiddle  = NULL;
Gdiplus::Bitmap * Theme::TabVertInactiveRight   = NULL;
Gdiplus::Bitmap * Theme::TabVertInactiveEnd     = NULL;

Gdiplus::Pen * Theme::PatientInfoPen = NULL;
Gdiplus::Font * Theme::PatientInfoFont = NULL;
Gdiplus::SolidBrush * Theme::PatientInfoBrush = NULL;

Gdiplus::Font * Theme::ButtonFont = NULL;
Gdiplus::SolidBrush * Theme::ButtonBrush = NULL;
Gdiplus::SolidBrush * Theme::ButtonDisabledBrush = NULL;
Gdiplus::Pen * Theme::ButtonFocusPen = NULL;

Gdiplus::Bitmap * Theme::BkgndSideLeft          = NULL;
Gdiplus::Bitmap * Theme::BkgndSideRight         = NULL;
Gdiplus::Bitmap * Theme::BkgndSideBottom        = NULL;
Gdiplus::Bitmap * Theme::BkgndSideTop           = NULL;
Gdiplus::Bitmap * Theme::BkgndCornerBottomLeft  = NULL;
Gdiplus::Bitmap * Theme::BkgndCornerBottomRight = NULL;
Gdiplus::Bitmap * Theme::BkgndCornerTopLeft     = NULL;
Gdiplus::Bitmap * Theme::BkgndHeaderLeft        = NULL;
Gdiplus::Bitmap * Theme::BkgndHeaderMiddle      = NULL;
Gdiplus::Bitmap * Theme::BkgndHeaderRight       = NULL;

Gdiplus::Bitmap * Theme::BkgndHeaderRightWidescreen = NULL;

Gdiplus::Bitmap * Theme::LogoRegistrationView     = NULL;

Gdiplus::Font * Theme::MouseHelpFont                   = NULL;
Gdiplus::Font * Theme::MouseHelpKeyFont                = NULL;
Gdiplus::SolidBrush * Theme::MouseHelpBrush            = NULL;
Gdiplus::SolidBrush * Theme::MouseHelpKeyBrush         = NULL;
Gdiplus::SolidBrush * Theme::MouseHelpKeyEnabledBrush  = NULL;

Gdiplus::Bitmap *    Theme::ReportingBkgnd = NULL;

Gdiplus::Font * Theme::MenubarFont                = NULL;
Gdiplus::Pen * Theme::MenubarSelectedPen          = NULL;
Gdiplus::Bitmap * Theme::MenubarBkgndLeft         = NULL;
Gdiplus::Bitmap * Theme::MenubarBkgndMiddle       = NULL;
Gdiplus::Bitmap * Theme::MenubarBkgndRight        = NULL;
Gdiplus::SolidBrush * Theme::MenubarSelectedBrush = NULL;
Gdiplus::SolidBrush * Theme::MenubarTextBrush     = NULL;

Gdiplus::Font * Theme::DisclaimerFont = NULL;
Gdiplus::SolidBrush * Theme::DisclaimerBrush = NULL;

CBrush * Theme::DefaultBackgroundBrush = NULL;
COLORREF Theme::DefaultBackgroundColor = RGB( 108, 119, 166 );


/**
 * Initalizes the theme.
 */
void Theme::Initialize()
{

  // tabstrip fonts and brushes
  TabFont = new Gdiplus::Font( L"Arial", 10 );
  TabBrush = new Gdiplus::SolidBrush( Gdiplus::Color::Black );
  TabSelectedFont = new Gdiplus::Font( L"Arial", 10 );
  TabSelectedBrush = new Gdiplus::SolidBrush( Gdiplus::Color::Black );

  // tabstrip horizontal tab images
  TabHorzActiveLeft     = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_ACTIVE_LEFT ),      "PNG" );
  TabHorzActiveMiddle   = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_ACTIVE_MIDDLE ),    "PNG" );
  TabHorzActiveRight    = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_ACTIVE_RIGHT ),     "PNG" );
  TabHorzActiveEnd      = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_ACTIVE_END ),       "PNG" );
  TabHorzInactiveLeft   = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_INACTIVE_LEFT ),    "PNG" );
  TabHorzInactiveMiddle = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_INACTIVE_MIDDLE ),  "PNG" );
  TabHorzInactiveRight  = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_INACTIVE_RIGHT ),   "PNG" );
  TabHorzInactiveEnd    = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_TAB_INACTIVE_END ),     "PNG" );

  // tabstrip vertical tab images
  TabVertActiveLeft     = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_ACTIVE_LEFT ),      "PNG" );
  TabVertActiveMiddle   = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_ACTIVE_MIDDLE ),    "PNG" );
  TabVertActiveRight    = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_ACTIVE_RIGHT ),     "PNG" );
  TabVertActiveEnd      = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_ACTIVE_END ),       "PNG" );
  TabVertInactiveLeft   = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_INACTIVE_LEFT ),    "PNG" );
  TabVertInactiveMiddle = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_INACTIVE_MIDDLE ),  "PNG" );
  TabVertInactiveRight  = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_INACTIVE_RIGHT ),   "PNG" );
  TabVertInactiveEnd    = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_SIDE_TAB_INACTIVE_END ),     "PNG" );

  // menubar
  MenubarFont           = new Gdiplus::Font( L"Arial", 10 );
  MenubarSelectedPen    = new Gdiplus::Pen( Gdiplus::Color( 0,102,153 ) );
  MenubarSelectedBrush  = new Gdiplus::SolidBrush( Gdiplus::Color( 125, 102, 153,204 ) );
  MenubarTextBrush   = new Gdiplus::SolidBrush( Gdiplus::Color::Black );
  MenubarBkgndLeft   = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_MENUBAR_LEFT ),      "PNG" );
  MenubarBkgndMiddle = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_MENUBAR_MIDDLE ),    "PNG" );
  MenubarBkgndRight  = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_MENUBAR_RIGHT ),     "PNG" );

  // patient info
  PatientInfoPen = new Gdiplus::Pen( Gdiplus::Color::DarkGray );
  PatientInfoFont = new Gdiplus::Font( L"Tahoma", 14 );
  PatientInfoBrush = new Gdiplus::SolidBrush( Gdiplus::Color::Black );

  // button
  ButtonFont = new Gdiplus::Font( L"Tahoma", 12, Gdiplus::FontStyleBold);
  ButtonBrush = new Gdiplus::SolidBrush( Gdiplus::Color::Black );
  ButtonDisabledBrush = new Gdiplus::SolidBrush( Gdiplus::Color::Gray );
  ButtonFocusPen = new Gdiplus::Pen( Gdiplus::Color::DarkGray );
  ButtonFocusPen->SetDashStyle( Gdiplus::DashStyleDash );

  // background images
  BkgndSideLeft          = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_SIDE_LEFT ),            "PNG" );
  BkgndSideRight         = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_SIDE_RIGHT ),           "PNG" );
  BkgndSideBottom        = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_SIDE_BOTTOM ),          "PNG" );
  BkgndSideTop           = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_SIDE_TOP ),             "PNG" );
  BkgndCornerBottomLeft  = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_CORNER_BOTTOM_LEFT ),   "PNG" );
  BkgndCornerBottomRight = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_CORNER_BOTTOM_RIGHT ),  "PNG" );
  BkgndCornerTopLeft     = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_CORNER_TOP_LEFT ),      "PNG" );
  BkgndHeaderLeft        = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_HEADER_LEFT ),          "PNG" );
  BkgndHeaderMiddle      = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_HEADER_MIDDLE ),        "PNG" );
  BkgndHeaderRight       = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_HEADER_RIGHT ),         "PNG" );

  BkgndHeaderRightWidescreen = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_BKGND_HEADER_RIGHT_WIDESCREEN ),         "PNG" );


  // mouse help fonts and brushes
  MouseHelpFont             = new Gdiplus::Font( L"Arial", 10 );
  MouseHelpKeyFont          = new Gdiplus::Font( L"Arial", 8 );
  MouseHelpBrush            = new Gdiplus::SolidBrush( Gdiplus::Color::Black );
  MouseHelpKeyBrush         = new Gdiplus::SolidBrush( Gdiplus::Color::DarkGray );
  MouseHelpKeyEnabledBrush  = new Gdiplus::SolidBrush( Gdiplus::Color::Black );

  // reporting dialog background
  ReportingBkgnd = new  Gdiplus::Bitmap( AfxGetResourceHandle(), MAKEINTRESOURCEW( IDB_BLUEMETAL ) );

  // disclaimer
  DisclaimerFont = new Gdiplus::Font( L"Microsoft Sans Serif", 32 );
  DisclaimerBrush = new Gdiplus::SolidBrush( Gdiplus::Color::Red );

  // default dialog background brush
  DefaultBackgroundBrush = new CBrush( DefaultBackgroundColor );

  InitializeLogo();

} // Initialize()


/**
 * Initalizes the logo.
 */
void Theme::InitializeLogo()
{

  //
  // pre-composite the logo over the background so that subsequent drawing is flicker-free
  //

  // load the original
  Gdiplus::Bitmap * pTempLogo = GdiplusUtilities::LoadBitmapFromResource( MAKEINTRESOURCE( IDB_LOGO_REGISTRATION_VIEW ), "PNG" );

  // make a blank bitmap
  LogoRegistrationView = new Gdiplus::Bitmap( pTempLogo->GetWidth(), pTempLogo->GetHeight() );

  // draw the background first
  Gdiplus::Graphics * pGfx = Gdiplus::Graphics::FromImage( LogoRegistrationView );
  Gdiplus::SolidBrush brush( Gdiplus::Color( 255, GetRValue( DefaultBackgroundColor ), GetGValue( DefaultBackgroundColor ), GetBValue( DefaultBackgroundColor ) ) );
  pGfx->FillRectangle( & brush, 0, 0, LogoRegistrationView->GetWidth(), LogoRegistrationView->GetHeight() );

  // composite the logo
  pGfx->DrawImage( pTempLogo, 0, 0 );

  delete pTempLogo;

} // Initialize()


/**
 * Draws the disclaimer
 *
 * @param   gfx     graphics object to draw to
 * @param   rect    area to draw to
 */
void Theme::DrawDisclaimer( Gdiplus::Graphics & gfx, Gdiplus::RectF rect )
{

  USES_CONVERSION;

  CString s;
  VERIFY( s.LoadString( IDS_DISCLAIMER ) );
  
  // store the current transformation matrix
  Gdiplus::Matrix transform;      
  gfx.GetTransform( &transform );

  // rotate it
  gfx.TranslateTransform( 0, 500 );
  gfx.RotateTransform( -30 );

  // draw it
  gfx.DrawString( A2W( (LPCSTR) s),  s.GetLength(), Theme::DisclaimerFont, Gdiplus::PointF( 0, 0 ), Theme::DisclaimerBrush );

  // restore original transformation matrix
  gfx.SetTransform( & transform );

} // DrawDisclaimer()


// $Log: Theme.C,v $
// Revision 1.8.2.1  2009/11/25 22:09:06  dongqing
// widescreen
//
// Revision 1.8  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.7  2006/01/31 14:27:33  frank
// code review
//
// Revision 1.6  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5.2.3  2006/02/28 19:54:57  jmeade
// bold font on buttons.
//
// Revision 1.5.2.2  2006/02/28 17:26:05  geconomos
// added theme elements for buttons
//
// Revision 1.5.2.1  2006/01/04 13:05:32  frank
// Moved default background brush to the theme class
// Pre-blended logo for supine/prone view to fix over-accumulation of white
//
// Revision 1.5  2005/10/07 13:12:01  geconomos
// added disclaimer related items
//
// Revision 1.4  2005/10/06 20:35:21  geconomos
// updated for menubar and patient information dialog
//
// Revision 1.3  2005/10/04 02:06:00  geconomos
// added backgound header and registration view logo images
//
// Revision 1.2  2005/10/03 15:01:06  geconomos
// updated for changes made to GdiplusUtilities
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Theme.C,v 1.8.2.1 2009/11/25 22:09:06 dongqing Exp $
// $Id: Theme.C,v 1.8.2.1 2009/11/25 22:09:06 dongqing Exp $
