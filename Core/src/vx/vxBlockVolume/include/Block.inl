// $Id: Block.inl,v 1.3 2006/01/18 20:36:48 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Block.inl,v 1.3 2006/01/18 20:36:48 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Description: see below.
// Created: March. 01
// Author: Soeren soeren@viatronix.com

#define FILE_REVISION "$Revision: 1.3 $"


BlockInfo * BlockStatic::GetNewBlockInfo()
{
  static std::vector<BlockInfo> vBlockInfo = std::vector<BlockInfo>(0);
  static std::vector<BlockInfo>::iterator iterBlockInfo;
  if ( vBlockInfo.empty() )
  {
    vBlockInfo.resize(512/16 * 512/16 * 512/16 * 16 * 8); // 32MB
    for ( iterBlockInfo = vBlockInfo.begin(); iterBlockInfo != vBlockInfo.end(); ++iterBlockInfo )
    {
      iterBlockInfo->m_uRefCount = 0;
      iterBlockInfo->m_uBlockSize = 0;
      iterBlockInfo->m_bIsDirty = 0;
      iterBlockInfo->m_pData = NULL;
    }  
    iterBlockInfo = vBlockInfo.begin();
  }
  // continue from last found position
  for ( ; iterBlockInfo != vBlockInfo.end(); ++iterBlockInfo )
  {
    if ( iterBlockInfo->m_uRefCount == 0 )
    {
      iterBlockInfo->m_uBlockSize = 0;
      iterBlockInfo->m_bIsDirty = 0;
      iterBlockInfo->m_pData = NULL;
      BlockInfo * pBlockInfoReturn = &(*iterBlockInfo);
      ++iterBlockInfo;
      return(pBlockInfoReturn);
    }
  }      
  // wrap around, continue from vector beginning
  for ( iterBlockInfo = vBlockInfo.begin(); iterBlockInfo != vBlockInfo.end(); ++iterBlockInfo )
  {
    if ( iterBlockInfo->m_uRefCount == 0 )
    {
      iterBlockInfo->m_uBlockSize = 0;
      iterBlockInfo->m_bIsDirty = 0;
      iterBlockInfo->m_pData = NULL;
      BlockInfo * pBlockInfoReturn = &(*iterBlockInfo);
      ++iterBlockInfo;
      return(pBlockInfoReturn);
    }
  }  
  throw ex::VException(LogRec("ref count array is too small (need to change source code)","Block::BlockInfo","GetNewBlockInfo"));
}

template <class T>
inline Block<T>::Block()
{
  //m_pData = NULL;
  //m_pBlockInfo = NULL;
  Init(0);
}    


template <class T>
inline Block<T>::Block(const uint4 & uBlockSize) 
{
  Init(uBlockSize);
}    


template <class T>
inline Block<T>::Block(const uint4 & uBlockSize, const T & value) 
{
  Init(uBlockSize, value);
}


template <class T>
inline Block<T>::~Block()
{
  Destroy();
}


/**
 * Create data chunk.
 * Each item in the block is uninitialized.
 * @param uuBlockSize number of elements in the block
*/
template <class T>
inline void Block<T>::Init(uint4 uBlockSize)
{
  m_pBlockInfo = GetNewBlockInfo();
  //m_pData = new T __declspec(align(16)) [uBlockSize];
  m_pData = new T[uBlockSize];
  m_pBlockInfo->m_pData = static_cast<void *>(m_pData);
  IncreaseReferenceCounter();// set reference counter to one. If reference counter is greater than one the data in the block is shared.
  m_pBlockInfo->m_uBlockSize = uBlockSize; // save block size.
  m_pBlockInfo->m_bIsDirty = false; // set dirty flag to false
}


/**
 * Create data chunk and initialize data chunk with a init value.
 * The first parameter specifies the size of the block. After that each item in the block
 * will be initialized with the second parameter.
 * @param uuBlockSize number of elements in the block
 * @param initValue
*/
template <class T>
inline void Block<T>::Init(uint4 uBlockSize, const T & value)
{
  Init(uBlockSize);
  for ( uint4 i = 0; i < uBlockSize; ++i ) 
  {
    m_pData[i] = value; // initialize items 
  }
}


/**
 * Access function for item in the block.
 * This function returns the item at position iIndex.
 * There is no checking, if the position is valid (performance reasons).
 * At is prefered over operator[] to make clear that read and write access are handled different.
 * @param iIndex position in the block
*/
template <class T>
inline T & Block<T>::At( const uint4 & iIndex ) const
{
  return(m_pData[iIndex]); // return data at index 
}

/**
 * Access function to data pointer
*/
template <class T>
inline T * Block<T>::GetDataPtr()
{
  return m_pData;
}

/**
 * Ensure that the Block is unshared.
 * If the data in the block is non shared, nohing happens.
 * If the data in the block is shared, the whole data will be copied
 * and the reference counter of the other copy will be decreased. 
 * @param k positions in the block
 * @param value value 
*/
template <class T>
inline void Block<T>::UnShare()
{
  // there is only one copy of this block -> already unshared
  if ( IsUnShared() ) return;
  // about to unshare block -> clone
  DecreaseReferenceCounter();  // decrease reference counter, since we make now a copy
  T * pData = m_pData; // save old data pointer
  Init(m_pBlockInfo->m_uBlockSize); // create new data block 
  for (uint4 k(0); k < m_pBlockInfo->m_uBlockSize; ++k) // copy old data
  {
    m_pData[k] = pData[k];
  }
} // UnShare


/**
 * Set item at position iIndex in the block data with value.
 * @param k positions in the block
 * @param value value 
*/
template <class T>
inline void Block<T>::Set(const T & value, const uint4 & uIndex ) 
{
  if ( uIndex < m_pBlockInfo->m_uBlockSize ) //check if position is valid
  { 
    if ( m_pData[uIndex] != value )  // if value is already the same there is no need to set it
    {
      UnShare(); // make block writeable
      m_pBlockInfo->m_bIsDirty = true;
      m_pData[uIndex] = value;               
    }    
  }
  else
  {
    throw ex::VException(LogRec("Index greater than block size", "Block<T>","Set"));
  }
} // Set


/**
 * Returns the position of the first item which contains value.
 * Return: If succeed, the positions will be returned otherwise 0xFFFFFFFF
 * @param value value
 * @param iIndex position in data chunk
*/
template <class T>
inline uint4 Block<T>::FindFirstOf(const T value, const uint4 iIndex) const
{
  uint4 i;
  if ( iIndex < m_pBlockInfo->m_uBlockSize )
  {
    for ( i = iIndex; i < m_pBlockInfo->m_uBlockSize; ++i )
    {
      if ( m_pData[i] == value )
      {
        return(i);
      }
    }
    return(0xFFFFFFFF);
  } 
  else
  {
    return(0xFFFFFFFF);
  }
}


/**
 * Returns the position of the first item which not contains value.
 * Return: If succeed, the positions will be returned otherwise 0xFFFFFFFF
 * @param value value
 * @param iIndex position in data chunk
*/
template <class T>
inline uint4 Block<T>::FindFirstNotOf(const T value, const uint4 iIndex) const
{
  uint4 i;
  if ( iIndex < m_pBlockInfo->m_uBlockSize )
  {
    for ( i = iIndex; i < m_pBlockInfo->m_uBlockSize; ++i )
    {
      if ( m_pData[i] != value )
      {
        return(i);
      }
    }
    return(0xFFFFFFFF);
  } 
  else
  {
    return(0xFFFFFFFF);
  }
}


/**
* Resize the block chunk
* @param uBlockSize size of Block
*/
template <class T>
inline bool Block<T>::Resize(uint4 uBlockSize)
{
  bool bSuccess(false);
  if (uBlockSize < 0xFFFFFFFF)
  {
    if(m_pBlockInfo->m_uBlockSize != uBlockSize)
    {    
      Destroy(); // carefully destroy current data
      Init(uBlockSize);
    }
    bSuccess = true;
  } 
  return bSuccess;
}


/**
 * Merge two blocks.
 * This functions compares two blocks. If the blocks
 * have the same items. The data in the this-block will be (carefully) destroyed
 * and after that, the data of the rhs-block will be assigned to the this-block.
 * Carefully destroyed means, the data will only be destroyed if no other block
 * is pointing on it.
 * @param rhs other block
*/
template <class T>
inline bool Block<T>::Merge(const Block<T> &rhs)
{
  bool bSuccess(true);
  if ( m_pData != rhs.m_pData ) //Are they already merged.
  {
    if ( DeepCmp(rhs) ) // If isEqual is still true, merge is possible, so merge the two blocks 
    {
      Destroy();
      m_pData = rhs.m_pData; // assign pointer to the data in the rhs block.
      IncreaseReferenceCounter();// increase refernce counter, block is now shared.
    } 
    else
    {
      bSuccess = false;
    }
  } 
  return(bSuccess);
}


/**
 * Compares the two data chunks of the blocks
 * If every item in the data chunk is equal,
 * 
 * @param rhs other block
*/
template <class T>
inline bool Block<T>::DeepCmp(const Block<T> & rhs)
{
  bool bIsEqual(false);
  if( m_pData != rhs.m_pData )
  {    
    if (m_pBlockInfo->m_uBlockSize == rhs.m_pBlockInfo->m_uBlockSize)
    {
      if(memcmp(m_pData,rhs.m_pData,m_pBlockInfo->m_uBlockSize * sizeof(T)) == 0)
      {
        bIsEqual = true;
      }
    }
  }
  else
  {
    bIsEqual = true;
  }
  return bIsEqual;
}


/**
 * Compare two blocks
 * If the data in the blocks are shared, this function returns true,
 * otherwise false.
 * @param rhs other block
*/
template <class T>
inline bool Block<T>::operator==( const Block<T> & rhs ) const
{
  return(m_pData==rhs.m_pData);  // if the data pointer is the same, the rest should be the same.
} 


/**
 * Compare two blocks.
 * If the data in the blocks are shared, this function returns false,
 * otherwise true.
 * @param rhs other block
*/
template <class T>
inline bool Block<T>::operator!=( const Block<T> & rhs ) const
{
  return(!(*this==rhs));
}


/**
 * Return const pointer to data of block.
 * This functions returns a const pointer to the data in the block
 * You should never modify the data, because the data is may be shared.
 * This function should only be used for read issues.
 * For assigning you should use the assign function.
*/
template <class T>
inline const T * Block<T>::Raw() const
{
  return(m_pData); 
}


/**
 * Assign new data to the data in the block.
 * Carefully destroys the current data, allocates new memory for the
 * data and copies.
 * @param data pointer to new data
 * @param uuBlockSize soize of block 
*/
template <class T>
inline  void Block<T>::Assign(T * pData)
{
  UnShare();
  for (uint4 k(0); k < m_pBlockInfo->m_uBlockSize; ++k) // copy old data
  {
    m_pData[k] = pData[k];
  }
  m_pBlockInfo->m_bIsDirty = true; // set dirty flag to true, since data is now modified.
}


/**
 * Assign new data to the data in the block.
 * Carefully destroys the current data, allocates new memory for the
 * data and copies.
 * @param data pointer to new data
 * @param uuBlockSize soize of block 
*/
template <class T>
inline  void Block<T>::Assign(T * pData, const uint4 & uBlockSize)
{
  Resize(uBlockSize); // carefully destroy current data
  UnShare();
  for (uint4 k(0); k < m_pBlockInfo->m_uBlockSize; ++k) // copy old data
  {
    m_pData[k] = pData[k];
  }
  m_pBlockInfo->m_bIsDirty = true; // set dirty flag to true, since data is now modified.
}


/**
 * Assigns one Block to another.
 * This functions carefully destroys the current data.
 * Then the data pointer of the rhs block will be assigned
 * to the data pointer of this-block. After that the Refercence
 * Counter will be increased. Now the data is shared.
 *@param rhs other block
*/
template <class T>
inline const Block<T> & Block<T>::operator= (const Block<T> & rhs)
{
  if ( rhs.m_pData == m_pData ) // if rhs is the same than this return.
  {
    return(*this);
  }
  Destroy(); // delete old data
  m_pData = rhs.m_pData; // copy pointer to data from rhs to this.
  m_pBlockInfo = rhs.m_pBlockInfo; // copy pointer to data from rhs to this.
  IncreaseReferenceCounter();// increase reference counter, since data in the block is now shared.
  return(*this);
}


/**
 * Returns Dirty flag.
 * If you create a new block the data is not dirty.
 * If you use the assign() or set function, the data
 * status will be change to dirty.
 */
template <class T>
inline const bool & Block<T>::IsDirty() const
{
  return(m_pBlockInfo->m_bIsDirty); // return dirty flag.
}


/**
 * Carefully destroys data in the block.
 * If the data is not shared, the data will be deleted,
 * otherwise the reference counter will be decreased.
*/
template <class T>
inline void Block<T>::Destroy() 
{
  if (IsReferenceCounterValid() ) // check is reference counter is broken.
  {
    if (IsShared()) // check if data in block is shared, if not delete data.
    {
      DecreaseReferenceCounter();  // important to do this after can to IsShared()
    }
    else
    {  
      DecreaseReferenceCounter();  // important to do this after can to IsShared()
      if(m_pData != NULL)
      {
      delete [] m_pData;
      m_pData = NULL;                
      }      
      m_pBlockInfo = NULL;
    }
  } 
  else
  {
    throw ex::VException(LogRec("FATAL ERROR IN BLOCK CLASS TEMPLATE, REFERENCE COUNTER REACHED VALUE WHICH IS IMPOSSIBLE","Block<T>","Destroy"));
  }
} 


/**
 * Increases the reference counter
*/
template <class T>
inline void Block<T>::IncreaseReferenceCounter()
{
  ++m_pBlockInfo->m_uRefCount;
}


/**
 * Decreases the reference counter
*/
template <class T>
inline void Block<T>::DecreaseReferenceCounter()
{
  --m_pBlockInfo->m_uRefCount;
}


/**
 * Returns true is the Block is shared, otherwise false
*/
template <class T>
inline bool Block<T>::IsShared()
{
  return(m_pBlockInfo->m_uRefCount > 1);
}


/**
 * Returns true is the Block is unshared, otherwise false
*/
template <class T>
inline bool Block<T>::IsUnShared()
{
  return(m_pBlockInfo->m_uRefCount == 1);
}


/**
 * Returns if the reference counter is valid, this means if the reference counter is still greater than 0
*/
template <class T>
inline bool Block<T>::IsReferenceCounterValid()
{
  return(m_pBlockInfo->m_uRefCount > 0);
}

#undef FILE_REVISION

// $Log: Block.inl,v $
// Revision 1.3  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2005/04/27 17:36:12  geconomos
// no longer exporting this class ( soon to be removed )
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/16 11:44:24  michael
// code review
//
// Revision 3.8  2002/06/04 03:26:02  soeren
// fixed memory leak
//
// Revision 3.7  2002/03/20 16:04:39  ingmar
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
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:58   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:17  soeren
// renamed volume  directory
//
// Revision 1.19  2001/08/28 23:58:03  ingmar
// std::vector<Block<T >, BlockAllocator<Block<T> > > -> BlockVector<T>
//
// Revision 1.18  2001/07/24 23:00:22  tushar
// fixed a byg
//
// Revision 1.17  2001/05/21 00:06:16  soeren
// More Coding standard compliant
//
// Revision 1.16  2001/04/19 18:48:36  soeren
// change some names to the coding standards
//
// Revision 1.15  2001/04/12 18:52:58  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.14  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.13  2001/04/03 05:59:52  soeren
// added findFirstNotOf
// function
//
// Revision 1.12  2001/04/03 05:47:00  soeren
// added documentation
//
// Revision 1.11  2001/04/02 21:08:42  soeren
// addes link to design specification,
// modified merge function, added
// FindFirstOf function
//
// Revision 1.10  2001/04/02 19:20:52  soeren
// added function deepcmp
//
// Revision 1.9  2001/04/02 15:25:44  soeren
// beautified block and
// changed behavior of set and delete
// function.
//
// Revision 1.8  2001/04/01 20:41:45  ingmar
// added log at end of file
//
//
// $Id: Block.inl,v 1.3 2006/01/18 20:36:48 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Block.inl,v 1.3 2006/01/18 20:36:48 geconomos Exp $

