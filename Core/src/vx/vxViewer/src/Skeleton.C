// $Id: Skeleton.C,v 1.3 2006/09/13 15:26:19 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "Skeleton.h"
#include "Serialization.h"
#include "File.h"


// Namespaces
using namespace std;
using namespace Serialization;


// Const member initialization
const SkeletonNode Skeleton::m_background;
const std::string Skeleton::m_sDelimiter = "Skeleton";


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/*
 * Default constructor
 */
Skeleton::Skeleton()
{
  //m_vSkelMarkerIndex.resize(3);
  //m_vSkelMarkerIndex[0] = m_vSkelMarkerIndex[1] = m_vSkelMarkerIndex[2] = 0;
} // Skeleton()


/**
 * Constructor for known skeleton size.
 *
 * @param uNumElements the desired number of elements
 */
Skeleton::Skeleton( const uint2 uNumElements )
{
  //m_vSkelMarkerIndex.resize(3);
  //m_vSkelMarkerIndex[0] = m_vSkelMarkerIndex[1] = m_vSkelMarkerIndex[2] = 0;
  m_vSke.resize( uNumElements );
} // Skeleton()


/**
 * Copy Constructor
 *
 * @param other the incoming data to copy
 */
Skeleton::Skeleton( const Skeleton & other )
{
  *this = other;
} // Skeleton(other)


/**
 * Destructor
 */
Skeleton::~Skeleton()
{
} // ~Skeleton()


/**
 * Resize the skeleton node array.
 *
 * @param uNumElements the desired number of elements
 */
void Skeleton::Resize( const uint2 uNumElements )
{
  m_vSke.resize( uNumElements );
} // Resize()


/**
 * Erase a particular element.
 *
 * @param uIndex the desired element to remove
 */
void Skeleton::Erase( const uint2 uIndex )
{
  if ( uIndex < m_vSke.size() )
  {
    vector<SkeletonNode>::iterator it( m_vSke.begin() + uIndex );
    m_vSke.erase( it );
  } 
  else 
  {
    throw ex::IndexOutOfBoundsException( LogRec( "Index out of bounds: " + ToAscii( uint4( uIndex ) ), "Skeleton", "Erase" ) );
  }
} // Erase()


/**
 * Read skeleton information from disk.
 *
 * @param sFileName the filename
 */
void Skeleton::Read( const string & sFileName )
{
  // check if the file exists
  vx::File skeFileTest( sFileName );
  if ( skeFileTest.Exists() == false )
    throw ex::FileNotFoundException( LogRec( "can not find .ske file: " + sFileName, "Skeleton", "Read" ) );

  // open the file
  ifstream skeFile( sFileName.c_str(),ios::binary | ios::in );
  if ( skeFile.is_open() == false )
    throw ex::IOException( LogRec( "can not open .ske file: " + sFileName, "Skeleton", "Read" ) );

  // try to read the file
  try
  {
    Read( skeFile );
  }
  catch ( ex::OutOfMemoryException & e )
  {
    throw e;
  }
  catch ( ex::DataCorruptionException & e )
  {
    throw e;
  }
  catch ( ex::EOFException & e )
  {
    throw e;
  }
  catch ( ex::IOException & e )
  {
    throw e;
  }
  catch ( ... )
  {
    throw ex::DataCorruptionException( LogRec( "Trouble reading the skeleton information from disk.", "Skeleton", "Read" ) );
  }
  
  // try to close the file
  try
  {
    skeFile.close();
  }
  catch ( ... )
  {
    throw ex::IOException( LogRec( "Trouble closing the .ske file", "Skeleton", "Read" ) );
  }
} // Read()


/**
 * Read skeleton information from disk.
 *
 * @param skeFile the file to read
 */
void Skeleton::Read( ifstream & skeFile )
{
  if ( skeFile.bad() )
    throw ex::IOException( LogRec( "Bad input stream in skeleton read", "Skeleton", "Read" ) );

  const uint4 uBufSize( 1000 );
  int1 vBuffer[uBufSize];

  uint4 uNumberOfElements;
  skeFile >> uNumberOfElements; skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize); // #0
  skeFile.getline(vBuffer,uBufSize); // #1
  skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize);
  skeFile.getline(vBuffer,uBufSize); // #9
  m_vSke.resize(uNumberOfElements);

  SkeletonNode skeletonNode;
  for ( uint4 k=0; k<m_vSke.size(); ++k )
  {
    skeFile.read( (int1 *)( & skeletonNode ), sizeof( SkeletonNode ) );
    m_vSke[k] = skeletonNode;
  }
  
  ComputeUpVectors();
} // Read()


/**
 * Retrieves the number of bytes of the serialized data.
 *
 * @return the number of bytes
 */
uint4 Skeleton::GetSerializedDataSize() const
{

  uint4 uSum( 0 );

  // starting delimiter
  uSum += string(m_sDelimiter).length() + 1;

  // std::vector<SkeletonNode> m_vSke
  uSum += sizeof(uint4);
  uSum += m_vSke.size() * sizeof(SkeletonNode);

  // ending delimiter
  uSum += string(m_sDelimiter).length() + 1;

  return uSum;

} // GetSerializedDataSize()


/**
 * Retrieves all the data in serialized form.
 *
 * @param piBuffer    a pointer to the pointer of the start of the input buffer.
 * @param piBufferEnd a pointer to the byte after the end of the valid input buffer range.
 *
 * @return true upon success
 */
void Skeleton::WriteSerializedData( int1 ** piBuffer, const int1 * piBufferEnd ) const
{

  // ensure there is enough space
  if (*piBuffer + GetSerializedDataSize() > piBufferEnd)
    throw ex::OutOfMemoryException( LogRec( "insufficient space.", "Skeleton", "WriteSerializedData" ) );

  if (*piBuffer == NULL || piBufferEnd == NULL)
    throw ex::NullPointerException( LogRec( "Null pointer", "Skeleton", "WriteSerializedData" ) );

  // write a delimiter to help verify the data
  SerializeStringData(piBuffer, piBufferEnd, m_sDelimiter);

  // std::vector<SkeletonNode> m_vSke
  const uint4 vectorSize = m_vSke.size();
  SerializeSimpleData(piBuffer, piBufferEnd, vectorSize);
  for (uint4 i=0; i<m_vSke.size(); i++)
    SerializeSimpleData(piBuffer, piBufferEnd, m_vSke[i]);

  // write a delimiter to help verify the data
  SerializeStringData(piBuffer, piBufferEnd, m_sDelimiter);

} // WriteSerializedData()


/**
 * Reads in all the data from serialized form.
 *
 * @param piBuffer    a pointer to the pointer of the start of the input buffer.
 * @param piBufferEnd a pointer to the byte after the end of the valid input buffer range.
 */
void Skeleton::ReadSerializedData(int1 ** piBuffer, const int1 * piBufferEnd)
{
  try
  {
    if (*piBuffer == NULL || piBufferEnd == NULL)
      throw ex::NullPointerException( LogRec( "Null pointer to skeleton buffer", "Skeleton", "ReadSerializedData" ) );

    // attempt to verify the stream by reading a delimiter
    string sDelimiter = DeserializeStringData(piBuffer, piBufferEnd);
    if ( sDelimiter != m_sDelimiter )
      throw ex::DataCorruptionException( LogRec( "data corruption bad starting delimiter: " + sDelimiter, "Skeleton", "ReadSerializedData" ) );

    // std::vector<SkeletonNode> m_vSke
    uint4 uVectorSize;
    DeserializeSimpleData( piBuffer, piBufferEnd, uVectorSize );
    m_vSke.resize( uVectorSize );
    for ( uint4 i(0); i<m_vSke.size(); i++ ) 
      DeserializeSimpleData( piBuffer, piBufferEnd, m_vSke[i] );

    // attempt to verify the stream by reading a delimiter
    sDelimiter = DeserializeStringData( piBuffer, piBufferEnd );
    if ( sDelimiter != m_sDelimiter )
      throw ex::DataCorruptionException( LogRec( "data corruption bad ending delimiter: " + sDelimiter, "Skeleton", "ReadSerializedData" ) );

    ComputeUpVectors();
  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "Skeleton", "ReadSerializedData" );
    throw;
  }
} // ReadSerializedData()


/**
 * Equality operator.
 *
 * @param other the other skeleton
 */
bool Skeleton::operator ==(const Skeleton & other) const
{
  // std::vector<SkeletonNode> m_vSke
  if (m_vSke.size() != other.m_vSke.size()) 
    return false;

  for ( uint4 i=0; i<m_vSke.size(); i++ )
    if ( ! ( m_vSke[i] == other.m_vSke[i] ) ) return false;

  return true;
} // operator ==


/**
 * Compute an up vector for each skeleton node.
 */
void Skeleton::ComputeUpVectors()
{
  // Compute an "up" vector for each skeleton node
  if ( m_vSke.size() > 0 )
  {
    // assume positive Y is up
    m_vSke[0].m_vWorldUp = Normal<float4>(0,1,0);

    // Enforce orthogonality between forward and up (as in Submarine,C)
    float4 dotProduct = GetSkelNode(0).m_vWorldTan.Dot(m_vSke[0].m_vWorldUp);
    if( abs( dotProduct ) == 1 )
      m_vSke[0].m_vWorldUp = Normal<float4>( 1, 0, 0 );

    
    Vector<float4> a( m_vSke[0].m_vWorldUp );
    Vector<float4> b( GetSkelNode(0).m_vWorldTan * dotProduct );
    
    m_vSke[0].m_vWorldUp = Normal<float4>( a - b );

    // loop over all skeleton nodes
    for ( uint4 n=1; n<GetNumOfElements(); ++n )
    {

      // assume same up vector as last
      m_vSke[n].m_vWorldUp = m_vSke[n-1].m_vWorldUp;

      // Enforce orthogonality between forward and up (as in Submarine,C)
      float4 dotProduct = GetSkelNode(n).m_vWorldTan.Dot(m_vSke[n].m_vWorldUp);
      m_vSke[n].m_vWorldUp = Normal<float4>(Vector<float4>(m_vSke[n].m_vWorldUp) - Vector<float4>(GetSkelNode(n).m_vWorldTan) * dotProduct);

    } // endfor all skeleton nodes

  } // end if there are any nodes at all

} // Read()


/**
 * write skeleton information to disk
 *
 * @param sFileName the file name
 */
void Skeleton::Write(const std::string & sFileName) const
{
  ofstream skeFile(sFileName.c_str(),ios::binary | ios::out);
  if ( skeFile.is_open() == false )
    throw ex::IOException( LogRec( "Could not open skeleton file.", "Skeleton", "WriteSerializedData" ) );

  try
  {
    Write( skeFile );
  }
  catch ( ex::IOException & e )
  {
    throw e;
  }
  catch (...)
  {
    throw ex::IOException( LogRec( "Trouble writing the skeleton information to disk.", "Skeleton", "Write" ) );
  }

  try
  {
    skeFile.close();
  }
  catch ( ... )
  {
    LogRec( "Trouble closing the skeleton file.", "Skeleton", "Write" );
    throw;
  }

  if ( skeFile.bad() )
  {
    throw ex::IOException( LogRec( "Trouble after writing the skeleton information to disk.", "Skeleton", "Write" ) );
  }
} // Write()


/**
 * Write skeleton information to disk.
 *
 * @param out the output stream
 */
void Skeleton::Write( std::ofstream & out ) const
{
  if ( out.bad() )
  {
    throw ex::IOException( LogRec( "<Skeleton::Write> Bad file stream.", "Skeleton", "Write" ) );
  }

  // used to write out elements here, but no longer

  if ( out.bad() )
  {
    throw ex::IOException( LogRec( "<Skeleton::Write> Bad file stream.", "Skeleton", "Write" ) );
  }
} // Write()


/*
 * Applies the scaling factor to all positions and directions
 *
 * @param factors the scale factors to apply
 */
void Skeleton::ApplyScale( const Triple<float4> & factors )
{
  int4 i;
  for ( i=0; i<m_vSke.size(); i++ )
  {
    SkeletonNode & node = m_vSke[i];
    node.m_vWorldPos = Point<float4>( m_vSke[i].m_vVolumePos );
    node.m_vWorldPos = Point<float4>( node.m_vWorldPos.GetScaledAnIso( factors ) );
  }
  for ( i=1; i<int4(m_vSke.size())-2; i++ )
  {
    Point<float4> & point0 = m_vSke[i-1].m_vWorldPos;
    Point<float4> & point1 = m_vSke[i+1].m_vWorldPos;
    m_vSke[i].m_vWorldTan = Normal<float4>( point1 - point0 );
  }
  if ( m_vSke.size() > 0 )
  {
    m_vSke[0].m_vWorldTan = m_vSke[1].m_vWorldTan;
  }
  if ( m_vSke.size() > 1 )
  {
    m_vSke[ m_vSke.size() - 1 ].m_vWorldTan = m_vSke[ m_vSke.size() - 2 ].m_vWorldTan;
  }
} // ApplyScale()


/*
 * Smooths out the skeleton
 */
void Skeleton::Smooth()
{
  // check that this is not a degenerate case (we use 5 elements to smooth)
  if ( m_vSke.size() < 5 )
    return;

  // prime the pump
  Point<float4> point1;
  Point<float4> point2;
  Point<float4> point3;
  Point<float4> point4;
  Point<float4> point5;
  point1 = m_vSke[0].m_vWorldPos;
  point2 = m_vSke[1].m_vWorldPos;
  point3 = m_vSke[2].m_vWorldPos;
  point4 = m_vSke[3].m_vWorldPos;

  // loop over the interior
  for ( uint4 i=2; i<m_vSke.size()-2; i++ )
  {
    point5 = m_vSke[i+2].m_vWorldPos;
    SkeletonNode & node = m_vSke[i];

    node.m_vWorldPos.m_x = point1.m_x*0.05F + point2.m_x*0.275F + point3.m_x*0.35F + point4.m_x*0.275F + point5.m_x*0.05F;
    node.m_vWorldPos.m_y = point1.m_y*0.05F + point2.m_y*0.275F + point3.m_y*0.35F + point4.m_y*0.275F + point5.m_y*0.05F;
    node.m_vWorldPos.m_z = point1.m_z*0.05F + point2.m_z*0.275F + point3.m_z*0.35F + point4.m_z*0.275F + point5.m_z*0.05F;

    point1 = point2;
    point2 = point3;
    point3 = point4;
    point4 = point5;
  }

  // smooth the end
  for ( uint4 j=2; j<m_vSke.size()-2; j++ )
  {
    point1 = m_vSke[j-2].m_vWorldPos;
    point2 = m_vSke[j+2].m_vWorldPos;
    SkeletonNode & node = m_vSke[j];

    node.m_vWorldTan = Normal<float4>(point2-point1);
  }
} // Smooth()


/*
 * Recalculate the DFS and DFE.
 * Useful to perform after smoothing the centerline.  This
 * better approximates the true distance, at least much
 * better than the Manhattan distance of the original.
 */
void Skeleton::RecalculateDistances()
{
  // Reset all distances to zero
  const int4 iSize = m_vSke.size();
  int4 i;
  for ( i=0; i<iSize; i++ )
  {
    m_vSke[i].m_worldDfs = 0;
    m_vSke[i].m_worldDfe = 0;
  }

  // compute distances from start
  float8 fSum(0);
  for ( i=1; i<iSize; i++ )
  {
    Vector<float4> difference = m_vSke[ i ].m_vWorldPos - m_vSke[ i - 1 ].m_vWorldPos;
    fSum += difference.GetLength();
    m_vSke[ i ].m_worldDfs = fSum;
  }

  // compute distances from end
  fSum = 0;
  for ( i=iSize-2; i>=0; i-- )
  {
    Vector<float4> difference = m_vSke[ i ].m_vWorldPos - m_vSke[ i + 1 ].m_vWorldPos;
    fSum += difference.GetLength();
    m_vSke[ i ].m_worldDfe = fSum;
  }
} // RecalculateDistances()


/**
 * Get the piecewise Euclidean length
 *
 * @return the total Euclidean length 
 */
float4 Skeleton::GetPiecewiseEuclideanLength() const
{
  // compute distances from start
  float4 fSum( 0.0F );
  for ( uint4 i(0); i<m_vSke.size()-1; ++i )
  {
    const Point<float4> p1 = m_vSke[ i     ].m_vWorldPos;
    const Point<float4> p2 = m_vSke[ i + 1 ].m_vWorldPos;
    const Vector<float4> difference( p1 - p2 );
    fSum += difference.GetLength();
  }
  return fSum;
} // GetPiecewiseEuclideanLength()


/**
 * Compute the tangent directions using a specified central difference radius.
 *
 * @param fRadius the filter radius
 */
void Skeleton::ComputeTangents( const float4 & fRadius )
{
  // loop over all elements
  for ( int4 i(0); i<GetNumOfElements(); ++i )
  {
    // get two skeleton nodes some distance apart
    int4 index1( static_cast< int >( i - fRadius + 0.5F ) ); // round to
    int4 index2( static_cast< int >( i + fRadius + 0.5F ) ); // nearest integer
    index1 = Bound( int(0), index1, static_cast<int>( GetNumOfElements() - 1 ) );
    index2 = Bound( int(0), index2, static_cast<int>( GetNumOfElements() - 1 ) );

    // see if there is any data to difference
    if ( index1 != index2 )
    {
      Point<float4> point1( GetWorldPos( index1 ) );
      Point<float4> point2( GetWorldPos( index2 ) );
      SetWorldTan( i, Normal<float4>( point2 - point1 ) );
    }
    else
    {
      // just try a simple central difference then
      Point<float4> point1( GetWorldPos( i + 0 ) );
      Point<float4> point2( GetWorldPos( i + 1 ) );
      SetWorldTan( i, Normal<float4>( point2 - point1 ) );
    } // any data to difference

  } // loop over all elements

} // ComputeTangents()


/**
 * Allow to direct a SkeletonNode to an output stream
 *
 * @param os reference to std::ostream output stream
 */
ostream & Skeleton::AsOstream( ostream & os ) const
{
  os << "Skeleton{" << endl;
  os << "}" << endl;
  return os;
} // AsOstream()


/**
 * Calculate the inside angle of 3 points.
 *
 * @return the inside angle
 */
//float4 Skeleton::CalculateInsideAngle() const
//{
//  Point<float> vP0( GetSkelNode( m_vSkelMarkerIndex[0] ).m_vWorldPos );
//  Point<float> vP1( GetSkelNode( m_vSkelMarkerIndex[1] ).m_vWorldPos );
//  Point<float> vP2( GetSkelNode( m_vSkelMarkerIndex[2] ).m_vWorldPos );
//  Vector<float4> vV1( vP0 - vP1 );
//  vV1.Normalize();
//  Vector<float4> vV2( vP2 - vP1 );
//  vV2.Normalize();
//  return acos( vV1.Dot(vV2) ) * 180.0F / M_PI;
//} // CalculateInsideAngle()


/**
 * Calculate the outside angle of 3 points.
 *
 * @param the outside angle
 */
//float Skeleton::CalculateOutsideAngle() const
//{
//  Point<float> vP0( GetSkelNode( m_vSkelMarkerIndex[0] ).m_vWorldPos );
//  Point<float> vP1( GetSkelNode( m_vSkelMarkerIndex[1] ).m_vWorldPos );
//  Point<float> vP2( GetSkelNode( m_vSkelMarkerIndex[2] ).m_vWorldPos );
//  Normal<float> vT0( GetSkelNode( m_vSkelMarkerIndex[0] ).m_vWorldTan );
//  Normal<float> vT2( GetSkelNode( m_vSkelMarkerIndex[2] ).m_vWorldTan );
//
//  // Compute the plane which contains vP0,vP1,vP2
//  Vector<float4> vV1( vP0 - vP1 );
//  vV1.Normalize();
//  Vector<float4> vV2( vP2 - vP1 );
//  vV2.Normalize();
//  Vector<float4> vPlaneNormal( vV1.Cross( vV2 ) );
//  vPlaneNormal.Normalize();
//  float4 fPlane1A( vPlaneNormal.X() );
//  float4 fPlane1B( vPlaneNormal.Y() );
//  float4 fPlane1C( vPlaneNormal.Z() );
//  float4 fPlane1D( -( fPlane1A * vP0.X() + fPlane1B * vP0.Y() + fPlane1C * vP0.Z() ) );
//  
//  // Flip one of the tangents. Otherwise, they both point
//  // in the same direction along the centerline, instead of
//  // away from the vertex point (which is what we want).
//  // We can flip an arbitrary tangent, 
//  vT0 = -vT0;
//  // project the tangents to the plane
//  float4 fDotProduct( vPlaneNormal.Dot( vT0 ) );
//  Vector<float4> vT0prime( Vector<float4>( vT0 ) - vPlaneNormal * fDotProduct );
//  vT0prime.Normalize();
//  fDotProduct = vPlaneNormal.Dot(vT2);
//  Vector<float4> vT2prime( Vector<float4>( vT2 ) - vPlaneNormal * fDotProduct );
//  vT2prime.Normalize();
//  
//  // return the angle between the tangents
//  return acos( vT0prime.Dot( vT2prime ) ) * 180.0F / M_PI;
//
//} // CalculateOutsideAngle()


/**
 * Reverses the order of points in the skeleton.
 * It only preserves the m_vVolumePos field.
 */
void Skeleton::ReverseOrder()
{
  Skeleton other;
  for ( uint4 i(0); i<m_vSke.size(); ++i )
  {
    SkeletonNode node;
    node.m_vVolumePos = m_vSke[ m_vSke.size() - 1 - i ].m_vVolumePos;
    other.AddSkelNode( node );
  }

  *this = other;

} // ReverseOrder()


/**
 * Concatenates a skeleton to the end of the current skeleton.
 *
 * @param otherSkeleton the other skeleton
 */
void Skeleton::ConcatentateSkeleton( const Skeleton & otherSkeleton )
{
  for ( uint4 i(0); i<otherSkeleton.GetNumOfElements(); ++i )
  {
    AddSkelNode( otherSkeleton.GetSkelNode( i ) );
  }
} // ConcatentateSkeleton()


/**
 * Gets the size of the array
 *
 * @return the size of the array
 */
uint4 Skeleton::GetNumOfElements() const
{
  return m_vSke.size();
} // GetNumOfElements()


/**
 * get complete Skeleton Node: we need this version in some cases. 
 *
 * @param uIndex         the desired skeleton node index
 * @param bNotOutOfBound whether to clamp it to the end of the array
 *
 * @return 
 */
const SkeletonNode & Skeleton::GetSkelNode( const uint2 & uIndex, const bool & bNotOutOfBound ) const
{
  uint2 uMyIndex( uIndex );

  if ( uMyIndex >= GetNumOfElements() ) 
  {
    if ( bNotOutOfBound )
    {
      uMyIndex = GetNumOfElements() - 1;
    }
    else
    {
      std::stringstream tt;
      tt << "tried to GetSkelNode out of bounds index was: " << uint4( uIndex );
      tt << " and there are only " << GetNumOfElements() << " elements" << std::ends;
      LogWrn( tt.str(), "Skeleton", "GetSkelNode" );
      return m_background;
    }
  }
  return m_vSke[ uMyIndex ];
} // GetSkelNode()


/**
 * get 3D position along the skeleton in World coordinates.
 *
 * @param uIndex the desired skeleton index
 *
 * @return the world position
 */
const Point<float4> & Skeleton::GetWorldPos( const uint2 & uIndex ) const
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to read Skeleton Node m_vWorldPos out of bounds "
       << "index was: " << uint4(uIndex) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn(tt.str(),"Skeleton","GetWorldPos");
    return m_background.m_vWorldPos;
  }
  return m_vSke[uIndex].m_vWorldPos;
} // GetWorldPos()


/**
 * Get 3D position along the skeleton in volume coordinates.
 *
 * @param uIndex the desired skeleton index
 *
 * @return the volume position
 */
const Point<uint2> & Skeleton::GetVolumePos( const uint2 & uIndex ) const
{
  static Point<uint2> dummy;

  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to read Skeleton Node m_vVolumePos out of bounds", "Skeleton", "GetVolumePos" );
    dummy = Point<uint2>( m_background.m_vVolumePos );
    return dummy;
  }
  
  dummy = Point<uint2>( m_vSke[uIndex].m_vVolumePos );
  return dummy;

} // GetVolumePos()


/*
 * get skeleton tangent vector in World coordinates.
 *
 * @param uIndex the desired skeleton index
 *
 * @return the world tangent vector
 */
const Normal<float4> & Skeleton::GetWorldTan( const uint2 & uIndex ) const
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to read Skeleton Node m_vWorldTan out of bounds", "Skeleton", "GetWorldTan" );
    return( m_background.m_vWorldTan );
  }
  return m_vSke[ uIndex ].m_vWorldTan;
} // GetWorldTan()


/*
 * get distance from start in World coordinates.
 *
 * @param uIndex the desired skeleton index
 *
 * @return the distance from the start in world coordinates
 */
float4 Skeleton::GetWorldDfs( const uint2 & uIndex ) const
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to read Skeleton Node m_worldDfs out of bounds", "Skeleton", "GetWorldDfs" );
    return m_background.m_worldDfs;
  }
  return m_vSke[ uIndex ].m_worldDfs;
} // GetWorldDfs


/*
 * get distance from end in World coordinates.
 *
 * @param uIndex the desired skeleton index
 *
 * @return the distance from the end in world coordinates
 */
float4 Skeleton::GetWorldDfe( const uint2 & uIndex ) const
{
  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to read Skeleton Node m_worldDfs out of bounds", "Skeleton", "GetWorldDfe" );
    return m_background.m_worldDfe;
  }

  return m_vSke[ uIndex ].m_worldDfe;
} // GetWorldDfe()


/*
 * set complete skeleton node
 *
 * @param uIndex  the skeleton index to set
 * @param skeNode the new node
 *
 * @return true if set
 */
bool Skeleton::SetSkelNode( const uint2 & uIndex, const SkeletonNode & skeNode )
{

  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to set Skeleton Node out of bounds", "Skeleton", "SetSkelNode" );
    return false;
  }

  m_vSke[ uIndex ] = skeNode;
  return true;

} // SetSkelNode()


/*
 * set 3D position along the skeleton in World coordinates
 *
 * @param uIndex the skeleton index
 * @param vPos   the new world position
 *
 * @return true if set
 */
bool Skeleton::SetWorldPos( const uint2 & uIndex, const Point<float4> & vPos )
{

  if ( uIndex >= GetNumOfElements() ) 
  {
    std::stringstream tt;
    tt << "tried to set Skeleton Node m_vWorldPos out of bounds" << std::endl;
    tt << "index was: " << uint4( uIndex ) << " and there are only " << GetNumOfElements() << " elements" << std::ends;
    LogWrn( tt.str(), "Skeleton", "SetWorldPos" );
    return false;
  }

  m_vSke[ uIndex ].m_vWorldPos = vPos;
  return true;

} // SetWorldPos()


/*
* set skeleton tangent vector in World coordinates
*
* @param uIndex the skeleton index
* @param vTan   the new tangent vector
*
* @return true if set
*/
bool Skeleton::SetWorldTan( const uint2 & uIndex, const Normal<float4> & vTan )
{

  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to set Skeleton Node m_vWorldTan out of bounds", "Skeleton", "SetWorldTan" );
    return false;
  }

  m_vSke[ uIndex ].m_vWorldTan = vTan;
  return true;

} // SetWorldTan()


/*
 * set distance from start in World coordinates.
 *
 * @param uIndex the skeleton index
 * @param vPos   the new world position
 *
 * @return true if set
 */
bool Skeleton::SetWorldDfs( const uint2 & uIndex, const float4 fDfs )
{

  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to set Skeleton Node m_worldDfs out of bounds", "Skeleton", "SetWorldDfs" );
    return false;
  }

  m_vSke[ uIndex ].m_worldDfs = fDfs;
  return true;

} // SetWorldDfs()


/**
 * Set distance from end in World coordinates
 *
 * @param uIndex the array index
 * @param fDfe   the new distance
 *
 * @return true if successful
 */
bool Skeleton::SetWorldDfe( const uint2 & uIndex, const float4 fDfe )
{

  if ( uIndex >= GetNumOfElements() ) 
  {
    LogWrn( "tried to set Skeleton Node m_worldDfe out of bounds", "Skeleton", "SetWorldDfe" );
    return false;
  }

  m_vSke[ uIndex ].m_worldDfe = fDfe;
  return true;

} // SetWorldDfe()


/**
 * Set 3D position along the skeleton in World coordinates.
 *
 * @param uIndex the array index
 * @param pos    the new position
 *
 * @return true if successful
 */
bool Skeleton::SetVolumePos( const uint2 & uIndex, const Point<float4> & pos )
{
  if ( uIndex >= GetNumOfElements() )
  {
    LogWrn( "tried to set Skeleton Node m_worldPos out of bounds", "Skeleton", "SetVolumePos" );
    return false;
  }

  m_vSke[ uIndex ].m_vVolumePos = Point<uint4>( pos );
  return true;

} // SetVolumePos()


/**
 * Add a new skeleton node.
 *
 * @param node the new skeleton node
 *
 * @return true if successful
 */
bool Skeleton::AddSkelNode( const SkeletonNode & node )
{

  m_vSke.push_back( node );

  return true;

} // SetSkelNode()


/**
 * Get an interpolated position along the skeleton in world coordinates.
 *
 * @param fIndex the desired fractional array index
 *
 * @return the interpolated skeleton world position
 */
Point<float4> Skeleton::GetInterpolatedWorldPos( const float4 fIndex ) const
{

  Point<float4> returnPoint( 0.0F, 0.0F, 0.0F );

  if ( GetNumOfElements() > 0 )
  {
    if ( fIndex >= 0 )
    {
      if ( fIndex < GetNumOfElements()-1 )
      {
        const int4 index0( fIndex );
        Vector<float4> pos0( m_vSke[ index0   ].m_vWorldPos );
        Vector<float4> pos1( m_vSke[ index0+1 ].m_vWorldPos );
        const float4 fWeight1( fIndex - float4( index0 ) );
        const float4 fWeight0( 1.0F - fWeight1 );
        returnPoint = Point<float4>( pos0 * fWeight0 + pos1 * fWeight1 );
      }
      else
      {
        returnPoint = Point<float4>( m_vSke[ GetNumOfElements()-1 ].m_vWorldPos );
      }
    }
    else
    {
      returnPoint = m_vSke[0].m_vWorldPos;
    }
  }

  return returnPoint;

} // GetInterpolatedWorldPos()


/**
 * Rotate a prone centerline to supine
 *
 * @param worldSize the size of the entire volume in MM
 */
void Skeleton::RotateProneToSupine( const Triple<float4> & worldSize )
{

  for ( std::vector< SkeletonNode >::iterator iter = m_vSke.begin(); iter != m_vSke.end(); ++iter )
    iter->RotateProneToSupine( worldSize );

} // RotateProneToSupine()


// undefines
#undef FILE_REVISION 


// $Log: Skeleton.C,v $
// Revision 1.3  2006/09/13 15:26:19  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.2  2005/08/05 13:06:37  geconomos
// added a check in ComputeUpVectors() to enusre correctness of "up" vector
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.15  2003/05/27 13:17:53  frank
// Removed dead includes
//
// Revision 3.14  2003/05/14 12:58:48  frank
// continued code review
//
// Revision 3.13  2003/05/06 16:18:49  frank
// code review
//
// Revision 3.12  2003/01/22 21:37:32  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.11  2001/01/02 18:55:22  ingmar
// explicit typecast
//
// Revision 3.10  2002/11/20 14:20:57  dongqing
// add copy constructor
//
// Revision 3.9  2002/10/29 19:35:34  kevin
// Cleaned up debug so the log files look cleaner
//
// Revision 3.8  2002/10/21 18:54:52  kevin
// Cleaned up some debug
//
// Revision 3.7  2002/10/14 20:21:08  geconomos
// Removed inline specifier from GetInterpolatedWorlPos method.
//
// Revision 3.6  2002/09/06 16:10:58  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.5  2002/08/14 16:10:34  wenli
// initial version - Vascular
//
// Revision 3.4  2002/05/03 22:25:46  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 3.3  2002/03/25 18:00:07  geconomos
// no message
//
// Revision 3.2  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 3.1.2.1  2002/08/06 20:43:45  jmeade
// Issue 2596:  Condensed two equivalent functions into one; Code standards.
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
//    Rev 2.2   Oct 04 2001 10:50:52   jmeade
// Removed frivolous log warning in equality operator (should be logged by caller if necessary); code standards
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
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.40  2001/08/13 20:05:44  tushar
// removed errors due to the new logger
//
// Revision 1.39  2001/08/13 00:24:28  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.38  2001/08/08 15:25:47  frank
// Fixed misuse of uint4 where an int4 is required.
//
// Revision 1.37  2001/07/31 19:25:38  frank
// Added a concatenation function.
//
// Revision 1.36  2001/07/26 20:28:51  binli
// ID 89: exception handling.
//
// Revision 1.35  2001/07/25 02:42:03  tushar
// fixed exception bug
//
// Revision 1.34  2001/07/16 23:40:45  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.33  2001/07/13 19:52:16  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.32  2001/07/12 21:14:14  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.31  2001/06/08 15:03:29  dave
// changed array to std::vect, no copy, = constructors
//
// Revision 1.30  2001/05/15 17:49:39  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.29  2001/05/02 19:32:38  ingmar
// Segment.C
//
// Revision 1.27.2.1  2001/05/01 21:51:26  jmeade
// Added exception handling, cleaned up code. (Frank D.)
//
// Revision 1.28  2001/04/26 11:17:33  frank
// Added exception handling, cleaned up code.
//
// Revision 1.27  2001/04/05 18:00:23  dmitry
// updated datatypes
//
// Revision 1.26  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.25  2001/03/26 17:21:10  frank
// Moved serialization functions to separate namespace.
//
// Revision 1.24  2001/02/07 18:57:22  frank
// Fixed distance calculation.
//
// Revision 1.23  2000/12/18 19:51:52  frank
// Removed stringstream I/O
//
// Revision 1.22  2000/12/14 14:52:39  frank
// Modified the interface for serialization/deserialization
//
// Revision 1.21  2000/12/13 15:06:45  frank
// Added equality operators
//
// Revision 1.20  2000/12/12 16:43:41  frank
// Added delimiters to serialized data stream
//
// Revision 1.19  2000/12/09 18:17:25  frank
// Added serialized input
//
// Revision 1.18  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.17  2000/12/09 14:43:54  frank
// Stream I/O was too slow, went to temp binary file
//
// Revision 1.16  2000/12/09 12:54:40  frank
// Improved stream I/O
//
// Revision 1.15  2000/12/05 19:57:25  frank
// Added I/O functions
// Added ComputeUpVectors function
//
// Revision 1.14  2000/11/22 14:30:35  frank
// Added the erase function
//
// Revision 1.13  2000/11/20 21:45:50  kevin
// Fixed Start/End skel distances
//
// Revision 1.12  2000/11/16 15:45:16  frank
// Added tangent computation function
//
// Revision 1.11  2000/11/13 12:11:48  frank
// Changed comment
//
// Revision 1.10  2000/11/06 19:54:37  frank
// Added static data
//
// Revision 1.9  2000/11/06 19:45:39  frank
// Removed branchidx stuff
// Cleaned up error cases
//
// Revision 1.8  2000/11/06 19:24:18  frank
// Fixed order reversal
//
// Revision 1.7  2000/11/06 18:41:25  frank
// Added function to reverse the node order
//
// Revision 1.6  2000/11/03 19:10:49  ingmar
// fixed gcc warnings
//
// Revision 1.5  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.4.6.4  2000/10/30 19:13:48  frank
// removed old debugging code
//
// Revision 1.4.6.3  2000/10/30 17:05:58  frank
// merged back using old v2k branch
//
// Revision 1.4.6.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.4.6.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.4.2.2  2000/10/27 15:20:54  frank
// Got segment structure working
//
// Revision 1.4.2.1  2000/10/26 13:24:55  frank
// Added new segment structure
//
// Revision 1.4  2000/10/20 17:12:00  kevin
// Added up vector for each skeleton node
//
// Revision 1.3.2.3  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.3.2.2  2000/10/20 18:53:59  frank
// Cleaned up legacy structures
//
// Revision 1.3.2.1  2000/10/19 19:05:26  frank
// Added up direction and revised scale function
//
// Revision 1.3  2000/09/28 20:18:26  frank
// Fixed file red procedure
//
// Revision 1.2  2000/09/28 17:10:00  frank
// Added I/O for segments and segment lists
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
//
// 41    9/05/00 9:16p Ingmar
// towards compile in linux
//
// 40    9/02/00 4:22p Kevin
// Changed to larger neighborhood for smoother gradient directions
//
// 39    8/31/00 12:15p Frank
// Finished recalculating distances
//
// 38    8/31/00 11:25a Dave
//
// 37    8/31/00 11:19a Dave
// some gcc warnings okay
//
// 36    8/31/00 9:58a Frank
// Work in progress
//
// 35    8/31/00 9:52a Frank
// Updated recalculation logic
//
// 34    8/29/00 3:21p Frank
// Added DF* recalculation function
//
// 33    8/20/00 6:49p Sarang
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// 32    8/16/00 2:03p Antonio
// modified header and footer
//
// *****************  Version 31  *****************
// User: Kevin        Date: 8/16/00    Time: 8:08a
// Updated in $/Viatron1000/src/Volume
// Check for degenerate case on smooth
//
// *****************  Version 30  *****************
// User: Antonio      Date: 8/11/00    Time: 12:39p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 29  *****************
// User: Jay             Date:  8/11/00  Time:  1:33a
// Checked in $/v2k/src/Volume
// Comment:
//
//
// *****************  Version 28  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 27  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 26  *****************
// User: Kevin           Date:  8/06/00  Time:  7:02p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added Picking of segments for order. To accomplish this, I
// pulled the guts out of some data structures and replaced them
//
// *****************  Version 25  *****************
// User: Kevin           Date:  8/04/00  Time: 12:54p
// Checked in $/v2k/src/Volume
// Comment:
//   Put read/write of endIdx back in
//
// *****************  Version 24  *****************
// User: Kevin           Date:  8/04/00  Time: 12:53p
// Checked in $/v2k/src/Volume
//
// *****************  Version 23  *****************
// User: Kevin           Date:  8/04/00  Time:  9:54a
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Temp fix for endIndex bug. This should eventually get overwritten
// with Micheals new stuff
//
// *****************  Version 22  *****************
// User: Kevin           Date:  8/03/00  Time:  4:47p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added branches back
//
// *****************  Version 21  *****************
// User: Michael         Date:  8/03/00  Time:  3:48p
// Checked in $/v2k/src/Volume
// Comment:
//   start/end indices
//
// *****************  Version 20  *****************
// User: Lihong          Date:  8/02/00  Time:  1:16a
// Checked in $/v2k/src/Volume
// Comment:
//   added vector of segment start indices
//
// *****************  Version 19  *****************
// User: Ingmar          Date:  7/28/00  Time: 10:48a
// Checked in $/v2k/src/Volume
// Comment:
//   added comments for read and write functiuons
//
// *****************  Version 18  *****************
// User: Kevin           Date:  7/26/00  Time:  6:57p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added a simple skeleton smooth function
//
// *****************  Version 17  *****************
// User: Kevin           Date:  7/26/00  Time:  6:03p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Fixed "ApplyScale" (NOTE: "ScaleAniso" does not work correctly)
//
// *****************  Version 16  *****************
// User: Kevin           Date:  7/26/00  Time: 12:05p
// Checked in $/v2k/src/Volume
// Comment:
//   Added ios::bin to binary ouput
//
// *****************  Version 15  *****************
// User: Frank           Date:  7/25/00  Time: 10:57a
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added a function to scale the skeleton
//
// *****************  Version 14  *****************
// User: Ingmar          Date:  7/25/00  Time:  8:15a
// Checked in $/v2k/src/Volume
// Comment:
//   added debug print
//
// *****************  Version 13  *****************
// User: Frank           Date:  7/24/00  Time:  4:05p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Removed Draw()
//
// *****************  Version 12  *****************
// User: Ingmar          Date:  7/24/00  Time:  9:40a
// Checked in $/v2k/src/Skeleton
// Comment:
//   replaced old array with std::vector
//
// *****************  Version 11  *****************
// User: Ingmar          Date:  7/24/00  Time:  9:23a
// Checked in $/v2k/src/Skeleton
// Comment:
//   deleted Debug tracing
//
// *****************  Version 10  *****************
// User: Ingmar          Date:  7/24/00  Time:  8:58a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added Error debug when write happenes
//
// *****************  Version 9  *****************
// User: Ingmar          Date:  7/24/00  Time:  8:41a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added AsOstream
//
// *****************  Version 8  *****************
// User: Ingmar          Date:  7/23/00  Time:  7:15p
// Checked in $/v2k/src/Skeleton
// Comment:
//   chnaged read/write skeleton to show the first 10 entries
//
// *****************  Version 7  *****************
// User: Michael         Date:  7/23/00  Time:  6:15p
// Checked in $/v2k/src/Skeleton
// Comment:
//   add branch output structure
//
// *****************  Version 6  *****************
// User: Ingmar          Date:  7/23/00  Time:  4:32p
// Checked in $/v2k/src/Skeleton
// Comment:
//   made Resize copy data from old to new array
//
// *****************  Version 5  *****************
// User: Ingmar          Date:  7/23/00  Time:  3:38p
// Checked in $/v2k/src/Skeleton
// Comment:
//   moved finding skeleton into Skeletonator, Skeleton now just holde
// the Skeleton Array
//
// *****************  Version 4  *****************
// User: Mie             Date:  7/19/00  Time: 12:02p
// Checked in $/v2k/src/Skeleton
// Comment:
//   deleted argv dependence
//
// *****************  Version 3  *****************
// User: Mie             Date:  7/19/00  Time: 11:06a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added cmd line switch args to skeleton
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/19/00  Time: 10:31a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added CmdLineSwtiches to Skeleton class
//
// *****************  Version 1  *****************
// User: Michael         Date:  7/18/00  Time:  7:16p
// Created Skeleton.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/Skeleton.C,v 1.3 2006/09/13 15:26:19 frank Exp $
// $Id: Skeleton.C,v 1.3 2006/09/13 15:26:19 frank Exp $
