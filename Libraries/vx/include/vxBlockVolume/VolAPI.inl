// $Id: VolAPI.inl,v 1.2 2006/02/03 15:22:11 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: summer 00
// Author: Dave & Jay dave@viatronix.com
//
// Complete History at bottom of file.

#define FILE_REVISION "$Revision: 1.2 $"

/**
 * Initializes all data members to the default volume.  Normally followed
 * by a call to Read().
 */
template <class T>
VolAPI<T>::VolAPI() : m_id(0), m_dimension(2,2,2), m_uiBitsPerVoxel(sizeof(T)*8),
m_units(1.0,1.0,1.0), m_unitType(METER), m_bestCompression(LZO),
m_fileCompression(m_bestCompression), m_background(0), m_extension(VOL), m_sTitle("Blank"),
m_sAuthor("Blank"), m_sFileName("BLANK.vol"), m_headerMap(),
m_uiMaxTitleLength(100), m_uiMaxAuthorLength(250), m_creationDate()
{
  InitializeTypes();
} // constructor


/**
 * Constructor for building new volumes.
 * @param dim the volume dimensions.
 * @param units the volume units.
 * @param dataType the volume dataType.
 * @param uiBitsPerVoxel the bits per voxel.
 * @param unitType
 * @param ext the volume extension.
 * @param sFileName the file name.
 * @param sTitle the volume title.
 * @param sAuthor the volume author/source.
 */
template <class T>
VolAPI<T>::VolAPI(const Triple<uint4> & dim, const uint4 & uiBitsPerVoxel,
                  const Triple<float4> & units, UnitTypeEnum unitType,
                  const T & background, ExtensionEnum ext,
                  const std::string & sFileName, const std::string & sTitle,
                  const std::string & sAuthor, const vx::Date & date)
: m_id(0), m_dimension(dim), m_uiBitsPerVoxel(uiBitsPerVoxel), m_units(units),
m_unitType(unitType), m_bestCompression(hdr::LZO),
m_fileCompression(m_bestCompression), m_background(background),
m_extension(ext), m_sTitle(), m_sAuthor(), m_sFileName(sFileName),
m_headerMap(), m_uiMaxTitleLength(100), m_uiMaxAuthorLength(250),
m_creationDate(date)
{
  // check validity of user assigned values
  std::string sError = "Invalid parameters passed to volume constructor, assigning default values.";
/*
  if ( m_dimension.m_x < 2 || m_dimension.m_y < 2 || m_dimension.m_z < 2 )
  {
    LogWrn("Invalid parameters passed to volume constructor, assigning default values.","VolAPI<T>","VolAPI");
    m_dimension = Triple<uint4>(2,2,2);
  }
*/
  if ( m_units.m_x <= 0 || m_units.m_y <= 0 || m_units.m_z <= 0 )
  {
    LogWrn("Invalid parameters passed to volume constructor, assigning default values.","VolAPI<T>","VolAPI");
    m_units = Triple<float4>(1,1,1);
  }

  // make sure no units are zero
  if (m_units.m_x == 0.0F) m_units.m_x = 1.0F;
  if (m_units.m_y == 0.0F) m_units.m_y = 1.0F;
  if (m_units.m_z == 0.0F) m_units.m_z = 1.0F;
  
  // set up coordinate conversion matrices
  m_mVolumeToWorld.Identity();
  m_mWorldToVolume.Identity();
  m_mVolumeToWorld.Scale(Vector<float4>(m_units.m_x, m_units.m_y, m_units.m_z));
  m_mWorldToVolume.Scale(Vector<float4>(1.0F / m_units.m_x, 1.0F / m_units.m_y, 1.0F / m_units.m_z));
  
  VerifyHeader();
  SetTitle(sTitle);  // function call to check length
  SetAuthor(sAuthor);

  InitializeTypes();
} // constructor


/**
 * Copy Constructor.
 * @param rhs the input VolAPI.
 */
template <class T>
VolAPI<T>::VolAPI(const VolAPI<T> & rhs) : m_bestCompression(hdr::LZO),
m_uiMaxTitleLength(100), m_uiMaxAuthorLength(250) // initialize const types
{
  Copy(rhs);
} // copy constructor


/**
 * Assignment operator.
 * @param rhs the right hand VolAPI.
 * @return the result.
 */
template <class T>
VolAPI<T> const & VolAPI<T>::operator=(const VolAPI<T> & rhs)
{
  //check for auto-assignment using memory address
  if ( this == &rhs )
  {
    return(*this);
  }

  Destroy();
  Copy(rhs);

  return(*this);
} // Assignment operator


/**
 * Destructor.
 */
template <class T>
VolAPI<T>::~VolAPI()
{
  Destroy();
}


/**
 * Prints all file header variables to the ostream.
 * @param os the stream to send output.
 * @return the ostream passed in as an argument is returned.
 */
template <class T>
std::ostream & VolAPI<T>::AsOstream(std::ostream & os) const
{
  os << "DIMENSIONS: "<< m_dimension.m_x<< " "<<m_dimension.m_y<< " ";
  os << m_dimension.m_z<< " UNITS: "<< m_units.m_x<< " "<<m_units.m_y<< " ";
  os << m_units.m_z<< std::endl;
  os << "\" UNIT TYPE: \""<< UnitTypeStr[m_unitType];
  os << "\" EXTENSION: \""<< ExtensionStr[m_extension]<< "\""<< std::endl;
  os << "TITLE: \""<< m_sTitle<< "\" AUTHOR: \""<< m_sAuthor<< "\" ";
  os << "BITS PER VOXEL: "<< m_uiBitsPerVoxel << std::endl;
  os << m_creationDate.AsString();
  return(os);
}


/**
 * Gets the value returned when voxel access is attempted outside the volume.
 * @return const T &
 */
template <class T>
const T & VolAPI<T>::GetBackground() const
{
  return(m_background);
}


/**
 * When a volume is read initially
 * the compression is automatically set to the best available method so this
 * function should only be called if the user wishes to force the volume
 * to be stored with a certain compression type.
 * @param compressType the compression used when storing the volume on disk.
 */
template <class T>
void VolAPI<T>::SetCompression(FileCompressionEnum compressType)
{
  m_fileCompression = compressType;
}


/**
 * Returns the current compression, normally m_bestCompression. When a file is
 * read the compression type is always set to the value of m_bestCompression as
 * specified in VolAPI.
 * @return the current compression.
 */
template <class T>
hdr::FileCompressionEnum VolAPI<T>::GetCompression() const
{
  return(m_fileCompression);
}


/**
 * Returns the extension of the volume as read from the file header.
 * @return the extension of the volume.
 */
template <class T>
ExtensionEnum VolAPI<T>::GetExtension() const
{
  return(m_extension);
}


/**
 * The input std::string is
 * truncated if it exceeds the maximum allowed length.
 * @param sAuthor the author/source of the volume.
 */
template <class T>
void VolAPI<T>::SetAuthor(const std::string & sAuthor)
{
  if ( sAuthor.size() > m_uiMaxAuthorLength )
  {
    LogWrn("<VolAPI::SetAuthor> Author string truncated to max. length","VolAPI<T>","SetAuthor");
    m_sAuthor = sAuthor.substr(0, m_uiMaxAuthorLength);
  } else
  {
    m_sAuthor = sAuthor;
  }
}


/**
 * Returns the author/source string in the volume header.
 * @return the author/source string from the volume header.
 */
template <class T>
const std::string & VolAPI<T>::GetAuthor() const
{
  return(m_sAuthor);
}


/**
 * Sets the volume title. Input strings may to truncated to the max. length.
 * @param sTitle the title of the volume.
 */
template <class T>void VolAPI<T>::SetTitle(const std::string & sTitle)
{
  if ( sTitle.size() > m_uiMaxTitleLength )
  {
    LogWrn("<VolAPI::SetTitle> Title string truncated to max length","VolAPI<T>","SetTitle");
    m_sTitle = sTitle.substr(0, m_uiMaxTitleLength);
  } else
  {
    m_sTitle = sTitle;
  }
}


/**
 * Returns the title of the volume.
 * @return the title string of the volume.
 */
template <class T> const std::string & VolAPI<T>::GetTitle() const
{
  return(m_sTitle);
}


/**
 * Sets the volume creation data.
 * @param date on which the file is being created.
 */
template <class T>
inline void VolAPI<T>::SetCreationDate(vx::Date d)
{
  m_creationDate = d;
}


/**
 * Returns the volume creation date.
 * @return const hdr::Date &
 */
template <class T>
inline const vx::Date & VolAPI<T>::GetCreationDate() const
{
  return(m_creationDate);
}


/**
 * Returns the unitType of the stored volume such as METER, MICRON, etc.
 * @return the unit type of the stored volume from the UnitTypeEnum
 * enumeration.
 */
template <class T> UnitTypeEnum VolAPI<T>::GetUnitType() const
{
  return(VolAPI<T>::m_unitType);
}


/**
 * Returns the x,y,z voxel dimensions of the volume.
 * @return the dimensions of the volume.
 */
template <class T> const Triple<uint4> & VolAPI<T>::GetDim() const
{
  return(m_dimension);
}


/**
 * Returns the x,y,z units of the volume.
 * @return the x,y,z distances from one voxel to the next.
 */
template <class T> const Triple<float4> & VolAPI<T>::GetUnits() const
{
  return(m_units);
}


/**
 * Sets the units for the volume.
 * @param the new units.
 */
template <class T> void VolAPI<T>::SetUnits(const Triple<float4> & units)
{
  m_units = units;

  // make sure no units are zero
  if (m_units.m_x == 0.0F) m_units.m_x = 1.0F;
  if (m_units.m_y == 0.0F) m_units.m_y = 1.0F;
  if (m_units.m_z == 0.0F) m_units.m_z = 1.0F;
  
  // set up coordinate conversion matrices
  m_mVolumeToWorld.Identity();
  m_mWorldToVolume.Identity();
  m_mVolumeToWorld.Scale(Vector<float4>(m_units.m_x, m_units.m_y, m_units.m_z));
  m_mWorldToVolume.Scale(Vector<float4>(1.0F / m_units.m_x, 1.0F / m_units.m_y, 1.0F / m_units.m_z));
}


/**
 * Returns the bits per voxel.
 * @return the number of bits per voxel.
 */
template <class T> const uint4 & VolAPI<T>::GetBitsPerVoxel()  const
{
  return(m_uiBitsPerVoxel);
}


/**
 * Returns the bytes per voxel.
 * @return the number of bytes needed to store one voxel.
 */
template <class T> const uint4 VolAPI<T>::GetBytesPerVoxel()  const
{
  return(((m_uiBitsPerVoxel-1) / 8) + 1);
}


/**
 * Returns the Filename string.
 * @return sFilename string with extension
 */
template <class T> const std::string & VolAPI<T>::GetFileName() const
{
  return(m_sFileName);
}


/**
 * Returns the Filename string without the extension.
 * @return filename string without the extension
 */
template <class T> std::string VolAPI<T>::GetFileNameRoot() const
{
  std::string sTok(".");
  std::string sTemp = m_sFileName.substr(0,m_sFileName.find_last_of(sTok));
  return(sTemp);
}


/**
 * Sets the volume file name.
 * @param fileName the new file name.
 */
template <class T> void VolAPI<T>::SetFileName(const std::string & sFileName)
{
  m_sFileName = sFileName;
}


/**
 * Returns a matrix useful for converting a point or vector
 * from volume space into world space.
 * @return a floating point matrix encapsulating the coordinate conversion
 */
template <class T>
const Matrix<float4> & VolAPI<T>::GetVolumeToWorldMatrix() const
{
  return m_mVolumeToWorld;
} // GetVolumeToWorldMatrix()


/**
 * Returns a matrix useful for converting a point or vector
 * from world space into volume space.
 * @return a floating point matrix encapsulating the coordinate conversion
 */
template <class T>
const Matrix<float4> & VolAPI<T>::GetWorldToVolumeMatrix() const
{
  return m_mWorldToVolume;
} // GetWorldToVolumeMatrix()


/**
 * Returns true if the point is within the volume.
 * @param p the point in the volume.
 * @return true if the point is within the volume.
 */
template <class T>
bool VolAPI<T>::InsideVolume(const Point<uint2> & p) const
{
  if ( p.m_x >= m_dimension.m_x ||
       p.m_y >= m_dimension.m_y ||
       p.m_z >= m_dimension.m_z )
  {
    return(false);
  }
  return(true);
}


/**
 * Reports the number of voxels.
 * @return the number of voxels.
 */
template <class T>
const uint4 VolAPI<T>::GetNumberOfVoxels() const
{
  return(m_dimension.m_x * m_dimension.m_y * m_dimension.m_z);
}


/**
 * Copies all volume data and fields.
 * @param rhs the right hand.
 * @return the result.
 */
template <class T>
void VolAPI<T>::Copy(VolAPI<T> const & rhs)
{
  m_id = rhs.m_id;
  m_dimension = rhs.m_dimension;
  m_units = rhs.m_units;
  m_unitType = rhs.m_unitType;
  m_mVolumeToWorld = rhs.m_mVolumeToWorld;
  m_mWorldToVolume = rhs.m_mWorldToVolume;
  m_fileCompression = rhs.m_fileCompression;
  m_extension = rhs.m_extension;
  m_sTitle = rhs.m_sTitle; m_sAuthor = rhs.m_sAuthor;
  m_uiBitsPerVoxel = rhs.m_uiBitsPerVoxel;
  m_sFileName = rhs.m_sFileName;
  m_headerMap = rhs.m_headerMap;
  m_background = rhs.m_background;
  m_creationDate = rhs.m_creationDate;

  UnitTypeStr = rhs.UnitTypeStr;
  ExtensionStr = rhs.ExtensionStr;
  MemoryLayoutStr = rhs.MemoryLayoutStr;
} // Copy


/**
 * Deletes all VolAPI ptrs.
 */
template <class T>
void VolAPI<T>::Destroy()
{
}


/**
 * Builds the ascii header for a volume according to the
 * "Viatronix File Specification Document"
 * @param ml the memory layout.
 * @param ext the file extension.
 * @return string the ascii header for a volume
 */
template <class T>
const std::string VolAPI<T>::BuildAsciiHeader(MemoryLayoutEnum ml, ExtensionEnum ext)
{
  T learnType = 0;
  std::string sDataType = typeid(learnType).name();
  // dataType.replace(0, ' ','-'); ??
  for ( uint4 j = 0; j < sDataType.length(); ++j )
  {
    if ( sDataType[j] == ' ' ) sDataType[j] = '-';
  }

  std::ostrstream os;

  os << "\r\n";
  os << ext << " # Extension - "<< ExtensionStr[ext]<< "\r\n";
  os << m_sTitle << " # Title \r\n";
  os << m_sAuthor<< " # Author/Source \r\n";
  os << m_dimension.m_x<< " "<<m_dimension.m_y<< " "<< m_dimension.m_z<< " # Volume Dimensions \r\n";
  os << m_units.m_x<< " "<< m_units.m_y<< " "<< m_units.m_z<< " # Voxel Units \r\n";
  if ( typeid(m_background) == typeid(int1) || typeid(m_background) == typeid(uint1) )
  {
    // must cast to avoid printing as wrong ASCII char
    os << (int4) m_background << " # Background value for non-specified voxels \r\n";
  } else
  {
    os << m_background << " # Background value for non-specified voxels \r\n";
  }
  os << m_unitType<< " # Unit Type - "<< UnitTypeStr[m_unitType] << "\r\n";
  os << sDataType<< " "<<m_uiBitsPerVoxel<< " # Data Type, Bits Per Voxel "<< "\r\n";
  os << ml << " # Memory Storage Order - "<< MemoryLayoutStr[ml] << "\r\n";
  os << m_creationDate.AsString() << " # Date of Creation \r\n";

  os<<std::ends;
  std::string result(os.str());

  return(result);
} // BuildAscii


/**
 * Reads and assigns the common header fields, then assigns the
 * SpecificHeader pointer and calls that ReadSpecificHeader() function to assign
 * all variables unique to the extension.
 * @param headerPtr the "vOL" ascii header.
 * @return the memory layout from the header.
 */
template <class T>
MemoryLayoutEnum VolAPI<T>::ReadCommonHeader(const std::string & headerPtr)
{
  // note to self -- Update ReadHeader() in HeaderVolume concurrently

  std::istrstream header(headerPtr.c_str());
  const int4 bSize = 400;
  int1 buffer[bSize];
  int4 iDelimiter = '#';

  uint4 extension, unitType, memoryLayout;
  std::string sDataType;

  header.getline(buffer, bSize);
  header >> extension; header.getline(buffer, bSize);
  header.getline(buffer, bSize, iDelimiter);
  m_sTitle = buffer;
  header.getline(buffer, bSize); //read rest of line
  header.getline(buffer, bSize, iDelimiter);
  m_sAuthor = buffer;
  header.getline(buffer, bSize); //read rest of line
  header >> m_dimension.m_x >> m_dimension.m_y >> m_dimension.m_z;
  header.getline(buffer, bSize); //read rest of line
  header >> m_units.m_x >> m_units.m_y >> m_units.m_z;
  header.getline(buffer, bSize); //read rest of line
  if ( typeid(int1) == typeid(m_background) || typeid(uint1) == typeid(m_background) )
  {
    int4 back;
    header >> back; header.getline(buffer, bSize);
    m_background = static_cast<T>(back);
  } else
  {
    header >> m_background; header.getline(buffer, bSize);
  }
  header >> unitType; header.getline(buffer, bSize);

  header >> sDataType >> m_uiBitsPerVoxel; header.getline(buffer, bSize);
  header >> memoryLayout; header.getline(buffer, bSize);

  header.getline(buffer, bSize, iDelimiter);
  vx::Date dt(buffer);
  m_creationDate = dt;
  header.getline(buffer, bSize); // get rest of line

  m_extension = (ExtensionEnum) extension;
  m_unitType = (UnitTypeEnum) unitType;

  for ( uint4 j = 0; j < sDataType.length(); ++j )
  {
    if ( sDataType[j] == '-' ) sDataType[j] = ' ';
  }

  // make sure no units are zero
  if (m_units.m_x == 0.0F) m_units.m_x = 1.0F;
  if (m_units.m_y == 0.0F) m_units.m_y = 1.0F;
  if (m_units.m_z == 0.0F) m_units.m_z = 1.0F;
  
  // set up coordinate conversion matrices
  m_mVolumeToWorld.Identity();
  m_mWorldToVolume.Identity();
  m_mVolumeToWorld.Scale(Vector<float4>(m_units.m_x, m_units.m_y, m_units.m_z));
  m_mWorldToVolume.Scale(Vector<float4>(1.0F / m_units.m_x, 1.0F / m_units.m_y, 1.0F / m_units.m_z));
  
  VerifyHeader(sDataType);

  return((MemoryLayoutEnum) memoryLayout);
} // ReadCommonHeader


/**
 * Checks if the data read in are in the acceptable range.
 */
template <class T>
void VolAPI<T>::VerifyHeader(const std::string & sDataType)
{
  if ( sDataType != "NULL" )
  {
    T learnType = 0;
    std::string sRealDataType = typeid(learnType).name();
    if ( sRealDataType != sDataType )
    {
      // no easy way to check in cross-platform way if type of volume read matches type T
      // for example Linux typeid().name() != Windows typeid().name() for the same types
      
      // can place wrapper around typeinfo to correct this w/out much work
    }
  }

  CheckBitsPerVoxel();

  if ( (m_extension < VOL) || (m_extension > UNKNOWN_EXT) )
  {
    LogWrn("<VolAPI::VerifyHeader> Invalid File Extension. Assigning default type","VolAPI<T>","VerifyHeader");
    m_extension = UNKNOWN_EXT;
  }
  if ( (m_unitType < METER) || (m_unitType > INCH) )
  {
    LogWrn("Unit type unexpected. Assigning default type ","VolAPI<T>","VerifyHeader");
    m_unitType = INCH;
  }
} // VerifyHeader


/**
 *
 */
template <class T>
void VolAPI<T>::CheckBitsPerVoxel()
{
  uint4 uiDataSize = sizeof(T);

  if ( uiDataSize < GetBytesPerVoxel() )
  {
    LogWrn("<VolAPI::CheckBitsPerVoxel> Bits per voxel must correspond to data type size","VolAPI<T>","CheckBitsPerVoxel");
    m_uiBitsPerVoxel = uiDataSize * 8;
  }
} // CheckBitsPerVoxel


/**
 * Converts a suffix to the corresponding ExtensionEnum value.
 * @param suffix the suffix in string format.
 * @return the enum corresponding to the suffix.
 */
template <class T>
ExtensionEnum VolAPI<T>::GetExtension(const std::string & suffix)
{
  for ( int4 i = 0; i < 8; i++ )
  {
#ifdef _WIN32
    if ( _stricmp(suffix.c_str(), ExtensionStr[i].c_str()) == 0 )
    {
#else
    if ( strcasecmp(suffix.c_str(), ExtensionStr[i].c_str()) == 0 )
    {
#endif
      return(ExtensionEnum) i;
    }
  }
  return(UNKNOWN_EXT);
} // GetExtension


/**
 * Initializes the vectors containing the File, Data, Layout, Extension, and Unit types.
 */
template <class T>
void VolAPI<T>::InitializeTypes()
{
  uint4 count = 0;

  const int1 * const cstrUnitTypeStr[NUM_OF_UNIT_TYPE_ENUM] = UNIT_TYPE_STRINGS;
  UnitTypeStr.resize(NUM_OF_UNIT_TYPE_ENUM);
  for ( count = 0; count < NUM_OF_UNIT_TYPE_ENUM; count++ )
    UnitTypeStr[count] = cstrUnitTypeStr[count];

  const int1 * const cstrExtensionStr[NUM_OF_EXTENSION_ENUM] = EXTENSION_STRINGS; 
  ExtensionStr.resize(NUM_OF_EXTENSION_ENUM);
  for ( count = 0; count < NUM_OF_EXTENSION_ENUM; count++ )
    ExtensionStr[count] = cstrExtensionStr[count];

  const int1 * const cstrMemoryLayoutStr[NUM_OF_MEMORY_LAYOUT_ENUM] = MEMORY_LAYOUT_STRINGS;
  MemoryLayoutStr.resize(NUM_OF_MEMORY_LAYOUT_ENUM);
  for ( count = 0; count < NUM_OF_MEMORY_LAYOUT_ENUM; count++ )
    MemoryLayoutStr[count] = cstrMemoryLayoutStr[count];

} // InitializeTypes

#undef FILE_REVISION

// $Log: VolAPI.inl,v $
// Revision 1.2  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.4  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.3.2.2  2003/08/19 17:38:32  mkontak
// Added vx:: to Date
//
// Revision 3.3.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.3  2003/05/16 11:54:10  michael
// code review
//
// Revision 3.2  2002/07/09 15:09:53  mkontak
// Removed demension check in constructor since the NM study only contains
// one slice.
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:08   ingmar
// Initial revision.
// Revision 1.5  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:04:58  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:09  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.46  2001/08/31 18:53:41  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.45  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.44  2001/06/25 21:23:56  geconomos
// moved from 51_applicationModel
//
// Revision 1.43  2001/05/03 15:43:58  dave
// removed pragma in ReadHeader
//
// Revision 1.42  2001/04/28 01:21:37  ingmar
// simplified bits ber voxel computation
//
// Revision 1.41  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.40  2001/04/16 12:53:04  frank
// Added permanent volume coordinate conversion matrix.
//
// Revision 1.39  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.38  2001/03/31 19:04:53  ingmar
// oops (removed this->)
//
// Revision 1.37  2001/03/31 18:24:50  ingmar
// improved documentation and fixed addition of .SLC extension
//
// Revision 1.36  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.35  2001/03/23 00:47:33  dave
// towards a more Viatronix standards compliant version
//
// Revision 1.34  2001/03/20 12:40:52  frank
// The use of reinterpret_cast is contraindicated.
//
// Revision 1.33  2001/03/19 18:55:21  dongqing
// Updated calls to Date class methods w/ casting
//
// Revision 1.32  2000/12/12 18:34:13  dave
// corrected VolAPI constructor
//
// Revision 1.31  2000/12/12 15:55:50  dave
// removed SetBackground()
//
// Revision 1.30  2000/12/11 21:38:40  dave
// new libpng, exception, file versions etc
//
// Revision 1.29  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.28  2000/11/21 23:43:57  ingmar
// disabled preformance warning
//
// Revision 1.27  2000/11/15 05:14:05  dave
// revision of Date class
//
// Revision 1.26  2000/11/14 03:57:38  dave
// saves m_back state on read/write
//
// Revision 1.25  2000/11/07 06:33:21  dave
// corrected hdrMap reading error
//
// Revision 1.24  2000/11/05 19:51:23  dave
// reads correctly again
//
// Revision 1.23  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.22  2000/11/03 15:50:12  ingmar
// GetNumberOfVoxels now returns uint
//
// Revision 1.21  2000/11/03 03:14:07  ingmar
// fixed some gcc syntax errors and warnings
//
// Revision 1.20  2000/11/02 19:48:09  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.19  2000/11/02 16:03:47  dave
// allow to read old format vols
//
// Revision 1.18  2000/11/02 04:17:49  dave
// misc updates
//
// Revision 1.17  2000/11/01 21:22:52  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.16  2000/11/01 17:44:26  dave
// removed GetDataType() from Volume interface
//
// Revision 1.15  2000/10/31 21:16:42  dave
// changed VOLAPI to use ZLIB
//
// Revision 1.13  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.12  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.11  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.10  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.9  2000/10/22 00:24:23  dave
// better iterators
//
// Revision 1.8  2000/10/18 02:59:49  antonio
// fixed minor problems in skeleton stage
//
// Revision 1.7  2000/09/21 06:37:37  dave
// fixed specific header in volume conversion
//
// Revision 1.6  2000/09/21 06:01:23  dave
// replaced map with vector
//
// Revision 1.5  2000/09/21 03:35:51  dave
// searching for segfaults
//
// Revision 1.4  2000/09/17 02:16:53  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.3  2000/09/11 22:46:09  dave
// added date field to constructors
//
// Revision 1.2  2000/09/11 04:12:01  antonio
// got rid of a few more warnings
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 29    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 28    9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 27    8/31/00 1:08p Dave
// added SetFileName
//
// 26    8/31/00 11:19a Dave
// some gcc warnings okay
//
// 25    8/30/00 2:42p Dave
// added SetUnits()
//
// 24    8/21/00 1:55a Sarang
// updated header
//
// *****************  Version 23  *****************
// User: Ingmar       Date: 8/20/00    Time: 11:53p
// Updated in $/vx/src/Volume
// added missing hdr and std
//
// *****************  Version 22  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Volume
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// *****************  Version 21  *****************
// User: Ingmar       Date: 8/19/00    Time: 3:16p
// Updated in $/v2k/src/Volume
// GetVoxel/GetVoxelRaw + inline Error::DebugMsg
//
// *****************  Version 20  *****************
// User: Jay          Date: 8/17/00    Time: 10:12p
// Updated in $/v2k/src/Volume
//
// *****************  Version 19  *****************
// User: Jay          Date: 8/17/00    Time: 1:51p
// Updated in $/v2k/src/Volume
//
// *****************  Version 18  *****************
// User: Jay          Date: 8/17/00    Time: 1:42p
// Updated in $/v2k/src/Volume
// Date field tested to work
//
// *****************  Version 17  *****************
// User: Jay          Date: 8/17/00    Time: 1:03p
// Updated in $/v2k/src/Volume
// Date string reformatted
//
// *****************  Version 16  *****************
// User: Dave         Date: 8/17/00    Time: 10:39a
// Updated in $/v2k/src/Volume
//
// *****************  Version 15  *****************
// User: Jay          Date: 8/17/00    Time: 5:16a
// Updated in $/v2k/src/Volume
// CopySpecificHeader added in VolAPI.
//
// *****************  Version 14  *****************
// User: Jay          Date: 8/17/00    Time: 2:27a
// Updated in $/v2k/src/Volume
// Added header for Creation Date
//
// *****************  Version 13  *****************
// User: Jay          Date: 8/16/00    Time: 6:13p
// Updated in $/v2k/src/Volume
//
// *****************  Version 12  *****************
// User: Jay          Date: 8/16/00    Time: 5:49p
// Updated in $/v2k/src/Volume
//
// *****************  Version 11  *****************
// User: Jay          Date: 8/16/00    Time: 4:41p
// Updated in $/v2k/src/Volume
//
// *****************  Version 10  *****************
// User: Jay          Date: 8/16/00    Time: 1:35p
// Updated in $/v2k/src/Volume
// Overloaded constructor in Volume to enable creating HashVolumes with
// various hash bits
//
// *****************  Version 9  *****************
// User: Jay          Date: 8/16/00    Time: 1:56a
// Updated in $/v2k/src/Volume
// SpeceficHeaderIns.h/C added
//
// *****************  Version 8  *****************
// User: Antonio      Date: 8/11/00    Time: 1:38p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 7  *****************
// User: Jay             Date:  8/11/00  Time:  1:35a
// Checked in $/v2k/src/Volume
// Comment:
//
//
// *****************  Version 6  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 5  *****************
// User: Jay             Date:  8/08/00  Time: 12:00a
// Checked in $/vx/src/Volume
// Comment:
//   Cleaned up
//
// *****************  Version 4  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 3  *****************
// User: Dave            Date:  7/27/00  Time:  1:17p
// Checked in $/v2k/src/Volume
// Comment:
//   All functions that should be in .inl are now there
//
// *****************  Version 2  *****************
// User: Jeff            Date:  7/26/00  Time:  8:14p
// Branched at version 2
// Comment:
//
//
// *****************  Version 1  *****************
// User: Dave            Date:  7/26/00  Time:  8:08p
// Created VolAPI.inl
// Comment:
//   Movin' to inline in VolAPI
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/VolAPI.inl,v 1.2 2006/02/03 15:22:11 frank Exp $
// $Id: VolAPI.inl,v 1.2 2006/02/03 15:22:11 frank Exp $
