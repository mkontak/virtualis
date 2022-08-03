// $Id: Block.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: March. 01
// Author: Soeren soeren@viatronix.com

#ifndef BLOCK_H
#define BLOCK_H

// includes
#include "Mutex.h"

// forward declarations
template <class T> class BlockVector;
template <class T> class BlockVolume;

//#define m_pInfoStruct->m_uBlockSize         (m_pInfoStruct->m_bIsDirty)
//#define m_pInfoStruct->m_uBlockSize       (m_pInfoStruct->m_uBlockSize)
//#define rhs.m_pInfoStruct->m_uBlockSize   (rhs.m_pInfoStruct->m_uBlockSize)

/**
 * The Block Class Template. The block class consist of a pointer to a data chunk, a reference counter, a dirty flag and the block size.
 * Everything is stored in the data chunk. Two or more blocks can share the same data chunk. Consistency is 
 * controlled by the reference counter. A data chunk will only be deleted, if the block is not shared. If you
 * write or assign something to a data chunk in a block, the data chunk will be copied to new data chunk and the dirty flag 
 * is set to true. So the modification is only visibly in the block who modified the data chunk. If you read something from the data
 * chunk there is no check if the position is valid (performance reasons).
 \warning If you need a vector or similar (maps, ...) of Blocks, you should override the default allocator 
 by BlockAllocator, otherwise your application will die and you will spend several hours to find the problem. E.g.:
 vector<Block<T >, BlockAllocator<Block<T> > > m_mblocks;.  
 * $Id: Block.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/


#define FILE_REVISION "$Revision: 1.2 $"



class BlockInfo 
{
public:
  uint4 m_uBlockSize;
  bool m_bIsDirty;
  uint1 m_uDummy[3];
  void *m_pData;
  int4 m_uRefCount;  
};

class  BlockStatic
{
protected:
  /// Returns a free BlockInfo
  static inline BlockInfo * GetNewBlockInfo();
};

template <class T>
class  Block : public BlockStatic
{
private:

  ///Constructor: Creates data chunk, MAXIMUM size of Block = 0xFFFFFFFE. 
  /// \warning Please read class description before you use the block template class

  public: 

  inline Block();
  inline Block(const uint4 & uBlockSize);
  ///Constructor Creates data chunk, MAXIMUM size of Block = 0xFFFFFFFE.
  /// \warning Please read class description before you use the block template class
  inline Block(const uint4 & uBlockSize, const T & value);

  ///Destructor: Carefully destroys data chunk (Reference Counting)
  inline ~Block();

  ///Merge two blocks
  inline bool Merge(const Block<T> &rhs);

  ///Access function for item in the block
  inline T & At( const uint4 & iIndex ) const;

  ///Set item at position iIndex in the block data with value
  inline void Set(const T & value, const uint4 & iIndex );

  ///Returns position of item which first matches the specified value after iIndex
  inline uint4 FindFirstOf(const T value, const uint4 iIndex = 0) const;

  ///Returns position of item which first not matches the specified value after iIndex
  inline uint4 FindFirstNotOf(const T value, const uint4 iIndex = 0) const;

  ///Resize the block chunk
  inline bool Resize(uint4 uBlockSize);

  ///Compare two blocks (compares the items in the data chunk)
  inline bool DeepCmp(const Block<T> & rhs);

  ///Compare two blocks (pointer to data chunk compare)
  inline bool operator==( const Block<T> & rhs ) const;

  ///Compare two blocks
  inline bool operator!=( const Block<T> & rhs ) const;

  ///Returns const pointer to data of block
  inline const T * Raw() const;

  /// Assign new data to the data in the block
  inline  void Assign(T * data);

  /// Assign new data to the data in the block
  inline  void Assign(T * data, const uint4 & uBlockSize);

  /// Assign one Block to another
  inline const Block<T> & operator = (const Block<T> & rhs);

  /// Return Dirty flag
  inline const bool & IsDirty() const;

  /// Ensures that the block is not shared. Possibly makes a copy of itself.
  inline void UnShare();

    /// Returns true is the Block is shared, otherwise false
  inline bool IsShared();

  /// Returns true is the Block is unshared, otherwiese false
  inline bool IsUnShared();

  /// Returns if the reference counter is valid, this means if the reference counter is still greater than 0
  inline bool IsReferenceCounterValid();

  /// Returns pointer to data
  inline T * GetDataPtr();

private:

    /// Increases the reference counter
  inline void IncreaseReferenceCounter();

  /// Decreases the reference counter
  inline void DecreaseReferenceCounter();

  /// Create data chunk 
  inline void Init(uint4 uBlockSize);

  /// Create data chunk and initialize the elements of the data chunk with a init value.
  inline void Init(uint4 uBlockSize, const T & value);

  /// Carefully destroys data in the block
  inline void Destroy();

  /// A hack that helps BlockVolume activation
  Block(T* pData, uint4 uRefCount, uint4 uBlockSize, bool bIsDirty)
  {
  //  m_pData = pData;    
  //  m_pInfoStruct->m_uRefCount = uRefCount;
  //  m_pInfoStruct->m_uBlockSize = uBlockSize;
  //  m_pInfoStruct->m_uBlockSize = bIsDirty;
  }
  /// Pointer to the data chunk in the block  
  T *m_pData;
  BlockInfo *m_pBlockInfo;
  friend class BlockVector< T >;
  friend class BlockVolume< T >;
}; // Block

#undef FILE_REVISION 

#include "Block.inl"

#endif

// $Log: Block.h,v $
// Revision 1.2  2005/04/27 17:36:12  geconomos
// no longer exporting this class ( soon to be removed )
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.9  2002/08/14 16:10:48  wenli
// initial version - Vascular
//
// Revision 3.8  2002/04/02 20:01:56  ingmar
// made Increase/DecreaseReferenceCounter() private
//
// Revision 3.7  2002/03/20 16:04:38  ingmar
// Added fast acces funtion to the data pointer
//
// Revision 3.6  2002/03/12 19:50:57  ingmar
// removed mutex locking of critical sections, now its fast again, but not thread save
//
// Revision 3.5  2002/03/11 22:24:29  ingmar
// Fixed little static member initialization raise condition ;-)
//
// Revision 3.4  2002/03/08 16:21:11  ingmar
// reorganized Block class and added mutexes
//
// Revision 3.3  2002/03/07 23:33:30  ingmar
// added IsShared function
//
// Revision 3.2  2002/03/06 01:23:35  ingmar
// Reference Counter is now separat
//
//
// $Id: Block.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Block.h,v 1.2 2005/04/27 17:36:12 geconomos Exp $

