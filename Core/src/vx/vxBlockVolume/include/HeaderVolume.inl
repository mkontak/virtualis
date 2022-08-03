// $Id: HeaderVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Created: 
// Author: David dave@cs.sunysb.edu
//
// Complete history on bottom of file

#define FILE_REVISION "$Revision: 1.1.1.1 $"

template <class T>
HeaderVolume<T>::HeaderVolume() : m_sTypeName("blank"), m_iLayout(0)
{
}


/**
 * Reads using the new V3D file format.
 */
template <class T>
void HeaderVolume<T>::Read(std::string sFileName)
{
  if ( sFileName.size() <= 4 ) 
  {
    throw ex::IOException(LogRec("Filename: " + sFileName + " is suspiciously short, not reading in.","HeadVolume::T","Read"));
  }
  
  V3DFile img;
  try
  {
    img.SetSkipImage(true);
    img.SetSkipData(true);
    img.Read(sFileName, std::make_pair( 0.0F, 100.0F ) );
  }
  catch (ex::VException & ) // include FileNotFoundException/DataCorruptionException/IOException
  {
    LogErr("Exception occurred during reading header information.","HeadVolume::T","Read");
    throw;// ex::VException(LogRec("Exception occurred during reading header information.","HeadVolume<T>","Read"));
  }
  catch (...)
  {
    throw ex::VException(LogRec("Unhandled Exception occurred during reading header information.","HeadVolume::T","Read"));
  }
  m_sFileName = sFileName;
  try
  {
    std::string header = img.GetDescription();  
    m_iLayout = ReadHeader(header);
    std::string extraVars = img.GetComment();
    // convert string back to map
    m_headerMap = VarMap(extraVars);
  }
  catch (...)
  {
    throw ex::VException(LogRec("Unhandled Exception occurred during reading header information.","HeadVolume::T","Read"));
  } // catch all other exceptions.
} // Read()


/**
 * Clear out all the information.
 */
template <class T>
void HeaderVolume<T>::Clear()
{
  *this = HeaderVolume<T>();
}


/// Returns the name of the stored volume type
template<class T>
const std::string & HeaderVolume<T>::GetDataType()
{
  return m_sTypeName;
}


// Returns the name of the stored volume type
template<class T>
int4 HeaderVolume<T>::GetDiskLayout()
{
  return m_iLayout;
}


/// @see other fun
template <class T>
hdr::MemoryLayoutEnum HeaderVolume<T>::ReadHeader(const std::string & headerPtr)
{
  std::istrstream header(headerPtr.c_str());
  const int4 bSize = 400;
  int1 buffer[bSize];
  int4 delimiter = '#';

  uint4 extension, unitType, memoryLayout;
  std::string dataType;

  header.getline(buffer, bSize);
  header >> extension; header.getline(buffer, bSize);
  header.getline(buffer, bSize, delimiter);
  m_sTitle = buffer;
  header.getline(buffer, bSize); //read rest of line
  header.getline(buffer, bSize, delimiter);
  m_sAuthor = buffer;
  header.getline(buffer, bSize); //read rest of line
  header >> m_dimension.m_x >> m_dimension.m_y >> m_dimension.m_z;
  header.getline(buffer, bSize); //read rest of line
  header >> m_units.m_x >> m_units.m_y >> m_units.m_z;
  header.getline(buffer, bSize); //read rest of line
  if(typeid(int1) == typeid(m_background) || typeid(uint1) == typeid(m_background)) 
  {
    int4 back;
    header >> back; header.getline(buffer, bSize);
    m_background = static_cast<T>(back);
  }
  else 
  {
    header >> m_background; header.getline(buffer, bSize);
  }
  header >> unitType; header.getline(buffer, bSize);
  
  header >> dataType >> m_uiBitsPerVoxel; header.getline(buffer, bSize);
  header >> memoryLayout; header.getline(buffer, bSize);

  header.getline(buffer,bSize,delimiter);
  vx::Date dt(buffer);
  m_creationDate = dt;
  header.getline(buffer, bSize); // get rest of line

  m_extension = (hdr::ExtensionEnum) extension;
  m_unitType = (hdr::UnitTypeEnum) unitType;

  for(uint4 j = 0; j < dataType.length(); ++j)
  {
    if(dataType[j] == '-') dataType[j] = ' ';
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
  
  //VerifyHeader(dataType);
  m_sTypeName = dataType;

  return((hdr::MemoryLayoutEnum) memoryLayout);
} // ReadHeader



template <class T>
void HeaderVolume<T>::CheckBitsPerVoxel()
{
  // do nothing here.
}


template <class T>
HeaderVolume<T>::HeaderVolume(const Triple<uint4> & dim, 
                              const uint4 & uiBitsPerVoxel, const Triple<float4> & units,
                              hdr::UnitTypeEnum unitType,
                              const T & background,
                              hdr::ExtensionEnum ext,
                              const std::string & sFileName,
                              const std::string & sTitle,
                              const std::string & sAuthor,
                              const vx::Date & date)
{
}


template <class T>
void HeaderVolume<T>::Write(std::string sFileName)
{
  return 1;
}


template <class T>
const uint4 HeaderVolume<T>::GetMemoryUsage() const
{
  return 1;
}

template <class T>
void HeaderVolume<T>::Stats(std::ostream & os) const
{
}

#undef FILE_REVISION

// $Log: HeaderVolume.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.7  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.6.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.6  2003/05/17 15:49:14  michael
// code review
//
// Revision 3.5  2003/04/18 15:59:54  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 3.4  2002/11/26 00:36:44  kevin
// Don't change the exception that is re-thrown in a "catch"
//
// Revision 3.3  2001/10/30 15:26:25  soeren
// Removed some unnecessary lines
//
// Revision 3.2  2001/10/30 15:25:37  soeren
// Removed some unnecessary lines
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
//    Rev 2.0   Sep 16 2001 23:47:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:02   ingmar
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
// Revision 1.2  2001/09/07 17:59:08  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.40  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.39  2001/08/07 17:03:39  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.38  2001/07/24 23:35:54  tushar
// fixed some compiler errors (logger)
//
// Revision 1.37  2001/07/24 23:13:34  tushar
// fixed a byg
//
// Revision 1.36  2001/07/18 20:53:17  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.35  2001/07/16 23:23:26  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.34  2001/07/13 19:51:43  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.33  2001/07/12 21:14:47  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.32  2001/07/12 16:23:54  frank
// Skipped reading icon data in header read.
//
// Revision 1.31  2001/07/09 16:52:05  frank
// Added support for the new V3D file format.
//
// Revision 1.30  2001/05/03 15:43:59  dave
// removed pragma in ReadHeader
//
// Revision 1.29  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.28  2001/04/16 12:53:04  frank
// Added permanent volume coordinate conversion matrix.
//
// Revision 1.27  2001/04/12 18:54:06  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.26  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.25  2001/04/10 16:13:26  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.24  2001/04/09 18:03:51  dmitry
// Removed Revision History from Top of File
//
// Revision 1.23  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.22  2001/03/31 05:34:38  ingmar
// uint8 -> uint4
//
// Revision 1.21  2001/03/30 18:50:04  dmitry
// Updated datatypes.
//
// Revision 1.20  2001/03/20 12:40:55  frank
// The use of reinterpret_cast is contraindicated.
//
// Revision 1.19  2001/03/19 18:55:21  dongqing
// Updated calls to Date class methods w/ casting
//
// Revision 1.18  2001/02/06 02:25:11  dave
// re-organized linVol Read for memory savings
//
// Revision 1.17  2001/01/08 19:23:05  dave
// does not read unknown chunks
//
// Revision 1.16  2000/12/22 15:52:00  frank
// Added function to clear out old data.
//
// Revision 1.15  2000/12/11 21:38:40  dave
// new libpng, exception, file versions etc
//
// $Id: HeaderVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
