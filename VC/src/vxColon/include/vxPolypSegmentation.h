// $Id: vxPolypSegmentation.h,v 1.16.2.2 2009/07/30 19:26:28 kchalupa Exp $
//
// Copyright © 2004-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Dongqing Chen dongqing@viatronix.net


// pragmas
#pragma once


// includes
#include <iostream>
#include <vector>
#include "Global.h"
#include "volume.h"
#include "vxBlockVolume.h"


// namespaces
namespace ReaderLib
{

/** 
 *  This class is to segment polyp region by an user input. The user will use
 *  the mouse to click a voxel on the surface of polyp to the air colonic 
 *  segment that the polyp belongs to. The class should automatically 
 *  segment out the polyp region and label the region in the label volume. The
 *  class will also determine minimum and maximum diameter, height (of a pedunculated
 *  polyp), and volume of the Polyp.
 */
  class vxPolypSegmentation
  {
  // construction
  public:

    /// Constructor
    vxPolypSegmentation( LinVolUC & ecvVolUC, vxBlockVolume<uint2> & vol, 
              vxBlockVolume<uint2> & labelVol, 
              const uint2 & uComponentLabel, vxBlockVolume<uint1> & insVol, 
              LinVolUC & paintVol, const Point<float4> & selectPoint );

	  /// destructor
	  ~vxPolypSegmentation();

  // member functions
  public:

    /// Start the polyp segmentation. The control function
    int4 Run();

    /// reset the paint volume per user request
    void ResetPaintVolPerUserRequest();

	  /// get the maximum diameter of polyp
	  float4 GetMaxDiameter( Point<float4> & vertex01, Point<float4> & vertex02 ) const;

    /// get the volume of polyp in mm^3
    float4 GetVolume() const;

    /// get the volume ROI
    void GetPolypVolumeROI( std::vector< Vector3D<int2> > & viVolROI ) const;

  private:

    /// determine the colonic segment index that the polyp locates in
    bool DetermineColonSegmentIndex();

    /// extract subvolume
    void ExtractSubvolume();

    /// initialize the mask for software tissue and label the associated segment
    bool SetInitialMask();

    ///grow out the nearest lumen segment
    void GrowLumen( const Vector3D<int2> & viSeedOfLumen, const uint1 & uROIlabel, const uint1 & uNewLabel );

    /// extract polyp region
    int4 ExtractPolypRegion();

    /// search for nearest tissue voxel
    void SearchForNearestTissuePos( const Vector3D<int2> & viSeed, std::vector< Vector3D<int2> > & vTissuePointList );

    /// determine all surface point within fSearchRadius
    void ComputeInitialSurfacePos( std::vector< Vector3D<int2> > & viInitialSurfacePos );

    /// ray detection algorithm for the polyp surface patch
    void ComputingPolypSurfacePatch( const std::vector< Vector3D<int2> > & viInitialSurfacePos,
                        std::vector<uint1> & viPatchIndicator, const Vector3D<int2> & rayOrigin, 
                        const float & fRayLength, const uint1 & iNewIndicatorValue  );

    /// computing the enclosed region by line filling from surface patch
    void ComputeEncloseROIbyLineFilling( const std::vector< Vector3D<int2> > & viSurfacePos,
                                         const uint1 & uPolypLabel );

    /// compute the necessary measurements for the polyp features
    bool ComputePolypMeasurements();

    /// grow out the top surface patch. This will get rid of surface on the other side of big fold
    int4 GrowOutTopSurfacePatch( std::vector< Vector3D<int2> > & viInitialSurfacePos, 
               std::vector<uint1> & viPatchIndicator, const Vector3D<int2> & closestTissuePoint );

    /// Curve surface thinning algorithm for determine the center of the polyp patch
    bool DeterminePolypPatchCenter( Vector3D<int2> & viPolypPatchCenter, const std::vector<Vector3D<int2>> & viInitialSurfacePos, 
                                    const std::vector<uint1> & viPatchIndicator );

    /// general 6-connected region grow
    void RegionGrow6NB( const Vector3D<int2> & viSeedOfLumen, 
                          const uint1 & uROIlabel, const uint1 & uNewLabel );


  // member variables
  private: 	

    /// 8bit ECV volume for 3D rendering
    LinVolUC &  m_ecvVol;

    /// intensity (w/o cleansed) volume
    vxBlockVolume<uint2> & m_vol;

	  /// ins volume
    vxBlockVolume<uint1> & m_insVol;

    /// label volume for storing info of ROI
    vxBlockVolume<uint2> & m_labelVol;

	  /// paint volume for polyp region labeling
	  LinVolUC & m_paintVol;

	  /// extracted sub-volume that encloses the polyp
	  vxBlockVolume<uint2> * m_pSubVol;

	  /// sub-mask
    vxBlockVolume<uint1> * m_pmask;

	  /// user selected voxel in world coordinate
	  Point<float4> m_fUserInputPoint;

    /// user selected point in volume coordinate
    Vector3D<int2> m_iCenterOfSubvolume;

    /// tissue point that is most close to the user selected point in sub-volume coordinate
    Vector3D<int2> m_iClosestTissuePoint;

	  /// maximum diameter
	  Point<float4> m_vfMaxDiameterEnd1;
	  Point<float4> m_vfMaxDiameterEnd2;
	  float m_fMaximumDiameter;

	  /// volume of the polyps
	  float m_fVolumeOfPolyp;

    /// subvolume bounding box in the original volume
    int m_ixl;
    int m_ixu;
    int m_iyl;
    int m_iyu;
    int m_izl;
    int m_izu;

    uint1 m_uSegmentIndex;
    uint1 m_uTissueLabel;
    uint1 m_uWallSurfaceLabel;
    uint1 m_uPolypLabel;
    
    /// label index for the extracted ROI in the label volume
    const uint2 m_uComponentLabel;

    /// tissue HU bounds
    uint2 m_uTissueLow;
    uint2 m_uTissueHigh;

    /// flat or diminish polyp indicator
    bool m_IsFlatPolyp;

    /// half length of subvolume
    float m_fHalfWidthOfSubvolume;

    /// ray length for detection of polyp surface patch
    float m_f1stLevelRayLength;
    float m_f2ndLevelRayLength;
    float m_f3rdLevelRayLength;

    /// square of the radius for looking secondary ray origins
    float m_fSquareOfRadiusForSearching1stLevelRayOrigins;
    float m_fSquareOfRadiusForSearching2ndLevelRayOrigins;

    /// flat or diminutive polyp size ratio
    float m_fROISizeRatio;

    /// the maximum radius that the polyp region will be searched
    float m_fSearchRadius;

    /// container of the all final polyp voxels
    std::vector< Vector<int2> > m_viPolypVoxelContainer;
  }; // vxPolypSegmentation
} // namespace ReaderLib

// $Log: vxPolypSegmentation.h,v $
// Revision 1.16.2.2  2009/07/30 19:26:28  kchalupa
// Code Walkthrough
//
// Revision 1.16.2.1  2008/01/03 04:27:37  jmeade
// comments and standards.
//
// Revision 1.16  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.15  2007/01/05 16:29:56  dongqing
// remove GrowOutPolyp()
//
// Revision 1.14  2007/01/05 15:55:42  dongqing
// add GetPolypVolumeROI()
//
// Revision 1.13  2007/01/05 15:38:28  dongqing
// optimize the polyp surface patch computation
// The 2nd round of patch computation was removed
//
// Revision 1.12  2006/12/15 23:11:19  dongqing
// determine the center of polyp patch based on the vector from center of edge
// ring to the center of the patch
//
// Revision 1.11  2006/12/13 22:04:23  dongqing
// add determination of the center of patch
//
// Revision 1.10  2006/10/10 18:11:36  dongqing
// 1st algorithm optimization for the VCS7
// also, it is the 1st version that S&P match reasonable
//
// Revision 1.9  2006/10/03 15:50:36  dongqing
// 3 level patch detection check-in
//
// Revision 1.8  2006/09/29 16:08:25  dongqing
// change the ray detection to 2-level iteration.
//
// Revision 1.7  2006/09/26 19:34:28  dongqing
// Switch back to work on the labelVolume from sliceRenderData
//
// Revision 1.6  2006/09/26 15:55:36  dongqing
// pass in the bool label volume
//
// Revision 1.5  2006/09/21 21:17:22  dongqing
// allow to output flat polyp
//
// Revision 1.4  2006/09/14 19:57:43  dongqing
// add flat/diminish polyp detection
//
// Revision 1.3  2006/09/14 18:25:19  dongqing
// tune up the maximum diameter measurement
//
// Revision 1.2  2006/09/13 19:33:29  dongqing
// switch WallSurface from tissue to air near tissue
//
// Revision 1.1  2006/09/12 21:34:16  dongqing
// initial check in the vxPolypSegmentation
//
// Revision 1.0 2006/09/05 
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/vxPolypSegmentation.h,v 1.16.2.2 2009/07/30 19:26:28 kchalupa Exp $
// $Id: vxPolypSegmentation.h,v 1.16.2.2 2009/07/30 19:26:28 kchalupa Exp $
