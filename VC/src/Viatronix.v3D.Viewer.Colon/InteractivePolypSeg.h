// $Id: InteractivePolypSeg.h,v 1.4 2007/03/13 03:10:51 jmeade Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Dongqing Chen dongqing@viatronix.net


/** 
 *  This class is to segment polyp region by an user input. The user will use
 *  the mouse to click a voxel on/in polyp region. The class should automatically 
 *  segment out the polyp region and label the region in the label volume. The
 *  class will also determine minimum and maximum diameter, height (of a pedunculated
 *  polyp), and volume of the Polyp.
 */


// pragmas
#pragma once


// includes
#include <iostream>
#include <vector>
#include "Global.h"
#include "volume.h"


// namespaces
namespace ReaderLib
{

  class InteractivePolypSeg
  {
  // public functions
  public:

    /// Constructor
    InteractivePolypSeg( LinVolUC & ecvVolUC, BlockVolUC & difVol, 
		                     BlockVolUC & insVol, LinVolUC & m_paintVol );

	  /// destructor
	  ~InteractivePolypSeg();

	  /// trigger the polyp segmentation by a click in 3D
	  bool ClickIn3D( const Point<float4>& selectVoxel, Triple<float4>& viewDirection, 
		  const Triple<float4>& skeletonNearestPoint, std::string & sMessageBox ) const;

	  /// trigger the polyp segmentation by a click in 2D
	  bool ClickIn2D( const Point<int4>& selectVoxel, const Triple<float4>& viewDirection, 
		  const Triple<float4>& skeletonNearestPoint, std::string & sMessageBox ) const;

	  /// get the maximum diameter of polyp
	  float4 GetMaxDiameter( Point<int4> & vertex01, Point<int4> & vertex02 );

	  /// get the minimum diameter of polyp
	  float4 GetMinDiameter( Point<int4> & vertex01, Point<int4> & vertex02 );

    /// get the height of polyp
	  float4 GetHeight( Point<int4> & bottomVertex, Point<int4> & topVertex );

    /// get the volume of polyp in mm^3
    void GetVolume( float4 & fVolume );

  // private functions
  private:

	  /// extract the sub-volume encloseing the polyp
	  bool ExtractSubvolume( std::string & sMessageBox );

	  /// determine the true bounding box dimension for the isotropic sub-volume
	  void DetermineTrueBoundingBox( Triple<int4> & dimLB, Triple<int4> & dimUB );

	  /// extract contour of the related colonic surface
	  void ExtractColonSurface();

    /// Determine the polyp patch surface
    bool DeterminePolypRegion( std::string & sMessageBox );

    /// 3D 26-connected distance constrained region grow
    int4 ConstrainedRegionGrow3D( const Point<int4> & viSeed, uint1 * pMask, const uint1 uLabelOld, 
																  const uint1 uLabelNew, const int4 iDistanceBound );
  	
	  /// Ray searching algorithm for polyp shell determination based on convex rule
    int4 DeterminePolypPatchByRaySearch( Point<int4> & viOrigin, int4 & iDistanceBound, 
																			   uint1 & uROILabel, uint1 & uNewLabel );
  	
	  /// 3D 6-connected feature constrained region grow for determination of the polyp patch
    int4 RegionGrow6NB( LinVolUC & vol, int4 & iSeed, uint1 & uROILabel, uint1 & uNewLabel );
  		
    /// Label true polyp volume in the m_difVol. This will allow the polyp volume displaying in 2D MPR views
    void LabelPolypRegionInDifVol( uint1 * pTmp, uint1 & uROILabel );

	  /// compute the polyp region and copy back to paint volume
	  void ComputePolypRegionInOriginalVol();

  // private variables
  private: 	

	  /// ecv volume
	  LinVolUC & m_ecvVol;

	  /// dif volume
	  BlockVolUC & m_difVol;

	  /// ins volume
	  BlockVolUC & m_insVol;

	  /// test volume for polyp region labeling
	  LinVolUC & m_paintVol;

	  /// isotropic sub-volume
	  LinVolUC m_subVol;

	  /// isotropic sub-mask
	  LinVolUC m_subMask;

	  /// user selected voxel in volume coordinate
	  Point<float4> m_fSelectedVoxel;

	  /// viewing direction when user select the point on the polyp
	  Triple<float4> m_fViewerDirection;

	  /// the nearest SKE point in whole coordinate
	  Triple<float4> m_fNearestSKEnode;

	  /// initial lumen label
	  uint1 m_uInitialLumenLabel;

	  /// background label
	  uint1 m_uBackgroundLabel;

	  /// label for the surface of the colon lumen
	  uint1 m_uColonSurfaceLabel;
	  uint1 m_uPolypSurfaceLabel;

	  /// working label for the extracted polyp region
	  uint1 m_uPolypLabel;

    /// final polyp label for the out put
	  uint1 m_uOutputPolypLabel;

	  /// threshold for the air lumen in raw value
	  uint1 m_uAirLumen;

	  /// maximum diameter
	  Triple<float4> m_vfMaxDiameterEnd1;
	  Triple<float4> m_vfMaxDiameterEnd2;
	  float4 m_fMaximumDiameter;

	  /// minimum diameter
	  Triple<float4> m_vfMinDiameterEnd1;
	  Triple<float4> m_vfMinDiameterEnd2;
	  float4 m_fMinimumDiameter;

	  /// number of voxels in the polyp region
	  int4 m_iNumberOfVoxelsInPolyp;

	  /// volume of the polyps
	  float4 m_fVolumeOfPolyp;
  };

} // namespace ReaderLib


/*************************************************
*       directional index matching graph
*
*      07 | 06 | 05
*			 ------------
*			 08 | 12 | 04        upper adjacent slice
*			 ------------
*			 09 | 10 | 11
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*
*			 03 | 02 | 01
*			 ------------
*			    |    | 00        the current slice
*			 ------------
*			    |    |
**************************************************/


// $Log: InteractivePolypSeg.h,v $
// Revision 1.4  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.3  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/InteractivePolypSeg.h,v 1.4 2007/03/13 03:10:51 jmeade Exp $
// $Id: InteractivePolypSeg.h,v 1.4 2007/03/13 03:10:51 jmeade Exp $
