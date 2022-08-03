// $Id: vxSkeletonator.C,v 1.11 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright(c) 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen   (dongqing@viatronix.com)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "vxSkeletonator.h"
#include "Vector.h"
#include "Timer.h"
#include "SkeletonNode.h"
#include "Segment.h"
#include "Skeleton.h"
#include "SegmentList.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeRegionGrowIterator.h"


using namespace std;
using namespace hdr;


#define FILE_REVISION "$Revision: 1.11 $"

/**
*  Constructor
*  makes references to the ins and dfb arrays provided by the pipeline.
*/
vxSkeletonator::vxSkeletonator( vxBlockVolume<uint1> *pIns, vxBlockVolume<uint1> *pSld, 
	                              const std::string & destFilePath )  
: m_pIns(pIns), m_pSld(pSld), m_destFilePath(destFilePath) 
{
} // vxSkeletonator() constructor


/**
*   Default destructor
*/
vxSkeletonator::~vxSkeletonator()  
{
} // ~vxSkeletonator() destructor


/**
*   Run the complete task of finding the skeleton.
*   It will create the following files:
*   PRD, CSI, and SKE
*
*   @return true if success
*/
bool vxSkeletonator::Run()
{ 
  stringstream tt;
  LogFyi ("Start SKE computation", "vxSkeletonator", "Run");

  // get number of segments 
  VarMap map(m_pIns->GetHeader().GetVariableVolumeHeader());
  uint4 numSegments = 0;
  map.GetHdrVar("numSegments", numSegments);
  
  try
  {
    Triple<int4>   dim  (m_pIns->GetHeader().GetVolDim());
    Triple<float4> units(m_pIns->GetHeader().GetVoxelUnits());

    vxBlockVolume<float4> *pPdr0Vol = new vxBlockVolume<float4>(vxVolumeHeader(dim, units, 32, 0.0));
    vxBlockVolume<float4> *pPdr1Vol = new vxBlockVolume<float4>(vxVolumeHeader(dim, units, 32, 0.0));
    vxBlockVolume<uint2>  *pCsi0Vol = new vxBlockVolume<uint2> (vxVolumeHeader(dim, units, 16, 0));
    vxBlockVolume<uint2>  *pCsi1Vol = new vxBlockVolume<uint2> (vxVolumeHeader(dim, units, 16, 0));
  
    // initlialize the node volume
    m_nodeVol = vxBlockVolume<vxVoxelCost>(vxVolumeHeader(dim, units, 8*sizeof(vxVoxelCost), 0));
    Vector3D<int2> startPoint, endPoint;
    SegmentList    segmentList;
  
	  vxBlockVolumeIterator<uint1> iti(*m_pIns);

    register uint4 i;
	  uint1 uu;
    // loop over all the segments
    for( i=0; i<numSegments; ++i )
    {   
      tt << "[" << i+1 << "/" << numSegments << "] Processing segment "<<i+1<<" out of "<<numSegments;   
      LogFyi(tt.str(), "vxSkeletonator", "Run"); tt.str("");        
    
      // Locate the first voxel of the segment to process    
      if( i == 0 )
      {
        startPoint = GetFirstSegmentSeed(m_pIns);
      } 
      else
      { // find the next segment if there is one
        startPoint = GetNextSegmentSeed(m_pIns, endPoint, numSegments, i+2);
      } // if first segment
        
      iti.SetPos(startPoint);
	    uu = iti.GetVoxel();
      const uint4 segmentVoxels = CountSegmentVoxels(m_pIns, uu);

      /// Growing from initial start
      endPoint = GrowFromSeed(m_pIns, m_pSld, startPoint, segmentVoxels, false, pPdr0Vol);
			pPdr0Vol->Compact();
    
      /// Growing from real end @ " << endPoint << flush;
      startPoint = GrowFromSeed(m_pIns, m_pSld, endPoint, segmentVoxels, true,  pPdr1Vol);
			pPdr1Vol->Compact();
    
      /// Growing back from real end to find real start " << startPoint << std::endl;
      endPoint = GrowFromSeed(m_pIns, m_pSld, startPoint, segmentVoxels, true,  pPdr0Vol);    
			pPdr0Vol->Compact();

      // Find the skeleton
      Skeleton skeleton0, skeleton1;
      /// Finding skeleton 0
      FindCenterline(endPoint, pPdr0Vol, m_pIns, skeleton0);
    
	    /// Finding skeleton 1
      FindCenterline(startPoint, pPdr1Vol, m_pIns, skeleton1);
    
      // Compute the CSI fields
      /// Computing CSI field 0
      ComputeCsi(skeleton0, m_pIns, pCsi0Vol, segmentVoxels);
			pCsi0Vol->Compact();

      /// Computing CSI field 1
      ComputeCsi(skeleton1, m_pIns, pCsi1Vol, segmentVoxels);
			pCsi1Vol->Compact();
    
	    iti.SetPos(startPoint);
      // Add the segment to the segment list
      const uint2 segmentNumber(iti.GetVoxel());
      AddSegment(segmentList, segmentNumber, skeleton0, skeleton1, m_destFilePath);
    
	    LogFyi("End of computation SKE", "Run", "vxSkeletonator.C");
    } // end processing all segments
  
    // write all output
    segmentList.Write(m_destFilePath);

		pPdr0Vol->Write(m_destFilePath+"_0.pdr");
		delete pPdr0Vol;
		pPdr1Vol->Write(m_destFilePath+"_1.pdr");
		delete pPdr1Vol;
		pCsi0Vol->Write(m_destFilePath+"_0.csi");   
		delete pCsi0Vol;
		delete pCsi1Vol;
  }
  catch( ... )
  {
    LogErr("Fail SKE computation", "Run", "vxSkeletonator.C");
	  return(false);
  }

  return(true);
} // Run


/**
*  Find the first segment and its start point
*
*  @param pIns the pointer to the INS volume
*  @return the start point in the next segment
*/
Vector3D<int2> vxSkeletonator::GetFirstSegmentSeed(vxBlockVolume<uint1> *pIns)
{ // find the cloest voxel to the startPosition
  const Triple<int4>   dim            (pIns->GetHeader().GetVolDim());
  const Vector3D<int2> startPosition  (dim.m_x/2, dim.m_y/2, 2);   // nearest initial to the anal canal
  Vector3D<int2>       lastPos        (startPosition);
  int4                 minDistSquare  ( INT_MAX );
  Vector3D<int2>       minDistVoxelPos(0,0,0);

  vxBlockVolumeIterator<uint1> iti(*pIns);

  register Vector3D<int2> position, diff;
  //register int4           distSquare;
	bool bQuitLoop(false);
  for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
  {
		bQuitLoop = false;
    for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
	  {
	    if( iti.GetVoxel() > 0x00 )
	    {
				minDistVoxelPos = iti.GetPos();
				bQuitLoop = true;
				break;
				// The old algorithm is waste of time. If the arrangement of block and the 
				// volume coordinates system are as designed as that of patient oriantation, 
				// the 1st found point should be very close to the best seed. Dongqing 3-6-07
        //position   = iti.GetPos();
        //diff       = lastPos - position;
        //distSquare = diff.Dot(diff);
        //if( minDistSquare > distSquare )
        //{
        //  minDistSquare   = distSquare;
        //  minDistVoxelPos = position;
        //} // if
	    } // if( iter.GetVoxel() > 0x00 )
	  } // for inside block

		if( bQuitLoop )
			break;
  } // for all blocks
    
  // did we find any segment voxels?
  if ( (minDistVoxelPos.m_x == 0) &&
       (minDistVoxelPos.m_y == 0) &&
       (minDistVoxelPos.m_z == 0) ) 
  {
    LogWrn("<vxSkeletonator::FindNextSegment> The colon volume is empty !!", "vxSkeletonator", "GetFirstSegmentSeed");
    throw ex::AbortDataProcessingException(LogRec("vxSkeletonator", "GetFirstSegmentSeed", "The colon volume is empty" ));
  }
    
  position = minDistVoxelPos; // if any segments found
      
  LogFyi("Segment found", "vxSkeletonator.C", "GetFirstSegmentSeed");
    
  return(position); // assume first segment is the one containing rectum
} // GetFirstSegmentSeed
  
  
/**
* Find the next segment and its start point
*
* @param pIns          the pointer to the INS volume
* @param lastGridPos
* @param numSegments   total number of segments
* @param passNumber    number of processed segment + 1 (or 2) ?
*/
Vector3D<int2> vxSkeletonator::GetNextSegmentSeed( vxBlockVolume<uint1> *pIns,
                 const Vector3D<int2> & lastGridPos, const uint4 & numSegments, const uint4 & passNumber)
{
  vxBlockVolumeIterator<uint1> iti(*pIns);
  
  /// Removing segment
  uint4 numRemovedVoxels( RemoveSegment(pIns, lastGridPos) );

  // now search for next nearest voxel inside colon
  /// Finding next segment
  int4                  minDistSquare( INT_MAX );
  Vector3D<int2>        minDistVoxelPos(0,0,0);
  Vector3D<int2>        lastPos(lastGridPos);
  register Vector<int4> position, diff;
  register int4         distSquare;
  for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
  {
    for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
	  {
	    if( iti.GetVoxel() > 0x00 )
      {
        position   = iti.GetPos();
        diff       = lastPos - position;
        distSquare = diff.Dot(diff);
        if( minDistSquare > distSquare )
        {
          minDistSquare   = distSquare;
          minDistVoxelPos = iti.GetPos();
        } // if
      } // if( iti.GetVoxel() > 0x00 )
    } // for inside block
  } // for all blocks

  lastPos = minDistVoxelPos;
    
  LogFyi("Found segment", "vxSkeletonator", "GetNextSegmentSeed");

  return(lastPos);    
} // GetNextSegmentSeed
  
  
/**
*  count the number of voxel in the segment
*
*  @param pIns the pointer to the INS volume
*  @param segmentID
*  @return number of voxels
*/
uint4 vxSkeletonator::CountSegmentVoxels( vxBlockVolume<uint1> *pIns, const uint1 & segmentId )
{
  uint4 iCount(0);
  vxBlockVolumeIterator<uint1> iti(*pIns);
  register uint1 u(segmentId);
  for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
  {
    for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
	  {
	    if( iti.GetVoxel() == u )
	      iCount++;
	  } // for inside block
  } // for all blocks
    
  return(iCount);
} // CountSegmentVoxels


/**
*  Remove segment by region growing
*  
*  @param pIns the pointer to the INS volume
*  @param startPosition
*  @return number of voxels in the segment
*/
uint4 vxSkeletonator::RemoveSegment( vxBlockVolume<uint1> *pIns, const Vector3D<int2> & startPosition )
{
  const Triple<int4> dim(pIns->GetHeader().GetVolDim());
  // loop over all voxels
  uint4 iCount(0);
  vxBlockVolumeIterator<uint1> iti(*pIns);
  iti.SetPos(startPosition);
  vxDeciderEqual<uint1> decider (iti.GetVoxel());
  // create region grow iterator
  vxBlockVolumeRegionGrowIterator<uint1> regionGrowIter(startPosition, *pIns, decider, 
	                    vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18);

  // use it to find all voxels in the intensity range
  for( ; regionGrowIter.IsNotAtEnd(); ++regionGrowIter )
  {
    ++iCount; 
    iti.SetPos( regionGrowIter.GetPos() ) ;
    iti.SetVoxel(0x00);
  } // for

  return(iCount);
} /// RemoveSegment
  
  
/**
* class to encapsulate the costs associated with each voxel during the PDR grow.
*/
class PDRcost 
{
  public:
    PDRcost(const PDRcost & src)
    {
      m_piecewiseEuclideanDistance = src.m_piecewiseEuclideanDistance;
      m_costToRoot                 = src.m_costToRoot;
    } // PDRcost

    PDRcost( float4 piecewiseEuclideanDistance=FLT_MAX, float4 costToRoot=FLT_MAX )
    {
      m_piecewiseEuclideanDistance = piecewiseEuclideanDistance;
      m_costToRoot                 = costToRoot;
    } // PDRcost

    inline bool operator==(const PDRcost & rhs) const
    {
      return(m_costToRoot == rhs.m_costToRoot && m_piecewiseEuclideanDistance == rhs.m_piecewiseEuclideanDistance);
    }
    inline bool operator> (const PDRcost & rhs) const
    {
      return(m_costToRoot >  rhs.m_costToRoot);
    }

  public:
    // Data members
  public:
    /// distance from root voxel (either start or end).
    float4 m_piecewiseEuclideanDistance;
    
    /// accumulated cost from root voxel (either start or end).
    float4 m_costToRoot;
}; // class PDRcost
  
  
/**
* class to decide wether to update a voxel in the PDR volume and heap.
*/
class PDRinserter 
{
  public:
    PDRinserter( vxBlockVolume<uint1> * pSld, vxBlockVolume<float4> * pPdrVol, bool setPdr )
      : m_pSld(pSld), m_pPdrVol(pPdrVol), m_bSetPdr(setPdr),
        m_globalMaxDfb(30), m_maxPiecewiseEuclideanDistance(0)
    {
	    const Triple<float4> units(m_pSld->GetHeader().GetVoxelUnits());
      // m_globalMaxDfb(30) because 29.5 was the maximum in our first 200 datasets
      m_neighborDistances.clear();
      Vector3D<int2> centerPos(1,1,1);
	    
      vxBlockVolumeIterator<uint1> its(*pSld);
	  
      register Vector<float4> delta;
      for( its.SetPos(0,0,0); its.IsNotAtEnd(); its.NextBlockZYX() )
      {
        for( ; its.IsNotAtEndOfBlock(); its.NextZYXinsideBlock() )
	      {
	        delta.m_x = centerPos.m_x - its.GetPos().m_x;
          delta.m_y = centerPos.m_y - its.GetPos().m_y;
          delta.m_z = centerPos.m_z - its.GetPos().m_z;
          delta.ScaleAnIso(units);
          m_neighborDistances.push_back(delta.GetLength());
        } // for inside block
	    } // for all blocks

      m_neighborDistances.push_back(0); // for cernter point at 0 distance
    } // constructor
    
    /// The API function called from the heap region grow iterator
    bool NeedToUpdateHeap(const PDRcost & srcCost, PDRcost & destCost, 
                          const Vector3D<int2> & pos, uint4 neighborIndex)
    {
	    vxBlockVolumeIterator<float4> itp(*m_pPdrVol);
	    itp.SetPos(pos);
      float4 newCostToRoot(srcCost.m_costToRoot + m_neighborDistances[neighborIndex] + ComputePenaltyFromSld(itp.GetVoxel()));
      if( newCostToRoot < destCost.m_costToRoot - 0.00001 )
      {
        destCost.m_costToRoot = newCostToRoot;
        destCost.m_piecewiseEuclideanDistance = srcCost.m_piecewiseEuclideanDistance + m_neighborDistances[neighborIndex];
        if( m_maxPiecewiseEuclideanDistance < destCost.m_piecewiseEuclideanDistance )
        {
          m_maxPiecewiseEuclideanDistance = destCost.m_piecewiseEuclideanDistance;
          m_posOfMax = pos;
        }
        if ( m_bSetPdr )
        {
		      itp.SetPos(pos);
		      itp.SetVoxel(newCostToRoot);
        } // write pdr
        return( true );
      } 
      else
      {
        return( false );
      }
    } // NeedToUpdateHeap
    
  public:
    /// public members
    float4         m_maxPiecewiseEuclideanDistance;
    Vector3D<int2> m_posOfMax;

  protected:
    const float4  m_globalMaxDfb;

    vxBlockVolume<uint1>  *m_pSld;
    vxBlockVolume<float4> *m_pPdrVol;

    const bool     m_bSetPdr;
    vector<float4> m_neighborDistances; // world space distances between neighbors
    
    inline float4 ComputePenaltyFromSld(float4 sld)
    {
      const float4 dfb      = sld * 0.25;
      const float4 zero2one = 1 - dfb/m_globalMaxDfb;
      const float4 raised2  = zero2one*zero2one;
      const float4 raised4  = raised2*raised2;
      const float4 raised8  = raised4*raised4;

      return(5000*raised8);
    } // ComputePenaltyFromSld   
}; // class PDRinserter
  
  
/**
 *  heap region grow
 *
 *  @param pIns the pointer to the INS volume
 *  @param pSld the pointer to the SLD volume
 *  @param startVoxelLocation
 *  @param segmentVoxels
 *  @param setPdr whether to modify the voxels in the PDR volume
 *  @param pPdrVol the pointer to the PDR volume
 *  @return the furthest Eucledian distance point
 */
Vector3D<int2> vxSkeletonator::GrowFromSeed( vxBlockVolume<uint1> *pIns, 
				  vxBlockVolume<uint1> *pSld,  const Vector3D<int2> & startVoxelLocation,
          const uint4 & segmentVoxels, const bool & setPdr, vxBlockVolume<float4> * pPdrVol )
{    
  vxBlockVolumeIterator<uint1>  iti(*pIns);
  vxBlockVolumeIterator<uint1>  its(*pSld);
  vxBlockVolumeIterator<float4> itp(*pPdrVol);

  // Initializing
  uint4 numVoxelsDiscovered(1);
  iti.SetPos(startVoxelLocation);
  uint4 segmentId(iti.GetVoxel());
    
  // initialize
  const TwentySixNeighborhood twentySixNeighborhood(pIns->GetHeader().GetVoxelUnits());
  const Triple<int4> insVolDim(pIns->GetHeader().GetVolDim());
  uint4 numVoxelsHeapified(0);
    
  // create heap and volume data structures
  vxVoxelCost::vxCostHeap costHeap;
    
  // create empty bit fields
  vxBlockVolume<bool> *pbDiscovered = new vxBlockVolume<bool>(pIns->GetHeader());
  vxBlockVolume<bool> *pbProcessed  = new vxBlockVolume<bool>(pIns->GetHeader());
    
  // load the start voxel into the queue
  Vector3D<int2> voxelLocation(startVoxelLocation);
  vxVoxelCost    voxelCost(0.0F, 0.0F, 0.0F, voxelLocation);

  vxBlockVolumeIterator<vxVoxelCost> itn(m_nodeVol);
  itn.SetPos(voxelLocation);
  itn.SetVoxel(voxelCost);

  if ( setPdr )
  {
    itp.SetPos(voxelLocation);
    itp.SetVoxel(0.0F);
  }
  vxBlockVolumeIterator<bool> itbd(*pbDiscovered);
  itbd.SetPos(voxelLocation);
  itbd.SetVoxel(true);
  costHeap.push(&voxelCost); // stl priority_queue
    
  // initialize end search variables
  Vector3D<int2> endVoxel(0,0,0);
  float4         maximumDistance(-1.0F);
    
  vxBlockVolumeIterator<bool> itbp(*pbProcessed);

  register int4 i;
  register Vector3D<int2> basePosition, thisPosition;
  register float4 newCostToRoot;
  // main grow loop
  while( !costHeap.empty() )
  { // get the voxel with lowest cost
    vxVoxelCost * pLowestCostVoxel( costHeap.top() );
    costHeap.pop();
    basePosition = pLowestCostVoxel->m_location;
      
    // mark this voxel as processed
	  itbp.SetPos(basePosition);
    itbp.SetVoxel(true);
      
    // examine each neighbor
    for( i=0; i<6; ++i )
    {        
      // see if this point is inside the volume and unprocessed
      thisPosition = twentySixNeighborhood.GetNeighborRaw(basePosition, i);
      if( twentySixNeighborhood.IsInsideVolume(thisPosition, insVolDim) )
      {          
	      itbp.SetPos(thisPosition);
        // see if this neighbor is already processed
        if( itbp.GetVoxel() == false )
        { // see if this neighbor is within the same segment
		      iti.SetPos(thisPosition);
          const uint4 thisSegmentId(iti.GetVoxel());
          if ( thisSegmentId == segmentId )
          { // this neighbor is within the same segment
              
            // get a handle on the current node
		        itn.SetPos(thisPosition);
            vxVoxelCost & thisCost = const_cast <vxVoxelCost &> (itn.GetVoxel());
              
            // see if we have discovered a new voxel
			      itbd.SetPos(thisPosition);
            const bool bVoxelDiscovered(itbd.GetVoxel());
            if ( bVoxelDiscovered == false )
            { // this is a newly discovered voxel
			        itbd.SetPos(thisPosition);
              itbd.SetVoxel(true);
              vxVoxelCost tempCost;
			        its.SetPos(thisPosition);
							const uint1 sld(its.GetVoxel());
							/// The most intensive math calculation of the skeleton stage
              tempCost.m_costOfVoxel = GetWeightFromSld(sld);
              tempCost.m_location = thisPosition;
			        itn.SetPos(thisPosition);
              itn.SetVoxel(tempCost);
              thisCost = tempCost;
              numVoxelsDiscovered++;
              if( numVoxelsDiscovered > segmentVoxels * 1.1F + 10 )
              {
                throw ex::AbortDataProcessingException(LogRec("vxSkeletonator", "GrowFromSeedOld", "Discovered too many voxels"));
              }
            } // voxel discovered
              
            // compute the new cost to root
            const float4 distanceToThisVoxel(twentySixNeighborhood.GetDistance(i));
            newCostToRoot = pLowestCostVoxel->m_costToRoot + thisCost.m_costOfVoxel + distanceToThisVoxel;
              
            // see if we can get there a int2er way
            if( newCostToRoot < thisCost.m_costToRoot )
            { // update the cost to root
              thisCost.m_costToRoot = newCostToRoot;
              if( setPdr )
              {
			          itp.SetPos(thisPosition);
                itp.SetVoxel(newCostToRoot);
              } // write pdr
                
              // update the Euclidean distance
              thisCost.m_piecewiseEuclideanDistance =
                  pLowestCostVoxel->m_piecewiseEuclideanDistance + distanceToThisVoxel;
                
              // see if we are the maximum Euclidean distance voxel from the start
              if( thisCost.m_piecewiseEuclideanDistance < FLT_MAX &&
                  thisCost.m_piecewiseEuclideanDistance > maximumDistance &&
                  endVoxel != thisPosition )
              {
                maximumDistance = thisCost.m_piecewiseEuclideanDistance ;
                endVoxel = thisPosition;
              } // test for maximum distance
                
              // add to heap
              costHeap.push(&thisCost);
              numVoxelsHeapified++;
                
            } // end cheaper cost to root
          } // in the same segment
        } // already processed
      } // is inside the volume
    } // loop over all 26 neighbor voxels
      
    // search for the next unprocessed voxel
    if ( !costHeap.empty() )
    {
      pLowestCostVoxel = costHeap.top();
      basePosition = pLowestCostVoxel->m_location;
	    itbp.SetPos(basePosition);
      while ( !costHeap.empty() && itbp.GetVoxel() == true )
      {
        costHeap.pop();
        pLowestCostVoxel = costHeap.top();
        basePosition = pLowestCostVoxel->m_location;
		    itbp.SetPos(basePosition);
      } // end search for next unprocessed voxel
    } // if heap has any elements      
  } // end of pulling from the heap
  
  delete pbDiscovered;
  delete pbProcessed;

  if( numVoxelsDiscovered < segmentVoxels )
  {
    LogDbg("Warning: percent of segment discovered", "vxSkeletonator", "GrowFromSeedOld");
  }
  if( numVoxelsHeapified < segmentVoxels )
  {  
    itn.SetPos(endVoxel);
	  stringstream tt;
	  tt << "      Average heapification " << float4(numVoxelsHeapified) / numVoxelsDiscovered << endl
       << "      Furthest Euclidean distance: " << itn.GetVoxel().m_piecewiseEuclideanDistance << endl;
    LogFyi(tt.str(),"vxSkeletonator","GrowFromSeedOld");
    tt.str("");
  }
  
  return(endVoxel);
} // GrowFromSeed(...)
  

/**
* Trace back to find the centerline.
*
* @param startPoint location of start point
* @param pPdrVol
* @param pIns
* @param skeleton
* @return TRUE if the centerline is found
*/
bool vxSkeletonator::FindCenterline( const Vector3D<int2> startPoint, vxBlockVolume<float4> * pPdrVol, 
                                      vxBlockVolume<uint1> * pIns, Skeleton & skeleton)
{ // initialize
  uint4 count(0);
  skeleton.Resize(0);
    
  // work backward from the seed until there is no lower cost voxel
  register bool bWorking( true );
  vxBlockVolumeIteratorNeighbors<float4> neighborIter(*pPdrVol, (Vector3D<int2>(1,1,1)), 
	                                vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06);
  vxBlockVolumeIterator<float4> itp(*pPdrVol);
  itp.SetPos(startPoint);
  vxBlockVolumeIterator<uint1> iti(*m_pIns);

  register uint1 u;
  register float4 minimumCost;
  while ( bWorking )
  { // add a node to the skeleton
    SkeletonNode node;
    node.m_vVolumePos = Point<uint2>(itp.GetPos());
    skeleton.AddSkelNode(node);
      
    // search all the neighbors for the minimum cost voxel
    minimumCost = itp.GetVoxel();
	  iti.SetPos(startPoint);
	  u = iti.GetVoxel();
	  for( neighborIter.CenterAt(itp.GetPos()); !neighborIter.IsAtEnd(); ++neighborIter )
	  {
	    if( neighborIter.IsInsideVolume() )
	    {
	      iti.SetPos(neighborIter.GetPos());
	      if( (neighborIter.GetVoxel() < minimumCost) && 
            (iti.GetVoxel() == u) )
        {
          minimumCost = neighborIter.GetVoxel() - 0.1;
          itp.SetPos(neighborIter.GetPos());
        } // if
      } // if
    } // search all neighbors
      
    // see if we have found a new location
    if( itp.GetPos() == neighborIter.GetPos() )
    {
      bWorking = false;
    } // new location
      
    count++;
  } // end centerline extraction
    
  // add the last node to the skeleton
  SkeletonNode node;
  node.m_vVolumePos = Point<uint2>(itp.GetPos());
  skeleton.AddSkelNode(node);
    
  // reverse the skeleton order because we inserted them from the real end to the real start
  skeleton.ReverseOrder();
    
  // Scale the centerline back up from voxels to world distances
  skeleton.ApplyScale(pIns->GetHeader().GetVoxelUnits());
  skeleton.RecalculateDistances();
        
  return(true);
} // FindCenterline()
  
  
/**
* class to encapsulate the costs associated with each voxel during the CSI grow.
*/
class CSIcost
{
  public:
    CSIcost(const CSIcost & src)
    {
      m_piecewiseEuclideanDistance = src.m_piecewiseEuclideanDistance; 
	    m_csi = src.m_csi;
    }

    CSIcost( float4 distance=FLT_MAX, uint2 csi=0 )
    {
      m_piecewiseEuclideanDistance = distance;  
	    m_csi = csi;
    }

    inline bool operator==(const CSIcost & rhs) const
    {
      return(m_piecewiseEuclideanDistance == rhs.m_piecewiseEuclideanDistance && m_csi == rhs.m_csi);
    }

    inline bool operator> (const CSIcost & rhs) const
    {
      return(m_piecewiseEuclideanDistance >  rhs.m_piecewiseEuclideanDistance);
    }

    // Data members
  public:    
    /// distance from closest centerline voxel.
    float4 m_piecewiseEuclideanDistance;
    uint2  m_csi;
}; // class CSIcost
  
  
/**
* class to decide wether to update a voxel in the CSI volume and heap.
*/
class CSIinserter 
{
  public:
    CSIinserter(vxBlockVolume<uint2> * pCsiVol) : m_pCsiVol(pCsiVol)
    {
	    const Triple<float4> units(m_pCsiVol->GetHeader().GetVoxelUnits());
      m_neighborDistances.clear();
      register Vector3D<int2> centerPos(1,1,1);

	    vxBlockVolumeIterator<uint2> itc(*m_pCsiVol);

	    register Vector<float4> delta;
      for( itc.SetPos(0,0,0); itc.IsNotAtEnd(); itc.NextBlockZYX() )
      {
        for( ; itc.IsNotAtEndOfBlock(); itc.NextZYXinsideBlock() )
	      {
		      delta.m_x = centerPos.m_x - itc.GetPos().m_x;
          delta.m_y = centerPos.m_y - itc.GetPos().m_y;
          delta.m_z = centerPos.m_z - itc.GetPos().m_z;
          delta.ScaleAnIso(units);
          m_neighborDistances.push_back(delta.GetLength());
        } // for inside block
	    } // for all blocks
        
	    m_neighborDistances.push_back(0); // for cernter point at 0 distance
    } // constructor
    
    /// The API function called from the heap region grow iterator
    bool NeedToUpdateHeap( const CSIcost & srcCost, CSIcost & destCost, 
                           const Vector3D<int2> & pos, uint4 neighborIndex )
    {
	    vxBlockVolumeIterator<uint2> itc(*m_pCsiVol);
      float4 newCost(srcCost.m_piecewiseEuclideanDistance + m_neighborDistances[neighborIndex]);
      if ( newCost < destCost.m_piecewiseEuclideanDistance - 0.00001 )
      {
        destCost.m_piecewiseEuclideanDistance = newCost;
        destCost.m_csi = srcCost.m_csi;
		    itc.SetPos(pos);
		    itc.SetVoxel(srcCost.m_csi);
        return( true );
      } 
	    else
      {
        return( false );
      }
    } // NeedToUpdateHeap
    
  protected:
    /// members
    vxBlockVolume<uint2>  * m_pCsiVol;
    vector<float4>          m_neighborDistances; // world space distances between neighbors
}; // class CSIinserter
  
  
/**
 *  compute CSI
 *
 *  @param skeleton
 *  @param pIns
 *  @param pCsiVol
 *  @param segmentVoxels
 *  @return true if success
 */
bool vxSkeletonator::ComputeCsi( const Skeleton & skeleton, vxBlockVolume<uint1> *pIns, 
                                vxBlockVolume<uint2> *pCsiVol, const uint4 & segmentVoxels)
{ /// Initializing
  vxVoxelCost::vxEuclideanHeap distanceHeap; // stl priority_queue
  const Triple<uint4>  insVolDim( pIns->GetHeader().GetVolDim() );
  const Triple<float4> units    ( pIns->GetHeader().GetVoxelUnits() );
    
  // create empty bit fields
  vxBlockVolume<bool> *pbDiscovered = new vxBlockVolume<bool>(pIns->GetHeader());
  vxBlockVolume<bool> *pbProcessed  = new vxBlockVolume<bool>(pIns->GetHeader());
  vxBlockVolumeIterator<bool> itbd (*pbDiscovered);
  vxBlockVolumeIterator<bool> itbp (*pbProcessed);
  vxBlockVolumeIterator<uint1> iti (*pIns);
    
  Vector3D<int2> startPosition( skeleton.GetSkelNode(0).m_vVolumePos.m_x, 
		                            skeleton.GetSkelNode(0).m_vVolumePos.m_y, 
		                            skeleton.GetSkelNode(0).m_vVolumePos.m_z );
  iti.SetPos(startPosition);
  uint4 segmentId(iti.GetVoxel());
    
  // initialize
  const TwentySixNeighborhood twentySixNeighborhood(units);
  uint4 numVoxelsHeapified (0);
  uint4 numVoxelsDiscovered(0);
    
  vxBlockVolumeIterator<vxVoxelCost> itn(m_nodeVol);
  vxBlockVolumeIterator<uint2>       itc(*pCsiVol);

  register uint4 i;
  register Vector3D<int2> voxelLocation, basePosition, thisPosition;
  // add all the skeleton voxels to the heap with initial cost of zero to grow from this line
  for( i=0; i<skeleton.GetNumOfElements(); i++ )
  {
    const SkeletonNode & skel = skeleton.GetSkelNode(i);
    voxelLocation.m_x = skel.m_vVolumePos.m_x;
    voxelLocation.m_y = skel.m_vVolumePos.m_y;
    voxelLocation.m_z = skel.m_vVolumePos.m_z;
    vxVoxelCost distance(0.0F, 0.0F, float4(i), voxelLocation); // overload cost to root with skeleton index
	  itn.SetPos(voxelLocation);
    itn.SetVoxel(distance);
		vxVoxelCost & thisCost = const_cast <vxVoxelCost &> (itn.GetVoxel());
    distanceHeap.push(&thisCost);
	  itbd.SetPos(voxelLocation);
    itbd.SetVoxel(true);
	  itc.SetPos(voxelLocation);
    itc.SetVoxel(i);
    numVoxelsDiscovered++;
  } // end all skeleton nodes
    
  // main grow loop
  while ( !distanceHeap.empty() )
  { // get the nearest voxel to the centerline
    vxVoxelCost * pNearestVoxel = distanceHeap.top();
    distanceHeap.pop();
    basePosition = pNearestVoxel->m_location;
    const uint4 nearestSkeletonIndex(static_cast<uint4>(pNearestVoxel->m_costToRoot));
      
    // mark this voxel as processed
	  itbp.SetPos(basePosition);
    itbp.SetVoxel(true);
	  itc.SetPos(basePosition);
    itc.SetVoxel(nearestSkeletonIndex);
      
	  register uint4 i;
    // examine each neighbor, convert to 18 w/iter ??
    for( i=0; i<6; ++i )
    { // see if this point is inside the volume and unprocessed
      thisPosition = twentySixNeighborhood.GetNeighbor(basePosition, i);
      if( twentySixNeighborhood.IsInsideVolume(thisPosition, insVolDim) )
      { // see if this neighbor is already processed
	      itbp.SetPos(thisPosition);
        if ( itbp.GetVoxel() == false )
        { // see if this neighbor is within the same segment
		      iti.SetPos(thisPosition);
          const uint4 thisSegmentId(iti.GetVoxel());
          if ( thisSegmentId == segmentId )
          { // this neighbor is within the same segment              
		        itn.SetPos(thisPosition);
            // get a handle on the current node
            vxVoxelCost & thisCost = const_cast<vxVoxelCost &> (itn.GetVoxel());
              
			      itbd.SetPos(thisPosition);
            // see if we have discovered a new voxel
            const bool bVoxelDiscovered(itbd.GetVoxel());
            if ( bVoxelDiscovered == false )
            { // this is a newly discovered voxel
              itbd.SetVoxel(true);
              vxVoxelCost tempCost;
              tempCost.m_location = thisPosition;
              tempCost.m_piecewiseEuclideanDistance = FLT_MAX;
              tempCost.m_costToRoot = pNearestVoxel->m_costToRoot; // overloaded skeleton index
			        itn.SetPos(thisPosition);
              itn.SetVoxel(tempCost);
							thisCost = itn.GetVoxel();    //std::cout << "  " << thisPosition;//=======================
              numVoxelsDiscovered++;
            } // voxel discovered
              
            // compute the new Euclidean distance to nearest voxel
            const float4 distanceToThisVoxel( twentySixNeighborhood.GetDistance(i) );
            float4 newDistanceToThisVoxel( pNearestVoxel->m_piecewiseEuclideanDistance + distanceToThisVoxel );
              
            // see if we can get there a int2er way
            const float4 threshold( 1e-5F );
            const float4 difference( thisCost.m_piecewiseEuclideanDistance - newDistanceToThisVoxel );
            if ( difference > threshold )
            { // update the Euclidean distance
              thisCost.m_piecewiseEuclideanDistance = newDistanceToThisVoxel;
                
              // overloaded skeleton index
              thisCost.m_costToRoot = pNearestVoxel->m_costToRoot;
                
              // add to heap
              distanceHeap.push(&thisCost);
              numVoxelsHeapified++;
                
            } // end cheaper cost to root
          } // in the same segment
        } // already processed
      } // is inside the volume
    } // loop over all 26 neighbor voxels
      
    // search for the next unprocessed voxel
    if ( !distanceHeap.empty() )
    {
      pNearestVoxel = distanceHeap.top();
      basePosition  = pNearestVoxel->m_location;
	    itbp.SetPos(basePosition);
      while ( !distanceHeap.empty() && itbp.GetVoxel() == true )
      {
        distanceHeap.pop();
        pNearestVoxel = distanceHeap.top();
        basePosition = pNearestVoxel->m_location;
		    itbp.SetPos(basePosition);
      } // end search for next unprocessed voxel
    } // if heap has any elements      
  } // end of pulling from the heap
    
  delete pbDiscovered;
  delete pbProcessed;

  if ( numVoxelsDiscovered < segmentVoxels )
  {
    stringstream tt;
    tt << "      Warning: percent of segment discovered " << 100 * float4(numVoxelsDiscovered) / segmentVoxels << endl;
    LogDbg(tt.str(),"vxSkeletonator","ComputeCsiOld");
    tt.str("");
  }
    
  return(true);
} // ComputeCsi()

  
/**
*  Add segment
*
*  @param segmentList
*  @param segmentNumber
*  @param skeleton1
*  @param skeleton2
*  @param filenameRoot
*/
void vxSkeletonator::AddSegment( SegmentList & segmentList, const uint4 & segmentNumber,
                                 Skeleton & skeleton1, Skeleton & skeleton2, const std::string & filenameRoot)
{ // make a segment
  std::vector<Skeleton> vSkeletons;
  vSkeletons.push_back(skeleton1);
  vSkeletons.push_back(skeleton2);
    
  Segment segment;
  segment.Initialize(segmentNumber, vSkeletons, filenameRoot);
    
  // add it to the list
  segmentList.AddNewSegmentAndSelectIt(segment);
} // AddSegment()  
  

#undef FILE_REVISION


// $Log: vxSkeletonator.C,v $
// Revision 1.11  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.10  2006/06/22 19:28:26  dongqing
// add vxBlockVolume::Compact to save memory space
//
// Revision 1.9  2006/04/26 16:40:26  dongqing
// fix the CSI bug
//
// Revision 1.8  2006/04/21 20:55:28  dongqing
// First complete beta version
//
// Revision 1.7  2006/04/19 19:53:44  dongqing
// bug free beta version
//
// Revision 1.6  2006/04/12 20:52:43  dongqing
// Rewrite CRES to get rid of gradient feature
//
// Revision 1.5  2006/04/06 15:08:57  dongqing
// rewrite the iterator
//
// Revision 1.4  2006/03/21 21:09:46  dongqing
// rewrite CRES and iterator
//
// Revision 1.3  2006/03/13 18:39:24  dongqing
// finialize SKE codes
//
// Revision 1.2  2006/03/10 21:51:46  dongqing
// adjust the region grow iterator
//
// Revision 1.1  2006/03/09 18:40:13  dongqing
// ad vxSkeletonator class initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxSkeletonator.C,v 1.11 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxSkeletonator.C,v 1.11 2007/03/06 16:34:01 dongqing Exp $





























