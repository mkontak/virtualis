// $Id: vxVoxelizer.C,v 1.3 2007/03/01 19:04:36 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxVoxelizer.h"
#include <fstream>
#include <vector>
#include "Matrix.h"
#include "Box.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * constructor
 */
vxVoxelizer::vxVoxelizer()
{
}


/**
 * destructor
 */
vxVoxelizer::~vxVoxelizer()
{
}

  
/**
 * read a set of triangles from disk
 *
 * @param 
 */
void vxVoxelizer::ReadFileOFF( const std::string & sFilename )
{
  std::vector<Point3Df> vertices;
  std::ifstream off( sFilename.c_str() );
  if ( ! off )
    throw ex::DataCorruptionException( LogRec( "unabled to read file: " + sFilename, "vxVoxelizer", "ReadFileOff" ) );

  // read the header information
  const int iLineLength(5000);
  char line [ iLineLength ];
  off.getline( line, iLineLength );
  char header[10];
  sscanf( line, "%s", header );
  off.getline( line, iLineLength );
  int numVerts, numTri, numEdges;
  sscanf( line, "%i %i %i", & numVerts, & numTri, & numEdges );
  //off >> header >> numVerts >> numTri >> numEdges;
  
  // read the vertices
  for (int i=0; i<numVerts; i++)
  {
    Point3Df p;
    off.getline( line, iLineLength );
    sscanf( line, "%f %f %f", & p.m_x, & p.m_y, & p.m_z );
    vertices.push_back( p );
  }

  // read the triangles
  int4 iGood(0);
  int4 iBad(0);
  for (int i=0; i<numTri; i++)
  {
    int n=0,p1,p2,p3;
    off.getline( line, iLineLength );
    sscanf( line, "%i %i %i %i", & n, & p1, & p2, & p3 );
    if (n != 3)
    {
      LogWrn( std::string( "problem - not a triangle: " ) + line, "vxVoxelizer", "ReadFileOff" );
      iBad++;
    }
    else
    {

      // test for a valid triangle
      Point3Df & v1 = vertices[p1];
      Point3Df & v2 = vertices[p2];
      Point3Df & v3 = vertices[p3];
      const float4 fEpsilon( 0.001f );
      if ( Vector3Df( v1 - v2 ).GetLength() < fEpsilon ||
           Vector3Df( v1 - v3 ).GetLength() < fEpsilon ||
           Vector3Df( v2 - v3 ).GetLength() < fEpsilon ||
           p1 < 0 && p1 >= vertices.size() ||
           p2 < 0 && p2 >= vertices.size() ||
           p3 < 0 && p3 >= vertices.size() )
      {
        iBad++;
      }
      else
      {
        iGood++;
        m_triangles.push_back( v1 );
        m_triangles.push_back( v2 );
        m_triangles.push_back( v3 );
      }
    }
  }
  LogDbg( "good triangles: " + ToAscii( iGood ), "vxVoxelizer", "VoxelizeTriangles" );
  LogDbg( "bad triangles: " + ToAscii( iBad ), "vxVoxelizer", "VoxelizeTriangles" );
}

/**
 * Gets the bounding box for the specified points
 * 
 * @param   points      input ponts
 * @return  bounding box
 */
Box<float4> GetBounds( const std::vector< Point3D< float4 > > & points )
{

  if ( points.size() == 0 )
    return Box<float4>( 0, 0, 0, 1, 1, 1 );

  Box<float4> bounds( points[0], points[0] );
  for (int i=0; i<points.size(); i++)
    bounds.Union( points[i] );

  return bounds;

} // GetBounds()


/**
 * primary voxelization function
 *
 * @param volume the output volume. It should be clear and isotropic
 */
void vxVoxelizer::VoxelizeTriangles( vxBlockVolume<uint2> & volume )
{

  //
  // transform the triangles
  //

  // get the bounds of the triangle mesh
  Box<float4> bounds = GetBounds( m_triangles );
  LogDbg( "Original bounds: " + bounds.ToString(), "", "" );

  // scale to fit within the given volume
  vxVolumeHeader & header = volume.GetHeader();
  int4 iCriticalDimension = bounds.GetDim().GetDividedAnIso( header.GetWorldDim() ).GetMaxAbsComponentCoord();
  float4 fCriticalVolumeDimension = header.GetWorldDim()[ iCriticalDimension ];
  float4 fCriticalMeshDimension = bounds.GetDim()[ iCriticalDimension ];

  // move back to the origin
  Matrix<float4> matrix;
  matrix.Translate( - Vector3Df( bounds.GetMinPoint() ) );
  
  // scale to fit the smallest volume dimension
  const float4 fPadding( 8 );
  const float4 fScaleFactor = static_cast< float4 >( fCriticalVolumeDimension - 2 * fPadding ) / fCriticalMeshDimension;
  const Vector3Df unitVector( 1, 1, 1 );
  matrix.Scale( unitVector * fScaleFactor );

  // recenter in the volume
  Point3Df newMeshCenter( Vector3Df( bounds.GetDim() ) * fScaleFactor / 2.0f );
  Point3Df volumeCenter( Vector3Df( header.GetWorldDim() ) / 2.0f );
  matrix.Translate( volumeCenter - newMeshCenter );

  // transform
  for ( int i=0; i<m_triangles.size(); i++ )
    m_triangles[i] = matrix * m_triangles[i];
  LogDbg( "Bounds after transformation: " + GetBounds( m_triangles ).ToString(), "", "" );
 
  float points[ 9 ];
  LogDbg( "voxelizing " + ToAscii( m_triangles.size() / 3 ) + " triangles", "vxVoxelizer", "VoxelizeTriangles" );
  LogDbg( "volume is " + header.GetVolDim().ToString(), "", "" );
  vxBlockVolumeIterator<uint2> volIter( volume );
  for ( int4 i=0; i<m_triangles.size(); i+=3 )
  {
    const Point3Df & p1 = m_triangles[ i + 0 ];
    const Point3Df & p2 = m_triangles[ i + 1 ];
    const Point3Df & p3 = m_triangles[ i + 2 ];
    points[0] = p1.m_x;
    points[1] = p1.m_y;
    points[2] = p1.m_z;
    points[3] = p2.m_x;
    points[4] = p2.m_y;
    points[5] = p2.m_z;
    points[6] = p3.m_x;
    points[7] = p3.m_y;
    points[8] = p3.m_z;
    VoxelizeTriangle( volIter, volume.GetHeader().GetVolDim(), points );
  }
  LogDbg( "done voxelizing", "vxVoxelizer", "VoxelizeTriangles" );
}


/**
 * transform the triangles
 *
 * @param transformation the transformation matrix
 */
void vxVoxelizer::Transform( const Matrix<float4> & transformation )
{

  for ( int i=0; i<m_triangles.size(); i++ )
    m_triangles[i] = transformation * m_triangles[i];

} // Transform( matrix )


/**
 * voxelizes a triangle
 *
 * @param   volIter             volume iterator
 * @param   volumeDimensions    volume dimensions
 * @param   points              points
 */
void vxVoxelizer::VoxelizeTriangle( vxBlockVolumeIterator<uint2> & volIter, const Triple<int4> & volumeDimensions, float4 * points )
{

  const float fMaximumValue = 4095.0F;
  const float three =  3.464101615138f; //2*sqrt(3)
  const float oneOverThree =  0.2886751345948f; // 1/(2*sqrt(3))
  const float width = 0.5f;
  const float threeHalf = three + width;
  const float xdim = volumeDimensions.m_x;
  const float ydim = volumeDimensions.m_y;
  const float zdim = volumeDimensions.m_z;
  float & ax = points[0], & ay = points[1], & az = points[2];
  float & bx = points[3], & by = points[4], & bz = points[5];
  float & cx = points[6], & cy = points[7], & cz = points[8];

  // get vectors
  float vabx = float(bx - ax);
  float vaby = float(by - ay);
  float vabz = float(bz - az);
  float vabLength = float(sqrt(vabx*vabx + vaby*vaby + vabz*vabz));
  if ( vabLength == 0 )
    return;
  float normalize = float(1.0 / (vabLength * vabLength));
  vabx *= normalize;
  vaby *= normalize;
  vabz *= normalize;
  float vabOffset = float(-(vabx*ax + vaby*ay + vabz*az));

  float vbcx = float(cx - bx);
  float vbcy = float(cy - by);
  float vbcz = float(cz - bz);
  float vbcLength = float(sqrt(vbcx*vbcx + vbcy*vbcy + vbcz*vbcz));
  if ( vbcLength == 0 )
    return;
  normalize = float(1.0 / (vbcLength * vbcLength));
  vbcx *= normalize;
  vbcy *= normalize;
  vbcz *= normalize;
  float vbcOffset = float(-(vbcx*bx + vbcy*by + vbcz*bz));

  float vcax = float(ax - cx);
  float vcay = float(ay - cy);
  float vcaz = float(az - cz);
  float vcaLength = float(sqrt(vcax*vcax + vcay*vcay + vcaz*vcaz));
  if ( vcaLength == 0 )
    return;
  normalize = float(1.0 / (vcaLength * vcaLength));
  vcax *= normalize;
  vcay *= normalize;
  vcaz *= normalize;
  float vcaOffset = float(-(vcax*cx + vcay*cy + vcaz*cz));

  float normal_x = float(vaby*vbcz - vabz*vbcy);
  float normal_y = float(-vabx*vbcz + vabz*vbcx);
  float normal_z = float(vabx*vbcy - vaby*vbcx);
  float normalLength = float(1.0 / sqrt(normal_x*normal_x + normal_y*normal_y + normal_z*normal_z));
  if ( normalLength == 0 )
    return;
  normal_x *= normalLength;
  normal_y *= normalLength;
  normal_z *= normalLength;
  float normalOffset = float(-(normal_x*ax + normal_y*ay + normal_z*az));

  float vdx = float(normal_y*vbcz - normal_z*vbcy);
  float vdy = float(-normal_x*vbcz + normal_z*vbcx);
  float vdz = float(normal_x*vbcy - normal_y*vbcx);
  float vdLength = float(1.0 / sqrt(vdx*vdx + vdy*vdy + vdz*vdz));
  vdx *= vdLength;
  vdy *= vdLength;
  vdz *= vdLength;
  float vdOffset = float(-(vdx*bx + vdy*by + vdz*bz));
  
  float vex = float(normal_y*vcaz - normal_z*vcay);
  float vey = float(-normal_x*vcaz + normal_z*vcax);
  float vez = float(normal_x*vcay - normal_y*vcax);
  float veLength = float(1.0 / sqrt(vex*vex + vey*vey + vez*vez));
  vex *= veLength;
  vey *= veLength;
  vez *= veLength;
  float veOffset = float(-(vex*cx + vey*cy + vez*cz));
  
  float vfx = float(normal_y*vabz - normal_z*vaby);
  float vfy = float(-normal_x*vabz + normal_z*vabx);
  float vfz = float(normal_x*vaby - normal_y*vabx);
  float vfLength = float(1.0 / sqrt(vfx*vfx + vfy*vfy + vfz*vfz));
  vfx *= vfLength;
  vfy *= vfLength;
  vfz *= vfLength;
  float vfOffset = float(-(vfx*ax + vfy*ay + vfz*az));

  // get bounding box
  float minx = ax, miny = ay, minz = az;
  float maxx = ax, maxy = ay, maxz = az;
  if (bx < minx) minx = bx;
  if (by < miny) miny = by;
  if (bz < minz) minz = bz;
  if (bx > maxx) maxx = bx;
  if (by > maxy) maxy = by;
  if (bz > maxz) maxz = bz;
  if (cx < minx) minx = cx;
  if (cy < miny) miny = cy;
  if (cz < minz) minz = cz;
  if (cx > maxx) maxx = cx;
  if (cy > maxy) maxy = cy;
  if (cz > maxz) maxz = cz;
  minx -= threeHalf; miny -=threeHalf; minz -=threeHalf;
  maxx += threeHalf; maxy +=threeHalf; maxz +=threeHalf;
  int iminx = int(floor(minx)), iminy = int(floor(miny)), iminz = int(floor(minz));
  int imaxx = int(ceil (maxx)), imaxy = int(ceil (maxy)), imaxz = int(ceil (maxz));

  if (iminx < 0) iminx = 0;
  if (iminy < 0) iminy = 0;
  if (iminz < 0) iminz = 0;
  if (imaxx < 0) imaxx = 0;
  if (imaxy < 0) imaxy = 0;
  if (imaxz < 0) imaxz = 0;
  if (iminx > xdim-1) iminx = xdim-1;
  if (iminy > ydim-1) iminy = ydim-1;
  if (iminz > zdim-1) iminz = zdim-1;
  if (imaxx > xdim-1) imaxx = xdim-1;
  if (imaxy > ydim-1) imaxy = ydim-1;
  if (imaxz > zdim-1) imaxz = zdim-1;

  // scan bounding box
  float dNormal = normal_x*iminx + normal_y*iminy + normal_z*iminz + normalOffset;
  float dvd = vdx*iminx + vdy*iminy + vdz*iminz + vdOffset;
  float dve = vex*iminx + vey*iminy + vez*iminz + veOffset;
  float dvf = vfx*iminx + vfy*iminy + vfz*iminz + vfOffset;
  float dvab = vabx*iminx + vaby*iminy + vabz*iminz + vabOffset;
  float dvbc = vbcx*iminx + vbcy*iminy + vbcz*iminz + vbcOffset;
  float dvca = vcax*iminx + vcay*iminy + vcaz*iminz + vcaOffset;
  for (int z=iminz; z<=imaxz; z++)
  {
    float dNormalLastZ = dNormal;
    float dvdLastZ = dvd;
    float dveLastZ = dve;
    float dvfLastZ = dvf;
    float dvabLastZ = dvab;
    float dvbcLastZ = dvbc;
    float dvcaLastZ = dvca;
    for (int y=iminy; y<=imaxy; y++)
    {
      float dNormalLastY = dNormal;
      float dvdLastY = dvd;
      float dveLastY = dve;
      float dvfLastY = dvf;
      float dvabLastY = dvab;
      float dvbcLastY = dvbc;
      float dvcaLastY = dvca;
      volIter.SetPos( Vector3Di( iminx, y, z ) );
      for ( int x=iminx; x<=imaxx; x++, volIter.IncX() )
      {
        double distance = 10;
        if (dNormal < three+width && dNormal > -three-width)
        {
          if (dvd >= 0 && dve >= 0 && dvf >= 0) {
            distance = fabs(dNormal);
          } else if (dvab < 0 && dvca > 1) {
            // a corner
            float dx = ax-x, dy = ay-y, dz = az-z;
            distance = float(sqrt(dx*dx+dy*dy+dz*dz));
          } else if (dvab > 1 && dvbc < 0) {
            // b corner
            float dx = bx-x, dy = by-y, dz = bz-z;
            distance = float(sqrt(dx*dx+dy*dy+dz*dz));
          } else if (dvbc > 1 && dvca < 0) {
            // c corner
            float dx = cx-x, dy = cy-y, dz = cz-z;
            distance = float(sqrt(dx*dx+dy*dy+dz*dz));
          } else if (dvab >= 0 && dvab <= 1 && dvf < 0) {
            // ab line
            distance = sqrt(dNormal * dNormal + dvf * dvf);
          } else if (dvbc >= 0 && dvbc <= 1 && dvd < 0) {
            // bc line
            distance = sqrt(dNormal * dNormal + dvd * dvd);
          } else if (dvca >= 0 && dvca <= 1 && dve < 0) {
            // ca line
            distance = sqrt(dNormal * dNormal + dve * dve);
          }
          if (distance < three+width)
          {
            uint2 value;
            if (distance < width)
            {
              value = fMaximumValue;
            }
            else
            {
              distance -= width;
              value = (uint2) (fMaximumValue * (1.0 - distance / three));
            }
            if ( value > volIter.GetVoxel() )
            {
              volIter.SetVoxel( value );
            }
          }
        }
        dNormal += normal_x;
        dvd += vdx;
        dve += vex;
        dvf += vfx;
        dvab += vabx;
        dvbc += vbcx;
        dvca += vcax;
      }
      dNormal = dNormalLastY + normal_y;
      dvd = dvdLastY + vdy;
      dve = dveLastY + vey;
      dvf = dvfLastY + vfy;
      dvab = dvabLastY + vaby;
      dvbc = dvbcLastY + vbcy;
      dvca = dvcaLastY + vcay;
    }
    dNormal = dNormalLastZ + normal_z;
    dvd = dvdLastZ + vdz;
    dve = dveLastZ + vez;
    dvf = dvfLastZ + vfz;
    dvab = dvabLastZ + vabz;
    dvbc = dvbcLastZ + vbcz;
    dvca = dvcaLastZ + vcaz;
  }
}


// undefines
#undef FILE_REVISION


// $Log: vxVoxelizer.C,v $
// Revision 1.3  2007/03/01 19:04:36  geconomos
// code review preparation
//
// Revision 1.2  2006/05/31 16:16:41  geconomos
// porting to visual studio 2005
//
// Revision 1.1  2006/02/17 15:10:48  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxVoxelizer.C,v 1.3 2007/03/01 19:04:36 geconomos Exp $
// $Id: vxVoxelizer.C,v 1.3 2007/03/01 19:04:36 geconomos Exp $
