// $Id: vxManipulatorSliceShadows.C,v 1.6 2005/11/18 21:15:22 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


//includes
#include "stdafx.h"
#include "vxManipulatorSliceShadows.h"
#include "vxEnvironment.h"
#include "vxSliceShadowData.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxManipulatorSliceShadows::vxManipulatorSliceShadows( vxEnvironment & environment )
:vxManipulator( environment )
{
} // constructor


/**
 * This event handler to receive mouse enter events for the viewer.    
 */
void vxManipulatorSliceShadows::OnMouseEnter()
{

  SetSliceShadowPtr();

} // OnMouseEnter()


/**
 * Handle the mouse scroll events
 *
 * @param position       mouse position
 * @param iDelta         delta of mouse wheel
 * @param uKeyboardState keyboard state
 * @return handled?
 */
bool vxManipulatorSliceShadows::OnMouseWheel( const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState )
{

  SetSliceShadowPtr();

  return false;

} // OnMouseWheel()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         key id
 * @param uKeyboardState keyboard state
 * @return handled?
 */ 
bool vxManipulatorSliceShadows::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{

  SetSliceShadowPtr();

  return false;

} // OnKeyPressed()


/**
 * Event handler if the mouse is moved
 *
 * @param position       the mouse position
 * @param uKeyboardState the keyboard state
 * @return handled?
 */
bool vxManipulatorSliceShadows::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{

  SetSliceShadowPtr();

  return false;

} // OnMouseMove()


/**
 * If slice shadow is enabled, set slice shadow ptr to the current orthoplane ptr position.
 */
void vxManipulatorSliceShadows::SetSliceShadowPtr()
{
  if( GetEnvironment().GetActiveEnvironment() == NULL )
  {
    LogDbg("active environment is null", "", "" );
    return;
  }

  // We seem to have a problem with dangling pointers in the active viewer type!
  // Lets ensure that it does not interfere with the slice shadow operation.
  try
  {

    // determine if we need to update the shadow
    bool bUpdate( false );

    // make sure the slice shadow is valid
    if ( GetEnvironment().GetSliceShadow() == NULL )
    {
      LogWrn( "Null slice shadow pointer", "vxManipulatorSliceshadows", "SetSliceShadowPtr" );
    }

    // if it is already an ortho slice shadow then see it it needs updating
    if ( GetEnvironment().GetSliceShadow()->GetSliceType() == "vxPlaneOrtho<float4>" )
    {
      // check if its up to date
      vxSliceShadowDataOrtho * pOrthoData = dynamic_cast< vxSliceShadowDataOrtho * >( GetEnvironment().GetSliceShadow()->GetSliceShadowData() );
      if ( pOrthoData == NULL || ( pOrthoData->GetPlaneOrtho() != * GetEnvironment().GetPlaneOrtho() ) )
      {
        bUpdate = true;
      }
    }
    else
    {
      // it is not an ortho slice shadow so update it
      bUpdate = true;
    }

    // we need to update the data
    if ( bUpdate == true )
    {
      GetEnvironment().GetSliceShadow()->SetSliceShadowData(
        new vxSliceShadowDataOrtho( * GetEnvironment().GetPlaneOrtho() ) );
      GetEnvironment().GetSliceShadow()->SetSliceType( "vxPlaneOrtho<float4>" );
      if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_SLICE_SHADOW ) )
        GetEnvironment().GetSliceShadow()->Modified( vxModInfo( vxSliceShadow, 0 ) );
    }
  }

  catch ( ... )
  {
    // probably crashed due to a dangling pointer!
    LogWrn( "unhandled exception in SetSliceShadowPtr() - probably due to a dangling pointer in GetActiveEnvironment()",
            "vxManipulatorSliceShadows", "SetSliceShadowPtr" );
  }

} // SetSliceShadowPtr()


#undef FILE_REVISION


// $Log: vxManipulatorSliceShadows.C,v $
// Revision 1.6  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.1  2005/07/19 11:49:31  frank
// missing include
//
// Revision 1.5  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.4  2005/05/18 15:06:48  frank
// cleaning up slice shadows
//
// Revision 1.3  2001/01/03 19:30:23  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.2  2004/04/06 16:25:59  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2003/09/16 15:39:35  frank
// Added oblique slice shadow manipulator
//
// Revision 1.20  2003/09/12 14:22:50  frank
// Implemented the oblique slice shadow
//
// Revision 1.19  2003/09/10 20:59:24  dongqing
// set environment.viewerType = NULL on purpose to avoid them point
// to dangling pointer
//
// Revision 1.18  2003/09/10 20:25:58  dongqing
// // We seem to have a problem with dangling pointers in the active viewer type!
//   // Lets ensure that it does not interfere with the slice shadow operation.
//
// Temporary fix the exception at 2D VesselSelectView
//
// Revision 1.17  2003/09/09 14:17:19  frank
// Caught unhandled exception to handle the case in which the active viewer type pointer is dangling (don't let your pointer dangle!)
//
// Revision 1.16  2003/09/09 13:34:23  frank
// formatting
//
// Revision 1.15  2003/09/02 14:52:48  frank
// Changed the way slice shadows are determined - made it more flexible
//
// Revision 1.14  2003/05/20 17:40:36  frank
// code review
//
// Revision 1.13  2003/05/12 18:15:15  frank
// Added slice shadows object
//
// Revision 1.12  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.11  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.10  2002/12/17 14:38:55  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.9  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.8  2002/08/20 22:51:13  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.7  2002/08/19 20:23:37  michael
// spacing ...
//
// Revision 1.6  2002/06/14 20:01:18  michael
// cosmetics
//
// Revision 1.5  2002/05/03 14:30:03  jenny
// implemented OnMouseMove and SetSliceShadowPtr
//
// Revision 1.4  2002/05/02 15:46:03  jenny
// implemented onKeyPressed()
//
// Revision 1.3  2002/04/29 20:27:46  uday
// update mouse handling events
//
// Revision 1.2  2002/04/29 19:14:06  uday
// added OnMouseWheel()
//
// Revision 1.1  2002/04/24 16:35:58  uday
// Added initial versions of the slice shadow manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorSliceShadows.C,v 1.6 2005/11/18 21:15:22 frank Exp $
// $Id: vxManipulatorSliceShadows.C,v 1.6 2005/11/18 21:15:22 frank Exp $
