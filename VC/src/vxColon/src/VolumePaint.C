// $Id: VolumePaint.C,v 1.10.2.5 2007/06/22 00:10:03 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Kevin Kreeger (kevin@viatronix.net)


/**
 * Class to implement painting, marking and counting of visualized surfaces
 */


// includes

#include "stdafx.h"
#include <stdio.h>
#include <memory.h>
#include "VolumePaint.h"
#include "ReaderGlobal.h"
#include "vxBlockVolumeIterator.h"
#include "RenderUtil.h"


// defines
#define FILE_REVISION "$Revision: 1.10.2.5 $"


// namespaces
using namespace ReaderLib;


// static member initializations
const std::string VolumePaint::sHdrVarName_CurrentSurfaceCount("VolumePaint_CurrentSurfaceCount");
const std::string VolumePaint::sHdrVarDesc_CurrentSurfaceCount("Number of surface voxels currently viewed");
const std::string VolumePaint::sHdrVarName_TotalSurfaceCount("VolumePaint_TotalSurfaceCount");
const std::string VolumePaint::sHdrVarDesc_TotalSurfaceCount("Total number of viewable surface voxels");


/**
 * Constructor.
 */
VolumePaint::VolumePaint()
{
  Reset();
} // constructor


/**
 * Constructor 
 *
 * @param dim               Dimensions
 * @param uiBitsPerVoxel    Number of bits per voxel.
 * @param units             Units.
 * @param unitType          Type of voxels.
 * @param background        Background value.
 * @param ext               Volume extension.
 * @param sFileName         Filename.
 * @param sTitle            Title.
 * @param sAuthor           Author.
 * @param date              Date.
 */
VolumePaint::VolumePaint(const Triple<uint4> & dim, const uint4 & uBitsPerVoxel,
                         const Triple<float4> & units, hdr::UnitTypeEnum eUnitType,
                         const uint1 & uBackground, hdr::ExtensionEnum eExt,
                         const std::string & sFileName, const std::string & sTitle,
                         const std::string & sAuthor, const vx::Date & date)
:LinVolUC(dim, uBitsPerVoxel, units, eUnitType, uBackground, eExt, sFileName, sTitle, sAuthor, date)
{
  Reset();
} // constructor


/**
 * Destructor.
 */
VolumePaint::~VolumePaint()
{
} // destructor


/**
 * Resets all variables that change whenever a new study is loaded.
 */
void VolumePaint::Reset()
{
  m_uCurrentSeenVal = NOTCOUNTMASK;
  m_bSurfacesNeedReinit = false;
  m_uTotalSurfaceCount = 1;
  m_uCurrentSurfaceCount = 0;
} // Reset()


/**
 * Returns true if the world point is visible
 */
bool Visible(Point<float4> viewPointWorld, Point<float4> centerWorld, vxBlockVolume<uint1> & insVol)
{
  Vector<float4> vViewPoint2Center = centerWorld - viewPointWorld;
  float4 fDistViewPoint2Center = vViewPoint2Center.GetLength();
  float4 fParametricStepLength = 1/fDistViewPoint2Center;

  if( vViewPoint2Center == Vector< float4 >( 0, 0, 0 ) )
    vViewPoint2Center = Vector< float4 >( 1, 0, 0 );
  vViewPoint2Center.Normalize();

  vxBlockVolumeIterator<uint1> insIter( insVol );
  for(float4 fParametricDist = 0; fParametricDist < 1.0; fParametricDist += fParametricStepLength)
  {
    Vector<float4> viewToCenterVec = Vector<float4>( (viewPointWorld*fParametricDist) + Vector<float4>(centerWorld*(1.0F-fParametricDist)) );
    Vector<float4> thisPos = RenderUtil::GetWorldToVolumeMatrix( insVol.GetHeader() ) * viewToCenterVec;
    if ( ! insIter.Peek( thisPos.m_x+0.5F,thisPos.m_y+0.5F,thisPos.m_z+0.5F ) )
      return false;
  }

  return true;
} // Visible()


/**
 * Finds the best viewing position for the given patch
 *
 * @param dataset     the dataset
 * @param thisPatch   patch
 * @param segment     segment
 * @param vAveNormal  ...
 */
void VolumePaint::MakeViewingGeometry(Dataset & dataset, Patch & thisPatch, Segment & segment, Vector<float4> vAveNormal)
{
  Skeleton & skeleton = dataset.m_segmentList.GetSkeleton(thisPatch.m_iSegment);
  /////////////////////////////////////////////////////////////////////////
  // Calculate a viewpoint and direction from the CSI
  /////////////////////////////////////////////////////////////////////////
  Point<float4> csi_viewPointWorld = skeleton.GetSkelNode(thisPatch.m_uCSI).m_vWorldPos;
  Vector<float4> csi_viewDirWorld = thisPatch.m_centerWorld - csi_viewPointWorld;
  float4 distPatchToCSI = csi_viewDirWorld.GetLength();
  csi_viewDirWorld.Normalize();
  
  /////////////////////////////////////////////////////////////////////////
  // Take the average normal for the patch, and move
  // away from the patch along this normal direction the radius of the colon at this point.
  // Sometimes this not within the colon, so what we really do is walk from the CSI towards
  // that point as long as we are still inside the volume. 
  /////////////////////////////////////////////////////////////////////////
  Vector<float4> vCSI2Normal = (thisPatch.m_centerWorld + (vAveNormal*distPatchToCSI)) - csi_viewPointWorld;
  float4 distCSI2Normal = vCSI2Normal.GetLength();
  vCSI2Normal.Normalize();
  const float4 distFromWallThresh = 3;
  float4 step = distFromWallThresh;
  Point<float4> thisPos = dataset.m_fields.m_volumeRender.GetWorldToVolumeMatrix() * ( csi_viewPointWorld+(vCSI2Normal*step) );
  vxBlockVolumeIterator<uint1> insIter( dataset.m_fields.m_ins );
  while(thisPos.m_x > 0.0 && thisPos.m_x < dataset.m_vDim.m_x &&
        thisPos.m_y > 0.0 && thisPos.m_y < dataset.m_vDim.m_y &&
        thisPos.m_z > 0.0 && thisPos.m_z < dataset.m_vDim.m_z &&
        step < distCSI2Normal &&
        insIter.Peek( thisPos.m_x+0.5F, thisPos.m_y+0.5F, thisPos.m_z+0.5F ) )
  {
    step++;
    thisPos = dataset.m_fields.m_volumeRender.GetWorldToVolumeMatrix() * ( csi_viewPointWorld+(vCSI2Normal*step) );
  }
  Point<float4> normal_viewPointWorld = csi_viewPointWorld + (vCSI2Normal*(step-distFromWallThresh));
  Vector<float4> normal_viewDirWorld = thisPatch.m_centerWorld - normal_viewPointWorld;
  normal_viewDirWorld.Normalize();
  
  /////////////////////////////////////////////////////////////////////////
  // follow the navigation field away from the closer end of the segment
  // (in case the patch is right up against the end) and store 3
  // positions back from where the center becomes not visible
  /////////////////////////////////////////////////////////////////////////
  int4 count = 0;
  bool bFromStart = (thisPatch.m_uCSI > skeleton.GetNumOfElements() / 2) ^ segment.IsRegularDirection(); // if we selected near the after end of the skeleton
  Point<float4> testPosition  = thisPatch.m_centerWorld;
  Point<float4> lastPosition1 = thisPatch.m_centerWorld;
  Point<float4> lastPosition2 = thisPatch.m_centerWorld;
  Point<float4> lastPosition3 = thisPatch.m_centerWorld;
  do
  {
    // Translate the point along the potential field
    Point<float4> positionVolume(testPosition.m_x/dataset.m_vUnits.m_x,
                                 testPosition.m_y/dataset.m_vUnits.m_y,
                                 testPosition.m_z/dataset.m_vUnits.m_z);
    Vector<float4> vPotentialVector = dataset.m_fields.GetMinimumPotentialDirection(positionVolume, bFromStart);
    vPotentialVector.Normalize();
    vPotentialVector *= 0.5F;
    // update the position
    lastPosition3 = lastPosition2;
    lastPosition2 = lastPosition1;
    lastPosition1 = testPosition;
    testPosition += vPotentialVector;
  }   while (Visible(testPosition ,thisPatch.m_centerWorld, dataset.m_fields.m_ins) &&
         (testPosition-thisPatch.m_centerWorld).GetLength() < distPatchToCSI && count++ < 100); // iterate walk loop
  
  Point<float4> fNav_viewPointWorld = lastPosition3;
  Vector<float4> nav_viewDirWorld = thisPatch.m_centerWorld - fNav_viewPointWorld;
  if( nav_viewDirWorld == Vector<float4>(0,0,0) )
    nav_viewDirWorld = Vector<float4>(1,0,0);
  nav_viewDirWorld.Normalize();
  
  /////////////////////////////////////////////////////////////////////////
  // set the viewpoint and viewdir according to which one is visible
  /////////////////////////////////////////////////////////////////////////
  if (Visible(normal_viewPointWorld,thisPatch.m_centerWorld, dataset.m_fields.m_ins))
  {
    thisPatch.m_viewPointWorld = normal_viewPointWorld;
    thisPatch.m_vViewDirWorld  = normal_viewDirWorld;
  }
  else if (Visible(csi_viewPointWorld,thisPatch.m_centerWorld, dataset.m_fields.m_ins))
  {
    thisPatch.m_viewPointWorld = csi_viewPointWorld;
    thisPatch.m_vViewDirWorld  = csi_viewDirWorld;
  }
  else if (Visible(fNav_viewPointWorld,thisPatch.m_centerWorld, dataset.m_fields.m_ins))
  {
    thisPatch.m_viewPointWorld = fNav_viewPointWorld;
    thisPatch.m_vViewDirWorld  = nav_viewDirWorld;
  }
  else // should be visible cause it was called during the creation of this last chance
  {
    thisPatch.m_viewPointWorld = csi_viewPointWorld;
    thisPatch.m_vViewDirWorld  = csi_viewDirWorld;
  }
} // MakeViewingGeometry()


/**
 * Mark all the potentially visible surface voxels as unseen.
 *
 * @param segmentList   list of colon segments
 * @param insVol        inside volume
 */
void VolumePaint::InitializeMarkedSurfaces(SegmentList & segmentList, vxBlockVolume<uint1> & insVol)
{
  uint1* pPaintVol = (unsigned char *) GetDataPtr();
  if (pPaintVol == NULL)
  {
    LogWrn("Paint volume not yet available.","Dataset", "MarkSurfaces");
    return;
  } // null memory exception

  m_uCurrentSurfaceCount = 0;
  m_uTotalSurfaceCount = 0;
  uint4& ruTotalSurfaceCount = m_uTotalSurfaceCount;

  // Ignore a few voxels around the border 
  // and we grow bigger than this size anyway
  int4 uStartX = 3;
  int4 uStartY = 3;
  int4 uStartZ = 3;
  int4 uStopX = GetDim().m_x-3;
  int4 uStopY = GetDim().m_y-3;
  int4 uStopZ = GetDim().m_z-3;

  uint4 uStepY = GetDim().m_x;
  uint4 uStepZ = GetDim().m_x*GetDim().m_y;

  memset(pPaintVol,0,GetDim().m_x*GetDim().m_y*GetDim().m_z);

  for (int4 uSegmentIndex=0; uSegmentIndex<segmentList.GetNumSelectedSegments(); uSegmentIndex++)
  {
    // get the current segment
    Segment & segment = segmentList.GetSegment(uSegmentIndex);
    uint1 uThisIndex = segment.GetIdentifier();

    // Loop through the ins volume marking the surface voxels in the paintVolume
    // see InsideDetector::LabelOutsideSurface for similar functionality
    vxBlockVolumeIterator<uint1> insIter( insVol );
    // efficiently traverse the block volume
    for ( insIter.FirstNonBackgroundBlockZYX(); insIter.IsNotAtEnd(); insIter.NextNonBackgroundBlockZYX() )
    {
      for ( ; insIter.IsNotAtEndOfBlock(); insIter.NextNonBackgroundZYXinsideBlock() )
      {

        if ( insIter.GetVoxel() == uThisIndex )
        {
          Point<unsigned short> pos( insIter.GetPos() );
          int4 iX = pos.m_x;
          int4 iY = pos.m_y;
          int4 iZ = pos.m_z;
          if (iX>uStartX && iX<uStopX &&
              iY>uStartY && iY<uStopY &&
              iZ>uStartZ && iZ<uStopZ)
          {
            uint4 uIndexOffset = iZ*uStepZ + iY*uStepY + iX;

            if ( insIter.Peek1px0py0pz()==0 && pPaintVol[uIndexOffset + 1     ]==0 ) { pPaintVol[uIndexOffset + 1     ] = COUNTMASK; ruTotalSurfaceCount++; }
            if ( insIter.Peek1nx0py0pz()==0 && pPaintVol[uIndexOffset - 1     ]==0 ) { pPaintVol[uIndexOffset - 1     ] = COUNTMASK; ruTotalSurfaceCount++; }
            if ( insIter.Peek0px1py0pz()==0 && pPaintVol[uIndexOffset + uStepY]==0 ) { pPaintVol[uIndexOffset + uStepY] = COUNTMASK; ruTotalSurfaceCount++; }
            if ( insIter.Peek0px1ny0pz()==0 && pPaintVol[uIndexOffset - uStepY]==0 ) { pPaintVol[uIndexOffset - uStepY] = COUNTMASK; ruTotalSurfaceCount++; }
            if ( insIter.Peek0px0py1pz()==0 && pPaintVol[uIndexOffset + uStepZ]==0 ) { pPaintVol[uIndexOffset + uStepZ] = COUNTMASK; ruTotalSurfaceCount++; }
            if ( insIter.Peek0px0py1nz()==0 && pPaintVol[uIndexOffset - uStepZ]==0 ) { pPaintVol[uIndexOffset - uStepZ] = COUNTMASK; ruTotalSurfaceCount++; }
          } // if not along the edges
          
        } // if part of this segment
            
      } // for all non-empty voxels
    } // for all non-empty blocks
    
  } // for all segments

  if (m_uTotalSurfaceCount == 0) m_uTotalSurfaceCount = 1;

  // The paint volume surfaces are now newly initialized, all unsullied and clean
  m_bSurfacesNeedReinit = false;
} // InitializeMarkedSurfaces


Point<uint4> gMissed_Sum;
Vector<float4> gMissed_vSumNormals;
uint4 gCount;
uint4 gSumCSI;
uint4 gCountCSI;
uint2 gMinCSI;
Point<uint2> gMinCSILocation;
std::vector< Point<uint2> > gPatchPoints;
/**
 * Description
 *
 * @param pPaintVol              
 * @param maskVol           
 * @param volumeRender   
 * @param seedVoxel        
 * @param zStep                   
 * @param yStep                   
 * @return        ...
 */
uint4 CheckMissedAreaNeighbors( uint1 * pPaintVol, BlockVolB & maskVol, VolumeRender & volumeRender,
                                uint4 zStep, uint4 yStep, vxBlockVolume<uint2> & csiVol, Dataset & dataset )
{
  Point<uint2> seedVoxel = gPatchPoints.back();
  gPatchPoints.pop_back();
  // Accumulate values for average pos and direction
  Vector<uint4> uintSeedVoxel(seedVoxel);
  gMissed_Sum += uintSeedVoxel;
  float4 fDummy;
  gMissed_vSumNormals += volumeRender.CentralDiffGradient(seedVoxel,fDummy);

  // Check all 26-connected neighbors
  uint4 iCount = 1;
  vxBlockVolumeIterator<uint1> insIter( dataset.m_fields.m_ins );
  vxBlockVolumeIterator<uint2> csiIter( csiVol );
  csiIter.SetPos( seedVoxel.m_x, seedVoxel.m_y, seedVoxel.m_z );
  for(int4 z=-1;z<2;z++)
    for(int4 y=-1;y<2;y++)
      for(int4 x=-1;x<2;x++)
      {
        // Check for a possible valid csi at this point
        const uint4 val = csiIter.PeekRel( x, y, z );
        Point<uint2> nextVoxel( seedVoxel.m_x + x, seedVoxel.m_y + y, seedVoxel.m_z + z );
        if ( val != 0 )
        {
          int4 iSegment = dataset.m_segmentList.GetSortedSegmentIndexFromIdentifier( insIter.Peek( nextVoxel.m_x, nextVoxel.m_y, nextVoxel.m_z ) );
          if ( iSegment > -1 )
          {
            gSumCSI += val;
            gCountCSI++;
            if (val < gMinCSI)
            {
              gMinCSI = val;
              gMinCSILocation = nextVoxel;
            }
          }
        }
        
        if ( (pPaintVol[nextVoxel.m_z*zStep+nextVoxel.m_y*yStep+nextVoxel.m_x] & COUNTMASK) && !maskVol.GetVoxel(nextVoxel) )
        {
          gCount++;
          maskVol.SetVoxel(nextVoxel,true);
          gPatchPoints.push_back(nextVoxel);
        } // endif
      } // endfor x,y,z
      
      return (iCount);
} // CheckMissedAreaNeighbors



bool VolumePaint::MakePatch(Dataset & dataset, Patch & thisPatch, Point<uint2> seedPoint, uint1* pPaintVol, BlockVolB  &maskVol)
{
  try
  {
    uint4 zStep = dataset.m_vDim.m_x*dataset.m_vDim.m_y;
    uint4 yStep = dataset.m_vDim.m_x;
    
    // detect patch
    gMissed_Sum = Point<uint4>(0,0,0);
    gMissed_vSumNormals = Vector<float4>(0.0F,0.0F,0.0F);
    gSumCSI = 0;
    gCountCSI = 0;
    gMinCSI = 9999;
    gMinCSILocation = Point<uint2>(0,0,0);
    gPatchPoints.clear();
    gCount = 1;
    maskVol.SetVoxel(seedPoint,true);
    gPatchPoints.push_back(seedPoint);
    while(!gPatchPoints.empty()) CheckMissedAreaNeighbors(pPaintVol, maskVol, dataset.m_fields.m_volumeRender, zStep, yStep, dataset.m_fields.m_csi, dataset);
    uint4 uSurfaceCount = gCount;
    Point<float4> center = Point<float4>(float4(gMissed_Sum.m_x)/float4(uSurfaceCount),
                                        float4(gMissed_Sum.m_y)/float4(uSurfaceCount),
                                        float4(gMissed_Sum.m_z)/float4(uSurfaceCount));
    Vector<float4> vAveNormal = Vector<float4>(-gMissed_vSumNormals.m_x/float4(uSurfaceCount),
                                              -gMissed_vSumNormals.m_y/float4(uSurfaceCount),
                                              -gMissed_vSumNormals.m_z/float4(uSurfaceCount));
    if ( vAveNormal.GetLength() > 0.0F ) vAveNormal.Normalize();
    
    // build patch info
    thisPatch.m_uSurfaceCount = uSurfaceCount;
    thisPatch.m_centerWorld = Point<float4>(center.m_x*dataset.m_vUnits.m_x,
                                            center.m_y*dataset.m_vUnits.m_y,
                                            center.m_z*dataset.m_vUnits.m_z);

    
    if(uSurfaceCount < m_uTotalSurfaceCount/10)
    {
      Point<uint2> startPosition, foundPoint;
      startPosition = center;
      if (dataset.m_fields.FindClosestValidCSI(startPosition, 1, 100, foundPoint))
      {
        vxBlockVolumeIterator<uint1> insIter( dataset.m_fields.m_ins );
        thisPatch.m_iSegment = dataset.m_segmentList.GetSortedSegmentIndexFromIdentifier( insIter.Peek( foundPoint.m_x, foundPoint.m_y, foundPoint.m_z ) );
        
        if ( gCountCSI != 0)
          thisPatch.m_uCSI = gSumCSI/gCountCSI;
        else
        {
          thisPatch.m_uCSI = 0;
          LogWrn("gCountCSI is equal to zero; protecting against division by 0.", "VolumePaint", "MakePatch");
        }

        Segment & segment= dataset.m_segmentList.GetSegment(thisPatch.m_iSegment);
        Skeleton & skeleton = dataset.m_segmentList.GetSkeleton(thisPatch.m_iSegment);
      
        MakeViewingGeometry(dataset, thisPatch, segment, vAveNormal);

        // adjust the distance for the change in the view point
        Point3D<float4> volPos = dataset.m_fields.m_volumeRender.GetWorldToVolumeMatrix() * thisPatch.m_viewPointWorld;
        thisPatch.m_fDistanceFromRectumMM = dataset.GetDistanceFromRectumMM( segment, skeleton, volPos );
      }
      else
      {
        LogWrn("Missed Patch detected with no remotely-close CSI", "VolumePaint", "MakePatch");
        thisPatch.m_iSegment = -1;
        thisPatch.m_uCSI     = 0;
        thisPatch.m_fDistanceFromRectumMM = -1;
        thisPatch.m_viewPointWorld = thisPatch.m_centerWorld;
        thisPatch.m_vViewDirWorld  = Vector<float4>(0,1,0);
      }// endif found/not-found a csi for this point
    }
    else // large patch found
    {
      if(gMinCSI < 9999)
      {           
        vxBlockVolumeIterator<uint1> insIter( dataset.m_fields.m_ins );
        thisPatch.m_iSegment = dataset.m_segmentList.GetSortedSegmentIndexFromIdentifier( insIter.Peek( gMinCSILocation.m_x, gMinCSILocation.m_y, gMinCSILocation.m_z ) );
        vxBlockVolumeIterator<uint2> csiIter( dataset.m_fields.m_csi );
        thisPatch.m_uCSI = csiIter.Peek( gMinCSILocation.m_x, gMinCSILocation.m_y, gMinCSILocation.m_z );
      
        Segment * pSegment = & dataset.m_segmentList.GetSegment(thisPatch.m_iSegment);
        Skeleton * pSkeleton = & dataset.m_segmentList.GetSkeleton(thisPatch.m_iSegment);
      
        thisPatch.m_fDistanceFromRectumMM = pSegment->GetOffsetDistanceFromRectumMM();
        if (pSegment->IsRegularDirection() == true)
        { 
          thisPatch.m_fDistanceFromRectumMM += pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_worldDfs;
        }
        else
        {
          thisPatch.m_fDistanceFromRectumMM += pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_worldDfe;
        }
      
        // sanity check
        if ( thisPatch.m_uCSI > pSkeleton->GetNumOfElements() || gMinCSILocation == Point<uint2>(0,0,0) )
        {
          LogWrn( "Found large missed patch with invalid CSI: " + ToAscii( int4( thisPatch.m_uCSI ) ) + ">" + ToAscii( int4( pSkeleton->GetNumOfElements() ) ), "VolumePaint", "MakePatch" );
          thisPatch.m_iSegment = 1;
          thisPatch.m_uCSI = 0;
          pSegment = & dataset.m_segmentList.GetSegment(thisPatch.m_iSegment);
          pSkeleton = & dataset.m_segmentList.GetSkeleton(thisPatch.m_iSegment);
          thisPatch.m_fDistanceFromRectumMM = pSegment->GetOffsetDistanceFromRectumMM();
        }

        thisPatch.m_viewPointWorld = pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_vWorldPos;
        thisPatch.m_vViewDirWorld  = Vector<float4>(pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_vWorldTan);
    
      }
      else
      {
        LogWrn("Large missed patch found with NO valid CSI at all","VolumePaint","MakePatch");
        thisPatch.m_iSegment = -1;
        thisPatch.m_uCSI     = 0;    
        thisPatch.m_fDistanceFromRectumMM = -2;
        thisPatch.m_viewPointWorld = thisPatch.m_centerWorld;
        thisPatch.m_vViewDirWorld  = Vector<float4>(0,1,0);
      } // endif found at least one valid CSI on large surface
    } // endif small patch or large patch
  } // try
  catch ( ... )
  {
    LogErr( "unhandled exception", "VolumePaint", "MakePatch" );
  }

  // catch the rare case in which the viewpoint is at the origin
  if ( thisPatch.m_fDistanceFromRectumMM < 1 )
  {
    Segment * pSegment = & dataset.m_segmentList.GetSegment(thisPatch.m_iSegment);
    Skeleton * pSkeleton = & dataset.m_segmentList.GetSkeleton(thisPatch.m_iSegment);
    if ( thisPatch.m_viewPointWorld == Point3Df( 0, 0, 0 ) )
    {
      thisPatch.m_viewPointWorld = pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_vWorldPos;
      thisPatch.m_vViewDirWorld = Vector3Df( pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_vWorldTan );
      thisPatch.m_fDistanceFromRectumMM = pSegment->GetOffsetDistanceFromRectumMM();
      if (pSegment->IsRegularDirection() == true)
      { 
        thisPatch.m_fDistanceFromRectumMM += pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_worldDfs;
      }
      else
      {
        thisPatch.m_fDistanceFromRectumMM += pSkeleton->GetSkelNode(thisPatch.m_uCSI).m_worldDfe;
      }
    }
  }

  Point3D<float4> volPos = dataset.m_fields.m_volumeRender.GetWorldToVolumeMatrix() * thisPatch.m_viewPointWorld;
  if ( !dataset.IsPointNearSelectedLumen( volPos ) )
  {
    return false;
  }

  // if this patch passes the features of clinically significant patches we want to consider
  //if ((thisPatch.m_fDistanceFromRectum > 5.0) && (thisPatch.m_uSurfaceCount > 20)) return true;
  if (thisPatch.m_uSurfaceCount > 20) return true;
    
  return false;
}

/**
 *
 */
void VolumePaint::CreateMissedPatchList(Dataset & dataset)
{
  try
  {
    uint1 * pPaintVol = GetDataPtr();

    // empty out any old elements in the vector
    dataset.m_vPatchList.clear();

    int2 xDim = dataset.m_vDim.m_x;
    int2 yDim = dataset.m_vDim.m_y;
    int2 zDim = dataset.m_vDim.m_z;
    uint4 zStep = xDim*yDim;
    uint4 yStep = xDim;

    BlockVolB maskVol(dataset.m_vDim,1, Triple<float4>(1.0,1.0,1.0),hdr::MILLIMETER,false);
    
    // Loop through the ins volume detecting the surface voxel patches
    for ( int4 z = 0; z < zDim; z++ )
      for ( int4 y = 0; y < yDim; y++ )
        for ( int4 x = 0; x < xDim; x++ )
        {
          if ( (pPaintVol[z*zStep+y*yStep+x] & COUNTMASK) && !maskVol.GetVoxelRaw(Point<uint2>(x,y,z)) )
          {
            Patch thisPatch;
            if(MakePatch(dataset,thisPatch,Point<uint2>(x,y,z),pPaintVol,maskVol)) dataset.m_vPatchList.push_back(thisPatch);
          } 
        } // endfor x,y,z

    // First check list.size(), as it returns a <uint> which would cause an infinite loop below
    if (dataset.m_vPatchList.size() > 0)
    {
      qsort((void*)&dataset.m_vPatchList[0], dataset.m_vPatchList.size(), sizeof(Patch), Patch::SurfaceCountGreaterCompare);
    } // endif patch list non-empty
  } // try
  catch ( ... )
  {
    LogErr( "unhandled exception", "VolumePaint", "CreateMissedPatchList" );
  }
} // CreateMissedPatchList()


/**
 * 
 */
void VolumePaint::Transfer3DMarking()
{
  m_uCurrentSeenVal--;

  if (m_uCurrentSeenVal <= 1)
  {
    uint1 * pPaintVol = (unsigned char *) GetDataPtr();
    int4 iSize = GetDim().m_x*GetDim().m_y*GetDim().m_z;
    for(int4 i=0;i<iSize;i++)
    {
      if( pPaintVol[i] & SUSPICIOUSROI || pPaintVol[i] & CADROI )
      { /// this is a suspicious ROI 
        ;
      }
      else if( (pPaintVol[i]&NOTCOUNTMASK) >= 1 )
        pPaintVol[i] = NOTCOUNTMASK;
    }

    m_uCurrentSeenVal = NOTCOUNTMASK-1;   

   }
} // VolumePaint::Transfer3DMarking()



/**
 * Returns the percentage of voxels seen.
 *
 * @return float4  0.0 to 1.0 percentage of total voxels seen.
 */
float4 VolumePaint::GetPercentExamined() const
{
  return float4(m_uCurrentSurfaceCount) / float4(m_uTotalSurfaceCount);
} // GetPercentExamined()


/**
 * Reads the volume specified into memory.
 *
 * @param const std::string& sFileName   Paint volume file name
 */
void VolumePaint::ReadData(const std::string& sFileName)
{
  LinVolUC::Read(sFileName);

  GetHdrVar(sHdrVarName_CurrentSurfaceCount, m_uCurrentSurfaceCount);
  GetHdrVar(sHdrVarName_TotalSurfaceCount, m_uTotalSurfaceCount);

  uint4 uPaintVolumeVersion = 0;   /// The version 3.0 matches the Console 3.0 release.
  GetHdrVar("PaintVol_Version", uPaintVolumeVersion);

  /// check the session file is created by version prior to Console 3.0 release
  if ( uPaintVolumeVersion < 3 || m_uCurrentSurfaceCount == 0 )
  {
    // This is an old version of session file
    LogFyi("The version of this paint volume is earlier than 3.0; reinitializing", "VolumePaint.C", "ReadData");

    //uint1 * pPaintVol = (unsigned char *) GetDataPtr();
    //register int4 iSize = GetDim().m_x*GetDim().m_y*GetDim().m_z;

    //const uint1 u = NOTCOUNTMASK-1;
    //for( int4 i=0; i<iSize; i++ )
    //{
    //  if( pPaintVol[i] < SUSPICIOUSROI && pPaintVol[i] > 0 )
    //    pPaintVol[i] = u;  // set to seen value
    //  else
    //    pPaintVol[i] = 0;
    //} // for

    m_uCurrentSeenVal = NOTCOUNTMASK-1;
    m_bSurfacesNeedReinit = true;

  } // if 
} // ReadData


/**
 * Writes the volume to the specified file using the new V3D File format.
 *
 * @param const std::string& sFileName   Paint volume file name.
 * @param bool bBuildIcon                Whether to build thumbnail icon.
 */
void VolumePaint::WriteData(const std::string & sFileName, bool bBuildIcon)
{
  SetHdrVar(sHdrVarName_CurrentSurfaceCount, m_uCurrentSurfaceCount, sHdrVarDesc_CurrentSurfaceCount);
  SetHdrVar(sHdrVarName_TotalSurfaceCount, m_uTotalSurfaceCount, sHdrVarDesc_TotalSurfaceCount);
  
  /// this is for the backward compatible. The old version session file may have SUSPICIOUSROI index 
  /// for seen area. The new version COUNTMASK = 0x20, that is way much smaller than previous
  /// version, where COUNTMASK = 0x80.
  uint4 uPaintVolumeVersion = 3.0;   /// The version 3.0 matches the Console 3.0 release.
  SetHdrVar("PaintVol_Version", uPaintVolumeVersion, "Version of the Paint Volume");

  // Note: Set current seen variable to its lowest value...
  m_uCurrentSeenVal = 1;
  // ...and effectively set all seen voxels to visible...
  Transfer3DMarking();
  
  try
  {
    // ...prior to saving the visible paint volume
    LinVolUC::Write(sFileName, false);
  }
  catch( ... )
  {
    LogErr( util::Strings::Format("Error occurred while writing paint volume [%ld]", ::GetLastError()), "VolumePaint", "WriteData"  );
  }
}

// $Log: VolumePaint.C,v $
// Revision 1.10.2.5  2007/06/22 00:10:03  jmeade
// Issue 5692: reject a missed region if the viewpoint found is outside of selected colon lumen.
//
// Revision 1.10.2.4  2007/06/20 16:45:03  jmeade
// Issue 5689: prevent possible divide by zero operation.
//
// Revision 1.10.2.3  2007/06/15 20:04:24  jmeade
// comments.
//
// Revision 1.10.2.2  2007/06/15 17:57:31  dongqing
// add paint volume version check and value reset for priori 3.0 version
// simplify to set paint vol 0
//
// Revision 1.10.2.1  2007/06/15 15:16:05  dongqing
// add paint volume version check and value reset for priori 3.0 version
//
// Revision 1.10  2007/03/12 17:13:40  dongqing
// fix the Paint SRT & CAD index backword compatible problem
//
// Revision 1.9  2007/01/26 22:32:49  dongqing
// add CAD ROI display
//
// Revision 1.8  2006/10/03 19:03:51  dongqing
// fix reading the csi from ins volume bug. that cause the missed region
// index wrong
//
// Revision 1.7  2006/09/20 18:52:14  dongqing
// 1st code adjustment for the Suspicious region tool
//
// Revision 1.6  2006/09/13 13:06:45  dongqing
// add temporary polyp segmentation display code in paint volume codes
//
// Revision 1.5  2005/09/30 14:21:21  vxconfig
// Improper use of new vx block volume iterators fixed
//
// Revision 1.4  2005/09/29 14:11:46  vxconfig
// switched csi volume to new vx block volume
//
// Revision 1.3  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.2  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.1.2.1  2005/09/16 16:02:44  frank
// Issue #4433: fixed more missed regions at 0.0cm and camera is placed outside the colon.
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.6  2005/09/01 19:28:19  geconomos
// Issue# 4392: Paint volume was not saving properly. Error turned out to be in the linear volume class.
//
// Revision 1.5  2005/08/25 12:33:57  geconomos
// Issue# 4389: Crash in MakeViewingGeometry. Fixed Normalizing vectors that had all components equal to zero.
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.11.2.2.4.3.6.3  2005/07/18 13:12:42  frank
// Issue #4279: Prevented jumping to unselected segments in missed regions
//
// Revision 3.11.2.2.4.3.6.2  2005/06/30 23:26:48  jmeade
// Default to re-initialize visualization for datasets.
//
// Revision 3.11.2.2.4.3.6.1  2005/06/27 17:36:38  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.11.2.2.4.3  2005/03/25 23:10:43  frank
// Issue #4043: added try/catch blocks to help find problem with missed region detection apparently running out of memory
//
// Revision 3.11.2.2.4.2  2005/03/22 20:21:38  frank
// avoided normalizing zero-length vectors
//
// Revision 3.11.2.2.4.1  2005/03/03 19:22:39  jmeade
// Issue 4003: Removed unnecessary current patch placeholder.
//
// Revision 3.11.2.2  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.11.2.1  2003/04/10 17:49:26  kevin
// we no longer link with a 50MB stack size, and the recursive growing
// has been replaced with a list metho
//
// Revision 3.11  2003/01/22 22:29:15  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.10  2002/11/26 00:39:46  kevin
// Changed surface initialization logic
//
// Revision 3.9  2002/11/24 17:46:21  kevin
// Cleaned up and made sub-functions out of mising patch creation to handle large paatches now that the 90% limit is no longer utilized
//
// Revision 3.8  2002/10/31 19:57:50  kevin
// Removed dead code
//
// Revision 3.7  2002/10/29 19:37:29  kevin
// Read and Writing should not be inline functions
//
// Revision 3.6  2002/09/13 17:33:29  jmeade
// Paint volume handles all volume and value (e.g. current/total seen voxel values) serialization.
//
// Revision 3.5  2002/09/13 00:54:41  jmeade
// Move paint volume characteristics to the VolumePaint class.
//
// Revision 3.4  2002/09/12 18:24:15  jmeade
// Setup for saving session data.
//
// Revision 3.3  2002/04/12 14:52:12  jmeade
// Changed Patch sort function to sort in Descending order.
//
// Revision 3.2  2002/04/12 02:20:40  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.1  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:22   ingmar
// Initial revision.
// Revision 1.2  2001/08/11 22:23:49  soeren
// now uses save version of getGradient
// if raw is ok, then use if for gradient and voxels throughout the function
//
// Revision 1.1  2001/05/07 01:40:08  kevin
// Added VolumePaint back in, somehow it got lost in the CVS debacle that occurred today
//

//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/VolumePaint.C,v 1.10.2.5 2007/06/22 00:10:03 jmeade Exp $
// $Id: VolumePaint.C,v 1.10.2.5 2007/06/22 00:10:03 jmeade Exp $
