// $Id: v3D_Renderers.h,v 1.20 2007/07/14 01:43:53 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Interfaces.h"
#include "vxRenderer3DOrientationCube.h"
#include "vxRenderer3dCursor.h"
#include "vxRendererAnnotations.h"
#include "vxRendererBorder.h"
#include "vxRendererCropBox.h"
#include "vxRendererCpu3D.h"
#include "vxRendererOverlay.h"
#include "vxRendererOverlayMPROrtho.h"
#include "vxRendererOverlayMPROblique.h"
#include "vxRendererOverlay3D.h"
#include "vxRendererOverlayPhaseVolume.h"
#include "vxRendererMPROrthoPanZoom.h"
#include "vxRendererSampleRange.h"
#include "vxRendererColormap.h"
#include "vxRendererColorChooser.h"
#include "vxRendererAlphaFullRange.h"
#include "vxRendererAlphaActiveRange.h"
#include "vxRendererSculpt.h"
#include "vxRendererSoftwareMPROrtho.h"
#include "vxRendererSoftwareMPROblique.h"
#include "vxRendererTexMap2DMPRNew.h"
#include "vxRendererContour.h"
#include "vxRendererOverlayRayProfile.h"
#include "vxRendererPath3D.h"
#include "vxRendererPlaneObliqueCreate.h"
#include "vxRendererRaytracer.h"
#include "vxRendererSoftwareMPRCurved.h"
#include "vxRendererSoftwareMPRCurvedProjected.h"
#include "vxRendererSoftwareMPRCurvedLuminal.h"
#include "vxRendererOverlayMPRCurved.h"
#include "vxRendererVesselCurved.h"
#include "vxRendererPlanetracer.h"
#include "vxRendererCurvetracer.h"
#include "vxRendererOverlayVessel.h"
#include "vxRendererOverlayNoVessel.h"
#include "vxRendererVessel3D.h"
#include "vxRendererOverlayCArm.h"
#include "vxRendererOverlayVesselDetailMPROblique.h"
#include "vxRendererOverlayVesselSection.h"
#include "vxRendererVesselDetailMPROblique.h"
#include "vxRendererVesselDetailMPROrtho.h"
#include "vxRendererOverlaySeedpointsMPROrtho.h"
#include "vxRendererOverlayIncrementalGrowth.h"

#include "vxRendererSliceShadowsMPROrtho.h"
#include "vxRendererSliceShadowsMPROblique.h"
#include "vxRendererSliceShadows3d.h"
#include "vxRendererSliceShadowsMPRCurved.h"
#include "vxRendererGraph.h"
#include "vxRendererCadFindings.h"
#include "vxRendererMesh.h"




// namespaces
OPEN_VISUALIZATION_NS

DEFINE_RENDERER( RendererSoftwareMPROrtho )

DEFINE_RENDERER( RendererSoftwareMPROblique )

DEFINE_RENDERER( RendererCpu3D )

DEFINE_RENDERER( Renderer3DOrientationCube )

DEFINE_RENDERER( RendererSampleRange )

DEFINE_RENDERER( RendererColormap )

DEFINE_RENDERER( RendererColorChooser )

DEFINE_RENDERER( RendererAlphaFullRange )

DEFINE_RENDERER( RendererAlphaActiveRange )

DEFINE_RENDERER( RendererSculpt )

DEFINE_RENDERER( RendererContour )

DEFINE_RENDERER( RendererOverlayRayProfile )

DEFINE_RENDERER( RendererOverlay )

DEFINE_RENDERER( RendererOverlayMPROrtho )

DEFINE_RENDERER( RendererOverlayMPROblique )

DEFINE_RENDERER( RendererOverlay3D )

DEFINE_RENDERER( RendererOverlayPhaseVolume )

DEFINE_RENDERER( RendererAnnotations )

DEFINE_RENDERER( RendererCursor3d )

DEFINE_RENDERER( RendererCropBox )

DEFINE_RENDERER( RendererBorder )

DEFINE_RENDERER( RendererMPROrthoPanZoom )

DEFINE_RENDERER( RendererPath3D )

DEFINE_RENDERER( RendererPlaneObliqueCreate )


DEFINE_RENDERER( RendererSoftwareMPRCurved )

DEFINE_RENDERER( RendererSoftwareMPRCurvedProjected )

DEFINE_RENDERER( RendererSoftwareMPRCurvedLuminal )


DEFINE_RENDERER( RendererOverlayMPRCurved )

DEFINE_RENDERER( RendererVesselCurved )


DEFINE_RENDERER( RendererOverlayVessel )

DEFINE_RENDERER( RendererOverlayNoVessel )

DEFINE_RENDERER( RendererVessel3D )

DEFINE_RENDERER( RendererOverlayCArm )

DEFINE_RENDERER( RendererOverlayVesselDetailMPROblique )

DEFINE_RENDERER( RendererOverlayVesselSection )

DEFINE_RENDERER( RendererVesselDetailMPROblique )

DEFINE_RENDERER( RendererVesselDetailMPROrtho )

DEFINE_RENDERER( RendererOverlaySeedpointsMPROrtho )

DEFINE_RENDERER( RendererOverlayIncrementalGrowth )



DEFINE_RENDERER( RendererRaytracer )

DEFINE_RENDERER( RendererPlanetracer )

DEFINE_RENDERER( RendererCurvetracer )



DEFINE_RENDERER( RendererSliceShadows3d )

DEFINE_RENDERER( RendererSliceShadowsMPROrtho )

DEFINE_RENDERER( RendererSliceShadowsMPROblique )

DEFINE_RENDERER( RendererSliceShadowsMPRCurved )

DEFINE_RENDERER( RendererGraph )

DEFINE_RENDERER( RendererCadFindings )

DEFINE_RENDERER( RendererMesh )



// class definition
public ref class RendererTexMapMPR : public IRendererFactory
{
// functions
public:

  /// create a renderer
  virtual System::IntPtr CreateRenderer( System::IntPtr environment );

}; // RendererTexMapMPR


CLOSE_VISUALIZATION_NS


// $Log: v3D_Renderers.h,v $
// Revision 1.20  2007/07/14 01:43:53  frank
// Added mesh renderer
//
// Revision 1.19  2007/04/11 20:57:02  romy
// added RendererCadFindings
//
// Revision 1.18  2006/11/02 14:56:06  romy
// build error fixed
//
// Revision 1.17  2006/11/02 04:55:37  romy
// commented out  GraphRenderer because of linker error
//
// Revision 1.16  2006/11/02 04:42:10  romy
// added GraphRenderer
//
// Revision 1.15  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.14  2006/10/03 20:14:36  geconomos
// updated for new slice shadow renderers
//
// Revision 1.13  2006/09/25 13:27:58  geconomos
// moving shared code for vascular and cardiac to visualization dll
//
// Revision 1.12  2006/09/21 15:06:50  geconomos
// added vxRendererOverlayNoVessel
//
// Revision 1.11  2006/09/20 14:32:08  geconomos
// added RendererVessel3D
//
// Revision 1.10  2006/09/14 20:26:35  geconomos
// added RendererCurvetracer and RendererOverlayVessel
//
// Revision 1.9  2006/08/17 14:07:16  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.8  2006/07/21 15:08:25  geconomos
// added Planetracer renderer
//
// Revision 1.7  2006/06/21 18:47:32  geconomos
// added vxRendererRaytracer
//
// Revision 1.6  2006/03/27 19:44:42  gdavidson
// Added RendererSliceShadowsAll and RendererSliceShadowsActive
//
// Revision 1.5  2004/10/23 09:58:56  frank
// moved fusion IDs to the fusion plugin
//
// Revision 1.4  2004/04/26 21:28:11  michael
// renamed classes (vxRendererMPRORthoPanZoom and vxManipulatorMPRORthoPanZoom)
//
// Revision 1.3  2004/04/06 18:22:01  frank
// renamed vessel to path
//
// Revision 1.2  2004/03/26 13:50:26  frank
// working on PET fusion
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.10.2.1  2004/02/23 21:36:08  frank
// removed pet fusion renderer
//
// Revision 1.10  2003/12/18 14:18:55  frank
// Working on new renderer for PET fusion
//
// Revision 1.9  2003/11/04 15:49:09  michael
// accounting for the changes in the overlay renderers (inheritance and naming)
//
// Revision 1.8  2003/09/25 03:07:49  frank
// Merged oblique views into one
//
// Revision 1.7  2003/08/20 14:34:14  michael
// added renderer for plane oblique create data
//
// Revision 1.6  2003/08/14 16:58:03  frank
// removed oblique renderer
//
// Revision 1.5  2003/08/12 17:02:04  frank
// Added rendering
//
// Revision 1.4  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.3  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.2  2003/04/11 18:33:50  frank
// Got overlay strings working
//
// Revision 1.1  2003/03/21 15:42:08  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Renderers.h,v 1.20 2007/07/14 01:43:53 frank Exp $
// $Id: v3D_Renderers.h,v 1.20 2007/07/14 01:43:53 frank Exp $
