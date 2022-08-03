// $Id: vxManipulatorContourAdjust.C,v 1.3 2006/06/01 13:41:26 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Manjushree Nulkar (mailto:manju@viatronix.com)


// includes
#include "stdafx.h"
#include <queue>
#include "vxManipulatorUtils.h"
#include "vxManipulatorContourAdjust.h"
#include "vxBlockVolume.h"
#include "vxContourList.h"
#include "vxMouseCursor.h"
#include "vxContour.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxMessageBox.h"
#include "vxUndoActionSegmentation.h"
#include "vxComponentUtils.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxUndoActionSegmentation.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorContourAdjust::vxManipulatorContourAdjust( vxEnvironment & environment )
: vxManipulator( environment ),
m_bActive( false ),
m_uStartLabelIndex( 0 ),
m_eDrawingStage( BEFORE_ADJUST )
{
} // vxManipulatorContourAdjust()


/**
 * Destructor
 */
vxManipulatorContourAdjust::~vxManipulatorContourAdjust()
{
} // ~vxManipulatorContour()


/**
 * Public method to process button down event
 *
 * @param position mouse position
 * @param eButtonId mouse button enum
 * @param uKeyboardState state of the mouse
 * @return handled
 */ 
bool vxManipulatorContourAdjust::OnButtonDown( const Point2D<int2> & mousePosition, 
                                               const ButtonEnum eButtonId, 
                                               const uint4 uKeyboardState )
{

  m_bActive = false;
  m_eDrawingStage = BEFORE_ADJUST;
  const bool bOrthoMPRView( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType() ) != NULL );
  if ( ! bOrthoMPRView )
    return false;

  Point<float4> worldPos;
  if ( eButtonId == BUTTON_LEFT &&
       GetEnvironment().GetWinRect().IsInside( mousePosition ) &&
       GetEnvironment().GetViewerType()->ConvertMouseToWorld( mousePosition, worldPos, false ) )
  {

    vxBlockVolumeIterator<uint2> labelIter( * GetEnvironment().GetLabelVolume() );
    Vector3Di volumePos = Vector3Di( Vector3Df( worldPos ).GetDividedAnIso( GetEnvironment().GetLabelVolume()->GetHeader().GetVoxelUnits() ) );
    labelIter.SetPos( volumePos );
    if ( labelIter.IsInsideVolume() )
    {
      m_uStartLabelIndex = labelIter.GetVoxel();
      m_bActive = true;
      m_eDrawingStage = BEFORE_ADJUST;
      GetEnvironment().GetContourList()->Clear();
      GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().clear();
      m_edgeTrackVolume.clear();
      m_drawnContourVolume.clear();
      m_lastVolumePosition = volumePos;
    }

  } // if left button. 

  return m_bActive;

} // OnButtonDown()


/**
 * Event handler if a key is pressed.
 *
 * @param uKeyId the key pressed.
 * @param uKeyboardState key state.
 * @return handled?
 */ 
bool vxManipulatorContourAdjust::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  bool bHandled( false );
  
  switch( uKeyId )
  {
  case vxInput::KEY_ESCAPE:
    if ( m_eDrawingStage == BEFORE_ADJUST )
    {
      m_bActive = false;
      // notify the feedback so that manipulator could be removed
      GetEnvironment().GetInputFeedback().Add( vxFeedback( typeid( * this ), vxFeedback::DONE ) );
      GetEnvironment().GetContourList()->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );
    }
    else
    {
      m_bActive = false;
      m_eDrawingStage = BEFORE_ADJUST;
      GetEnvironment().GetContourList()->Clear();
      GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().clear();
      GetEnvironment().GetContourList()->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );
      m_edgeTrackVolume.clear();
      m_drawnContourVolume.clear();
    }
    bHandled = true;
    break;

  default:
    break;
  } // switch

  return bHandled;
} // OnKeyPressed()


/**
 * Determines if the given voxel position is on the outside edge of a component
 *
 * @param labelIter           an iterator for the label volume
 * @param queryPositionVolume the position to test (in volume coordinates)
 * @param uComponentLabel     the label value of the component in question
 *
 * @return if on the outside
 */
bool vxManipulatorContourAdjust::IsOnOutsideEdgeOfComponent( vxBlockVolumeIterator<uint2> & labelIter, const Vector3Di & queryPositionVolume,
                                                             const uint2 & uComponentLabel )
{

  // check the center voxel
  labelIter.SetPos( queryPositionVolume );
  const uint2 & uCenterLabel = labelIter.GetVoxel();
  if ( uCenterLabel == uComponentLabel )
    return false;

  const uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
  if ( uOrientation == 0 )
  {
    if (
        // not all peeks are available yet
        labelIter.Peek0px1ny1nz() == uComponentLabel ||
        labelIter.Peek0px0py1nz() == uComponentLabel ||
        labelIter.PeekRel( 0, 1, -1 ) == uComponentLabel ||
        labelIter.Peek0px1ny0pz() == uComponentLabel ||
        labelIter.Peek0px1py0pz() == uComponentLabel ||
        labelIter.PeekRel( 0, -1, 1 ) == uComponentLabel ||
        labelIter.Peek0px0py1pz() == uComponentLabel ||
        labelIter.Peek0px1py1pz() == uComponentLabel
      )
      return true;
  }
  else if ( uOrientation == 1 )
  {
    if (
        labelIter.Peek1nx0py1nz() == uComponentLabel ||
        labelIter.Peek0px0py1nz() == uComponentLabel ||
        labelIter.PeekRel( 1, 0, -1 ) == uComponentLabel ||
        labelIter.Peek1nx0py0pz() == uComponentLabel ||
        labelIter.Peek1px0py0pz() == uComponentLabel ||
        labelIter.PeekRel( -1, 0, 1 ) == uComponentLabel ||
        labelIter.Peek0px0py1pz() == uComponentLabel ||
        labelIter.Peek1px0py1pz() == uComponentLabel
      )
      return true;
  }
  else if ( uOrientation == 2 )
  {
    if (
        labelIter.Peek1nx1ny0pz() == uComponentLabel ||
        labelIter.Peek1nx0py0pz() == uComponentLabel ||
        labelIter.PeekRel( -1, 1, 0 ) == uComponentLabel ||
        labelIter.Peek0px1ny0pz() == uComponentLabel ||
        labelIter.Peek0px1py0pz() == uComponentLabel ||
        labelIter.PeekRel( 1, -1, 0 ) == uComponentLabel ||
        labelIter.Peek1px0py0pz() == uComponentLabel ||
        labelIter.Peek1px1py0pz() == uComponentLabel
      )
      return true;
  }

  return false;

} // IsOnOutsideEdgeOfComponent()


/**
 * get the proper neighborhood for processing (either 4 or 8-connected)
 *
 * @param bFourConnected true for 4-connected neighborhood or false for 8-connected
 *
 * @return the proper neighborhood
 */
enum vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum vxManipulatorContourAdjust::GetNeighborhood( const bool bFourConnected )
{
  const uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );
  enum vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum eNeighborhood = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xy;
  switch ( uOrientation )
  {
  case 0:
    eNeighborhood = bFourConnected ? vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04yz : vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08yz;
  	break;
  case 1:
    eNeighborhood = bFourConnected ? vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04xz : vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xz;
  	break;
  case 2:
    eNeighborhood = bFourConnected ? vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors04xy : vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xy;
  	break;
  default:
    throw ex::IllegalArgumentException( LogRec( "Unrecognized plane orientation", "vxManipulatorContourAdjust", "GetNeighborhoodFourConnected" ) );
  	break;
  }

  return eNeighborhood;
} // GetNeighborhood()


/**
 * create a contour around the existing component
 *
 * @param startPointVolume the starting point in volume coordinates
 */
void vxManipulatorContourAdjust::GenerateContour( const Vector3Di & startPointVolume )
{

  // start fresh
  m_existingContourVolume.clear();

  // search for a starting point just outside the component but touching
  Vector3Di justOutsidePointVolume = startPointVolume;
  bool bFound = false;
  vxBlockVolumeIterator<uint2> labelIterInner( * GetEnvironment().GetLabelVolume() );
  for ( vxBlockVolumeIteratorNeighbors<uint2> neighborIter1( * GetEnvironment().GetLabelVolume(), Vector3Di( startPointVolume ),
                                                             GetNeighborhood( false ) );
        ! neighborIter1.IsAtEnd();
        ++neighborIter1 )
  {
    if ( IsOnOutsideEdgeOfComponent( labelIterInner, neighborIter1.GetPos(), m_uStartLabelIndex ) )
    {
      justOutsidePointVolume = neighborIter1.GetPos();
      bFound = true;
    }
  }
  if ( ! bFound )
  {
    LogDbg( "Unable to find outside contour of component", "vxManipulatorContourAdjust", "GenerateContour" );
    return;
  }

  vxBlockVolume<bool> maskVolume( GetEnvironment().GetLabelVolume()->GetHeader() );
  vxBlockVolumeIterator<bool> maskIter( maskVolume );
  std::queue<Vector3Di> paths;
  paths.push( justOutsidePointVolume );
  maskIter.SetPos( justOutsidePointVolume );
  maskIter.SetVoxel( true );
  int max = 100000;
  vxBlockVolumeIteratorNeighbors<uint2> neighborIter( * GetEnvironment().GetLabelVolume(), Vector3Di( 0, 0, 0 ),
                                                      GetNeighborhood( false ) );
  while ( ! paths.empty() && m_existingContourVolume.size() < max )
  {
    Vector3Di position = paths.front();
    paths.pop();
    m_existingContourVolume.push_back( Point3Di( position ) );
    const Point3Df positionWorld = Point3Df( Vector3Df( position ).GetScaledAnIso( GetEnvironment().GetLabelVolume()->GetHeader().GetVoxelUnits() ) );
    GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().push_front( positionWorld );

    // search for nearby positions that are not already discovered
    for ( neighborIter.CenterAt( position );
          ! neighborIter.IsAtEnd();
          ++neighborIter )
    {
      if ( IsOnOutsideEdgeOfComponent( labelIterInner, neighborIter.GetPos(), m_uStartLabelIndex ) )
      {
        // see if we already discovered this one
        maskIter.SetPos( neighborIter.GetPos() );
        if ( maskIter.GetVoxel() == false )
        {
          maskIter.SetVoxel( true );
          paths.push( neighborIter.GetPos() );
        }
      }
    }

  }

  LogDbg( "found edge voxels: " + ToAscii( m_existingContourVolume.size() ), "", "" );

} // GenerateContour()


/**
 * Event handler if the mouse is moved
 * @param position Relative Mouse position
 * @param uKeyboardState mouse state
 * @return handled?
 */
bool vxManipulatorContourAdjust::OnMouseMove( const Point2D<int2> & mousePosition, const uint4 uKeyboardState )
{

  if ( ! m_bActive )
    return false;

  // check for a valid point
  Point<float4> worldPos;
  if ( m_bActive &&
       GetEnvironment().GetWinRect().IsInside( mousePosition ) &&
       GetEnvironment().GetViewerType()->ConvertMouseToWorld( mousePosition, worldPos, false ) )
  {

    // see where we are now in the volume
    Vector3Di volumePos = Vector3Di( Vector3Df( worldPos ).GetDividedAnIso( GetEnvironment().GetLabelVolume()->GetHeader().GetVoxelUnits() ) );
    if ( volumePos == m_lastVolumePosition )
      return false;

    // see how much we have moved since the last time and break it down into single-voxel movements
    const float4 fDistance = ( Point3Df( volumePos ) - Point3Df( m_lastVolumePosition ) ).GetLength();
    const Point3Di startPosition = Point3Di( m_lastVolumePosition );
    if ( fDistance > 0.5F )
    {
      for ( float fInterpolate = 0; fInterpolate < fDistance; fInterpolate += 0.5F )
      {
        const float4 fFactor = fInterpolate / fDistance;
        Point3Df newPositionVolume = Point3Df( Vector3Df( startPosition ) * ( 1 - fFactor ) + Vector3Df( volumePos ) * fFactor );
        if ( Vector3Di( newPositionVolume ) != m_lastVolumePosition )
        {
          m_lastVolumePosition = Vector3Di( newPositionVolume );
          OnePixelMove( Vector3Di( m_lastVolumePosition ) );
        }
      }
    }
    else
    {
      OnePixelMove( Vector3Di( volumePos ) );
    }
    return true;
  }

  return false;
}


/**
 * Perform one pixel's movement worth of work.
 * On mouse move only gets called infrequently so we break up all the movement into
 * single-pixel movements and call this function.
 *
 * @param volumePosition the position at which to perform the work
 */
void vxManipulatorContourAdjust::OnePixelMove( const Vector3Di & volumePosition )
{

  vxBlockVolumeIterator<uint2> labelIter( * GetEnvironment().GetLabelVolume() );
  labelIter.SetPos( volumePosition );
  if ( labelIter.IsInsideVolume() )
  {

    if ( m_eDrawingStage == BEFORE_ADJUST )
    {
      if ( labelIter.GetVoxel() != m_uStartLabelIndex )
      {
        m_eDrawingStage = DURING_ADJUST;

        GenerateContour( labelIter.GetPos() );

        // TODO better search for first intersection point and add it
        m_edgeTrackVolume.push_back( Point3Di( GetNearestEdgePoint( labelIter.GetPos() ) ) );
        const Point3Df volumePos( m_edgeTrackVolume.front() );
        m_drawnContourVolume.push_back( Vector3Di( m_edgeTrackVolume.front() ) );
        const Point3Df worldPos( volumePosition.GetScaledAnIso( GetEnvironment().GetLabelVolume()->GetHeader().GetVoxelUnits() ) );
        GetEnvironment().GetContourList()->GetAdjustmentContour().AddPoint( worldPos );
        GetEnvironment().GetContourList()->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );
      }
    }

    if ( m_eDrawingStage == DURING_ADJUST )
    {

      if ( labelIter.GetVoxel() == m_uStartLabelIndex )
      {
        m_eDrawingStage = AFTER_ADJUST;
      }
      else
      {
        const Point3Df volumePos( volumePosition );
        const Point3Df worldPos( volumePos.GetScaledAnIso( GetEnvironment().GetLabelVolume()->GetHeader().GetVoxelUnits() ) );
        GetEnvironment().GetContourList()->GetAdjustmentContour().AddPoint( worldPos );
        GetEnvironment().GetContourList()->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );
        m_drawnContourVolume.push_back( volumePosition );
        //ExtendEdgeContour( labelIter.GetPos() );
      }

    }
    
  }

} // OnePixelMove()


/**
 * Extend the edge contour to be nearer to the given point
 *
 * @param startPointVolume the position to extend toward
 */
void vxManipulatorContourAdjust::ExtendEdgeContour( const Vector3Di & startPointVolume )
{

  //
  // first try to backtrack and see if we get closer to the starting point
  //
  const int4 iMaximumTries = 10;
  for ( int i=0; i<iMaximumTries && m_edgeTrackVolume.size() > 2; i++ )
  {

    // get the current distance
    const float4 fCurrentDistance = ( Point3Df( startPointVolume ) - Point3Df( m_edgeTrackVolume.back() ) ).GetLength();

    // check if it gets closer when we move one backward one in the stack
    const float4 fPreviousDistance = ( Point3Df( startPointVolume ) - Point3Df( m_edgeTrackVolume[ m_edgeTrackVolume.size() - 2 ] ) ).GetLength();
    if ( fPreviousDistance < fCurrentDistance )
    {
      m_edgeTrackVolume.pop_back();
      GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().pop_front();
    }

    // check if it gets closer when we move one backward two in the stack
    const float4 fPreviousDistance2 = ( Point3Df( startPointVolume ) - Point3Df( m_edgeTrackVolume[ m_edgeTrackVolume.size() - 3 ] ) ).GetLength();
    if ( fPreviousDistance2 < fCurrentDistance )
    {
      m_edgeTrackVolume.pop_back();
      m_edgeTrackVolume.pop_back();
      GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().pop_front();
      GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().pop_front();
    }

  }

  bool bBetter = false;
  for ( int i=0; i<iMaximumTries; i++ )
  {

    // get the current distance
    const float4 fCurrentDistance = ( Point3Df( startPointVolume ) - Point3Df( m_edgeTrackVolume.back() ) ).GetLength();

    // check if it gets closer when we move one step in another direction
    float4 fBetterDistance = fCurrentDistance;
    Vector3Di betterPositionVolume( m_edgeTrackVolume.back() );
    for ( vxBlockVolumeIteratorNeighbors<uint2> neighborIter( * GetEnvironment().GetLabelVolume(), Vector3Di( m_edgeTrackVolume.back() ),
                                                              GetNeighborhood( false ) );
          ! neighborIter.IsAtEnd();
          ++neighborIter )
    {
      if ( neighborIter.GetVoxel() == m_uStartLabelIndex && neighborIter.GetPos() != betterPositionVolume )
      {
        const float4 fNewDistance = ( Point3Df( startPointVolume ) - Point3Df( neighborIter.GetPos() ) ).GetLength();
        if ( fNewDistance < fBetterDistance )
        {
          fBetterDistance = fNewDistance;
          betterPositionVolume = neighborIter.GetPos();
          bBetter = true;
        }
      }
    }

    if ( bBetter )
    {
      const Vector3Df betterPositionWorld = Vector3Df( betterPositionVolume ).GetScaledAnIso( GetEnvironment().GetLabelVolume()->GetHeader().GetVoxelUnits() );
      GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().push_front( Point3Df( betterPositionWorld ) );
      m_edgeTrackVolume.push_back( Point3Di( betterPositionVolume ) );
    }

  }

} // ExtendEdgeContour()


/**
 * Search for the nearest edge point
 *
 * @param startPointVolume the point to start searching from (in volume coordinates)
 *
 * @return the nearest point on the edge of the selected segment (in volume coordinates)
 */
Vector3Di vxManipulatorContourAdjust::GetNearestEdgePoint( const Vector3Di & startPointVolume )
{

  for ( vxBlockVolumeIteratorNeighbors<uint2> neighborIter( * GetEnvironment().GetLabelVolume(), startPointVolume,
                                                            GetNeighborhood( false ) );
        ! neighborIter.IsAtEnd();
        ++neighborIter )
  {
    if ( neighborIter.GetVoxel() == m_uStartLabelIndex )
    {
      return neighborIter.GetPos();
    }
  }

  return startPointVolume;

} // GetNearestEdgePoint()


/**
 * Public method to process button down event
 *
 * @param position mouse position
 * @param eButtonId mouse button enum
 * @param uKeyboardState state of the mouse
 * @return handled?
 */ 
bool vxManipulatorContourAdjust::OnButtonUp( const Point2D<int2> & mousePosition, 
                                             const ButtonEnum eButtonId, 
                                             const uint4 uKeyboardState )
{

  if ( ! m_bActive )
    return false;

  m_bActive = false;

  if ( m_eDrawingStage == AFTER_ADJUST )
  {
    m_eDrawingStage = BEFORE_ADJUST;
    GetEnvironment().GetContourList()->Clear();
    GetEnvironment().GetContourList()->GetAdjustmentContour().GetContourPtVector().clear();

    // save the current state for undo purposes and relinquish control of memory
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionSegmentation( GetEnvironment().GetLabelVolume(),
      GetEnvironment().GetComponents() ) );

    vxBlockVolumeIterator<uint2> labelIter( * GetEnvironment().GetLabelVolume() );
    for ( int i=0; i<m_drawnContourVolume.size(); i++ )
    {
      labelIter.SetPos( m_drawnContourVolume[i] );
      labelIter.SetVoxel( m_uStartLabelIndex );
    }

    if ( m_drawnContourVolume.size() > 1 && m_existingContourVolume.size() > 0 )
    {
      const Vector3Di middlePositionVolume = m_drawnContourVolume[ m_drawnContourVolume.size() / 2 ];

      // compute the nearest edge point
      int iNearest = 0;
      float fNearest = (std::numeric_limits<float>::max)();
      for ( int i=0; i<m_existingContourVolume.size(); i++ )
      {
        const float4 fDistance = ( Point3Df( middlePositionVolume ) - Point3Df( m_existingContourVolume[i] ) ).GetLength();
        if ( fDistance < fNearest )
        {
          fNearest = fDistance;
          iNearest = i;
        }
      }

      // fill with component label
      std::queue<Vector3Di> toFill;
      toFill.push( Vector3Di( m_existingContourVolume[ iNearest ] ) );
      labelIter.SetPos( Vector3Di( m_existingContourVolume[ iNearest ] ) );
      vxBlockVolumeIteratorNeighbors<uint2> neighborIter( * GetEnvironment().GetLabelVolume(), Vector3Di( 0, 0, 0 ),
                                                          GetNeighborhood( true ) );
      while ( ! toFill.empty() )
      {
        Vector3Di position = toFill.front();
        toFill.pop();

        // search for nearby positions that are not already filled
        for ( neighborIter.CenterAt( position ); ! neighborIter.IsAtEnd(); ++neighborIter )
        {
          // see if we already discovered this one
          if ( neighborIter.GetVoxel() != m_uStartLabelIndex )
          {
            neighborIter.SetVoxel( m_uStartLabelIndex );
            toFill.push( neighborIter.GetPos() );
          }
        }

      }

    }
  }

  GetEnvironment().GetLabelVolume()->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );

  return true;

} // OnButtonUp()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorContourAdjust::OnSetCursor()
{

  const bool bOrthoMPRView( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType() ) != NULL );

  vxMouseCursor::SetCursor( bOrthoMPRView ? "CURSOR_RESTRICTIVE_LASSO" : "CURSOR_NO" );

  return true;

} // OnSetCursor()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorContourAdjust.C,v $
// Revision 1.3  2006/06/01 13:41:26  frank
// more C++ compliant
//
// Revision 1.2  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.1  2006/03/13 13:55:19  frank
// initial version
//
// Revision 1.13  2006/03/02 15:48:18  frank
// pulled the environment out of one of the utility functions
//
// Revision 1.12  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.11  2006/02/17 15:13:58  frank
// working on undoing all segmentation actions
//
// Revision 1.10  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// Revision 1.9  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.8  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.7.2.2  2005/08/26 13:28:24  frank
// bug when appending to background component fixed
//
// Revision 1.7.2.1  2005/07/25 20:10:41  frank
// Issue #4304: Fixed problem when sculpting or contouring locked voxels
//
// Revision 1.7  2001/01/03 19:30:08  michael
// Moved function WarnShiftOblique out of the base class and into vxManipulatorUtils
// where it can be used by the (few) manipulators that need it.
//
// Revision 1.6  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.6.2  2005/02/15 19:50:04  frank
// Issue #3948: Showed proper tooltip for editable and not editable component names
//
// Revision 1.5.6.1  2005/02/15 19:45:28  frank
// Issue #3948: Showed proper tooltip for editable and not editable component names
//
// Revision 1.5  2004/04/06 16:25:11  frank
// unified environment access naming
//
// Revision 1.4  2004/03/31 17:35:03  frank
// unused include files
//
// Revision 1.3  2004/03/12 19:50:34  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/12 18:03:58  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/01/19 20:31:54  frank
// fixed sculpting in fullscreen mode
//
// Revision 1.34  2004/01/16 21:22:43  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.33  2004/01/14 17:15:51  frank
// cleared the old contour in the destructor
//
// Revision 1.32  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.31  2004/01/13 13:13:40  frank
// Contouring only works in ortho views
//
// Revision 1.30  2004/01/12 14:19:24  frank
// fixed problem with manipulating on oblique with shift held down
//
// Revision 1.29  2003/12/16 20:22:24  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 1.28  2003/11/20 18:27:00  frank
// Fixed new component creation procedure
//
// Revision 1.27  2003/07/31 14:02:20  frank
// Issue #3407: Fixed contour drawing on small-unit datasets
//
// Revision 1.26  2003/06/24 19:20:50  frank
// formatting
//
// Revision 1.25  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.24  2003/06/18 15:19:55  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.23  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.22  2003/05/13 17:42:05  frank
// Issue #3215: Contour broken in some slices
//
// Revision 1.21  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.20  2003/04/29 14:13:12  frank
// Moved projection functionality to the viewer type
//
// Revision 1.19  2003/04/08 20:34:23  frank
// Issue #3062: Trying to reproduce problem with buggy contour drawing
//
// Revision 1.18  2003/04/08 18:17:24  frank
// Issued #3060: Preventing operation on oblique views
//
// Revision 1.17  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.16  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.15  2003/02/25 22:11:48  frank
// Fixed problem with pan/zoom window killing selection points
//
// Revision 1.14  2003/02/25 20:40:00  frank
// Fixed mouse jumping to (0,0) when you go off the edge problem.
//
// Revision 1.13  2003/02/25 16:38:18  frank
// Believe to have fixed the contour missing slices
//
// Revision 1.12  2003/01/22 21:56:15  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.11  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.10  2002/12/16 23:47:26  frank
// Removed old comment
//
// Revision 1.9  2002/11/29 21:01:22  geconomos
// Added manipulator to vxInputFeedback list on ESC.
//
// Revision 1.8  2002/11/27 17:37:30  frank
// removed comment
//
// Revision 1.7  2002/11/25 21:06:57  michael
// update mask volume and take this into account
//
// Revision 1.6  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.5  2002/11/12 21:29:22  geconomos
// Made vxContour  a first class citizen of vxEnvironment (frank really did it).
//
// Revision 1.4  2002/11/12 00:11:54  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.3  2002/11/08 23:58:03  frank
// cleaned out name "restricted lasso"
//
// Revision 1.2  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.25  2002/10/08 16:42:51  frank
// Issue #2658: Fixed when you click outside the menu
//
// Revision 1.24  2002/10/05 02:37:12  frank
// Issue #2658: Can no longer unassign with the restricted lasso.
//
// Revision 1.23  2002/10/04 19:10:55  frank
// Fixed up the scoring mechanism.  Cleaned up code significantly.
//
// Revision 1.22  2002/10/03 19:12:25  dmitry
// Rendering Mode not concidered when calc world coord.
//
// Revision 1.21  2002/09/30 14:36:59  dmitry
// EVENT_COMPLETED called
//
// Revision 1.20  2002/09/27 21:06:15  frank
// Issue #2699: Added wait cursor during assignment.
//
// Revision 1.19  2002/09/26 18:08:57  dmitry
// Cleaned up mouse input handlers and orientation change handled.
//
// Revision 1.18  2002/09/26 14:51:52  frank
// Moved InitializeData into C++.
//
// Revision 1.17  2002/09/24 22:58:54  geconomos
// Prevented popup menu during marking of bones and noise.
//
// Revision 1.16  2002/09/18 20:17:33  dmitry
// Cursors added.
//
// Revision 1.15  2002/09/16 20:20:39  dmitry
// New vxID (s) used.
//
// Revision 1.14  2002/09/12 03:26:42  frank
// Toggled plaque label display with F12.
//
// Revision 1.13  2002/09/09 21:10:40  dmitry
// point vector changed to list
//
// Revision 1.12  2002/09/05 22:37:48  jaddonisio
// New vxComponent.
//
// Revision 1.11  2002/08/27 16:29:13  dmitry
// SetColor method used
//
// Revision 1.10  2002/08/26 19:30:34  frank
// Positioned pop-up window under mouse.
//
// Revision 1.9  2002/08/26 17:01:15  frank
// Cleaned up dead code.
//
// Revision 1.8  2002/08/22 22:34:25  uday
// update.
//
// Revision 1.7  2002/08/21 23:27:19  uday
// update.
//
// Revision 1.6  2002/08/20 22:44:25  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.5  2002/08/13 15:53:02  dmitry
// Took out unnecessary param from CalculateScoresFreeForm.
//
// Revision 1.4  2002/07/15 15:20:52  geconomos
// no message
//
// Revision 1.3  2002/07/11 21:09:11  manju
// Speed up.
//
// Revision 1.2  2002/07/10 15:13:31  manju
// Added methods to implement copy-paste of a lasso and creation of
// new plaque.
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.3  2002/06/27 12:07:49  manju
// Added OnKeyPressed method.
//
// Revision 1.2  2002/06/26 17:24:58  manju
// Modified the code to get it working,
//
// Revision 1.1  2002/06/19 14:24:20  manju
// Manipulator for restricted lasso.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorContourAdjust.C,v 1.3 2006/06/01 13:41:26 frank Exp $
// $Id: vxManipulatorContourAdjust.C,v 1.3 2006/06/01 13:41:26 frank Exp $
