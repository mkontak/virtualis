// $Id: ExceptionSetup.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: David (dave@viatronix.com)

/**
 * Class providing utility functions for setup of exception handling.
 */


// class definition
class VX_BASE_DLL ExceptionSetup
{
// functions
public:

  /// set new handler(throw OutOfMemoryException when memory allocation fails)
  static void SetNewHandler();

  /// have std::unexpected() call Terminate()
  static void SetTerminateHandler();

private:

  /// Function called by std::unexpected, should log error and call abort(), does not return.
  static void Terminate();

#ifdef _WIN32
  static int NoMemory(size_t size);
#endif
#ifdef __linux
  static void NoMemory();
#endif
}; // ExceptionSetup


// $Log: ExceptionSetup.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/06/05 17:16:22  jmeade
// code review.
//
// Revision 3.2  2003/05/12 16:31:07  michael
// code review
//
// Revision 3.1  2002/11/13 18:41:16  jmeade
// Header/footer corrections.
//
// Revision 3.0  2001/10/14 23:02:13  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
//    Rev 2.1   Sep 19 2001 14:29:12   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:45:38   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:32:48   ingmar
// Initial revision.
// Revision 1.2  2001/09/02 23:40:44  ingmar
// added 01_logger.h
//
// Revision 1.1  2001/09/02 01:15:18  soeren
// moved exceptions from 03_utils to 01_logger
//
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.1  2001/05/22 23:52:17  dave
// Initial version of ExceptionSetup class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ExceptionSetup.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ExceptionSetup.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
