#include "stdafx.h"
#include "vxHttpClient.h"
#include "vxTransportLayer.h"
#include "Exception.h"
#include "HttpSession.h"
#include "HttpConnection.h"



using namespace io;


/**
 * Creates the HttpClient using the setting in the transport layer
 *
 * @return Returns a new HttpClient
 */

std::shared_ptr<HttpClient> HttpClient::Create()
{

  if ( ! TransportLayer::GetCurrentTransportLayer().IsProtocol(http::Protocols::P_HTTP) && ! TransportLayer::GetCurrentTransportLayer().IsProtocol(http::Protocols::P_HTTPS) )
    throw ex::InvalidDataException(LogRec("Cannot create http client, invalid transport layer","HttpFilePtr","Create"));


  return Create(TransportLayer::GetCurrentTransportLayer().GetUrl());
 
} // Create()


/**
 * Creates an HttpClient
 *
 * @param url       Url
 *
 * @return Returns a new HttpClient
 */
std::shared_ptr<HttpClient> HttpClient::Create(const http::Url & url)
{

  std::shared_ptr<HttpClient> client(new HttpClient(url, http::HttpConnection::Create(url, http::HttpSession::Create())));


  if ( client == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate HttpClient", "HttpClient", "Create"));

  
  return client;

} // Create(const http::Url & url)
