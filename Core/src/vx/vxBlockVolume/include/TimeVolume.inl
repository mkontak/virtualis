// $Id: TimeVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete history at bottom of file.

#include "V3DFile.h"

// declare all explicit specializations

#define FILE_REVISION "$Revision: 1.1.1.1 $"

/**
* Default constructor for an empty volume.
*/
template <class T>
TimeVolume<T>::TimeVolume()
{
} // TimeVolume()


/**
 * Destructor.
 */
template <class T>
TimeVolume<T>::~TimeVolume()
{
} // ~TimeVolume()


/**
 * Reads the volume specified into memory using the new V3D File format.
 * @param sFileName the name of the volume to read.
 */
template <class T>
void TimeVolume<T>::Read(const std::string & sFileName)
{
  std::stringstream tt;

  try
  {
    // read the header variables
    V3DFile img;
    img.Read(sFileName);
    // could check version number now
    // std::pair<int,int> version = img.GetFileVersion();
    VarMap headerMap(img.GetDescription());
    bool bResult = headerMap.GetHdrVar("filePrefix", m_sFilePrefix);
    if (bResult == false)
    {
      throw ex::VException(LogRec("file read error: header variable (filePrefix) not found", "TimeVolume<T>", "Read"));
    }
    uint4 uNumPhases(0);
    bResult = headerMap.GetHdrVar("numPhases", uNumPhases);
    if (bResult == false)
    {
      throw ex::VException(LogRec("file read error: header variable (numPhases) not found", "TimeVolume<T>", "Read"));
    }
    const uint4 uActualSize(img.GetDataBuffer().GetSize());
    const uint4 uExpectedSize(uNumPhases * sizeof(float4));
    if (uNumPhases == 0 || uActualSize != uExpectedSize)
    {
      throw ex::VException(LogRec("file read error: header variable (numPhases) is zero", "TimeVolume<T>", "Read"));
    }

    // try reading all of the times from the data
    float4 * pTimes = reinterpret_cast<float4 *>(img.GetDataBuffer().GetDataPtr());
    if (pTimes == NULL)
    {
      throw ex::VException(LogRec("file read error: data is empty", "TimeVolume<T>", "Read"));
    }
    for (uint4 i(0); i<uNumPhases; ++i)
    {
      m_timeVector.push_back(*pTimes++);
    }

    // try reading the volumes from disk
    for (i=0; i<uNumPhases; ++i)
    {
      // create filename
      std::string sFileName = m_sFilePrefix + ToAscii(i) + ".vol";

      // read data and save in vector
      T * pVolume = new T;
      pVolume->Read(sFileName);
      m_volumeVector.push_back(pVolume);
    }
    
  } // end-try
  catch ( ex::FileNotFoundException )
  {
    throw ex::VException(LogRec("file read error: can not found", "TimeVolume<T>", "Read"));
  }
  
  catch ( ... )
  {
    throw ex::VException(LogRec("file read error: can not found""error loading data files", "TimeVolume<T>", "Read"));
  }

  return;
} // Read()


/**
 * Writes a file using the V3D File format.
 * If no file name is specified the same
 * name is used from the call to Read().  The filename specified should end
 * with a valid hdr::ExtensionEnum from that enumeration.
 * @param sFileName the file name to write, default value is the same as the name
 * of the input file.
 * @param buildIcon by default a new icon is rendered unless this value is set
 * to false.
 */
template <class T>
void TimeVolume<T>::Write(const std::string & sFileName)
{
  try
  {
    if (m_timeVector.size() == 0)
    {
      throw ex::VException(LogRec("file write error: empty dataset", "TimeVolume<T>", "Write"));
    }
    
    // write out the meta data file
    try
    {
      V3DFile img;
      VarMap headerVars;
      headerVars.SetHdrVar("filePrefix", m_sFilePrefix, "the prefix of the volume dataset files");
      headerVars.SetHdrVar("numPhases", m_timeVector.size(), "the size of the time dimension");
      img.SetDescription(headerVars.ToString());
      img.SetTitle("no title given");
      img.SetComment("no comment given");
      img.SetData(MemoryBuffer((uint1 *)(&m_timeVector[0]), m_timeVector.size() * sizeof(float4)));
      img.Write(sFileName);
    }
    catch (...)
    {
      throw ex::VException(LogRec("file write error: img scope", "TimeVolume<T>", "Write"));
    }
    
    // write out the volume files
    for (uint4 i(0); i<m_volumeVector.size(); ++i)
    {
      // create filename
      std::string sFileName = m_sFilePrefix + ToAscii(i) + ".vol";
      
      // read data and save in vector
      m_volumeVector[i]->Write(sFileName);
    }
  }
  catch (...)
  {
    throw ex::VException(LogRec("file write error: unknown", "TimeVolume<T>", "Write"));
  }
  
  return;
} // Write()


/**
 * Set the value at the location specified.
 * @param location const Point<ushort> &
 * @param value the new value
 */
template <class T>
void TimeVolume<T>::SetVoxel(const Quad<uint2> & location, T value)
{
  if (location.m_t > m_volumeVector.size())
  {
    throw ex::InvalidDataException(LogRec("time out of range: " + ToAscii(location.m_t), "TimeVolume<T>", "SetVoxel"));
  }
  
  return m_volumeVector[location.m_t]->GetVoxel(Point<uint2>(location.m_x, location.m_y, location.m_z));
}


/**
 * Gets the voxel at the specified location.  No bounds checking!
 * @param location the location in the volume to get the voxel.
 * @return the value at the location specified.
 */
template <class T>
const int2 & TimeVolume<T>::GetVoxel(const Quad<uint2> & location) const
{
  if (location.m_t > m_volumeVector.size())
  {
    throw ex::InvalidDataException(LogRec("time out of range: " + ToAscii(location.m_t), "TimeVolume<T>", "GetVoxel"));
  }

  return m_volumeVector[location.m_t]->GetVoxel(Point<uint2>(location.m_x, location.m_y, location.m_z));
}


/**
 * Retrieve a volume from the list.
 */
template <class T>
inline T & TimeVolume<T>::GetVolume(const uint2 & uTime)
{
  if (uTime > m_volumeVector.size())
  {
    throw ex::InvalidDataException(LogRec("time out of range: " + ToAscii(uTime), "TimeVolume<T>", "GetVolume"));
  }
  
  return *(m_volumeVector[uTime]);
} // GetVolume()


/**
 * Set a volume in the list.
 */
template <class T>
inline void TimeVolume<T>::SetVolume(const uint2 & uTimeIndex, T & inVol)
{
  if (uTimeIndex > m_timeVector.size())
  {
    throw ex::InvalidDataException(LogRec("time out of range: " + ToAscii(uTimeIndex), "TimeVolume<T>", "SetVolume"));
  }
  
  m_volumeVector[uTimeIndex] = &inVol;
} // SetVolume()


/**
 * Set the time (in seconds) of a volume in the list.
 */
template <class T>
inline void TimeVolume<T>::SetVolumeTimeSeconds(const uint2 & uTimeIndex, const float4 & fTimeValueSeconds)
{
  if (uTimeIndex > m_timeVector.size())
  {
    throw ex::InvalidDataException(LogRec("time out of range: " + ToAscii(uTimeIndex), "TimeVolume<T>", "SetVolumeTimeSeconds"));
  }
  
  m_timeVector[uTimeIndex] = fTimeValueSeconds;
} // SetVolumeTimeSeconds()


/**
 * Get the time (in seconds) of a volume in the list.
 */
template <class T>
inline const float4 & TimeVolume<T>::GetVolumeTimeSeconds(const uint2 & uTimeIndex) const
{
  if (uTimeIndex > m_timeVector.size())
  {
    throw ex::InvalidDataException(LogRec("time out of range: " + ToAscii(uTimeIndex), "TimeVolume<T>", "GetVolumeTimeSeconds"));
  }
  
  return m_timeVector[uTimeIndex];
} // GetVolumeTimeSeconds()


/**
 * Sets the number of volumes to store.
 */
template <class T>
inline void TimeVolume<T>::SetTimeDimension(const uint2 & uTimeDimension)
{
  m_timeVector.resize(uTimeDimension);
  m_volumeVector.resize(uTimeDimension);
} // SetTimeDimension()


/**
 * Gets the number of volumes in the time dimension.
 */
template <class T>
inline const uint2 & TimeVolume<T>::GetTimeDimension() const
{
  return m_timeVector.size();
} // GetTimeDimension()


/**
 * Test all the functionality of the class.
 */
template <class T>
bool TimeVolume<T>::Test()
{
  try
  {
    const std::string sFilePath("test.mpv");
    TimeVolume<vol::LinVolInt2> timeVol;
    timeVol.SetTimeDimension(3);
    timeVol.SetVolumeTimeSeconds(0, 0.0F);
    timeVol.SetVolumeTimeSeconds(1, 1.1F);
    timeVol.SetVolumeTimeSeconds(2, 2.2F);
    vol::LinVolInt2 sampleVol0, sampleVol1, sampleVol2;
    timeVol.SetVolume(0, sampleVol0);
    timeVol.SetVolume(1, sampleVol1);
    timeVol.SetVolume(2, sampleVol2);
    timeVol.Write(sFilePath);
    
    TimeVolume<vol::LinVolInt2> readTimeVol;
    readTimeVol.Read(sFilePath);
  }
  catch (...)
  {
    return false;
  }

  return true;
} // Test()


#undef FILE_REVISION

// $Log: TimeVolume.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/16 11:53:31  michael
// code review
//
// Revision 1.6  2002/02/22 00:26:27  frank
// Wrote I/O routines and test for class.
//
// Revision 1.5  2002/02/18 20:08:04  frank
// Changed to store a vector of pointers.
//
// Revision 1.4  2002/02/15 23:20:41  frank
// Read vars from description.
//
// Revision 1.3  2002/02/15 20:19:12  frank
// Revised entire class so that it stores a vector of volumes.
//
// Revision 1.2  2002/01/12 04:19:19  frank
// Disabled time_varying datasets for now.
//
// Revision 1.1  2002/01/11 04:55:11  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/TimeVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: TimeVolume.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
