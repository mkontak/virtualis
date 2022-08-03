// $Id: VersionInfo.h,v 1.3 2007/03/08 18:27:39 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Jeffrey Meade  jmeade@viatronix.net

 
extern "C" long VersionVerifyRegisterRTF(HWND* hWnd);


// pragmas
#pragma once


// includes
#include "stdafx.h"
#include <string>

/**
 * Application version information
 */
class VersionInformation
{
public:
  /// constructor
  VersionInformation();
  /// constructor
  VersionInformation(HMODULE hModule, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);
  /// constructor
	VersionInformation(LPTSTR szFilename, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);
  /// destructor
	~VersionInformation();

  /// get version info by filename
  void GetVersionInfo(LPTSTR szFilename, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);
  /// get version info by module handle
	void GetVersionInfo(HMODULE hModule, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);

public:
  /// version info
  std::string m_sVersionInfo;

  /// file version
  std::string m_sFixedFileVersion;

  /// product version
  std::string m_sFixedProductVersion;

  /// file flags
  std::string m_sFixedFileFlags;

  /// file info structure
  VS_FIXEDFILEINFO m_fixedFileInfo;
}; // VersionInformation


// $Log: VersionInfo.h,v $
// Revision 1.3  2007/03/08 18:27:39  jmeade
// code standards.
//
// Revision 1.2  2006/01/31 14:28:04  frank
// code review
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:40:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:48   ingmar
// Initial revision.
// Revision 1.2  2001/05/24 22:23:08  jmeade
// Bug fix: 469 - Version information
//
// Revision 1.1.2.3  2001/05/24 16:05:18  jmeade
// File flags string
//
// Revision 1.1.2.2  2001/05/23 22:28:00  jmeade
// header and footer
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VersionInfo.h,v 1.3 2007/03/08 18:27:39 jmeade Exp $
// $Id: VersionInfo.h,v 1.3 2007/03/08 18:27:39 jmeade Exp $
