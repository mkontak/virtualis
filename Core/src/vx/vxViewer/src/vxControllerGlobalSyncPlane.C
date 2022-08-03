// $Id: vxControllerGlobalSyncPlane.C,v 1.2 2007/10/03 20:35:05 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#include "StdAfx.h"
#include "vxControllerGlobalSyncPlane.h"
#include "vxEnvironment.h"
#include "vxViewer.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerTypeMPRCurved.h"
#include "vxElementBool.h"

// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
vxControllerGlobalSyncPlane::vxControllerGlobalSyncPlane() :
  m_bIsLastEnabled( false )
{
} // vxControllerGlobalSliceShadows()


/**
 * Destructor
 */
vxControllerGlobalSyncPlane::~vxControllerGlobalSyncPlane()
{
} // ~vxControllerGlobalSliceShadows()


/**
 * Processes the controller
 */
void vxControllerGlobalSyncPlane::Process()
{
  const vxEnvironment * pActiveEnvironment = vxEnvironment::GetActiveEnvironment();
  vxElementBool * pSyncPlane = dynamic_cast< vxElementBool * >( pActiveEnvironment->GetElement( vxIDs::SyncPlane ));
  if( pSyncPlane->GetValue() == m_bIsLastEnabled )
  {
    // determine if any of the viewer's planes have changed
    if( pActiveEnvironment->IsModified( vxEnvironment::PLANE_MPR ) == false )
      return;
  }

  m_bIsLastEnabled = pSyncPlane->GetValue();
  if( pSyncPlane != NULL && pSyncPlane->GetValue() )
  {
    // synch orthogonal planes
    if( dynamic_cast< vxViewerTypeMPROrtho * >( pActiveEnvironment->GetViewerType() ) != NULL )
    {
      vxPlaneOrtho< float4 > & activePlane = *pActiveEnvironment->GetPlaneOrtho();
      vxCamera & activeCamera = *pActiveEnvironment->GetCamera();
      for( int4 i = 0; i < GetViewers().size(); ++i )
      {
        vxEnvironment & environment = GetViewers()[i]->GetEnvironment();
        if( &environment != pActiveEnvironment && dynamic_cast< vxViewerTypeMPROrtho * >( environment.GetViewerType() ) != NULL )
        {
          vxPlaneOrtho< float4 > & plane = *environment.GetPlaneOrtho();

          // if orientations are different reset the camera
          if( activePlane.GetNormal() != plane.GetNormal() )
          {
            vxCamera & camera = *environment.GetCamera();
            camera = activeCamera;
            environment.SetModified( vxEnvironment::CAMERA, true );
          }

          plane = activePlane;
          environment.SetModified( vxEnvironment::PLANE_MPR, true );
        }
      }
    }

    // synch oblique planes
    else if( dynamic_cast< vxViewerTypeMPROblique * >( pActiveEnvironment->GetViewerType() ) != NULL )
    {
      for( int4 i = 0; i < GetViewers().size(); ++i )
      {
        vxPlaneOblique< float4 > & activePlane = *pActiveEnvironment->GetPlaneOblique();
        vxEnvironment & environment = GetViewers()[i]->GetEnvironment();
        if( &environment != pActiveEnvironment && dynamic_cast< vxViewerTypeMPROblique * >( environment.GetViewerType() ) != NULL )
        {
          vxPlaneOblique< float4 > & plane = *environment.GetPlaneOblique();
          plane = activePlane;
          environment.SetModified( vxEnvironment::PLANE_MPR, true );
        }
      }
    }
  }
} // Process()


// $Log: vxControllerGlobalSyncPlane.C,v $
// Revision 1.2  2007/10/03 20:35:05  gdavidson
// Synchronize the normals of the ortho planes
//
// Revision 1.1  2007/10/01 15:00:47  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerGlobalSyncPlane.C,v 1.2 2007/10/03 20:35:05 gdavidson Exp $
// $Id: vxControllerGlobalSyncPlane.C,v 1.2 2007/10/03 20:35:05 gdavidson Exp $


