// $Id: Resampler.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright (c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wenli Cai, wenli@viatronix.com

/**
 * define the re-sampler class
 */

#ifndef Resampler_h
#define Resampler_h

#include "vxBlockVolume.h"
#include "Point.h"

// Includes
static const float4 One_Sixth = 1.0f / 6.0f;

/// resampler
class VX_VIEWER_DLL Resampler
{
public:

  /// filter type enum
  enum FilterType
  {
    BOX = 0, 
    LINEAR  = 1, 
    CBSPLINE    = 2, 
    SINC  = 3,
    AVG = 4,
    MIP = 5
  };
  
  /// zoom directory type
  enum ZoomDirType
  {
    XZoom = 0,
    YZoom = 1,
    ZZoom = 2
  };


  /// Constructor
  Resampler ( vxBlockVolume<uint2> &srcvol, vxBlockVolume<uint2> &resvol );

  /// Destructor
  virtual ~Resampler();


public: // Member Functions...

  /// set center
  void SetResamplingCenter( Point<float4> center ) { m_center = center; };

  /// set filter
  void SetResamplingFilter( FilterType filter );

  /// zooming
  void RunZoom();

  /// how to use it 
  static void MipMapRun( vxBlockVolume<uint2> &srcVol, vxBlockVolume<uint2> &dstVol, int2 level );
  /// how to use it 
  static void BoxCutRun( vxBlockVolume<uint2> *pSrcVol, vxBlockVolume<uint2> *pCutVol, Point<float4> startPnt, Point<float4> endPnt, float4 scaleFactor );
  /// how to use it
  static void GetIsotropicVolume( vxBlockVolume<uint2> * pSrcVol, vxBlockVolume<uint2> * pResVol, float4 fIsoVoxelUnit );


private:

  /// resample
  //float4 Resample( Point<int2> iPnt, Point<float4> delta );

  /// cubic B-spline
  //float4 cubic_bspline( float4 x );

  /// box filter
  float4 Box_filter( float4 x );

  /// linear filter
  float4 Linear_filter( float4 x );

  /// MIP filter
  float4 MIP_filter( float4 x );

  /// cubic B-spline filter
  float4 CBSpline_filter( float4 x );

  /// SINC filter
  float4 Sinc_filter( float4 x );

  /// Average filter
  float4 AVG_filter( float4 t);

  /// fill contributor list
  float4 FillContribList();

  /// calculation contribute
  void CalContribute( Triplei srcDim, Triplei resDim, ZoomDirType dir );

  /// Zoom along axises
  void ZoomX(vxBlockVolume<uint2> &srcVol, vxBlockVolume<uint2> &resVol );
  void ZoomY(vxBlockVolume<uint2> &srcVol, vxBlockVolume<uint2> &resVol );
  void ZoomZ(vxBlockVolume<uint2> &srcVol, vxBlockVolume<uint2> &resVol );


private: // Member Variables
  /// data structure CONTRIB
  typedef struct 
  {
    int voxelInSrc;
    float4 weight;
  } CONTRIB;

  /// data structure CLIST
  typedef struct 
  {
    int num;
    CONTRIB *kernelP;
  } CLIST;

  /// source volume
  vxBlockVolume<uint2> &m_srcVol;
  /// reconstruction volume
  vxBlockVolume<uint2> &m_resVol;

  /// center
  Point<float4> m_center;
  /// origine
  Point<float4> m_srcOrgPnt;
  ///  scaler
  Triple<float4> m_s2rScale, m_r2sScale;

  /// filter type
  FilterType m_filtertype;
  /// filter kernal
  float4 m_filterSupport;
  /// filter contributor list
  CLIST *m_contribList;

}; // class Resampler


#endif // #ifndef Resampler_h

// $Log: Resampler.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/03/02 01:13:52  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.9.4.1  2004/03/01 22:41:43  wenli
// Add GetIsotropicVolume()
//
// Revision 1.9  2003/06/18 15:19:51  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.8  2003/05/22 17:41:29  wenli
// 2nd code review
//
// Revision 1.7  2003/05/22 16:35:08  dongqing
// code review
//
// Revision 1.6  2003/05/16 11:45:00  michael
// code review
//
// Revision 1.5  2003/03/25 16:56:38  wenli
// the sampling at the boundary of the volume
//
// Revision 1.4  2003/03/13 17:15:43  wenli
// float volume resample and boundary fixed
//
// Revision 1.3  2003/01/22 21:40:49  ingmar
// dimension and units are now Triples
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
// Revision 1.5  2002/10/07 21:41:33  wenli
// scale down vesselness volume to origianl resoution
//
// Revision 1.4  2002/10/01 22:30:52  wenli
// remove the linearvolume and blockvolume data structures
//
// Revision 1.3  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.2  2002/08/21 23:28:18  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/12 15:52:58  wenli
// initial version - Vascular
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/Resampler.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: Resampler.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $