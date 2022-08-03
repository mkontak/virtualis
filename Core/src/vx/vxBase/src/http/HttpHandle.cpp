// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpHandle.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#include "stdafx.h"
#include "http\HttpHandle.h"
#include "Exception.h"
#include <assert.h>

using namespace http;

/**
 * Constructor
 */
HttpHandle::HttpHandle() :
m_handle(NULL)
{
} // HttpHandle()

/**
 * Destructor 
 *
 * Makes sure the handle is closed
 */
HttpHandle::~HttpHandle()
{
  Close();
} // ~HttpHandle()



/**
 * Closes the handle
 */
void HttpHandle::Close()
{

  // ======================================
  // If the handle is set we close it
  // ======================================
  if ( m_handle != NULL )
  {
    LogDbg("Closing handle","HttpHandle","Close");

    // Close handle
    if ( ::WinHttpCloseHandle(m_handle) != TRUE )
      LogErr(util::Strings::Format("Failed to close HTTP handle error (%ld)",::GetLastError()), "HttpHandle", "Close");
      
    // Set the handle to 0
    m_handle = NULL;

  } // END ... If the handle is not set

}  // Close()

/**
 * Set the specified option to this handle
 *
 * @param option      Option
 * @param value       Value
 * @param length      Length
 */
void HttpHandle::SetOption(DWORD option, const void * value, DWORD length)
{

  if (!::WinHttpSetOption(m_handle, option, const_cast<void*>(value), length))
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to set option %ld, error (%ld)",option, ::GetLastError()), "HttpHandle","SetOption"));

} // SetOption(DWORD option, const void * value, DWORD length)



/**
 * Queries the specified option
 *
 * @param option      Option
 * @param value       Value
 * @param length      Length
 */
void HttpHandle::QueryOption(DWORD option, void* value, DWORD & length) const
{
  if (!::WinHttpQueryOption(m_handle, option, value, &length))
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to query option - %ld , error (%ld)",option, ::GetLastError()), "HttpHandle","QueryOption"));
}  // QueryOption(DWORD option, void* value, DWORD & length) const

