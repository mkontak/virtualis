// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpSession.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)


#include "stdafx.h"
#include "http\HttpSession.h"
#include "Exception.h"

using namespace http;


/**
 * Creates an HTTP session
 */
std::shared_ptr<HttpSession> HttpSession::Create()
{

  std::shared_ptr<HttpSession> session(std::shared_ptr<HttpSession>(new HttpSession()));

  if ( session == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate HttpSession","HttpSession","Create"));

  try
  {
    LogDbg("Opening HTTP session", "HttpSession", "Create");

    // Open the HTTP session
    HINTERNET handle = ::WinHttpOpen( 0, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC );

    // Make suret the sesion was opened
    if (  handle == NULL )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to create session : error (%ld)", ::GetLastError()), "HttpSession","Initialize"));
   
    LogDbg(util::Strings::Format("HTTP session handle %I64d", (__int64)handle), "HttpSession", "Create");

    session->m_handle = handle;

  }
  catch ( ex::VException & )
  {
    throw;
  }
  catch( ... )
  {
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to create session : unspecified error (%ld)", ::GetLastError()), "HttpSession","Initialize"));

  }

  return session;

} // Create()

