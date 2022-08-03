// $Id: MemoryBufferPool.inl,v 1.3 2007/03/07 19:55:26 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)


/**
 * Gets the next available buffer from the pool
 *
 * @return next available MemoryBuffer
 */
MemoryBuffer * MemoryBufferPool::GetNextAvailableBuffer()
{
  // try and find the next available buffer
  MemoryBuffer * pMemoryBuffer( NULL );
  for( unsigned int i( 0 ); i < m_buffers.size(); ++i )
  {
    // is the buffer available (first item in the pair indicates this)
    if( m_buffers[ i ].first )
    {
      // grab a pointer to the MemoryBuffer object
      pMemoryBuffer = m_buffers[ i ].second;

      // indicate the buffer is no longer avaiable
      m_buffers[ i ].first = false;
      break;
    }
  }
  return pMemoryBuffer;
} // GetNextAvaiableMemoryBuffer()


// $Log: MemoryBufferPool.inl,v $
// Revision 1.3  2007/03/07 19:55:26  gdavidson
// Code review
//
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/03/29 19:34:21  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/MemoryBufferPool.inl,v 1.3 2007/03/07 19:55:26 gdavidson Exp $
// $Id: MemoryBufferPool.inl,v 1.3 2007/03/07 19:55:26 gdavidson Exp $
