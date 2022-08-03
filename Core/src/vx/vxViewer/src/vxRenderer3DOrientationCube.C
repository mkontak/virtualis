// $Id: vxRenderer3DOrientationCube.C,v 1.2 2004/04/06 16:26:11 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: jenny hizver (jenny@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRenderer3DOrientationCube.h"
#include "vxEnvironment.h"
#include "GLEnv.h"
#include "GLFontGeorge.h"
#include "ResourceImage.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


// static member initialization
bool vxRenderer3DOrientationCube::m_bCubeImageLoaded = false;
uint4 vxRenderer3DOrientationCube::m_uSizeX = 0;
uint4 vxRenderer3DOrientationCube::m_uSizeY = 0;
uint1 * vxRenderer3DOrientationCube::m_piCubeImageRGBA[6] = { NULL, NULL, NULL, NULL, NULL, NULL };


/**
 * Constructor
 * @param environment
 */
vxRenderer3DOrientationCube::vxRenderer3DOrientationCube(vxEnvironment & environment)
:vxRenderer(environment)
//*******************************************************************
{
} // constructor


/**
 * Generate the textures for the orientation cube.
 */
void vxRenderer3DOrientationCube::LoadImages()
//*******************************************************************
{
  static char sNames[6][8] = { "CUBE_L\0", "CUBE_R\0", "CUBE_P\0", "CUBE_A\0", "CUBE_I\0", "CUBE_S\0"};

  for (int4 i(0); i<6; i++)
  {
    std::string sResource(sNames[i]);
    Image<PixelRGBA<uint1> > * pImage = ResourceImage::FromResource(sResource);
    if (pImage == NULL)
    {
      throw ex::FileNotFoundException(LogRec("Unable to load cube texture from resource", 
                                             "vxRenderer3DOrientationCube",
                                             "InitializeCubeTextures()"));
    }

    // WATCH OUT: dimensions need to be a power of two!!!!
    // ACTUALLY, needs to be 32x32, otherwise BREAKS!!!
    m_uSizeX = pImage->GetDim().m_x;
    m_uSizeY = pImage->GetDim().m_y;
    m_piCubeImageRGBA[i] = new uint1[4*m_uSizeX*m_uSizeY];

    // read data from image
    for (int4 t(0); t<m_uSizeY; t++)
    {
      for (int4 s(0); s<m_uSizeX; s++)
      {
        m_piCubeImageRGBA[i][4*(t*m_uSizeX+s)    ] = pImage->GetPixel(Point2D<uint2>(s,t)).R();
        m_piCubeImageRGBA[i][4*(t*m_uSizeX+s) + 1] = pImage->GetPixel(Point2D<uint2>(s,t)).G();
        m_piCubeImageRGBA[i][4*(t*m_uSizeX+s) + 2] = pImage->GetPixel(Point2D<uint2>(s,t)).B();
        m_piCubeImageRGBA[i][4*(t*m_uSizeX+s) + 3] = pImage->GetPixel(Point2D<uint2>(s,t)).A();
      }
    }

  }
  return;
} // LoadImages()


/**
 * Generate the textures for the orientation cube
 */
void vxRenderer3DOrientationCube::Initialize()
//*******************************************************************
{ 
  if (m_uSizeX == 0 || m_uSizeY == 0)
  {
    LoadImages();
  }
  
  if (m_uSizeX == m_uSizeY == 0)
  {
    throw ex::DataCorruptionException(LogRec("Something went badly wrong, static data is not loaded",
                                             "vxRenderer3DOrientationCube",
                                             "Initialize()"));
  }

  for (int i(0); i<6; i++)
  {
    glGenTextures(1, &m_viTextureIDs[i]);
    glBindTexture(GL_TEXTURE_2D, m_viTextureIDs[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_uSizeX, m_uSizeY, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, m_piCubeImageRGBA[i]);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_uSizeX, m_uSizeY, 
                      GL_RGBA, GL_UNSIGNED_BYTE, m_piCubeImageRGBA[i]);
  }

  return;
} // Initialize()


/**
 * Needed when switching views in the COM part.
 * Enables to uninitalize the textures and later
 * on generate new ones in the new context.
 */
void vxRenderer3DOrientationCube::Uninitialize()
//*******************************************************************
{
  for (int i(0); i<6; i++)
  {
    if (m_viTextureIDs[i] > 0)
    {
      glDeleteTextures(1, &m_viTextureIDs[i]);
    }
  }
} // Uninitialize()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRenderer3DOrientationCube::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(false);

  bRedrawNecessary |= GetEnvironment().IsModified(vxEnvironment::CAMERA);
  bRedrawNecessary |= GetEnvironment().IsModified(vxEnvironment::WIN_RECT);

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Renders the orientation cube
 */  
void vxRenderer3DOrientationCube::Render()
//*******************************************************************
{  
  // save OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();    
  
  // new settings
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  vxWinRect rect(GetEnvironment().GetWinRect());
  vxShareableObject<vxCamera> & camera(*GetEnvironment().GetCamera());
  float4 fAspectRatio((float4)rect.GetWindowSize().second/(float4)rect.GetWindowSize().first);

  if (camera.GetProjectionType() == vxCamera::PERSPECTIVE)
  {
    if (fAspectRatio < 1)
    {
      glFrustum(-0.0227/fAspectRatio, 0.0227/fAspectRatio, -0.0227, 0.0227, 0.1, 800);
    }
    else
    {
      glFrustum(-0.0227, 0.0227, -0.0227*fAspectRatio, 0.0227*fAspectRatio, 0.1, 800);
    }
  }
  else
  {
    if (fAspectRatio < 1)
    {
      glOrtho(-1.0/fAspectRatio, 1.0/fAspectRatio, -1.0, 1.0, 0.1, 800);
    }
    else
    {
      glOrtho(-1.0, 1.0, -1.0*fAspectRatio, 1.0*fAspectRatio, 0.1, 800);
    }
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  Point<float4> eye(camera.GetEye());
  Point<float4> ref(camera.GetViewReference());
  const Normal<float4> up(camera.GetViewUp());
  float4 fDist(5);
  Vector<float4> refToEye(eye-ref);
  refToEye.Normalize();
  ref = Point<float4>(0,0,0);
  eye.m_x = ref.m_x + fDist*refToEye.m_x;
  eye.m_y = ref.m_y + fDist*refToEye.m_y;
  eye.m_z = ref.m_z + fDist*refToEye.m_z;

  gluLookAt(eye.m_x, eye.m_y, eye.m_z, 
            ref.m_x, ref.m_y, ref.m_z, 
            up.X(),  up.Y(),  up.Z());
  
  // render cube
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  DrawOrientationCube();

  // restore OpenGL state
  glPopMatrix();
  glPopAttrib();    
} // Render()


/**
 * Draw the orientation cube for the 3D view. Uses six textures
 * which are mapped onto the cube faces to enhance orientation
 */
void vxRenderer3DOrientationCube::DrawOrientationCube()
//*******************************************************************
{
  float4 fCubeSize(0.6F);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Draw faces of the cube
  ///////////////////////////
  glBindTexture(GL_TEXTURE_2D, m_viTextureIDs[0]); // "L"
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(fCubeSize, -fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 1.0); glVertex3f(fCubeSize,  fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 0.0); glVertex3f(fCubeSize,  fCubeSize,  fCubeSize);
    glTexCoord2f(0.0, 0.0); glVertex3f(fCubeSize, -fCubeSize,  fCubeSize);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, m_viTextureIDs[1]); // "R"
  glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex3f(-fCubeSize, -fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 0.0); glVertex3f(-fCubeSize, -fCubeSize,  fCubeSize);
    glTexCoord2f(0.0, 0.0); glVertex3f(-fCubeSize,  fCubeSize,  fCubeSize);
    glTexCoord2f(0.0, 1.0); glVertex3f(-fCubeSize,  fCubeSize, -fCubeSize);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, m_viTextureIDs[3]); // "A"
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(-fCubeSize, -fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 1.0); glVertex3f( fCubeSize, -fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 0.0); glVertex3f( fCubeSize, -fCubeSize,  fCubeSize);
    glTexCoord2f(0.0, 0.0); glVertex3f(-fCubeSize, -fCubeSize,  fCubeSize);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, m_viTextureIDs[2]); // "P"
  glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex3f(-fCubeSize, fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 0.0); glVertex3f(-fCubeSize, fCubeSize,  fCubeSize);
    glTexCoord2f(0.0, 0.0); glVertex3f( fCubeSize, fCubeSize,  fCubeSize);
    glTexCoord2f(0.0, 1.0); glVertex3f( fCubeSize, fCubeSize, -fCubeSize);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, m_viTextureIDs[4]); // "I"
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-fCubeSize, -fCubeSize, -fCubeSize);
    glTexCoord2f(0.0, 1.0); glVertex3f(-fCubeSize,  fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 1.0); glVertex3f( fCubeSize,  fCubeSize, -fCubeSize);
    glTexCoord2f(1.0, 0.0); glVertex3f( fCubeSize, -fCubeSize, -fCubeSize);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, m_viTextureIDs[5]); // "S"
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(-fCubeSize, -fCubeSize, fCubeSize);
    glTexCoord2f(1.0, 1.0); glVertex3f( fCubeSize, -fCubeSize, fCubeSize);
    glTexCoord2f(1.0, 0.0); glVertex3f( fCubeSize,  fCubeSize, fCubeSize);
    glTexCoord2f(0.0, 0.0); glVertex3f(-fCubeSize,  fCubeSize, fCubeSize);
  glEnd();

  glDisable(GL_TEXTURE_2D);

  // only for debugging / orientation purposes to see where which axis goes
/*
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glLineWidth(3.0F);

  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(2*fCubeSize, 0.0, 0.0);
  glEnd();

  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 2*fCubeSize, 0.0);
  glEnd();

  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 2*fCubeSize);
  glEnd();
*/

  return;
} // DrawOrientationCube()


#undef FILE_REVISION


// Revision History:
// $Log: vxRenderer3DOrientationCube.C,v $
// Revision 1.2  2004/04/06 16:26:11  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.44  2004/01/30 01:21:33  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.43  2004/01/23 16:57:04  michael
// took out the rendering of the axises (debugging only!)
//
// Revision 1.42  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.41  2003/12/17 18:02:55  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.40  2003/08/07 12:26:47  michael
// added some debugging code to show axis of our coordinate system
//
// Revision 1.39  2003/05/19 14:37:25  frank
// Removed unused return parameter
//
// Revision 1.38  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.37  2003/05/16 13:22:13  frank
// formatting
//
// Revision 1.36  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.35  2003/05/15 20:09:40  frank
// removed compile warning
//
// Revision 1.34  2003/05/14 18:30:03  dongqing
// code review
//
// Revision 1.33  2003/05/14 16:12:22  michael
// fixed issue 3233
//
// Revision 1.32  2003/05/14 16:10:20  michael
// code review
//
// Revision 1.31  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.30  2003/01/22 22:00:02  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.29  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.28  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.27  2002/06/18 21:40:20  michael
// fixed compiler warning
//
// Revision 1.26  2002/06/18 20:57:19  michael
// added comment why not to delete static members :-)
//
// Revision 1.25  2002/06/17 23:30:30  uday
// Render(), DrawOrientationCube() update to reflect panning in 3D.
//
// Revision 1.24  2002/06/14 21:29:17  manju
// Freeing memory in destructor.
//
// Revision 1.23  2002/05/24 14:43:29  ingmar
// fixed openGL state push/pop
//
// Revision 1.22  2002/05/15 14:43:01  michael
// fixed orientation cube to modify the OpenGL state in Render() only.
// Removed PreRender() and POstRender()
//
// Revision 1.21  2002/05/15 14:39:39  michael
// fixed orientation cube to modify the OpenGL state in Render() only.
// Removed PreRender() and POstRender()
//
// Revision 1.20  2002/05/09 21:59:38  jaddonisio
// Changed constant to float for compiler warning.
//
// Revision 1.19  2002/05/09 20:54:16  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 1.18  2002/05/09 17:37:52  uday
// Added PreRender() and PostRender().
//
// Revision 1.17  2002/05/07 18:34:18  frank
// Removed unneccessary includes.
//
// Revision 1.16  2002/04/30 21:33:19  jenny
// vxBlockVolume stuff
//
// Revision 1.15  2002/04/26 13:39:56  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.14  2002/04/22 21:51:20  uday
// 1> Removed camera set up.
// 2> Orientation cube size update.
//
// Revision 1.13  2002/04/08 13:36:41  michael
// cosmetics
//
// Revision 1.12  2002/04/01 16:37:09  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.11  2002/03/28 23:16:50  jaddonisio
// Enable GL_DEPTH_TEST.
//
// Revision 1.10  2002/03/26 18:43:00  dmitry
// Renamed GetContext to GetContextPtr.
//
// Revision 1.9  2002/03/22 00:51:40  geconomos
// fixed slow texture loading (George (70%), Michael (29%), Uday & Subba (1%))
//
// Revision 1.8  2002/03/21 20:10:13  jenny
// put comments
//
// Revision 1.7  2002/03/21 19:16:41  michael
// added log history lines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderer3DOrientationCube.C,v 1.2 2004/04/06 16:26:11 frank Exp $
// $Id: vxRenderer3DOrientationCube.C,v 1.2 2004/04/06 16:26:11 frank Exp $
