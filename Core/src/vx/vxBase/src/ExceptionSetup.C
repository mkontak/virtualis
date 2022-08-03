// $Id: ExceptionSetup.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: David dave@viatronix.comt


// includes
#include "stdafx.h"
#include "ExceptionSetup.h"

// definees
#define FILE_REVISION "$Revision: 1.1.1.1 $"


// namespaces
using namespace std;


/**
 * Throw OutOfMemoryException when memory allocation fails
 */ 
void ExceptionSetup::SetNewHandler()
//*******************************************************************
{
#ifdef _WIN32
  _set_new_mode(1); // also catch for malloc()
  _set_new_handler(ExceptionSetup::NoMemory);
#endif // _WIN32
#ifdef __linux  // this won't work for UNIX flavors
  set_new_handler(ExceptionSetup::NoMemory);
#endif // __linux
} // SetNewHandler()


/**
 * Set terminate handler
 */
void ExceptionSetup::SetTerminateHandler()
//*******************************************************************
{
  set_terminate(Terminate);
} // SetTerminateHandler()


/**
 * Terminate
 */
void ExceptionSetup::Terminate()
//*******************************************************************
{
  LogWrn("Terminate handler is called, because of an unhandled exception => Calling abort!", "ExceptionSetup", "Terminate");
  abort();
} // Terminate()


/**
 * No memory condition handler.
 *
 * @param size   The amount of memory for which the offending allocation failed.
 * @return       Non-zero to allow the run-time system to retry allocation, or zero to fail allocation.
 */
#ifdef _WIN32
int ExceptionSetup::NoMemory(size_t size)
//*******************************************************************
{
  throw ex::OutOfMemoryException(LogRec("Failed to allocate enough memory, probably we run out of virtual address space.", "ExceptionSetup", "NoMemory"));
  return 0;
}
#endif // _WIN32

#ifdef __linux
void Error::NoMemory()
{
  throw ex::OutOfMemoryException();
}
#endif // __linux


#undef FILE_REVISION


// $Log: ExceptionSetup.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/06/05 17:16:21  jmeade
// code review.
//
// Revision 3.2  2003/05/12 16:31:07  michael
// code review
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:13  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:12   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:45:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:32:48   ingmar
// Initial revision.
// Revision 1.2  2001/09/13 21:06:15  soeren
// changed messages and added messages to terminate
// and out of memory handlers
//
// Revision 1.1  2001/09/02 01:15:18  soeren
// moved exceptions from 03_utils to 01_logger
//
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.4  2001/08/13 20:04:53  tushar
// removed errors due to the new logger
//
// Revision 1.3  2001/08/07 17:02:11  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.2  2001/07/24 22:32:38  tushar
// fixed a byg
//
// Revision 1.1  2001/05/22 23:52:17  dave
// Initial version of ExceptionSetup class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/ExceptionSetup.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ExceptionSetup.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
