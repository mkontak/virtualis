//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wenli Cai, wenli@viatronix.com


// Includes
#include "stdafx.h"
#include "Resampler.h"
#include "vxEnvironment.h"
#include "Timer.h"


// Namespaces
using namespace std;


// Defines
#define FILE_REVISION "$Revision: 1.2 $"

/**
 * constructor
 * @param srcvol
 * @param resvol
 */
Resampler::Resampler( vxBlockVolume<uint2> &srcvol, vxBlockVolume<uint2> &resvol ) : m_srcVol(srcvol), m_resVol(resvol)
{
}

/**
 * destructor
 */
Resampler::~Resampler()
{
}


/**
 * cubic B-spline Filter 
 * @param u
 * @return
 */
//float4 Resampler::cubic_bspline( float4 u )
//{
//  float4 a, b, c, d, x, fx;
//
//  if ( (x=u+2.0f) < 0.0f )
//    a = 0.0f;
//  else
//    a = x*x*x;
//
//  if ( (x=u+1.0f) < 0.0f )
//    b = 0.0f;
//  else
//    b = x*x*x;
//
//  if ( (x=u) < 0.0f )
//    c = 0.0f;
//  else
//    c = x*x*x;
//
//  if ( (x=u-1.0f) < 0.0f )
//    d = 0.0f;
//  else
//    d = x*x*x;
//
//  fx = One_Sixth*(a - 4.0f*b + 6.0f*c - 4.0f*d);
//
//  return fx;
//
//} // cubic_bspline


#define	BoxSupport		(0.5)

/**
 * box filter 
 * @param t
 * @return
 */
float4 Resampler::Box_filter( float4 t)
{
  if ((t > -0.5f) && (t <= 0.5f)) return(1.0f);
  return(0.0f);
} // Box_filter

#define	LinearSupport	(1.0)

/**
 * linear filter
 * @param t
 * @return
 */
float4 Resampler::Linear_filter( float4 t)
{
  if (t < 0.0f) t = -t;
  if (t < 1.0f) return(1.0f - t);
  return(0.0f); 
} // Linear_filter

#define	MIPSupport	(1.0)

/**
 * MIP filter
 * @param t
 * @return
 */
float4 Resampler::MIP_filter( float4 t)
{
  if (t < 0.0f) t = -t;
  if (t <= 1.0f) return(1.0f);
  return(0.0f);
} // MIP_filter

#define	AVGSupport	(1.0)

/**
 * average filter
 * @param t
 * @return
 */
float4 Resampler::AVG_filter( float4 t)
{
  if (t < 0.0f) t = -t;
  if (t <= 1.0f) return(0.5f);
  return(0.0f);
} // AVG_filter

#define	CBSplineSupport	(2.0)

/** 
 * Cubic B-Spline filter
 * @param t
 * @return
 */
float4 Resampler::CBSpline_filter( float4 t)  /* box (*) box (*) box (*) box */
{
  float4 tt;

  if (t < 0.0f) t = -t;
  if (t < 1.0f)
  {
    tt = t * t;
    return((.5f * tt * t) - tt + (2.0f / 3.0f));
  }
  else if (t < 2.0f)
  {
    t = 2.0f - t;
    return((1.0f / 6.0f) * (t * t * t));
  }
  return(0.0f);
} // CBSpline_filter

#define	SincSupport	(2.0)

/**
 * Sinc filter
 * @param x
 * @return
 */
float4 Resampler::Sinc_filter( float4 x)
{
  x *= static_cast<float4>(M_PI);
  if (x != 0.0f) return(sin(x) / x);
  return(1.0f);
} // Sinc_filter


/**
 * set filter
 * @param filter
 */
void Resampler::SetResamplingFilter( FilterType filter ) 
{
  switch ( filter )
  {
  case BOX:
    m_filterSupport = BoxSupport;
    break;
  case LINEAR:
    m_filterSupport = LinearSupport;
    break;
  case CBSPLINE:
    m_filterSupport = CBSplineSupport;
    break;
  case SINC:
    m_filterSupport = SincSupport;
    break;
  case MIP:
    m_filterSupport = MIPSupport;
    break;
  case AVG:
    m_filterSupport = AVGSupport;
    break;
  }

  m_filtertype = filter;  
} // SetResamplingFilter



/**
 * resample
 * @param iPnt
 * @param delta
 * @return
 */
//float4 Resampler::Resample( Point<int2> iPnt, Point<float4> delta )
//{
//  float4 sum = 0.0f, rw, rv, ru;;
//  Point<uint2> pos;
//
//
//  vxBlockVolumeIterator<uint2> srcIter( m_srcVol );
//
//  for ( int2 w=-1; w<3; w++ )
//  {
//    rw = cubic_bspline( -1.0f*(w - delta.m_z) );
////  rw = 1.0; int2 w =0;
//    pos.m_z = iPnt.m_z + w;
//
//    for ( int2 v=-1; v<3; v++ )
//    {
//      rv = cubic_bspline( -1.0f*(v - delta.m_y) );
//      pos.m_y = iPnt.m_y + v;
//
//      for ( int2 u=-1; u<3; u++ )
//      {
//        ru = cubic_bspline( u - delta.m_x );
//        pos.m_x = iPnt.m_x + u;
//
//
//        srcIter.SetPos( Vector3D<int4> ( pos.m_x, pos.m_y, pos.m_z) );
//        sum += ( srcIter.GetVoxel() * ru * rv *rw );
//
//      }
//    }
//  }
//
//  return sum;
//} // Resample


/**
 * calculate filter contribute
 * @param srcDim
 * @param resDim
 * @param zoomDir
 */
void Resampler::CalContribute( Triplei srcDim, Triplei resDim, ZoomDirType zoomDir ) 
{
  CLIST *contribList;
  const float4 fwidth = m_filterSupport;

  int2 size, limit, orgpnt;
  float4 fOrgPntDecimal;
  float4 fscale;

  switch ( zoomDir )
  {
  case XZoom:
    fscale = m_r2sScale.m_x;
    size = resDim.m_x;
    limit  = srcDim.m_x;
    orgpnt = (int2)(m_srcOrgPnt.m_x);
    fOrgPntDecimal = m_srcOrgPnt.m_x - orgpnt;
    break;
  case YZoom:
    fscale = m_r2sScale.m_y;
    size = resDim.m_y;
    limit  = srcDim.m_y;
    orgpnt = (int2)(m_srcOrgPnt.m_y);
    fOrgPntDecimal = m_srcOrgPnt.m_y - orgpnt;
    break;
  case ZZoom:
    fscale = m_r2sScale.m_z;
    size = resDim.m_z;
    limit  = srcDim.m_z;
    orgpnt = (int2)(m_srcOrgPnt.m_z);
    fOrgPntDecimal = m_srcOrgPnt.m_z - orgpnt;
    break;
  }
  
  m_contribList = (CLIST *)calloc( size, sizeof(CLIST) );
  contribList = m_contribList;
  if ( fscale <= 1.0 )  //zoom in 
  {
    for (int2 i=0; i<size; i++ )
    {
      float4 weight; 
      float4 center = i * fscale + fOrgPntDecimal;
      int2 xstart = ceil( center - fwidth );
      int2 xend = floor( center+fwidth ) + 1;
      int2 nj;

      contribList[i].num= xend-xstart;
      contribList[i].kernelP = (CONTRIB *)calloc( xend-xstart, sizeof(CONTRIB) );
      float4 fWeightSum = 0.0f;
      for ( int2 j=xstart, p=0; j<xend; j++, p++)
      {
        float4 x = center - j;
        switch ( m_filtertype )
        {
        case BOX:
          weight = Box_filter( x );
          break;
        case LINEAR:
          weight = Linear_filter( x );
          break;
        case CBSPLINE:
          weight = CBSpline_filter( x );
          break;
        case SINC:
          weight = Sinc_filter( x );
          break;
        case AVG:
          weight = AVG_filter( x );
          break;
        case MIP:
          weight = MIP_filter( x );
          break;
        }

        nj = j + orgpnt;
        if ( nj < 0 )
          nj = -nj; //0;
        else if ( nj > limit-1 )
          nj =  ( limit - nj ) + limit - 2; //limit-1;

        contribList[i].kernelP[p].voxelInSrc = nj;
        contribList[i].kernelP[p].weight = weight;   
        fWeightSum += weight;
      }
      for( int2 p=0; p<xend-xstart; ++p)
      {
        contribList[i].kernelP[p].weight /= fWeightSum;
      }

    }
  }
  else //zoom out
  {
    float4 xfwidth = fwidth*fscale;
    float4 xfscale = fscale;

    for (int2 i=0; i<size; i++ )
    {
      float4 weight; 
      float4 center = i * fscale + fOrgPntDecimal;
      int2 xstart = ceil( center - xfwidth );
      int2 xend = floor( center+xfwidth ) + 1;
      int2 nj;

      contribList[i].num= xend-xstart;
      contribList[i].kernelP = (CONTRIB *)calloc( xend-xstart, sizeof(CONTRIB) );
      float4 fWeightSum = 0.0f;
      for ( int2 j=xstart, p=0; j<xend; j++, p++)
      {
        float4 x = (center - j)/xfscale;
        switch ( m_filtertype )
        {
        case BOX:
          weight = Box_filter( x );
          break;
        case LINEAR:
          weight = Linear_filter( x );
          break;
        case CBSPLINE:
          weight = CBSpline_filter( x );
          break;
        case SINC:
          weight = Sinc_filter( x );
          break;
        case MIP:
          weight = MIP_filter( x );
          break;
        case AVG:
          weight = AVG_filter( x );
          break;
        }

        nj = j + orgpnt;
        if ( nj < 0 )
          nj = -nj; //0;
        else if ( nj > limit-1 )
          nj =  ( limit - nj ) + limit - 2; //limit-1;

        contribList[i].kernelP[p].voxelInSrc = nj;
        contribList[i].kernelP[p].weight = weight; //weight/xfscale;   
        fWeightSum += weight;

        //if ( i > size - 10 )
        //{
        //  LogDbg(" scale :"+ToAscii(fscale)+" , kernel "+ToAscii(p)+" voxel :"+ToAscii(nj+orgpnt)+" , weight : "+ToAscii(weight), "test", "test" );
        //}
      }

      for( int2 p=0; p<xend-xstart; ++p)
      {
        contribList[i].kernelP[p].weight /= fWeightSum;
      }

    }
  }

} // CalContribute


/**
 * zoom along X - X direction resampling
 * @param srcVol
 * @param resVol
 */
void Resampler::ZoomX(vxBlockVolume<uint2> &srcVol, vxBlockVolume<uint2> &resVol )
{
  CLIST *contrib = m_contribList;
  vxBlockVolumeIterator<uint2> srcIter( srcVol );
  vxBlockVolumeIterator<uint2> resIter( resVol );

  Triple<int4> srcVolDim = srcVol.GetHeader().GetVolDim();
  Triple<int4> srcDim( srcVolDim.X(), srcVolDim.Y(), srcVolDim.Z() );
  Triple<int4> resDim = resVol.GetHeader().GetVolDim();

  Vector3D<int4> srcpos, respos;
  Point<float4> orgPnt = m_srcOrgPnt;
  float4 delk = m_r2sScale.m_z;
  float4 delj = m_r2sScale.m_y;
  float4 deli = m_r2sScale.m_x;
  int4 k, j, i, p;

  const int2 len = m_srcVol.GetHeader().GetVolDim().X();
  //int2 boundary = (int2)(m_filterSupport)+1;
  int4 *buf, *bufP;

  buf = (int4 *)calloc(len+1, sizeof(int4) );

  for ( k=0; k<resDim.Z(); k++)
  {
    srcpos.m_z = (int2)(orgPnt.m_z + k);
    respos.m_z = k;

    for ( j=0; j<resDim.Y(); j++)
    {
      srcpos.m_y = (int2)(orgPnt.m_y + j);
      respos.m_y = j;

      //get one row;
      srcpos.m_x = 0;
      srcIter.SetPos( srcpos );
      bufP = &(buf[0]);
      for ( p=0; p<srcDim.X(); ++p, srcIter.IncX(), ++bufP ) // maybe we donot need the whole line 
      {
        //int2 voxel = srcIter.GetVoxel();
        ////buf[p] = (voxel<0?0:voxel);
        //buf[p] = voxel;
        *bufP = (int4)(srcIter.GetVoxel());
      }

      //convolve one row
      respos.m_x = 0;
      resIter.SetPos( respos );
      if ( m_filtertype != MIP )
      {
        for (i=0; i<resDim.X(); i++, resIter.IncX() )
        {
          float4 sum = 0.0;
          for ( int2 u=0; u<contrib[i].num; u++)
            sum += buf[ contrib[i].kernelP[u].voxelInSrc ] * contrib[i].kernelP[u].weight; 
          resIter.SetVoxel( (uint2)(sum+0.5) );
        }
      }
      else
      {
        for (i=0; i<resDim.X(); i++, resIter.IncX() )
        {
          float4 mip = 0.0, mipvoxel;
          for ( int2 u=0; u<contrib[i].num; u++)
          {
            if ( contrib[i].kernelP[u].weight > 1.0e-5 )
              mipvoxel = buf[ contrib[i].kernelP[u].voxelInSrc ] ;
            if ( mip < mipvoxel )
              mip = mipvoxel;
          }
          resIter.SetVoxel( (uint2)(mip+0.5) );
        }
      }      
    }
  }

} // ZoomX


/**
 * zoom along Y - y direction resampling
 * @param srcVol
 * @param resVol
 */
void Resampler::ZoomY( vxBlockVolume<uint2>  &srcVol, vxBlockVolume<uint2> &resVol )
{
  CLIST *contrib = m_contribList;
  vxBlockVolumeIterator<uint2> srcIter( srcVol );
  vxBlockVolumeIterator<uint2> resIter( resVol );

  Triple<int4> srcDim = srcVol.GetHeader().GetVolDim();
  Triple<int4> resdim = resVol.GetHeader().GetVolDim();

  Vector3D<int4> srcpos, respos;
  Point<float4> orgPnt = m_srcOrgPnt;
  float4 delk = m_r2sScale.m_z;
  int4 k, j, i, p;

  const int2 len = m_srcVol.GetHeader().GetVolDim().Y();
  const int2 srcOrg = (int2)(orgPnt.m_y);
  //int2 boundary = (int2)(m_filterSupport)+1;
  int4 *buf, *bufP;

  buf = (int4 *)calloc(len+1, sizeof(int4) );

  for ( k=0; k<resdim.Z(); k++)
  {
    srcpos.m_z = (int2)(orgPnt.m_z + k);;
    respos.m_z = k;

    for ( j=0; j<resdim.X(); j++)
    {
      srcpos.m_x = j;
      respos.m_x = j;

      //get one row;
      srcpos.m_y = 0;
      srcIter.SetPos( srcpos );
      bufP = &(buf[srcOrg]);
      for ( p=0; p<srcDim.Y(); ++p, ++bufP, srcIter.IncY() )
      {
        *bufP = (int4)(srcIter.GetVoxel());
      }

      //convolve one row
      respos.m_y = 0;
      resIter.SetPos( respos );
      if ( m_filtertype != MIP )
      {
        for (i=0; i<resdim.Y(); i++, resIter.IncY() )
        {
          float4 sum = 0.0;
          for ( int2 u=0; u<contrib[i].num; u++)
            sum += buf[ contrib[i].kernelP[u].voxelInSrc ] * contrib[i].kernelP[u].weight;

          resIter.SetVoxel( (uint2)(sum+0.5) );
        }
      }
      else
      {
        for (i=0; i<resdim.Y(); i++, resIter.IncY() )
        {
          float4 mip = 0.0, mipvoxel;
          for ( int2 u=0; u<contrib[i].num; u++)
          {
            if ( contrib[i].kernelP[u].weight > 1.0e-5 )
              mipvoxel = buf[ contrib[i].kernelP[u].voxelInSrc ];
            if ( mip < mipvoxel )
              mip = mipvoxel;
          }
          resIter.SetVoxel( (uint2)(mip+0.5) );
        }

      }

    }
  }

} // ZoomY


/**
 * zoom along Z - Z direction resampling
 * @param srcVol
 * @param resVol
 */
void Resampler::ZoomZ(vxBlockVolume<uint2> &srcVol, vxBlockVolume<uint2> &resVol )
{
  CLIST *contrib = m_contribList;
  vxBlockVolumeIterator<uint2> srcIter( srcVol );
  vxBlockVolumeIterator<uint2> resIter( resVol );


  Triple<int4> srcDim = srcVol.GetHeader().GetVolDim();
  Triple<int4> resDim = resVol.GetHeader().GetVolDim(); 

  Vector3D<int4> srcpos, respos;;
  Point<float4> orgPnt = m_srcOrgPnt;
  int4 k, j, i, p;

  const int2 len = m_srcVol.GetHeader().GetVolDim().Z();
  const int2 srcOrg = (int2)(orgPnt.m_z);
  int4 *buf, *bufP;

  buf = (int4 *)calloc(len+1, sizeof(int4) );

  for ( k=0; k<resDim.Y(); k++)
  {
    srcpos.m_y = k;
    respos.m_y = k;

    for ( j=0; j<resDim.X(); j++)
    {
      srcpos.m_x = j;
      respos.m_x = j;

      //get one row;
      srcpos.m_z = 0;
      srcIter.SetPos( srcpos );
      bufP = &(buf[srcOrg]);
      for ( p=0; p<srcDim.Z(); ++p, ++bufP, srcIter.IncZ() )
      {
        *bufP = (int4)(srcIter.GetVoxel());
      }


      //convolve one row
      respos.m_z = 0;
      resIter.SetPos( respos );
      if ( m_filtertype != MIP )
      {
        for (i=0; i<resDim.Z(); i++, resIter.IncZ() )
        {
          float4 sum = 0.0;
          for ( int2 u=0; u<contrib[i].num; u++)
            sum += buf[ contrib[i].kernelP[u].voxelInSrc ] * contrib[i].kernelP[u].weight;

          resIter.SetVoxel( (uint2)(sum+0.5) );          
        }
      }
      else
      {
        for (i=0; i<resDim.Z(); i++, resIter.IncZ() )
        {
          float4 mip = 0.0, mipvoxel;
          for ( int2 u=0; u<contrib[i].num; u++)
          {
            if ( contrib[i].kernelP[u].weight > 1.0e-5 )
              mipvoxel = buf[ contrib[i].kernelP[u].voxelInSrc ];
            if ( mip < mipvoxel )
              mip = mipvoxel;
          }

          resIter.SetVoxel( (uint2)(mip+0.5) );
        }
      }

    }
  }

} // ZoomZ


/**
 * RunZoom - resmapling volume 
 */
void Resampler::RunZoom()
{
  Triple<float4> srcunits(m_srcVol.GetHeader().GetVoxelUnits());
  Triple<float4> resunits(m_resVol.GetHeader().GetVoxelUnits());

  Triple<float4> scale, invscale;
  scale.m_x = srcunits.m_x / resunits.m_x;
  scale.m_y = srcunits.m_y / resunits.m_y;
  scale.m_z = srcunits.m_z / resunits.m_z;

  invscale.m_x = 1.0/scale.m_x;
  invscale.m_y = 1.0/scale.m_y;
  invscale.m_z = 1.0/scale.m_z;

  m_s2rScale = scale;
  m_r2sScale = invscale;

  Triple<int4> srcdim(m_srcVol.GetHeader().GetVolDim());
  Triple<int4> resdim(m_resVol.GetHeader().GetVolDim()); 

  Point<float4> center = m_center;
  float4 zlen((resdim.m_z-1)*invscale.m_z);
  float4 ylen((resdim.m_y-1)*invscale.m_y);
  float4 xlen((resdim.m_x-1)*invscale.m_x);
  float4 zorg(center.m_z - 0.5*zlen);
  float4 yorg(center.m_y - 0.5*ylen);
  float4 xorg(center.m_x - 0.5*xlen);
  m_srcOrgPnt.m_x = xorg;
  m_srcOrgPnt.m_y = yorg;
  m_srcOrgPnt.m_z = zorg;

  Triple<int4> tmpdim( (int4)(xlen)+1, (int4)(ylen)+1, (int4)(zlen)+1 );
  Triple<float4> tmpunits( srcunits );

  vxBlockVolume<uint2> xzoomResVol( m_srcVol.GetHeader() );
  tmpdim.m_x   = resdim.m_x;
  tmpunits.m_x = resunits.m_x;
  xzoomResVol.GetHeader().SetVolDim( tmpdim );
  xzoomResVol.GetHeader().SetVoxelUnits( tmpunits );

  vxBlockVolume<uint2> yzoomResVol( m_srcVol.GetHeader() );
  tmpdim.m_y   = resdim.m_y;
  tmpunits.m_y = resunits.m_y;
  yzoomResVol.GetHeader().SetVolDim( tmpdim );
  yzoomResVol.GetHeader().SetVoxelUnits( tmpunits );

  vxBlockVolume<uint2> & zzoomResVol(m_resVol);

  CalContribute( m_srcVol.GetHeader().GetVolDim(), xzoomResVol.GetHeader().GetVolDim(), XZoom );
  ZoomX( m_srcVol, xzoomResVol );
  for ( int2 i(0); i<resdim.m_x; i++ )
    free( m_contribList[i].kernelP);
  free(m_contribList);
  //xzoomResVol.Write("D:\\ds\\tmp\\Resampled-X.vol" );


  CalContribute( xzoomResVol.GetHeader().GetVolDim(), yzoomResVol.GetHeader().GetVolDim(), YZoom ); 
  ZoomY( xzoomResVol, yzoomResVol );
  for ( int2 i(0); i<resdim.m_y; i++ )
    free( m_contribList[i].kernelP);
  free(m_contribList);
  //yzoomResVol.Write("D:\\ds\\tmp\\Resampled-Y.vol" );


  CalContribute( yzoomResVol.GetHeader().GetVolDim(), m_resVol.GetHeader().GetVolDim(), ZZoom );
  ZoomZ( yzoomResVol, m_resVol );
  for ( int2 i(0); i<resdim.m_z; i++ )
    free( m_contribList[i].kernelP);
  free(m_contribList);

  //m_resVol.Write("D:\\ds\\tmp\\Resampled-Z.vol" );

  //check the value and remove the highest value
  //const int2 iMinVoxel(0);
  //const int2 iMaxVoxel(4095);
  //vxBlockVolumeIterator<uint2> resVolIter( m_resVol );
  //for ( resVolIter.SetPos(0,0,0); resVolIter.IsNotAtEnd(); resVolIter.NextBlockZYX() )
  //for ( ; resVolIter.IsNotAtEndOfBlock();  resVolIter.NextXZYinsideBlock() )
  //{
  //  int2 voxelInVol(resVolIter.GetVoxel());
  //  int2 voxel = Bound(iMinVoxel, voxelInVol, iMaxVoxel);
  //  if ( voxel != voxelInVol )
  //    resVolIter.SetVoxel( voxel );
  //}

} // RunZoom


/**
 * MIP map run
 * @param srcVol - original volume
 * @param scaleVol - result volume
 * @param level -  level in MIP map 
 */
void Resampler::MipMapRun( vxBlockVolume<uint2> & srcVol, vxBlockVolume<uint2> & scaleVol, int2 level )
{
  
  Triple<float4> srcunits = srcVol.GetHeader().GetVoxelUnits();
  float4  minUnit = Min( srcunits.X(), srcunits.Y(), srcunits.Z() );  

  int4 levelExp = 1;
  for (int4 i=0; i<level; i++)
    levelExp *=2;
  float4 scale = 1.0f/levelExp;

  minUnit /= scale; 
  Triple<float4> scaleunits( minUnit, minUnit, minUnit );

  Triple<int4> srcDim = srcVol.GetHeader().GetVolDim();
  Triple<int4> dim;
  dim.m_x = (int4)( (srcDim.X()-1)*srcunits.X()/minUnit + 1.0f );
  dim.m_y = (int4)( (srcDim.Y()-1)*srcunits.Y()/minUnit + 1.0f );
  dim.m_z = (int4)( (srcDim.Z()-1)*srcunits.Z()/minUnit + 1.0f );

  Point<float4> center;
  center.m_x = 0.5f*(srcDim.X()-1);
  center.m_y = 0.5f*(srcDim.Y()-1);
  center.m_z = 0.5f*(srcDim.Z()-1);

  //scaleVol.SetHeader( srcVol.GetHeader() );
  scaleVol.GetHeader().SetVolDim( dim );
  scaleVol.GetHeader().SetVoxelUnits( scaleunits );
  scaleVol.Clear();

  LogDbg( " mip level = " + ToAscii(level), "Resampler", "MipMapRun" );
  LogDbg( " units = " + ToAscii( scaleunits ), "Resampler", "MipMapRun" );
  LogDbg( " size = " + ToAscii( dim ), "Resampler", "MipMapRun" );

  Resampler scaler( srcVol, scaleVol);
  scaler.SetResamplingCenter( center ); 
  scaler.SetResamplingFilter( CBSPLINE );  //or AVG
  scaler.RunZoom();
} // MipMapRun


/** 
 * run Box-cut - resample the volume in the Box
 * @param pIntensityVol - original volume
 * @param pCutVol - resuklt volume
 * @param startPnt - topleft point
 * @param endPnt - down right point
 * @param scaleFactor - scaling factor
 */
void Resampler::BoxCutRun( vxBlockVolume<uint2> *pIntensityVol, vxBlockVolume<uint2> *pCutVol, Point<float4> startPnt, Point<float4> endPnt, float4 scaleFactor )
{
  Timer timer;

  timer.Reset(); // start the clock

  Triple<int4> srcDim = pIntensityVol->GetHeader().GetVolDim();
  Triple<float4> srcunits = pIntensityVol->GetHeader().GetVoxelUnits();
  float4  minUnit = Min( srcunits.m_x, srcunits.m_y, srcunits.m_z );  

  minUnit /= scaleFactor; // the sacling voxel units
  Triple<float4> scaleunits = Triple<float4>( minUnit, minUnit, minUnit );

  Triple<float4> fCutVolSize;
  fCutVolSize.m_x = fabs( endPnt.m_x - startPnt.m_x )*srcunits.m_x/minUnit;
  fCutVolSize.m_y = fabs( endPnt.m_y - startPnt.m_y )*srcunits.m_y/minUnit;
  fCutVolSize.m_z = fabs( endPnt.m_z - startPnt.m_z )*srcunits.m_z/minUnit;

  Point<float4> center;  
  center.m_x = 0.5f*(endPnt.m_x + startPnt.m_x);
  center.m_y = 0.5f*(endPnt.m_y + startPnt.m_y);
  center.m_z = 0.5f*(endPnt.m_z + startPnt.m_z);

  Triple<int4> cutVolDim = Triple<int4> ( fCutVolSize.m_x+1.0f, fCutVolSize.m_y+1.0f, fCutVolSize.m_z+1.0f );
  Triple<float4> cutVolUnits = scaleunits;
  pCutVol->SetHeader( pIntensityVol->GetHeader() );
  pCutVol->GetHeader().SetVolDim( cutVolDim );
  pCutVol->GetHeader().SetVoxelUnits( cutVolUnits );
  //pCutVol->Clear();

  LogDbg("Volume Cut startPoint = " + startPnt.ToString() + " , endPoint = " + endPnt.ToString(), "Resampler", "BoxCutRun");
  LogDbg("Original Volume Units = " + srcunits.ToString() + " , cutting volume units = " + cutVolUnits.ToString(), "Resampler", "BoxCutRun");

  Resampler scaler( *pIntensityVol, *pCutVol );


  scaler.SetResamplingCenter( center ); 
  scaler.SetResamplingFilter( CBSPLINE );
  scaler.RunZoom();

  LogDbg("resampling is finished!", "Resampler", "BoxCutRun");
  center.m_x *= srcunits.m_x;
  center.m_y *= srcunits.m_y;
  center.m_z *= srcunits.m_z;

  LogDbg("center is " + center.ToString(), "Resampler", "BoxCutRun");

  VarMap varmap ( pCutVol->GetHeader().GetVariableVolumeHeader() );  
  Point<float4> centerWorldPos( center );
  varmap.SetHdrVar( "VolumeCenter", centerWorldPos, "The world position of the volume center (mm) " );

  //Point<float4> refercentPnt;
  //varmap.GetHdrVar( "VolumeCenter", refercentPnt );

  LogFyi( timer.AsString() + " -- Detail Volume Resampling is finished. ", "void", "BoxCutRun" );

  pCutVol->GetHeader().SetVariableVolumeHeader(varmap);
} // BoxCutRun

/**
 * run scale - resampling the volume in terms of the unit setting. 
 * @param srcVol - orignal volume
 * @param scaleVol - result volume
 */
void Resampler::GetIsotropicVolume( vxBlockVolume<uint2> * pSrcVol, vxBlockVolume<uint2> * pResVol, float4 fIsoVoxelUnit )
{
  Timer timer;

  timer.Reset(); // start the clock

  const Triple<int4> srcDim ( pSrcVol->GetHeader().GetVolDim() );
  const Triple<float4> srcUnits( pSrcVol->GetHeader().GetVoxelUnits() );
  
  LogDbg(" srcdim = "+srcDim.ToString()+" ; srcUnits = "+srcUnits.ToString(), " " , " " );

  const Triple<float4> resUnits( fIsoVoxelUnit, fIsoVoxelUnit, fIsoVoxelUnit );

  Triple<int4> resDim;
  resDim.m_x = (int4)( ( srcDim.X() - 1 )*srcUnits.X()/fIsoVoxelUnit + 1.0000000001F );
  resDim.m_y = (int4)( ( srcDim.Y() - 1 )*srcUnits.Y()/fIsoVoxelUnit + 1.0000000001F );
  resDim.m_z = (int4)( ( srcDim.Z() - 1 )*srcUnits.Z()/fIsoVoxelUnit + 1.0000000001F );
  
  LogDbg(" resDim = "+resDim.ToString(), " " , " " );

  Point<float4> center;  
  center.m_x = 0.5F*(resDim.X()-1)*fIsoVoxelUnit/srcUnits.X();
  center.m_y = 0.5F*(resDim.Y()-1)*fIsoVoxelUnit/srcUnits.Y();
  center.m_z = 0.5F*(resDim.Z()-1)*fIsoVoxelUnit/srcUnits.Z();

  pResVol->SetHeader( pSrcVol->GetHeader() );
  pResVol->GetHeader().SetVolDim( resDim );
  pResVol->GetHeader().SetVoxelUnits( resUnits );

  LogDbg("Original Volume Units = " + srcUnits.ToString() + " , isotropic volume units = " + resUnits.ToString(), "Resampler", "GetIsotropicVolume");
  LogDbg("resampling center volume position = " + center.ToString(), "Resampler", "GetIsotropicVolume" );


  Resampler scaler( *pSrcVol, *pResVol );

  scaler.SetResamplingCenter( center ); 
  scaler.SetResamplingFilter( CBSPLINE );
  scaler.RunZoom();
  
  center.m_x *= srcUnits.m_x;
  center.m_y *= srcUnits.m_y;
  center.m_z *= srcUnits.m_z;

  LogDbg("center world position = " + center.ToString(), "Resampler", "BoxCutRun");

  VarMap varmap ( pResVol->GetHeader().GetVariableVolumeHeader() );  
  Point<float4> centerWorldPos( center );
  varmap.SetHdrVar( "VolumeCenter", centerWorldPos, "The world position of the volume center (mm) " );

  LogFyi( timer.AsString() + " -- Detail Volume Resampling is finished. ", "void", "BoxCutRun" );

  pResVol->GetHeader().SetVariableVolumeHeader(varmap);
 
  
} // GetIsotropicVolume()

#undef FILE_REVISION





// $Log: Resampler.C,v $
// Revision 1.2  2005/07/01 12:32:02  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2004/03/02 01:13:53  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.16.4.1  2004/03/01 22:41:43  wenli
// Add GetIsotropicVolume()
//
// Revision 1.16  2003/06/24 20:35:10  wenli
// 16Bits (uint2) resampling -- removing the Bound(0, .., 4095)
//
// Revision 1.15  2003/06/18 15:19:51  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.14  2003/06/09 16:17:16  michael
// fixed compile bug (typo)
//
// Revision 1.13  2003/05/22 17:41:29  wenli
// 2nd code review
//
// Revision 1.12  2003/05/22 16:35:08  dongqing
// code review
//
// Revision 1.11  2003/05/16 11:45:00  michael
// code review
//
// Revision 1.10  2003/04/02 21:32:06  wenli
// fix the buffer size in ZoomX, ZoomY, and ZoomZ
//
// Revision 1.9  2003/04/02 19:13:50  wenli
// MipMap volume size is set
//
// Revision 1.8  2003/04/01 17:08:25  wenli
// MipMap volume size is set
//
// Revision 1.7  2003/03/26 20:02:39  wenli
// removed the grid that sometimes appeared in the detail volume
//
// Revision 1.6  2003/03/25 16:56:38  wenli
// the sampling at the boundary of the volume
//
// Revision 1.5  2003/03/13 17:15:44  wenli
// float volume resample and boundary fixed
//
// Revision 1.4  2003/01/22 21:40:49  ingmar
// dimension and units are now Triples
//
// Revision 1.3  2002/11/08 16:05:51  ingmar
// removed obsolete #define
// code beautify
//
// Revision 1.2  2002/11/05 18:57:12  frank
// Moved from CTA plugin
//
// Revision 1.2  2002/11/05 17:44:18  frank
// Formatting
//
// Revision 1.1  2002/10/31 12:46:51  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.8  2002/10/07 21:41:33  wenli
// scale down vesselness volume to origianl resoution
//
// Revision 1.7  2002/10/01 22:30:52  wenli
// remove the linearvolume and blockvolume data structures
//
// Revision 1.6  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.5  2002/08/23 15:25:09  wenli
// initial version - Vascular
//
// Revision 1.4  2002/08/22 18:01:47  wenli
// initial version - Vascular
//
// Revision 1.3  2002/08/21 23:28:18  wenli
// initial version - Vascular
//
// Revision 1.2  2002/08/14 16:12:09  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/12 15:52:58  wenli
// initial version - Vascular
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/Resampler.C,v 1.2 2005/07/01 12:32:02 vxconfig Exp $
// $Id: Resampler.C,v 1.2 2005/07/01 12:32:02 vxconfig Exp $


