// $Id: vxContourList.C,v 1.7 2006/05/31 16:18:22 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include <algorithm>
#include "vxContourList.h"
#include "vxContour.h"
#include "vxBlockVolumeIterator.h"
#include "Point.h"
#include "Vector.h"
#include "Normal.h"


// Defines

/**
 * Default constructor
 */
vxContourList::vxContourList()
: m_bComputePtFlag( false ),
m_uOrientation( 2 )
{
  m_pMaskVolume = NULL;
} // vxContourList()


/**
 * Destructor
 */
vxContourList::~vxContourList()
{
  if( m_pMaskVolume != NULL )
  {
    delete m_pMaskVolume;
    m_pMaskVolume = NULL;
  }
} // ~vxContourList()


/**
 * Clear the vector of lassos.
 */
void vxContourList::Clear()
{
  LassoList::iterator lassoIter = m_vContourList.begin();
  for ( ; lassoIter != m_vContourList.end(); lassoIter++ )
    delete * lassoIter;

  m_vContourList.clear();
} // Clear()


/**
 * Create a bool mask volume
 *
 * @param volDim volume dimensions
 * @param volUnits volume units
 */
void vxContourList::Initialize( const Triple<int4> & volDim, const Triple<float4> & volUnits )
{

  // generate a bool volume to mark the desired voxels
  if ( m_pMaskVolume != NULL )
    delete m_pMaskVolume;

  m_pMaskVolume = new vxBlockVolume<bool>( vxVolumeHeader( volDim, volUnits ) );

} // Initialize()





/**
 * Compute same number of points forall lassos
 *
 * @return number of points
 */
uint2 vxContourList::ComputePoints()
{
  uint2 uMaxPoints(0);
  uint2 uIndex(0);

  // make the iterator point to the begining of the list.
  LassoList::iterator lassoIter = m_vContourList.begin();
  
  // get the lasso with the max points.
  uint2 i(0);
  for(i = 0; lassoIter != m_vContourList.end(); i++, lassoIter++)
  {
    if ( (*lassoIter)->GetPointNumOfContour() > uMaxPoints )
    {
      uMaxPoints = (*lassoIter)->GetPointNumOfContour();
      uIndex = i;
    }
  } // end for

  // make the iterator point to the begining of the list.
  lassoIter = m_vContourList.begin();

  // typedef to define an iterator over the vector of contour points for each lasso.
  typedef std::list<Point<float4> > contourList;

  // go through all the lassos and put same number of points for all.
  for ( i = 0; lassoIter != m_vContourList.end(); i++, lassoIter++ )
  {

    // the following should really be done elsewhere within contour-specific code
    if ( i != uIndex )
    {
      vxContour * pRoi = *lassoIter;

      // get how many points to add for this lasso.
      int diff( uMaxPoints - pRoi->GetPointNumOfContour() );
 
      while ( diff > 0 )
      {
        float4 fMaxDist(0.0F);
        uint2 uPtIndex(0);
        
        // get an iterator for the vector of lasso points.
        contourList::iterator iterLasso = pRoi->GetContourPtVector().begin();

        // loop over the entire lasso to find the longest segment where to add a new point.       
        for ( uint2 j=0; j<pRoi->GetPointNumOfContour() - 1; j++ )
        {
          float4 fDist = ( pRoi->GetPoint(j) - ( pRoi->GetPoint(j+1) ) ).GetLength();
          if(fDist > fMaxDist)
          {
            fMaxDist = fDist;
            uPtIndex = j;            
          } // end if
        } // end for j= 0 to j < lasso->points

        // move the iterator to point to the position before which a new point will be inserted.
        std::advance( iterLasso, uPtIndex+1 );

        // create the new point to be added
        // this should really just take the average of the two points, regardless of the orientation!!!
        Point<float4> fNewPt; 
        switch ( pRoi->GetOrientation() )
        {
        case 0: 
          fNewPt.m_x = pRoi->GetPoint(uPtIndex).m_x;
          fNewPt.m_y = ( pRoi->GetPoint(uPtIndex).m_y + pRoi->GetPoint(uPtIndex+1).m_y ) / 2.0;
          fNewPt.m_z = ( pRoi->GetPoint(uPtIndex).m_z + pRoi->GetPoint(uPtIndex+1).m_z ) / 2.0;
          break;
        case 1: 
          fNewPt.m_y = pRoi->GetPoint(uPtIndex).m_y;
          fNewPt.m_x = (pRoi->GetPoint(uPtIndex).m_x + pRoi->GetPoint(uPtIndex+1).m_x)/2.0;
          fNewPt.m_z = (pRoi->GetPoint(uPtIndex).m_z + pRoi->GetPoint(uPtIndex+1).m_z)/2.0;
          break;
        case 2: 
          fNewPt.m_z = pRoi->GetPoint(uPtIndex).m_z;
          fNewPt.m_x = (pRoi->GetPoint(uPtIndex).m_x + pRoi->GetPoint(uPtIndex+1).m_x)/2.0;
          fNewPt.m_y = (pRoi->GetPoint(uPtIndex).m_y + pRoi->GetPoint(uPtIndex+1).m_y)/2.0;
          break;
        } // end switch

        // insert the new point. !!! check if the point really gets added and the total number increases.
        pRoi->GetContourPtVector().insert(iterLasso, fNewPt);     
        
        // decrement the number of points to be added.
        diff--;        
      } // end while diff > 0.
    } // end if   
  } // end for

  return uMaxPoints - 1;
} // Compute()


/**
 * Get minimum distance between the two lassos
 *
 * @param uIndex position of lasso after which there is an empty slice
 * @param uMaxPts number of points of each lasso
 */
void vxContourList::ComputeMinMatch( const uint2 & uIndex, const uint2 & uMaxPts )
{
  // get pointers to the two lassos
  LassoList::iterator lassoIter = m_vContourList.begin();
  std::advance( lassoIter, uIndex );
  vxContour * prevRoi = *lassoIter;
  lassoIter++;
  vxContour * nextRoi = *lassoIter;

  // initialize the search variables
  float4 fMinDistance( (std::numeric_limits<float>::max)() );
  std::list< Point3Df > bestList = nextRoi->GetContourPtVector();

  // try it one direction
  for ( int4 j(0); j<uMaxPts; ++j )
  {
    // check for a minimum sum length
    const float4 fSumDistance( GetMatchingDistance( uIndex ) );
    if ( fSumDistance < fMinDistance )
    {
      fMinDistance = fSumDistance;
      bestList = nextRoi->GetContourPtVector();
    }

    // move the front to the back
    nextRoi->GetContourPtVector().push_back( nextRoi->GetContourPtVector().front() );
    nextRoi->GetContourPtVector().pop_front();
  }

  // then the other direction
  std::reverse( nextRoi->GetContourPtVector().begin(), nextRoi->GetContourPtVector().end() );
  for ( int4 j=0; j<uMaxPts; ++j )
  {
    // check for a minimum sum length
    const float4 fSumDistance( GetMatchingDistance( uIndex ) );
    if ( fSumDistance < fMinDistance )
    {
      fMinDistance = fSumDistance;
      bestList = nextRoi->GetContourPtVector();
    }

    // move the front to the back
    nextRoi->GetContourPtVector().push_back( nextRoi->GetContourPtVector().front() );
    nextRoi->GetContourPtVector().pop_front();
  }

  // copy the best into the lasso
  nextRoi->GetContourPtVector() = bestList;

} // ComputeMinMatch()


/**
 * Get the sum distance of all matching points
 *
 * @param uLassoIndex position of lasso after which there is an empty slice.
 *
 * @return sum distance of all matching points
 */
float4 vxContourList::GetMatchingDistance( const uint2 & uLassoIndex )
{
  // get pointers to the two lassos
  LassoList::iterator lassoIter = m_vContourList.begin();
  std::advance( lassoIter, uLassoIndex );
  vxContour * prevRoi = *lassoIter;
  lassoIter++;
  vxContour * nextRoi = *lassoIter;

  // compute the sum distance
  float4 fSum( 0.0F );
  for ( int4 i(0); i<prevRoi->GetContourPtVector().size(); ++i )
    fSum += prevRoi->GetPoint( i ).GetEuclideanDistance( nextRoi->GetPoint( i ) );

  return fSum;

} // GetMatchingDistance()




/**
 * Fill the mask volume from the lassos in the list
 */                                         
void vxContourList::ProcessLassoList()
{
  m_bComputePtFlag = false;

  // clear the volume before using.
  if ( m_pMaskVolume != NULL )
    m_pMaskVolume->Clear();

  //Set the mask volume for interpolate contours
  SetMaskVolumeWithInterpolateContours();

  //Set Mask volume for original lassos
  vxContourList::LassoList::iterator lassoIter = m_vContourList.begin();
  for ( ; lassoIter != m_vContourList.end(); lassoIter++ )
  {
    vxContour * pLasso = * lassoIter;
    SetMaskVolume( pLasso );
  } // loop over lassos

  // clear the list for next use.
  Clear();
 
} // ProcessLassoList()

/**
 * Check for gaps between lassos in the list if so process all lassos to fill them up
 */                    
void vxContourList::SetMaskVolumeWithInterpolateContours()
{
  // maximum number of points for all lassos.
  uint2 uMaxPts(0);
  std::list< vxContour * >::iterator contourIterator = m_vContourList.begin();  

  for ( uint2 i=0; i< m_vContourList.size()  - 1 ; i++, contourIterator++ )
  {
    //Get the prev and next contours
    vxContour * pPrevRoi = * contourIterator++;
    vxContour * pNextRoi = * contourIterator--;

    if ( pPrevRoi->GetOrientation() != pNextRoi->GetOrientation() )
      continue;

    // figure out how many slices are between the two lassos that we have
    const uint2 uOrientation( pPrevRoi->GetOrientation() );
    const int4 iSliceNumPrev( pPrevRoi->GetPoint(0)[ uOrientation ] / m_pMaskVolume->GetHeader().GetVoxelUnits()[ uOrientation ] + 0.5F );
    const int4 iSliceNumNext( pNextRoi->GetPoint(0)[ uOrientation ] / m_pMaskVolume->GetHeader().GetVoxelUnits()[ uOrientation ] + 0.5F );
    const int4 iDifferenceBetweenSlices( ( iSliceNumPrev < iSliceNumNext ) ? iSliceNumNext - iSliceNumPrev : iSliceNumPrev - iSliceNumNext );

    // if we need to fill in slices
    if ( iDifferenceBetweenSlices > 1 )
    {
      // empty slice found put same number of points on all the lassos.
      if( m_bComputePtFlag == false )
      {
        uMaxPts = ComputePoints();
        m_bComputePtFlag = true;
      }

      // find all matching pairs and get the min one.
      ComputeMinMatch( i, uMaxPts );

      // get the number of empty slices in between.
      for ( uint4 uSlice(1); uSlice < iDifferenceBetweenSlices; uSlice++ )
      {
        int4 iNewSliceNum( iSliceNumPrev + uSlice );

        // create a new lasso on the empty slice and insert the lasso in the vector before the lasso(i+1).
        // the slice is in index coord. need to multiply with volume units.
        vxContour * interpolateContour = new vxContour();

        GetInterpolateContour( iNewSliceNum, i+uSlice, pPrevRoi->GetOrientation(), uMaxPts,  pPrevRoi , pNextRoi, interpolateContour );
        
        SetMaskVolume( interpolateContour );

        delete interpolateContour;

      } // end for multiple empty slices.
    } // end if there is at least one empty slice.
  } // end for

} // InterpolateLassos()


/**
 * GetInterpolateContour
 *
 * @param fSliceNum current plane index
 * @param uPos position in the vector of lassos before which to insert the new lasso
 * @param uOrientation plane orientation
 * @param uMaxPts number of points of each lasso
 * @param roiPrev previous contour
 * @param roiNex next contour
 */
void vxContourList::GetInterpolateContour( const float4 fSliceNum, const uint2 & uPos, const uint1 & uOrientation, const uint2 & uMaxPts,
                                             vxContour * roiPrev, vxContour * roiNext, vxContour * pInterpolateContour  )
{
  // initialize some variables
  float4 fCurSliceNum(0.0F);
  Point<float4> fPlanePt;
  Normal<float4> fNormal;
  switch(uOrientation)
  {
  case 0:
    fCurSliceNum = fSliceNum * m_pMaskVolume->GetHeader().GetVoxelUnits().X(); 
    fPlanePt = Point<float4>( fCurSliceNum, roiPrev->GetPoint(0).m_y, roiPrev->GetPoint(0).m_z );
    fNormal = Normal<float4>( 1.0F, 0.0F, 0.0F);
    break;
  case 1:
    fCurSliceNum = fSliceNum * m_pMaskVolume->GetHeader().GetVoxelUnits().Y(); 
    fPlanePt = Point<float4>( roiPrev->GetPoint(0).m_x, fCurSliceNum, roiPrev->GetPoint(0).m_z );
    fNormal = Normal<float4>( 0.0F, 1.0F, 0.0F );
    break;
  case 2:
    fCurSliceNum = fSliceNum * m_pMaskVolume->GetHeader().GetVoxelUnits().Z(); 
    fPlanePt = Point<float4>( roiPrev->GetPoint(0).m_x, roiPrev->GetPoint(0).m_y, fCurSliceNum );
    fNormal = Normal<float4>( 0.0F, 0.0F, 1.0F );
    break;
  }

  pInterpolateContour->SetOrientation( uOrientation );

  // find out the new points for the new lasso by taking the intersection of line and plane.
  for ( uint2 i=0; i< uMaxPts; i++ )
  {
    // interpolate the two points
    Point<float4> fPoint1( roiPrev->GetPoint( i           ) );
    Point<float4> fPoint2( roiNext->GetPoint( i % uMaxPts ) );
    float4 fNumerator( fNormal.Dot( fPlanePt - fPoint1 ) );
    float4 fDenominator( fNormal.Dot( fPoint2 - fPoint1 ) );
    float4 fParam( fNumerator / fDenominator );

    pInterpolateContour->AddPoint( fPoint1 + ( ( fPoint2 - fPoint1 ) * fParam ) );
  } // end for

  // to close the contour.
  pInterpolateContour->AddPoint( pInterpolateContour->GetPoint(0) );
   
} // GetInterpolateContour()

/*
 * Sets the Mask volume based on the contour
 * @param contour to set
 */
void vxContourList::SetMaskVolume( vxContour * pContour )
{
    Point<int2> iTopLeftPt;
    Image<bool> bMask(false);

    pContour->CalculateArea( m_pMaskVolume->GetHeader().GetVoxelUnits(), iTopLeftPt, bMask );

    // iterator for the volume.
    vxBlockVolumeIterator<bool> iterMaskVol(*m_pMaskVolume);
    
    // scan all the voxels inside the area
    Triple<uint4> maskDim = bMask.GetDim();
    int2 iSlice( pContour->GetPoint(0)[ m_uOrientation ] / m_pMaskVolume->GetHeader().GetVoxelUnits()[ m_uOrientation ] + 0.5F );
    int2 top = iTopLeftPt.m_y;
    int2 left = iTopLeftPt.m_x;

    // get the plane orientation.
    Point<int4> maskPoint;
    maskPoint[ pContour->GetOrientation() ] = iSlice;

    // loop over scanlines
    for ( uint4 y(0); y<maskDim.m_y; ++y )
    {
      switch ( pContour->GetOrientation() )
      {
        case 2: maskPoint.m_y = top + y;  break;
        case 0: maskPoint.m_y = top + y;  break;
        case 1: maskPoint.m_z = top + y;  break;
      }

      // loop over pixels in the scanline
      for ( uint4 x(0); x<maskDim.m_x; ++x )
      {
        switch ( pContour->GetOrientation() )
        {
          case 2: maskPoint.m_x = left+x; break;
          case 0: maskPoint.m_z = left+x; break;
          case 1: maskPoint.m_x = left+x; break;
        }

        // see if the voxel is inside the lasso
        if ( bMask.GetPixel( Point2D<uint2>(x, y) ) )
        {
          iterMaskVol.SetPos( Vector3D<int4>( maskPoint.m_x, maskPoint.m_y, maskPoint.m_z ) );
          iterMaskVol.SetVoxel( true );
        } // end if voxel inside the lasso

      } // for all pixels in scanline

    } // for all scanlines
}//SetMaskVolume( vxContour * pContour )

#undef FILE_REVISION 


// $Log: vxContourList.C,v $
// Revision 1.7  2006/05/31 16:18:22  geconomos
// porting to visual studio 2005
//
// Revision 1.6  2006/04/30 20:48:27  romy
// bug fix in contour
//
// Revision 1.5  2006/04/28 17:55:23  romy
// Fixed infinite loop issue with the Contour issue
//
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/08/01 19:12:07  frank
// changed function name according to coding standards
//
// Revision 1.2  2004/03/12 19:49:33  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1  2004/03/12 18:01:54  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2004/01/19 20:17:00  frank
// removed some old typedefs
//
// Revision 1.10  2003/05/21 20:09:44  dongqing
// code review
//
// Revision 1.9  2003/05/21 20:05:00  frank
// Fixed contour locations
//
// Revision 1.8  2003/05/13 18:28:22  frank
// code review
//
// Revision 1.7  2003/05/13 17:40:49  frank
// Issue #3215: Contour broken in some slices
//
// Revision 1.6  2003/04/28 16:42:46  frank
// Issue #3141: Fixed contour if you draw clockwise, then counterclockwise
//
// Revision 1.5  2003/02/25 16:38:40  frank
// Believe to have fixed the contour missing slices
//
// Revision 1.4  2003/01/22 21:44:03  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.3  2002/12/20 19:02:12  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.2  2002/11/19 23:03:41  frank
// removed extra pragmas
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.5  2002/09/26 14:51:52  frank
// Moved InitializeData into C++.
//
// Revision 1.4  2002/09/09 21:10:49  dmitry
// point vector changed to list
//
// Revision 1.3  2002/07/11 21:09:11  manju
// Speed up.
//
// Revision 1.2  2002/07/10 19:24:59  manju
// Changed CreateMaskVolume to Initialize().
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.4  2002/06/27 12:08:29  manju
// Clearing the bool mask volume before use.
//
// Revision 1.3  2002/06/26 17:24:27  manju
// Changed vector of lassos to list of lassos and modified the code to get
// it working,
//
// Revision 1.2  2002/06/19 18:21:09  manju
// Clearing the volume after creation.
//
// Revision 1.1  2002/06/19 14:23:37  manju
// Restricted Lasso class.
//
// Revision 1.2  2002/06/11 13:06:31  manju
// $Id: vxContourList.C,v 1.7 2006/05/31 16:18:22 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxContourList.C,v 1.7 2006/05/31 16:18:22 geconomos Exp $
