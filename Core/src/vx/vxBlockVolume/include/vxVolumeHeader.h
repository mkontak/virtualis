// $Id: vxVolumeHeader.h,v 1.14 2006/06/29 16:12:13 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

/*
  Common information for all volumes. 
  Can be read with out having to read the volume data.
  Has all info to determine what memory to allocate before the actual volume data is read.
*/


// pragmas
#pragma once


// includes
#include "vxBlock.h"
#include "Triple.h"
#include "Vector3D.h"
#include "Date.h"
#include "vxInflate.h"
#include "VarMap.h"
#include "PersonName.h"
#include "Date.h"
#include "vxVoxelCost.h"
#include "SerializedDicom.h"


// forward declarations
class vxComplex;
template <class VoxelType> class vxBlockVolume;


// class definition
class VX_BLOCK_VOLUME_DLL vxVolumeHeader
{
// enums
public:

  /// enumeration of possible volume file extensions
  enum ExtensionEnum
  {
    VOL                    =  0, /// generic
    ECV                    =  1, /// electronic cleansing
    INS                    =  2, /// inside detection
    SKE                    =  3, /// skeleton
    DFB                    =  4, /// distance from boundary
    DDB                    =  5, /// dilated distance from boundary
    SLD                    =  6, /// space leap distance
    DFS                    =  7, /// distance from source
    DFE                    =  8, /// distance from end
    PDE                    =  9, /// penalized distance from end
    SLC                    = 10, /// slice (VolVis volume file format)
    UNKNOWN_EXTENSION_ENUM = 11, /// ???
    NUM_OF_EXTENSION_ENUM  = 12  /// total number of listed file extensions
  };

#define EXTENSION_STRINGS { ".VOL", ".ECV", ".INS", ".SKE", ".DFB", ".DDB", ".SLD", ".DFS", ".DFE", ".PDE", ".SLC", ".UNKNOWN"};

  // enum for the unit type
  enum UnitTypeEnum
  {
    METER                 = 0,  
    MILLIMETER            = 1,
    MICRON                = 2,
    FOOT                  = 3,
    INCH                  = 4,
    UNKNOWN_UNIT_TYPE     = 5,
    NUM_OF_UNIT_TYPE_ENUM = 6
  };

#define UNIT_TYPE_STRINGS { "METER", "MILLIMETER", "MICRON", "FOOT", "INCH", "UNKNOWN_UNIT_TYPE" };

  // enum for memory layout
  enum MemoryLayoutEnum
  {
    LINEAR                    = 0,
    HASH                      = 1,
    BLOCK                     = 2,
    REC_BLOCK                 = 3,
    UNKNOWN_MEMORY_LAYOUT     = 4,
    NUM_OF_MEMORY_LAYOUT_ENUM = 5
  };

#define MEMORY_LAYOUT_STRINGS { "LINEAR", "HASH", "BLOCK", "RECURSIVE_BLOCKING", "UNKNOWN_MEMORY_LAYOUT" };

  // enum for voxel type
  enum VoxelTypeEnum
  {
    BOOL                   =  0,
    UINT1                  =  1,
    UINT2                  =  2,
    UINT4                  =  3,
    INT1                   =  4,
    INT2                   =  5,
    INT4                   =  6,
    FLOAT4                 =  7,
    FLOAT8                 =  8,
    COMPLEX                =  9,
	VXVOXELTYPE            = 10,
    UNKNOWN_VOXEL_TYPE     = 11,
    NUM_OF_VOXEL_TYPE_ENUM = 12
  };

#define VOXEL_TYPE_STRINGS { "BOOL", "UINT1", "UINT2", "UINT4", "INT1", "INT2", "INT4", "FLOAT4", "FLOAT8", "COMPLEX", "VXVOXELTYPE", "UNKNOWN_VOXEL_TYPE" };

  // enum for variable header entries
  enum VarVolHeaderEntryEnum
  {
    Current                =  0,
    Voltage                =  1,
    SliceThickness         =  2,
    Pitch                  =  3,
    Tilt                   =  4,
    FieldOfView            =  5,
    HeaderVersion          =  6,
    SiteID                 =  7,
    StudyDate              =  8,
    StudyTime              =  9,
    SeriesUID              = 10,
    StudyUID               = 11,
    PhysicianName          = 12,
    PatientID              = 13,
    InstitutionName        = 14,
    OperatorName           = 15,
    PatientName            = 16,
    PatientBirthdate       = 17,
    PatientAge             = 18,
    PatientAgeUnits        = 19,
    PatientWeight          = 20,
    PatientSex             = 21,
    PatientHistory         = 22,
    PatientPosition        = 23,
    StudyType              = 24,
    SliceOrder             = 25,
    Manufacturer           = 26,
    DicomHeader            = 27,
    DataModality           = 28,
    ReconstructionInterval = 29,
    Contrast               = 30,
    BodyPart               = 31,
    ExposureTime           = 32,
    TransducerFrequency    = 33,
    ScanningSequence       = 34,
    SequenceVariant        = 35,
    AcquisitionType        = 36,
    EchoTime               = 37,
    RepetitionTime         = 38,
    ModificationString     = 39,
    StudyDescription       = 40,
    HistoryEntries         = 41,
    BitStored              = 42,
    Scale                  = 43,
    Bias                   = 44,
    NUM_OF_VARVOLHEADER_ENTRY_ENUM  
  };
  
#define VARVOLHEADER_STRINGS { "mA", "kv", "sliceThickness", "pitch", "tilt", "FOV", \
                               "version", "siteID", "studyDate", "studyTime", "seriesUID", "studyUID", "physicianName", \
                               "patientID", "institutionName", "operatorName", "patientName", "patientBirthDate", \
                               "patientAge", "pateintAgeUnits", "patientWeight", "patientSex", "patientHistory", \
                               "patientPosition", "studyType", "zOrderReversed", "manufacturer", "dicomHeader", \
                               "DataModality", "ReconstructionInterval", "Contrast", "BodyPart", "ExposureTime", \
                               "TransducerFrequency", "ScanningSequence", "SequenceVariant", "AcquisitionTime", \
                               "EchoTime", "RepetitionTime", "ModificationString", "StudyDescription", "HistoryEntries", \
                               "BitStored", "Scale", "Bias" };

// functions
public:

  /// contructor
  vxVolumeHeader(const Triple<int4>   & dim            = Triple<int4>(0,0,0), 
                 const Triple<float4> & units          = Triple<float4>(1.0,1.0,1.0),
                 const int4 & iSignificantBitsPerVoxel = 8*sizeof(int2),
                 const int4 & background               = 0, 
                 UnitTypeEnum eUnitType                = MILLIMETER,
                 const std::string & sFileName         = "unspecified.vol",
                 const std::string & sTitle            = "unspecified",
                 const std::string & sAuthor           = "unspecified",
                 const vx::Date & date                 = vx::Date(),
                 ExtensionEnum eExtension              = VOL);

  /// copy contructor
  vxVolumeHeader(const vxVolumeHeader & other);

  /// assignment operator
  vxVolumeHeader & operator=(const vxVolumeHeader & other);

  /// invalidate cached values
  inline void ClearCachedValues();

  /// fast access to bias
  //inline bool GetCachedBias(std::string & sBias) const;

  /// fast access to stored bits
  //inline std::string GetCachedBitsStored(const std::string & sDefault) const;

  /// read header of volume from disk.
  void Read(const std::string & sFileName);

  /// returns the volume dimensions
  const Triple<float4> & GetWorldDim() const { return m_worldDimensions; }

  /// returns the volume dimensions
  const Triple<int4> & GetVolDim() const { return m_volumeDimensions; }

  /// sets the volume dimensions
  void SetVolDim(const Triple<int4> & dim);

  /// sets the volume dimensions
  void SetVolDim(int4 iDimensionX, int4 iDimensionY, int4 iDimensionZ);

  /// returns the voxel units
  const Triple<float4> & GetVoxelUnits() const { return m_units; }

  /// sets the voxel units
  void SetVoxelUnits(const Triple<float4> & units);

  /// returns the voxel's physical unit type enum
  const UnitTypeEnum & GetVoxelUnitTypeEnum() const { return m_eUnitType; }

  /// sets the voxel's physical unit type enum
  void SetVoxelUnitTypeEnum(UnitTypeEnum eVoxelUnitType) { m_eUnitType = eVoxelUnitType; }

  /// returns the voxel's physical unit type string
  const std::string & GetVoxelUnitTypeString() const;

  /// returns the significant bits per voxel
  int4 GetSignificantBitsPerVoxel() const { return m_iSignificantBitsPerVoxel; }

  /// sets significant bits per voxel
  void SetSignificantBitsPerVoxel(int4 iSignificantBitsPerVoxel) { m_iSignificantBitsPerVoxel = iSignificantBitsPerVoxel; }

  /// returns the voxel data type string
  const std::string & GetVoxelTypeString() const;

  /// returns the voxel data type string
  const VoxelTypeEnum & GetVoxelTypeEnum() const { return m_eVoxelType; }

  /// sets the voxel type enum
  void SetVoxelTypeEnum(VoxelTypeEnum eVoxelType) { m_eVoxelType = eVoxelType; }

  /// returns the background voxel value
  const int4 & GetBackground() const { return m_iBackground; }

  /// sets the background voxel value
  void SetBackground(int4 iBackground) { m_iBackground = iBackground; }

  /// returns the volume title
  const std::string & GetTitle() const { return m_sTitle; }

  /// sets the volume title
  void SetTitle(const std::string & sTitle) { m_sTitle = sTitle; }

  /// returns the volume author/source
  const std::string & GetAuthor() const { return m_sAuthor; }

  /// sets the volume author/source
  void SetAuthor(const std::string & sAuthor) { m_sAuthor = sAuthor; }

  /// returns the volume file name
  const std::string & GetFileName() const { return m_sFileName; }

  /// sets the volume file name
  void SetFileName(const std::string & sFileName) { m_sFileName = sFileName; }

  /// returns the volume file name extension enum
  const ExtensionEnum & GetFileNameExtensionEnum() const { return m_eFileNameExtension; }

  /// returns the volume file name extension string
  const std::string & GetFileNameExtensionString() const;

  /// sets the volume file name extension
  void SetFileNameExtension(ExtensionEnum eFileNameExtension) { m_eFileNameExtension = eFileNameExtension; }

  /// returns the volume creation date
  const vx::Date & GetCreationDate() const { return m_creationDate; }

  /// sets the volume creation date
  void SetCreationDate(const vx::Date & date) { m_creationDate = date; }

  /// returns the enum of the best possible file compression
  const hdr::FileCompressionEnum & GetBestFileCompression() const { return m_eBestFileCompression; }

  /// returns the enum of the used possible file compression
  const hdr::FileCompressionEnum & GetUsedFileCompression() const { return m_eUsedFileCompression; }

  /// sets the enum of the used possible file compression
  void SetUsedFileCompression(const hdr::FileCompressionEnum & eUsedFileCompression) { m_eUsedFileCompression = eUsedFileCompression; }

  /// returns the variable VarMap volume header for volume semantics info for reading
  template <class Type> 
  void GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum eEntry, Type & value) const;

  /// try to retrieve the patient's age in years
  float4 GetPatientAge() const;

  /// gets the variable VarMap volume header
  const VarMap & GetVariableVolumeHeader() const { return m_varHeader; }

  /// sets the variable VarMap volume header
  inline void SetVariableVolumeHeader(const VarMap & varMap);

  /// Remove a variable from the VarMap volume header
  void RemoveVariableFromHeader( std::string & sVarName );

	/// returns variable volume header and dicom header as string
  std::string GetVarMapsAsString() const;

  /// return as string
  std::string GetAsString() const;

  /// returns the volume creation date
  const std::pair<int4,int4> & GetFileVersion() const { return m_fileVersionPair; }

  /// sets file version number
  void SetFileVersion(const std::pair<int4,int4> & fileVersionPair) { m_fileVersionPair = fileVersionPair; }

  /// sets file version number
  void SetFileVersion(int4 iMajorVersionNumber, int4 iMinorVersionNumber);

  /// return number of voxels in the volume.
  uint4 GetNumberOfVoxels() const;

  /// populate a serialized DICOM structure
  //void GetSerializedDicom( SerializedDicom & dicomInformation ) const;

  /// return a specific DICOM field entry
  std::string GetDicomFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sDefaultValue = "") const;

  /// reset a specific DICOM header entry
  //bool SetDicomFieldValue(const std::string & sGroup, const std::string & sField, const std::string & sNewValue);

  /// append history information
  void AppendHistory(const std::string & sHistory);

  /// returns the history information
  std::vector<std::string> GetHistory() const;
  
  /// returns the memory layout enum
  const MemoryLayoutEnum & GetMemoryLayoutEnum() const { return m_eMemoryLayout; }


private:
  /// returns the enum of the type that matches the given string
  VoxelTypeEnum GetVoxelTypeEnumFromString(const std::string & sVoxelTypeString) const;

  /// sets the volume file name extension
  void SetMemoryLayoutEnum(const MemoryLayoutEnum & eMemoryLayout) { m_eMemoryLayout = eMemoryLayout; }

  /// returns the memory layout string
  const std::string & GetMemoryLayoutString() const;


  /// Initializes the vectors containing the File, Data, Layout, Extension, and Unit types.
  void InitializeStringVectors();

/// data
private:

  // cached values
  //////////////////

  // cache
  mutable std::vector<std::pair<bool, std::string> > m_vCache;

/*
  // bits stored
  mutable std::string m_sCachedBitsStored;

  // bits stored valid?
  mutable bool m_bCachedBitsStoredValid;
*/

  // bias
  mutable std::string m_sCachedBias;

  // bias valid?
  mutable bool m_bCachedBiasValid;

  // bias in map?
  mutable bool m_bCachedBiasInMap;

  /// volume dimensions
  Triple<int4> m_volumeDimensions;

  /// volume dimensions
  Triple<float4> m_worldDimensions;

  /// voxel units (distance between voxels in world coordinates) (i.e. 0.7, 0.7, 2 for 2mm CT scans)
  Triple<float4> m_units;

  /// type of units i.e. METER, MILLIMETER, etc
  UnitTypeEnum m_eUnitType; 

  /// string representing the unit type
  std::vector<std::string> m_vsUnitType;

  /// number of significant bits per voxel (i.e. 12 for CT data)
  int4 m_iSignificantBitsPerVoxel;

  /// data type of voxels i.e. bool, int2, etc
  VoxelTypeEnum m_eVoxelType; 

  /// string representing the unit type
  std::vector<std::string> m_vsVoxelType;

  /// vector representing the history of this volume file
  std::vector<std::string> m_vsHistory;

  /// the background value as int4 
  int4 m_iBackground;

  /// volume title
  std::string m_sTitle;

  /// volume creation author
  std::string m_sAuthor;

  /// volume file name
  std::string m_sFileName;

  /// file name extension
  ExtensionEnum m_eFileNameExtension;

  /// string representing the extension
  std::vector<std::string> m_vsFileNameExtension;

  /// volume creation date
  vx::Date m_creationDate;

  /// best available compression method
  const hdr::FileCompressionEnum m_eBestFileCompression;

  /// used compression method
  hdr::FileCompressionEnum m_eUsedFileCompression;

  /// variable entry header map
  VarMap m_varHeader;

  /// The version of the file on disk
  std::pair<int4,int4> m_fileVersionPair;

  /// memory layout
  MemoryLayoutEnum m_eMemoryLayout;

  /// string representing the memory layout
  std::vector<std::string> m_vsMemoryLayout;

  /// string representing the varHeader entries
  std::vector<std::string> m_vsVarHeaderEntries;

  friend vxBlockVolume<int1>;
  friend vxBlockVolume<int2>;
  friend vxBlockVolume<int4>;
  friend vxBlockVolume<uint1>;
  friend vxBlockVolume<uint2>;
  friend vxBlockVolume<uint4>;
  friend vxBlockVolume<bool>;
  friend vxBlockVolume<float4>;
  friend vxBlockVolume<vxComplex>;
  friend vxBlockVolume<vxVoxelCost>;

}; // vxVolumeHeader


// inlines
#include "vxVolumeHeader.inl"


// $Log: vxVolumeHeader.h,v $
// Revision 1.14  2006/06/29 16:12:13  jmeade
// const declarations on 'get' methods.
//
// Revision 1.13  2006/06/08 18:20:15  frank
// Exposed the serialized dicom information
//
// Revision 1.12  2006/04/03 16:45:42  geconomos
// removed friend decaration for vxBlockVolumeFileReader
//
// Revision 1.11  2006/03/29 19:48:46  geconomos
// made a friend to class vxBlockVolumeFileReader
//
// Revision 1.10  2006/03/09 18:36:32  dongqing
// add vxVoxelCost voxel type to the vxBlockVolume
//
// Revision 1.9  2005/10/31 21:46:27  dongqing
// move the GetMemoryLayoutEnum as public function
//
// Revision 1.8  2005/05/23 11:15:38  michael
// Reworked Scale and Bias to always be returned
//
// Revision 1.7  2005/05/10 20:00:24  michael
// Made BitStored part of the enum of header vars instead of having a dedicated
// access function.
//
// Revision 1.6  2005/03/29 23:05:18  michael
// Adjusted header enums and assigned default numbers to match it in a way that
// these are backward compatible. Having the "UNKNOWN" being first was intro-
// ducing a non compatability with previous versions.
//
// Revision 1.5  2005/03/14 18:55:19  dongqing
// add RemoveHdrVar
//
// Revision 1.4  2004/11/02 18:05:24  michael
// for consistency reasons, renamed "SetMemoryLayout" to "SetMemoryLayoutEnum"
//
// Revision 1.3  2004/10/27 17:45:20  geconomos
// volume header encoding needs to be reviewed
//
// Revision 1.2  2004/10/26 13:29:54  michael
// added caching mechanism to variable
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.52  2004/02/10 19:29:28  michael
// added volume history into the class
//
// Revision 1.51  2003/09/04 17:29:44  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.50.2.1  2003/08/11 11:07:43  mkontak
// moved Date into vx namespace
//
// Revision 1.50  2003/06/20 18:41:03  dongqing
// add SetDicomFieldValue()
//
// Revision 1.49  2003/06/04 18:20:46  michael
// missing spacing
//
// Revision 1.48  2003/06/03 12:25:07  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.47  2003/05/17 16:22:37  michael
// reformatting
//
// Revision 1.46  2003/05/17 16:19:41  michael
// reformatting
//
// Revision 1.45  2003/05/06 18:51:47  frank
// Added Study Description header entry, but it still needs the actual DICOM lookup values from Dongqing.
//
// Revision 1.44  2003/05/02 18:29:12  michael
// initial code review
//
// Revision 1.43  2003/04/07 17:10:48  frank
// Added a modification string to the variable list.  This captures the sequence of modifications done to the volume since creation from original DICOM.
//
// Revision 1.42  2003/04/02 16:13:56  frank
// Issue #2953: Fixed referring physician name truncation if there are spaces
//
// Revision 1.41  2003/03/24 18:05:24  frank
// Issue #2953: Added referring physician to volume header display
//
// Revision 1.40  2003/01/22 21:27:05  ingmar
// dimension and units are now Triples
//
// Revision 1.39  2002/10/16 16:37:02  frank
// Added function to retrieve patient age when only birth date and study date are known.
//
// Revision 1.38  2002/10/07 16:50:31  dmitry
// GetLastFirstMidSuffix used
//
// Revision 1.37  2002/09/25 14:32:19  sgagliardo
// format patient date of birth.
//
// Revision 1.36  2002/09/24 16:00:44  michael
// removed last occurences of ColorTableGL
//
// Revision 1.35  2002/08/28 15:05:31  ingmar
// added support for vxComplex
//
// Revision 1.34  2002/08/13 15:50:47  ingmar
// circumvented linker issue of multiply defined symbols by moving the implementation into the .h file
//
// Revision 1.33  2002/08/13 15:15:38  ingmar
// added GetVariableVolumeHeader()
//
// Revision 1.32  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.31  2002/06/19 17:31:12  frank
// Fixed study date format.
//
// Revision 1.30  2002/06/17 22:38:47  soeren
// renamed GetSignificantBitPerVoxel to GetSignificantBitsPerVoxel
//
// Revision 1.29  2002/06/17 20:57:14  soeren
// renamed SetSignificantBitPerVoxel to SetSignificantBitsPerVoxel
//
// Revision 1.28  2002/06/12 23:19:45  frank
// Moved DICOM string functions back to RendererOverlay.
//
// Revision 1.27  2002/06/05 23:11:13  frank
// Fixed some fields.
//
// Revision 1.26  2002/06/05 11:29:35  soeren
// removed non const GetVolDim
//
// Revision 1.25  2002/06/05 11:27:03  soeren
// renamed SetVoxelUnitType to SetVoxelUnitTypeEnum
//
// Revision 1.24  2002/05/29 16:20:30  frank
// Fixed the study date and time.
//
// Revision 1.23  2002/05/22 23:41:55  ingmar
// added non-const access to VolDim
//
// Revision 1.22  2002/05/22 19:57:52  jenny
// added a bunch of functions to retrieve name, id, slicethickness and some other stuff used by 2D/3D renderers
//
// Revision 1.21  2002/05/21 20:42:29  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.20  2002/05/17 15:49:35  soeren
// removed rest of getDatamodality funtions
//
// Revision 1.19  2002/05/17 13:40:56  ingmar
// improved comments
//
// Revision 1.18  2002/05/17 00:47:15  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 1.17  2002/05/08 19:13:52  tushar
// made vxVolume complatible with float4
//
// Revision 1.16  2002/05/02 15:58:58  ingmar
// cleaned up header enums
//
// Revision 1.15  2002/05/01 21:10:55  soeren
// added data modality
//
// Revision 1.14  2002/04/27 04:19:53  frank
// Permitted setting the header map variables.
// Implemented DICOM variable lookup in header map.
//
// Revision 1.13  2002/04/26 19:55:37  soeren
// added VoxelTypeEnum
//
// Revision 1.12  2002/04/26 18:52:23  frank
// Added Dicom field interface to volume class.
//
// Revision 1.11  2002/04/24 19:07:11  michael
// default is now CT since most of our datasets are CT but they would need to
// be modified "by hand".
//
// Revision 1.10  2002/04/19 18:43:19  jaddonisio
// added new type bool
//
// Revision 1.9  2002/04/18 14:44:40  ingmar
// added GetDataModality and SetDataModality
//
// Revision 1.8  2002/04/17 19:50:52  ingmar
// added GetWorlDim
//
// Revision 1.7  2002/04/16 19:45:48  ingmar
// added more functionality
//
// Revision 1.6  2002/04/12 23:04:10  ingmar
// GetVolumeDimension()  -> GetVolDim()  ;-)
//
// Revision 1.5  2002/04/11 14:54:16  ingmar
// moved GetMemoryUsage() to vxBlockVolume
//
// Revision 1.4  2002/04/10 20:59:09  ingmar
// added more Get functions to VolumeHeader
//
// Revision 1.3  2002/04/10 19:14:16  ingmar
// BlockVolume supports now reading of a linearvolume
//
// Revision 1.2  2002/04/10 18:07:46  ingmar
// worked on new header volume class
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxVolumeHeader.h,v 1.14 2006/06/29 16:12:13 jmeade Exp $
// $Id: vxVolumeHeader.h,v 1.14 2006/06/29 16:12:13 jmeade Exp $
