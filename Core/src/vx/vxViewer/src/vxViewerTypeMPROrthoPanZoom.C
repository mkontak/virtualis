// $Id: vxViewerTypeMPROrthoPanZoom.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
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
#include "vxViewerTypeMPROrthoPanZoom.h"
#include "vxEnvironment.h"
#include "vxWinRect.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxIntensityConverter.h"
#include "vxUtils.h"
#include "vxAnnotation.h"
#include "vxAnnotationPoint.h"
#include "vxAnnotationArrow.h"
#include "vxAnnotationDistance.h"
#include "vxAnnotationAngle.h"
#include "vxAnnotationBox.h"
#include "vxAnnotationEllipse.h"
#include "vxAnnotationComponent.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 */
vxViewerTypeMPROrthoPanZoom::vxViewerTypeMPROrthoPanZoom(vxEnvironment & environment)
:vxViewerTypeMPROrtho(environment)
//*******************************************************************
{
}


/**
 * Initialize viewer.
 * This method is responsable to initialize the OpenGL viewing
 */
void vxViewerTypeMPROrthoPanZoom::SetupView()
//*******************************************************************
{
  vxWinRect winRect(GetEnvironment().GetWinRect());
  glViewport(winRect.GetReferencePoint().first, 
             winRect.GetReferencePoint().second, 
             winRect.GetViewportSize().first, 
             winRect.GetViewportSize().second);

  // find the actual camera setup (reference, eye, up)
  //////////////////////////////////////////////////////
  vxUtils::StandardCameraViewEnum eView;
  switch (GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord())
  {
  case 0: 
    eView = vxUtils::LEFT; 
    break;
  case 1: 
    eView = vxUtils::ANTERIOR; 
    break;
  case 2: 
    eView = vxUtils::INFERIOR; 
    break;
  default:
    throw ex::IllegalArgumentException(LogRec("There is only three axis, invalid index", "vxViewerTypeMPROrthoPanZoom", "SetupView"));
    break;
  }
  vxCamera camera(*GetEnvironment().GetCamera());
  Triplef size(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  Box<float4> roi(0, 0, 0, size[0], size[1], size[2]);
  vxUtils::SetStandardCameraView(camera, eView, roi,  false);

  Point<float4> eye(camera.GetEye());
  Point<float4> ref(camera.GetViewReference());
  Normal<float4> up(camera.GetViewUp());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye.X(), eye.Y(), eye.Z(),
            ref.X(), ref.Y(), ref.Z(), 
            up.X(),  up.Y(),  up.Z());

  // find the actual projection parameters
  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  Triple<float4> volDim(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  float4 fAspectRatioVolume(volDim[uAxisY]/volDim[uAxisX]);
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());
  float4 fLeft, fRight, fBottom, fTop;
  
  if (fAspectRatioWindow < fAspectRatioVolume)
  {
    // x-axis adjustment
    fLeft   = - 0.5F * volDim[uAxisX] * fAspectRatioVolume/fAspectRatioWindow;
    fRight  = + 0.5F * volDim[uAxisX] * fAspectRatioVolume/fAspectRatioWindow;
    fBottom = - 0.5F * volDim[uAxisY];
    fTop    = + 0.5F * volDim[uAxisY];
  }
  else
  {
    // y-axis adjustment
    fLeft   = - 0.5F * volDim[uAxisX];
    fRight  = + 0.5F * volDim[uAxisX];
    fBottom = - 0.5F * volDim[uAxisY] * fAspectRatioWindow/fAspectRatioVolume;
    fTop    = + 0.5F * volDim[uAxisY] * fAspectRatioWindow/fAspectRatioVolume;
  }
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(fLeft, fRight, fBottom, fTop, 0.1, 3000); // is this really it?

  return;
} // SetupView()


// Revision History:
// $Log: vxViewerTypeMPROrthoPanZoom.C,v $
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2004/04/26 18:58:39  michael
// moved GetLeft() / GetRight() / etc. from vxCamera to vxViewerType
//
// Revision 1.2  2004/04/06 16:27:47  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.8  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.7  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.6  2003/04/18 16:04:09  michael
// initial code review
//
// Revision 1.5  2003/04/08 16:37:07  michael
// removed redundant include (vxWindowHandle)
//
// Revision 1.4  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.3  2003/03/26 22:54:53  michael
// moved empty method implementation to header file
//
// Revision 1.2  2003/03/19 16:16:30  michael
// fixed compile issues and added it to DEFINES in managed code
//
// Revision 1.1  2003/03/19 16:05:01  michael
// Added initial version
//
// $Id: vxViewerTypeMPROrthoPanZoom.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerTypeMPROrthoPanZoom.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
