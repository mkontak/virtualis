// $Id: RegionSampler.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

#ifndef RegionSampler_h
#define RegionSampler_h


// includes
#include "Point.h"
#include "Normal.h"
#include "Quadrilateral.h"


// forward declarations
template <class T> class vxBlockVolume;


/**
 * Class that encapsulates Region Sampler for image processing.
 */
class RegionSampler
{
// enums
public:

  enum SampleRestrictionEnum { 
    XY_PLANE,
    YZ_PLANE,
    XZ_PLANE,
    OBLIQUE_PLANE,
    SPHERICAL,
  }; // enum SampleRestrictionEnum

// functions
public:
  
  /// default constructor
  RegionSampler();

  /// destructor
  virtual ~RegionSampler() {}

  /// gets the sample radius
  const float4 & GetSampleRadius() const { return m_fSampleRadius; }

  /// set the sample radius
  void SetSampleRadius( const float4 fRadiusMM ) { m_fSampleRadius = fRadiusMM; }

  /// gets the computed threshold range
  const std::pair< uint2, uint2 > & GetThresholdRange() const { return m_uThresholdRange; }

  /// gets the currently active sampling plane
  const enum SampleRestrictionEnum & GetSamplingPlane() const { return m_eSamplePlane; }

  /// gets the seed point
  const Point<uint2> & GetSeedPoint() const { return m_seedPoint; }

  /// gets the oblique quadrilateral representing the sample region on an oblique plane
  const Quadrilateral & GetObliqueQuadrilateral() const { return m_obliqueQuadrilateral; }

  /// sample intensity range for an orthographic MPR plane
  void SampleIntensityRangeOrtho( const Point<uint2> & rSeedPt, vxBlockVolume<uint2> * pScanVol,
                                  const enum SampleRestrictionEnum & planeRestriction );

  /// sample intensity range for an oblique MPR plane
  void SampleIntensityRangeOblique( const Quadrilateral & quadrilateral, vxBlockVolume<uint2> * pScanVol );

  /// sample intensity range for a spherical region
  void SampleIntensityRangeSphere( const Point<uint2> & rSeedPt, vxBlockVolume<uint2> * pScanVol );

private:

  /// sample intensity range centered at seed point
  void SampleIntensityRange( const Point<uint2> & rSeedPt, vxBlockVolume<uint2> * pScanVol );

// data
private:

  /// sample sphere, plane, oblique, XY, YZ, or XZ
  SampleRestrictionEnum m_eSamplePlane;

  /// the computed lower and upper threshold range
  std::pair< uint2, uint2 > m_uThresholdRange;

  /// The definition of the oblique plane sample region
  Quadrilateral m_obliqueQuadrilateral;

  /// the center of an ortho sample region
  Point<uint2> m_seedPoint;

  /// radius of sample area
  float4 m_fSampleRadius;

}; // class RegionSampler


#endif // RegionSampler_h


// $Log: RegionSampler.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2004/01/23 16:30:06  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.18  2003/12/22 16:20:57  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.17  2003/06/19 20:20:06  michael
// adjusted segmentation from int2 to uint2, following the changes in the block volume.
//
// Revision 1.16  2003/06/18 15:19:54  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.15  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.14  2003/06/07 15:36:37  michael
// reformatting
//
// Revision 1.13  2003/05/16 13:20:38  frank
// formatting
//
// Revision 1.12  2003/04/14 21:06:03  michael
// performed initial code review
//
// Revision 1.11  2003/01/03 16:58:33  frank
// Extended region sampling to 3D spherical region.
//
// Revision 1.10  2002/06/21 18:16:02  jaddonisio
// Modified region sample to use different planes(XY,YZ,XZ).
//
// Revision 1.9  2002/06/03 21:23:21  jaddonisio
// Coding standard changes.
//
// Revision 1.8  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.7  2002/05/01 14:06:44  jaddonisio
// Clean-up.
//
// Revision 1.6  2002/04/27 23:15:07  jaddonisio
// Segmentation with new block volumes. Pardon the mess.
//
// Revision 1.5  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.4  2001/12/17 16:38:11  jaddonisio
// i. Changed thresholds to int2 instead of uint2.
// ii. Changed Seta and Get to pass iPropID by value(to fix problem when value is an enum).
//
// Revision 1.3  2001/12/07 23:51:27  jaddonisio
// Corrected function comments for doxygen.
//
// Revision 1.2  2001/12/06 23:54:21  jaddonisio
// More mods for coding standard compliance.
//
// Revision 1.1  2001/11/06 22:41:47  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/RegionSampler.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: RegionSampler.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
