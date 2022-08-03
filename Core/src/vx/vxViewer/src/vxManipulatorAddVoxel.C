// $Id: vxManipulatorAddVoxel.C,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] (mailto:[kchalupa@viatronix.com])


// includes
#include "StdAfx.h"
#include "vxManipulatorAddVoxel.h"
#include "vxManipulatorContour.h"
#include "Point.h"
#include "vxEnvironment.h"
#include "vxUndoActionSegmentation.h"
#include "vxComponentUtils.h"
#include "vxViewerTypeMPROrtho.h"

// defines
#define FILE_REVISION "$Revision: 1.1.2.1 $"


/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorAddVoxel::vxManipulatorAddVoxel( vxEnvironment & environment ) : 
  vxManipulatorContour( environment )
{
} // vxManipulatorAddVoxel( environment )


/**
 * Destructor
 */
vxManipulatorAddVoxel::~vxManipulatorAddVoxel()
{
} // ~vxManipulatorAddVoxel()

/** 
 * This event handler can be reimplemented in a subclass to receive set cursor events for the viewer.
 *
 * @return true if the cursor was set and nobody else should set theirs
 */
bool vxManipulatorAddVoxel::OnSetCursor()
{
  const bool bOrthoMPRView(dynamic_cast< vxViewerTypeMPROrtho * >(GetEnvironment().GetViewerType()) != NULL);

  if(bOrthoMPRView)
  {
    vxMouseCursor::SetCursor( GetModuleHandle(), "CURSOR_ADD_VOXELS" );
  }
  else
  {
    vxMouseCursor::SetCursor("CURSOR_NO");
  }

  return true;
} // OnSetCursor()


/**
 * Event handler if a key is pressed.
 *
 * @param uKeyId the key pressed.
 * @param uKeyboardState key state.
 * @return handled?
 */ 
bool vxManipulatorAddVoxel::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  switch(uKeyId)
  {
    case vxInput::KEY_ESCAPE:
    {
      return vxManipulatorContour::OnKeyPressed(uKeyId, uKeyboardState);
    }
    default:
    {
      break;
    }
  } // switch

  return false;
} // OnKeyPressed( uKeyId, uKeyboardState )


/**
 * Event handler if mouse button is released
 *
 * @param position       the mouse position
 * @param eButtonId      the button that was pressed
 * @param uKeyboardState the state of the keyboard
 * @return true if handled
 */ 
bool vxManipulatorAddVoxel::OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{
  if(vxManipulatorContour::OnButtonUp(position, eButtonId, uKeyboardState))
  {
    PerformVoxelOperation( uKeyboardState );
    vxManipulatorContour::Clear();
    vxShareableObject<vxContourList> * pContourList( GetEnvironment().GetContourList() );
    pContourList->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );

    //notify the feedback so that manipulator could be removed
    //GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));    

    return true;
  }
  else
  {
    vxManipulatorContour::SetActive(false);
    Clear();
    return false;
  }
} // OnButtonUp( position, eButtonId, uKeyboardState )


/**
 * Performs voxel manipulator specific operations after voxels are selected.
 *
 * @param uKeyboardState  State of the keyboard.
 */
void vxManipulatorAddVoxel::PerformVoxelOperation(const uint4 uKeyboardState)
{
  AddVoxelsToCurrentComponent(uKeyboardState);
} // PerformVoxelOperation( uKeyboardState )


/**
 * Append to current component from the list of lassos
 *
 * @param uKeyboardState  The keyboard state
 */
void vxManipulatorAddVoxel::AddVoxelsToCurrentComponent( const uint4 uKeyboardState )
{
  if (GetEnvironment().GetContourList() != NULL)
  {
    vxContourList * pContourList( GetEnvironment().GetContourList() );
    pContourList->Initialize( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim(),
                              GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );

    // If the lasso list is not empty
    if (pContourList->GetLassoList().empty() == false)
    {
      // create new component
      vxMouseCursor cursor("CURSOR_WAIT");

      // Process the lasso list to create a bool mask volume used to create a new plaque
      pContourList->ProcessLassoList();

      // save the current state for undo purposes and relinquish control of memory
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionSegmentation( GetEnvironment().GetLabelVolume(),
        GetEnvironment().GetComponents() ) );

      // restrict the operation to the visible volume if shift is held down
      if ( uKeyboardState & vxInput::STATE_SHIFT )
      {
        vxComponentUtils::CullOutside( * pContourList->GetMaskVolume(), vxUtils::GetActiveBoundingBox( true,
        GetEnvironment().GetIntensityVolume(), GetEnvironment().GetViewerType(), GetEnvironment().GetPlaneOrtho(),
        GetEnvironment().GetRenderingMode(), GetEnvironment().GetOffset(),
        GetEnvironment().GetMaximumOffset(), GetEnvironment().GetCropBox() ) );
      }

      // Add voxels to current component
      vxComponentUtils::AddVoxelsToCurrentComponent( * pContourList->GetMaskVolume(),
        GetEnvironment().GetComponents(), GetEnvironment().GetLabelVolume(), *GetEnvironment().GetInteractiveSegmentation(), true );

      // let everyone know that the volume has changed
      GetEnvironment().GetLabelVolume()->Modified(vxModInfo(vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED));
      GetEnvironment().GetComponents()->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED));

      // clear the mask volume for next use
      pContourList->GetMaskVolume()->Clear();

    } // lasso is not empty?
  } // contour is not null?
} // AddVoxelsToCurrentComponent( uKeyboardState )


#pragma region revision history

// $Log: vxManipulatorAddVoxel.C,v $
// Revision 1.1.2.1  2009/08/31 13:55:34  kchalupa
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorAddVoxel.C,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
// $Id: vxManipulatorAddVoxel.C,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $

#pragma endregion
