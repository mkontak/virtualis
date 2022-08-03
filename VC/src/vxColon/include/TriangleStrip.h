// $Id: TriangleStrip.h,v 1.2 2006/09/13 15:33:16 frank Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Bin Li (binli@viatronix.net)

/**
 * Description: This class is reponsible for:
 *     1> read/write triangle strips description from/to file.
 *        (the triangle strips is stored in a simplified WaveFront's object format)
 *     2> render triangle strips. 
 */


// pragmas
#pragma once


// includes
#include "Triple.h"
#include "Point.h"
#include "Matrix.h"


class TriangleStrip 
{
// member functions
public:

  // Constructor/Destructor

  /// Default constructor.
  TriangleStrip();

  /// Copy constructor.
  TriangleStrip(const TriangleStrip & other);

  /// Assignment operator.
  TriangleStrip & operator =(const TriangleStrip & other);

  /// Destructor.
  virtual ~TriangleStrip();

  /// Display Strips.
  void DisplayStrips() const;

  /// Display strips with transparency.
  void DisplayStripsTTP(const Matrix<float4> & mCurrMatrix);
  
  /// Clear all the data.
  void Clear();

  //**********************************************************************//
  // Some structures, Face & Strip,  defined for storing polygon data set //
  //**********************************************************************//
  
  /// Class Face: a class that stores the one strip information
  class Face
  {
  public:

    /// default constructor
    Face();

    /// copy constructor
    Face(const Face & other);

    /// Assignment operator.
    Face & operator =(const Face & other);

    /// Destructor.
    ~Face();

    /// Read face from file.
    bool Read(std::ifstream & in);

    /// Write face to file.
    bool Write(std::ofstream & out) const;

    /// Generalized read face from stream.
    bool Read(std::istream & in);

    /// Retrieves the number of bytes of the serialized data.
    uint4 GetSerializedDataSize() const;
    
    /// Retrieves all the data in serialized form.
    bool WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const;

    /// Reads in all the data from serialized form.
    bool ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

    /// Equality operator.
    bool operator ==(const Face & other) const;

    /// Convert indices into much faster pointers
    void ConvertIndicesIntoPointers(Point<float4> * pvVertices, Vector<float4> * pvNormals);

  public:

    /// label a triangle: 0: new tri_strip; 1: continue a strip; 2: just a face
    int4 m_iTriStrip;

    /// number of faces
    int4 m_iNumOfFaces;

    /// pointers to the object(vertex/normal).
    Triple<float4> ** m_hIndex;

    /// indices to the vertices/normals
    uint4 * m_piVertexIndex;

  }; // class Face

  /// class Strip: a structure to store information about triangle strips of a segment.
  class Strip
  {
  public:

    /// Constructor.
    Strip();

    /// Copy constructor.
    Strip(const Strip & other);

    /// Assignment constructor.
    Strip & operator =(const Strip & other);

    /// Destructor.
    ~Strip();

    /// Read strip from disk.
    bool Read(std::ifstream & in);

    /// Write strip to disk.
    bool Write(std::ofstream & out) const;

    /// Retrieves the number of bytes of the serialized data.
    uint4 GetSerializedDataSize() const;
    
    /// Retrieves all the data in serialized form.
    bool WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const;

    /// Reads in all the data from serialized form.
    bool ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

    /// Equality operator.
    bool operator ==(const Strip & other) const;

    /// Verify that all the vertices are inside the active volume.
    bool Verify(const Triple<uint4> vVolumeDimension) const;

    /// Rotate a prone Strip to supine
    void RotateProneToSupine( const Triple<float4> & worldSize );

  public:

    /// number of strips/triangles in this segment.
    int4 m_iFacesNo;

    /// number of vertices in the buffer.
    int4 m_iNumOfVertices;

    /// number of normals in the buffer.
    int4 m_iNumOfNormals;

    /// face buffer.
    Face * m_pFaces;

    /// vertices buffer.
    Point<float4> * m_pVertices;

    /// normals buffer.
    Vector<float4> * m_pNormals;

  }; // class Strip

// Primary functions
//-------------------

  /// Read triangle strips information.
  bool ReadTriangleStripDataSet(const std::string & sFilename);

  /// Remove the .tri files from the disk.
  bool RemoveTriangleFile(const std::string & sFilename);

  /// Render these triangle strips.
  void RenderSegmentTTP(const Matrix<float4> & mCurrMatrix);

  /// Render segments (color).
  void RenderSegment(const Triple<uint1> vColor) const;

  /// Render segments (painting)
  void RenderSegment(uint1 * puPaintVolume, const uint1 uCurrentSeenVal,
                     const Triple<uint4> & vuStep, const Point<float4> & vfWorld2vol);

  /// Read triangle strips from disk.
  bool Read(std::ifstream & in);

  /// Write triangle strips to disk.
  bool Write(std::ofstream & out) const;

  /// Retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// Retrieves all the data in serialized form.
  bool WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const;

  /// Reads in all the data from serialized form.
  bool ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// Equality operator.
  bool operator ==(const TriangleStrip & other) const;

  /// Verify that all the vertices are inside the active volume.
  bool Verify(const Triple<uint4> vVolumeDimension) const;

  /// Rotate a prone TriangleStrip to supine
  void RotateProneToSupine( const Triple<float4> & worldSize );

// data members
private:

  /// this structure store the triangle strips information of all segments
  Strip * m_pStrip;

}; // TriangleStrip Class


// $Log: TriangleStrip.h,v $
// Revision 1.2  2006/09/13 15:33:16  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.2.16.1  2005/04/06 15:31:01  frank
// formatting
//
// Revision 3.2  2002/05/07 18:26:44  frank
// Cleaned up includes.
//
// Revision 3.1  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:28  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:48   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.35  2001/05/16 15:00:03  binli
// code conventions.
//
// Revision 1.34  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.33  2001/04/05 15:23:11  binli
// name convension
//
// Revision 1.32  2001/04/04 12:04:55  frank
// Further conformed to variable naming conevention.
//
// Revision 1.31  2001/04/03 21:13:39  binli
// 1st code walkthrough
//
// Revision 1.30  2001/04/03 17:08:00  binli
// added comments.
//
// Revision 1.29  2001/03/30 15:01:02  binli
// m_iNumOfFaces: long->int
//
// Revision 1.28  2001/03/29 16:18:21  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.27  2001/03/27 15:12:30  binli
// reflected Typedef.h
//
// Revision 1.26  2001/03/21 13:00:37  frank
// Moved SEEN and NOTSEEN colors out of #define into coding compliant Triple<uint1>
//
// Revision 1.25  2001/03/16 22:13:53  binli
// renamed some variables:  according to new code standard
// brackets: according to new code standard
//
// Revision 1.24  2001/03/07 17:30:03  binli
// removed the .tri file after skeletonor processing.
//
// Revision 1.23  2001/01/18 16:10:06  kevin
// Made transparent polygon render be polygon-TTP
//
// Revision 1.22  2000/12/19 17:08:31  sarang
// gcc/linux fixes
//
// Revision 1.21  2000/12/18 23:03:36  kevin
// Fixed paint vooume positional indexes since triangle verticies are now in world coordinates
//
// Revision 1.20  2000/12/18 19:51:52  frank
// Removed stringstream I/O
//
// Revision 1.19  2000/12/18 14:55:57  frank
// Added some simple verification
//
// Revision 1.18  2000/12/18 13:55:02  frank
// Cleared up paint volume size discrepancy
//
// Revision 1.17  2000/12/14 14:53:00  frank
// Modified the interface for serialization/deserialization
//
// Revision 1.16  2000/12/13 15:06:45  frank
// Added equality operators
//
// Revision 1.15  2000/12/13 13:45:20  frank
// Added equality operator
//
// Revision 1.14  2000/12/09 17:49:13  frank
// Added serialized output
//
// Revision 1.13  2000/12/09 15:38:51  frank
// Updated segment reading for new file format
//
// Revision 1.12  2000/12/09 14:43:54  frank
// Stream I/O was too slow, went to temp binary file
//
// Revision 1.11  2000/12/09 12:54:14  frank
// Improved stream I/O
//
// Revision 1.10  2000/12/05 14:45:43  frank
// Added generalized stream I/O
//
// Revision 1.9  2000/11/20 03:32:28  kevin
// Switched to "paint volume" schema.
//
// Revision 1.8  2000/11/09 19:56:48  binli
// added function 'displaystrips' (the function is extracted from RenderSegment).
// The purpose of this change is for call list and for Supine/Prone works.
//
// Revision 1.7  2000/11/03 12:59:40  frank
// Fixed triangle file path
//
// Revision 1.6  2000/11/02 17:34:34  kevin
// Added drawing with pointers into RGB vol for painting
//
// Revision 1.5  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.4.12.4  2000/10/30 19:20:27  frank
// fixed copy constructor
//
// Revision 1.4.12.3  2000/10/30 17:05:58  frank
// merged back using old v2k branch
//
// Revision 1.4.12.2  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.4.12.1  2000/10/28 17:38:59  ingmar
// new ierators for new segments
//
// Revision 1.4.8.2  2000/10/27 15:20:54  frank
// Got segment structure working
//
// Revision 1.4.8.1  2000/10/26 13:28:32  frank
// Added new segment structure
//
// Revision 1.4.2.2  2000/10/26 14:30:27  frank
// Added C++
//
// Revision 1.4.2.1  2000/10/26 11:16:39  frank
// Wrote out new segment structure
//
// Revision 1.4  2000/10/02 16:10:01  frank
// Improved SegmentList I/O
//
// Revision 1.3  2000/09/30 15:01:33  frank
// Added disk I/O
//
// Revision 1.2  2000/09/28 17:10:00  frank
// Added I/O for segments and segment lists
//
// Revision 1.1  2000/09/27 15:07:52  frank
// Added segment data structures
//
// Revision 1.2  2000/09/13 15:55:32  binli
// fitted to the Viatronix C++ coding conventions and standards
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
//
// 16    8/29/00 2:55p Binli
//
// 15    8/29/00 2:42p Binli
// adjust the data structure: struct --> class
//
// 14    8/28/00 12:51p Antonio
// updated header and footer
//
//*****************  Version 13  *****************
//User: Binli           Date:  8/22/00  Time: 10:07a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 12  *****************
//User: Binli           Date:  8/18/00  Time:  4:43p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 11  *****************
//User: Kevin           Date:  8/17/00  Time:  7:09p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Moved tri strips to the segmnet data structure so that the correlate
//with the skeleton indexes for picking
//
//*****************  Version 10  *****************
//User: Binli           Date:  8/10/00  Time:  3:55p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 9  *****************
//User: Kevin           Date:  8/06/00  Time:  7:02p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added Picking of segments for order. To accomplish this, I
//pulled the guts out of some data structures and replaced them
//
//*****************  Version 8  *****************
//User: Binli           Date:  8/02/00  Time: 10:48a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 7  *****************
//User: Kevin           Date:  8/02/00  Time:  2:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Begining of multi-segmnet handling
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/02/00  Time:  1:33a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Aligned centerline with polygons and cleaned up overview a little
//
//*****************  Version 5  *****************
//User: Binli           Date:  8/01/00  Time:  3:44p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 4  *****************
//User: Binli           Date:  7/31/00  Time:  9:35a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 3  *****************
//User: Binli           Date:  7/31/00  Time:  9:32a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 2  *****************
//User: Binli           Date:  7/28/00  Time:  1:05p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 1  *****************
//User: Binli           Date:  7/28/00  Time: 10:35a
//Created TriangleStrip.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/TriangleStrip.h,v 1.2 2006/09/13 15:33:16 frank Exp $
// $Id: TriangleStrip.h,v 1.2 2006/09/13 15:33:16 frank Exp $
