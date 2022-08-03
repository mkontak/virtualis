// $Id: vxManipulatorCamera.C,v 1.7 2006/02/24 20:14:32 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: jenny hizver (mailto:jenny@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorCamera.h"
#include "vxCamera.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "vxUndoActionCamera.h"


// defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorCamera::vxManipulatorCamera(vxEnvironment & environment):
vxManipulator(environment)
{
} // constructor


/**
 * handle the common keypress-activated camera operations
 *
 * @param environment    the environment
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */
bool vxManipulatorCamera::ManipulateStandardViews(const uint1 uKeyId, 
                                                  const uint4 uKeyboardState)
{
  bool bHandled(false);

  Triplef size(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  Box<float4> roi(0, 0, 0, size[0], size[1], size[2]);

  if (uKeyboardState & vxInput::STATE_CTRL)
  {
    bHandled = true;

    switch (uKeyId)
    {
    case vxInput::KEY_A:
      // anterior view
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      vxUtils::SetStandardCameraView(* GetEnvironment().GetCamera(), vxUtils::ANTERIOR, roi, false);
      GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
      break;
    case vxInput::KEY_P:
      // posterior view
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      vxUtils::SetStandardCameraView(* GetEnvironment().GetCamera(), vxUtils::POSTERIOR, roi, false);
      GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
      break;
    case vxInput::KEY_L:
      // left view
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      vxUtils::SetStandardCameraView(* GetEnvironment().GetCamera(), vxUtils::LEFT, roi, false);
      GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
      break;
    case vxInput::KEY_R:
      // right view
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      vxUtils::SetStandardCameraView(* GetEnvironment().GetCamera(), vxUtils::RIGHT, roi, false);
      GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
      break;
    case vxInput::KEY_S:
      // superior view
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      vxUtils::SetStandardCameraView(* GetEnvironment().GetCamera(), vxUtils::SUPERIOR, roi, false);
      GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
      break;
    case vxInput::KEY_I:
      // inferior view
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCamera( GetEnvironment().GetCamera() ) );
      vxUtils::SetStandardCameraView(* GetEnvironment().GetCamera(), vxUtils::INFERIOR, roi, false);
      GetEnvironment().GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
      break;
    default:
      bHandled = false;
      break;
    } // switch uKeyId
  } // if ctrl key pressed

  return bHandled;
} // ManipulateStandardViews()


/**
 * toggle the projection type between perspective and orthographic
 */
void vxManipulatorCamera::ToggleProjectionType()
{
  
  if ( GetEnvironment().GetCamera()->GetProjectionType() == vxCamera::PERSPECTIVE )
    GetEnvironment().GetCamera()->SetProjectionType( vxCamera::ORTHOGRAPHIC );
  else
    GetEnvironment().GetCamera()->SetProjectionType( vxCamera::PERSPECTIVE );
  
  GetEnvironment().GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
  
} // ToggleProjectionType()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorCamera.C,v $
// Revision 1.7  2006/02/24 20:14:32  frank
// made the undo more granular
//
// Revision 1.6  2006/02/24 16:57:00  frank
// added undo information to oblique plane manipulation
//
// Revision 1.5  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/12/08 16:42:30  frank
// added undo support
//
// Revision 1.3  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.2.1  2005/02/15 22:02:08  frank
// removed dead code
//
// Revision 1.2  2004/11/18 02:23:26  michael
// fixed wrong logic that always made it handled == true by default and bailing
// out without setting it to false if CTRL was not down
//
// Revision 1.1  2004/05/25 14:31:16  frank
// added base class for camera manipulators
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorCamera.C,v 1.7 2006/02/24 20:14:32 frank Exp $
// $Id: vxManipulatorCamera.C,v 1.7 2006/02/24 20:14:32 frank Exp $
