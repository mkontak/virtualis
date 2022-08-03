// $Id: vxBlockVolume.h,v 1.8 2006/07/05 15:08:52 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

/**
 * Encapsulate the vxBlock volume definition
 */

#ifndef vxBlockVolume_h
#define vxBlockVolume_h


// include declarations
#include "Typedef.h"
#include "vxBlock.h"
#include "vxVolumeHeader.h"
#include "Image.h"
#include "V3DFile.h"
#include "vxElement.h"
#include "vxVoxelCost.h"
#include "BaseHistogram.h"


// forward declarations
template <class VoxelType> class vxBlockVolumeIterator;
template <class VoxelType> class vxBlockVolumeRayIterator;

// class definition
template <class VoxelType>
class VX_BLOCK_VOLUME_DLL vxBlockVolume : public vxElement
{
  /// the iterator of this volume gives us access to the voxels and in our friend
  friend class vxBlockVolumeIterator<VoxelType>;
  friend class vxBlockVolumeRayIterator<VoxelType>;
  friend class vxBlockVolumeRayIterator<int2>;
  friend class vxBlockVolumeRayIterator<uint2>;
  friend class vxBlockVolumeIterator<int1>;
  friend class vxBlockVolumeIterator<int2>;
  friend class vxBlockVolumeIterator<int4>;
  friend class vxBlockVolumeIterator<uint1>;
  friend class vxBlockVolumeIterator<uint2>;
  friend class vxBlockVolumeIterator<uint4>;
  friend class vxBlockVolumeIterator<bool>;
  friend class vxBlockVolumeIterator<float4>;
  // the following is only used in VC Processor on March 2006
  friend class vxBlockVolumeIterator<vxVoxelCost>;

// functions
public:

  /// contructor
  explicit vxBlockVolume(const vxVolumeHeader & volumeHeader = vxVolumeHeader());

  /// copy contructor
  explicit vxBlockVolume(const vxBlockVolume<VoxelType> & otherVol);

  /// from file contructor
  explicit vxBlockVolume(const std::string & sFileName);

  /// destructor
  ~vxBlockVolume();

  /// copy other volume to current (will not allocate more memory, but share the voxels from the other volume (until written to))
  vxBlockVolume<VoxelType> const & operator=(const vxBlockVolume<VoxelType> & rhs);

  /// clear volume to background (release memory of old blocks)
  void Clear();

  /// clear volume to with new background value (release memory of old blocks)
  void Clear(VoxelType newBackgroundValue);

  /// returns true if volume consist only of background
  bool IsClear();
  
  /// write volume to disk
  void Write(const std::string & sFileName = "sameAsInHeader");

  /// write older linear volume version to disk
  void WriteAsLinVolVer1_0(const std::string & sFileName = "sameAsInHeader");

  /// write older block volume version to disk
  void WriteAsBlkVolVer1_0(const std::string & sFileName = "sameAsInHeader");

  /// write raw volume to disk
  void WriteRaw(const std::string & sFileName);

  /// compute preview image
  void ComputePreviewImage(Image< PixelRGB<uint1> > & image);

  /// read volume from disk
  void Read(const std::string & sFileName, std::pair<float4, float4> & progressPair = std::make_pair( 0.0F, 100.0F ) );

  // get current memory usage
  uint4 GetMemoryUsage() const;

  /// returns the volume header
  inline vxVolumeHeader & GetHeader() { return m_header; }

  /// returns the volume header
  inline const vxVolumeHeader & GetHeader() const { return m_header; }

  /// sets the header
  void SetHeader(const vxVolumeHeader header) { m_header = header; }

  /// gets the base histogram
  inline BaseHistogram & GetHistogram() { return m_histogram; }

  /// compact the volume
  void Compact();
  
  /// compare the data against another volume
  bool IsDataEquivalent( vxBlockVolume<VoxelType> & otherVol );

  /// gets the number of voxels per block
  uint4 GetVoxelsPerBlock() const; 

  /// returns the number of blocks based on the volume dimensions
  uint4 GetNumberOfBlocks();

private:


  /// read .slc volume from disk
  void ReadSlc(const std::string & sFileName);


// data
private:

  /// all volume data mamory independent header info
  vxVolumeHeader m_header;

  /// base histogram
  BaseHistogram m_histogram;

  /// typedef the block of voxels
  typedef vxBlock<VoxelType> BlockL0Vox;  
  
  /// block of background voxels
  BlockL0Vox m_bgBlockL0Vox;

  /// block of blocks of voxels
  typedef vxBlock<BlockL0Vox> BlockL1Blk;  
  
  /// block of blocks of background voxels
  BlockL1Blk m_bgBlockL1Blk;

  /// block containing the whole volume
  typedef vxBlock<BlockL1Blk> BlockL2Vol;  
  
  /// block containing the whole volume
  BlockL2Vol m_BlockL2Vol;

  /// dimensions of L0 block
  static Triple< int4 > m_dimL0;

  /// dimensions of L1 block
  static Triple< int4 > m_dimL1;

  /// dimensions of L2 block
  static Triple< int4 > m_dimL2;

}; // vxBlockVolume


#endif // vxBlockVolume_h


// $Log: vxBlockVolume.h,v $
// Revision 1.8  2006/07/05 15:08:52  romy
// derives from vxElement
//
// Revision 1.7  2006/04/03 16:46:16  geconomos
// + added base histogram
// + removed friend decaration for vxBlockVolumeFileReader
//
// Revision 1.6  2006/03/29 19:51:02  geconomos
// + GetVoxelsPerBlock()
// + statics for  L0. L1, & L2 sizes
//
// Revision 1.5  2006/03/09 15:17:04  dongqing
// move vxVoxelCost from Colon Processor to vxBlockVolume
//
// Revision 1.4  2005/03/23 19:28:57  michael
// Added destructor to class
//
// Revision 1.3  2004/04/26 12:15:19  frank
// added code to compare the data in two volumes
//
// Revision 1.2  2004/03/22 20:58:23  frank
// fixed some typos
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2003/06/18 15:19:47  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.34  2003/05/19 17:31:08  dongqing
// fix the vxBlockVolume constructor
//
// Revision 1.33  2003/05/17 16:19:40  michael
// reformatting
//
// Revision 1.32  2003/05/01 18:51:19  dongqing
// code review
//
// Revision 1.31  2003/04/30 20:08:43  frank
// Removed const from progress metering
//
// Revision 1.30  2003/04/30 12:00:45  michael
// initial code review
//
// Revision 1.29  2003/04/18 16:00:05  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 1.28  2003/04/14 17:11:48  frank
// Exposed progress metering to managed classes
//
// Revision 1.27  2003/04/07 16:59:22  michael
// renamed "background color" to "background value"
//
// Revision 1.26  2002/12/12 16:59:47  ingmar
// added WriteRaw()
//
// Revision 1.25  2002/10/01 14:21:14  ingmar
// added ReadSlc()
//
// Revision 1.24  2002/09/25 21:00:23  ingmar
// added rayIterator friends
//
// Revision 1.23  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.22  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.21  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.20  2002/07/02 17:14:46  soeren
// added compact function
//
// Revision 1.19  2002/07/01 23:23:30  soeren
// added SetHeader function
//
// Revision 1.18.2.1  2002/07/02 20:28:40  jaddonisio
// Merge from main trunk, per Soeren changes.
//
// Revision 1.18  2002/05/22 20:14:33  ingmar
// added sectioning for doxygen documentation
//
// Revision 1.17  2002/05/15 18:15:42  soeren
// added isClear function in volume
//
// Revision 1.16  2002/05/14 17:15:25  ingmar
// added copy constructor
//
// Revision 1.15  2002/05/10 23:03:07  soeren
// removed some unnecessary functions and warnings
//
// Revision 1.14  2002/05/09 22:18:02  ingmar
// added ClearWithBackground() to resolve Compilre preformance warning
//
// Revision 1.13  2002/05/08 19:13:52  tushar
// made vxVolume complatible with float4
//
// Revision 1.12  2002/05/02 22:21:21  ingmar
// added progressCallback function to Read() of 2.0 FileVersion files
//
// Revision 1.11  2002/05/02 16:58:09  ingmar
// added ComputePreviewImage()
//
// Revision 1.10  2002/04/30 21:27:56  soeren
// added GetVoxel at iterator pos
//
// Revision 1.9  2002/04/27 23:18:39  soeren
// added SetPos from Iterator
//
// Revision 1.8  2002/04/26 13:26:35  soeren
// added write of old Linear volume format
//
// Revision 1.7  2002/04/24 19:59:15  ingmar
// added const version of GetHeader()
//
// Revision 1.6  2002/04/24 18:40:40  soeren
// fixed background bug and added clear of the whole volume by value function
//
// Revision 1.5  2002/04/16 19:46:45  ingmar
// added more functionality
//
// Revision 1.4  2002/04/12 23:05:46  ingmar
// added From file Contructor and Calk NuOf BlkOfOldVolLayout
//
// Revision 1.3  2002/04/11 14:54:34  ingmar
// added GetMemoryUsage()
//
// Revision 1.2  2002/04/10 18:07:46  ingmar
// worked on new header volume class
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolume.h,v 1.8 2006/07/05 15:08:52 romy Exp $
// $Id: vxBlockVolume.h,v 1.8 2006/07/05 15:08:52 romy Exp $
