// $Id: BlockVolume.h,v 1.4 2006/02/09 14:01:50 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: Sept. 00
// Author: David dave@cs.sunysb.edu
//
// Complete history at bottom of file.

#ifndef BlockVolume_h
#define BlockVolume_h

// includes
#include <minilzo.h>
#include <zlib.h>

#include "VolAPI.h"
#include "vxInflate.h"
#include "Point.h"
#include "BlockVector.h"
#include "V3DFile.h"
#include "vxInflate.h"
#include "Deflate.h"
#include "Block.h"
#include "Box.h"
#include "BoundingRect.h"
#include "SerializedBlockVolume.h"
#include "MemoryBuffer.h"

#define FILE_REVISION "$Revision: 1.4 $"

/**
  * Implements the VolAPI interface using blocks.
  * $Id: BlockVolume.h,v 1.4 2006/02/09 14:01:50 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
  */
template <class T> class BlockVolume : public hdr::VolAPI<T>
{
public:
  /// Default constructor for an empty BlockVolume.
  BlockVolume(const uint4 & blockBitSize = 3);
  /// Constructor for building new volumes.
  BlockVolume(const Triple<uint4> & dim, const uint4 & uiBitsPerVoxel = 8*sizeof(T),
              const Triple<float4> & units = Triple<float4>(1.0,1.0,1.0),
              hdr::UnitTypeEnum unitType = hdr::MILLIMETER,
              const T & background = 0, hdr::ExtensionEnum ext= hdr::VOL,
              const std::string & sFileName = "Blank.vol",
              const std::string & sTitle    = "Blank",
              const std::string & sAuthor   = "Blank",
              const vx::Date & date = vx::Date(),
              const uint4 & uiBlockBitSize = 3);
  /// Copy Constructor
  BlockVolume(BlockVolume<T> const & rhs);
  /// Assignment operator
  BlockVolume const & operator=(BlockVolume<T> const & rhs);
private:
  /// Copies all Blockvolume data.
  void RefCountCopy(BlockVolume<T> const & rhs);

public:
  /// Destructor
  ~BlockVolume();
  /// Constructor for converting layout types, e.g. LINEAR -> HASH.
  template <class L>
  BlockVolume(const L & other, bool dummy)
  : hdr::VolAPI<T>(other.GetDim(), other.GetBitsPerVoxel(),
                   other.GetUnits(), other.GetUnitType(),
                   other.GetBackground(), other.GetExtension(),
                   other.GetFileName(), other.GetTitle(),
                   other.GetAuthor(), other.GetCreationDate())
  {
    // Copy VolAPI data.
    Copy(other);

    m_uiBlockBitSize = 3;

    InitMemberVars();

    Point<uint2> pos;
    for ( pos.m_z = 0; pos.m_z < m_dimension.m_z; ++pos.m_z )
    {
      for ( pos.m_y = 0; pos.m_y < m_dimension.m_y; ++pos.m_y )
      {
        for ( pos.m_x = 0; pos.m_x < m_dimension.m_x; ++pos.m_x )
        {

          SetVoxelRaw(pos, other.GetVoxelRaw(pos));
        }
      }
    }
		m_eMemoryLayout = hdr::BLOCK;
  }
  
  ///Merge to volumes. Meaning, after merging the equal blocks are shared
  void Merge(BlockVolume<T> const & otherVolume);
  ///Creates a difference volume of two volumens
  BlockVolume<T> *CreateDiffVol(BlockVolume<T> const & rhs);
  /// Reads the volume specified into memory using the new V3D file format, ALWAYS check the return value.
  void Read(std::string sFileName); 
  /// Writes the volume to the specified file.

  void Write(std::string sFileName = "NULL");

protected:
  /// Serializes block volume in memory so that it could be written to disk
  void Serialize(SerializedBlockVolume&);

  /// Writes the volume to the specified file using 1.1 format (with serialization)
  void WriteV2(std::string sFileName);

  /// Writes the volume to the specified file using 1.0 format
  void WriteV1(std::string sFileName);

public:  
  inline void Resize(const Triple<uint4> & dimension);
  /// Set the value at the location specified.
  inline void SetVoxelRaw(const Point<uint2> & location, T value);
  /// Gets the voxel at the specified location.
  inline const T & GetVoxelRaw(const Point<uint2> & location) const;

  /// Computes and returns the resampled value at the location specified.
  float4 GetSample(const Point<float4> & point) const;
    
  /// Returns the total memory usage in bytes.
  const uint4 GetMemoryUsage() const;

  /// Reports relevant volume layout statistics
  void Stats(std::ostream & os = std::cout) const;

  /// Test function to verify and demonstrate the class functionality.
  static bool Test      (std::ostream & os = std::cout);
  static bool TestByType(std::ostream & os = std::cout);

  // could implement with template param. of BlockVolume to call it's functions
  // i.e. GetDim() or tolerate some code replication and leave out extra temp. arg.
  // reason # 7553 why VC++ sucks, can't handle temp. and inner classes
#undef FILE_REVISION
#include "BlockVolIterator.h" // must define functions in the .h


  VolIterator GetVolIterator(const Point<uint2> & startPoint)
  {
    if ( !InsideVolume(startPoint) )
    {
//      int4 addInvalidInputException;
    }
    return( VolIterator(startPoint, m_dimension, m_background, m_mBlocks, 
                        m_uiBlockBitsX, m_uiBlockBitsXY) );
  }

#include "BlockVolRayIterator.h"
  
  RayIterator GetRayIterator(const Point<float4> startPoint, 
                                            const Vector<float> & directionVector, 
                                            const float4 fSampleDistance)
  {
    return(RayIterator(startPoint,  m_dimension, m_background, m_mBlocks, 
                        m_uiBlockBitsX, m_uiBlockBitsXY,  directionVector, fSampleDistance));
  }

#include "BlockSubVolIterator.h"
  SubVolIterator GetSubVolIterator(const uint2 uBoundary = 0)
  {
    return( SubVolIterator(m_dimension, m_background, m_mBlocks, 
			m_uiBlockBitsX, m_uiBlockBitsXY, uBoundary) );
  }
	
  SubVolIterator GetSubVolIterator(const Box<uint4>& boundingBox)
  {
    return( SubVolIterator(m_dimension, m_background, m_mBlocks, 
			m_uiBlockBitsX, m_uiBlockBitsXY, boundingBox) );
  }

#include "BlockVolSliceIterator.h"
  XSliceIterator GetXSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_x);

    return(XSliceIterator(uIdxSlice, m_dimension, m_background, m_mBlocks, 
                        m_uiBlockBitsX, m_uiBlockBitsXY, uBoundary));

  }

  XSliceIterator GetXSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_x);


    return(XSliceIterator(uIdxSlice, m_dimension, m_background, m_mBlocks, 
			m_uiBlockBitsX, m_uiBlockBitsXY, boundingBox));
  }
	
  YSliceIterator GetYSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)

  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_y );
	
		return(YSliceIterator(uIdxSlice, m_dimension, m_background, m_mBlocks, 
                        m_uiBlockBitsX, m_uiBlockBitsXY, uBoundary));
  }
	
  YSliceIterator GetYSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_y );
		
		return(YSliceIterator(uIdxSlice, m_dimension, m_background, m_mBlocks, 
			m_uiBlockBitsX, m_uiBlockBitsXY, boundingBox));
  }
	
  ZSliceIterator GetZSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_z );


    return(ZSliceIterator(uIdxSlice, m_dimension, m_background, m_mBlocks, 
                        m_uiBlockBitsX, m_uiBlockBitsXY, uBoundary));
  }

  ZSliceIterator GetZSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_z );
		
		return(ZSliceIterator(uIdxSlice, m_dimension, m_background, m_mBlocks, 
			m_uiBlockBitsX, m_uiBlockBitsXY, boundingBox));
  }

protected: //functions
  
  /// Initialize all data members except m_blockBitSize.
  void inline InitMemberVars();
  
  /// Calculates the number of blocks based on dimensions.
  inline uint4 CalculateNBlocks();
  
  /// Writes all volume data to a stream
  uint8 WriteVolDataToStream(std::ostrstream & os);
  
  /// Sets all block values to m_background.
  void SetToBackground(Block<T> & blockPtr);
  
  /// Write values of private data members to cerr
  void WritePrivateData();
  
  /// Returns the number of blocks with non-zero data.
  inline const uint4 AllocatedBlocks() const;
  
  /// Returns the number of bits needed to store n
  inline const uint4 SmPwr2(const uint4 & n) const;
  
  /// returns the block that the point belongs to.
  inline uint4 GetBlock(const Point<uint2> & pos) const;
  
  /// returns the block index for a given point.
  inline uint2 GetBlockIndex(const Point<uint2> & pos) const;

  /// returns the XYZ position given by the two indices.
  inline Point<uint2> GetPosFromIndices(const uint4 & block, const uint2 & index) const;

  /// reads a PNG format volume.
  int4 ReadPNG(std::string fileName, FILE * fp, int4 bytesRead);

  /// function called by libpng for reading unknown chunks.
  void ReadDataChunk(uint1 * dataIn, uint4 dataLength);


protected: // data
  /// vector of all blocks in the volume
  BlockVector<T> m_mBlocks;

  /// number of entries per block
  uint4 m_uiBlocksize;     
  uint4 m_uiBlockBitSize;
  uint4 m_uiBlockBitsX;    // number of sig X bits minus blockBitSize
  uint4 m_uiBlockBitsXY;
  uint4 m_uiRmHighBits;    // used in GetBlockIndex to remove bits above blockBitSize
};

#include "BlockVolume.inl"

#endif

// complete revision history:
// $Log: BlockVolume.h,v $
// Revision 1.4  2006/02/09 14:01:50  geconomos
// updated external libraries
//
// Revision 1.3  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.2  2005/04/27 17:36:12  geconomos
// no longer exporting this class ( soon to be removed )
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.4.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.4  2003/05/16 11:44:24  michael
// code review
//
// Revision 3.3  2001/12/24 16:47:05  ingmar
// moved SerializedBlockVolume into its own file
//
// Revision 3.2  2001/11/09 01:23:14  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/10/31 01:02:18  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding the volumes
//
// Revision 3.1  2001/10/31 00:45:57  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding
// the volumes
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 01 2001 16:10:16   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.1   Sep 20 2001 20:35:48   pagron
// changes to block volume implementation will reduce memory fragmentation
// 
//    Rev 2.0   Sep 16 2001 23:47:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:00   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.53  2001/08/28 23:58:03  ingmar
// std::vector<Block<T >, BlockAllocator<Block<T> > > -> BlockVector<T>
//
// Revision 1.52  2001/08/19 22:28:11  ingmar
// added GetSample()
//
// Revision 1.51  2001/08/09 21:17:26  liwei
// Constructor of BoundingRect and Box changed.
//
// Revision 1.50  2001/08/08 17:32:57  liwei
// Bounding boxes are all of the type Box<uint4>.
// Bugs in LinearVolumeIterator::NextZYX() fixed.
//
// Revision 1.49  2001/08/07 17:03:39  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.48  2001/08/06 23:24:26  liwei
// Passing boundingBox rather minCorner and maxCorner when creating SubVolume Iterator
// and slice iterators.
//
// Revision 1.47  2001/07/31 18:46:29  liwei
// Bugs in the revised version of SliceIterator fixed.
//
// Revision 1.46  2001/07/30 23:29:30  liwei
// Renamed VolIteratorSkipBoundary to SubVolIterator.
// Make slice iterators be subclasses of SubVolIterator.
//
// Revision 1.45  2001/07/30 17:39:15  huamin
// Added RayIterator
//
// Revision 1.44  2001/07/19 22:50:43  liwei
// Added VolIteratorSkipBoundary for linear and block volume.
// Bugs of the PeekX() functions of the block volume fixed.
//
// Revision 1.43  2001/07/18 20:53:17  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.42  2001/07/02 16:29:16  frank
// Added BODY segment type.
//
// Revision 1.41  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.40  2001/06/18 23:40:12  liwei
// Slice iterator added
//
// Revision 1.39  2001/06/14 23:53:41  liwei
// Bookkeeping the information of memory layout and datatype
//
// Revision 1.38  2001/04/19 20:59:04  dave
// updated variable names
//
// Revision 1.37  2001/04/12 18:53:08  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.36  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.35  2001/04/10 16:13:25  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.34  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.33  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.32  2001/04/03 05:47:00  soeren
// added documentation
//
// Revision 1.31  2001/03/31 05:34:38  ingmar
// uint8 -> uint4
//
// Revision 1.30  2001/03/30 18:50:04  dmitry
// Updated datatypes.
//
// Revision 1.29  2001/03/30 00:44:46  soeren
// BlockVolume now powered by blocks with Reference Counting
//
// Revision 1.28  2001/03/23 00:47:33  dave
// towards a more Viatronix standards compliant version
//
// Revision 1.27  2001/03/22 20:12:49  soeren
// fixed bug in constructor
//
// Revision 1.26  2001/03/21 22:30:40  soeren
// added merge function
//
// Revision 1.25  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.24  2000/12/12 18:34:12  dave
// corrected VolAPI constructor
//
// Revision 1.23  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.22  2000/11/17 00:28:40  dave
// remove syntax problems for gcc
//
// Revision 1.21  2000/11/16 17:34:06  frank
// Back to non-specialized
//
// Revision 1.20  2000/11/16 17:32:17  frank
// Specialized block iterator and GetPosFromIndices function
//
// Revision 1.19  2000/11/16 17:19:48  dave
// added native BlockVolume VolIterator
//
// Revision 1.18  2000/11/10 17:34:05  dave
// impl. API changes for VolIterator
//
// Revision 1.17  2000/11/08 19:46:36  frank
// Added CountInstancesOfValue function for quick native counting
//
// Revision 1.16  2000/11/03 18:20:15  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.15  2000/11/03 17:02:25  dave
// converted back
//
// Revision 1.14  2000/11/02 19:48:08  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.13  2000/11/01 21:11:29  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.12  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.11  2000/10/26 02:48:46  ingmar
// added Set/GetVoxel for Point<short>
//
// Revision 1.10  2000/10/25 01:35:24  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.9  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.8  2000/10/22 23:48:21  dave
// added RecBlockVolume
//
// Revision 1.7  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.6  2000/10/22 00:24:22  dave
// better iterators
//
// Revision 1.5  2000/10/19 19:59:18  ingmar
// moved Global.h up to get rid of VC++ compile warnings
//
// Revision 1.4  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.3.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.3  2000/09/13 22:39:57  dave
// BlockVolume Write improved
//
// Revision 1.2  2000/09/11 22:46:08  dave
// added date field to constructors
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
//
// 3     9/08/00 6:48p Dave
// more debugging performed
//
// 2     9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 1     9/04/00 9:44p Ingmar
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockVolume.h,v 1.4 2006/02/09 14:01:50 geconomos Exp $
// $Id: BlockVolume.h,v 1.4 2006/02/09 14:01:50 geconomos Exp $
