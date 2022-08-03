// $Id: vxManipulatorVesselCurvedCreate.C,v 1.4 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxManipulatorVesselCurvedCreate.h"
#include "vxIDs.h"
#include "vxViewerTypes.h"
#include "vxInput.h"
#include "vxVesselUtilities.h"
#include "vxComponentUtils.h"
#include "vxEnvironment.h"
#include "vxVessel.h"
#include "vxVesselPreferences.h"
#include "vxUndoActionSegmentation.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


// static declarations
bool vxManipulatorVesselCurvedCreate::m_bFirstTime = true;


/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorVesselCurvedCreate::vxManipulatorVesselCurvedCreate(vxEnvironment & environment):
vxManipulator(environment),
m_pVessel( NULL )
{
  m_bFirstTime = true;
} // vxManipulatorVesselCurvedCreate()


/**
 * One time initialization.  This gets called at least once just before some user interaction occurs.
 * Apparently if you put it in the constructor, it is too early because not all data is available yet.
 */
void vxManipulatorVesselCurvedCreate::OneTimeInitialize()
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );

  if ( m_pVessel == NULL  || ( m_pVessel != pVessel )  )
  {

    m_bFirstTime = false;

		
  	// save the undo information - relinquish control of memory too
    vxUndoActionSegmentation * pUndo = new vxUndoActionSegmentation( GetEnvironment().GetLabelVolume(), GetEnvironment().GetComponents() );
    vxEnvironment::GetUndoActions().PushAction( pUndo );
    pUndo = NULL;

    // create a new component
    vxBlockVolume<bool> componentVol( GetEnvironment().GetIntensityVolume()->GetHeader() );
    uint2 uLabel = vxComponentUtils::CreateNewComponent( componentVol, vxComponentUtils::GetUniqueComponentName( * GetEnvironment().GetComponents(), "Curved Path" ), GetEnvironment(), true );

    // create a new vessel
    vxVessel * pNewVessel = new vxVessel();
    pNewVessel->SetMeasurementDomain( pNewVessel->GetEntireDomain() );
    pNewVessel->SetSectionInterval( -1.0F );
    pNewVessel->SetCalcificationThresholdHU( 500 );
    pNewVessel->SetLabel( uLabel );
    GetEnvironment().GetComponents()->ArrayByLabel()[ uLabel ]->SetComponentDataPtr( pNewVessel );
    GetEnvironment().GetComponents()->ArrayByLabel()[ uLabel ]->SetGrowable( false );
    GetEnvironment().GetComponents()->ArrayByLabel()[ uLabel ]->SetShrinkable( false );
    GetEnvironment().GetComponents()->ArrayByLabel()[ uLabel ]->SetLabelVisible( false );
    GetEnvironment().GetComponents()->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_ADDED ) );

    m_pVessel = pNewVessel;

    vxModifiedQueue::Flush();
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
bool vxManipulatorVesselCurvedCreate::OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState)
{
  OneTimeInitialize();

   bool bHandled(false);

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorVesselCurvedCreate", "Render" );
    return bHandled;
  }

  // make sure the vessel has some data in it
  if ( pVessel->size() == 0 )
    return bHandled;

  switch ( uKeyId )
  {
 /* case vxInput::KEY_ESCAPE:
    pVessel->clear();
    pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
    bHandled = true;
    break;*/

  // backspace
  case vxInput::KEY_BACKSPACE:
    if( pVessel->size() > 1 )
    {
      pVessel->pop_back();
      pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
      bHandled = true;
    }
    break;
  default:
    //LogDbg( ToAscii( uint4( uKeyId ) ) + " " + ToAscii( uint4( uKeyboardState ) ),"vxManipulatorVesselCurvedCreate", "OnKeyPressed");
    break; // nothing else is handled
  }

  if ( bHandled == true )
  {
    GetEnvironment().GetComponents()->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );
  }

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
bool vxManipulatorVesselCurvedCreate::OnButtonDown(const Point2D<int2> & mouseCoord, 
                                                   const ButtonEnum eButtonId, 
                                                   const uint4 uKeyboardState)
{
  OneTimeInitialize();

  bool bHandled(false);

  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorVesselCurvedCreate", "OnButtonDown" );
    return bHandled;
  }

  if ( eButtonId == BUTTON_LEFT )
  {

    // pick a 3D position for the centerline point
    Point<float4> worldCoord;

    vxViewerType3D * pViewerType3D = dynamic_cast< vxViewerType3D *>( GetEnvironment().GetViewerType() );
    if ( pViewerType3D != NULL )
    {
      std::vector<float4> profile;
      if( pViewerType3D->GetVesselLumenWorldFromMouse( mouseCoord, worldCoord, profile ) == false )
        return false;
    }
    else
    {
      if( GetEnvironment().GetViewerType()->ConvertMouseToWorld( mouseCoord, worldCoord ) == false )
        return false;
    }

    // store the centerline point in a vessel section
    vxVesselSection section;
    section.SetPosition( worldCoord );
    section.SetTanDirection( Normal3Df( 0.0F, 1.0F, 1.0F ) );
    section.SetUpDirection( Normal3Df( 1.0F, 0.0F, 1.0F ) );

    // add the section to the vessel
    pVessel->push_back( section );
    pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
    pVessel->EstimateTangents( vxVesselPreferences::GetToruosityArcLengthMM() );
    pVessel->RecalculateLengths();

    // update the GUI
    GetEnvironment().GetComponents()->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED ) );

  } // if left button.

  return bHandled;
} // OnButtonDown()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorVesselCurvedCreate::OnSetCursor()
{

  vxMouseCursor::SetCursor( "CURSOR_CURVED_MPR" );
  return true;
  
} // OnSetCursor()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVesselCurvedCreate.C,v $
// Revision 1.4  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.3.2.2  2007/05/02 19:41:12  romy
// added the UndoAction. issue#5637
//
// Revision 1.3.2.1  2007/04/16 18:25:54  geconomos
// corrected component handling
//
// Revision 1.3  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.2  2006/08/17 14:02:06  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.8  2006/07/06 15:08:42  frank
// Issue #4840: Refactored component creation and naming method
//
// Revision 1.7  2006/06/05 15:06:52  frank
// updated for visual studio 2005
//
// Revision 1.6  2006/04/25 13:16:12  frank
// Issue #4725: Fixed double numbering of components
//
// Revision 1.5  2006/02/27 21:38:10  frank
// split out component utilities
//
// Revision 1.4  2006/02/17 15:21:15  frank
// working on undoing all segmentation actions
//
// Revision 1.3  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.2.2  2005/10/25 13:46:38  frank
// Issue #4462: was missing module handle when setting cursor
//
// Revision 1.2.2.1  2005/10/05 15:24:08  frank
// Issue #4452: Added special cursor for new curved MPR tool
//
// Revision 1.2  2005/06/07 18:09:26  vxconfig
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
// Revision 1.8  2004/03/02 01:10:32  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/25 14:58:42  frank
// formatting
//
// Revision 1.7  2004/02/09 15:47:34  dongqing
// replace ID number to Enum name for all keys.
//
// Revision 1.6  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.5  2003/12/23 16:38:07  wenli
// Issue #3571: new curved MPR selection points ....
//
// Revision 1.4  2003/10/27 16:13:58  frank
// Issue #3481: Created a new component in the list when you first start creating a new curved MPR path
//
// Revision 1.3  2003/08/12 20:20:24  frank
// Moved the vessel data structure and tools to vxBase
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVesselCurvedCreate.C,v 1.4 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxManipulatorVesselCurvedCreate.C,v 1.4 2007/06/28 19:33:55 jmeade Exp $
