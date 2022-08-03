// $Id: VolumeRenderASExternal.C,v 1.3 2006/09/26 19:33:25 frank Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VolumeRenderASExternal.C  
// Description: implementation of raycasting function of external view for the VolumeRender class. 
// Created:  
// Author: Wei Li (liwei@viatronix.net)


#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include "VolumeRender.h"
#include "ReaderGlobal.h"

#include "VolumeRender.inl"

#define Color_Threshold 30
#define Depth_Threshold 3.0

using namespace ReaderLib;

// Return the sign 
static inline int4 sign(float4 x)
{
	if(x >= 0)
		return 1;
	return -1;
}

// Test whether val is >= vMin and <= vMax
static inline bool InRange(float4 val, float4 vMin, float4 vMax)
{
	if(val > vMax || val < vMin)
		return false;
	return true;
}

// Computer the intersection of the ray with the closest X slice ( slice perpendicular to the X axis
// If intersected, set pt to the intersection point and return true
// Else return false 
static bool IntersectXSlice(Point<float4>& pt, Vector<float4>& vRayDirVC, int4 flag, 
														Point<unsigned int> vol_min, Point<unsigned int>  vol_max)
{
	const uint4 MARGIN = 5;
	uint4 slcPos;
	if(flag == 0)
		return false;
	if(flag == 1)
		slcPos = vol_max.m_x-MARGIN;
	else
		slcPos = vol_min.m_x+MARGIN;
	float4 xDis = slcPos - pt.m_x;
	if(sign(xDis) != sign(vRayDirVC.m_x))
		return false;
	pt.m_x = slcPos;
	if(vRayDirVC.m_x !=0){
		float4 ratio = xDis / vRayDirVC.m_x;
		pt.m_y += vRayDirVC.m_y * ratio;
		pt.m_z += vRayDirVC.m_z * ratio;
	}
	if(InRange(pt.m_y, vol_min.m_y, vol_max.m_y) && InRange(pt.m_z, vol_min.m_z, vol_max.m_z))
		return true;
	return false;
}


// VC means Volume Coordinates
// If bOutside == false, do nothing except setting ptVC to the viewpointVC 
// If bOutside == true, test whether the ray will intersect with any of the X, Y, Z slices
// If there is an intersection, set ptVC to the intersection, firstLeapdistance to the distance and return true
// If no intersection, return false
inline bool VolumeRender::CastSingleRayExternal(Point<float4>& ptWC,
                                                const Point<float4>& viewpointVC,
                                                Normal<float4>& vRayDirection,
                                                bool bOutside,
                                                const int4 vol_min_x,
                                                const int4 vol_min_y,
                                                const int4 vol_min_z,
                                                const int4 vol_max_x,
                                                const int4 vol_max_y,
                                                const int4 vol_max_z,
                                                int flagX, int4 flagY, int4 flagZ,
                                                const float4 flc_xScl,
                                                const float4 flc_yScl,
                                                const float4 flc_zScl,
                                                const	float4 v2wXScl, const	float4 v2wYScl, const	float4 v2wZScl,
                                                uint4 pixelOffset,
                                                uint4 depthOffset,
                                                float4& firstLeapDistance,
                                                const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                                                unsigned char * const flc_pSLDvol,
                                                const uint4 flc_xStep,
                                                const uint4 flc_yStep,
                                                const uint4 flc_zStep,
                                                const float4 flc_dfbScale)
{
			  int4 voxel_x, voxel_y, voxel_z;
				Point<float4> ptVC = viewpointVC;
				if(bOutside){
					Vector<float4> vRayDirVC = Vector<float4>(vRayDirection.X() * flc_xScl, 
																									vRayDirection.Y() * flc_yScl, 
																									vRayDirection.Z() * flc_zScl);
					Point<unsigned int> vol_min = Point<unsigned int>(vol_min_x, vol_min_y, vol_min_z);
					Point<unsigned int> vol_max = Point<unsigned int>(vol_max_x, vol_max_y, vol_max_z);
					if(IntersectXSlice(ptVC, vRayDirVC, flagX, vol_min, vol_max)){
								FastTruncFloatToIntXYZ(voxel_x,voxel_y,voxel_z, ptVC.m_x, ptVC.m_y, ptVC.m_z);
								firstLeapDistance = flc_pSLDvol[voxel_x*flc_xStep + voxel_y*flc_yStep + voxel_z*flc_zStep]*flc_dfbScale - flc_fDfbSpaceLeapConversationFactor;	
					}
					else{
						ptVC = viewpointVC;			
						ptVC.Permute(-1);
						vRayDirVC.Permute(-1);
						vol_min.Permute(-1);
						vol_max.Permute(-1);
						if(IntersectXSlice(ptVC, vRayDirVC, flagY, vol_min, vol_max)){
									ptVC.Permute(1);
									FastTruncFloatToIntXYZ(voxel_x,voxel_y,voxel_z, ptVC.m_x, ptVC.m_y, ptVC.m_z);
									firstLeapDistance = flc_pSLDvol[voxel_x*flc_xStep + voxel_y*flc_yStep + voxel_z*flc_zStep]*flc_dfbScale - flc_fDfbSpaceLeapConversationFactor;	
						} else {
							ptVC = viewpointVC;			
							ptVC.Permute(-2);
							vRayDirVC.Permute(-1);
							vol_min.Permute(-1);
							vol_max.Permute(-1);
							if(IntersectXSlice(ptVC, vRayDirVC, flagZ, vol_min, vol_max)){
										ptVC.Permute(2);
										FastTruncFloatToIntXYZ(voxel_x,voxel_y,voxel_z, ptVC.m_x, ptVC.m_y, ptVC.m_z);
										firstLeapDistance = flc_pSLDvol[voxel_x*flc_xStep + voxel_y*flc_yStep + voxel_z*flc_zStep]*flc_dfbScale - flc_fDfbSpaceLeapConversationFactor;	
							} else
								/// Does not intersect with the volume, proceed to the next ray.
								return false;
						}
					}
				}

				ptWC = Point<float4>(ptVC.m_x * v2wXScl, ptVC.m_y * v2wYScl, ptVC.m_z * v2wZScl);
				return true;
}


// Constants used to convert gradient magnitude to [0, 1]
const float4 UNSIGNEDCHAR_TO_FLOAT = 1.0F/255.0F;

// Simplified version of CastSingleRay
// Used for external view of colon in overview
// If isosurface, bReverseNormal should be true, since we use the same gradient-calculating function as the internal view
inline bool VolumeRender::CastSingleRaySp(const uint4 &u, const uint4 &v,
                                          const bool &flc_bSpaceLeaping,
                                          Normal<float4> &vRayDirection,
                                          Point<float4> &point,
                                          float4 &leapDistance,
                                          const uint4 &flc_xStep, const uint4 &flc_yStep, const uint4 &flc_zStep,
                                          const uint4 &flc_xyStep, const uint4 &flc_yzStep, const uint4 &flc_xzStep,
                                          const uint4 &flc_xyzStep,
                                          const float4 &flc_xScl, const float4 &flc_yScl, const float4 &flc_zScl,
                                          const uint4 &flc_volMaxX, const uint4 &flc_volMaxY, const uint4 &flc_volMaxZ,
                                          uint1 * const flc_pSLDvol,
                                          unsigned char * const flc_pOpacityVol,
                                          float4 &alphaValue, float4 &depthValue,
                                          const float4 &flc_dfbScale,
                                          const float4 &flc_fDfbSpaceLeapConversationFactor, const float4 &flc_fDfbSpaceLeapThreshold,
                                          Vector<float4> &vInterval)
{
      float4 depth = 0;
      float4 accumulatedOpacity = 0.0;
      uint4 voxel_x, voxel_y, voxel_z;
      
      // compute the inter-sample interval vector
			float4 interSampleDistance = 1.0;
      vInterval = Vector<float4>(vRayDirection) * interSampleDistance;

      if (flc_bSpaceLeaping) {
					if(!SpaceLeap(vRayDirection, flc_xStep, flc_yStep, flc_zStep, flc_xScl, flc_yScl, flc_zScl,
                        flc_pSLDvol, flc_dfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,
                        leapDistance, point, depth, flc_volMaxX, flc_volMaxY, flc_volMaxZ))
						// Ray hits nothing
						return false;
      }
      
      while (accumulatedOpacity < m_fOpacityThreshold) {

        FastTruncFloatToIntXYZ(voxel_x,voxel_y,voxel_z,(point.m_x*flc_xScl),(point.m_y*flc_yScl),(point.m_z*flc_zScl));
        float4 x_diff = point.m_x*flc_xScl - voxel_x;
        float4 y_diff = point.m_y*flc_yScl - voxel_y;
        float4 z_diff = point.m_z*flc_zScl - voxel_z;
        
        if (voxel_x < 2 || voxel_x > flc_volMaxX - 2 ||  
            voxel_y < 2 || voxel_y > flc_volMaxY - 2 ||  
            voxel_z < 2 || voxel_z > flc_volMaxZ - 2)
          break;

        const uint4 indexOffset = voxel_x * flc_xStep + voxel_y * flc_yStep + voxel_z * flc_zStep;
        const uint1 * dptr = flc_pOpacityVol + indexOffset;

        double value = Trilin(x_diff,y_diff,z_diff,
                              *(dptr), 
                              *(dptr + flc_xStep), 
                              *(dptr + flc_yStep), 
                              *(dptr + flc_xyStep),
                              *(dptr + flc_zStep), 
                              *(dptr + flc_xzStep),
                              *(dptr + flc_yzStep),
                              *(dptr + flc_xyzStep));
        accumulatedOpacity += value*UNSIGNEDCHAR_TO_FLOAT*(1.0F-accumulatedOpacity);
        
        leapDistance = flc_pSLDvol[indexOffset]*flc_dfbScale - flc_fDfbSpaceLeapConversationFactor;
        if (leapDistance <= flc_fDfbSpaceLeapThreshold || !flc_bSpaceLeaping) {
          // if no leap, be sure to still STEP along ray
          point += vInterval;
          depth += interSampleDistance;
        } else { // space leaping after first sample
          if(!SpaceLeap(vRayDirection, flc_xStep, flc_yStep, flc_zStep, flc_xScl, flc_yScl, flc_zScl,
                        flc_pSLDvol, flc_dfbScale, flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,
                        leapDistance, point, depth, flc_volMaxX, flc_volMaxY, flc_volMaxZ))
            // Leap outside the volume, hit nothing
            break;
        } // endif leapDistance > 1
          
      } // endwhile not early ray termination
      
      if(accumulatedOpacity>1.0) accumulatedOpacity=1.0;
      alphaValue = accumulatedOpacity*255;
			return true;
}

#define CAST_ONE_RAY																																																		\
        Point<float4> ptWC = viewpoint;																																									\
				if(!CastSingleRayExternal(																																											\
														ptWC,																																												\
														viewpointVC,																																								\
													  vRayDirection,																																							\
														bOutside,																																										\
														vol_min_x,																																									\
														vol_min_y,																																									\
														vol_min_z,																																									\
														vol_max_x,																																									\
														vol_max_y,																																									\
														vol_max_z,																																									\
														flagX, flagY, flagZ,																																				\
														flc_xScl,																																										\
														flc_yScl,																																										\
														flc_zScl,																																										\
														v2wXScl, v2wYScl, v2wZScl,																																	\
														pixelOffset,																																								\
														depthOffset,																																								\
														firstLeapDistance,																																					\
                            flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,                            \
														flc_pSLDvol,																																								\
													  flc_xStep,																																									\
													  flc_yStep,																																									\
														flc_zStep,																																									\
														flc_dfbScale																																								\
													))																																														\
				  continue;																																																			\
																																																												\
        float4 leapDistance = firstLeapDistance;																																				\
        float4 alphaValue = 0, depthValue =0;															\
        if(!CastSingleRaySp(u, v,																																												\
											      flc_bSpaceLeaping,																																					\
                            vRayDirection,																								                              \
                            ptWC,																																												\
                            leapDistance,																																								\
                            flc_xStep, flc_yStep, flc_zStep,																														\
                            flc_xyStep, flc_yzStep, flc_xzStep, flc_xyzStep,                                     				\
                            flc_xScl, flc_yScl, flc_zScl,                                                               \
                            vol_max_x, vol_max_y, vol_max_z,				                              											\
                            flc_pSLDvol,																																								\
                            flc_pOpacityVol,																																				\
                            alphaValue, depthValue,																		\
                            flc_dfbScale,																																								\
                            flc_fDfbSpaceLeapConversationFactor, flc_fDfbSpaceLeapThreshold,                            \
                            vInterval))																																				  				\
					continue;																																																			\
				/* store result*/																																																\
        *(pDestinationPixel + pixelOffset    ) = (unsigned char) alphaValue;																							\
        *(pDestinationPixel + pixelOffset + 1) = (unsigned char) alphaValue;																						\
        *(pDestinationPixel + pixelOffset + 2) = (unsigned char) alphaValue;																							\
        *(pDestinationPixel + pixelOffset + 3) = (unsigned char) alphaValue;																						\
        *(pDepthPixel + depthOffset)           = depthValue;																														
//End CAST_ONE_RAY


// Similar to RaycastAS(), but allow viewpoint to be outside of the volume
// The volume to render and the rendered results are stored in the RenderContext, rc
void VolumeRender::RaycastASExternal(const int4 id,	RenderContext & rc, Dataset & dataset)
{
	const uint4 stride = 1;

	LinVolUC& sld = *(rc.m_pSld);
	LinVolUC& opacityVol = *(rc.m_pVolOpa);
  const Point<float4>& viewpoint = rc.m_viewpoint;
  const Vector<float4>& vViewDirection = rc.m_vViewDirection;
  const Vector<float4>& vUpDirection = rc.m_vUpDirection;
  const Vector<float4>& vDownDirection = rc.m_vDownDirection;
  const Vector<float4> vRight = rc.m_vRight * rc.m_fRatio;
	
	const float4 flc_fovRadians = rc.m_fFov * M_PI / 180.0F;
  const float4 flc_twoOverSizeX = 2.0f / float4(rc.m_uDisplayWidth);
	const float4 flc_twoOverSizeY = 2.0f / float4(rc.m_uDisplayHeight);
  const float4 flc_tanFov = float4( tan(flc_fovRadians/2));
	
	const int4 vol_min_x = 0;
	const int4 vol_min_y = 0;
	const int4 vol_min_z = 0;
	const int4 vol_max_x = sld.GetDim().m_x -1;
	const int4 vol_max_y = sld.GetDim().m_y -1;
	const int4 vol_max_z = sld.GetDim().m_z -1;
	
  const bool flc_bAxialShadow = rdrGlobal.m_bAxialShadow & !dataset.m_submarine.IsFlying();
  const bool flc_bSagittalShadow = rdrGlobal.m_bSagittalShadow & !dataset.m_submarine.IsFlying();
  const bool flc_bCoronalShadow = rdrGlobal.m_bCoronalShadow & !dataset.m_submarine.IsFlying();
  const bool flc_bObliqueShadow = rdrGlobal.m_bObliqueShadow & !dataset.m_submarine.IsFlying();
  const bool flc_bGridShadow = rdrGlobal.m_bGridShadow & !dataset.m_submarine.IsFlying();
	const bool flc_bShadow = flc_bAxialShadow || flc_bSagittalShadow || flc_bCoronalShadow || flc_bObliqueShadow;
  const bool flc_bSpaceLeaping = true;

  const float4 flc_fDfbSpaceLeapConversationFactor = Max(GetUnits().m_x,GetUnits().m_y,GetUnits().m_z);
  const float4 flc_fDfbSpaceLeapThreshold = flc_fDfbSpaceLeapConversationFactor;
	
  const uint4 flc_xStep = 1;
  const uint4 flc_yStep = sld.GetDim().m_x;
  const uint4 flc_zStep = sld.GetDim().m_x*sld.GetDim().m_y;
  const uint4 flc_xyStep = flc_xStep+flc_yStep;
  const uint4 flc_xzStep = flc_xStep+flc_zStep;
  const uint4 flc_yzStep = flc_yStep+flc_zStep;
  const uint4 flc_xyzStep = flc_xyStep+flc_zStep;
	
  const Triple<float4> & units = dataset.m_fields.m_ins.GetHeader().GetVoxelUnits();
  const Triple<int4> & dim = dataset.m_fields.m_ins.GetHeader().GetVolDim();
  
  const	float4 v2wXScl = rc.m_fZoomFactor * (float4(dim.m_x * units.m_x)) / float4(sld.GetDim().m_x);
	const	float4 v2wYScl = rc.m_fZoomFactor * (float4(dim.m_y * units.m_y)) / float4(sld.GetDim().m_y);
	const	float4 v2wZScl = rc.m_fZoomFactor * (float4(dim.m_z * units.m_z)) / float4(sld.GetDim().m_z);
	
  const float4 flc_xScl = 1.0f / v2wXScl;
  const float4 flc_yScl = 1.0f / v2wYScl;
  const float4 flc_zScl = 1.0f / v2wZScl;
	
  uint1 * const flc_pSLDvol = (unsigned char *) sld.GetDataPtr();
  uint1 * const flc_pOpacityVol = (unsigned char *) opacityVol.GetDataPtr();
	
  const uint1 flc_currentSeenVal = dataset.m_fields.m_paintVol.m_uCurrentSeenVal;
	
	const int4 renderWidth = rc.m_uRenderWidth;
	const int4 renderHeight = rc.m_uRenderHeight;
	const int4 displayWidth = rc.m_uDisplayWidth;
	const int4 displayHeight = rc.m_uDisplayHeight;

	const uint4 byte_per_pixel = 4;
  const uint4 flc_xStepPixel = byte_per_pixel;
  const uint4 flc_yStepPixel = renderWidth*byte_per_pixel;

  const float4 flc_dfbScale = dataset.m_fields.GetSldScale();
  const float4 flc_colonWallThreshold = dataset.m_fColonWallThreshold;
	
	unsigned char * const dumpVol = NULL;
  const char * const &flc_pGradientVol = NULL;
	
  // perform per-frame setup
  uint4 surfaceCount = 0;
	
	unsigned char * pDestinationPixel = rc.m_pImageBuffer;
	float4 * pDepthPixel = rc.m_pDepthBuffer;
	
  // If parallel compute, which section to work on
  uint4 startV, endV;
  if (id == -1)
  {
    startV = 0;
    endV = displayHeight-stride;
  }
  else if (id == 0) 
  {
    startV = 0;
    endV = displayHeight/2;
  }
  else if (id == 1)
  {
    startV = displayHeight/2;
    endV = displayHeight-stride;
  }
	
  // step through all the pixels in three step as follows: 
  Vector<float4> vHalfway;
  Vector<float4> vInterval;
  Vector<float4> normal;
  Point<float4> point = viewpoint;
  const Vector<float4> vMoveLight(vUpDirection * m_fMoveLightVertDirection +
                                 vRight * m_fMoveLightHorizDirection);
  Normal<float4> vLight1Direction;
  Normal<float4> vLight2Direction(Vector<float4>(vDownDirection) +	
                                 vViewDirection * m_fMoveLightVertDirection +
                                 vRight * m_fMoveLightHorizDirection);
  float4 du,dv;
  Normal<float4> vRayDirection;
  const Point<float4> viewpointVC = Point<float4>(viewpoint.m_x * flc_xScl, viewpoint.m_y * flc_yScl, viewpoint.m_z * flc_zScl);
	
	bool bOutside = false;
  int4 voxel_x, voxel_y, voxel_z;
	FastTruncFloatToIntXYZ(voxel_x,voxel_y,voxel_z,(point.m_x*flc_xScl),(point.m_y*flc_yScl),(point.m_z*flc_zScl));				

  float4 firstLeapDistance;
	int flagX, flagY, flagZ;
	
	if(voxel_x < vol_min_x)
		flagX = -1;
	else if(voxel_x > vol_max_x)
		flagX = 1;
	else
		flagX = 0;
	
	if(voxel_y < vol_min_y)
		flagY = -1;
	else if(voxel_y > vol_max_y)
		flagY = 1;
	else
		flagY = 0;
	
	if(voxel_z < vol_min_z)
		flagZ = -1;
	else if(voxel_z > vol_max_z)
		flagZ = 1;
	else
		flagZ = 0;
	
  if ((flagX==0) && (flagY==0) && (flagZ==0))
		firstLeapDistance	= flc_pSLDvol[voxel_x*flc_xStep + voxel_y*flc_yStep +
																		voxel_z*flc_zStep]*flc_dfbScale - flc_fDfbSpaceLeapConversationFactor;
	else
		bOutside = true;
	
	const int4 step = 2.0 * stride;
	Vector<float4> vRayDirVC;
  
  /**************************************************************************/
  /**************************************************************************/
  /******************** (1) ray casting corner pixels ***********************/
  /**************************************************************************/
  /**************************************************************************/
  {
    
    for (uint4 v=startV; v<endV; v+=step) {
      dv = float4(v) * flc_twoOverSizeY - 1.0F;
      Vector<float4> vRowDirection(vViewDirection + vUpDirection * dv * flc_tanFov);
      for (uint4 u=0; u<displayWidth; u+=step) {
        du = float4(u) * flc_twoOverSizeX - 1.0F;

        uint4 pixelOffset = (v * flc_yStepPixel + u * flc_xStepPixel);
        uint4 depthOffset = (u + v * renderWidth);
        
        //m_rayCount++;
        vRayDirection = Normal<float4>( vRowDirection + vRight * du * flc_tanFov );
        vLight1Direction = Normal<float4>( Vector<float4>(vRayDirection) + vMoveLight );
 
				CAST_ONE_RAY

      } // end pixel column
    } // end pixel row
  } // end of step (1).
  
  const int4 xStepPixel = flc_xStepPixel * stride;
	const int4 yStepPixel = flc_yStepPixel * stride;
  /**************************************************************************/
  /**************************************************************************/
  /************** (2) compute vacant pixels in non vacant rows **************/
  /**************************************************************************/
  /**************************************************************************/
#ifdef FINAL_RELEASE
  try
#endif
  {
    for (uint4 v=startV; v<endV; v+= step) {
      dv = float4(v) * flc_twoOverSizeY - 1.0F;
      Vector<float4> vRowDirection(vViewDirection + vUpDirection * dv * flc_tanFov);
      for (uint4 u=0+stride; u<displayWidth-stride; u+= step) {
        du = float4(u) * flc_twoOverSizeX - 1.0F;
        
        uint4 pixelOffset = (v * flc_yStepPixel + u * flc_xStepPixel);
				// All the pixels values are set to zero before the rendering
				// Assume non of the rendered pixel has a red component of zero
				if(*(pDestinationPixel + pixelOffset    ) > 0)
					continue;
        uint4 depthOffset = u + v * renderWidth;
        float4 prevDepth = *(pDepthPixel + depthOffset - stride);
        float4 nextDepth = *(pDepthPixel + depthOffset + stride);
        if ((abs(prevDepth-nextDepth))<Depth_Threshold) {
          uint1 prevRed =  *(pDestinationPixel + pixelOffset - xStepPixel);
          uint1 nextRed =  *(pDestinationPixel + pixelOffset + xStepPixel);
          uint1 prevGreen =  *(pDestinationPixel + pixelOffset - xStepPixel + 1);
          uint1 nextGreen =  *(pDestinationPixel + pixelOffset + xStepPixel + 1);
          uint1 prevBlue  =  *(pDestinationPixel + pixelOffset - xStepPixel + 2);
          uint1 nextBlue  =  *(pDestinationPixel + pixelOffset + xStepPixel + 2);
          uint1 prevAlpha  =  *(pDestinationPixel + pixelOffset - xStepPixel + 3);
          uint1 nextAlpha  =  *(pDestinationPixel + pixelOffset + xStepPixel + 3);
            
          if ( ( (abs(prevRed  -nextRed  ))<Color_Threshold ) &&
               ( (abs(prevGreen-nextGreen))<Color_Threshold ) &&
               ( (abs(prevBlue -nextBlue ))<Color_Threshold ) ) {
						uint4 numPixels = stride * 2;
						float4 deltaRed		= ((float4)nextRed   - prevRed   ) / numPixels;
						float4 deltaGreen	= ((float4)nextGreen - prevGreen ) / numPixels;
						float4 deltaBlue		= ((float4)nextBlue  - prevBlue  ) / numPixels;
						float4 deltaAlpha	= ((float4)nextAlpha - prevAlpha ) / numPixels;
						float4 deltaDepth	= ((float4)nextDepth - prevDepth ) / numPixels;
						float4 red, green, blue, alpha, depth;
						red = prevRed; green = prevGreen; blue = prevBlue; alpha= prevAlpha, depth = prevDepth;
						for(int i=0; i < numPixels-1; i++){
							red  += deltaRed;
							green += deltaGreen;
							blue += deltaBlue;
							alpha += deltaAlpha;
							depth += deltaDepth;

							*(pDestinationPixel + pixelOffset     + i * flc_xStepPixel) = (unsigned char)(red);
							*(pDestinationPixel + pixelOffset + 1 + i * flc_xStepPixel) = (unsigned char)(green);
							*(pDestinationPixel + pixelOffset + 2 + i * flc_xStepPixel) = (unsigned char)(blue);
							*(pDestinationPixel + pixelOffset + 3 + i * flc_xStepPixel) = (unsigned char)(alpha);
           
							*(pDepthPixel + depthOffset + i ) = depth;
						}

            continue;
          }
        }
        
        //m_rayCount++;
        vRayDirection =    Normal<float4>( vRowDirection + vRight * du * flc_tanFov );
        vLight1Direction = Normal<float4>( Vector<float4>(vRayDirection) + vMoveLight );

				CAST_ONE_RAY
      } // end pixel column
    } // end pixel row
  }  // end of step (2)
#ifdef FINAL_RELEASE
	catch(...){
					printf("Error \n");				
	}
#endif
  /**************************************************************************/
  /**************************************************************************/
  /****************** (3) compute all pixels in vacant rows *****************/
  /**************************************************************************/
  /**************************************************************************/
#ifdef FINAL_RELEASE
  try
#endif
	{  
    for (uint4 v=startV+stride; v<endV; v+=step) 
    {
      dv = float4(v) * flc_twoOverSizeY - 1.0F;
      Vector<float4> vRowDirection(vViewDirection + vUpDirection * dv * flc_tanFov);
      for (uint4 u=0; u<displayWidth; u+=stride) 
      {
        du = float4(u) * flc_twoOverSizeX - 1.0F;

        uint4 pixelOffset = (v * flc_yStepPixel + u * flc_xStepPixel);
				// All the pixels values are set to zero before the rendering
				// Assume non of the rendered pixel has a red component of zero
				if(*(pDestinationPixel + pixelOffset    ) > 0)
					continue;
        uint4 depthOffset = u + v * renderWidth;

        float4 prevDepth = *(pDepthPixel + depthOffset - renderWidth*stride);
        float4 nextDepth = *(pDepthPixel + depthOffset + renderWidth*stride);
        
				if ((abs(prevDepth-nextDepth))<Depth_Threshold) 
        {
          uint1 prevRed =  *(pDestinationPixel + pixelOffset - yStepPixel);
          uint1 nextRed =  *(pDestinationPixel + pixelOffset + yStepPixel);
          uint1 prevGreen =  *(pDestinationPixel + pixelOffset - yStepPixel + 1);
          uint1 nextGreen =  *(pDestinationPixel + pixelOffset + yStepPixel + 1);
          uint1 prevBlue  =  *(pDestinationPixel + pixelOffset - yStepPixel + 2);
          uint1 nextBlue  =  *(pDestinationPixel + pixelOffset + yStepPixel + 2);
          uint1 prevAlpha  =  *(pDestinationPixel + pixelOffset - yStepPixel + 3);
          uint1 nextAlpha  =  *(pDestinationPixel + pixelOffset + yStepPixel + 3);
          
          if ( ( (abs(prevRed  -nextRed  ))<Color_Threshold ) &&
               ( (abs(prevGreen-nextGreen))<Color_Threshold ) &&
               ( (abs(prevBlue -nextBlue ))<Color_Threshold ) ) 
          {
							uint4 numPixels = stride * 2;
							float4 deltaRed		= ((float4)nextRed   - prevRed   ) / numPixels;
							float4 deltaGreen	= ((float4)nextGreen - prevGreen ) / numPixels;
							float4 deltaBlue		= ((float4)nextBlue  - prevBlue  ) / numPixels;
							float4 deltaAlpha	= ((float4)nextAlpha - prevAlpha ) / numPixels;
							float4 deltaDepth	= ((float4)nextDepth - prevDepth ) / numPixels;
							float4 red, green, blue, alpha, depth;
							red = prevRed; green = prevGreen; blue = prevBlue; alpha= prevAlpha, depth = prevDepth;
							for(int i=0; i < numPixels-1; i++){
								red  += deltaRed;
								green += deltaGreen;
								blue += deltaBlue;
								alpha += deltaAlpha;
								depth += deltaDepth;

								*(pDestinationPixel + pixelOffset     + i * flc_yStepPixel) = (unsigned char)(red);
								*(pDestinationPixel + pixelOffset + 1 + i * flc_yStepPixel) = (unsigned char)(green);
								*(pDestinationPixel + pixelOffset + 2 + i * flc_yStepPixel) = (unsigned char)(blue);
								*(pDestinationPixel + pixelOffset + 3 + i * flc_yStepPixel) = (unsigned char)(alpha);
           
								*(pDepthPixel + depthOffset + i ) = depth;
							}
            continue;
          }
        }

        //m_rayCount++;
        vRayDirection    = Normal<float4>( vRowDirection + vRight * du * flc_tanFov);
        vLight1Direction = Normal<float4>( Vector<float4>(vRayDirection) + vMoveLight);

				CAST_ONE_RAY
      } // end pixel column
    } // end pixel row
  }  // end of step (3)
#ifdef FINAL_RELEASE
	catch(...){
					printf("Error \n");				
	}
#endif
} // VolumeRender::RaycastASExternal()


// $Log: VolumeRenderASExternal.C,v $
// Revision 1.3  2006/09/26 19:33:25  frank
// replaced hardcoded numbers with constants
//
// Revision 1.2  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/08/29 13:52:03  vxconfig
// Issue #4390: Prevented showing outside colon in transparent rendering
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.9.2.1.4.1  2005/04/06 20:35:04  frank
// Issue #3994: Allowed non-square aspect ratios in volume rendering
//
// Revision 3.9.2.1  2003/05/22 19:59:43  kevin
// Tracker issue 2784: This is now fixed correctly. The  system will now use
// more memory and the external DFB volume is sotred in it's own volume
// so the internal and enternal will not "leak" together.
//
// This also indirectly fixes the black holes problem
//
// Revision 3.9  2003/01/22 22:30:28  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.8  2002/10/21 21:33:39  kevin
// Oops accidentally removed this when removing the old S/P registration stuff
//
// Revision 3.6  2002/04/12 02:20:41  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.5  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.4.2.1  2002/02/10 20:22:29  kevin
// Major fixes for TTP rendering algorithm
//
// Revision 3.4  2002/01/31 15:58:10  jmeade
// Unreferenced var.
//
// Revision 3.3  2002/01/04 16:33:19  kevin
// Some cleanup after the hacking to get TTP rendering without ses,set,sec
//
// Revision 3.2  2002/01/03 02:30:52  kevin
// Fixed TTP rendering now that set, ses and sec are gone
//
// Revision 3.1  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 11 2001 10:11:46   binli
// issue 1122: incorrect transparent overview
// fixed: settings of Dim/Units in VolumeRender class did not follow the change of study.
// (not only 'Binnie', but all cases have the same problem. this one is obvious)
// 
//    Rev 2.2   Oct 10 2001 11:56:24   binli
// issue 1027: transparent rendering of overview reflect selection
// 
//    Rev 2.1   Oct 08 2001 21:37:52   jmeade
// Specify study to be rendered (removed legacy assumption of rdrGlobal.m_pStudy)
// Resolution for 1078: Registration support
// 
//    Rev 2.0   Sep 16 2001 23:40:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:26   ingmar
// Initial revision.
// Revision 1.28  2001/05/25 20:29:00  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.27  2001/05/21 15:35:13  liwei
// no message
//
// Revision 1.26  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.25  2001/04/18 19:11:37  liwei
// m_pOpacityVol changed to m_opacityVol
//
// Revision 1.24  2001/04/13 11:53:38  frank
// Protected some members of Fields.
//
// Revision 1.23  2001/04/11 20:50:27  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.22  2001/04/11 14:07:28  frank
// Renamed class members of Fields.
//
// Revision 1.21  2001/04/11 11:58:51  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.20  2001/03/21 21:14:24  liwei
// Updated RenderContext class for coding guideline compliance.
//
// Revision 1.19  2001/03/21 14:59:48  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.18  2001/03/16 17:11:09  frank
// Updated Fields class variables for new coding standards.
//
// Revision 1.17  2001/03/16 16:24:34  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.16  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.15  2001/03/05 22:43:58  liwei
// Code rearranged to comply with the coding standard
//
// Revision 1.14  2001/02/14 21:38:00  liwei
// Weird problem walked around
//
// Revision 1.13  2001/02/12 21:17:51  liwei
// TTP rendering quality improved, with a little speed sacrifice
//
// Revision 1.12  2001/02/10 01:38:33  kevin
// Minor change
//
// Revision 1.11  2001/02/10 01:19:55  liwei
// Boundary handling changed again
//
// Revision 1.10  2001/02/10 01:16:10  liwei
// Boundary handling changed
//
// Revision 1.9  2001/02/10 00:46:01  liwei
// The dissappearing problem of TTP after switching among views fixed
//
// Revision 1.8  2001/02/05 22:18:33  liwei
// Minor change, playing with jitter
//
// Revision 1.7  2001/01/30 23:44:59  liwei
// pVolOpa moved to Study from RenderContext, so that when toggle between
// supine and prone, it is initialized properly
//
// Revision 1.6  2001/01/25 17:56:39  liwei
// Code cleaned further
//
// Revision 1.5  2001/01/25 17:38:59  liwei
// Code cleaned
//
// Revision 1.4  2001/01/25 15:59:40  liwei
// Jittering for volume rendering in the overview
//
// Revision 1.3  2001/01/24 22:36:44  liwei
// Transfer function for non-transparent external view tuned
//
// Revision 1.2  2001/01/24 20:21:40  frank
// Compiles again.
//
// Revision 1.1  2001/01/24 19:21:38  liwei
// Software volume rendering for overview
//
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/VolumeRenderASExternal.C,v 1.3 2006/09/26 19:33:25 frank Exp $
// $Id: VolumeRenderASExternal.C,v 1.3 2006/09/26 19:33:25 frank Exp $
