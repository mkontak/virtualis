// $Id: vxControllerGlobalSliceShadows.C,v 1.6 2008/01/18 21:29:27 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxControllerGlobalSliceShadows.h"
#include "vxEnvironment.h"
#include "vxViewer.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerTypeMPRCurved.h"

// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Constructor
 */
vxControllerGlobalSliceShadows::vxControllerGlobalSliceShadows()
{
} // vxControllerGlobalSliceShadows()


/**
 * Destructor
 */
vxControllerGlobalSliceShadows::~vxControllerGlobalSliceShadows()
{
} // ~vxControllerGlobalSliceShadows()


/**
 * Processes the controller
 */
void vxControllerGlobalSliceShadows::Process()
{
  // check if any slice shadow has been modified
  bool bAnySliceShadowModified = false;
  for( int4 i = 0; i < GetViewers().size(); ++i )
  {
    vxViewer * pViewer = GetViewers()[i];
    if( pViewer->GetEnvironment().GetIgnoreModifiedEvents() )
      continue;
    vxViewerType * pViewerType = pViewer->GetViewerType();
    
    // check if the plane has been modified if the viewer type is
    // "orthogonal", "oblique" or  "curved
    if( dynamic_cast< vxViewerTypeMPROrtho   * >( pViewerType ) || 
        dynamic_cast< vxViewerTypeMPROblique * >( pViewerType ) || 
        dynamic_cast< vxViewerTypeMPRCurved  * >( pViewerType ))
    {
      bAnySliceShadowModified = pViewer->GetEnvironment().IsModified( vxEnvironment::PLANE_MPR );
      if( bAnySliceShadowModified )
        break;
    }
  }

  // check if the active environment slice shadow has been modified
  bool bActiveSliceShadowModified = false;
  const vxEnvironment * pActiveEnvironment = vxEnvironment::GetActiveEnvironment();
  if( pActiveEnvironment )
  {
    vxViewerType * pActiveViewerType = pActiveEnvironment->GetViewerType();
    if( dynamic_cast< vxViewerTypeMPROrtho   * >( pActiveViewerType ) || 
        dynamic_cast< vxViewerTypeMPROblique * >( pActiveViewerType ) || 
        dynamic_cast< vxViewerTypeMPRCurved  * >( pActiveViewerType ))
    {
      bActiveSliceShadowModified = pActiveEnvironment->IsModified( vxEnvironment::PLANE_MPR );
    }
  }

  // update modified bits accordingly
  for( int4 i = 0; i < GetViewers().size(); ++i )
  {
    vxViewer * pViewer = GetViewers()[i];
    if( pViewer->GetEnvironment().GetIgnoreModifiedEvents() )
      continue;
    
    vxRenderingMode::vxSliceShadowModeEnum eShadowMode = pViewer->GetEnvironment().GetRenderingMode()->GetSliceShadowMode();
    
    if( eShadowMode == vxRenderingMode::SHADOWS_ACTIVE || eShadowMode == vxRenderingMode::SHADOWS_ACTIVE_IMAGE && bActiveSliceShadowModified )
      pViewer->GetEnvironment().SetModified( vxEnvironment::SLICE_SHADOWS, true );
    
    else if( eShadowMode == vxRenderingMode::SHADOWS_ALL && bAnySliceShadowModified )
      pViewer->GetEnvironment().SetModified( vxEnvironment::SLICE_SHADOWS, true );
  }
} // Process()


// $Log: vxControllerGlobalSliceShadows.C,v $
// Revision 1.6  2008/01/18 21:29:27  geconomos
// Issue# 5869: Images updating in background while making a movie
//
// Revision 1.5  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.4  2007/02/22 16:07:37  geconomos
// code cleanup
//
// Revision 1.3  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.2  2007/01/18 15:44:26  geconomos
// added new slice shadow mode
//
// Revision 1.1  2006/10/24 12:30:02  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerGlobalSliceShadows.C,v 1.6 2008/01/18 21:29:27 geconomos Exp $
// $Id: vxControllerGlobalSliceShadows.C,v 1.6 2008/01/18 21:29:27 geconomos Exp $


