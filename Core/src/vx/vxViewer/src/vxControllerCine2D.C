// $Id: vxControllerCine2D.C,v 1.3 2007/02/05 15:29:25 geconomos Exp $
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
#include "vxControllerCine2D.h"
#include "Box.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "vxRenderingMode.h"
#include "vxPlaneOrtho.h"
#include "vxPlaneOblique.h"
#include "vxMathUtils.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerTypeVesselDetailMPROrtho.h"
#include "vxCine.h"
#include "vxVesselUtilities.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxControllerCine2D::vxControllerCine2D(vxEnvironment & environment) :
  vxController(environment),
  m_bSwingForward( true )
{
  vxEnvironment::SetUserInteracting( true );
} // vxControllerCine2D()


/**
 * Destructor
 */
vxControllerCine2D::~vxControllerCine2D()
{
  vxEnvironment::SetUserInteracting( false );
} // ~vxControllerCine2D()


/**
 * determine if processing is necessary
 *
 * @return if processing is necessary
 */
bool vxControllerCine2D::IsProcessingNecessary()
{
  bool bIsProcessingNecessary( false );
  if( GetEnvironment().GetCine()->GetCineMode() != vxCine::IDLE )
  {
    float8 delta = 1.0F / GetEnvironment().GetCine()->GetFramesPerSecond();
    if( m_timer.ElapsedTime() > delta )
    {
      bIsProcessingNecessary = true;
      m_timer.Reset();
    }
  }
  
  return bIsProcessingNecessary;
} // IsProcessingNecessary()


/**
 * Process the data
 */
void vxControllerCine2D::Process()
{
  if( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType() ) != NULL )
    CinePlaneOrtho();
  else if( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) != NULL )
    CinePlaneOblique();
} // Process()


/**
 * cine for an ortho plane
 */
void vxControllerCine2D::CinePlaneOrtho()
{
  vxPlaneOrtho<float4> * pPlane = dynamic_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho());
  if( pPlane != NULL )
  {
    Triple<int4> dim(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVolDim());
    Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
    Normal<float4> normal(pPlane->GetNormal());
    const uint1 uAxis((uint1)normal.GetMaxAbsComponentCoord());
    int4 iDistance(0.5 + pPlane->GetDistance()/units[uAxis]);
    int4 iDelta(0);

    switch( GetEnvironment().GetCine()->GetCineMode() )
    {
    case vxCine::LOOP_BACKWARD: iDelta = -1; break;
    case vxCine::LOOP_FORWARD:  iDelta =  1; break;
    case vxCine::SWING:
      if (iDistance == 0)            m_bSwingForward = true;
      if (iDistance == dim[uAxis]-1) m_bSwingForward = false;
      iDelta = (m_bSwingForward == true) ? 1 : -1;
      break;
    }
    
    uint4 uNewSlice((iDistance+iDelta < 0) ? dim[uAxis]-1 : (iDistance+iDelta) % dim[uAxis]);
    pPlane->SetDistance(uNewSlice * units[uAxis]);
    GetEnvironment().GetPlaneOrtho()->Modified(vxModInfo(vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED));
  } // plane exists?
} // CinePlaneOrtho()


/**
 * cine for an oblique plane
 */
void vxControllerCine2D::CinePlaneOblique()
{
  vxPlaneOblique<float4> * pPlane = dynamic_cast<vxPlaneOblique<float4> *>(GetEnvironment().GetPlaneOblique());
  if ( pPlane != NULL )
  {
    Triple<int4> dim(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVolDim());
    Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
    Normal<float4> normal(pPlane->GetNormal());
    Box<float4> boxVolume(0, 0, 0, (dim[0]-1)*units[0], (dim[1]-1)*units[1], (dim[2]-1)*units[2]);
    float4 fDistance(pPlane->GetDistance());
    float4 fDistanceMin(vxMathUtils::GetMinDistance(boxVolume, *pPlane));
    float4 fDistanceMax(vxMathUtils::GetMaxDistance(boxVolume, *pPlane));
    float4 fDistanceUnit(Min(Min(units[0], units[1]), units[2]));
    float4 fDistanceMinMax(fDistanceMax-fDistanceMin);

    int4 iSign(0);

    switch(GetEnvironment().GetCine()->GetCineMode() )
    {
    case vxCine::LOOP_BACKWARD: iSign =  1; break;
    case vxCine::LOOP_FORWARD:  iSign = -1; break;
    case vxCine::SWING:
      if( fabs( fDistanceMax ) < fDistanceUnit )
        m_bSwingForward = false;
      if( fabs( fDistanceMin ) < fDistanceUnit ) 
        m_bSwingForward = true;

      iSign = (m_bSwingForward == true) ? -1 : 1; 
      break;
    }

    float4 fStepDelta(iSign*fDistanceUnit);
    float4 fDistanceNew(fDistance + fStepDelta);
    if (fStepDelta < 0 && fabs(fStepDelta) > fDistanceMax)
    {
      fDistanceNew +=fDistanceMinMax;
    }
    else if (fStepDelta > 0 && fStepDelta > fabs(fDistanceMin)) 
    {
      fDistanceNew -=fDistanceMinMax;
    }

    pPlane->SetDistance(fDistanceNew);
    GetEnvironment().GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
  }
} // CinePlaneOblique()


///**
// * This event handler can be reimplemented in a subclass to receive timer events for the viewer.     
// * @param sEvent the name of the triggering event timer
// */
//void vxManipulatorVesselDetailMPROrtho::OnTimer( const std::string & sEvent )
//{
//
//  if ( vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() ) == NULL )
//    return;
//
//  // early return if not for us
//  if ( sEvent != "CINE" ) return;
//
//  // prepare to change the index
//  float4 fNewIndex( GetVesselSectionIndex() );
//  const float4 fIncrement( 0.5F );
//
//  // decide which direction to go
//  switch( GetEnvironment().GetCine() )
//  {
//  case vxEnvironment::LOOP_BACKWARD:
//    fNewIndex = GetVesselSectionIndex() - fIncrement;
//    if ( fNewIndex < 0 ) fNewIndex = GetMaximumVesselSectionIndex();
//    break;
//
//  case vxEnvironment::LOOP_FORWARD:
//    fNewIndex = GetVesselSectionIndex() + fIncrement;
//    if ( fNewIndex > GetMaximumVesselSectionIndex() ) fNewIndex = 0;
//    break;
//
//  case vxEnvironment::SWING:
//    if ( m_bSwingForward )
//    {
//      // move forward a little
//      fNewIndex = GetVesselSectionIndex() + fIncrement;
//      if ( fNewIndex > GetMaximumVesselSectionIndex() )
//      {
//        // gone too far, reverse direction
//        fNewIndex = GetMaximumVesselSectionIndex() - fIncrement;
//        m_bSwingForward = false;
//      }
//    }
//    else
//    {
//      // move backward a little
//      fNewIndex = GetVesselSectionIndex() - fIncrement;
//      if ( fNewIndex < 0 )
//      {
//        // gone too far, reverse direction
//        fNewIndex = fIncrement;
//        m_bSwingForward = true;
//      }
//    }
//    break;
//
//  default:
//    throw ex::IllegalArgumentException( LogRec( "Unsupported cine mode", "vxManipulatorVesselDetailMPROrtho", "OnTimer" ) );
//    break;
//  } // switch( cine mode )
//  
//  SetVesselSectionIndex( GetEnvironment(), fNewIndex );
//
//} // OnTimer()

// $Log: vxControllerCine2D.C,v $
// Revision 1.3  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.2  2007/01/22 15:27:55  gdavidson
// Issue #5323: Fixed swing in oblique
//
// Revision 1.1  2007/01/16 16:27:12  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerCine2D.C,v 1.3 2007/02/05 15:29:25 geconomos Exp $
// $Id: vxControllerCine2D.C,v 1.3 2007/02/05 15:29:25 geconomos Exp $

