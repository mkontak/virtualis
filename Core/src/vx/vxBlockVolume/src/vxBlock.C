// $Id: vxBlock.C,v 1.7.2.1 2010/07/16 20:31:42 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "vxBlock.h"
#include "SingleLock.h"



// defines
#define FILE_REVISION "$Revision: 1.7.2.1 $"


/**
 * Get the critical section
 * @return pointer to critical section
 */
CriticalSection * vxBlockStatic::GetCriticalSection()
//*******************************************************************
{
  static CriticalSection criticalSection;

  if (IsThreadSafe() == true)
    return &criticalSection;
  else
    return 0;
} // GetCriticalSection()


/**
 * Is it thread safe?
 * @return successful
 */
bool & vxBlockStatic::IsThreadSafe()
//*******************************************************************
{
  static bool bIsThreadSafe(true);

  return bIsThreadSafe;
} // IsThreadSafe()


/**
 * Get new block info
 * @return pointer to block info
 */
vxBlockInfo * vxBlockStatic::GetNewBlockInfo()
//*******************************************************************
{
#ifdef CRITICAL_SECTION_POSSIBLE
  SingleLock< CriticalSection > lock( GetCriticalSection(), true );
#endif
  
  static std::vector<vxBlockInfo> vBlockInfo = std::vector<vxBlockInfo>(0);
  static LONG iBlockInfoIndex = -1;

  if (vBlockInfo.empty())
  {
    const uint4 uSize( (512/16*512/16*512/16) * 32); // 16MB
    //const uint4 uSize( (64*64*64) * 64); // 16MB
    vBlockInfo.resize( uSize );
    
    memset( reinterpret_cast< uint1 * >( &( *vBlockInfo.begin() ) ), 0, uSize * sizeof ( vxBlockInfo ) );
  } // block info empty 


  // two passes before determining that we are out of memory
  for( int4 i = 0; i < 2; ++i )
  {
    // iterate over the blocks in search of an unused one
    for( LONG index = InterlockedIncrement( &iBlockInfoIndex ); index < vBlockInfo.size(); index = InterlockedIncrement( &iBlockInfoIndex ))
    {
      vxBlockInfo * pBlockInfo = &vBlockInfo[ index ];
      if( pBlockInfo->m_iRefCount == 0 )
      {
        pBlockInfo->m_iRefCount = 1;
        pBlockInfo->m_uBlockSize = 0;
        pBlockInfo->m_bIsDirty = 0;
        pBlockInfo->m_bIsHomogenous = 0;
        pBlockInfo->m_pData = NULL;

        //_ASSERT( pBlockInfo->m_iRefCount == 1 );
        return pBlockInfo;
      }
    }

    // reset the index just before the beginning of the vector
    InterlockedExchange( &iBlockInfoIndex, -1 );
  }

  throw ex::VException(LogRec("ref count array is too small (need to change source code)","Block::BlockInfo","GetNewBlockInfo"));
} // GetNewBlockInfo()


#undef FILE_REVISION


// Revision History:
// $Log: vxBlock.C,v $
// Revision 1.7.2.1  2010/07/16 20:31:42  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7  2007/11/03 00:02:37  jmeade
// Merge_from_CON_30_on_20071102
//
// Revision 1.4.2.1  2007/10/22 20:14:42  gdavidson
// Issue #5510: Fixed creation of the blocks
//
// Revision 1.6  2007/09/17 18:21:29  romy
// fixed the  CriticalSection not locking issue.
//
// Revision 1.5  2007/09/12 21:59:16  gdavidson
// Changed the creation of the blocks
//
// Revision 1.4  2006/10/12 14:23:53  geconomos
// corrected SingleLock usage
//
// Revision 1.3  2006/03/29 20:13:24  geconomos
// + memset initialization of static memory
// + added binary histogram to block info structure
//
// Revision 1.2  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2003/05/17 16:22:37  michael
// reformatting
//
// Revision 1.12  2003/04/30 10:53:36  michael
// initial code review
//
// Revision 1.11  2002/11/27 01:26:32  ingmar
// wrapped critical section tet code in #define to avoid critical scope instance generation and if statement
//
// Revision 1.10  2002/10/17 22:25:02  ingmar
// CriticalScope -> vxCriticalScope
// CriticalSection-> vxCriticalSection
// now with static function to enable/disable thread safety
//
// Revision 1.9  2002/10/04 23:01:00  ingmar
// improved critical section as used in vxBlock
//
// Revision 1.8  2002/09/27 18:26:04  ingmar
// now using CriticalScope for thread safety
//
// Revision 1.7  2002/09/27 16:37:32  ingmar
// removed non-working InterlockedCompareExchange() from MS VC++6.0 part
//
// Revision 1.6  2002/09/26 20:16:33  jaddonisio
// Conditional compile 'InterlockedCompareExchange' function call(prototype change in SDK 2000).
//
// Revision 1.5  2002/09/26 18:50:58  ingmar
// maked reference counting thread safe
//
// Revision 1.4  2002/06/05 19:13:33  soeren
// Added Homogenous Flag
//
// Revision 1.3  2002/05/23 16:27:20  ingmar
// reduced the blockinfo struct pool to the equivalent of 32 volumes of size 512^3
//
// Revision 1.2  2002/04/04 19:08:22  ingmar
// changed syntax, related to VC70
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Id: vxBlock.C,v 1.7.2.1 2010/07/16 20:31:42 dongqing Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlock.C,v 1.7.2.1 2010/07/16 20:31:42 dongqing Exp $
