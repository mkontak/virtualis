// $Id: v3D_PresetAdjusterPET.cpp,v 1.3.8.1 2009/11/16 20:11:23 dongqing Exp $
//
// Copyright (c) 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_PresetAdjusterPET.h"
#include "v3D_SelfAdaptivePTWindowLevel.h"
#include "v3D_Colormap.h"
#include "v3D_Volume.h"
#include "v3D_Preset.h"
#include "v3D_VisualizationPreset.h"

// namespaces
using namespace System;
USING_VISUALIZATION_NS


/**
 * default constructor
 */
PresetAdjusterPET::PresetAdjusterPET()
{
} // PresetAdjusterPET()


/**
 * Adjusts window level based on modality.
 *
 * @param     mpPreset      The preset.
 * @param     mpVolume      The volume.
 * @return    true if the parameters are successfully modified; otherwise false
 */
void PresetAdjusterPET::Adjust( Preset ^ mpPreset, Volume ^ mpVolume )
{ 
  // get the unmanged vxPreset reference
  vxPreset * pPreset = ToNativePresetPtr( mpPreset->GetElementPtr() );

  // grab a reference to the contained 2d and 3d visualzation preset vectors
  const std::vector< const vxVisualizationPreset * > & vPresets2d = pPreset->GetVisualizationPresets2D();
  const std::vector< const vxVisualizationPreset * > & vPresets3d = pPreset->GetVisualizationPresets3D();

  bool bIsNotReady = false;
  try
  {  /// this data availability check is necessary since the new viewer launch was plit into multi-thread
     /// the volume data load is in background thread.
     bIsNotReady = ( &mpVolume == NULL );
     ToNativeVolumeUint2Ptr( mpVolume->GetVolumePtr() )->GetHeader().GetVolDim();
  }
  catch( ... )
  {
    bIsNotReady = true;
  }

  if( bIsNotReady )
  { 
    for ( int iIndex( 0 ); iIndex<vPresets2d.size(); ++iIndex )
    {
      vxColormap * pColormap = vPresets2d[iIndex]->GetColormap();
      if ( pColormap != NULL )
      {
        vxWindowLevel wl;

        // the text for the menu item
        std::string sName( pColormap->GetName() );

        // reset colormap's all entries
        if ( sName == "General" )
        { // for white background
          wl.SetLevel( 0.01F );
          wl.SetWindow( 0.005F );
        } // General              
        else if ( sName == "Inverse" )
        { // for back background
          wl.SetLevel( 0.01F );
          wl.SetWindow( 0.005F );
        } // inverse

        pColormap->SetWindowLevel( wl );
      } // if
    } // for ( iIndex )

    for ( int iIndex( 0 ); iIndex<vPresets3d.size(); ++iIndex )
    {
      vxColormap * pColormap= vPresets3d[iIndex]->GetColormap();

      if ( pColormap != NULL )
      {      
        vxWindowLevel wl;

        // the text for the menu item
        std::string sName( pColormap->GetName() );

        // reset colormap's all entries
        if ( sName == "General" )
        { // for white background
          wl.SetLevel( 0.01F );
          wl.SetWindow( 0.005F );
          //wl.SetLevel( mpComputeWindowLevel->Level3d);
          //wl.SetWindow( mpComputeWindowLevel->Window3d );
        } // General              
        else if ( sName == "Inverse" )
        { // for black background
          wl.SetLevel( 0.01F );
          wl.SetWindow( 0.005F );
          //wl.SetLevel( mpComputeWindowLevel->Level3d );
          //wl.SetWindow( mpComputeWindowLevel->Window3d );
        } // Inverse

        pColormap->SetWindowLevel( wl );
      } // if
    } // for ( iIndex )

  } // if ///////////////////////////////////////////////////////////
  else
  {
    // adaptive adjust window / level for PET case
    SelfadaptivePTWindowLevel ^ mpComputeWindowLevel = gcnew SelfadaptivePTWindowLevel( mpVolume );
    if ( mpComputeWindowLevel->IsAvailable() )
    {
      for ( int iIndex( 0 ); iIndex<vPresets2d.size(); ++iIndex )
      {
        vxColormap * pColormap = vPresets2d[iIndex]->GetColormap();
        if ( pColormap != NULL )
        {
          vxWindowLevel wl;

          // the text for the menu item
          std::string sName( pColormap->GetName() );

          // reset colormap's all entries
          if ( sName == "General" )
          { // for white background
            wl.SetLevel( mpComputeWindowLevel->Level2d );
            wl.SetWindow( mpComputeWindowLevel->Window2d );
          } // General              
          else if ( sName == "Inverse" )
          { // for back background
            wl.SetLevel( mpComputeWindowLevel->Level2d );
            wl.SetWindow( mpComputeWindowLevel->Window2d );
          } // inverse

          pColormap->SetWindowLevel( wl );
        } // if
      } // for ( iIndex )

      for ( int iIndex( 0 ); iIndex<vPresets3d.size(); ++iIndex )
      {
        vxColormap * pColormap= vPresets3d[iIndex]->GetColormap();

        if ( pColormap != NULL )
        {      
          vxWindowLevel wl;

          // the text for the menu item
          std::string sName( pColormap->GetName() );

          // reset colormap's all entries
          if ( sName == "General" )
          { // for white background
            wl.SetLevel( mpComputeWindowLevel->Level3d);
            wl.SetWindow( mpComputeWindowLevel->Window3d );
          } // General              
          else if ( sName == "Inverse" )
          { // for black background
            wl.SetLevel( mpComputeWindowLevel->Level3d);
            wl.SetWindow( mpComputeWindowLevel->Window3d );
          } // Inverse

          pColormap->SetWindowLevel( wl );
        } // if
      } // for ( iIndex )
    } // if new window level is available
  } // else /////////////////////////////////

} // Adjust( Preset ^ mpPreset, Volume ^ mpVolume )()


// $Log: v3D_PresetAdjusterPET.cpp,v $
// Revision 1.3.8.1  2009/11/16 20:11:23  dongqing
// correctly handle the PET and MRI self-adaptive presets adjust
// in multi-thread load situation
//
// Revision 1.3  2007/02/28 22:16:49  dongqing
// update PT and MRI W/L computation due to the change of using
// significantBits to determine maximum value in volume.
//
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2005/01/07 16:58:51  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PresetAdjusterPET.cpp,v 1.3.8.1 2009/11/16 20:11:23 dongqing Exp $
// $Id: v3D_PresetAdjusterPET.cpp,v 1.3.8.1 2009/11/16 20:11:23 dongqing Exp $
