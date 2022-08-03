// $Id: VersionInfo.C,v 1.3 2007/03/08 18:27:39 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Jeffrey Meade  jmeade@viatronix.net



//**********************************************************************************************
// VersionInformation is a tiny class, which wraps the stuff needed to get the version info from a
// resource file.
// It handles both, the fixed version information and the language dependent string version
// information of the resource file.
//
// It contains three CString members which hold the retrieved information:
// m_sFixedFileVersion:  the fixed file version info (language independent)
// m_sFixedProductVersion: the fixed product version info (language independent)
// m_sVersionInfo:   the desired version info string (language dependent)
//
// VersionInformation requires to be linked with VERSION.LIB!
//
//**********************************************************************************************
#include "StdAfx.h"
#include "VersionInfo.h"

/**
 * Default constructor.
 */
VersionInformation::VersionInformation()
{
  memset(&m_fixedFileInfo, 0, sizeof(VS_FIXEDFILEINFO));
} // default constructor


/**
 * Destructor
 */
VersionInformation::~VersionInformation()
{
} // default destructor


/**
 * Constructor, stores version info
 *
 * @param hModule       identifies file/app for version info
 * @param strLangId     language ID
 * @param strInfoType   type of the module
 */
VersionInformation::VersionInformation(HMODULE hModule, LPCTSTR strLangID, LPCTSTR strInfoType)
{
  memset(&m_fixedFileInfo, 0, sizeof(VS_FIXEDFILEINFO));
  GetVersionInfo(hModule, strLangID, strInfoType);
} // Constructor (hModule, strLangId, strInfoType)


/**
 * Constructor, stores version info
 *
 * @param szFilename    identifies file/app for version info
 * @param strLangId     language ID
 * @param strInfoType   type of the module
 */
VersionInformation::VersionInformation(LPTSTR szFilename, LPCTSTR strLangID, LPCTSTR strInfoType)
{
  memset(&m_fixedFileInfo, 0, sizeof(VS_FIXEDFILEINFO));
  GetVersionInfo(szFilename, strLangID, strInfoType);
} // Constructor (szFilename, strLangId, strInfoType)


/**
 * store the specified module's version information
 *
 * @param hModule       identifies file/app for version info
 * @param strLangId     language ID
 * @param strInfoType   type of the module
 */
void VersionInformation::GetVersionInfo(HMODULE hModule, LPCTSTR strLangID, LPCTSTR strInfoType)
{
  TCHAR szExeName[MAX_PATH];
  // Note: A NULL hModule is quite valid, it simply specifies the current module
  GetModuleFileName(hModule, szExeName, sizeof(szExeName));
  GetVersionInfo(szExeName, strLangID, strInfoType);
} // GetVersionInfo()


//**********************************************************************************************
// GetVersionInfo (requires VERSION.LIB!!!)
//----------------------------------------------------------------------------------------------
// Retrieves the desired version entry from the VERSIONINFO structure of the resource file.
//
// strLangID is the language ID for which the version info is desired
// 040904E4 means e.g.:
// 04------        = SUBLANG_ENGLISH_USA
// --09----        = LANG_ENGLISH
// ----04E4 = 1252 = Codepage for Windows:Multilingual
//
// strInfoType  is the desired version entry, eg.:"ProductName", or "ProductVersion"
//
// hModule ist the Instance handle of the module for which the version info should be retrieved
// with EXE's: Get hModule calling AfxGetInstanceHandle.
// with DLL's: Get hModule calling  ::GetModuleHandle ("DLLName"), where "DLLName" is the Name of the module
//
// the version information is stored in the member variables:
// m_sFixedFileVersion, m_sFixedProductVersion and m_sVersionInfo, where:
// m_sFixedFileVersion and m_sFixedProductVersion contain the fixed version info (language independent) and
// m_sVersionInfo contains the desired version info string (language dependent)
//
//**********************************************************************************************
void VersionInformation::GetVersionInfo(LPTSTR szFilename, LPCTSTR strLangID, LPCTSTR strInfoType)
{
  DWORD dwVerInfoSize;
  DWORD dwHnd;
  void* pBuffer;
  VS_FIXEDFILEINFO *pFixedInfo; // pointer to fixed file info structure
  LPVOID  lpVersion;    // String pointer to 'version' text
  UINT    uVersionLen;   // Current length of full version string
  TCHAR szGetName[500];
  dwVerInfoSize = GetFileVersionInfoSize(szFilename, &dwHnd);
  if (dwVerInfoSize)
  {
    pBuffer = malloc(dwVerInfoSize);
    if (pBuffer == NULL)
      return;
    
    GetFileVersionInfo(szFilename, dwHnd, dwVerInfoSize, pBuffer);
    
    // get the fixed file info (language-independent)
    VerQueryValue(pBuffer,_T("\\"),(void**)&pFixedInfo,(UINT *)&uVersionLen);

    memmove(&m_fixedFileInfo, pFixedInfo, sizeof(VS_FIXEDFILEINFO));

    CString sTempFmt;
    sTempFmt.Format("%u.%u.%u.%u", HIWORD(pFixedInfo->dwProductVersionMS), LOWORD(pFixedInfo->dwProductVersionMS),
      HIWORD(pFixedInfo->dwProductVersionLS), LOWORD(pFixedInfo->dwProductVersionLS));
    m_sFixedProductVersion = sTempFmt;

    sTempFmt.Format("%u.%u.%u.%u", HIWORD(pFixedInfo->dwFileVersionMS), LOWORD(pFixedInfo->dwFileVersionMS),
      HIWORD(pFixedInfo->dwFileVersionLS), LOWORD(pFixedInfo->dwFileVersionLS));
    m_sFixedFileVersion = sTempFmt;

    if (pFixedInfo->dwFileFlags & VS_FF_DEBUG)            m_sFixedFileFlags += "-Debug";
    if (pFixedInfo->dwFileFlags & VS_FF_PATCHED)          m_sFixedFileFlags += "-Patched";
    if (pFixedInfo->dwFileFlags & VS_FF_PRERELEASE)       m_sFixedFileFlags += "-PreRelease";
    if (pFixedInfo->dwFileFlags & VS_FF_PRIVATEBUILD)     m_sFixedFileFlags += "-Private";
    if (pFixedInfo->dwFileFlags & VS_FF_SPECIALBUILD)     m_sFixedFileFlags += "-Special";

    // get the string file info (language-dependend)
    if (strLangID != NULL || strInfoType != NULL)
    {
      lstrcpy(szGetName, "\\StringFileInfo\\");
      lstrcat(szGetName, strLangID);
      lstrcat(szGetName, "\\");
      lstrcat(szGetName, strInfoType);
      // copy version info, if desired entry exists
      if (VerQueryValue(pBuffer,szGetName,(void**)&lpVersion,(UINT *)&uVersionLen) != 0)
        m_sVersionInfo = (LPTSTR)lpVersion;
    }
    if (pBuffer != NULL)
      free(pBuffer);
  }
} // GetVersionInfo()


// $Log: VersionInfo.C,v $
// Revision 1.3  2007/03/08 18:27:39  jmeade
// code standards.
//
// Revision 1.2  2006/01/31 14:28:04  frank
// code review
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VersionInfo.C,v 1.3 2007/03/08 18:27:39 jmeade Exp $
// $Id: VersionInfo.C,v 1.3 2007/03/08 18:27:39 jmeade Exp $
