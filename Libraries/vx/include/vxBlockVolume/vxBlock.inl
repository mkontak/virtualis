// $Id: vxBlock.inl,v 1.8 2007/11/03 00:02:40 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

                             
//#define CRITICAL_SECTION_POSSIBLE
#undef CRITICAL_SECTION_POSSIBLE
                             

// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Default constructor
 */
template <class Type>
inline vxBlock<Type>::vxBlock()
//*******************************************************************
{
  m_pBlockInfo = GetNewBlockInfo();
  m_pBlockInfo->m_uBlockSize = 0;
  m_pBlockInfo->m_pData = NULL;
  m_pData = NULL;
}    


/**
 * Constructor
 * @param block size
 */
template <class Type>
inline vxBlock<Type>::vxBlock(const uint4 & uBlockSize) 
//*******************************************************************
{
  Init(uBlockSize);
}    


/**
* Constructor
 * @param block size
 * @param value
 * @param background flag
 */
template <class Type>
inline vxBlock<Type>::vxBlock(const uint4 & uBlockSize, 
                              const Type & value, 
                              const bool bBackgroundFlag) 
//*******************************************************************
{
  Init(uBlockSize, value, bBackgroundFlag);
}


/**
 * Destructor
 */
template <class Type>
inline vxBlock<Type>::~vxBlock()
//*******************************************************************
{
  Destroy();
}


/**
 * Init a chunk of data
 * Each item in the block is uninitialized.
 * @param uuBlockSize number of elements in the block
 */
template <class Type>
inline void vxBlock<Type>::Init(uint4 uBlockSize)
//*******************************************************************
{
  m_pBlockInfo = GetNewBlockInfo();
  m_pData = new Type[uBlockSize];
  m_pBlockInfo->m_pData = static_cast<void *>(m_pData);
  m_pBlockInfo->m_uBlockSize = uBlockSize; // save block size.
  m_pBlockInfo->m_bIsDirty = false; // set Background flag to false
  m_pBlockInfo->m_bIsHomogenous = true;
} // Init()


/**
 * Create data chunk and initialize data chunk with a init value.
 * The first parameter specifies the size of the block. After that each 
 * item in the blockwill be initialized with the second parameter.
 * @param uuBlockSize number of elements in the block
 * @param initValue
 * @param background flag
*/
template <class Type>
inline void vxBlock<Type>::Init(uint4 uBlockSize, 
                                const Type & value, 
                                const bool bBackgroundFlag)
//*******************************************************************
{
  Init(uBlockSize);

  for (uint4 i(0); i<uBlockSize; ++i)
  {
    m_pData[i] = value; // initialize items 
  }
  m_pBlockInfo->m_bIsDirty = bBackgroundFlag;
} // Init()


/**
 * Access function for item in the block.
 * This function returns the item at position iIndex.
 * There is no checking, if the position is valid (performance reasons).
 * At is prefered over operator[] to make clear that read and write access are handled different.
 * @param iIndex position in the block
 * @return value
*/
template <class Type>
inline Type & vxBlock<Type>::At(const uint4 & iIndex) const
//*******************************************************************
{
  return(m_pData[iIndex]); // return data at index 
}

/**
 * Access function to data pointer
 * @return pointer to data
 */
template <class Type>
inline Type * vxBlock<Type>::GetDataPtr()
//*******************************************************************
{
  return m_pData;
}

/**
 * Ensure that the vxBlock is unshared.
 * If the data in the block is non shared, nohing happens.
 * If the data in the block is shared, the whole data will be copied
 * and the reference counter of the other copy will be decreased. 
 * @param k positions in the block
 * @param value value 
 */
template <class Type>
inline void vxBlock<Type>::UnShare()
//*******************************************************************
{
  // there is only one copy of this block -> already unshared
  if (IsUnShared() == true)
    return;

  // save old blockInfo object
  vxBlockInfo * pOldBlockInfo = m_pBlockInfo;

  // save old data pointer
  Type * pOldData = m_pData; 

  // create new data block 
  Init( m_pBlockInfo->m_uBlockSize );

  // copy old data
  for( uint4 k(0); k < m_pBlockInfo->m_uBlockSize; ++k )
  {
    m_pData[k] = pOldData[k];
  }

  // about to unshare block -> clone
  LONG iRefCount = DecreaseReferenceCounter( pOldBlockInfo );
  //_ASSERT( iRefCount >= 0 );

  // free old data if you are now the only reference
  if( iRefCount == 0 && pOldData != NULL )
    delete[] pOldData;

} // UnShare()


/**
 * Set item at position iIndex in the block data with value.
 * @param value 
 * @param k positions in the block
 */
template <class Type>
inline void vxBlock<Type>::Set(const Type & value, 
                               const uint4 & uIndex) 
//*******************************************************************
{
  if (uIndex < m_pBlockInfo->m_uBlockSize) //check if position is valid
  { 
    if (m_pData[uIndex] != value)  // if value is already the same there is no need to set it
    {
      UnShare(); // make block writeable
      m_pBlockInfo->m_bIsDirty = true;
      m_pBlockInfo->m_bIsHomogenous = false;
      m_pData[uIndex] = value;               
    }    
  }
  else
  {
    throw ex::VException(LogRec("Index greater than block size", "Block<Type>","Set"));
  }
} // Set()


/**
 * Returns the position of the first item which contains value.
 * Return: If succeed, the positions will be returned otherwise 0xFFFFFFFF
 * @param value value
 * @param iIndex position in data chunk
 * @return index
*/
template <class Type>
inline uint4 vxBlock<Type>::FindFirstOf(const Type value, 
                                        const uint4 iIndex) const
//*******************************************************************
{
  if (iIndex < m_pBlockInfo->m_uBlockSize)
  {
    for (uint4 i(iIndex); i < m_pBlockInfo->m_uBlockSize; ++i)
    {
      if (m_pData[i] == value)
      {
        return i;
      }
    }
    return 0xFFFFFFFF;
  } 
  else
  {
    return 0xFFFFFFFF;
  }
} // FindFirstOf()


/**
 * Returns the position of the first item which not contains value.
 * Return: If succeed, the positions will be returned otherwise 0xFFFFFFFF
 * @param value 
 * @param position in data chunk
 * @return index
*/
template <class Type>
inline uint4 vxBlock<Type>::FindFirstNotOf(const Type value, 
                                           const uint4 iIndex) const
//*******************************************************************
{
  if (iIndex < m_pBlockInfo->m_uBlockSize)
  {
    for (uint4 i(iIndex); i < m_pBlockInfo->m_uBlockSize; ++i)
    {
      if (m_pData[i] != value)
      {
        return i;
      }
    }
    return 0xFFFFFFFF;
  } 
  else
  {
    return 0xFFFFFFFF;
  }
} // FindFirstNotOf()


/**
 * Resize the block chunk
 * @param uBlockSize size of vxBlock
 * @return successful
 */
template <class Type>
inline bool vxBlock<Type>::Resize(uint4 uBlockSize)
//*******************************************************************
{
  bool bSuccess(false);

  if (uBlockSize < 0xFFFFFFFF)
  {
    if (m_pBlockInfo->m_uBlockSize != uBlockSize)
    {    
      Destroy(); // carefully destroy current data
      Init(uBlockSize);
    }
    bSuccess = true;
  } 
  return bSuccess;
} // Resize()


/**
 * Return size
 * @return size
 */
template <class Type>
int4 vxBlock<Type>::GetSize() const
//*******************************************************************
{
  return m_pBlockInfo->m_uBlockSize;
}


/**
 * Merge two blocks.
 * This functions compares two blocks. If the blocks
 * have the same items. The data in the this-block will be (carefully) destroyed
 * and after that, the data of the other-block will be assigned to the this-block.
 * Carefully destroyed means, the data will only be destroyed if no other block
 * is pointing on it.
 * @param other other block
 * @return success
 */
template <class Type>
inline bool vxBlock<Type>::Merge(const vxBlock<Type> & other)
//*******************************************************************
{
  bool bSuccess(true);

  if (m_pData != other.m_pData) //Are they already merged.
  {
    if (DeepCmp(other)) // If isEqual is still true, merge is possible, so merge the two blocks 
    {
      Destroy();
      m_pData = other.m_pData; // assign pointer to the data in the other block.
      IncreaseReferenceCounter();// increase refernce counter, block is now shared.
    } 
    else
    {
      bSuccess = false;
    }
  } 
  return bSuccess;
} // Merge()


/**
 * Compares the two data chunks of the blocks
 * If every item in the data chunk is equal,
 * @param other block
 * @return successful
 */
template <class Type>
inline bool vxBlock<Type>::DeepCmp(const vxBlock<Type> & other)
//*******************************************************************
{
  bool bIsEqual(false);

  if (m_pData != other.m_pData)
  {    
    if (m_pBlockInfo->m_uBlockSize == other.m_pBlockInfo->m_uBlockSize)
    {
      if (memcmp(m_pData,other.m_pData,m_pBlockInfo->m_uBlockSize * sizeof(T)) == 0)
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
} // DeepCmp()


/**
 * Checks if the whole block is homogenous, if yes the homegenous flag is set 
 * @return successful
 */
template <class Type>
const bool vxBlock<Type>::IsUpdateOfHomogenousBitPossible() const
//*******************************************************************
{
  bool bUpdatePossible(false);

  if (IsHomogenous() == false)
  {
    int4 iIndex(m_pBlockInfo->m_uBlockSize);
    Type tmp = At(0);
    while(--iIndex && (tmp == At(iIndex))){}
    if (iIndex == 0)
    {
      bUpdatePossible = true;
    }    
  }

  return bUpdatePossible;
} // IsUpdateOfHomogenousBitPossible()


/**
 * Compare two blocks
 * If the data in the blocks are shared, this function returns true,
 * otherwise false.
 * @param other block
 * @return successful
 */
template <class Type>
inline bool vxBlock<Type>::operator==(const vxBlock<Type> & other) const
//*******************************************************************
{
  return (m_pData==other.m_pData);  // if the data pointer is the same, the rest should be the same.
} 


/**
 * Compare two blocks.
 * If the data in the blocks are shared, this function returns false,
 * otherwise true.
 * @param other block
 * @return successful
 */
template <class Type>
inline bool vxBlock<Type>::operator!=(const vxBlock<Type> & other) const
//*******************************************************************
{
  return(!(*this==other));
}


/**
 * Return const pointer to data of block.
 * This functions returns a const pointer to the data in the block
 * You should never modify the data, because the data is may be shared.
 * This function should only be used for read issues.
 * For assigning you should use the assign function.
 * @return raw data pointer
*/
template <class Type>
inline const Type * vxBlock<Type>::Raw() const
//*******************************************************************
{
  return m_pData; 
}


/**
 * Assigns one vxBlock to another.
 * This functions carefully destroys the current data.
 * Then the data pointer of the other block will be assigned
 * to the data pointer of this-block. After that the Refercence
 * Counter will be increased. Now the data is shared.
 * @param other block
 * @return block
 */
template <class Type>
inline const vxBlock<Type> & vxBlock<Type>::operator=(const vxBlock<Type> & other)
//*******************************************************************
{
  if (other.m_pData == m_pData) // if other is the same than this return.
  {
    return *this;
  }
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  Destroy(); // delete old data
  m_pData = other.m_pData; // copy pointer to data from other to this.
  m_pBlockInfo = other.m_pBlockInfo; // copy pointer to data from other to this.
  IncreaseReferenceCounter();// increase reference counter, since data in the block is now shared.

  return *this;
} // operator=()


/**
 * Returns Background flag.
 * If you create a new block the data is not Background.
 * If you use the assign() or set function, the data
 * status will be change to Background.
 * @return successful
 */
template <class Type>
inline const bool & vxBlock<Type>::IsDirty() const
//*******************************************************************
{
  return m_pBlockInfo->m_bIsDirty; // return dirty flag
}


/**
 * Returns binary histogram
 * @return histogram
 */
template <class Type>
inline const int4 vxBlock<Type>::GetBinaryHistogram() const
//*******************************************************************
{
  return m_pBlockInfo->m_iBinaryHistogram;
}


/**
 * Returns Background flag.
 * If you create a new block the data is not Background.
 * If you use the assign() or set function, the data
 * status will be change to Background.
 * @return successful
 */
template <class Type>
inline const bool & vxBlock<Type>::IsHomogenous() const
//*******************************************************************
{
  return m_pBlockInfo->m_bIsHomogenous; // return homogeneous flag
}


/**
 * Carefully destroys data in the block.
 * If the data is not shared, the data will be deleted,
 * otherwise the reference counter will be decreased.
 */
template <class Type>
inline void vxBlock<Type>::Destroy() 
//*******************************************************************
{
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  
  // decrease reference count for the block
  const LONG iRefCount = DecreaseReferenceCounter( m_pBlockInfo );

  // free memory if no other block is accessing it
  if( iRefCount == 0 )
  {
    if (m_pData != NULL)
    {
      delete[] m_pData;
      m_pData = NULL;                
    }      
    m_pBlockInfo = NULL;
  }
  else if( iRefCount < 0 )
  {
    LogDbg( "Destroy (Invalid): " + ToAscii( GetCurrentThreadId() ), "", "" );
    //_ASSERT( m_pBlockInfo->m_iRefCount < 0 );

    /// fix reference count and write error message, but do not throw
    LONG iRefCount = 12345;
    if (m_pBlockInfo != NULL)
    {
      iRefCount = m_pBlockInfo->m_iRefCount;
      m_pBlockInfo->m_iRefCount = 1;
    }
  }
} // Destroy()


/**
 * Increases the reference counter
 */
template <class Type>
inline void vxBlock<Type>::IncreaseReferenceCounter()
//*******************************************************************
{
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  InterlockedIncrement( &m_pBlockInfo->m_iRefCount );
} // IncreaseReferenceCounter()


/**
 * Decreases the reference counter
 */
template <class Type>
inline LONG vxBlock<Type>::DecreaseReferenceCounter( vxBlockInfo * pBlockInfo )
//*******************************************************************
{
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  return InterlockedDecrement( &pBlockInfo->m_iRefCount );
} // DecreaseReferenceCounter( blockInfo )


/**
 * Returns true is the vxBlock is shared, otherwise false
 */
template <class Type>
inline bool vxBlock<Type>::IsShared() const
//*******************************************************************
{
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  return(m_pBlockInfo->m_iRefCount > 1);
} // IsShared()


/**
 * Returns true is the vxBlock is unshared, otherwise false
 */
template <class Type>
inline bool vxBlock<Type>::IsUnShared() const
//*******************************************************************
{
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  return(m_pBlockInfo->m_iRefCount == 1);
} // IsUnShared()


/**
 * Returns if the reference counter is valid, this means if the reference counter is still greater than 0
 */
template <class Type>
inline bool vxBlock<Type>::IsReferenceCounterValid() const
//*******************************************************************
{
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  return(m_pBlockInfo->m_iRefCount > 0);
} // IsReferenceCounterValid()


#undef FILE_REVISION


// Revision History:
// $Log: vxBlock.inl,v $
// Revision 1.8  2007/11/03 00:02:40  jmeade
// Merge_from_CON_30_on_20071102
//
// Revision 1.4.2.1  2007/10/22 20:14:42  gdavidson
// Issue #5510: Fixed creation of the blocks
//
// Revision 1.7  2007/09/17 18:21:29  romy
// fixed the  CriticalSection not locking issue.
//
// Revision 1.6  2007/09/13 19:43:33  gdavidson
// Added synchronization to IncrementReferenceCounter and modified the Unshare method
//
// Revision 1.5  2007/09/12 21:59:16  gdavidson
// Changed the creation of the blocks
//
// Revision 1.4  2006/10/12 14:23:53  geconomos
// corrected SingleLock usage
//
// Revision 1.3  2006/03/29 19:53:15  geconomos
// + added binary histogram to block info
// + changed to use new CriticalSection and SingleLock classes
//
// Revision 1.2  2005/03/23 19:25:26  michael
// delete [] --> delete[]
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.23  2003/05/17 16:22:37  michael
// reformatting
//
// Revision 1.21  2003/04/30 10:53:36  michael
// initial code review
//
// Revision 1.20  2002/11/27 17:48:26  frank
// Moved warning out of inline function.
//
// Revision 1.19  2002/11/27 17:28:54  frank
// Fewer debugging messages than before - bring back after RSNA.
//
// Revision 1.18  2002/11/27 01:26:32  ingmar
// wrapped critical section tet code in #define to avoid critical scope instance generation and if statement
//
// Revision 1.17  2002/10/17 22:25:02  ingmar
// CriticalScope -> vxCriticalScope
// CriticalSection-> vxCriticalSection
// now with static function to enable/disable thread safety
//
// Revision 1.16  2002/10/04 23:01:00  ingmar
// improved critical section as used in vxBlock
//
// Revision 1.15  2002/09/26 18:50:58  ingmar
// maked reference counting thread safe
//
// Revision 1.14  2002/09/25 21:12:30  ingmar
// LogErr -> LogWrn
//
// Revision 1.13  2002/07/23 16:22:01  ingmar
// issue 2559: removed exception and replaced it with code to fix the refcounter and enter an error message in the log
//
// Revision 1.12  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.11  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.10  2002/07/19 16:03:32  ingmar
// made log msg more descriptive
//
// Revision 1.9  2002/07/02 17:14:32  soeren
// added IsUpdateOfHomogenousBitPossible
//
// Revision 1.8.2.1  2002/07/02 20:28:40  jaddonisio
// Merge from main trunk, per Soeren changes.
//
// Revision 1.8  2002/06/05 19:13:34  soeren
// Added Homogenous Flag
//
// Revision 1.7  2002/05/17 20:38:38  frank
// Added const to some constant functions.
//
// Revision 1.6  2002/05/03 17:08:15  frank
// added Backgroundflag in constructor
//
// Revision 1.5  2002/05/02 16:30:48  soeren
// added fix for slow debug mode - blockVolume
//
// Revision 1.4  2002/04/25 00:47:54  michael
// Implemented faster initialization of blocks in BlockVolume constructor (ingmar)
//
// Revision 1.3  2002/04/11 14:56:19  ingmar
// made GetSize() const function
// fixed reference counting during Delete()
//
// Revision 1.2  2002/04/09 23:43:08  ingmar
// improved operator=
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Id: vxBlock.inl,v 1.8 2007/11/03 00:02:40 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlock.inl,v 1.8 2007/11/03 00:02:40 jmeade Exp $
