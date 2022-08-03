// $Id: BSPTreePolygon.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: BSPTreePolygon.C  
// Description: Generate polygons in Marching Cubes approach. 
//		The generation process is a Binary Space Partition (BSP) procedure. 
// Created: 07/31/2000 
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "BSPTreePolygon.h"

using namespace std;

const int4 MaxPolygonsPerFile = 500000;
const int4 DefaultThreshold = 100;

#define FILE_REVISION "$Revision: 1.2 $"

/// Constructor : responsible for setting up default values
BSPTreePolygon::BSPTreePolygon()
{
  // set up this resolution as a default
  m_iDimX = 512;
  m_iDimY = 512;
  m_iDimZ = 512;

  m_iPlgNoPerFile = MaxPolygonsPerFile;
  m_iThreshold = DefaultThreshold;
  m_pIndata = NULL;
  m_iPlgFileNo = 0;
} // BSPTreePolygon().


/// Destructor.
BSPTreePolygon::~BSPTreePolygon()
{
} // ~BSPTreePolygon().


/**
 *  get the pointer to the polygon buffer
 */
float4 * BSPTreePolygon::GetPtrToPolygonBuffer() const
{
  return (m_pPlgBuffer);
} // GetPolygonBuffer()


/**
 *  generate polygons in Marching Cubes.
 *
 *  @param sizeX,sizeY,sizeZ    the size of the volume
 *  @param srcfilename      source file name
 *  @param desfilename      destination file name
 *  @param dataPtr          pointer to the data set
 */
void BSPTreePolygon::PolygonGen(uint1 *puDataPtr, Triple<uint4> & vSize, Triple<float4> & vUnit, uint4 uSegId)

{
	m_iDimX = vSize.m_x;
	m_iDimY = vSize.m_y;
	m_iDimZ = vSize.m_z;
	m_vUnits = vUnit;
  m_uSegId = uSegId;
	m_iPlgFileNo = 0;

  // allocate memory for buffering pre-calc indexes
  m_pIndata = puDataPtr;
	// make sure the low 4 bits will not be reused.
	for (int4 i=0; i<m_iDimX*m_iDimY*m_iDimZ; i++) 
	{
		*(puDataPtr+i) &= 0xf0;
	}
	
	m_pPlgIndex = new uint1 [ m_iDimX * m_iDimY * m_iDimZ ];
  if ( !m_pPlgIndex )
    throw ex::OutOfMemoryException(LogRec( "BSPTreePolygon", "PolygonGen", "memory allocation error"));

  // pre-calculation: index and polygon number in each cell, the values are bufferred
	int4 iCount = PreMarchingCubes(m_iDimX, m_iDimY, m_iDimZ, uSegId<<4);

  // generate polygons based on a BST-Tree structure
  BSPTreeConstruct(0, 0, 0, m_iDimX, m_iDimY, m_iDimZ, iCount);

  // delete [] m_pIndata;
  delete [] m_pPlgIndex;
} // PolygonGen()


/**
 * control the calculation of indexes in all cells,
 * as well as the number of polygons in each cell
 * these calculated indexes and numbers are bufferred in 'm_pplgIndex' and 'm_pIndata'
 *
 * @param sizeX, sizeY, sizeZ   the size of the volume to be processed
 * @return the number of polygons in this block volume
 */
int4 BSPTreePolygon::PreMarchingCubes(int4 iSizeX, int4 iSizeY, int4 iSizeZ, uint4 uSegId)
{
  int4 iOffset;
  uint1 *puPtr, *pTemp;

	int4 * puIndex = new int4[iSizeX];
	for ( int4 k=0; k<iSizeZ-1; k++ ) 
	{
		for ( int4 j=0; j<iSizeY-1; j++ ) 
		{
			iOffset = k*iSizeX*iSizeY + j*iSizeX;
			puPtr = m_pIndata + iOffset;
			pTemp = m_pPlgIndex + iOffset;

			// calculate indexes of a stick, meanwhile, calculate
			// the number of polygons in each cell along the stick.
			CalcIndex(puPtr, iSizeX, iSizeX*iSizeY, puIndex, uSegId);
			for ( int4 i=0; i<iSizeX; i++ )
				pTemp[i] = (uint1) puIndex[i];
		} // end-for (j)
	} // end-for (k)
	delete [] puIndex;

	return (ColonWall(uSegId));
} // PreMarchingCubes().


/**
 * calculate indexes of a 'stick' of voxels and
 * get to know the number of polygons in each cell along the stick
 *
 * @param data      pointer to the data set
 * @param xdim      size of the volume in x-direction
 * @param XdimYdim  the size of slice Z (= xdim * ydim)
 * @param index     pointer to index buffer
 * @param thresh    threshold used to binarize value at the vertex of cube
 */
void BSPTreePolygon::CalcIndex(uint1 *puData, int4 iDimX, int4 iDimXY, int4 *piIndex, const uint4 uThresh)
{
  register uint1 uValue = (uint1)uThresh;
  register uint1 *puTemp;
  register int4  iCase;

  // calculate the index of the first cell in the stick
  // index setting : | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
  puTemp = puData;
  iCase  =  (uValue == (puTemp[0] & 0xf0));       // z, y, 0 --> bit 0
  iCase |= ((uValue == (puTemp[1] & 0xf0)) << 1); // z, y, 1 --> bit 1

  puTemp += iDimX;
  iCase |= ((uValue == (puTemp[1] & 0xf0)) << 2); // z, y+1, 1 --> bit 3
  iCase |= ((uValue == (puTemp[0] & 0xf0)) << 3); // z, y+1, 0 --> bit 2

  puTemp = puTemp - iDimX + iDimXY;
  iCase |= ((uValue == (puTemp[0] & 0xf0)) << 4); // z+1, y, 0 --> bit 4
  iCase |= ((uValue == (puTemp[1] & 0xf0)) << 5); // z+1, y, 1 --> bit 5

  puTemp += iDimX;
  iCase |= ((uValue == (puTemp[1] & 0xf0)) << 6); // z+1, y+1, 1 --> bit 7
  iCase |= ((uValue == (puTemp[0] & 0xf0)) << 7); // z+1, y+1, 0 --> bit 6

  piIndex[0] = iCase;
  puTemp -= (iDimX + iDimXY);

  // calculate indexes of other voexls along the stick
  // the result from last voxel in the stick is re-used in this cell through bit shift operations
  int4 x0;
  for (x0=1; x0<iDimX-1; x0++ )
	{
    ++puTemp;

    // bit 1 in last voxel --> bit 0 in this voxel
    // bit 2 in last voxel --> bit 3 in this voxel
    // bit 5 in last voxel --> bit 4 in this voxel
    // bit 6 in last voxel --> bit 7 in this voxel
    iCase = ((iCase&0x44)<<1) | ((iCase&0x22)>>1);

		iCase |= ((uValue == (puTemp[1] & 0xf0)) << 1);                 // --> bit 1
		iCase |= ((uValue == (puTemp[iDimX + 1] & 0xf0)) << 2);            // --> bit 2
		iCase |= ((uValue == (puTemp[iDimXY + 1] & 0xf0)) << 5);        // --> bit 5
		iCase |= ((uValue == (puTemp[iDimXY + iDimX + 1] & 0xf0)) << 6);   // --> bit 6

		piIndex[x0] = iCase;
	} // end-for (x0)

  // get to know the number of polygons in each cell in the stick
  // the result is stored in the low 3 bits in the data cell.
	puTemp = puData;
	for ( x0=0; x0<iDimX-1; x0++ )
	{
		puTemp[x0] |= cell_table[piIndex[x0]].npolys;
  } // end-for
} // Calc_index()


/**
 *  count the total number of polygons in this data set
 * (based on the calculation in 'Cal_index()'
 */
int4 BSPTreePolygon::ColonWall(uint4 uSegId)
{
  int4 uCount = 0;
  uint1 *puTemp = m_pIndata;

  for (int4 k=0; k<m_iDimX*m_iDimY*m_iDimZ; k++, puTemp++)
	{
    //  the number is stored in low 3 bits in the data cell
		uCount += (puTemp[0] & 0x07);
  } // end-for

  return (uCount);
} // ColonWall()


/**
 *  recursively bi-divides the data volume, follow a BSP-Tree constuction method
 *  when dividing the volume, the algorithm try to find a position that
 *  if the volume is divided at this position, the number of polygons
 *  in two sub-volumes are almost same.
 *  The dividing stops when the number of polygons in the sub-volume
 *  reaches a pre-set value (valued by 'm_plgNoPerFile').
 *
 *  @param (x0,y0,z0)   corner of the volume block (left-low-front)
 *  @param (x1,y1,z1)   another corner of the volume block (right-upper-back)
 *  @param plgNo            number of the polygons determining if continue a subdivision
 *  @param filename     output file name
 */
void BSPTreePolygon::BSPTreeConstruct(int4 x0, int4 y0, int4 z0, int4 x1, int4 y1, int4 z1, int4 iPlgNo)
{
  int4 iDimension, iCount = 0;
  int4 iSliceX, iSliceY, iSliceZ;
  // determine if the bi-division is needed
  if ( iPlgNo > m_iPlgNoPerFile )
	{
    // determine which direction for bi-division
    int4 iLength = x1 - x0;
    iDimension = 0;  // x-direnction
    if ( (y1-y0) > iLength ) 
		{
      iDimension = 1;  // y-direction
      iLength = y1 - y0;
    } // longest Y
    if ( (z1-z0) > iLength ) 
		{
      iDimension = 2;  // z-direction
    } // longest Z

    // Recursively bi-divide the block.
    switch ( iDimension ) 
		{
    case 0: // divied along x-direction.
      iSliceX = x0;

      // find the position to divided the block.
      do 
      {
        iCount += XSlicePlgNo(iSliceX, y0, z0, y1, z1);
        iSliceX ++;
      } 
      while ( iCount<(iPlgNo/2) ); // almost the half of the total number.

      // continue such construction in the first sub-volume.
      BSPTreeConstruct(x0, y0, z0, iSliceX, y1, z1, iCount);

      // continue such construction in the second sub-volume.
      BSPTreeConstruct(iSliceX, y0, z0, x1, y1, z1, iPlgNo-iCount);

      break;

    case 1: // divied along y-direction.
      iSliceY = y0;

      // find the position for division.
      do 
      {
        iCount += YSlicePlgNo(iSliceY, x0, z0, x1, z1);
        iSliceY ++;
      } 
      while ( iCount<(iPlgNo/2) ); // almost the half of the total.

      // continue such construction in the first sub-volume.
      BSPTreeConstruct(x0, y0, z0, x1, iSliceY, z1, iCount);

      // continue such construction in the second sub-volume.
      BSPTreeConstruct(x0, iSliceY, z0, x1, y1, z1, iPlgNo-iCount);
      break;

    case 2: // divied along z-direction.
      iSliceZ = z0;

      // find the position for division.
      do 
      {
        iCount += ZSlicePlgNo(iSliceZ,x0,y0,x1,y1);
        iSliceZ ++;
      } 
      while ( iCount<(iPlgNo/2) ); // almost the half of the total.

      // continue such construction in the first sub-volume.
      BSPTreeConstruct(x0, y0, z0, x1, y1, iSliceZ, iCount);

      // continue such construction in the second sub-volume.
      BSPTreeConstruct(x0, y0, iSliceZ, x1, y1, z1, iPlgNo-iCount);
      break;

    default:
      break;
    } // end-switch(dimension)
  } // end-if
  // the division is end. now generate polygons in the block.
  else 
	{
    // record this block
    m_iPlgFileNo ++;

    // generating ... ;
    BSPPolygonGenerate(x0,y0,z0,x1,y1,z1);
  } // end-else
} // BSPTreeConstruct()


/**
 *  calculate the number of polygons in the boundary area ((y0,z0),(y1,z1)) in slice x0
 *
 *  @param x0       slice number
 *  @param (y0,z0)  left-low corner of the boundary area
 *  @param (y1,z1)  right-upper corner of the boundary area
 *  @return the number of polygons in this slice
 */
int4 BSPTreePolygon::XSlicePlgNo(int4 x0,int4 y0,int4 z0,int4 y1,int4 z1)
{
  int4 iValue = 0;
  uint1 * puPtr;

  for ( int4 k=z0; k<z1; k++ ) 
	{
    puPtr = m_pIndata + k*m_iDimX*m_iDimY + y0*m_iDimX + x0;
    for ( int4 j=y0; j<y1; j++ ) 
		{
      iValue += ((*puPtr) & 0x07);
      puPtr += m_iDimX;
    } // end-for (j)
  } // end for(k)
  return (iValue);
} // XSlicePlgNo()


/**
 *  calculate the number of polygons in the boundary area ((x0,z0),(x1,z1)) in slice y0
 *
 *  @param y0       slice number
 *  @param (x0,z0)  left-low corner of the boundary area
 *  @param (x1,z1)  right-upper corner of the boundary area
 *  @return the number of polygons in this slice
 */
int4 BSPTreePolygon::YSlicePlgNo(int4 y0,int4 x0,int4 z0,int4 x1,int4 z1)
{
  int4 iValue = 0;
  uint1 *puPtr;

  for ( int4 k=z0; k<z1; k++ ) 
	{
    puPtr = m_pIndata + k*m_iDimX*m_iDimY + y0*m_iDimX + x0;
    for ( int4 i=x0; i<x1; i++ ) 
		{
      iValue += ((*puPtr) & 0x07);
      puPtr ++;
    } // end-for (i)
  } // end-for (k)
  return (iValue);
} // YSlicePlgNo()


/**
 *  calculate the number of polygons in the boundary area ((x0,y0),(x1,y1)) in slice z0
 *
 *  @param z0       slice number
 *  @param (x0,y0)  left-low corner of the boundary area
 *  @param (x1,y1)  right-upper corner of the boundary area
 *  @return the number of polygons in this slice
 */
int4 BSPTreePolygon::ZSlicePlgNo(int4 z0,int4 x0,int4 y0,int4 x1,int4 y1)
{
  int4 iValue = 0;
  uint1 *puPtr, *puPtr1;

  puPtr = m_pIndata + z0*m_iDimX*m_iDimY;
  for ( int4 j=y0; j<y1; j++ ) 
	{
    puPtr1 = puPtr + j*m_iDimX + x0;
    for ( int4 i=x0; i<x1; i++ ) 
		{
      iValue += ((*puPtr1) & 0x07);
      puPtr1 ++;
    } // end-for (i)
  } // end-for (j)
  return (iValue);
} // ZSlicePlgNo()


/**
 *  the following 3 functions calculate the accumulated gray value around a 3x3 area.
 *  the center of this area is pointed by 'Ptr'. The contribution of points in the area
 *  are weighted according to their position, which is shown below:
 *      1 - 2 - 1
 *      2 - 4 - 2
 *      1 - 2 - 1
 *  the return value is used to calculate the normal of vertex of the cell
 *  such weighted gray value can make the surface smoother.
 *
 *  a 3x3 area which is perpendicular to X-axle
 *
 *  @param Ptr  pointer to the data set
 *  @return     weighted value in this 3x3 area
 */
float4 BSPTreePolygon::ValueX(uint1 *puPtr)
{
  // center : weight = 4
  float4 fValue = (float4)(*puPtr*4);

  // edge : weight = 2
  fValue += (float4)(*(puPtr + m_iDimX*m_iDimY) * 2);
  fValue += (float4)(*(puPtr - m_iDimX*m_iDimY) * 2);
  fValue += (float4)(*(puPtr + m_iDimX) * 2);
  fValue += (float4)(*(puPtr - m_iDimX) * 2);

  // corner : weight = 1
  fValue += (float4)(*(puPtr + m_iDimX*m_iDimY + m_iDimX));
  fValue += (float4)(*(puPtr + m_iDimX*m_iDimY - m_iDimX));
  fValue += (float4)(*(puPtr - m_iDimX*m_iDimY + m_iDimX));
  fValue += (float4)(*(puPtr - m_iDimX*m_iDimY - m_iDimX));

  return (fValue);
} // ValueX()


/**
 *  a 3x3 area which is perpendicular to Y-axle
 *
 *  @param Ptr  pointer to the data set
 *  @return     weighted value in this 3x3 area
 */
float4 BSPTreePolygon::ValueY(uint1 *puPtr)
{
  // center : weight = 4
  float4 fValue = (float4)(*puPtr*4);

  // edge : weight = 2
  fValue += (float4)(*(puPtr+1) * 2);
  fValue += (float4)(*(puPtr-1) * 2);
  fValue += (float4)(*(puPtr + m_iDimX*m_iDimY) * 2);
  fValue += (float4)(*(puPtr - m_iDimX*m_iDimY) * 2);

  // corner : weight = 1
  fValue += (float4)(*(puPtr + m_iDimX*m_iDimY + 1));
  fValue += (float4)(*(puPtr + m_iDimX*m_iDimY - 1));
  fValue += (float4)(*(puPtr - m_iDimX*m_iDimY + 1));
  fValue += (float4)(*(puPtr - m_iDimX*m_iDimY - 1));

  return (fValue);
} // ValueY()


/**
 *  a 3x3 area which is perpendicular to Z-axle
 *
 *  @param Ptr  pointer to the data set
 *  @return     weighted value in this 3x3 area
 */
float4 BSPTreePolygon::ValueZ(uint1 *puPtr)
{
  // center : weight = 4
  float4 fValue = (float4)(*puPtr*4);

  // edge : weight = 2
  fValue += (float4)(*(puPtr+1) * 2);
  fValue += (float4)(*(puPtr-1) * 2);
  fValue += (float4)(*(puPtr + m_iDimX) * 2);
  fValue += (float4)(*(puPtr - m_iDimX) * 2);

  // corner : weight = 1
  fValue += (float4)(*(puPtr + m_iDimX + 1));
  fValue += (float4)(*(puPtr - m_iDimX + 1));
  fValue += (float4)(*(puPtr + m_iDimX - 1));
  fValue += (float4)(*(puPtr - m_iDimX - 1));

  return (fValue);
} // ValueZ()


/**
 *  Generate polygons in block [(x0,y0,z1), (x1,y1,z1)]
 *
 *  @param (x0,y0,z0)   one corner of the boundary box
 *  @param (x1,y1,z1)   another corner of the boundary box
 *  @param filename     prefix of the file
 */
void BSPTreePolygon::BSPPolygonGenerate(int4 x0,int4 y0,int4 z0,int4 x1,int4 y1,int4 z1)
{
  int4 i,j,k,m,iEdgeNum,iPlgNum, iCurrEdge;
  static int4  iFileNo = 0;
  int4 iValue, iCount=0;
  uint4 uIndex;
  uint1 *puPtr;
  float4   *pPlgPtr, *pfPolysBuf;
  float4   cx,cy,cz,nx,ny,nz,fNormal[8][3];
  float4   nn, fEdge[13][6];    // For convinent, first element is wasted 

  int4 iMinX = x1;  int4 iMaxX = x0;
  int4 iMinY = y1;  int4 iMaxY = y0;
  int4 iMinZ = z1;  int4 iMaxZ = z0;

  // find the boundary box of inside cells in this block
  for ( k=z0; k<z1; k++ )
    for ( j=y0; j<y1; j++ ) 
		{
      puPtr = m_pIndata + k*m_iDimX*m_iDimY + j*m_iDimX + x0;
      for ( i=x0; i<x1; i++ ) 
			{
        if ( iValue = (*puPtr & 0x07) )
				{
					if ( i>iMaxX )  iMaxX = i;
					if ( i<iMinX )  iMinX = i;
					if ( j>iMaxY )  iMaxY = j;
					if ( j<iMinY )  iMinY = j;
					if ( k>iMaxZ )  iMaxZ = k;
					if ( k<iMinZ )  iMinZ = k;
        } // end-if
        ++puPtr;
      } // endfor (i)
    } // endfor (j)

  // To be adjusted here
  if ( iMinX==0 ) iMinX = 1;  if ( iMaxX==(m_iDimX-2) ) iMaxX = m_iDimX-3;
  if ( iMinY==0 ) iMinY = 1;  if ( iMaxY==(m_iDimY-2) ) iMaxY = m_iDimY-3;
  if ( iMinZ==0 ) iMinZ = 1;  if ( iMaxZ==(m_iDimZ-2) ) iMaxZ = m_iDimZ-3;

  iFileNo++;
  pPlgPtr = pfPolysBuf = new float4 [m_iPlgNoPerFile*18];
  for ( k=iMinZ; k<=iMaxZ; k++ ) 
	{
    for ( j=iMinY; j<=iMaxY; j++ )
      for ( i=iMinX; i<=iMaxX; i++ )
			{
        puPtr = m_pIndata + k*m_iDimX*m_iDimY + j*m_iDimX + i;
        if ( (*puPtr) & 0x07 ) 
				{ // make sure there are polygons in this cell.

					// vertex (x0,y0,z0)
					uIndex = (uint4)(*(m_pPlgIndex + k*m_iDimX*m_iDimY + j*m_iDimX + i));
					fNormal[0][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
					fNormal[0][1] = ( ValueY(puPtr+m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
					fNormal[0][2] = ( ValueZ(puPtr+m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          // vertex (x0+1,y0,z0)
          puPtr += 1;
          fNormal[1][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
          fNormal[1][1] = ( ValueY(puPtr+m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
          fNormal[1][2] = ( ValueZ(puPtr+m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          // vertex (x0+1,y0+1,z0)
          puPtr += m_iDimX;
          fNormal[2][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
          fNormal[2][1] = ( ValueY(puPtr+m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
          fNormal[2][2] = ( ValueZ(puPtr+m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          // vertex (x0,y0+1,z0)
          puPtr -= 1;
          fNormal[3][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
          fNormal[3][1] = ( ValueY(puPtr + m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
          fNormal[3][2] = ( ValueZ(puPtr + m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          // vertex (x0,y0,z0+1)
          puPtr = puPtr - m_iDimX + m_iDimX*m_iDimY;
          fNormal[4][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
          fNormal[4][1] = ( ValueY(puPtr+m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
          fNormal[4][2] = ( ValueZ(puPtr+m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          // vertex (x0+1,y0,z0+1)
          puPtr += 1;
          fNormal[5][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
          fNormal[5][1] = ( ValueY(puPtr+m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
          fNormal[5][2] = ( ValueZ(puPtr+m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          // vertex (x0+1,y0+1,z0+1)
          puPtr += m_iDimX;
          fNormal[6][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
          fNormal[6][1] = ( ValueY(puPtr+m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
          fNormal[6][2] = ( ValueZ(puPtr+m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          // vertex (x0,y0+1,z0+1)
					puPtr -= 1;
					fNormal[7][0] = ( ValueX(puPtr+1) - ValueX(puPtr-1) ) / m_vUnits.m_x;
					fNormal[7][1] = ( ValueY(puPtr+m_iDimX) - ValueY(puPtr-m_iDimX) ) / m_vUnits.m_y;
					fNormal[7][2] = ( ValueZ(puPtr+m_iDimX*m_iDimY) - ValueZ(puPtr-m_iDimX*m_iDimY) ) / m_vUnits.m_z;

          iEdgeNum = cell_table[uIndex].nedges;
          // for each edge, procesing...
          for ( m=0; m<iEdgeNum; m++ ) 
					{
            iCurrEdge = cell_table[uIndex].edges[m];
            switch ( iCurrEdge ) 
						{
            case 1: // edge 1 : (x0,y0,z0) -- (x0+1,y0,z0)
              cx = (float4)(i+0.5);
              cy = (float4)j;
              cz = (float4)k;
              nx = (fNormal[0][0] + fNormal[1][0])/2;
              ny = (fNormal[0][1] + fNormal[1][1])/2;
              nz = (fNormal[0][2] + fNormal[1][2])/2;
              break;
            case 2: // edge 2 : (x0+1,y0,z0) -- (x0+1,y0+1,z0)
              cx = (float4)(i+1.0);
              cy = (float4)(j+0.5);
              cz = (float4)k;
              nx = (fNormal[1][0] + fNormal[2][0])/2;
              ny = (fNormal[1][1] + fNormal[2][1])/2;
              nz = (fNormal[1][2] + fNormal[2][2])/2;
              break;
            case 3: // edge 3 : (x0,y0+1,z0) -- (x0+1,y0+1,z0)
              cx = (float4)(i+0.5);
              cy = (float4)(j+1.0);
              cz = (float4)k;
              nx = (fNormal[2][0] + fNormal[3][0])/2;
              ny = (fNormal[2][1] + fNormal[3][1])/2;
              nz = (fNormal[2][2] + fNormal[3][2])/2;
              break;
            case 4: // edge 4 : (x0,y0,z0) -- (x0,y0+1,z0)
              cx = (float4)i;
              cy = (float4)(j+0.5);
              cz = (float4)k;
              nx = (fNormal[3][0] + fNormal[0][0])/2;
              ny = (fNormal[3][1] + fNormal[0][1])/2;
              nz = (fNormal[3][2] + fNormal[0][2])/2;
              break;
            case 5: // edge 5 : (x0,y0,z0+1) -- (x0+1,y0,z0+1)
              cx = (float4)(i+0.5);
              cy = (float4)j;
              cz = (float4)(k+1);
              nx = (fNormal[4][0] + fNormal[5][0])/2;
              ny = (fNormal[4][1] + fNormal[5][1])/2;
              nz = (fNormal[4][2] + fNormal[5][2])/2;
              break;
            case 6: // edge 6 : (x0+1,y0,z0+1) -- (x0+1,y0+1,z0+1)
              cx = (float4)(i+1.0);
              cy = (float4)(j+0.5);
              cz = (float4)(k+1.0);
              nx = (fNormal[5][0] + fNormal[6][0])/2;
              ny = (fNormal[5][1] + fNormal[6][1])/2;
              nz = (fNormal[5][2] + fNormal[6][2])/2;
              break;
            case 7: // edge 7 : (x0,y0+1,z0+1) -- (x0+1,y0+1,z0+1)
              cx = (float4)(i+0.5);
              cy = (float4)(j+1.0);
              cz = (float4)(k+1.0);
              nx = (fNormal[6][0] + fNormal[7][0])/2;
              ny = (fNormal[6][1] + fNormal[7][1])/2;
              nz = (fNormal[6][2] + fNormal[7][2])/2;
              break;
            case 8: // edge 8 : (x0,y0,z0+1) -- (x0,y0+1,z0+1)
              cx = (float4)i;
              cy = (float4)(j+0.5);
              cz = (float4)(k+1.0);
              nx = (fNormal[4][0] + fNormal[7][0])/2;
              ny = (fNormal[4][1] + fNormal[7][1])/2;
              nz = (fNormal[4][2] + fNormal[7][2])/2;
              break;
            case 9: // edge 9 : (x0,y0,z0) -- (x0,y0,z0+1)
              cx = (float4)i;
              cy = (float4)j;
              cz = (float4)(k+0.5);
              nx = (fNormal[4][0] + fNormal[0][0])/2;
              ny = (fNormal[4][1] + fNormal[0][1])/2;
              nz = (fNormal[4][2] + fNormal[0][2])/2;
              break;
            case 10: // edge 10 : (x0+1,y0,z0) -- (x0+1,y0,z0+1)
              cx = (float4)(i+1.0);
              cy = (float4)j;
              cz = (float4)(k+0.5);
              nx = (fNormal[1][0] + fNormal[5][0])/2;
              ny = (fNormal[1][1] + fNormal[5][1])/2;
              nz = (fNormal[1][2] + fNormal[5][2])/2;
              break;
            case 11: // edge 11 : (x0,y0+1,z0) -- (x0,y0+1,z0+1)
              cx = (float4)i;
              cy = (float4)(j+1.0);
              cz = (float4)(k+0.5);
              nx = (fNormal[3][0] + fNormal[7][0])/2;
              ny = (fNormal[3][1] + fNormal[7][1])/2;
              nz = (fNormal[3][2] + fNormal[7][2])/2;
              break;
            case 12: // edge 12 : (x0+1,y0+1,z0) -- (x0+1,y0+1,z0+1)
              cx = (float4)(i+1.0);
              cy = (float4)(j+1.0);
              cz = (float4)(k+0.5);
              nx = (fNormal[2][0] + fNormal[6][0])/2;
              ny = (fNormal[2][1] + fNormal[6][1])/2;
              nz = (fNormal[2][2] + fNormal[6][2])/2;
              break;
            default:
              break;
            } // end-switch(curr_edge)

            // buffer results (coordinates of intection point)
            fEdge[iCurrEdge][0] = cx;
            fEdge[iCurrEdge][1] = cy;
            fEdge[iCurrEdge][2] = cz;

            // normalize
						nn = nx*nx + ny*ny + nz*nz;
						if ( nn>1e-6 ) nn = (float4)sqrt(nn);
						else nn = 1.0;

						// buffer results (normal of interection point)
						fEdge[iCurrEdge][3] = -nx/nn;
						fEdge[iCurrEdge][4] = -ny/nn;
						fEdge[iCurrEdge][5] = -nz/nn;

					} // end-for (m)
					iPlgNum = cell_table[uIndex].npolys;

					// construct polygons
					if ( iPlgNum>0 ) 
					{
            iCount += iPlgNum;
            for ( m=0; m<iPlgNum; m++ )
						{
              // first vertex
              iCurrEdge = cell_table[uIndex].polys[m*3];
              *pPlgPtr = fEdge[iCurrEdge][0];    pPlgPtr++;   // cx 
              *pPlgPtr = fEdge[iCurrEdge][1];    pPlgPtr++;   // cy 
              *pPlgPtr = fEdge[iCurrEdge][2];    pPlgPtr++;   // cz 
              *pPlgPtr = fEdge[iCurrEdge][3];    pPlgPtr++;   // nx 
              *pPlgPtr = fEdge[iCurrEdge][4];    pPlgPtr++;   // ny 
              *pPlgPtr = fEdge[iCurrEdge][5];    pPlgPtr++;   // nz 

              // second vertex
              iCurrEdge = cell_table[uIndex].polys[m*3+1];
              *pPlgPtr = fEdge[iCurrEdge][0];    pPlgPtr++;   // cx 
              *pPlgPtr = fEdge[iCurrEdge][1];    pPlgPtr++;   // cy 
              *pPlgPtr = fEdge[iCurrEdge][2];    pPlgPtr++;   // cz 
              *pPlgPtr = fEdge[iCurrEdge][3];    pPlgPtr++;   // nx 
              *pPlgPtr = fEdge[iCurrEdge][4];    pPlgPtr++;   // ny 
              *pPlgPtr = fEdge[iCurrEdge][5];    pPlgPtr++;   // nz 

              // third vertex
              iCurrEdge = cell_table[uIndex].polys[m*3+2];
              *pPlgPtr = fEdge[iCurrEdge][0];    pPlgPtr++;   // cx 
              *pPlgPtr = fEdge[iCurrEdge][1];    pPlgPtr++;   // cy 
              *pPlgPtr = fEdge[iCurrEdge][2];    pPlgPtr++;   // cz 
              *pPlgPtr = fEdge[iCurrEdge][3];    pPlgPtr++;   // nx 
              *pPlgPtr = fEdge[iCurrEdge][4];    pPlgPtr++;   // ny 
              *pPlgPtr = fEdge[iCurrEdge][5];    pPlgPtr++;   // nz 
            } // end "for"
          } // end-if (nplg)
        } // end-if (*Ptr)
      } // end-for (i)
  } // end-for (k)
  // record the number of polygons generated and set a pointer to the polygon buffer.
  m_iPlgNoGenerated = iCount;
  m_pPlgBuffer = pfPolysBuf;
} // BSPPolygonGenerate()


// $Log: BSPTreePolygon.C,v $
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:04  geconomos
// initial revision
//
// Revision 3.1.20.1  2004/08/27 13:15:39  dongqing
// Fix a typo in PolygonGen()
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:01:49  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:06   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:38:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:28   ingmar
// Initial revision.
// Revision 1.33  2001/08/29 16:30:23  sarang
// BUG_FIX: fixed scope of loop variable
// gcc/linux ok now
//
// Revision 1.32  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.31  2001/07/25 16:16:42  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.30  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.29  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.28  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.27  2001/07/03 14:59:47  soeren
// removed stdafx.c and .h
//
// Revision 1.26  2001/05/30 15:43:31  binli
// code conventions
//
// Revision 1.25  2001/05/21 00:22:53  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.24  2001/04/09 17:31:27  dmitry
// updated datatypes
//
// Revision 1.23  2001/04/08 21:38:58  dave
// updated doc headers
//
// Revision 1.22  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.21  2001/03/27 15:25:30  binli
// 'const' functions for safety.
//
// Revision 1.20  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.19  2001/03/21 20:15:03  binli
// code cleaned.
//
// Revision 1.18  2001/03/21 19:01:48  binli
// moved 'cell_table' to class BSPTreePolygon
//
// Revision 1.17  2001/03/12 16:49:03  binli
// changed back to '15' segments limitation for safety.
//
// Revision 1.16  2001/03/12 15:48:14  binli
// can process up to 31 segments now.
//
// Revision 1.15  2001/01/02 17:59:51  binli
// fixed bug of 'zero' triangle in a segment
//
// Revision 1.14  2000/12/04 18:42:11  binli
// write out triangles in World coordinates.
//
// Revision 1.13  2000/11/06 23:57:16  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.12  2000/11/05 00:05:52  dave
// fixed one segfault, need to remove or re-write code entirely
//
// Revision 1.11  2000/10/31 21:09:49  binli
// changed the order of the pipeline (for the purpose of proper skeleton rendering)
//
// Revision 1.10  2000/10/21 19:10:06  ingmar
// code beautify
//
// Revision 1.9  2000/10/21 06:29:12  ingmar
// code cleanup from v1.7
//
// Revision 1.8  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.7  2000/10/21 05:13:40  ingmar
// bad merge, but has all comments
//
// Revision 1.6  2000/10/18 02:59:50  antonio
// fixed minor problems in skeleton stage
//
// Revision 1.5  2000/10/16 08:29:19  antonio
// resolved most warnings
//
// Revision 1.4.10.3  2000/10/20 15:29:56  binli
// fixed bug of 'hole' in surface.
//
// Revision 1.4.10.2  2000/10/19 20:37:01  binli
// code optimization
//
// Revision 1.4.10.1  2000/10/18 17:38:42  binli
// made changes for new 'down-sampling shrinking'
//
// Revision 1.4  2000/09/20 15:02:03  binli
// Viatronix C++ coding walk through -- 2nd
//
// Revision 1.3  2000/09/20 14:42:44  binli
// Viatronix C++ coding walk through (moved some inline functions to .inl file)
//
// Revision 1.2  2000/09/14 18:15:13  binli
// fitted to Viatronix C++ code convention and standard
//
// Revision 1.1.1.1  2000/08/11 22:15:53  ingmar
// unit tests complete
//
//
// 11    9/06/00 10:17a Binli
// fitted to viatronix standard
//
// 10    9/05/00 4:21p Binli
// change the sub-pipeline of polygon generation (3) : gap the 'data
// format transfer' submodule and 'getting triangle strips' submodule, so
// no temporary file(s) '.obj' is (are) generated.
//
// 9     9/01/00 12:49p Binli
// change the sub-pipeline of polygon generation (2) : gap the 'polygon
// generation'
// sub-module and 'data format transfer' sub-module, so no temporary
// file(s)
// '.plg' is(are) generated.
//
// 8     8/31/00 4:42p Binli
// change the sub-pipeline of polygon generation (1) : gap the 'shrink'
// sub-module and 'polygon generation' sub-module, so no temporary file(s)
// '.shr' is(are) generated.
//
// 7     8/29/00 1:55p Antonio
// updated footer and header
//
// *****************  Version 6  *****************
// User: Binli           Date:  8/22/00  Time:  1:24p
// Checked in $/v2k/src/Triangles
// Comment:
//   add comments
//
// *****************  Version 5  *****************
// User: Antonio         Date:  8/21/00  Time:  5:45p
// Checked in $/v2k/src/Triangles
// Comment:
//   added header
//
// *****************  Version 4  *****************
// User: Sarang          Date:  8/20/00  Time:  6:49p
// Checked in $/v2k/src/Triangles
// Comment:
//   The whole pipeline now works under linux+gcc. Removed tons of bugs
// and syntax errors found by gcc.
//
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
//
// *****************  Version 3  *****************
// User: Binli           Date:  8/09/00  Time:  2:13p
// Checked in $/v2k/src/Triangles
//
// *****************  Version 2  *****************
// User: Binli           Date:  7/31/00  Time:  9:55p
// Checked in $/v2k/src/Triangles
// Comment:
//   Muiti-Seg preprocessing
//
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  4:34p
// Created BSPTreePolygon.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/BSPTreePolygon.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: BSPTreePolygon.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
