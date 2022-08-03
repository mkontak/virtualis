// $Id: GLFormat.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos( geconomos@viatronix.com )

/*
  GLFormat specifies the display format of a rendering context. 
 
  DEFAULT SETTINGS:
 
     Double buffer: Enabled. 
     Depth buffer: Enabled. 
     RGBA: Enabled (i.e. color index disabled). 
     Alpha channel: Disabled. 
     Accumulator buffer: Disabled. 
     Stencil buffer: Disabled. 
     Stereo buffer: Disabled. 
     Direct rendering: Enabled. 
*/

#ifndef GLFormat_h
#define GLFormat_h


// includes
#include "GLPaintDevice.h"


// class declaration
class VX_VIEWER_DLL GLFormat
{
// enums
private:
  
  enum FormatOptionEnum
  {
    DOUBLEBUFFER		  = 0X0001,
    DEPTHBUFFER		    = 0X0002,
    RGBA			        = 0X0004,
    ALPHACHANNEL		  = 0X0008,
    ACCUMBUFFER		    = 0X0010,
    STENCILBUFFER		  = 0X0020,
    STEREOBUFFERS		  = 0X0040,
    DIRECTRENDERING	  = 0X0080,
    SINGLEBUFFER      = DOUBLEBUFFER    << 16,
    NODEPTHBUFFER     = DEPTHBUFFER     << 16,
    COLORINDEX        = RGBA            << 16,
    NOALPHACHANNEL    = ALPHACHANNEL    << 16,
    NOACCUMBUFFER     = ACCUMBUFFER     << 16,
    NOSTENCILBUFFER   = STENCILBUFFER   << 16,
    NOSTEREOBUFFERS   = STEREOBUFFERS   << 16,
    INDIRECTRENDERING = DIRECTRENDERING << 16,
  };

// member functions
public:

  /// default constructor  
  GLFormat();
    
  /// returns TRUE if double buffering is enabled, otherwise FALSE. Double buffering is enabled by default. 
  bool GetDoubleBuffer() const;
  
  /// sets double buffering if bEnable is TRUE or single buffering if bEnable is FALSE. 
  void SetDoubleBuffer( const bool bEnable );
  
  /// returns TRUE if the depth buffer is enabled, otherwise FALSE. The depth buffer is enabled by default. 
  bool GetDepthBuffer() const;
  
  /// enables the depth buffer if bEnable is TRUE, or disables it if bEnable is FALSE. 
  void SetDepthBuffer( const bool bEnable );
  
  /// returns TRUE if the alpha channel of the framebuffer is enabled, otherwise FALSE. The alpha channel is disabled by default. 
  bool GetAlpha() const;
  
  /// enables the alpha channel of the framebuffer if enable is TRUE, or disables it if enable is FALSE. 
  void SetAlpha( const bool bEnable );
  
  /// returns TRUE if the accumulation buffer is enabled, otherwise FALSE. The accumulation buffer is disabled by default. 
  bool GetAccumulationBuffer() const;
  
  /// enables the accumulation buffer if enable is TRUE, or disables it if enable is FALSE. 
  void SetAccumulationBuffer( const bool bEnable );
  
  /// returns TRUE if direct rendering is enabled, otherwise FALSE. Direct rendering is enabled by default. 
  bool GetDirectRendering() const;
  
  /// enables direct rendering if enable is TRUE, or disables it if enable is FALSE. 
  void SetDirectRendering( const bool bEnable );
  
  /// returns TRUE if the stencil buffer is enabled, otherwise FALSE. The stencil buffer is disabled by default. 
  bool GetStencilBuffer() const;
  
  /// enables the stencil buffer if enable is TRUE, or disables it if enable is FALSE. 
  void SetStencilBuffer( const bool bEnable );
  
  /// returns TRUE if stereo buffering is enabled, otherwise FALSE. Stereo buffering is disabled by default. 
  bool GetStereoBuffer() const;
  
  /// enables stereo buffering if enable is TRUE, or disables it if enable is FALSE. 
  void SetStereoBuffer( const bool bEnable );
  
private:
  
  /// sets the specified option 
  void SetOption( const FormatOptionEnum eOption );
  
  /// returns TRUE if format option opt is set, otherwise FALSE. 
  bool TestOption( const FormatOptionEnum eOption ) const;

// member variables
private:
  
  /// current options  
  uint4 m_uOptions;
}; // GLFormat


#endif // GLFormat_h


// $Log: GLFormat.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/14 13:26:59  geconomos
// Coding standards.
//
// Revision 3.4  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.3  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 3.2  2002/03/04 16:43:26  michael
// fomatting and coding guidelines
//
// Revision 3.1  2001/12/18 17:16:46  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
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
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.2  2001/06/22 18:22:07  geconomos
// Initial OpenGL implementation.
//
// Revision 1.1  2001/06/18 21:52:14  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/GLFormat.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: GLFormat.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
