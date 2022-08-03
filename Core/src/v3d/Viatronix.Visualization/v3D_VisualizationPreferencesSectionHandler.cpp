// $Id: v3D_VisualizationPreferencesSectionHandler.cpp,v 1.11 2007/10/24 20:09:25 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_VisualizationPreferencesSectionHandler.h"
#include "vxVisualizationPreferences.h"
#include "v3D_Enums.h"
#include "v3D_VisualizationGraph.h"


// defines
#define FILE_REVISION "$Revision: 1.11 $"


// namespaces
using namespace System::Xml;
USING_VISUALIZATION_NS
USING_GRAPH_NS


/**
 * REturns the XMlNode containing the configuration information.
 *
 * @param       mpParent            The configuration settings in a corresponding parent configuration section.
 * @param       mpConfigContext     This parameter is reserved and is a null reference.
 * @param       mpSection           The XmlNode that contains the configuration information from the configuration file.        
 * @return              The XmlNode that contains the configuration information from the configuration file.
 */
System::Object ^ VisualizationPreferencesSectionHandler::Create( System::Object ^ mpParent, System::Object ^ mpConfigContext, System::Xml::XmlNode ^ mpSection )
{
  // rendering statistics
  if( mpSection->Attributes[ "showStatistics" ] != nullptr )
    vxVisualizationPreferences::SetShowStatistics( System::Boolean::Parse( mpSection->Attributes->GetNamedItem( "showStatistics" )->Value ) );

  // max render threads
  if( mpSection->Attributes[ "maxRenderThreads" ] != nullptr )
    vxVisualizationPreferences::SetMaxRenderThreads( System::Int32::Parse( mpSection->Attributes[ "maxRenderThreads" ]->Value ));

  // high quailty thread enabled
  if( mpSection->Attributes[ "highQualityEnabled" ] != nullptr )
    vxVisualizationPreferences::SetHighQualityEnabled( System::Boolean::Parse( mpSection->Attributes[ "highQualityEnabled" ]->Value ));

	/// show coordinates
  if( mpSection->Attributes[ "showCoordinates" ] != nullptr )
    vxVisualizationPreferences::SetShowCoordinates( System::Boolean::Parse( mpSection->Attributes[ "showCoordinates" ]->Value ));

	/// show HU Flag
  if( mpSection->Attributes[ "showHU" ] != nullptr )
    vxVisualizationPreferences::SetShowHU( System::Boolean::Parse( mpSection->Attributes[ "showHU" ]->Value ));

  // 2D tile size
  XmlNode ^ mpTileSize2D = mpSection->SelectSingleNode( "preference[@name=\'2D\']" );
  if( mpTileSize2D != nullptr && mpTileSize2D->Attributes[ "tilesize" ] != nullptr )
    vxVisualizationPreferences::SetTileSize2D( System::Int32::Parse( mpTileSize2D->Attributes[ "tilesize" ]->Value ));


  // 3D frame rate
  XmlNode ^ mpFrameRate3D = mpSection->SelectSingleNode( "preference[@name=\'3D\']/framerate" );
  vxVisualizationPreferences::SetFrameRateMin3D( static_cast< float4 >( System::Double::Parse( mpFrameRate3D->Attributes->GetNamedItem( "min" )->Value )));
  vxVisualizationPreferences::SetFrameRateMax3D( static_cast< float4 >( System::Double::Parse( mpFrameRate3D->Attributes->GetNamedItem( "max" )->Value )));
  vxVisualizationPreferences::SetMaintainFrameRate3D( System::Boolean::Parse( mpFrameRate3D->Attributes->GetNamedItem( "maintainFrameRate" )->Value ));

  // 3D render size
  XmlNode ^ mpRenderSize3D = mpSection->SelectSingleNode( "preference[@name=\'3D\']/rendersize" );
  vxVisualizationPreferences::SetRenderSizeMin3D( System::Int32::Parse( mpRenderSize3D->Attributes->GetNamedItem( "min" )->Value ));
  vxVisualizationPreferences::SetRenderSizeMax3D( System::Int32::Parse( mpRenderSize3D->Attributes->GetNamedItem( "max" )->Value ));
  vxVisualizationPreferences::SetRenderSizeInitial3D( System::Int32::Parse( mpRenderSize3D->Attributes->GetNamedItem( "initialSize" )->Value ));

  // 3D tile size
  XmlNode ^ mpTileSize3D = mpSection->SelectSingleNode( "preference[@name=\'3D\']" );
  if( mpTileSize3D->Attributes[ "tilesize" ] != nullptr )
    vxVisualizationPreferences::SetTileSize3D( System::Int32::Parse( mpTileSize3D->Attributes[ "tilesize" ]->Value ));

  // oblique frame rate
  XmlNode ^ mpFrameRateOblique = mpSection->SelectSingleNode( "preference[@name=\'oblique\']/framerate" );
  vxVisualizationPreferences::SetFrameRateMinOblique( static_cast< float4 >( System::Double::Parse( mpFrameRateOblique->Attributes->GetNamedItem( "min" )->Value )));
  vxVisualizationPreferences::SetFrameRateMaxOblique( static_cast< float4 >( System::Double::Parse( mpFrameRateOblique->Attributes->GetNamedItem( "max" )->Value )));
  vxVisualizationPreferences::SetMaintainFrameRateOblique( System::Boolean::Parse( mpFrameRateOblique->Attributes->GetNamedItem( "maintainFrameRate" )->Value ));

  // oblique render size
  XmlNode ^ mpRenderSizeOblique = mpSection->SelectSingleNode( "preference[@name=\'oblique\']/rendersize" );
  vxVisualizationPreferences::SetRenderSizeMinOblique( System::Int32::Parse( mpRenderSizeOblique->Attributes->GetNamedItem( "min" )->Value ));
  vxVisualizationPreferences::SetRenderSizeMaxOblique( System::Int32::Parse( mpRenderSizeOblique->Attributes->GetNamedItem( "max" )->Value ));
  vxVisualizationPreferences::SetRenderSizeInitialOblique( System::Int32::Parse( mpRenderSizeOblique->Attributes->GetNamedItem( "initialSize" )->Value ));

  // oblique tile size
  XmlNode ^ mpTileSizeOblique = mpSection->SelectSingleNode( "preference[@name=\'oblique\']" );
  if( mpTileSizeOblique->Attributes[ "tilesize" ] != nullptr )
    vxVisualizationPreferences::SetTileSizeOblique( System::Int32::Parse( mpTileSizeOblique->Attributes[ "tilesize" ]->Value ));


  // setup the rendering context mode
  if( mpSection->Attributes[ "context" ] != nullptr )
  {
    RenderingContextMode eMode = static_cast< RenderingContextMode >( System::Enum::Parse( 
                                              RenderingContextMode::typeid, mpSection->Attributes[ "context" ]->Value, true ));
    VisualizationGraph::Instance->RenderingContextMode = eMode;
  }


  return nullptr;
} // Create( mpParent, mpConfigContext, mpSection )


#pragma region revision history

// $Log: v3D_VisualizationPreferencesSectionHandler.cpp,v $
// Revision 1.11  2007/10/24 20:09:25  gdavidson
// Added configuration for RenderingContextMode
//
// Revision 1.10  2007/06/29 20:20:42  geconomos
// added rendering tile size
//
// Revision 1.9  2007/03/01 20:38:11  gdavidson
// Commented code
//
// Revision 1.8  2007/01/25 22:44:46  gdavidson
// Fixed an error with SetRenderSizeInitialOblique
//
// Revision 1.7  2007/01/19 15:28:59  geconomos
// reorganized visualization preferences
//
// Revision 1.6  2007/01/17 22:11:16  gdavidson
// Added highQualityEnabled attribute
//
// Revision 1.5  2006/12/05 22:36:16  gdavidson
// Added null reference checks for showCoordinates and showHU attributes
//
// Revision 1.4  2006/11/24 19:53:05  romy
// reading  the shosHU parameter from the Config file
//
// Revision 1.3  2006/10/31 20:50:31  romy
// added show coordinate vizualization preset
//
// Revision 1.2  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2006/09/27 17:09:54  geconomos
// changed file extension to .cpp
//
// Revision 1.7  2006/09/20 15:05:26  frank
// added preference for displaying rendering manager statistics
//
// Revision 1.6  2006/08/23 17:29:31  geconomos
// added "DrawTimingStatistics"
//
// Revision 1.5  2006/08/23 15:53:03  gdavidson
// Added SliceShadowsAllEnabled
//
// Revision 1.4  2006/08/03 14:21:18  geconomos
// class was totally foobar; working now
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VisualizationPreferencesSectionHandler.cpp,v 1.11 2007/10/24 20:09:25 gdavidson Exp $
// $Id: v3D_VisualizationPreferencesSectionHandler.cpp,v 1.11 2007/10/24 20:09:25 gdavidson Exp $

#pragma endregion