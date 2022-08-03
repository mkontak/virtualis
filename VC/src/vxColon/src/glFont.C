// $Id: glFont.C,v 1.3.2.2 2007/12/12 23:04:46 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: glFont.C: implementation of the GLFont class.
// Owner: Jeffrey Meade  jmeade@viatronix.net


#include "stdafx.h"
#include "glFont.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILE_REVISION "$Revision: 1.3.2.2 $"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Characters bitmaps
const GLubyte SpaceCharBitmap[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const GLubyte vNumberCharBitmap[][13] =
{
  {0x00, 0x00, 0x18, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x18}, // '0'
  {0x00, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x38, 0x18, 0x08}, // '1'
  {0x00, 0x00, 0xff, 0xff, 0x40, 0x20, 0x10, 0x08, 0x04, 0xc3, 0xc3, 0x63, 0x3c}, // '2'
  {0x00, 0x00, 0x7c, 0xc2, 0xc3, 0x13, 0x03, 0x3e, 0x3c, 0x03, 0x63, 0x62, 0x3c}, // '3'
  {0x00, 0x00, 0x17, 0x0c, 0x0c, 0xff, 0xff, 0xcc, 0x4c, 0x4c, 0x2c, 0x2c, 0x0c}, // '4'
  {0x00, 0x00, 0x38, 0xc6, 0xc3, 0x03, 0x03, 0x02, 0xfc, 0xc0, 0xc0, 0xc0, 0xfe}, // '5'
  {0x00, 0x00, 0x3c, 0x42, 0x81, 0x81, 0x82, 0xfc, 0x80, 0x40, 0x20, 0x10, 0x0c}, // '6'
  {0x00, 0x00, 0x80, 0x40, 0x20, 0x20, 0x10, 0x08, 0x08, 0x04, 0x02, 0x82, 0x7f}, // '7'
  {0x00, 0x00, 0xc3, 0x42, 0x81, 0x81, 0x42, 0x3c, 0x42, 0x81, 0x81, 0x42, 0x3c}, // '8'
  {0x00, 0x00, 0x30, 0x08, 0x04, 0x02, 0x01, 0x3f, 0x41, 0x81, 0x81, 0x42, 0x3c}  // '9'
};


/**
 * constructor
 **/
GLFont::GLFont() : 
  m_eCharSet( 0 ),
  m_iNumCharsInSet( 0 ),
  m_iCharDispListBase( 0 ),
  m_iFontWidth( 0 ),
  m_iFontHeight( 0 )
{
} // GLFont()


/**
 * constructor
 * @param hdc             device context for the view
 * @param iCharHeight     height of window
 * @param iCharWidth      height of window
 * @param sTypeFaceName   identifies which font to use
 * @param iCharWeight     bolding
 * @param bItalic         italicizing
 * @param bUnderline      underlining
 * @param bUnicode        Unicode characters
 **/
GLFont::GLFont(const HDC & hdc, const int iCharHeight, const int iCharWidth, const std::string & sTypeFaceName,
               const long iCharWeight, const bool bItalic, const bool bUnderline, const bool bUniCode) : 
  m_iNumCharsInSet( 0 ),
  m_iCharDispListBase( 0 ),
  m_iFontWidth( 0 ),
  m_iFontHeight( 0 )

{
  Create(hdc, iCharHeight, iCharWeight, sTypeFaceName, iCharWeight, bItalic, bUnderline, bUniCode);
} // GLFont()


/**
 * initialize routine
 * @param hdc             device context for the view
 * @param iCharHeight     height of window
 * @param iCharWidth      height of window
 * @param sTypeFaceName   identifies which font to use
 * @param iCharWeight     bolding
 * @param bItalic         italicizing
 * @param bUnderline      underlining
 * @param bUnicode        Unicode characters
 **/
void GLFont::Create(const HDC & hdc, const int iCharHeight, const int iCharWidth, const std::string & sTypeFaceName,
                    const long iCharWeight, const bool bItalic, const bool bUnderline, const bool bUniCode)
{
  char buf[255];

  if( m_iCharDispListBase )
  {
    return;   // Font has already been created
  }
  
  m_iNumCharsInSet = 96;
  m_eCharSet = ANSI_CHARSET;
  
  if( _stricmp(sTypeFaceName.c_str(), "symbol") == 0 )  
    m_eCharSet = SYMBOL_CHARSET;
  
  if( bUniCode )
  {
    m_eCharSet = DEFAULT_CHARSET;
    m_iNumCharsInSet = 224;
  }

  if( !( m_iCharDispListBase = glGenLists( m_iNumCharsInSet ) ) )
  {
    wsprintf( buf, "glGenLists failed. error=%d", glGetError() );
    LogErr( buf, "glFont", "Create" );
    return;
  }
  
  HFONT hFont = CreateFont(iCharHeight, iCharWidth, 0, 0, iCharWeight,
    bItalic == true, bUnderline == true, FALSE, m_eCharSet,
    OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH,
    sTypeFaceName.c_str());
  
  if( hFont == NULL )
  {
    wsprintf( buf, "Unable to create font. GetLastError returned %d", GetLastError() );
    LogErr( buf, "glFont", "Create" );
    return;
  }

  LOGFONT logFont;
  GetObject( hFont, sizeof(LOGFONT), &logFont);
  m_iFontWidth = logFont.lfWidth;
  m_iFontHeight = logFont.lfHeight;
  

  HGDIOBJ hFontOld = SelectObject( hdc, hFont );

  // Create bitmaps for each character
  if(!wglUseFontBitmaps(hdc, 32 /* ' ' */, m_iNumCharsInSet, m_iCharDispListBase))
  {
    wsprintf( buf, "wglUseFontBitmaps failed. error=%d", glGetError() );
    LogErr( buf, "glFont", "Create" );
    return;
  }

  // Get the width and height information for each character 
  GetCharWidth( hdc, 0, 255, m_viWidths );
  m_iFontHeight = abs( iCharHeight );

  SelectObject( hdc, hFontOld );

  DeleteObject( hFont );

} // Create()


/**
 * Frees the resources associated with the font
 */
void GLFont::Free()
{
  if( m_iCharDispListBase )
  {
    glDeleteLists( m_iCharDispListBase, m_iNumCharsInSet );
  }
  
  // Reset all members for safe keeping
  m_eCharSet = 0;
  m_iNumCharsInSet = 0;
  m_iCharDispListBase = 0;
} // Free()


/**
 * destructor
 **/
GLFont::~GLFont()
{
  Free();
} // ~GLFont()


/**
 * Compute the width of the string in terms of pixels
 */
int4 GLFont::GetPixelWidth( const std::string & sText ) const
{

  int iWidth;
  char * ptr;
  char * s = const_cast< char * >( sText.c_str() ); // fixme
  for ( ptr = s, iWidth = 0; *ptr; ptr++ )
    iWidth += m_viWidths[ * ptr ];
  return iWidth;

} // GetPixelWidth()


/**
 * draws the given text
 * @param iX      x location
 * @param iY      y location
 * @param sText   text to draw
 * @param eLocn   location information
 */
void GLFont::DrawText(int iX, int iY, const std::string & sText, const GDT_TEXTLOC eLocn) const
{
  if (!m_iCharDispListBase || (sText.length() <= 0))
    return;
  
  if ((eLocn & GDT_BELOW) == GDT_BELOW)
    iY -= abs(m_iFontHeight);
  if ((eLocn & GDT_LEFT) == GDT_LEFT)
  {
    iX -= GetPixelWidth( sText );
  }
  
  glRasterPos2i(iX, iY);

  // write into a fixed buffer to avoid problem with OpenGL driver!
  const int4 iBufferSize = 1024;
  char sTemp[ iBufferSize ] = ""; // ensure it is null-terminated in case printf fails
  int4 iLength = _snprintf( sTemp, iBufferSize-1, "%s", sText.c_str() );
  if ( iLength > 0 )
  {

    glPushAttrib(GL_LIST_BIT);
    glListBase(m_iCharDispListBase - ' ');
    glCallLists(iLength, GL_UNSIGNED_BYTE, sTemp);
    glPopAttrib();

  }
  
#ifndef FINAL_RELEASE
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR)
  {
    char  buf[255];
    wsprintf( buf, "Unable to draw gl string. error=%d", err );
    LogErr( buf, "glFont", "DrawText" );
    return;
  }
#endif
} // DrawText()


/**
 * draws the given text
 * @param iX      x location
 * @param iY      y location
 * @param iZ      z location
 * @param sText   text to draw
 * @param eLocn   location information
 */
void GLFont::DrawText(int iX, int iY, int iZ,const std::string & sText, const GDT_TEXTLOC eLocn) const
{

  if (!m_iCharDispListBase || (sText.length() <= 0))
    return;
  
  if ((eLocn & GDT_BELOW) == GDT_BELOW)
    iY -= abs(m_iFontHeight);
  if ((eLocn & GDT_LEFT) == GDT_LEFT)
  {
    iX -= GetPixelWidth( sText );
  }
  
  glRasterPos3i(iX, iY, iZ);
  
  glPushAttrib(GL_LIST_BIT);
  glListBase(m_iCharDispListBase - ' ');
  glCallLists(sText.length(), GL_UNSIGNED_BYTE, sText.c_str());
  glPopAttrib();
  
#ifndef FINAL_RELEASE
  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR)
  {
    char  buf[255];
    wsprintf( buf, "Unable to draw gl string. error=%d", err );
    LogErr( buf, "glFont", "DrawText" );
    return;
  }
#endif

} // DrawText()


/**
 * display characters/digits in bitmap in 3D environment.
 * @param positionPt    location
 * @param color         text color
 * @param sText         text to display
 * @param bShadow       text shadowing
 * @param shadowColor   color of shadow
 */
void GLFont::CharBitmap(const Point<float> & positionPt, const Triple<float> & color, const std::string & sText,
                        const bool bShadow, const Triple<float> & shadowColor) const
{

  glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);
  {
    glDisable(GL_LIGHTING);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (bShadow == true)
    {
      glColor3f(shadowColor.m_x, shadowColor.m_y, shadowColor.m_z);
      glRasterPos3f(positionPt.m_x, positionPt.m_y, positionPt.m_z);
    
      for (int i=0; i < sText.length(); i++)
      {
        int j = sText[i] - '0';
        glBitmap(8, 13, 1.0, 5.0, 10.0, 0.0, vNumberCharBitmap[j]);
      }
    } // if shadowing

    glColor3f(color.m_x, color.m_y, color.m_z);
    glRasterPos3f(positionPt.m_x, positionPt.m_y, positionPt.m_z);
    for (int i=0; i < sText.length(); i++)
    {
      int j = sText[i] - '0';
      glBitmap(8, 13, 2.0, 4.0, 10.0, 0.0, vNumberCharBitmap[j]);
    }
  }
  glPopAttrib();

} // CharBitmap()


// undefines
#undef FILE_REVISION


// $Log: glFont.C,v $
// Revision 1.3.2.2  2007/12/12 23:04:46  jmeade
// comments.
//
// Revision 1.3.2.1  2007/12/12 16:45:12  jmeade
// comments.
//
// Revision 1.3  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.2  2006/06/01 20:26:50  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.1.4.2  2006/05/22 12:40:40  geconomos
// fixed resource leak with regard to font object
//
// Revision 1.1.4.1  2006/05/09 13:51:03  geconomos
// Restoring original font back to device context after creating font glyphs
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.1.2.1.4.6  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.1.2.1.4.4.2.1  2005/05/25 15:50:20  geconomos
// initial backup
//
// Revision 3.1.2.1.4.4  2005/04/20 14:45:08  frank
// fixed font height and width computation and retrieval
//
// Revision 3.1.2.1.4.3  2005/04/20 10:36:31  frank
// fixed computation of string width
//
// Revision 3.1.2.1.4.2  2005/04/19 14:04:15  geconomos
// - changed view windows to use one open gl rendering context
// - now using 2 fonts per glwnd
//
// Revision 3.1.2.1.4.1  2005/03/21 19:54:00  frank
// Issue #4038: Sped up font drawing
//
// Revision 3.1.2.1  2003/02/12 23:20:36  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.2  2003/02/12 17:43:29  jmeade
// glAttribute push/pop in method.
//
// Revision 3.1  2002/11/04 20:35:41  kevin
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
// Revision 1.18  2001/05/11 19:30:17  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/glFont.C,v 1.3.2.2 2007/12/12 23:04:46 jmeade Exp $
// $Id: glFont.C,v 1.3.2.2 2007/12/12 23:04:46 jmeade Exp $
