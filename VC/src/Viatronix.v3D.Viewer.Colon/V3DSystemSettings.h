// $Id: V3DSystemSettings.h,v 1.7 2007/03/08 18:29:00 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Jeffrey Cohen  (jcohen@viatronix.net)


// pragmas
#pragma once
#pragma warning(disable:4786)


// includes
#include "typedef.h"
#include <string>
#include <utility>


/**
 * Writes v1k specific information to the registry
 */
class V3DSystemSettings
{
/// methods
public:  

  ///Returns the Report Header's full file Name
	static std::string GetReportHeaderFile();
  
  /// Report numbering enum
  enum ReportNumberingEnum { eNUMBERING_NONE = 0, eNUMBERING_BOTTOM = 1 };
  enum ReportHeaderEnum { eHEADER_NONE = 0, eHEADER_BITMAP = 1, eHEADER_USER = 2 };

}; // class V3DSystemSettings


// $Log: V3DSystemSettings.h,v $
// Revision 1.7  2007/03/08 18:29:00  jmeade
// code standards.
//
// Revision 1.6  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.5  2006/09/28 22:51:17  romy
// removed the hardcoded v1k path for report
//
// Revision 1.4  2006/09/28 18:08:19  romy
// function added to get the report header file
//
// Revision 1.3  2006/08/01 18:15:34  geconomos
// Added registry entry for "report disclaimer"
//
// Revision 1.2  2006/01/31 14:27:47  frank
// code review
//
// Revision 1.1.4.1  2006/08/01 18:17:56  geconomos
// Added registry entry for "report disclaimer"
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.7  2002/07/18 19:55:18  jmeade
// Changed an MFC-String return value to an std::string.
//
// Revision 1.6  2002/06/04 18:37:01  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.3.2.4  2002/05/21 01:31:40  jmeade
// Issue 2265:  Implemented free-form text block for report header text.
//
// Revision 1.5  2002/05/29 15:44:24  jmeade
// Code standards.
//
// Revision 1.4  2002/05/15 19:23:36  jmeade
// Allowing for general use of registry (and not just Software\AppName\...).
//
// Revision 1.3  2002/05/13 20:52:33  mkontak
// Added Viatronix Global Database DSN for XP
//
// Revision 1.2  2002/04/26 20:01:18  jmeade
// Explorer studies share.
//
// Revision 1.1  2002/04/18 18:08:37  jmeade
// Moved/renamed ViatronSettings class to v3DSystemSettings.
//
// Revision 3.5  2002/03/28 18:48:55  jmeade
// Moved RegistrySetting class.
//
// Revision 3.4  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.3.2.2  2002/02/22 23:09:05  jmeade
// Log level system setting.
//
// Revision 3.3.2.1  2002/02/22 23:07:44  jmeade
// Specify default values to registry functions; removed MFCString.
//
// Revision 3.3  2002/01/06 06:50:40  jmeade
// Separate study local storage directory and share (for colon, calcium, etc.)
//
// Revision 3.2  2001/12/04 16:24:51  jmeade
// ViatronSettings::Setting<>GetDefault method; Moved report preferences enums to ViatronSettings class;
//
// Revision 3.1  2001/11/09 01:17:50  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.2  2001/10/28 20:48:59  jmeade
// Remainder of settings using the ViatronSettings::Setting class
//
// Revision 3.0.2.1  2001/10/26 20:49:21  jmeade
// Settings::GetSection and GetEntry methods
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.3   Sep 25 2001 22:41:20   jmeade
// Database DSN to the registry
// 
//    Rev 2.2   Sep 25 2001 11:46:50   jmeade
// Minor cleanup to Settings::Get function
// 
//    Rev 2.1   Sep 22 2001 03:09:12   jmeade
// Local store path and share; consolidating similar Get/Set code via a template class
// 
//    Rev 2.0   Sep 16 2001 23:43:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:26   ingmar
// Initial revision.
// Revision 1.8  2001/06/07 19:03:58  geconomos
// Code walkthru preparation
//
// Revision 1.7  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.6  2001/02/27 20:58:37  geconomos
// Added report header stuff
//
// Revision 1.5  2001/01/31 22:22:43  geconomos
// Added report registry entries
//
// Revision 1.4  2001/01/25 21:51:13  jcohen
// Hooks for path for local storage
//
// Revision 1.3  2001/01/18 22:33:45  jcohen
// Function for saving drive mapping
//
// Revision 1.2  2001/01/16 19:23:37  jcohen
// Change approach to drive mapping
//
// Revision 1.1  2001/01/04 23:09:41  jcohen
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/V3DSystemSettings.h,v 1.7 2007/03/08 18:29:00 jmeade Exp $
// $Id: V3DSystemSettings.h,v 1.7 2007/03/08 18:29:00 jmeade Exp $
