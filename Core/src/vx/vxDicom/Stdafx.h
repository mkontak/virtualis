// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#pragma warning(disable : 4251)
#pragma warning(disable : 4482)

#define _CRT_SECURE_NO_DEPRECATE


#include <winsock2.h>
#include <windows.h>


#include <ole2.h>
#include <atlbase.h>
#include <comutil.h>
#include <comdef.h>
#include <limits.h>
#include <Shlobj.h>
#include <ShlGuid.h>
#include <Psapi.h>
#include <agents.h>
#include <ppl.h>
#include <concurrent_queue.h>
#include <concurrent_vector.h>


#include <sys/types.h>
#include <sys/stat.h>



#include "Logger.h"
#include "DcmLibrary.h"
#include "vxBaseLibrary.h"




#ifdef UNICODE
  typedef std::wstd::string  STRING;
  typedef std::wostringstream OSTRINGSTREAM;
  typedef std::wistringstream ISTRINGSTREAM;
#else
  typedef std::string   STRING;
  typedef std::ostringstream OSTRINGSTREAM;
  typedef std::istringstream STRINGISTREAM;
#endif



typedef std::vector<xmlNodePtr> NODELIST;

/// Used for undefined exceptions
const std::string g_undefinedError("Undefined error");


/**
 * Converts the DWORD to a std::string
 * 
 *  @param dw
 *  @return the string
 */
inline const std::string ToString(DWORD dw) 
{
  std::ostringstream os;

  os << dw;

  return os.str();
} // ToString(DWORD dw) 


/**
 * Iterates through item calling the specified function until we have been cancelled
 */
template<class InputIterator, class Func>
Func for_each_infinite(InputIterator first, InputIterator last, Func f)
{

  if (first == last)
      return f;

  InputIterator i(first);
  bool cancelled(false);

  // ======================
  // Loop until cancelled
  // =====================
  do
  {
      if (i == last)
          i = first;
      cancelled = f(*i);
      ++i;
  } while (!cancelled);

  return f;

} // for_each_infinite(InputIterator first, InputIterator last, Func f)

/// Trim the specified string from the right 
inline std::string trim_right(const std::string &source , const std::string& t = " ")
{
std::string str = source;
return str.erase( str.find_last_not_of(t) + 1);
}

/// Trim the specified string from the left
inline std::string trim_left( const std::string& source, const std::string& t = " ")
{
std::string str = source;
return str.erase(0 , source.find_first_not_of(t) );
}

/// Trim the specified string from both sides
inline std::string trim(const std::string& source, const std::string& t = " ")
{
std::string str = source;
return trim_left( trim_right( str , t) , t );
} 
