// $Id: BlockVector.h,v 1.3 2006/02/09 14:01:50 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: Aug. 2001
// Author: Ingmar & Soeren ingmar@viatronix.com
//
// Complete history at bottom of file.

#ifndef BlockVector_h
#define BlockVector_h

// includes
#include "Block.h"

/**
 * The BlockVector has similar functionality to std::vector<Block<T> >.
 * However, in particular when resizing it, the reference counting of 
 * blocks may get messed up, depending on the STL implemetation of the system
 * To make it safe (and also faster) we reimplemented the vector class
 * specifically for the use of reference counted blocks.
 */
template<class T>
class BlockVector
{
public:
  /// Constructor
  inline BlockVector();

  /// Destructor
  inline ~BlockVector();

  /// Resize vector of blocks and initialize with reference counted copy of default block (second argument)
  inline void Resize(uint4 uSize, const Block<T> & block);

  /// The current size of the vector
  inline uint4 Size()  const;

  /// read access to vector element at index
  inline const Block<T> & operator[](size_t uIndex) const;
  
  /// write access to vector element at index
  inline       Block<T> & operator[](size_t uIndex);	

  /// Clear the whole BlockVector
  inline void  Clear();

  typedef Block<T> * iterator;
  
private:
  /// Destroy the content of the BlockVector
  inline void Destroy();

  /// Pointer to blocks
  Block<T> * m_pBlocks;
  uint4 m_uSize;

}; // class BlockVector
#include "BlockVector.inl"

#endif // BlockVector_h

// $Log: BlockVector.h,v $
// Revision 1.3  2006/02/09 14:01:50  geconomos
// updated external libraries
//
// Revision 1.2  2005/04/27 17:36:12  geconomos
// no longer exporting this class ( soon to be removed )
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/16 11:44:24  michael
// code review
//
// Revision 3.1  2001/12/24 16:46:19  ingmar
// added class Sec_30_volume to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:00   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.1  2001/08/28 23:58:28  ingmar
// std::vector<Block<T >, BlockAllocator<Block<T> > > -> BlockVector<T>
//
//
// $Id: BlockVector.h,v 1.3 2006/02/09 14:01:50 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockVector.h,v 1.3 2006/02/09 14:01:50 geconomos Exp $