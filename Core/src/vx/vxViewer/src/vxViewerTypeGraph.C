// $Id: vxViewerTypeGraph.C,v 1.2 2007/08/07 14:57:46 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Romy Sreedharan, (mailto:romy@viatronix.com)


// includes
#include "stdafx.h"
#include "vxViewerTypeGraph.h"
#include "vxEnvironment.h"
#include "vxWinRect.h"

#include "vxRenderingNodePluginPlanetracer.h"




// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
vxViewerTypeGraph::vxViewerTypeGraph(vxEnvironment & environment)
:vxViewerType(environment)
//*******************************************************************
{
}


/**
 * Initializes the view
 */
void vxViewerTypeGraph::InitializeView()
//*******************************************************************
{
  
} // InitializeView()


/**
 * This method is responsable to initialize the OpenGL viewing
 */
void vxViewerTypeGraph::SetupView()
//*******************************************************************
{
  vxWinRect winRect(GetEnvironment().GetWinRect());
  vxCamera camera(*GetEnvironment().GetCamera());
  const Point<float4> eye(camera.GetEye());
  const Point<float4> ref(camera.GetViewReference());
  const Normal<float4> up(camera.GetViewUp());

  Point3Df center(GetEnvironment().GetZoomFactor()->GetCenter());

  // set the viewport
  /////////////////////
  glViewport(winRect.GetReferencePoint().first, 
             winRect.GetReferencePoint().second, 
             winRect.GetViewportSize().first, 
             winRect.GetViewportSize().second);

  // just setup the view using the camera
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();     
  glOrtho(GetLeftPlane()  , GetRightPlane(),
          GetBottomPlane(), GetTopPlane(), 
          //camera.GetNearPlane()  , camera.GetFarPlane());
          0, 10000); // THIS is a HACK for RSNA 2002
 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(center.m_x + eye.m_x - ref.m_x, 
            center.m_y + eye.m_y - ref.m_y, 
            center.m_z + eye.m_z - ref.m_z, 
            center.m_x, center.m_y, center.m_z, 
            up.X(),  up.Y(),  up.Z());

  return;
} // SetupView()


/**
 * Initialize view/objects based on annotation
 * @param annotation
 * @return success
 */
//bool vxViewerTypeImage::InitializeFrom(vxAnnotation & annotation)
////*******************************************************************
//{
//  return false;
//} // InitializeFrom()


/**
 * Handles modified events.
 * @param type the type of the modified object
 * @param info the detailed information
 */
void vxViewerTypeGraph::OnModified(const type_info & type, 
                                      const vxModifiedInfo & info)
//*******************************************************************
{
} // OnModified()



/**
 * Is this annotation type supported by this viewer type?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerTypeGraph::IsAnnotationSupported(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
  return false;
} // IsAnnotationSupported()


/**
 * Is the handlesSet visible and should be drawn?
 * @param handlesSet
 * @return succesful
 */
bool vxViewerTypeGraph::IsVisible(const vxHandlesObj & handlesObject) const
//*******************************************************************
{
 return true;
} // IsVisible()


/**
 * Is this position visible in the current view?
 * @param position the query position in world coordinates
 * @return true if visible
 */
bool vxViewerTypeGraph::IsVisible(const Point3Df & point) const
//*******************************************************************
{
 return true;
} // IsVisible()


/**
 * Returns if the viewer supports dropping of the specified type.
 *
 * @param eSourceType drop type
 * @return true if element can be fropped, false otherwise
 */
bool vxViewerTypeGraph::CanDragDrop(const DragDropEnum eSourceType)
//*******************************************************************
{
  return false;
} // CanDragDrop()

/**
 * Creates an instance of a rendering engine plugin for this particular viewer type
 **************** THIS ONE IS A DUMMY ONE TO AVOID VIZ GRAPH CRASH *****************
 //TODO: Remove this dummy PluginEngine once the Viz graph crash is fixed
 * @return    rendering engine plugin instance
 */
vxRenderingNodePlugin * vxViewerTypeGraph::CreateRenderingEnginePlugin()
{
  return new vxRenderingNodePluginPlanetracer( GetEnvironment() );
}//CreateRenderingEnginePlugin


// Revision History:
// $Log: vxViewerTypeGraph.C,v $
// Revision 1.2  2007/08/07 14:57:46  romy
// added a dummy PluginNode to avoid VizualizationGraph initialization crash -
//
// Revision 1.1  2006/11/02 03:36:28  romy
// added new viewer type
//
// Revision 1.1  2006/11/02 03:31:33  romy
// added new viewer type
//
// Revision 1.1  2006/10/31 15:41:31  romy
// test version
//
// $Id: vxViewerTypeGraph.C,v 1.2 2007/08/07 14:57:46 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeGraph.C,v 1.2 2007/08/07 14:57:46 romy Exp $
