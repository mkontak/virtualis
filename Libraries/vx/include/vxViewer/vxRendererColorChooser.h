// $Id: vxRendererColorChooser.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)

/**
 * Define the color chooser
 */

#ifndef vxRendererColorChooser_h
#define vxRendererColorChooser_h


// includes
#include "vxRenderer.h"
#include "Point.h"


// forward declarations
class vxControlPoint;



// class definition
class VX_VIEWER_DLL vxRendererColorChooser: public vxRenderer
{
// functions
public:

  /// constructor
  vxRendererColorChooser(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererColorChooser() {};

  /// this virtual function is called when this object is initialized
  virtual void Initialize();

  /// event handler is called when the OpenGL drawing should take place
  virtual void Render();

// data
private:
  
  /// OpenGL texture ID
  GLuint m_iTextureID;
  
  /// image needed to search for new locations within
  Image<PixelRGBA<uint1> > * m_pImage;
}; // vxRendererColorChooser


#endif // vxRendererColorChooser_h


// Revision History:
// $Log: vxRendererColorChooser.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.13  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.12  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.11  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.10  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.9  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.8  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.7  2002/09/26 01:18:46  michael
// Use utility methods to draw points and surrounding outline
//
// Revision 1.6  2002/09/10 20:37:56  michael
// Fixed "foreward" --> "forward"
//
// Revision 1.5  2002/09/10 17:18:47  frank
// Added colormap point rendering and manipulation.
//
// Revision 1.4  2002/09/10 15:36:35  michael
// removed redundant includes and added some formatting
//
// Revision 1.3  2002/09/09 22:11:25  frank
// Working on color chooser
//
// Revision 1.2  2002/09/09 21:31:58  frank
// Working on color map editor
//
// Revision 1.1  2002/09/06 23:06:17  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererColorChooser.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxRendererColorChooser.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
