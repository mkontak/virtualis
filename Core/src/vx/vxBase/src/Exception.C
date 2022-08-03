// $Id: Exception.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Jeff M. jmeade@viatronix.com


// includes
#include "stdafx.h"
#include "Exception.h"
#include "File.h"
#include "Date.h"

// namespaces
using namespace std;
using namespace ex;


// defines
#define FILE_REVISION  "$Revision: 1.2 $"


/**
 * Constructor
 * @param log record
 */
VException::VException(const vxLogRecord & logRec) : 
m_logRecord(logRec),
std::exception(logRec.GetTextMessage().c_str())
//*******************************************************************
{
  //vxLogger::Message(logRec);
  vxLogFactory::Message( vxLogFactory::GetDefaultLogger(), logRec );

}


/** 
 * Test method
 */
void VException::Test()
//*******************************************************************
{
  try
  {
    throw ex::VException(LogRec("<message for log>", "VException", "Test"));
  }
  catch(ex::VException & ve)
  {
    throw ex::VException(LogRec(ve.GetLogRecord().GetTextMessage()+"caught", "VException", "Test"));
  }
} // Test()


// $Log: Exception.C,v $
// Revision 1.2  2005/07/01 12:30:07  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.6  2003/06/04 20:10:56  jmeade
// code review.
//
// Revision 3.5  2003/05/12 16:31:07  michael
// code review
//
// Revision 3.4  2002/11/13 21:46:08  jmeade
// Code compaction.
//
// Revision 3.3  2002/08/01 18:20:20  frank
// Renamed Logger to vxLogger.
//
// Revision 3.2  2002/03/08 17:15:18  michael
// further removed include dependencies
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
//    Rev 2.2   Sep 19 2001 14:29:10   liwei
// No change.
// 
//    Rev 2.1   Sep 17 2001 16:32:58   soeren
// tested pvcs
// 
//    Rev 2.0   Sep 16 2001 23:45:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:32:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/02 01:15:18  soeren
// moved exceptions from 03_utils to 01_logger
//
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.14  2001/08/13 00:24:27  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.13  2001/08/12 18:54:28  soeren
// cosmetics - Ingmar
//
// Revision 1.12  2001/08/07 17:02:10  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.11  2001/07/24 22:34:30  tushar
// fixed a byg
//
// Revision 1.10  2001/07/24 22:33:14  tushar
// fixed a byg
//
// Revision 1.9  2001/07/24 21:54:35  tushar
// fixed a byg
//
// Revision 1.8  2001/07/24 20:22:23  dmitry
// Forced constructor with vxLogRecord. vxLogRecord handling added to base class.
//
// Revision 1.7  2001/07/16 23:46:02  jmeade
// vxLogRecord implementation
//
// Revision 1.6  2001/07/12 21:13:15  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.5  2001/05/19 00:32:42  jmeade
// Changed const& to const where appropriate (storage <= int4)
//
// Revision 1.4  2001/05/09 20:09:30  soeren
// fixed some bugs
//
// Revision 1.3  2001/04/12 18:50:38  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.2  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.1  2001/04/08 21:40:18  dave
// added stream related files
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Exception.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
// $Id: Exception.C,v 1.2 2005/07/01 12:30:07 vxconfig Exp $
