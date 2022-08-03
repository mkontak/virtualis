// $Id: vxControllerGlobalSyncViewers.C,v 1.1.2.2 2009/03/11 17:39:57 dongqing Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Cheng Qian (mailto:cqian@viatronix.com)

#include "StdAfx.h"
#include "vxControllerGlobalSyncviewers.h"
#include "vxEnvironment.h"
#include "vxViewer.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerTypeMPRCurved.h"
#include "vxSyncData.h"
#include "vxIDs.h"

// defines
#define FILE_REVISION "$Revision: 1.1.2.2 $"


/**
 * Constructor
 */
vxControllerGlobalSyncViewers::vxControllerGlobalSyncViewers() :
  m_bIsLastEnabled( false )
{
} // vxControllerSyncViewers()


/**
 * Destructor
 */
vxControllerGlobalSyncViewers::~vxControllerGlobalSyncViewers()
{
} // ~vxControllerSyncViewers()


/**
 * Processes the controller
 */
void vxControllerGlobalSyncViewers::Process()
{
  /// This class has not been finished by Cheng. It has bug on orientation change! Dongqing 20090311

  const vxEnvironment * pActiveEnvironment = vxEnvironment::GetActiveEnvironment();
  vxSyncData * pSyncData = dynamic_cast< vxSyncData * >( pActiveEnvironment->GetElement( vxIDs::SyncData ));
  if( pSyncData->IsEnabled() == m_bIsLastEnabled )
  {
    // determine if any of the viewer's planes have changed
    if( pActiveEnvironment->IsModified( vxEnvironment::PLANE_MPR ) == false )
      return;
  }
  //LogErr( "Before viewer type" + ToAscii(pActiveEnvironment), "", ""  ); 

  m_bIsLastEnabled = pSyncData->IsEnabled();
  if( pSyncData != NULL && pSyncData->IsEnabled() )
  {
     //LogErr( "Enabled", "", ""  ); 

     vxPlaneOrtho< float4 > & activePlane = *pActiveEnvironment->GetPlaneOrtho();

    // synch orthogonal planes
    if( dynamic_cast< vxViewerTypeMPROrtho * >( pActiveEnvironment->GetViewerType() ) != NULL )
    {
      //  LogErr( "Ortho", "", ""  ); 

      vxPlaneOrtho< float4 > & activePlane = *pActiveEnvironment->GetPlaneOrtho();
      vxCamera & activeCamera = *pActiveEnvironment->GetCamera();
      for( int4 i = 0; i < pSyncData->GetEnvironments().size(); ++i )
      {
        vxEnvironment & environment = *( pSyncData->GetEnvironments()[i] );

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
      //LogErr( "Oblique", "", "" );

      vxPlaneOblique< float4 > & activePlane = *pActiveEnvironment->GetPlaneOblique();
      vxCamera & activeCamera = *pActiveEnvironment->GetCamera();
      for( int4 i = 0; i < pSyncData->GetEnvironments().size(); ++i )
      {
        vxEnvironment & environment = *( pSyncData->GetEnvironments()[i] );
        if( &environment != pActiveEnvironment && dynamic_cast< vxViewerTypeMPROblique * >( environment.GetViewerType() ) != NULL )
        {
          vxPlaneOblique< float4 > & plane = *environment.GetPlaneOblique();

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
  }
} // Process()


// $Log: vxControllerGlobalSyncViewers.C,v $
// Revision 1.1.2.2  2009/03/11 17:39:57  dongqing
// comments
//
// Revision 1.1.2.1  2009/03/11 16:07:41  dongqing
// add comments to show bugs for future fixing
//
// Revision 1.1  2008/05/02 04:56:30  cqian
// check in
//

// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerGlobalSyncViewers.C,v 1.1.2.2 2009/03/11 17:39:57 dongqing Exp $
// $Id: vxControllerGlobalSyncViewers.C,v 1.1.2.2 2009/03/11 17:39:57 dongqing Exp $


