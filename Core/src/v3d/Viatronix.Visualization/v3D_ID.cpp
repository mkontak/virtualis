// $Id: v3D_ID.cpp,v 1.25.2.3 2009/07/29 16:43:29 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_ID.h"
#include "vxIDs.h"


// namespaces
USING_VISUALIZATION_NS


/************************************************************************/
/*                             ElementID                                */
/************************************************************************/
  
/// get colormap editor id
System::String ^ ElementID::DataColormapEditor::get     () { return gcnew System::String( vxIDs::DataColormapEditor     .c_str() ); }

/// get data annotation create id
System::String ^ ElementID::DataAnnotationCreate::get   () { return gcnew System::String( vxIDs::DataAnnotationCreate   .c_str() ); }

/// get data plane oblque id
System::String ^ ElementID::DataPlaneObliqueCreate::get () { return gcnew System::String( vxIDs::DataPlaneObliqueCreate .c_str() ); }

/// get crop box id
System::String ^ ElementID::CropBox::get                () { return gcnew System::String( vxIDs::CropBox               .c_str() ); }

/// get phase number id
System::String ^ ElementID::PhaseNumber::get            () { return gcnew System::String( vxIDs::PhaseNumber           .c_str() ); }

/// get number of phases id
System::String ^ ElementID::NumPhases::get              () { return gcnew System::String( vxIDs::NumPhases             .c_str() ); }

/// get box of interest id
System::String ^ ElementID::BoxOfInterest::get          () { return gcnew System::String( vxIDs::BoxOfInterest         .c_str() ); }

/// get view oblique detail id
System::String ^ ElementID::ViewObliqueData::get        () { return gcnew System::String( vxIDs::ViewObliqueData       .c_str() ); }

/// get overlay strings id
System::String ^ ElementID::OverlayStrings::get         () { return gcnew System::String( vxIDs::OverlayStrings        .c_str() ); }

/// get path id
System::String ^ ElementID::Path::get                   () { return gcnew System::String( vxIDs::Path                  .c_str() ); }

/// get path index position id
System::String ^ ElementID::PathIndexPosition::get      () { return gcnew System::String( vxIDs::PathIndexPosition     .c_str() ); }

/// get progressive refinement id
System::String ^ ElementID::ProgressiveRefinement::get  () { return gcnew System::String( vxIDs::ProgressiveRefinement .c_str() ); }

/// get dataset index id
System::String ^ ElementID::DatasetIndex::get           () { return gcnew System::String( vxIDs::DatasetIndex          .c_str() ); }

/// get dataset index cycling id
System::String ^ ElementID::DatasetIndexCycling::get    () { return gcnew System::String( vxIDs::DatasetIndexCycling   .c_str() ); }

/// get environment list id
System::String ^ ElementID::EnvironmentList::get        () { return gcnew System::String( vxIDs::EnvironmentList       .c_str() ); }

/// get ribbon id
System::String ^ ElementID::Ribbon::get                 () { return gcnew System::String( vxIDs::Ribbon              .c_str() ); }

/// get detail volume id
System::String ^ ElementID::DetailVolume::get           () { return gcnew System::String( vxIDs::DetailVolume        .c_str() ); }

/// get vessel array id
System::String ^ ElementID::VesselArray::get            () { return gcnew System::String( vxIDs::VesselArray         .c_str() ); }

/// get current vessel section id
System::String ^ ElementID::CurrentVesselSection::get   () { return gcnew System::String( vxIDs::CurrentVesselSection.c_str() ); }

/// get vessel preferences id
System::String ^ ElementID::VesselPreferences::get      () { return gcnew System::String( vxIDs::VesselPreferences   .c_str() ); }

/// get seedpoints id
System::String ^ ElementID::Seedpoints::get             () { return gcnew System::String( vxIDs::Seedpoints          .c_str() ); }

/// get vesselness loaded id
System::String ^ ElementID::IsVesselnessLoaded::get     () { return gcnew System::String( vxIDs::IsVesselnessLoaded  .c_str() ); }

/// get incremental growth id
System::String ^ ElementID::IncrementalGrowth::get      () { return gcnew System::String( vxIDs::IncrementalGrowth   .c_str() ); }

/// get vessel detail endoluminal id
System::String ^ ElementID::VesselDetailEndoluminalColormap::get() { return gcnew System::String( vxIDs::VesselDetailEndoluminalColormap.c_str() ); }

/// get endo visual preset index id
System::String ^ ElementID::EndoluminalVisualizationPresetIndex::get() { return gcnew System::String( vxIDs::EndoluminalVisualizationPresetIndex.c_str() ); }

/// get vessel detail 2D map id
System::String ^ ElementID::VesselDetailColormap2D::get() { return gcnew System::String( vxIDs::VesselDetailColormap2D.c_str() ); }

/// get vessel detail 3D map id
System::String ^ ElementID::VesselDetailColormap3D::get() { return gcnew System::String( vxIDs::VesselDetailColormap3D.c_str() ); }

/// get graph id
System::String ^ ElementID::Graph::get () { return gcnew  System::String( vxIDs::Graph   .c_str() ); }  

/// get cycle settings id
System::String ^ ElementID::CycleSettings::get () { return gcnew  System::String( vxIDs::CycleSettings   .c_str() ); }  

/// get phase info id
System::String ^ ElementID::PhaseInfo::get () { return gcnew  System::String( vxIDs::PhaseInfo           .c_str() ); }  

/// get sync plane id
System::String ^ ElementID::SyncPlane::get () { return gcnew  System::String( vxIDs::SyncPlane           .c_str() ); }  

/// get sync data id
System::String ^ ElementID::SyncData::get () { return gcnew  System::String( vxIDs::SyncData           .c_str() ); }  

/// get title string id
System::String ^ ElementID::TitleString::get () { return gcnew  System::String( vxIDs::TitleString       .c_str() ); }  

/// get fullscreen id
System::String ^ ElementID::FullScreen::get () { return gcnew  System::String( vxIDs::FullScreen           .c_str() ); }  

/// get mesh id
System::String ^ ElementID::Mesh::get () { return gcnew  System::String( vxIDs::Mesh       .c_str() ); }  

/// get polar graph id
System::String ^ ElementID::PolarGraph::get () { return gcnew  System::String( vxIDs::PolarGraph           .c_str() ); }  


// $Log: v3D_ID.cpp,v $
// Revision 1.25.2.3  2009/07/29 16:43:29  kchalupa
// Code Walkthrough
//
// Revision 1.25.2.2  2009/03/26 14:52:02  gdavidson
// Added managed equivalents for vxIDs PolarGraph and Mesh
//
// Revision 1.25.2.1  2008/11/24 17:14:14  gdavidson
// Issue #6171: Added fullscreen double click
//
// Revision 1.25  2008/05/02 05:01:23  cqian
// add syncData ID
//
// Revision 1.24  2008/03/20 16:55:51  cqian
// add titlestring
//
// Revision 1.23  2007/10/01 14:29:27  gdavidson
// Added SyncPlane
//
// Revision 1.22  2007/09/19 14:32:21  gdavidson
// Added PhaseInfo id
//
// Revision 1.21  2007/05/03 21:37:03  gdavidson
// Added CycleSettings
//
// Revision 1.20  2006/12/21 15:03:30  gdavidson
// Issue #5189 & 5106: Added EndoluminalVisualizationPresetIndex, VesselDetailColormap2D and VesselDetailColormap3D
//
// Revision 1.19  2006/11/02 04:10:27  romy
// added Graph
//
// Revision 1.18  2006/10/05 17:28:06  geconomos
// removed slice shadow color id
//
// Revision 1.17  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.16  2006/09/25 20:09:35  geconomos
// removed DetailCropBox DetailCamera and EndoluminalCamera element ids
//
// Revision 1.15  2006/09/25 13:27:58  geconomos
// moving shared code for vascular and cardiac to visualization dll
//
// Revision 1.14  2006/09/14 20:24:38  geconomos
// moved some ids from vascular project
//
// Revision 1.13  2006/09/01 13:26:49  frank
// moved seedpoints to viewer/visualization
//
// Revision 1.12  2006/08/17 14:07:16  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.11  2006/03/27 19:43:39  gdavidson
// Added EnvironmentList and SliceShadowColor
//
// Revision 1.10  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.9.2.1  2005/06/22 17:25:31  frank
// added dataset index cycling property
//
// Revision 1.9  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.8  2004/10/23 09:58:12  frank
// moved fusion IDs to the fusion plugin
//
// Revision 1.7  2004/10/04 15:12:15  frank
// added dataset controller to handle 4D data with multiple renderer data objs
//
// Revision 1.6  2004/09/10 19:52:08  mkontak
// Added Fusion Element ID's
//
// Revision 1.5  2004/09/09 18:34:15  michael
// Added fusion IDs in fusion plugin and took them out of core
//
// Revision 1.4  2004/04/06 18:20:52  frank
// renamed vessel to path
//
// Revision 1.3  2004/03/30 18:16:20  frank
// split out the progressive refinement flag from the renderer CPU 3d data so that it can be applied to all renderers in an environment at once
//
// Revision 1.2  2004/03/26 15:51:39  frank
// added fusion element stack
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.20.2.1  2004/02/24 20:08:26  michael
// coding standard: assign NULL after deletion
//
// Revision 1.20  2004/01/16 18:38:27  frank
// added renderer 3d cpu data extended element
//
// Revision 1.19  2003/12/18 14:13:30  frank
// Renamed the element ID filename to match class name
//
// Revision 1.18  2003/08/28 16:51:36  frank
// Added path index position
//
// Revision 1.17  2003/08/20 14:28:13  michael
// removed old TransferFunctionEditor ID
//
// Revision 1.16  2003/08/13 20:02:50  frank
// Removed old element
//
// Revision 1.15  2003/08/13 17:59:41  michael
// added ID for PlaneObliqueCreateData
//
// Revision 1.14  2003/08/12 16:57:57  frank
// Added a vessel identifier
//
// Revision 1.13  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.12  2003/05/06 17:56:58  geconomos
// Coding standards
//
// Revision 1.11  2003/04/10 01:24:11  frank
// Added OverlayStrings
//
// Revision 1.10  2003/03/05 13:51:54  frank
// Added CineSeries element
//
// Revision 1.9  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.8  2002/10/04 18:11:38  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.7  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.6  2002/09/25 22:37:46  wenli
// Add BoxOfInterest
//
// Revision 1.5  2002/09/24 18:59:11  geconomos
// Exception handling and formatting.
//
// Revision 1.4  2002/09/24 17:36:31  geconomos
// Exception handling and formatting.
//
// Revision 1.3  2002/09/20 16:41:08  geconomos
// Changed implemetation of class vxID
//
// Revision 1.2  2002/09/19 14:26:00  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.1  2002/09/16 20:18:32  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ID.cpp,v 1.25.2.3 2009/07/29 16:43:29 kchalupa Exp $
// $Id: v3D_ID.cpp,v 1.25.2.3 2009/07/29 16:43:29 kchalupa Exp $
