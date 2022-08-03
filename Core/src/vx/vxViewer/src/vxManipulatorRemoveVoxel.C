// $Id: vxManipulatorRemoveVoxel.C,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa (kchalupa@viatronix.com )


// includes
#include "StdAfx.h"
#include "vxManipulatorRemoveVoxel.h"
#include "Point.h"
#include "vxEnvironment.h"
#include "vxManipulatorAddVoxel.h"
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
vxManipulatorRemoveVoxel::vxManipulatorRemoveVoxel( vxEnvironment & environment ) 
  : vxManipulatorAddVoxel( environment )
{
} // vxManipulatorRemoveVoxel()


/**
 * Destructor
 */
vxManipulatorRemoveVoxel::~vxManipulatorRemoveVoxel()
{
} // ~vxManipulatorRemoveVoxel()


/** 
 * This event handler can be reimplemented in a subclass to receive set cursor events for the viewer.
 *
 * @return true if the cursor was set and nobody else should set theirs
 */
bool vxManipulatorRemoveVoxel::OnSetCursor()
{
  const bool bOrthoMPRView(dynamic_cast< vxViewerTypeMPROrtho * >(GetEnvironment().GetViewerType()) != NULL);

  if(bOrthoMPRView)
  {
    vxMouseCursor::SetCursor( GetModuleHandle(), "CURSOR_REMOVE_VOXELS" );
  }
  else
  {
    vxMouseCursor::SetCursor("CURSOR_NO");
  }

  return true;
} // OnSetCursor()


/**
 * Performs voxel operations.
 *
 * @param uKeyboardState  The state of the keyboard.
 */
void vxManipulatorRemoveVoxel::PerformVoxelOperation(const uint4 uKeyboardState)
{
  RemoveVoxelsFromCurrentComponent(uKeyboardState);
} // PerformVoxelOperation( uKeyboardState )


/**
 * Remove voxels from current component
 *
 * @param uKeyboardState  The keyboard state
 */
void vxManipulatorRemoveVoxel::RemoveVoxelsFromCurrentComponent( const uint4 uKeyboardState )
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

      // remove voxels from current component
      vxComponentUtils::RemoveVoxelsFromCurrentComponent( * pContourList->GetMaskVolume(),
        GetEnvironment().GetComponents(), GetEnvironment().GetLabelVolume(), *GetEnvironment().GetInteractiveSegmentation(), true );

      // let everyone know that the volume has changed
      GetEnvironment().GetLabelVolume()->Modified(vxModInfo(vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED));
      GetEnvironment().GetComponents()->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED));

      // clear the mask volume for next use
      pContourList->GetMaskVolume()->Clear();

    } // lasso is not empty?
  } // contour is not null?
} // RemoveVoxelsFromCurrentComponent( uKeyboardState )


#pragma region revision history

// $Log: vxManipulatorRemoveVoxel.C,v $
// Revision 1.1.2.1  2009/08/31 13:55:34  kchalupa
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorRemoveVoxel.C,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
// $Id: vxManipulatorRemoveVoxel.C,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $

#pragma endregion 
