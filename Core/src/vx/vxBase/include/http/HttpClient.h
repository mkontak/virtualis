// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpClient.h
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#pragma once

#include "HttpHandle.h"
#include "HttpEnums.h"
#include "vxUrl.h"

#pragma warning(push)
#pragma warning(disable:4251)


/// Forward declarations
class VX_BASE_DLL Buffer;

namespace http
{
 
/// Forward declarations
class VX_BASE_DLL HttpConnection;
class VX_BASE_DLL HttpResponse;
class VX_BASE_DLL HttpRequest;



/**
 * Implements an HTTP request
 */
class VX_BASE_DLL HttpClient : public HttpHandle
{
public:


  /// Destructor
  ~HttpClient();


protected:

  HttpClient(const Url & url);

  /// Constructor
  HttpClient(const Url & url,  const std::shared_ptr<HttpConnection> & connection);

private:

  /// Copy
  HttpClient(const HttpClient & client)
  {}

public:


  /// Creates the client using the setting in the transport layer
  static std::shared_ptr<HttpClient> Create();
 
  /// Creates the client using the supplied URL
  static std::shared_ptr<HttpClient> Create(const Url & url);
 

  /// Sends the request
  std::shared_ptr<HttpResponse> Send(const std::shared_ptr<HttpRequest> & request );

  ///
  std::shared_ptr<Buffer> Send(const std::shared_ptr<Buffer> & request, const std::string & sVerb, const std::string & sPath );

  ///
  void Send(const std::shared_ptr<Buffer> & request, const std::string & sVerb, const std::string & sPath, std::shared_ptr<Buffer> & response );


  /// Send async
  virtual void Send(const std::shared_ptr<HttpRequest> & request, LPVOID callbackData, std::function< void ( const HttpEvent, std::shared_ptr<Buffer> & ,  LPVOID )> callbackFunction);

  static std::string ToString(HttpVerb verb);

protected:

  /// Opens the request ( Data contains extra path information for a GET )
  void Open(const std::string & sVerb, const std::string & sPath);


  /// Async callback used to receive data
  static void __stdcall AsyncCallback(HINTERNET handle, DWORD_PTR context, DWORD code, void * info, DWORD length);

  /// Performs the callback using the supplied parameters
  void Callback( const HttpEvent evenet, const std::string & sMessage = "");

  /// Performs the callback using the supplied parameters
  void Callback( std::shared_ptr<Buffer> & buffer );

public:

  /// Gets the last response (Used for async sending)
  std::shared_ptr<HttpResponse> GetAsyncResponse();


  /// Gets the url
  const Url & GetUrl() const
  { return m_url; }

  /// Gets the server
  const std::string & GetServer() const 
  { return m_url.GetServer(); }


private:


  /// Url
  Url m_url;


  /// Response
  std::shared_ptr<Buffer> m_response;


  /// Http connection
  std::shared_ptr<HttpConnection> m_connection;

  /// Callback data
  LPVOID m_callbackData;

  /// Callback function
  std::function< void ( const HttpEvent, std::shared_ptr<Buffer> & ,  LPVOID ) > m_callbackFunction;


};  // class HttpClient





} // namespace http


#pragma warning(pop)