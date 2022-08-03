// $Id: vxRendererAlphaActiveRange.C,v 1.4 2005/05/24 13:50:02 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (mailto:ingmar@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererAlphaActiveRange.h"
#include "vxDataColormapEditor.h"
#include "vxEnvironment.h"
#include "vxControlPoint.h"
#include "BaseHistogram.h"
#include "vxIntensityConverter.h"
#include "vxRendererUtils.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 * @param environment Reference to the vxEnvironment object
 */
vxRendererAlphaActiveRange::vxRendererAlphaActiveRange(vxEnvironment & environment)
:vxRendererSoftware(environment)
//*******************************************************************
{
}


/**
 * This virtual function is called when Viewer is initially initialized
 */
void vxRendererAlphaActiveRange::Initialize()
//*******************************************************************
{ 
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  GenerateHistogramTexture();
} // Initialize()


/**
 * Generates the texture for the histogram and since the display area
 * is assumed to be small, we make a texture of fixed size 512x64.
 */
void vxRendererAlphaActiveRange::GenerateHistogramTexture()
//*******************************************************************
{
  // set image to size
  const uint2 uSizeX(2048), uSizeY(64);
  GetImage().Resize(Triple<uint2>(uSizeX, uSizeY, 1));

  BaseHistogram * pHistogram = static_cast<BaseHistogram *>(GetEnvironment().GetHistogram());
  PixelRGBA<uint1> color(190, 190, 190, 255);

  vxRendererUtils::CreateHistogramImage(pHistogram, GetImage(), 
                                        0, vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader()),
                                        color);

  // call base class to make this image a texture
  CreateTexture();
} // GenerateHistogramTexture()


/**
 * Draws the histogram texture
 */
void vxRendererAlphaActiveRange::DrawTexture()
//*******************************************************************
{
  // mix/max value of range
  const vxWindowLevel & wl(GetEnvironment().GetColormap()->GetWindowLevel());
  const float4 fMinX(wl.GetLevel() - wl.GetWindow() / 2.0F);
  const float4 fMaxX(wl.GetLevel() + wl.GetWindow() / 2.0F);

  GLuint id(GetTextureTile().GetDensityTextureId());

  glBindTexture(GL_TEXTURE_2D, id);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // GL draw textured tile:
  glColor3f(1, 1, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_TRIANGLE_FAN);

  glTexCoord2f(fMinX, 0.0F); glVertex2f(0.0F, 0.0F);
  glTexCoord2f(fMinX, 1.0F); glVertex2f(0.0F, 1.0F);
  glTexCoord2f(fMaxX, 1.0F); glVertex2f(1.0F, 1.0F);
  glTexCoord2f(fMaxX, 0.0F); glVertex2f(1.0F, 0.0F);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  return;
} // DrawTexture()


/**
 * Draws the window/level
 */
void vxRendererAlphaActiveRange::DrawWindowLevel()
//*******************************************************************
{
  vxDataColormapEditor * pData = dynamic_cast<vxDataColormapEditor *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return;
  }

  const vector<Point2D<ZeroOne> > & alphaList(GetEnvironment().GetColormap()->GetAlphaList());

  // draw piecewise linear alpha curve
  glLineWidth(1.5F);
  glColor3f(1.0F, 1.0F, 1.0F);
  glBegin(GL_LINE_STRIP);

  for (int i(0); i<alphaList.size(); ++i)
  {
    glVertex2f(alphaList[i].m_x, alphaList[i].m_y);
  }
  glEnd();

  // draw control points
  glPointSize(5.0F);
  glBegin(GL_POINTS);

  for (int j(0); j<alphaList.size(); ++j)
  {
    if (pData->GetSelectedAlphaPointIndex() == j)
      glColor3f(0.0F, 1.0F, 0.0F);
    else
      glColor3f(1.0F, 1.0F, 1.0F);

    glVertex2f(alphaList[j].m_x, alphaList[j].m_y);
  }
  glEnd();

  return;
} // DrawWindowLevel()


/**
 * Perform any actions before the actual rendering
 */
int4 vxRendererAlphaActiveRange::PreRender()
//*******************************************************************
{
  bool bRenderingNecessary(false);
  bRenderingNecessary |= GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY);
  bRenderingNecessary |= GetEnvironment().IsModified(vxEnvironment::VOLUME_INTENSITY_HISTOGRAM);
  bRenderingNecessary |= GetEnvironment().IsModified(vxEnvironment::WIN_RECT);

  SetRenderingNecessary(bRenderingNecessary);

  return 1;
} // PreRender()


/**
 * This event handler is called when the OpenGL drawing should take place. 
 */
void vxRendererAlphaActiveRange::Render()
//*******************************************************************
{
  if (GetRenderingNecessary() == true)
  {
    GenerateHistogramTexture();
  }

  // save OpenGL parameters and set local parameters
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDrawBuffer(GL_BACK);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_REGISTER_COMBINERS_NV);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POINT_SMOOTH);

  // setup model and projection matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0F, 1.0F, 0.0F, 1.0F);
  glViewport(0, 0, GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight());

  glDisable(GL_DEPTH_TEST);

  // draw histogram
  DrawTexture();

  // draw the window/level
  DrawWindowLevel();

  // restore OpenGL parameters
  glPopAttrib();
} // Render()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererAlphaActiveRange.C,v $
// Revision 1.4  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.3  2004/11/03 17:15:07  geconomos
// updated PreRender method to be interetsed in when the volume changes
//
// Revision 1.2  2004/04/06 16:26:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.29  2004/01/30 01:21:33  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.28  2003/08/20 14:22:15  michael
// added "mailto:"
//
// Revision 1.27  2003/08/06 13:26:15  michael
// took out log messages
//
// Revision 1.26  2003/07/30 19:47:42  michael
// removed check from Initialize() and added texture initialization (black image)
//
// Revision 1.25  2003/07/30 18:51:27  michael
// Render(): took out logging of timing
//
// Revision 1.24  2003/07/30 18:49:14  michael
// replaced active rendering from histogram to a texture based rendering of the
// histogram where the texture is only updated when the histogram is modified.
//
// Revision 1.23  2003/07/29 11:05:08  michael
// replaced redundant code by one centralized piece that draws the histogram.
// However, it is still actively drawn but should use a texture that is only updated
// if the BaseHistogram has changed ...
//
// Revision 1.22  2003/06/09 17:56:21  michael
// added missing inludes
//
// Revision 1.21  2003/06/09 12:30:22  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.20  2003/06/04 18:33:54  michael
// accounting for new vxIntensityConverter parameters (header, not modality)
//
// Revision 1.19  2003/05/19 14:37:25  frank
// Removed unused return parameter
//
// Revision 1.18  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.17  2003/05/16 13:22:13  frank
// formatting
//
// Revision 1.16  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.15  2003/05/14 16:10:20  michael
// code review
//
// Revision 1.14  2003/03/28 16:45:01  geconomos
// Commented out Correspondence lines.
//
// Revision 1.13  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.12  2002/10/14 15:31:28  michael
// accounting for const methods in vxColormap
//
// Revision 1.11  2002/10/04 18:11:07  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.10  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.9  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.8  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.7  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.6  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.5  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.4  2002/09/16 16:51:19  frank
// Added correspondence lines
//
// Revision 1.3  2002/09/10 23:05:08  frank
// Made alpha curve manipulation possible.
//
// Revision 1.2  2002/09/10 15:57:09  michael
// removed redundant includes and added some formatting
//
// Revision 1.1  2002/09/06 23:06:17  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererAlphaActiveRange.C,v 1.4 2005/05/24 13:50:02 michael Exp $
// $Id: vxRendererAlphaActiveRange.C,v 1.4 2005/05/24 13:50:02 michael Exp $
