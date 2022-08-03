// $Id: vxRendererColorChooser.C,v 1.3 2005/05/24 13:50:02 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererColorChooser.h"
#include "vxDataColormapEditor.h"
#include "vxEnvironment.h"
#include "ResourceImage.h"
#include "vxRendererUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 * @param reference to the vxEnvironment object
 */
vxRendererColorChooser::vxRendererColorChooser(vxEnvironment & environment): 
vxRenderer(environment)
//*******************************************************************
{
} // constructor


/**
 * This virtual function is called when Viewer is initially initialized.
 */
void vxRendererColorChooser::Initialize()
//*******************************************************************
{ 
  Image<PixelRGBA<uint1> > * pImage = ResourceImage::FromResource("COLORMAP");

  // WATCH OUT: dimensions need to be a power of two!!!!
  // ACTUALLY, needs to be 32x32, otherwise it would BREAK!!!
  uint2 uSizeX(pImage->GetDim().m_x);
  uint2 uSizeY(pImage->GetDim().m_y);
  
  GLubyte * pData = new GLubyte[4*uSizeY*uSizeX];
  
  // read data from image
  for (uint4 t(0); t<uSizeY; t++)
  {
    for (uint4 s(0); s<uSizeX; s++)
    {
      pData[4*(t*uSizeX+s)    ] = pImage->GetPixel(Point2D<uint2>(s,t)).R();
      pData[4*(t*uSizeX+s) + 1] = pImage->GetPixel(Point2D<uint2>(s,t)).G();
      pData[4*(t*uSizeX+s) + 2] = pImage->GetPixel(Point2D<uint2>(s,t)).B();
      pData[4*(t*uSizeX+s) + 3] = 255;
    }
  }
  
  glGenTextures(1, &m_iTextureID);
  glBindTexture(GL_TEXTURE_2D, m_iTextureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, uSizeX, uSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, uSizeX, uSizeY, GL_RGBA, GL_UNSIGNED_BYTE, pData);
  
  delete[] pData;
} // Initialize()


/**
 * This event handler is called when the OpenGL drawing should take place. 
 */
void vxRendererColorChooser::Render()
{
  vxDataColormapEditor * pData = dynamic_cast<vxDataColormapEditor *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) return;

  // setup matrices
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0F, 1.0F, 0.0F, 1.0F);
  glViewport(0, 0, GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // set up environment
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_REGISTER_COMBINERS_NV);
  glDisable(GL_DEPTH_TEST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  
  // draw texture
  glBindTexture(GL_TEXTURE_2D, m_iTextureID);
  glBegin(GL_QUADS);
  {
    glTexCoord2i(0, 1); glVertex2i(0, 0);
    glTexCoord2i(1, 1); glVertex2i(1, 0);
    glTexCoord2i(1, 0); glVertex2i(1, 1);
    glTexCoord2i(0, 0); glVertex2i(0, 1);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);

  // draw the control point if one is selected
  const int4 iSelectedPoint(pData->GetSelectedColorPointIndex());

  if (iSelectedPoint != -1)
  {
    const PixelRGBA<float4> color(GetEnvironment().GetColormap()->GetRedList  ()[iSelectedPoint].m_y,
                                  GetEnvironment().GetColormap()->GetGreenList()[iSelectedPoint].m_y,
                                  GetEnvironment().GetColormap()->GetBlueList ()[iSelectedPoint].m_y,
                                  1.0F);
    vxRendererUtils::GLDrawPointWithBorder2f(pData->GetColorLocation(), color, 
                                             vxRendererUtils::InvertColor(color), 5.0F, 2.0F);
  } // if colormap point is selected
} // Render()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererColorChooser.C,v $
// Revision 1.3  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.2  2004/04/06 16:26:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.27  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.26  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.25  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.24  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.23  2003/05/15 11:52:35  michael
// code review
//
// Revision 1.22  2003/05/14 18:30:04  dongqing
// code review
//
// Revision 1.21  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.20  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.19  2003/04/07 18:59:31  michael
// cosmetics
//
// Revision 1.18  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.17  2003/01/22 22:05:49  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.16  2002/12/20 15:57:44  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.15  2002/10/14 18:43:20  michael
// fixed type bug that ocurred during the const-transition of vxCamera and vxColormap
//
// Revision 1.14  2002/10/04 18:11:07  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.13  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.12  2002/09/26 01:18:45  michael
// Use utility methods to draw points and surrounding outline
//
// Revision 1.11  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.10  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.9  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.8  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.7  2002/09/11 20:43:36  frank
// Working on transfer function event mechanism
//
// Revision 1.6  2002/09/11 17:03:06  frank
// Flipped image
//
// Revision 1.5  2002/09/10 17:19:52  frank
// Removed debug
//
// Revision 1.4  2002/09/10 17:18:46  frank
// Added colormap point rendering and manipulation.
//
// Revision 1.3  2002/09/10 15:36:35  michael
// removed redundant includes and added some formatting
//
// Revision 1.2  2002/09/09 22:11:25  frank
// Working on color chooser
//
// Revision 1.1  2002/09/06 23:06:17  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererColorChooser.C,v 1.3 2005/05/24 13:50:02 michael Exp $
// $Id: vxRendererColorChooser.C,v 1.3 2005/05/24 13:50:02 michael Exp $
