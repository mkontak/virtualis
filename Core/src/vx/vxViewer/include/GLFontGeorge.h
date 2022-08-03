// $Id: GLFontGeorge.h,v 1.2 2004/04/02 14:01:32 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconomos@viatronix.com)

#ifndef GLFontGeorge_h
#define GLFontGeorge_h


// includes
#include "Point.h"
#include "Point2D.h"
#include "GLEnv.h"


// forward declarations
class vxIRenderingContext;


// class definition
class VX_VIEWER_DLL GLFontGeorge 
{
/// enums
public:
  
  enum FontAlignEnum
  {
    ALIGN_RIGHT   = -1,
    ALIGN_CENTER  = 0,
    ALIGN_LEFT    = 1,
  };

  enum StyleEnum
  {
    PLAIN         = 0,
    SHADOW_AROUND = 1,
  };

// functions
public:
	
  /// constructor
  GLFontGeorge();
	
  /// destructor
  virtual ~GLFontGeorge();

  /// returns pixel width
  int4 GetPixelWidth( const std::string & sString ) const;
  
  /// returns pixel height
  int4 GetPixelHeight() const;

  /// returns style
  StyleEnum GetStyle() const { return m_style; };

  /// set style
  void SetStyle( StyleEnum style ) { m_style = style; };
  
  /// returns the height of the font
  int GetFontHeight() const { return m_iHeight; }

  /// create font
  void Create( const vxIRenderingContext * pContext, const std::string & sTypeface, int4 iHeight, int4 iWeight, uint4 uItalic );

  /// free
  void Free();

  /// put string
  void PutString(const std::string & sString);

  /// put formatted string
  void PutFormattedString( const std::string & sString, const FontAlignEnum eAlignment, ...);

  /// put formatted string
  void PutFormattedString( const std::string & sString, const FontAlignEnum eAlignment, const Point<float4> & fRasterPos, ... );

  /// put formatted string
  void PutFormattedString( const std::string & sString, const FontAlignEnum eAlignment, const Point2D<int4> & RasterPos, const Triple<float4>fColor, ... );

// data
private:
  
  /// display list number of first character 
  GLuint m_uBase;
  
  /// Style to display the font 
  StyleEnum m_style;

  /// width of each character in pixels 
  int4 m_piWidths[ 256 ]; 
  
  /// height of characters 
  int4 m_iHeight;

  /// font created
  bool m_bFontCreated;
}; // GLFontGeorge


#endif // GLFontGeorge_h


// $Log: GLFontGeorge.h,v $
// Revision 1.2  2004/04/02 14:01:32  frank
// working on unifying the various rendering contexts
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.12  2003/10/09 01:12:07  geconomos
// Added get method for font height.
//
// Revision 3.11  2003/05/14 13:26:59  geconomos
// Coding standards.
//
// Revision 3.10  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.9  2003/01/28 17:02:05  michael
// fixed statistics of box annotation
//
// Revision 3.8  2003/01/17 00:34:23  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 3.7  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 3.6  2002/05/07 18:33:17  frank
// Removed unneccessary includes.
//
// Revision 3.5  2002/03/04 16:43:26  michael
// fomatting and coding guidelines
//
// Revision 3.4  2001/12/18 17:16:46  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.3  2001/11/21 22:18:58  michael
// added a style to the GLFontGeorge. PLAIN is as usually and SHADOWED
// is makeing a black shadow around it.
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
//    Rev 2.0   Sep 16 2001 23:49:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:34   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:18  ingmar
// new directory structure
//
// Revision 1.3  2001/07/02 15:43:40  geconomos
// Implemented GLFont class
//
// Revision 1.2  2001/07/02 14:54:05  huamin
// Changed the constructor
//
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/GLFontGeorge.h,v 1.2 2004/04/02 14:01:32 frank Exp $
// $Id: GLFontGeorge.h,v 1.2 2004/04/02 14:01:32 frank Exp $
