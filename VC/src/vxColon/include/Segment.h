// $Id: Segment.h,v 1.2.2.1 2010/11/15 20:37:39 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.net)


/**
 * A class to store all the information related to a segment
 */


// pragmas
#pragma once


// includes
#include "TriangleStrip.h"
#include "Skeleton.h"


class Segment
{
// member functions
public:

  /// default constructor
  Segment();

  /// destructor
  ~Segment();

  /// copy constructor
  Segment( const Segment & other );

  /// assignment operator
  Segment & operator =( const Segment & other );


  // Input / Output
  // --------------

  /// Read from disk.
  void Read(std::ifstream & in);

  /// Write to disk.
  void Write(std::ofstream & out) const;

  /// Retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// Retrieves all the data in serialized form.
  void WriteSerializedData(int1 ** piBuffer, const int1 * piBufferEnd) const;

  /// Reads in all the data from serialized form.
  void ReadSerializedData(int1 ** piBuffer, const int1 * piBufferEnd);


  // Access Functions
  // ----------------

  /// Retrieves the distance from the rectum of the start of the segment.
  inline float4 GetOffsetDistanceFromRectumMM() const { return m_fOffsetDistanceFromRectumMM; }
  
  /// Retrieves the Euclidean length of this segment.
  inline float4 GetEuclideanLengthMM() const { return m_fLengthMM; }
  
  /// Retrieves the identifier of this segment.
  inline uint4 GetIdentifier() const { return m_uIdentifier; }
  
  /// Retrieves the startpoint of the segment centerline.
  inline const Point<float4> & GetStartPointMM() const { return m_vStartpointMM; }
  
  /// Retrieves the endpoint of the segment centerline.
  inline const Point<float4> & GetEndPointMM() const { return m_vEndpointMM; }
  
  /// Select the preferred direction of travel within this segment.
  inline void SelectDirection(const bool bRegularDirection);
  
  /// Returns if this segment is oriented in the regular direction (from start to end of centerline).
  inline bool IsRegularDirection() const { return m_bRegularDirection; }
  
  /// Returns if this segment is selected as part of the "selected colon segments".
  inline bool IsSelected() const { return m_bSelected; }
  
  /// Changes whether this segment is selected as part of the "selected colon segments".
  inline void SetSelected(const bool bSelected) { m_bSelected = bSelected; }
  
  /// Retrieves a given skeleton from the list of skeletons
  inline Skeleton & GetCenterline(const uint4 & uIndex) { return m_vCenterlines[uIndex]; }
  
  /// Returns the number of centerlines.
  inline uint4 GetNumCenterlines() const { return m_vCenterlines.size(); }
  
  /// Returns a reference to the triangle strips data.
  inline TriangleStrip & GetTriangleStrip() { return m_triangleStrip; }
  
  /// Returns a reference to the current skeleton.
  inline Skeleton & GetCurrentSkeleton();

  /// Returns a const reference to the current skeleton.
  inline const Skeleton & GetCurrentSkeleton() const;
  
	/// The m_fVolumeCC was not recorded in the Serialzed data. The rationals are:
	/// 1> The computation of volume needs volume files. The volume files are not available in Segment structure
	/// 2> Backward compatibility concern for old .seg file.

	/// get volume measurement
	inline float4 GetAirVolume  () { return m_fAirVolumeCC; }
	inline float4 GetTotalVolume() { return m_fTotalVolumeCC; }
	inline double GetAverageTaggedHU() { return m_fAverageTaggedHU; }
	inline uint4 GetTaggedVoxelCount() { return m_iTaggedVoxelCount; }

	/// set volume measurement
	inline void SetAirVolume  ( const float4 & fVolumeCC ) { m_fAirVolumeCC = fVolumeCC; }
	inline void SetTotalVolume( const float4 & fVolumeCC ) { m_fTotalVolumeCC = fVolumeCC; }
	inline void SetAverageTaggedHU( const double & fAverageHU ) { m_fAverageTaggedHU = fAverageHU; }
	inline void SetTaggedVoxelCount( const uint4 & iVoxelCount ) { m_iTaggedVoxelCount = iVoxelCount; }

  // Primary Functions
  // -----------------

  /// Initializes a segment based on centerlines and triangle strip data.
  bool Initialize(const uint4 uSegmentNumber, std::vector<Skeleton> & vSkeletons, const std::string & sFilenameRoot);

  /// Retrieve first skeleton node of the centerline.
  const SkeletonNode & GetBeginningSkelNode();

  /// Retrieve last skeleton node of the centerline.
  const SkeletonNode & GetEndingSkelNode();

  /// Recalculate the Euclidean length of the segment.
  void UpdateLength();

  /// Update the distance from rectum including distended areas.
  void UpdateOffsetDistanceFromRectum(const Segment & prevSegment);

  /// Initialize the distance from rectum to zero (use this for the first segment).
  void SetZeroDistanceFromRectum();

  /// Adds a centerline to the list of centerlines.
  void AddCenterline(const Skeleton & centerline);
  
  /// Equality operator.
  bool operator ==(const Segment & other) const;

  /// Rotate a prone segment to supine
  void RotateProneToSupine( const Triple<float4> & worldSize );

// data members
private:

  /// Identifier of each segment.
  uint4 m_uIdentifier;

  /// The direction of the segment (true = start to end).
  bool m_bRegularDirection;

  /// Length of the current selected skelton for this segment (in MM).
  float4 m_fLengthMM;

	/// the volume of the lumen in cm^3 = cc
	float4 m_fAirVolumeCC;      // pure air colon lumen
	float4 m_fTotalVolumeCC;    // pure air colon lumen plus lumen covered by fecal residual
	double  m_fAverageTaggedHU;
	uint4  m_iTaggedVoxelCount;

  /// The adjusted distance from Rectum (eg, the sum of lengths of the semgents up to this point) in MM.
  float4 m_fOffsetDistanceFromRectumMM;

  /// Flag if this segment is part of the "selected colon segments".
  bool m_bSelected;

  /// The polygonal representation of this segment as triangle strips.
  TriangleStrip m_triangleStrip;

  /// List of skeletons.
  std::vector<Skeleton> m_vCenterlines;

  /// List of endpoints.
  std::vector< Point<float4> > m_vEndpointsMM;

  /// The startpoint for this segment.
  Point<float4> m_vStartpointMM;
  
  /// The endpoint for this segment.
  Point<float4> m_vEndpointMM;

  /// The offset between the csi volume and the skeleton index.
  uint4 m_uCsiOffset;

  /// The filenames of the PDR fields corresponding to the endpoints.
  std::vector<std::string> m_vPdrFilenames;
  // There should be one PDR for each endpoint
  
  /// The filenames of the CSI fields corresponding to the endpoints.
  std::vector<std::string> m_vCsiFilenames;
  // There should be one CSI for each centerline
  
  /// A null centerline reserved for graceful error handling.
  static Skeleton m_nullCenterline;

  /// The delimiter used to verify the beginning of file I/O.
  static const std::string m_sDelimiter;

}; // class Segment


// inlines
#include "Segment.inl"


// $Log: Segment.h,v $
// Revision 1.2.2.1  2010/11/15 20:37:39  dongqing
// add codes for measuring air lumen volume of selected segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2  2006/09/13 15:23:40  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.4.4.1.12.1  2005/06/28 12:06:18  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.4.4.1  2003/03/26 23:14:56  jmeade
// Issue 2849:  Const equivalents to a few access methods.
//
// Revision 3.4  2002/08/14 16:12:09  wenli
// initial version - Vascular
//
// Revision 3.3  2002/07/03 03:23:43  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.2  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 3.1.2.1  2002/07/02 20:51:51  jmeade
// Code standards.
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
//    Rev 2.0   Sep 16 2001 23:49:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:42   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.36  2001/05/04 15:31:14  binli
// moved the displaylist of triangle to OverviewRender.
//
// Revision 1.35  2001/04/30 12:29:45  frank
// Added exception handling.
//
// Revision 1.34  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.33  2001/04/06 13:54:01  frank
// Removed all pointers.
//
// Revision 1.32  2001/04/06 12:15:06  frank
// Changes according to code walkthrough.
//
// Revision 1.31  2001/04/04 14:32:58  frank
// More comments.
//
// Revision 1.30  2001/03/30 18:48:23  dmitry
// Updated datatypes.
//
// Revision 1.29  2001/03/29 16:18:22  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.28  2001/03/28 13:56:25  frank
// Added function to add a centerline to the segment.
//
// Revision 1.27  2001/03/26 16:26:46  frank
// Added function to initialize a segment.
//
// Revision 1.26  2001/03/26 16:01:07  frank
// Hid member variables from prying eyes.
//
// Revision 1.25  2001/03/26 15:31:38  frank
// Working on coding compliance.
//
// Revision 1.24  2001/03/19 17:53:34  frank
// Updated to reflect new coding standards.
//
// Revision 1.23  2001/02/22 01:34:59  jeff
// code without coding conventions is gettting harder and harder to comprehend
//
// Revision 1.22  2001/01/27 00:06:47  dave
// moved some comments
//
// Revision 1.21  2001/01/19 21:07:09  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.20  2000/12/18 19:51:53  frank
// Removed stringstream I/O
//
// Revision 1.19  2000/12/14 14:51:46  frank
// Modified the interface for serialization/deserialization
//
// Revision 1.18  2000/12/13 15:06:44  frank
// Added equality operators
//
// Revision 1.17  2000/12/13 13:45:20  frank
// Added equality operator
//
// Revision 1.16  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.15  2000/12/09 15:38:51  frank
// Updated segment reading for new file format
//
// Revision 1.14  2000/12/09 14:43:54  frank
// Stream I/O was too slow, went to temp binary file
//
// Revision 1.13  2000/12/09 12:55:14  frank
// Improved stream I/O
//
// Revision 1.12  2000/12/05 19:58:28  frank
// Added stream I/O functions
//
// Revision 1.11  2000/11/22 18:39:03  binli
// added function for display list delete.
//
// Revision 1.10  2000/11/22 17:14:59  binli
// fixed display list for strips rendering.
//
// Revision 1.9  2000/11/09 14:54:16  frank
// Renamed function
//
// Revision 1.8  2000/11/03 12:59:40  frank
// Fixed triangle file path
//
// Revision 1.7  2000/10/31 14:15:07  frank
// Merged in new segment structure
//
// Revision 1.6  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.5.8.3  2000/10/30 17:05:57  frank
// merged back using old v2k branch
//
// Revision 1.5.8.2  2000/10/28 19:31:06  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.5.8.1  2000/10/28 17:38:58  ingmar
// new ierators for new segments
//
// Revision 1.5.4.1  2000/10/26 13:28:31  frank
// Added new segment structure
//
// Revision 1.5  2000/10/12 02:45:05  kevin
// FIxed OffsetDistanceFromRectum
//
// Revision 1.4  2000/10/11 14:47:02  kevin
// Added jump to begining/end of skeleton
//
// Revision 1.3  2000/10/04 14:19:04  kevin
// Added Endpoint picking (so far simple for no
// branches case untill skel is redone)
//
// Revision 1.2.2.2  2000/10/26 14:30:06  frank
// Added new segment structure
//
// Revision 1.2.2.1  2000/10/26 11:16:38  frank
// Wrote out new segment structure
//
// Revision 1.2  2000/09/28 17:10:00  frank
// Added I/O for segments and segment lists
//
// Revision 1.1  2000/09/27 15:07:52  frank
// Added segment data structures
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 7     9/01/00 4:55p Frank
// Worked on unwrapping the colon wall
//
// 6     8/28/00 12:33p Antonio
// updated header and footer
//
//*****************  Version 5  *****************
//User: Kevin           Date:  8/21/00  Time:  3:41p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added segment length checking
//
//*****************  Version 4  *****************
//User: Kevin           Date:  8/20/00  Time: 10:20p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added choosing of flight direction
//
//*****************  Version 3  *****************
//User: Kevin           Date:  8/17/00  Time:  7:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved tri strips to the segmnet data structure so that the correlate
//with the skeleton indexes for picking
//
//*****************  Version 2  *****************
//User: Frank           Date:  8/08/00  Time:  2:23p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Broke Segment class out into it's own class
//
//*****************  Version 1  *****************
//User: Frank           Date:  8/08/00  Time:  2:16p
//Created Segment.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Segment.h,v 1.2.2.1 2010/11/15 20:37:39 dongqing Exp $
// $Id: Segment.h,v 1.2.2.1 2010/11/15 20:37:39 dongqing Exp $
