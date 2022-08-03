// $Id: SkeletonNode.C,v 1.2 2006/09/13 15:27:07 frank Exp $
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
#include "SkeletonNode.h"
#include "Logger.h"
#include "Triple.h"


// Namespaces
using namespace std;


// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * allow to direct a SkeletonNode to an output stream
 *
 * @param os reference to std::ostream output stream
 *
 * @return the output stream
 */
ostream & SkeletonNode::AsOstream( ostream & os ) const
{
  os << "SkeletonNode{posVol=" << m_vVolumePos
  << ", posWld=" << m_vWorldPos
  << ", tan=" << m_vWorldTan
  << ", up=" << m_vWorldUp
  << ", dfs=" << m_worldDfs
  << ", dfe=" << m_worldDfe
  << ", branchIdx=" << m_branchIdx << "}";
  return os;
} // AsOstream()


/**
 * Equality operator
 *
 * @param other the other node
 *
 * @return true if ==
 */
bool SkeletonNode::operator ==( const SkeletonNode & other ) const
{
  // Point<uint> m_vVolumePos
  if (m_vVolumePos != other.m_vVolumePos) 
  {  
    LogWrn("Something varies.","SkeletonNode","operator==");
    return false;
  } // if different

  // Point<float> m_vWorldPos
  if (m_vWorldPos != other.m_vWorldPos) {
    LogWrn("Something varies.","SkeletonNode","operator==");
    return false;
  } // if different

  // Normal<float> m_vWorldTan
  if (m_vWorldTan != other.m_vWorldTan) {
    LogWrn("Something varies.","SkeletonNode","operator==");
    return false;
  } // if different

  // float m_worldDfs
  if (m_worldDfs != other.m_worldDfs) {
    LogWrn("Something varies.","SkeletonNode","operator==");
    return false;
  } // if different

  // float m_worldDfe
  if (m_worldDfe != other.m_worldDfe) {
    LogWrn("Something varies.","SkeletonNode","operator==");
    return false;
  } // if different

  // ushort m_branchIdx
  if (m_branchIdx != other.m_branchIdx) {
    LogWrn("Something varies.","SkeletonNode","operator==");
    return false;
  } // if different

  // Normal<float> m_vWorldUp
  if (m_vWorldUp != other.m_vWorldUp) {
    LogWrn("Something varies.","SkeletonNode","operator==");
    return false;
  } // if different

  return true;

} // operator ==()


/**
 * Convert the world position into a volume position given the voxel units
 *
 * @param unitsTriple the volume units
 */
void SkeletonNode::ConvertWorldToVolume( const Triple<float4> & unitsTriple )
{
  m_vVolumePos = m_vWorldPos.GetDividedAnIso( unitsTriple );
} // ConvertWorldToVolume()


/**
 * Compute the right direction based on the tangent (forward) and up directions (this helps to define a local coordinate system).
 *
 * @return the right direction
 */
Normal<float4> SkeletonNode::GetWorldRight() const
{
  return Normal<float4>( m_vWorldTan.Cross( m_vWorldUp ) );
} // GetWorldRight()


/**
 * Helper function to output a node to a stream
 *
 * @param os the output stream
 * @param the skeleton node
 *
 * @return the output stream
 */
std::ostream & operator <<( std::ostream & os, const SkeletonNode & skelNode )
{
  return skelNode.AsOstream( os );
} // operator <<()


/**
 * Rotate a prone skeleton node to supine
 *
 * @param worldSize the size of the entire volume in MM
 */
void SkeletonNode::RotateProneToSupine( const Triple<float4> & worldSize )
{

  // early return if there is not enough data to compute the scaling from world to voxel coordinates
  if ( m_vWorldPos.m_x == 0 || m_vWorldPos.m_y == 0 )
    return;

  // calculate the size of the volume in voxels
  const int4 iVoxelDimX = worldSize.m_x * m_vVolumePos.m_x / m_vWorldPos.m_x;
  const int4 iVoxelDimY = worldSize.m_y * m_vVolumePos.m_y / m_vWorldPos.m_y;

  // rotate all the points
  m_vVolumePos.m_x = iVoxelDimX - m_vVolumePos.m_x;
  m_vVolumePos.m_y = iVoxelDimY - m_vVolumePos.m_y;
  m_vWorldPos.m_x = worldSize.m_x - m_vWorldPos.m_x;
  m_vWorldPos.m_y = worldSize.m_y - m_vWorldPos.m_y;

  // flip all the normals
  m_vWorldTan = Normal3D<float4>( - m_vWorldTan.X(), - m_vWorldTan.Y(), m_vWorldTan.Z() );
  m_vWorldUp = Normal3D<float4>( - m_vWorldUp.X(), - m_vWorldUp.Y(), m_vWorldUp.Z() );

} // RotateProneToSupine()


// undefines
#undef FILE_REVISION 


// $Log: SkeletonNode.C,v $
// Revision 1.2  2006/09/13 15:27:07  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.4  2003/05/21 14:20:44  frank
// code review
//
// Revision 3.3  2003/05/16 11:44:58  michael
// code review
//
// Revision 3.2  2003/01/02 14:00:04  ingmar
// added missing include
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
//    Rev 2.1   Oct 01 2001 16:10:20   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:49:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.5  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.4  2000/12/13 15:06:45  frank
// Added equality operators
//
// Revision 1.3  2000/11/06 19:24:01  frank
// Cleaned up output text
//
// Revision 1.2  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.1.1.1.16.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.1.1.1.16.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.1.1.1.12.1  2000/10/26 13:28:32  frank
// Added new segment structure
//
// Revision 1.1.1.1.6.2  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.1.1.1.6.1  2000/10/19 19:05:27  frank
// Added up direction and revised scale function
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
// 7     8/16/00 2:05p Antonio
// modified header and footer
//
// *****************  Version 6  *****************
// User: Antonio      Date: 8/11/00    Time: 12:47p
// Updated in $/vx/src/Volume
// added header and history to end of file
//
// *****************  Version 5  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 4  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 3  *****************
// User: Ingmar          Date:  7/23/00  Time:  3:38p
// Checked in $/v2k/src/Skeleton
// Comment:
//   moved finding skeleton into Skeletonator, Skeleton now just holde
// the Skeleton Array
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/20/00  Time:  9:27a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added SkeletonNode class
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/20/00  Time:  8:22a
// Created SkeletonNode.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/SkeletonNode.C,v 1.2 2006/09/13 15:27:07 frank Exp $
// $Id: SkeletonNode.C,v 1.2 2006/09/13 15:27:07 frank Exp $
