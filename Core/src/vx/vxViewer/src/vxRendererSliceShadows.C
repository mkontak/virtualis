// $Id: vxRendererSliceShadows.C,v 1.18 2007/01/16 17:02:13 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:george@viatronix.net)


// includes
#include "stdafx.h"
#include "vxRendererSliceShadows.h"
#include "vxEnvironment.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerTypeMPRCurved.h"

// explicit instantiation
template class VX_VIEWER_DLL vxElementPixelRGBA< uint1 >;
template class VX_VIEWER_DLL vxElementPixelRGBA< float4 >;



// defines
#define FILE_REVISION "$Revision: 1.18 $"


/**
 * Constructor
 * @param environment the environment
 */
vxRendererSliceShadows::vxRendererSliceShadows( vxEnvironment & environment ) : vxRenderer( environment )
{
} // vxRendererSliceShadows()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererSliceShadows::IsRedrawNecessary()
{
  // early return if no rendering necessary
  if ( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_NONE )
    return false;

  return true;
} // IsRedrawNecessary()


/**
 * Gets the color for the slice shadow represented by the specified environment
 *
 * @param   environment   contains plane to retrieve
 */
const PixelRGBA<uint1> vxRendererSliceShadows::GetSliceShadowColor( const vxEnvironment & environment )
{
  // TODO: clean this up
  PixelRGBA< uint1 > color( 255, 255, 255, 255 );
  
  vxViewerType * pViewerType = environment.GetViewerType();
  if( dynamic_cast< const vxViewerTypeMPROrtho * >( pViewerType ) )
  {
    const uint1 uNormalAxis = environment.GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord();
    switch( uNormalAxis )
    {
      case 0:
        color.SetRGBA( 0, 0, 255, 255 );
        break;
      case 1:
        color.SetRGBA( 0, 255, 0, 255 );
        break;
      case 2:
        color.SetRGBA( 255, 0, 0, 255 );
        break;
      default:
        break;
    }
  }
  else if( dynamic_cast< const vxViewerTypeMPROblique * >( pViewerType ) )
  {
    color.SetRGBA( 0, 255, 255, 255 );
  }
 else if( dynamic_cast< const vxViewerTypeMPRCurved * >( pViewerType ) )
  {
    color.SetRGBA( 200, 200, 200, 255 );
  }

  if( &environment != environment.GetActiveEnvironment() )
    color.A() *= 0.8;

  return color;
}


/**
 * Gets the ortho/oblique plane for the specified environment
 *
 * @param   environment   contains plane to retrieve
 */
const vxPlaneOblique< float4 > * vxRendererSliceShadows::GetPlane( const vxEnvironment & environment )
{
  const vxViewerType * pViewerType = environment.GetViewerType();
  
  const vxPlaneOblique< float4 > * pPlane = NULL;
  
  if( dynamic_cast< const vxViewerTypeMPROrtho * >( pViewerType ) )
    pPlane =  environment.GetPlaneOrtho();
  
  else if( dynamic_cast< const vxViewerTypeMPROblique * >( pViewerType ) )
    pPlane = environment.GetPlaneOblique();

  else
    throw ex::UnsupportedDataTypeException( LogRec( "Invalid viewer type", "vxRendererSliceShadows", "GetPlane" ));
  
  return pPlane;
} // GetPlane( const vxEnvironment & environment )


// $Log: vxRendererSliceShadows.C,v $
// Revision 1.18  2007/01/16 17:02:13  gdavidson
// Implemented IsRedrawNecessary()
//
// Revision 1.17  2006/10/24 12:32:21  geconomos
// reworked curved slice shadow rendering
//
// Revision 1.16  2006/10/23 01:38:54  geconomos
// code cleanup
//
// Revision 1.15  2006/10/05 19:44:59  geconomos
// changed opacty value for non-selected slice shadow colors
//
// Revision 1.14  2006/10/05 16:59:36  geconomos
// redesgined and implemented
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSliceShadows.C,v 1.18 2007/01/16 17:02:13 gdavidson Exp $
// $Id: vxRendererSliceShadows.C,v 1.18 2007/01/16 17:02:13 gdavidson Exp $
