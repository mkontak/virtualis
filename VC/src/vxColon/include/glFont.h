// $Id: glFont.h,v 1.2.2.1 2007/12/12 16:45:12 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Jeffrey Meade( mailto:jmeade@viatronix.net )

/**
 * interface for the GLFont class
 */


// pragmas
#pragma once


// includes
#include "Rect.h"
#include "Point.h"
#include "Triple.h"


// enumerations
enum GDT_TEXTLOC { GDT_TEXTLOCNONE = 0, GDT_LEFT = 1, GDT_RIGHT = 2, GDT_ABOVE = 4, GDT_BELOW = 8,
                    GDT_ABOVELEFT = GDT_ABOVE | GDT_LEFT, GDT_ABOVERIGHT = GDT_ABOVE | GDT_RIGHT,
                    GDT_BELOWLEFT = GDT_BELOW | GDT_LEFT, GDT_BELOWRIGHT = GDT_BELOW | GDT_RIGHT };


/// for drawing text in OpenGL child windows
class GLFont
{
public:

  /// Default constructor
  GLFont();

  /// Destructor
  virtual ~GLFont();

  /// Constructor, calls Create()
  GLFont(const HDC & hDC, const int iCharHeight, const int iCharWidth, const std::string & sTypeFaceName,
          const long iCharWeight = FW_DONTCARE, const bool bItalic = false, const bool bUnderline = false,
          const bool bUniCode = false);
	
  /// Creates the font; NOTE: A font can only be created once per class instance
  void Create(const HDC & hDC, const int iCharHeight, const int iCharWidth, const std::string & sTypeFaceName,
          const long iCharWeight = FW_DONTCARE, const bool bItalic = false, const bool bUnderline = false,
          const bool bUniCode = false);

  /// frees the resources associated with the font
  void Free();

  /// Draws 2D text at the point and location specified
  void DrawText(int iX, int iY, const std::string & sText, const GDT_TEXTLOC eTextLocation = GDT_ABOVERIGHT) const;

  /// Draws 2D text at the point and location specified
  void DrawText(int iX, int iY, int iZ,const std::string & sText, const GDT_TEXTLOC eLocn = GDT_ABOVERIGHT) const;

  /// Draw a string as a bitmap in a 3D rendering context
  void CharBitmap(const Point<float> & positionPt, const Triple<float> & color, const std::string & sText,
                  const bool bShadow = false, const Triple<float> & shadowColor = Triple<float>(0,0,0) ) const;

  /// Returns the font height in pixels
  const int4 GetPixelHeight() const { return m_iFontHeight; }

  /// Compute the width of the string in terms of pixels
  int4 GetPixelWidth( const std::string & sText ) const;

  /// Returns the width of a single character
  inline int4 GetPixelWidth( const char c ) const { return m_viWidths[c]; }

private:

  /// Character set, uses specific identifiers, so it is named like an enum
  long m_eCharSet;
  long m_iNumCharsInSet;
  long m_iCharDispListBase;

  /// Width of font
  long m_iFontWidth;

  /// Height of font
  long m_iFontHeight;

  /// width of each character in pixels 
  int4 m_viWidths[256]; 

}; // class GLFont


/// Convenience typedef, pointer to GLFont
typedef GLFont * GLFontPtr;


// $Log: glFont.h,v $
// Revision 1.2.2.1  2007/12/12 16:45:12  jmeade
// comments.
//
// Revision 1.2  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.4.2  2006/05/22 12:40:46  geconomos
// fixed resource leak with regard to font object
//
// Revision 1.1.4.1  2006/05/09 13:51:03  geconomos
// Restoring original font back to device context after creating font glyphs
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.2.8.6  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.2.8.4  2005/04/20 14:45:08  frank
// fixed font height and width computation and retrieval
//
// Revision 3.2.8.3  2005/04/20 10:36:31  frank
// fixed computation of string width
//
// Revision 3.2.8.2  2005/04/19 14:04:15  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.2.8.1  2005/03/21 19:54:00  frank
// Issue #4038: Sped up font drawing
//
// Revision 3.2  2003/01/22 22:24:45  ingmar
// added missing includes
//
// Revision 3.1  2002/11/04 20:35:42  kevin
// Added 3D text positioning
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:40   ingmar
// Initial revision.
// Revision 1.13  2001/05/11 19:30:17  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/glFont.h,v 1.2.2.1 2007/12/12 16:45:12 jmeade Exp $
// $Id: glFont.h,v 1.2.2.1 2007/12/12 16:45:12 jmeade Exp $
