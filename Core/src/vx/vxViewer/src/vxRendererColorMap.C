// $Id: vxRendererColorMap.C,v 1.3 2005/05/24 13:50:02 michael Exp $
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
#include "vxRendererColormap.h"
#include "vxDataColormapEditor.h"
#include "vxEnvironment.h"
#include "vxRendererUtils.h"



// define declarations
#define FILE_REVISION "$Revision: 1.3 $"



/**
 * Constructor
 * @param reference to the vxEnvironment object
 */
vxRendererColormap::vxRendererColormap(vxEnvironment & environment):
vxRenderer(environment)
//*******************************************************************
{
} // vxRendererColormap()


/**
 * This event handler is called when the OpenGL drawing should take place.
 */
void vxRendererColormap::Render()
//*******************************************************************
{
  vxDataColormapEditor * pData = dynamic_cast<vxDataColormapEditor *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) return;

  const std::vector<Point2D<ZeroOne> > & redList(GetEnvironment().GetColormap()->GetRedList());
  const std::vector<Point2D<ZeroOne> > & greenList(GetEnvironment().GetColormap()->GetGreenList());
  const std::vector<Point2D<ZeroOne> > & blueList(GetEnvironment().GetColormap()->GetBlueList());

  glDrawBuffer(GL_BACK);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_REGISTER_COMBINERS_NV);
  
  // View and Projection on Transfer Function Editor 2D
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-0.01F, 1.0F, 0.0F, 1.0F);
  glViewport(0, 0, GetEnvironment().GetWinRect().GetViewportWidth(), GetEnvironment().GetWinRect().GetViewportHeight());
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Draw color gradient
  glShadeModel(GL_SMOOTH);
  glBegin(GL_QUAD_STRIP);
  {
    for (int i(0); i<redList.size(); i++)
    {
      glColor3f(redList[i].m_y, greenList[i].m_y, blueList[i].m_y);
      glVertex2f(redList[i].m_x, 0.0F);
      glVertex2f(redList[i].m_x, 1.0F);
    }
  }
  glEnd();
  
  // draw control points
  glEnable(GL_POINT_SMOOTH); // doesn't do much
  for (int i(0); i<redList.size(); i++)
  {
    // draw selected point with green highlight
    const PixelRGBA<float4> color(redList[i].m_y, greenList[i].m_y, blueList[i].m_y, 1.0F);
    if (pData->GetSelectedColorPointIndex() == i)
    {
      vxRendererUtils::GLDrawPointWithBorder2f(Point2D<float4>(redList[i].m_x, 0.5F), color,
                                               vxRendererUtils::InvertColor(color), 5.0F, 2.0F);
    }
    else
    {
      vxRendererUtils::GLDrawPoint2f(Point2D<float4>(redList[i].m_x, 0.5F), 
                                     vxRendererUtils::InvertColor(color), 5.0F);
    }
  }
} // Render()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererColorMap.C,v $
// Revision 1.3  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.2  2004/04/06 16:26:30  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.33  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.32  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.31  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.30  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.29  2003/05/15 11:52:35  michael
// code review
//
// Revision 1.28  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.27  2003/05/08 16:39:06  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.26  2003/03/28 16:45:01  geconomos
// Commented out Correspondence lines.
//
// Revision 1.25  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.24  2003/01/21 22:14:39  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.23  2003/01/13 19:29:15  ingmar
// Point2D is no longer derived from the 3D Point
// serialization and 2D rendering are updated accordingly
//
// Revision 1.22  2002/11/05 01:31:33  michael
// readded version with sucking name :-(
//
// Revision 1.20  2002/10/14 18:43:20  michael
// fixed type bug that ocurred during the const-transition of vxCamera and vxColormap
//
// Revision 1.19  2002/10/14 15:31:27  michael
// accounting for const methods in vxColormap
//
// Revision 1.18  2002/10/04 18:11:07  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.17  2002/10/03 21:22:32  michael
// Added old (not renamed) renderer ... because of hash name problems :-(
//
// Revision 1.15  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.14  2002/09/26 01:18:46  michael
// Use utility methods to draw points and surrounding outline
//
// Revision 1.13  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.12  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.11  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.10  2002/09/16 20:00:22  dmitry
// New vxID (s) used.
//
// Revision 1.9  2002/09/16 16:51:19  frank
// Added correspondence lines
//
// Revision 1.8  2002/09/11 20:43:36  frank
// Working on transfer function event mechanism
//
// Revision 1.7  2002/09/10 17:19:52  frank
// Removed debug
//
// Revision 1.6  2002/09/10 17:18:47  frank
// Added colormap point rendering and manipulation.
//
// Revision 1.5  2002/09/10 15:36:35  michael
// removed redundant includes and added some formatting
//
// Revision 1.4  2002/09/09 22:28:41  frank
// Improved selection and creation of colormap points.
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererColorMap.C,v 1.3 2005/05/24 13:50:02 michael Exp $
// $Id: vxRendererColorMap.C,v 1.3 2005/05/24 13:50:02 michael Exp $
