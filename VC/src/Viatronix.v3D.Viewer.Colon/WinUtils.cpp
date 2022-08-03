// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: WinUtils.cpp
// Owner: Greg Davidson


#include "StdAfx.h"
#include "WinUtils.h"
#include "LanguageSupport.h"

/**
 * Loads and returns the string with given resource ID (convenience function).
 *
 * @param UINT uID         ID of the string resource.
 * @return inline CString  The specified string.
 */
CString WinUtils::LoadResourceString(UINT uID)
{
  CString sStr;  
  sStr.LoadString( CLanguageSupport::GetInstance().GetResourceHandle(), uID );  
  return sStr;
}


/**
 * Loads and returns the string with given resource ID (first letter being upper - convenience function)
 *
 * @param UINT uID         ID of the string resource.
 * @return inline CString  The specified string.
 */
CString WinUtils::LoadResourceStringWithUpperHead(UINT uID)
{
  return UpperHeadString(WinUtils::LoadResourceString(uID));
}


/**
 * Loads and returns the string with given resource ID (first letter being lower - convenience function)
 *
 * @param UINT uID         ID of the string resource.
 * @return inline CString  The specified string.
 */
CString WinUtils::LoadResourceStringWithLowerHead(UINT uID)
{
  CString sStr;  
  sStr.LoadString( CLanguageSupport::GetInstance().GetResourceHandle(), uID );
  sStr.MakeUpper();
  return LowerHeadString(sStr);
}