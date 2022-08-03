// $Id: v3D_ID.h,v 1.25.2.3 2009/07/29 16:43:29 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// namspaces
OPEN_VISUALIZATION_NS


// class definition
public ref class ElementID
{
// id declarations
public:
  
  /// gets the "CropBox" element id
  property static System::String ^ CropBox { System::String ^ get(); }  
  
  /// gets the "PhaseNumber" element id
  property static System::String ^ PhaseNumber { System::String ^ get(); }  
  
  /// gets the "NumPhases" element id
  property static System::String ^ NumPhases { System::String ^ get(); }
  
  /// gets the "BoxOfInterest" element id
  property static System::String ^ BoxOfInterest { System::String ^ get(); }
  
  /// gets the "ViewObliqueData" element id
  property static System::String ^ ViewObliqueData { System::String ^ get(); }
  
  /// gets the "OverlayStrings" element id
  property static System::String ^ OverlayStrings { System::String ^ get(); }

  /// gets the path element id
  property static System::String ^ Path { System::String ^ get(); }

  /// gets the "ColormapEditorData" element id
  property static System::String ^ DataColormapEditor { System::String ^ get(); }

  /// gets the annotation creation data element id
  property static System::String ^ DataAnnotationCreate { System::String ^ get(); }

  /// gets the plane oblique creation data element id
  property static System::String ^ DataPlaneObliqueCreate { System::String ^ get(); }

  /// gets the path index position
  property static System::String ^ PathIndexPosition { System::String ^ get(); }

  /// gets the progressive refinement rendering flag
  property static System::String ^ ProgressiveRefinement { System::String ^ get(); }

  /// gets the dataset index
  property static System::String ^ DatasetIndex { System::String ^ get(); }

  /// gets the dataset index cycling element id
  property static System::String ^ DatasetIndexCycling { System::String ^ get(); }

  /// gets the environment list element id
  property static System::String ^ EnvironmentList { System::String ^ get(); }

  /// gets the ribbion id
  property static System::String ^ Ribbon { System::String ^ get(); }

  /// get the vessel ribbon id
  property static System::String ^ VesselArray { System::String ^ get(); }

  /// get the detail volume id
  property static System::String ^ DetailVolume { System::String ^ get(); }

  /// get the current vessel section id
  property static System::String ^ CurrentVesselSection { System::String ^ get(); }

  /// get the vessel preferences id
  property static System::String ^ VesselPreferences { System::String ^ get(); }

  /// get the seedpoints id
  property static System::String ^ Seedpoints { System::String ^ get(); }

  /// determine if vesselness is loaded
  property static System::String ^ IsVesselnessLoaded { System::String ^ get(); }

  /// determin if incremental growth is set
  property static System::String ^ IncrementalGrowth { System::String ^ get(); }

  /// get vessel detail endo colormap id
  property static System::String ^ VesselDetailEndoluminalColormap { System::String ^ get(); }

  /// Get endo visualization preset index id
  property static System::String ^ EndoluminalVisualizationPresetIndex { System::String ^ get(); }

  /// get vessel detail colormap 2D id
  property static System::String ^ VesselDetailColormap2D { System::String ^ get(); }  

  /// get vessel detail colormap 3D id
  property static System::String ^ VesselDetailColormap3D { System::String ^ get(); }

	/// gets the "CropBox" element id
  property static System::String ^ Graph { System::String ^ get(); }  

  /// gets the "CycleSettings" element id
  property static System::String ^ CycleSettings { System::String ^ get(); }  

  /// gets the "PhaseInfo" element id
  property static System::String ^ PhaseInfo {  System::String ^ get(); }

  /// gets the "SyncPlane" element id
  property static System::String ^ SyncPlane {  System::String ^ get(); }

  /// gets the "SyncData" element id
  property static System::String ^ SyncData {  System::String ^ get(); }

  /// gets the "TitleString" element id
  property static System::String ^ TitleString {  System::String ^ get(); }

  /// gets the "FullScreen" element id
  property static System::String ^ FullScreen {  System::String ^ get(); }

  /// gets the "Mesh" element id
  property static System::String ^ Mesh {  System::String ^ get(); }

  /// gets the "PolarGraph" element id
  property static System::String ^ PolarGraph {  System::String ^ get(); }
  
}; // class ElementID


CLOSE_VISUALIZATION_NS


// $Log: v3D_ID.h,v $
// Revision 1.25.2.3  2009/07/29 16:43:29  kchalupa
// Code Walkthrough
//
// Revision 1.25.2.2  2009/03/26 14:52:02  gdavidson
// Added managed equivalents for vxIDs PolarGraph and Mesh
//
// Revision 1.25.2.1  2008/11/24 17:14:14  gdavidson
// Issue #6171: Added fullscreen double click
//
// Revision 1.25  2008/05/02 05:01:28  cqian
// add syncData ID
//
// Revision 1.24  2008/03/20 16:55:55  cqian
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
// Revision 1.20  2006/12/21 15:00:16  gdavidson
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
// Revision 1.5  2004/09/09 18:34:25  michael
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
// Revision 1.17  2004/01/16 18:38:27  frank
// added renderer 3d cpu data extended element
//
// Revision 1.16  2003/12/18 14:13:30  frank
// Renamed the element ID filename to match class name
//
// Revision 1.15  2003/08/28 16:51:36  frank
// Added path index position
//
// Revision 1.14  2003/08/20 14:28:13  michael
// removed old TransferFunctionEditor ID
//
// Revision 1.13  2003/08/13 20:02:50  frank
// Removed old element
//
// Revision 1.12  2003/08/13 17:59:41  michael
// added ID for PlaneObliqueCreateData
//
// Revision 1.11  2003/08/12 16:57:57  frank
// Added a vessel identifier
//
// Revision 1.10  2003/05/16 13:10:14  frank
// code formatting
//
// Revision 1.9  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.8  2003/05/06 17:48:43  geconomos
// Coding standards
//
// Revision 1.7  2003/04/10 01:24:12  frank
// Added OverlayStrings
//
// Revision 1.6  2003/03/05 13:51:54  frank
// Added CineSeries element
//
// Revision 1.5  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.4  2002/10/04 18:11:38  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.3  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.2  2002/09/25 22:37:46  wenli
// Add BoxOfInterest
//
// Revision 1.1  2002/09/24 18:59:31  geconomos
// Initial revision.
//
// Revision 1.2  2002/09/24 16:08:22  geconomos
// Exception handling and formatting.
//
// Revision 1.1  2002/09/16 20:18:32  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ID.h,v 1.25.2.3 2009/07/29 16:43:29 kchalupa Exp $
// $Id: v3D_ID.h,v 1.25.2.3 2009/07/29 16:43:29 kchalupa Exp $