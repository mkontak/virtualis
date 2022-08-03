// $Id: vxControllerCursor3D.C,v 1.4 2005/11/18 21:14:53 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxControllerCursor3D.h"
#include "vxOverlayStrings.h"
#include "vxEnvironment.h"
#include "vxViewerType3D.h"


/**
 * constructor
 *
 * @param environment the environment
 */
vxControllerCursor3D::vxControllerCursor3D( vxEnvironment & environment ) :
vxController( environment )
{
} // vxControllerCursor3D()


/**
 * determine if processing is necessary
 *
 * @return if processing is necessary
 */
bool vxControllerCursor3D::IsProcessingNecessary()
{

  const bool bCursorModified( GetEnvironment().IsModified( vxEnvironment::CURSOR3D ) );
  const bool bSamePlane( GetEnvironment().GetActiveEnvironment() != NULL &&
                         GetEnvironment().GetActiveEnvironment()->GetPlaneOrtho() == GetEnvironment().GetPlaneOrtho() );
  return bCursorModified && ! bSamePlane;

} // IsProcessingNecessary()


/**
 * process the data
 */
void vxControllerCursor3D::Process()
{

  if ( IsProcessingNecessary() )
  {

    Point<float4> position( GetEnvironment().GetCursor3d()->GetPosition() );

    // ortho plane case
    if ( GetEnvironment().GetPlaneOrtho() != NULL )
    {

      uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
      GetEnvironment().GetPlaneOrtho()->SetDistance( position[ uOrientation ] ); 
      GetEnvironment().SetModified( vxEnvironment::PLANE_MPR, true );

    } // ortho plane case

    // oblique plane case
    if ( GetEnvironment().GetPlaneOblique() != NULL )
    {

      Triple<float4> units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
      float4 fMin( Min( Min( units[0], units[1]), units[2] ) );
      const float fEpsilon( 0.0001F * fMin );
      const float fDistance( GetEnvironment().GetPlaneOblique()->GetDistance( position ) );

      // this is needed to prevent the precision issue of constantly moving
      // the plane due to the changes in the 3D cursor that then moves the plane
      * GetEnvironment().GetPlaneOblique() = vxPlaneOblique< float4 >( GetEnvironment().GetPlaneOblique()->GetNormal(), Point3D<float4>( position ) );
      GetEnvironment().SetModified( vxEnvironment::PLANE_MPR, true );

    } // oblique plane case

    // update the 3D viewpoint if we are a 3D view
    const bool b3dView( dynamic_cast< vxViewerType3D * >( GetEnvironment().GetViewerType() ) != NULL );
    if ( b3dView )
    {
      // update the view position without changing the view direction
      // (must remember the original view direction since view direction is determined by
      // comparing the eye point with the view reference point)
      Normal3Df viewDirection( GetEnvironment().GetCursor3d()->GetViewDirection() );
      GetEnvironment().GetCamera()->SetEye( GetEnvironment().GetCursor3d()->GetPosition() );
      GetEnvironment().GetCamera()->SetViewDirection( viewDirection );
      GetEnvironment().SetModified( vxEnvironment::CAMERA, true );
    } // update 3D view

  } // if processing necessary

} // Process()


// $Log: vxControllerCursor3D.C,v $
// Revision 1.4  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/04 18:30:56  frank
// Issue #4330: Moved cursor tracking from the 3D renderer to a controller
//
// Revision 1.3  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.2  2005/05/16 14:45:12  frank
// fixed synchronization of 3x3 view
//
// Revision 1.1  2004/10/27 18:37:33  frank
// moved cursor 3D functionality to a controller
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerCursor3D.C,v 1.4 2005/11/18 21:14:53 frank Exp $
// $Id: vxControllerCursor3D.C,v 1.4 2005/11/18 21:14:53 frank Exp $
