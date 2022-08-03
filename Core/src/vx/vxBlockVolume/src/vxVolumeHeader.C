// $Id: vxVolumeHeader.C,v 1.14 2006/06/29 16:12:13 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "V3DFile.h"
#include "vxVolumeHeader.h"
#include "SerializedDicom.h"
#include "PersonName.h"
#include "vxComplex.h"
#include "vxDOMDocument.h"
#include "vxDOMElement.h"
#include "vxDOMNodeList.h"
#include "vxDOMNode.h"
#include "vxDOMNamedNodeMap.h"


// defines
#define FILE_REVISION "$Revision: 1.14 $"


// namespaces
using namespace std;


/**
 * Constructor
 * @param dimension of volume
 * @param units of volume
 * @param number of significant bits per voxel
 * @param background value
 * @param unit type
 * @param file name
 * @param title
 * @param author
 * @param date
 * @param extension
 */
vxVolumeHeader::vxVolumeHeader(const Triple<int4>   & dim,
                               const Triple<float4> & units,
                               const int4 & iSignificantBitsPerVoxel,
                               const int4 & iBackground,
                               UnitTypeEnum eUnitType,
                               const std::string & sFileName,
                               const std::string & sTitle,
                               const std::string & sAuthor,
                               const vx::Date & date,
                               ExtensionEnum eExtension
                             )
:
//m_bCachedBitsStoredValid(false),
//m_bCachedBiasValid(false),
m_volumeDimensions(dim),
m_units(units),
m_iSignificantBitsPerVoxel(iSignificantBitsPerVoxel),
m_iBackground(iBackground),
m_eUnitType(eUnitType),
m_sFileName(sFileName),
m_sTitle(sTitle),
m_sAuthor(sAuthor),
m_creationDate(date),
m_eFileNameExtension(eExtension),
m_eBestFileCompression(hdr::LZO),
m_eUsedFileCompression(m_eBestFileCompression),
m_worldDimensions(Triple<float4>((dim.X() - 1) * units.X(),(dim.Y() - 1) * units.Y(),(dim.Z() - 1) * units.Z())),
m_eVoxelType(UINT2)
{
  InitializeStringVectors();
  m_vCache.resize(NUM_OF_VARVOLHEADER_ENTRY_ENUM);
  ClearCachedValues();
} // constructor


/**
 * Copy constructor
 * @param other volume header to copy from
 */
vxVolumeHeader::vxVolumeHeader(const vxVolumeHeader & other)
:m_eBestFileCompression(other.m_eBestFileCompression)
{
  *this = other;
} // copy constructor


/**
 * Assignment operator
 * @param other volume header to copy from.
 * @return reference to the volume header.
 */
vxVolumeHeader & vxVolumeHeader::operator= (const vxVolumeHeader & other)
{
  if (&other != this)
  {
    m_vCache                     = other.m_vCache;
/*
    m_sCachedBitsStored          = other.m_sCachedBitsStored;
    m_bCachedBitsStoredValid     = other.m_bCachedBitsStoredValid;
*/
    m_bCachedBiasValid           = other.m_bCachedBiasValid;
    m_bCachedBiasInMap           = other.m_bCachedBiasInMap;
    m_sCachedBias                = other.m_sCachedBias;

    m_volumeDimensions         = other.m_volumeDimensions;
    m_units                    = other.m_units;
    m_iSignificantBitsPerVoxel = other.m_iSignificantBitsPerVoxel;
    m_iBackground              = other.m_iBackground;
    m_eUnitType                = other.m_eUnitType;
    m_sFileName                = other.m_sFileName;
    m_sTitle                   = other.m_sTitle;
    m_sAuthor                  = other.m_sAuthor;
    m_creationDate             = other.m_creationDate;
    m_eFileNameExtension       = other.m_eFileNameExtension;
    //m_eBestFileCompression     = other.m_eBestFileCompression,
    m_eUsedFileCompression     = other.m_eUsedFileCompression;
    m_worldDimensions          = other.m_worldDimensions;
    m_eVoxelType               = other.m_eVoxelType;
    m_vsUnitType               = other.m_vsUnitType;
    m_vsVoxelType              = other.m_vsVoxelType;
    m_vsFileNameExtension      = other.m_vsFileNameExtension;
    m_varHeader                = other.m_varHeader;
    m_fileVersionPair          = other.m_fileVersionPair;
    m_eMemoryLayout            = other.m_eMemoryLayout;
    m_vsMemoryLayout           = other.m_vsMemoryLayout;

    InitializeStringVectors();
  }
  return(*this);
}


/**
 * Builds the ascii version of the volume header according to the
 * "Viatronix File Specification Document"
 * 
 * @return the header string.
 */
std::string vxVolumeHeader::GetAsString() const
{
  VarMap fixedHeader;
  if (GetFileVersion() != std::pair<int4,int4>(1,0))
  {
    fixedHeader.SetHdrVar("sFilenameExtension", GetFileNameExtensionString(), "File Name Extension");
    fixedHeader.SetHdrVar("sTitle", m_sTitle, "Title");
    fixedHeader.SetHdrVar("sAuthor", m_sAuthor, "Author/Source");
    std::ostrstream dim;
    dim << m_volumeDimensions.X() << " " << m_volumeDimensions.Y() << " " << m_volumeDimensions.Z() << std::ends;
    fixedHeader.SetHdrVar("ivVolumeDimensions", dim.str() , "Volume Dimensions");
    std::ostrstream units;
    units << m_units.X() << " " << m_units.Y() << " " << m_units.Z() << std::ends;
    fixedHeader.SetHdrVar("fvUnits", units.str(), "Voxel Units");
    fixedHeader.SetHdrVar("iBackground", m_iBackground, "Background value for non-specified voxels");
    fixedHeader.SetHdrVar("sUnitType", GetVoxelUnitTypeString(), "Unit Type");    
    fixedHeader.SetHdrVar("sDataType", GetVoxelTypeString(), "Data Type");
    fixedHeader.SetHdrVar("iSignificantBitsPerVoxel", m_iSignificantBitsPerVoxel, "Bits Per Voxel");
    fixedHeader.SetHdrVar("sMemoryLayout", GetMemoryLayoutString(), "Memory Storage Order/Layout");
    fixedHeader.SetHdrVar("sCreationDate", m_creationDate.AsString(), "Date of Creation");
    
    return fixedHeader.ToString();
  }
  else
  {
    std::ostrstream ss;
    
    ss << "\r\n";
    
    /// file name extrension
    switch (GetFileNameExtensionEnum())
    {
    case vxVolumeHeader::VOL: ss << "0  # Extension - .VOL \r\n"; break;
    case vxVolumeHeader::ECV: ss << "1  # Extension - .ECV \r\n"; break;
    case vxVolumeHeader::INS: ss << "2  # Extension - .INS \r\n"; break;
    case vxVolumeHeader::SKE: ss << "3  # Extension - .SKE \r\n"; break;
    case vxVolumeHeader::DFB: ss << "4  # Extension - .DFB \r\n"; break;
    case vxVolumeHeader::DDB: ss << "5  # Extension - .DDB \r\n"; break;
    case vxVolumeHeader::SLD: ss << "6  # Extension - .SLD \r\n"; break;
    case vxVolumeHeader::DFS: ss << "7  # Extension - .DFS \r\n"; break;
    case vxVolumeHeader::DFE: ss << "8  # Extension - .DFE \r\n"; break;
    case vxVolumeHeader::PDE: ss << "9  # Extension - .PDE \r\n"; break;
    case vxVolumeHeader::SLC: ss << "10 # Extension - .SLC \r\n"; break;
    default                : ss << "11 # Extension - UNKNOWN_EXT \r\n";
    }
    
    ss << GetTitle()                  << " # Title \r\n";
    ss << GetAuthor()                 << " # Author/Source \r\n";
    ss << GetVolDim().X() << " "<< GetVolDim().Y() << " "<< GetVolDim().Z() << " # Volume Dimensions \r\n";
    ss << GetVoxelUnits().X()  << " " << GetVoxelUnits().Y()  << " "<< GetVoxelUnits().Z()  << " # Voxel Units \r\n";
    ss << GetBackground()             << " # Background value for non-specified voxels \r\n";
    
    /// voxel unit type
    switch (GetVoxelUnitTypeEnum())
    {
    case vxVolumeHeader::METER     : ss << "0  # Unit Type - METER      \r\n"; break;
    case vxVolumeHeader::MILLIMETER: ss << "1  # Unit Type - MILLIMETER \r\n"; break;
    case vxVolumeHeader::MICRON    : ss << "2  # Unit Type - MICRON     \r\n"; break;
    case vxVolumeHeader::FOOT      : ss << "3  # Unit Type - FOOT       \r\n"; break;
    case vxVolumeHeader::INCH      : ss << "4  # Unit Type - INCH       \r\n"; break;
    default                      : ss << "5  # Unit Type - UNKNOWN    \r\n"; break;
    }
    
    //// voxel data type
    string sDataType = GetVoxelTypeString();
    if (m_eVoxelType ==  BOOL) sDataType = typeid(bool).name();
    if (m_eVoxelType ==  INT1) sDataType = typeid(int1).name();
    if (m_eVoxelType ==  INT2) sDataType = typeid(int2).name();
    if (m_eVoxelType ==  INT4) sDataType = typeid(int4).name();
    if (m_eVoxelType == UINT1) sDataType = typeid(uint1).name();
    if (m_eVoxelType == UINT2) sDataType = typeid(uint2).name();
    if (m_eVoxelType == UINT4) sDataType = typeid(uint4).name();
    if (m_eVoxelType == FLOAT4) sDataType = typeid(float4).name();
    if (m_eVoxelType == FLOAT8) sDataType = typeid(float8).name();
    for (uint4 j = 0; j < sDataType.length(); ++j)
    {
      if (sDataType[j] == ' ') sDataType[j] = '-';
    }
    ss << sDataType << " " << GetSignificantBitsPerVoxel() << " # Data Type, Bits Per Voxel "<< "\r\n";
    
    //// memory layout
    switch (GetMemoryLayoutEnum())
    {
    case vxVolumeHeader::LINEAR:    ss << "0 # Memory Storage Order/Layout - LINEAR    \r\n"; break;
    case vxVolumeHeader::HASH:      ss << "1 # Memory Storage Order/Layout - HASH      \r\n"; break;
    case vxVolumeHeader::BLOCK:     ss << "2 # Memory Storage Order/Layout - BLOCK     \r\n"; break;             
    case vxVolumeHeader::REC_BLOCK: ss << "3 # Memory Storage Order/Layout - REC_BLOCK \r\n"; break;             
    default:                          ss << "4 # Memory Storage Order/Layout - UNKNOWN   \r\n"; break;
    }
    
    //// date
    ss << GetCreationDate().AsString()<< " # Date of Creation \r\n";
    
    ss << std::ends;
    
    return(string(ss.str())); 
  }  
} // GetAsString


/**
 * Returns variable volume header and dicom header as string
 * @return string
 */
std::string vxVolumeHeader::GetVarMapsAsString() const
{
  VarMap varHeader;
  varHeader.SetHdrVar("varHeader", m_varHeader.ToString(), "");
  
  return (varHeader.ToString_1());
} // GetVarMapsAsString()


/**
 * Read Volume from disk
 * @param filename on disk including the path
 */
void vxVolumeHeader::Read(const std::string & sFileName)
{
  std::stringstream sErrorMessage;
  V3DFile img;
  try
  {
    img.SetSkipData(true);
    img.Read(sFileName, make_pair(0.0F, 100.0F)); // could throw many exceptions
  }
  catch (ex::VException & ex)
  {
    sErrorMessage.clear();
    sErrorMessage << "Reading of " << sFileName << " failed.";
    LogErr(sErrorMessage.str(),"vxVolumeHeader","Read");    
    return;
  }

  VarMap varHeader(img.GetDescription());
  std::istrstream sheader(img.GetDescription().c_str());
  SetFileVersion(img.GetFileVersion());

  const int4 bSize(1024);
  int1 sbuffer[bSize];
  int4 iDelimiter = '#';
  std::string sDataType;


  // File version 2.0 example
  //
  // .VOL # File Name Extension                     
  // FFS scan # Title                            
  // UNIV. MED. CTR. STONYBROOK CT1 on GE MEDICAL SYSTEMS,HiSpeed CT/i # Author/Source                    
  // 32 32 32 # Volume Dimensions                   
  // 1 1 1 # Voxel Units                            
  // 0 # Background value for non-specified voxels  
  // MILLIMETER # Unit Type                         
  // UINT4 16 # Data Type, Bits Per Voxel           
  // BLOCK # Memory Storage Order/Layout            
  // 2002.05.02 11:42:26.0 # Date of Creation       
  // CT # data modality                             


  // File version 1.0 example
  //
  // 0 # Extension - .VOL
  // FFS scan       # Title 
  // UNIV. MED. CTR. STONYBROOK CT1 on GE MEDICAL SYSTEMS,HiSpeed CT/i      # Author/Source 
  // 64 64 51 # Volume Dimensions 
  // 0.253906 0.253906 0.24636 # Voxel Units 
  // 0 # Background value for non-specified voxels 
  // 1 # Unit Type - MILLIMETER
  // short 16 # Data Type, Bits Per Voxel 
  // 0 # Memory Storage Order - LINEAR
  // 2002.03.19 16:19:48.0 # Date of Creation 

  // file name extension
  if (GetFileVersion() == std::pair<int4,int4>(1,0))
  {
    m_varHeader = VarMap(img.GetComment());
    
    uint4 extension;
    sheader >> extension; sheader.getline(sbuffer, bSize);
    
    switch (extension)
    {
    case 0 : SetFileNameExtension(vxVolumeHeader::VOL); break;
    case 1 : SetFileNameExtension(vxVolumeHeader::ECV); break;
    case 2 : SetFileNameExtension(vxVolumeHeader::INS); break;
    case 3 : SetFileNameExtension(vxVolumeHeader::SKE); break;
    case 4 : SetFileNameExtension(vxVolumeHeader::DFB); break;
    case 5 : SetFileNameExtension(vxVolumeHeader::DDB); break;
    case 6 : SetFileNameExtension(vxVolumeHeader::SLD); break;
    case 7 : SetFileNameExtension(vxVolumeHeader::DFS); break;
    case 8 : SetFileNameExtension(vxVolumeHeader::DFE); break;
    case 9 : SetFileNameExtension(vxVolumeHeader::PDE); break;
    case 10: SetFileNameExtension(vxVolumeHeader::SLC); break;
    default: SetFileNameExtension(vxVolumeHeader::UNKNOWN_EXTENSION_ENUM); break;
    }
    
    // title
    sheader.getline(sbuffer, bSize, iDelimiter);
    m_sTitle = sbuffer;
    sheader.getline(sbuffer, bSize); //read rest of line
    
    // author
    sheader.getline(sbuffer, bSize, iDelimiter);
    m_sAuthor = sbuffer;
    sheader.getline(sbuffer, bSize); //read rest of line
    
    // volume dimensions
    sheader >> m_volumeDimensions.X() >> m_volumeDimensions.Y() >> m_volumeDimensions.Z();
    sheader.getline(sbuffer, bSize); //read rest of line
    
    // units
    sheader >> m_units.X() >> m_units.Y() >> m_units.Z();
    sheader.getline(sbuffer, bSize); //read rest of line
    
    // background
    sheader >> m_iBackground; sheader.getline(sbuffer, bSize);
    
    uint4 unitType;
    sheader >> unitType; sheader.getline(sbuffer, bSize);
    
    switch (unitType)
    {
    case 0 : SetVoxelUnitTypeEnum(vxVolumeHeader::METER);             break;
    case 1 : SetVoxelUnitTypeEnum(vxVolumeHeader::MILLIMETER);        break;
    case 2 : SetVoxelUnitTypeEnum(vxVolumeHeader::MICRON);            break;
    case 3 : SetVoxelUnitTypeEnum(vxVolumeHeader::FOOT);              break;
    case 4 : SetVoxelUnitTypeEnum(vxVolumeHeader::INCH);              break;
    default: SetVoxelUnitTypeEnum(vxVolumeHeader::UNKNOWN_UNIT_TYPE); break;
    }
    
    sheader >> sDataType >> m_iSignificantBitsPerVoxel; sheader.getline(sbuffer, bSize);
    // srip dashes if present
    for (uint4 j = 0; j < sDataType.length(); ++j)
    {
      if (sDataType[j] == '-') sDataType[j] = ' ';
    }
    m_eVoxelType = GetVoxelTypeEnumFromString(sDataType);   
    
    // memory layout
    uint4 memoryLayout;
    sheader >> memoryLayout; sheader.getline(sbuffer, bSize);
    switch (memoryLayout)
    {
    case 0 : SetMemoryLayoutEnum(vxVolumeHeader::LINEAR);                break;
    case 1 : SetMemoryLayoutEnum(vxVolumeHeader::HASH);                  break;
    case 2 : SetMemoryLayoutEnum(vxVolumeHeader::BLOCK);                 break;
    case 3 : SetMemoryLayoutEnum(vxVolumeHeader::REC_BLOCK);             break;
    default: SetMemoryLayoutEnum(vxVolumeHeader::UNKNOWN_MEMORY_LAYOUT); break;
    }
    
    // date
    sheader.getline(sbuffer, bSize, iDelimiter);
    vx::Date dt(sbuffer);
    m_creationDate = dt;
    sheader.getline(sbuffer, bSize); // get rest of line
  }
  else
  {
    // split the comment into several submaps.
    VarMap commentMap;
    commentMap.CreateMap(img.GetComment());
    std::string sVarHeader;
    commentMap.GetHdrVar("varHeader", sVarHeader);
    VarMap mapVar(sVarHeader); 
    m_varHeader = mapVar;

    // as used by file version 2.0 and later
    string sFileNameExtension;
    varHeader.GetHdrVar("sFilenameExtension", sFileNameExtension);

    for (int k=0; k<vxVolumeHeader::NUM_OF_EXTENSION_ENUM; ++k)
    {
      if (sFileNameExtension == m_vsFileNameExtension[k])
      {
        SetFileNameExtension(static_cast<ExtensionEnum>(k)); 
        break;
      }
    }

    // title
    varHeader.GetHdrVar("sTitle", m_sTitle);

    // author
    varHeader.GetHdrVar("sAuthor", m_sAuthor);

    // volume dimensions
    varHeader.GetHdrVar("ivVolumeDimensions", m_volumeDimensions);

    // units
    varHeader.GetHdrVar("fvUnits", m_units);

    // background
    varHeader.GetHdrVar("iBackground", m_iBackground);

    // unit type
    string sUnitType;
    varHeader.GetHdrVar("sUnitType", sUnitType);

    for (int4 k=0; k<vxVolumeHeader::NUM_OF_UNIT_TYPE_ENUM; ++k)
    {
      if (sUnitType == m_vsUnitType[k])
      {
        SetVoxelUnitTypeEnum(static_cast<UnitTypeEnum>(k)); 
        break;
      }
    }

    // get voxel data type  and  significant bits per voxel
    varHeader.GetHdrVar("sDataType", sDataType);
    varHeader.GetHdrVar("iSignificantBitsPerVoxel", m_iSignificantBitsPerVoxel);
 
    // srip dashes if present
    for (uint4 j = 0; j < sDataType.length(); ++j)
    {
      if (sDataType[j] == '-') sDataType[j] = ' ';
    }
  
    m_eVoxelType = GetVoxelTypeEnumFromString(sDataType);

    // memory layout
    string sMemoryLayout;
    varHeader.GetHdrVar("sMemoryLayout", sMemoryLayout);

    for (int4 k=0; k<vxVolumeHeader::NUM_OF_MEMORY_LAYOUT_ENUM; ++k)
    {
      if (sMemoryLayout == m_vsMemoryLayout[k])
      {
        SetMemoryLayoutEnum(static_cast<MemoryLayoutEnum>(k)); 
        break;
      }
    }

    // date
    string sCreationDate;
    varHeader.GetHdrVar("sCreationDate", sCreationDate);
    vx::Date dt(sCreationDate);
    m_creationDate = dt;

  }

  // Validation, make sure no units are zero
  if (m_units.X() == 0.0F) m_units.X() = 1.0F;
  if (m_units.Y() == 0.0F) m_units.Y() = 1.0F;
  if (m_units.Z() == 0.0F) m_units.Z() = 1.0F;
  
  m_worldDimensions.X() = (m_volumeDimensions.X() - 1) * m_units.X();
  m_worldDimensions.Y() = (m_volumeDimensions.Y() - 1) * m_units.Y();
  m_worldDimensions.Z() = (m_volumeDimensions.Z() - 1) * m_units.Z();

  ClearCachedValues();
} // Read()


/**
 * Set the volume dimensions
 * @param new volume dimension vector
 */
void vxVolumeHeader::SetVolDim(const Triple<int4> & volumeDimensions)
{
  m_volumeDimensions = volumeDimensions;
  m_worldDimensions.X() = (m_volumeDimensions.X() - 1) * m_units.X();
  m_worldDimensions.Y() = (m_volumeDimensions.Y() - 1) * m_units.Y();
  m_worldDimensions.Z() = (m_volumeDimensions.Z() - 1) * m_units.Z();
} // SetVolDim()


/**
 * Set the volume dimensions
 * @param new x dimension
 * @param new y dimension
 * @param new z dimension
 */
void vxVolumeHeader::SetVolDim(int4 iDimensionX, 
                               int4 iDimensionY, 
                               int4 iDimensionZ)
{
  m_volumeDimensions.X() = iDimensionX;
  m_volumeDimensions.Y() = iDimensionY;
  m_volumeDimensions.Z() = iDimensionZ;
  m_worldDimensions.X() = (m_volumeDimensions.X() - 1) * m_units.X();
  m_worldDimensions.Y() = (m_volumeDimensions.Y() - 1) * m_units.Y();
  m_worldDimensions.Z() = (m_volumeDimensions.Z() - 1) * m_units.Z();
} // SetVolDim()


/**
 * Sets the voxel units
 * @param units
 */
void vxVolumeHeader::SetVoxelUnits(const Triple<float4> & units)
{
  m_units = units;
  m_worldDimensions.X() = (m_volumeDimensions.X() - 1) * m_units.X();
  m_worldDimensions.Y() = (m_volumeDimensions.Y() - 1) * m_units.Y();
  m_worldDimensions.Z() = (m_volumeDimensions.Z() - 1) * m_units.Z();
}


/**
 * Returns the voxel unit type string
 * @return string
 */
const std::string & vxVolumeHeader::GetVoxelUnitTypeString() const
{
  return m_vsUnitType[GetVoxelUnitTypeEnum()];
}


/**
 * Returns the voxel type string
 * @return string
 */
const std::string & vxVolumeHeader::GetVoxelTypeString() const
{
  return m_vsVoxelType[m_eVoxelType];
}

/**
 * Returns the enum of the type that matches the given string.
 * @param voxel type string
 * @return voxel type
 */
vxVolumeHeader::VoxelTypeEnum vxVolumeHeader::GetVoxelTypeEnumFromString(const std::string & sVoxelTypeString) const
{
  // as used by file version 2.0 and later
  for (int k(0); k<vxVolumeHeader::NUM_OF_VOXEL_TYPE_ENUM; ++k)
  {
    if (sVoxelTypeString == m_vsVoxelType[k])
    {
      return(VoxelTypeEnum(k));
    }
  }

  // as used in file version 1.0
  if (sVoxelTypeString == typeid(bool).name()) return(vxVolumeHeader::BOOL );
  if (sVoxelTypeString == typeid(int1).name()) return(vxVolumeHeader::INT1 );
  if (sVoxelTypeString == typeid(int2).name()) return(vxVolumeHeader::INT2 );
  if (sVoxelTypeString == typeid(int4).name()) return(vxVolumeHeader::INT4 );
  if (sVoxelTypeString == typeid(uint1).name()) return(vxVolumeHeader::UINT1);
  if (sVoxelTypeString == typeid(uint2).name()) return(vxVolumeHeader::UINT2);
  if (sVoxelTypeString == typeid(uint4).name()) return(vxVolumeHeader::UINT4);
  if (sVoxelTypeString == typeid(float4).name()) return(vxVolumeHeader::FLOAT4);
  if (sVoxelTypeString == typeid(float8).name()) return(vxVolumeHeader::FLOAT8);
  if (sVoxelTypeString==typeid(vxComplex).name()) return(vxVolumeHeader::COMPLEX);
  if (sVoxelTypeString==typeid(vxVoxelCost).name()) return(vxVolumeHeader::VXVOXELTYPE);

  // default unknown
  return(vxVolumeHeader::UNKNOWN_VOXEL_TYPE);
} // GetVoxelTypeEnumFromString()


/**
 * Returns the volume file name extension string
 * @return file name extension
 */
const std::string & vxVolumeHeader::GetFileNameExtensionString() const
{
  return m_vsFileNameExtension[GetFileNameExtensionEnum()];
}


/**
 * Sets the volume creation date
 * @param iMajorVersionNumber
 * @param iMinorVersionNumber
 */
void vxVolumeHeader::SetFileVersion(int4 iMajorVersionNumber, 
                                    int4 iMinorVersionNumber)
{
  m_fileVersionPair.first  = iMajorVersionNumber;
  m_fileVersionPair.second = iMinorVersionNumber; 
}


/**
 * Return number of voxels in the volume
 * @return number of voxels
 */
uint4 vxVolumeHeader::GetNumberOfVoxels() const
{
  return m_volumeDimensions.X() * m_volumeDimensions.Y() * m_volumeDimensions.Z();
}


/**
 * Returns the memory layout string
 * @return memory layout
 */
const std::string & vxVolumeHeader::GetMemoryLayoutString() const
{
  return(m_vsMemoryLayout[GetMemoryLayoutEnum()]);
}


/**
 * Initializes the vectors containing the File, Data, Layout, Extension, and Unit types.
 */
void vxVolumeHeader::InitializeStringVectors()
{
  int4 iCount(0);

  const int1 * const cstrUnitTypeStr[NUM_OF_UNIT_TYPE_ENUM] = UNIT_TYPE_STRINGS;
  m_vsUnitType.resize(NUM_OF_UNIT_TYPE_ENUM);
  for (iCount = 0; iCount < NUM_OF_UNIT_TYPE_ENUM; ++iCount)
    m_vsUnitType[iCount] = cstrUnitTypeStr[iCount];

  const int1 * const cstrExtensionStr[NUM_OF_EXTENSION_ENUM] = EXTENSION_STRINGS; 
  m_vsFileNameExtension.resize(NUM_OF_EXTENSION_ENUM);
  for (iCount = 0; iCount < NUM_OF_EXTENSION_ENUM; ++iCount)
    m_vsFileNameExtension[iCount] = cstrExtensionStr[iCount];

  const int1 * const cstrMemoryLayoutStr[NUM_OF_MEMORY_LAYOUT_ENUM] = MEMORY_LAYOUT_STRINGS;
  m_vsMemoryLayout.resize(NUM_OF_MEMORY_LAYOUT_ENUM);
  for (iCount = 0; iCount < NUM_OF_MEMORY_LAYOUT_ENUM; ++iCount)
    m_vsMemoryLayout[iCount] = cstrMemoryLayoutStr[iCount];

  const int1 * const cstrVoxelTypeStr[NUM_OF_VOXEL_TYPE_ENUM] = VOXEL_TYPE_STRINGS;
  m_vsVoxelType.resize(NUM_OF_VOXEL_TYPE_ENUM);
  for (iCount = 0; iCount < NUM_OF_VOXEL_TYPE_ENUM; ++iCount)
    m_vsVoxelType[iCount] = cstrVoxelTypeStr[iCount];

  const int1 * const cstrVarHeaderEntries[NUM_OF_VARVOLHEADER_ENTRY_ENUM] = VARVOLHEADER_STRINGS;
  m_vsVarHeaderEntries.resize(NUM_OF_VARVOLHEADER_ENTRY_ENUM);
  for (iCount = 0; iCount < NUM_OF_VARVOLHEADER_ENTRY_ENUM; ++iCount)
    m_vsVarHeaderEntries[iCount] = cstrVarHeaderEntries[iCount];
} // InitializeStringVectors()


///**
// * Populate a serialized DICOM structure
// *
// * @param dicomInformation   out structure for dicom info
// */
//void vxVolumeHeader::GetSerializedDicom( SerializedDicom & dicomInformation ) const
//{
//
//  std::string sDicomVars;
//  if ( m_varHeader.GetHdrVar("dicomHeader", sDicomVars) )
//  {
//    std::stringstream ss( sDicomVars );
//    ss >> dicomInformation;
//  }
//  else
//  {
//    LogWrn( "No DICOM information is available", "vxVolumeHeader", "GetSerializedDicom" );
//  }
//
//} // GetSerializedDicom( SerializedDicom )


/**
 * Return a specific DICOM header entry
 * @param group
 * @param field
 * @param default value
 * @return string
 */
std::string vxVolumeHeader::GetDicomFieldValue(const std::string & sGroup, 
                                               const std::string & sField, 
                                               const std::string & sDefaultValue) const 
{

  // Set value to the default
  std::string sValue = sDefaultValue;

  std::string sDicomVars;
  if (m_varHeader.GetHdrVar("dicomHeader", sDicomVars) == false)
  {
    return(sDefaultValue);
  } // END ... Dicom Header not found

  SerializedDicom serDicom;
  std::stringstream ss(sDicomVars);
  ss >> serDicom;
  sValue = serDicom.GetFieldValue(sGroup, sField);

  return sValue;

} // GetDicomFieldValue()


/**
 * Try to retrieve the patient's age in years.
 * @return the patient's age in years
 */
float4 vxVolumeHeader::GetPatientAge() const
{
  // first check the age field
  string sAge("");
  GetHeaderEntry(vxVolumeHeader::PatientAge, sAge);
  float4 fAge(0.0F);
  ToType(sAge, fAge);

  // not so easy - must compute it
  if(fAge == 0.0F)
  {
    // try to compute the age by subtracting the date of birth from the scan date
    string sBirthDate(""), sStudyDate("");
    GetHeaderEntry(vxVolumeHeader::PatientBirthdate, sBirthDate);
    GetHeaderEntry(vxVolumeHeader::StudyDate       , sStudyDate);
    vx::Date birthDate, studyDate;
    birthDate.SetDate(sBirthDate);
    studyDate.SetDate(sStudyDate);

    // compute difference and convert to years
    int4 iDaysOld(vx::Date::DateDiff(studyDate, birthDate));
    const float4 fDaysInYear(365.25);
    fAge = iDaysOld / fDaysInYear;
    if(fAge > 200)
    {
      fAge = 0.0F;
    }
  } // age == 0?

  return fAge;
} // GetPatientAge()

/**
 * Remove a variable from the VarMap volume header
 *
 * @param sVarName 
 */
void vxVolumeHeader::RemoveVariableFromHeader( std::string & sVarName )
{
  m_varHeader.RemoveHdrVar( sVarName );
  ClearCachedValues();
}


///**
// * Reset a specific DICOM header entry
// * @param group
// * @param field
// * @param new value
// * @return true if this field is available in the current header
// */
//bool vxVolumeHeader::SetDicomFieldValue(const std::string & sGroup, 
//                                        const std::string & sField, 
//                                        const std::string & sNewValue)
//{
//  std::string sDicomVars;
//  if (m_varHeader.GetHdrVar("dicomHeader", sDicomVars) == false)
//  {
//    return(false);
//  }
//
//  // we could search for each individually but let's just invalidate them all
//  ClearCachedValues();
//
//  SerializedDicom serDicom;
//  std::stringstream ss(sDicomVars);
//  ss >> serDicom;
//  serDicom.SetFieldValue(sGroup, sField, sNewValue);
//  std::stringstream sss(" ");
//  sss << serDicom;
//
//  m_varHeader.SetHdrVar( "dicomHeader", sss.str(), "dicomHeader");
//
//  return(true);
//} // GetDicomFieldValue()


/** 
 * Append history information
 * @param sHistory information to be addded
 */
void vxVolumeHeader::AppendHistory(const std::string & sHistory)
{
  const int4 iMaxEntries(20); // keep no more entries than that!!!
  int4 iNumEntries(0);
  string sNumEntries;

  if (m_varHeader.GetHdrVar("HistoryEntries", iNumEntries) == true)
  {
    iNumEntries = atoi(sNumEntries.c_str());

    if (iNumEntries < iMaxEntries)
    {
      // increase entries number by one and update VarMap
      iNumEntries++;
      m_varHeader.SetHdrVar("HistoryEntries", ToAscii(iNumEntries), "number of history entries");

      // write new entry into VarMap
      string sToken("HistoryEntry" + ToAscii(iNumEntries));
      m_varHeader.SetHdrVar(sToken, sHistory, "history entry");
    }
    else
    {
      // reached maximum of entries, need to keep last n-1 and add the new one
      std::vector<std::string> vsEntries(vxVolumeHeader::GetHistory());

      // leave oldest entry out
      for (uint4 i(1); i<vsEntries.size(); i++)
      {
        string sToken("HistoryEntry" + ToAscii(i-1));
        m_varHeader.SetHdrVar(sToken, vsEntries[i], "history entry");
      }

      // get number of entries and store back (in case there was a mismatch, this will keep us in a consitent state)
      iNumEntries = static_cast< int4 >( vsEntries.size() );
      m_varHeader.SetHdrVar("HistoryEntries", ToAscii(iNumEntries), "number of history entries");

      // write new entry
      string sToken("HistoryEntry" + ToAscii(iNumEntries+1));
      m_varHeader.SetHdrVar(sToken, sHistory, "history entry");
    }
  }
  else
  {
    // no hirsoty so far? --> Add initial comment ...
    m_varHeader.SetHdrVar("HistoryEntry0", "Initial start of creation history", "this was the first thim a hsitory entry was created");
    iNumEntries = 1;
    m_varHeader.SetHdrVar("HistoryEntries", ToAscii(iNumEntries), "number of history entries");

    // ... and now add new entry
    AppendHistory(sHistory);
  }

  return;
} // AppendHistory()


/** 
 * Returns history information
 * @return overall information
 */
std::vector<std::string> vxVolumeHeader::GetHistory() const
{
  int4 iNumEntries(1);
  string sNumEntries;

  if (m_varHeader.GetHdrVar("HistoryEntries", sNumEntries) == true)
  {
    iNumEntries = atoi(sNumEntries.c_str());
  }

  vector<string> vsHistory;
  string sEntry;

  for (int4 i(0); i<iNumEntries; i++)
  {
    string sToken("HistoryEntry" + ToAscii(i));

    if (m_varHeader.GetHdrVar(sToken, sEntry) == false)
    {
      sEntry = "missing history entry";
    }
    vsHistory.push_back(sEntry);
  }

  return vsHistory;
} // GetHistory()


// undefines
#undef FILE_REVISION


// $Log: vxVolumeHeader.C,v $
// Revision 1.14  2006/06/29 16:12:13  jmeade
// const declarations on 'get' methods.
//
// Revision 1.13  2006/06/08 18:20:15  frank
// Exposed the serialized dicom information
//
// Revision 1.12  2006/05/31 16:16:41  geconomos
// porting to visual studio 2005
//
// Revision 1.11  2006/03/09 18:35:39  dongqing
// add vxVoxelCost voxel type to the vxBlockVolume
//
// Revision 1.10  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.9  2005/05/10 20:00:24  michael
// Made BitStored part of the enum of header vars instead of having a dedicated
// access function.
//
// Revision 1.8  2005/03/29 23:05:43  michael
// Added a ClearCachedValues() since after removing an entry, the cache needs
// to be updated.
//
// Revision 1.7  2005/03/14 18:55:31  dongqing
// add RemoveHdrVar
//
// Revision 1.6  2005/03/08 19:09:33  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.1  2005/01/31 19:23:54  frank
// was missing variable initialization
//
// Revision 1.5  2004/11/02 18:05:24  michael
// for consistency reasons, renamed "SetMemoryLayout" to "SetMemoryLayoutEnum"
//
// Revision 1.4  2004/10/27 17:45:20  geconomos
// volume header encoding needs to be reviewed
//
// Revision 1.3  2004/10/26 16:23:16  michael
// added a clear cached values after Read() of volume header
//
// Revision 1.2  2004/10/26 13:29:54  michael
// added caching mechanism to variable
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.46  2004/02/10 19:29:28  michael
// added volume history into the class
//
// Revision 1.45  2004/02/03 12:28:20  michael
// fixed bug in the world dimensions computation; was done in two functions
// but not in all.
//
// Revision 1.44  2003/09/04 17:29:44  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.43.2.1  2003/08/11 11:07:43  mkontak
// moved Date into vx namespace
//
// Revision 1.43  2003/06/20 18:41:02  dongqing
// add SetDicomFieldValue()
//
// Revision 1.42  2003/05/17 16:22:37  michael
// reformatting
//
// Revision 1.41  2003/05/17 16:19:41  michael
// reformatting
//
// Revision 1.40  2003/05/15 20:12:22  frank
// spelling
//
// Revision 1.39  2003/05/02 18:29:12  michael
// initial code review
//
// Revision 1.38  2003/04/19 16:24:27  frank
// Added progress bar parameter
//
// Revision 1.37  2003/01/22 21:27:20  ingmar
// dimension and units are now Triples
//
// Revision 1.36  2002/10/16 16:37:01  frank
// Added function to retrieve patient age when only birth date and study date are known.
//
// Revision 1.35  2002/08/28 15:05:30  ingmar
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
// Revision 1.31  2002/06/17 22:38:47  soeren
// renamed GetSignificantBitPerVoxel to GetSignificantBitsPerVoxel
//
// Revision 1.30  2002/06/17 20:57:14  soeren
// renamed SetSignificantBitPerVoxel to SetSignificantBitsPerVoxel
//
// Revision 1.29  2002/06/12 23:19:45  frank
// Moved DICOM string functions back to RendererOverlay.
//
// Revision 1.28  2002/06/07 20:12:50  binli
// copy constructor
//
// Revision 1.27  2002/06/05 11:27:03  soeren
// renamed SetVoxelUnitType to SetVoxelUnitTypeEnum
//
// Revision 1.26  2002/05/29 16:20:30  frank
// Fixed the study date and time.
//
// Revision 1.25  2002/05/23 14:49:06  ingmar
// included Global.h to get rid of debug compile warnings
//
// Revision 1.24  2002/05/22 19:57:52  jenny
// added a bunch of functions to retrieve name, id, slicethickness and some other stuff used by 2D/3D renderers
//
// Revision 1.23  2002/05/17 15:49:35  soeren
// removed rest of getDatamodality funtions
//
// Revision 1.22  2002/05/17 00:47:15  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 1.21  2002/05/14 16:57:42  ingmar
// fixed test for self in operator =
//
// Revision 1.20  2002/05/02 15:59:23  ingmar
// new version 2.0 file header
//
// Revision 1.19  2002/05/01 21:10:55  soeren
// added data modality
//
// Revision 1.18  2002/04/30 21:37:04  jenny
// i think when you get world dimensions, it should be (dim-1) * unit rather than dim*unit
//
// Revision 1.17  2002/04/29 19:23:13  soeren
// fixed worlddimension
//
// Revision 1.16  2002/04/27 04:19:53  frank
// Permitted setting the header map variables.
// Implemented DICOM variable lookup in header map.
//
// Revision 1.15  2002/04/26 20:05:56  frank
// Added Dicom field interface to volume class.
//
// Revision 1.14  2002/04/26 19:55:36  soeren
// added VoxelTypeEnum
//
// Revision 1.13  2002/04/18 19:52:13  ingmar
// now reading file version number from file
//
// Revision 1.12  2002/04/18 14:44:40  ingmar
// added GetDataModality and SetDataModality
//
// Revision 1.11  2002/04/17 19:50:52  ingmar
// added GetWorlDim
//
// Revision 1.10  2002/04/17 00:55:37  ingmar
// Fixed GetAsString()
//
// Revision 1.9  2002/04/16 19:45:48  ingmar
// added more functionality
//
// Revision 1.8  2002/04/11 14:54:16  ingmar
// moved GetMemoryUsage() to vxBlockVolume
//
// Revision 1.7  2002/04/10 20:59:09  ingmar
// added more Get functions to VolumeHeader
//
// Revision 1.6  2002/04/10 19:14:16  ingmar
// BlockVolume supports now reading of a linearvolume
//
// Revision 1.5  2002/04/10 18:57:49  ingmar
// worked on new header volume class
//
// Revision 1.4  2002/04/10 18:56:14  ingmar
// worked on new header volume class
//
// Revision 1.3  2002/04/10 18:53:33  ingmar
// worked on new header volume class
//
// Revision 1.2  2002/04/10 18:07:46  ingmar
// worked on new header volume class
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxVolumeHeader.C,v 1.14 2006/06/29 16:12:13 jmeade Exp $
// $Id: vxVolumeHeader.C,v 1.14 2006/06/29 16:12:13 jmeade Exp $
