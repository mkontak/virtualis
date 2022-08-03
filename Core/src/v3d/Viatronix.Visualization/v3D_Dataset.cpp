// $Id: v3D_Dataset.cpp,v 1.30.2.1 2009/07/29 13:23:40 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos(mailto:george@viatronix.com)


// includes
#include "Stdafx.h"
#include "v3D_Dataset.h"
#include "v3D_Annotations.h"
#include "v3D_Camera.h"
#include "v3D_Colormap.h"
#include "v3D_Components.h"
#include "v3D_Enums.h"
#include "v3D_RenderingMode.h"
#include "v3D_Segmentation.h"
#include "v3D_SegmentationPreset.h"
#include "v3D_SegmentationPresetCollection.h"
#include "v3D_Volume.h"
#include "v3D_CropBox.h"
#include "v3D_Cursor3d.h"
#include "v3D_Events.h"
#include "v3D_SimpleTypes.h"
#include "v3d_Vessel.h"
#include "v3D_Elements.h"
#include "v3D_ID.h"
#include "v3D_VisualizationPreset.h"
#include "v3D_CadFindings.h"

#include "vxDatasetsInfo.h"
#include "vxDataset.h"


// defines
#define FILE_REVISION "$Revision: 1.30.2.1 $"

#ifdef CreateDirectory
#undef CreateDirectory
#endif


// namespaces
using namespace System::Xml;
USING_VISUALIZATION_NS


/**
 * Default constructor
 */
Dataset::Dataset()
{
  CommonInit();
  InitializeFromVolume();
} // Dataset()


/**
 * Constructor from volume.
 *
 * @param volume 
 */
Dataset::Dataset( Viatronix::Visualization::Volume ^ mpVolume )
{
  CommonInit();

  this->IntensityVolume = mpVolume;
  this->LabelVolume = gcnew Volume( VolumeType::Intensity, mpVolume->Dimensions );
  this->LabelVolume->Units = mpVolume->Units;
	
  InitializeFromVolume(); 
} // Dataset( mpVolume )


/**
 * Constructor from filename.
 *
 * @param filename volume file to load
 */
Dataset::Dataset( System::String ^ mpFilename )
{
  CommonInit();

  // read int the instensity volume
  m_mpIntensityVolume->Read( mpFilename );

  InitializeFromVolume(); 
} // Dataset( mpFilename )


/**
 * destructor
 */
Dataset::~Dataset()
{
  this->!Dataset();
} // ~Dataset()


/**
 * Finalizer
 */
Dataset::!Dataset()
{
  delete m_pDataset;
	m_pDataset = NULL;
} // !Dataset()


/**
 * saves the dataset information to the specified path
 *
 * @param   mpPath    Path to the dataset directory
 */
void Dataset::Save( System::String ^ mpPath )
{
  // save the "label volume"
  this->LabelVolume->Write( System::IO::Path::Combine( mpPath, L"label.vol" ));

  // write out the "components" to xml
  System::IO::StreamWriter ^ mpWriter( System::IO::File::CreateText( System::IO::Path::Combine( mpPath, L"components.xml" )));
	
  //Compact the components before write.
  this->Components->Compact();
  mpWriter->Write( this->Components->ToXml() );
  mpWriter->Close();

  // write out the "annotations" to xml
  mpWriter = System::IO::File::CreateText( System::IO::Path::Combine( mpPath, L"annotations.xml" ));
  mpWriter->Write( this->Annotations->ToXml() );
  mpWriter->Close();

  // write out the "path" to xml
  mpWriter = System::IO::File::CreateText( System::IO::Path::Combine( mpPath, L"path.xml" ));
  Vessel ^ mpVessel( safe_cast< Vessel ^ >( this->ExtendedElements[ ElementID::Path ]));
  mpWriter->Write( mpVessel->ToXml() );
  mpWriter->Close();

} // Save( mpPath )


/**
 * loads the dataset information from the specified path
 *
 * @param   mpPath    Path to the dataset directory
 */
void Dataset::Load( System::String ^ mpPath )
{
  // read "label volume"
  Events::RaiseProgressMessage( "Reading Segmentation" );
  this->LabelVolume->Read( System::IO::Path::Combine( mpPath, L"label.vol" ), 0.0F, 85.0F );

  XmlDocument doc;
  doc.Load( System::IO::Path::Combine( mpPath, L"components.xml" ));

  // read in the components
  this->Components = Viatronix::Visualization::Components::FromXml( doc.DocumentElement->OuterXml );        

  // associate the label volume with the components
  this->Components->LabelVolume = this->LabelVolume; 

  this->Components->Initialize();

  this->Segmentation->IntensityVolume = this->IntensityVolume;
  this->Segmentation->LabelVolume = this->LabelVolume;
  this->Segmentation->Components = this->Components;

  Events::RaiseProgressMessage( "Reading Annotations" );
  Events::RaiseProgressPercentage( 90.0F );

  // read in the annotations
  doc.Load( System::IO::Path::Combine( mpPath, L"annotations.xml" ));
  this->Annotations = Viatronix::Visualization::Annotations::FromXml( doc.DocumentElement->OuterXml );

  Events::RaiseProgressMessage( "Reading Path" );
  Events::RaiseProgressPercentage( 95.0F );

  // read in the path
  System::String ^ mpPathFile( System::IO::Path::Combine(  mpPath, L"path.xml" ));
  if( System::IO::File::Exists( mpPathFile ))
  {
    doc.Load( mpPathFile );
    this->ExtendedElements[ ElementID::Path ] = Vessel::FromXml( doc.DocumentElement->OuterXml );
  }
 
  Events::RaiseProgressPercentage( 100.0F );
  Events::RaiseProgressMessage( "" );
} // Load( mpPath )


/**
 * Common initialization routine.
 */
void Dataset::CommonInit()
{
  m_bAllowReproduction = true;

	m_pDataset = new vxDataset();

  this->IntensityVolume           = gcnew Volume( VolumeType::Intensity );
  this->LabelVolume               = gcnew Volume( VolumeType::Label );
  m_mpElements                    = gcnew System::Collections::Hashtable();
  m_mpSegmentationPresets         = SegmentationPresetCollection::GetRanges(m_mpIntensityVolume);
  m_mpAnnotations                 = gcnew Viatronix::Visualization::Annotations();
  m_mpRenderingMode2D             = gcnew Viatronix::Visualization::RenderingMode();
  m_mpRenderingMode3D             = gcnew Viatronix::Visualization::RenderingMode();
  m_mpSegmentation                = gcnew Viatronix::Visualization::Segmentation();
	this->Components								= gcnew Viatronix::Visualization::Components( );
  this->Colormap                  = gcnew Viatronix::Visualization::Colormap();
  this->RenderingMode             = gcnew Viatronix::Visualization::RenderingMode();
  m_mpColormap2d                  = gcnew Viatronix::Visualization::Colormap();
  m_mpColormap3d                  = gcnew Viatronix::Visualization::Colormap();
  m_mpCropBox                     = gcnew Viatronix::Visualization::CropBox();
  m_mpCursor3d                    = gcnew Viatronix::Visualization::Cursor3d();
  m_mpDataRendererSoftware        = gcnew Viatronix::Visualization::DataRendererSoftware();
  m_mpVisualizationPreset2d       = gcnew Viatronix::Visualization::VisualizationPreset();
  m_mpVisualizationPreset3d       = gcnew Viatronix::Visualization::VisualizationPreset();
  m_mpVisualizationPresetCamera3d = gcnew Viatronix::Visualization::Camera();
  m_mpCadFindings                 = gcnew Viatronix::Visualization::CadFindings();
  m_mpPhaseInfo                   = gcnew Viatronix::Visualization::PhaseInfo();

  //this->ExtendedElements->Add( L"Snapshots",  new Viatronix::Visualization::SnapshotCollection() );
  //this->ExtendedElements->Add( L"Movies",     new Viatronix::Visualization::MovieCollection() );
  this->ExtendedElements->Add( ElementID::PathIndexPosition,      gcnew Viatronix::Visualization::ElementFloat4() );
  this->ExtendedElements->Add( ElementID::Path,                   gcnew Viatronix::Visualization::Vessel() );
  this->ExtendedElements->Add( ElementID::DatasetIndex,           gcnew Viatronix::Visualization::ElementInt4( 0 ) );
  this->ExtendedElements->Add( ElementID::DatasetIndexCycling,    gcnew Viatronix::Visualization::ElementBool( false ) );

} // CommonInit()


/**
 * Initializes the dataset from the current intensity volume.
 */
void Dataset::InitializeFromVolume()
{
   // create a label volume with same dimensions as label volume
  m_mpLabelVolume->Create( m_mpIntensityVolume->Dimensions );
  m_mpLabelVolume->Units = m_mpIntensityVolume->Units;

  // initialize components
  m_mpComponents->LabelVolume = m_mpLabelVolume;
    
  // initialize crop box
  m_mpCropBox->Initialize( m_mpIntensityVolume );

  // initialize cursor3d
  m_mpCursor3d->Position = gcnew Triplet(( m_mpIntensityVolume->Dimensions->x/2) * m_mpIntensityVolume->Units->x,
                                         ( m_mpIntensityVolume->Dimensions->y/2) * m_mpIntensityVolume->Units->y,
                                         ( m_mpIntensityVolume->Dimensions->z/2) * m_mpIntensityVolume->Units->z );

  // initialize segmentation
  m_mpSegmentation->IntensityVolume = m_mpIntensityVolume;
  m_mpSegmentation->LabelVolume = m_mpLabelVolume;
  m_mpSegmentation->Components = m_mpComponents; 

  // select the proper segmentation defaults based on the modality of the volume
  this->SegmentationPresets = SegmentationPresetCollection::GetRanges(m_mpIntensityVolume);
} // InitializeFromVolume()


/**
 * Return the base filename.
 */
System::String ^ Dataset::BaseFilename::get()
{
  System::String ^ mpFilename = System::String::Empty;
  
  if ( this->IntensityVolume->Filename != System::String::Empty )
  {
    mpFilename = this->IntensityVolume->Filename->Substring( 0, this->IntensityVolume->Filename->Length - 4 );
  }
  
  return mpFilename;
} // get_BaseFilename()


/**
 * Gets the directoru for the dataset.
 *
 * @return  Directory containing dataset.
 */
System::String ^ Dataset::Directory::get()
{
  System::String ^ mpDirectory = System::String::Empty;

  if( this->IntensityVolume->Filename != System::String::Empty )
  {
    System::IO::FileInfo fileInfo( this->IntensityVolume->Filename );

    System::String ^ mpDirectory = System::String::Concat( fileInfo.DirectoryName, L"\\XP" );

    // if the directory doesn't exist create it
    if( !System::IO::Directory::Exists( mpDirectory ) )
    {
      System::IO::Directory::CreateDirectory( mpDirectory );
    }
  } // if( this->IntensityVolume->Filename != System::String::Empty )

  return mpDirectory;
} // get_Directory()


/*
 * Sets the Intensity volume
 */
void Dataset::IntensityVolume::set( Viatronix::Visualization::Volume ^ mpIntensityVolume )
{ 
	m_mpIntensityVolume = mpIntensityVolume; 
	m_pDataset->SetIntensityVolume( ToNativeVolumeUint2Ptr( m_mpIntensityVolume->GetElementPtr() ));
} // set_IntensityVolume( mpIntensityVolume )


/*
 * Sets the Label volume
 */
void Dataset::LabelVolume::set( Viatronix::Visualization::Volume ^ mpLabelVolume )
{
	m_mpLabelVolume = mpLabelVolume; 
	m_pDataset->SetLabelVolume ( ToNativeVolumeUint2Ptr( m_mpLabelVolume->GetElementPtr() ));
} // set_LabelVolume( mpLabelVolume )


/*
 * Sets the components array
 */
void Dataset::Components::set( Viatronix::Visualization::Components ^ mpComponents )
{
	m_mpComponents = mpComponents; 
	m_pDataset->SetComponents( ToNativeComponentsPtr( m_mpComponents->GetElementPtr() ) );
} // set_Components( mpComponents )
  

/*
 * Sets the colormap
 */
void Dataset::Colormap::set( Viatronix::Visualization::Colormap ^ mpColormap )
{ 
	m_mpColormap = mpColormap; 
  m_pDataset->SetColormap( ToNativeColormapPtr( m_mpColormap->GetElementPtr() ));
} // set_Colormap( mpColormap )


/*
 * Sets the rendering mode
 */
void Dataset::RenderingMode::set( Viatronix::Visualization::RenderingMode ^ mpRenderingMode )
{ 
	m_mpRenderingMode = mpRenderingMode; 
  m_pDataset->SetRenderingMode( ToNativeRenderingModePtr( m_mpRenderingMode->GetElementPtr() ));
} // set_RenderingMode( mpRenderingMode )


/**
 * Sets the phase info
 *
 * @param       mpPhaseInfo           phase information
 */
void Dataset::PhaseInfo::set( Viatronix::Visualization::PhaseInfo ^ mpPhaseInfo )
{
	m_mpPhaseInfo = mpPhaseInfo; 
  m_pDataset->SetPhaseInfo( reinterpret_cast< vxPhaseInfo * >( mpPhaseInfo->GetElementPtr().ToPointer() ));
} // set_PhaseInfo( mpPhaseInfo )


/**
 * Adds the dataset to the Dataset info
 */
void Dataset::AddDataset( Dataset ^ mpDataset )
{
  m_mpDatasets->Add( mpDataset );
	vxDatasetsInfo::AddDataset( mpDataset->m_pDataset );
} // AddDataset( Dataset ^ mpDataset )


// $Log: v3D_Dataset.cpp,v $
// Revision 1.30.2.1  2009/07/29 13:23:40  kchalupa
// Code Walkthrough
//
// Revision 1.30  2008/04/02 16:04:03  cqian
// check in
//
// Revision 1.29  2007/09/19 14:36:46  gdavidson
// Added PhaseInfo property
//
// Revision 1.28  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.27  2007/06/05 21:20:23  gdavidson
// Set the initial distance for an axial plane to the middle of the volume
//
// Revision 1.26  2007/05/03 21:28:16  gdavidson
// Added GetDataset method
//
// Revision 1.25  2007/05/03 14:03:36  geconomos
// removed progress message text from dataset load
//
// Revision 1.24  2007/04/06 14:11:19  gdavidson
// Added CadFindings
//
// Revision 1.23  2007/04/02 21:42:06  gdavidson
// Added ColorMap and RenderingMode
//
// Revision 1.22  2007/03/30 17:43:18  romy
// Phase percentage property added
//
// Revision 1.21.2.1  2007/04/30 21:15:02  romy
// calls the compact method before Writing the components in to XML. Issue# 5625
//
// Revision 1.21  2006/11/09 23:20:18  romy
// CommonInit modified to go through the Properties
//
// Revision 1.20  2006/11/08 22:57:24  romy
// vxDatasetInfo is now receiving datasets
//
// Revision 1.19  2006/10/24 12:34:14  geconomos
// removed old slice shadows mechanism
//
// Revision 1.18  2006/10/05 20:55:27  romy
// Corrected Adding an item in to the ElementID hash table
//
// Revision 1.17  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.16  2006/08/31 14:02:46  geconomos
// Added static property to get/set the number of datasets currently loaded (hopefully this is temporary)
//
// Revision 1.15  2006/03/27 19:48:02  gdavidson
// Added include for SliceShadow
//
// Revision 1.14  2006/02/17 15:17:40  frank
// rendering mode is no longer necessary for the segmentation object
//
// Revision 1.13  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.12  2005/11/23 00:59:11  frank
// re-enabled dataset cycling
//
// Revision 1.11  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.10  2005/11/08 21:43:45  gdavidson
// Added a check before loading the Path.xml file.
//
// Revision 1.9  2005/09/26 13:48:33  gdavidson
// Added a save method and modified the load method.
//
// Revision 1.8.2.4  2005/08/15 19:33:08  geconomos
// initialized the z-component of the 3d cursor to 0
//
// Revision 1.8.2.3  2005/07/07 13:03:49  frank
// Implemented a segmentation preset collection; moved CT presets to XML
//
// Revision 1.8.2.2  2005/06/23 19:02:32  frank
// removed dead code
//
// Revision 1.8.2.1  2005/06/22 17:25:43  frank
// added dataset index cycling property
//
// Revision 1.8  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.7  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.6.2.2  2005/02/23 17:38:09  frank
// initialized the dataset on the first slice
//
// Revision 1.6.2.1  2005/02/17 20:58:57  geconomos
// added visualization preset mechansim
//
// Revision 1.6  2004/10/04 15:08:25  frank
// added dataset controller to handle 4D data with multiple renderer data objs
//
// Revision 1.5  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.4  2004/04/07 21:08:28  geconomos
// Fixed compile error.
//
// Revision 1.3  2004/04/07 20:32:24  geconomos
// Moved dataset initialization code from MainForm.cs to here.
//
// Revision 1.2  2004/04/05 19:06:08  geconomos
// Added addition of snapshots and movies to dataset initialization.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.41  2004/02/02 21:11:11  frank
// Issue #3485: Preserved the last 3D camera for future camera initialization
//
// Revision 1.40  2004/01/16 18:38:58  frank
// added renderer 3d cpu data extended element
//
// Revision 1.39  2003/11/19 14:16:16  frank
// Added 2D and 3D rendering modes
//
// Revision 1.38  2003/11/10 21:19:50  frank
// Removed excess slab thickness
//
// Revision 1.37  2003/11/05 01:18:44  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.36  2003/10/25 00:39:24  frank
// allowed and disallowed dataset reproduction
//
// Revision 1.35  2003/09/05 14:59:03  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.34  2003/08/28 16:51:49  frank
// Added path index position
//
// Revision 1.33  2003/08/22 03:42:12  geconomos
// Added a property for the directoy name that the datasets resides in.
//
// Revision 1.32  2003/08/20 14:33:47  michael
// added plane oblique create data
//
// Revision 1.31  2003/08/12 17:00:41  frank
// Added a vessel
//
// Revision 1.30.2.1  2003/08/17 21:50:06  geconomos
// Made SnapshotCollection and extended element.
//
// Revision 1.30  2003/07/29 11:03:01  michael
// adjusted the segmentation preset to use a volume (volume header) to put
// together the ranges for the auto segments based on modality etc.
//
// Revision 1.29  2003/07/17 17:34:31  geconomos
// Implemented SnapshotCollection class.
//
// Revision 1.28  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.27  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.26  2003/05/12 19:47:08  frank
// Added slice shadows object
//
// Revision 1.25  2003/05/12 14:11:16  geconomos
// spelling mistake.
//
// Revision 1.24  2003/05/06 15:12:18  geconomos
// Coding standards.
//
// Revision 1.23  2003/04/24 14:03:45  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.22  2003/04/14 16:45:50  frank
// Separated progress percent and progress message interfaces
//
// Revision 1.21  2003/03/31 13:51:49  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.20  2003/03/30 21:04:43  michael
// extended presets to have 2D and 3D presets
//
// Revision 1.19  2003/03/21 16:48:34  geconomos
// Issue #2931: Crash occurs when segmented when no volume is loaded.
//
// Revision 1.18  2003/03/04 15:34:37  geconomos
// Code cleanup.
//
// Revision 1.17  2003/03/03 16:51:05  geconomos
// Some code cleanup.
//
// Revision 1.16  2002/11/27 14:32:23  geconomos
// Now preserving slab thickness per dataset.
//
// Revision 1.15  2002/11/25 23:21:13  geconomos
// Fixed default segmentation presets.
//
// Revision 1.14  2002/11/25 13:53:53  geconomos
// Added segmentation presets  property.
//
// Revision 1.13  2002/11/22 04:36:03  frank
// Label volume units were not set properly.
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
// Revision 1.6  2002/11/05 22:44:10  ingmar
// added Cursor3d
//
// Revision 1.5  2002/11/04 18:52:13  frank
// Enabling crop box
//
// Revision 1.4  2002/11/02 03:12:01  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.3  2002/10/31 17:34:51  geconomos
// Headers, footers and comments.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Dataset.cpp,v 1.30.2.1 2009/07/29 13:23:40 kchalupa Exp $
// $Id: v3D_Dataset.cpp,v 1.30.2.1 2009/07/29 13:23:40 kchalupa Exp $
