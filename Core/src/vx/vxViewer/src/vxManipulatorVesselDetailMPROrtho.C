// $Id: vxManipulatorVesselDetailMPROrtho.C,v 1.2 2007/01/16 17:04:06 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxManipulatorVesselDetailMPROrtho.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "vxElementFloat4.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"

// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorVesselDetailMPROrtho::vxManipulatorVesselDetailMPROrtho( vxEnvironment & environment )
: vxManipulator( environment ),
m_bDragging( false ),
m_bSwingForward( true )
{
} // vxManipulatorVesselDetailMPROrtho()


/**
 * Event handler if button is pressed down
 *
 * @param position       the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
*/ 
bool vxManipulatorVesselDetailMPROrtho::OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{

  if ( vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() ) == NULL )
    return false;

  if ( eButtonId == BUTTON_MIDDLE )
  {
    SetLastMousePosition( position );
    m_bDragging = true;
  } // middle button

  return m_bDragging;
} // OnButtonDown()


/**
 * Event handler if mouse button is released
 *
 * @param position       the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselDetailMPROrtho::OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{

  if ( vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() ) == NULL )
    return false;

  if( m_bDragging )
    OnMouseMove( position, uKeyboardState );

  m_bDragging = false;

  return m_bDragging;
} // OnButtonUp()


/**
 * Event handler if mouse is moved
 *
 * @param position       the screen position
 * @param uKeyboardState the keyboard state at the time of button press
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselDetailMPROrtho::OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState)
{

  if ( vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() ) == NULL )
    return false;

  if ( m_bDragging == true )
  {

    int4 yDifference( position.m_y - GetLastMousePosition().m_y );

    // go faster with special keys
    yDifference *= GetKeyMultiplier(uKeyboardState);

    TranslateSection( yDifference );

    SetLastMousePosition( position );

  } // if dragging

  return m_bDragging;
} // OnMouseMove()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselDetailMPROrtho::OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState)
{

  if ( vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() ) == NULL )
    return false;

  bool bHandled(false);

  switch (uKeyId)
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
    SetVesselSectionIndex( GetEnvironment(), 0 );
    bHandled = true;
    break;

  // End key
  case 35:
    TranslateSection( 100000 );
    bHandled = true;
    break;

  default:
    break;
  }

  return bHandled;
} // OnKeyPressed


/**
* Translate the section by a certain distance.
*
*  @param fDistance
*/
void vxManipulatorVesselDetailMPROrtho::TranslateSection( const float4 fDistance )
{
  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
    throw ex::NullPointerException( LogRec( "Vessel pointer is NULL", "vxManipulatorVesselDetailMPROrtho", "TranslateSection" ) );

  // clamp and set
  float4 fNewIndex( Bound( float4(0), float4( GetVesselSectionIndex() + fDistance ), float4( pVessel->size()-1 ) ) );
  SetVesselSectionIndex( GetEnvironment(), fNewIndex );

} // TranslateSection()


/**
 * Event handler if mouse wheel is scrolled
 *
 * @param uKeyId         the key that was pressed
 * @param iDelta         the amount the wheel was scrolled in either direction
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */
bool vxManipulatorVesselDetailMPROrtho::OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState)
{

  if ( vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() ) == NULL )
    return false;

  TranslateSection( float4( iDelta ) / 120.0F * float4( GetKeyMultiplier(uKeyboardState) ) );

  return true;

} // OnMouseWheel()


/**
 *  @return section index
 */
float4 vxManipulatorVesselDetailMPROrtho::GetVesselSectionIndex() const
{
  // get the current index pointer
  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ));
  if( pCurrentVesselSection == NULL )
  {
    LogDbg( "no current vessel section: ", "vxManipulatorVesselLuminal", "GetVesselSectionIndex" );
    return 0;
  }

  return pCurrentVesselSection->GetValue();

} // GetVesselSectionIndex()


/**
 * Retrieve the maximum possible section index
 *
 * @return the maximum possible section index
 */
float4 vxManipulatorVesselDetailMPROrtho::GetMaximumVesselSectionIndex() const
{

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorVesselDetailMPROrtho", "GetMaximumVesselSectionIndex" );
    return 0;
  }

  return pVessel->size() - 1;

} // GetMaximumVesselSectionIndex()


/**
 * Set the current section index
 *
 * @param environment   the environment
 * @param fSectionIndex the desired section index
 */
void vxManipulatorVesselDetailMPROrtho::SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex )
{
  // get the current index pointer
  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( environment.GetElement( vxIDs::CurrentVesselSection ));
  if( pCurrentVesselSection == NULL )
  {
    LogDbg( "no current vessel section: ", "vxManipulatorVesselLuminal", "SetVesselSectionIndex" );
    return;
  }

  // set the current vessel section index
  pCurrentVesselSection->SetValue( fSectionIndex );
  pCurrentVesselSection->Modified( vxModInfoID( vxElement, vxIDs::CurrentVesselSection, 0 ) );

  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorVesselDetailMPROrtho", "SetVesselSectionIndex" );
    return;
  }

  // check for a valid section index
  if ( fSectionIndex < 0.0F || fSectionIndex > pVessel->size()-1 )
    return;

  Point<float4> position( pVessel->GetResampledPosition( fSectionIndex ) );

  // set the ortho plane location
  uint1 uOrientation( environment.GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
  environment.GetPlaneOrtho()->SetDistance( position[ uOrientation ] );
  environment.SetModified( vxEnvironment::PLANE_MPR, true );

} // SetVesselSectionIndex()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVesselDetailMPROrtho.C,v $
// Revision 1.2  2007/01/16 17:04:06  gdavidson
// Removed OnTimer
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.5  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/15 17:41:11  frank
// Issue #4352: Fixed problem when no vessel is selected
//
// Revision 1.3  2005/06/08 15:00:39  vxconfig
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
// Revision 1.6  2004/03/02 01:10:32  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/25 15:28:26  frank
// formatting
//
// Revision 1.5  2004/01/15 14:23:20  frank
// Issue #3645: Added cine mode for orthogonal MPR vessel views
//
// Revision 1.4  2004/01/14 16:57:52  frank
// removed old include
//
// Revision 1.3  2003/12/22 18:54:11  frank
// Set the current vessel section extended element modified
//
// Revision 1.2  2003/08/13 20:09:02  dongqing
// replace SetDistance to SetPosition for axial view correlation
// between slice index and vessel section
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/05/01 18:49:45  dongqing
// code review
//
// Revision 1.3  2003/04/30 17:31:08  frank
// code review
//
// Revision 1.2  2003/04/30 16:56:32  frank
// code reviews
//
// Revision 1.1  2003/03/24 16:23:59  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVesselDetailMPROrtho.C,v 1.2 2007/01/16 17:04:06 gdavidson Exp $
// $Id: vxManipulatorVesselDetailMPROrtho.C,v 1.2 2007/01/16 17:04:06 gdavidson Exp $
