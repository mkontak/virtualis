// $Id: vxBlock.h,v 1.6 2007/11/03 00:02:40 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

/*
 The vxBlock Class Template. The block class consist of a pointer to a data chunk, a reference counter, a Background flag and the block size.
 Everything is stored in the data chunk. Two or more blocks can share the same data chunk. Consistency is 
 controlled by the reference counter. A data chunk will only be deleted, if the block is not shared. If you
 write or assign something to a data chunk in a block, the data chunk will be copied to new data chunk and the Background flag 
 is set to true. So the modification is only visibly in the block who modified the data chunk. If you read something from the data
 chunk there is no check if the position is valid (performance reasons).
*/

#ifndef vxBlock_h
#define vxBlock_h


// includes
#include "CriticalSection.h"
#include "SingleLock.h"

// defines
#define FILE_REVISION "$Revision: 1.6 $"

// class definition
class VX_BLOCK_VOLUME_DLL vxBlockInfo 
{
public:
  uint4 m_uBlockSize;
  bool  m_bIsDirty;
  uint1 m_uDummy1;
  bool  m_bIsHomogenous;
  uint1 m_uDummy2;
  void *m_pData;
  LONG  m_iRefCount;  
  int4 m_iBinaryHistogram;
}; // vxBlockInfo


// class definition
class VX_BLOCK_VOLUME_DLL vxBlockStatic 
{
// functions
public:
  /// sets gobally for all blocks in the running process, if thread safety should be considered or not
  static void SetThreadSafety(bool bIsNowThreadSafe) { IsThreadSafe() = bIsNowThreadSafe; }

  /// returns a free vxBlockInfo
  static bool & IsThreadSafe();

protected:

  /// returns a free vxBlockInfo
  static vxBlockInfo * GetNewBlockInfo();

  /// returns a free vxBlockInfo
  static CriticalSection * GetCriticalSection();
}; // vxBlockStatic


// class definition
template <class Type>
class vxBlock : public vxBlockStatic
{
  friend class vxBlock<int1>;  
  friend class vxBlock<vxBlock<int1> >;  
  friend class vxBlock<vxBlock<vxBlock<int1> > >;
  friend class vxBlock<int2>;  
  friend class vxBlock<vxBlock<int2> >;  
  friend class vxBlock<vxBlock<vxBlock<int2> > >;

// functions
public: 

  // default constructor
  inline vxBlock();

  // constructor
  inline vxBlock(const uint4 & uBlockSize);

  /// constructor creates data chunk, MAXIMUM size of vxBlock = 0xFFFFFFFE.
  inline vxBlock(const uint4 & uBlockSize, const Type & value, const bool bBackgroundFlag = false);

  /// destructor: carefully destroys data chunk (reference counting)
  inline ~vxBlock();

  /// merge two blocks
  inline bool Merge(const vxBlock<Type> &rhs);

  /// access function for item in the block
  inline Type & At(const uint4 & iIndex) const;

  /// set item at position iIndex in the block data with value
  inline void Set(const Type & value, const uint4 & iIndex);

  /// returns position of item which first matches the specified value after iIndex
  inline uint4 FindFirstOf(const Type value, const uint4 iIndex = 0) const;

  /// returns position of item which first not matches the specified value after iIndex
  inline uint4 FindFirstNotOf(const Type value, const uint4 iIndex = 0) const;

  /// return size of block
  inline int4 GetSize() const;

  /// resize the block chunk
  inline bool Resize(uint4 uBlockSize);

  /// compare two blocks (compares the items in the data chunk)
  inline bool DeepCmp(const vxBlock<Type> & rhs);

  /// compare two blocks (pointer to data chunk compare)
  inline bool operator==(const vxBlock<Type> & rhs) const;

  /// compare two blocks
  inline bool operator!=(const vxBlock<Type> & rhs) const;

  /// returns const pointer to data of block
  inline const Type * Raw() const;

  /// assign one vxBlock to another
  inline const vxBlock<Type> & operator = (const vxBlock<Type> & rhs);

  /// return background flag
  inline const bool & IsDirty() const;

  /// Return homogeneous flag
  inline const bool & IsHomogenous() const;

  /// updates homogeneous flag if possible
  inline const bool IsUpdateOfHomogenousBitPossible() const;

  /// return binrary histogram
  inline const int4 GetBinaryHistogram() const;

  /// ensures that the block is not shared. Possibly makes a copy of itself.
  inline void UnShare();

  /// returns true is the vxBlock is shared, otherwise false
  inline bool IsShared() const;

  /// returns true is the vxBlock is unshared, otherwiese false
  inline bool IsUnShared() const;

  /// returns if the reference counter is valid, this means if the reference counter is still greater than 0
  inline bool IsReferenceCounterValid() const;

  /// returns pointer to data
  inline Type * GetDataPtr();

  /// increases the reference counter
  inline void IncreaseReferenceCounter();

private:

  /// decreases the reference counter
  inline LONG DecreaseReferenceCounter( vxBlockInfo * pBlockInfo );

  /// create data chunk 
  inline void Init(uint4 uBlockSize);

  /// create data chunk and initialize the elements of the data chunk with a init value.
  inline void Init(uint4 uBlockSize, const Type & value, const bool bBackgroundFlag = false);

  /// carefully destroys data in the block
  inline void Destroy();

// data
public:

  /// pointer to the data chunk in the block  
  Type * m_pData;

  /// pointer to the block info
  vxBlockInfo * m_pBlockInfo;
}; // vxBlock


#undef FILE_REVISION 


#include "vxBlock.inl"


#endif // vxBlock_h


// Revision History:
// $Log: vxBlock.h,v $
// Revision 1.6  2007/11/03 00:02:40  jmeade
// Merge_from_CON_30_on_20071102
//
// Revision 1.3.2.1  2007/10/22 20:14:42  gdavidson
// Issue #5510: Fixed creation of the blocks
//
// Revision 1.5  2007/09/13 19:44:00  gdavidson
// Modified the method signiture for the DecreaseReferenceCounter
//
// Revision 1.4  2007/09/12 21:59:16  gdavidson
// Changed the creation of the blocks
//
// Revision 1.3  2006/10/12 14:23:53  geconomos
// corrected SingleLock usage
//
// Revision 1.2  2006/03/29 19:52:33  geconomos
// + added binary histogram to block info
// + changed to use new CriticalSection class
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.18  2003/05/17 16:19:39  michael
// reformatting
//
// Revision 1.17  2003/04/30 12:00:45  michael
// initial code review
//
// Revision 1.16  2003/04/30 10:53:36  michael
// initial code review
//
// Revision 1.15  2002/10/17 22:25:02  ingmar
// CriticalScope -> vxCriticalScope
// CriticalSection-> vxCriticalSection
// now with static function to enable/disable thread safety
//
// Revision 1.14  2002/10/04 23:01:00  ingmar
// improved critical section as used in vxBlock
//
// Revision 1.13  2002/09/30 20:47:49  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 1.12  2002/09/26 18:50:58  ingmar
// maked reference counting thread safe
//
// Revision 1.11  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.10  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.9  2002/07/02 17:14:31  soeren
// added IsUpdateOfHomogenousBitPossible
//
// Revision 1.8.2.1  2002/07/02 20:28:40  jaddonisio
// Merge from main trunk, per Soeren changes.
//
// Revision 1.8  2002/06/05 19:13:34  soeren
// Added Homogenous Flag
//
// Revision 1.7  2002/05/23 16:27:40  ingmar
// code beautify
//
// Revision 1.6  2002/05/17 20:38:37  frank
// Added const to some constant functions.
//
// Revision 1.5  2002/05/03 17:08:15  frank
// added Backgroundflag in constructor
//
// Revision 1.4  2002/05/02 16:30:48  soeren
// added fix for slow debug mode - blockVolume
//
// Revision 1.3  2002/04/25 00:47:54  michael
// Implemented faster initialization of blocks in BlockVolume constructor (ingmar)
//
// Revision 1.2  2002/04/11 14:55:49  ingmar
// made GetSize() const function
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Id: vxBlock.h,v 1.6 2007/11/03 00:02:40 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlock.h,v 1.6 2007/11/03 00:02:40 jmeade Exp $
