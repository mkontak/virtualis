// $Id: vxVesselSection.C,v 1.3 2004/06/17 14:00:15 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


/**
 * A section of a tubular vessel object containing a local coordinate
 * system plus the vessel wall contour.
 */


// Includes
#include "stdafx.h"
#include "utilities.h"
#include "vxVesselSection.h"
#include "ValueRange.h"
#include "Vector4D.h"
#include "Triangle.h"
#include "Serialization.h"
#include "MemoryBuffer.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


// Namespaces
using namespace Serialization;
using namespace std;


// Static member initialization
const std::string vxVesselSection::m_sSectionDelimiter( "vxVesselSection" );
const std::string vxVesselSection::m_sContourDelimiter( "contour" );
const float4 vxVesselSection::m_fVersion( 1.0F );


/**
 * Constructor
 */
vxVesselSection::vxVesselSection() :
m_fLumenArea( 0.0F ),
m_fWallArea( 0.0F ),
m_fAccumulatedVolumeCubicMM( 0.0F ),
m_fAccumulatedLengthMM( 0.0F ),
m_diameterMin( Vector4Df(0,0,0,0), Vector4Df(0,0,0,1) ),
m_diameterMax( Vector4Df(0,0,0,0), Vector4Df(0,0,0,1) )
{
} // vxVesselSection()


/**
 * Retrieves the number of bytes of the serialized data.
 */
uint4 vxVesselSection::GetSerializedDataSize() const
{
  uint4 uSum(0);

  // start delimiter
  uSum += m_sSectionDelimiter.length() + 1;

  // version
  uSum += sizeof( m_fVersion );

  // m_position
  uSum += sizeof(m_position);

  // m_tangent
  uSum += sizeof(m_tangent);

  // m_up
  uSum += sizeof(m_up);

  // m_lumenContour size
  uSum += sizeof(uint4);

  // m_lumenContour
  uSum += sizeof(Point3Df) * m_lumenContour.size();

  // m_wallContour size
  uSum += sizeof(uint4);

  // m_wallContour
  uSum += sizeof(Point3Df) * m_wallContour.size();

  // m_fLumenArea
  uSum += sizeof(m_fLumenArea);

  // m_fWallArea
  uSum += sizeof(m_fWallArea);

  // m_fVolumeCubicMM
  uSum += sizeof(m_fAccumulatedVolumeCubicMM);

  // m_fLengthMM
  uSum += sizeof(m_fAccumulatedLengthMM);

  // m_fCurvatureDegree2MM
  uSum += sizeof(m_fCurvatureDegree2MM);

  // m_diameterMin
  uSum += sizeof( m_diameterMin );

  // m_diameterMax
  uSum += sizeof( m_diameterMax );

  // end delimiter
  uSum += m_sSectionDelimiter.length() + 1;

  return uSum;
} // GetSerializedDataSize()


/**
 *  Writes all the data in serialized form.
 *
 *  @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 *  @param piBufferEnd pointer to the end of strips memory buffer.
 *  @return            true when success
 */
void vxVesselSection::WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const
{
  // ensure there is enough space
  if (*hiBuffer + GetSerializedDataSize() > piBufferEnd)
    throw ex::VException(LogRec("Not enough space." ,"vxVesselSection", "WriteSerializedData"));

  /// start delimiter
  SerializeStringData(hiBuffer, piBufferEnd, m_sSectionDelimiter);

  // version
  SerializeSimpleData(hiBuffer, piBufferEnd, m_fVersion);

  // m_position
  SerializeSimpleData(hiBuffer, piBufferEnd, m_position);

  // m_tangent
  SerializeSimpleData(hiBuffer, piBufferEnd, m_tangent);

  // m_up
  SerializeSimpleData(hiBuffer, piBufferEnd, m_up);

  // m_lumenContour and its size
  uint4 uSize(m_lumenContour.size());
  SerializeSimpleData(hiBuffer, piBufferEnd, uSize);
  for ( int4 i=0; i<uSize; i++ )
    SerializeSimpleData(hiBuffer, piBufferEnd, m_lumenContour[i]);

  // m_wallContour and its size
  uSize = m_wallContour.size();
  SerializeSimpleData(hiBuffer, piBufferEnd, uSize);
  for ( int4 i=0; i<uSize; i++ )
    SerializeSimpleData(hiBuffer, piBufferEnd, m_wallContour[i]);

  // m_fLumenArea
  SerializeSimpleData(hiBuffer, piBufferEnd, m_fLumenArea);

  // m_fWallArea
  SerializeSimpleData(hiBuffer, piBufferEnd, m_fWallArea);

  // m_fVolumeCubicMM
  SerializeSimpleData(hiBuffer, piBufferEnd, m_fAccumulatedVolumeCubicMM);

  // m_fLengthMM
  SerializeSimpleData(hiBuffer, piBufferEnd, m_fAccumulatedLengthMM);

  // m_fCurvatureDegree2MM
  SerializeSimpleData(hiBuffer, piBufferEnd, m_fCurvatureDegree2MM);

  // m_diameterMin
  SerializeSimpleData(hiBuffer, piBufferEnd, m_diameterMin);

  // m_diameterMax
  SerializeSimpleData(hiBuffer, piBufferEnd, m_diameterMax);

  /// end delimiter
  SerializeStringData(hiBuffer, piBufferEnd, m_sSectionDelimiter);

} // WriteSerializedData()


/**
 *  Reads in all the data from serialized form.
 *
 *  @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 *  @param piBufferEnd pointer to the end of memory buffer
 *  @return            true when success
 */
void vxVesselSection::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  try
  {
    // verify the stream by reading start delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sSectionDelimiter)
      throw ex::DataCorruptionException( LogRec( "bad starting delimiter", "vxVesselSection", "ReadSerializedData" ) );
 
    // version
    float4 fDiskVersion( 0.0F );
    DeserializeSimpleData(hiBuffer, piBufferEnd, fDiskVersion);
    if( fDiskVersion != m_fVersion )
      throw ex::UnsupportedDataTypeException( LogRec( "Unsupported file version: " + ToAscii( fDiskVersion ), "vxVesselSection", "ReadSerializedData" ) );

    // m_position
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_position);

    // m_tangent
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_tangent);

    // m_up
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_up);

    // m_lumenContour
    uint4 uSize;
    DeserializeSimpleData(hiBuffer, piBufferEnd, uSize);
    m_lumenContour.resize(uSize);
    for ( int4 i=0; i<uSize; i++ )
      DeserializeSimpleData(hiBuffer, piBufferEnd, m_lumenContour[i]);

    // m_wallContour
    DeserializeSimpleData(hiBuffer, piBufferEnd, uSize);
    m_wallContour.resize(uSize);
    for ( int4 i=0; i<uSize; i++ )
      DeserializeSimpleData(hiBuffer, piBufferEnd, m_wallContour[i]);

    // m_fLumenArea
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_fLumenArea);

    // m_fWallArea
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_fWallArea);

    // m_fVolumeCubicMM
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_fAccumulatedVolumeCubicMM);

    // m_fLengthMM
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_fAccumulatedLengthMM);

    // m_fCurvatureDegree2MM
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_fCurvatureDegree2MM);

    // m_diameterMin
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_diameterMin);

    // m_diameterMax
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_diameterMax);

    // verify the stream by reading end delimiter
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sSectionDelimiter)
      throw ex::DataCorruptionException( LogRec( "bad ending delimiter", "vxVesselSection", "ReadSerializedData" ) );

  }
  catch (ex::OutOfMemoryException)
  {
    throw ex::OutOfMemoryException( LogRec( "Out of memory", "VxVesselSection", "ReadSerializedData" ) );
  }
  catch (ex::VException & error)
  {
    throw ex::VException( LogRec( error.GetLogRecord().GetTextMessage(), "VxVesselSection", "ReadSerializedData" ) );
  }
} // ReadSerializedData()


/**
 * Retrieves the number of bytes of a serialized contour
 */
uint4 vxVesselSection::GetSerializedContourSize( const std::vector<Point3Df> & contour )
{

  uint4 uSum(0);

  // start delimiter
  uSum += m_sContourDelimiter.length() + 1;

  // version
  const float4 fVersion( 1.0F );
  uSum += sizeof( fVersion );

  // contour size
  uSum += sizeof(uint4);

  // contour
  uSum += sizeof(Point3Df) * contour.size();

  // end delimiter
  uSum += m_sContourDelimiter.length() + 1;

  return uSum;
  
} // GetSerializedContourSize()


/**
 * Writes all the data of a contour
 *
 * @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 * @param piBufferEnd pointer to the end of strips memory buffer.
 * @param contour     the input contour
 */
void vxVesselSection::WriteSerializedContour( int1 ** hiBuffer, const int1 * piBufferEnd, const std::vector<Point3Df> & contour )
{

  // ensure there is enough space
  if (*hiBuffer + GetSerializedContourSize( contour ) > piBufferEnd)
    throw ex::VException(LogRec("Not enough space." ,"vxVesselSection", "WriteSerializedContour"));

  /// start delimiter
  SerializeStringData(hiBuffer, piBufferEnd, m_sContourDelimiter);

  // version
  SerializeSimpleData(hiBuffer, piBufferEnd, m_fVersion);

  // contour size
  uint4 uSize( contour.size() );
  SerializeSimpleData(hiBuffer, piBufferEnd, uSize);
  
  // contour points
  for ( int4 i=0; i<uSize; i++ )
    SerializeSimpleData(hiBuffer, piBufferEnd, contour[i]);

  /// end delimiter
  SerializeStringData(hiBuffer, piBufferEnd, m_sContourDelimiter);

} // WriteSerializedContour()


/**
 * Reads in a contour from serialized form
 *
 * @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 * @param piBufferEnd pointer to the end of memory buffer
 * @return            resulting contour
 */
std::vector<Point3Df> vxVesselSection::ReadSerializedContour( int1 ** hiBuffer, const int1 * piBufferEnd )
{

  std::vector<Point3Df> contour;

  try
  {

    // verify the stream by reading start delimiter
    string sStartingDelimiter( DeserializeStringData(hiBuffer, piBufferEnd) );
    if ( sStartingDelimiter != m_sContourDelimiter )
      throw ex::DataCorruptionException( LogRec( "bad starting delimiter: " + sStartingDelimiter, "vxVesselSection", "ReadSerializedContour" ) );

    // version
    float4 fDiskVersion( 0.0F );
    DeserializeSimpleData(hiBuffer, piBufferEnd, fDiskVersion);
    if( fDiskVersion != m_fVersion )
      throw ex::UnsupportedDataTypeException( LogRec( "Unsupported file version: " +
      ToAscii( fDiskVersion ), "vxVesselSection", "ReadSerializedContour" ) );

    // contour size
    uint4 uSize;
    DeserializeSimpleData(hiBuffer, piBufferEnd, uSize);

    // contour points
    contour.resize(uSize);
    for ( int4 i=0; i<uSize; i++ )
      DeserializeSimpleData(hiBuffer, piBufferEnd, contour[i]);

    // verify the stream by reading end delimiter
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sContourDelimiter)
      throw ex::DataCorruptionException( LogRec( "bad ending delimiter", "vxVesselSection", "ReadSerializedContour" ) );

  } // try
  catch (ex::OutOfMemoryException)
  {
    throw ex::OutOfMemoryException( LogRec( "Out of memory", "VxVesselSection", "ReadSerializedContour" ) );
  }
  catch (ex::VException & error)
  {
    throw ex::VException( LogRec( error.GetLogRecord().GetTextMessage(), "VxVesselSection", "ReadSerializedContour" ) );
  }
  
  return contour;

} // ReadSerializedContour()


/**
 * Equality operator
 * @param other the vessel section we are to compare to
 * @return      true if they are equal
 */
bool vxVesselSection::operator ==(const vxVesselSection & other) const
{
  bool bResult(true);
  bResult &= (m_position == other.m_position);
  bResult &= (m_tangent == other.m_tangent);
  bResult &= (m_up == other.m_up);
  bResult &= (m_lumenContour.size() == other.m_lumenContour.size());
  for ( int4 i=0; i<m_lumenContour.size(); i++ )
    bResult &= (m_lumenContour[i] == other.m_lumenContour[i]);
  bResult &= (m_wallContour.size() == other.m_wallContour.size());
  for ( int4 i=0; i<m_wallContour.size(); i++ )
    bResult &= (m_wallContour[i] == other.m_wallContour[i]);
  bResult &= (m_fLumenArea == other.m_fLumenArea);
  bResult &= (m_fWallArea == other.m_fWallArea);
  bResult &= (m_fAccumulatedVolumeCubicMM == other.m_fAccumulatedVolumeCubicMM);
  bResult &= (m_fAccumulatedLengthMM == other.m_fAccumulatedLengthMM);
  bResult &= (m_diameterMin == other.m_diameterMin);
  bResult &= (m_diameterMax == other.m_diameterMax);

  return bResult;
} // operator ==()


/**
 * Inequality operator
 * @param other  the vessel section we are to compare to
 * @return       true if they are not equal
 */
bool vxVesselSection::operator !=(const vxVesselSection & other) const
{
  return ! (*this == other);
} // operator !=()


/**
 * Comparison operator
 *
 * @param other the other vessel section
 *
 * @return this < other
 */
bool vxVesselSection::operator <( const vxVesselSection & other ) const
{
  return m_fAccumulatedLengthMM < other.m_fAccumulatedLengthMM;
} // operator <()


/**
 * Get the minimum and maximum diameter of the section in millimeters.
 * @return the min and max diameters
 */
ValueRange< float4 > vxVesselSection::GetDiameterRange() const
{
  ValueRange<float4> diameters;
  diameters.AddValue( m_diameterMin.GetLength() );
  diameters.AddValue( m_diameterMax.GetLength() );
  return diameters;
} // GetDiameterRange()


/**
 * Get the < min, max > diameter points
 * @return the two line segments representing the min and max diameters
 */
vxVesselSection::RaySegmentPair vxVesselSection::GetDiameterPoints() const
{
  return make_pair( m_diameterMin, m_diameterMax );
} // GetDiameterPoints()


/**
 * Set the < min, max > diameter points
 * @param pairs the two line segments representing the min and max diameters
 */
void vxVesselSection::SetDiameterPoints( const RaySegmentPair & pairs )
{
  m_diameterMin = pairs.first;
  m_diameterMax = pairs.second;
} // GetDiameterPoints()


/**
 * Calculate the max diameter points of the contour.
 * @return the maximum diameter line segment
 */
RaySegment vxVesselSection::CalculateMaxDiameterPoints() const
{
  RaySegment segment( Vector4Df(0,0,0,0), Vector4Df(1,0,0,1) );
  try
  {
    if ( m_lumenContour.size() == 0 )
      return segment;

    // compute the max diameter - brute force (try all pairs)
    ValueRange< float4 > diameters;
    std::pair< uint4, uint4 > uMaxIndices = make_pair(0,0);

    // loop over all pairs
    for( uint4 i(0); i<m_lumenContour.size(); ++i )
    {
      for( uint4 j(i+1); j<m_lumenContour.size(); ++j )
      {
        // check the lengths
        const float4 fLength( ( m_lumenContour[i] - m_lumenContour[j] ).GetLength() );
        if( fLength > diameters.GetMaximum() )
          uMaxIndices = make_pair( i, j );
        diameters.AddValue( fLength );
      }
    }

    segment = RaySegment( Vector4Df(m_lumenContour[uMaxIndices.first],0), Vector4Df(m_lumenContour[uMaxIndices.second], 1) );
  }
  catch ( ... )
  {
    throw ex::VException(LogRec("unhandled exception" ,"vxVesselSection", "CalculateMaxDiameterPoints"));
  }

  return segment;
} // RecalculateMaxDiameterPoints()


/**
 * Calculate the min diameter points of the contour.
 */
RaySegment vxVesselSection::CalculateMinDiameterPoints() const
{
  RaySegment segment( Vector4Df(0,0,0,0), Vector4Df(1,0,0,1) );
  try
  {
    if ( m_lumenContour.size() == 0 )
      return segment;

    // now go over all the pairs of points that are perpendicular to the major diameter
    const Vector4Df dir4D( m_diameterMax.GetDirection() );
    if ( dir4D.GetLengthXYZ() < 1.0e-5F )
    {
      return segment;
    }
    const Normal<float4> majorDirection( dir4D.X(), dir4D.Y(), dir4D.Z() );
    std::pair< uint4, uint4 > uMinIndices = make_pair(0,0);
    ValueRange< float4 > maxLength;
    for( uint4 i(0); i<m_lumenContour.size(); ++i )
    {
      const uint4 uMostPerpendicularIndex( GetMostPerpendicularSegmentIndex( majorDirection, i ) );

      // see if this pair of points forms the largest line segment
      const float4 fLength( ( m_lumenContour[i] - m_lumenContour[uMostPerpendicularIndex] ).GetLength() );
      if( fLength > maxLength.GetMaximum() )
        uMinIndices = make_pair( i, uMostPerpendicularIndex );
      maxLength.AddValue( fLength );
    }

    segment = RaySegment( Vector4Df(m_lumenContour[ uMinIndices.first ],0), Vector4Df(m_lumenContour[ uMinIndices.second ], 1) );
  }

  catch ( ... )
  {
    throw ex::VException(LogRec("unhandled exception" ,"vxVesselSection", "CalculateMinDiameterPoints"));
  }

  return segment;
} // CalculateMinDiameterPoints()


/**
 * Returns the index of the other point which forms a segment that is most perpendicular to the given direction.
 *
 * @param majorDirection the direction of the major axis
 * @param uFirstIndex    the index of one of the contour points
 * @return               the index of the most perpendicular points
 */
uint4 vxVesselSection::GetMostPerpendicularSegmentIndex( const Normal<float4> & majorDirection, const uint4 uFirstIndex ) const
{
  float4 fMostPerpendicularDotProduct( 1.0F );
  uint4 uMostPerpendicularIndex( 0 );

  try
  {
    // get the second contour point that is the most perpendicular with the major direction
    for( uint4 j(0); j<m_lumenContour.size(); ++j )
    {
      Vector3Df vDir( m_lumenContour[uFirstIndex] - m_lumenContour[j] );
      if( j != uFirstIndex && vDir.GetLength() > 1.0e-5F )
      {
        Normal<float4> dir( vDir );
        float4 fDotProduct( fabs( dir.Dot( majorDirection ) ) );
        if( fDotProduct < fMostPerpendicularDotProduct )
        {
          fMostPerpendicularDotProduct = fDotProduct;
          uMostPerpendicularIndex = j;
        }
      }
    }
  }   

  catch ( ... )
  {
    throw ex::VException(LogRec("unhandled exception" ,"vxVesselSection", "GetMostPerpendicularSegmentIndex"));
  }

  return uMostPerpendicularIndex;

} // GetMostPerpendicularSegmentIndex()


/**
* Recalculate the < min, max > diameter points from the contour.
*/
void vxVesselSection::RecalculateDiameterPoints()
{
  m_diameterMax = CalculateMaxDiameterPoints();
  m_diameterMin = CalculateMinDiameterPoints();
} // RecalculateDiameterPoints()


/**
 * Recalculate the area based on the contour points.
 */
void vxVesselSection::RecalculateArea()
{

  // Calculate Lumen Area
  m_fLumenArea = 0.0F;  
  if ( m_lumenContour.size() > 1 )
  {
    // sum up the area of the triangle fans
    for ( int4 i(0); i<m_lumenContour.size()-1; ++i )
    {
      // make a triangle
      Triangle< Point3Df > triangle( m_lumenContour[i], m_lumenContour[i+1], m_position );

      // sum up the area
      m_fLumenArea += triangle.GetArea();
    }
  }

  // Calculate Wall Area
  m_fWallArea = 0.0F;
  if ( m_wallContour.size() > 1 )
  {
    // sum up the area of the triangle fans
    for ( int4 i(0); i<m_wallContour.size()-1; ++i )
    {
      // make a triangle
      Triangle< Point3Df > triangle( m_wallContour[i], m_wallContour[i+1], m_position );

      // sum up the area
      m_fWallArea += triangle.GetArea();
    }
  }

} // RecalculateArea()


/**
 * Gets the mean diameter by derivation from the area.
 */
float4 vxVesselSection::GetMeanDiameter() const
{
  return CalculateMeanDiameter( m_fLumenArea );
} // GetMeanDiameter()


/**
 * Calculate the mean diameter given an area.
 *
 * @param fArea the area
 * @return      the mean diameter that would produce this area if it were circular
 */
float4 vxVesselSection::CalculateMeanDiameter( const float4 fArea )
{
  return 2.0F * sqrtf( fabs( fArea / M_PI ) );
} // CalculateMeanDiameter()


/**
 * Calculate the percent stenosis given a normal and a stenotic measure
 * @param fNormalMeasure   the measurement of the normal vessel diameter or area
 * @param fStenoticMeasure the measurement of the stenotic vessel diameter or area
 * @return                 the percent stenosis in the range [0, 100]
 */
float4 vxVesselSection::CalculatePercentStenosis( const float4 fNormalMeasure, const float4 fStenoticMeasure )
{
  if( fNormalMeasure < fStenoticMeasure )
    return 0.0F;
  else
    return (fNormalMeasure - fStenoticMeasure) / fNormalMeasure * 100.0F;
} // CalculatePercentStenosis()


#undef FILE_REVISION

// $Log: vxVesselSection.C,v $
// Revision 1.3  2004/06/17 14:00:15  frank
// avoided showing zero measurement values for simple curved MPR vessels
//
// Revision 1.2  2004/03/30 21:51:28  frank
// addeds contour serialization
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/12/22 21:20:38  wenli
// Calculate both the wall area when the lumen contour is zero.
//
// Revision 1.2  2003/08/13 11:26:35  frank
// Added comparison operator
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.8  2003/06/19 20:04:25  wenli
// Serialization of m_fCurvatureDegree2MM
//
// Revision 1.7  2003/06/04 19:00:00  michael
// added namespace declaration
//
// Revision 1.6  2003/05/28 17:45:36  frank
// Displayed the vessel wall area on the graph.
//
// Revision 1.5  2003/05/20 21:08:19  wenli
// no contour is (area = 0.0f)
//
// Revision 1.4  2003/05/14 14:10:16  frank
// typos
//
// Revision 1.3  2003/05/14 13:48:00  frank
// Added the wall contour
//
// Revision 1.2  2003/05/14 12:02:35  frank
// Adding a new contour to the vessel section
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.17  2003/04/08 15:16:45  frank
// Properly serialized the section interval
//
// Revision 1.16  2003/03/28 17:17:08  wenli
// update the region growing
//
// Revision 1.15  2003/03/12 14:44:40  frank
// Changed the length to accumulated length and volume to accumulated volume
//
// Revision 1.14  2003/02/27 18:47:23  frank
// removed debug messages
//
// Revision 1.12  2003/02/24 14:03:33  frank
// Added some exception handling
//
// Revision 1.11  2003/01/22 22:20:50  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.10  2003/01/16 21:27:13  frank
// code review
//
// Revision 1.9  2003/01/14 16:16:28  dongqing
// Code review
//
// Revision 1.8  2002/11/27 16:54:35  frank
// Added resample functions to improve the quality of graphs.
//
// Revision 1.7  2002/11/20 17:08:27  frank
// Added volume
//
// Revision 1.6  2002/11/19 20:16:57  frank
// Fixed mean diameter
//
// Revision 1.5  2002/11/15 02:07:21  frank
// Changed "normal" to "tangent" throughout
//
// Revision 1.4  2002/11/14 18:32:57  frank
// #undef FILE_REVISION
//
// Revision 1.3  2002/11/13 01:06:36  frank
// Implemented min/mean/max diameter estimation from the contour and the area, respectively.
//
// Revision 1.2  2002/11/12 21:15:30  frank
// Added function to recalculate the min/max diameter.
//
// Revision 1.1  2002/10/31 12:46:53  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.10  2002/10/30 23:33:31  wenli
// extract contour directly from DTB crosssection
//
// Revision 1.9  2002/10/30 00:02:17  frank
// Added SetDiamterPoints
//
// Revision 1.8  2002/10/29 16:49:59  frank
// revamped measurement
//
// Revision 1.7  2002/09/20 22:18:26  frank
// Worked on reading/writing.
//
// Revision 1.6  2002/08/22 17:31:42  ana
// Bugs fixed.
//
// Revision 1.5  2002/08/22 17:18:06  frank
// Cleaned up warnings.
//
// Revision 1.4  2002/08/20 15:29:54  ana
// Exceptions log messages.
//
// Revision 1.3  2002/08/19 23:15:38  wenli
// initial version - Vascular
//
// Revision 1.2  2002/08/16 21:06:56  ana
// Test function.
//
// Revision 1.1  2002/08/15 20:40:19  ana
// WriteSerializedData and ReadSerializedData implemented.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVesselSection.C,v 1.3 2004/06/17 14:00:15 frank Exp $
// $Id: vxVesselSection.C,v 1.3 2004/06/17 14:00:15 frank Exp $
