// $Id: v3D_RenderingNodePlugin.cpp,v 1.3 2007/05/03 21:36:02 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_RenderingNodePlugin.h"
#include "vxRenderingNodePlugin.h"
#include "vxShareableObject.h"
#include "v3D_Triplet.h"
#include "Triple.h"


// namespace
USING_VISUALIZATION_NS
USING_GRAPH_NS


/**
 * constructor
 */
RenderingNodePlugin::RenderingNodePlugin( System::IntPtr renderingManagerPlugin ) :
  m_pRenderingManagerPlugin( reinterpret_cast< vxRenderingNodePlugin * >( renderingManagerPlugin.ToPointer() ))
{
}


/**
 * Gets the min render size
 */
Triplet ^ RenderingNodePlugin::MinRenderSize::get()
{
  const Triple< uint4 > & triple = m_pRenderingManagerPlugin->GetMinRenderSize();
  return gcnew Triplet( static_cast< float4 >( triple.m_x ), static_cast< float4 >( triple.m_y ), static_cast< float4 >( triple.m_z ));
} // MinRenderSize


/**
 * Sets the min render size
 */
void RenderingNodePlugin::MinRenderSize::set( Triplet ^ mpTriplet )
{
  vxShareableObject< Triple< float4 > > & oldTriple = * ToNativeTripletPtr( mpTriplet->GetElementPtr() );
  Triple< uint4 > newTriple( static_cast< uint4 >( oldTriple.m_x ), static_cast< uint4 >( oldTriple.m_y ), static_cast< uint4 >( oldTriple.m_z ));

  m_pRenderingManagerPlugin->SetMinRenderSize( newTriple );
} // MinRenderSize


/**
 * Gets the max render size
 */
Triplet ^ RenderingNodePlugin::MaxRenderSize::get()
{
  const Triple< uint4 > & triple = m_pRenderingManagerPlugin->GetMaxRenderSize();
  return gcnew Triplet( static_cast< float4 >( triple.m_x ), static_cast< float4 >( triple.m_y ), static_cast< float4 >( triple.m_z ));
} // MaxRenderSize


/**
 * Sets the max render size
 */
void RenderingNodePlugin::MaxRenderSize::set( Triplet ^ mpTriplet )
{
  vxShareableObject< Triple< float4 > > & oldTriple = * ToNativeTripletPtr( mpTriplet->GetElementPtr() );
  Triple< uint4 > newTriple( static_cast< uint4 >( oldTriple.m_x ), static_cast< uint4 >( oldTriple.m_y ), static_cast< uint4 >( oldTriple.m_z ));

  m_pRenderingManagerPlugin->SetMaxRenderSize( newTriple );
} // MaxRenderSize


/**
 * resets the rendering manager
 */
void RenderingNodePlugin::Reset()
{
  m_pRenderingManagerPlugin->Reset();
} // Reset();


  
// $Log: v3D_RenderingNodePlugin.cpp,v $
// Revision 1.3  2007/05/03 21:36:02  gdavidson
// Moved to Viatronix.Visualization.Graph namespace
//
// Revision 1.2  2007/03/01 23:02:02  gdavidson
// Removed v3D_VisualizationPreferences
//
// Revision 1.1  2007/02/05 19:28:00  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.1  2007/01/16 17:14:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_RenderingNodePlugin.cpp,v 1.3 2007/05/03 21:36:02 gdavidson Exp $
// $Id: v3D_RenderingNodePlugin.cpp,v 1.3 2007/05/03 21:36:02 gdavidson Exp $

