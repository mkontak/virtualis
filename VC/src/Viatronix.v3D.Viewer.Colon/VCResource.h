// $Id: VCResource.h,v 1.2 2007/03/08 18:29:00 jmeade Exp $
//
// Copyright©2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Jeffrey Meade (jmeade@viatronix.net)

// VCResource.h : main header file for the VCRESOURCE DLL
//

#if !defined(AFX_VCRESOURCE_H__2DD61A1F_A6A7_4AF8_AEAD_C402994B8D92__INCLUDED_)
#define AFX_VCRESOURCE_H__2DD61A1F_A6A7_4AF8_AEAD_C402994B8D92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"		// main symbols
#include "typedef.h"
#include "global.h"
#include "utilities.h"
#include "LanguageSupport.h"

#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Converts a string to a UNICODE (multibyte) string
 *
 * @param s   String to convert
 * @return    Converted, multibyte, string
 */
inline std::wstring widestring(const std::string &s)
{
  int iWCLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s.c_str(), -1, NULL, 0);
  if (iWCLen <= 0)    return std::wstring();
  wchar_t * pWCStr = new wchar_t[iWCLen];
  if (pWCStr == NULL) return std::wstring();
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s.c_str(), -1, pWCStr, iWCLen);
  std::wstring wsRtn(pWCStr);
  delete pWCStr;
  return wsRtn;
} // widestring()


/**
 * Loads and returns the string with given resource ID (convenience function).
 *
 * @param uID   ID of the string resource.
 * @return      The specified string.
 */
inline CString LoadResourceString(UINT uID)
{
  CString sStr;  
  if ( ! sStr.LoadString( CLanguageSupport::GetInstance().GetResourceHandle(), uID) )
  {
    LogErr("Could not load string from resource " + ToAscii(uID) + " [" + ToAscii(GetLastError())  + "] ", "","");
  }
  return sStr;
} // LoadResourceString()


/**
 * Loads and returns the string with given resource ID (convenience function).
 *
 * @param uID   ID of the string resource.
 * @return      The specified string.
 */
inline CString LoadResourceString(UINT iID, CString sDefault)
{
  CString sResourceString(LoadResourceString(iID));

  if ( sResourceString.IsEmpty() )
  {
    return sDefault;
  }
  else
  {
    return sResourceString;
  }
} // LoadResourceString()


/**
 * Change the first alpha letter to Upper.
 *
 * @param sStr  the string to be processed.
 * @return      the string after the processed.
 */
inline CString UpperHeadString(CString sStr)
{
  for (int4 i=0; i<sStr.GetLength(); i++)
  {
    if (isalpha(sStr.GetAt(i)))
    {
      sStr.SetAt(i, toupper(sStr.GetAt(i)));
      break;
    }
    else
      continue;
  }
  return sStr;
} // UpperHeadString()


/**
 * Change the first alpha letter to Lower.
 *
 * @param sStr  the string to be processed.
 * @return      the string after the processed.
 */
inline CString LowerHeadString(CString sStr)
{
  for (int4 i=0; i<sStr.GetLength(); i++)
  {
    if (isalpha(sStr.GetAt(i)))
    {
      sStr.SetAt(i, tolower(sStr.GetAt(i)));
      break;
    }
    else
      continue;
  }
  return sStr;
} // LowerHeadString()


/**
 * Loads and returns the string with given resource ID (first letter being upper - convenience function)
 *
 * @param uID   ID of the string resource.
 * @return      The specified string.
 */
inline CString LoadResourceStringWithUpperHead(UINT uID)
{
  CString sStr;  
  sStr.LoadString( CLanguageSupport::GetInstance().GetResourceHandle(), uID);
  sStr.MakeLower();
  return UpperHeadString(sStr);
} // LoadResourceStringWithUpperHead()


/**
 * Loads and returns the string with given resource ID (first letter being lower - convenience function)
 *
 * @param uID   ID of the string resource.
 * @return      The specified string.
 */
inline CString LoadResourceStringWithLowerHead(UINT uID)
{
  CString sStr;  
  sStr.LoadString( CLanguageSupport::GetInstance().GetResourceHandle(), uID);
  sStr.MakeUpper();
  return LowerHeadString(sStr);
} // LoadResourceStringWithLowerHead()


/**
 * returns a lower-case string of that specified
 *
 * @param sStr   string to alter
 * @return       low-case version of <sStr>
 */
inline CString LowerCaseString(CString sStr)
{
  sStr.MakeLower();
  return sStr;
} // LowerCaseString()


/**
 * returns a upper-case string of that specified
 *
 * @param sStr   string to alter
 * @return       upper-case version of <sStr>
 */
inline CString UpperCaseString(CString sStr)
{
  sStr.MakeUpper();
  return sStr;
} // UpperCaseString()


/**
 * loads a string and returns a lower-case string of that specified
 *
 * @param uID    string to alter
 * @return       low-case version of the loaded string
 */
inline CString LowerCaseString(UINT uID)
{
  CString sStr = LoadResourceString(uID);
  sStr.MakeLower();
  return sStr;
} // LowerCaseString()

/**
 * loads a string and returns a upper-case string of that specified
 *
 * @param uID    string to alter
 * @return       upper-case version of the loaded string
 */
inline CString UpperCaseString(UINT uID)
{
  CString sStr = LoadResourceString(uID);
  sStr.MakeUpper();
  return sStr;
} // UpperCaseString()


/**
 * Formats a message string based on a system error (e.g. from GetLastError())
 *
 * @param iError   The error.
 * @return         A string representation of the error.
 */
inline std::string FormatSystemErrorMessage(const uint4 iError)
{
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, iError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf, 0,NULL);
  std::string sError = LPCTSTR(lpMsgBuf);
  LocalFree( lpMsgBuf );
  return sError;
} // FormatSystemErrorMessage()

#undef FILE_REVISION

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCRESOURCE_H__2DD61A1F_A6A7_4AF8_AEAD_C402994B8D92__INCLUDED_)

// $Log: VCResource.h,v $
// Revision 1.2  2007/03/08 18:29:00  jmeade
// code standards.
//
// $Id: VCResource.h,v 1.2 2007/03/08 18:29:00 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VCResource.h,v 1.2 2007/03/08 18:29:00 jmeade Exp $
