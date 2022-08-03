// $Id: v3DSystemSettings.c,v 1.8 2007/03/08 18:29:00 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Jeffrey Cohen  jcohen@viatronix.net


// includes
#include "stdafx.h"
#include "V3DSystemSettings.h"
#include "WinGLUtils.h"
#include "File.h"


// namespaces
using namespace std;
using namespace vx;

#define FILE_REVISION 1.4

/*
 * Returns the Report Header's full file Name
 * @return   the path for the report header image
 */
std::string V3DSystemSettings::GetReportHeaderFile()
{
  // get the path of the executable
  char sExePath[ _MAX_PATH ];
  GetModuleFileName( NULL, sExePath, _MAX_PATH );

  // get the drive letter and directory of executable
  char sDrive[_MAX_DRIVE];
  char sDirectory[_MAX_DIR ];
  _splitpath( sExePath, sDrive, sDirectory, NULL, NULL );

  // format the image path
  char sSettingsPath[ _MAX_PATH];
  wsprintf( sSettingsPath, "%s%s%s", sDrive, sDirectory, "config\\RptHdr.png" );
  return sSettingsPath;
} // GetReportHeaderFile()
