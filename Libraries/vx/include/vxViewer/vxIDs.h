// $Id: vxIDs.h,v 1.25.2.1 2008/11/24 17:13:00 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)

/**
 *  Define VX ID
 */


#ifndef vxIDs_h
#define vxIDs_h


// type definitions
typedef std::string vxID;


// class definition
struct VX_VIEWER_DLL vxIDs
{
  /// distance from boundary (temporary)
  static const vxID DistanceFromBoundary;

  static const vxID CropBox;
  static const vxID BoxOfInterest;
  static const vxID PhaseNumber;
  static const vxID NumPhases;
  static const vxID WindowHandle;
  static const vxID ViewObliqueData;
  static const vxID RayProfileData;
  static const vxID CineSeries;
  static const vxID OverlayStrings;
  static const vxID Path;
  static const vxID PathIndexPosition;
  static const vxID ProgressiveRefinement;
  static const vxID DatasetIndex;
  static const vxID DatasetIndexCycling;
  static const vxID EnvironmentList;
  static const vxID Ribbon;
  static const vxID VesselArray;
  static const vxID Vessel;
  static const vxID IsVesselnessLoaded;
  static const vxID DetailVolume;
  static const vxID CurrentVesselSection;
  static const vxID VesselPreferences;
  static const vxID Seedpoints;
  static const vxID IncrementalGrowth;
  static const vxID Mesh;
  static const vxID PolarGraph;
  static const vxID ColorBar;

  static const vxID VesselDetailEndoluminalColormap;
  static const vxID EndoluminalVisualizationPresetIndex;
  static const vxID VesselDetailColormap2D;
  static const vxID VesselDetailColormap3D;

  static const vxID DataAnnotationCreate;
  static const vxID DataColormapEditor;
  static const vxID DataPlaneObliqueCreate;

  static const vxID RendererTFE;            
  static const vxID RendererCropBox;
  static const vxID RendererColormap;        
  static const vxID RendererColorChooser;    
  static const vxID RendererAlphaFullRange;  
  static const vxID RendererAlphaActiveRange;

  static const vxID ManipulatorTFE;         
  static const vxID ManipulatorCropBox;     
  static const vxID ManipulatorColormap;    
  static const vxID ManipulatorColorChooser;
  static const vxID ManipulatorAlphaFullRange;
  static const vxID ManipulatorAlphaActiveRange;

	static const vxID Graph;
	static const vxID RendererGraph;
  static const vxID CycleSettings;
  static const vxID PhaseInfo;
  static const vxID SyncPlane;

  static const vxID TitleString;

  static const vxID SyncData;
  static const vxID FullScreen;
}; // class vxIDs


#endif // vxIDs_h


// $Log: vxIDs.h,v $
// Revision 1.25.2.1  2008/11/24 17:13:00  gdavidson
// Issue #6171: Added fullscreen double click
//
// Revision 1.25  2008/05/02 04:58:54  cqian
// add SyncData ID
//
// Revision 1.24  2008/03/20 16:50:44  cqian
// add titlestring
//
// Revision 1.23  2007/10/01 14:29:27  gdavidson
// Added SyncPlane
//
// Revision 1.22  2007/09/19 14:32:07  gdavidson
// Added PhaseInfo id
//
// Revision 1.21  2007/07/14 01:23:12  frank
// Added mesh renderer
//
// Revision 1.20  2007/05/03 21:33:04  gdavidson
// Added CycleSettings
//
// Revision 1.19  2006/12/21 15:04:11  gdavidson
// Issue #5189 & 5106: Added EndoluminalVisualizationPresetIndex, VesselDetailColormap2D and VesselDetailColormap3D
//
// Revision 1.18  2006/11/02 04:41:44  romy
// added GraphRenderer
//
// Revision 1.17  2006/11/02 03:29:30  romy
// addeded Graph ID
//
// Revision 1.16  2006/10/05 17:00:39  geconomos
// removed slice shadow color
//
// Revision 1.15  2006/09/25 20:07:50  geconomos
// removed DetailCropBox DetailCamera and EndoluminalCamera element ids
//
// Revision 1.14  2006/09/25 13:19:41  geconomos
// added ids from vascular
//
// Revision 1.13  2006/09/14 20:07:02  geconomos
// moved some ids from vascular project
//
// Revision 1.12  2006/09/01 13:24:48  frank
// moved seedpoints to vxViewer
//
// Revision 1.11  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.10  2006/03/27 19:03:26  gdavidson
// Added EnvironmentList and SliceShadowColor
//
// Revision 1.9  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.8.2.1  2005/06/22 17:25:07  frank
// added dataset index cycling property
//
// Revision 1.8  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.7  2004/10/25 23:50:22  michael
// refering to the vxViewer vxID
//
// Revision 1.6  2004/10/23 09:57:08  frank
// moved fusion IDs to the fusion plugin
//
// Revision 1.5  2004/10/04 14:58:51  frank
// added dataset controller to handle 4D data with multiple renderer data objs
//
// Revision 1.4  2004/04/06 18:18:47  frank
// renamed vessel to path
//
// Revision 1.3  2004/03/30 18:16:20  frank
// split out the progressive refinement flag from the renderer CPU 3d data so that it can be applied to all renderers in an environment at once
//
// Revision 1.2  2004/03/26 15:51:31  frank
// added fusion element stack
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/01/16 18:38:16  frank
// added renderer 3d cpu data extended element
//
// Revision 1.1  2003/12/18 14:10:55  frank
// Renamed to match class name
//
// Revision 1.21  2003/08/28 16:52:03  frank
// Added path index position
//
// Revision 1.20  2003/08/13 17:59:42  michael
// added ID for PlaneObliqueCreateData
//
// Revision 1.19  2003/08/12 15:46:56  frank
// Added a vessel identifier
//
// Revision 1.18  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.17  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.16  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.15  2003/04/10 01:23:56  frank
// Added OverlayStrings
//
// Revision 1.14  2003/03/05 13:51:54  frank
// Added CineSeries element
//
// Revision 1.13  2003/01/03 12:54:25  frank
// Added ray profile data used for sampling along a ray for 3D picking.
//
// Revision 1.12  2002/11/13 22:08:45  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.11  2002/11/13 20:12:08  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.10  2002/11/12 21:29:22  geconomos
// Made vxContour  a first class citizen of vxEnvironment ( frank really did it ).
//
// Revision 1.9  2002/11/06 18:28:23  frank
// Added contour
//
// Revision 1.8  2002/10/04 18:10:49  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.7  2002/10/04 16:34:19  michael
// colormap renderer/manipulator ID fix
//
// Revision 1.6  2002/10/04 14:38:13  michael
// renaming ...
//
// Revision 1.5  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.4  2002/09/25 22:09:10  wenli
// Add BoxOfInterest
//
// Revision 1.3  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.2  2002/09/19 14:25:24  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.1  2002/09/16 19:57:05  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxIDs.h,v 1.25.2.1 2008/11/24 17:13:00 gdavidson Exp $
// $Id: vxIDs.h,v 1.25.2.1 2008/11/24 17:13:00 gdavidson Exp $
