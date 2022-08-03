// $Id: Submarine.inl,v 1.2.6.1 2007/08/17 16:09:40 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Submarine.inl
// Owner: Frank Dachille (frank@viatronix.net)


/**
 * Description: A class to support guided navigation within the colon lumen.
 * It contains a physical simulation of an object with mass, rotation
 * mass, forward/reverse thrust, and pitch/heading rotation thrust.
 */


/**
 * Get current view position.
 */
const Point<float4> & Submarine::GetPositionWorld() const
{
  return m_vpPositionWorld;
} // GetPositionWorld()


/**
 * Get current view position.
 */
const Point<float4> & Submarine::GetPositionVolume() const
{
  return m_vpPositionVolume;
} // GetPositionVolume()


/**
 * Get current view direction.
 */
const Normal<float4> & Submarine::GetOrientation() const
{
  return m_vnOrientation;
} // GetOrientation()


/**
 * Set current view direction
 * 
 * @param orientation       new view direction in world coordinates
 */
void Submarine::SetOrientation(const Normal<float4> & vnOrientation)
{
  m_vnOrientation = vnOrientation;
  m_uViewIdentifier++;
} // SetOrientation()


/**
 * Get current view up direction.
 */
const Normal<float4> & Submarine::GetUpDirection() const
{
  return m_vnUp;
} // GetUpDirection()


/**
 * Set current view up direction
 * 
 * @param upDirection       new up direction in world coordinates
 */
void Submarine::SetUpDirection(const Normal<float4> & vnUpDirection)
{
  m_vnUp = vnUpDirection;
  m_uViewIdentifier++;
} // SetUpDirection()


/**
 * Get the status of the leveling feature.
 */
const bool & Submarine::GetLeveling() const
{
  return m_bLeveling;
} // GetLeveling()


/**
 * Set the leveling feature
 * 
 * @param leveling          new status of leveling
 */
void Submarine::SetLeveling(const bool bLeveling)
{
  m_bLeveling = bLeveling;
} // SetLeveling()


/**
 * Get the status of the potential field.
 */
const bool & Submarine::GetPotential() const
{
  return m_bPotential;
} // GetPotential()


/**
 * Set the influence of the potential field
 * 
 * @param potential         new potential strength
 */
void Submarine::SetPotential(const bool bPotential)
{
  if (bPotential != m_bPotential)
  {
    m_potentialTimer.Reset();
    m_bPotential = bPotential;
  }
} // SetPotential()


/**
 * Get the neighbor searching state
 * 
 * @return the value of the neighbor searching toggle
 */
const bool & Submarine::GetNeighborSearching() const
{
  return m_bSearchNeighbors;
} // GetNeighborSearching()


/**
 * Set the neighbor searching toggle state
 * 
 * @param neighborSearch    whether or not to search the neighbors
 */
void Submarine::SetNeighborSearching(const bool bNeighborSearch)
{
  m_bSearchNeighbors = bNeighborSearch;
} // SetNeighborSearching()


/**
 * Set the amount of pan parallel to the image plane
 * 
 * @param panAmount         how much u-v movement
 */
void Submarine::SetPanAmount(const Vector<float4> & vPanAmount)
{
  m_vPan = vPanAmount;
	// clamp to the range [-1,1]
  m_vPan.Clamp(-1.0F, 1.0F);
} // SetPanAmount()


/**
 * Control whether we are panning or rotating.
 */
void Submarine::SetPanMode(const bool bPan)
{
  m_bPan = bPan;
} // SetPanMode()

  
/**
 * Retrieves the heading turn rate.
 */
const float4 & Submarine::GetHeading() const
{
  return m_fHeadingInput;
} // GetHeading()


/**
 * Replace the heading input by some value.
 * 
 * @param heading           [-1,1] turn left (-1) and right (1)
 */
void Submarine::SetHeading(const float4 heading)
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
void Submarine::AddHeading(const float4 heading)
{
	m_fHeadingInput += heading;
	// clamp to the range [-1,1]
  m_fHeadingInput = Bound(-1.0F, m_fHeadingInput, 1.0F);
} // AddHeading()


/**
 * Retrieves the pitch turn rate.
 */
const float4 & Submarine::GetPitch() const
{
  return m_fPitchInput;
} // GetPitch()


/**
 * Replace the heading input by some value.
 * 
 * @param pitch             [-1,1] turn up (1) and down (-1)
 */
void Submarine::SetPitch(const float4 pitch)
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
void Submarine::AddPitch(const float4 pitch)
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
void Submarine::SetForces(const float4 thrust, const float4 heading, const float4 pitch)
{
	m_fThrustInput = thrust;
	m_fHeadingInput = heading;
	m_fPitchInput = pitch;

	// clamp to the range [-1,1]
  m_fThrustInput  = Bound(-1.0F, m_fThrustInput , 1.0F);
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
void Submarine::AddForces(const float4 thrust, const float4 heading, const float4 pitch)
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
const float4 & Submarine::GetThrottle() const
{
	return m_fThrottle;
} // GetThrottle()


/**
 * Set the throttle value.
 * 
 * @param percent           the desired percentage of the maximum speed
 */
void Submarine::SetThrottle(const float4 percent)
{
	m_fThrottle = percent;
} // SetThrottle()


/**
 * Gets how much the potential field affects the submarine.
 * 
 * @return the strength of the potential field
 */
const float4 & Submarine::GetPotentialStrength() const
{
	return m_fPotentialStrength;
} // GetPotentialStrength()


/**
 * Set how much the potential field affects the submarine.
 * 
 * @param percent           the strength of the potential strength
 */
void Submarine::SetPotentialStrength(const float4 percent)
{
	m_fPotentialStrength = percent;
  m_fPotentialStrength = Bound(0.0F, m_fPotentialStrength, 1.0F);
} // SetPotentialStrength()


/**
 * Get the status of the boundary field.
 */
const bool & Submarine::GetBoundary() const
{
  return m_bBoundary;
} // GetBoundary()


/**
 * Set the influence of the boundary field.
 * 
 * @param boundary          new status of the boundary field
 */
void Submarine::SetBoundary(const bool bBoundary)
{
  m_bBoundary = bBoundary;
} // SetBoundary()


/**
 * Get the future predicted position
 * 
 * @return the predicted position
 */
const Point<float4> & Submarine::GetPredictedPosition() const
{
  return m_vpPredictedPosition;
} // GetPredictedPosition()


/**
 * Retrieve a unique identifier for each view position and orientation.
 */
const uint4 & Submarine::GetViewIdentifier() const
{
  return m_uViewIdentifier;
} // GetViewIdentifier()


/**
 * Set the parent pointer.
 *
 * @param pDataset a pointer to our parent Dataset object.
 */
void Submarine::SetParentDataset(Dataset * pDataset)
{
  m_pDataset = pDataset;
} // SetParentDataset


/**
 * Gets the transition mode
 *
 * @return the transition mode
 */
const Submarine::TransitionModeEnum  Submarine::GetTransitionMode() const
{
  return m_eTransitionMode;
} // GetTransitionMode()


/**
 * Sets the transition mode
 * 
 * @param   eMode   new transition mode
 */
void Submarine::SetTransitionMode( const Submarine::TransitionModeEnum eMode )
{
  m_eTransitionMode = eMode;
} // SetTransitionMode( const TransitionModeEnum eMode )


// $Log: Submarine.inl,v $
// Revision 1.2.6.1  2007/08/17 16:09:40  geconomos
// Still reworking the transition
//
// Revision 1.2  2006/02/07 21:32:44  gdavidson
// Code Review
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.3  2003/01/27 21:47:15  jmeade
// Method to set current flight direction.
//
// Revision 3.2  2002/04/26 15:57:30  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.1  2002/04/12 02:15:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.0.4.1  2002/04/08 23:09:56  jmeade
// SetPotential(): Set variable only when necessary.
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
//    Rev 1.0   Sep 16 2001 17:27:20   ingmar
// Initial revision.
// Revision 1.18  2001/05/16 19:14:29  frank
// Rolled forward all the way.
//
// Revision 1.17  2001/05/16 17:09:02  frank
// Slowly rolling forward...
//
// Revision 1.16  2001/05/16 16:32:18  frank
// Virtual rollback to fix jittering bug.
//
// Revision 1.12  2001/04/10 17:52:21  frank
// New coding conventions in Submarine class.
//
// Revision 1.11  2001/04/10 12:49:55  frank
// Removed revision history from top of file.
//
// Revision 1.10  2001/01/23 18:16:34  frank
// Saved a unique view identifier for view caching purposes.
//
// Revision 1.9  2001/01/05 15:35:23  frank
// Used SegmentList to contain navigation direction.
//
// Revision 1.8  2000/12/22 19:30:54  frank
// Changed the style of panning back to relative mode from absolute.
//
// Revision 1.7  2000/11/20 14:36:18  frank
// Added some access functions
//
// Revision 1.6  2000/10/12 02:48:49  kevin
// Added flight direction access and toggling
//
// Revision 1.5  2000/09/21 19:37:21  frank
// C++ Coding Standards
//
// Revision 1.4  2000/09/19 18:16:52  kevin
// Reversed one version due to errors that could
// not be fixed due to broken pipeline/volume utlity stuff.
// (This will be revisited once the other errors
// are fixed and we can work at all once again)
//
// Revision 1.2  2000/09/12 14:54:51  frank
// Changes for coding standard compliance
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 19    9/01/00 9:18a Frank
// Cosmetics
// 
// 18    8/28/00 12:48p Antonio
// updated header and footer
//
//*****************  Version 17  *****************
//User: Kevin           Date:  8/22/00  Time: 11:14p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed slice positioning to utilize "orthogonalPoint"
//
//*****************  Version 16  *****************
//User: Jeff            Date:  8/22/00  Time:  7:29p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Value access functions for current throttle and potential strength
//"const TYPE &" in function headers replaces "const TYPE"
//
//*****************  Version 15  *****************
//User: Frank           Date:  8/18/00  Time:  2:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Prevented updates during all navigation
//
//*****************  Version 14  *****************
//User: Frank           Date:  8/08/00  Time:  9:38a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Renamed a function for clarity
//
//*****************  Version 13  *****************
//User: Frank           Date:  7/27/00  Time: 11:23a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added a boundary field toggle
//
//*****************  Version 12  *****************
//User: Frank           Date:  7/25/00  Time:  2:30p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Enabled look-around in replay mode
//
//*****************  Version 11  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 10  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 10
//Comment:
//  
//
//*****************  Version 9  *****************
//User: Jeff            Date:  7/18/00  Time:  1:47p
//Checked in $/v1k/src/ReaderLib
//
//*****************  Version 8  *****************
//User: Frank           Date:  7/18/00  Time:  1:23p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  Implemented instant replay
//
//*****************  Version 7  *****************
//User: Frank           Date:  7/18/00  Time: 10:45a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  virtual rollback
//
//*****************  Version 6  *****************
//User: Frank           Date:  7/18/00  Time: 10:28a
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  new time zone test
//
//*****************  Version 5  *****************
//User: Frank           Date:  7/18/00  Time: 10:23p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  trying out new comment style
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/18/00  Time: 10:10p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  tested
//
//*****************  Version 3  *****************
//User: Frank           Date:  7/17/00  Time: 10:01p
//Checked in $/Viatron1000/ReaderLib
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/17/00  Time:  9:58p
//Checked in $/Viatron1000/ReaderLib
//Comment:
//  testing history
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Submarine.inl
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Submarine.inl,v 1.2.6.1 2007/08/17 16:09:40 geconomos Exp $
// $Id: Submarine.inl,v 1.2.6.1 2007/08/17 16:09:40 geconomos Exp $
