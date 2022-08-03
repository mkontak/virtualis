// $Id: vxManipulatorIncrementalGrowth.C,v 1.6 2007/03/07 17:04:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include <fstream>
#include "vxManipulatorIncrementalGrowth.h"
#include "vxViewerTypes.h"
#include "vxIncrementalGrowth.h"
#include "vxProgressBar.h"
#include "Timer.h"
#include "vxMessageBox.h"
#include "vxInput.h"
#include "vxEasySelection.h"
#include "vxComponentUtils.h"
#include "vxEnvironment.h"
#include "vxUndoActionSegmentation.h"



// Defines
#define FILE_REVISION "$Revision: 1.6 $"


// Statics
uint4 vxManipulatorIncrementalGrowth::m_uVoxelsPerPixel( 300 );


/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorIncrementalGrowth::vxManipulatorIncrementalGrowth( vxEnvironment & environment )
: vxManipulator( environment )
{
} // vxManipulatorIncrementalGrowth()


/**
 * Destructor
 */
vxManipulatorIncrementalGrowth::~vxManipulatorIncrementalGrowth()
{
  QuitGrowing();
} // ~vxManipulatorIncrementalGrowth()



/**
 * Event handler if key is pressed
 *
 * @param uKeyId
 * @param uKeyboardState
 *
 * @return true if handled
 */ 
bool vxManipulatorIncrementalGrowth::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  bool bHandled( false );

  // select an operation
  switch ( uKeyId )
  {

  // Shift or CTRL key pressed
  case vxInput::KEY_SHIFT:
  case vxInput::KEY_CONTROL:
    SetVoxelsPerStep( uKeyboardState );
    GrowSome();
    break;

  // Escape key pressed
  case vxInput::KEY_ESCAPE:
   QuitGrowing();
    bHandled = true;
    break;

  // nothing else is supported
  default:
    break;

  } // switch on keypress

  return bHandled;
} // OnKeyPressed()


/**
 * Quit growing immediately
 */
void vxManipulatorIncrementalGrowth::QuitGrowing()
{
  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "QuitGrowing" );
    return;
  }

  // clear everything if there is something in the works
  if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() )
  {

    // clear everything
    pIncrementalGrowth->Clear();
    pIncrementalGrowth->Modified( vxModInfoID( vxElement, vxIDs::IncrementalGrowth, 0 ) );

    pIncrementalGrowth->GetIncrementalSlider().SetActive( false );

  } // if anything growing right now

} // QuitGrowing()


/**
 * Event handler if key is released
 *
 * @param uKeyId
 * @param uKeyboardState
 *
 * @return true if handled
 */ 
bool vxManipulatorIncrementalGrowth::OnKeyReleased(const uint1 uKeyId, const uint4 uKeyboardState)
{
  bool bHandled(false);

  // select an operation
  switch ( uKeyId )
  {

  // Shift or CTRL key released
  case 16:
  case 17:
    SetVoxelsPerStep( uKeyboardState );
    GrowSome();
    break;

  default:
    // nothing else is supported
    break;
  }

  return bHandled;
} // OnKeyPressed()


/**
 * Event handler if key is pressed
 *
 * @param position       the screen position
 * @param eButtonId      the button(s) pressed
 * @param uKeyboardState the keyboard state at the time of button press
 *
 * @return true if handled
 */ 
bool vxManipulatorIncrementalGrowth::OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{

  // not handled at this time
  return false;

} // OnButtonDown()


/**
 * Event handler if mouse button is released
 *
 * @param mousePosition  the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorIncrementalGrowth::OnButtonUp(const Point2D<int2> & mousePosition, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{

  bool bHandled( false );

  // check for left button
  if ( eButtonId == BUTTON_LEFT )
  {

    // get the incremental growth object
    vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
    if ( pIncrementalGrowth == NULL )
    {
      LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "OnButtonUp" );
      return false;
    }

    // see if nothing is growing
    if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() == false )
    {

      // compute the seed point
      Point3Df worldPosition( 0, 0, 0 );
      bool bValidSeed = ComputeSeedPoint( worldPosition, mousePosition );
      
      if ( bValidSeed == false )
      {

        // beep a warning
        MessageBeep( MB_ICONHAND );

        // show a warning that disappears automatically
        vxMessageBox::ShowInfo( "Unable to grow from this point.\nEnsure the component here is unlocked." );

        return false;

      } // invalid seed point

      // set the keyboard multipler
      SetVoxelsPerStep( uKeyboardState );

      // start something growing
      bHandled = StartGrowth( mousePosition, worldPosition );

    }
    else
    {

      // set the keyboard multipler
      SetVoxelsPerStep( uKeyboardState );

      // nothing is currently growing, just set the voxels per step and stop growing
      EndGrowth();
      bHandled = true;
    }
  } // if left button

  return bHandled;

} // OnButtonUp()


/**
 * Get the seed point underneath the cursor, if available
 *
 * @param worldPosition  the computed world position
 * @param mousePosition the mouse position
 *
 * @return true if a valid seed point is found
 */
bool vxManipulatorIncrementalGrowth::ComputeSeedPoint( Point<float4> & worldPosition, const Point2D<int2> & mousePosition )
{

  // get the seed point under (or near) the cursor
  vxViewerType3D * pViewerType3D = dynamic_cast< vxViewerType3D *>( GetEnvironment().GetViewerType() );
  if (pViewerType3D != NULL)
  {
    std::vector<float4> profile;
    if (pViewerType3D->GetVesselLumenWorldFromMouse( mousePosition, worldPosition, profile ) == false )
      return false;
  }
  else
  {
    if ( GetEnvironment().GetViewerType()->ConvertMouseToWorld( mousePosition, worldPosition ) == false )
      return false;
  }

  // convert world position to volume position
  Point3Df volumePos( worldPosition.GetDividedAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() ) );

  // check to see if the seed point is valid (and if the component is unlocked)
  vxBlockVolumeIterator< bool > lockMaskIter( * GetEnvironment().GetInteractiveSegmentation()->GetCompHdlrPtr()->GetMaskTagVolume() );
  lockMaskIter.SetPos( Vector3Di( volumePos ) );
  const bool bLocked( lockMaskIter.GetVoxel() );

  return bLocked == false;

} // ComputeSeedPoint()


/**
 * Set the number of voxels that are segmented for each screen pixel
 */
void vxManipulatorIncrementalGrowth::SetVoxelsPerStep( const uint4 uKeyboardState )
{
  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "SetVoxelsPerStep" );
    return;
  }

  // adjust if there is something currently growing
  if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() )
  {

    // get before and after values
    const uint4 uPreviousValue( pIncrementalGrowth->GetIncrementalSlider().GetVoxelsPerStep() );
    const uint4 uNewValue( m_uVoxelsPerPixel * GetKeyMultiplier( uKeyboardState ) );

    // only update if anything changed
    if ( uPreviousValue != uNewValue )
    {

      pIncrementalGrowth->GetIncrementalSlider().SetVoxelsPerStep( uNewValue );
      pIncrementalGrowth->Modified( vxModInfoID( vxElement, vxIDs::IncrementalGrowth, 0 ) );

    } // if anything changed

  } // if anything is currently growing

} // SetVoxelsPerStep()


/**
 * Event handler if mouse is moved
 *
 * @param position       the screen position
 * @param uKeyboardState the keyboard state at the time of button press
 *
 * @return true if handled
 */ 
bool vxManipulatorIncrementalGrowth::OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState)
{
  bool bHandled( false );

  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "OnMouseMove" );
    return false;
  }

  // check if currently growing something
  if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() )
  {

    // set the keyboard multipler
    SetVoxelsPerStep( uKeyboardState );

    // set the mouse position
    pIncrementalGrowth->GetIncrementalSlider().SetCurrentMousePos( position );

    // grow some more if necessary
    GrowSome();
    bHandled = true;

  } // if currently growing

  SetMousePosition( position );

  return bHandled;
} // OnMouseMove()


/** 
 * This event handler can be reimplemented in a subclass to receive set cursor events for the viewer.
 *
 * @return true if the cursor was set and nobody else should set theirs
 */
bool vxManipulatorIncrementalGrowth::OnSetCursor()
{
  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "OnSetCursor" );
    return false;
  }

  // if growing something, then show up-down arrow, otherwise just show easy select cursor
  if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() )
  {

    vxMouseCursor::SetCursor( "CURSOR_NS" );

  } // if currently growing
  else
  {

    // Check to see if there is anything underneath the cursor that
    // *could* be grown if the user were to click right now.
    Point3Df worldPosition( 0, 0, 0 );
    const bool bValidSeed( ComputeSeedPoint( worldPosition, GetMousePosition() ) );
    
    vxMouseCursor::SetCursor( GetModuleHandle() ,bValidSeed ? "CURSOR_EASY_SELECT_ACTIVE" : "CURSOR_EASY_SELECT" );
  } // if not currently growing

  return true;
} // OnSetCursor()


/**
 * Start growing
 *
 * @param mousePos the mouse screen position
 * @param worldPos the world position of the seedpoint
 *
 * @return true if seedpoint found
 */
bool vxManipulatorIncrementalGrowth::StartGrowth( const Point2D<int2> & mousePosition, const Point3Df & worldPos )
{
  //Check if the background component is locked or not.
  if ( ( * GetEnvironment().GetComponents() )[ 0 ]->IsLocked() )
  {
    // show a warning that disappears automatically
    vxMessageBox::ShowInfo( "Unable to grow from this point.\nEnsure the background component here is unlocked." );
    return false;
  }
  
  // convert world position to volume position
  Point3Df volumePos( worldPos.GetDividedAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() ) );

  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "StartGrowth" );
    return false;
  }
  pIncrementalGrowth->GetIncrementalSlider().Initialize( mousePosition );

  // set up for growing in vesselness or in intensity
  vxBlockVolumeIterator< uint2 > * pDataIter = new vxBlockVolumeIterator< uint2 >( * GetEnvironment().GetIntensityVolume() );
  if ( GetEnvironment().GetElement( vxIDs::IsVesselnessLoaded ) != NULL )
  {
    // growing in vesselness
    vxIncrementalHeap::GetIncrementalSimilarity().SetGrowingInVesselness( pDataIter );
  }
  else
  {
    // growing in intensity
    pDataIter->SetPos( Vector3Di( volumePos ) );
    const float4 fStandardDeviation( vxIncrementalHeap::GetStandardDeviation( * pDataIter ) );
    vxIncrementalHeap::GetIncrementalSimilarity().SetGrowingInIntensity( pDataIter, pDataIter->GetVoxel(), fStandardDeviation );
  }

  // get the mask of locked voxels
  vxBlockVolumeIterator< bool > * pLockMaskIter = new vxBlockVolumeIterator< bool >( * GetEnvironment().GetInteractiveSegmentation()->GetCompHdlrPtr()->GetMaskTagVolume() );

  // create the growth object
  pIncrementalGrowth->Initialize( volumePos, GetEnvironment().GetIntensityVolume(), pDataIter, pLockMaskIter );

  // see if anything was found
  pIncrementalGrowth->GetIncrementalSlider().SetActive( pIncrementalGrowth->GetComponentVoxels().size() > 0 );
  if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() == false )
  {

     // disable the slider
     pIncrementalGrowth->GetIncrementalSlider().SetActive( false );

     // beep a warning
     MessageBeep( MB_ICONHAND );

     // show a warning that disappears automatically
     vxMessageBox::ShowInfo( "Unable to grow from this point.\nEnsure the component here is unlocked." );

  } // if no seedpoint found

  // run a performance system test
  const bool bTest( false );
  if ( bTest && pIncrementalGrowth->GetIncrementalSlider().GetActive() )
  {
    Timer performanceTestTimer;
    const uint4 uNumVoxelsInTest( 100000 );
    for ( int4 i=0; i<uNumVoxelsInTest * 10; ++i )
    {
      if ( pIncrementalGrowth->GetComponentVoxels().size() < uNumVoxelsInTest )
        pIncrementalGrowth->Cycle();
    } // loop over voxels to try growing
    LogDbg( "performance test: " + performanceTestTimer.AsString(), "", "" );    
  }


  return pIncrementalGrowth->GetIncrementalSlider().GetActive();

} // StartGrowth()


/**
 * See if we need to grow out any more to match the user's cursor and do so.
 */
void vxManipulatorIncrementalGrowth::GrowSome()
{
  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "GrowSome" );
    return;
  }

  // early return if inactive
  if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() == false ) 
    return;
  
  const vxIncrementalSlider & slider = pIncrementalGrowth->GetIncrementalSlider();
  const uint4 uNumVoxelsRequired( slider.GetNumberDesiredVoxels() );
  const uint4 uNumVoxelsComputed( pIncrementalGrowth->GetComponentVoxels().size() );
  const uint4 uAdditionalVoxelsRequired( ( uNumVoxelsComputed > uNumVoxelsRequired ) ? 0 : uNumVoxelsRequired - uNumVoxelsComputed );

  // try to get the required number - try up to five times per voxels required (some voxels will be locked)
  const uint4 uAdditionalVoxelsToTry( uAdditionalVoxelsRequired * 5 );
  vxMouseCursor cursor( "CURSOR_WAIT" );
  for ( int4 i=0; i<uAdditionalVoxelsToTry; ++i )
  {
    if ( pIncrementalGrowth->GetComponentVoxels().size() < uNumVoxelsRequired )
      pIncrementalGrowth->Cycle();
  } // loop over voxels to try growing

  // set image modified
  pIncrementalGrowth->Modified( vxModInfoID( vxElement, vxIDs::IncrementalGrowth, 0 ) );

  // save the undo information - relinquish control of memory too
  vxUndoActionSegmentation * pUndo = new vxUndoActionSegmentation( GetEnvironment().GetLabelVolume(), GetEnvironment().GetComponents() );
  vxEnvironment::GetUndoActions().PushAction( pUndo );
  pUndo = NULL;

} // GrowSome()


/**
 * End incremental growth
 */
void vxManipulatorIncrementalGrowth::EndGrowth()
{
  // get the incremental growth object
  vxShareableObject< vxIncrementalGrowth > * pIncrementalGrowth = dynamic_cast< vxShareableObject< vxIncrementalGrowth > * >( GetEnvironment().GetElement( vxIDs::IncrementalGrowth ) );
  if ( pIncrementalGrowth == NULL )
  {
    LogWrn( "Incremental growth object pointer is NULL", "vxManipulatorIncrementalGrowth", "EndGrowth" );
    return;
  }

  // early return if inactive
  if ( pIncrementalGrowth->GetIncrementalSlider().GetActive() == false ) return;

  //// Re-grow the region so we get exactly the number of voxels requested
  
  // first initiate processing
  ProgressBarUpdater progressUpdater;
  vxProgressBar::SetProgressMessage( "Easy Select" );
  vxMouseCursor cursor( "CURSOR_WAIT" );
  const vxIncrementalSlider & slider = pIncrementalGrowth->GetIncrementalSlider();
  pIncrementalGrowth->InitiateProcessing( pIncrementalGrowth->GetSeedPosition() );

  // figure out how many times we need to cycle the growth process in order to get the required number of found voxels
  uint4 uMaxProcessingCycles = slider.GetNumberDesiredVoxels();

  // cycle processing at least enough times to get the required number of voxels
  for ( uint4 uCycles(0),uFound(0); uCycles<uMaxProcessingCycles && uFound<slider.GetNumberDesiredVoxels(); ++uCycles )
  {

    // save how many voxels there were before processing
    const uint4 uVoxelsBefore( pIncrementalGrowth->GetComponentVoxels().size() );

    // try to get another voxel
    pIncrementalGrowth->Cycle();

    // check now many voxels there are after processing
    const uint4 uVoxelsAfter( pIncrementalGrowth->GetComponentVoxels().size() );
    if ( uVoxelsAfter > uVoxelsBefore )
      ++uFound;

    // update progress bar
    progressUpdater.Update( 80.0F * float4( uFound ) / float4( fabs( static_cast<float4>(slider.GetNumberDesiredVoxels() )) + 1 ) );
  } // for all cycles

  // spit out a warning if the growth was incomplete
  if ( pIncrementalGrowth->GetComponentVoxels().size() < slider.GetNumberDesiredVoxels() )
    LogWrn( "Unable to find enough voxels", "vxManipulatorIncrementalGrowth", "EndGrowth" );

  // clear the growth image
  //pGrowthImage->Clear();

  // hide the in-window slider
  pIncrementalGrowth->GetIncrementalSlider().SetActive( false );

  // dilate the mask twice
  vxComponentUtils::DilateMask( * pIncrementalGrowth->GetComponentVolumePtr(), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );
  progressUpdater.Update( 90 );
  vxComponentUtils::DilateMask( * pIncrementalGrowth->GetComponentVolumePtr(), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06 );

  // hide the progress bar
  progressUpdater.Update( 100 );
  vxProgressBar::SetProgressMessage( "" );

  //keep this value incase you need to set this back for append to Background condition.
  SegParams::RegionAddOptionEnum regionGrowSelection = 
           (SegParams::RegionAddOptionEnum)GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowAddOpt();


  // check the segmentation mode and query the user if append mode is slected
  // to make sure they really want to append - assuming they do not
   if ( GetEnvironment().GetInteractiveSegmentation()->GetRegionGrowAddOpt() == SegParams::APPEND_REGIONS )
   {
     // make sure they aren't trying to append to none
     bool bAppendingToNone( GetEnvironment().GetComponents()->GetSelectedComponentIndex() == 0 );
     if ( bAppendingToNone )
     {
       std::string sComponentName( GetEnvironment().GetComponents()->ArrayByLabel()[ 0 ]->GetName() );
       vxMessageBox::ShowInfo( "Appending to " + sComponentName + " is not possible. A new component will be created instead" );
       GetEnvironment().GetInteractiveSegmentation()->SetRegionGrowAddOpt( SegParams::NEW_REGIONS );
       GetEnvironment().GetInteractiveSegmentation()->Modified( vxModInfo( InteractiveSegmentation, InteractiveSegmentation::EVENT_SEGPARAMS_UPDATE ) );
     }
  }

  // add the component to the environment
  uint2 uLabel = vxComponentUtils::CreateNewComponent( * pIncrementalGrowth->GetComponentVolumePtr(), vxComponentUtils::GetUniqueComponentName( * GetEnvironment().GetComponents(), "Object" ), GetEnvironment(), true );
  vxEasySelection * pEasySelection = new vxEasySelection();
  pEasySelection->SetLabel( uLabel );
  pEasySelection->SetVesselness( vxIncrementalHeap::GetIncrementalSimilarity().IsGrowingInVesselness() );
  GetEnvironment().GetComponents()->ArrayByLabel()[ uLabel ]->SetComponentDataPtr( pEasySelection );
  GetEnvironment().GetComponents()->Modified( vxModIndInfo( vxComponentArray, 0, vxUtils::COMPONENTARRAY_ADDED ) );

  // notify the feedback so that manipulator could be removed
  //GetEnvironment().GetInputFeedback().Add( vxFeedback( typeid(*this), vxFeedback::ActionType::DONE ) );

  // deactivate growth operation
  pIncrementalGrowth->Clear();
  pIncrementalGrowth->GetIncrementalSlider().SetActive( false );

  //Set the old regionGrow option
  GetEnvironment().GetInteractiveSegmentation()->SetRegionGrowAddOpt( regionGrowSelection );

} // EndGrowth()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorIncrementalGrowth.C,v $
// Revision 1.6  2007/03/07 17:04:39  geconomos
// code review
//
// Revision 1.5  2007/01/11 16:55:02  romy
// Issue of #5274 fixed. Checks the background component Lock state before any Easy Selection Grow.
//
// Revision 1.4  2007/01/11 15:28:22  romy
// Issue #5273 fixed. Added check for background component if the operation is append.
//
// Revision 1.3  2007/01/09 22:36:58  romy
// modified for easy select icons
//
// Revision 1.2  2007/01/03 20:01:58  romy
// added UndoAction
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.10  2006/09/14 20:20:24  geconomos
// updated for changes to element ids
//
// Revision 1.9  2006/09/01 13:25:34  frank
// moved dilate volume function to vxViewer
//
// Revision 1.8  2006/07/06 15:05:34  frank
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
// Revision 1.3  2005/06/08 14:59:24  vxconfig
// added easy select object
//
// Revision 1.2  2005/06/07 18:07:47  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.3  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.2  2004/03/08 19:04:21  geconomos
// Updated to compile with vc++ 7.1 compiler
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2004/03/02 01:10:33  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.21.2.1  2004/02/25 14:58:06  frank
// formatting
//
// Revision 1.21  2004/02/09 15:47:34  dongqing
// replace ID number to Enum name for all keys.
//
// Revision 1.20  2004/01/29 18:04:01  frank
// fixed the selection and display of easy select in 2D views
//
// Revision 1.19  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.18  2004/01/09 15:41:03  frank
// code review
//
// Revision 1.17  2004/01/08 12:49:10  frank
// Changed the cursor when you can actually select something
//
// Revision 1.16  2004/01/07 21:47:27  wenli
// Save the easy grown vessels in to  vessel array
//
// Revision 1.15  2003/12/23 13:02:07  frank
// Adjusted the locked component message
//
// Revision 1.14  2003/12/22 21:06:47  frank
// Showed message box when you can not start incremental growth
//
// Revision 1.13  2003/12/15 16:19:27  frank
// Working on showing a modeless information dialog box
//
// Revision 1.12  2003/12/08 20:22:43  frank
// Removed an extra data member
//
// Revision 1.11  2003/11/26 13:57:39  frank
// Fixed slider
//
// Revision 1.10  2003/11/20 14:22:50  frank
// Made easy select work on all views simultaneously
//
// Revision 1.9  2003/10/28 15:44:49  frank
// Improved the Easy Select speed by 35%
//
// Revision 1.8  2003/08/04 20:11:29  frank
// Issue #3393: Cleaned up easy select mode after mode switch
//
// Revision 1.7  2003/06/24 11:51:07  frank
// Issue #3326: Stay in Easy-Select mode after operation
//
// Revision 1.6  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.5  2003/06/06 14:57:17  frank
// Issue #3277: Disallowed growth if seedpoint is locked
//
// Revision 1.4  2003/05/28 13:51:55  frank
// Dilated the volume twice after growth.
//
// Revision 1.3  2003/05/20 16:35:53  frank
// Moved ProgressBarUpdater class to vxProgressBar files
//
// Revision 1.2  2003/05/19 18:48:24  frank
// Fixed progress bar during re-grow
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.16  2003/04/30 16:58:04  frank
// code reviews
//
// Revision 1.15  2003/04/29 19:51:53  frank
// Grew additional for key multipliers
//
// Revision 1.14  2003/04/29 19:47:55  frank
// Handled the key up and key down message properly for shift and control
//
// Revision 1.13  2003/04/28 18:59:35  frank
// code review
//
// Revision 1.12  2003/04/24 20:39:46  frank
// cleaned up cursors
//
// Revision 1.11  2003/04/23 18:52:20  frank
// Fixing up cursor
//
// Revision 1.10  2003/04/23 14:43:07  frank
// Stored the number of voxels processed so that we can be sure about how many times to try to find voxels in the second pass of incremental growth.
//
// Revision 1.9  2003/04/22 12:03:23  frank
// Added ESC key
//
// Revision 1.8  2003/04/18 16:01:12  frank
// Fixed growing, progress bar, and screen issues
//
// Revision 1.7  2003/04/08 19:51:39  geconomos
// updated for new graphics.
//
// Revision 1.6  2003/04/08 14:45:21  frank
// Moved GetVesselLumenWorldCoordinates() into vxViewerType3D
//
// Revision 1.5  2003/04/07 18:09:13  frank
// Refactored code to improve reusability and maintainability.
//
// Revision 1.4  2003/04/07 13:18:33  frank
// Made it only segment the number of voxels desired according to the slider.
//
// Revision 1.3  2003/04/04 21:27:45  frank
// Made the number of voxels in the region interactive
//
// Revision 1.2  2003/04/04 15:55:08  frank
// Cleaned up class and file names and respected the component lock flag.
//
// Revision 1.1  2003/04/04 13:01:23  frank
// Added the ability to incrementally grow a new component.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorIncrementalGrowth.C,v 1.6 2007/03/07 17:04:39 geconomos Exp $
// $Id: vxManipulatorIncrementalGrowth.C,v 1.6 2007/03/07 17:04:39 geconomos Exp $