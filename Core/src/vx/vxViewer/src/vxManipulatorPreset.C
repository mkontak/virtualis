// $Id: vxManipulatorPreset.C,v 1.13 2006/12/21 15:11:14 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorPreset.h"
#include "vxEnvironment.h"
#include "vxSerializer.h"
#include "vxViewerTypes.h"
#include "vxIntensityConverter.h"
#include "vxMathUtils.h"
#include "vxVisualizationPreset.h"
#include "ZeroOne.h"
#include "PixelRGBA.h"
#include "vxVoxelizer.h"
#include "vxUndoActionPreset.h"



// defines
#define FILE_REVISION "$Revision: 1.13 $"


/**
 * Constructor
 * @param environment the environment
 */
vxManipulatorPreset::vxManipulatorPreset( vxEnvironment & environment )
: vxManipulator( environment )
{
} // vxManipulatorPreset()


/**
 * Event handler if key is pressed
 * @param uKeyId         the key id
 * @param uKeyboardState the keyboard state
 * @return handled?
 */ 
bool vxManipulatorPreset::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{

  bool bIfHandled(false);

  try
  {

    // Here is an example of how to voxelize a triangle model into a volume
    //if ( uKeyId == vxInput::KEY_V )
    //{
      // an example of how to rotate a model
      //Matrix<float4> m;
      //m.Rotate( Normal3D<float4>( 0, 1, 0 ), static_cast< float4 >( M_PI / 2 ) );
      //voxelizer.Transform( m );

      //vxBlockVolume<uint2> volume( vxVolumeHeader( Triple<int4>( 512, 512, 1024 ) ) );
      //vxVoxelizer voxelizer;
      //voxelizer.ReadFileOFF( "D:/ds/explorer/OFF/out.off" );
      //voxelizer.VoxelizeTriangles( volume );
      //volume.Write( "D:/ds/explorer/voxelized/out.vol" );
    //}

    // check if any modifier keys are pressed
    if ( uKeyboardState & ( STATE_SHIFT | STATE_CTRL | STATE_ALT ) )
    {
      return bIfHandled;
    }

    // handle the standard number keys as well as the numeric keypad number keys
    int4 iPresetIndex( -1 );
    switch ( uKeyId )
    {
    // standard number keys (across top of keyboard)
    // key '1' should be first preset (index 0)
    case vxInput::KEY_1: iPresetIndex = 0; break;
    case vxInput::KEY_2: iPresetIndex = 1; break;
    case vxInput::KEY_3: iPresetIndex = 2; break;
    case vxInput::KEY_4: iPresetIndex = 3; break;
    case vxInput::KEY_5: iPresetIndex = 4; break;
    case vxInput::KEY_6: iPresetIndex = 5; break;
    case vxInput::KEY_7: iPresetIndex = 6; break;
    case vxInput::KEY_8: iPresetIndex = 7; break;
    case vxInput::KEY_9: iPresetIndex = 8; break;
    case vxInput::KEY_0: iPresetIndex = 9; break;

    // numeric keypad number keys
    // key '1' should be first preset (index 0) to match standard number keys
    case vxInput::KEY_a: iPresetIndex = 0; break;
    case vxInput::KEY_b: iPresetIndex = 1; break;
    case vxInput::KEY_c: iPresetIndex = 2; break;
    case vxInput::KEY_d: iPresetIndex = 3; break;
    case vxInput::KEY_e: iPresetIndex = 4; break;
    case vxInput::KEY_f: iPresetIndex = 5; break;
    case vxInput::KEY_g: iPresetIndex = 6; break;
    case vxInput::KEY_h: iPresetIndex = 7; break;
    case vxInput::KEY_i: iPresetIndex = 8; break;
    case vxInput::KEY_LFET_SINGLE_QUOTE: iPresetIndex = 9; break;

    default:
    	break;
    }

    // if not pressing any modifier keys
    if ( iPresetIndex != -1 )
    {

      // set the selected preset index
      ( GetEnvironment().GetViewerType()->Is2D() ) ?
        GetEnvironment().GetPreset()->SetSelectedVisualizationPreset2D( iPresetIndex ) :
        GetEnvironment().GetPreset()->SetSelectedVisualizationPreset3D( iPresetIndex );

      // get the proper visualization presets
      const std::vector<const vxVisualizationPreset *> & vVisPresets(
        GetEnvironment().GetViewerType()->Is2D() ?
        GetEnvironment().GetPreset()->GetVisualizationPresets2D() :
        GetEnvironment().GetPreset()->GetVisualizationPresets3D() );

      // check for a valid preset
      if ( vVisPresets.size() > iPresetIndex )
      {
        vxVisualizationPreset visPreset( * vVisPresets[ iPresetIndex ] );

        vxEnvironment::GetUndoActions().PushAction( new vxUndoActionPreset( GetEnvironment().GetColormap(),
          GetEnvironment().GetBackgroundColor(), GetEnvironment().GetRenderingMode() ) );

        vxUtils::ApplyVisualizationPreset( & visPreset, & GetEnvironment() );
        bIfHandled = true;
      } // if valid preset
    } // any number key and no modifier keys
  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "unhandled exception", "vxManipulatorPreset", "OnKeyPressed" ) );
  }

  return bIfHandled;

} // OnKeyPressed()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorPreset.C,v $
// Revision 1.13  2006/12/21 15:11:14  gdavidson
// Issue #5189 & 5106: Calls SetSelectedVisualizationPreset2D and SetSelectedVisualizationPreset3D
//
// Revision 1.12  2006/04/27 19:16:38  romy
// UnDoAction Reset method
//
// Revision 1.11  2006/02/24 20:14:11  frank
// added undo for rendering mode and colormap
//
// Revision 1.10  2006/02/17 15:16:25  frank
// provided a sample voxelization
//
// Revision 1.9  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.8.2.1  2005/06/24 13:50:21  frank
// refactored the application of visualization presets
//
// Revision 1.8  2005/05/24 11:32:51  frank
// added background color and rendering mode during preset changes
//
// Revision 1.7  2005/03/30 14:25:46  geconomos
// Fixed compile error due to change in vxUtils
//
// Revision 1.6  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.2  2005/02/18 19:39:53  frank
// fixed zero key on numeric keypad for preset selection
//
// Revision 1.5.2.1  2005/02/07 13:50:03  frank
// allowed numeric keypad numbers to change presets
//
// Revision 1.5  2004/10/05 14:31:07  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.4  2004/05/25 14:31:40  frank
// moved camera preset to camera manipulator
//
// Revision 1.3  2004/04/26 19:00:16  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.2  2004/04/16 20:32:52  frank
// fixed reading visualization presets
//
// Revision 1.1  2004/04/15 15:51:31  frank
// split out the preset manipulation from the window/level manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorPreset.C,v 1.13 2006/12/21 15:11:14 gdavidson Exp $
// $Id: vxManipulatorPreset.C,v 1.13 2006/12/21 15:11:14 gdavidson Exp $
