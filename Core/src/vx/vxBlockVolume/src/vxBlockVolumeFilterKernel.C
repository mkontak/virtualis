// $Id: vxBlockVolumeFilterKernel.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Wenli {ingmar|wenli}@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlockVolumeFilterKernel.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"
#define	BoxSupport		  (0.5)
#define	LinearSupport	  (1.0)
#define	CBSplineSupport	(2.0)
#define	SincSupport	    (2.0)


// namespaces
using namespace std;


/**
 * Default constructor
 */
vxBlockVolumeFilterKernel::vxBlockVolumeFilterKernel()
{
} // vxBlockVolumeFilterKernel()


/**
 * Box filter
 * @param fT
 * @return the filtered voxel
 */
float4 vxBlockVolumeFilterKernel::BoxFilter( float4 fT )
{
  if ((fT > -0.5f) && (fT <= 0.5f)) return(1.0f);
  return 0.0f;
} // BoxFilter()


/**
 * Linear filter
 * @param fT
 * @return the filtered voxel
 */
float4 vxBlockVolumeFilterKernel::LinearFilter( float4 fT )
{
  if (fT < 0.0f) fT = -fT;
  if (fT < 1.0f) return(1.0f - fT);
  return(0.0f);
} // LinearFilter()


/**
 * cubic B-spline filter
 * @param t
 * @return the filtered voxel
 */
float4 vxBlockVolumeFilterKernel::CBSplineFilter( float4 fT )
{
  float4 tt;

  if (fT < 0.0f) fT = -fT;
  if (fT < 1.0f)
  {
    tt = fT * fT;
    return((.5f * tt * fT) - tt + (2.0f / 3.0f));
  }
  else if (fT < 2.0f)
  {
    fT = 2.0f - fT;
    return((1.0f / 6.0f) * (fT * fT * fT));
  }
  return(0.0f);
} // CBSplineFilter()


/**
 * SINC filter
 * @param t
 * @return the filtered voxel
 */
float4 vxBlockVolumeFilterKernel::SincFilter( float4 fT )
{
  fT *= static_cast< float4 >( M_PI );
  if (fT != 0.0f) return(sin(fT) / fT);
  return(1.0f);
} // SincFilter()


/**
 * setup filter type
 * @param eFilter
 */
void vxBlockVolumeFilterKernel::SetFilterType( FilterType eFilter )
{

  switch ( eFilter )
  {
  case BOX:      m_fFilterSupport = BoxSupport;      break;
  case LINEAR:   m_fFilterSupport = LinearSupport;   break;
  case CBSPLINE: m_fFilterSupport = CBSplineSupport; break;
  case SINC:     m_fFilterSupport = SincSupport;     break;
  }

  m_eFilterType = eFilter;

} // SetFilterType()


/**
 * fill filter kernel container
 * @param vKernel       the kernel
 * @param fd2sUnitScale the unit scaling
 * @param iRowSize      the row size
 */
void vxBlockVolumeFilterKernel::FillFilterKernel( vector< vxBlockVolumeFilterKernel > & vKernel, float4 fd2sUnitScale, int4 iRowSize) 
{
  float4 fwidth( m_fFilterSupport );
  float4 fScale( fd2sUnitScale );
  FilterType efilter( m_eFilterType );
  vector< vxBlockVolumeFilterKernel > & vKernelList = vKernel;
  vKernelList.clear();

  int4 srcRowSize( iRowSize );
  int4 dstRowSize( (int4)( srcRowSize / fScale + 0.5 ) );

  // zoom in  -- units 
  if ( fScale <= 1.0 )  
  {
    for (int2 i=0; i<dstRowSize; i++ )
    {
      float4 weight; 
      float4 center = i * fScale;
      int2 xstart = ceil( center - fwidth );
      int2 xend = floor( center+fwidth ) + 1;

      vxBlockVolumeFilterKernel thisKernel;
      thisKernel.m_vfWeight.clear();
      thisKernel.m_ifirstPos = xstart;

      for ( int2 j=xstart, p=0; j<xend; j++, p++)
      {
        float4 x = center - j;
        switch ( efilter )
        {
        case BOX:
          weight = BoxFilter( x );
          break;
        case LINEAR:
          weight = LinearFilter( x );
          break;
        case CBSPLINE:
          weight = CBSplineFilter( x );
          break;
        case SINC:
          weight = SincFilter( x );
          break;
        }       
        thisKernel.m_vfWeight.push_back(weight);  
      }
      vKernelList.push_back( thisKernel );
    }
  } // zoom in
  else
  {
    // zoom out
    float4 xfwidth = fwidth*fScale;
    float4 xfscale = fScale;

    for (int2 i=0; i<dstRowSize; i++ )
    {
      float4 weight; 
      float4 center = i * fScale;
      int2 xstart = ceil( center - xfwidth );
      int2 xend = floor( center+xfwidth ) + 1;

      vxBlockVolumeFilterKernel thisKernel;
      thisKernel.m_vfWeight.clear();
      thisKernel.m_ifirstPos = xstart;

      for ( int2 j=xstart, p=0; j<xend; j++, p++)
      {
        float4 x = (center - j)/xfscale;
        switch ( efilter )
        {
        case BOX:
          weight = BoxFilter( x );
          break;
        case LINEAR:
          weight = LinearFilter( x );
          break;
        case CBSPLINE:
          weight = CBSplineFilter( x );
          break;
        case SINC:
          weight = SincFilter( x );
          break;
        }
        thisKernel.m_vfWeight.push_back( weight / xfscale );
      }
      vKernelList.push_back( thisKernel );
    }
  } // zoom out

} // FillFilterKernel


/**
 * fill the vector of a kernel to do the convolvetion. 
 * @param vKernel
 * @param fScale
 * @param iRowSize
 */
void vxBlockVolumeFilterKernel::GetKernelCubicBSpline( vector< vxBlockVolumeFilterKernel > & vKernel, float4 fScale, int4 iRowSize )
{
  vxBlockVolumeFilterKernel thisKernel;

  thisKernel.SetFilterType( CBSPLINE );
  thisKernel.FillFilterKernel( vKernel, fScale, iRowSize );

} // GetKernelCubicBSpline()


#undef FILE_REVISION


// $Log: vxBlockVolumeFilterKernel.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/22 13:04:28  frank
// code review
//
// Revision 1.9  2003/05/19 13:15:40  dongqing
// add comments
//
// Revision 1.8  2003/05/17 18:30:41  michael
// fixed typo
//
// Revision 1.7  2003/05/17 16:19:41  michael
// reformatting
//
// Revision 1.6  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.5  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.4  2003/05/15 15:03:46  frank
// spelling
//
// Revision 1.3  2002/11/12 20:47:35  ingmar
// code beautify
//
// Revision 1.2  2002/11/08 23:24:54  michael
// fixed compiler warning by adding an #ifdef/#endif around #define M_PI
//
// Revision 1.1  2002/05/24 20:37:49  wenli
// Cubic BSpline Filter to resample the volume
//
// Revision 1.1  2002/05/22 23:39:15  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolumeFilterKernel.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxBlockVolumeFilterKernel.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
