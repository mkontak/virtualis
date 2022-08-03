// $Id: VxConsole.h,v 1.6.2.1 2010/01/07 18:46:16 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VxConsole.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#pragma once

#include "vxVolumeHeader.h"
#include "File.h"


/**
 * Encloser for v3D Console functionality.
 */
namespace VxConsole  
{
// Methods
  
  /// Returns full pathname of application
  std::string GetAppPathname();

  /// Gets an entry from the volume header
  std::string GetVolumeHeaderVariable(const vxVolumeHeader& volumeHeader, const std::string & sHeaderVarIdentifer);

}; // VxConsole


// $Log: VxConsole.h,v $
// Revision 1.6.2.1  2010/01/07 18:46:16  dongqing
// const declaration
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.5  2006/06/27 17:32:28  jmeade
// Read patient position from volume header.
//
// Revision 1.4  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.3  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.2  2005/08/05 15:17:32  geconomos
// removed netshare code
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.12.2.3  2004/02/05 20:52:11  mkontak
// Fixes to delete of old directory
//
// Revision 1.12.2.2  2004/02/02 20:32:44  mkontak
// Issue 3624
//
// Revision 1.12.2.1  2003/06/09 17:32:26  jmeade
// code review prep.
//
// Revision 1.12  2002/11/07 21:48:44  jmeade
// Method to OnDrop... on a single drop-files entry.
//
// Revision 1.11  2002/10/07 23:05:42  jmeade
// Replaced MFC scan date with Win32 SYSTEMTIME; FormatDateString for SYSTEMTIME.
//
// Revision 1.10  2002/09/30 23:09:51  jmeade
// Moved messages with others in VxMutex.
//
// Revision 1.9  2002/09/30 20:37:19  jmeade
// Interapp messages to ConsoleLib.
//
// Revision 1.8  2002/09/27 19:27:45  jmeade
// Cleaned up comments.
//
// Revision 1.7  2002/09/25 20:01:11  jmeade
// Moved database search function to ConsoleLib.
//
// Revision 1.6  2002/09/25 19:33:53  jmeade
// StoreStudyData changed to reflect method that gets study path and title.
//
// Revision 1.5  2002/09/17 19:54:21  jmeade
// Changed "empty" VxConsole class to a namespace.
//
// Revision 1.4  2002/09/17 15:12:06  jmeade
// Moving transfer and background process functionality to ConsoleLib.
//
// Revision 1.3  2002/07/18 21:47:45  jmeade
// Moved drop files-type functionality.
//
// Revision 1.2  2002/07/18 20:12:27  jmeade
// Moved report functionality to ConsoleLib.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxConsole.h,v 1.6.2.1 2010/01/07 18:46:16 dongqing Exp $
// $Id: VxConsole.h,v 1.6.2.1 2010/01/07 18:46:16 dongqing Exp $
