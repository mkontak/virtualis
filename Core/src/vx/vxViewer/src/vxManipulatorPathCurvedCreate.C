// $Id: vxManipulatorPathCurvedCreate.C,v 1.4 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "vxManipulatorPathCurvedCreate.h"
#include "vxEnvironment.h"
#include "vxVessel.h"
#include "vxIDs.h"
#include "vxViewerTypes.h"
#include "vxShareableObject.h"


// Defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorPathCurvedCreate::vxManipulatorPathCurvedCreate(vxEnvironment & environment):
vxManipulator(environment)
{
  m_bFirstTime = true;
} // vxManipulatorPathCurvedCreate()


/**
 * One time initialization.  This gets called at least once just before some user interaction occurs.
 * Apparently if you put it in the constructor, it is too early because not all data is available yet.
 */
void vxManipulatorPathCurvedCreate::OneTimeInitialize()
{
  if ( m_bFirstTime )
  {
    m_bFirstTime = false;

    // get the vessel
    vxShareableObject< vxVessel > * pVessel = dynamic_cast< vxShareableObject< vxVessel > * >( GetEnvironment().GetElement( vxIDs::Path ) );
    if ( pVessel == NULL )
    {
      //LogDbg( "Vessel pointer is NULL", "vxManipulatorPathCurvedCreate", "vxManipulatorPathCurvedCreate" );
      return;
    }

    // create a new vessel
    pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
    pVessel->SetSectionInterval( -1.0F );
    pVessel->SetCalcificationThresholdHU( 500 );
  }
} // OneTimeInitialization()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorPathCurvedCreate::OnKeyPressed(const uint1 uKeyId, 
                                                   const uint4 uKeyboardState)
{
  OneTimeInitialize();

  bool bHandled(false);

  // get the vessel
  vxShareableObject< vxVessel > * pVessel = dynamic_cast< vxShareableObject< vxVessel > * >( GetEnvironment().GetElement( vxIDs::Path ) );
  if ( pVessel == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorPathCurvedCreate", "OnKeyPressed" );
    return bHandled;
  }

  // make sure the vessel has some data in it
  if ( pVessel->size() == 0 )
    return bHandled;

  switch ( uKeyId )
  {
  case vxInput::KEY_ESCAPE:
    pVessel->clear();
    pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
    bHandled = true;
    break;

  // backspace
  case vxInput::KEY_BACKSPACE:
    if ( pVessel->size() > 0 ) pVessel->pop_back();
    pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
    bHandled = true;
    break;

  default:
    break;
  }

  if ( bHandled == true )
    pVessel->Modified( vxModInfoID( vxElement, vxIDs::Path, 0 ) );

  return bHandled;
} // OnKeyPressed()


/**
 * Event handler if button is pressed down
 *
 * @param position       the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorPathCurvedCreate::OnButtonDown(const Point2D<int2> & mouseCoord,
                                                   const ButtonEnum eButtonId, 
                                                   const uint4 uKeyboardState)
{
  OneTimeInitialize();

  bool bHandled(false);

  // get the vessel
  vxShareableObject<vxVessel> * pVessel = dynamic_cast< vxShareableObject< vxVessel > * >( GetEnvironment().GetElement( vxIDs::Path ) );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorPathCurvedCreate", "OnButtonDown" );
    return bHandled;
  }

  if (eButtonId == BUTTON_LEFT)
  {

    // pick a 3D position for the centerline point
    Point<float4> worldPosition;

    vxViewerType3D * pViewerType3D = dynamic_cast< vxViewerType3D *>( GetEnvironment().GetViewerType() );
    if (pViewerType3D != NULL)
    {
      std::vector<float4> profile;
      if (pViewerType3D->GetVesselLumenWorldFromMouse(mouseCoord, worldPosition, profile) == false)
        return false;
    }
    else
    {
      if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mouseCoord, worldPosition) == false)
        return false;
    }

    // store the centerline point in a vessel section
    vxVesselSection section;
    section.SetPosition( worldPosition );
    section.SetTanDirection( Normal3Df( 0.0F, 1.0F, 1.0F ) );
    section.SetUpDirection( Normal3Df( 1.0F, 0.0F, 1.0F ) );

    // add the section to the vessel
    pVessel->push_back( section );
    pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
    const float4 fArcLengthMM( 20 );
    pVessel->EstimateTangents( fArcLengthMM );
    pVessel->RecalculateLengths();

    // update the GUI
    pVessel->Modified( vxModInfoID( vxElement, vxIDs::Path, 0 ) );

  } // if left button.

  return bHandled;
} // OnButtonDown()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorPathCurvedCreate::OnSetCursor()
{
  vxMouseCursor::SetCursor( "CURSOR_CURVED_MPR" );
  return true;
  
} // OnSetCursor()


#undef FILE_REVISION


// $Log: vxManipulatorPathCurvedCreate.C,v $
// Revision 1.4  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.3.2.1  2007/04/16 15:08:43  romy
// fixes the curosr display  issue.
//
// Revision 1.3  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.2  2006/02/27 20:11:58  gdavidson
// Added an OnSetCursor method
//
// Revision 1.1  2004/04/06 18:19:41  frank
// renamed vessel to path
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.5  2003/12/18 14:14:17  frank
// Renamed the element ID filename to match class name
//
// Revision 1.4  2003/12/16 19:06:31  frank
// Added additional key definitions
//
// Revision 1.3  2003/08/22 19:22:58  frank
// comment fix
//
// Revision 1.2  2003/08/13 11:27:19  frank
// Removed pointers to vessel sections
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.2  2003/07/23 16:48:01  dongqing
// set calcification value to 500 when new vessel is created
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.12  2003/04/30 16:57:39  frank
// code reviews
//
// Revision 1.11  2003/04/30 15:21:19  frank
// Issue #3151: Used component handler to create new components so that they can be handled properly, e.g., grow, shrink, etc.
//
// Revision 1.10  2003/04/30 14:16:50  frank
// code review
//
// Revision 1.9  2003/04/11 18:40:55  frank
// Working on new curved MPR creation
//
// Revision 1.8  2003/04/08 14:45:22  frank
// Moved GetVesselLumenWorldCoordinates() into vxViewerType3D
//
// Revision 1.7  2003/03/21 15:40:50  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.6  2003/02/18 22:11:59  frank
// Working on creating a new vessel
//
// Revision 1.5  2003/01/22 22:17:21  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.4  2001/01/09 16:59:38  frank
// Enabled picking of vessels in 3D.
//
// Revision 1.3  2002/12/17 17:41:32  frank
// New syntax in GetWorldCoordinates()
//
// Revision 1.2  2002/12/16 23:47:59  frank
// updated for new vessel structure
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.1  2002/10/23 23:05:57  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorPathCurvedCreate.C,v 1.4 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxManipulatorPathCurvedCreate.C,v 1.4 2007/06/28 19:33:55 jmeade Exp $