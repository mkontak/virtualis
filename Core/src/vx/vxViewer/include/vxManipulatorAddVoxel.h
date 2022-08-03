// $Id: vxManipulatorAddVoxel.h,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
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

// includes
#include "vxEnvironment.h"
#include "vxManipulator.h"
#include "vxManipulatorContour.h"

/**
 * Manipulator to add voxels to a current component.
 */
class VX_VIEWER_DLL vxManipulatorAddVoxel : public vxManipulatorContour
{
// construction
public:

  /// Constructor
  vxManipulatorAddVoxel( vxEnvironment & environment );

  /// Destructor
  virtual ~vxManipulatorAddVoxel();

// member functions
public:

  /// This event handler can be reimplemented in a subclass to receive mouse up events for the viewer.     
  virtual bool OnButtonUp( const Point2D<int2> & position, const vxManipulator::ButtonEnum eButtonId, const uint4 uKeyboardState ) override;

  /// Handles the OnKeyPressed event.
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState ) override;

  /// This event handler can be reimplemented in a subclass to receive mouse enter events for the viewer.     
  virtual bool OnSetCursor() override;

protected:

  /// Performs voxel manipulator specific operations after voxels are selected.
  virtual void PerformVoxelOperation(const uint4 uKeyboardState);

private:

  /// Call through to label volume add.
  void AddVoxelsToCurrentComponent( const uint4 uKeyboardState );

}; // class vxManipulatorAddVoxel


#pragma region revision history

// $Log: vxManipulatorAddVoxel.h,v $
// Revision 1.1.2.1  2009/08/31 13:55:34  kchalupa
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorAddVoxel.h,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $
// $Id: vxManipulatorAddVoxel.h,v 1.1.2.1 2009/08/31 13:55:34 kchalupa Exp $

#pragma endregion 
