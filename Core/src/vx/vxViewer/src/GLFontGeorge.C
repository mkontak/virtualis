// $Id: GLFontGeorge.C,v 1.5 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconomos@viatronix.com)
//
// Complete history at bottom of file.


// includes
#include "stdafx.h"
#include "GLFontGeorge.h"
#include "Triple.h"
#include "vxIRenderingContext.h"


/**
 * Constructor
 */
GLFontGeorge::GLFontGeorge()
{
  m_style = GLFontGeorge::PLAIN;
  m_bFontCreated  = false;
}


/**
 * Destructor
 */
GLFontGeorge::~GLFontGeorge()
{
  Free();
}


/**
 * Create font
 * @param GL context
 * @param type face
 * @param font height
 * @param weight
 * @param italic
 */
void GLFontGeorge::Create( const vxIRenderingContext * pContext, const std::string & sTypeface,
                           int4 iHeight, int4 iWeight, uint4 uItalic )
{

  Free();
  // allocate display lists 
  if( ( m_uBase = glGenLists( 256 ) ) == 0 )
  {
    return;
  }
  
  // Select a character set 
  int4 iCharset(0);
  if( !_stricmp( sTypeface.c_str(), "symbol" ) )
    iCharset = SYMBOL_CHARSET;
  else
    iCharset = ANSI_CHARSET;
  
  // Load the font 
  HFONT hFont = CreateFont( iHeight, 0, 0, 0, iWeight, uItalic, FALSE, FALSE,
    iCharset, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, sTypeface.c_str() );
  
  HGDIOBJ hOldFont =  SelectObject( pContext->GetHDC(), hFont );
  
  // Create bitmaps for each character
  wglUseFontBitmaps( pContext->GetHDC(), 0, 256, m_uBase );
  
  // Get the width and height information for each character 
  GetCharWidth( pContext->GetHDC(), 0, 255, m_piWidths );
  m_iHeight       = iHeight;
  m_bFontCreated  = true;

  SelectObject( pContext->GetHDC(), hOldFont );

} // Create()


/**
 *	Gets the width of the specified string in pixels.
 *
 * @param sString String to calculate width.
 * @return Width of string in pixels.
 */
int4 GLFontGeorge::GetPixelWidth( const std::string & sString ) const
//*******************************************************************
{
  int iWidth;
  char * ptr;
  char * s = const_cast< char * >( sString.c_str() ); // Yes, this is bad and I hate it but I need the size and this damned font classes need a rework anyway (bad excuse, I know) :-(
  for (ptr = s, iWidth = 0; *ptr; ptr++)
    iWidth += m_piWidths[ *ptr ];
  return iWidth;
} // GetPixelWidth()

  
/**
 *	Gets the height of the font in pixels.
 *
 * @return Height of font in pixles.
 */
int4 GLFontGeorge::GetPixelHeight() const
//*******************************************************************
{
  if( GetStyle() == GLFontGeorge::SHADOW_AROUND )
  {
    return m_iHeight+2;
  }
  return m_iHeight;
} // GetPixelHeight()


/**
 * Fress all resources asociated with the font.
 */
void GLFontGeorge::Free()
{
  if( m_bFontCreated == true )
  {
    glDeleteLists(m_uBase, 256);
    m_bFontCreated = false;
  }
} // Free()


/**
 * Writes the specified string to the display.
 *
 * @param sString Output string.
 */
void GLFontGeorge::PutString( const std::string & sString )
{
  if( m_bFontCreated == false || sString.length() == 0 )
    return;

  /**
   * For one reason or another simply using the string's c_str() method 
   * occasionaly causes a crash. Copying the value to a temporay buffer fixes the situation.
   * Seems to happen for certain video cards? I anycase this is a band-aid!
   */
  const int4 iMaxStringLength( 1024 );  
  char sTmpString[ iMaxStringLength ];
  lstrcpyn( sTmpString, sString.c_str(), iMaxStringLength );  
  int4 iStringLength( sString.length() );
  
  
  glPushAttrib( GL_LIST_BIT );
    glListBase( m_uBase );
    glCallLists( iStringLength, GL_UNSIGNED_BYTE, sTmpString );
  glPopAttrib();
} // PutString()


/**
 * Formats the specified string and writes it to the display.
 *
 * @param sString Output string.
 * @param eAlignment String alignment.
 * @param ... variable argument list for formatting string
 */
void GLFontGeorge::PutFormattedString( const std::string & sString, const FontAlignEnum eAlignment, ...)
{
  va_list ap;
  char s[1024];
  char* ptr;
  int4 iWidth;
  
  if( !m_bFontCreated || sString.length() == 0 )
    return;
 
  if( sString.length() == 0 )
    return;
  
  // Format the string 
  va_start(ap, eAlignment);
  vsprintf((char *)s, sString.c_str(), ap);
  va_end(ap);
  
  // Figure out the width of the string in pixels... 
  for(ptr = s, iWidth = 0; *ptr; ptr ++)
    iWidth += m_piWidths[*ptr];
  
  // Adjust the bitmap position as needed to justify the text 
  if( eAlignment == ALIGN_RIGHT )
    glBitmap( 0, 0, 0.0F, 0.0F, (GLfloat)-iWidth, 0.0F, NULL );
  else if (eAlignment == ALIGN_CENTER)
    glBitmap( 0, 0, 0.0F, 0.0F, (GLfloat)-iWidth / 2, 0.0F, NULL );
  
  // Draw the string 
  PutString( s );
} // PutFormattedString( sString, eAlignment, ...)


/**
 * Formats the string and writes it to the display at the specified raster position.
 *
 * @param sString Output string.
 * @param eAlignment String alignment.
 * @param rasterPos position to render string
 * @param ... variable argument list for formatting string
 */
void GLFontGeorge::PutFormattedString( const std::string & sString, 
                                       const FontAlignEnum eAlignment, 
                                       const Point<float4> & rasterPos, ... )
{
  if( m_style == GLFontGeorge::PLAIN )
  {
    glColor3f(1,1,1);
    glRasterPos2f(rasterPos.m_x, rasterPos.m_y);
    PutFormattedString(sString, GLFontGeorge::ALIGN_LEFT);
  }
  else if( m_style == GLFontGeorge::SHADOW_AROUND )
  {
    // draw text with shadows in black in all offset combinations around it
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x - 1, rasterPos.m_y - 1 );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x, rasterPos.m_y - 1 );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x + 1, rasterPos.m_y - 1 );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x - 1, rasterPos.m_y );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x + 1, rasterPos.m_y );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x - 1, rasterPos.m_y + 1 );
    PutFormattedString(sString, GLFontGeorge::ALIGN_LEFT );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x, rasterPos.m_y + 1 );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x + 1, rasterPos.m_y + 1 );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );

    // draw text itself in white
    glColor3f( 1,1,1 );
    glRasterPos2f( rasterPos.m_x, rasterPos.m_y );
    PutFormattedString( sString, GLFontGeorge::ALIGN_LEFT );
  }
} // PutFormattedString( sString, eAlignment, rasterPos, ...)



/**
 * Formats the string and writes it to the display at the specified raster position.
 *
 * @param sString Output string.
 * @param eAlignment String alignment.
 * @param rasterPos position to render string
 * @param color text color
 * @param ... variable argument list for formatting string
 */
void GLFontGeorge::PutFormattedString( const std::string & sString, const FontAlignEnum eAlignment, const Point2D<int4> &rasterPos, const Triple<float4> color, ... )
{
  if( m_style == GLFontGeorge::PLAIN )
  {
    // set color and display the text
    glColor3f( color.m_x, color.m_y, color.m_z );
    glRasterPos2i( rasterPos.m_x, rasterPos.m_y );
    PutFormattedString( sString, eAlignment );
  }
  else if( m_style == GLFontGeorge::SHADOW_AROUND )
  {
    // draw text with shadows in black in all offset combinations around it
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x-1, rasterPos.m_y-1 );
    PutFormattedString( sString, eAlignment );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x, rasterPos.m_y-1 );
    PutFormattedString( sString, eAlignment );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x+1, rasterPos.m_y-1 );
    PutFormattedString( sString, eAlignment );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x-1, rasterPos.m_y );
    PutFormattedString( sString, eAlignment );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x+1, rasterPos.m_y );
    PutFormattedString( sString, eAlignment );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x-1, rasterPos.m_y+1 );
    PutFormattedString( sString, eAlignment );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x, rasterPos.m_y+1 );
    PutFormattedString( sString, eAlignment );
    glColor3f( 0, 0, 0 );
    glRasterPos2f( rasterPos.m_x+1, rasterPos.m_y+1 );
    PutFormattedString( sString, eAlignment );

    // set color and display the text
    glColor3f( color.m_x, color.m_y, color.m_z );
    glRasterPos2i( rasterPos.m_x, rasterPos.m_y );
    PutFormattedString( sString, eAlignment );
  }
} // // PutFormattedString( sString, eAlignment, rasterPos, color, ...)


// $Log: GLFontGeorge.C,v $
// Revision 1.5  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.4  2006/04/21 18:50:10  geconomos
// fixed random crash in PutString
//
// Revision 1.3  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2.18.1  2006/05/09 13:50:40  geconomos
// Restoring original font back to device context after creating font glyphs
//
// Revision 1.2  2004/04/02 14:01:32  frank
// working on unifying the various rendering contexts
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.15  2003/10/02 10:24:13  geconomos
// Added a call to Free before trying to recreate a font.
//
// Revision 3.14  2003/05/16 11:45:25  michael
// code review
//
// Revision 3.13  2003/05/14 13:26:59  geconomos
// Coding standards.
//
// Revision 3.12  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.11  2003/01/28 17:02:05  michael
// fixed statistics of box annotation
//
// Revision 3.10  2003/01/22 21:53:12  ingmar
// added missing includes
//
// Revision 3.9  2003/01/17 00:34:23  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 3.8  2002/05/07 18:33:17  frank
// Removed unneccessary includes.
//
// Revision 3.7  2002/03/06 13:09:37  michael
// formatting
//
// Revision 3.6  2002/02/27 20:59:47  michael
// removed compiler warning
//
// Revision 3.5  2001/11/21 22:18:58  michael
// added a style to the GLFontGeorge. PLAIN is as usually and SHADOWED
// is makeing a black shadow around it.
//
// Revision 3.4  2001/11/14 01:07:36  michael
// set the color for shadowed text to black after drawing the first text. Something
// seems to be overriding it with white and I am not happy with this solution ....
// but for RSNA it should be enough. :-)
//
// Revision 3.3  2001/11/09 15:39:07  manju
// Modified the shadow function.
//
// Revision 3.2  2001/11/07 20:42:58  manju
// Added another PutFormattedString function to display shadow and the text.
//
// Revision 3.1  2001/11/07 00:22:08  jenny
// created one more PutFormattedString()
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:32   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:18  ingmar
// new directory structure
//
// Revision 1.3  2001/07/02 15:43:40  geconomos
// Implemented GLFontGeorge class
//
// Revision 1.2  2001/07/02 14:54:10  huamin
// Changed the constructor
//
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/GLFontGeorge.C,v 1.5 2007/02/15 00:08:23 jmeade Exp $
// $Id: GLFontGeorge.C,v 1.5 2007/02/15 00:08:23 jmeade Exp $
