// $Id: v3D_Dataset.h,v 1.16.2.3 2009/07/29 13:23:40 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos(mailto:george@viatronix.com)


// pragmas
#pragma once

// forward declarations
class vxDataset;

// namespaces
OPEN_VISUALIZATION_NS


// forward managed declarations
ref class Annotations;
ref class Components;
ref class Colormap;
ref class RenderingMode;
ref class Segmentation;
ref class SegmentationPresetCollection;
ref class Camera;
ref class Volume;
ref class CropBox;
ref class Triplet;
ref class Cursor3d;
ref class Preset;
ref class ElementFloat4;
ref class DataRendererSoftware;
ref class VisualizationPreset;
ref class CadFindings;
ref class PhaseInfo;


/**
 * Represents a dataset
 */
public ref class Dataset
{
// construction
public:
  
  /// constructor
  Dataset();

  /// initializer from volume
  Dataset( Viatronix::Visualization::Volume ^ mpVolume );

  /// initializer from filename
  Dataset( System::String ^ mpFilename );
  
  /// destructor
  virtual ~Dataset();

  /// finalizer
  !Dataset();

// member functions
public:

  /// gets a pointer to the extended element
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pDataset ); }

  /// loads the dataset information from the specified path
  void Load( System::String ^ mpPath );

	///Adds the dataset 
	static void AddDataset( Dataset ^ mpDataset );

  /// gets the dataset at the specified index
  static Viatronix::Visualization::Dataset ^ GetDataset( int4 iIndex ) 
  {
    return m_mpDatasets[ iIndex ];
  } // GetDataset( int4 iIndex ) 

private:

  /// common initialization routine
  void CommonInit();

  /// initilizes the dataset from the currently loaded volume
  void InitializeFromVolume();

// properties
public:

  /// gets or sets  intensity volume
  property Viatronix::Visualization::Volume ^ IntensityVolume
  {
    Viatronix::Visualization::Volume ^ get() { return m_mpIntensityVolume; }
    void set( Viatronix::Visualization::Volume ^ mpIntensityVolume ); 
  } // IntensityVolume

  /// gets label volume
  property Viatronix::Visualization::Volume ^ LabelVolume
  {
    Viatronix::Visualization::Volume ^ get() { return m_mpLabelVolume; }
    void set( Viatronix::Visualization::Volume ^ mpLabelVolume ); 
  } // LabelVolume

  /// gets annotations
  property Viatronix::Visualization::Annotations ^ Annotations
  {
    Viatronix::Visualization::Annotations ^ get() { return m_mpAnnotations; }
    void set( Viatronix::Visualization::Annotations ^ mpAnnotations ) { m_mpAnnotations = mpAnnotations; }
  } // Annotations

  /// gets components
  property Viatronix::Visualization::Components ^ Components
  {
    Viatronix::Visualization::Components ^ get() { return m_mpComponents; }
    void set( Viatronix::Visualization::Components ^ mpComponents );// { m_mpComponents = mpComponents; }
  } // Components

  /// gets segmentation
  property Viatronix::Visualization::Colormap ^ Colormap
  {
    Viatronix::Visualization::Colormap ^ get() { return m_mpColormap; }
    void set( Viatronix::Visualization::Colormap ^ mpColormap );
  } // Colormap

  /// gets segmentation
  property Viatronix::Visualization::RenderingMode ^ RenderingMode
  {
    Viatronix::Visualization::RenderingMode ^ get() { return m_mpRenderingMode; }
    void set( Viatronix::Visualization::RenderingMode ^ mpRenderingMode );
  } // RenderingMode

  /// gets segmentation
  property Viatronix::Visualization::Segmentation ^ Segmentation
  {
    Viatronix::Visualization::Segmentation ^ get() { return m_mpSegmentation; }
    void set( Viatronix::Visualization::Segmentation ^ mpSegmentation ) { m_mpSegmentation = mpSegmentation; }
  } // Segmentation

  /// gets crop box
  property Viatronix::Visualization::CropBox ^ CropBox
  {
    Viatronix::Visualization::CropBox ^ get() { return m_mpCropBox; }
    void set( Viatronix::Visualization::CropBox ^ mpCropBox ) { m_mpCropBox = mpCropBox; }
  } // CropBox
  
  /// gets 3d cursor
  property Viatronix::Visualization::Cursor3d ^ Cursor3d
  {
    Viatronix::Visualization::Cursor3d ^ get() { return m_mpCursor3d; }
    void set( Viatronix::Visualization::Cursor3d ^ mpCursor3d ) { m_mpCursor3d = mpCursor3d; }
  } // Cursor3d

  /// gets segmentation preset
  property Viatronix::Visualization::SegmentationPresetCollection ^ SegmentationPresets
  {
    Viatronix::Visualization::SegmentationPresetCollection ^ get() { return m_mpSegmentationPresets; }
    void set( Viatronix::Visualization::SegmentationPresetCollection ^ mpPresets ) { m_mpSegmentationPresets = mpPresets; }
  } // SegmentationPresets

  /// gets data renderer
  property Viatronix::Visualization::DataRendererSoftware ^ DataRendererSoftware
  {
    Viatronix::Visualization::DataRendererSoftware ^ get() { return m_mpDataRendererSoftware; }
    void set( Viatronix::Visualization::DataRendererSoftware ^ mpDataRendererSoftware ) { m_mpDataRendererSoftware = mpDataRendererSoftware; }
  } // DataRendererSoftware

  /// gets the extended elements hastable
  property System::Collections::Hashtable ^ ExtendedElements
  {
    System::Collections::Hashtable ^ get() { return m_mpElements; }
  } // ExtendedElements

  /// gets the base file name of the dataset( intensity volfile minus the extension )
  property System::String ^ BaseFilename { System::String ^ get(); }
  
  /// gets the directory for dataset
  property System::String ^ Directory { System::String ^ get(); }

  /// gets the allow dataset reproduction property
  property bool AllowReproduction 
  { 
    bool get() { return m_bAllowReproduction; }
    void set( bool bAllowReproduction ) { m_bAllowReproduction = bAllowReproduction; }
  } // AllowReproduction

  /// gets preset
  property Viatronix::Visualization::Preset ^ Preset
  {
    Viatronix::Visualization::Preset ^ get() { return m_mpPreset; }
    void set( Viatronix::Visualization::Preset ^ preset ) { m_mpPreset = preset; }
  } // Preset

  /// gets the 2d visualization preset
  property Viatronix::Visualization::VisualizationPreset ^ VisualizationPreset2d
  {
    Viatronix::Visualization::VisualizationPreset ^ get() { return m_mpVisualizationPreset2d; }
    void set( Viatronix::Visualization::VisualizationPreset ^ mpPreset ) { m_mpVisualizationPreset2d = mpPreset; }
  } // VisualizationPreset2d
  
  /// gets the 3d visualization preset
  property Viatronix::Visualization::VisualizationPreset ^ VisualizationPreset3d
  {
    Viatronix::Visualization::VisualizationPreset ^ get() { return m_mpVisualizationPreset3d; }
    void set( Viatronix::Visualization::VisualizationPreset ^ mpPreset ) { m_mpVisualizationPreset3d = mpPreset; }
  } // VisualizationPreset3d

  /// gets the 3d visualization preset camera
  property Viatronix::Visualization::Camera ^ VisualizationPresetCamera3d
  {
    Viatronix::Visualization::Camera ^ get() { return m_mpVisualizationPresetCamera3d; }
    void set( Viatronix::Visualization::Camera ^ mpCamera ) { m_mpVisualizationPresetCamera3d = mpCamera; }
  } // VisualizationPresetCamera3d

  /// gets the 3d visualization preset camera
  property Viatronix::Visualization::CadFindings ^ CadFindings
  {
    Viatronix::Visualization::CadFindings ^ get() { return m_mpCadFindings; }
  } // CadFindings


  /// gets or sets the phase info
  property Viatronix::Visualization::PhaseInfo ^ PhaseInfo
  {
    Viatronix::Visualization::PhaseInfo ^ get() { return m_mpPhaseInfo; }
    void set( Viatronix::Visualization::PhaseInfo ^ mpPhaseInfo );
  } // PhaseInfo

  /// saves the dataset information to the specified path
  void Save( System::String ^ mpPath );

// member variables
private:

  /// intensity volume
  Viatronix::Visualization::Volume ^ m_mpIntensityVolume;
  
  /// label volume
  Viatronix::Visualization::Volume ^ m_mpLabelVolume;
  
  /// annotations
  Viatronix::Visualization::Annotations ^ m_mpAnnotations;
  
  /// rendering mode for 2D views
  Viatronix::Visualization::RenderingMode ^ m_mpRenderingMode2D;

  /// rendering mode for 3D views
  Viatronix::Visualization::RenderingMode ^ m_mpRenderingMode3D;

  /// segmentation
  Viatronix::Visualization::Segmentation ^ m_mpSegmentation;
  
  /// components
  Viatronix::Visualization::Components ^ m_mpComponents;

  /// colormap
  Viatronix::Visualization::Colormap ^ m_mpColormap;

  /// rendering mode
  Viatronix::Visualization::RenderingMode ^ m_mpRenderingMode;
  
  /// colormap 2d
  Viatronix::Visualization::Colormap ^ m_mpColormap2d;

  /// colormap 3d
  Viatronix::Visualization::Colormap ^ m_mpColormap3d;
  
  /// crop box
  Viatronix::Visualization::CropBox ^ m_mpCropBox;

  /// 3d cursor
  Viatronix::Visualization::Cursor3d ^ m_mpCursor3d;

  /// preset
  Viatronix::Visualization::Preset ^ m_mpPreset;

  /// extended elements
  System::Collections::Hashtable ^ m_mpElements;

  /// segmentation presets
  Viatronix::Visualization::SegmentationPresetCollection ^ m_mpSegmentationPresets;

  /// renderer 3D data
  Viatronix::Visualization::DataRendererSoftware ^ m_mpDataRendererSoftware;

  /// 2d visualization preset
  Viatronix::Visualization::VisualizationPreset ^ m_mpVisualizationPreset2d;

  /// 3d visualization preset
  Viatronix::Visualization::VisualizationPreset ^ m_mpVisualizationPreset3d;

  /// 3d visualization preset camera
  Viatronix::Visualization::Camera ^ m_mpVisualizationPresetCamera3d;

  /// phase info
  Viatronix::Visualization::PhaseInfo ^ m_mpPhaseInfo;

  /// cad findings
  Viatronix::Visualization::CadFindings ^ m_mpCadFindings;

  /// if it is permitted to create new datasets (eg, create from visible)
  bool m_bAllowReproduction;

	///vxDataset pointer
	vxDataset * m_pDataset;

  /// colletion of loaded datasets
  static System::Collections::Generic::List< Dataset ^ > ^ m_mpDatasets = gcnew System::Collections::Generic::List< Dataset ^ >();

}; // class Dataset


CLOSE_VISUALIZATION_NS


// $Log: v3D_Dataset.h,v $
// Revision 1.16.2.3  2009/07/29 13:23:40  kchalupa
// Code Walkthrough
//
// Revision 1.16.2.2  2008/11/17 16:05:39  kchalupa
// Undo previous checkin
//
// Revision 1.16.2.1  2008/11/17 15:22:55  kchalupa
// Fix for crash when only one dataset is present.
//
// Revision 1.16  2007/09/19 14:36:46  gdavidson
// Added PhaseInfo property
//
// Revision 1.15  2007/05/03 21:28:16  gdavidson
// Added GetDataset method
//
// Revision 1.14  2007/04/06 14:11:19  gdavidson
// Added CadFindings
//
// Revision 1.13  2007/04/02 21:42:06  gdavidson
// Added ColorMap and RenderingMode
//
// Revision 1.12  2007/03/30 17:43:18  romy
// Phase percentage property added
//
// Revision 1.11  2006/11/08 22:57:24  romy
// vxDatasetInfo is now receiving datasets
//
// Revision 1.10  2006/10/24 12:34:14  geconomos
// removed old slice shadows mechanism
//
// Revision 1.9  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.8  2006/08/31 14:02:46  geconomos
// Added static property to get/set the number of datasets currently loaded (hopefully this is temporary)
//
// Revision 1.7  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.6  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5  2005/09/26 13:48:33  gdavidson
// Added a save method and modified the load method.
//
// Revision 1.4.2.3  2005/07/07 13:04:02  frank
// Implemented a segmentation preset collection; moved CT presets to XML
//
// Revision 1.4.2.2  2005/07/05 17:52:51  frank
// spelling
//
// Revision 1.4.2.1  2005/06/23 19:02:32  frank
// removed dead code
//
// Revision 1.4  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.2.1  2005/02/17 20:58:57  geconomos
// added visualization preset mechansim
//
// Revision 1.3  2004/08/30 18:19:38  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.2  2004/04/07 20:32:24  geconomos
// Moved dataset initialization code from MainForm.cs to here.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/02/02 21:11:11  frank
// Issue #3485: Preserved the last 3D camera for future camera initialization
//
// Revision 1.34  2004/01/16 18:38:58  frank
// added renderer 3d cpu data extended element
//
// Revision 1.33  2003/11/19 14:16:16  frank
// Added 2D and 3D rendering modes
//
// Revision 1.32  2003/11/10 21:19:51  frank
// Removed excess slab thickness
//
// Revision 1.31  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.30  2003/10/25 00:39:24  frank
// allowed and disallowed dataset reproduction
//
// Revision 1.29  2003/09/05 14:59:03  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.28  2003/08/28 16:51:49  frank
// Added path index position
//
// Revision 1.27  2003/08/22 03:42:12  geconomos
// Added a property for the directoy name that the datasets resides in.
//
// Revision 1.26  2003/08/20 14:33:47  michael
// added plane oblique create data
//
// Revision 1.25  2003/08/12 17:00:41  frank
// Added a vessel 
//
// Revision 1.24.2.1  2003/08/17 21:50:06  geconomos
// Made SnapshotCollection and extended element.
//
// Revision 1.24  2003/07/17 17:34:31  geconomos
// Implemented SnapshotCollection class.
//
// Revision 1.23  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.22  2003/05/12 19:47:08  frank
// Added slice shadows object
//
// Revision 1.21  2003/05/12 14:11:01  geconomos
// spelling mistake.
//
// Revision 1.20  2003/05/06 15:12:18  geconomos
// Coding standards.
//
// Revision 1.19  2003/04/24 14:03:45  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.18  2003/03/30 21:04:43  michael
// extended presets to have 2D and 3D presets
//
// Revision 1.17  2003/03/26 16:59:04  sgagliardo
// rewrote AddDatasetToStudy to remove logic that created filename convention, now passed in as fileNameRoot.
//
// Revision 1.16  2003/03/04 15:34:37  geconomos
// Code cleanup.
//
// Revision 1.15  2003/03/03 16:51:05  geconomos
// Some code cleanup.
//
// Revision 1.14  2002/11/27 14:32:23  geconomos
// Now preserving slab thickness per dataset.
//
// Revision 1.13  2002/11/25 13:53:53  geconomos
// Added segmentation presets  property.
//
// Revision 1.12  2002/11/22 00:28:04  geconomos
// Added BaseFilename property
//
// Revision 1.11  2002/11/20 13:47:27  geconomos
// Added Snapshots property.
//
// Revision 1.10  2002/11/19 20:39:29  geconomos
// Added constructor from Volume.
//
// Revision 1.9  2002/11/15 18:04:23  geconomos
// Added Preset field and property.
//
// Revision 1.8  2002/11/13 22:01:20  geconomos
// added dataset ID field and property
//
// Revision 1.7  2002/11/11 20:00:00  geconomos
// Added Colormap2d and Colormap3d to dataset class.
//
// Revision 1.6  2002/11/06 15:00:15  geconomos
// Added set property of Colormap item.
//
// Revision 1.5  2002/11/05 22:44:26  ingmar
// added Cursor3d
//
// Revision 1.4  2002/11/02 03:12:01  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.3  2002/10/31 17:34:51  geconomos
// Headers, footers and comments.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Dataset.h,v 1.16.2.3 2009/07/29 13:23:40 kchalupa Exp $
// $Id: v3D_Dataset.h,v 1.16.2.3 2009/07/29 13:23:40 kchalupa Exp $
