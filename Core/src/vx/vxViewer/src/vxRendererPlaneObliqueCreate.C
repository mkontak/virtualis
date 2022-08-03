// $Id: vxRendererPlaneObliqueCreate.C,v 1.2 2005/05/24 13:50:02 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererPlaneObliqueCreate.h"
#include "vxEnvironment.h"
#include "vxDataPlaneObliqueCreate.h"


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 * @param environment the environment
 */
vxRendererPlaneObliqueCreate::vxRendererPlaneObliqueCreate(vxEnvironment & environment)
:vxRenderer(environment)
//*******************************************************************
{
} // Constructor


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererPlaneObliqueCreate::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(false);

  bool bChangedDataPlaneObliqueCreate(GetEnvironment().IsModified(vxIDs::DataPlaneObliqueCreate));

  bRedrawNecessary |= bChangedDataPlaneObliqueCreate;
  bRedrawNecessary |= GetEnvironment().IsModified(vxEnvironment::WIN_RECT);
  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Any preparation necessary?
 */
int4 vxRendererPlaneObliqueCreate::PreRender()
//*******************************************************************
{
  vxShareableObject<vxDataPlaneObliqueCreate> * pTmp  = dynamic_cast<vxShareableObject<vxDataPlaneObliqueCreate> *>(GetEnvironment().GetElement(vxIDs::DataPlaneObliqueCreate));
  vxDataPlaneObliqueCreate * pData = static_cast<vxDataPlaneObliqueCreate *>(pTmp);

  if (pData == NULL || pData->m_uNumPointsSet == 0 || !(pData->m_pManipulationEnvironment == &GetEnvironment() || pData->m_pEnvironment == &GetEnvironment()))
  {
    return false;
  }

  for (int4 i(0); i<2; i++)
  {
    pData->m_vHandles[i].PreRender(GetEnvironment());
  }

  return true;
} // PreRender()


/**
 * Render the vessel using OpenGL
 */
void vxRendererPlaneObliqueCreate::Render()
//*******************************************************************
{
  vxShareableObject<vxDataPlaneObliqueCreate> * pTmp  = dynamic_cast<vxShareableObject<vxDataPlaneObliqueCreate> *>(GetEnvironment().GetElement(vxIDs::DataPlaneObliqueCreate));
  vxDataPlaneObliqueCreate * pData = static_cast<vxDataPlaneObliqueCreate *>(pTmp);

  if (pData == NULL || pData->m_uNumPointsSet == 0 || !(pData->m_pManipulationEnvironment == &GetEnvironment() || pData->m_pEnvironment == &GetEnvironment()))
  {
    return;
  }

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // set new OpenGL state
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // in the viewer we are manipulating, draw the line stippled
  if (pData->m_pEnvironment == &GetEnvironment())
  {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x0303);
  }

  for (int i(0); i<2; i++)
  {
    if (i == 0)
    {
      glColor4f(0.0F, 0.0F, 0.0F, 255.0);
      glLineWidth(5.0);
    }
    else
    {
      glColor4f(0.0F, 255.0F, 255.0F, 255.0);   
      glLineWidth(3.0);
    }

    glBegin(GL_LINES);
    glVertex3f(pData->m_vHandles[0].GetPosition()[0], pData->m_vHandles[0].GetPosition()[1], pData->m_vHandles[0].GetPosition()[2]);
    glVertex3f(pData->m_vHandles[1].GetPosition()[0], pData->m_vHandles[1].GetPosition()[1], pData->m_vHandles[1].GetPosition()[2]);
    glEnd();
  }

  // restore the OpenGL state
  glPopAttrib();

  // save it again for the handles
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // render the handles
  for (int4 i(0); i<2; i++)
  {
    pData->m_vHandles[i].Render(GetEnvironment());
  }

  // restore the OpenGL state
  glPopAttrib();

  return;
} // Render()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererPlaneObliqueCreate.C,v $
// Revision 1.2  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/01/30 01:21:34  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.7  2004/01/23 04:00:28  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.6  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.5  2003/09/23 22:09:31  michael
// added push and pop of the attributes to ensure handle not being drawn stippled
//
// Revision 1.4  2003/09/22 21:12:34  michael
// changed points to two handles that can be rendered
//
// Revision 1.3  2003/09/22 15:55:43  michael
// added "mailto:"
//
// Revision 1.2  2003/08/21 13:11:03  michael
// change line stiple pattern to be a dotted line ...
//
// Revision 1.1  2003/08/19 15:22:33  michael
// added inital verions of renderer for creating an oblique plane
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererPlaneObliqueCreate.C,v 1.2 2005/05/24 13:50:02 michael Exp $
// $Id: vxRendererPlaneObliqueCreate.C,v 1.2 2005/05/24 13:50:02 michael Exp $
