// $Id: vxControllerRebox.C,v 1.11 2006/09/14 20:06:22 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxControllerRebox.h"
#include "Box.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.11 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxControllerRebox::vxControllerRebox(vxEnvironment & environment) :
vxController(environment)
{
} // vxControllerRebox()


/**
 * determine if processing is necessary
 *
 * @return if processing is necessary
 */
bool vxControllerRebox::IsProcessingNecessary()
{

  bool bProcessingNecessary( false );

  if ( GetEnvironment().IsModified( vxEnvironment::CROP_BOX         ) ) bProcessingNecessary = true;
  if ( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE   ) ) bProcessingNecessary = true;
  if ( GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY ) ) bProcessingNecessary = true;
  if ( GetEnvironment().IsModified( vxEnvironment::VOLUME_LABEL     ) ) bProcessingNecessary = true;
  if ( GetEnvironment().IsModified( vxEnvironment::COLORMAP         ) ) bProcessingNecessary = true;
  if ( GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL ) ) bProcessingNecessary = true;

  return bProcessingNecessary;

} // IsProcessingNecessary()


/**
 * Process the data
 */
void vxControllerRebox::Process()
{
  try
  {
    
    // perform the update if necessary
    if ( IsProcessingNecessary() )
    {
      // set the class member crop box to the visible data boundary
      Box<float4> bboxPrevious(GetEnvironment().GetRenderingMode()->GetVisibleDataBox());

      // get the visibility box in world coordinates and store it in the environment for use of the vxManipulatorCameraFly
      const Triplef & units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
      GetEnvironment().GetRenderingMode()->GetVisibleDataBox() = vxUtils::GetVisibleBoundingBox(GetEnvironment());
      GetEnvironment().GetRenderingMode()->GetVisibleDataBox().ScaleAnIso(units);

      Box<float4> bboxCurrent(GetEnvironment().GetRenderingMode()->GetVisibleDataBox());

      // set the camera box to the visibility box
      float4 fDiff(bboxCurrent.GetOuterRadius() - bboxPrevious.GetOuterRadius());
      float4 fSize(bboxPrevious.GetOuterRadius());
      const float4 fThreshold(0.25F);

      if (fabs(fDiff / fSize) > fThreshold)
      {
        vxShareableObject< vxCamera > * pCamera = GetEnvironment().GetCamera();

        // apart from setting the box, we need to keep the viewing direction!!!
        Vector3Df direction(pCamera->GetViewDirection());
        Point3Df reference(bboxCurrent.GetMidPoint());

        pCamera->SetViewReference(reference);
        pCamera->SetEye(pCamera->GetViewReference() - direction);
        vxUtils::CameraViewAll(*pCamera, bboxCurrent);
        pCamera->Modified( vxModInfo( vxCamera,vxUtils::CAMERA_MODIFIED ) );
      } // if changed significantly
    } // if update necessary

  } // try
  catch (...)
  {
    throw ex::VException(LogRec("UpdateBoundingBox failed", "vxController", "Process"));
  }
} // Process()


// undefines
#undef FILE_REVISION


// Revision History:
// $Log: vxControllerRebox.C,v $
// Revision 1.11  2006/09/14 20:06:22  geconomos
// settings camera modified after modifying
//
// Revision 1.10  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.9  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.8.2.2  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.8.2.1  2005/07/19 11:49:31  frank
// missing include
//
// Revision 1.8  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.7  2004/10/18 14:49:42  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.6  2004/09/27 14:15:35  frank
// added pre-processing check to controller interface
//
// Revision 1.5  2004/09/27 14:14:32  frank
// added pre-processing check to controller interface
//
// Revision 1.4  2004/09/01 12:30:24  frank
// decreased the granularity of component array modified bits
//
// Revision 1.3  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.2  2004/05/24 15:51:39  frank
// removed debug
//
// Revision 1.1  2004/05/20 13:25:06  frank
// initial version of class
//
// Revision 1.1  2004/05/19 15:47:08  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerRebox.C,v 1.11 2006/09/14 20:06:22 geconomos Exp $
// $Id: vxControllerRebox.C,v 1.11 2006/09/14 20:06:22 geconomos Exp $
