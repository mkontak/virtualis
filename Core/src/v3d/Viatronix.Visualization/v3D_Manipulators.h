// $Id: v3D_Manipulators.h,v 1.13.8.3 2009/08/31 13:11:55 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// includes
#include "stdafx.h"
#include "v3D_Interfaces.h"
#include "vxManipulator3DCursor.h"
#include "vxManipulatorAlphaActiveRange.h"
#include "vxManipulatorAnnotationCreate.h"
#include "vxManipulatorAnnotationAdjust.h"
#include "vxManipulatorCameraExamine.h"
#include "vxManipulatorCameraFly.h"
#include "vxManipulatorCameraMPR.h"
#include "vxManipulatorCameraMomentum.h"
#include "vxManipulatorCameraPath.h"
#include "vxManipulatorColorChooser.h"
#include "vxManipulatorColormap.h"
#include "vxManipulatorContour.h"
#include "vxManipulatorAddVoxel.h"
#include "vxManipulatorRemoveVoxel.h"
#include "vxManipulatorContourAdjust.h"
#include "vxManipulatorCropBox.h"
#include "vxManipulatorCurrentVoxel.h"
#include "vxManipulatorDataRendererSoftware3d.h"
#include "vxManipulatorMPROrthoPanZoom.h"
#include "vxManipulatorPathCurvedCreate.h"
#include "vxManipulatorPlaneCurved.h"
#include "vxManipulatorPlaneOblique.h"
#include "vxManipulatorPlaneObliqueCreate.h"
#include "vxManipulatorPlaneOrtho.h"
#include "vxManipulatorPreset.h"
#include "vxManipulatorResearch.h"
#include "vxManipulatorSampleRegion.h"
#include "vxManipulatorSculpt.h"
#include "vxManipulatorSegmentation.h"
#include "vxManipulatorWindowLevel.h"

#include "vxManipulatorVesselCurvedCreate.h"
#include "vxManipulatorVesselCurvedProjected.h"
#include "vxManipulatorVesselDetailMPROblique.h"
#include "vxManipulatorVesselDetailMPROrtho.h"
#include "vxManipulatorVesselLuminal.h"
#include "vxManipulatorVesselNavigation.h"
#include "vxManipulatorSubcube.h"
#include "vxManipulatorIncrementalGrowth.h"
#include "vxManipulatorCenterSelection.h"
#include "vxManipulatorFullScreen.h"

OPEN_VISUALIZATION_NS

  DEFINE_MANIPULATOR( ManipulatorPlaneOblique )

  DEFINE_MANIPULATOR( ManipulatorWindowLevel )

  DEFINE_MANIPULATOR( ManipulatorCurrentVoxel )

  DEFINE_MANIPULATOR( ManipulatorPlaneOrtho )

  DEFINE_MANIPULATOR( ManipulatorPlaneCurved )

  DEFINE_MANIPULATOR( ManipulatorCameraExamine )

  DEFINE_MANIPULATOR( ManipulatorCameraMomentum )

  DEFINE_MANIPULATOR( ManipulatorCameraMPR )

  DEFINE_MANIPULATOR( ManipulatorDataRendererSoftware3d )

  DEFINE_MANIPULATOR( ManipulatorCameraFly )

  DEFINE_MANIPULATOR( ManipulatorCropBox )

  DEFINE_MANIPULATOR( ManipulatorResearch )

  DEFINE_MANIPULATOR( ManipulatorMPROrthoPanZoom )

  DEFINE_MANIPULATOR( ManipulatorCursor3d )

  DEFINE_MANIPULATOR( ManipulatorAlphaActiveRange )

  DEFINE_MANIPULATOR( ManipulatorColormap )

  DEFINE_MANIPULATOR( ManipulatorColorChooser )

  DEFINE_MANIPULATOR( ManipulatorSculpt )

  DEFINE_MANIPULATOR( ManipulatorSegmentation )

  DEFINE_MANIPULATOR( ManipulatorContour )

  DEFINE_MANIPULATOR( ManipulatorAddVoxel )

  DEFINE_MANIPULATOR( ManipulatorRemoveVoxel )

  DEFINE_MANIPULATOR( ManipulatorContourAdjust )

  DEFINE_MANIPULATOR( ManipulatorSampleRegion )

  DEFINE_MANIPULATOR( ManipulatorAnnotationCreate )

  DEFINE_MANIPULATOR( ManipulatorAnnotationAdjust )

  DEFINE_MANIPULATOR( ManipulatorPathCurvedCreate )

  DEFINE_MANIPULATOR( ManipulatorCameraPath )

  DEFINE_MANIPULATOR( ManipulatorPlaneObliqueCreate )

  DEFINE_MANIPULATOR( ManipulatorPreset )

  DEFINE_MANIPULATOR( ManipulatorVesselCurvedCreate );

  DEFINE_MANIPULATOR( ManipulatorVesselCurvedProjected );

  DEFINE_MANIPULATOR( ManipulatorVesselDetailMPROblique );

  DEFINE_MANIPULATOR( ManipulatorVesselDetailMPROrtho );

  DEFINE_MANIPULATOR( ManipulatorVesselLuminal );

  DEFINE_MANIPULATOR( ManipulatorVesselNavigation );

  DEFINE_MANIPULATOR( ManipulatorSubcube );

  DEFINE_MANIPULATOR( ManipulatorIncrementalGrowth );

  DEFINE_MANIPULATOR( ManipulatorCenterSelection );

  DEFINE_MANIPULATOR( ManipulatorFullScreen );

CLOSE_VISUALIZATION_NS


// $Log: v3D_Manipulators.h,v $
// Revision 1.13.8.3  2009/08/31 13:11:55  kchalupa
// Add references to new Add Voxel/Remove Voxel
//
// Revision 1.13.8.2  2008/11/24 17:14:14  gdavidson
// Issue #6171: Added fullscreen double click
//
// Revision 1.13.8.1  2008/11/18 19:33:44  gdavidson
// Issue #6203: Added center selection for obliques
//
// Revision 1.13  2007/02/06 18:58:18  geconomos
// corrected "user interacting" usage with momentum
//
// Revision 1.12  2006/10/24 12:34:56  geconomos
// removed old slice shadows mechanism
//
// Revision 1.11  2006/09/25 13:27:58  geconomos
// moving shared code for vascular and cardiac to visualization dll
//
// Revision 1.10  2006/08/17 14:07:16  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.9  2006/03/27 19:44:19  gdavidson
// Added ManipulatorSliceShadowsAll and ManipulatorSliceShadowsActive
//
// Revision 1.8  2006/03/13 13:56:54  frank
// added component contour adjustment
//
// Revision 1.7  2004/08/30 18:19:38  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.6  2004/05/25 14:27:29  frank
// renamed camera path manipulator
//
// Revision 1.5  2004/04/26 21:28:11  michael
// renamed classes (vxRendererMPRORthoPanZoom and vxManipulatorMPRORthoPanZoom)
//
// Revision 1.4  2004/04/15 16:05:55  frank
// split out the preset manipulation from the window/level manipulator
//
// Revision 1.3  2004/04/06 18:22:01  frank
// renamed vessel to path
//
// Revision 1.2  2004/03/11 18:20:33  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/01/20 21:30:45  frank
// added zooming on plus and minus keys in pan / zoom window
//
// Revision 1.11  2003/12/19 13:53:07  michael
// Renaming: vxManipulatorRegionSampler -> vxManipulatorSampleRegion
//
// Revision 1.10  2003/09/25 03:07:49  frank
// Merged oblique views into one
//
// Revision 1.9  2003/09/16 16:04:13  frank
// Separated oblique from ortho slice shadows
//
// Revision 1.8  2003/09/16 15:40:03  frank
// Added oblique slice shadow manipulator
//
// Revision 1.7  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.6  2003/08/14 17:01:07  frank
// Added vessel navigation
//
// Revision 1.5  2003/08/13 19:37:31  michael
// added vxManipulatorPlaneObliqueCreate
//
// Revision 1.4  2003/08/12 15:48:48  frank
// Moved the vessel tool down to vxBase
//
// Revision 1.3  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.2  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.1  2003/05/06 18:51:42  geconomos
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Manipulators.h,v 1.13.8.3 2009/08/31 13:11:55 kchalupa Exp $
// $Id: v3D_Manipulators.h,v 1.13.8.3 2009/08/31 13:11:55 kchalupa Exp $
