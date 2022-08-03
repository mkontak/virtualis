// $Id: vxManipulatorVesselNavigation.C,v 1.4 2006/09/25 20:12:59 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (dachille@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxElementFloat4.h"
#include "vxManipulatorVesselNavigation.h"
#include "vxManipulatorVessel.h"
#include "vxVesselUtilities.h"
#include "vxEnvironment.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * default constructor
 * @param environment the environment
 */
vxManipulatorVesselNavigation::vxManipulatorVesselNavigation( vxEnvironment & environment )
: vxManipulator(environment)
{
  GetEnvironment().GetCamera()->SetProjectionType( vxCamera::PERSPECTIVE );
} // Constructor()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselNavigation::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  bool bHandled( false );

  // get the vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
  {
    LogDbg( "there is no current vessel", "vxManipulatorVesselOblique", "OnKeyPressed" );
    return bHandled;
  }

  switch ( uKeyId )
  {
  // PageUp key
  case 33:
    TranslateSection( GetKeyMultiplier( uKeyboardState ) );
    bHandled = true;
    break;

  // PageDown key
  case 34:
    TranslateSection( - int4( GetKeyMultiplier( uKeyboardState ) ) );
    bHandled = true;
    break;

  // Home key
  case 36:
    SetVesselSectionIndex( GetEnvironment(), 0, false );
    bHandled = true;
    break;

  // End key
  case 35:
    SetVesselSectionIndex( GetEnvironment(), pVessel->size() - 1, true );
    bHandled = true;
    break;

  default:
    break;
  }

  return bHandled;
} // OnKeyPressed


/**
 * Event handler if mouse wheel is scrolled
 *
 * @param uKeyId         the key that was pressed
 * @param iDelta         the amount the wheel was scrolled in either direction
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */
bool vxManipulatorVesselNavigation::OnMouseWheel( const Point2D<int2> & position, const int2 iDelta,
                                                  const uint4 uKeyboardState )
{

  TranslateSection( float4( iDelta ) / vxInput::GetMouseWheelQuantum() * float4( GetKeyMultiplier( uKeyboardState ) ) );
  return true;
  
} // OnMouseWheel()


/**
 * Translate the section by a certain distance.
 *
 * @param fDistance the distance in millimeters
 */
void vxManipulatorVesselNavigation::TranslateSection( const float4 fDistance )
{
  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );

  // clamp to valid range and set
  const float4 fOldIndex( vxManipulatorVessel::GetVesselSectionIndex( GetEnvironment() ) );
  float4 fNewIndex( Bound( 0.0F, fOldIndex + fDistance, float4( pVessel->size()-1 ) ) );

  SetVesselSectionIndex( GetEnvironment(), fNewIndex, fOldIndex > fNewIndex );

} // TranslateSection()


/**
 * Set the current section index
 *
 * @param environment   the environment
 * @param fSectionIndex the desired section index
 * @param bLookForward
 */
void vxManipulatorVesselNavigation::SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex,
                                                           const bool bLookForward )
{
  // get the current index pointer
  typedef vxShareableObject< vxElementFloat4 > * pSharedFloat;
  pSharedFloat pCurrentVesselSection =
    dynamic_cast< pSharedFloat >( environment.GetElement( vxIDs::CurrentVesselSection ));
  if ( pCurrentVesselSection == NULL )
  {
    LogDbg( "no current vessel section: ", "vxManipulatorVesselNavigation", "SetVesselSectionIndex" );
    return;
  }

  // get the vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );
  if ( pVessel == NULL )
  {
    throw ex::NullPointerException( LogRec( "Vessel pointer is NULL", "vxManipulatorVesselNavigation",
                                            "SetVesselSectionIndex" ) );
  }

  // bounds check the section index
  if ( fSectionIndex < 0.0F || fSectionIndex > static_cast< float4 >( pVessel->size()-1 ) )
  {
    LogDbg( "new index out of bounds: " + ToAscii( fSectionIndex ), "vxManipulatorVesselNavigation", "SetVesselSectionIndex" );
    return;
  }

  // set the current vessel section index
  pCurrentVesselSection->SetValue( fSectionIndex );
  pCurrentVesselSection->Modified( vxModInfoID( vxElement, vxIDs::CurrentVesselSection, 0 ) );

  //// set the camera location - need to interpolate these
  //const Point<float4> position( pVessel->GetResampledPosition( fSectionIndex ) -
  //                              vxManipulatorVessel::GetDetailVolumeOffset( environment ) );
  //Normal<float4> tangent( pVessel->GetResampledTangent( fSectionIndex ) );
  //if ( bLookForward ) tangent = - tangent;
  //const Normal<float4> upDirection( pVessel->GetResampledUpDirection( fSectionIndex ) );

  //environment.GetCamera()->SetEye( position );
  //environment.GetCamera()->SetViewDirection( tangent );
  //environment.GetCamera()->SetViewUp( upDirection );
  //environment.GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

  //// set the endoluminal camera object modified
  //vxShareableObject< vxCamera > * pEndoluminalCamera = dynamic_cast< vxShareableObject< vxCamera > * >( environment.GetElement( vxIDs::VesselDetailEndoluminalCamera ) );
  //if ( pEndoluminalCamera == NULL )
  //{
  //  LogWrn( "no endoluminal camera", "vxManipulatorVesselNavigation", "SetVesselSectionIndex" );
  //  return;
  //}
  //pEndoluminalCamera->Modified( vxModInfoID( vxElement, vxIDs::VesselDetailEndoluminalCamera, 0 ) );

} // SetVesselSectionIndex()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVesselNavigation.C,v $
// Revision 1.4  2006/09/25 20:12:59  geconomos
// moved initialization of endoluminal to vxControllerVesselDetail
//
// Revision 1.3  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.6  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.5  2006/06/05 15:07:38  frank
// updated for visual studio 2005
//
// Revision 1.4  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/09/07 13:56:15  frank
// Issue #4407: Fixed endoluminal navigation from the curved MPR image.
//
// Revision 1.3  2005/06/08 15:00:29  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:10:42  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2004/03/02 01:10:32  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.6.2.3  2004/02/26 05:21:15  frank
// Issue #3757: Fixed the 3D endoluminal view update when switching among vessels
//
// Revision 1.6.2.2  2004/02/25 17:38:41  frank
// Issue #3757: fixing the 3D overview update during endoluminal flight
//
// Revision 1.6.2.1  2004/02/25 14:59:13  frank
// formatting
//
// Revision 1.6  2003/12/22 18:54:22  frank
// Set the current vessel section extended element modified
//
// Revision 1.5  2003/07/15 17:09:57  frank
// Fixed endoluminal flight speed
//
// Revision 1.4  2003/05/27 13:20:01  frank
// code review
//
// Revision 1.3  2003/05/21 12:04:21  frank
// code review
//
// Revision 1.2  2003/05/20 18:18:19  dongqing
// code review
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.12  2003/04/30 15:51:25  frank
// code review
//
// Revision 1.11  2003/03/14 18:02:54  frank
// Fixed view direction along vessel
//
// Revision 1.10  2003/03/14 17:29:05  frank
// Turned the camera in the direction of movement
//
// Revision 1.9  2003/03/14 16:06:30  frank
// Organized code better
//
// Revision 1.8  2003/03/13 14:32:31  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.7  2003/03/12 22:38:51  geconomos
// Commented out code to initialize the logger.
//
// Revision 1.6  2003/03/07 20:42:43  geconomos
// Defaulted handled flag in OnKeyPress to false instead of true.
//
// Revision 1.5  2003/02/18 17:59:16  frank
// Cleaned up debug messages
//
// Revision 1.4  2003/02/14 22:34:52  frank
// Made it work
//
// Revision 1.3  2003/02/14 19:25:02  frank
// Working on endoluminal flight
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVesselNavigation.C,v 1.4 2006/09/25 20:12:59 geconomos Exp $
// $Id: vxManipulatorVesselNavigation.C,v 1.4 2006/09/25 20:12:59 geconomos Exp $
