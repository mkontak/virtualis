// $Id: SkeletonNode.h,v 1.2 2006/09/13 15:27:07 frank Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "Point.h"
#include "Vector.h"
#include "Normal.h"


/**
 * Structure for storing all elements of a skeleton node.
 */
class SkeletonNode 
{
// member functions
public:

  /// default constructor
  SkeletonNode() : m_branchIdx(0) {}

  /// allow to direct a SkeletonNode to an output stream.
  std::ostream & AsOstream( std::ostream & os ) const;

  /// Helper function to output a node to a stream
  friend std::ostream & operator <<( std::ostream & os, const SkeletonNode & skelNode );

  /// Equality operator.
  bool operator ==( const SkeletonNode & other ) const;

  /// Convert the world position into a volume position given the voxel units.
  void ConvertWorldToVolume( const Triple<float4> & unitsTriple );

  /// Compute the right direction based on the tangent (forward) and up directions (this helps to define a local coordinate system).
  Normal<float4> GetWorldRight() const;

  /// Rotate a prone skeleton node to supine
  void RotateProneToSupine( const Triple<float4> & worldSize );

// data members
public:

  /// 3D position along the skeleton in Volume coordinates
  Point<uint4> m_vVolumePos;

  /// 3D position along the skeleton in World coordinates
  Point<float4> m_vWorldPos;

  /// skeleton tangent vector in World coordinates
  Normal<float4> m_vWorldTan;

  /// distance from start in World coordinates
  float4 m_worldDfs;

  /// distance from end in World coordinates
  float4 m_worldDfe;

  /// index at which the next branch starts, or 0 for no branch, or MAX_USHORT for branch end
  uint2 m_branchIdx;

  /// up vector in World coordinates
  Normal<float4> m_vWorldUp;

}; // class SkeletonNode


// $Log: SkeletonNode.h,v $
// Revision 1.2  2006/09/13 15:27:07  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/05/21 14:20:44  frank
// code review
//
// Revision 3.7  2003/05/16 11:44:58  michael
// code review
//
// Revision 3.6  2003/01/22 21:37:03  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.5  2002/09/29 21:34:57  kevin
// Coding Standards (member variables are at the bottom)
//
// Revision 3.4  2002/05/07 18:24:01  frank
// Removed unneccessary includes.
//
// Revision 3.3  2002/03/25 18:00:07  geconomos
// no message
//
// Revision 3.2  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 3.1  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
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
// Revision 1.13  2001/08/20 11:24:28  frank
// Added function to get right direction.
//
// Revision 1.12  2001/08/07 13:36:48  frank
// Added world to volume coordinate conversion function.
//
// Revision 1.11  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.10  2001/04/09 18:03:34  dmitry
// Removed Revision History from Top of File
//
// Revision 1.9  2001/04/05 18:00:23  dmitry
// updated datatypes
//
// Revision 1.8  2001/01/26 14:19:14  frank
// All data files are written with unsigned ints...we need to prepare data files before changing this.
//
// Revision 1.7  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.6  2000/12/13 15:06:45  frank
// Added equality operators
//
// Revision 1.5  2000/10/31 14:15:08  frank
// Merged in new segment structure
//
// Revision 1.4  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.3.6.3  2000/10/30 17:05:58  frank
// merged back using old v2k branch
//
// Revision 1.3.6.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.3.6.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.3.2.1  2000/10/26 13:27:14  frank
// Added new segment structure
//
// Revision 1.3  2000/10/20 17:12:00  kevin
// Added up vector for each skeleton node
//
// Revision 1.2.4.2  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.2.4.1  2000/10/19 19:05:27  frank
// Added up direction and revised scale function
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
// 11    9/04/00 11:10p Ingmar
// now works with new naming sceme
//
// 10    8/21/00 1:45a Sarang
// updated header
//
// *****************  Version 9  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Volume
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// *****************  Version 8  *****************
// User: Antonio      Date: 8/11/00    Time: 1:27p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 7  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 6  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 5  *****************
// User: Ingmar          Date:  7/24/00  Time:  2:11p
// Checked in $/v2k/src/Volume
// Comment:
//   added skeleton and skletonNode classes
//
// *****************  Version 4  *****************
// User: Ingmar          Date:  7/23/00  Time: 11:03p
// Checked in $/v2k/src/Skeleton
// Comment:
//   near nice Skeleton
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
// User: Ingmar          Date:  7/20/00  Time:  7:54a
// Created SkeletonNode.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/SkeletonNode.h,v 1.2 2006/09/13 15:27:07 frank Exp $
// $Id: SkeletonNode.h,v 1.2 2006/09/13 15:27:07 frank Exp $
