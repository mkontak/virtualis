// $Id: vxSubmarine.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * Get current view direction.
 */
const Normal<float4> & vxSubmarine::GetOrientation() const
{
  return m_vnOrientation;
} // GetOrientation()


/**
 * Set current view direction
 * 
 * @param orientation       new view direction in world coordinates
 */
void vxSubmarine::SetOrientation(const Normal<float4> & vnOrientation)
{
  m_vnOrientation = vnOrientation;
} // SetOrientation()


/**
 * Get current view up direction.
 */
const Normal<float4> & vxSubmarine::GetUpDirection() const
{
  return m_vnUp;
} // GetUpDirection()


/**
 * Set current view up direction
 * 
 * @param upDirection       new up direction in world coordinates
 */
void vxSubmarine::SetUpDirection(const Normal<float4> & vnUpDirection)
{
  m_vnUp = vnUpDirection;
} // SetUpDirection()


/**
 * Get the status of the leveling feature.
 */
const bool & vxSubmarine::GetLeveling() const
{
  return m_bLeveling;
} // GetLeveling()


/**
 * Set the leveling feature
 * 
 * @param leveling          new status of leveling
 */
void vxSubmarine::SetLeveling(const bool bLeveling)
{
  m_bLeveling = bLeveling;
} // SetLeveling()



/**
 * Set the amount of pan parallel to the image plane
 * 
 * @param panAmount         how much u-v movement
 */
void vxSubmarine::SetPanAmount(const Vector<float4> & vPanAmount)
{
  m_panVector = vPanAmount;
  // clamp to the range [-1,1]
  m_panVector.Clamp(-1.0F, 1.0F);
} // SetPanAmount()


/**
 * Control whether we are panning or rotating.
 */
void vxSubmarine::SetPanMode(const bool bPan)
{
  m_bPan = bPan;
} // SetPanMode()


/**
 * Retrieves the heading turn rate.
 */
const float4 & vxSubmarine::GetHeading() const
{
  return m_fHeadingInput;
} // GetHeading()


/**
 * Replace the heading input by some value.
 * 
 * @param heading           [-1,1] turn left (-1) and right (1)
 */
void vxSubmarine::SetHeading(const float4 heading)
{
  m_fHeadingInput = heading;
  // clamp to the range [-1,1]
  m_fHeadingInput = Bound(-1.0F, m_fHeadingInput, 1.0F);
} // SetHeading()


/**
 * Add some amount to the heading input.
 * 
 * @param heading           [-1,1] turn left (-1) and right (1)
 */
void vxSubmarine::AddHeading(const float4 heading)
{
  m_fHeadingInput += heading;
  // clamp to the range [-1,1]
  m_fHeadingInput = Bound(-1.0F, m_fHeadingInput, 1.0F);
} // AddHeading()


/**
 * Retrieves the pitch turn rate.
 */
const float4 & vxSubmarine::GetPitch() const
{
  return m_fPitchInput;
} // GetPitch()


/**
 * Replace the heading input by some value.
 * 
 * @param pitch             [-1,1] turn up (1) and down (-1)
 */
void vxSubmarine::SetPitch(const float4 pitch)
{
  m_fPitchInput = pitch;
  // clamp to the range [-1,1]
  m_fPitchInput = Bound(-1.0F, m_fPitchInput  , 1.0F);
} // SetPitch()


/**
 * Add some amount to the pitch input.
 * 
 * @param pitch             [-1,1] turn up (1) and down (-1)
 */
void vxSubmarine::AddPitch(const float4 pitch)
{
  m_fPitchInput += pitch;
  // clamp to the range [-1,1]
  m_fPitchInput = Bound(-1.0F, m_fPitchInput  , 1.0F);
} // AddPitch()


/**
 * Apply forces from the user to the simulation.
 * The amount of forward (or reverse) thrust is added to the
 * velocity in the view direction.  The amount of heading
 * turning (left-right) is added to the headingVelocity and
 * the amount of pitch turning (up-down) is added to the
 * pitch velocity.
 * 
 * @param thrust            [-1,1] throttle fwd (1) and rev (-1)
 * @param heading           [-1,1] turn left (-1) and right (1)
 * @param pitch             [-1,1] turn up (1) and down (-1)
 */
void vxSubmarine::SetForces(const float4 thrust, const float4 heading, const float4 pitch)
{
  m_fThrustInput = thrust;
  m_fHeadingInput = heading;
  m_fPitchInput = pitch;

  // clamp to the range [-1,1]
  //m_fThrustInput  = Bound(-1.0F, m_fThrustInput , 1.0F);
  m_fHeadingInput = Bound(-1.0F, m_fHeadingInput, 1.0F);
  m_fPitchInput   = Bound(-1.0F, m_fPitchInput  , 1.0F);
} // SetForces()


/**
 * Apply additional forces from the user to the simulation.
 * 
 * @param thrust            [-1,1] throttle fwd (1) and rev (-1)
 * @param heading           [-1,1] turn left (-1) and right (1)
 * @param pitch             [-1,1] turn up (1) and down (-1)
 */
void vxSubmarine::AddForces(const float4 thrust, const float4 heading, const float4 pitch)
{
  m_fThrustInput += thrust;
  m_fHeadingInput += heading;
  m_fPitchInput += pitch;

  // clamp to the range [-1,1]
  m_fThrustInput  = Bound(-1.0F, m_fThrustInput , 1.0F);
  m_fHeadingInput = Bound(-1.0F, m_fHeadingInput, 1.0F);
  m_fPitchInput   = Bound(-1.0F, m_fPitchInput  , 1.0F);
} // AddForces()


/**
 * Get the value of the throttle setting.
 * 
 * @return the throttle value
 */
const float4 & vxSubmarine::GetThrottle() const
{
  return m_fThrottle;
} // GetThrottle()


/**
 * Set the throttle value.
 * 
 * @param percent           the desired percentage of the maximum speed
 */
void vxSubmarine::SetThrottle(const float4 percent)
{
  m_fThrottle = percent;
} // SetThrottle()


// $Log: vxSubmarine.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/04/18 19:56:30  michael
// initial code review
//
// Revision 1.2  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.1  2002/10/21 23:19:30  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxSubmarine.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxSubmarine.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
