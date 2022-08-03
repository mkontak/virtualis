// includes
#include "StdAfx.h"
#include "VersionInfo.h"

// namespaces
using namespace System;

USING_GENMANIFESTBASE_NS


/**
 * Retrieves file version information for the specified file.
 *
 * @param mpFilename  The filename to retrieve version information for.
 * @returns  Version information for the provided file.  Item1 is FileVersion, Item2 is ProductVersion.
 */
Tuple<String ^, String ^> ^ VersionInfo::RetrieveVersionInformation(String ^ mpFilename)
{
  DWORD handle = 0;
  DWORD size = GetFileVersionInfoSizeA(LPCSTR(ss(mpFilename).c_str()), &handle);
  BYTE* versionInfo = new BYTE[size];
  if (!GetFileVersionInfoA(LPCSTR(ss(mpFilename).c_str()), handle, size, versionInfo))
  {
    delete[] versionInfo;
    return gcnew System::Tuple<System::String ^, System::String ^>(System::String::Empty, System::String::Empty);
  }

  // we have version information
  UINT	len = 0;
  VS_FIXEDFILEINFO* vsfi = NULL;
  VerQueryValue(versionInfo, L"\\", (void**)&vsfi, &len);

  WORD fVersion[4], pVersion[4];
  fVersion[0] = HIWORD(vsfi->dwFileVersionMS);
  fVersion[1] = LOWORD(vsfi->dwFileVersionMS);
  fVersion[2] = HIWORD(vsfi->dwFileVersionLS);
  fVersion[3] = LOWORD(vsfi->dwFileVersionLS);
  pVersion[0] = HIWORD(vsfi->dwProductVersionMS);
  pVersion[1] = LOWORD(vsfi->dwProductVersionMS);
  pVersion[2] = HIWORD(vsfi->dwProductVersionLS);
  pVersion[3] = LOWORD(vsfi->dwProductVersionLS);

  delete[] versionInfo;

  // Return the file version and the product version.
  return gcnew Tuple<String ^, String ^>(String::Join(".", Convert::ToInt32(fVersion[0]), Convert::ToInt32(fVersion[1]), Convert::ToInt32(fVersion[2]), 
    Convert::ToInt32(fVersion[3])), String::Join(".", Convert::ToInt32(pVersion[0]), Convert::ToInt32(pVersion[1]), Convert::ToInt32(pVersion[2]), 
    Convert::ToInt32(pVersion[3])));
} // RetrieveVersionInformation( mpFilename )
