// $Id: vxSubmarine.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include <math.h>
#include "Global.h"
#include "vxSubmarine.h"
#include "Matrix.h"
#include "Timer.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// static member initializations
const float vxSubmarine::m_fMaximumPotentialTime = 0.75F; // seconds
float4 vxSubmarine::m_fIdealFrameRate(90.0F);
float4 vxSubmarine::m_fTurnRateScale(0.0015F);
float4 vxSubmarine::m_fThrustScale(0.015F);


/**
 * Constructor.
 * Responsible for setting up all default values.
 */
vxSubmarine::vxSubmarine()
{
  Reset();
} // constructor


/**
 * Copy constructor
 * @param submarine to copy from
 */
vxSubmarine::vxSubmarine(const vxSubmarine & rhs)
{
  // Just call the assignment operator
  *this = rhs;
} // copy constructor


/**
 * Assignment operator
 * @param submarine to copy from
 * @return const reference to this
 */
vxSubmarine & vxSubmarine::operator =(const vxSubmarine & rhs)
{
  // test for self assignment
  if (&rhs == this)
  {
    return *this;
  }

  m_worldPosition = rhs.m_worldPosition;
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
  m_panVector = rhs.m_panVector;
  m_bPan = rhs.m_bPan;
  m_bLeveling = rhs.m_bLeveling;
  m_bPotential = rhs.m_bPotential;
  m_bBoundary = rhs.m_bBoundary;
  m_bTestStats = rhs.m_bTestStats;
  // we want the following members to be re-initialized:
  // m_accelerationTimer, m_frameTimer, m_uViewIdentifier,
  return * this;
} // operator=()


/**
 * Reset all data members to default values.
 */
void vxSubmarine::Reset()
{
  // Select the initial point at the origin
  m_worldPosition  = Point<float4>(0.0F, 0.0F, 0.0F);

  // Select the initial orientation as the first derivative of
  // the skeleton using a forward difference
  m_vnUp = Normal<float4>(0.0F, 1.0F, 0.0F);
  m_vnOrientation = Normal<float4>(0.0F ,0.0F ,-1.0F);

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
  m_panVector = Vector<float4>(0.0F, 0.0F, 0.0F);
  m_bPan = false;

  // Default to off
  m_bLeveling = false;
  m_bPotential = false;
  m_bBoundary = true;

  m_bTestStats = false;
} // Reset()


/**
 * Set current view position
 * @param new position in world coordinates
 */
void vxSubmarine::SetPositionWorld(const Point<float4> & worldPosition)
{
  m_worldPosition = worldPosition;
} // SetPositionWorld()


/**
 * Begin to add forces to the simulation
 */
void vxSubmarine::StartFlying()
{
  m_accelerationTimer.Reset();
  m_frameTimer.Reset();
} // StartFlying()


/**
 * Remove all forces from the model and allow to coast.
 */
void vxSubmarine::StopFlying()
{
  SetForces(0.0F, 0.0F, 0.0F);
  m_accelerationTimer.Reset();
} // StopFlying()


/**
 * The update function should be called at regular intervals.
 * It moves the vxSubmarine along its path and rotates it accordingly.
 */
void vxSubmarine::UpdatePosition(Vector3D<float4> panVector)
{
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

  const float4 fMaximumDistance(2.5F);  // distance in mm
  // the maximum amount of movement in world space (mm)
  const float4 fOffsetScale(0.1F);

  for (uint4 i(0); i<uUpdates; i++)
  {
    // update the heading turn rate
    m_fHeadingTurnRate += m_fThrottle * m_fHeadingInput * m_fTurnRateScale;

    // update the pitch turn rate
    m_fPitchTurnRate += m_fThrottle * m_fPitchInput * m_fTurnRateScale;
    {
      //// first pan the viewpoint
      // compute offset from original pan location
      Vector<float4> vOffset(panVector.X(), panVector.Y(), 0);

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
      m_worldPosition -= vOffsetWorld;
    }
    {
      //// now rotatte the view
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

    // Test to make sure we have not gone too far in this step
    if (vVelocity.GetLength() > fMaximumDistance)
    {
      vVelocity.Normalize();
      vVelocity *= fMaximumDistance;
    } // moved too far

    // Update position
    m_worldPosition += vVelocity;

  } // performing updates

  // Re-enforce the orthogonality constraint for good measure
  EnforceOrthogonality();

} // Update()


/**
 * Update the view to a position and direction.
 * 
 * const Point<float4> & worldPosition    world position
 * const Normal<float4> & vnViewDirection   view direction
 * const Normal<float4> & vnUp              view up orientation
 */
void vxSubmarine::SetView(const Point<float4> & worldPosition, 
                          const Normal<float4> & vnViewDirection, 
                          const Normal<float4> & vnUp)
{
  SetPositionWorld(worldPosition);
  SetOrientation(vnViewDirection);
  SetUpDirection(vnUp);
  EnforceOrthogonality();
} // SetView()


/**
 * Enforce the orthogonality constraint (between the forward and up directions)
 *
 */
void vxSubmarine::EnforceOrthogonality()
{
  // Project the view up vector so that it's perpendicular to the 
  // orientation vector. The dot product between them should be zero.
  float4 dotProduct(m_vnOrientation.Dot(m_vnUp));

  // This should remove any skew in the up vector so that
  // they are orthogonal.
  m_vnUp = Normal<float4>(Vector<float4>(m_vnUp) - Vector<float4>(m_vnOrientation) * dotProduct);
} // EnforceOrthogonality()


/**
 * Apply friction, leveling, boundary, and potential fields
 * @return the change in position after applying fields
 */
const Vector<float4> vxSubmarine::GetFieldForce()
{
  // Friction slows it down; specify the amount of friction
  const float4 fPositionFriction(0.92F);
  const float4 fOrientationFriction(0.92F);

  m_fSpeed *= fPositionFriction;
  m_fHeadingTurnRate *= fOrientationFriction;
  m_fPitchTurnRate *= fOrientationFriction;

  // Optionally perform leveling roll
  if (m_bLeveling == true)
  {
    ApplyLeveling();
  }

  return Vector<float4>(0,0,0);
} // GetFieldForce()


/**
 * Return whether or not the vxSubmarine is currently flying
 * @return true if the vxSubmarine is flying
 */
bool vxSubmarine::IsFlying()
{
  const float4 fMinimumSpeed(0.001F);
  const float4 fMinimumTurnRate(0.001F);

  return ( ( fabs( m_fSpeed ) > fMinimumSpeed              ||
             fabs( m_fHeadingTurnRate ) > fMinimumTurnRate ||
             fabs( m_fPitchTurnRate ) > fMinimumTurnRate ) );

} // IsFlying()


/**
 * Apply the leveling function to orient the view-up vector "up".
 */
void vxSubmarine::ApplyLeveling()
{

  // Global "up", not to be confused with view up vector.
  // It is the preferred "up" direction (should point away from the
  // center of the earth).
  const Vector<float4> vGlobalUp(0.0F, 0.0F, 1.0F);
  // controls the tightness of leveling, or how fast the orientation changes
  const float4 levelingFactor(0.1F);

  // add a little global up to the view up
  m_vnUp = Normal<float4>(Vector<float4>(m_vnUp) + vGlobalUp * levelingFactor);

  // project view up vector so it's perpendicular to the orientation vector
  float4 fDot(m_vnOrientation.Dot(m_vnUp));
  m_vnUp = Normal<float4>(Vector<float4>(m_vnUp) - Vector<float4>(m_vnOrientation) * fDot);

} // ApplyLeveling()


/**
 * Come to a complete stop immediately
 */
void vxSubmarine::FullStop()
{
  StopFlying();

  // stop all movement by resetting the timers
  m_fSpeed = 0;
  m_accelerationTimer.Reset();

} // FullStop()


#undef FILE_REVISION


// $Log: vxSubmarine.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/07/15 17:08:22  frank
// Removed dead code
//
// Revision 1.7  2003/04/24 14:19:55  frank
// code review
//
// Revision 1.6  2003/04/18 19:56:30  michael
// initial code review
//
// Revision 1.5  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.4  2003/01/22 21:46:41  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.3  2002/11/12 16:31:49  ingmar
// added combination of pan and rotate to facilitate auto flight without centerline
//
// Revision 1.2  2002/11/10 23:04:06  michael
// cosmetics
//
// Revision 1.1  2002/10/21 23:19:30  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxSubmarine.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxSubmarine.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
