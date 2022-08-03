// $Id: vxRenderer3DOrientationCube.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: jenny hizver (mailto:jenny@viatronix.com)

/**
 *  This class defines the 3D orientation cube
 */

#ifndef vxRenderer3DOrientationCube_h
#define vxRenderer3DOrientationCube_h


// includes
#include "vxRenderer.h"


// class definition
class VX_VIEWER_DLL vxRenderer3DOrientationCube : public vxRenderer
{
// functions
public:

  /// constructor
  vxRenderer3DOrientationCube(vxEnvironment & environment);

  /// destructor
  virtual ~vxRenderer3DOrientationCube() {}
  
public:
  
  /// initialize
  virtual void Initialize();
  
  /// uninitialize
  virtual void Uninitialize();
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform rendering operations
  virtual void Render();
  
protected:

  /// are the cube textures already initialized?
  bool m_bTexturesInitialized;
  
  /// texture data for the orientation cube
  GLubyte m_uData[6][4096];
  
  /// texture IDs for orietation cube
  GLuint m_viTextureIDs[6];

private:

  /// copy constructor
  vxRenderer3DOrientationCube(const vxRenderer3DOrientationCube & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRenderer3DOrientationCube & operator=(const vxRenderer3DOrientationCube & other); // should only be public if really implemented!
  
  /// draws the orientation cube
  void DrawOrientationCube();
  
  /// loads the images from the resource once!
  static void LoadImages();

// data
private:

  /// cube images loaded
  static bool m_bCubeImageLoaded;

  /// pixel arrays
  static uint1 * m_piCubeImageRGBA[6];

  /// size in x
  static uint4 m_uSizeX;

  /// size in y
  static uint4 m_uSizeY;
}; // vxRenderer3DOrientationCube


#endif // vxRenderer3DOrientationCube_h


// Revision History:
// $Log: vxRenderer3DOrientationCube.h,v $
// Revision 1.2  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.1.1.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2003/12/17 18:02:55  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.18  2003/05/19 14:37:25  frank
// Removed unused return parameter
//
// Revision 1.17  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.16  2003/05/16 13:22:13  frank
// formatting
//
// Revision 1.15  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.14  2003/05/14 18:30:03  dongqing
// code review
//
// Revision 1.13  2003/05/14 16:10:20  michael
// code review
//
// Revision 1.12  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.11  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.10  2002/05/15 14:39:39  michael
// fixed orientation cube to modify the OpenGL state in Render() only.
// Removed PreRender() and POstRender()
//
// Revision 1.9  2002/05/09 20:54:16  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 1.8  2002/05/09 17:37:52  uday
// Added PreRender() and PostRender().
//
// Revision 1.7  2002/05/07 18:34:18  frank
// Removed unneccessary includes.
//
// Revision 1.6  2002/04/08 13:35:21  michael
// added missing privately non implemented copy constructor and assignment operator
//
// Revision 1.5  2002/03/22 00:51:40  geconomos
// fixed slow texture loading (George (70%), Michael (29%), Uday & Subba (1%))
//
// Revision 1.4  2002/03/21 20:09:45  jenny
// put comments
//
// Revision 1.3  2002/03/21 19:16:41  michael
// added log history lines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderer3DOrientationCube.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRenderer3DOrientationCube.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
