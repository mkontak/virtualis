// $Id: CrossPlane.C,v 1.2 2007/10/03 16:14:01 cqian Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Wenli Cai (wenli@viatronix.com)


// includes
#include "StdAfx.h"
#include <stack>
#include "Global.h"
#include "Plane.h"
#include "Timer.h"
#include "File.h"
#include "vxBlockVolumeRayIterator.h"
#include "CrossPlane.h"
#include "ValueRange.h"
#include "vxBlockVolumeIterator.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 *	constructor -- initialize the iSize of the crossplane
 *
 *  @param iSize : the iSize of crossplane is iSize^2
 *  @return : N/A
 */
CrossPlane::CrossPlane( const int4 iSize ) :
m_crossBuf( 0.0F ), 
m_crossBufMask( false ),
m_fScale( 1.0F ),
m_bValidMask( true ),
m_iBufXlenHalf( iSize/2 ),
m_iBufYlenHalf( iSize/2 ), 
m_pColorTbl( NULL ),
m_fArea( 0.0F )
{
  m_bufDim = Triple<uint4>( m_iBufXlenHalf*2+1, m_iBufYlenHalf*2+1, 0 ); //2n+1  
  m_crossBuf.Resize( m_bufDim );
  m_crossBufMask.Resize( m_bufDim );
  m_iMeanXCenter = m_iBufXlenHalf;
  m_iMeanYCenter = m_iBufYlenHalf;

} // constructor


/**
 *	constructor -- initialize the iSize and position of the crossplane
 *
 *  @param centerPnt : the location of the crossplane's centerPnt
 *  @param vNormal : the vNormal of the crossplane
 *  @param vUp : the upvector of the crossplane
 *  @param iSize : the iSize of the crossplane is iSize^2
 *  @return : N/A
 */
CrossPlane::CrossPlane( const Point<float4> & centerPnt, const Vector<float4> & vNormal, const Vector<float4> & vUp, const int4 iSize ) :
m_crossBuf( 0.0F ), 
m_crossBufMask( false ),
m_fScale( 1.0F ),
m_fArea( 0.0F ),
m_bValidMask( true ),
m_iBufXlenHalf( iSize/2 ),
m_iBufYlenHalf( iSize/2 ), 
m_centerPnt( centerPnt ),
m_vNormal( vNormal ),
m_vUp( vUp )
{
  // initialize the crossplane iSize
  m_bufDim = Triple<uint4>( m_iBufXlenHalf*2+1, m_iBufYlenHalf*2+1, 0 ); //2n+1
  m_crossBuf.Resize( m_bufDim );
  m_crossBufMask.Resize( m_bufDim );
  m_iMeanXCenter = m_iBufXlenHalf;
  m_iMeanYCenter = m_iBufYlenHalf;
  
  // initialize the position and orientation of the crossplane
  m_vDirV = m_vUp;
  m_vDirU = m_vUp.Cross( m_vNormal );
  m_vDirU.Normalize();

  //inverse = reverse here. So I just reverse it.
  Matrix<float4> matRot( m_vDirU.m_x, m_vDirV.m_x, m_vNormal.m_x, 0.0f, 
                         m_vDirU.m_y, m_vDirV.m_y, m_vNormal.m_y, 0.0f,
                         m_vDirU.m_z, m_vDirV.m_z, m_vNormal.m_z, 0.0f,
                         0.0f,       0.0f,       0.0f,         1.0f );
  
  Matrix<float4> matTran;
  matTran.Translate( Vector3Df(m_centerPnt) );  

  m_matIM2Obj = matTran*matRot;

} // constructor


/**
 *	Deconstructor
 */
CrossPlane::~CrossPlane()
{
}


/**
 *	move the crossplane to the (centerPnt) position and set the orientation (vNormal, vUp)
 *
 *  @param centerPnt : the location of the crossplane's centerPnt
 *  @param vNormal : the vNormal of the crossplane
 *  @param vUp : the upvector of the crossplane
 *  @return : N/A
 */
void CrossPlane::MoveCrossPlane( const Point<float4> & centerPnt, const Vector<float4> & vNormal, const Vector<float4> & vUp )
{
  m_centerPnt = centerPnt;
  m_vNormal = vNormal;
  m_vUp = vUp;

  m_vDirV = m_vUp;
  m_vDirU = m_vUp.Cross( m_vNormal );
  m_vDirU.Normalize();

  //inverse = reverse here. So I just reverse it.
  Matrix<float4> matRot(  m_vDirU.m_x, m_vDirV.m_x, m_vNormal.m_x, 0.0f, 
                          m_vDirU.m_y, m_vDirV.m_y, m_vNormal.m_y, 0.0f,
                          m_vDirU.m_z, m_vDirV.m_z, m_vNormal.m_z, 0.0f,
                          0.0f,        0.0f,        0.0f,          1.0f  );

  Matrix<float4> matTran;
  matTran.Translate( Vector3Df(m_centerPnt) );  

  m_matIM2Obj = matTran*matRot;

} // MoveCrossPlane


/**
 *	fill the crossplane by scanning it in the sampleing volume (samVol)
 *  @param samVol : the volume sampled
 *  @return : N/A
 */
void CrossPlane::FillCrossPlane( vxBlockVolume<uint2> & samVol )
{ 
  const Triplef units( samVol.GetHeader().GetVoxelUnits() );
  const Matrix<float4> mIM2Vol( GetIM2VolTransformMatrix(units) );

  m_crossBuf.Clear();
  float4 * pBuf( (float4*)(m_crossBuf.GetDataPtr()) );

  const int4 iXlenHlaf( m_iBufXlenHalf );
  const int4 iYlenHalf( m_iBufYlenHalf );
  const int4 iXlen( 2*iXlenHlaf+1 );
  
  for ( int4 v(-iYlenHalf); v<iYlenHalf+1; ++v )
  {
    Point<float4> p0( -iXlenHlaf, v, 0.0f );   //index ( uint ) 
    Point<float4> p1(  iXlenHlaf, v, 0.0f );

    Point<float4> bgnPnt( mIM2Vol*p0 );
    Point<float4> endPnt( mIM2Vol*p1 );

    RaySegment raySegment( Vector4Df(bgnPnt, 0), Vector4Df(endPnt, 1) );
    Vector4Df rayIncrement( raySegment.GetDirection() / (2.0F*iXlenHlaf) );
    vxBlockVolumeRayIterator<uint2> samLineIter( samVol, raySegment, rayIncrement );

    for ( int4 u(0); u<iXlen; ++u, ++samLineIter, ++pBuf )
    {
      *pBuf = samLineIter.GetSample();
    }
  } // for

} // FillCrossPlane



/**
 *	fill the crossplane by scanning a component volume
 *
 *  @param labelVol : the segmentation component volume
 *  @return : N/A
 */
void CrossPlane::FillCrossPlane( vxBlockVolume<bool> & componentVol )
{
  Triplef units( componentVol.GetHeader().GetVoxelUnits() ); 
  const Matrix<float4> mIM2Vol( GetIM2VolTransformMatrix(units) );

  m_crossBuf.Clear();
  float4 * pBuf( (float4*)(m_crossBuf.GetDataPtr()) );

  const int4 iXlenHlaf( m_iBufXlenHalf );
  const int4 iYlenHalf( m_iBufYlenHalf );
  const int4 iXlen( 2*iXlenHlaf+1 );
  const float4 fLabel2BoundaryScale( 400.0F );

  for ( int4 v(-iYlenHalf); v<iYlenHalf+1; ++v )
  {
    Point<float4> p0( -iXlenHlaf, v, 0.0f );
    Point<float4> p1(  iXlenHlaf, v, 0.0f );

    Point<float4> bgnPnt( mIM2Vol*p0 );
    Point<float4> endPnt( mIM2Vol*p1 );

    RaySegment raySegment( Vector4Df(bgnPnt, 0), Vector4Df(endPnt, 1) );
    Vector4Df rayIncrement( raySegment.GetDirection() / (2.0F*iXlenHlaf) );
    vxBlockVolumeRayIterator<bool> samLineIter( componentVol, raySegment, rayIncrement );

    for ( int4 u(0); u<iXlen; ++u, ++samLineIter, ++pBuf )
    {
      int2 labelCount( 0 );

      vxBlockVolumeIterator<bool> compVolNBIter( componentVol, samLineIter.GetPosInt() );

      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); compVolNBIter.IncX();
      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); compVolNBIter.IncY();
      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); compVolNBIter.DecX();
      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); compVolNBIter.DecY();

      compVolNBIter.IncZ();
      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); compVolNBIter.IncX();
      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); compVolNBIter.IncY();
      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); compVolNBIter.DecX();
      labelCount += ( compVolNBIter.GetVoxel() ? 1 : 0 ); 

      float4 fpixel( labelCount/8.0f );  //background = 200
      fpixel = fpixel*fpixel;
      fpixel *= fLabel2BoundaryScale;

      * pBuf = fpixel;
    } 
  }  // for
} // FillCrossPlane



/**
 *	2D linear resampling at a point on crossplane
 * 
 *  @param buf : the 2D buffer of crossplane
 *  @param pnt : the resampling point
 *  @return : the value resampled
 */
float4 CrossPlane::GetResampledValueRaw( Image<float4> & buf, Point<float4> & pnt ) const
{
  int4 x0( (int4)(pnt.m_x) );
  int4 y0( (int4)(pnt.m_y) );

  float4 dx( pnt.m_x - x0 );
  float4 dy( pnt.m_y - y0 );

  Point2D<uint2> pxy(x0, y0);
  float4 f00( buf.GetPixelRaw(pxy) );
  pxy.m_x++;
  float4 f10( buf.GetPixelRaw(pxy) );
  pxy.m_y++;
  float4 f11( buf.GetPixelRaw(pxy) );
  pxy.m_x--;
  float4 f01( buf.GetPixelRaw(pxy) );

  float4 fxy( f00*(1-dx)*(1-dy) + f10*(dx)*(1-dy) +
              f11*dx*dy + f01*(1-dx)*dy );

  return fxy;

} // GetResampledValueRaw


/*
 *	Get the maxium value at point 
 *  @param buf : the crossplane buffer
 *  @param pnt : the resampling point
 *  @return the maximum value
 */

float4 CrossPlane::GetResampledValueMax( Image<float4> &buf, Point<float4> &pnt ) const
{
  int4 x0( (int4)(pnt.m_x) );
  int4 y0( (int4)(pnt.m_y) );

  float4 dx( pnt.m_x - x0 );
  float4 dy( pnt.m_y - y0 );

  Point2D<uint2> pxy(x0, y0);
  float4 f00( buf.GetPixelRaw(pxy) );
  pxy.m_x++;
  float4 f10( buf.GetPixelRaw(pxy) );
  pxy.m_y++;
  float4 f11( buf.GetPixelRaw(pxy) );
  pxy.m_x--;
  float4 f01( buf.GetPixelRaw(pxy) );

  float4 fxy = Max( Max(f00, f10), Max(f11, f10) );

  return fxy;

} // GetResampledValueRaw



/**
 *	clear the mask buffer of crossplane
 *
 *  @param NO
 *  @return : N/A
 */
void CrossPlane::ClearMaskBuf()
{
  m_crossBufMask.Clear();
} // ClearMaskBuf


/*
 *	isolate the crossplane by thresholding, 
 *  detecting the area closest to the centerPnt of crossplane
 *
 *  @param fParMaskTH : threshold
 *  @return : N/A
 */
void CrossPlane::IsolateMask( const float4 fMaskTH )
{
  ClearMaskBuf();
  Image<bool> & maskBuf = m_crossBufMask;

  m_maskTH = fMaskTH; // save it.  
  
  const Image<float4> &crossBuf = m_crossBuf;
  const int4 iXlen( 2*m_iBufXlenHalf );
  const int4 iYlen( 2*m_iBufYlenHalf );

  std::stack< Point2D<uint2> > seedStk;  
  Point2D<uint2> seedPos( m_iBufXlenHalf, m_iBufYlenHalf );

  // find the seed point close to the centerPnt
  if ( crossBuf.GetPixel( seedPos ) > fMaskTH )
    seedStk.push( seedPos );
  else // find the nearest first seed
  {  
    const int2 r( 15 );  // Qian Added: you are sure that 15<m_iBufYlenHalf and m_iBufYlenHalf???????
       
    bool found( false );   
    int4 iMinDis( 512 );
    Point2D<uint2> minpos(0, 0);

    for ( int2 y=-r; y<r; ++y )
    for ( int2 x=-r; x<r; ++x )
    {
      seedPos.m_x = m_iBufXlenHalf+x;
      seedPos.m_y = m_iBufYlenHalf+y;

      if ( crossBuf.GetPixel( seedPos ) > fMaskTH )
      {
        int4 iDis( x*x + y*y );
        if ( iDis < iMinDis )
        {
          iMinDis = iDis;
          minpos = seedPos;
        }
      }
    }

    if ( iMinDis < 255 )
      seedStk.push( minpos );
  }
  

  // grow from the seed point
  bool growOK( true );
  while ( !seedStk.empty() )
  {
    seedPos = seedStk.top();     
    seedStk.pop(); 

    maskBuf.SetPixelRaw( seedPos, true );
    if ( seedPos.m_x <= 0 || seedPos.m_y <= 0 || seedPos.m_x >= iXlen || seedPos.m_y >= iYlen )
    {
      growOK = false;
      continue;
    }    

    --seedPos.m_x;
    if ( ( !maskBuf.GetPixel(seedPos) ) && ( crossBuf.GetPixel(seedPos) > fMaskTH ) )
        seedStk.push( seedPos );
   
    ++seedPos.m_x;
    --seedPos.m_y;
    if ( ( !maskBuf.GetPixel(seedPos) ) && ( crossBuf.GetPixel(seedPos) > fMaskTH ) )
      seedStk.push( seedPos );
  
    ++seedPos.m_x;
    ++seedPos.m_y;
    if ( ( !maskBuf.GetPixel(seedPos) ) && ( crossBuf.GetPixel(seedPos) > fMaskTH ) )
      seedStk.push( seedPos );

    --seedPos.m_x;
    ++seedPos.m_y;
    if ( ( !maskBuf.GetPixel(seedPos) ) && ( crossBuf.GetPixel(seedPos) > fMaskTH ) )
      seedStk.push( seedPos );

  }

  m_bValidMask = growOK;

  // if it failed we donot clear the maskbuf -- we need it later

} // IsolateMask


/**
 *	extract the contour of the vessel on the crossplane - the isloated mask 
 *  -- extract the general ellipse
 *  -- smooth the pointlist
 *  -- resample to the iNumOfPoint points
 *
 *  @param iNumOfPoints : set the number of points in the contour 
 *  @param contourPoint : the contour extracted 
 *  @return : N/A
 */
void CrossPlane::ExtractContour( const int2 iNumOfPoints, std::vector< Point<float4> > & contourPoint )
{
  //find all the points on boundary
  Image<bool> & maskBuf = m_crossBufMask;
  Image<float4> & crossBuf = m_crossBuf;

  const float4 fDistTH( m_maskTH ); //201.0f
  const int4 iDefaultSeg( 30 );
  const float4 fDelta( static_cast< float4 >( 2 * M_PI / iDefaultSeg ) );

  std::vector< Point<float4> > distEdgePointList;

  //initialize the contour
  float4 angleA(0.0F);
  int4 a(0);
  for ( ; a<iDefaultSeg; ++a, angleA+=fDelta )
  {
    //normalized 
    float4 fDelX( cos(angleA) );
    float4 fDelY( sin(angleA) );

    Vector<float4> vDelV(fDelX, fDelY, 0.0F );

    int4 nX( fabs(fDelX) > 1.0e-5f ? (int4)( 0.5+ (m_iBufXlenHalf)/fabs(fDelX) ) : 2*m_iBufXlenHalf );
    int4 nY( fabs(fDelY) > 1.0e-5f ? (int4)( 0.5+ (m_iBufYlenHalf)/fabs(fDelY) ) : 2*m_iBufYlenHalf );
    int4 deln( nX > nY ? nY : nX );

    Point<float4> pnt( m_iBufXlenHalf, m_iBufYlenHalf, 0.0F );
    float4 fLastPixel( GetResampledValueRaw( crossBuf, pnt ) );
    float4 fCurrPixel( fLastPixel );
    for (int2 i(0); i<deln; ++i )
    {
      pnt += vDelV;

      // for the case of dist-volume 
      fCurrPixel = GetResampledValueRaw( crossBuf, pnt );
      if ( fCurrPixel < fDistTH )
        break;
      
      fLastPixel = fCurrPixel;
    }

    float4 fDl( (fCurrPixel-fDistTH)/(fCurrPixel-fLastPixel) ); //supposed they are linear 
    fDl = Bound( 0.01f, fDl, 0.99f );
    pnt = pnt - vDelV*fDl;

    distEdgePointList.push_back( pnt );
  }
  
  //Qian Added: the following procedures both work on units ( NOT floats ) 
  SmoothPointList( distEdgePointList );
  
  //Qian Added: the input of Pointlist is by (unit) index on the section , the output of the list is by relative index( units ) to the center(m_iBufXlenHalf, m_iBufYlenHalf)  of the section 
  Parameterize( distEdgePointList, iNumOfPoints, contourPoint );
  
} // ExtractContour



/*
 *	find a non-calcificied center on the crossplane
 *  @param vNonPlaqueCenter : position 
 *  @param iPlaqueTh : calcium threhold 
 *  @return true if found
 */
bool CrossPlane::FindNonPlaqueCenter( Point<float4> & vNonPlaqueCenter, const int4 iPlaqueTh )
{
  const int2 iXlenHalf( m_iBufXlenHalf );
  const int2 iYlenHalf( m_iBufYlenHalf );
  const float4 fPlaqueTh( iPlaqueTh );
  Image<float4> & crossBuf = m_crossBuf;
  Image<bool> & maskBuf = m_crossBufMask;

  const int4 iAvgRadius( (int4)( sqrt(m_fArea / M_PI) ) - 1.0F );

  float4 fWeight( 0.0F ), fXWeight( 0.0F ), fYWeight( 0.0F );
  int4 r( 0 );
  bool bFoundInnerArea( false );
  bool bFoundNonPlaqueCenter( false );
  Point<float4> nonPlaquePnt( -r+iXlenHalf, r+iYlenHalf, 0.0F );

  //Qian Added: Search the neighorhood of the center, rectangle by rectangle
  while ( ++r < iAvgRadius && !bFoundInnerArea )
  {    
    for ( nonPlaquePnt.m_x=-r+iXlenHalf; nonPlaquePnt.m_x<=r+iXlenHalf&&!bFoundInnerArea; ++nonPlaquePnt.m_x )
    {
      if ( GetResampledValueMax( crossBuf, nonPlaquePnt ) < fPlaqueTh )
        bFoundInnerArea = true;
    }
    if ( bFoundInnerArea )
      break;

    for ( nonPlaquePnt.m_y=r+iYlenHalf; nonPlaquePnt.m_y>=-r+iYlenHalf&&!bFoundInnerArea; --nonPlaquePnt.m_y )
    {
      if ( GetResampledValueMax( crossBuf, nonPlaquePnt ) < fPlaqueTh )
        bFoundInnerArea = true;
    }
    if ( bFoundInnerArea )
      break;

    for ( nonPlaquePnt.m_x=r+iXlenHalf; nonPlaquePnt.m_x>=-r+iXlenHalf&&!bFoundInnerArea; --nonPlaquePnt.m_x )
    {
      if ( GetResampledValueMax( crossBuf, nonPlaquePnt ) < fPlaqueTh )
        bFoundInnerArea = true;
    }
    if ( bFoundInnerArea )
      break;

    for ( nonPlaquePnt.m_y=-r+iYlenHalf; nonPlaquePnt.m_y<=r+iYlenHalf&&!bFoundInnerArea; ++nonPlaquePnt.m_y )
    {
      if ( GetResampledValueMax( crossBuf, nonPlaquePnt ) < fPlaqueTh )
        bFoundInnerArea = true;
    }
    if ( bFoundInnerArea )
      break;
  }

  //Qian Added: weighted center 
  if ( bFoundInnerArea ) 
  {
    r += 2;
    for ( int4 y=-r; y<=r; ++y )
    { 
      Point2D<int2> pnt(0,0);
      
      ///////////////////////ModificationByQian /////////////////////     
      //-----------------------Replace -----------------------------
      //pnt.m_y = y + iYlenHalf;
      //-----------------------by ----------------------------------
      pnt.m_y = y + nonPlaquePnt.m_y;
      ///////////////////// End of Modification ///////////////////
      for ( int4 x=-r; x<=r; ++x )
      {
        ///////////////////////ModificationByQian /////////////////////     
        //-----------------------Replace -----------------------------
        //pnt.m_x = x + iXlenHalf;
        //-----------------------by ----------------------------------
        pnt.m_x = x + nonPlaquePnt.m_x;
        ///////////////////// End of Modification ///////////////////

        if ( maskBuf.GetPixelRaw(pnt) )
        {
          float4 w = GetResampledValueMax( crossBuf, Point<float4>(pnt.X(), pnt.Y(), 0.0F) );
          if ( w < fPlaqueTh )
          {
            w = fPlaqueTh - w;
            w = w*w;
            fWeight += w;
            fXWeight += (pnt.m_x*w);
            fYWeight += (pnt.m_y*w);   
          }
        }
      }
    }
  }

  if ( fWeight < 1.0 )
  {
    bFoundNonPlaqueCenter = false;
  }
  else
  {
    vNonPlaqueCenter.m_x = (float4)(fXWeight/fWeight);
    vNonPlaqueCenter.m_y = (float4)(fYWeight/fWeight);
    vNonPlaqueCenter.m_z = 0.0f;

    if ( GetResampledValueMax( crossBuf, Point<float4>(vNonPlaqueCenter) ) >= fPlaqueTh )
      bFoundNonPlaqueCenter = false;
    else
      bFoundNonPlaqueCenter = true;
  }

  //LogDbg(" nonPlaqueCenter = " + vNonPlaqueCenter + " r = " + ToAscii(r), "CenterlineRefiner", "SaveOnePath2vxVessel");

  return bFoundNonPlaqueCenter;

}


/*
 *	is there calcium on the current crossplane
 *  @param intensityVol : inteisty volume
 *  @iPlaqueTh : calcium threhold
 *  @return true if found
 */
bool CrossPlane::IsCalcified( vxBlockVolume<uint2> & intensityVol, const int4 iPlaqueTh )
{
  const int4 iXlenHlaf( m_iBufXlenHalf );
  const int4 iYlenHalf( m_iBufYlenHalf );
  const float4 fDistTH( m_maskTH ); //201.0f

  const Triplef units( intensityVol.GetHeader().GetVoxelUnits() );
  Matrix<float4> mIM2Vol( GetIM2VolTransformMatrix(units) );

  bool bCalcified(false);
  float4 * pBuf( (float4*)(m_crossBuf.GetDataPtr()) );
  
  for ( int4 v(-iYlenHalf); v<iYlenHalf+1 && !bCalcified; ++v )
  {
    Point<float4> p0( -iXlenHlaf, v, 0.0F );
    Point<float4> p1(  iXlenHlaf, v, 0.0F );

    Point<float4> bgnPnt( mIM2Vol*p0 );
    Point<float4> endPnt( mIM2Vol*p1 );

    RaySegment raySegment( Vector4Df(bgnPnt, 0), Vector4Df(endPnt, 1) );
    Vector4Df rayIncrement( raySegment.GetDirection() / (2.0F*iXlenHlaf) );
    vxBlockVolumeRayIterator<uint2> samLineIter( intensityVol, raySegment, rayIncrement );

    for ( int4 u(0); u<2*iXlenHlaf+1 && !bCalcified ; ++u, ++samLineIter, ++pBuf )
    {
      if ( *pBuf > fDistTH && samLineIter.GetSample() >= iPlaqueTh )
        bCalcified = true;
    }
  } // for

  return bCalcified;
} //IsCalcified


/*
 *	shrink the wall contour to inner contour
 */
void CrossPlane::ShrinkContour( const std::vector< Point<float4> > & wallContour, std::vector< Point<float4> > & lumenContour, const int4 iPlaqueTh )
{
  Image<float4> & crossBuf = m_crossBuf;
  float4 *radiusArray = new float4[m_iBufXlenHalf*2];
  const float4 fPlaqueTh( iPlaqueTh );
  const float4 fInnerTh( fPlaqueTh - 50.0F );
  const float4 fPlaqueEdgeTh( 30.0F );
  const float4 fBoundGradTh( 100.0F );

  std::vector< Point<float4> > intpWallContour, shiftedWallContour, innerContour;
  
  //get length of contour and reparameterize it
  const float4 fStep( 1.0F );	
  float4 fAccLen( 0.0F ), fPrevLen( 0.0F ), fdLen( 0.0F );
  int4 count( 1 );
  Point<float4> prevPnt( wallContour[0] );
  for ( int4 i(1); i<wallContour.size(); ++i )
  {
    Point<float4> nextPnt( wallContour[i] );
    float4 fXlen( (float4)(nextPnt.m_x - prevPnt.m_x) );
    float4 fYlen( (float4)(nextPnt.m_y - prevPnt.m_y) );
    float4 fLength( sqrt( fXlen*fXlen + fYlen*fYlen ) );
    
  	//////////////////////////// ModificationByQian ///////////////////////////
    //-------------------------- Replace -------------------------------------
    //float4 fPrevLen( fAccLen );
		//---------------------------- by ----------------------------------------
    float4  fPrevLen = fStep - fAccLen;
    /////////////////////////////////////////////////////////////////////////  
    fAccLen += fLength;
    while ( fAccLen > fStep )
    {
      float4 coef( fPrevLen / fLength );
      Point<float4> fpnt;
      fpnt.m_x = ( prevPnt.m_x*(1.0-coef) + nextPnt.m_x*coef);  // refer to centerPnt
      fpnt.m_y = ( prevPnt.m_y*(1.0-coef) + nextPnt.m_y*coef);
      intpWallContour.push_back(fpnt);

      fAccLen -= fStep;
      fPrevLen += fStep;
    }
    prevPnt = nextPnt;
  } // for


  // adjust centerPnt if necessary
  Point<float4> vBufCenter( m_iBufXlenHalf, m_iBufYlenHalf, 0.0f ); 
  bool bHasNonPlaqueCenter( true );
  Point<float4> nonPlaqueCenter( vBufCenter );
  if ( GetResampledValueMax( crossBuf, vBufCenter ) >= fPlaqueTh )
  {
    bHasNonPlaqueCenter = FindNonPlaqueCenter( nonPlaqueCenter, iPlaqueTh );
  }
  else
    bHasNonPlaqueCenter = true;


  if ( !bHasNonPlaqueCenter )
  {
    LogDbg(" !!! NO lumen -- full plaque !!! " + nonPlaqueCenter + " PlaqueThrehold = "+ToAscii(iPlaqueTh), "CenterlineRefiner", "SaveOnePath2vxVessel");
  }

  if ( bHasNonPlaqueCenter )
  {
    //shrink the wall contour first and create inner contour

    const float4 fRelaxR( 1.0F );  // Qian Added: the shrinking distance ( units ), By setting fRelaxR as 1.0, it means we shrink by approximately 1 pixel
    const float4 fSampleStep( 1.0F );
    const Vector<float4> vNonPlaqueCenter( nonPlaqueCenter );
    bool bHasPlaqueInContour( false );

    for ( int4 i(0); i<intpWallContour.size(); ++i )
    {
      Point<float4> wallPnt( intpWallContour[i] + Vector<float4>(vBufCenter) );
      Normal<float4> vRelaxDir( Vector<float4>(wallPnt)- vNonPlaqueCenter );  // Qian Added: vRelaxDir is automatically normalized
      wallPnt = wallPnt + fRelaxR * vRelaxDir;

      Vector<float4> vWall2Center ( vNonPlaqueCenter - Vector<float4>(wallPnt) );  

      float4 fRadius( vWall2Center.GetLength() );
      vWall2Center.Normalize();
      vWall2Center *= fSampleStep;
      int4 iRadius = (int4)( fRadius / fSampleStep ) + 1;
       
      float4 fWallR( fRelaxR/fSampleStep );
 
      float4 fInnerR( 0.0F );   // Qian Added: the unit distance to the wall ( not the center ) 

      // find the lumen point
      bool bPlaqueFound( false );
      Point<float4> pnt( wallPnt );
      for (int4 l(0); l<iRadius; ++l )
      {
        float4 fCurrPixel( GetResampledValueMax( crossBuf, pnt) );
        radiusArray[l] = fCurrPixel;

        pnt += vWall2Center; 
      }

      int4 l(iRadius-1);
      for( ; l > 1; --l )
      {
        if ( radiusArray[l] >= fPlaqueTh )
        {
          bPlaqueFound = true;
          fInnerR = l;
          break;
        }
      }

      if ( bPlaqueFound )
      {
         bHasPlaqueInContour = true;
        if ( l < iRadius -1 )
          fInnerR = Interpolate(radiusArray[l+1], fPlaqueTh, radiusArray[l], (float4)(l+1), (float4)l );
        else
          fInnerR = iRadius - 0.5f*(fRadius-iRadius+1);
      }
       

      if ( fInnerR > 100.0f )
      {
        LogDbg(" invalid fInnerR = "+ToAscii(fInnerR)+" ; l = "+ToAscii(l)+" radiusArray[l+1] = "+ToAscii(radiusArray[l+1])+" radiusArray[l] = "+ToAscii(radiusArray[l]), " ", " " ); 
      }

      if ( bPlaqueFound )
      {
        if ( fInnerR < fWallR )
          fWallR = fInnerR;
      }
      else 
        fInnerR = fWallR;

      Point<float4> shiftedWallPnt( wallPnt + vWall2Center*fWallR );
      Point<float4> innerPnt( wallPnt + vWall2Center*fInnerR );
      if ( innerPnt.m_x < 0.0f || innerPnt.m_y < 0.0f )
      {
        LogDbg(" fInnerR = "+ToAscii(fInnerR) + " wallPnt = "+wallPnt + " innerPnt = "+innerPnt+", vWall2Center = "+vWall2Center, "CenterlineRefiner", "SaveOnePath2vxVessel");
      }

      shiftedWallContour.push_back( shiftedWallPnt );
      innerContour.push_back( innerPnt );

    }

    if ( bHasPlaqueInContour ) // found plaque
    {
      SmoothPointList( innerContour );
      Parameterize(innerContour, wallContour.size(), lumenContour );
    }
    else
      lumenContour = wallContour;
  }
  else
  {
    lumenContour.resize(0);   
  }


  delete [] radiusArray;
  radiusArray = NULL;

} // ShrinkContour()




/**
 *	smoothing the point on one contour
 *
 *  @param pointList : the point list of contout
 *  @return : N/A
 */
void CrossPlane::SmoothPointList( std::vector< Point<float4> > & pointList ) const
{
  std::vector< Point<float4> > & srcPointList = pointList;
  std::vector< Point<float4> > resPointList;

  const int4 iNumOfPoints( srcPointList.size() );

  for ( int4 i(0); i<iNumOfPoints; ++i )
  {
    int4 i0( (iNumOfPoints+i-1)%iNumOfPoints );
    int4 i1( i );
    int4 i2( (i+1)%iNumOfPoints );

    Point<float4> pnt(0.0F, 0.0F, 0.0F);    
    pnt.m_x = (srcPointList[i0].m_x + 2*srcPointList[i1].m_x + srcPointList[i2].m_x ) / 4.0;
    pnt.m_y = (srcPointList[i0].m_y + 2*srcPointList[i1].m_y + srcPointList[i2].m_y ) / 4.0;

    resPointList.push_back(pnt);
  }

  pointList.resize(0);
  for ( int4 i(0); i<iNumOfPoints; ++i )
  {
    pointList.push_back( resPointList[i] );
  }


} // SmoothPointList



/**
 *	parameterize the contour accodring to the fLength ( working on units not float ) 
 *
 *  @param srcContour :  input contour
 *  @param iNumOfPoints : set the number of points
 *  @param resContour : resulting contour
 *  @return : N/A
 */
void CrossPlane::Parameterize( const std::vector< Point<float4> > & srcContour, const int4 iNumOfPoints, std::vector< Point<float4> > & resContour ) const
{ 
  const float4 fEpsilon( 0.0000001F );

  if ( srcContour.size() == 0 )
  {
    resContour.resize(0);
    return;
  }

  resContour.resize(0);

  const int4 iSrcContoutNumOfPoints( srcContour.size() );
  std::vector< Point<float4> > keepPntList;

  // supposing that the contour starts from x-axis and anti-clockwise and close the contour
  for ( int4 i=0; i<iSrcContoutNumOfPoints; ++i )
  {
    keepPntList.push_back( srcContour[i] );
  }
  keepPntList.push_back( srcContour[0] );

	float4 fTotalLength( 0.0F ); // for pixel fLength
	Point<float4> prevPnt( keepPntList[0] );
	for ( int4 i(1); i<keepPntList.size(); ++i )
	{
		Point<float4> nextPnt = keepPntList[i];
		float4 fXlen ( (float4)(nextPnt.m_x - prevPnt.m_x) );
		float4 fYlen ( (float4)(nextPnt.m_y - prevPnt.m_y) );
		fTotalLength += sqrt( fXlen*fXlen + fYlen*fYlen );

    prevPnt = nextPnt;
	}
	
	prevPnt = keepPntList[0];
  Point<float4> fpnt(0.0F, 0.0F, 0.0F);
  fpnt.m_x = prevPnt.m_x - m_iBufXlenHalf;
  fpnt.m_y = prevPnt.m_y - m_iBufYlenHalf;

	resContour.push_back(fpnt);	// the start point
  if ( fTotalLength < 0.5F )
  {
    LogDbg(" source contour is too short : "+ToAscii(fTotalLength), "CrossPlane", "Parameterize");
    for (int4 i(0); i<srcContour.size(); ++i )
    {
      LogDbg( ToAscii(i)+" : "+srcContour[i].ToString(), "CrossPlane", "Parameterize");
    }

    resContour.resize(0);
    return ;
  }

  //Qian Added: evenly resample the contour at interval of fStep 
  const float4 fStep( fTotalLength / (iNumOfPoints-2) );	
  float4 fAccLen( 0.0F ), fPrevLen = 0.0, fdLen=0.0;
  int4 count( 1 );
	for ( int4 i(1); i<keepPntList.size(); ++i )
	{
		Point<float4> nextPnt( keepPntList[i] );
		float4 fXlen( (float4)(nextPnt.m_x - prevPnt.m_x) );
		float4 fYlen( (float4)(nextPnt.m_y - prevPnt.m_y) );
		float4 fLength( sqrt( fXlen*fXlen + fYlen*fYlen ) );
		
		///////////////////////////// ModificationByQian ///////////////////////////
    //-------------------------- Replace -------------------------------------
    //fPrevLen = fAccLen;
		//---------------------------- by ----------------------------------------
    fPrevLen = fStep - fAccLen;
    /////////////////////////////////////////////////////////////////////////
    fAccLen += fLength;
		fdLen += fLength;
    //Qian Added: fill the big gap between two Pnt in KeepPntList
		while ( fAccLen > fStep )
		{
      float4 coef( fPrevLen / fLength ); 
   		fpnt.m_x = ( prevPnt.m_x*(1.0-coef) + nextPnt.m_x*coef) - m_iBufXlenHalf;  // refer to centerPnt
			fpnt.m_y = ( prevPnt.m_y*(1.0-coef) + nextPnt.m_y*coef) - m_iBufYlenHalf;
			resContour.push_back(fpnt);
			
      fAccLen -= fStep;
			fPrevLen += fStep;
		}
		prevPnt = nextPnt;
	} // for

  fpnt = keepPntList[keepPntList.size()-1];
  fpnt.m_x -= m_iBufXlenHalf;
  fpnt.m_y -= m_iBufYlenHalf; 
  while ( resContour.size() < iNumOfPoints )  // close it ...
  {
	  resContour.push_back(fpnt);
  }
} // Parameterize



/**
 *	calculate the centroid of the crossplae
 *
 *  @param NO
 *  @return : centroid 
 */
Point<float4> CrossPlane::CalCentroid( )
{
  const Triple<uint4> dim ( m_crossBuf.GetDim() );
  const float4 * pCrossBuf = (float4* )(m_crossBuf.GetDataPtr());
  const bool * pMaskBuf( (bool* )(m_crossBufMask.GetDataPtr()) );

  float4 fWeight( 0.0F );
  float4 fXWeight( 0.0F );
  float4 fYWeight( 0.0F );
  for ( int4 y(0); y<dim.m_y; ++y )
  {
    for ( int4 x(0); x<dim.m_x; ++x, ++pMaskBuf, ++pCrossBuf )
    {
      if ( *pMaskBuf )
      {
        float4 w = *pCrossBuf;
        fWeight += w;
        fXWeight += (x-m_iBufXlenHalf)*w;
        fYWeight += (y-m_iBufYlenHalf)*w;        
      }
    }
  }

  if ( fWeight < 1.0F ) // should not happen
  {
    m_centroid = Point<float4>(0.0F, 0.0F, 0.0F);    
    return m_centroid;
  }
  else
  {
    m_centroid.m_x = (float4)(fXWeight/fWeight);
    m_centroid.m_y = (float4)(fYWeight/fWeight);
    m_centroid.m_z = 0.0F;
    
    return m_centroid; 
  }
} // CalCentroid


/**
 *	calculate the area of the mask --  the number of the pixels
 *  
 *  @param NO
 *  @return : area (number of pixel)
 */
float4 CrossPlane::CalArea()
{
  const Image<bool> & maskBuf = m_crossBufMask; 
  const bool * pBuf = (bool*)(maskBuf.GetDataPtr());

  const Triple<uint4> dim = maskBuf.GetDim();  
  int4 maskCount(0);  

  for ( int4 y(0); y<dim.m_y; ++y )
  {
    for ( int4 x(0); x<dim.m_x; ++x )
    {
      if ( *pBuf++ )
        maskCount++;
    }
  }

  return (float4)(maskCount);

} // CalArea


/**
 *	calculate the two eigen vectors of the crossplane using moments
 *
 *  @param v1 : 1st eigen vector
 *  @param v2 : 2nd eigen vector
 *  @return : N/A
 */
void CrossPlane::CalTwoEigenVectors( Vector<float4> & v1, Vector<float4> & v2 )
{  
  float4 xm, ym, u20, u02, u11;
  int4 count;
  CalMoments( u20, u02, u11, xm, ym, count );

  float4 m4( 4.0*fabs(u02*u20 - u11*u11) );
  float4 a11( u02/m4 );
  float4 a12( u11/m4 );
  float4 a22( u20/m4 );

  // mean centerPnt
  m_iMeanXCenter = xm;
  m_iMeanYCenter = ym;

  float4 theta( 0.5F*atan2(2.0F*u11, u20-u02) );
  if ( theta < 0.0F )
    theta += float4( M_PI*0.5F );

  if ( a12 > 0.0f )
    theta += float4( M_PI*0.5F );
  else if ( a12  == 0.0F )
  {
    if ( a22 > a11 )
    {
      theta = 0.0F;
      float4 tmp = a22;
      a22 = a11;
      a11 = tmp;
    }
    else if ( a11 != a22 )
      theta = float4( M_PI*0.5F );
  } // for

  float4 dy( sin(theta) );
  float4 dx( cos(theta) );

  m_vEigenV1 = Vector<float4>( dx, dy, 0.0f );
  m_vEigenV2 = Vector<float4>( dy, -dx, 0.0f );

  v1 = m_vEigenV1;
  v2 = m_vEigenV2;

}


/**
 *	calculate the moments of the crossplane
 *
 *  @param u20 : second moment on x axis
 *  @param u02 : second moment on y axis
 *  @param u11 : second moment of xy axis
 *  @param xm  : mean centerPnt on X axis
 *  @param ym  : mean centerPnt on Y axis
 *  @param xycount : total number 
 *  @return : N/A
 */
void CrossPlane::CalMoments( float4 & u20, float4 & u02, float4 & u11, float4 & xm, float4 & ym, int4 & xycount )
{
  float4 xsum( 0.0F ), ysum( 0.0F ), x2sum( 0.0F ), y2sum( 0.0F ), xysum( 0.0F );
  float4 xe, ye;
  int4 count(0);
  
  const Image<bool> & maskBuf = m_crossBufMask; 
  const bool * pMaskBuf = (bool *) ( maskBuf.GetDataPtr() );

  const Triple<uint4> dim = maskBuf.GetDim();
  const int4 iXlen( dim.m_x );
  const int4 iYlen( dim.m_y );
  for ( int4 y(0); y<iYlen; ++y )
  {
    int4 xCountOfLine( 0 );
    int4 xSumOfLine( 0 );
    for ( int4 x(0); x<iXlen; ++x )
    {
      if ( *pMaskBuf++ )
      {
        ++xCountOfLine;
        xSumOfLine += x;
        x2sum += x*x;
      }
    }

    xsum += xSumOfLine;
    ysum += xCountOfLine*y;
    xe = xSumOfLine;
    ye = y;
    xysum += xe*ye;
    y2sum += ye*ye*xCountOfLine;

    count += xCountOfLine;
  }

  if ( count == 0 )
    return;

  x2sum += 0.08333333 * count;
  y2sum += 0.08333333 * count;
  
  float4 n ( count );

  float4 x1( xsum / n );
  float4 y1( ysum / n );
  float4 x2( x2sum / n );
  float4 y2( y2sum / n );
  float4 xy( xysum / n );
  
  u20 = x2 - ( x1*x1 );
  u02 = y2 - ( y1*y1 );
  u11 = xy - x1*y1;
  
  xm = x1;
  ym = y1;

  xycount = count;

} // CalMoments


/**
 *	calculate the long and short axis of the current mask
 *
 *  @param No
 *  @return N/A  
 */
void CrossPlane::CalTwoAxis( )
{
  // Qian Added: all the computation are executed on the unit representation of voxels, even they are float type
  Vector<float4> eigenV1, eigenV2;
  CalTwoEigenVectors( eigenV1, eigenV2);

  Vector3Df V1P0, V1P1;
  Vector3Df V2P0, V2P1;
  
  // find the axis
  Image<float4> & crossBuf = m_crossBuf;
  const float4 fDistTH( m_maskTH ); //same as in the extractcontour() //Qian Added, same as the threshold of the mask 
  const float4 fDl( 0.9F ); //detect fStep

  const Triple<uint4> dim( crossBuf.GetDim() );
  Vector3D<float4> vReferPnt( m_iBufXlenHalf, m_iBufXlenHalf, 0.0F );
  const Point3D<float4> centerPnt( GetCentroid() + vReferPnt );

  Point3D<float4> pnt( centerPnt );
  int4 iMaxDis( (int4)(1.5f*m_iBufXlenHalf/fDl) );
  eigenV1.Normalize();
  eigenV1 *= fDl;

  //Qian Added: follow eigenV1  to find the boundary of the mask
  int4 i(0);
  for ( ; i<iMaxDis; ++i )
  {
    pnt += eigenV1;
    float4 fCurrPixel = GetResampledValueRaw( crossBuf, pnt);
    if ( fCurrPixel < fDistTH )
      break;
  }
  float4 fMaxD1 = (i+0.5)*fDl;
  V1P0 = eigenV1*(i+0.5)+Vector3Df(centerPnt)-Vector3Df(vReferPnt);

  pnt = centerPnt;;
  eigenV1 *= -1.0f;
  for (i=0; i<iMaxDis; i++ )
  {
    pnt += eigenV1;
    float4 fCurrPixel = GetResampledValueRaw( crossBuf, pnt);
    if ( fCurrPixel < fDistTH )
      break;
  }
  float4 fMinD1 = (i + 0.5)*fDl;
  V1P1 = eigenV1*(i+0.5)+Vector3Df(centerPnt)-Vector3Df(vReferPnt);

  pnt = centerPnt;
  eigenV2.Normalize();
  eigenV2 *= fDl;
  for (i=0; i<iMaxDis; i++ )
  {
    pnt += eigenV2;
    float4 fCurrPixel = GetResampledValueRaw( crossBuf, pnt);
    if ( fCurrPixel < fDistTH )
      break;
  }
  float4 fMaxD2 = (i+0.5)*fDl;
  V2P0 = eigenV2*(i+0.5)+Vector3Df(centerPnt)-Vector3Df(vReferPnt);

  pnt = centerPnt;
  eigenV2 *= -1.0f;
  for (i=0; i<iMaxDis; i++ )
  {
    pnt += eigenV2;
    float4 fCurrPixel = GetResampledValueRaw( crossBuf, pnt);
    if ( fCurrPixel < fDistTH )
      break;
  }
  float4 fMinD2 = (i+0.5)*fDl;
  V2P1 = eigenV2*(i+0.5)+Vector3Df(centerPnt)-Vector3Df(vReferPnt);

  float4 fDisD1 = fabs(fMaxD1 + fMinD1);
  float4 fDisD2 = fabs(fMaxD2 + fMinD2);

  eigenV1.Normalize();
  eigenV2.Normalize();

  //Qian Added: m_vLongAxis...fWidth are all in UNITS!!!! and m_longAxisP0...m_shortAxisP1 are all RELATIVE UNITs to the center ( m_iBufXlenHalf, m_iBufXlenHalf ) of the section 
	if ( fDisD1 > fDisD2 )
  {
    m_vLongAxis = eigenV1;
    m_vShortAxis = eigenV2;
    m_fDiameter = fDisD1;
    m_fWidth = fDisD2;
    m_longAxisP0  = Point3Df(V1P0);
    m_longAxisP1  = Point3Df(V1P1);
    m_shortAxisP0 = Point3Df(V2P0);
    m_shortAxisP1 = Point3Df(V2P1);
  }
  else
  {
    m_vLongAxis = eigenV2;
    m_vShortAxis = eigenV1;
    m_fDiameter = fDisD2;
    m_fWidth = fDisD1;
    m_longAxisP0  = Point3Df(V2P0);
    m_longAxisP1  = Point3Df(V2P1);
    m_shortAxisP0 = Point3Df(V1P0);
    m_shortAxisP1 = Point3Df(V1P1);
  } 
} // CalTwoAxis


/**
 *	calculate all the shape parameters
 *
 *  @param NO
 *  @return : N/A
 */
void CrossPlane::CalShapePars( )
{
  m_fArea = CalArea();
  if ( m_fArea < 1.0f || !m_bValidMask )  //no crosssection
    return ;
   
  //Qian Added: the centroid is the RELATIVE UNIT distance to the center of the section ( m_iBufXlenHalf, m_iBufXlenHalf )
  CalCentroid();
  CalTwoAxis();

} // CalShapePars


/**
 *	deciding whether the current crossplane is valid 
 *  followings are the invalud cases:
 *    -- area less than 1.0
 *    -- grow to the boundary 
 *    -- move is longer than the width
 * 
 *  @param NO
 *  @return : true if valid
 */
bool CrossPlane::IsValid() const
{
  const float4 fCenterMoveTH( m_fWidth );

  if ( m_fArea < 1.0F ) // zero mask
  {
    //LogDbg(" zero area -- no intersection", "CrossPlane", "IsValid" );
    return false;
  }

  if ( !m_bValidMask ) // grow outside
  {
    //LogDbg(" growing to outside ", "CrossPlane", "IsValid" );
    return false;
  }

  Vector<float4> vDel( m_iMeanXCenter-m_iBufXlenHalf, m_iMeanYCenter-m_iBufYlenHalf, 0.0f);
  
  Point2D<uint2> centerPos( m_iBufXlenHalf, m_iBufYlenHalf );
  bool bCentered( m_crossBuf.GetPixel( centerPos ) > m_maskTH ? true : false );

  if (  vDel.GetLength() > fCenterMoveTH && bCentered )
  {
    //LogDbg(" invlaid centerPnt position, displacement : "+ToAscii(fPossibleMove)+" is bigger than width = "+ToAscii(fCenterMoveTH), "CrossPlane", "IsValid" );
    return false;
  }

  return true;
} // IsValid


/**
 *	transform a pnt on crossplane to its volume coordinate
 *
 *  @param volUnits : volume units (mm)
 *  @param pnt : point on crossplane
 *  @return : volume coordinate 
 */
Point<float4> CrossPlane::ConvertPnt2VolPos( const Triple<float4> & volUnits, const Point<float4> & pnt ) const
{
  const Vector<float4> volScaleVect( 1.0/volUnits.m_x, 1.0/volUnits.m_y, 1.0/volUnits.m_z );
  Matrix<float4> obj2volScale;
  obj2volScale.Scale(volScaleVect);

  const Vector<float4> imgScaleVect(1.0/m_fScale*volUnits.m_x, 1.0/m_fScale*volUnits.m_y, 1.0/m_fScale*volUnits.m_z);
  Matrix<float4> imgScaleMat; 
  imgScaleMat.Scale( imgScaleVect );

  Matrix<float4> mat( obj2volScale*m_matIM2Obj*imgScaleMat );
  
  Point<float4> newPnt ( mat*pnt );

  return newPnt;
} // ConvertPnt2VolPos


/**
 *	get the crossplane to volume coordinate transformation matrix ( Qian Added, the transformation returns is units to units ( NOT float to float ) 
 *
 *  @param volunits : volume units (mm) 
 *  @return : transformation matrix
 */
Matrix<float4> CrossPlane::GetIM2VolTransformMatrix( const Triple<float4> & volUnits ) const
{
  const Vector<float4> volScaleVect( 1.0/volUnits.m_x, 1.0/volUnits.m_y, 1.0/volUnits.m_z );
  Matrix<float4> obj2volScale;
  obj2volScale.Scale(volScaleVect);

  const Vector<float4> imgScaleVect( 1.0/m_fScale*volUnits.m_x, 1.0/m_fScale*volUnits.m_y, 1.0/m_fScale*volUnits.m_z );
  Matrix<float4> imgScaleMat; 
  imgScaleMat.Scale( imgScaleVect );
  
  Matrix<float4> mat( obj2volScale*m_matIM2Obj*imgScaleMat );

  return mat;
} // GetIM2VolTransformMatrix


/**
 *	get the crossplane to world coordinate transformation matrix
 *
 *  @param volunits : volume units (mm) 
 *  @return : transformation matrix
 */
Matrix<float4> CrossPlane::GetIM2WorldTransformMatrix( const Triple<float4> & volUnits ) const
{
  const Vector<float4> imgScaleVect(1.0/m_fScale*volUnits.m_x, 1.0/m_fScale*volUnits.m_y, 1.0/m_fScale*volUnits.m_z);
  Matrix<float4> imgScaleMat; 
  imgScaleMat.Scale( imgScaleVect );

  Matrix<float4> mat( m_matIM2Obj*imgScaleMat );

  return mat;
} // GetIM2WorldTransformMatrix


/**
 *	save the crossplane to a PPM image 
 *
 *  @param sFileName : PPM image filename
 *  @return : N/A
 */
void CrossPlane::SaveSnapshot( const std::string & sFileName ) const
{
  const Image<float4> & crossBuf = m_crossBuf;
  const Image<bool> & maskBuf = m_crossBufMask;

  const Triple<uint4> dim( m_crossBuf.GetDim() );

  Image< PixelRGBA< uint1 > > savedBuf( dim, PixelRGBA< uint1 >(0,0,0,0) );

  Point2D<uint2> pos(0,0);
  const float4 fScale( 255.0f/4095.0f );
  for ( int2 y=0; y<dim.m_y; y++)
  {
    pos.m_y = y;
    for ( int2 x=0; x<dim.m_x; x++ )
    {
      pos.m_x = x;

      float4 fPixel(0.0F);
      if ( maskBuf.GetPixelRaw(pos) )
      {
        fPixel = crossBuf.GetPixelRaw(pos);
        fPixel *= fScale;
      }
      
      int2 iPixel = Bound( (int2)0, (int2)( fPixel ), (int2)255 );

      savedBuf.SetPixelRaw(pos, PixelRGBA<uint1>( iPixel, iPixel, iPixel, 255 ) ) ;
    }
  }

  pos = Point2D<uint2>(m_iBufXlenHalf, m_iBufXlenHalf);
  savedBuf.SetPixelRaw( pos, PixelRGBA<uint1>( 255, 0, 0, 255 ) );  
  
  Point<float4> fpos( m_centroid.m_x+m_iBufXlenHalf, m_centroid.m_y+m_iBufYlenHalf, 0.0F );
  pos = Point2D<uint2>( fpos.m_x+0.5f, fpos.m_y+0.5f );
  savedBuf.SetPixelRaw( pos, PixelRGBA<uint1>( 255, 255, 255, 255 ) );  

  //to save the image
  std::string sFilePrefix = sFileName;
  PixelRGBA< uint1 > * pBuf ( (PixelRGBA< uint1 > *)savedBuf.GetDataPtr() );
  std::ofstream outStream(sFilePrefix.c_str(), std::ios::binary);
  outStream << "P6" << std::endl;
  outStream << "# no comment" << std::endl;
  outStream << dim.m_x << " " << dim.m_y << std::endl;
  outStream << "255" << std::endl;
  PixelRGBA< uint1 > * pPixelEnd = pBuf + dim.m_x * dim.m_y;
  for (PixelRGBA< uint1 > * pPixel=pBuf; pPixel != pPixelEnd; ++pPixel)
  {
    uint1 vRawValue[3];
    vRawValue[0] = pPixel->R();
    vRawValue[1] = pPixel->G();
    vRawValue[2] = pPixel->B();
    outStream.write((char *) vRawValue, 3*sizeof(uint1));
  }
  
} // SaveSnapshot






// Research code -- not used any more
// Donot delete them
//
///**
//*	fill a test cross plane
//*/
//void CrossPlane::FillTestPlane( float4 angle )
//{
//  float4 cx = m_iBufXlenHalf, rx, ru;
//  float4 cy = m_iBufYlenHalf, ry, rv;
//
//  float4 ra = 30.0;
//  float4 rb = 10.0;
//
//  float4 cosa = cos(angle);
//  float4 sina = sin(angle);
//
//  Image<float4> & crossBuf = m_crossBuf; 
//  Triple<uint4> dim = crossBuf.GetDim();
//  Point2D<uint2> pos;
//  int4 x, y, maskCount = 0;
//
//  for (y=0; y<dim.m_y; y++)
//  {
//    pos.m_y = y;
//    ry = y-cy;
//    for (x=0; x<dim.m_x; x++ )
//    {
//      pos.m_x = x;
//      rx = x-cx;
//
//      ru = (rx*cosa - ry*sina)/ra;
//      rv = (rx*sina + ry*cosa)/rb;
//
//
//      if ( ru*ru + rv*rv < 1.0005f )
//      {
//        crossBuf.SetPixelRaw(pos,  500.0f );
//      }
//    }
//  }
//} // FillTestPlane
//
//
//
///**
//* isolate the mask by a ZXPoints crossplane	
//*/
//void CrossPlane::IsolateMask( CrossPlane &zxCrossPlane )
//{
//  Image<bool> &maskBuf = m_crossBufMask;
//  Image<float4> &crossBuf = m_crossBuf;
//  Image<float4> &zxCrossBuf = zxCrossPlane.m_crossBuf;
//  int4 iXlen = 2*m_iBufXlenHalf;
//  int4 iYlen = 2*m_iBufYlenHalf;
//
//  Point<int4> mask[4] = 
//  {
//    Point<int4>(-1, 0, 0), Point<int4>( 1, 0, 0), 
//      Point<int4>( 0, -1, 0), Point<int4>( 0, 1, 0)
//  };
//
//  maskBuf.Clear();
//
//  float4 distMaskTH = 215.0f; // background of the distvol
//  float4 zxMaskTH = 3000.0f;
//
//
//  std::stack< Point<int4> > seedStk;  
//  Point2D<uint2> seedPos;
//  Point<int4> pos = Point<int4>(m_iBufXlenHalf, m_iBufYlenHalf, 0 );
//  int2 r=2, x, y;
//
//  seedPos.Set(pos.X(), pos.Y());
//  if ( crossBuf.GetPixel( seedPos ) > distMaskTH )
//    seedStk.push( pos );
//  else // find the nearest first seed
//  {  
//    bool found = false;
//    int4 iMinDis=255, dis;
//    Point<int4> minpos(0, 0, 0);
//
//    for ( y=-r; y<r; y++ )
//      for ( x=-r; x<r; x++ )
//      {
//        pos.m_x = m_iBufXlenHalf+x;
//        pos.m_y = m_iBufYlenHalf+y;
//        seedPos.Set(pos.X(), pos.Y());
//        if ( crossBuf.GetPixel( seedPos ) > distMaskTH )
//        {
//          dis = x*x + y*y;
//          if ( dis < iMinDis )
//          {
//            iMinDis = dis;
//            minpos = pos;
//          }
//        }
//      }
//
//      if ( iMinDis < 255 )
//        seedStk.push( minpos );
//  }
//
//
//  bool growOK = true;
//  while ( !seedStk.empty() )
//  {
//    Point<int4> centerPnt = seedStk.top(); seedStk.pop();
//
//    seedPos.Set(centerPnt.X(), centerPnt.Y());
//    maskBuf.SetPixelRaw( seedPos, true );
//
//    if ( zxCrossBuf.GetPixel( seedPos ) > zxMaskTH )
//      continue;
//
//    for (int4 i=0; i<4; i++)
//    {
//      pos = centerPnt+Vector3Di(mask[i]);
//      if ( pos.m_x <= 0 || pos.m_y <= 0 || pos.m_x >= iXlen-1 || pos.m_y >= iYlen-1 )
//      {
//        growOK = false;
//        continue;
//      }
//
//      seedPos.Set(pos.X(), pos.Y());
//      if ( maskBuf.GetPixel(seedPos) == false )
//      {
//        if ( crossBuf.GetPixel(seedPos) > distMaskTH )
//          seedStk.push( pos );
//      }
//    }   
//  }
//
//  if ( !growOK )  // grow to the boundary -- cut plane is too small
//  {
//    maskBuf.Clear();
//  }
//} // IsolateMask
//
//
///**
// * parametrized
// */
//void CrossPlane::Parameterize( std::vector< Point<float4> > &srcContour, int4 iNumOfPoints, std::vector< Point<float4> > &resContour )
//{
//  const float4 fEpsilon = 0.0000001f;
//
//  resContour.resize(0);
//  int4 iSrcContoutNumOfPoints = srcContour.size();
//  std::vector< Point<float4> > keepPntList;
//
//  Point<float4> pnts = srcContour[0];
//  Point<float4> pnte = srcContour[iSrcContoutNumOfPoints -1];
//
//  int4 order=0, startPos, i;
//  //float4 xtmp, maxXcenter=0.0f, coef;
//  Point<float4> fpnt;
//  Point<float4> prevPnt, nextPnt, startPnt, pnt;
//
//  // supposing that the contour starts from x-axis and anti-clockwise and close the contour
//  for ( int4 i=0; i<iSrcContoutNumOfPoints; ++i )
//  {
//    keepPntList.push_back(srcContour[i]);
//  }
//  keepPntList.push_back( pnts );
//
//
//  //now we find the nearest point on x-axis as the start point
//  if ( (pnts - pnts).GetMaxAbsComponent() < 0.5f ) // closed curve -- looking the start point.
//  {
//    Point<float4> centerPnt( m_iBufXlenHalf, m_iBufYlenHalf, 0);
//
//    //we donot need to detect the orientation of the contour.
//    //prevPnt = srcContour[0];
//    //for ( i=1; i<iSrcContoutNumOfPoints; i++)
//    //{
//    //	nextPnt = srcContour[i];
//    //  xtmp = -1.0f;
//
//    //	if ( prevPnt.m_y <= centerPnt.m_y && nextPnt.m_y > centerPnt.m_y )
//    //	{
//    //		coef = (float4) (centerPnt.m_y - prevPnt.m_y) 
//    //			/ (nextPnt.m_y - prevPnt.m_y);
//    //		xtmp = (1.0-coef)*prevPnt.m_x + coef*nextPnt.m_x;
//    //		ordertmp = -1;
//    //	}
//    //	else if ( nextPnt.m_y <= centerPnt.m_y && prevPnt.m_y > centerPnt.m_y )
//    //	{ 
//    //		coef = (float4) (centerPnt.m_y - nextPnt.m_y) 
//    //			/ (prevPnt.m_y - nextPnt.m_y);
//    //		xtmp = (1.0-coef)*nextPnt.m_x + coef*prevPnt.m_x;
//    //		ordertmp = 1;
//    //	}
//    //	
//    //	if ( xtmp > maxXcenter )
//    //	{
//    //		maxXcenter = xtmp;
//    //		startPos = i;
//    //		order = ordertmp;
//    //	}
//
//    //	prevPnt = nextPnt;
//    //}
//
//    order = -1;
//    startPos = 0;
//    float4 maxXcenter = pnts.m_x;
//    if ( order == 0 ) //wrong contouring
//    {
//      cout<< "wrong contouring !!! " << endl ;
//      return;
//    }
//
//    startPnt.m_x = maxXcenter;
//    startPnt.m_y = centerPnt.m_y;
//
//    keepPntList.resize(0);
//    if ( order > 0 )
//    {
//      keepPntList.push_back( startPnt );
//      for ( i=startPos; i<iSrcContoutNumOfPoints; i++)
//      {
//        pnt = srcContour[i];
//        keepPntList.push_back(pnt);
//      }
//      for ( i=1; i<startPos; i++)
//      {
//        pnt = srcContour[i];
//        keepPntList.push_back(pnt);
//      }
//      keepPntList.push_back( startPnt );
//    }
//    else
//    {
//      keepPntList.push_back( startPnt );
//      //cout << "startPos " << startPos << endl;
//      //cout << "startPnt " << startPnt.AsString() << endl; 
//      for ( i=startPos-1; i>0; i--)
//      {
//        pnt = srcContour[i];
//        keepPntList.push_back(pnt);
//        //cout << "pnt " << pnt.AsString() << endl;
//      }
//      for ( i=iSrcContoutNumOfPoints-1; i>startPos-1; i--)
//      {
//        pnt = srcContour[i];
//        keepPntList.push_back(pnt);
//        //cout << "pnt " << pnt.AsString() << endl;
//      }
//      keepPntList.push_back( startPnt );
//      //cout << "endPnt " << startPnt.AsString() << endl;
//    }
//  }
//  else  // opened curve
//  {
//    for ( i=0; i<iSrcContoutNumOfPoints; i++)
//    {
//      pnt = srcContour[i];
//      keepPntList.push_back(pnt);
//    }
//  }
//
//
//  float4 fLength = 0.0, fXlen, fYlen; // for pixel fLength
//  prevPnt = keepPntList[0];
//  for (i=1; i<keepPntList.size(); i++)
//  {
//    nextPnt = keepPntList[i];
//    fXlen = (float4)(nextPnt.m_x - prevPnt.m_x);
//    fYlen = (float4)(nextPnt.m_y - prevPnt.m_y);
//    prevPnt = nextPnt;
//    fLength += sqrt( fXlen*fXlen + fYlen*fYlen );
//  }
//
//
//  prevPnt = keepPntList[0];
//  fpnt.m_x = prevPnt.m_x - m_iBufXlenHalf;
//  fpnt.m_y = prevPnt.m_y - m_iBufYlenHalf;
//  fpnt.m_z = 0.0f;
//  resContour.push_back(fpnt);	// the start point
//
//  float4 fAccLen = 0.0; 
//  float4 fPrevLen = 0.0;
//  int4 count= 1;
//  float4 fdLen=0.0;
//  float4 fStep = fLength / (iNumOfPoints-2);
//  for (i=1; i<keepPntList.size(); i++)
//  {
//    nextPnt = keepPntList[i];
//    fXlen = (float4)(nextPnt.m_x - prevPnt.m_x);
//    fYlen = (float4)(nextPnt.m_y - prevPnt.m_y);
//    fLength = sqrt( fXlen*fXlen + fYlen*fYlen );
//
//    fPrevLen = fAccLen;
//    fAccLen += fLength;
//    fdLen += fLength;
//    while ( fAccLen > fStep )
//    {
//      float4 coef = fPrevLen / fLength;
//      fpnt.m_x = ( prevPnt.m_x*(1.0-coef) + nextPnt.m_x*coef) - m_iBufXlenHalf;  // refer to centerPnt
//      fpnt.m_y = ( prevPnt.m_y*(1.0-coef) + nextPnt.m_y*coef) - m_iBufYlenHalf;
//      resContour.push_back(fpnt);
//
//      fAccLen -= fStep;
//      fPrevLen += fStep;
//    }
//    prevPnt = nextPnt;
//  }
//  while ( resContour.size() < iNumOfPoints )
//  {
//    fpnt = keepPntList[keepPntList.size()-1];
//    fpnt.m_x -= m_iBufXlenHalf;
//    fpnt.m_y -= m_iBufYlenHalf;
//    resContour.push_back(fpnt);
//
//  }
//
//  return;
//
//} // Parameterize
//
//
///**
//*	calculate the geometrical centroid 
//*  !!! not used any more !!!
//*/
//Point<float4> CrossPlane::CalGeoCenter()
//{
//  const int4 num = 30;
//  std::vector< Point<float4> > crossContour;
//
//  ExtractContour( num, crossContour );
//
//  float4 sumX=0.0f, sumY=0.0f;
//  for ( int cnt = 0; cnt < crossContour.size(); cnt++ )
//  {
//    Point<float4> crossPoint = crossContour[cnt];
//    sumX += crossPoint.X();
//    sumY += crossPoint.Y();
//  }
//
//  if ( crossContour.size() )
//  {
//    m_centroid.m_x = sumX/(float4)(crossContour.size());
//    m_centroid.m_y = sumY/(float4)(crossContour.size());
//    m_centroid.m_z = 0.0f;
//  }
//
//  return m_centroid;
//
//} // CalGeoCenter
//
///**
//*	calculate the geometrical centerPnt 
//*  !!! not used any more !!!
//*/
//void CrossPlane::CalMeanCenter( float4 &xc, float4 &yc )
//{
//  Image<bool> &maskBuf = m_crossBufMask; 
//  Triple<uint4> dim = maskBuf.GetDim();
//
//  Point2D<uint2> pos;
//  int4 x, y, fWeight=0, fXWeight=0, fYWeight=0;
//
//  for (y=0; y<dim.m_y; y++)
//  {
//    pos.m_y = y;
//    for (x=0; x<dim.m_x; x++ )
//    {
//      pos.m_x = x;
//      if ( maskBuf.GetPixelRaw(pos) )
//      {
//        fWeight++;
//        fXWeight += x;
//        fYWeight += y;
//      }
//    }
//  }
//
//  if ( fWeight > 0 )
//  {
//    m_iMeanXCenter = (float4)fXWeight/(float4)fWeight;
//    m_iMeanYCenter = (float4)fYWeight/(float4)fWeight;
//  }
//  else
//  {
//    m_iMeanXCenter = 0.0f;
//    m_iMeanYCenter = 0.0f;
//  }
//
//  xc = m_iMeanXCenter;
//  yc = m_iMeanYCenter;
//
//  return ;
//
//} // CalMeanCenter
//
///**
//*	get the geometrical centerPnt
//*/
//Point<float4> CrossPlane::GetMeanCenter()
//{ 
//  return Point<float4>( m_iMeanXCenter - m_iBufXlenHalf, m_iMeanYCenter - m_iBufXlenHalf, 0.0f);
//} // GetMeanCenter


// undefines
#undef FILE_REVISION


// $Log: CrossPlane.C,v $
// Revision 1.2  2007/10/03 16:14:01  cqian
// make it able to handle anisotropic blockvolume
//
// Revision 1.1  2006/08/30 21:18:56  frank
// moved here from vascular
//
// Revision 1.3  2006/06/05 15:01:18  frank
// updated for visual studio 2005
//
// Revision 1.2  2005/06/08 14:51:43  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/03/05 21:24:09  geconomos
// Modifications for 7.1 compilerby  resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2004/03/02 01:10:37  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.20.2.1  2004/02/24 18:28:44  frank
// set deleted pointers to null
//
// Revision 1.20  2004/01/16 21:25:28  wenli
// Code Reviewer
//
// Revision 1.19  2004/01/14 18:51:43  wenli
// Fill cross-section with Bool volume is added
//
// Revision 1.18  2004/01/09 16:28:22  wenli
// remove non-used codes
//
// Revision 1.17  2003/11/04 16:54:52  frank
// removed compile warning
//
// Revision 1.16  2003/11/03 16:31:52  frank
// formatting
//
// Revision 1.15  2003/09/09 14:59:01  wenli
// initialize the area in abnormal cases
//
// Revision 1.14  2003/08/21 13:08:03  michael
// using M_PI instead of typed in values for it
//
// Revision 1.13  2003/08/12 18:22:25  wenli
// remove the warning in IsolateMask()
//
// Revision 1.12  2003/06/19 20:15:07  wenli
// save the crosssection to a frame (Image: RGBA<uint1>)
//
// Revision 1.11  2003/06/18 15:19:44  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.10  2003/06/12 20:15:53  wenli
// copy the wallContour to lumenContour if there is no plaque
//
// Revision 1.9  2003/06/03 14:52:03  wenli
// fScale the value in savesnapshot
//
// Revision 1.8  2003/05/28 17:27:34  wenli
// using average radius to estimate the centerPnt
//
// Revision 1.7  2003/05/27 20:28:25  wenli
// fixing the lumencontour
//
// Revision 1.6  2003/05/21 20:50:57  wenli
// donot change the wallcontour -- keep the one from Refinement
//
// Revision 1.5  2003/05/20 21:19:57  wenli
// calculate a new centerPnt if centerPnt is in plaque
//
// Revision 1.4  2003/05/16 16:44:28  frank
// code review
//
// Revision 1.3  2003/05/14 18:10:43  wenli
// add WallContour and LumenContour
//
// Revision 1.2  2003/05/08 20:09:30  geconomos
// Moved from Viatronix.Processors.Explorer.Plugins.CTA and forgot to update.
//
// Revision 1.6  2003/05/07 20:59:46  dongqing
// code walkthrough
//
// Revision 1.5  2003/04/30 19:16:10  wenli
// code reviewed
//
// Revision 1.4  2003/03/25 16:50:50  wenli
// set the crosssection iSize dynamicly
//
// Revision 1.3  2003/03/18 19:06:58  wenli
// code refinement
//
// Revision 1.2  2003/03/17 18:54:23  wenli
// CenterlineRefiner is splited into two files and unused codes are cleared.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/CrossPlane.C,v 1.2 2007/10/03 16:14:01 cqian Exp $
// $Id: CrossPlane.C,v 1.2 2007/10/03 16:14:01 cqian Exp $