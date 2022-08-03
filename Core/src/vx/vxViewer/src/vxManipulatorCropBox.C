// $Id: vxManipulatorCropBox.C,v 1.13 2007/07/20 19:57:27 cqian Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorCropBox.h"
#include "vxEnvironment.h"
#include "vxMouseCursor.h"
#include "vxCropBox.h"
#include "vxUtils.h"
#include "vxViewerTypeMPROrtho.h"

// defines
#define FILE_REVISION "$Revision: 1.13 $"


/**
 * Constructor
 */
vxManipulatorCropBox::vxManipulatorCropBox( vxEnvironment & environment )
: vxManipulator( environment )
{
} // vxManipulatorCropBox()


/*
 * Event handler if the mouse is pressed
 * @param mouse position
 * @param mouse button
 * @param state of the mouse
 * @return handled?
 */ 
bool vxManipulatorCropBox::OnButtonDown( const Point2D<int2> & mousePosition, const ButtonEnum eButtonId, 
                                         const uint4 uKeyboardState )
{

  // exit if there is no volume
  uint2 iX( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().X() );
  uint2 iY( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().Y() );
  uint2 iZ( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().Z() );
  if ( iX * iY * iZ == 0 ) return false;

  // early return if no crop box
  if ( GetEnvironment().GetCropBox() == NULL ) throw ex::AbortDataProcessingException( LogRec("GetEnvironment().GetCropBox() == NULL","","") );

  if ( eButtonId == BUTTON_LEFT )
  {

    // select a handle or nothing (-1)
    GetEnvironment().GetCropBox()->SetActiveHandle( GetTouchingHandle( mousePosition ) );
    UpdateCursor( GetEnvironment().GetCropBox()->GetActiveHandle() );

    // if selected a handle
    if ( GetEnvironment().GetCropBox()->GetActiveHandle() != -1 )
    {
      GetEnvironment().GetCropBox()->Modified( vxModInfo( vxCropBox, vxUtils::CROPBOX_MODIFIED ) );
      GetEnvironment().SetUserInteracting( true );
    }

  } // if left button down
  
  return GetEnvironment().GetCropBox()->GetActiveHandle() != -1;

} // OnButtonDown()


/**
 * get the index of the handle under the cursor, if any exists
 *
 * @param position the mouse position
 *
 * @return handle index
 */
int4 vxManipulatorCropBox::GetTouchingHandle( const Point2D<int2> & position ) const
{

  // the selected handle
  int4 iSelectedHandleIndex( -1 );

  // get the screen coordinates of the mouse position
  Point2D<int4> mouseScreen( GetEnvironment().GetViewerType()->ConvertMouseToScreen( position ) );

  // get the screen coordinates of the bounding box of the handle positions
  BoundingRect<int2> handleRect;
  for( uint4 i(0); i<GetEnvironment().GetCropBox()->GetNumHandles(); ++i )
  {

    // get the position of the handle point in world coordinates
    Point3Df fHandleWorld( GetEnvironment().GetCropBox()->GetHandlePosition( i ) );

    // convert to screen coordinates
    Point2D<int2> handleScreen;
    if ( GetEnvironment().GetViewerType()->ConvertWorldToScreen( fHandleWorld, handleScreen ) )
    {

      if ( i == 0 )
      {
        // first time, reset the rectangle bounds
        handleRect = BoundingRect<int2>( handleScreen.m_x, handleScreen.m_y, handleScreen.m_x, handleScreen.m_y );
      }
      else
      {
        // every time after that, calculate the union
        handleRect.Union( Point3D<int2>( handleScreen.m_x, handleScreen.m_y, 0 ) );
      }

    } // if converted to screen coordinates

  } // for all the crop box handles

  const float4 fThresholdPixels( 5 );

  // test for touching the top edge
  BoundingRect<int2> selectionArea( handleRect.GetMinX() + fThresholdPixels, handleRect.GetMaxY() - fThresholdPixels,
                                    handleRect.GetMaxX() - fThresholdPixels, handleRect.GetMaxY() + fThresholdPixels );
  const uint4 uAxisIndex( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = ( uAxisIndex == 2 ) ? 1 : 7;

  // test for touching the bottom edge
  selectionArea = BoundingRect<int2>( handleRect.GetMinX() + fThresholdPixels, handleRect.GetMinY() - fThresholdPixels,
                                      handleRect.GetMaxX() - fThresholdPixels, handleRect.GetMinY() + fThresholdPixels );
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = ( uAxisIndex == 2 ) ? 7 : 1;

  // test for touching the left edge
  selectionArea = BoundingRect<int2>( handleRect.GetMinX() - fThresholdPixels, handleRect.GetMinY() + fThresholdPixels,
                                      handleRect.GetMinX() + fThresholdPixels, handleRect.GetMaxY() - fThresholdPixels );
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = 3;

  // test for touching the right edge
  selectionArea = BoundingRect<int2>( handleRect.GetMaxX() - fThresholdPixels, handleRect.GetMinY() + fThresholdPixels,
                                      handleRect.GetMaxX() + fThresholdPixels, handleRect.GetMaxY() - fThresholdPixels );
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = 5;

  // test for touching the top left corner
  if ( uAxisIndex == 2 )
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMinX() - fThresholdPixels, handleRect.GetMaxY() - fThresholdPixels,
                                        handleRect.GetMinX() + fThresholdPixels, handleRect.GetMaxY() + fThresholdPixels );
  }
  else
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMinX() - fThresholdPixels, handleRect.GetMinY() - fThresholdPixels,
                                        handleRect.GetMinX() + fThresholdPixels, handleRect.GetMinY() + fThresholdPixels );
  }
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = 0;

  // test for touching the top right corner
  if ( uAxisIndex == 2 )
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMaxX() - fThresholdPixels, handleRect.GetMaxY() - fThresholdPixels,
                                        handleRect.GetMaxX() + fThresholdPixels, handleRect.GetMaxY() + fThresholdPixels );
  }
  else
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMaxX() - fThresholdPixels, handleRect.GetMinY() - fThresholdPixels,
                                        handleRect.GetMaxX() + fThresholdPixels, handleRect.GetMinY() + fThresholdPixels );
  }
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = 2;

  // test for touching the bottom left corner
  if ( uAxisIndex == 2 )
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMinX() - fThresholdPixels, handleRect.GetMinY() - fThresholdPixels,
                                        handleRect.GetMinX() + fThresholdPixels, handleRect.GetMinY() + fThresholdPixels );
  }
  else
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMinX() - fThresholdPixels, handleRect.GetMaxY() - fThresholdPixels,
                                        handleRect.GetMinX() + fThresholdPixels, handleRect.GetMaxY() + fThresholdPixels );
  }
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = 6;

  // test for touching the bottom right corner
  if ( uAxisIndex == 2 )
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMaxX() - fThresholdPixels, handleRect.GetMinY() - fThresholdPixels,
                                        handleRect.GetMaxX() + fThresholdPixels, handleRect.GetMinY() + fThresholdPixels );
  }
  else
  {
    selectionArea = BoundingRect<int2>( handleRect.GetMaxX() - fThresholdPixels, handleRect.GetMaxY() - fThresholdPixels,
                                        handleRect.GetMaxX() + fThresholdPixels, handleRect.GetMaxY() + fThresholdPixels );
  }
  if ( selectionArea.IsInside( mouseScreen.m_x, mouseScreen.m_y ) )
    iSelectedHandleIndex = 8;

  // test for touching the center of the box
  const float4 fDistanceCenter( sqrt( pow( static_cast< float4 >( mouseScreen.m_x - handleRect.GetMidX() ), 2.0F ) + pow( static_cast< float4 >( mouseScreen.m_y - handleRect.GetMidY() ), 2.0F ) ) );
  if ( fDistanceCenter < fThresholdPixels )
    iSelectedHandleIndex = 4;

  return iSelectedHandleIndex;

} // GetTouchingHandle()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorCropBox::OnSetCursor()
{

  bool bHandled( false );

  // only show cursor when over top a handle and on an ortho slice
  if ( typeid( * GetEnvironment().GetViewerType() ) == typeid( vxViewerTypeMPROrtho ) )
  {
  
    UpdateCursor( GetEnvironment().GetCropBox()->GetActiveHandle() );
    bHandled = GetEnvironment().GetCropBox()->GetActiveHandle() != -1;
    
  } // if over a handle and on ortho slice

  return bHandled;
  
} // OnSetCursor()


/**
 * Sets the cursor for a specific handle
 */
void vxManipulatorCropBox::UpdateCursor( int4 iHandleIndex ) const
{

  // if a valid handle is not selected
  if ( iHandleIndex < 0 )
  {
  
    vxMouseCursor::SetCursor("CURSOR_DEFAULT");
    
  } // if a valid handle is not selected
  else
  {

    // a valid handle is selected
    
    // determine the plane orientation
    if ( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() == 2 )
    {

      // axial cursors
      switch ( iHandleIndex )
      {
      case 0: vxMouseCursor::SetCursor( "CURSOR_NWSE" ); break;
      case 1: vxMouseCursor::SetCursor( "CURSOR_NS"   ); break;
      case 2: vxMouseCursor::SetCursor( "CURSOR_NESW" ); break;
      case 3: vxMouseCursor::SetCursor( "CURSOR_WE"   ); break;
      case 4: vxMouseCursor::SetCursor( "CURSOR_MOVE" ); break;
      case 5: vxMouseCursor::SetCursor( "CURSOR_WE"   ); break;
      case 6: vxMouseCursor::SetCursor( "CURSOR_NESW" ); break;
      case 7: vxMouseCursor::SetCursor( "CURSOR_NS"   ); break;
      case 8: vxMouseCursor::SetCursor( "CURSOR_NWSE" ); break;
      default: break;
      } // switch on active handle

    } // if axial
    else
    {

      // sagittal or coronal cursors
      switch ( iHandleIndex )
      {
      case 0: vxMouseCursor::SetCursor( "CURSOR_NESW" ); break;
      case 1: vxMouseCursor::SetCursor( "CURSOR_NS"   ); break;
      case 2: vxMouseCursor::SetCursor( "CURSOR_NWSE" ); break;
      case 3: vxMouseCursor::SetCursor( "CURSOR_WE"   ); break;
      case 4: vxMouseCursor::SetCursor( "CURSOR_MOVE" ); break;
      case 5: vxMouseCursor::SetCursor( "CURSOR_WE"   ); break;
      case 6: vxMouseCursor::SetCursor( "CURSOR_NWSE" ); break;
      case 7: vxMouseCursor::SetCursor( "CURSOR_NS"   ); break;
      case 8: vxMouseCursor::SetCursor( "CURSOR_NESW" ); break;
      default: break;
      } // switch on active handle

    } // if not axial

  } // a valid handle is selected


} // UpdateCursor()


/**
 * This event handler receives mouse move events for the viewer    
 */
void vxManipulatorCropBox::OnMouseLeave()
{

  GetEnvironment().GetCropBox()->SetActiveHandle( -1 );
  
} // OnMouseLeave()


/**
 * Event handler if the mouse is moved
 * @param relative Mouse position
 * @param mouse state.
 * @return handled?
 */ 
bool vxManipulatorCropBox::OnMouseMove( const Point2D<int2> & mousePosition, const uint4 uKeyboardState )
{

  // set the cursor when over top of a handle
  if ( GetEnvironment().GetCropBox()->GetActiveHandle() == -1 )
  {
    UpdateCursor( GetTouchingHandle( mousePosition ) );
  }
  else
  {
    UpdateCursor( GetEnvironment().GetCropBox()->GetActiveHandle() );
  }

  bool bIfHandled( false );

  if ( GetEnvironment().GetCropBox() != NULL &&
       GetEnvironment().GetCropBox()->GetActiveHandle() != -1 )
  {

    bIfHandled = true;

    // NOTE: MouseToScreen clamps for you
    Point2D<int2> mousePositionClamped(mousePosition);
    mousePositionClamped.m_x = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().first), (int2)(mousePosition.m_x), 
                                     (int2)(GetEnvironment().GetWinRect().GetViewportSize().first-1));
    mousePositionClamped.m_y = Bound((int2)(GetEnvironment().GetWinRect().GetReferencePoint().second), (int2)(mousePosition.m_y), 
                                     (int2)(GetEnvironment().GetWinRect().GetViewportSize().second-1));

    // get the world coordinates of the mouse position
    Point<float4> mouseWorldPos;
    if ( GetEnvironment().GetViewerType()->ConvertMouseToWorld( mousePositionClamped, mouseWorldPos ) == false ) return false;

    Box<float4> fWorldBox( Point<float4>(0,0,0), Point<float4>( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() ) );
    
    // move the handle point to the desired location
    int1 iOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
    GetEnvironment().GetCropBox()->SetHandlePosition( mouseWorldPos, GetEnvironment().GetCropBox()->GetActiveHandle(), iOrientation, fWorldBox );
    GetEnvironment().GetCropBox()->Modified( vxModInfo( vxCropBox, vxUtils::CROPBOX_MODIFIED ) );
    
  } // if an active handle

  return bIfHandled;

} // OnMouseMove()


/**
 * Event handler if the mouse is released
 * @param mouse position
 * @param mouse button
 * @param keyboard state
 * @return handled?
 */ 
bool vxManipulatorCropBox::OnButtonUp(const Point2D<int2> & position, 
                                      const ButtonEnum eButtonId, 
                                      const uint4 uKeyboardState)
{
 
  // exit if there is no volume
  uint2 iX( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().X() );
  uint2 iY( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().Y() );
  uint2 iZ( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().Z() );
  if ( iX * iY * iZ == 0 ) return false;

  bool bIfHandled(false);

  if ( eButtonId == BUTTON_LEFT && GetEnvironment().GetCropBox()->GetActiveHandle() != -1 )
  {

    bIfHandled = true;
    GetEnvironment().GetCropBox()->SetActiveHandle( -1 );
    GetEnvironment().GetCropBox()->Modified( vxModInfo( vxCropBox, vxUtils::CROPBOX_MODIFIED ) );
    GetEnvironment().SetUserInteracting( false );
  }

  return bIfHandled;

} // OnButtonUp()


#undef FILE_REVISION


// $Log: vxManipulatorCropBox.C,v $
// Revision 1.13  2007/07/20 19:57:27  cqian
// add mesh rendering
//
// Revision 1.12  2006/08/03 14:01:35  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.11  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.10  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.9  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.8  2005/05/23 12:48:20  frank
// fixed file revision definition so that we get correct information in log files
//
// Revision 1.7  2005/05/16 13:49:42  frank
// made the crop box easier to handle
//
// Revision 1.6  2004/04/06 16:25:11  frank
// unified environment access naming
//
// Revision 1.5  2004/03/31 17:35:14  frank
// unused include files
//
// Revision 1.4  2004/03/25 18:36:53  frank
// added special cursors to the crop box handles
//
// Revision 1.3  2004/03/12 19:50:51  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/11 17:41:01  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.34  2003/08/12 17:00:08  michael
// Took out the GetWorldCoord() from manipulator since it is part of viewer type
//
// Revision 1.33  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.32  2003/06/05 13:01:00  michael
// fixed bug I introduced myself while addressing the empty volume ...
//
// Revision 1.31  2003/06/03 20:30:54  michael
// added check to see if volume is valid (each dimension > 0)
//
// Revision 1.30  2003/05/22 13:04:28  frank
// code review
//
// Revision 1.29  2003/05/16 18:38:39  michael
// code review
//
// Revision 1.28  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.27  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.26  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.25  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.24  2003/03/27 16:44:48  frank
// Issue #2907: Crop box should be made visible any time you adjust it.
//
// Revision 1.23  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.22  2003/02/03 15:25:17  frank
// cosmetics
//
// Revision 1.21  2003/01/22 21:56:26  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.20  2003/01/06 17:13:55  michael
// formatting
//
// Revision 1.19  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.18  2002/11/02 03:09:30  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.17  2002/09/26 18:10:38  dmitry
// CUrsors for something other then Axial view for the crop box implemented.
//
// Revision 1.16  2002/09/24 15:33:15  frank
// Clarified variable names.
//
// Revision 1.15  2002/09/23 15:17:26  dmitry
// Cusror manipulation fixed up.
//
// Revision 1.14  2002/09/20 21:10:33  geconomos
// Implemented new mouse cursors from crop box.
//
// Revision 1.13  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.12  2002/09/20 16:09:01  michael
// changed name to GetWorldCoordinateOpenGL()
//
// Revision 1.11  2002/09/17 21:42:37  dmitry
// Cursors added.
//
// Revision 1.10  2002/09/16 19:59:03  dmitry
// New vxID (s) used.
//
// Revision 1.9  2002/08/20 22:31:34  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.8  2002/08/09 20:54:25  uday
// GetHandlePoint() takes the volume dimensions into account.
//
// Revision 1.7  2002/08/08 16:00:06  uday
// Removed display m_longAxis.
//
// Revision 1.6  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.5  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.4  2002/07/15 15:21:40  geconomos
// Removed virtually inherited classes.
//
// Revision 1.3.2.1  2002/07/11 22:37:05  michael
// took out the MakeCurrent() since this is ensured by V3D_Viewer and vxViewer
//
// Revision 1.3  2002/06/26 20:19:59  manju
// Modified OnButtonDown method.
//
// Revision 1.2  2002/06/25 17:32:13  manju
// Slight modifications.
//
// Revision 1.1  2002/06/11 14:55:38  manju
// Manipulator for crop box.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCropBox.C,v 1.13 2007/07/20 19:57:27 cqian Exp $
// $Id: vxManipulatorCropBox.C,v 1.13 2007/07/20 19:57:27 cqian Exp $
