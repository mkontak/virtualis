// $Id: vxVessel.C,v 1.13 2006/12/14 22:00:41 romy Exp $
//
// Copyright (c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Wenli Cai (wenli@viatronix.net)

/**
 * Class to maintain all the data associated with a single vessel.
 */

// Includes
#include "stdafx.h"
#include "vxVessel.h"
#include "utilities.h"
#include "vxTetrahedron.h"
#include "ValueRange.h"
#include "Matrix.h"
#include "vxElementInt4.h"
#include "vxRendererCpu3D.h"
#include "vxSerializer.h"


// Defines
#define FILE_REVISION "$Revision: 1.13 $"

// export the vector
template class VX_VIEWER_DLL std::vector< vxVesselSection >;


// Namespaces
using namespace Serialization;
using namespace std;


// Static member initialization
const std::string vxVessel::m_sDelimiter( "vxVessel" );
const float4 vxVessel::m_fVersion( 1.0F );


/**
 * Constructor
 */
vxVessel::vxVessel()
: m_fSectionIntervalMM( -2.0F ), 
m_iCalcificationThresholdHU(0),
m_bStenosisValid( false )
{
} // vxVessel()


/**
 * Destructor
 */
vxVessel::~vxVessel()
{
} // ~vxVessel()


/**
 * Copy constructor
 *
 * @param   other   other vessel
 */
vxVessel::vxVessel( const vxVessel & other )  :
  m_uMeasurementPair(other.m_uMeasurementPair),
  m_bStenosisValid(other.m_bStenosisValid),
  m_uLabel(other.m_uLabel),
  m_fSectionIntervalMM(other.m_fSectionIntervalMM),
  m_iCalcificationThresholdHU(other.m_iCalcificationThresholdHU),
  std::vector< vxVesselSection >( other )
{
} // vxVessel( const vxVessel & other )


/*
 * Clones an instance
 */
vxVessel * vxVessel::Clone() const
{
  return new vxVessel( *this );
}//Clone() const


/**
 * assignment  operator
 * 
 * @param   rhs   other vessel
 * @return reference to self
 */
vxVessel & vxVessel::operator=( const vxVessel & rhs )
{
  if( &rhs != this )
  {
    m_uMeasurementPair = rhs.m_uMeasurementPair;
    m_bStenosisValid = rhs.m_bStenosisValid;
    m_uLabel = rhs.m_uLabel;
    m_fSectionIntervalMM = rhs.m_fSectionIntervalMM;
    m_iCalcificationThresholdHU = rhs.m_iCalcificationThresholdHU;
    std::vector< vxVesselSection >::operator=( rhs );
  }
  return *this;
} // operator=( const vxVessel & rhs )


/**
 * retrieves the number of bytes of the serialized data.
 * @return the size of the vxVessel
 */
uint4 vxVessel::GetSerializedDataSize() const
{
  uint4 uSum(0);

  // start delimiter
  uSum += m_sDelimiter.length() + 1;

  // version
  uSum += sizeof( m_fVersion );

  // name
  //uSum += m_sName.length() + 1;

  // name
  uSum += sizeof( m_uLabel );

  // sampling interval
  uSum += sizeof( m_fSectionIntervalMM );

  // calcification threshold (HU)
  uSum += sizeof( m_iCalcificationThresholdHU );

  // start and end indices
  uSum += sizeof( m_uMeasurementPair );

  // stenosis valid
  uSum += sizeof( m_bStenosisValid );

  // vector<VesselSection> size
  uSum += sizeof(uint4);

  // all the vessel sections
  for( int4 i(0); i<size(); i++ )
    uSum += at(i).GetSerializedDataSize();

  // end delimiter
  uSum += m_sDelimiter.length() + 1;

  return uSum;
} // GetSerializedDataSize()




/**
 * Reads in all the data from serialized form.
 *
 * @param hiBuffer    handle to the memory buffer which stores the vessel sections.
 * @param piBufferEnd pointer to the end of the memory buffer.
 */
void vxVessel::ReadSerializedData( int1 ** hiBuffer, const int1 * piBufferEnd )
{
  try
  {
    // verify the stream by reading start delimiter
    if ( DeserializeStringData( hiBuffer, piBufferEnd ) != m_sDelimiter )
      throw ex::DataCorruptionException( LogRec( "Bad starting delimiter not '" + m_sDelimiter + "'", "vxVessel", "ReadSerializedData" ) );

    // version
    float4 fVersion;
    DeserializeSimpleData( hiBuffer, piBufferEnd, fVersion );
    if ( fVersion != m_fVersion )
      throw ex::DataCorruptionException( LogRec( "Old versions not supported (" + ToAscii(fVersion) + ")", "vxVessel", "ReadSerializedData" ) );

    // name
    std::string sName = DeserializeStringData( hiBuffer, piBufferEnd );

    // label
    DeserializeSimpleData( hiBuffer, piBufferEnd, m_uLabel );
    LogDbg( "read in label value of " + ToAscii(int4(m_uLabel)), "", "" );

    // Interval
    DeserializeSimpleData( hiBuffer, piBufferEnd, m_fSectionIntervalMM );
    
    // Calcification Threshold (HU)
    DeserializeSimpleData( hiBuffer, piBufferEnd, m_iCalcificationThresholdHU );

    // start and end indices
    DeserializeSimpleData( hiBuffer, piBufferEnd, m_uMeasurementPair );

    // stenosis valid
    DeserializeSimpleData( hiBuffer, piBufferEnd, m_bStenosisValid );

    // read number of sections
    uint4 uSize(0);
    DeserializeSimpleData( hiBuffer, piBufferEnd, uSize );
    resize( uSize );

    // read in all the sections
    for( int4 i(0); i<size(); i++ )
    {
      //at(i) = new vxVesselSection();
      at(i).ReadSerializedData( hiBuffer, piBufferEnd );
    }
    
    // verify the stream again by reading end delimiter.
    if ( DeserializeStringData( hiBuffer, piBufferEnd ) != m_sDelimiter ) 
      throw ex::DataCorruptionException( LogRec( "Bad ending delimiter not '" + m_sDelimiter + "'", "vxVessel", "ReadSerializedData" ) );
  }
  catch( ex::OutOfMemoryException )
  {
    throw ex::OutOfMemoryException( LogRec( "Out of memory", "vxVessel", "ReadSerializedData" ) );
  }
  catch( ex::VException & error )
  {
    throw ex::VException( LogRec( error.GetLogRecord().GetTextMessage(), "VxVessel", "ReadSerializedData" ) );
  }

} // ReadSerializedData()


/**
 * Equality operator for a whole vessel
 *
 * @param other is the vessel we are to compare to
 * @return true if the vessels are equal
 */
bool vxVessel::operator ==(const vxVessel & other) const
{

  // check measurement start
  if ( m_uMeasurementPair != other.m_uMeasurementPair )
    return false;

  // check stenosis valid
  if ( m_bStenosisValid != other.m_bStenosisValid )
    return false;

  // check if they have the same number of vessel segments
  if ( size() != other.size() )
    return false;

  for( int4 i(0); i<size(); i++ )
  {
    if ( at(i) != other[i] )
      return false;
  } // foreach vessel section

  return true;
} // operator ==


/**
 * Inequality operator for a whole vessel
 * @param other is the vessel we are to compare to
 * @return true if the vessels are not equal
 */
bool vxVessel::operator !=(const vxVessel & other) const
{
  return ! (*this == other);
} // operator !=


/**
 * Get the range of diameters of all the section in mm.
 * @parm measurePair is the pair of index 
 * @return is the range of diamenter
 */
ValueRange<float4> vxVessel::GetDiameterRange( const MeasurePair & measurePair ) const
{
  ValueRange<float4> diameters;

  try
  {
    for( int4 i(measurePair.GetMinimum()); i<=measurePair.GetMaximum(); ++i )
    {
      diameters.AddValue( at(i).GetDiameterRange().GetMinimum() );
      diameters.AddValue( at(i).GetDiameterRange().GetMaximum() );
    }
  }
  catch (...)
  {
    // do nothing for now
  }

  return diameters;
} // GetDiameterRange()


/**
 * Get the section indices of the min and max diameter locations
 * @parm measurePair is the pair of index 
 * @return is the range of diamenter index
 */
vxVessel::MeasurePair vxVessel::GetDiameterRangeIndices( const MeasurePair & measurePair ) const
{
  ValueRange<float4> diameters;
  vxVessel::MeasurePair indices( 0, 0 );

  try
  {
    for( int4 i(measurePair.GetMinimum()); i<=measurePair.GetMaximum(); ++i )
    {
      // prime it the first time
      if ( i == measurePair.GetMinimum() )
      {
        indices = vxVessel::MeasurePair( i, i );
      }
      else
      {
        if ( at(i).GetDiameterRange().GetMinimum() < diameters.GetMinimum() ) indices.first  = i;
        if ( at(i).GetDiameterRange().GetMaximum() > diameters.GetMaximum() ) indices.second = i;
      }

      if (i < size())
      {
        diameters.AddValue( at(i).GetDiameterRange().GetMinimum() );
        diameters.AddValue( at(i).GetDiameterRange().GetMaximum() );
      }
    }
  }
  catch (...)
  {
    // do nothing for now
  }

  return indices;
} // GetDiameterRangeIndices()


/**
 * Get the range of lumen areas of all the section in square mm.
 *
 * @parm measurePair is the pair of index 
 * @return is the range of lumen area
 */
ValueRange<float4> vxVessel::GetLumenAreaRange( const MeasurePair & measurePair ) const
{
  ValueRange<float4> areas;

  // see if valid endpoints
  if ( size() > 1 && measurePair.GetMinimum() < size() && measurePair.GetMaximum() < size() )
  {
    // loop over sections
    for( int4 i(measurePair.GetMinimum()); i<=measurePair.GetMaximum(); i++ )
    {
      float4 fArea = at(i).GetLumenArea();
      if ( fArea > 0.5f )  //not interpolated value
        areas.AddValue( fArea );
    }

  } // if valid

  return areas;
} // GetLumenAreaRange()


/**
 * Get the range of wall areas of all the section in square mm.
 *
 * @parm measurePair is the pair of index 
 * @return is the range of wall area
 */
ValueRange<float4> vxVessel::GetWallAreaRange( const MeasurePair & measurePair ) const
{
  ValueRange<float4> areas;

  // see if valid endpoints
  if ( size() > 1 && measurePair.GetMinimum() < size() && measurePair.GetMaximum() < size() )
  {
    // loop over sections
    for( int4 i(measurePair.GetMinimum()); i<=measurePair.GetMaximum(); i++ )
    {
      float4 fArea = at(i).GetWallArea();
      if ( fArea > 0.5f )  //not interpolated value
        areas.AddValue( fArea );
    }

  } // if valid

  return areas;
} // GetWallAreaRange()


/**
 * Get the section indices of the min and max areas
 * @parm measurePair is the pair of index 
 * @return is the range of diamenter indeces
 */
vxVessel::MeasurePair vxVessel::GetAreaRangeIndices( const MeasurePair & measurePair ) const
{
  vxVessel::MeasurePair indices;

  // see if valid endpoints
  if ( size() > 1 && measurePair.first < size() && measurePair.second < size() )
  {

    ValueRange<float4> areas;

    // loop over sections
    for( int4 i(measurePair.GetMinimum()); i<=measurePair.GetMaximum(); i++ )
    {
      // prime it the first time
      if ( i == measurePair.GetMinimum() )
      {
        indices = vxVessel::MeasurePair( i, i );
      }
      else
      {
        if ( at(i).GetLumenArea() < areas.GetMinimum() ) indices.first  = i;
        if ( at(i).GetLumenArea() > areas.GetMaximum() ) indices.second = i;
      }

      areas.AddValue( at(i).GetLumenArea() );
    }
  } // if valid

  return indices;
} // GetAreaRangeMeasure()


/**
 * Get the length from measurement start to end in mm.
 *
 * @param measurePair the inclusive endpoints of measurement
 * @return the length in mm between the endpoints of measurement
 */
float4 vxVessel::GetLength( const MeasurePair & measurePair ) const
{

  // see if we have valid endpoints
  if ( size() == 0 ||
      measurePair.first  < 0.0F || measurePair.first  > size()-1 ||
      measurePair.second < 0.0F || measurePair.second > size()-1 )
  {
    LogDbg( "GetLength from " + ToAscii(measurePair.first) + " to " + ToAscii( measurePair.second ) + " is zero", "", "" );
    return 0.0F;
  }

  //LogDbg( "GetLength from " + ToAscii(measurePair.first) + " to " + ToAscii( measurePair.second ) + " is " + ToAscii( ResampleAccumulatedLengthMM( measurePair.GetMaximum() ) - ResampleAccumulatedLengthMM( measurePair.GetMaximum() ) ), "", "" );
  return ResampleAccumulatedLengthMM( measurePair.GetMaximum() ) - ResampleAccumulatedLengthMM( measurePair.GetMinimum() );

} // GetLength()


/**
 * Get the volume from measurement start to end in mm^3.
 *
 * @param measurePair the inclusive endpoints of measurement
 * @return the volume in cubic mm between the endpoints of measurement
 */
float4 vxVessel::GetVolume( const MeasurePair & measurePair ) const
{
  // see if we have valid endpoints
  if ( size() == 1 ||
      measurePair.first  < 0.0F || measurePair.first  > size()-1 ||
      measurePair.second < 0.0F || measurePair.second > size()-1 )
  {
    return 0.0F;
  }

  return ResampleAccumulatedVolumeCubicMM( measurePair.GetMaximum() ) - ResampleAccumulatedVolumeCubicMM( measurePair.GetMinimum() );

} // GetVolume()


/**
 * Get the percent stenosis as determined by the start and end of measurement
 * @return the percentage of the stensis
 */
float4 vxVessel::GetPercentStenosis() const
{
  if ( m_uMeasurementPair.first < size() && m_uMeasurementPair.second < size() )
  {
    float fAreaNormal( at(m_uMeasurementPair.first).GetLumenArea() );
    float4 fAreaStenotic( at(m_uMeasurementPair.second).GetLumenArea() );
    float4 fPercentile = (fAreaNormal - fAreaStenotic) / fAreaNormal * 100 ;
    if ( fAreaNormal < fAreaStenotic )
      fPercentile = 0;
    return fPercentile;
  }
  else
    return 0.0F;
} // GetPercentStenosis()


/**
 * Recalculate the < min, max > diameter points from the contour.
 */
void vxVessel::RecalculateDiameterPoints()
{
  for( int i(0); i<size(); ++i )
    this->at(i).RecalculateDiameterPoints();
} // RecalculateDiameterPoints()


/**
 * Recalculate the area from the contour.
 */
void vxVessel::RecalculateArea()
{
  for( int i(0); i<size(); ++i )
    this->at(i).RecalculateArea();
} // RecalculateArea()


/**
 * Smooth orientations along the length so as to minimize distortion in luminal view.
 */
void vxVessel::SmoothOrientations()
{
  // perform a discrete smoothing by averaging
  for( int i(1); i<size()-1; ++i )
  {
    vxVesselSection & prior   = at( i - 1 );
    vxVesselSection & current = at( i     );
    vxVesselSection & next    = at( i + 1 );

    current.SetTanDirection( Normal3Df(( Vector<float4>( prior.GetTanDirection() ) + Vector<float4>( next.GetTanDirection() ) ) / 2.0F) );
    current.SetUpDirection ( Normal3Df(( Vector<float4>( prior.GetUpDirection()  ) + Vector<float4>( next.GetUpDirection()  ) ) / 2.0F) );
  }
} // SmoothOrientations()


/**
 * Computes the toruosity (in degrees/mm) at given location by looking ahead and behind <radius> millimeters. 
 *
 * @param calipers compute given these end points
 * @return tortuosity
 */
float4 vxVessel::ComputeTortuosity( const MeasurePair & calipers ) const
{
  // check for valid input
  if ( size() == 0 )
    throw ex::InvalidDataException( LogRec( "There is no data in this vessel", "vxVessel", "ComputeToruosity" ) );
  if ( calipers.first < 0 || calipers.first >= size() || calipers.second < 0 || calipers.second >= size() )
    throw ex::IndexOutOfBoundsException( LogRec( "Requested location was outside the vessel", "vxVessel", "ComputeToruosity" ) );

  // arc length
  const float4 fArcLength( GetLength( calipers ) );

  // compute angle
  Normal<float4> n1( at( calipers.first ).GetTanDirection() );
  Normal<float4> n2( at( calipers.second ).GetTanDirection() );
  const float4 fAngleRadians( fabs( acos( n1.Dot( n2 ) ) ) );
  const float4 fAngleDegrees( fAngleRadians * 180.0F / M_PI );

  // tortuosity in degrees per mm arc length
  float4 fTortuosity = fAngleDegrees / fArcLength;

  return fTortuosity;

} // ComputeToruosity()



float4 vxVessel::GetTortuosity( uint4 uLocation ) const
{
  // check for valid input
  if ( size() == 0 )
    throw ex::InvalidDataException( LogRec( "There is no data in this vessel", "vxVessel", "ComputeToruosity" ) );
  if ( uLocation < 0 || uLocation >= size() )
    throw ex::IndexOutOfBoundsException( LogRec( "Requested location was outside the vessel", "vxVessel", "ComputeToruosity" ) );

  float4 fCurv = at(uLocation).GetCurvatureDegree2MM();

  return fCurv;

} // ComputeToruosity()





/**
 * Compute the maximum tortuosity and it's location
 * @param uLocation the index of the maximum tortuosity
 * @param measureDomain the given two indeces measured
 * @return maximum tortuosity
 */
float4 vxVessel::ComputeMaximumTortuosity( uint4 & uLocation, const MeasurePair & measureDomain ) const
{
  float4 fMaxTortuosity( -0.0F );
  uLocation = 0;

  for( uint4 i=measureDomain.GetMinimum(); i<=measureDomain.GetMaximum(); ++i )
  {
    //MeasurePair measurePair = GetEnclosingMeasurePair( i );
    //float4 fTortuosity( ComputeTortuosity( measurePair ) );

    float4 fTortuosity( ResampleTortuosityDegree2MM( i ) );
    if ( fTortuosity > fMaxTortuosity )
    {
      fMaxTortuosity = fTortuosity;
      uLocation = i;
    }
  }

  return fMaxTortuosity;

} // ComputeMaximumTortuosity()

float4 vxVessel::ComputeMinimumTortuosity( uint4 & uLocation, const MeasurePair & measureDomain ) const
{
  float4 fMinTortuosity( 1.0e20F );
  uLocation = 0;

  for( uint4 i=measureDomain.GetMinimum(); i<=measureDomain.GetMaximum(); ++i )
  {
    //MeasurePair measurePair = GetEnclosingMeasurePair( i );
    //float4 fTortuosity( ComputeTortuosity( measurePair ) );

    float4 fTortuosity( ResampleTortuosityDegree2MM( i ) );
    if ( fTortuosity < fMinTortuosity )
    {
      fMinTortuosity = fTortuosity;
      uLocation = i;
    }
  }

  return fMinTortuosity;

} // ComputeMaximumTortuosity()

float4 vxVessel::ComputeAverageTortuosity( uint4 & uLocation, const MeasurePair & measureDomain ) const
{
  float4 fAvgTortuosity( 0.0F );

  for( uint4 i=measureDomain.GetMinimum(); i<=measureDomain.GetMaximum(); ++i )
  {
    //MeasurePair measurePair = GetEnclosingMeasurePair( i );
    //float4 fTortuosity( ComputeTortuosity( measurePair ) );

    float4 fTortuosity( ResampleTortuosityDegree2MM( i ) );
    fAvgTortuosity += fTortuosity;
  }
  fAvgTortuosity /= (float4)( measureDomain.GetMaximum()-measureDomain.GetMinimum()+1.0F );

  return fAvgTortuosity;

} // ComputeMaximumTortuosity()


/**
 * Find the measurement pair surrounding this location with the given arc length.
 *
 * @param uLocation    the center about which to creep outwards until the length is achieved
 * @param fArcLengthMM the desired arc length along the centerline in mm
 * @return             a pair of indices which indicate the start and end of the resulting arc
 */
vxVessel::MeasurePair vxVessel::GetEnclosingMeasurePair( const uint4 uLocation, const float4 fArcLengthMM ) const
{
  vxVessel::MeasurePair outMeasurePair( uLocation, uLocation );
  bool bWithinRange = true;

  for( int4 iRadius=1; iRadius<size() && bWithinRange; ++iRadius )
  {

    // choose two points straddling this location
    MeasurePair measure( Bound( int4(0), int4( uLocation - iRadius ), int4( size() - 1 ) ),
                         Bound( int4(0), int4( uLocation + iRadius ), int4( size() - 1 ) ) );

    // arc length
    const float4 fArcLength( GetLength( measure ) );

    // at least use the first two neighbor points
    // keep going until we are too long
    if ( iRadius == 1 || fArcLength < fArcLengthMM )
      outMeasurePair = measure;

    if ( fArcLength > fArcLengthMM )
      bWithinRange = false;
  }

  return outMeasurePair;
} // GetEnclosingMeasurePair()


/**
 * Compute the volume between sections along the vessel.
 */
void vxVessel::RecalculateVolumes()
{
  double fAccumulatedVolume( 0 );
  for( int4 i=1; i<size(); ++i )
  {
    fAccumulatedVolume += ComputeInterSectionVolume( i );
    at( i ).SetAccumulatedVolumeCubicMM( fAccumulatedVolume );
  }
} // RecalculateVolumes()


/**
 * Compute the accumulated length between sections along the vessel.
 */
void vxVessel::RecalculateLengths()
{
  float4 fAccumulatedLength( 0 );

  // set first length
  if ( size() > 0 )
    front().SetAccumulatedLengthMM( fAccumulatedLength );

  // set remaining lengths
  for( int4 i=1; i<size(); ++i )
  {
    // see if we have to calculate the interval for each pair or if they are all fixed
    if ( m_fSectionIntervalMM < 0 )
    {
      // must calculate each individually
      fAccumulatedLength += Vector3Df( at( i ).GetPosition() - at( i - 1 ).GetPosition() ).GetLength();
    }
    else
    {
      // fixed length
      fAccumulatedLength += m_fSectionIntervalMM;
    }
    at( i ).SetAccumulatedLengthMM( fAccumulatedLength );
  }
} // RecalculateLengths()


/**
 * Compute the volume between two sections
 * @param uUpperSectionIndex index of the section
 * @return the volume of the section 
 */
float4 vxVessel::ComputeInterSectionVolume( const uint4 uUpperSectionIndex ) const
{
  if ( uUpperSectionIndex < 1 || uUpperSectionIndex > size() - 1 )
    throw ex::IllegalArgumentException( LogRec( "Bad section index", "vxVessel", "ComputeInterSectionVolume" ) );

  // get the approximate centroid of the object
  const Point<float4> centroid( ( Vector<float4>( at( uUpperSectionIndex     ).GetPosition() ) +
                                  Vector<float4>( at( uUpperSectionIndex - 1 ).GetPosition() ) ) / 2.0F );

  // compute the sum of volumes
  float8 fVolume( 0.0F );

  int iNeedToCheckForConvexContours;

  // go around the bottom contour
  const vxVesselSection & bottomSection( at( uUpperSectionIndex - 1 ) );
  const Point<float4> bottomCenter( bottomSection.GetPosition() );
  const std::vector< Point<float4> > & bottomContour = bottomSection.GetLumenContour();
  int4 i(0);
  for( i=0; i<bottomContour.size(); ++i )
  {
    vxTetrahedron tetra( bottomCenter, bottomContour[i], bottomContour[ ( i + 1 ) % bottomContour.size() ], centroid );
    fVolume += tetra.GetVolume();
  }

  // go around the top contour
  const vxVesselSection & topSection( at( uUpperSectionIndex ) );
  const Point<float4> topCenter( bottomSection.GetPosition() );
  const std::vector< Point<float4> > & topContour = bottomSection.GetLumenContour();
  for( i=0; i<topContour.size(); ++i )
  {
    const int4 iNext( ( i + 1 ) % topContour.size() );
    vxTetrahedron tetra( topCenter, topContour[i], topContour[ iNext ], centroid );
    fVolume += tetra.GetVolume();
  }

  // check to make sure they have the same number of points
  if ( bottomContour.size() != topContour.size() )
    throw ex::DataCorruptionException( LogRec( "The two sections have a different number of points", "vxVessel", "ComputeInterSectionVolume" ) );

  // go around the edge - each pie-shaped wedge is composed to two tetrahedrons
  for( i=0; i<bottomContour.size() && i<topContour.size(); ++i )
  {
    const int4 iNext( ( i + 1 ) % topContour.size() );
    vxTetrahedron tetra1( centroid, bottomContour[i], topContour[i], bottomContour[ iNext ] );
    vxTetrahedron tetra2( centroid, bottomContour[ iNext ], topContour[ i ], topContour[ iNext ] );
    fVolume += tetra1.GetVolume() + tetra2.GetVolume();
  }

  // why does this make it right?
  fVolume *= 3;

  //LogDbg( "length: " + ToAscii( fLength ) + "  area: " + ToAscii( fAverageArea ) + "  fVolume by integration: " + ToAscii( fVolume ) + "  fVolume by estimation: " + ToAscii( fAverageArea * fLength ), "", "" );
  //return fAverageArea * fLength;

  return fVolume;
  
} // ComputeInterSectionVolume()


/**
 * Calculate the inside angle of 3 points.
 * @param p0 proceed point
 * @param p1 middle point
 * @param p2 succeed point
 * @return inside angle between three points
 */
float4 vxVessel::CalculateInsideAngle( const Point<float4> & p0, const Point<float4> & p1, const Point<float4> & p2 )
{
  Vector<float4> v1 = p0-p1; v1.Normalize();
  Vector<float4> v2 = p2-p1; v2.Normalize();
  return acos( v1.Dot( v2 ) ) * 180.0F / M_PI;
} // CalculateInsideAngle()


/**
 * Calculate the outside angle of 3 points.
 * @param p0 proceed point
 * @param p1 middle point
 * @param p2 succeed point
 * @return outside angle between three points
 */
float vxVessel::CalculateOutsideAngle( const Point<float4> & p0, const Point<float4> & p1, const Point<float4> & p2, const Vector<float4> & t0, const Vector<float4> & t2 )
{
  // Compute the plane which contains vP0,vP1,vP2
  Vector<float4> v1 = p0-p1; v1.Normalize();
  Vector<float4> v2 = p2-p1; v2.Normalize();
  Vector<float4> vPlaneNormal = v1.Cross(v2); vPlaneNormal.Normalize();
  float4 fPlane1A = vPlaneNormal.m_x;
  float4 fPlane1B = vPlaneNormal.m_y;
  float4 fPlane1C = vPlaneNormal.m_z;
  float4 fPlane1D = -(fPlane1A*p0.m_x + fPlane1B*p0.m_y + fPlane1C*p0.m_z);

  // Flip one of the tangents. Otherwise, they both point
  // in the same direction along the centerline, instead of
  // away from the vertex point (which is what we want).
  // We can flip an arbitrary tangent, 
  Vector<float4> t0Flipped = -t0;
  // project the tangents to the plane
  float4 fDotProduct = vPlaneNormal.Dot(t0Flipped);
  Vector<float4> t0prime = t0Flipped - vPlaneNormal * fDotProduct; t0prime.Normalize();
  fDotProduct = vPlaneNormal.Dot(t2);
  Vector<float4> t2prime = t2 - vPlaneNormal * fDotProduct; t2prime.Normalize();

  // return the angle between the tangents
  return acos( t0prime.Dot(t2prime) ) * 180.0F / M_PI;

} // CalculateOutsideAngle()


/**
 * Returns the weight of a Gaussian filter centered at x=0 and the next data points at 1 and -1
 * @param fX is sampling point
 * @param fSigma the size of the filter
 * @return gaussian wieght
 */
inline float4 GetGaussianWeight( const float4 fX, const float4 fSigma = 1.0F )
{

  const float4 fSigmaSquared( pow( fSigma, 2 ) );
  const float4 fNumerator( exp( - fX * fX / fSigmaSquared ) );
  const float4 fDenominator( sqrt( 2 * M_PI * fSigmaSquared ) );
  return fNumerator / fDenominator;

} // GetGaussianWeight()


/**
 * Resample the lumen area value at a location along the vessel.
 *
 * @param fLocation  the location along the vessel in fractional section coordinates
 * @return           the resampled area in square millimeters
 */
float4 vxVessel::ResampleLumenAreaSquareMM( const float4 fLocation ) const
{
  float4 fArea( 0.0F );
  float4 fWeightSum( 0.0F );

  // loop over three points on either side and sum
  const int4 iRadius( 3 );
  for( int4 iX( fLocation - iRadius ); iX<=fLocation + iRadius; ++iX )
  {
    // see if the requested location is within bounds
    if ( ( iX >= 0 ) && ( iX <= size()-1 ) )
    {
      // sum weighted values
      fArea += GetGaussianWeight( fLocation - iX ) * at(iX).GetLumenArea();
      fWeightSum += GetGaussianWeight( fLocation - iX );
    }
  }

  // normalize
  if ( fWeightSum != 0.0F ) fArea /= fWeightSum;

  return fArea;

} // ResampleLumenAreaSquareMM()


/**
 * Resample the wall area value at a location along the vessel.
 *
 * @param fLocation  the location along the vessel in fractional section coordinates
 * @return           the resampled area in square millimeters
 */
float4 vxVessel::ResampleWallAreaSquareMM( const float4 fLocation ) const
{
  float4 fArea( 0.0F );
  float4 fWeightSum( 0.0F );

  // loop over three points on either side and sum
  const int4 iRadius( 3 );
  for( int4 iX( fLocation - iRadius ); iX<=fLocation + iRadius; ++iX )
  {
    // see if the requested location is within bounds
    if ( ( iX >= 0 ) && ( iX <= size()-1 ) )
    {
      // sum weighted values
      fArea += GetGaussianWeight( fLocation - iX ) * at(iX).GetWallArea();
      fWeightSum += GetGaussianWeight( fLocation - iX );
    }
  }

  // normalize
  if ( fWeightSum != 0.0F ) fArea /= fWeightSum;

  return fArea;

} // ResampleWallAreaSquareMM()


/**
 * Resample the accumulated length at a location along the vessel.
 *
 * @param fLocation  the location along the vessel in fractional section coordinates
 * @return           the resampled accumulated length in millimeters
 */
float4 vxVessel::ResampleAccumulatedLengthMM( const float4 fLocation ) const
{
  // check for no data
  if ( size() == 0 ) return 0.0F;

  // check for before the start
  if ( fLocation < 0.0F ) return 0.0F;

  // check for at or after the end
  if ( fLocation >= size()-1 ) return back().GetAccumulatedLengthMM();

  // otherwise, interpolate
  const int4 iMinIndex( fLocation );
  const int4 iMaxIndex( iMinIndex + 1 );
  return Interpolate( float4( iMinIndex ),
                      fLocation,
                      float4( iMaxIndex ),
                      at( iMinIndex ).GetAccumulatedLengthMM(),
                      at( iMaxIndex ).GetAccumulatedLengthMM() );

} // ResampleAccumulatedLengthMM()


/**
 * Resample the accumulated volume at a location along the vessel.
 *
 * @param fLocation  the location along the vessel in fractional section coordinates
 * @return           the resampled accumulated volume in cubic millimeters
 */
float4 vxVessel::ResampleAccumulatedVolumeCubicMM( const float4 fLocation ) const
{
  // check for no data
  if ( size() == 0 ) return 0.0F;

  // check for before the start
  if ( fLocation < 0.0F ) return 0.0F;

  // check for at or after the end
  if ( fLocation >= size()-1 ) return back().GetAccumulatedVolumeCubicMM();

  // otherwise, interpolate
  const int4 iMinIndex( fLocation );
  const int4 iMaxIndex( iMinIndex + 1 );
  return Interpolate( float4( iMinIndex ),
                      fLocation,
                      float4( iMaxIndex ),
                      at( iMinIndex ).GetAccumulatedVolumeCubicMM(),
                      at( iMaxIndex ).GetAccumulatedVolumeCubicMM() );

} // ResampleAccumulatedVolumeCubicMM()


/**
 * Resample the minimum and maximum diameters.
 *
 * @param fLocation  the location along the vessel in fractional section coordinates
 * @return           the resampled minimum and maximum dimeters in millimeters
 */
ValueRange<float4> vxVessel::ResampleMinMaxDiameter( const float4 fLocation ) const
{
  float4 fMinDiameter( 0.0F );
  float4 fMaxDiameter( 0.0F );
  float4 fWeightSum( 0.0F );

  // loop over three points on either side and sum
  const int4 iRadius( 3 );
  for( int4 iX( fLocation - iRadius ); iX<=fLocation + iRadius; ++iX )
  {
    // see if the requested location is within bounds
    if ( ( iX >= 0 ) && ( iX <= size()-1 ) )
    {
      // sum weighted values
      const float4 fWeight( GetGaussianWeight( fLocation - iX ) );
      fWeightSum += fWeight;
      fMinDiameter += GetGaussianWeight( fLocation - iX ) * at(iX).GetDiameterRange().GetMinimum();
      fMaxDiameter += GetGaussianWeight( fLocation - iX ) * at(iX).GetDiameterRange().GetMaximum();
    }
  }

  // normalize
  if ( fWeightSum != 0.0F )
  {
    fMinDiameter /= fWeightSum;
    fMaxDiameter /= fWeightSum;
  }

  // return a value range
  ValueRange<float4> diameters;
  diameters.AddValue( fMinDiameter );
  diameters.AddValue( fMaxDiameter );
  return diameters;

} // ResampleMinMaxDiameter()


/**
* Resample the tortuosity value at a location along the vessel.
*
* @param fLocation  the location along the vessel in fractional section coordinates
* @return           the resampled tortuosity in degree/mm
*/
float4 vxVessel::ResampleTortuosityDegree2MM( const float4 fLocation ) const
{
  float4 fTortuosity( 0.0F );
  float4 fWeightSum( 0.0F );

  // loop over three points on either side and sum
  const int4 iRadius( 3 );
  for( int4 iX( fLocation - iRadius ); iX<=fLocation + iRadius; ++iX )
  {
    // see if the requested location is within bounds
    if ( ( iX >= 0 ) && ( iX <= size()-1 ) )
    {
      // sum weighted values
      fTortuosity += GetGaussianWeight( fLocation - iX ) * GetTortuosity(iX);
      fWeightSum += GetGaussianWeight( fLocation - iX );
    }
  }

  // normalize
  if ( fWeightSum != 0.0F ) fTortuosity /= fWeightSum;

  return fTortuosity;

} // ResampleLumenAreaSquareMM()


/**
 * Resample the centerline position at a fractional section index
 *
 * @param fLocation the fraction section index
 *
 * @return the resampled centerline position
 */
Point3Df vxVessel::GetResampledPosition( const float4 fLocation ) const
{
  // check boundary conditions
  if ( size() == 0 )
    return Point3Df( 0, 0, 0 );
  if ( size() == 1 )
    return front().GetPosition();
  if ( fLocation < 0.0F )
    return front().GetPosition();
  if ( fLocation >= size()-1 )
    return back().GetPosition();

  // interpolate
  const Vector3Df left ( at( uint4( fLocation     ) ).GetPosition() );
  const Vector3Df right( at( uint4( fLocation + 1 ) ).GetPosition() );
  const float4 fPercentageRight( fLocation - uint4( fLocation ) );
  const float4 fPercentageLeft( 1.0F - fPercentageRight );
  return Point3Df( left * fPercentageLeft + right * fPercentageRight );

} // GetResampledPosition()


/** 
 *  Get the resampling index at the location of specific length 
 *  @param fDesiredLengthMM is the input length ( for location )
 *  @return the resampled index
 */
float4 vxVessel::GetFractionalIndex( const float4 fDesiredLengthMM ) const
{
  // search for the proper interval to achieve the desired distance
  float4 fNewIndex( 0 );
  for ( int4 index( 0 ); index < size()-1; ++index )
  {
    const float4 fLowerMM( at( index ).GetAccumulatedLengthMM() );
    const float4 fUpperMM( at( index + 1 ).GetAccumulatedLengthMM() );
    if ( fLowerMM <= fDesiredLengthMM && fDesiredLengthMM <= fUpperMM )
    {
      if ( fLowerMM < fUpperMM )
      {
        fNewIndex = Interpolate( fLowerMM, fDesiredLengthMM, fUpperMM, float4( index ), float4( index + 1 ) );
      }
    }
  }

  return fNewIndex;
} // GetFractionalIndex()


/**
 * Resample the centerline tangent direction at a fraction section index
 *
 * @param fLocation the fraction section index
 *
 * @return the resampled centerline tangent direction
 */
Normal3Df vxVessel::GetResampledTangent( const float4 fLocation ) const
{
  // check boundary conditions
  if ( fLocation < 0.0F )
    return front().GetTanDirection();
  if ( fLocation >= size()-1 )
    return back().GetTanDirection();

  // interpolate
  const Vector3Df left ( Vector3Df( at( uint4( fLocation     ) ).GetTanDirection() ) );
  const Vector3Df right( Vector3Df( at( uint4( fLocation + 1 ) ).GetTanDirection() ) );
  const float4 fPercentageRight( fLocation - uint4( fLocation ) );
  const float4 fPercentageLeft( 1.0F - fPercentageRight );
  return Normal3Df( left * fPercentageLeft + right * fPercentageRight );

} // GetResampledTangent()


/**
 * Resample the centerline up direction at a fraction section index
 *
 * @param fLocation the fraction section index
 *
 * @return the resampled centerline up direction
 */
Normal3Df vxVessel::GetResampledUpDirection( const float4 fLocation ) const
{
  // check boundary conditions
  if ( fLocation < 0.0F )
    return front().GetUpDirection();
  if ( fLocation >= size()-1 )
    return back().GetUpDirection();

  // interpolate
  const Vector3Df left ( Vector3Df( at( uint4( fLocation     ) ).GetUpDirection() ) );
  const Vector3Df right( Vector3Df( at( uint4( fLocation + 1 ) ).GetUpDirection() ) );
  const float4 fPercentageRight( fLocation - uint4( fLocation ) );
  const float4 fPercentageLeft( 1.0F - fPercentageRight );
  return Normal3Df( left * fPercentageLeft + right * fPercentageRight );

} // GetResampledUpDirection()


/**
 * Estimate the tangent vectors for the entire vessel from scratch
 *
 * @param fSampleRadius the distance from the current point with which to estimate the tangent - larger values yield smoother tangents
 */
void vxVessel::EstimateTangents( const float4 fArcLengthMM )
{
  for ( int i=0; i<size(); ++i )
  {
    MeasurePair samplePair = GetEnclosingMeasurePair( i, fArcLengthMM );
    Vector3Df directionVector( at( samplePair.second ).GetPosition() - at( samplePair.first ).GetPosition() );
    if ( directionVector.GetLengthSquared() > 0.0F )
    {
      at( i ).SetTanDirection( Normal3Df( directionVector ) );
    }
  }
} // EstimateTangents()


/**
 * Load a centerline from a text file.
 *
 * @param sFilename the input filename
 *
 * @return true on success
 */
bool vxVessel::LoadCenterlineFromTextFile( const std::string & sFileName )
{
  int ShouldReturnVoidAndThrowExceptionUponFailure;

  vxVessel & loadedVessel = *this;

  int4 nodeNum, nodeS, pntI;
  FILE    *fp;  
  char dummy[16];

  fopen_s ( &fp, (char*)sFileName.c_str(), "rt");
  if (!fp)
  {
    LogErr( "Can't open file " + sFileName + " not existed! ", "vxVessel", "LoadCenterlineFromTextFile" );
    return false;
  }

  fscanf_s ( fp, "%s \n", dummy, 16 ); // name of the path
  fscanf_s ( fp, "%s", dummy, 16 );
  fscanf_s ( fp, "%i \n", &nodeNum );

  fscanf_s ( fp, "%s", dummy, 16 );
  fscanf_s ( fp, "%f \n", &( loadedVessel.m_fSectionIntervalMM ) );

  //read the nodes
  nodeS = 0;
  loadedVessel.resize(0);
  Point<float4> readPnt;
  float4 dx, dy, dz;

  while ( !feof(fp) && nodeS++ < nodeNum )
  {
    fscanf_s (fp, "%i : ( %f, %f, %f ) ( %f, %f, %f )", 
      &pntI, 
      &readPnt.m_x, &readPnt.m_y, &readPnt.m_z,
      &dx, &dy, &dz );

    Point3D<float4> pointOnPath = readPnt;
    Normal3D<float4>  tanOnPath( dx, dy, dz );
    
    vxVesselSection oneSection;
    oneSection.SetPosition(pointOnPath);
    oneSection.SetTanDirection(tanOnPath);
    loadedVessel.push_back( oneSection );

    char ch;
    while ( (ch=fgetc(fp)) != '\n' );
  }

  if ( feof(fp) ) 
  {
    fclose(fp);
    return false;
  }

  fclose(fp);
  return true;

} // LoadCenterlineFromTextFile()


/**
 * Save a centerline to a text file.
 *
 * @param sFilename the output filename
 *
 * @return true on success
 */
bool vxVessel::SaveCenterlineToTextFile( const std::string & sFileName )
{
  int ShouldReturnVoidAndThrowExceptionUponFailure;

  const vxVessel & savedVessel = *this;
  std::string name = "OneCenterlineBetween2Pnts";

  FILE    *fp;
  uint2 i, num = savedVessel.size();

  fopen_s( &fp, (char*)sFileName.c_str(), "w+t");
  if (!fp)
  {
    LogErr("Can't open file " + sFileName + "for writing", "vxVessel", "SaveCenterlineToTextFile" );
    return false;
  }

  fprintf_s( fp, "%s \n", (char*)name.c_str() );
  fprintf_s( fp, "Nodes %d \n", num );
  fprintf_s( fp, "NodeInterval %.3f \n", m_fSectionIntervalMM );
  float4 fAccumulatedVolume(0);

  for ( i=0; i<num; i++ )
  {
    vxVesselSection oneSection = savedVessel[i];
    Point<float4> pointOnPath = oneSection.GetPosition();
    Normal3D<float4> tanOnPath = oneSection.GetTanDirection();
    float4 fArea = oneSection.GetLumenArea();
    ValueRange<float4> fDiameterRange = oneSection.GetDiameterRange();
    float4 fMeandiameter = oneSection.GetMeanDiameter();
    fAccumulatedVolume = oneSection.GetAccumulatedVolumeCubicMM();

    fprintf_s( fp,"%d : (%7.3f, %7.3f, %7.3f) (%7.3f, %7.3f, %7.3f) %7.3lf %7.3lf (%7.3lf, %7.3lf) %7.3lf", 
      i, pointOnPath.m_x, pointOnPath.m_y, pointOnPath.m_z,
      tanOnPath.X(), tanOnPath.Y(), tanOnPath.Z(),
      fArea, fMeandiameter, 
      fDiameterRange.GetMinimum(), fDiameterRange.GetMaximum(),
      fAccumulatedVolume
      );

    fprintf_s( fp, "\n" );

  }
  fclose(fp);

  return true;

} // SaveCenterlineToTextFile()


/**
 * generate Xml node
 *
 * @param element the xml node
 */
void vxVessel::ToXml( vxDOMElement & element ) const
{

  LogDbg( "entering vxVessel::ToXml", "vxVessel", "ToXml" );

  //const std::string & name(GetName());
  uint2 uLabel(GetLabel());
  float4 fSectionInvervalMM(GetSectionInterval());
  int4 iCalcificationThresholdHU(GetCalcificationThresholdHU());
  vxVessel::MeasurePair measurePair(GetMeasurementDomain());
  bool bStenosisValid(GetStenosisValid());
  vector< vxVesselSection > sections;
  for (int4 i(0); i<size(); ++i) sections.push_back(at(i));

  vxSerializer::SetAttribute(element, "version", "2.0");
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(uLabel), "label"), uLabel);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(fSectionInvervalMM), "section interval"), fSectionInvervalMM);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(iCalcificationThresholdHU), "calcification threshold"), iCalcificationThresholdHU);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(measurePair), "measure pair"), measurePair);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(bStenosisValid), "stenosis valid"), bStenosisValid);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sections), "sections"), sections);

} // ToXml()


/**
 * initialize from Xml node
 *
 * @param element the xml node
 *
 * @return success
 */
bool vxVessel::FromXml( vxDOMElement & element )
{

  LogDbg( "entering vxVessel::FromXml", "vxVessel", "FromXml" );

  bool bSuccess(false);

  if( element.IsNull() ) return bSuccess;

  string sVersion("0.0");
  vxSerializer::GetAttribute(element, "version", sVersion);

  if (sVersion == "2.0")
  {

    std::string name("");
    uint2 uLabel(0);
    float4 fSectionInvervalMM(0);
    int4 iCalcificationThresholdHU(0);
    vxVessel::MeasurePair measurePair;
    bool bStenosisValid(false);
    vector< vxVesselSection > sections;

    bSuccess = true;
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(uLabel), "label"), uLabel);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(fSectionInvervalMM), "section interval"), fSectionInvervalMM);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(iCalcificationThresholdHU), "calcification threshold"), iCalcificationThresholdHU);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(measurePair), "measure pair"), measurePair);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(bStenosisValid), "stenosis valid"), bStenosisValid);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(sections), "sections"), sections);

    if ( bSuccess )
    {
      //SetName(name);
      SetLabel(uLabel);
      SetSectionInterval(fSectionInvervalMM);
      SetCalcificationThresholdHU(iCalcificationThresholdHU);
      SetMeasurementDomain(measurePair);
      SetStenosisValid(bStenosisValid);
      clear();
      for (int4 i(0); i<sections.size(); ++i) push_back(sections.at(i));
    }

  } // if version 2.0
  else
  {
    LogErr("Could not read format of vxVessel", "vxSerializer", "Read");
  }

  return bSuccess;

} // FromXml()


// undefines
#undef FILE_REVISION


// $Log: vxVessel.C,v $
// Revision 1.13  2006/12/14 22:00:41  romy
// added Clone interface to Component Data
//
// Revision 1.12  2006/11/07 21:34:50  geconomos
// corrected initialzation of vessel segment size
//
// Revision 1.11  2006/09/21 14:55:14  geconomos
// added copy constructor and assignment operator
//
// Revision 1.10  2006/07/06 15:58:42  romy
// fopen Migration issues fixed
//
// Revision 1.9  2006/07/06 14:58:00  romy
// fopen to fopen_s Migration
//
// Revision 1.8  2006/07/06 13:29:22  romy
// fopen to fopen_s Migration
//
// Revision 1.7  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.6  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.1  2005/08/15 18:08:11  frank
// Issue #4365: Fixed problem with curved MPR of only one section
//
// Revision 1.5  2005/06/07 17:59:18  vxconfig
// removed name
// moved serialization to class
//
// Revision 1.4  2004/04/07 15:30:48  wenli
// GetWallArea fixed
//
// Revision 1.3  2004/03/23 21:13:03  frank
// code review, working on xml serialization for vessels
//
// Revision 1.2  2004/03/06 00:39:30  wenli
// remove cout
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/01/22 14:31:47  wenli
// no message
//
// Revision 1.4  2003/08/20 21:19:13  dongqing
// add function GetFractionalIndex()
//
// Revision 1.3  2003/08/18 14:24:28  michael
// fixed debug crash: index out of bounce
//
// Revision 1.2  2003/08/13 11:26:54  frank
// Removed pointers to sections
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.13  2003/06/30 19:02:20  dongqing
// change m_CalcificationTh back to 0
//
// Revision 1.12  2003/06/30 14:24:08  dongqing
// change the calcification threshold initialized value from 0 to 200 HU
//
// Revision 1.11  2003/06/20 14:34:09  wenli
// Default Calcification Threshold is set to 0 (avoid problem in MR cases).
//
// Revision 1.10  2003/06/19 20:09:39  wenli
// NO_AVI_SAVE
//
// Revision 1.8  2003/06/10 14:22:35  wenli
// Get Tortousity from centerlinerefiner
//
// Revision 1.7  2003/06/09 14:30:46  frank
// Prevented stenosis text from dislaying unless the stenosis line is manipulated
//
// Revision 1.6  2003/06/05 20:36:59  dongqing
// fix bug in Tortuosity computation
//
// Revision 1.5  2003/06/04 19:00:00  michael
// added namespace declaration
//
// Revision 1.4  2003/05/28 17:45:35  frank
// Displayed the vessel wall area on the graph.
//
// Revision 1.3  2003/05/27 14:03:36  frank
// Made the cross-section area more specific to lumen
//
// Revision 1.2  2003/05/14 12:02:35  frank
// Adding a new contour to the vessel section
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.32  2003/04/11 18:37:20  frank
// Extended the vessel to permit non-uniform section intervals
//
// Revision 1.31  2003/04/08 15:16:45  frank
// Properly serialized the section interval
//
// Revision 1.30  2003/04/02 19:48:58  wenli
// fInterval is added
//
// Revision 1.29  2003/03/17 18:17:26  frank
// Removed excess debug
//
// Revision 1.28  2003/03/13 14:09:43  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.27  2003/03/12 14:46:25  frank
// Changed the length to accumulated length and volume to accumulated volume, added resampled for these values, and allowed fractional length measurement between endpoints.
//
// Revision 1.26  2003/02/28 21:43:48  frank
// Added volume output to text file.
//
// Revision 1.25  2003/02/27 23:29:55  wenli
// filter out the interpolated cross-section in area statistics
//
// Revision 1.24  2003/02/27 18:46:46  frank
// Calculated the area using the contour points.
//
// Revision 1.23  2003/02/26 21:39:45  frank
// Fixed up the volume calculation.
//
// Revision 1.22  2003/02/18 21:47:47  frank
// Added function to estimate the tangents along the vessel
//
// Revision 1.21  2003/02/13 17:01:58  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.20  2003/02/11 16:39:01  wenli
// save area and diameter to the output txt file
//
// Revision 1.19  2003/02/10 21:14:12  dongqing
// fix bug in the algorithm of tortuosity
//
// Revision 1.18  2003/02/05 23:20:59  wenli
// Load and Save vxVessel to TXT file
//
// Revision 1.17.2.1  2003/02/12 23:11:08  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.17  2003/01/22 22:20:29  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.16  2003/01/22 15:37:45  frank
// Removed debug
//
// Revision 1.15  2003/01/14 16:16:41  dongqing
// Code review
//
// Revision 1.14  2002/11/27 23:30:39  frank
// volume inclusive of endpoints
//
// Revision 1.13  2002/11/27 21:41:29  wenli
// fix the size of the volumeDim-1
//
// Revision 1.12  2002/11/27 16:54:09  frank
// Added resample functions to improve the quality of graphs.
//
// Revision 1.11  2002/11/22 03:34:20  frank
// Added angle utility functions
//
// Revision 1.10  2002/11/21 19:31:38  frank
// Revised tortuosity computation
//
// Revision 1.9  2002/11/20 17:09:38  frank
// Added volume computation.
//
// Revision 1.8  2002/11/19 20:14:33  frank
// Computed tortuosity
//
// Revision 1.7  2002/11/15 02:08:08  frank
// Added SmoothOrientations()
//
// Revision 1.6  2002/11/14 18:32:32  frank
// #undef FILE_REVISION
//
// Revision 1.5  2002/11/13 00:00:37  frank
// Added function to recalculate the min/max diameter.
//
// Revision 1.4  2002/11/12 17:38:47  frank
// Removed random name
//
// Revision 1.3  2002/11/08 17:50:02  frank
// added try-catch
//
// Revision 1.2  2002/11/08 03:06:05  frank
// added try-catch
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.21  2002/10/30 23:12:27  frank
// Fixed measurement indices
//
// Revision 1.20  2002/10/30 00:02:41  frank
// Renamed a variable
//
// Revision 1.19  2002/10/29 18:27:11  frank
// went from std::pair to MeasurePair
//
// Revision 1.18  2002/10/29 16:49:58  frank
// revamped measurement
//
// Revision 1.17  2002/10/25 18:43:30  frank
// rounding
//
// Revision 1.16  2002/10/25 17:12:02  frank
// int -> int4
//
// Revision 1.15  2002/10/25 16:35:41  frank
// Serialized/Deserialized the vessel length, name, version, and percent stenosis.
//
// Revision 1.14  2002/10/25 16:07:03  frank
// Got percent stenosis to be live.
//
// Revision 1.13  2002/10/25 02:29:43  frank
// Added GetPercentStenosis()
//
// Revision 1.12  2002/09/20 22:18:26  frank
// Worked on reading/writing.
//
// Revision 1.11  2002/09/20 18:51:21  wenli
// v3D vessel component list
//
// Revision 1.10  2002/08/22 21:24:34  ana
// Vessel area range over the measured area or over the whole vessel.
//
// Revision 1.9  2002/08/22 17:31:42  ana
// Bugs fixed.
//
// Revision 1.8  2002/08/22 17:18:17  frank
// working on live updates
//
// Revision 1.7  2002/08/22 03:33:28  frank
// Added length function.
//
// Revision 1.6  2002/08/22 02:59:20  frank
// Working on visualization.
//
// Revision 1.5  2002/08/20 18:03:11  ana
// if the file is being read for the first time, it is not there. No exception should be
// thrown.
//
// Revision 1.4  2002/08/20 15:29:54  ana
// Exceptions log messages.
//
// Revision 1.3  2002/08/16 21:06:56  ana
// Test function.
//
// Revision 1.2  2002/08/15 20:39:02  ana
// Read and Write from/to file implemented.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVessel.C,v 1.13 2006/12/14 22:00:41 romy Exp $
// $Id: vxVessel.C,v 1.13 2006/12/14 22:00:41 romy Exp $
