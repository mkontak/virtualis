// $Id: v3D_Volume.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Interfaces.h"
#include "v3D_Triplet.h"
#include "v3D_Enums.h"
#include "v3D_Components.h"
#include "v3D_VolumeHeader.h"
#include "ColorList.h"

// forward declarations
class vxManagedObserver;

// namespaces 
OPEN_VISUALIZATION_NS

// managed forward declarations
ref class CropBox;

// class definition
public ref class  Volume : public IObserver, public IElement
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pBlockVolume ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// processes modified events
  virtual void OnModified( System::IntPtr modifiedList );

// events
public:

  // raised when a volume is modified
  event System::EventHandler ^ Changed;

// member functions
public:
  
  /// initializer from data type
  Volume( VolumeType eType );

  /// initializer from data type and volfile
  Volume( VolumeType eType, System::String ^ mpFilename );

  Volume( VolumeType eType, System::String ^ mpFilename, bool bProgress );

  /// initializer from data type and dimensions
  Volume( VolumeType eType, Triplet ^ mpDimensions );

  /// initializer from data type and vxBlockVolume pointer
  Volume( VolumeType eType, System::IntPtr volume );
  
  /// destructor
  virtual ~Volume();

  /// finalizer
  !Volume();

  /// reads a volume from disk
  void Read( System::String ^ mpFilepath );
  
  /// reads a volume from disk and choose to update the progress meter
  void Read( System::String ^ mpFilepath, float4 fPercentBegin, float4 fPercentEnd );

  /// writes a volume to disk
  void Write( System::String ^ mpFilepath );

  /// creates a volume
  void Create( Triplet ^ mpDimensions );

  /// updates the required histograms
  void UpdateHistograms();

  /// gets the specified volume header variable
  System::String ^ GetHeaderVariable( HeaderVariables eVariable );
 
  /// gets the specified volume header variable
  System::String ^ GetHeaderDicomElementValue( System::String ^ mpsGroup, System::String ^ mpsField );

  /// creates a new volume by intersection with a crop box
  Volume ^ Crop( CropBox ^ mpCropBox, System::String ^ mpModification );

  /// creates a new isotropic volume by intersection with a crop box
  Volume ^ CropIsotropic( CropBox ^ mpCropBox, System::String ^ mpModification );

  /// create a new volume based on the visible components only
  Volume ^ GetVolumeFromVisible( Volume ^ mpLabelVolume, Components ^ mpComponents, System::String ^ mpModification );

  /// gets the specified volume header
  property VolumeHeader ^ Header { VolumeHeader ^ get() { return m_mpHeader; }}

  /// returns the volume dimensions
  property Triplet ^ Dimensions { Triplet ^ get(); }

  /// gets the world dimensions
  property Triplet ^ WorldDimensions { Triplet ^ get(); }
  
  /// gets or sets the volume units
  property Triplet ^ Units
  {
    Triplet ^ get();
    void set( Triplet ^ mpUnits );
  }
  
  /// gets if the volume is valid
  property bool Valid { bool get() { return m_bValid; }; }

  /// gets the patient age contained within the volume's header
  property int PatientAge { int get() { return m_pBlockVolume->GetHeader().GetPatientAge(); }}

  /// gets the volume's filename
  property System::String ^ Filename { System::String ^ get() { return gcnew System::String( m_pBlockVolume->GetHeader().GetFileName().c_str() ); }}

  /// gets the thread safety flag
  property static bool ThreadSafety
  {
    bool get(); 
    void set( bool bIsThreadSafe );
  }
  
  /// assigned color property
  property System::Drawing::Color AssignedColor { System::Drawing::Color get() { return m_color; }}

  /// gets a vxBlockVolume pointer
  System::IntPtr GetVolumePtr() { return System::IntPtr( m_pBlockVolume ); }

 /// gets a vxBlockVolume pointer
  System::IntPtr GetVolumePtr2() { return System::IntPtr( dynamic_cast< vxBlockVolume< uint2 > *>( m_pBlockVolume ) ); }

  /// gets a BaseHistogram pointer
  System::IntPtr GetHistogramPtr() { return System::IntPtr( &m_pBlockVolume->GetHistogram() ); }
 
private:

  /// common initialization routine
  void CommonInit( Viatronix::Visualization::VolumeType eType );

  /// update the modification string to include the given edit operation
  void UpdateModificationString( Volume ^ mpVolume, const std::string & sModification );

// member variables
private:

  /// volume header
  VolumeHeader ^ m_mpHeader;

  /// block volume
  vxShareableObject< vxBlockVolume< uint2 > > * m_pBlockVolume;

  /// flag to enable binary histogram creation
  bool m_bCreateBinaryHistogram;

  /// observer
  vxManagedObserver * m_pObserver;

  /// filename
  System::String ^ m_mpFilename;

  /// indicates if the volume is valid
  bool m_bValid;

  /// volume datatype
  Viatronix::Visualization::VolumeType m_eType;

  /// assigned color
  System::Drawing::Color m_color;

  /// color list
  static ColorList * m_colors = new ColorList();

  bool m_bProgress;

}; // class Volume


CLOSE_VISUALIZATION_NS


// Revision history:
// $Log: v3D_Volume.h,v $
// Revision 1.7  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.5  2006/04/03 16:48:08  geconomos
// + added  histogram updater as template argument
//
// Revision 1.4  2006/03/29 20:12:43  geconomos
// + removed vxBlockVolumeBinaryHistogram related code
// + added new file reading mechanism
//
// Revision 1.3  2004/10/04 15:11:55  frank
// improved the extended elements modified interface
//
// Revision 1.2  2004/03/08 22:18:27  geconomos
// Added ToVolumePtr2() whuch dynamically casts the type to a vxBlockVolume< uint2 >.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.36  2003/12/16 13:27:53  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.35  2003/09/09 19:16:34  dongqing
// Stopped caching the filename and retrieve from header instead - remove m_mpFilename when this is proven acceptable
//
// Revision 1.34  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.33  2003/08/08 19:50:09  wenli
// Add EventHandler Change();
//
// Revision 1.32  2003/07/30 13:16:32  geconomos
// Reworked threading of base histogram.
//
// Revision 1.31  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.30  2003/06/04 18:30:21  michael
// added VolumeHeader
//
// Revision 1.29  2003/06/03 20:27:58  michael
// typo
//
// Revision 1.28  2003/06/03 12:25:07  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.27  2003/05/20 14:46:57  frank
// Made the edit and crop labels be more specific on the GUI
//
// Revision 1.26  2003/05/16 13:13:57  frank
// formatting
//
// Revision 1.25  2003/05/14 13:08:25  frank
// code review
//
// Revision 1.24  2003/05/14 12:34:01  michael
// code review
//
// Revision 1.23  2003/05/13 19:49:30  frank
// Refactored code
//
// Revision 1.22  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.21  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.20  2003/04/14 19:42:42  frank
// Exposed progress metering to managed classes
//
// Revision 1.19  2003/04/07 17:12:05  frank
// spacing
//
// Revision 1.18  2003/04/01 16:01:43  geconomos
// Code cleanup and threaded base histogram creation.
//
// Revision 1.17  2003/03/14 14:01:38  geconomos
// Changed the return type for the BinaryHistogram to IntPtr
//
// Revision 1.16  2003/03/13 16:30:46  frank
// Added VisibleComponentExport
//
// Revision 1.15  2003/02/13 16:56:46  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.14  2003/02/06 20:07:23  frank
// Added new read function that doesn't automatically show the progress events
//
// Revision 1.13.2.1  2003/02/12 23:01:35  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.13  2002/11/19 20:39:15  geconomos
// Added creation of histogram nad binary historgam in the Crop and CropIsotropic functions.
//
// Revision 1.12  2002/11/15 18:05:06  geconomos
// Added assigned color.
//
// Revision 1.11  2002/11/08 14:04:15  geconomos
// Added WorldDimensions property.
//
// Revision 1.10  2002/11/06 19:35:42  frank
// Added cropping without resampling.
//
// Revision 1.9  2002/11/05 19:51:27  frank
// Implemented cropping
//
// Revision 1.8  2002/11/05 18:56:40  frank
// Added Crop function
//
// Revision 1.7  2002/10/31 12:48:51  geconomos
// Added property for volume filepath.
//
// Revision 1.6  2002/10/17 22:26:43  ingmar
// added ThreadSafety property
//
// Revision 1.5  2002/10/16 16:37:01  frank
// Added function to retrieve patient age when only birth date and study date are known.
//
// Revision 1.4  2002/10/07 15:20:34  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.13  2002/09/24 14:45:04  geconomos
// Exception handling and formatting.
//
// Revision 1.12  2002/08/26 19:31:17  geconomos
// Added dicom header support.
//
// Revision 1.11  2002/08/16 18:16:06  dmitry
// set units added
//
// Revision 1.10  2002/08/15 19:14:43  dmitry
// Units exposed in the volume.
//
// Revision 1.9  2002/08/14 19:28:17  dmitry
// Histogram creation disabling added.
//
// Revision 1.8  2002/08/09 16:20:08  dmitry
// Added Connect and Disconnect methods to IElement.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Volume.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_Volume.h,v 1.7 2006/10/04 18:29:06 gdavidson Exp $
