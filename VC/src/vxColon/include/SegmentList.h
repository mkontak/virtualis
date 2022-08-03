// $Id: SegmentList.h,v 1.4.2.1 2009/01/15 04:12:26 jmeade Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.net)


///
/// Description: A class to aid navigation through segments and branches
///


// pragmas
#pragma once


// includes
#include "Segment.h"
#include "Skeleton.h"
#include "Typedef.h"


/// Segment list index, Skeleton node index pair
typedef std::pair<int4,int4> SegmentSkeletonNodeID;

enum ColonSegmentEnum
{
    eRECTUM = 0,
    eSIGMOID = 1,
    eDESCENDING = 2,
    eSPLENIC_FLEXURE = 3,
    eTRANSVERSE = 4,
    eHEPATIC_FLEXURE = 5,
    eASCENDING = 6,
    eCECUM = 7,
};


class SegmentList
{
// Type definitions
public:

  /// List of segment indices and directions defining a segment ordering
  class SegmentOrderVector : public std::vector< std::pair<uint4,bool> >
  {
  public:
    
    /// Input stream operator
    friend std::istream& operator>>(std::istream& is, SegmentOrderVector& v);
    
    /// Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const SegmentOrderVector& v);
  };

// Member Functions
public:

  /// Constructor.
  SegmentList();

  /// Destructor.
  ~SegmentList();

  /// File input.
  void Read(const std::string & sFilePrefix) ;

  /// File input.
  void Read(std::ifstream & inStream);

  /// File output.
  void Write(const std::string & sFilePrefix) const;

  /// Output to a file stream.
  void Write(std::ofstream & outStream) const;

  /// Retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// Retrieves all the data in serialized form.
  void WriteSerializedData(int1 ** piBuffer, const int1 * piBufferEnd) const;

  /// Reads in all the data from serialized form.
  void ReadSerializedData(int1 ** piBuffer, const int1 * piBufferEnd);

  /// Remove all segments.
  void Reset();

  /// Equality operator
  bool operator ==(const SegmentList & other) const;

  /// Retrieve the total colon length (in MM).
  float4 GetTotalLengthMM() const;

  /// Move the current segment pointer to the first segment.
  bool GoToFirstSegment();

  /// Move the current segment pointer to the last segment.
  bool GoToLastSegment();

  /// Move the current segment pointer toward the cecum.
  bool GoToNextSegmentToCecum();

  /// Move the current segment pointer toward the rectum.
  bool GoToNextSegmentToRectum();

  /// Verify that the total colon length is below some value.
  void VerifyTotalLength(const float4 fMinimumColonLengthCm);

  /// Return the last segment in the current selected list.
  Segment & GetLastSelectedSegment();

  /// Return the second-from-last segment in the current selected list.
  Segment & GetLast2SelectedSegment();

  /// Returns the current sorted segment.
  Segment & GetCurrentSegment();

  /// Returns the current sorted segment.
  const Segment & GetCurrentSegment() const;

  /// Returns the current sorted segment skeleton.
  Skeleton & GetCurrentSkeleton();

  /// Returns the skeleton for a given segment index
  Skeleton & GetSkeleton(const uint4 uSortedSegmentIndex);

  /// Returns the skeleton for a given segment index
  const Skeleton & GetSkeleton(const uint4 uSortedSegmentIndex) const;

  /// Return the selected unsorted segment.
  Segment & GetUnsortedSegment(const uint4 uUnsortedSegmentIndex);

  /// Returns the segment ordering and directions
  SegmentOrderVector GetSegmentOrder() const;

  /// Sets the segment ordering and directions
  void SetSegmentOrder(const SegmentOrderVector & vOrder);

  /// Return a sorted segment by index.
  Segment & GetSegment(const uint4 uSortedSegmentIndex);

  /// Return a sorted segment by index.
  const Segment & GetSegment(const uint4 uSortedSegmentIndex) const;

  /// Returns whether or not the segment data is yet loaded from disk.
  bool IsLoaded() const { return (m_bSegmentsLoaded); }

  /// Return the number of selected segments.
  const uint4 GetNumSelectedSegments() const;

  /// Return the number of unsorted segments.
  const uint4 GetNumUnsortedSegments() const;

  /// Add the indicated unsorted segment to the list of sorted segments.
  void AddUnsortedSegmentToSortedSegments(const uint4 uUnsortedSegmentIndex);

  /// Add the indicated new segment to the list of unsorted segments.
  void AddNewSegmentAndSelectIt(const Segment & segment);

  /// Add all the segments to the sorted list.
  void SelectAllSegments();

  /// Unselect all the segments.
  void UnselectAllSegments();

  /// Unselect the tail segment.
  void UnselectTailSegment();

  /// Sets the index of the current sorted segment.
  void SetCurrentSegmentIndex(const int4 iSortedSegmentIndex);

  /// Searches for the sorted segment index of an unsorted segment index.
  int4 GetSortedSegmentIndex(const uint4 uUnsortedSegmentIndex) const;

  /// Searches for the unsorted segment index of a sorted segment index.
  int4 GetUnsortedSegmentIndex(const uint4 uSortedSegmentIndex) const;

  /// Searches for the sorted segment index from a segment identifier.
  int4 GetSortedSegmentIndexFromIdentifier(const uint2 uSegmentIdentifier) const;

  /// Searches for the unsorted segment index from a segment identifier.
  int4 GetUnsortedSegmentIndexFromIdentifier(const uint2 uSegmentIdentifier) const;

  /// Test the serialization functionality.
  bool TestSerialization() const;

  /// Reverse the direction of navigation.
  void ReverseCurrentDirection();

  /// Sets the direction of navigation.
  void SetCurrentDirection(const bool bStartToEnd);

  /// Retrieve the current direction of navigation.
  bool GetCurrentDirectionStartToEnd() const;

  /// Returns whether the current segment is the first selected segment.
  bool IsFirstSegmentCurrent() const;

  /// Returns whether the current segment is the last selected segment.
  bool IsLastSegmentCurrent() const;

  /// Update total length of segments and offsets including non-distended areas.
  void UpdateTotalLength();

  /// Returns the color of the centerline gradient of the given segment and end.
  Triple<float4> GetSegmentColor(const uint4 uSortedSegmentIndex, const bool bStart);

  /// Rotate a prone segment list to supine
  void RotateProneToSupine( const Triple<float4> & worldSize );

// data members
public:
  class UnsortedIterator
  {
  public:
    /// Constructor
    UnsortedIterator(SegmentList& segList) : m_segList(segList), m_iCurrent(0) { }
    /// Returns true if end of list reached
    bool IsAtEnd() { return m_iCurrent >= m_segList.GetNumUnsortedSegments(); }
    /// Returns current index
    operator int4() { return m_iCurrent; }
    /// Returns current
    Segment& operator*() { return m_segList.GetUnsortedSegment(m_iCurrent);  }
    /// Prefix increment operator
    UnsortedIterator operator++(int) { UnsortedIterator itPrefix(*this); m_iCurrent++; return itPrefix; }
    /// Postfix increment operator
    UnsortedIterator& operator++() { m_iCurrent++; return *this; }

  private:
    /// The segment list
    SegmentList& m_segList;
    /// Current segment
    int4 m_iCurrent;
  };

  class SelectedIterator
  {
  public:
    /// Constructor
    SelectedIterator(SegmentList& segList) : m_segList(segList), m_iCurrent(0) { }
    /// Returns true if end of list reached
    bool IsAtEnd() { return m_iCurrent >= m_segList.GetNumSelectedSegments(); }
    /// Returns current index
    operator int4() { return m_iCurrent; }
    /// Returns current
    Segment& operator*() { return m_segList.GetSegment(m_iCurrent);  }
    /// Prefix increment operator
    SelectedIterator operator++(int) { SelectedIterator itPrefix(*this); m_iCurrent++; return itPrefix; }
    /// Postfix increment operator
    SelectedIterator& operator++() { m_iCurrent++; return *this; }

  private:
    /// The segment list
    SegmentList& m_segList;
    /// Current segment
    int4 m_iCurrent;
  };

private:

  /// List of all segments in unsorted order.
  std::vector<Segment> m_vAllSegments;

  /// List of indices of sorted segments.
  std::vector<uint4> m_vSortedSegments;

  /// The current direction.
  bool m_bCurrentStartToEnd;

  /// The current segment identifier.
  int4 m_iCurrentSortedSegmentIndex;

  /// Whether or not the segments have been loaded from disk.
  bool m_bSegmentsLoaded;

  /// A null segment used for error conditions.
  static Segment m_nullSegment;

  /// The delimiter used to verify file I/O.
  static const std::string m_sDelimiter;

}; // class SegmentList


// $Log: SegmentList.h,v $
// Revision 1.4.2.1  2009/01/15 04:12:26  jmeade
// constant method variant of GetSegment()
//
// Revision 1.4  2006/09/13 15:24:18  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.3  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.2.4.1  2006/03/29 20:53:20  frank
// read the segment number from the cad findings file
//
// Revision 1.2  2005/08/23 16:32:43  geconomos
// Corrected merge from vx repository.
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.10.4.2.12.1  2005/06/28 12:06:18  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.10.4.2  2003/03/26 23:14:57  jmeade
// Issue 2849:  Const equivalents to a few access methods.
//
// Revision 3.10.4.1  2003/02/21 17:53:26  jmeade
// Selected and unsorted (i.e. all) segment iterator classes to clarify iteration routines.
//
// Revision 3.10  2002/09/13 00:48:14  jmeade
// Segment list serialize function; code standards.
//
// Revision 3.9  2002/09/06 16:10:58  jmeade
// Merged with VC_1-2-1_CS-1 branch, from 07-02-2002 to 09-05-2002.
//
// Revision 3.8  2002/08/15 15:36:52  kevin
// Add ColonSegEnum back in
//
// Revision 3.7  2002/08/14 16:12:09  wenli
// initial version - Vascular
//
// Revision 3.6  2002/07/18 19:57:28  jmeade
// Segment list identifier should be with SegmentList class.
//
// Revision 3.5  2002/07/15 15:52:42  jmeade
// typedef for <Segment Index, Segment node index> pair
//
// Revision 3.4  2002/07/03 03:23:21  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.3.2.2  2002/08/06 20:44:47  jmeade
// Issue 2596:  Ensure when you set segment order that an ordered segment is selected as current.
//
// Revision 3.3.2.1  2002/07/02 20:50:13  jmeade
// Issue 2062:  Get/Set of segment ordering/directions.
//
// Revision 3.3  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
// Revision 3.2  2001/11/05 18:48:11  ana
// GoThroughSegments moved to Study.
//
// Revision 3.1  2001/11/05 17:53:45  ana
// GoThroughSegments method added in order to extract registration input data.
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.39  2001/07/18 21:24:12  binli
// not to include "v3dFile.h"
//
// Revision 1.38  2001/07/18 20:55:06  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.37  2001/05/04 15:32:17  binli
// reflected works of refering triangle display list.
//
// Revision 1.36  2001/04/30 12:29:45  frank
// Added exception handling.
//
// Revision 1.35  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.34  2001/04/10 13:14:37  frank
// Added comments and variable name changes.
//
// Revision 1.33  2001/04/06 13:54:19  frank
// Removed some pointers.
//
// Revision 1.32  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.31  2001/03/28 13:57:00  frank
// Worked on coding compliance and eliminating pointers.
//
// Revision 1.30  2001/03/20 12:49:37  frank
// Updated to reflect new coding standards.
//
// Revision 1.29  2001/02/27 22:01:59  jmeade
// SelectAllSegments() function
//
// Revision 1.28  2001/02/07 15:23:46  frank
// Rendered centerline in continuous color gradient.
//
// Revision 1.27  2001/02/07 13:45:49  frank
// Merged the UpdateOffset and UpdateTotalLength functions.
//
// Revision 1.26  2001/02/06 21:43:34  jeff
// clarified segment jump operations
//
// Revision 1.25  2001/01/31 22:17:45  geconomos
// Cache total length of sorted segments. Added UpdateTotalLength()
// and changed GetTotalLength() to return cached value.
//
// Revision 1.24  2001/01/29 20:02:52  frank
// Added a function to update the offsets.
//
// Revision 1.23  2001/01/29 18:16:26  frank
// Removed error suppression.
//
// Revision 1.22  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.21  2001/01/23 14:12:34  frank
// Added functions to query the current segment index.
//
// Revision 1.20  2001/01/05 14:56:47  frank
// Added access functions for direction control.
//
// Revision 1.19  2000/12/22 14:56:06  frank
// Removed spurious warnings
//
// Revision 1.18  2000/12/18 19:51:53  frank
// Removed stringstream I/O
//
// Revision 1.17  2000/12/14 14:52:18  frank
// Modified the interface for serialization/deserialization
// Added a serialization test function
//
// Revision 1.16  2000/12/13 15:06:45  frank
// Added equality operators
//
// Revision 1.15  2000/12/13 13:45:20  frank
// Added equality operator
//
// Revision 1.14  2000/12/09 18:17:25  frank
// Added serialized input
//
// Revision 1.13  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.12  2000/12/09 15:38:51  frank
// Updated segment reading for new file format
//
// Revision 1.11  2000/12/09 12:54:50  frank
// Improved stream I/O
//
// Revision 1.10  2000/12/05 19:58:28  frank
// Added stream I/O functions
//
// Revision 1.9  2000/11/25 03:04:44  kevin
// Fixed multi-segment patch jumping
//
// Revision 1.8  2000/11/13 18:26:35  frank
// Added function to search for segment based on identifier.
//
// Revision 1.7  2000/11/10 15:21:49  frank
// Added jumping to new segments in overview window
//
// Revision 1.6  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.5.6.3  2000/10/30 17:05:58  frank
// merged back using old v2k branch
//
// Revision 1.5.6.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.5.6.1  2000/10/28 17:38:58  ingmar
// new ierators for new segments
//
// Revision 1.5.2.2  2000/10/27 15:20:54  frank
// Got segment structure working
//
// Revision 1.5.2.1  2000/10/26 13:28:32  frank
// Added new segment structure
//
// Revision 1.5  2000/10/16 08:29:18  antonio
// resolved most warnings
//
// Revision 1.4  2000/10/12 02:45:46  kevin
// FIxed OffsetDistanceFromRectum and added access function to
// get segments at or near the end of the list
//
// Revision 1.3  2000/10/11 14:48:31  kevin
// Added jump to next segment prev segment, and changed
// some of the index variables to be in here
//
// Revision 1.2.2.1  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.2  2000/10/02 21:59:12  jeff
// Added IsLoaded() query to SegmentList class
//
// Revision 1.1  2000/09/27 15:07:52  frank
// Added segment data structures
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
//
// 12    8/28/00 12:35p Antonio
// updated header and footer
//
//*****************  Version 11  *****************
//User: Kevin           Date:  8/20/00  Time: 10:20p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added choosing of flight direction
//
//*****************  Version 10  *****************
//User: Kevin           Date:  8/17/00  Time:  7:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved tri strips to the segmnet data structure so that the correlate
//with the skeleton indexes for picking
//
//*****************  Version 9  *****************
//User: Frank           Date:  8/08/00  Time:  2:23p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Broke Segment class out into it's own class
//
//*****************  Version 8  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 7  *****************
//User: Kevin           Date:  8/02/00  Time:  5:51p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added some multi segment paraaaameters
//
//*****************  Version 6  *****************
//User: Binli           Date:  8/02/00  Time: 10:48a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 5  *****************
//User: Kevin           Date:  8/01/00  Time: 10:05p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added num segments to inf file
//
//*****************  Version 4  *****************
//User: Frank           Date:  8/01/00  Time:  4:08p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added bidirectional potential fields
//
//*****************  Version 3  *****************
//User: Frank           Date:  8/01/00  Time:  9:54a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added file I/O
//
//*****************  Version 2  *****************
//User: Frank           Date:  7/31/00  Time:  4:32p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Work in progress
//
//*****************  Version 1  *****************
//User: Frank           Date:  7/31/00  Time:  4:24p
//Created Segments.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/SegmentList.h,v 1.4.2.1 2009/01/15 04:12:26 jmeade Exp $
// $Id: SegmentList.h,v 1.4.2.1 2009/01/15 04:12:26 jmeade Exp $
