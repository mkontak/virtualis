// $Id: vxControllerCADFindings.C,v 1.4 2007/04/17 20:09:24 romy Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Romy Sreedharan (romy@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "vxCadFindings.h"
#include "vxControllerCADFindings.h"

// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
* Constructor
*
* @param environment the environment
*/
vxControllerCADFindings::vxControllerCADFindings(vxEnvironment & environment) :
vxController(environment)
{
} // vxControllerCADFindings()


/**
* determine if processing is necessary
*
* @return if processing is necessary
*/
bool vxControllerCADFindings::IsProcessingNecessary()
{

  const bool bProcessingNecessary ( GetEnvironment().IsModified( vxEnvironment::CAD_FINDINGS ) );
  return bProcessingNecessary;

} // IsProcessingNecessary()


/**
* Process the data
*/
void vxControllerCADFindings::Process()
{
  try
  {
    if ( GetEnvironment().GetCadFindings()->GetSelectedIndex() < 0 )
      return;

    const vxCAD::vxCadFinding & finding = GetEnvironment().GetCadFindings()->GetSelectedFinding();
    Point<float4> cadCentroid  = Point3Df( finding.GetCentroidMM().GetVector());

    //If the units are in voxels, scale it to Voxel Units
    if ( finding.GetCentroidMM().GetUnits() == vxCAD::vxCadVector::VOXELS )
      cadCentroid.ScaleAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
    
    // Bring the ortho slice to the front.
    if ( GetEnvironment().GetPlaneOrtho() != NULL )
    {
      uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
      GetEnvironment().GetPlaneOrtho()->SetDistance( cadCentroid[ uOrientation ] );
      GetEnvironment().GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );  
    } // if not yet visible

    // Bring the oblique slice to the front.
    if ( GetEnvironment().GetPlaneOblique() != NULL )
    {
      GetEnvironment().GetPlaneOblique()->SetNormal( Normal3D<float4>(0,0,-1));
      GetEnvironment().GetPlaneOblique()->SetDistance(  cadCentroid[ vxUtils::AXIAL ]);
      GetEnvironment().GetPlaneOblique()->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED ) );  
    }
    //if ( GetEnvironment().GetViewerType()->Is3D() )
    //{
    //  //create a crop box
    //  float4 fFindingRadiusMM = 100.0F;
    //  Vector3Df radiusVector = Vector3Df( fFindingRadiusMM, fFindingRadiusMM, fFindingRadiusMM ).GetDividedAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
    //  Point3Di pointUL( cadCentroid - radiusVector );
    //  Point3Di pointLR( cadCentroid + radiusVector );
    //  Box<int4> bounds( pointUL.m_x, pointUL.m_y, pointUL.m_z, pointLR.m_x, pointLR.m_y, pointLR.m_z );

    //  // set the crop box
    //  GetEnvironment().GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_CROP_BOX, true );
    //  GetEnvironment().GetCropBox()->SetBox( bounds);
    //  GetEnvironment().GetCropBox()->Modified( vxModInfo( vxCropBox, vxUtils::CROPBOX_MODIFIED ) );    
    //}
  } // try
  catch (...)
  {
    throw ex::VException(LogRec("Process failed", "vxControllerCADFindings", "Process"));
  }
} // Process()



// undefines
#undef FILE_REVISION


// Revision History:
// $Log: vxControllerCADFindings.C,v $
// Revision 1.4  2007/04/17 20:09:24  romy
// added code to display the cad findings in nearby planes also.
//
// Revision 1.3  2007/04/12 18:54:05  romy
// Added code to display the Findings in Oblique slice
//
// Revision 1.2  2007/04/12 18:53:14  romy
// Added code to display the Findings in Oblique slice
//
// Revision 1.1  2007/04/11 20:50:27  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerCADFindings.C,v 1.4 2007/04/17 20:09:24 romy Exp $
// $Id: vxControllerCADFindings.C,v 1.4 2007/04/17 20:09:24 romy Exp $
