// $Id: BlockVector.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
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


/*
 * Constructor.
 */
template <class T>
BlockVector<T>::BlockVector()
{
  m_pBlocks = NULL;
  m_uSize = 0;
} // Constructor


/*
 * Destructor.
 */
template <class T>
BlockVector<T>::~BlockVector()
{
  Destroy();
} // Destructor


/*
 * Resize vector of blocks and initialize with reference counted copy of default block (second argument).
*/
template <class T>
void BlockVector<T>::Resize(uint4 uSize, const Block<T> & block)
{
  if(m_uSize != uSize)
  {
    Destroy();
    m_pBlocks = new Block<T>[uSize];
    m_uSize = uSize;  
    for (uint4 k(0); k < uSize; ++k)
    {
      m_pBlocks[k] = block;
    }
  }  
} // Resize


/*
 * The current size of the vector
 */
template <class T>
uint4 BlockVector<T>::Size() const
{
  return (m_uSize); 
} // Size


/*
 * read access to vector element at index
 */
template <class T>
const Block<T> & BlockVector<T>::operator[](size_t pos) const
{
  return m_pBlocks[pos];
} // op[]


/*
 * write access to vector element at index
 */
template <class T>
Block<T> & BlockVector<T>::operator[](size_t pos)
{
  return m_pBlocks[pos]; 
} // op[]

/*
 * Clear the whole BlockVector.
 */
template <class T>
void BlockVector<T>::Clear()
{
  Destroy();
} // Clear


/*
 * Destroy the content of the BlockVector.
 */
template <class T>
void BlockVector<T>::Destroy()
{
  if(m_pBlocks != NULL) 
  {
    delete [] m_pBlocks;
  }
  m_pBlocks = NULL;
  m_uSize = 0;
} // Destroy


// $Log: BlockVector.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/05/16 11:44:24  michael
// code review
//
// Revision 3.2  2001/11/09 01:20:32  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/10/31 01:00:26  soeren
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
//    Rev 2.0   Sep 16 2001 23:47:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:00   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.2  2001/08/29 15:23:29  soeren
// changed size_t to uint4 in Size()
//
// Revision 1.1  2001/08/28 23:58:28  ingmar
// std::vector<Block<T >, BlockAllocator<Block<T> > > -> BlockVector<T>
//
//
// $Id: BlockVector.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockVector.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $=======
