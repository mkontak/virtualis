// $Id: vxRenderingNodePluginCurvetracer.C,v 1.3.2.1 2009/08/27 21:17:44 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxRenderingNodePluginCurvetracer.h"
#include "vxEnvironment.h"
#include "vxVesselutilities.h"

#define FILE_REVISION "$Revision: 1.3.2.1 $"


/**
 * Constructor
 *
 * @param   environment   environment
 */
vxRenderingNodePluginCurvetracer::vxRenderingNodePluginCurvetracer( vxEnvironment & environment ) : vxRenderingNodePlugin( environment )
{
} // vxRenderingNodePluginCurvetracer


/**
 * Destructor
 */
vxRenderingNodePluginCurvetracer::~vxRenderingNodePluginCurvetracer()
{
} // ~vxRenderingNodePluginCurvetracer()


/**
 * Initializes the plugin from the specified vxEnvironment
 */
void vxRenderingNodePluginCurvetracer::InitializeFromEnvironment()
{
  GetEnvironment().SetModified( vxEnvironment::ALL, true );
} // InitializeFromEnvironment()


/**
 * Determines if rendering is necessary
 *
 * @param   environment     environment to check
 * @return    true if necessary
 */
bool vxRenderingNodePluginCurvetracer::IsRenderingNecessary() const
{
  bool bRenderingNecessary = false;

  const bool bRibbonChanged           = GetEnvironment().IsModified( vxIDs::Ribbon );
  const bool bVesselChanged           = GetEnvironment().IsModified( vxIDs::VesselArray );
  //const bool bCurvedPlaneChanged      = GetEnvironment().IsModified( vxEnvironment::PLANE_MPR );
  const bool bChangedVolumeIntensity  = GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY );
  const bool bChangedColormap         = GetEnvironment().IsModified( vxEnvironment::COLORMAP );
  const bool bChangedComponents       = GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL );
  const bool bChangedRenderingMode    = GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE );
  const bool bChangedWinRect          = GetEnvironment().IsModified( vxEnvironment::WIN_RECT );

	
  bRenderingNecessary |= bRibbonChanged;
  bRenderingNecessary |= bVesselChanged;
  //bRenderingNecessary |= bCurvedPlaneChanged;
  bRenderingNecessary |= bChangedVolumeIntensity;
  bRenderingNecessary |= bChangedColormap;
  bRenderingNecessary |= bChangedComponents;
  bRenderingNecessary |= bChangedRenderingMode;
  bRenderingNecessary |= bChangedWinRect;

  return bRenderingNecessary;

} // IsRenderingNecessary()


/**
 * Updates the pluging from the specified vxEnvironment
 */
void vxRenderingNodePluginCurvetracer::UpdateFromEnvironment()
{
  bool bUpdateTransferFuntion = false;
   
  // intensity volume
  if( GetEnvironment().IsModified( vxEnvironment::VOLUME_INTENSITY )  )
  {
    bUpdateTransferFuntion = true;
    m_curvetracer.GetSettings().SetIntensityVolume( *GetEnvironment().GetIntensityVolume() );
  }

  // colormap
  if( GetEnvironment().IsModified( vxEnvironment::COLORMAP ) )
    bUpdateTransferFuntion = true;
  
  // does the transfer need updating?
  if( bUpdateTransferFuntion )
    m_curvetracer.GetSettings().SetTransferFunction( *GetEnvironment().GetIntensityVolume(), *GetEnvironment().GetColormap() );

  // rendering mdoe
  if ( GetEnvironment().IsModified( vxEnvironment::RENDERING_MODE ) == true )
    m_curvetracer.GetSettings().SetRenderingMode( *GetEnvironment().GetRenderingMode() );

  // components
  //if ( GetEnvironment().IsModified( vxEnvironment::COMPONENT_VISUAL ) )
	// The above if statement is not necessary. The curved MPR view has never built with the component control
	// mechanism. It should always grab the current selected/available vessel to display. Dongqing and Greg 27 August 2009
  {
    // check for vessel
    vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
    if( pVessel )
      m_curvetracer.GetSettings().SetVessel( *pVessel );
  }

  // win rect
  if ( GetEnvironment().IsModified( vxEnvironment::WIN_RECT ) )
    m_curvetracer.GetSettings().SetWinRect( GetEnvironment().GetWinRect() );

  // ribbon
  if( GetEnvironment().IsModified( vxIDs::Ribbon ) )
    m_curvetracer.GetSettings().SetRibbon( *dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) ) );

} // UpdateFromEnvironment()


// $Log: vxRenderingNodePluginCurvetracer.C,v $
// Revision 1.3.2.1  2009/08/27 21:17:44  dongqing
// Fix the full screen vessel selection left curved MPR view black bug.
// The component control mechanism has never been built into the
// curved MPR view!
//
// Revision 1.3  2007/08/23 18:47:36  romy
// fixed the issue with slow Rendering of Vessel detail view in full screen. Unnecessary check for PLANE_MPR was the cause of performance
//
// Revision 1.2  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.5  2007/01/16 16:49:41  gdavidson
// Changed the constructor to take a vxEnvironment
//
// Revision 1.4  2006/09/27 16:08:56  geconomos
// corrected the IsRenderingNecessary method
//
// Revision 1.3  2006/09/21 15:02:25  geconomos
// removed unused vxComponentArray
//
// Revision 1.2  2006/09/20 14:36:17  geconomos
// correct usage of vxVessel
//
// Revision 1.1  2006/09/14 20:11:48  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingNodePluginCurvetracer.C,v 1.3.2.1 2009/08/27 21:17:44 dongqing Exp $
// $Id: vxRenderingNodePluginCurvetracer.C,v 1.3.2.1 2009/08/27 21:17:44 dongqing Exp $
