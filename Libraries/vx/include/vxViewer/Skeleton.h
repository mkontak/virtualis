// $Id: Skeleton.h,v 1.2 2006/09/13 15:26:20 frank Exp $
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
#include "SkeletonNode.h"



/**
 * A class to encapsulate the skeleton information.
 */
class VX_VIEWER_DLL Skeleton
{
// Constants and Enumerations
public:
 
  /// The events supported by this class.
  enum EventsEnum
  {
    EVENT_ADDED           = 0x00000001,
    EVENT_REMOVED         = 0x00000002,
    EVENT_MODIFIED        = 0x00000004,
  }; // enum EventsEnum
  
public:

  /// Constructor
  Skeleton();

  /// Convenience constructor
  Skeleton( const uint2 uNumElements );

  /// Copy Constructor
  Skeleton( const Skeleton & other );

  /// Destructor
  virtual ~Skeleton();

  /// Resize the array to the given number of elements
  void Resize( const uint2 uNumElements );

  /// Erase an element from the array
  void Erase( const uint2 uIndex );

  /// Read from stream
  void Read( std::ifstream & skeFile );

  /// Write to a stream
  void Write(std::ofstream & skeFile) const;

  /// Read from file
  void Read ( const std::string & sFileName );

  /// Write to file
  void Write( const std::string & sFileName ) const;

  /// Retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// Retrieves all the data in serialized form.
  void WriteSerializedData( int1 ** piBuffer, const int1 * piBufferEnd ) const;

  /// Reads in all the data from serialized form.
  void ReadSerializedData( int1 ** piBuffer, const int1 * piBufferEnd );

  /// Equality operator
  bool operator ==( const Skeleton & other ) const;

  /// Returns the number of elements.
  uint4 GetNumOfElements() const;

  /// get complete Skeleton Node: not out of range.
  const SkeletonNode & GetSkelNode( const uint2 & uIndex, const bool & bNotOutOfBound = false ) const;
  
  /// Get 3D position along the skeleton in volume coordinates.
  const Point<uint2> & GetVolumePos( const uint2 & uIndex ) const;

  /// Get 3D position along the skeleton in World coordinates
  const Point<float4> & GetWorldPos( const uint2 & uIndex ) const;
  
  /// Get skeleton tangent vector in World coordinates.
  const Normal<float4> & GetWorldTan( const uint2 & uIndex ) const;
  
  /// Get distance from start in World coordinates.
  float4 GetWorldDfs( const uint2 & uIndex ) const;

  /// Get distance from end in World coordinates.
  float4 GetWorldDfe( const uint2 & uIndex ) const;
  
  /// Set complete Skeleton Node.
  bool SetSkelNode(const uint2 & uIndex, const SkeletonNode & node );
  
  /// Set 3D position along the skeleton in World coordinates.
  bool SetVolumePos( const uint2 & uIndex, const Point<float4> & volumePosition );
  
  /// Set 3D position along the skeleton in World coordinates.
  bool SetWorldPos( const uint2 & uIndex, const Point<float4> & worldPosition );
  
  /// Set skeleton tangent vector in World coordinates.
  bool SetWorldTan( const uint2 & uIndex, const Normal<float4> & tangent );
  
  /// Set distance from start in World coordinates.
  bool SetWorldDfs( const uint2 & uIndex, const float4 fDfs );
  
  /// Set distance from end in World coordinates.
  bool SetWorldDfe( const uint2 & uIndex, const float4 fDfe );
  
  /// Add a new node to the end of the skeleton.
  bool AddSkelNode( const SkeletonNode & node );

  /// Get an interpolated position along the skeleton in world coordinates.
  Point<float4> GetInterpolatedWorldPos( const float4 fIndex ) const;

  /// Applies the scaling factor to all positions and directions
  void ApplyScale( const Triple<float4> & factors );

  /// Smooths the world coordinates of the skeleton with a small box filter.
  void Smooth();

  /// Recalculate the DFS and DFE
  void RecalculateDistances();

  /// Get the piecewise Euclidean length
  float4 GetPiecewiseEuclideanLength() const;

  /// Calculate appropriate "up" vectors for the entire skeleton.
  void ComputeUpVectors();

  /// Compute the tangent directions using a specified central difference radius.
  void ComputeTangents( const float4 & fRadius );

  /// Calculate the inside angle of 3 points.
  //float4 CalculateInsideAngle() const;
  
  /// Calculate the outside angle of 3 points.
  //float4 CalculateOutsideAngle() const;
  
  /// Reverses the order of points in the skeleton.
  void ReverseOrder();

  /// Concatenates a skeleton to the end of the current skeleton.
  void ConcatentateSkeleton( const Skeleton & otherSkeleton );

  /// Allow to direct a SkeletonNode to an output stream.
  std::ostream & AsOstream( std::ostream & os ) const;

  /// Output to a stream
  friend std::ostream & operator << ( std::ostream & os, const Skeleton & s ) { return ( s.AsOstream( os ) ); }

  /// Rotate a prone centerline to supine
  void RotateProneToSupine( const Triple<float4> & worldSize );

// data members
public:

  /// centerline points for angular and volume measurement.
  //std::vector<uint4> m_vSkelMarkerIndex;
  
protected:
  
  // The list of skeleton nodes
  std::vector<SkeletonNode> m_vSke;
  
private:  
  
  // A default node to return in case of error
  static const SkeletonNode m_background;

  /// The delimiter used to verify the beginning of file I/O.
  static const std::string m_sDelimiter;
  
}; // class Skeleton


// $Log: Skeleton.h,v $
// Revision 1.2  2006/09/13 15:26:20  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.12  2003/05/14 12:58:48  frank
// continued code review
//
// Revision 3.11  2003/05/06 16:18:49  frank
// code review
//
// Revision 3.10  2003/01/02 18:00:35  ingmar
// added missing include
//
// Revision 3.9  2002/11/20 14:20:57  dongqing
// add copy constructor
//
// Revision 3.8  2002/09/29 21:35:29  kevin
// Made the vector of skeleton nodes protected so it can be inherited
//
// Revision 3.7  2002/09/06 16:10:58  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.6  2002/08/21 15:47:06  frank
// No more Skeleton.inl
//
// Revision 3.5  2002/08/14 16:10:34  wenli
// initial version - Vascular
//
// Revision 3.4  2002/06/21 16:15:53  frank
// Added an interpolated position retrieval function.
//
// Revision 3.3.2.2  2002/05/20 17:18:55  frank
// Added interpolated position access function.
//
// Revision 3.3.2.1  2002/05/08 17:23:06  frank
// Added Curved MPR rendering.
//
// Revision 3.3  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 3.2  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 3.1.2.1  2002/08/06 20:43:13  jmeade
// Issue 2596:  Condensed two equivalent functions into one; Code standards.
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
// 
//    Rev 2.0   Sep 16 2001 23:49:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.28  2001/08/09 21:44:49  binli
// bug 000656: make sure no out of boundary
//
// Revision 1.27  2001/07/31 19:25:38  frank
// Added a concatenation function.
//
// Revision 1.26  2001/06/08 15:03:29  dave
// changed array to std::vect, no copy, = constructors
//
// Revision 1.25  2001/05/15 17:49:39  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.24  2001/05/02 19:32:38  ingmar
// Segment.C
//
// Revision 1.22.2.1  2001/05/01 21:44:11  jmeade
// Added exception handling, cleaned up code. (Frank D.)
//
// Revision 1.23  2001/04/26 11:17:23  frank
// Added exception handling, cleaned up code.
//
// Revision 1.22  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.21  2001/04/10 17:35:22  dmitry
// updated datatypes
//
// Revision 1.20  2001/04/05 18:00:23  dmitry
// updated datatypes
//
// Revision 1.19  2001/03/29 16:18:21  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.18  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.17  2001/01/09 16:50:27  frank
// Better form.
//
// Revision 1.16  2000/12/18 19:51:52  frank
// Removed stringstream I/O
//
// Revision 1.15  2000/12/14 14:52:49  frank
// Modified the interface for serialization/deserialization
//
// Revision 1.14  2000/12/13 15:06:45  frank
// Added equality operators
//
// Revision 1.13  2000/12/13 13:45:20  frank
// Added equality operator
//
// Revision 1.12  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.11  2000/12/09 12:54:25  frank
// Improved stream I/O
//
// Revision 1.10  2000/12/05 19:57:25  frank
// Added I/O functions
// Added ComputeUpVectors function
//
// Revision 1.9  2000/11/22 14:30:35  frank
// Added the erase function
//
// Revision 1.8  2000/11/16 15:45:16  frank
// Added tangent computation function
//
// Revision 1.7  2000/11/06 19:45:27  frank
// Removed branchidx stuff
// Cleaned up error cases
//
// Revision 1.6  2000/11/06 18:41:25  frank
// Added function to reverse the node order
//
// Revision 1.5  2000/10/31 14:15:07  frank
// Merged in new segment structure
//
// Revision 1.4  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.3.12.3  2000/10/30 19:14:08  frank
// cosmetics
//
// Revision 1.3.12.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.3.12.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.3.8.2  2000/10/27 15:20:54  frank
// Got segment structure working
//
// Revision 1.3.8.1  2000/10/26 13:28:32  frank
// Added new segment structure
//
// Revision 1.3.2.3  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.3.2.2  2000/10/19 19:05:26  frank
// Added up direction and revised scale function
//
// Revision 1.3.2.1  2000/10/19 14:07:50  frank
// Added new function
//
// Revision 1.3  2000/09/28 17:10:00  frank
// Added I/O for segments and segment lists
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
//
// 20    9/04/00 11:10p Ingmar
// now works with new naming sceme
//
// 19    8/29/00 3:21p Frank
// Added DF* recalculation function
//
// 18    8/21/00 1:42a Sarang
// modified header
//
// *****************  Version 17  *****************
// User: Antonio      Date: 8/11/00    Time: 1:24p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 16  *****************
// User: Jay             Date:  8/11/00  Time:  1:33a
// Checked in $/v2k/src/Volume
// Comment:
//
//
// *****************  Version 15  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 14  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 13  *****************
// User: Michael         Date:  8/03/00  Time:  3:48p
// Checked in $/v2k/src/Volume
// Comment:
//   start/end indices
//
// *****************  Version 12  *****************
// User: Lihong          Date:  8/02/00  Time:  1:16a
// Checked in $/v2k/src/Volume
// Comment:
//   added vector of segment start indices
//
// *****************  Version 11  *****************
// User: Kevin           Date:  7/26/00  Time:  6:57p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added a simple skeleton smooth function
//
// *****************  Version 10  *****************
// User: Frank           Date:  7/25/00  Time: 10:58a
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added a function to scale the skeleton
//
// *****************  Version 9  *****************
// User: Frank           Date:  7/24/00  Time:  4:05p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Removed Draw()
//
// *****************  Version 8  *****************
// User: Ingmar          Date:  7/24/00  Time:  2:11p
// Checked in $/v2k/src/Volume
// Comment:
//   added skeleton and skletonNode classes
//
// *****************  Version 7  *****************
// User: Ingmar          Date:  7/24/00  Time:  9:40a
// Checked in $/v2k/src/Skeleton
// Comment:
//   replaced old array with std::vector
//
// *****************  Version 6  *****************
// User: Ingmar          Date:  7/24/00  Time:  8:41a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added AsOstream
//
// *****************  Version 5  *****************
// User: Ingmar          Date:  7/23/00  Time:  3:38p
// Checked in $/v2k/src/Skeleton
// Comment:
//   moved finding skeleton into Skeletonator, Skeleton now just holde
// the Skeleton Array
//
// *****************  Version 4  *****************
// User: Mie             Date:  7/19/00  Time: 12:02p
// Checked in $/v2k/src/Skeleton
// Comment:
//   deleted argv dependence
//
// *****************  Version 3  *****************
// User: Mie             Date:  7/19/00  Time: 11:06a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added cmd line switch args to skeleton
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/19/00  Time: 10:31a
// Checked in $/v2k/src/Skeleton
// Comment:
//   added CmdLineSwtiches to Skeleton class
//
// *****************  Version 1  *****************
// User: Michael         Date:  7/18/00  Time:  7:16p
// Created Skeleton.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/Skeleton.h,v 1.2 2006/09/13 15:26:20 frank Exp $
// $Id: Skeleton.h,v 1.2 2006/09/13 15:26:20 frank Exp $
