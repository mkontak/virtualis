// $Id: Segment.C,v 1.3.2.1 2010/11/15 20:37:29 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


/**
 * Detailed description: A class to store all the information related to a segment.
 */


// includes
#include "stdafx.h"
#include "Segment.h"
#include "Serialization.h"
#include "Skeleton.h"
#include "FileExtensions.h"


// namespaces
using namespace std;
using namespace Serialization;


// defines
#define FILE_REVISION "$Revision: 1.3.2.1 $"


// static member initialization
Skeleton Segment::m_nullCenterline;
const std::string Segment::m_sDelimiter = "Segment";


/**
 * Constructor.
 */
Segment::Segment()
{
  m_uIdentifier = 0;
  m_bRegularDirection = true;
  m_fLengthMM = 0;
	m_fAirVolumeCC = 0.0F;
	m_fTotalVolumeCC = 0.0F;
  m_fAverageTaggedHU = 0.0F;
  m_iTaggedVoxelCount = 0;
  m_fOffsetDistanceFromRectumMM = 0;
  m_bSelected = false;
  m_uCsiOffset = 0;
} // Segment()


/**
 * Destructor.
 */
Segment::~Segment()
{
} // ~Segment()


/**
 * Copy Constructor.
 *
 * @param other the incoming data to copy.
 */
Segment::Segment(const Segment & other)
{
  *this = other;
} // Segment(other)


/**
 * Assignment operator.
 *
 * @param other the incoming data to copy.
 * @return a reference to itself.
 */
Segment & Segment::operator =(const Segment & other)
{
  // beware of self assignment
  if (this != &other)
  {
    try
    {
      m_uIdentifier = other.m_uIdentifier;
      m_bRegularDirection = other.m_bRegularDirection;
      m_fLengthMM = other.m_fLengthMM;
      m_fOffsetDistanceFromRectumMM = other.m_fOffsetDistanceFromRectumMM;
      m_bSelected = other.m_bSelected;
      m_vCenterlines = other.m_vCenterlines;
      m_vEndpointsMM = other.m_vEndpointsMM;
      m_uCsiOffset = other.m_uCsiOffset;
      m_vPdrFilenames = other.m_vPdrFilenames;
      m_vCsiFilenames = other.m_vCsiFilenames;
      m_triangleStrip = other.m_triangleStrip;
      m_vStartpointMM = other.m_vStartpointMM;
      m_vEndpointMM = other.m_vEndpointMM;
			m_fAirVolumeCC = other.m_fAirVolumeCC;
			m_fTotalVolumeCC = other.m_fTotalVolumeCC;
    }
    catch (...)
    {
      throw ex::OutOfMemoryException(LogRec("<Segment::operator => Out of memory.","Segment", "operator"));
    }
  } // if not self assignment

  return * this;
} // operator =()


/**
 * Recalculate the Euclidean length of the segment.
 */
void Segment::UpdateLength()
{
  m_fLengthMM = m_vCenterlines[0].GetPiecewiseEuclideanLength();
} // UpdateLength()


/**
 * Update the distance from rectum including distended areas.
 *
 * @param prevSegment the previous segment.
 */
void Segment::UpdateOffsetDistanceFromRectum(const Segment & prevSegment)
{
  Vector<float4> gapMM(prevSegment.m_vEndpointMM - m_vStartpointMM);
  m_fOffsetDistanceFromRectumMM = prevSegment.m_fOffsetDistanceFromRectumMM +
    prevSegment.m_fLengthMM +
    gapMM.GetLength();
} // UpdateOffsetDistanceFromRectum()


/**
 * Initialize the distance from rectum to zero (use this for the first segment).
 */
void Segment::SetZeroDistanceFromRectum()
{
  m_fOffsetDistanceFromRectumMM = 0;
} // SetZeroDistanceFromRectum()


/**
 * Retrieve first skeleton node of the centerline.
 *
 * @return a reference to the beginning skeleton node or a null node upon error.
 */
const SkeletonNode & Segment::GetBeginningSkelNode()
{
  uint2 skelIndex;
  Skeleton & skeleton = GetCurrentSkeleton();

  // check for a short centerline
  const uint4 uSkipElements = 10;  // skip over the first n elements
  if (skeleton.GetNumOfElements() <= uSkipElements)
    skelIndex = skeleton.GetNumOfElements() / 2;

  // choose either the start or the end
  if (m_bRegularDirection == true)
    skelIndex = uSkipElements;
  else
    skelIndex = skeleton.GetNumOfElements()-uSkipElements;
  
  return skeleton.GetSkelNode(skelIndex);
} // GetBeginningSkelNode()


/**
 * Retrieve last skeleton node of the centerline.
 *
 * @return a reference to the ending skeleton node or a null node upon error.
 */
const SkeletonNode & Segment::GetEndingSkelNode()
{
  uint2 skelIndex;
  Skeleton & skeleton = GetCurrentSkeleton();

  // check for a short centerline
  const uint4 uSkipElements = 10;  // skip over the first n elements
  if (skeleton.GetNumOfElements() <= uSkipElements)
    skelIndex = skeleton.GetNumOfElements() / 2;

  // choose either the start or the end
  if (m_bRegularDirection == true)
    skelIndex = skeleton.GetNumOfElements()-uSkipElements;
  else
    skelIndex = uSkipElements;
  
  return skeleton.GetSkelNode(skelIndex);
} // GetEndingSkelNode()


/**
 * Read from disk.
 *
 * @param in a reference to the input file stream.
 * @return true upon success.
 */
void Segment::Read(std::ifstream & in)
{
  // make sure the stream is ok before reading
  if (in.bad())
  {
    throw ex::IOException(LogRec("Trouble beginning to read segment file.","Segment", "Read"));
  }

  // read identifier
  const uint4 uBufSize = 1000;
  int1 iBuffer[uBufSize];
  string sIdentifier;
  in >> sIdentifier;
  in.getline(iBuffer,uBufSize);
  if (sIdentifier.compare(m_sDelimiter) != 0)
  {
    throw ex::DataCorruptionException(LogRec("Unable to read this segment from disk.  Bad delimiter: "+sIdentifier,"Segment", "Read"));
  }

  // read in the simple data members
  in >> m_uIdentifier;
  in.getline(iBuffer,uBufSize);
  int4 intBool;
  in >> intBool;
  m_bRegularDirection = (intBool != 0);
  in.getline(iBuffer,uBufSize);
  in.read((int1*)(&m_fLengthMM), sizeof(float4));
  in.getline(iBuffer,uBufSize);
  in.read((int1*)(&m_fOffsetDistanceFromRectumMM), sizeof(float4));
  in.getline(iBuffer,uBufSize);
  in >> intBool;
  m_bSelected = (intBool != 0);
  in.getline(iBuffer,uBufSize);
  in >> m_uCsiOffset;
  in.getline(iBuffer,uBufSize);
  
  // read in the triangle strips
  m_triangleStrip.Clear();
  if (!m_triangleStrip.Read(in))
    throw ex::DataCorruptionException(LogRec("Unable to read triangle strips from disk.","Segment","Read"));

  // read in the centerlines
  int4 numCenterlines;
  in >> numCenterlines;
  in.getline(iBuffer,uBufSize);
  if (numCenterlines < 0)
    throw ex::DataCorruptionException(LogRec("Bad number of centerlines in segment file.","Segment","Read"));
  m_vCenterlines.clear();
  int4 clIndex = 0;
  for (; clIndex<numCenterlines; ++clIndex)
  {
    Skeleton skeleton;
    try 
    {
      skeleton.Read(in);
    }
    catch (...)
    {      
      LogWrn("Unable to read centerlines from disk.","Segment","Read"); 
      return;
    } // bad skeleton read
    m_vCenterlines.push_back(skeleton);
    const uint4 length = m_vCenterlines[clIndex].GetNumOfElements();
    const uint4 originalLength = skeleton.GetNumOfElements();
    if (length < originalLength)
    {
      throw ex::VException(LogRec("Short skeleton copy!","Segment","Read"));
    } // bad skeleton copy
  } // for all centerlines

  // read in the endpoints
  int4 numEndpoints;
  in >> numEndpoints;
  in.getline(iBuffer,uBufSize);
  if (numEndpoints < 0)
  {
    LogWrn("Bad number of endpoints in segment file.","Segment","Read");
    return;
  } // bad number of endpoints
  m_vEndpointsMM.clear();
  for (int4 epNum=0; epNum<numEndpoints; ++epNum)
  {
    Point<float4> point;
    in.read((int1*)(&point), sizeof(Point<float4>));
    m_vEndpointsMM.push_back(point);
  } // loop over endpoints

  // initialize the start and end points
  if (m_vEndpointsMM.size() > 0)
    m_vStartpointMM = m_vEndpointsMM[0];
  if (m_vEndpointsMM.size() > 1)
    m_vEndpointMM = m_vEndpointsMM[1];

  // read in the pdr filenames
  int4 numPdrFilenames;
  in >> numPdrFilenames;
  in.getline(iBuffer, uBufSize);
  if (numPdrFilenames < 0)
  {
    
    LogWrn("Bad number of PDR filenames in segment file.","Segment","Read"); 
    return;
  } // error
  m_vPdrFilenames.clear();
  int4 i;
  for (i=0; i<numPdrFilenames; ++i)
  {
    string tmp;
    in >> tmp;
    in.getline(iBuffer,uBufSize);
    m_vPdrFilenames.push_back(tmp);
  } // end loop over all pdr filenames

  // read in the csi filenames
  int4 numCsiFilenames;
  in >> numCsiFilenames;
  in.getline(iBuffer, uBufSize);
  m_vCsiFilenames.clear();
  if (numCsiFilenames < 0)
  {
    LogWrn("Bad number of CSI filenames in segment file.","Segment","Read"); 
    return;
  } // error
  for (i=0; i<numCsiFilenames; ++i)
  {
    string tmp;
    in >> tmp;
    in.getline(iBuffer,uBufSize);
    m_vCsiFilenames.push_back(tmp);
  } // end loop over all csi filenames

  // read in the postfix message for verification
  in.getline(iBuffer,uBufSize);
  const string sEndMessage = "End-of-segment";
  if (string(iBuffer).compare(sEndMessage) != 0)
  {
    LogWrn("Postfix message incorrect.","Segment","Read"); 
    return;
  } // end verification

  // make sure the disk is still ok after all that reading...whew
  if (in.bad())
    throw ex::IOException(LogRec("Trouble reading segment file.","Segment","Read"));

  return;
} // Read()


/**
 * Write to disk.
 *
 * @param out a reference to the output file stream.
 * @return true upon success.
 */
void Segment::Write(std::ofstream & out) const 
{
  // make sure the stream is ok before writing
  if (out.bad())
    throw ex::IOException(LogRec("Trouble beginning to write segment file.","Segment","Write"));

  // write identifier
  out << m_sDelimiter << endl;

  // write out the simple data members
  out << m_uIdentifier << " # Identifier of this segment" << endl;
  out << static_cast<int4>(m_bRegularDirection) << " # Flying in the regular direction" << endl;
  out.write((int1*)(&m_fLengthMM), sizeof(float4));
  out << m_fLengthMM << " # Length of centerline" << endl;
  out.write((int1*)(&m_fOffsetDistanceFromRectumMM), sizeof(float4));
  out << m_fOffsetDistanceFromRectumMM << " # Offset of the distance from rectum" << endl;
  out << static_cast<int4>(m_bSelected) << " # If this segment is selected" << endl;
  out << m_uCsiOffset << " # Offset of the CSI" << endl;
  
  // write out the triangle strip
  const uint4 uBufSize = 1000;
  //char buffer[bufSize];
  //sprintf(buffer, "%s_%d.tri", GetBaseName(filePrefix).c_str(), m_usIdentifier);
  bool bResult = m_triangleStrip.Write(out);
  if (!bResult)
    throw ex::IOException(LogRec("Unable to write all triangle strips to disk.","Segment","Write"));

  // write out the centerlines
  out << m_vCenterlines.size() << " # Number of centerlines" << endl;
  uint4 i=0;
  for (i=0; i<m_vCenterlines.size(); i++)
  {
    try
    {
      m_vCenterlines[i].Write(out);
    }
    catch (...)
    {
      throw ex::IOException(LogRec("Unable to write all centerlines to disk.","Segment","Write"));
    } // if bad centerline write
  } // loop over centerlines

  // write out the endpoints
  out << m_vEndpointsMM.size() << " # Number of endpoints" << endl;
  for (i=0; i<m_vEndpointsMM.size(); ++i)
    out.write((int1*)(&(m_vEndpointsMM[i])), sizeof(Point<float4>));

  // write out the pdr filenames
  out << m_vPdrFilenames.size() << " # Number of PDR filenames" << endl;
  for (i=0; i<m_vPdrFilenames.size(); ++i)
    out << m_vPdrFilenames[i] << endl;

  // write out the csi filenames
  out << m_vCsiFilenames.size() << " # Number of CSI filenames" << endl;
  for (i=0; i<m_vCsiFilenames.size(); ++i)
    out << m_vCsiFilenames[i] << endl;

  // write out a postfix message
  out << "End-of-segment" << endl;

  // make sure the disk is still ok after all that writing...whew
  if (out.bad())
    throw ex::IOException(LogRec("Trouble writing segment file.","Segment","Write"));

  return;
} // Write()


/**
 * Retrieves the number of bytes of the serialized data.
 *
 * @return the number of bytes in the serialized data.
 */
uint4 Segment::GetSerializedDataSize() const
{
  uint4 uSum = 0;

  // starting delimiter
  uSum += string(m_sDelimiter).length() + 1;

  // uint4 m_usIdentifier
  uSum += sizeof(uint4);

  // bool m_bRegularDirection
  uSum += sizeof(bool);

  // float m_fLength
  uSum += sizeof(float4);

  // float m_fOffsetDistanceFromRectum
  uSum += sizeof(float4);

  // bool m_bSelected
  uSum += sizeof(bool);

  // TriangleStrip m_triangleStrip
  uSum += m_triangleStrip.GetSerializedDataSize();
  
  // std::vector<Skeleton> m_vCenterlines
  uint4 i;
  uSum += sizeof(uint4); // m_vCenterlines.size()
  for (i=0; i<m_vCenterlines.size(); i++)
  {
    uSum += m_vCenterlines[i].GetSerializedDataSize();
  } // loop over all centerlines

  // std::vector< Point<float> > m_vEndpoints
  uSum += sizeof(uint4); // m_vEndpoints.size()
  uSum += m_vEndpointsMM.size() * sizeof(Point<float4>);

  // Point<float> m_startpoint
  uSum += sizeof(Point<float4>);

  // Point<float> m_endpoint
  uSum += sizeof(Point<float4>);

  // uint4 m_uCsiOffset
  uSum += sizeof(uint4);

  // std::vector<std::string> m_vPdrFilenames
  uSum += sizeof(uint4); // m_vPdrFilenames.size()
  for (i=0; i<m_vPdrFilenames.size(); i++)
  {
    uSum += m_vPdrFilenames[i].length() + 1; // + 1 for newlines
  } // loop over all pdrFilenames

  // std::vector<std::string> m_vCsiFilenames
  uSum += sizeof(uint4); // m_vCsiFilenames.size()
  for (i=0; i<m_vCsiFilenames.size(); i++)
  {
    uSum += m_vCsiFilenames[i].length() + 1; // + 1 for newlines
  } // loop over all csiFilenames

  // ending delimiter
  uSum += string(m_sDelimiter).length() + 1;

  return uSum;
} // GetSerializedDataSize()


/*
 * Retrieves all the data in serialized form.
 *
 * @param piBuffer a pointer to the pointer of the start of the output buffer.
 * @param piBufferEnd a pointer to the byte after the end of the valid output buffer range.
 * @return true upon success.
 */
void Segment::WriteSerializedData(int1 ** piBuffer, const int1 * piBufferEnd) const
{
  try
  {
    // ensure there is enough space
    if (*piBuffer + GetSerializedDataSize() > piBufferEnd)
      throw ex::OutOfMemoryException(LogRec("Not enough space","Segment","WriteSerializedData"));
    if (*piBuffer == NULL || piBufferEnd == NULL)
      throw ex::NullPointerException(LogRec("piBuffer is Null pointer","Segment","WriteSerializedData"));
  
    // write a delimiter to help verify the data
    SerializeStringData(piBuffer, piBufferEnd, m_sDelimiter);

    // unsigned short m_usIdentifier
    SerializeSimpleData(piBuffer, piBufferEnd, m_uIdentifier);

    // bool m_bRegularDirection
    SerializeSimpleData(piBuffer, piBufferEnd, m_bRegularDirection);

    // float m_fLength
    SerializeSimpleData(piBuffer, piBufferEnd, m_fLengthMM);

    // float m_fOffsetDistanceFromRectum
    SerializeSimpleData(piBuffer, piBufferEnd, m_fOffsetDistanceFromRectumMM);

    // bool m_bSelected
    SerializeSimpleData(piBuffer, piBufferEnd, m_bSelected);

    // TriangleStrip m_triangleStrip
    int1 * piBeforeWrite = *piBuffer;
    bool bResult = m_triangleStrip.WriteSerializedData(piBuffer, piBufferEnd);
    if (m_triangleStrip.GetSerializedDataSize() != static_cast<int4>(*piBuffer - piBeforeWrite))
      throw ex::OutOfMemoryException(LogRec("<Segment::WriteSerializedData> Bad buffer write.","Segment","WriteSerializedData"));
    if (!bResult)
      throw ex::DataCorruptionException(LogRec("<Segment::WriteSerializedData> Bad serialization.","Segment","WriteSerializedData"));
  
    // std::vector<Skeleton> m_vCenterlines
    uint4 uVectorSize = m_vCenterlines.size();
    SerializeSimpleData(piBuffer, piBufferEnd, uVectorSize);
	uint4 i=0;
    for (; i<m_vCenterlines.size(); i++)
    {
      piBeforeWrite = *piBuffer;
      try
      {
        m_vCenterlines[i].WriteSerializedData(piBuffer, piBufferEnd);
      }
      catch (...)
      {
        throw ex::DataCorruptionException(LogRec("<Segment::WriteSerializedData> Bad serialization.","Segment", "WriteSerializedData"));
      }
      if (m_vCenterlines[i].GetSerializedDataSize() != static_cast<int4>(*piBuffer - piBeforeWrite))
        throw ex::DataCorruptionException(LogRec("<Segment::WriteSerializedData> Bad buffer write.","Segment","WriteSerializedData"));
      if (bResult == false)
        throw ex::DataCorruptionException(LogRec("<Segment::WriteSerializedData> Bad serialization.","Segment","WriteSerializedData"));
    } // loop over all centerlines

    // std::vector< Point<float> > m_vEndpoints
    uVectorSize = m_vEndpointsMM.size();
    SerializeSimpleData(piBuffer, piBufferEnd, uVectorSize);
    for (i=0; i<uVectorSize; i++)
    {
      SerializeSimpleData(piBuffer, piBufferEnd, m_vEndpointsMM[i]);
    } // loop over all endpoints

    // Point<float> m_startpoint
    SerializeSimpleData(piBuffer, piBufferEnd, m_vStartpointMM);

    // Point<float> m_endpoint
    SerializeSimpleData(piBuffer, piBufferEnd, m_vEndpointMM);

    // uint4 m_uiCsiOffset
    SerializeSimpleData(piBuffer, piBufferEnd, m_uCsiOffset);

    // std::vector<std::string> m_vPdrFilenames
    uVectorSize = m_vPdrFilenames.size();
    SerializeSimpleData(piBuffer, piBufferEnd, uVectorSize);
    for (i=0; i<m_vPdrFilenames.size(); i++)
    {
      SerializeStringData(piBuffer, piBufferEnd, m_vPdrFilenames[i]);
    } // loop over all pdrFilenames

    // std::vector<std::string> m_vCsiFilenames
    uVectorSize = m_vCsiFilenames.size();
    SerializeSimpleData(piBuffer, piBufferEnd, uVectorSize);
    for (i=0; i<m_vCsiFilenames.size(); i++)
    {
      SerializeStringData(piBuffer, piBufferEnd, m_vCsiFilenames[i]);
    } // loop over all csiFilenames

    // write a delimiter to help verify the data
    SerializeStringData(piBuffer, piBufferEnd, m_sDelimiter);
  }
  catch (ex::VException & ep)
  {
    throw ex::VException(LogRec(ep.GetLogRecord().GetTextMessage(), "Segment", "WriteSerializedData"));
  }
  catch (...)
  {
    throw ex::VException(LogRec("An unspecified error caught at stage 'writing segments data set'", "Segment", "WriteSerializedData"));
  }

  return;
} // WriteSerializedData()


/**
 * Reads in all the data from serialized form.
 *
 * @param piBuffer a pointer to the pointer of the start of the input buffer.
 * @param piBufferEnd a pointer to the byte after the end of the valid input buffer range.
 * @return true upon success.
 */
void Segment::ReadSerializedData(int1 ** piBuffer, const int1 * piBufferEnd)
{
  try
  {
    if (*piBuffer == NULL || piBufferEnd == NULL)
      throw ex::NullPointerException(LogRec("segment: Null pointer to segment buffer","Segment","ReadSerializedData"));

    // attempt to verify the stream by reading a delimiter
    string delimiter;
    delimiter = DeserializeStringData(piBuffer, piBufferEnd);
    if (delimiter != m_sDelimiter)
      throw ex::DataCorruptionException(LogRec("Segment read: data corruption: bad starting delimiter","Segment","ReadSerializedData"));
 
    // uint4 m_uIdentifier
    DeserializeSimpleData(piBuffer, piBufferEnd, m_uIdentifier);
    
    // bool m_bRegularDirection
    DeserializeSimpleData(piBuffer, piBufferEnd, m_bRegularDirection);

    // float4 m_fLength
    DeserializeSimpleData(piBuffer, piBufferEnd, m_fLengthMM);

    // float4 m_fOffsetDistanceFromRectum
    DeserializeSimpleData(piBuffer, piBufferEnd, m_fOffsetDistanceFromRectumMM);

    // bool m_bSelected
    DeserializeSimpleData(piBuffer, piBufferEnd, m_bSelected);

    // TriangleStrip m_triangleStrip
    m_triangleStrip.Clear();
    bool bResult = m_triangleStrip.ReadSerializedData(piBuffer, piBufferEnd);
    if (!bResult)
      throw ex::DataCorruptionException(LogRec("Segment:Triangle strips: data corruption::Bad deserialing triange strips","Segment","ReadSerializedData"));
  
    // std::vector<Skeleton> m_vCenterlines
    uint4 uVectorSize;
    DeserializeSimpleData(piBuffer, piBufferEnd, uVectorSize);

    m_vCenterlines.resize(uVectorSize);
	uint4 i=0;
    for (; i<m_vCenterlines.size(); i++)
    {
      m_vCenterlines[i].ReadSerializedData(piBuffer, piBufferEnd);
    } // loop over all centerlines

    // std::vector< Point<float> > m_vEndpoints
    DeserializeSimpleData(piBuffer, piBufferEnd, uVectorSize);
    m_vEndpointsMM.resize(uVectorSize);
    for (i=0; i<m_vEndpointsMM.size(); i++)
    {
      DeserializeSimpleData(piBuffer, piBufferEnd, m_vEndpointsMM[i]);
    } // loop over all endpoints

    // Point<float> m_startpoint
    DeserializeSimpleData(piBuffer, piBufferEnd, m_vStartpointMM);

    // Point<float> m_endpoint
    DeserializeSimpleData(piBuffer, piBufferEnd, m_vEndpointMM);

    // initialize the start and end points
    if (m_vEndpointsMM.size() > 0)
      m_vStartpointMM = m_vEndpointsMM[0];
    if (m_vEndpointsMM.size() > 1)
      m_vEndpointMM = m_vEndpointsMM[1];

    // uint4 m_uiCsiOffset
    DeserializeSimpleData(piBuffer, piBufferEnd, m_uCsiOffset);

    // std::vector<std::string> m_vPdrFilenames
    DeserializeSimpleData(piBuffer, piBufferEnd, uVectorSize);
    m_vPdrFilenames.resize(uVectorSize);
    for (i=0; i<m_vPdrFilenames.size(); i++)
      m_vPdrFilenames[i] = DeserializeStringData(piBuffer, piBufferEnd);

    // std::vector<std::string> m_vCsiFilenames
    DeserializeSimpleData(piBuffer, piBufferEnd, uVectorSize);
    m_vCsiFilenames.resize(uVectorSize);
    for (i=0; i<m_vCsiFilenames.size(); i++)
      m_vCsiFilenames[i] = DeserializeStringData(piBuffer, piBufferEnd);

    // attempt to verify the stream by reading a delimiter
    delimiter = DeserializeStringData(piBuffer, piBufferEnd);
    if (delimiter != m_sDelimiter)
      throw ex::DataCorruptionException(LogRec("Segment: data corruption: bad ending delimiter","Segment","ReadSerializedData"));
  }
  catch (ex::VException)
  {
    throw ex::VException(LogRec("ReadSerializedData","Segment","ReadSerializedData"));
  }
  catch (...)
  {
    throw ex::VException(LogRec("An unspecified error caught at stage 'reading segments data set'", "Segment", "ReadSerializedData"));
  }

  return;
} // ReadSerializedData()


/**
 * Equality operator.
 *
 * @param other the other segment for comparison purposes.
 * @return true if they are identical.
 */
bool Segment::operator ==(const Segment & other) const
{
  // unsigned short m_usIdentifier
  if (m_uIdentifier != other.m_uIdentifier)
  {
    LogWrn("m_usIdentifier varies.","Segment","operator==");
    return false;
  } // if different

  // bool m_bRegularDirection
  if (m_bRegularDirection != other.m_bRegularDirection)
  {    
    LogWrn("m_bRegularDirection varies.","Segment","operator==");
    return false;
  } // if different

  // float m_fLength
  if (m_fLengthMM != other.m_fLengthMM)
  {
    LogWrn("m_fLengthMM varies.","Segment","operator==");
   
    return false;
  } // if different

  // float m_fVolumeCC
  if( (m_fAirVolumeCC != other.m_fAirVolumeCC) || (m_fTotalVolumeCC != other.m_fTotalVolumeCC) )
  {
    LogWrn("m_fVolumeCC varies.","Segment","operator==");
   
    return false;
  } // if different

  // float m_fOffsetDistanceFromRectumMM
  if (m_fOffsetDistanceFromRectumMM != other.m_fOffsetDistanceFromRectumMM)
  {
    LogWrn("m_fOffsetDistanceFromRectumMM varies.","Segment","operator==");
    
    return false;
  } // if different

  // bool m_bSelected
  if (m_bSelected != other.m_bSelected)
  {
    LogWrn("m_bSelected varies.","Segment","operator==");
    
    return false;
  } // if different

  // TriangleStrip m_triangleStrip
  if (!(m_triangleStrip == other.m_triangleStrip))
  {
    LogWrn("m_triangleStrip varies.","Segment","operator==");
    
    return false;
  } // if different
  
  // int m_triDisplayList
  // this is going to be different anyways

  // std::vector<Skeleton> m_vCenterlines
  if (m_vCenterlines.size() != other.m_vCenterlines.size())
  {
    LogWrn("Number of centerlines varies.","Segment","operator==");
    
    return false;
  } // if different
  uint4 i;
  for (i=0; i<m_vCenterlines.size(); i++)
  {
    if (!(m_vCenterlines[i] == other.m_vCenterlines[i]))
    {
      LogWrn("Content of centerlines varies.","Segment","operator==");
      
      return false;
    } // if different
  } // loop over all centerlines

  // std::vector< Point<float> > m_vEndpoints
  if (m_vEndpointsMM.size() != other.m_vEndpointsMM.size())
  {
    LogWrn("Number of endpoints varies.","Segment","operator==");
   
    return false;
  } // if different
  for (i=0; i<m_vEndpointsMM.size(); i++)
  {
    if (m_vEndpointsMM[i] != other.m_vEndpointsMM[i])
    {
      LogWrn("Content of endpoints varies.","Segment","operator==");
      
      return false;
    } // if different
  } // loop over all endpoints

  // Point<float> m_startpoint
  if (m_vStartpointMM != other.m_vStartpointMM)
  {
    LogWrn("m_vStartpointMM varies.","Segment","operator==");
   
    return false;
  } // if different

  // Point<float> m_endpoint
  if (m_vEndpointMM != other.m_vEndpointMM)
  {
    LogWrn("m_vEndpointMM varies.","Segment","operator==");
    
    return false;
  } // if different

  // uint4 m_uiCsiOffset
  if (m_uCsiOffset != other.m_uCsiOffset)
  {
    LogWrn("m_uCsiOffset varies.","Segment","operator==");
   
    return false;
  } // if different

  // std::vector<std::string> m_vPdrFilenames
  if (m_vPdrFilenames.size() != other.m_vPdrFilenames.size())
  {
    LogWrn("Number of pdrFilenames varies.","Segment","operator==");
    
    return false;
  } // if different
  for (i=0; i<m_vPdrFilenames.size(); i++)
  {
    if (m_vPdrFilenames[i] != other.m_vPdrFilenames[i])
    {
      LogWrn("Content of pdrFilenames varies.","Segment","operator==");
      
      return false;
    } // if different
  } // loop over all pdr filenames

  // std::vector<std::string> m_vCsiFilenames
  if (m_vCsiFilenames.size() != other.m_vCsiFilenames.size())
  {
    LogWrn("Number of csiFilenames varies.","Segment","operator==");
    
    return false;
  } // if different
  for (i=0; i<m_vCsiFilenames.size(); i++)
  {
    if (m_vCsiFilenames[i] != other.m_vCsiFilenames[i])
    {
      LogWrn("Content of csiFilenames varies.","Segment","operator==");
      
      return false;
    } // if different
  } // loop over all csi filenames

  return true;
} // operator ==()


/**
 * Initializes a segment based on centerlines and triangle strip data.
 *
 * @param uSegmentNumber the identifier of the segment.
 * @param vSkeletons a vector of skeletons for the new segment.
 * @param sFilenameRoot the root filename.
 */
bool Segment::Initialize(const uint4 uSegmentNumber, std::vector<Skeleton> & vSkeletons, const std::string & sFilenameRoot)
{
  // check for at least two centerlines
  if (vSkeletons.size() < 2)
  {
    
    LogWrn("not enough centerlines.","Segment","Initialize");
    return false;
  } // if not enough centerlines

  // initialize members
  m_vCenterlines.push_back(vSkeletons[0]);
  m_vCenterlines.push_back(vSkeletons[1]);
  m_uCsiOffset = 0;
  m_vEndpointsMM.push_back(vSkeletons[0].GetSkelNode(0).m_vWorldPos);
  m_vEndpointsMM.push_back(vSkeletons[0].GetSkelNode(vSkeletons[0].GetNumOfElements()-1).m_vWorldPos);
  m_uIdentifier = uSegmentNumber;
  m_fLengthMM = vSkeletons[0].GetPiecewiseEuclideanLength();
	m_fAirVolumeCC = 0.0F;
	m_fTotalVolumeCC = 0.0F;
  m_fOffsetDistanceFromRectumMM = 0;
  m_bRegularDirection = true;
  m_bSelected = true;

  // read in the triangles from disk
  stringstream ss;
  ss << sFilenameRoot << "_" << uSegmentNumber << vx::TriangleExt.GetExtensionAsString();
  m_triangleStrip.ReadTriangleStripDataSet(ss.str());
  m_triangleStrip.RemoveTriangleFile(ss.str());

  return true;
} // Initialize()


/**
 * Adds a centerline to the list of centerlines.
 *
 * @param centerline the centerline to add to the list of centerlines.
 */
void Segment::AddCenterline(const Skeleton & centerline)
{
  m_vCenterlines.push_back(centerline);
} // AddCenterline()


/**
 * Rotate a prone segment list to supine
 *
 * @param worldSize the size of the entire volume in MM
 */
void Segment::RotateProneToSupine( const Triple<float4> & worldSize )
{

  m_triangleStrip.RotateProneToSupine( worldSize );

  for ( std::vector<Skeleton>::iterator iter = m_vCenterlines.begin(); iter != m_vCenterlines.end(); ++iter )
    iter->RotateProneToSupine( worldSize );

  for ( std::vector< Point< float4 > >::iterator iter2 = m_vEndpointsMM.begin(); iter2 != m_vEndpointsMM.end(); ++iter2 )
  {
    iter2->m_x = worldSize.m_x - iter2->m_x;
    iter2->m_y = worldSize.m_y - iter2->m_y;
  }

  m_vStartpointMM.m_x = worldSize.m_x - m_vStartpointMM.m_x;
  m_vStartpointMM.m_y = worldSize.m_y - m_vStartpointMM.m_y;

  m_vEndpointMM.m_x = worldSize.m_x - m_vEndpointMM.m_x;
  m_vEndpointMM.m_y = worldSize.m_y - m_vEndpointMM.m_y;

} // RotateProneToSupine()


// undefines
#undef FILE_REVISION 


// $Log: Segment.C,v $
// Revision 1.3.2.1  2010/11/15 20:37:29  dongqing
// add codes for measuring air lumen volume of selected segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3  2006/09/13 15:23:40  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.2  2006/06/01 20:28:29  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.2.18.1  2005/06/28 12:06:18  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.2  2002/07/03 03:23:21  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.1.2.1  2002/07/02 20:51:51  jmeade
// Code standards.
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
//    Rev 2.0   Sep 16 2001 23:49:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:42   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.62  2001/08/20 20:53:21  ingmar
// some new syntax exceptions
//
// Revision 1.61  2001/08/20 20:15:56  ingmar
// some new syntax exceptions
//
// Revision 1.60  2001/08/13 20:05:44  tushar
// removed errors due to the new logger
//
// Revision 1.59  2001/08/07 17:02:57  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.58  2001/07/26 20:37:14  binli
// ID 89: exception handling.
//
// Revision 1.57  2001/07/25 02:43:51  sachin
// fix of exceptions
//
// Revision 1.56  2001/07/16 23:47:12  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.55  2001/07/13 19:52:16  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.54  2001/07/12 21:14:14  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.53  2001/07/03 14:57:35  soeren
// removed stdafx.c and .h
//
// Revision 1.52  2001/05/19 00:38:38  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.51  2001/05/04 15:31:14  binli
// moved the displaylist of triangle to OverviewRender.
//
// Revision 1.50  2001/05/02 21:39:52  jmeade
// corrections after mergeLostApril26 merge
//
// Revision 1.49  2001/05/02 19:32:49  ingmar
// Segment.C
//
// Revision 1.47.2.1  2001/05/01 21:48:28  jmeade
// Reflected changes in Skeleton class.
//
// Revision 1.48  2001/04/26 11:17:53  frank
// Reflected changes in Skeleton class.
//
// Revision 1.47  2001/04/13 00:33:01  jmeade
// file extensions
//
// Revision 1.46  2001/04/06 13:54:01  frank
// Removed all pointers.
//
// Revision 1.45  2001/04/06 12:15:34  frank
// Changes according to code walkthrough.
//
// Revision 1.44  2001/04/04 14:32:58  frank
// More comments.
//
// Revision 1.43  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.42  2001/03/28 13:56:25  frank
// Added function to add a centerline to the segment.
//
// Revision 1.41  2001/03/26 17:21:10  frank
// Moved serialization functions to separate namespace.
//
// Revision 1.40  2001/03/26 16:26:50  frank
// Added function to initialize a segment.
//
// Revision 1.39  2001/03/26 15:31:38  frank
// Working on coding compliance.
//
// Revision 1.38  2001/03/23 16:37:02  frank
// Fixed some I/O problems with new variable types.
//
// Revision 1.37  2001/03/19 18:03:07  frank
// Updated to reflect new coding standards.
//
// Revision 1.36  2001/02/22 01:34:58  jeff
// code without coding conventions is gettting harder and harder to comprehend
//
// Revision 1.35  2001/02/12 17:13:34  jeff
// throw viatronix exceptions for V1K errors instead of aborting
//
// Revision 1.34  2001/02/10 20:19:31  soeren
// Removed double Error::Error::
//
// Revision 1.33  2001/01/19 21:07:09  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.32  2000/12/19 17:08:26  sarang
// gcc/linux fixes
//
// Revision 1.31  2000/12/18 19:51:53  frank
// Removed stringstream I/O
//
// Revision 1.30  2000/12/14 14:51:33  frank
// Modified the interface for serialization/deserialization
//
// Revision 1.29  2000/12/13 17:27:44  frank
// Moved to new png data format
//
// Revision 1.28  2000/12/13 15:06:44  frank
// Added equality operators
//
// Revision 1.27  2000/12/12 16:07:48  frank
// Fixed bugs in serialization and deserialization
//
// Revision 1.26  2000/12/12 12:08:02  frank
// Added delimiters to serialized data stream
//
// Revision 1.25  2000/12/09 18:17:25  frank
// Added serialized input
//
// Revision 1.24  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.23  2000/12/09 15:38:51  frank
// Updated segment reading for new file format
//
// Revision 1.22  2000/12/09 14:43:54  frank
// Stream I/O was too slow, went to temp binary file
//
// Revision 1.21  2000/12/09 12:55:32  frank
// Improved stream I/O
//
// Revision 1.20  2000/12/05 19:59:42  frank
// Temporarily removed stream I/O functions
//
// Revision 1.19  2000/12/05 19:58:28  frank
// Added stream I/O functions
//
// Revision 1.18  2000/11/22 18:39:03  binli
// added function for display list delete.
//
// Revision 1.17  2000/11/22 17:14:59  binli
// fixed display list for strips rendering.
//
// Revision 1.16  2000/11/17 18:28:54  frank
// Added Clear for tristrip member
//
// Revision 1.15  2000/11/09 14:54:30  frank
// Increased robustness
//
// Revision 1.14  2000/11/03 19:10:49  ingmar
// fixed gcc warnings
//
// Revision 1.13  2000/11/03 12:59:40  frank
// Fixed triangle file path
//
// Revision 1.12  2000/11/02 12:20:24  frank
// Fixed triangle file path
//
// Revision 1.11  2000/10/31 19:40:44  dave
// compiles in gcc
//
// Revision 1.10  2000/10/31 14:15:07  frank
// Merged in new segment structure
//
// Revision 1.9  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.8.8.4  2000/10/30 19:13:17  frank
// fixed typos
//
// Revision 1.8.8.3  2000/10/30 17:05:57  frank
// merged back using old v2k branch
//
// Revision 1.8.8.2  2000/10/28 19:31:06  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.8.8.1  2000/10/28 17:38:58  ingmar
// new ierators for new segments
//
// Revision 1.8.4.3  2000/10/28 01:07:34  ingmar
// frank's last changes
//
// Revision 1.8.4.2  2000/10/27 15:20:54  frank
// Got segment structure working
//
// Revision 1.8.4.1  2000/10/26 13:28:31  frank
// Added new segment structure
//
// Revision 1.8  2000/10/12 02:45:05  kevin
// FIxed OffsetDistanceFromRectum
//
// Revision 1.7  2000/10/11 14:46:43  kevin
// Added jump to begining/end of skeleton
//
// Revision 1.6.2.2  2000/10/26 14:30:06  frank
// Added new segment structure
//
// Revision 1.6.2.1  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.6  2000/10/02 16:07:47  frank
// Improved SegmentList I/O
//
// Revision 1.5  2000/10/02 15:51:41  ingmar
// fixed gcc complaints
//
// Revision 1.4  2000/09/30 15:00:17  frank
// Made functions jump-to-able within VC++
//
// Revision 1.3  2000/09/28 17:10:00  frank
// Added I/O for segments and segment lists
//
// Revision 1.2  2000/09/27 18:13:53  frank
// Fixed typo
//
// Revision 1.1  2000/09/27 15:07:52  frank
// Added segment data structures
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
//
// 3     8/28/00 11:50a Antonio
// updated header
//
//*****************  Version 2  *****************
//User: Frank           Date:  8/08/00  Time:  2:23p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Broke Segment class out into it's own class
//
//*****************  Version 1  *****************
//User: Frank           Date:  8/08/00  Time:  2:21p
//Created Segment.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Segment.C,v 1.3.2.1 2010/11/15 20:37:29 dongqing Exp $
// $Id: Segment.C,v 1.3.2.1 2010/11/15 20:37:29 dongqing Exp $
