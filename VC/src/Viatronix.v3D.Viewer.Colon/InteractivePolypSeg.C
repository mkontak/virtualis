// $Id: InteractivePolypSeg.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
//
// Copyright© 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: InteractivePolypSeg.C
// Owner(s): Dongqing Chen dongqing@viatronix.net

#include "stdafx.h"
#include "HUconverter.h"
#include "InteractivePolypSeg.h"
#include "Timer.h"

#include <queue>

using namespace hdr;
using namespace ReaderLib;

#define FILE_REVISION "$Revision: 1.3 $"

/** 
 * Constructor
 *
 * @param ecvVolUC    cleansed volume
 * @param difVol      differential volume
 * @param insVol      inside volume
 * @param paintVol    marking volume
 */
InteractivePolypSeg::InteractivePolypSeg( LinVolUC & ecvVolUC, BlockVolUC & difVol, 
																				  BlockVolUC & insVol, LinVolUC & paintVol )
: m_ecvVol(ecvVolUC), m_difVol(difVol), m_insVol(insVol), m_paintVol(paintVol)
{
	m_uInitialLumenLabel =   1;
	m_uPolypLabel        = 127;
	m_uOutputPolypLabel  = 126;
	m_uBackgroundLabel   = 254;
	m_uColonSurfaceLabel = m_uBackgroundLabel-1;
  m_uPolypSurfaceLabel = m_uColonSurfaceLabel-1;
  m_uAirLumen          = HUconverter::HU2uchar(-350);
	m_fVolumeOfPolyp     = -1.0F;

	Triple<int4>   dim(121, 121, 121); 
	Triple<float4> units(0.5F, 0.5F, 0.5F); 
	hdr::UnitTypeEnum eUnitType( ecvVolUC.GetUnitType() );
	// This is a 120 x 0.5 = 60 mm cube. the center voxel is at (60, 60, 60). 
	// The voxel spacing is 0.5^3 mm^3 
	LinVolUC isotropicVol( dim, 8*sizeof(uint1), units, eUnitType, 0, VOL, 
		                     "vol", "isotropic vol", "Made by Viatronix");
	// pass the pointer to the member volume
	m_subVol = isotropicVol;

	LinVolUC isotropicMask( dim, 8*sizeof(uint1), units, eUnitType, 0, VOL, 
		                      "vol", "isotropic mask", "Made by Viatronix");
	// pass the pointer to the member volume
	m_subMask = isotropicMask;
} // default constructor 


/** 
 *  destructor
 */
InteractivePolypSeg::~InteractivePolypSeg()
{
} // destructor


/** 
 *  trigger the polyp segmentation by a click in 3D
 *
 *  @param selectVoxel            is the point on the surface of polyp that user click
 *  @param viewDirection          is the current camera viewing direction
 *  @param skeletonNearestPoint   is the point on the centerline that is closest to the click point
 *  @param sMessageBox            is the error message passed back to the Viewer
 *  @return                       true if successfully segmented
 */
bool InteractivePolypSeg::ClickIn3D( const Point<float4>& selectVoxel, const Triple<float4>& viewDirection, 
																		 const Triple<float4>& skeletonNearestPoint, std::string & sMessageBox ) const
{
	m_fViewerDirection = viewDirection;
	m_fNearestSKEnode  = skeletonNearestPoint;
	m_fSelectedVoxel   = selectVoxel;

	if( m_ecvVol.GetDim() != m_paintVol.GetDim() )
	{
		sMessageBox = "The dimension of ECV and PAINT volumes do not match.";
		return( false );
	}

	bool bGood( true );
	// compute the isotropic sub-volume enclosing the polyp
	bGood = ExtractSubvolume( sMessageBox );
	if( !bGood )
	  return( false );
	//m_subVol.Write("d:/diag/subVolume.vol", false);
	//m_subMask.Write("d:/diag/subMask.vol", false);

	// extract contour of the related colonic surface
	ExtractColonSurface();
	//m_subMask.Write("d:/diag/subContour.vol", false);

	// compute the rough region of the polyp
  bGood = DeterminePolypRegion( sMessageBox );
	if( !bGood )
	  return( false );
	//m_subVol.Write("d:/diag/polypSubVolume.vol", false); 

	// compute the polyp
	ComputePolypRegionInOriginalVol();
  //m_labelVol.Write("polypOriginalVolume.vol", false); 

	// compute physical attributes of the segmented polyp
	//ComputeAttributes();

	return( bGood );
} // ClickIn3D


/** 
 *  trigger the polyp segmentation by a click in 2D
 *
 *  @param selectVoxel             is the point on the surface of polyp that user click
 *  @param skeletonNearestPoint    is the point on the centerline that is closest to the click point
 *  @param sMessageBox             is the error message passed back to the Viewer
 *  @return                        true if segmentation succeeds
 */
bool InteractivePolypSeg::ClickIn2D( const Point<int4>& selectVoxel, const Triple<float4>& viewDirection, 
																		const Triple<float4>& skeletonNearestPoint, std::string & sMessageBox ) const
{
	bool bGood( true );

	return( bGood );
} // ClickIn2D


/** 
 *  get the maximum diameter of polyp
 *  @param vertex01     is one end of the diameter line
 *  @param vertex02     is another end of the diameter line
 *  @return             the maximum diameter
 */
float4 InteractivePolypSeg::GetMaxDiameter( const Point<int4> & vertex01, const Point<int4> & vertex02 ) const
{
	float4 fMaxDiam(0.0F);

	return(fMaxDiam);
} // GetMaxDiameter


/** 
*  get the minimum diameter of polyp
*  @param vertex01    is one end of the diameter line
*  @param vertex02    is another end of the diameter line
*  @return            the minimum diameter
*/
float4 InteractivePolypSeg::GetMinDiameter( const Point<int4> & vertex01, const Point<int4> & vertex02 ) const
{
	float4 fMinDiam(0.0F);

	return(fMinDiam);
} // GetMinDiameter()


/** 
*  get the height of polyp
*  @param bottomVertex      is the root of the stalk
*  @param topVertex         is the top of the cap
*  @return                  the height
*/
float4 InteractivePolypSeg::GetHeight( const Point<int4> & bottomVertex, const Point<int4> & topVertex ) const
{
	float4 fHeight(0.0F);

	return(fHeight);
} // GetHeight()


/** 
*  get the volume of polyp in mm^3
* 
*  @param fVolume       is the volume of the polyp
*/
void InteractivePolypSeg::GetVolume( float4 & fVolume ) const
{
	if( m_fVolumeOfPolyp > 0.0F )
	  fVolume = m_fVolumeOfPolyp;
	else 
		fVolume = -1.0F;
} // GetHeight()


/** 
 *  extract the sub-volume enclosing the polyp
 * 
 *  @param sMessageBox      the container for the error message
 */
bool InteractivePolypSeg::ExtractSubvolume( std::string & sMessageBox )
{
	const Triple<int4>   dimOrig  ( m_ecvVol.GetDim() );
	const Triple<float4> unitsOrig( m_ecvVol.GetUnits() );
	const Triple<int4>   dimIso   ( m_subVol.GetDim() );
	const Triple<float4> unitsIso ( m_subVol.GetUnits() );

	uint1 * pSubVol( m_subVol.GetDataPtr() );
	uint1 * pSubMask( m_subMask.GetDataPtr() );
	// initialization of sub-volume
	const int4  iSubNum   ( m_subVol.GetNumberOfVoxels() );
	const uint1 uSoftissue( HUconverter::HU2uchar(0)     );
	// the initialize the sub-vol
	for( int4 i=iSubNum-1; i>0; i-- )
	  pSubVol[i] = uSoftissue;

	Triple<int4> dimLB(0,0,0);       // the lower bound overlapping the original volume
	Triple<int4> dimUB( dimIso );    // the upper bound overlapping the original volume
  DetermineTrueBoundingBox( dimLB, dimUB );
  
	// the world coordinate for the lower-left conner of the sub-volume
	const Triple<float4> subVolOriginWorldCoordinate( m_fSelectedVoxel.m_x-static_cast<float4>(dimIso.m_x)*unitsIso.m_x/2.0F,
                                                    m_fSelectedVoxel.m_y-static_cast<float4>(dimIso.m_y)*unitsIso.m_y/2.0F,
																										m_fSelectedVoxel.m_z-static_cast<float4>(dimIso.m_z)*unitsIso.m_z/2.0F );

	const int4  iSIso     ( dimIso.m_x*dimIso.m_y   );    // number of voxel in slice of the isotropic volume
	const int4  iSOriginal( dimOrig.m_x*dimOrig.m_y );    // number of voxel in slice of the original volume

	uint1 * pVol    ( m_ecvVol.GetDataPtr()  );
	int4 x, y, z, s, ss, t, tt;
	const int4 iNB[6] =
	{ // neighbor offset for speeding up computation
		dimOrig.m_x, dimOrig.m_x+1, iSOriginal, iSOriginal+1,
		iSOriginal+dimOrig.m_x, iSOriginal+dimOrig.m_x+1
	};
  float4 fValue[8];
	int4   iOffset[7];
	uint1 uTmp( 0x01 );

	// compute the isotropic sub-volume
	for( z=dimLB.m_z; z<dimUB.m_z; z++ )
	{ 
		float4 zWorldCurrent         ( subVolOriginWorldCoordinate.m_z+z*unitsIso.m_z );
		int4   zLsliceIndexInOriginal( zWorldCurrent/unitsOrig.m_z );
		int4   zUsliceIndexInOriginal( zLsliceIndexInOriginal+1 );
		float4 zScaleU               ( (zWorldCurrent/unitsOrig.m_z-static_cast<float4>(zLsliceIndexInOriginal)) );
		float4 zScaleL               ( 1.0F-zScaleU );

		s  = z*iSIso;
		ss = zLsliceIndexInOriginal*iSOriginal;
		for( y=dimLB.m_y; y<dimUB.m_y; y++)
		{
			float4 yWorldCurrent         ( subVolOriginWorldCoordinate.m_y+y*unitsIso.m_y );
			int4   yLsliceIndexInOriginal( yWorldCurrent/unitsOrig.m_y );
			int4   yUsliceIndexInOriginal( yLsliceIndexInOriginal+1 );
			float4 yScaleU               ( (yWorldCurrent/unitsOrig.m_y-static_cast<float4>(yLsliceIndexInOriginal)) );
			float4 yScaleL               ( 1.0F-yScaleU );

			t  = s+y*dimIso.m_x;
			tt = ss+yLsliceIndexInOriginal*dimOrig.m_x;
			for( x=dimLB.m_x; x<dimUB.m_x; x++ )
			{
				float4 xWorldCurrent         ( subVolOriginWorldCoordinate.m_x+x*unitsIso.m_x );
				int4   xLsliceIndexInOriginal( xWorldCurrent/unitsOrig.m_x );
				int4   xUsliceIndexInOriginal( xLsliceIndexInOriginal+1 );
				float4 xScaleU               ( (xWorldCurrent/unitsOrig.m_x-static_cast<float4>(xLsliceIndexInOriginal)) );
				float4 xScaleL               ( 1.0F-xScaleU );

				const int4 iCurrent( tt+xLsliceIndexInOriginal );  // the current voxel offset in the original volume
				// neighbor offset
				iOffset[0] = iCurrent+1;
				iOffset[1] = iCurrent+iNB[0];
				iOffset[2] = iCurrent+iNB[1];
				iOffset[3] = iCurrent+iNB[2];
				iOffset[4] = iCurrent+iNB[3];
				iOffset[5] = iCurrent+iNB[4];
				iOffset[6] = iCurrent+iNB[5];

				// recover original value from ECV
				fValue[0] = static_cast<float4>(pVol[iCurrent]);
				fValue[1] = static_cast<float4>(pVol[iOffset[0]]);
				fValue[2] = static_cast<float4>(pVol[iOffset[1]]);
				fValue[3] = static_cast<float4>(pVol[iOffset[2]]);
				fValue[4] = static_cast<float4>(pVol[iOffset[3]]);
				fValue[5] = static_cast<float4>(pVol[iOffset[4]]);
				fValue[6] = static_cast<float4>(pVol[iOffset[5]]);
				fValue[7] = static_cast<float4>(pVol[iOffset[6]]);

				pSubVol[x+t] = static_cast<uint1>( zScaleL*(yScaleL*(xScaleL*fValue[0] + xScaleU*fValue[1]) +
					                                 yScaleU*(xScaleL*fValue[2] + xScaleU*fValue[3])) + 
					                                 zScaleU*(yScaleL*(xScaleL*fValue[4] + xScaleU*fValue[5]) +
											                     yScaleU*(xScaleL*fValue[6] + xScaleU*fValue[7])) );

				if( pSubVol[x+t] < m_uAirLumen ) 
					pSubMask[x+t] = m_uInitialLumenLabel;
			} // for( x )
		} // for( y )
	} // for(z)
  
  int4 iCenterOffset( dimIso.m_z/2*iSIso+dimIso.m_y/2*dimIso.m_x+dimIso.m_x/2 );
	if( pSubVol[iCenterOffset] > HUconverter::HU2uchar(200) )
	{
		sMessageBox = "Seed of polyp is not at the air\\tissue boundary";
		return( false );
	}

	// label voxels outside the bounding box in sub-mask
	if( 0 < dimLB.m_z )
	{
		for( z=0; z<dimLB.m_z; z++ )
		{
			s = z*iSIso;
			if( 0 < dimLB.m_y )
			{
				for( y=0; y<dimLB.m_y; y++ )
				{
					t = s+y*dimIso.m_x;
					if( 0 < dimLB.m_x )
					  for( x=0; x<dimLB.m_x; x++ )
						  pSubMask[t+x] = m_uBackgroundLabel;

					if( dimUB.m_x < dimIso.m_x )
						for( x=dimUB.m_x; x<dimIso.m_x; x++ )
							pSubMask[t+x] = m_uBackgroundLabel;
				} // for( y )
			} // if
			
			if( dimUB.m_y < dimIso.m_y )
			{
				for( y=dimUB.m_y; y<dimIso.m_y; y++ )
				{
					t = s+y*dimIso.m_x;
					if( 0 < dimLB.m_x )
						for( x=0; x<dimLB.m_x; x++ )
							pSubMask[t+x] = m_uBackgroundLabel;

					if( dimUB.m_x < dimIso.m_x )
						for( x=dimUB.m_x; x<dimIso.m_x; x++ )
							pSubMask[t+x] = m_uBackgroundLabel;
				} // for( y )
			} // if
		} // for( z )
	} // if( 0 < dimLB.m_z )

	if( dimUB.m_z < dimIso.m_z )
	{
		for( z=dimUB.m_z; z<dimIso.m_z; z++ )
		{
			s = z*iSIso;
			if( 0 < dimLB.m_y )
			{
				for( y=0; y<dimLB.m_y; y++ )
				{
					t = s+y*dimIso.m_x;
					if( 0 < dimLB.m_x )
						for( x=0; x<dimLB.m_x; x++ )
							pSubMask[t+x] = m_uBackgroundLabel;

					if( dimUB.m_x < dimIso.m_x )
						for( x=dimUB.m_x; x<dimIso.m_x; x++ )
							pSubMask[t+x] = m_uBackgroundLabel;
				} // for( y )
			} // if

			if( dimUB.m_y < dimIso.m_y )
			{
				for( y=dimUB.m_y; y<dimIso.m_y; y++ )
				{
					t = s+y*dimIso.m_x;
					if( 0 < dimLB.m_x )
						for( x=0; x<dimLB.m_x; x++ )
							pSubMask[t+x] = m_uBackgroundLabel;

					if( dimUB.m_x < dimIso.m_x )
						for( x=dimUB.m_x; x<dimIso.m_x; x++ )
							pSubMask[t+x] = m_uBackgroundLabel;
				} // for( y )
			} // if
		} // for( z )
	} // if( 0 < dimLB.m_z )

	/*Triple<int4> viOrigin(-1,-1,-1);
  float4 f, fMin(999999.9F);
	// determine the nearest colon lumen voxel to the selected voxel
	for( z=-3; z<4; z++ )
	{
		s = iCenterOffset+z*iSIso;
		for( y=-3; y<4; y++ )
		{
			t = s+y*dimIso.m_x;
			for( x=-3; x<4; x++ )
			{
				i = t+x;
				if( pSubMask[i] == uTmp )
				{
					f = static_cast<float4>(x*x+y*y+z*z);
					if( f < fMin )
					{
						fMin = f;
						viOrigin.m_x = x;
						viOrigin.m_y = y;
						viOrigin.m_z = z;
					}
				} // if()
			} // for(x)
		} // for(y)
	} // for(z)  
	
	if( (viOrigin.m_x == -1)||(viOrigin.m_y == -1)||(viOrigin.m_z == -1) )
	{
		sMessageBox = "Seed far from air lumen.";
		return(false);
	}
	x = viOrigin.m_z*iSIso+viOrigin.m_y*dimIso.m_x+viOrigin.m_x;
	RegionGrow6NB(m_subMask, x, uTmp, m_uInitialLumenLabel );

	y = m_subMask.GetNumberOfVoxels();
	for( x=y-1; x>0; x-- )
		if( pSubMask[x] == uTmp )
			pSubMask[x] = 0x00;*/

	return( true );
} // ExtractSubvolume()


/** 
 *  determine the true bounding box dimension for the isotropic sub-volume
 *  this will prevent the sub-volume go beyond the original volume
 *
 *  @param dimLB    is the true lower bound for the sub-volume that enclosed in the original volume
 *  @param dimUB    is the true upper bound for the sub-volume that enclosed in the original volume
 */
void InteractivePolypSeg::DetermineTrueBoundingBox( Triple<int4> & dimLB, Triple<int4> & dimUB ) const
{ 
	const Triple<int4>   dimOrig  ( m_ecvVol.GetDim()   );
	const Triple<float4> unitsOrig( m_ecvVol.GetUnits() );
	const Triple<int4>   dimIso   ( m_subVol.GetDim()   );
	const Triple<float4> unitsIso ( m_subVol.GetUnits() );
	const float4 fXworld( m_fSelectedVoxel.m_x );   // world coordinate of the selected voxel 
	const float4 fYworld( m_fSelectedVoxel.m_y );   // world coordinate of the selected voxel
	const float4 fZworld( m_fSelectedVoxel.m_z );   // world coordinate of the selected voxel
  const float4 fXHalfLength( static_cast<float4>(dimIso.m_x/2)*unitsIso.m_x );       // half length of the sub-volume
	const float4 fYHalfLength( static_cast<float4>(dimIso.m_y/2)*unitsIso.m_y );       // half length of the sub-volume
	const float4 fZHalfLength( static_cast<float4>(dimIso.m_z/2)*unitsIso.m_z );       // half length of the sub-volume

	float4 fLB( fZworld-fZHalfLength );
	if( fLB > 0.0F )
		dimLB.m_z =	1;  
	else
    dimLB.m_z = dimIso.m_z/2-static_cast<int4>(fZworld/unitsIso.m_z);
	float4 fUB( fZworld+fZHalfLength );
	if( fUB < static_cast<float4>(dimOrig.m_z)*unitsOrig.m_z )
    dimUB.m_z = dimIso.m_z-2;
	else
		dimUB.m_z = static_cast<int4>( (static_cast<float4>(dimOrig.m_z)*unitsOrig.m_z-m_fSelectedVoxel.m_z)/unitsIso.m_z )+
		            (dimIso.m_z/2);

	fLB = fXworld-fXHalfLength;
	if( fLB > 0.0F )
		dimLB.m_x =	1;  
	else
		dimLB.m_x = dimIso.m_x/2-static_cast<int4>(fXworld/unitsIso.m_x);
	fUB = fXworld+fXHalfLength;
	if( fUB < static_cast<float4>(dimOrig.m_x)*unitsOrig.m_x )
		dimUB.m_x = dimIso.m_x-2;
	else
		dimUB.m_x = static_cast<int4>( (static_cast<float4>(dimOrig.m_x)*unitsOrig.m_x-m_fSelectedVoxel.m_x)/unitsIso.m_x )+
		            (dimIso.m_x/2);

	fLB = fYworld-fYHalfLength;
	if( fLB > 0.0F )
		dimLB.m_y =	1;  
	else
		dimLB.m_y = dimIso.m_y/2-static_cast<int4>(fYworld/unitsIso.m_y);
	fUB = fXworld+fXHalfLength;
	if( fUB < static_cast<float4>(dimOrig.m_y)*unitsOrig.m_y )
		dimUB.m_y = dimIso.m_y-2;
	else
		dimUB.m_y = static_cast<int4>( (static_cast<float4>(dimOrig.m_y)*unitsOrig.m_y-m_fSelectedVoxel.m_y)/unitsIso.m_y )+
		            (dimIso.m_y/2);
} // DetermineTrueBoundingBox


/** 
 *  extract contour of the related colonic surface
 */
void InteractivePolypSeg::ExtractColonSurface()
{
	const Triple<int4> dim(m_subMask.GetDim() );
	const int4 iS( dim.m_x*dim.m_y );
	const int4 iN( m_subMask.GetNumberOfVoxels() );
	const int4 viNB[26] =
	{
		 1, -1, dim.m_x, -dim.m_x, iS, -iS, dim.m_x+1, dim.m_x-1, -dim.m_x+1, -dim.m_x-1,
		 iS+1,  iS-1,  iS+dim.m_x,  iS-dim.m_x, 
		-iS+1, -iS-1, -iS+dim.m_x, -iS-dim.m_x, 
		 iS+dim.m_x+1,  iS+dim.m_x-1,  iS-dim.m_x+1,  iS-dim.m_x-1,
		-iS+dim.m_x+1, -iS+dim.m_x-1, -iS-dim.m_x+1, -iS-dim.m_x-1,
	};

	uint1 * pSubMask( m_subMask.GetDataPtr() );
	int4 i, j, k;
	const int4 iStart( 2*(iS+dim.m_x+2) );
	const int4 iEnd( iN-iStart );
	// determine the contour shell
	for( i=iStart; i<iEnd; i++ )
	{
		if( pSubMask[i] == m_uInitialLumenLabel )
		{
			for( j=0; j<6; j++ )
			{
				k = i+viNB[j];
				if( (k > 0)&&(k < iN)&&(pSubMask[k] != m_uInitialLumenLabel)&&
					  (pSubMask[k] != m_uBackgroundLabel) )
					pSubMask[k] = m_uColonSurfaceLabel;
			} // for(j)
		} // if
	} // for(i)
} // ExtractColonSurface()


/** 
 *  Determine the polyp patch surface
 *
 *  @param sMessageBox    is the error message if the segmentation fail
 *  @return               true if the segmentation success.
 */
bool InteractivePolypSeg::DeterminePolypRegion( std::string & sMessageBox )
{
	Triple<int4>      dim     ( m_subVol.GetDim()      ); 
	Triple<float4>    units   ( m_subVol.GetUnits()    ); 
	hdr::UnitTypeEnum uintType( m_subVol.GetUnitType() );

	bool bGood( true );
	const int4 iS( dim.m_x*dim.m_y );
	const int4 iN( m_subVol.GetNumberOfVoxels() );
  
	uint1 * pSubVol ( m_subVol.GetDataPtr()  );
	uint1 * pSubMask( m_subMask.GetDataPtr() );
	Point<int4> viOrigin(-1,-1,-1);
	int4 x, y, z, s, t, i;
	const int4 iCenterOffset( (dim.m_z/2)*iS+(dim.m_y/2)*dim.m_x+dim.m_x/2 );
	float4 f, fMin(999999.9F);
	// determine the nearest colon surface voxel to the selected voxel
	for( z=-4; z<5; z++ )
	{
		s = iCenterOffset+z*iS;
		for( y=-4; y<5; y++ )
		{
			t = s+y*dim.m_x;
			for( x=-4; x<5; x++ )
			{
				i = t+x;
				if( pSubMask[i] == m_uColonSurfaceLabel )
				{
					f = static_cast<float4>(x*x+y*y+z*z);
					if( f < fMin )
					{
						fMin = f;
						viOrigin.m_x = x;
						viOrigin.m_y = y;
						viOrigin.m_z = z;
					}
				} // if()
			} // for(x)
		} // for(y)
	} // for(z)

	if( viOrigin != Point<int4>(-1,-1,-1) )
	{ // get the seed coordinate in the sub-volume
    viOrigin.m_x = viOrigin.m_x+dim.m_x/2;  
		viOrigin.m_y = viOrigin.m_y+dim.m_y/2;  
		viOrigin.m_z = viOrigin.m_z+dim.m_z/2;  
	}
	else
	{
		sMessageBox = "Error seed of polyp";
		return( false );
 	}

	int4 iDistanceBound( static_cast<int4>(20.0F*20.0F/(units.m_x*units.m_x)) );
	int4 iNum = ConstrainedRegionGrow3D( viOrigin, pSubMask, m_uColonSurfaceLabel, 
		                                  m_uPolypSurfaceLabel, iDistanceBound );

  if( iNum < 3 )
	{
		sMessageBox = "Fail to extract region of polyp: ConstrainedRegionGrow3D()";
		return( false );
	}

	iDistanceBound = static_cast<int4>(20.0F/units.m_x);
	iNum = DeterminePolypPatchByRaySearch( viOrigin, iDistanceBound, 
		                               m_uPolypSurfaceLabel, m_uPolypLabel );
  if( iNum < 3 )
	{
		sMessageBox = "Fail to extract region of polyp: DeterminePolypRegion()";
		return( false );
	}

	std::vector< Triple<int4> > vPolypShell;
	for( z=dim.m_z-1; z>0; z-- )
	{
		s = z*iS;
		for( y=dim.m_y-1; y>0;y-- )
		{
			t = s+y*dim.m_x;
			for	( x=dim.m_x-1; x>0; x-- )
			{
				if( pSubMask[t+x] == m_uPolypLabel )
				  vPolypShell.push_back( Triple<int4>(x,y,z) );
			} // for(x)
		} // for(y)
	} // for(z)

	iNum = vPolypShell.size();
	if( iNum < 8 )
	{
		sMessageBox = "Polyp region is too small";
		return( false );
	}
	int4 j, p, q, iGap, nn(iNum-1);
	register Triple<float4> vfDirection, vfPoint;
	float4 fL, fX, fY, fZ;
	// line connection filling algorithm for determination of the volume enclosed by the shell
	for( i=0; i<nn; i++ )
	{ 
		for( j=i+1; j<iNum; j++ )
		{
			vfDirection.m_x = static_cast<float4>(vPolypShell[i].m_x-vPolypShell[j].m_x);
			vfDirection.m_y =	static_cast<float4>(vPolypShell[i].m_y-vPolypShell[j].m_y);
			vfDirection.m_z =	static_cast<float4>(vPolypShell[i].m_z-vPolypShell[j].m_z);
			if( (abs(vfDirection.m_x) > 2.0F)||(abs(vfDirection.m_y) > 2.0F)||(abs(vfDirection.m_z) > 2.0F) )
			{
				fL = sqrt(vfDirection.m_x*vfDirection.m_x+vfDirection.m_y*vfDirection.m_y+vfDirection.m_z*vfDirection.m_z);
				iGap = Max( Max(abs(vfDirection.m_x), abs(vfDirection.m_y)), 
					          Max(abs(vfDirection.m_x), abs(vfDirection.m_z)) );
				vfDirection.m_x = vfDirection.m_x/fL;   // directional COS value
				vfDirection.m_y = vfDirection.m_y/fL;   // directional COS value
				vfDirection.m_z = vfDirection.m_z/fL;   // directional COS value
				fX = fL*vfDirection.m_x/static_cast<float4>(iGap);   // incremental gap along axis
				fY = fL*vfDirection.m_y/static_cast<float4>(iGap);   // incremental gap along axis
				fZ = fL*vfDirection.m_z/static_cast<float4>(iGap);   // incremental gap along axis
        vfPoint.m_x = static_cast<float4>(vPolypShell[j].m_x);
        vfPoint.m_y = static_cast<float4>(vPolypShell[j].m_y);
        vfPoint.m_z = static_cast<float4>(vPolypShell[j].m_z);
				iGap++;
				for( p=1; p<iGap; p++ )
				{
          vfPoint.m_x = vfPoint.m_x+fX;
          vfPoint.m_y = vfPoint.m_y+fY;
          vfPoint.m_z = vfPoint.m_z+fZ;

					q = static_cast<int4>(vfPoint.m_z)*iS + static_cast<int4>(vfPoint.m_y)*dim.m_x + static_cast<int4>(vfPoint.m_x);
					if( (pSubMask[q] != m_uBackgroundLabel)&&(pSubMask[q] != m_uPolypLabel)&&(pSubMask[q] != m_uInitialLumenLabel) )
						pSubMask[q] = m_uPolypLabel;
				} // for(p)
			} // if()
		} // for(j)
	} // for(i)

	const int4 viNB[26] =
	{
		  1,    -1, dim.m_x,   dim.m_x-1,     dim.m_x+1,      -dim.m_x,  -dim.m_x-1,    -dim.m_x+1, 
		 iS,  iS+1,    iS-1,  iS+dim.m_x,  iS+dim.m_x-1,  iS+dim.m_x+1,  iS-dim.m_x,  iS-dim.m_x-1,  iS-dim.m_x+1, 
		-iS, -iS+1,   -iS-1, -iS+dim.m_x, -iS+dim.m_x-1, -iS+dim.m_x+1, -iS-dim.m_x, -iS-dim.m_x-1, -iS-dim.m_x+1 
	};
	uint1 * pTmp = new uint1[iN];
	if( pTmp == NULL )
	{
		sMessageBox = "No memory is available.";
		return( false );
	}
	// remove thin layer part in the polyp region
	p = iS+dim.m_x;
	q = iN-p;
	const int4 iThinLayerThreshold( 7 );
	m_iNumberOfVoxelsInPolyp = 0;
	for( i=p; i<q; i++ )
	{
		if( pSubMask[i] == m_uPolypLabel )
		{
			x = 0;
			for( j=25; j>=0; j-- )
				if( pSubMask[i+viNB[j]] == m_uPolypLabel )
					x++;

			if( x > iThinLayerThreshold )
			{
				pTmp[i] = m_uPolypLabel;
		    m_iNumberOfVoxelsInPolyp++;
			} // if( x > iThinLayerThreshold)
		} // if
	} // if(i)

	LabelPolypRegionInDifVol( pTmp, m_uPolypLabel );
  delete [] pTmp;

	// computation the polyp volume size in mm^3
	m_fVolumeOfPolyp = static_cast<float4>(m_iNumberOfVoxelsInPolyp)*units.m_x*units.m_y*units.m_z;  

	x = iS+dim.m_x+1;
	y = iN-iS-dim.m_x-1;
	const uint1 u1( m_uPolypLabel+1 );
	const uint1 ut1( HUconverter::HU2uchar(-850) );
	const uint1 ut2( HUconverter::HU2uchar(-200) );
	// dilation the polyp region into air lumen
  for( i=x; i<y; i++ )
	{
	  if( pSubMask[i] == m_uPolypLabel )
		{
			for( j=0; j<26; j++ )
			{
				z = i+viNB[j];
				if( (pSubMask[z] != m_uPolypLabel)&&(pSubMask[z] != u1)&&(pSubVol[z] > ut1)&&
					  (pSubVol[z] <= ut2)&&(pSubVol[z] < pSubVol[i]) )
					pSubMask[z] = u1;
			} // for(j)
		} // if()
	} // for(i)
  
	const uint1 u2( u1+1 );
	// dilation the polyp region into air lumen
  for( i=x; i<y; i++ )
	{
	  if( (pSubMask[i] == m_uPolypLabel)||(pSubMask[i] == u1) )
		{
			for( j=0; j<26; j++ )
			{
				z = i+viNB[j];
				if( (pSubMask[z] != m_uPolypLabel)&&(pSubMask[z] != u1)&&(pSubMask[z] != u2)&&
					  (pSubVol[z] > ut1)&&(pSubVol[z] <= ut2)&&(pSubVol[z] < pSubVol[i]) )
					pSubMask[z] = u2;
			} // for(j)
		} // if()
	} // for(i)

	return( bGood );
} //  DeterminePolypRegion()


/** 
*  3D 26-connected distance constrained region grow
*
*  @param viSeed            is the seed
*  @param uOldLabel 
*  @param uNewLabel
*  @param iDistanceBound
*  @return                  number of voxels that grown
*/
int4 InteractivePolypSeg::ConstrainedRegionGrow3D( const const Point<int4> & viSeed, uint1 * pMask, const uint1 uLabelOld, 
																									const uint1 uLabelNew, const int4 iDistanceBound ) const
{
	const Triple<int4> dim( m_subMask.GetDim()            );
	const int4          iS( dim.m_x*dim.m_y               );
	const int4          iN( m_subMask.GetNumberOfVoxels() );
  
	int4 iCount(0);
	std::queue< Point<int4> > growQueueP;    // region grow queue for the point coordinates
	std::queue<int4>          growQueueO;    // region grow queue for the point offsets
	growQueueP.push( viSeed );
	growQueueO.push( viSeed.m_z*iS+viSeed.m_y*dim.m_x+viSeed.m_x );
	pMask[growQueueO.front()] = uLabelNew;
	iCount++;

	Point<int4> viCurrent (0,0,0);
	Point<int4> viNeighbor(0,0,0);
	// offset variables
	int4 iCurrent, iNeighbor;
  int4 x, y, z, s, t;

	// 26-connected region grow
	while( !growQueueO.empty() )
	{
		iCurrent  = growQueueO.front();
		viCurrent = growQueueP.front();

		// go through all neighbors of the current voxel
		for( z=-1; z<2; z++ )
		{
			s = iCurrent+z*iS;
			viNeighbor.m_z = viCurrent.m_z+z;
			for( y=-1; y<2; y++ )
			{
				t = s+y*dim.m_x;
				viNeighbor.m_y = viCurrent.m_y+y;
				for( x=-1; x<2; x++ )
				{
					iNeighbor = t+x;
					viNeighbor.m_x = viCurrent.m_x+x;
					if( (iNeighbor > 0)&&(iNeighbor < iN)&&(pMask[iNeighbor] == uLabelOld) )
					{ // the neighbor is in the ROI
						if( (viNeighbor.m_x-viSeed.m_x)*(viNeighbor.m_x-viSeed.m_x) +
							  (viNeighbor.m_y-viSeed.m_y)*(viNeighbor.m_y-viSeed.m_y) +
							  (viNeighbor.m_z-viSeed.m_z)*(viNeighbor.m_z-viSeed.m_z) <= iDistanceBound )
						{ // the neighbor is within the range of distance
							pMask[iNeighbor] = uLabelNew;
							growQueueO.push( iNeighbor );
							growQueueP.push( viNeighbor );
							iCount++;
						} // if(the neighbor is within the range of distance)
					} // if(the neighbor is in the ROI)
				} // for(x)
			} // for(y)
		} // for(z)

		growQueueP.pop();
		growQueueO.pop();
	} // while()

	return( iCount );
} // ConstrainedRegionGrow3D()


/** 
*  Ray searching algorithm for polyp shell determination based on convex rule
*
*  @param viOrigin        is the source of the ray
*  @param fDistanceBound  is the radius for distance constrain
*  @param uROILabel 
*  @param uNewLabel
*  @return                number of voxels that grown
*/
int4 InteractivePolypSeg::DeterminePolypPatchByRaySearch( const Point<int4> & viOrigin, const int4 & iDistanceBound, 
																								          const uint1 & uROILabel, const uint1 & uNewLabel ) const
{
	const Triple<int4>   dim  ( m_subMask.GetDim()            );
	const Triple<float4> units( m_subMask.GetUnits()          );
	const int4           iS   ( dim.m_x*dim.m_y               );
	const int4           iN   ( m_subMask.GetNumberOfVoxels() );

	// Note, we assume isotropic voxel spacing here
	const int4 iLX( Max(2, viOrigin.m_x-iDistanceBound) );
	const int4 iUX( Min(dim.m_x-2, viOrigin.m_x+iDistanceBound) );
	const int4 iLY( Max(2, viOrigin.m_y-iDistanceBound) );
	const int4 iUY( Min(dim.m_y-2, viOrigin.m_y+iDistanceBound) );
	const int4 iLZ( Max(2, viOrigin.m_z-iDistanceBound) );
	const int4 iUZ( Min(dim.m_z-2, viOrigin.m_z+iDistanceBound) );

	LinVolUC tmpVol( dim, 8*sizeof(uint1), units, m_subMask.GetUnitType(), 0, VOL, 
		               "vol", "temporary vol", "Made by Viatronix");
	uint1 * pTmp( tmpVol.GetDataPtr() );
	uint1 * pSubMask( m_subMask.GetDataPtr() );

	int4 iCount(0);
	register int4 x, y, z, s, t, iCurrent, i, p;
	register float4 fL, fX, fY, fZ;
	register Triple<float4> vfPoint, vfDirection;
	
	int4 iSeedOffset(viOrigin.m_z*iS+viOrigin.m_y*dim.m_x+viOrigin.m_x);
	pTmp[iSeedOffset] = uNewLabel;
	// check all colon surface voxel within the distance range
	for( z=iLZ; z<iUZ; z++ )
	{
		s = z*iS;
		for( y=iLY; y<iUY; y++ )
		{
			t = s+y*dim.m_x;
			for( x=iLX; x<iUX; x++ )
			{
				iCurrent = x+t;
				if( pSubMask[iCurrent] == uROILabel )
				{ 
					vfDirection.m_x = static_cast<float4>(x-viOrigin.m_x);
					vfDirection.m_y = static_cast<float4>(y-viOrigin.m_y);
					vfDirection.m_z = static_cast<float4>(z-viOrigin.m_z);
					// distance between current point and the seed (or origin)
					fL = sqrt(vfDirection.m_x*vfDirection.m_x+
						        vfDirection.m_y*vfDirection.m_y+
						        vfDirection.m_z*vfDirection.m_z);
					if( fL > 0.0F )
					{ // this voxel is not the ray source voxel
						const int4 iGapNum( Max( Max(vfDirection.m_x, vfDirection.m_y), 
							                       Max(vfDirection.m_x, vfDirection.m_z) ) );	
						if( iGapNum > 1 )
						{ // voxel is not the nearest neighbor of the source voxel
							vfDirection.m_x = vfDirection.m_x/fL;   // directional COS value of X
							vfDirection.m_y = vfDirection.m_y/fL;   // directional COS value of Y
							vfDirection.m_z = vfDirection.m_z/fL;   // directional COS value of Z
							fX = fL*vfDirection.m_x/static_cast<float4>(iGapNum);   // incremental gap along axis X
							fY = fL*vfDirection.m_y/static_cast<float4>(iGapNum);   // incremental gap along axis Y
							fZ = fL*vfDirection.m_z/static_cast<float4>(iGapNum);   // incremental gap along axis Z
							vfPoint.m_x = static_cast<float4>(viOrigin.m_x);
							vfPoint.m_y = static_cast<float4>(viOrigin.m_y);
							vfPoint.m_z = static_cast<float4>(viOrigin.m_z);

							bool bOnPolyp( true );
							for( i=1; i<=iGapNum; i++ )
							{
								vfPoint.m_x = vfPoint.m_x+fX;
								vfPoint.m_y = vfPoint.m_y+fY;
								vfPoint.m_z = vfPoint.m_z+fZ;

								p = static_cast<int4>(vfPoint.m_z)*iS + 
									  static_cast<int4>(vfPoint.m_y)*dim.m_x + 
									  static_cast<int4>(vfPoint.m_x);
								if( (pSubMask[p] == m_uInitialLumenLabel)&&(p != iSeedOffset) )
								{
									bOnPolyp = false;
									break;  // break the inner loop
								} // if()
							} // for(ii)

							if( bOnPolyp )
							{  
								pTmp[iCurrent] = uNewLabel;	
								iCount++;
							}
						} // if( iGapNum > 1 )
					} // if( fL > 0 ) 
				} // if( uROILabel )
			} // for(x)
		} // for(y)
	} // for(z)

	for( i=iN-iS; i>0; i-- )
		if( pTmp[i] == uNewLabel )
			pSubMask[i] = uNewLabel;
		//else 
			//pSubMask[i] = 0x00;

	return( iCount );
} // DeterminePolypPatchByRaySearch


/** 
*  3D 26-connected  region grow 
*
*  @param vol 
*  @param iSeed
*  @param uROILabel 
*  @param uNewLabel
*  @return              number of voxels that grown
*/
int4 InteractivePolypSeg::RegionGrow6NB( LinVolUC & vol, const int4 iSeed, 
																					const uint1 uROILabel, const uint1 uNewLabel ) const
{
	const Triple<int4> dim( vol.GetDim()            );
	const int4          iS( dim.m_x*dim.m_y         );
	const int4          iN( vol.GetNumberOfVoxels() );
  
	int4 iCount(0);
	std::queue<int4> growQueue;
	growQueue.push( iSeed );
	uint1 * pVol( vol.GetDataPtr() );
	pVol[iSeed] = uNewLabel;
	iCount++;

	const int4 viNB[6] =
	{
		  1, -1,dim.m_x, dim.m_x-1, iS, -iS 
	};
	// offset variables
	register int4 i, iCurrent, iNeighbor;
	// 26-connected region grow
	while( !growQueue.empty() )
	{
		iCurrent = growQueue.front();
		// go through all neighbors of the current voxel
		for( i=5; i>=0; i-- )
		{
			iNeighbor = iCurrent+viNB[i];
			if( (iNeighbor > 0)&&(iNeighbor < iN)&&(pVol[iNeighbor] == uROILabel) )
			{ 
				pVol[iNeighbor] = uNewLabel;
				growQueue.push( iNeighbor );
				iCount++;
			} // if
		} // for(i)

		growQueue.pop();
	} // while()

	return( iCount );
} // RegionGrow26NB()


/**
 *  label true polyp volume in the m_difVol
 *  This will allow the polyp volume displaying in 2D MPR views
 *
 *  @param pTmp         is the pointer to the input linear volume
 *  @param uROILabel    is the label index for the ROI
 */
void InteractivePolypSeg::LabelPolypRegionInDifVol( uint1 * pTmp, const uint1 uROILabel )
{
	const Triple<int4>   dimOrig  ( m_ecvVol.GetDim() );
	const Triple<float4> unitsOrig( m_ecvVol.GetUnits() );
	const Triple<int4>   dimIso   ( m_subVol.GetDim() );
	const Triple<float4> unitsIso ( m_subVol.GetUnits() );
	const int4 iSO( dimOrig.m_x*dimOrig.m_y );
	const int4 iSS( dimIso.m_x*dimIso.m_x );
	const int4 iNO( m_ecvVol.GetNumberOfVoxels() );
	const int4 iNS( m_subVol.GetNumberOfVoxels() );

	uint1 * pSubMask( m_subMask.GetDataPtr() );
	uint1 * pPaint( m_paintVol.GetDataPtr() );

	// the world coordinate of the lower-left corner of the sub-volume
	const Triple<float4> subVolOriginWorldCoordinate( m_fSelectedVoxel.m_x-static_cast<float4>(dimIso.m_x)*unitsIso.m_x/2.0F,
		                                                m_fSelectedVoxel.m_y-static_cast<float4>(dimIso.m_y)*unitsIso.m_y/2.0F,
		                                                m_fSelectedVoxel.m_z-static_cast<float4>(dimIso.m_z)*unitsIso.m_z/2.0F );

	int4 x, y, z, xx, yy, zz, s, t, ss, tt, i, ii;
	float4 fWorld, ff;
  // get the polyp region in the original label volume
	for( z=0; z<dimIso.m_z; z++ )
	{
		fWorld = subVolOriginWorldCoordinate.m_z+static_cast<float4>(z)*unitsIso.m_z;
		zz = static_cast<int4>(fWorld/unitsOrig.m_z);
		ff = fWorld-static_cast<float4>(zz);
		if( ff > 0.5 )
			zz++;
		s  = z*iSS;
		ss = zz*iSO;

		for( y=0; y<dimIso.m_y; y++)
		{
			fWorld = subVolOriginWorldCoordinate.m_y+static_cast<float4>(y)*unitsIso.m_y;
			yy = static_cast<int4>(fWorld/unitsOrig.m_y);
			ff = fWorld-static_cast<float4>(yy);
			if( yy > 0.5 )
				yy++;
			t  = s+y*dimIso.m_x;
			tt = ss+yy*dimOrig.m_x;

			for( x=0; x<dimIso.m_x; x++ )
			{
				fWorld = subVolOriginWorldCoordinate.m_x+static_cast<float4>(x)*unitsIso.m_x;
				xx = static_cast<int4>(fWorld/unitsOrig.m_x);
				ff = fWorld-static_cast<float4>(xx);
				if( ff > 0.5 )
					xx++;
				i  = t+x;
				ii = tt+xx;
        
				if( pTmp[i] == uROILabel )
				{
					pPaint[ii] = m_uOutputPolypLabel;
					pSubMask[i] = m_uPolypLabel;
				} // if
				else
					pSubMask[i] = 0x00;
			} // for(x)
		} // for(y)
	} // for(z)

	register uint1 uAir( HUconverter::HU2uchar(-280) );
	uint1 * pEcv( m_ecvVol.GetDataPtr() );
	it::IterBlkVolUInt1 itDif( m_difVol.GetVolIterator() );
	it::IterBlkVolUInt1 itIns( m_insVol.GetVolIterator() );
	for ( i=0; i<iNO; itDif.NextZYX(), itIns.NextZYX(),  i++ )
	  if( (pPaint[i] == m_uOutputPolypLabel)&&(pEcv[i] > uAir) )
		{
			itDif.SetVoxel( 250 );
			itIns.SetVoxel( m_uOutputPolypLabel );  // keep true record in the INS volume
		} // if 
} // LabelPolypRegionInDifVol() 


/** 
 *  copy the polyp region back to paint volume
 */
void InteractivePolypSeg::ComputePolypRegionInOriginalVol()
{
	const Triple<int4>   dimOrig  ( m_ecvVol.GetDim() );
	const Triple<float4> unitsOrig( m_ecvVol.GetUnits() );
	const Triple<int4>   dimIso   ( m_subVol.GetDim() );
	const Triple<float4> unitsIso ( m_subVol.GetUnits() );
	const int4 iSO( dimOrig.m_x*dimOrig.m_y );
	const int4 iSS( dimIso.m_x*dimIso.m_x );
	const int4 iNO( m_ecvVol.GetNumberOfVoxels() );
	const int4 iNS( m_subVol.GetNumberOfVoxels() );

	uint1 * pSubMask( m_subMask.GetDataPtr() );
	uint1 * pEcv( m_ecvVol.GetDataPtr() );
	uint1 * pPaint( m_paintVol.GetDataPtr() );

	// the world coordinate of the lower-left corner of the sub-volume
	const Triple<float4> subVolOriginWorldCoordinate( m_fSelectedVoxel.m_x-static_cast<float4>(dimIso.m_x)*unitsIso.m_x/2.0F,
		                                                m_fSelectedVoxel.m_y-static_cast<float4>(dimIso.m_y)*unitsIso.m_y/2.0F,
		                                                m_fSelectedVoxel.m_z-static_cast<float4>(dimIso.m_z)*unitsIso.m_z/2.0F );

	const int4 viNB[7] =
	{
		1, dimOrig.m_x, dimOrig.m_x+1, iSO, iSO+1, iSO+dimOrig.m_x, iSO+dimOrig.m_x+1
	};

	int4 x, y, z, xx, yy, zz, s, t, ss, tt, i, ii;
	float4 fWorld, ff;
	uint1 uu( m_uPolypLabel+2 );
  // get the polyp region in the original label volume
	for( z=dimIso.m_z-2; z>1; z-- )
	{
		fWorld = subVolOriginWorldCoordinate.m_z+static_cast<float4>(z)*unitsIso.m_z;
		zz = static_cast<int4>(fWorld/unitsOrig.m_z);
		ff = fWorld-static_cast<float4>(zz);
		if( ff > 0.5 )
			zz++;
		s  = z*iSS;
		ss = zz*iSO;

		for( y=dimIso.m_y-2; y>1; y-- )
		{
			fWorld = subVolOriginWorldCoordinate.m_y+static_cast<float4>(y)*unitsIso.m_y;
			yy = static_cast<int4>(fWorld/unitsOrig.m_y);
			ff = fWorld-static_cast<float4>(yy);
			if( ff > 0.5 )
				yy++;
			t  = s+y*dimIso.m_x;
			tt = ss+yy*dimOrig.m_x;

			for( x=dimIso.m_x-2; x>1; x-- )
			{
				fWorld = subVolOriginWorldCoordinate.m_x+static_cast<float4>(x)*unitsIso.m_x;
				xx = static_cast<int4>(fWorld/unitsOrig.m_x);
				ff = fWorld-static_cast<float4>(xx);
				if( ff > 0.5 )
					xx++;
				i  = t+x;
				ii = tt+xx;
        
				if( (pSubMask[i] >= m_uPolypLabel)&&(pSubMask[i] <= uu) )
				  pPaint[ii]= m_uOutputPolypLabel;
			} // for(x)
		} // for(y)
	} // for(z)
} // ComputePolypRegionInOriginalVol()


#undef FILE_REVISION


// Revision History:
// $Log: InteractivePolypSeg.C,v $
// Revision 1.3  2007/03/13 03:10:51  jmeade
// code standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/InteractivePolypSeg.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
// $Id: InteractivePolypSeg.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $