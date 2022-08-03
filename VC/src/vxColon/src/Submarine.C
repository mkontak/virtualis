// $Id: Submarine.C,v 1.12.2.13 2010/12/21 15:40:48 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Submarine.C
// Owner: Frank Dachille (viatronix@usa.com)


/**
 * Description: A class to support guided navigation within the colon lumen.
 *   It contains a physical simulation of an object with mass, rotation
 *   mass, forward/reverse thrust, and pitch/heading rotation thrust.
 */


// includes
#include "StdAfx.h"
#include "Submarine.h"
#include "Matrix.h"
#include "Timer.h"
#include "ReaderGlobal.h"
#include "SpiralIterator.h"
#include "Dataset.h"


// defines
#define FILE_REVISION "$Revision: 1.12.2.13 $"


// namespaces
using namespace ReaderLib;


// Initialize constants
const float Submarine::m_fMaximumPotentialTime(0.75F); // seconds
// some constants to tune the feel of navigation
float4 Submarine::m_fIdealFrameRate(90.0F);
float4 Submarine::m_fTurnRateScale(0.0015F);
float4 Submarine::m_fThrustScale(0.015F);
bool Submarine::m_bPerceivedSpeed(false);


/**
 * Constructor.
 * Responsible for setting up all default values.
 */
Submarine::Submarine()
{
  Reset();
} // Submarine()


/**
 * Destructor.
 */
Submarine::~Submarine()
{
} // ~Submarine()


/**
 * Copy constructor.
 * 
 * @param other             The other submarine to copy from
 */
Submarine::Submarine(const Submarine & rhs)
{
  // Just call the assignment operator
  *this = rhs;
} // Submarine(rhs)


/**
 * Assignment operator.
 * 
 * @param other             the other submarine to copy from
 * @return myself
 */
Submarine & Submarine::operator =(const Submarine & rhs)
{
  // test for self assignment
  if (&rhs == this)
  {
    return *this;
  }

  m_vpPredictedPosition = rhs.m_vpPredictedPosition;
  m_bSearchNeighbors = rhs.m_bSearchNeighbors;
  m_bReplayMode = rhs.m_bReplayMode;
  m_iReplayFrame = rhs.m_iReplayFrame;
  m_replayDeque = rhs.m_replayDeque;
  m_potentialTimer = rhs.m_potentialTimer;
  m_vpPositionWorld = rhs.m_vpPositionWorld;
  m_vpPositionVolume = rhs.m_vpPositionVolume;
  m_vpLastCenterlinePositionWorld = rhs.m_vpLastCenterlinePositionWorld;
  m_vnLastCenterlineOrientation = rhs.m_vnLastCenterlineOrientation;
  m_vnLastCenterlineUp = rhs.m_vnLastCenterlineUp;
  m_vnUp = rhs.m_vnUp;
  m_vnOrientation = rhs.m_vnOrientation;
  m_fSpeed = rhs.m_fSpeed;
  m_fHeadingTurnRate = rhs.m_fHeadingTurnRate;
  m_fPitchTurnRate = rhs.m_fPitchTurnRate;
  m_fThrottle = rhs.m_fThrottle;
  m_fPotentialStrength = rhs.m_fPotentialStrength;
  m_fThrustInput = rhs.m_fThrustInput;
  m_fHeadingInput = rhs.m_fHeadingInput;
  m_fPitchInput = rhs.m_fPitchInput;
  m_vPan = rhs.m_vPan;
  m_bPan = rhs.m_bPan;
  m_bLeveling = rhs.m_bLeveling;
  m_bPotential = rhs.m_bPotential;
  m_bBoundary = rhs.m_bBoundary;
  m_bTestStats = rhs.m_bTestStats;
  m_eTransitionMode = rhs.m_eTransitionMode;
  m_bTransitioning = rhs.m_bTransitioning;
  m_uTransitionIndex = rhs.m_uTransitionIndex;
  // we want the following members to be re-initialized:
  // m_accelerationTimer, m_frameTimer, m_uViewIdentifier,
  return * this;
} // operator =(rhs)


/**
 * Reset all data members to default values.
 */
void Submarine::Reset()
{
  // Initialize to default
  m_pDataset = & rdrGlobal.m_nullDataset;
  m_vpPredictedPosition = Point<float4>(0.0F, 0.0F, 0.0F);

  // Initialize the replay facility
  m_bReplayMode = false;
  m_iReplayFrame = 0;
  m_replayDeque.clear();

  // Select the initial point at the origin
  m_vpLastCenterlinePositionWorld = m_vpPositionWorld = Point<float4>(0.0F, 0.0F, 0.0F);
  m_vpPositionVolume = Point<float4>(0.0F, 0.0F, 0.0F);

  // Select the initial orientation as the first derivative of
  // the skeleton using a forward difference
  m_vnLastCenterlineUp          = m_vnUp          = Normal<float4>(0.0F, 1.0F, 0.0F);
  m_vnLastCenterlineOrientation = m_vnOrientation = Normal<float4>(0.0F ,0.0F ,-1.0F);

  // Set the initial rotational velocity and acceleration to zero
  m_fSpeed = 0.0F;
  m_fHeadingTurnRate = 0.0F;
  m_fPitchTurnRate = 0.0F;

  // Specify the maximum speed, turning rates, and times
  m_fThrottle = 1.0F;
  m_fPotentialStrength = 1.0F;

  // Default the input to zero
  m_fThrustInput = 0.0F;
  m_fHeadingInput = 0.0F;
  m_fPitchInput = 0.0F;
  m_vPan = Vector<float4>(0.0F, 0.0F, 0.0F);
  m_bPan = false;

  // Transition default to smooth
  m_eTransitionMode = Submarine::TRANSITION_OTHER;
  m_bTransitioning = false;
  m_uTransitionIndex = 0;
  m_transitions.clear();

  // Default to off
  m_bLeveling = false;
  m_bPotential = false;
  m_bBoundary = true;

  // Neighbor searching avoids local minima
  m_bSearchNeighbors = true;

  m_uViewIdentifier = 0;

  m_bTestStats = false;

} // Reset()


/**
 * sets position, orientation and up vector
 * @param position          new position in world coordinates
 * @param orientation       new view direction in world coordinates
 * @param upDirection       new up direction in world coordinates
 */
void Submarine::SetCameraWorld(const Point<float4> & vpPosition, const Normal<float4> & vnOrientation, const Normal<float4> & vnUpDirection)
{
  SetPositionWorld(vpPosition);
  SetOrientation(vnOrientation);
  SetUpDirection(vnUpDirection);
  UpdatePosition();
//  FlushAccumulationBuffer();

} // SetWorldCamera


/**
 * Set current view position.
 * 
 * @param position          new position in world coordinates
 */
void Submarine::SetPositionWorld(const Point<float4> & vpPositionWorld)
{
  m_vpPositionWorld = vpPositionWorld;
  m_vpPositionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * m_vpPositionWorld;

  if( m_pDataset->IsLoadingComplete() )
  {
    // set up this point for rendering planes at correct position.
    m_pDataset->m_vOrthogonalPtWorld = m_vpPositionWorld;
    m_pDataset->m_vLastUpdatedOrthoPtWorld = m_vpPositionWorld;
    // update the cross-section sliding bar position    
    m_pDataset->m_iXsectionScrollBarLocationIndex = m_pDataset->m_joinedSkeleton.ConvertToJoinedIndex( m_pDataset->m_segmentList.GetCurrentSegment(), 
                                                         m_pDataset->m_fields.GetClosestSkeletonIndex(m_vpPositionVolume) );
  }

  // indicate that this view could be newer than the last known view
  m_uViewIdentifier++;

} // SetPositionWorld()


/**
 * Begin to add forces to the simulation.
 */
void Submarine::StartFlying()
{
  // reset the timers
  m_accelerationTimer.Reset();
  m_frameTimer.Reset();

  // get out of cruise control mode
  SwitchToManualMode();
  
  // gracefully get out of replay mode
  ExitReplayMode();
} // StartFlying()


/**
 * Remove all forces from the model and allow to coast.
 */
void Submarine::StopFlying()
{
  SetForces(0.0F, 0.0F, 0.0F);
  m_accelerationTimer.Reset();
} // StopFlying()


/**
 * The update function should be called at regular intervals.
 * It moves the submarine along its path and rotates it accordingly.
 */
void Submarine::UpdatePosition()
{
  try
  {
    // do nothing if we are in replay mode
    if (m_bReplayMode)
    {
      return;
    }

    // if we are transition from manual back to automatic and TRANSITION_SMOOTH is specified
    if( m_bTransitioning )
    {
      /// update submarine position from transition vector
      TransitionInfo info = m_transitions[ m_uTransitionIndex++ ];
      SetPositionWorld( info.m_vpPositionWorld );
      SetOrientation( info.m_vnOrientation );
      SetUpDirection( info.m_vnUp );
      m_bTransitioning =  m_uTransitionIndex < m_transitions.size()? true : false;

      EnforceOrthogonality();
      
      if( !m_bTransitioning )
      {
        SetPotential( true );
      }
      else
      {
        return;
      }
    }

    if ( GetPercentagePotential( m_potentialTimer.ElapsedTime() ) )
    {
      m_vpLastCenterlinePositionWorld = m_vpPositionWorld;
      m_vnLastCenterlineOrientation = m_vnOrientation;
      m_vnLastCenterlineUp = m_vnUp;
    }

    // determine the elapsed time since the last update
    const float4 fElapsedTime(float4(m_frameTimer.ElapsedTime()));
    m_frameTimer.Reset();

    // All the constants are tuned for 60 frames/second.  If the elapsed time
    // is greater than 1/60th of a second, then apply friction multiple times
    // to make up for the missed updates.
    uint4 uUpdates(1);

    if (fElapsedTime > 1.0 / m_fIdealFrameRate) 
    {
      uUpdates = int4(fElapsedTime * m_fIdealFrameRate);
    } // more than one update required
    // clamp to stave off numerical problems
    const uint4 uMinUpdates(1);
    const uint4 uMaxUpdates(1000);
    uUpdates = Bound(uMinUpdates, uUpdates, uMaxUpdates);

    const float4 fMaximumDistance(0.5F);	// distance in mm
    const float4 fMininumDistance(fMaximumDistance * 0.5F);
    // the maximum amount of movement in world space (mm)
    const float4 fOffsetScale(0.1F);
    // Flush accumulation buffer if we have moved too far
    const float4 fMovementThreshold(0.1F); // 0.1mm
    vxBlockVolumeIterator<uint2> csiIter( m_pDataset->m_fields.m_csi );

    for (uint4 i(0); i<uUpdates; i++) 
    {  
      // update the heading turn rate
      m_fHeadingTurnRate += m_fThrottle * m_fHeadingInput * m_fTurnRateScale;

      // update the pitch turn rate
      m_fPitchTurnRate += m_fThrottle * m_fPitchInput * m_fTurnRateScale;

      // either pan or rotate the viewpoint
      if (m_bPan) 
      {
        // compute offset from original pan location
        Vector<float4> vOffset(m_fHeadingInput, m_fPitchInput, 0);
        
        // compute the world space vector of movement
        Normal<float4> nRight(m_vnOrientation.Cross(m_vnUp));
        Vector<float4> vOffsetWorld(Vector<float4>(nRight) * vOffset.m_x +
          Vector<float4>(m_vnUp) * vOffset.m_y);

        vOffsetWorld *= fOffsetScale;

        // don't move too far in one update
        if (vOffsetWorld.GetLength() > fMaximumDistance)
        {
          vOffsetWorld.Normalize();
          vOffsetWorld *= fMaximumDistance;
        } // moved too far

        // offset the viewpoint
        m_vpPositionWorld -= vOffsetWorld;
        m_vpPositionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * m_vpPositionWorld;
      } 
      else
      {
        // rotate for heading (left-right)
        Matrix<float4> mRotationMatrix;
        mRotationMatrix.Rotate(m_vnUp,-m_fHeadingTurnRate);
        m_vnOrientation = mRotationMatrix * m_vnOrientation;
        Normal<float4> right = Normal<float4>(m_vnOrientation.Cross(m_vnUp));

        // rotate for pitch (up-down)
        mRotationMatrix.Identity();
        mRotationMatrix.Rotate(right,m_fPitchTurnRate);
        m_vnOrientation = mRotationMatrix * m_vnOrientation;
        m_vnUp = mRotationMatrix * m_vnUp;
      }

      // update the speed and user's desired velocity
      m_fSpeed += m_fThrottle * m_fThrustInput * m_fThrustScale;

      Vector<float4> vVelocity = Vector<float4>(m_vnOrientation) * m_fSpeed;
      // Apply friction, leveling, boundary, and potential fields
      vVelocity += GetFieldForce();

      // This is a super-duper hack!!!  Rather than figuring out how m_fSpeed affects
      //  velocity change differently in manual- as opposed to auto-flight, then spending
      //  the time to go thru the usual programming/debugging iterations (e.g. change something,
      //  change both, change something else, screw up both)
      //  I merely conducted a number of timing tests and adjusted for the difference.
      // I know, I am ashamed
      if ( GetPotential() )
      { // Hack adjustment for slower auto-flight velocity
        vVelocity *= 1.82F;
      }

      // Test to make sure we have not gone too far in this step
      if ( vVelocity.GetLength() > fMaximumDistance )
      {
        vVelocity.Normalize();
        vVelocity *= fMaximumDistance;
      } // end if moved too far

      // Apply affect of m_bPerceivedSpeed here, likely by reducing vVelocity by a percentage
      //  based on distance of nearest centerline point to the wall
      if ( m_bPerceivedSpeed )
      {
        const Skeleton& skel = m_pDataset->m_segmentList.GetCurrentSkeleton();
        uint4 uCsi = csiIter.Peek( m_vpPositionVolume.m_x + 0.5F, m_vpPositionVolume.m_y + 0.5F, m_vpPositionVolume.m_z + 0.5F );
        const bool bNormalDirn = ( (GetFlightDirection() == RECTUM_TO_CECUM) == m_pDataset->m_segmentList.GetCurrentSegment().IsRegularDirection() );
        const int4 iOffset = bNormalDirn ? 5 : -5;

        const int4 iNumElems(int4(skel.GetNumOfElements()-1));
        uCsi = Bound(int4(0), int4(uCsi)+iOffset, iNumElems);

        float4 fRadiusTtl(0.0f);
			  uint4 uCt(0);
        for (uint4 uCsiWalk(max(uCsi-50,0)), uCt(0); uCsiWalk < min(uCsi+50,iNumElems); uCsiWalk+=5, uCt++)
        {
          Point<float4> viewPtVol = skel.GetSkelNode(uCsiWalk).m_vVolumePos;
          fRadiusTtl += GetResampledSld(viewPtVol) * m_pDataset->m_fields.GetSldScale();
        }

        const float4 fMinRadius(5.0f), fMaxRadius(11.0f);
        float4 fRadius = max(fRadiusTtl/float4(uCt), fMinRadius);

        if (fRadius < fMaxRadius)
        {
          vVelocity *= fRadius*0.75f/fMaxRadius;
        }
      }

      // Keep the viewpoint inside the volume
      ConstrainPosition();

      // Update position
      m_vpPositionWorld += vVelocity;
      m_vpPositionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * m_vpPositionWorld;

      if (vVelocity.GetLength() > 0.001F)
      // if (vVelocity.GetLength() > 0.0F)
      {
        m_uViewIdentifier++;
     } // end if position changed significantly


      if (vVelocity.GetLength() > fMovementThreshold) 
      {
        m_pDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();

      } // end if moved significantly this update

    } // performing updates


    // update the orthogonal and cross-sectional 
    if (m_pDataset->m_vLastUpdatedOrthoPtWorld.GetEuclideanDistance(m_vpPositionWorld) > 1.0f)
    {
      m_pDataset->m_vOrthogonalPtWorld = m_vpPositionWorld;
      m_pDataset->m_vLastUpdatedOrthoPtWorld = m_vpPositionWorld;

      // update the cross-section sliding bar position    
      m_pDataset->m_iXsectionScrollBarLocationIndex = m_pDataset->m_joinedSkeleton.ConvertToJoinedIndex( m_pDataset->m_segmentList.GetCurrentSegment(), 
        m_pDataset->m_fields.GetClosestSkeletonIndex(m_vpPositionVolume) );
    }

    // Re-enforce the orthogonality constraint for good measure
    EnforceOrthogonality();

    // Orient the view direction according to the predicted position
    // Compute the percentage of the potential field that we should experience
    float4 fPercentagePotential = GetPercentagePotential(m_potentialTimer.ElapsedTime());
    if (fPercentagePotential > 0) fPercentagePotential = 1.0F;  // force to 0 or 1 in order to avoid weaving during starup and slowdown
    OrientPotential(m_fIdealFrameRate, fPercentagePotential);
    
    // Save the current position in the history buffer
    SaveHistoryFrame();

    // Turn off the potential field if we are at the very start
    // or the very end of the skeleton
    Point<float4> vTarget;
    if ((GetFlightDirection() == RECTUM_TO_CECUM) && m_pDataset->m_segmentList.IsLastSegmentCurrent())
    {
      // if we are traveling from start to end and we are close to the end
      vTarget = m_pDataset->m_segmentList.GetCurrentSegment().GetEndingSkelNode().m_vWorldPos;
    }
    else if ((GetFlightDirection() == CECUM_TO_RECTUM) && m_pDataset->m_segmentList.IsFirstSegmentCurrent())
    {
      // if we are traveling from end to start and we are close to the start
      vTarget = m_pDataset->m_segmentList.GetCurrentSegment().GetBeginningSkelNode().m_vWorldPos;
    }
    else
    {
      // keep moving otherwise
      vTarget = Point<float4>(0.0F, 0.0F, 0.0F);
    } // if rectum->cecum

    // see if we are close to the "end" of a segment
    const float8 fDistanceCloseToEnd(2.0F); // mm
    if (Vector<float4>(m_vpPositionWorld - vTarget).GetLength() < fDistanceCloseToEnd)
    {
      // turn off the autoflight
      SetPotential(false);
    } // close to either end
  } // try
  catch ( ... )
  {
    LogWrn( "unhandled exception", "Submarine", "UpdatePosition" );
  }
} // UpdatePosition()


/**
 * Update the view to a position along the skeleton.
 * 
 * @param uIndex             the index into the skeleton array
 */
void Submarine::SetViewSkeleton(const uint2 & uIndex)
{
  // clamp to valid range
  uint2 uMyIndex = uIndex;
  uint2 uMinIndex = 0;
  Skeleton & skeleton = m_pDataset->m_segmentList.GetCurrentSkeleton();
  uint2 uMaxIndex = skeleton.GetNumOfElements()-1;
  uMyIndex = Bound(uMinIndex, uMyIndex, uMaxIndex);
  Point<float4> startingPointWorld = skeleton.GetSkelNode(uMyIndex).m_vWorldPos;
  SetPositionWorld(startingPointWorld);
  
  // initialize the view direction
  Point<float4> startingPointVolume = skeleton.GetSkelNode(uMyIndex).m_vWorldPos;
  if (GetFlightDirection() == RECTUM_TO_CECUM)
  {
    SetOrientation(skeleton.GetSkelNode(uMyIndex).m_vWorldTan);
  }
  else
  {
    SetOrientation(-(skeleton.GetSkelNode(uMyIndex).m_vWorldTan));
  }
  EnforceOrthogonality();
  m_pDataset->m_vOrthogonalPtWorld = m_vpPositionWorld;

  if( m_pDataset->IsLoadingComplete() )
  {
    // update the cross-section sliding bar position    
    m_pDataset->m_iXsectionScrollBarLocationIndex = m_pDataset->m_joinedSkeleton.ConvertToJoinedIndex( m_pDataset->m_segmentList.GetCurrentSegment(), 
                                                         m_pDataset->m_fields.GetClosestSkeletonIndex(m_vpPositionVolume) );
  }

} // SetViewSkeleton(const uint2 & uIndex)


/**
 * Update the view to a position along the skeleton.
 * 
 * @param fIndex             the index into the skeleton array
 * @param bFlip              indicates the direction for the view forward or backward
 */
void Submarine::SetViewSkeleton(const float4 & fIndex, const bool bFlip)
{
  // this allows sub-position locations.
  uint2 uStartIndex = (uint2)fIndex;
  uint2 uEndIndex = (uint2)fIndex + 1;
  float4 fWeight = fIndex - uStartIndex;

  uint2 uMinIndex = 0;
  Skeleton & skeleton = m_pDataset->m_segmentList.GetCurrentSkeleton();
  uint2 uMaxIndex = skeleton.GetNumOfElements()-1;
  uStartIndex = Bound(uMinIndex, uStartIndex, uMaxIndex);
  uEndIndex = Bound(uMinIndex, uEndIndex, uMaxIndex);

  // set the interpolated world position
  Point<float4> startingPointWorld = skeleton.GetSkelNode(uStartIndex).m_vWorldPos;
  Point<float4> endingPointWorld   = skeleton.GetSkelNode(uEndIndex).m_vWorldPos;
  Point<float4> interpolatepoint(startingPointWorld.m_x * (1.0f-fWeight) + endingPointWorld.m_x * fWeight,
                                 startingPointWorld.m_y * (1.0f-fWeight) + endingPointWorld.m_y * fWeight,
                                 startingPointWorld.m_z * (1.0f-fWeight) + endingPointWorld.m_z * fWeight);
  SetPositionWorld(interpolatepoint);
  
  // set the interpolated view direction
  Normal<float4> startingPointNormal = skeleton.GetSkelNode(uStartIndex).m_vWorldTan;
  Normal<float4> endingPointNormal   = skeleton.GetSkelNode(uEndIndex).m_vWorldTan;
  Normal<float4> interpolateNormal(startingPointNormal.X() * (1.0f-fWeight) + endingPointNormal.X() * fWeight,
                                   startingPointNormal.Y() * (1.0f-fWeight) + endingPointNormal.Y() * fWeight,
                                   startingPointNormal.Z() * (1.0f-fWeight) + endingPointNormal.Z() * fWeight);
  SetOrientation((GetFlightDirection()==RECTUM_TO_CECUM) ? 
                      (bFlip ? -interpolateNormal : interpolateNormal) : 
                      (bFlip ? interpolateNormal : -interpolateNormal));

  // set the interpolated up direction
  Normal<float4> startingPointUp = skeleton.GetSkelNode(uStartIndex).m_vWorldUp;
  Normal<float4> endingPointUp   = skeleton.GetSkelNode(uEndIndex).m_vWorldUp;
  Normal<float4> interpolateUp(startingPointUp.X() * (1.0f-fWeight) + endingPointUp.X() * fWeight,
                               startingPointUp.Y() * (1.0f-fWeight) + endingPointUp.Y() * fWeight,
                               startingPointUp.Z() * (1.0f-fWeight) + endingPointUp.Z() * fWeight);
  SetUpDirection(interpolateUp);

  EnforceOrthogonality();
  //UpdatePosition();
  m_pDataset->m_vOrthogonalPtWorld = m_vpPositionWorld;
  m_vpLastCenterlinePositionWorld = m_vpPositionWorld;
  m_vnLastCenterlineOrientation = m_vnOrientation;
  m_vnLastCenterlineUp = m_vnUp;

  if( m_pDataset->IsLoadingComplete() )
  {
    // update the cross-section sliding bar position    
    m_pDataset->m_iXsectionScrollBarLocationIndex = m_pDataset->m_joinedSkeleton.ConvertToJoinedIndex( m_pDataset->m_segmentList.GetCurrentSegment(), 
                                                         m_pDataset->m_fields.GetClosestSkeletonIndex(m_vpPositionVolume) );
  }

} // SetViewSkeleton(const float4 & fIndex, const bool bFlip)


/**
 * Update the view to a position along the skeleton.
 * 
 * @param skelNode           the desired skeleton node
 * @param bFlip              indicates the direction for the view forward or backward
 */
void Submarine::SetViewSkeleton(const SkeletonNode & skelNode, const bool bFlip)
{
  SetPositionWorld(skelNode.m_vWorldPos);
  if (bFlip) 
  {
    SetOrientation(-skelNode.m_vWorldTan);
  } 
  else 
  {
    SetOrientation( skelNode.m_vWorldTan);
  }
  EnforceOrthogonality();
  m_pDataset->m_vOrthogonalPtWorld = m_vpPositionWorld;
  m_vpLastCenterlinePositionWorld = m_vpPositionWorld;
  m_vnLastCenterlineOrientation = m_vnOrientation;
  m_vnLastCenterlineUp = m_vnUp;

  if( m_pDataset->IsLoadingComplete() )
  {
    // update the cross-section sliding bar position    
    m_pDataset->m_iXsectionScrollBarLocationIndex = m_pDataset->m_joinedSkeleton.ConvertToJoinedIndex( m_pDataset->m_segmentList.GetCurrentSegment(), 
                                                         m_pDataset->m_fields.GetClosestSkeletonIndex(m_vpPositionVolume) );
  }

} // SetViewSkeleton(const SkeletonNode & skelNode, const bool flip)


/**
 * Update the view to a position and direction.
 * 
 * const Point<float4> & vpPositionWorld    world position
 * const Normal<float4> & vnViewDirection   view direction
 * const Normal<float4> & vnUp              view up orientation
 */
void Submarine::SetView(const Point<float4> & vpPositionWorld, const Normal<float4> & vnViewDirection, const Normal<float4> & vnUp)
{
  SetPositionWorld(vpPositionWorld);
  SetOrientation(vnViewDirection);
  SetUpDirection(vnUp);
  EnforceOrthogonality();
  m_pDataset->m_vOrthogonalPtWorld = m_vpPositionWorld;
  if( m_pDataset->IsLoadingComplete() )
  {
    // update the cross-section sliding bar position    
    m_pDataset->m_iXsectionScrollBarLocationIndex = m_pDataset->m_joinedSkeleton.ConvertToJoinedIndex( m_pDataset->m_segmentList.GetCurrentSegment(), 
                                                         m_pDataset->m_fields.GetClosestSkeletonIndex(m_vpPositionVolume) );
  }

  m_vpLastCenterlinePositionWorld = m_vpPositionWorld;
  m_vnLastCenterlineOrientation = m_vnOrientation;
  m_vnLastCenterlineUp = m_vnUp;

} // SetView


/**
 * Compute the preferred direction for flying in volume coordinates
 * 
 * @param positionVolume    the input position in volume coordinates
 * @return the change in position in volume coordinates
 */
Vector<float4> Submarine::GetPotentialGradient(const Point<float4> & positionVolume) const
{
  // select the proper direction
  Segment & segment= m_pDataset->m_segmentList.GetCurrentSegment();
  bool bRegularDirection = segment.IsRegularDirection();
  bool bFromStart = true;
  if ((GetFlightDirection() == RECTUM_TO_CECUM && bRegularDirection == true ) ||
      (GetFlightDirection() == CECUM_TO_RECTUM && bRegularDirection == false)   ) 
  {
    bFromStart = false;
  } // if forward direction
  
  // compute the potential field direction
  return m_pDataset->m_fields.GetMinimumPotentialDirection(positionVolume, bFromStart);
} // GetPotentialGradient() const


/**
 * Go back in time and replay past frames.
 * 
 * @param frames            the desired number of frames into the past
 */
void Submarine::ReplayPosition(const int4 iFrames)
{
  // can only replay if we have some history
  if (m_replayDeque.size() > 0) 
  {   
    // compute the new replay frame
    m_iReplayFrame += iFrames;
    m_iReplayFrame = Bound(0, m_iReplayFrame, int4(m_replayDeque.size()-1));

    // Ignore the first few requests, because the middle
    // mouse is easy to spin when you are really trying
    // to push it.
    const uint4 threshold = 3;
    if (m_bReplayMode == false && m_iReplayFrame > threshold) 
    {
      m_bReplayMode = true;
    } // delay entering replay mode
    
    // only update the viewpoint if we are now in replay mode
    if (m_bReplayMode == true) 
    {
      // update the current camera position to the old position
      Camera currCamera(m_replayDeque[m_iReplayFrame]);
      m_vpPositionWorld = currCamera.GetVrp();
      m_vpPositionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * m_vpPositionWorld;
      m_vnOrientation = currCamera.GetVpn();
      m_vnUp = currCamera.GetVup();
      m_pDataset->m_vOrthogonalPtWorld = m_vpPositionWorld;

      // update the cross-section sliding bar position    
      m_pDataset->m_iXsectionScrollBarLocationIndex = m_pDataset->m_joinedSkeleton.ConvertToJoinedIndex( m_pDataset->m_segmentList.GetCurrentSegment(), 
                                                           m_pDataset->m_fields.GetClosestSkeletonIndex(m_vpPositionVolume) );

      m_uViewIdentifier++;
      m_fHeadingTurnRate = 0;
      m_fPitchTurnRate = 0;
      m_fSpeed = 0;
      SetPotential( false );
      m_potentialTimer.SetElapsedTime( m_fMaximumPotentialTime );
    } // end if replay mode
  } // end if history size

  // start out fresh
  m_pDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
    
} // Replay(const int & frames)


/**
 * Reverse the current view direction and stop moving.
 */
void Submarine::ReverseDirection()
{
  // don't do anything if we are in replay mode
  if (m_bReplayMode) 
  {
    return;
  } // in replay mode

  m_uViewIdentifier++;
  FullStop();

  // reverse the orientation
  m_vnOrientation = -m_vnOrientation;
  
  // stop turning
  m_fHeadingTurnRate = 0;
  m_fPitchTurnRate = 0;
} // ReverseDirection()



/**
 * Enforce the orthogonality constraint (between the forward and up directions)
 *
 */
void Submarine::EnforceOrthogonality()
{
  // Project the view up vector so that it's perpendicular to the orientation
  // vector.  The dot product between them should be zero.
  float4 dotProduct = m_vnOrientation.Dot(m_vnUp);
  // This should remove any skew in the up vector so that
  // they are orthogonal.
  m_vnUp = Normal<float4>(Vector<float4>(m_vnUp) - Vector<float4>(m_vnOrientation) * dotProduct);
} // EnforceOrthogonality()


/**
 * Predict where we will be in a few seconds and look there.
 *
 * @param idealFrameRate          the best possible frame rate
 * @param fPercentagePotential    amount of potential field to be used
 */
void Submarine::OrientPotential(const float4 fIdealFrameRate, const float4 fPercentagePotential)
{
  // predict where we will be in a few seconds
  const float4 fSecondsLookAhead = 3.0F;
  const uint4 uPredictionSteps = fIdealFrameRate * fSecondsLookAhead;
  m_vpPredictedPosition = m_vpPositionWorld;
  for (uint4 i=0; i<uPredictionSteps; i++) 
  {
    // Translate the viewpoint along the potential field
    Vector<float4> vPotentialVector = GetPotentialField(m_vpPredictedPosition, fPercentagePotential);
    // Test to make sure we have not gone too far in this step
    const float4 fMaximumDistance(0.5F);	// 0.5mm
    if (vPotentialVector.GetLength() > fMaximumDistance) 
    {
      vPotentialVector.Normalize();
      vPotentialVector *= fMaximumDistance;
    } // moved too far
    // update the position
    m_vpPredictedPosition += vPotentialVector;
  } // iterate prediction loop

  // Orient according to the potential field
  // compute the preferred direction
  Vector<float4> vNewOrientationVector(m_vpPredictedPosition - m_vpPositionWorld);
  // only orient if there is significant movement
  const float8 fDistanceThreshold(1);	// mm
  if ( (fPercentagePotential > 0) && (vNewOrientationVector.GetLength() > fDistanceThreshold) )
  {
    // compute the preferred direction
    Normal<float4> nNewOrientation(vNewOrientationVector);

    // compute a weighted average of the current and preferred orientations
    // Specify the strength of the potential field orientation
    const float4 fOrientationStrength(0.15F);
    m_vnOrientation = Normal<float4>(
      Vector<float4>(m_vnOrientation) * (1.0F - fOrientationStrength) +
      Vector<float4>(nNewOrientation) * fOrientationStrength);

    // project view up vector so it's perpendicular to the orientation vector
    EnforceOrthogonality();
    m_uViewIdentifier++;
  } // if potential field
} // OrientPotential(const float & idealFrameRate)


/**
 * Apply friction, leveling, boundary, and potential fields
 * 
 * @return the change in position after applying fields
 */
const Vector<float4> Submarine::GetFieldForce()
{
  // Friction slows it down.
  // Specify the amount of friction
  const float4 fPositionFriction = 0.92F;
  const float4 fOrientationFriction = 0.92F;
  m_fSpeed *= fPositionFriction;
  m_fHeadingTurnRate *= fOrientationFriction;
  m_fPitchTurnRate *= fOrientationFriction;

  // Optionally perform leveling roll
  if (m_bLeveling) 
  {
    ApplyLeveling();
  }

  // Compute the percentage of the potential field that
  // we should experience
  const float4 fPercentagePotential = GetPercentagePotential(m_potentialTimer.ElapsedTime());

  // Translate the viewpoint along the potential field
  const Vector<float4> vPotentialVector = GetPotentialField(m_vpPositionWorld, fPercentagePotential);

  // Translate the viewpoint away from the wall
  const Vector<float4> vBoundaryVector = GetBoundaryField(m_vpPositionWorld, 1.0F-fPercentagePotential);

  return vPotentialVector + vBoundaryVector;
} // GetFieldForce()


/**
 * Return whether or not the submarine is currently flying.
 * 
 * @return true if the submarine is flying
 */
bool Submarine::IsFlying()
{
  if( m_bTransitioning )
    return true;

  float4 fPercentagePotential = GetPercentagePotential(m_potentialTimer.ElapsedTime());
  const float4 fMinimumSpeed = 0.001F;
  const float4 fMinimumTurnRate = 0.001F;
  if ((fabs(m_fSpeed) > fMinimumSpeed ||
       fPercentagePotential > 0 ||
       fabs(m_fHeadingTurnRate) > fMinimumTurnRate ||
       fabs(m_fPitchTurnRate) > fMinimumTurnRate) &&
      m_bReplayMode == false) 
  {
    return true;
  } 
  else 
  { 
    // not flying
    return false;
  } // flying
} // IsFlying()


/**
 * Pan the viewpoint parallel to the image plane.
 * 
 * @param newPanLocation    the new pan location
 */
void Submarine::PanPosition(const Point<float4> & newPanLocation)
{
  return;
  // compute offset from original pan location
  Vector<float4> vNewLocation(newPanLocation);
  Vector<float4> vOffset(vNewLocation - m_vPan);

  // compute the world space vector of movement
  Normal<float4> nRight(m_vnOrientation.Cross(m_vnUp));
  Vector<float4> vOffsetWorld(Vector<float4>(nRight) * vOffset.m_x +
    Vector<float4>(m_vnUp) * vOffset.m_y);

  // the maximum amount of movement in world space (mm)
  const float4 offsetScale = 10.0F;
  vOffsetWorld *= offsetScale;

  // don't move too far in one update
  const float4 maxDistance = 0.5F; // mm
  if (vOffsetWorld.GetLength() > maxDistance) 
  {
    vOffsetWorld.Normalize();
    vOffsetWorld *= maxDistance;
  } // moved too far

  // offset the viewpoint
  m_vpPositionWorld -= vOffsetWorld;
  m_vpPositionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * m_vpPositionWorld;

  // save the pan location as the old pan location
  m_vPan = vNewLocation;
} // PanPosition(const Point<float> & newPanLocation)


/**
 * Gracefully get out of replay mode.
 */
void Submarine::ExitReplayMode()
{
  // gracefully get out of replay mode
  if (m_bReplayMode) 
  {
    // pop from the front until we are at the current replay position
    while (m_iReplayFrame > 0) 
    {
      m_iReplayFrame--;
      m_replayDeque.pop_front();
    } // popping the stack

    m_bReplayMode = false;
  } // replay mode
} // ExitReplayMode()


/**
 * Routine to initialize the data structures.
 * The skeleton must be loaded already.
 * 
 * @return true on success
 */
bool Submarine::Initialize()
{
  // initialize the viewpoint
  const uint2 startIndex = 10;
  SetViewSkeleton(startIndex);

  return true;
} // Initialize()


/**
 * Translate the position along the gradient field one timestep
 * 
 * @param positionWorld     the input position in world coordinates
 * @param percentage        the percentage potential strength
 * @return the change in position in world coordinates
 */
const Vector<float4> Submarine::GetPotentialField(const Point<float4> & vpPositionWorld, const float4 fPercentage) const
{
  // early return if there is no force
  if (fPercentage * m_fPotentialStrength == 0) 
  {
    return Vector<float4>(0,0,0);
  } // no force

  // convert to volume space
  Point<float4> positionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * vpPositionWorld;

  // get the gradient and normalize
  Vector<float4> vGradientVolume = GetPotentialGradient(positionVolume);
  Vector<float4> vGradientWorld = m_pDataset->m_fields.m_volumeRender.GetVolumeToWorldMatrix() * vGradientVolume;
  vGradientWorld.Normalize();

  // scale by the potential strength factor
  const float4 fScale = 0.1F;
  vGradientWorld *= fPercentage * m_fPotentialStrength * fScale;

  // update the position and return it
  return vGradientWorld;
} // GetPotentialField(positionWorld, percentage) const


/**
 * Resample the space leap volume at a fractional position
 *
 * @param   vpPositionVolume  the fractional position at which to resample the volume
 * @return  the resampled space leap volume value
 */
float4 Submarine::GetResampledSld(const Point<float4> & vpPositionVolume) const
{
  // early return if no data is available
  if (m_pDataset == NULL || !m_pDataset->IsLoadingComplete()) 
  {
    LogWrn("Null dataset.", "Submarine", "GetResampledSld");
    return -1.0;
  } // if null data

  LinVolUC * pSld = & (m_pDataset->m_fields.m_sldInterior);
  LinVolUC * pUld = NULL;
	if( rdrGlobal.m_bUseULDvolume )
	{
		pUld = & (m_pDataset->m_fields.m_sldExterior);  //:: Use this for un-cleansed SLD, i.e. the ULD volume
	}
  Triple<uint4> dimension(pSld->GetDim());

  // bounds check
  if (vpPositionVolume.m_x < 0 || vpPositionVolume.m_x >= dimension.m_x ||
      vpPositionVolume.m_y < 0 || vpPositionVolume.m_y >= dimension.m_y ||
      vpPositionVolume.m_z < 0 || vpPositionVolume.m_z >= dimension.m_z ) 
  {
    return -1.0;
  } // if out of bounds

  // truncate
  const uint4 uNewx = uint4(vpPositionVolume.m_x);
  const uint4 uNewy = uint4(vpPositionVolume.m_y);
  const uint4 uNewz = uint4(vpPositionVolume.m_z);

  // weights for tri-linear interpolation
  const float4 fWeightX = vpPositionVolume.m_x - uNewx;
  const float4 fWeightY = vpPositionVolume.m_y - uNewy;
  const float4 fWeightZ = vpPositionVolume.m_z - uNewz;

  // values for tri-linear interpolation
  const uint4 uXstep = 1;
  const uint4 uYstep = dimension.m_x;
  const uint4 uZstep = dimension.m_y * dimension.m_x;
  const uint4 uIndex = uNewx + (uNewy * uYstep) + (uNewz * uZstep);
  const uint4 uMaxIndex = uZstep * dimension.m_z;

  // indices for lower left front (llf), lower left back (llb), etc
  const uint4 illf = uIndex;
  const uint4 illb = uIndex + uZstep;
  const uint4 iulf = uIndex + uYstep;
  const uint4 iulb = uIndex + uYstep + uZstep;
  const uint4 ilrf = uIndex + uXstep;
  const uint4 ilrb = uIndex + uXstep + uZstep;
  const uint4 iurf = uIndex + uXstep + uYstep;
  const uint4 iurb = uIndex + uXstep + uYstep + uZstep;

  uint1 * pData = pSld->GetDataPtr();
	if( m_pDataset->m_fields.IsDiffVolLoaded() && rdrGlobal.m_bUseULDvolume && !rdrGlobal.m_bSpaceLeaping )
  {
    pData = pUld->GetDataPtr();  //:: Use this for un-cleansed SLD, i.e. the ULD volume
  } // if

  uint1 llf, llb, ulf, ulb, lrf, lrb, urf, urb;
  // bounds check indices and retrieve values
  // Note: bending coding guidelines for readability
  if (illf <= uMaxIndex) llf = *(pData + illf); else llf = 0;
  if (illb <= uMaxIndex) llb = *(pData + illb); else llb = 0;
  if (iulf <= uMaxIndex) ulf = *(pData + iulf); else ulf = 0;
  if (iulb <= uMaxIndex) ulb = *(pData + iulb); else ulb = 0;
  if (ilrf <= uMaxIndex) lrf = *(pData + ilrf); else lrf = 0;
  if (ilrb <= uMaxIndex) lrb = *(pData + ilrb); else lrb = 0;
  if (iurf <= uMaxIndex) urf = *(pData + iurf); else urf = 0;
  if (iurb <= uMaxIndex) urb = *(pData + iurb); else urb = 0;

  // tri-linear interpolation
  float4 llm = fWeightZ * llb + (1 - fWeightZ) * llf;
  float4 ulm = fWeightZ * ulb + (1 - fWeightZ) * ulf;
  float4 lrm = fWeightZ * lrb + (1 - fWeightZ) * lrf;
  float4 urm = fWeightZ * urb + (1 - fWeightZ) * urf;
  float4 mlm = fWeightY * ulm + (1 - fWeightY) * llm;
  float4 mrm = fWeightY * urm + (1 - fWeightY) * lrm;
  float4 mmm = fWeightX * mrm + (1 - fWeightX) * mlm;

  return mmm;
} // GetResampledSld(const Point<float4> & vpPositionVolume) const


/**
 * Translate the viewpoint away from the boundary
 * 
 * @param positionWorld     the input position in world coordinates
 * @param percentage        the percentage boundary strength
 * @return the change in position in world coordinates
 */
const Vector<float4> Submarine::GetBoundaryField(const Point<float4> & vpPositionWorld, const float4 fPercentage) const
{
  // early return for speed
  if (fPercentage == 0) 
  {
    return Vector<float4>(0.0F, 0.0F, 0.0F);
  } // no boundary force

  // convert world coordinates to volume coordinates
  Point<float4> positionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * vpPositionWorld;

  // get the scalar value at this point
  float4 fDistance = GetResampledSld(positionVolume) * m_pDataset->m_fields.GetSldScale();

  // apply a transfer function to make force high at the edge
  // and drop off quickly as you move away from the edge
  const float4 fMinDistance = 0.1F;	// maximum force at this and smaller
  const float4 fMaxDistance = 1.0F;	// zero force at this and larger
  if (fDistance < fMinDistance) 
  {
    fDistance = fMinDistance;
  } // outside valid range
  float4 fForce = 0.0F;

  // check to see if there is any influence at all from the wall
  if (fDistance < fMaxDistance)
  {
    // there is some interaction so compute the force
    const float4 fPercentFromEdge = (fDistance - fMinDistance) / (fMaxDistance - fMinDistance);
    const float8 fMaxForce = 100; // the maximum force
    const float8 fPower = 2;	// how steeply the force goes up at the edge
    fForce = float4(fMaxForce * pow(( double)1.0F-fPercentFromEdge, fPower) * m_bBoundary);

    // get the gradient and normalize
    Vector3D<float4> incVec(positionVolume);
    float4 vx0=GetResampledSld(Point<float4>(-1, 0, 0)+incVec);
    float4 vx1=GetResampledSld(Point<float4>(+1, 0, 0)+incVec);
    float4 vy0=GetResampledSld(Point<float4>( 0,-1, 0)+incVec);
    float4 vy1=GetResampledSld(Point<float4>( 0,+1, 0)+incVec);
    float4 vz0=GetResampledSld(Point<float4>( 0, 0,-1)+incVec);
    float4 vz1=GetResampledSld(Point<float4>( 0, 0,+1)+incVec);
    Vector<float4> vGradientWorld(vx1-vx0, vy1-vy0, vz1-vz0);
    if( vGradientWorld.GetLength() > 0.01 )
    {
      vGradientWorld.Normalize();
    }

    // scale by the potential strength factor
    vGradientWorld *= fPercentage * fForce;

    // update the position and return it
    return vGradientWorld;
  } 
  else 
  {
    // early return if there is no significant force
    return Vector<float4>(0.0F, 0.0F, 0.0F);
  } // end boundary force
} // GetBoundaryField() const


/**
 * The potential field ramps up over time when activated and
 * ramps back down over time when deactivated.
 * 
 * @param elapsedPotentialTime    how long since the change in potential status
 * @return the percentage strength of the potential field
 */
float4 Submarine::GetPercentagePotential(const float4 fElapsedPotentialTime) const
{
  // If we have started flying, then slowly start adding in potential force
  // ramp up over several seconds
  // Percentage of force due to potential field
  float4 fPercentagePotential;
  if (m_bPotential) 
  {
    // ramp up to full potential
    fPercentagePotential = fElapsedPotentialTime / m_fMaximumPotentialTime;

    // clamp to the range [0,1]
    fPercentagePotential = Bound(0.0F, fPercentagePotential, 1.0F);

  } 
  else 
  { 
    // just ended potential field

    // ramp down to zero poential
    fPercentagePotential = 1.0F - fElapsedPotentialTime / m_fMaximumPotentialTime;

    // clamp to the range [0,1]
    fPercentagePotential = Bound(0.0F, fPercentagePotential, 1.0F);

  } // end potential field

  return fPercentagePotential;
} // GetPercentagePotential(const float & elapsedPotentialTime) const


/**
 * Apply the leveling function to orient the view-up vector "up".
 */
void Submarine::ApplyLeveling()
{
  // Global "up", not to be confused with view up vector.
  // It is the preferred "up" direction (should point away from the
  // center of the earth).
  const Vector<float4> vGlobalUp(0.0F, 0.0F, 1.0F);
  // controls the tightness of leveling, or how fast the orientation changes
  const float4 levelingFactor = 0.1F;

  // add a little global up to the view up
  m_vnUp = Normal<float4>( Vector<float4>(m_vnUp) + vGlobalUp * levelingFactor );

  // project view up vector so it's perpendicular to the orientation vector
  float4 dot = m_vnOrientation.Dot(m_vnUp);
  m_vnUp = Normal<float4>(Vector<float4>(m_vnUp) - Vector<float4>(m_vnOrientation) * dot);
} // ApplyLeveling()


/**
 * Save the old position in the FIFO if it has changed much
 * since the last recorded position.
 */
void Submarine::SaveHistoryFrame()
{
  // if there is anything in the FIFO already
  if (m_replayDeque.size() > 0) 
  {
    // check to see if we moved past our distance threshold
    Vector<float4> vMovement = m_vpPositionWorld - m_replayDeque.front().GetVrp();
    const float4 fSmallDistance(0.1F);	// world coordinates (in mm)
    const int4 iMaxReplayHistorySize(10000 - 1);

    if ( (m_replayDeque.size() <= iMaxReplayHistorySize) && (vMovement.GetLength() > fSmallDistance) )
    {
      // add it to the front of the FIFO
      m_replayDeque.push_front(Camera(m_vpPositionWorld, m_vnOrientation, m_vnUp));
    }
  } 
  else 
  {
    // nothing in the FIFO yet, just add it
    m_replayDeque.push_front(Camera(m_vpPositionWorld, m_vnOrientation, m_vnUp));
  } // end if anything in the FIFO yet
} // SaveHistory()


/**
 * Access to the currently selected flight direction
 * 
 * @return the current direction
 */
Submarine::NavigationDirection Submarine::GetFlightDirection() const
{
  if (m_pDataset->m_segmentList.GetCurrentDirectionStartToEnd())
  {
    return RECTUM_TO_CECUM;
  }
  else
  {
    return CECUM_TO_RECTUM;
  }
} // GetFlightDirection()


/**
 * Set to the current flight direction
 * 
 * @param the current direction
 */
void Submarine::SetFlightDirection(const Submarine::NavigationDirection eDir)
{
  m_pDataset->m_segmentList.SetCurrentDirection(eDir == RECTUM_TO_CECUM);
} // SetFlightDirection()


/**
 * Reverse the flight direction.
 * 
 * @return the new direction
 */
void Submarine::ToggleFlightDirection()
{
  m_pDataset->m_segmentList.ReverseCurrentDirection();
} // ToggleFlightDirection()


/**
 * Come to a complete stop immediately.
 */
void Submarine::FullStop()
{
  StopFlying();
  m_pDataset->m_fields.m_paintVol.Transfer3DMarking();
  SetPotential(false);
  ExitReplayMode();

  // stop all movement by resetting the timers
  m_fSpeed = 0;
  m_accelerationTimer.Reset();
  if (m_bPotential) 
  {
    // If the potential field is activated start
    // from zero speed and ramp up.
    m_potentialTimer.SetElapsedTime(0.0F);
  } 
  else 
  {
    // If we are not using the potential field,
    // then advance the timer beyond all movement.
    m_potentialTimer.SetElapsedTime(m_fMaximumPotentialTime);
  } // potential on or off
}


/**
 * Constrain the submarine position to inside the volume.
 */
void Submarine::ConstrainPosition()
{
  // get the maximum world coordinate
  Vector<float4> vDim;
  vDim = Vector<float4>(m_pDataset->m_fields.m_volumeRender.GetDim());
  vDim.m_x -= 1.0F;
  vDim.m_y -= 1.0F;
  vDim.m_z -= 1.0F;
  const Vector<float4> vtMax(vDim.ScaleAnIso(m_pDataset->m_fields.m_volumeRender.GetUnits()));
  
  // constrain
  if (m_vpPositionWorld.m_x < 0.0F)
  {
    m_vpPositionWorld.m_x = 0.0F;
    m_uViewIdentifier++;
  } // if x out of bounds
  if (m_vpPositionWorld.m_y < 0.0F)
  {
    m_vpPositionWorld.m_y = 0.0F;
    m_uViewIdentifier++;
  } // if y out of bounds
  if (m_vpPositionWorld.m_z < 0.0F)
  {
    m_vpPositionWorld.m_z = 0.0F;
    m_uViewIdentifier++;
  } // if z out of bounds
  if (m_vpPositionWorld.m_x > vtMax.m_x)
  {
    m_vpPositionWorld.m_x = vtMax.m_x;
    m_uViewIdentifier++;
  } // if x out of bounds
  if (m_vpPositionWorld.m_y > vtMax.m_y)
  {
    m_vpPositionWorld.m_y = vtMax.m_y;
    m_uViewIdentifier++;
  } // if y out of bounds
  if (m_vpPositionWorld.m_z > vtMax.m_z)
  {
    m_vpPositionWorld.m_z = vtMax.m_z;
    m_uViewIdentifier++;
  } // if z out of bounds
  m_vpPositionVolume = m_pDataset->m_fields.m_volumeRender.GetWorldToVolumeMatrix() * m_vpPositionWorld;
} // ConstrainPosition()


/**
 * Get the skeleton index nearest to the current view point.
 *
 * @return uint2  The index of the nearest skeleton to the view point.
 */
uint2 Submarine::GetClosestSkeletonIndex() const
{
  return GetClosestSkeletonIndex(GetPositionVolume());
} // GetClosestSkeletonIndex() const


/**
 * Get the skeleton index nearest to the given view point.
 *
 * @param posVolumeUSpoint   Volume location to which to find nearest skeleton.
 * @return uint2             The index of the nearest skeleton to the view point.
 */
uint2 Submarine::GetClosestSkeletonIndex( Point<uint2> posVolumeUSpoint ) const
{
  Skeleton & skeleton = m_pDataset->m_segmentList.GetCurrentSkeleton();
  uint2 uNumOfElements = m_pDataset->m_segmentList.GetCurrentSkeleton().GetNumOfElements();

  int4 iCurrIndex = -1;
  float4 fCurrDiff = 512.0 * 512.0;
  for (int4 index(0); index<uNumOfElements; index++)
  {
    SkeletonNode skelNode = skeleton.GetSkelNode(index);
    Point<uint2> skelNodeVolPos = skelNode.m_vVolumePos;
    float4 fDiff = (float4)(skelNodeVolPos.m_x - posVolumeUSpoint.m_x) * (float4)(skelNodeVolPos.m_x - posVolumeUSpoint.m_x)
                 + (float4)(skelNodeVolPos.m_y - posVolumeUSpoint.m_y) * (float4)(skelNodeVolPos.m_y - posVolumeUSpoint.m_y)
                 + (float4)(skelNodeVolPos.m_z - posVolumeUSpoint.m_z) * (float4)(skelNodeVolPos.m_z - posVolumeUSpoint.m_z);
    
    // if exactly at a skeleton node...
    if (fDiff < 1e-4)
    { 
      return index; 
    }

    if (fDiff < fCurrDiff) 
    {
      fCurrDiff = fDiff;  
      iCurrIndex = index;
    }
  } // end-for

  if (iCurrIndex<uNumOfElements) 
  {
    return iCurrIndex;
  }
  else
  {
    return uNumOfElements/2;  // no proper index found, return the mid-point of the skeleton.
  }
} // GetCloesestSkeletonIndex()


/**
 * Switch to manual mode
 */
void Submarine::SwitchToManualMode()
{
  SetPotential( false );

  m_vpLastCenterlinePositionWorld = m_vpPositionWorld;
  m_vnLastCenterlineOrientation = m_vnOrientation;
  m_vnLastCenterlineUp = m_vnUp;
} // SwitchToManualMode()


/**
 * Switch to automatic mode
 */
void Submarine::SwitchToAutomaticMode()
{
  ExitReplayMode();

  if( m_eTransitionMode == Submarine::TRANSITION_CLOSEST )
  {
    const Normal3D<float4> vnOrientationSave = m_vnOrientation;
    const Normal3D<float4> vnUpSave = m_vnUp;
    SetViewSkeleton( GetClosestSkeletonIndex() );
    m_vnOrientation = vnOrientationSave;
    m_vnUp = vnUpSave;
    m_uViewIdentifier++;
    SetPotential( true );
  }
  else if( m_eTransitionMode == Submarine::TRANSITION_SMOOTH )
  {
    PopulateTransitionVector();
  }
  else if( m_eTransitionMode == Submarine::TRANSITION_OTHER )
  {
    SetPotential( true );
  }

} // SwitchToAutomaticMode()


/**
 * Populates the transition vector for the manual to automatic switch. 
 */
void Submarine::PopulateTransitionVector()
{
 
  // grab the closest skeleton index, which is where we want to end up
  const Skeleton & skeleton = m_pDataset->m_segmentList.GetCurrentSkeleton();
  const uint2 uSkeletonIndex = Bound( static_cast< uint2 >( 0 ),  GetClosestSkeletonIndex(), static_cast< uint2 >(skeleton.GetNumOfElements()-1 ) );

  // start and end position
  const Point3Df positionStartWorld = m_vpPositionWorld;
  const Point3Df positionEndWorld = skeleton.GetSkelNode( uSkeletonIndex ).m_vWorldPos;

  // calculate the distance from the start to the end position
  const float4 fTransitionDistance = fabs( positionStartWorld.GetEuclideanDistance( positionEndWorld ) );

  // check if we are within a certain delta, in that case just do the normal flying
  const float4 fDistanceDelta = 1.0f;
  if( fTransitionDistance < fDistanceDelta )
  {
    // just enable the potential fields and return
    SetPotential( true );
    return;
  }

  
  // start and end orientation ( view direction )
  const Normal3Df vnStartOrientation = m_vnOrientation;
  const Normal3Df vnEndOrientation = GetFlightDirection() == RECTUM_TO_CECUM ? skeleton.GetSkelNode( uSkeletonIndex ).m_vWorldTan : -skeleton.GetSkelNode( uSkeletonIndex ).m_vWorldTan;

  // start and end up direction
  const Normal3Df vnStartUp = m_vnUp;
  const Normal3Df vnEndUp = skeleton.GetSkelNode( uSkeletonIndex ).m_vWorldUp;

  // compute the angle bewteen the start and end orientations
  const float4 fAngleDegrees = (  fabs( acos( vnStartOrientation.Dot( vnEndOrientation ) ) ) * 180.0F / M_PI );

  // detemermine the required amount of angle adjustments to ensure a smooth transition
  const float4 fAngleAdjustmentDegrees = 5.0F;
  const uint2 uReqiredAngleSteps = static_cast< uint2 >( fAngleDegrees / fAngleAdjustmentDegrees );

  
  // detemermine the required amount of distance adjustments to ensure a smooth transition
  const float4 fTransitionStepSizeWorld = 0.50F;
  const uint2 uRequiredDistanceSteps = static_cast< uint2 >( fTransitionDistance / fTransitionStepSizeWorld ) ;

  // calculate the number of steps for the transition
  const uint2 uNumberOfTransitionSteps = max( uRequiredDistanceSteps, max( 1, uReqiredAngleSteps ) );  

  // populate the transition vector, interpolating the position, orientation vector, up vector from start to end
  m_transitions.clear();
  for( uint2 uStep = 0; uStep < uNumberOfTransitionSteps; ++uStep )
  {
    const float4 fEndWeight = static_cast< float4 >( uStep + 1 ) / uNumberOfTransitionSteps;
    const float4 fStartWeight = 1.0F - fEndWeight;
    
    TransitionInfo info;
    info.m_vpPositionWorld = Point3Df ( ( static_cast< Vector3Df >( positionStartWorld ) * fStartWeight ) 
                                                      + ( static_cast< Vector3Df >( positionEndWorld ) * fEndWeight ) );
    
    info.m_vnOrientation   = Normal3Df( ( static_cast< Vector3Df >( vnStartOrientation ) * fStartWeight ) 
                                                      + ( static_cast< Vector3Df >( vnEndOrientation ) * fEndWeight ) );
    
    info.m_vnUp            = Normal3Df( ( static_cast< Vector3Df >( vnStartUp ) * fStartWeight ) 
                                                      + ( static_cast< Vector3Df >( vnEndUp ) * fEndWeight ) );
    
    m_transitions.push_back( info );
  } //   for( uint2 uStep = 0; uStep < uNumberOfTransitionSteps; ++uStep )

  // reset transition variables and indicate we are transitioning
  m_uTransitionIndex = 0;
  m_bTransitioning = true;
} // PopulateTransitionVector()



// undefines
#undef FILE_REVISION


// $Log: Submarine.C,v $
// Revision 1.12.2.13  2010/12/21 15:40:48  dongqing
// comments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12.2.12  2010/12/20 18:53:26  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.12.2.11  2009/11/10 19:03:48  dongqing
// Improving fly speed for non-cleansed images by using ULD volume
//
// Revision 1.12.2.10  2009/01/15 04:11:43  jmeade
// convenience function to set camera coordinates and orientation.
//
// Revision 1.12.2.9  2008/10/23 17:13:22  jmeade
// IsLoadingComplete() check.
//
// Revision 1.12.2.8  2008/10/22 02:19:01  jmeade
// improve threshold check for 3D movement to trigger synchronization to 2D slice positions.
//
// Revision 1.12.2.7  2007/08/20 19:13:05  geconomos
// defaulting to original transition mode from manual to automatic
//
// Revision 1.12.2.6  2007/08/17 16:09:40  geconomos
// Still reworking the transition
//
// Revision 1.12.2.5  2007/08/16 17:46:05  geconomos
// Took another stab at the transition from manual to automatic mode
//
// Revision 1.12.2.4  2007/07/31 21:22:32  jmeade
// Issue 5770: just jump to the centerline position when starting autofly, don't change up or orientation vector.
//
// Revision 1.12.2.3  2007/05/23 16:13:53  geconomos
// reworked transitioning from manual to automatic mode of flight
//
// Revision 1.12.2.2  2007/03/22 20:41:51  dongqing
// change the m_uXsectionScrollBarLocation to m_iXsectionScrollBarLocation
// The type is changed from uint4 to int4
//
// Revision 1.12.2.1  2007/03/16 16:58:49  dongqing
// finally finish the enhancement of the cross-sectional view going
// thru entire colon
//
// Revision 1.12  2007/03/14 21:52:58  dongqing
// make the cross-section oblique the same viewer mechanism as that of
// orthogonal views
//
// Revision 1.11  2007/03/05 23:17:53  dongqing
// add new m_iXsectionScrollBarLocationIndex
//
// Revision 1.10  2006/06/01 20:28:29  frank
// updated to visual studio 2005
// bitten by suspected OpenGL driver bug
//
// Revision 1.9  2006/05/03 17:55:31  frank
// cleaned up the logic in the smooth transition code
//
// Revision 1.8  2006/05/02 13:32:38  frank
// added the capability to remove the smooth transition between automatic and manual flight modes
//
// Revision 1.7  2006/02/07 21:32:44  gdavidson
// Code Review
//
// Revision 1.6  2006/01/27 20:31:38  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.4.2.2  2006/01/11 16:44:53  frank
// set const functions constant
//
// Revision 1.4.2.1  2005/11/15 13:21:37  frank
// additional error handling
//
// Revision 1.4  2005/10/27 15:17:41  vxconfig
// Issue #4481: Fixed looking around with arrow keys after rewinding.
//
// Revision 1.3  2005/09/29 14:11:46  vxconfig
// switched csi volume to new vx block volume
//
// Revision 1.2  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.26.2.8.2.2  2005/03/23 22:16:55  frank
// Issue #3860: Fixed hesitation when starting 3D reverse flight
//
// Revision 3.26.2.8.2.1  2005/03/22 15:26:39  frank
// Issue #3860: Fixed hesitation when starting 3D reverse flight
//
// Revision 3.26.2.8  2005/02/15 18:42:39  jmeade
// merge from vc1.4 branch.
//
// Revision 3.26.2.7.2.2  2005/02/04 23:57:09  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.26.2.7.2.1  2004/10/05 16:02:20  jmeade
// remove debugging code.
//
// Revision 3.26.2.7  2004/09/09 21:43:02  jmeade
// Issue 3538: Perceived speed adjustments: use average of local radii to temper sharp changes in speed.
//
// Revision 3.26.2.6  2004/08/31 22:49:57  jmeade
// Incremental check-in.
//
// Revision 3.26.2.5  2004/08/27 22:01:05  jmeade
// Issue 3538: Adjust perceived speed according to colon inner-radius.
//
// Revision 3.26.2.4  2004/07/17 03:06:28  jmeade
// Separate method for determining closest skeleton to any given volume coordinates.
//
// Revision 3.26.2.3  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.26.2.2  2003/05/22 19:26:24  kevin
// Fixed a problem I found where the gradient of the SLD volume was being
// normalized and I t could have been out in an area where the values were
// all 0, thus you got a (0,0,0) vector being normalized.
//
// Revision 3.26.2.1  2003/02/27 02:20:44  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.26  2003/01/27 21:47:15  jmeade
// Method to set current flight direction.
//
// Revision 3.25  2003/01/22 22:28:47  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.24  2002/11/15 16:46:40  kevin
// SetVIewSkeleton also updates the UpVector (for smooth 2D X-sect flights)
//
// Revision 3.23  2002/10/29 19:37:49  kevin
// Normal read/write is not consistent. SO rather than risk breakign
// something else, I put a LARGE warnign in Normal read and
// fixed the Camera class I/O outside of the Normal elements.
//
// Revision 3.22  2002/09/27 19:02:13  jmeade
// Coding standards.
//
// Revision 3.21  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.20  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.19  2002/04/12 02:15:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.18  2002/03/28 18:47:50  jmeade
// Allow run-time setting of maximum flight speeds.
//
// Revision 3.17  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.16.2.2  2002/04/08 23:11:38  jmeade
// Hack to get auto- and manual-flight velocities equal; Coding standards.
//
// Revision 3.16.2.1  2002/03/28 22:17:46  jmeade
// Allow run-time setting of maximum flight speeds.
//
// Revision 3.16  2001/12/21 23:58:41  jmeade
// Moved movie recording (for use in other views).
//
// Revision 3.15  2001/12/19 20:51:08  jmeade
// Need quotes around filenames with system commands.
//
// Revision 3.14  2001/12/14 21:52:48  jmeade
// Error checking and reporting to user on movie creation; Removed movie CD burn; Comments.
//
// Revision 3.13  2001/12/12 21:13:37  jmeade
// Added (empty) case for line measure mode during movie record.
//
// Revision 3.12  2001/12/12 20:06:59  jmeade
// Movies now save frames via current modes (translucent and flight currently);
// Use of Camera class for movies and history.
//
// Revision 3.11  2001/12/07 18:46:19  kevin
// Added "entertainment" bitmap and CD creation to movies
//
// Revision 3.10  2001/11/21 17:41:56  binli
// more accurate fucntion GetClosestSkeletonIndex().
//
// Revision 3.9  2001/11/21 17:26:05  jmeade
// Toggle movie record, is movie recording access method.
//
// Revision 3.8  2001/11/19 13:17:52  kevin
// Fixed some of the problems with S/P orientation when segments are flipped
//
// Revision 3.7  2001/11/16 22:30:40  binli
// new speed control to 2D flight
//
// Revision 3.6  2001/11/14 02:29:18  kevin
// More CLenaup of SUpine/Prone and added in orientation
// according to  submarine "lookahaed" algrotihm
//
// Revision 3.5  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.4  2001/10/29 14:13:42  kevin
// Added Jpeg Picture saving and tidied up avi/mpeg generation
//
// Revision 3.3  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.2  2001/10/25 20:31:40  kevin
// Cleaned up and got avi/mpeg to work
//
// Revision 3.1  2001/10/25 13:20:39  kevin
// Added avi/mpeg saving
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:10   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:40:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:20   ingmar
// Initial revision.
// Revision 1.81  2001/06/21 21:15:19  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.80  2001/05/29 11:26:56  frank
// Replaced missing log message.
//
// Revision 1.79  2001/05/17 16:03:45  binli
// bug 000462: need validation.
//
// Revision 1.78  2001/05/16 18:34:37  frank
// Found the jittering problem, one more step toward fix.
//
// Revision 1.77  2001/05/16 17:09:02  frank
// Slowly rolling forward...
//
// Revision 1.76  2001/05/16 16:32:18  frank
// Virtual rollback to fix jittering bug.
//
// Revision 1.75  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.74  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.73  2001/05/03 12:04:55  frank
// Ensured that the view identifier was adjusted when the view changed.
//
// Revision 1.72  2001/05/02 20:12:14  ingmar
// BookmarkList.C
//
// Revision 1.71  2001/05/02 18:33:53  binli
// bug fixed: correct render planes when picked up a bookmark in S&PView/VerifyView.
//
// Revision 1.70  2001/05/01 20:43:52  binli
// bug fixed: correct render planes when picked up a bookmark in S&PView/VerifyView.
//
// Revision 1.69  2001/04/30 12:31:53  frank
// Reflected changes in Study class.
//
// Revision 1.68.2.1  2001/05/01 23:52:45  jmeade
// m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.68  2001/04/19 11:56:56  frank
// Reorganized according to coding walkthrough.
//
// Revision 1.67  2001/04/18 15:45:25  frank
// Updated coding standards.
//
// Revision 1.66  2001/04/13 11:53:39  frank
// Protected some members of Fields.
//
// Revision 1.65  2001/04/11 17:09:59  frank
// Reflected changes in Fields.
//
// Revision 1.64  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.63  2001/04/11 13:27:32  frank
// Moved volume data to Fields class.
//
// Revision 1.62  2001/04/10 17:52:21  frank
// New coding conventions in Submarine class.
//
// Revision 1.61  2001/04/10 12:49:55  frank
// Removed revision history from top of file.
//
// Revision 1.60  2001/04/06 13:53:23  frank
// Reflected changes in SegmentList class.
//
// Revision 1.59  2001/03/28 13:58:45  frank
// Reflected changes in SegmentList class.
//
// Revision 1.58  2001/03/26 16:04:11  frank
// Reflected changes in Segment class.
//
// Revision 1.57  2001/03/16 17:11:09  frank
// Updated Fields class variables for new coding standards.
//
// Revision 1.56  2001/03/16 16:24:33  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.55  2001/03/09 01:06:44  jmeade
// protect access to some more Study members
//
// Revision 1.54  2001/02/22 01:35:19  jeff
// code without coding conventions is gettting harder and harder to comprehend
//
// Revision 1.53  2001/02/15 13:46:02  frank
// Constrained the viewpoint inside the volume to avoid crashes inside volume rendering code.
//
// Revision 1.52  2001/02/05 13:29:56  frank
// Made a single global null study.
//
// Revision 1.51  2001/02/02 08:01:04  jeff
// I know this is weird, but hear me out:  NULL pointers are crash prone, so maybe
// a defined NULL per type (as we mulled over before) might be better?
//
// Revision 1.50  2001/02/01 13:41:07  frank
// Revamped study initialization procedure.
//
// Revision 1.49  2001/02/01 12:55:32  frank
// Initialized parent pointer another way.
//
// Revision 1.48  2001/01/31 14:08:38  frank
// Added a parent pointer to retain the proper (supine/prone) study.
//
// Revision 1.47  2001/01/30 16:22:33  jeff
// no message
//
// Revision 1.46  2001/01/30 02:07:38  jeff
// NO ABORTS!!!
//
// Revision 1.45  2001/01/29 21:01:02  frank
// Fixed supine/prone problem by refreshing cached matrices.
//
// Revision 1.44  2001/01/27 02:57:12  kevin
// Hooked up the display paintinfo to the overview.
// (note there are still some bugs with the grab image,
// but I can't seemm to get it to connect from readerlib
// into viatron. ARGGHH I am a coding dinosaur)
//
// Revision 1.43  2001/01/27 02:02:01  kevin
// 1) Fixed 90% missed Patch error
// 2) Moved GetPotenetil to fields from Submarine
// as part of this fix.
//
// Revision 1.42  2001/01/24 17:47:09  frank
// Avoided null memory references.
//
// Revision 1.41  2001/01/24 15:55:20  jeff
// Aren't all bools supposed to start m_b...?
//
// Revision 1.40  2001/01/23 18:16:51  frank
// Saved a unique view identifier for view caching purposes.
//
// Revision 1.39  2001/01/23 15:48:52  frank
// Stopped at the start or the end of the skeleton.
// Implemented FullStop function.
//
// Revision 1.38  2001/01/11 21:10:44  frank
// Changed the swiftness of orientation.
//
// Revision 1.37  2001/01/05 15:35:23  frank
// Used SegmentList to contain navigation direction.
//
// Revision 1.36  2001/01/03 13:34:28  frank
// Avoided weaving during startup and slowdown.
//
// Revision 1.35  2000/12/22 19:31:08  frank
// Changed the style of panning back to relative mode from absolute.
//
// Revision 1.34  2000/11/24 17:09:48  frank
// Reduced look-ahead
//
// Revision 1.33  2000/11/24 15:29:57  frank
// Added ExitReplayMode function
//
// Revision 1.32  2000/11/24 14:34:37  frank
// Fixed "rewind before coming to a stop" bug
//
// Revision 1.31  2000/11/22 14:19:38  frank
// Relaxed boundary constraint so that you can fit through narrow openings without being bounced around
//
// Revision 1.30  2000/11/20 15:56:02  frank
// Fixed accumulation buffering during middle mousing
//
// Revision 1.29  2000/11/20 14:36:55  frank
// Cleaned up motion blur
//
// Revision 1.28  2000/11/17 18:27:47  frank
// Added Reset functions
//
// Revision 1.27  2000/11/17 16:12:26  frank
// Filtered out upper sld values
//
// Revision 1.26  2000/11/16 19:26:59  frank
// Patch for sld/ins marking issue
//
// Revision 1.25  2000/11/16 18:12:49  kevin
// Ingmar's change to get VolIterator to compile again
//
// Revision 1.24  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.23  2000/11/14 17:32:26  frank
// Flushed accumulation buffer in several instances
//
// Revision 1.22  2000/11/10 19:36:44  frank
// Reduced radius of neighbor searching
//
// Revision 1.21  2000/11/10 19:02:05  frank
// Avoided local minima in gradient descent
//
// Revision 1.20  2000/11/09 20:11:08  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.19  2000/11/09 14:51:51  frank
// Fixed centerline coloring
//
// Revision 1.18  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.17  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.16  2000/11/03 20:32:12  dave
// converted back to unsigned dim.
//
// Revision 1.15  2000/11/03 19:35:13  frank
// Used spiral iterator
//
// Revision 1.14  2000/11/02 20:09:12  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.13  2000/10/31 14:37:40  frank
// Merged in new segment structure
//
// Revision 1.12.2.1  2000/10/26 13:28:33  frank
// Added new segment structure
//
// Revision 1.12  2000/10/12 21:06:20  kevin
// Added direcitonal
// flag to semgnet jumping
//
// Revision 1.11  2000/10/12 02:48:25  kevin
// Added proper flight direction storage. Alsi implemented
// proper directional checking that considers flight direction
// as well as current segment direction
//
// Revision 1.10  2000/10/11 15:00:06  kevin
// Changes for rdrGloabl current seg/skel pointers
//
// Revision 1.9  2000/10/03 00:56:23  kevin
// Added update of orthogonal point when jumping to skeleton index
//
// Revision 1.8  2000/09/28 20:46:10  frank
// Replaced DFB with SLD
//
// Revision 1.7  2000/09/21 19:37:21  frank
// C++ Coding Standards
//
// Revision 1.6  2000/09/19 18:16:51  kevin
// Reversed one version due to errors that could
// not be fixed due to broken pipeline/volume utlity stuff.
// (This will be revisited once the other errors
// are fixed and we can work at all once again)
//
// Revision 1.4  2000/09/12 20:42:51  kevin
// Switched back to Block volumes (including .ins)
//
// Revision 1.3  2000/09/12 00:02:32  kevin
// CHanged back to Hash due to "grey screen" bug
// even though block uses significantly less memory
//
// Revision 1.2  2000/09/11 18:06:55  frank
// Changed from Hash volumes to Block volumes
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 55    9/07/00 5:02p Frank
// Made end-to-end navigation robust by searching neighbors
// instead of using central difference gradient.
// 
// 54    9/07/00 4:12p Frank
// Added a threshold to replay mode to avoid inadvertent entry
// 
// 53    9/06/00 3:09p Frank
// Added pan functionality
// 
// 52    9/06/00 10:13a Frank
// Added comments to bring it up to coding standards
// 
// 51    9/05/00 1:34a Ingmar
// now works with new naming
// 
// 50    9/01/00 9:18a Frank
// Cosmetics
// 
// 49    8/31/00 2:13p Frank
// Made navigation snappier
// 
// 48    8/28/00 12:04p Antonio
// updated header
//
//*****************  Version 47  *****************
//User: Kevin           Date:  8/23/00  Time: 11:41p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 46  *****************
//User: Kevin           Date:  8/23/00  Time:  1:41a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Really, Really fixed otrthogonalPoint to work with viewpoint
//color displaying whether point matches the viewpoint
//
//*****************  Version 45  *****************
//User: Kevin           Date:  8/23/00  Time: 12:07a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed the scroller
//
//*****************  Version 44  *****************
//User: Kevin           Date:  8/22/00  Time: 11:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed slice positioning to utilize "orthogonalPoint"
//
//*****************  Version 43  *****************
//User: Jeff            Date:  8/22/00  Time:  7:29p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Value access functions for current throttle and potential strength
//"const TYPE &" in function headers replaces "const TYPE"
//
//*****************  Version 42  *****************
//User: Kevin           Date:  8/20/00  Time: 10:20p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added choosing of flight direction
//
//*****************  Version 41  *****************
//User: Frank           Date:  8/18/00  Time:  1:33p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added possible neighbor searching to navigation
//
//*****************  Version 40  *****************
//User: Frank           Date:  8/17/00  Time: 11:54a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added navigation test mode
//
//*****************  Version 39  *****************
//User: Jeff            Date:  8/11/00  Time: 12:47p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Change in parameter list for GetResampledValue()
//
//*****************  Version 38  *****************
//User: Frank           Date:  8/10/00  Time:  4:21p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-worked the internals of the physics
//to better handle wall constaints.
//
//*****************  Version 37  *****************
//User: Jeff            Date:  8/09/00  Time:  4:10p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  
//
//*****************  Version 36  *****************
//User: Frank           Date:  8/08/00  Time:  9:38a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Renamed a function for clarity
//
//*****************  Version 35  *****************
//User: Frank           Date:  8/07/00  Time:  3:52p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Constrained the boundary forces
//
//*****************  Version 34  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 33  *****************
//User: Kevin           Date:  8/04/00  Time:  4:45p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed ReverseOrientation
//Added inside file testing
//
//*****************  Version 32  *****************
//User: Kevin           Date:  8/03/00  Time:  4:19p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed legacy padding
//fixed volume copy assignment bug
//
//*****************  Version 31  *****************
//User: Kevin           Date:  8/02/00  Time:  9:30a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Fixed submarine jumping probelm
//
//*****************  Version 30  *****************
//User: Kevin           Date:  8/02/00  Time:  1:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Aligned centerline with polygons and cleaned up overview a little
//
//*****************  Version 29  *****************
//User: Frank           Date:  8/01/00  Time:  4:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added bidirectional potential fields
//
//*****************  Version 28  *****************
//User: Kevin           Date:  8/01/00  Time: 10:23a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Code Cleanup after changeing to new file format
//
//*****************  Version 27  *****************
//User: Frank           Date:  8/01/00  Time:  9:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Increased the orientation strength
//
//*****************  Version 26  *****************
//User: Kevin           Date:  8/01/00  Time:  9:13a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed to use the new pipeline file formats
//
//*****************  Version 25  *****************
//User: Frank           Date:  7/31/00  Time:  3:59p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Explicitly enforced the orthogonality constraint every update
//
//*****************  Version 24  *****************
//User: Kevin           Date:  7/31/00  Time:  1:22p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  fied changes in GetSampleValue
//
//*****************  Version 23  *****************
//User: Frank           Date:  7/27/00  Time: 11:39a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Set default boundary to true
//
//*****************  Version 22  *****************
//User: Frank           Date:  7/27/00  Time: 11:22a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed the lookahead time to 4 seconds
//
//*****************  Version 21  *****************
//User: Frank           Date:  7/26/00  Time:  4:58p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added a way to get out of potential field
//
//*****************  Version 20  *****************
//User: Frank           Date:  7/26/00  Time:  4:23p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added DFS and CSI files and support
//
//*****************  Version 19  *****************
//User: Frank           Date:  7/26/00  Time: 10:15a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Cleaned out extra functions
//
//*****************  Version 18  *****************
//User: Frank           Date:  7/26/00  Time:  8:19a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Used new functions
//
//*****************  Version 17  *****************
//User: Frank           Date:  7/25/00  Time:  2:30p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Enabled look-around in replay mode
//
//*****************  Version 16  *****************
//User: Frank           Date:  7/25/00  Time:  1:49p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Extended replay mode to allow orientation
//
//*****************  Version 15  *****************
//User: Binli           Date:  7/25/00  Time: 11:20a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  updated to reflect new data structure
//
//*****************  Version 14  *****************
//User: Frank           Date:  7/25/00  Time: 10:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added validity testing for position
//
//*****************  Version 13  *****************
//User: Frank           Date:  7/25/00  Time: 10:41a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Used the skeleton to derive view direction
//
//*****************  Version 12  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 11  *****************
//User: Frank           Date:  7/20/00  Time:  1:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added PDS field using new Volume class
//
//*****************  Version 10  *****************
//User: Frank           Date:  7/18/00  Time:  3:56p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Removed "using namespace *" from header files to prevent ambiguities
//
//*****************  Version 9  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 9
//Comment:
//  
//
//*****************  Version 8  *****************
//User: Jeff            Date:  7/18/00  Time:  1:39p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/18/00  Time:  1:23p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Implemented instant replay
//
//*****************  Version 6  *****************
//User: Frank           Date:  7/18/00  Time: 10:44a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  virtual rollback
//
//*****************  Version 5  *****************
//User: Frank           Date:  7/18/00  Time: 10:32a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  testing time zone
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/17/00  Time:  9:57p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  testing history again
//
//*****************  Version 3  *****************
//User: Frank           Date:  7/17/00  Time:  9:54p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Testing history
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/13/00  Time: 10:20p
//Checked in $/NewViatron1000/ReaderLib
//Comment:
//  Improved navigation feel - exponential instead of linear
//acceleration
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Submarine.cpp
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Submarine.C,v 1.12.2.13 2010/12/21 15:40:48 dongqing Exp $
// $Id: Submarine.C,v 1.12.2.13 2010/12/21 15:40:48 dongqing Exp $
