// $Id: SerializedBlockVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: September 2000
// Author: David dave@viatronix.com
//
// Complete history at bottom of file.

#ifndef SerializedBlockVolume_h
#define SerializedBlockVolume_h



// include declarations
#include "Block.h"



// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"



// class definition
class VX_BLOCK_VOLUME_DLL SerializedBlockVolume
{
private:
  uint4 m_uBlockByteSize;
  uint4 m_uIndexArraySize;
  uint4 m_uBlockArraySIze;
  uint4 m_uByteSize;

  uint1 * m_pBuffer;
  int4  * m_pIndex;
  uint1 * m_pBlock;

public:
  SerializedBlockVolume(uint4 uBlockByteSize, uint4 uIndexArraySize, uint4 uAllocatedBlocks)    
  : m_uBlockByteSize(uBlockByteSize),
  m_uIndexArraySize(uIndexArraySize),
  m_uBlockArraySIze(uAllocatedBlocks),
  m_uByteSize(uAllocatedBlocks * uBlockByteSize + uIndexArraySize * sizeof(int4)),
  m_pBuffer(new uint1[m_uByteSize]),
  m_pIndex(reinterpret_cast<int4 *>(m_pBuffer)),
  m_pBlock(reinterpret_cast<uint1 *>(m_pBuffer + m_uIndexArraySize * sizeof(int4)))
  {
  }

  ~SerializedBlockVolume()
  {
    delete [] m_pBuffer;
  }

  uint1 * const GetBuffer()
  {
    return(m_pBuffer);
  }
  uint4         GetBufferSize()
  {
    return(m_uByteSize);
  }


  int4 &  Index(uint4 uIdx)
  {
    if ( uIdx < m_uIndexArraySize )
    {
      return(m_pIndex[uIdx]);
    }
    else
    {
      throw ex::VException(LogRec("out of bounds", "SerializedBlockVolume", "Index"));
    }
  }

  uint1 * Block(uint4 uIdx)
  {
    return(m_pBlock + uIdx * m_uBlockByteSize);
  }  

  /// copies block data into an internal array
  void CopyBlockIn(uint4 uDestIdx, void * const pRawBlock)
  {
    memcpy(Block(uDestIdx), pRawBlock, m_uBlockByteSize);
  }  

  /// copies block data out of an internal array
  void CopyBlockOut(void * const pRawBlock, uint4 uDestIdx)
  {
    memcpy(pRawBlock, Block(uDestIdx), m_uBlockByteSize);
  }

}; // class SerializedBlockVolume


#endif //  SerializedBlockVolume_h


#undef FILE_REVISION

// complete revision history:
// $Log: SerializedBlockVolume.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/16 11:53:31  michael
// code review
//
// Revision 1.2  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 1.1  2001/12/24 16:47:27  ingmar
// moved SerializedBlockVolume into its own file
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/SerializedBlockVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: SerializedBlockVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
