// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpConnection.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#include "stdafx.h"
#include "http\HttpConnection.h"
#include "http\HttpSession.h"
#include "Exception.h"
#include "vxUrl.h"

using namespace http;

/**
 * Constructor
 */
HttpConnection::HttpConnection(const std::string & sServer, INTERNET_PORT iPort, const std::shared_ptr<HttpSession> & session ) : 
HttpHandle(),
m_sServer(sServer),
m_iPort(iPort),
m_session(session)
{
}

/**
 * Creates the HTTP connection
 *
 * @param url         Url containing the server and port
 * @param  session    Http session
 *
 * @return shared connection
 */
std::shared_ptr<HttpConnection> HttpConnection::Create(const Url & url, const std::shared_ptr<HttpSession> & session)
{

  if ( ! url.IsProtocol(Protocols::P_HTTP) && ! url.IsProtocol(Protocols::P_HTTPS) )
    throw ex::InvalidDataException(LogRec("Invalid protocol in url", "HttpConnection", "Create"));

  /// Return
  return  HttpConnection::Create(url.GetServer(), url.GetPort(), (session == __nullptr ? HttpSession::Create() : session));

} // Create(const sUrl & url, std::shared_ptr<HttpSession> & session)

/**
 * Creates the HTTP connection
 *
 * @param sServer     Server name or ip
 * @param iPort       Server port
 * @param  session    Http session
 *
 * @return shared connection
 */
std::shared_ptr<HttpConnection> HttpConnection::Create(const std::string & sServer, INTERNET_PORT iPort, const std::shared_ptr<HttpSession> & session)
{

  LogDbg("Creating HttpConnection","HttpConnection","Create");
  
  /// Create
  std::shared_ptr<HttpConnection> connection = std::shared_ptr<HttpConnection>(new HttpConnection(sServer, iPort,  (session == __nullptr ? HttpSession::Create() : session) ));

  if ( connection == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate HttpConnection","HttpConnection","Create"));

    LogDbg("Attempting connection","HttpConnection","Create");

  /// Connect
  connection->Connect();


  /// Return
  return connection;

} // Create(const std::string & sServerName, INTERNET_PORT portNumber, std::shared_ptr<HttpSession> & session)

/**
 * Connect to the server/port
 */
void HttpConnection::Connect()
{

  try
  {

    int i(0);
    wchar_t wServer[512];
    RtlZeroMemory(wServer,512 * sizeof(wchar_t));
 
    // convert to wide string
    std::for_each(m_sServer.begin(), m_sServer.end(), [&wServer, &i] ( char ch ) { wServer[i++] = (wchar_t)ch; } );

    LogDbg(util::Strings::Format("Connecting to HTTP server %s:%d", m_sServer.c_str(), m_iPort), "HttpSession", "Create");

    // Attempt to connect
    HINTERNET handle = ::WinHttpConnect(m_session->GetHandle(), wServer, m_iPort, 0);


    // Check the handle
    if (  handle == NULL )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to connect to %s:%d, error (%ld)",m_sServer.c_str(), m_iPort, ::GetLastError()), "HttpConnection","Connect"));

    m_handle = handle;
  }
  catch ( ex::VException & )
  {
    throw;
  }
  catch ( ... )
  {
     throw ex::HttpException(LogRec(util::Strings::Format("Failed to connect to %s:%d, unspecified error (%ld)",m_sServer.c_str(), m_iPort, ::GetLastError()), "HttpConnection","Connect"));
  }

} // Connect()