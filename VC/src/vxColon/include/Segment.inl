// $Id: Segment.inl,v 1.2 2006/09/13 15:23:31 frank Exp $
//
// Copyright © 2001-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.net)


/**
 * Returns a const reference to the current skeleton.
 *
 * @return   a reference to the current skeleton.
 */
inline Skeleton & Segment::GetCurrentSkeleton()
{
  if (m_vCenterlines.size() < 1)
    return m_nullCenterline;
  else
    return m_vCenterlines[0];
} // GetCurrentSkeleton()


/**
 * Returns a const reference to the current skeleton.
 *
 * @return   a const reference to the current skeleton.
 */
inline const Skeleton & Segment::GetCurrentSkeleton() const
{
  if (m_vCenterlines.size() < 1)
    return m_nullCenterline;
  else
    return m_vCenterlines[0];
} // GetCurrentSkeleton()


/**
 * Select the preferred direction of travel within this segment.
 *
 * @param bRegularDirection if the perferred direction of travel is from the original start to end.
 */
void Segment::SelectDirection(const bool bRegularDirection)
{
  if (bRegularDirection == true)
  {
    // we want to go from the start to the end
    m_bRegularDirection = true;
    m_vStartpointMM = m_vEndpointsMM[0];
    m_vEndpointMM = m_vEndpointsMM[1];
  } 
  else 
  {
    // reverse the direction to go from the end to the start
    m_bRegularDirection = false;
    m_vStartpointMM = m_vEndpointsMM[1];
    m_vEndpointMM = m_vEndpointsMM[0];
  } // if selected the regular direction
} // SelectDirection()


// $Log: Segment.inl,v $
// Revision 1.2  2006/09/13 15:23:31  frank
// formatting
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.1.6.1.12.1  2005/06/28 12:06:18  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.1.6.1  2003/03/26 23:14:56  jmeade
// Issue 2849:  Const equivalents to a few access methods.
//
// Revision 3.1  2002/07/03 03:23:21  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.0.4.1  2002/07/02 20:51:51  jmeade
// Code standards.
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:49:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:42   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.1  2001/04/30 14:24:04  frank
// New file...
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Segment.inl,v 1.2 2006/09/13 15:23:31 frank Exp $
// $Id: Segment.inl,v 1.2 2006/09/13 15:23:31 frank Exp $
