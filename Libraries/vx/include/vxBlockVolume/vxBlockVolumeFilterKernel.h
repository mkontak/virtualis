// $Id: vxBlockVolumeFilterKernel.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Wenli {ingmar|wenli}@viatronix.com

#ifndef vxBlockVolumeFilterKernel_h
#define vxBlockVolumeFilterKernel_h


// includes
#include "vxBlockVolumeFilter.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"


/**
 * Filter Kernel for resampling volume
 */
class vxBlockVolumeFilterKernel
{
// enumerations
public: 

  /// enumeration for the filter type
  enum FilterType
  {
    BOX      = 0, 
    LINEAR   = 1, 
    CBSPLINE = 2, 
    SINC     = 3
  }; // enum FilterType


// member functions
public:

  /// standard constructor
  vxBlockVolumeFilterKernel();

  /// get the filter kernel
  static void GetKernelCubicBSpline( std::vector< vxBlockVolumeFilterKernel > & vKernel, float4 fScale, int4 iRowSize );


// private member functions
private:

  /// box filter
  float4 BoxFilter( float4 fT );

  /// linear filter
  float4 LinearFilter( float4 fT );

  /// cubic B-spline filter
  float4 CBSplineFilter( float4 fT );

  /// SINC filter
  float4 SincFilter( float4 fT );

  /// setup filter type
  void SetFilterType( FilterType eFilter );

  /// fill the filter kernel
  void FillFilterKernel( std::vector< vxBlockVolumeFilterKernel > & vKernel, float4 fScale, int4 iRowSize );


// publicly accessible data members (bad)
public:

  /// filter coefficients container
  std::vector<float4> m_vfWeight;

  /// index for the first position
  int4 m_ifirstPos;


// private data members
private:

  /// filter type
  FilterType m_eFilterType; 

  /// filter support range
  float4 m_fFilterSupport;

}; // class vxBlockVolumeFilterKernel


#endif // vxBlockVolumeFilterKernel_h


// $Log: vxBlockVolumeFilterKernel.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/22 13:04:28  frank
// code review
//
// Revision 1.4  2003/05/19 13:15:40  dongqing
// add comments
//
// Revision 1.3  2003/05/16 11:48:14  michael
// code review
//
// Revision 1.2  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.1  2002/05/24 20:37:49  wenli
// Cubic BSpline Filter to resample the volume
//
// Revision 1.1  2002/05/22 23:39:15  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeFilterKernel.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxBlockVolumeFilterKernel.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
