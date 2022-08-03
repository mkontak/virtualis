// $Id: v3D_PresetAdjusterMR.cpp,v 1.5.8.2 2009/11/16 20:11:23 dongqing Exp $
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
#include "v3D_PresetAdjusterMR.h"
#include "v3D_SelfAdaptiveMRWindowLevel.h"
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
PresetAdjusterMR::PresetAdjusterMR()
{
} // PresetAdjusterMR()


/**
 * Adjusts window level based on modality.
 *
 * @param     mpPreset      The preset.
 * @param     mpVolume      The volume.
 * @return    true if the parameters are successfully modified; otherwise false
 */
void PresetAdjusterMR::Adjust( Preset ^ mpPreset, Volume ^ mpVolume )
{
  // get the unmanged vxPreset reference
  vxPreset * pPreset = ToNativePresetPtr( mpPreset->GetElementPtr() );

  // grab a reference to the contained 2d and 3d visualzation preset vectors
 const  std::vector< const vxVisualizationPreset * > & vPresets2d = pPreset->GetVisualizationPresets2D();
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
    /// The following W/L setting is hacked due to the cardiac application. It needs a better solution later. Dongqing 9 September 2009.
    // adjust 2d visualtion presets    
    for ( int iIndex( 0 ); iIndex < vPresets2d.size(); ++iIndex )
    {
      vxColormap * pColormap  = vPresets2d[ iIndex ]->GetColormap( );
      if ( pColormap != NULL )
      {
        vxWindowLevel  wl;         
        
        // the text for the menu item
        std::string sName( pColormap->GetName() );

        // reset colormap's all entries
        if ( sName == "MRA" )
        { // for MRA
          wl.SetLevel( 0.002F );
          wl.SetWindow( 0.01F );
          //wl.SetLevel( mpComputeWindowLevel->Level2dMRA );
          //wl.SetWindow( mpComputeWindowLevel->Window2dMRA );
        } // MRA
        else if( sName == "MIP" )
        {
          wl.SetLevel( 0.002F );
          wl.SetWindow( 0.01F );
          //wl.SetLevel( mpComputeWindowLevel->Level2dMIP );
          //wl.SetWindow( mpComputeWindowLevel->Window2dMIP );
        } // MIP
        else
        { // general
          wl.SetLevel( 0.002F );
          wl.SetWindow( 0.01F );
          //wl.SetLevel( mpComputeWindowLevel->Level2dGeneral );
          //wl.SetWindow( mpComputeWindowLevel->Window2dGeneral );
        } // General
        pColormap->SetWindowLevel( wl );
      } // if
    } // for ( iIndex )

    for ( int iIndex( 0 ); iIndex < vPresets3d.size(); ++iIndex )
    {
      vxColormap * pColormap  = vPresets3d[ iIndex ]->GetColormap();
      if ( pColormap != NULL )
      {
        vxWindowLevel wl; 

        // the text for the menu item
        std::string sName( pColormap->GetName() );

        // reset colormap's all entries
        if ( sName == "MRA" )
        { // for MRA
          wl.SetLevel( 0.002F );
          wl.SetWindow( 0.01F );
          //wl.SetLevel( mpComputeWindowLevel->Level3dMRA );
          //wl.SetWindow( mpComputeWindowLevel->Window3dMRA );
        } // MRA              
        else if ( sName == "MIP" )
        { // for MRA
          wl.SetLevel( 0.002F );
          wl.SetWindow( 0.01F );
          //wl.SetLevel( mpComputeWindowLevel->Level3dMIP );
          //wl.SetWindow( mpComputeWindowLevel->Window3dMIP );
        } // MRA              
        else if ( sName == "EndoView" )
        { // for Endoluminal view
          wl.SetLevel( 0.002F );
          wl.SetWindow( 0.01F );
          //wl.SetLevel( mpComputeWindowLevel->Level3dEndoView );
          //wl.SetWindow( mpComputeWindowLevel->Window3dEndoView );
        } // EndoView
        else
        { // general
          wl.SetLevel( 0.002F );
          wl.SetWindow( 0.01F );
          //wl.SetLevel( mpComputeWindowLevel->Level3dGeneral );
          //wl.SetWindow( mpComputeWindowLevel->Window3dGeneral );
        } // General
        pColormap->SetWindowLevel( wl );
      } // if
    } // for ( iIndex )

  } //if /////////////////////////////////////////////////////
  else
  {
    // adaptive adjust window / level for MRI case
    SelfadaptiveMRWindowLevel ^ mpComputeWindowLevel = gcnew SelfadaptiveMRWindowLevel( mpVolume );
    if ( mpComputeWindowLevel->IsAvailable() )
    {
      // adjust 2d visualtion presets    
      for ( int iIndex( 0 ); iIndex < vPresets2d.size(); ++iIndex )
      {
        vxColormap * pColormap  = vPresets2d[ iIndex ]->GetColormap( );
        if ( pColormap != NULL )
        {
          vxWindowLevel  wl;         
          
          // the text for the menu item
          std::string sName( pColormap->GetName() );

          // reset colormap's all entries
          if ( sName == "MRA" )
          { // for MRA
            wl.SetLevel( mpComputeWindowLevel->Level2dMRA );
            wl.SetWindow( mpComputeWindowLevel->Window2dMRA );
          } // MRA
          else if( sName == "MIP" )
          {
            wl.SetLevel( mpComputeWindowLevel->Level2dMIP );
            wl.SetWindow( mpComputeWindowLevel->Window2dMIP );
          } // MIP
          else
          { // general
            wl.SetLevel( mpComputeWindowLevel->Level2dGeneral );
            wl.SetWindow( mpComputeWindowLevel->Window2dGeneral );
          } // General
          pColormap->SetWindowLevel( wl );
        } // if
      } // for ( iIndex )

     for ( int iIndex( 0 ); iIndex < vPresets3d.size(); ++iIndex )
      {
        vxColormap * pColormap  = vPresets3d[ iIndex ]->GetColormap();
        if ( pColormap != NULL )
        {
          vxWindowLevel wl; 

          // the text for the menu item
          std::string sName( pColormap->GetName() );

          // reset colormap's all entries
          if ( sName == "MRA" )
          { // for MRA
            wl.SetLevel( mpComputeWindowLevel->Level3dMRA );
            wl.SetWindow( mpComputeWindowLevel->Window3dMRA );
          } // MRA              
          else if ( sName == "MIP" )
          { // for MRA
            wl.SetLevel( mpComputeWindowLevel->Level3dMIP );
            wl.SetWindow( mpComputeWindowLevel->Window3dMIP );
          } // MRA              
          else if ( sName == "EndoView" )
          { // for Endoluminal view
            wl.SetLevel( mpComputeWindowLevel->Level3dEndoView );
            wl.SetWindow( mpComputeWindowLevel->Window3dEndoView );
          } // EndoView
          else
          { // general
            wl.SetLevel( mpComputeWindowLevel->Level3dGeneral );
            wl.SetWindow( mpComputeWindowLevel->Window3dGeneral );
          } // General
          pColormap->SetWindowLevel( wl );
        } // if
      } // for ( iIndex )
    } // if new window level is available

  } // else //////////////////////////////////////////////

} // Adjust( mpPreset, mpVolume )


// $Log: v3D_PresetAdjusterMR.cpp,v $
// Revision 1.5.8.2  2009/11/16 20:11:23  dongqing
// correctly handle the PET and MRI self-adaptive presets adjust
// in multi-thread load situation
//
// Revision 1.5.8.1  2009/09/09 14:10:24  dongqing
// hack the selfadaptive W/L setting to allow XP launch without crash.
// Need a better solution later
//
// Revision 1.5  2007/02/28 22:16:49  dongqing
// update PT and MRI W/L computation due to the change of using
// significantBits to determine maximum value in volume.
//
// Revision 1.4  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.4.1  2005/07/20 20:17:22  geconomos
// issue# 4296: mr presets not working
//
// Revision 1.2  2005/01/20 15:56:15  frank
// fixed typo that was crashing MRI preset
//
// Revision 1.1  2005/01/07 16:58:51  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PresetAdjusterMR.cpp,v 1.5.8.2 2009/11/16 20:11:23 dongqing Exp $
// $Id: v3D_PresetAdjusterMR.cpp,v 1.5.8.2 2009/11/16 20:11:23 dongqing Exp $