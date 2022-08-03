// $Id: SegmentList.C,v 1.8.2.1 2009/01/15 04:12:26 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.net)

// Description: A class to aid navigation through segments and branches


// includes
#include "stdafx.h"
#include "SegmentList.h"
#include "Skeleton.h"
#include "V3DFile.h"
#include "File.h"
#include "FileExtensions.h"
#include "Serialization.h"
#include "vxInflate.h"
#include "utilities.h" 


// namespaces
using namespace std;
using namespace Serialization;


// defines
#define FILE_REVISION "$Revision: 1.8.2.1 $"


// static member initialization
Segment SegmentList::m_nullSegment;
const std::string SegmentList::m_sDelimiter = "SegmentList";


/**
 * Input stream SegmentList::SegmentOrderVector operator
 *
 */
std::istream & operator>>(std::istream & is, SegmentList::SegmentOrderVector & v)
{
  uint1 cDummy(0);
  std::string sVersion;
  int4 iNumSegments(0);
  is >> sVersion >> iNumSegments >> cDummy;
  v.resize(iNumSegments);
  for (int4 i(0); i < iNumSegments; is >> v[i].first >> cDummy >> v[i].second >> cDummy, i++);
  return is;
}


/**
 * Output stream SegmentList::SegmentOrderVector operator
 *
 */
std::ostream & operator<<(std::ostream & os, const SegmentList::SegmentOrderVector & v)
{
  const int4 iNumSegments(v.size());
  os << std::string("SegmentOrdering-Version01") << std::ends << std::endl << iNumSegments << ":";
  for (int4 i(0); i < iNumSegments; os << v[i].first << "," << v[i].second << ",", i++);
  return os;
}


/**
 * Constructor.
 *
 */
SegmentList::SegmentList()
{
  Reset();
} // SegmentList()


/**
 * Destructor.
 *
 */
SegmentList::~SegmentList()
{
} // ~SegmentList()


/**
 * Read segments from a file.
 *
 * @param filename      The input filename.
 * @return true on success, false on failure
 */
void SegmentList::Read(const std::string & sFilePrefix)
{
  try
  {
    // open up the properly formatted segment file
    V3DFile pngImage;
    pngImage.Read( sFilePrefix + "." + vx::SegmentListExt, std::pair< float4, float4 >( -1.0F, -1.0F ) );

    //int1 * pOriginalBuffer = NULL;
    //uint4 originalBufferSize = 0;
    // read the data from a png data chunk
    MemoryBuffer memoryBuffer;
    // reinterpret_cast<uint1 **>( & pOriginalBuffer ), originalBufferSize );
    bool bResult = pngImage.GetExtraChunk( std::string( "seGl" ), memoryBuffer );
    if (!bResult)
    {
      throw ex::DataCorruptionException(LogRec("data corruption while loading segment list from file "
        + sFilePrefix + vx::SegmentListExt + ": unable to read data chunk","SegmentList", "Read"));
    }
  
    // determine if we are version 1.1 or greater
    bool bSerializedSegs;
    pair<int4,int4> thisFileVersion = pngImage.GetFileVersion();
    if (thisFileVersion.first >= 1 && thisFileVersion.second >= 1)
      bSerializedSegs = true;
    else
      bSerializedSegs = false;

    // read the new or old version
    if (bSerializedSegs == true)
    {
      // version 1.1 and greater
      // convert from a serialized memory stream into a data structure
      int1 * pBuffer = reinterpret_cast< int1 * >( memoryBuffer.GetDataPtr() );
      const int1 * pBufferEnd = pBuffer + memoryBuffer.GetSize();
      ReadSerializedData(&pBuffer, pBufferEnd);
    }
    else
    {
      // version 1.0
      // create a unique filename
      vx::File tempFile;
      tempFile = vx::File::CreateTempFile("VX");

      // write the segment data to a temp file
      const string tempPath = tempFile.ToString();
      ofstream out(tempPath.c_str(), ios::binary | ios::out);
      if (!out)
      {
        tempFile.Delete();
        throw ex::IOException(LogRec("loading segment list file " + sFilePrefix + "." + vx::SegmentListExt +
          " \\Error: Unable to open temp file " + tempPath + " for writing","SegmentList", "Read"));
      } // if bad file open
      out.write( reinterpret_cast< int1 * >( memoryBuffer.GetDataPtr() ), memoryBuffer.GetSize() );
      out.close();

      // open up the temp file
      ifstream in(tempPath.c_str(), ios::binary | ios::in);
      if (!in)
      {
        tempFile.Delete();
        throw ex::IOException(LogRec("loading segment list file " + sFilePrefix  + vx::SegmentListExt +
          " \\Error: Unable to open temp file " + tempPath + " for reading","SegmentList", "Read"));
      } // if bad temp file open

      // read in from disk 
      Read(in);
      in.close();
    
      // remove the temp file
      bool bResult = tempFile.Delete();
      if (!bResult)
      {
        // non-fatal, just warn and continue
        LogWrn("<SegmentList::Read>::Trouble removing temp file \""+ToAscii(tempPath)+"\".","SegmentList","Read");
      } // if trouble removing
    } // if serialized segs
  }
  catch (ex::FileNotFoundException & e)
  {
    LogErr("File not Found: " + e.GetLogRecord().GetTextMessage(),"SegmentList","Read");
    throw;
  }
  catch (ex::VException & e)
  {
    LogErr("File Read Error: " + e.GetLogRecord().GetTextMessage(),"SegmentList","Read");
    throw;
  }
  catch (...)
  {
    LogErr("An unspecified error caught 'segmentlist read'", "SegmentList", "Read");
    throw;
  }

  return;
} // Read()


/**
 * Read segments from a file stream.
 *
 * @param in    The input file stream.
 * @return true on success, false on failure.
 */
void SegmentList::Read(std::ifstream & in)
{
  // clear out any old segments
  Reset();

  // retrieve the number of segments
  int4 numSegments;
  in >> numSegments;
  // skip to end of line
  const uint4 bufSize = 1000;
  char buffer[bufSize];
  in.getline(buffer,bufSize);

  // error check
  if (numSegments < 0)
  {
    throw ex::DataCorruptionException(LogRec("<SegmentList::Read> Bad number of segments in file.","SegmentList","Read"));
  }

  // read in all the segments
  m_vAllSegments.resize(numSegments);
  for ( int4 segIdx=0; segIdx<numSegments; ++segIdx )
  {
    m_vAllSegments[segIdx].Read(in);
  }

  // add all the segments to the sorted list initially
  for (uint4 i=0; i<m_vAllSegments.size(); ++i)
  {
    m_vSortedSegments.push_back(i);
  } // loop over all segments

  // set the sorted segment index
  if (m_vSortedSegments.size() > 0)
  {
    m_iCurrentSortedSegmentIndex = 0;
  } // if any sorted segments

  m_bSegmentsLoaded = true;

  return;
} // SegmentList::Read


/**
 * Output the segments to a file.
 *
 * @param filename      The output filename.
 * @return true on success, false on failure.
 */
void SegmentList::Write(const std::string & sFilePrefix) const
{
  uint4 originalBufferSize = 0;
  int1 * pOriginalBuffer = NULL;
  
  // serialize the segments data
  originalBufferSize = GetSerializedDataSize();
  try
  {
    pOriginalBuffer = new int1[originalBufferSize];
  }
  catch (bad_alloc)
  {
    throw ex::OutOfMemoryException(LogRec("<SegmentList::Write> Out of memory.","SegmentList","Write"));
  }
  int1 * pBuffer = pOriginalBuffer;
  const int1 * pBufferEnd = pOriginalBuffer + originalBufferSize;
  WriteSerializedData(&pBuffer, pBufferEnd);
    
  // create a PNG formatted file
  V3DFile pngImage;
  pngImage.SetFileVersion( std::pair< int4, int4 >( 1, 1 ) );
  MemoryBuffer memoryBuffer( reinterpret_cast< uint1 * >( pOriginalBuffer ), originalBufferSize );
  pngImage.SetExtraChunk( "seGl", memoryBuffer );
  pngImage.Write( sFilePrefix + "." + vx::SegmentListExt );

  // release memory
  delete [] pOriginalBuffer;

  return;
} // SegmentList::Write


/**
 * Output to a file stream.
 *
 * @param outStream the output file stream.
 * @return true upon success.
 */
void SegmentList::Write(std::ofstream & outStream) const
{
  // write the number of segments
  outStream << m_vAllSegments.size() << " # number of segments" << endl;

  // write all the segments
  for (uint4 i=0; i<m_vAllSegments.size(); i++)
    m_vAllSegments[i].Write(outStream);

  return;
} // Write()


/**
 * Retrieves the number of bytes of the serialized data.
 *
 * @return the number of bytes in the serialized data.
 */
uint4 SegmentList::GetSerializedDataSize() const
{
  uint4 uSum = 0;

  // starting delimiter
  uSum += string(m_sDelimiter).length() + 1;

  // m_vAllSegments.size()
  uSum += sizeof(uint4);

  // loop over all segments
  for (uint4 i=0; i<m_vAllSegments.size(); i++) {
    uSum += m_vAllSegments[i].GetSerializedDataSize();
  } // loop over all segments

  // ending delimiter
  uSum += string(m_sDelimiter).length() + 1;

  return uSum;
} // GetSerializedDataSize()


/**
 * Retrieves all the data in serialized form.
 *
 * @param piBuffer a pointer to the pointer of the start of the output buffer.
 * @param piBufferEnd a pointer to the byte after the end of the valid output buffer range.
 * @return true upon success.
*/
void SegmentList::WriteSerializedData(int1 ** piBuffer, const int1 * piBufferEnd) const
{
  stringstream tt;
  // ensure there is enough space
  if (*piBuffer + GetSerializedDataSize() > piBufferEnd)
  {
    stringstream ss;
    ss << static_cast<int4>(piBufferEnd - *piBuffer) << " " << static_cast<uint4>(GetSerializedDataSize());
    tt<< "Sizes: " << ss.str();
    LogWrn(tt.str(), "SegmentList", "WriteSerializedData");
    throw ex::OutOfMemoryException(LogRec("<SegmentList::WriteSerializedData> Not enough space.","SegmentList","WriteSerializedData"));
  } // if not enough space

  // write a delimiter to help verify the data
  SerializeStringData(piBuffer, piBufferEnd, m_sDelimiter);

  // unsigned int m_vAllSegments.size()
  uint4 uNumSegments = m_vAllSegments.size();
  SerializeSimpleData(piBuffer, piBufferEnd, uNumSegments);

  // std::vector<Segment> m_vAllSegments
  for (uint4 uSegmentIndex=0; uSegmentIndex<m_vAllSegments.size(); uSegmentIndex++)
    m_vAllSegments[uSegmentIndex].WriteSerializedData(piBuffer, piBufferEnd);

  // write a delimiter to help verify the data
  SerializeStringData(piBuffer, piBufferEnd, m_sDelimiter);

  return;
} // WriteSerializedData()


/**
 * Reads in all the data from serialized form.
 *
 * @param piBuffer a pointer to the pointer of the start of the input buffer.
 * @param piBufferEnd a pointer to the byte after the end of the valid input buffer range.
 * @return true upon success.
 */
void SegmentList::ReadSerializedData(int1 ** piBuffer, const int1 * piBufferEnd)
{
  try
  {
    // clear out any old segments
    Reset();

    // attempt to verify the stream by reading a delimiter
    string sDelimiter = DeserializeStringData(piBuffer, piBufferEnd);
    if (sDelimiter != m_sDelimiter)
    {
      throw ex::DataCorruptionException(LogRec("data corruption:segmentlist::Bad starting delimiter: " + sDelimiter,
        "SegmentList", "ReadSerializedData"));
    }
  
    // unsigned int m_vAllSegments.size()
    uint4 uNumSegments;
    DeserializeSimpleData(piBuffer, piBufferEnd, uNumSegments);

    m_vAllSegments.resize(uNumSegments);

    // std::vector<Segment> m_vAllSegments
    for (uint4 uSegmentIndex=0; uSegmentIndex<m_vAllSegments.size(); uSegmentIndex++)
      m_vAllSegments[uSegmentIndex].ReadSerializedData(piBuffer, piBufferEnd);

    // attempt to verify the stream by reading a delimiter
    if (DeserializeStringData(piBuffer, piBufferEnd) != m_sDelimiter)
    {
      throw ex::DataCorruptionException(LogRec("data corruption: bad ending delimiter","SegmentList","ReadSerializedData"));
    }

    // check that we didn't under-read the buffer
    if (*piBuffer < piBufferEnd)
    {
      throw ex::DataCorruptionException(LogRec("data corruption: segment list buffer was not fully read!","SegmentList","ReadSerializedData"));
    }

    // check that we didn't ovverrun the buffer
    if (*piBuffer > piBufferEnd)
    {
      throw ex::DataCorruptionException(LogRec("data corruption::segment list read past the end of the buffer!","SegmentList","ReadSerializedData"));
    }

    // add all the segments to the sorted list initially
    for (uint4 uSortedSegmentIndex=0; uSortedSegmentIndex<m_vAllSegments.size(); ++uSortedSegmentIndex)
    {
      m_vSortedSegments.push_back(uSortedSegmentIndex);
    } // loop over all segments

    // set the sorted segment index
    if (m_vSortedSegments.size() > 0)
    {
      m_iCurrentSortedSegmentIndex = 0;
    } // if any sorted segments

    m_bSegmentsLoaded = true;
  }
  catch (ex::VException &)
  {
    LogErr("An unspecified VException caught at stage 'segmentlist read'", "SegmentList", "ReadSerializedData");
    throw;
  }
  catch (...)
  {
    LogErr("An unspecified error caught at stage 'segmentlist read'", "SegmentList", "ReadSerializedData");
    throw;
  }

  return;
} // ReadSerializedData()


/**
 * Equality operator.
 *
 * @param other the other segment for comparison purposes.
 * @return true if they are identical.
 */
bool SegmentList::operator ==(const SegmentList & other) const
{
  // std::vector<Segment> m_vAllSegments
  if (m_vAllSegments.size() != other.m_vAllSegments.size()) 
  {  
    LogWrn("Number of segments varies.","SegmentList","operator==");
    return false;
  } // if different
  uint4 i;
  for (i=0; i<m_vAllSegments.size(); i++)
  {
    if (!(m_vAllSegments[i] == other.m_vAllSegments[i]))
    {
      LogWrn("Content of segments varies.","SegmentList","operator==");
      return false;
    } // if different
  } // loop over all segments

  // std::vector<unsigned int> m_vSortedSegments
  if (m_vSortedSegments.size() != other.m_vSortedSegments.size())
  {
    LogWrn("Number of sorted segments varies.","SegmentList","operator==");
    return false;
  } // if different
  for (i=0; i<m_vSortedSegments.size(); i++)
  {
    if (m_vSortedSegments[i] != other.m_vSortedSegments[i])
    {
      LogWrn("Content of sorted segments varies.","SegmentList","operator==");
      return false;
    } // if different
  } // loop over all segments

  // bool m_bCurrentStartToEnd
  if (m_bCurrentStartToEnd != other.m_bCurrentStartToEnd) 
  {  
    LogWrn("m_bCurrentStartToEnd varies.","SegmentList","operator==");
    return false;
  } // if different

  // int m_iCurrentSortedSegmentIndex
  if (m_iCurrentSortedSegmentIndex != other.m_iCurrentSortedSegmentIndex)
  {
    LogWrn("m_iCurrentSortedSegmentIndex varies.","SegmentList","operator==");
    return false;
  } // if different

  // bool m_bSegmentsLoaded
  if (m_bSegmentsLoaded != other.m_bSegmentsLoaded)
  {
    LogWrn("m_bSegmentsLoaded varies.","SegmentList","operator==");
    return false;
  } // if different

  return true;
} // operator ==()


/**
 * Verify that the total colon length is below some value.
 *
 * @param fMinimumColonLengthCm the minimum length in cm below which a warning should be raised.
 */
void SegmentList::VerifyTotalLength(const float4 fMinimumColonLengthCm)
{
  // Notify the user
  UpdateTotalLength();
  if (GetTotalLengthMM() / 10.0F < fMinimumColonLengthCm)
  {
    string sWarning = "WARNING: Minimal colon space detected. Poor inflation may leave many areas\n";
    sWarning += "of the colon without space to flythrough resulting in\n";
    sWarning += "lack of endoscopic ability to examine region.\n";
    sWarning += "Note - certain pathologic conditions can mimic this appearance.";
    
    LogWrn(sWarning, "SegmentList", "operator==");
  } // if short colon
} // VerifyTotalLength()


/**
 * Compute the total colon length.
 */
void SegmentList::UpdateTotalLength()
{
  if (m_vSortedSegments.size() > 0)
  {
    // update the first segment length and offset
    GetSegment(0).UpdateLength();
    GetSegment(0).SetZeroDistanceFromRectum();

    // loop over rest of segments
    for (uint4 s=1; s<GetNumSelectedSegments(); s++)
    {
      GetSegment(s).UpdateLength();
      GetSegment(s).UpdateOffsetDistanceFromRectum(GetSegment(s-1));
    } // loop over rest of segments

  } // if any segments
} // UpdateTotalLength()


/**
 * Retrieve the total colon length.
 *
 * @return the total piecewise linear length of the colon in world coordinates.
 */
float4 SegmentList::GetTotalLengthMM() const
{
  if (m_vSortedSegments.size() > 0)
  {
    const Segment & lastSegment = m_vAllSegments[m_vSortedSegments[m_vSortedSegments.size()-1]];
    return lastSegment.GetOffsetDistanceFromRectumMM() + lastSegment.GetEuclideanLengthMM();
  } // if any segments
  else
  {
    return 0.0F;
  } // else no segments
} // GetTotalLengthMM()


/**
 * Clear all the segments.
 */
void SegmentList::Reset()
{
  m_vAllSegments.clear();
  m_vSortedSegments.clear();
  // Initial flight is Cecum->Rectum
  m_bCurrentStartToEnd = true;
  m_bSegmentsLoaded = false;
  m_iCurrentSortedSegmentIndex = -1;

  // make sure there is at least one centerline in the null segment
  Skeleton nullCenterline;
  if (m_nullSegment.GetNumCenterlines() < 1)
    m_nullSegment.AddCenterline(nullCenterline);
} // Clear()


/**
 * Reverse the direction of navigation.
 */
void SegmentList::ReverseCurrentDirection()
{
  m_bCurrentStartToEnd = !m_bCurrentStartToEnd;
} // ReverseCurrentDirection()


/**
 * Sets the direction of navigation.
 *
 * @param bStartToEnd whether the user wants to fly from the start to the end or back.
 */
void SegmentList::SetCurrentDirection(const bool bStartToEnd)
{
  m_bCurrentStartToEnd = bStartToEnd;
} // SetCurrentDirection()


/**
 * Retrieve the current direction of navigation.
 */
bool SegmentList::GetCurrentDirectionStartToEnd() const
{
  return m_bCurrentStartToEnd;
} // GetCurrentDirectionStartToEnd()


/**
 * Add all the segments to the sorted list.
 */
void SegmentList::SelectAllSegments()
{
  // add all unsorted segments to the selected list
  m_vSortedSegments.resize(m_vAllSegments.size());
  for (uint4 i=0; i<m_vAllSegments.size(); i++) {
    m_vAllSegments[i].SetSelected(true);
    m_vSortedSegments[i] = i;
  } // end loop over all segments
  m_iCurrentSortedSegmentIndex = 0;
} // SelectAllSegments()


/**
 * Unselect all the segments.
 */
void SegmentList::UnselectAllSegments()
{
  m_vSortedSegments.clear();

  for (uint4 i=0; i<m_vAllSegments.size(); i++) {
    m_vAllSegments[i].SetSelected(false);
  } // end loop over all segments

  // select "none"
  m_iCurrentSortedSegmentIndex = -1;
} // UnselectAllSegments()


/**
 * Unselect the tail segment.
 */
void SegmentList::UnselectTailSegment()
{
  // only do something if there are any sorted segments
  if (m_vSortedSegments.size() > 0)
  {
    m_vAllSegments[m_vSortedSegments.back()].SetSelected(false);
    m_vSortedSegments.pop_back();
    // set current to first segment if there are any segments at all
    m_iCurrentSortedSegmentIndex = (m_vSortedSegments.size() > 0) ? 0 : -1;
  } // end if there are any sorted segments
} // UnselectTailSegment()


/**
 * Move the current segment pointer to the first segment.
 *
 * @return true if we were able to go to the first segment.
 */
bool SegmentList::GoToFirstSegment()
{
  if (m_vSortedSegments.size() > 0)
  {
    m_iCurrentSortedSegmentIndex = 0;
    return true;
  } // if any segments
  else
  {
    m_iCurrentSortedSegmentIndex = -1;
    return false;
  } // else no segments
} // GoToFirstSegment()


/**
 * Move the current segment pointer to the last segment.
 *
 * @return true if we were able to go to the last segment.
*/
bool SegmentList::GoToLastSegment()
{
  if (m_vSortedSegments.size() > 0)
  {
    m_iCurrentSortedSegmentIndex = m_vSortedSegments.size() - 1;
    return true;
  } // if any segments
  else
  {
    m_iCurrentSortedSegmentIndex = -1;
    return false;
  } // else no segments
} // GoToLastSegment()


/**
 * Move the current segment pointer toward the cecum.
 *
 * @return true if we were able to go to the next segment.
 */
bool SegmentList::GoToNextSegmentToCecum()
{
  // see if there are any segments at all
  if (m_vSortedSegments.size() > 0)
  {
    // check if we are already at the last segment
    if (m_iCurrentSortedSegmentIndex == static_cast<int4>(m_vSortedSegments.size() - 1))
    {
      return false;
    } // if already at last segment
    else
    {
      m_iCurrentSortedSegmentIndex++;
      return true;
    } // else not already at the last segment
  } 
  else 
  {
    return true;
  } // if there are any segments at all
} // GoToNextSegmentToCecum()


/**
 * Move the current segment pointer toward the rectum.
 *
 * @return true if we were able to go to the next segment.
*/
bool SegmentList::GoToNextSegmentToRectum()
{
  // check if there are any segments at all
  if (m_vSortedSegments.size() > 0)
  {

    // check if we are already at the first segment
    if (m_iCurrentSortedSegmentIndex == 0)
    {
      return false;
    } // if already at first segment
    else
    {
      m_iCurrentSortedSegmentIndex--;
      return true;
    } // else not already at the first segment

  } // if any segments
  else
  {
    return true;
  } // else there are no segments
} // GoToNextSegmentToRectum()


/**
 * Returns the current sorted segment.
 *
 * @return a reference to the current segment.
 */
Segment & SegmentList::GetCurrentSegment()
{
  // gracefully check bounds of access
  try
  {
    const uint4 uUnsortedSegmentIndex = m_vSortedSegments.at(m_iCurrentSortedSegmentIndex);
    return m_vAllSegments.at(uUnsortedSegmentIndex);
  } // try to access the segment
  
  catch (out_of_range)
  {
    LogWrn("<SegmentList::GetCurrentSegment> attempted to access out of bounds","SegmentList","GetCurrentSegment");
  } // catch out of bounds exceptions
  
  return m_nullSegment;
} // GetCurrentSegment()


/**
 * Returns the current sorted segment.
 *
 * @return   a const reference to the current segment.
 */
const Segment & SegmentList::GetCurrentSegment() const
{
  // gracefully check bounds of access
  try
  {
    const uint4 uUnsortedSegmentIndex = m_vSortedSegments.at(m_iCurrentSortedSegmentIndex);
    return m_vAllSegments.at(uUnsortedSegmentIndex);
  } // try to access the segment
  
  catch (out_of_range)
  {
    LogWrn("<SegmentList::GetCurrentSegment> attempted to access out of bounds","SegmentList","GetCurrentSegment");
  } // catch out of bounds exceptions
  
  return m_nullSegment;
} // GetCurrentSegment()


/**
 * Returns the current sorted segment skeleton.
 *
 * @return a reference to the current skeleton.
 */
Skeleton & SegmentList::GetCurrentSkeleton()
{
  return GetCurrentSegment().GetCurrentSkeleton();
} // GetCurrentSkeleton()


/**
 * Returns the skeleton for a given segment index.
 *
 * @pararm uSortedSegmentIndex   the sorted segment we are interested in.
 * @return                       a reference to the skeleton.
 */
Skeleton & SegmentList::GetSkeleton(const uint4 uSortedSegmentIndex)
{
  // gracefully check bounds of access
  try
  {
    Segment & segment = m_vAllSegments.at(m_vSortedSegments.at(uSortedSegmentIndex));
    return segment.GetCurrentSkeleton();
  } // try to access the segment
  
  catch (out_of_range)
  {
    LogWrn("attempted to access out of bounds","SegmentList","GetSkeleton");
  } // catch out of bounds exceptions
  
  return m_nullSegment.GetCurrentSkeleton();
} // GetSkeleton()


/**
 * Returns the skeleton for a given segment index.
 *
 * @pararm uSortedSegmentIndex   the sorted segment we are interested in.
 * @return                       a const reference to the skeleton.
 */
const Skeleton & SegmentList::GetSkeleton(const uint4 uSortedSegmentIndex) const
{
  // gracefully check bounds of access
  try
  {
    const Segment & segment = m_vAllSegments.at(m_vSortedSegments.at(uSortedSegmentIndex));
    return segment.GetCurrentSkeleton();
  } // try to access the segment
  
  catch (out_of_range)
  {
    LogWrn("attempted to access out of bounds","SegmentList","GetSkeleton");
  } // catch out of bounds exceptions
  
  return m_nullSegment.GetCurrentSkeleton();
} // GetSkeleton()


/**
 * Return the last segment in the current selected list.
 */
Segment & SegmentList::GetLastSelectedSegment()
{
  // gracefully check bounds of access
  try
  {
    return m_vAllSegments.at(m_vSortedSegments.at(m_vSortedSegments.size()-1));
  } // try to access the segment
  
  catch (out_of_range)
  { 
    LogWrn("attempted to access out of bounds","SegmentList","GetLastSelectedSegment");
  } // catch out of bounds exceptions
  
  return m_nullSegment;
} // GetLastSelectedSegment()


/**
 * Return the second-from-last segment in the current selected list.
 */
Segment & SegmentList::GetLast2SelectedSegment()
{
  // gracefully check bounds of access
  try
  {
    return m_vAllSegments.at(m_vSortedSegments.at(m_vSortedSegments.size()-2));
  } // try to access the segment
  
  catch (out_of_range)
  { 
     LogWrn("attempted to access out of bounds","SegmentList","GetLast2SelectedSegment");
  } // catch out of bounds exceptions
  
  return m_nullSegment;
} // GetLast2SelectedSegment()


/**
 * Return the number of selected segments.
 */
const uint4 SegmentList::GetNumSelectedSegments() const
{
  return m_vSortedSegments.size();
} // GetNumSelectedSegments()


/**
 * Return the number of unsorted segments.
 */
const uint4 SegmentList::GetNumUnsortedSegments() const
{
  return m_vAllSegments.size();
} // GetNumUnsortedSegments()


/**
 * Returns the segment ordering and directions
 *
 */
SegmentList::SegmentOrderVector SegmentList::GetSegmentOrder() const
{
  SegmentOrderVector vOrder;
  if (m_vSortedSegments.empty())
  {
    return vOrder;
  }

  vOrder.resize(m_vSortedSegments.size());
  for (uint4 i(0); i < vOrder.size(); i++)
  {
    vOrder[i] = std::pair<uint4,bool>(m_vSortedSegments[i], m_vAllSegments[m_vSortedSegments[i]].IsRegularDirection());
  }

  return vOrder;
}


/**
 * Sets the segment ordering and directions
 *
 * @param const SegmentOrderVector & vOrder   List of segment indices/directions
 */
void SegmentList::SetSegmentOrder(const SegmentOrderVector & vOrder)
{
  UnselectAllSegments();
  for (uint4 i(0); i < vOrder.size(); i++)
  {
    if (vOrder[i].first >= m_vAllSegments.size())
    {
      continue;
    }

    AddUnsortedSegmentToSortedSegments(vOrder[i].first);
    m_vAllSegments[vOrder[i].first].SelectDirection(vOrder[i].second);
  }

  if (!vOrder.empty())
  {
    m_iCurrentSortedSegmentIndex = 0;
  }
}


/**
 * Return a sorted segment by index.
 */
Segment & SegmentList::GetSegment(const uint4 uSortedSegmentIndex)
{
  // gracefully check bounds of access
  try
  {
    return m_vAllSegments.at(m_vSortedSegments.at(uSortedSegmentIndex));
  } // try to access the segment

  catch (out_of_range)
  {
    LogWrn("attempted to access out of bounds","SegmentList","GetSegment");
  } // catch out of bounds exceptions

  return m_nullSegment;
} // GetSegment()


/**
 * Return a sorted segment by index.
 */
const Segment & SegmentList::GetSegment(const uint4 uSortedSegmentIndex) const
{
  // gracefully check bounds of access
  try
  {
    return m_vAllSegments.at(m_vSortedSegments.at(uSortedSegmentIndex));
  } // try to access the segment

  catch (out_of_range)
  {
    LogWrn("attempted to access out of bounds","SegmentList","GetSegment");
  } // catch out of bounds exceptions

  return m_nullSegment;
} // GetSegment()


/**
 * Return the selected unsorted segment.
 */
Segment & SegmentList::GetUnsortedSegment(const uint4 uUnsortedSegmentIndex)
{
  // gracefully check bounds of access
  try
  {
    return m_vAllSegments.at(uUnsortedSegmentIndex);
  } // try to access the segment
  
  catch (out_of_range)
  {
    LogWrn("attempted to access out of bounds","SegmentList","GetUnsortedSegment");
  } // catch out of bounds exceptions
  
  return m_nullSegment;
} // GetUnsortedSegment()


/**
 * Add the indicated unsorted segment to the list of sorted segments.
 */
void SegmentList::AddUnsortedSegmentToSortedSegments(const uint4 uUnsortedSegmentIndex)
{
  // Check for error conditions
  if (uUnsortedSegmentIndex >= m_vAllSegments.size())
  {
    LogWrn("Argument out of bounds.","SegmentList","AddUnsortedSegmentToSortedSegments");
    return;
  } // out of bounds

  Segment & segment = m_vAllSegments[uUnsortedSegmentIndex];
  if (segment.IsSelected() == true)
  {
    LogWrn("Segment already selected.","SegmentList","AddUnsortedSegmentToSortedSegments");
    return;
  } // already selected

  // select and add to sorted list
  segment.SetSelected(true);
  m_vSortedSegments.push_back(uUnsortedSegmentIndex);
} // AddUnsortedSegmentToSortedSegments()


/**
 * Add the indicated new segment to the list of sorted segments.
 */
void SegmentList::AddNewSegmentAndSelectIt(const Segment & segment)
{
  m_vAllSegments.push_back(segment);

  // add it to the list of selected segments
  const uint4 uIndex = m_vAllSegments.size() - 1;
  m_vSortedSegments.push_back(uIndex);

  // select it
  m_vAllSegments[uIndex].SetSelected(true);
} // AddNewSegmentAndSelectIt()


/**
 * Sets the index of the current sorted segment.
 */
void SegmentList::SetCurrentSegmentIndex(const int4 iSortedSegmentIndex)
{
  m_iCurrentSortedSegmentIndex = iSortedSegmentIndex;
  if (iSortedSegmentIndex >= m_vSortedSegments.size())
  {
    m_iCurrentSortedSegmentIndex = -1;
  } // if out of bounds
} // SetCurrentSegmentIndex()


/**
 * Searches for the sorted segment index of an unsorted segment index.
 *
 * @return      -1 if none is found.
 */
int4 SegmentList::GetSortedSegmentIndex(const uint4 uUnsortedSegmentIndex) const
{
  int4 iReturnIndex = -1;
  for (uint4 i=0; i<m_vSortedSegments.size(); i++)
  {
    if (m_vSortedSegments[i] == uUnsortedSegmentIndex)
    {
      iReturnIndex = i;
    } // if we have found the sought-after segment
  } // end loop over all sorted segments
  return iReturnIndex;
} // GetSortedSegmentIndex()


/**
 * Searches for the unsorted segment index of a sorted segment index.
 *
 * @return    the unsorted segment index.
 */
int4 SegmentList::GetUnsortedSegmentIndex(const uint4 uSortedSegmentIndex) const
{
  return m_vSortedSegments[uSortedSegmentIndex];
} // GetUnsortedSegmentIndex()


/**
 * Searches for the sorted segment index from a segment identifier.
 * Given a segment identifier which can be found in the ins volume,
 * this function returns the sorted index of the corresponding segment.
 *
 * @return      -1 if none is found.
 */
int4 SegmentList::GetSortedSegmentIndexFromIdentifier(const uint2 uSegmentIdentifier) const
{
  int4 iReturnIndex = -1;
  for (uint4 i=0; i<m_vSortedSegments.size(); i++)
  {
    if (m_vAllSegments[m_vSortedSegments[i]].GetIdentifier() == uSegmentIdentifier)
    {
      iReturnIndex = i;
    } // if we have found the sought-after segment
  } // end loop over all sorted segments
  return iReturnIndex;
} // GetSortedSegmentIndexFromIdentifier()


/**
 * Searches for the unsorted segment index from a segment identifier.
 * Given a segment identifier which can be found in the ins volume,
 * this function returns the unsorted index of the corresponding segment.
 *
 * @return      -1 if none is found.
 */
int4 SegmentList::GetUnsortedSegmentIndexFromIdentifier(const uint2 uSegmentIdentifier) const
{
  int4 iReturnIndex = -1;
  for (uint4 i=0; i<m_vAllSegments.size(); i++)
  {
    if (m_vAllSegments[i].GetIdentifier() == uSegmentIdentifier)
    {
      iReturnIndex = i;
    } // if we have found the sought-after segment
  } // end loop over all unsorted segments
  return iReturnIndex;
} // GetUnsortedSegmentIndexFromIdentifier()


/**
 * Test the serialization functionality.
 */
bool SegmentList::TestSerialization() const
{
  // serialize all the data
  const uint4 originalBufferSize = GetSerializedDataSize();
  int1 * pOriginalBuffer = new int1[originalBufferSize];
  int1 * pBuffer = pOriginalBuffer;
  const int1 * pBufferEnd = pBuffer + originalBufferSize;
  try
  {
    WriteSerializedData(&pBuffer, pBufferEnd);
  }
  catch (...)
  {
    LogWrn("<SegmentList::TestSerialization> Failed to write buffer.","SegmentList","TestSerialization");
    return false;
  } // failed write
  stringstream ss;
  ss << pBuffer << " " << pBufferEnd;
  LogWrn(ss.str(), "SegmentList", "TestSerialization");
  

  // deserialize the data into a new segment list
  SegmentList other;
  pBuffer = pOriginalBuffer;
  other.ReadSerializedData(&pBuffer, pBufferEnd);

  // reclaim memory
  delete [] pOriginalBuffer;
  
  // test the equality
  if (!(*this == other))
  {
    LogWrn("<SegmentList::TestSerialization> Failed equality test.","SegmentList","TestSerialization");
    return false;
  } // if equal

  return true;
} // TestSerialization()


/**
 * Returns whether the current segment is the first selected segment.
 */
bool SegmentList::IsFirstSegmentCurrent() const
{
  if (m_iCurrentSortedSegmentIndex == 0)
  {
    return true;
  } // if first segment is selected
  else
  {
    return false;
  } // else first segment is not selected
} // IsFirstSegmentCurrent()


/// Returns whether the current segment is the last selected segment
bool SegmentList::IsLastSegmentCurrent() const
{
  // early return if no segments are selected
  if (m_iCurrentSortedSegmentIndex == -1) {
    return false;
  } // if early return
  
  // if last segment is selected
  if (m_iCurrentSortedSegmentIndex == m_vSortedSegments.size()-1)
    return true;
  else
    return false;
} // IsLastSegmentCurrent()


/**
 * Returns the color of the centerline gradient of the given segment and end.
 */
Triple<float4> SegmentList::GetSegmentColor(const uint4 uSortedSegmentIndex, const bool bStart)
{
  Segment & segment = GetSegment(uSortedSegmentIndex);
  Skeleton skeleton = segment.GetCurrentSkeleton();

  // flip for flipped directions
  Vector<float4> startColor(1.0F, 0.0F, 0.0F), endColor(0.0F, 1.0F, 0.0F);
  if (segment.IsRegularDirection() == false)
    Swap(startColor, endColor);
  if (m_bCurrentStartToEnd == false)
    Swap(startColor, endColor);

  // develop a continuous red->green gradient
  const float4 fOffsetMM = segment.GetOffsetDistanceFromRectumMM();
  const SkeletonNode & firstNode = skeleton.GetSkelNode(0);
  const SkeletonNode & lastNode = skeleton.GetSkelNode(skeleton.GetNumOfElements()-1);
  const float4 dfs = bStart ? firstNode.m_worldDfs : lastNode.m_worldDfs;
  const float4 percent = (fOffsetMM + dfs) / GetTotalLengthMM();
  return (Triple<float4>)( startColor * (1.0F - percent) + endColor * percent );
} // GetSegmentColor()


/**
 * Rotate a prone segment list to supine
 *
 * @param worldSize the size of the entire volume in MM
 */
void SegmentList::RotateProneToSupine( const Triple<float4> & worldSize )
{
  for ( std::vector<Segment>::iterator iter = m_vAllSegments.begin(); iter != m_vAllSegments.end(); ++iter )
    iter->RotateProneToSupine( worldSize );
} // RotateProneToSupine()


// undefines
#undef FILE_REVISION


// $Log: SegmentList.C,v $
// Revision 1.8.2.1  2009/01/15 04:12:26  jmeade
// constant method variant of GetSegment()
//
// Revision 1.8  2006/09/13 15:24:18  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.7  2006/06/07 18:37:57  dongqing
// comments
//
// Revision 1.6  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.5  2006/02/10 18:05:12  jmeade
// newer seg file list identifier.
//
// Revision 1.4  2006/02/10 15:56:07  frank
// problem with getting extra segment chunk
//
// Revision 1.3  2006/02/10 01:53:27  jmeade
// more, necessary, debugging info in Read() method.
//
// Revision 1.2  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.1.4.1  2006/03/29 20:53:20  frank
// read the segment number from the cad findings file
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.9.2.3.8.1  2005/06/28 12:06:18  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.9.2.3  2005/02/15 20:06:34  jmeade
// merge from vc1.4 branch.
//
// Revision 3.9.2.2.2.1  2005/02/03 21:40:55  dongqing
// Stop create the Header PNG images. It has no value for
// diagnosis.
//
// Revision 3.9.2.2  2004/03/22 21:46:19  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.9.2.1  2003/03/26 23:14:56  jmeade
// Issue 2849:  Const equivalents to a few access methods.
//
// Revision 3.9  2002/10/31 19:48:34  kevin
// Cleaned up debug so the log files look cleaner
//
// Revision 3.8  2002/09/30 20:46:46  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.7  2002/09/13 00:48:14  jmeade
// Segment list serialize function; code standards.
//
// Revision 3.6  2002/09/06 16:10:58  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.5  2002/07/03 03:23:43  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.4  2002/03/08 22:37:22  michael
// include cleanup
//
// Revision 3.3.2.2  2002/08/06 20:44:46  jmeade
// Issue 2596:  Ensure when you set segment order that an ordered segment is selected as current.
//
// Revision 3.3.2.1  2002/07/02 20:50:13  jmeade
// Issue 2062:  Get/Set of segment ordering/directions.
//
// Revision 3.3  2001/11/05 18:48:11  ana
// GoThroughSegments moved to Study.
//
// Revision 3.2  2001/11/05 17:53:45  ana
// GoThroughSegments method added in order to extract registration input data.
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:18   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:49:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:44   ingmar
// Initial revision.
// Revision 1.5  2001/09/13 17:42:49  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:04:58  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:17  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:09  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.113  2001/08/21 16:45:52  soeren
// removed some warnings
//
// Revision 1.112  2001/08/14 14:59:48  binli
// exception
//
// Revision 1.111  2001/08/13 20:05:44  tushar
// removed errors due to the new logger
//
// Revision 1.110  2001/08/13 00:24:27  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.109  2001/08/07 17:02:57  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.108  2001/07/26 20:37:25  binli
// ID 89: exception handling.
//
// Revision 1.107  2001/07/25 02:43:42  sachin
// bad fix of exceptions
//
// Revision 1.106  2001/07/18 21:24:12  binli
// not to include "v3dFile.h"
//
// Revision 1.105  2001/07/18 20:55:06  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.104  2001/07/16 23:47:51  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.103  2001/07/13 19:52:16  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.102  2001/07/12 21:14:14  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.101  2001/07/03 14:57:35  soeren
// removed stdafx.c and .h
//
// Revision 1.100  2001/05/04 15:32:18  binli
// reflected works of refering triangle display list.
//
// Revision 1.99  2001/04/30 12:29:45  frank
// Added exception handling.
//
// Revision 1.98  2001/04/23 11:22:29  frank
// Added error checking to file input.
//
// Revision 1.97  2001/04/19 15:55:07  jmeade
// don't log out of range error on segment list, as it's a normal occurrence
// when a segment isn't selected when calling ReaderGlobal::GetCurrentSegment
//
// Revision 1.96  2001/04/12 18:52:39  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.95  2001/04/12 17:01:50  jmeade
// further simplified implementation of FileExtensions
//
// Revision 1.94  2001/04/12 16:25:43  jmeade
// changed use of FileExtensions class
//
// Revision 1.93  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.92  2001/04/10 17:35:37  dmitry
// updated datatypes
//
// Revision 1.91  2001/04/10 13:14:37  frank
// Added comments and variable name changes.
//
// Revision 1.90  2001/04/06 19:39:28  frank
// Added more graceful bounds checking.
//
// Revision 1.89  2001/04/06 13:54:19  frank
// Removed some pointers.
//
// Revision 1.88  2001/03/30 18:48:23  dmitry
// Updated datatypes.
//
// Revision 1.87  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.86  2001/03/28 13:57:46  frank
// Added a LOT of comments and worked to eliminate pointers.
//
// Revision 1.85  2001/03/26 17:21:10  frank
// Moved serialization functions to separate namespace.
//
// Revision 1.84  2001/03/26 16:01:33  frank
// Reflected changes in Segment class.
//
// Revision 1.83  2001/03/26 15:31:38  frank
// Working on coding compliance.
//
// Revision 1.82  2001/03/23 16:37:29  frank
// Upgraded segments file to version 1.1 and retained ability to read 1.0 datasets.
//
// Revision 1.81  2001/03/20 12:49:37  frank
// Updated to reflect new coding standards.
//
// Revision 1.80  2001/03/19 18:07:52  frank
// Reflected a variable name change in Segment class.
//
// Revision 1.79  2001/02/27 22:01:59  jmeade
// SelectAllSegments() function
//
// Revision 1.78  2001/02/22 01:34:59  jeff
// code without coding conventions is gettting harder and harder to comprehend
//
// Revision 1.77  2001/02/07 19:13:18  frank
// Typo: slow copy instead of reference...
//
// Revision 1.76  2001/02/07 17:11:54  frank
// Cleaned up centerline direction rendering.
//
// Revision 1.75  2001/02/07 15:23:52  frank
// Rendered centerline in continuous color gradient.
//
// Revision 1.74  2001/02/07 13:46:01  frank
// Merged the UpdateOffset and UpdateTotalLength functions.
//
// Revision 1.73  2001/02/06 21:43:34  jeff
// clarified segment jump operations
//
// Revision 1.72  2001/02/02 16:44:48  dave
// File class used to delete
//
// Revision 1.71  2001/02/02 13:08:20  frank
// Started navigation at the rectum.
//
// Revision 1.70  2001/01/31 22:17:45  geconomos
// Cache total length of sorted segments. Added UpdateTotalLength()
// and changed GetTotalLength() to return cached value.
//
// Revision 1.69  2001/01/29 20:02:52  frank
// Added a function to update the offsets.
//
// Revision 1.68  2001/01/29 18:16:26  frank
// Removed error suppression.
//
// Revision 1.67  2001/01/23 20:12:23  dave
// changed scope of i in for() again
//
// Revision 1.66  2001/01/23 20:00:41  dave
// compiles for UNIX
//
// Revision 1.65  2001/01/23 16:38:34  dave
// changed back to CreateTempFile
//
// Revision 1.64  2001/01/23 14:12:34  frank
// Added functions to query the current segment index.
//
// Revision 1.63  2001/01/23 13:21:24  frank
// Used File class for creating a unique filename.
//
// Revision 1.62  2001/01/19 13:08:17  frank
// Virtual rollback.
//
// Revision 1.61  2001/01/19 12:08:38  frank
// Added OS independence.
//
// Revision 1.60  2001/01/08 14:39:51  frank
// Moved temp file to windows temp directory.
//
// Revision 1.59  2001/01/05 14:56:47  frank
// Added access functions for direction control.
//
// Revision 1.58  2000/12/22 14:56:09  frank
// Removed spurious warnings
//
// Revision 1.57  2000/12/22 13:03:50  frank
// Moved forward to PNG encoded data chunk instead of a separate file.
//
// Revision 1.56  2000/12/19 18:31:50  frank
// Backtracked to read December 19 directory
//
// Revision 1.55  2000/12/19 17:08:30  sarang
// gcc/linux fixes
//
// Revision 1.54  2000/12/19 14:23:51  frank
// Compressed old format into png chunk
//
// Revision 1.53  2000/12/19 14:22:14  frank
// Compressed old format into png chunk
//
// Revision 1.52  2000/12/18 19:51:53  frank
// Removed stringstream I/O
//
// Revision 1.51  2000/12/18 19:21:59  frank
// Rolled back to old format
//
// Revision 1.50  2000/12/18 15:44:16  frank
// Testing triangle data
//
// Revision 1.49  2000/12/18 12:52:49  kevin
// Made Franks changes to read only the new seg files
//
// Revision 1.48  2000/12/14 14:52:10  frank
// Modified the interface for serialization/deserialization
// Added a serialization test function
//
// Revision 1.47  2000/12/14 12:30:24  frank
// Incorporated the triangle preview icon
//
// Revision 1.46  2000/12/13 18:01:46  frank
// Wrote out unserialized format
//
// Revision 1.45  2000/12/13 17:27:44  frank
// Moved to new png data format
//
// Revision 1.44  2000/12/13 15:06:44  frank
// Added equality operators
//
// Revision 1.43  2000/12/13 13:45:41  frank
// Worked on debugging serialized format
//
// Revision 1.42  2000/12/12 19:46:35  frank
// Worked on debugging serialized output
//
// Revision 1.41  2000/12/12 19:42:34  frank
// Worked on debugging serialized output
//
// Revision 1.40  2000/12/12 19:11:24  frank
// Cleaned up output function
//
// Revision 1.39  2000/12/12 17:31:13  frank
// Output the segments file in png format
//
// Revision 1.38  2000/12/12 17:16:50  frank
// Removed debug test
//
// Revision 1.37  2000/12/12 16:37:11  frank
// Temp moved back to old file format
//
// Revision 1.36  2000/12/12 16:07:47  frank
// Fixed bugs in serialization and deserialization
//
// Revision 1.35  2000/12/12 14:38:33  frank
// Temporary change to allow developers to read dec11 data files
//
// Revision 1.33  2000/12/12 14:26:36  frank
// Wrote serialized stream to a separate file (png problems...)
//
// Revision 1.32  2000/12/12 13:58:31  frank
// Backtracked a little for debugging
//
// Revision 1.31  2000/12/12 12:08:31  frank
// Added delimiters to serialized data stream
// Implemented png chunk serialized data
//
// Revision 1.30  2000/12/12 11:40:56  frank
// Cleaned up memory
//
// Revision 1.29  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.28  2000/12/09 18:45:56  frank
// Temporarily moved back to the legacy file format
//
// Revision 1.27  2000/12/09 18:42:55  frank
// Temporarily moved back to the legacy file format
//
// Revision 1.26  2000/12/09 18:17:25  frank
// Added serialized input
//
// Revision 1.25  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.24  2000/12/09 15:38:51  frank
// Updated segment reading for new file format
//
// Revision 1.23  2000/12/09 14:43:54  frank
// Stream I/O was too slow, went to temp binary file
//
// Revision 1.22  2000/12/09 12:55:05  frank
// Improved stream I/O
//
// Revision 1.21  2000/12/05 20:01:22  frank
// Temporarily removed stream I/O functions
//
// Revision 1.20  2000/12/05 19:58:28  frank
// Added stream I/O functions
//
// Revision 1.19  2000/11/25 03:04:43  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.18  2000/11/21 16:10:24  frank
// Removed non-critical warning
//
// Revision 1.17  2000/11/13 18:26:35  frank
// Added function to search for segment based on identifier.
//
// Revision 1.16  2000/11/10 15:21:49  frank
// Added jumping to new segments in overview window
//
// Revision 1.15  2000/11/03 19:10:49  ingmar
// fixed gcc warnings
//
// Revision 1.14  2000/11/03 16:44:06  frank
// Improved speed and removed dead code
//
// Revision 1.13  2000/11/03 12:59:40  frank
// Fixed triangle file path
//
// Revision 1.12  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.11.6.3  2000/10/30 17:05:57  frank
// merged back using old v2k branch
//
// Revision 1.11.6.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.11.6.1  2000/10/28 17:38:58  ingmar
// new ierators for new segments
//
// Revision 1.11.2.2  2000/10/27 15:20:54  frank
// Got segment structure working
//
// Revision 1.11.2.1  2000/10/26 13:28:32  frank
// Added new segment structure
//
// Revision 1.11  2000/10/16 08:29:18  antonio
// resolved most warnings
//
// Revision 1.10  2000/10/12 02:45:46  kevin
// FIxed OffsetDistanceFromRectum and added access function to
// get segments at or near the end of the list
//
// Revision 1.9  2000/10/11 14:47:58  kevin
// Added jump to next segment prev segment
//
// Revision 1.8  2000/10/04 14:19:33  kevin
// Added Endpoint picking (so far simple for no
// branches case untill skel is redone)
//
// Revision 1.7.2.1  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.7  2000/10/02 21:59:12  jeff
// Added IsLoaded() query to SegmentList class
//
// Revision 1.6  2000/10/02 16:08:37  frank
// Improved SegmentList I/O
//
// Revision 1.5  2000/10/02 15:51:41  ingmar
// fixed gcc complaints
//
// Revision 1.4  2000/09/28 20:10:36  frank
// Commented out centerline smoothing
//
// Revision 1.3  2000/09/28 17:10:00  frank
// Added I/O for segments and segment lists
//
// Revision 1.2  2000/09/27 18:14:18  frank
// Fixed typo
//
// Revision 1.1  2000/09/27 15:07:52  frank
// Added segment data structures
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
//
// 21    9/05/00 1:34a Ingmar
// now works with new naming
//
// 20    9/02/00 4:20p Kevin
// Smoothed the skeleton a few more times to make unwarpping more robust
// (THIS SHOULD BE FIXED PROPERLY SOMEDAY)
//
// 19    9/01/00 4:55p Frank
// Worked on unwrapping the colon wall
//
// 18    8/31/00 9:05a Frank
// Updated comments
//
// 17    8/28/00 11:53a Antonio
// updated header
//
// *****************  Version 16  *****************
// User: Kevin        Date: 8/21/00    Time: 3:41p
// Updated in $/Viatron1000/src/ReaderLib
// Added segment length checking
//
// *****************  Version 15  *****************
// User: Kevin        Date: 8/20/00    Time: 10:20p
// Updated in $/Viatron1000/src/ReaderLib
// Added choosing of flight direction
//
// *****************  Version 14  *****************
// User: Kevin        Date: 8/20/00    Time: 1:48p
// Updated in $/Viatron1000/src/ReaderLib
// Fixed centerline coloring
//
// *****************  Version 13  *****************
// User: Kevin        Date: 8/19/00    Time: 1:17p
// Updated in $/Viatron1000/src/ReaderLib
// Fixed Tri files for 2 digit files
//
// *****************  Version 12  *****************
// User: Kevin        Date: 8/17/00    Time: 7:09p
// Updated in $/Viatron1000/src/ReaderLib
// Moved tri strips to the segmnet data structure so that the correlate
// with the skeleton indexes for picking
//
// *****************  Version 11  *****************
// User: Kevin        Date: 8/17/00    Time: 4:39p
// Updated in $/Viatron1000/src/ReaderLib
// Fixed 1 voxel skeleton degeneracy
//
// *****************  Version 10  *****************
// User: Frank        Date: 8/08/00    Time: 2:27p
// Updated in $/Viatron1000/src/ReaderLib
// Reflected name change
//
// *****************  Version 9  *****************
// User: Frank        Date: 8/08/00    Time: 2:23p
// Updated in $/Viatron1000/src/ReaderLib
// Broke Segment class out into it's own class
//
// *****************  Version 8  *****************
// User: Kevin        Date: 8/07/00    Time: 6:00p
// Updated in $/Viatron1000/src/ReaderLib
// FIxed CSI problems with new segment data structures
//
// *****************  Version 7  *****************
// User: Kevin        Date: 8/06/00    Time: 7:02p
// Updated in $/Viatron1000/src/ReaderLib
// Added Picking of segments for order. To accomplish this, I
// pulled the guts out of some data structures and replaced them
//
// *****************  Version 6  *****************
// User: Kevin        Date: 8/02/00    Time: 5:51p
// Updated in $/Viatron1000/src/ReaderLib
// Added some multi segment paraaaameters
//
// *****************  Version 5  *****************
// User: Kevin        Date: 8/01/00    Time: 10:05p
// Updated in $/Viatron1000/src/ReaderLib
// Added num segments to inf file
//
// *****************  Version 4  *****************
// User: Frank        Date: 8/01/00    Time: 4:08p
// Updated in $/Viatron1000/src/ReaderLib
// Added bidirectional potential fields
//
// *****************  Version 3  *****************
// User: Frank        Date: 8/01/00    Time: 9:54a
// Updated in $/Viatron1000/src/ReaderLib
// Added file I/O
//
// *****************  Version 2  *****************
// User: Frank        Date: 7/31/00    Time: 4:32p
// Updated in $/Viatron1000/src/ReaderLib
// Work in progress
//
// *****************  Version 1  *****************
// User: Frank        Date: 7/31/00    Time: 4:23p
// Created in $/Viatron1000/src/ReaderLib
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/SegmentList.C,v 1.8.2.1 2009/01/15 04:12:26 jmeade Exp $
// $Id: SegmentList.C,v 1.8.2.1 2009/01/15 04:12:26 jmeade Exp $
