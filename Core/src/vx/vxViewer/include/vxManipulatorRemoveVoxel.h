// $Id: vxManipulatorRemoveVoxel.h,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa (kchalupa@viatronix.com )


// pragmas
#pragma once

// forward declarations
class vxEnvironment;

// includes
#include "vxManipulator.h"
#include "vxManipulatorAddVoxel.h"

/**
 * Removes the selected voxels from the selected component.
 */
class VX_VIEWER_DLL vxManipulatorRemoveVoxel : public vxManipulatorAddVoxel
{
// construction
public:

  /// Constructor
  vxManipulatorRemoveVoxel( vxEnvironment & environment );

  /// Destructor
  virtual ~vxManipulatorRemoveVoxel();

// member functions
public:

  /// This event handler can be reimplemented in a subclass to receive mouse enter events for the viewer.     
  virtual bool OnSetCursor() override;

protected:

  /// Performs voxel manipulator specific operations after voxels are selected.
  virtual void PerformVoxelOperation(const uint4 uKeyboardState) override;

private:

  /// Call through to vxComponentUtils::RemoveVoxels
  void RemoveVoxelsFromCurrentComponent( const uint4 uKeyboardState );

}; // class vxManipulatorRemoveVoxel


#pragma region revision history

// $Log: vxManipulatorRemoveVoxel.h,v $
// Revision 1.1.2.1  2009/08/31 13:55:34  kchalupa
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorRemoveVoxel.h,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
// $Id: vxManipulatorRemoveVoxel.h,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $

#pragma endregion
