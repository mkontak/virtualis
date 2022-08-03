// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpClient.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#include "stdafx.h"
#include "http\HttpClient.h"
#include "http\HttpConnection.h"
#include "Exception.h"
#include "vxBuffer.h"
#include "http\HttpResponse.h"
#include "http\HttpRequest.h"
#include "http\HttpSession.h"
#include "http\HttpConnection.h"
#include "vxTransportLayer.h"
#include "vxUrl.h"
#include "strsafe.h"


using namespace http;


/**
 * Constructor
 */
HttpClient::HttpClient(const Url & url, const std::shared_ptr<HttpConnection> & connection) :
HttpHandle(),
m_url(url),
m_response(Buffer::Create()),
m_connection(connection)
{
}


/**
 * Destructor
 *
 * Detach the callback
 */
HttpClient::~HttpClient()
{
  WinHttpSetStatusCallback(m_handle, NULL, NULL, NULL);
} // ~HttpClient()


/**
 * Create the client using  
 */
std::shared_ptr<HttpClient> HttpClient::Create()
{

  if ( ! io::TransportLayer::GetCurrentTransportLayer().IsProtocol(Protocols::P_HTTP) && ! io::TransportLayer::GetCurrentTransportLayer().IsProtocol(Protocols::P_HTTPS) )
    throw ex::InvalidDataException(LogRec("Cannot create http client, invalid transport layer","HttpFilePtr","Create"));


  return Create( io::TransportLayer::GetCurrentTransportLayer().GetUrl());
 
} // Create()

/**
 * Creates the Http request
 *
 * @param Path        Utl path
 * @param verb        HTTP verb
 * @param connection  Http connection
 *
 * @return Request
 */
std::shared_ptr<HttpClient> HttpClient::Create( const Url & url )
{


  std::shared_ptr<HttpClient> request = std::shared_ptr<HttpClient>(new HttpClient(url, HttpConnection::Create(url.GetServer(), (INTERNET_PORT)url.GetPort())) );

  if ( request == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate HttpClient","HttpClient","Create"));

  return request;

} // Create(   const std::string & sPath,  const HttpVerb & verb, std::shared_ptr<HttpConnection> & connection )

/**
 * Opens the request
 *
 * @param verb      HTTP verb (GET,POST,DELETE,PUT)
 * @param sPath     Extended path information   
 */
void HttpClient::Open(const std::string & sVerb, const std::string & sPath)
{

  try
  {
    // Extend the path with the extra data when not doing a POST
    std::string sUrlPath(  m_url.GetPath() + sPath );

    /// Append the tailing /
    //if (  sUrlPath.back() != '/' ) 
    //  sUrlPath.push_back('/');

    int i(0);
    wchar_t wPath[1024];
    RtlZeroMemory(wPath,1024 * sizeof(wchar_t)); 
    std::for_each(sUrlPath.begin(), sUrlPath.end(), [&wPath, &i] ( char ch ) { wPath[i++] = (wchar_t)ch; } );


    i = 0;
    wchar_t wVerb[1024];
    RtlZeroMemory(wVerb,1024 * sizeof(wchar_t)); 
    std::for_each(sVerb.begin(), sVerb.end(), [&wVerb, &i] ( char ch ) { wVerb[i++] = (wchar_t)ch; } );

    LogDbg(util::Strings::Format("Opening HTTP %s:%s",sVerb.c_str(), sUrlPath.c_str()), "HttpClient", "Open");



    LPCWSTR types[3] = { L"application/octet-stream" , L"text/*", NULL};



    HINTERNET hRequest = WinHttpOpenRequest(m_connection->GetHandle(), 
                                            wVerb, 
                                            wPath, 
                                            0, 
                                            WINHTTP_NO_REFERER, 
                                            WINHTTP_DEFAULT_ACCEPT_TYPES, 
                                            /*(LPCWSTR *)&types,*/
                                            ( m_url.IsProtocol(Protocols::P_HTTPS) ? WINHTTP_FLAG_SECURE : NULL ) );
 
    if ( ! hRequest )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to open path (%s), error (%ld)", m_url.ToString().c_str(), ::GetLastError()), "HttpClient", "Open"));

    m_handle = hRequest;

  }
  catch ( ex::HttpException & )
  {
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to open path (%s), unspecified error (%ld)", m_url.ToString().c_str(), ::GetLastError()), "HttpClient", "Open"));
  }


} // Open()


/**
 * Performs an Async send
 *
 * @param sData             Data to be sent
 * @param calblBackdata     Callback data
 * @param callbackFunction  Callback function
 *
 */
void HttpClient::Send(const std::shared_ptr<HttpRequest> & request,  LPVOID callbackData, std::function< void ( const HttpEvent, std::shared_ptr<Buffer> & ,  LPVOID )> callbackFunction)
{

  try
  {

    /// Release the previous response
    m_response->Release();

    // Set the callback information
    m_callbackData      = callbackData;
    m_callbackFunction  = callbackFunction;

    // Install the status callback function.
    if ( WinHttpSetStatusCallback( m_handle, (WINHTTP_STATUS_CALLBACK)AsyncCallback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, NULL) == WINHTTP_INVALID_STATUS_CALLBACK  )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to set status callback function, error (%ld)", ::GetLastError()), "HttpClient", "Send"));

    // Send the request
    Send(request);

  }
  catch  ( ex::HttpException & )
  {
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to send asynchronously, error unspecified (%ld)", ::GetLastError()), "HttpClient", "Send"));

  }

} // Send(const std::string & sData, LPVOID callbackData, std::function< void ( const HttpEvent, std::shared_ptr<Buffer> & ,  LPVOID )> callbackFunction)

/**
 * Opens the http connection and send the request
 *
 * @param verb        Http verb
 * @param requestData Data to be sent  
 */
std::shared_ptr<HttpResponse> HttpClient::Send(const std::shared_ptr<HttpRequest> & request)
{

  /// Response buffer
  std::shared_ptr<Buffer> response;

  try
  {

    /// Creates the response buffer
    response = Buffer::Create();

    // Open the connection
    Open(request->GetVerbAsString(), request->GetPath());

    // Get the data
    std::shared_ptr<Buffer> message(request->GetRequest());

    const unsigned char * pMessage(message->GetBuffer().get());
    const size_t iMessageSize(message->GetLength());

    LogDbg(util::Strings::Format("Attempting to send message (%s) to (%s) ",request->ToString().c_str(), m_url.ToString().c_str()),"HttpClient", "SendRequest");
  
    // =====================
    // Send the request
    // =====================
    if ( !WinHttpSendRequest( m_handle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
                              ( iMessageSize > 0 ? (LPVOID)pMessage : WINHTTP_NO_REQUEST_DATA),
                              (DWORD)iMessageSize, (DWORD)iMessageSize, NULL) )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to send request, error (%ld)", ::GetLastError()), "HttpClient", "SendRequest"));




    // ===================
    // wait for response
    // ===================
    if( ! WinHttpReceiveResponse( m_handle, NULL ) )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to waiting for responser (%ld)", ::GetLastError()), "HttpClient", "Send"));
 

    DWORD dwHttpByteRead(0);
    DWORD dwTotalSize(0);
    DWORD dwOffset(0);
    DWORD dwAvailableData(0);

    do 
    {

      // Rest the available data
      dwAvailableData = 0;

      // Query to find if any data is available
      if( ! WinHttpQueryDataAvailable( m_handle, &dwAvailableData ) )
          throw ex::HttpException(LogRec(util::Strings::Format("Query for available data failed, error (%ld)", ::GetLastError()), "HttpClient", "Send"));

      LogDbg(util::Strings::Format("%d bytes available to receive for http response",dwAvailableData),"HttpClient", "Send");

      if ( dwAvailableData > 0 )
      {

        dwTotalSize += dwAvailableData;
        
        response->Allocate(dwTotalSize);

        if (!WinHttpReadData(m_handle, (LPVOID)response->GetData(dwOffset), dwAvailableData, &dwHttpByteRead))
            throw ex::HttpException(LogRec(util::Strings::Format("Failed to read data, error (%ld)", ::GetLastError()), "HttpClient", "Send"));

        LogDbg(util::Strings::Format("%d bytes read from http response",dwAvailableData),"HttpClient", "Send");

        response->SetLength(dwHttpByteRead);

        dwOffset = dwHttpByteRead;
         
      }

    } while( dwAvailableData > 0 );

    return HttpResponse::Create(response);

  }
  catch ( ex::HttpException & )
  {
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to send request, unspecified error (%ld)", ::GetLastError()), "HttpClient", "SendRequest"));
  }

} // SendRequest(HttpVerb verb, const HttpRequest & requestData)

/**
 * Opens the http connection and send the request
 *
 * @param verb        Http verb
 * @param requestData Data to be sent  
 */
std::shared_ptr<Buffer> HttpClient::Send(const std::shared_ptr<Buffer> & request, const std::string & sVerb, const std::string & sPath )
{


  try
  {


    // Open the connection
    Open(sVerb, sPath);

    const unsigned char * pMessage(request->GetData());
    const size_t iMessageSize(request->GetLength());

    LogDbg(util::Strings::Format("Attempting to send message to (%s) ", m_url.ToString().c_str()),"HttpClient", "Send");
  
    if ( !WinHttpSendRequest( m_handle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
                              ( iMessageSize > 0 ? (LPVOID)pMessage : WINHTTP_NO_REQUEST_DATA), (DWORD)iMessageSize, (DWORD)iMessageSize, NULL) )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to send request, error (%ld)", ::GetLastError()), "HttpClient", "SendRequest"));

    // ===================
    // wait for response
    // ===================
    if( ! WinHttpReceiveResponse( m_handle, NULL ) )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to waiting for responser (%ld)", ::GetLastError()), "HttpClient", "Send"));
 

    DWORD dwHttpBytesRead(0);
    DWORD dwAvailableData(0);

    /// Response buffer
    std::shared_ptr<Buffer> response(Buffer::Create(2048));

    do 
    {

      // Rest the available data
      dwAvailableData = 0;

      // Query to find if any data is available
      if( ! WinHttpQueryDataAvailable( m_handle, &dwAvailableData ) )
          throw ex::HttpException(LogRec(util::Strings::Format("Query for available data failed, error (%ld)", ::GetLastError()), "HttpClient", "Send"));

      LogDbg(util::Strings::Format("%d bytes available to receive for http response",dwAvailableData),"HttpClient", "Send");

      if ( dwAvailableData > 0 )
      {
        
        if ( ! response->CanFit(dwAvailableData) )
          response->IncreaseSize(dwAvailableData);

        if( ! WinHttpReadData( m_handle, (LPVOID)response->GetData(BUFFER_END), dwAvailableData, &dwHttpBytesRead ) )
            throw ex::HttpException(LogRec(util::Strings::Format("Failed to read data, error (%ld)", ::GetLastError()), "HttpClient", "Send"));


        LogDbg(util::Strings::Format("%d bytes read from http response",dwHttpBytesRead),"HttpClient", "Send");

        response->IncrementLength(dwHttpBytesRead);

         
      }

    } while( dwAvailableData > 0 );

    return response;

  }
  catch ( ex::HttpException & )
  {
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to send request, unspecified error (%ld)", ::GetLastError()), "HttpClient", "SendRequest"));
  }

} // SendRequest(HttpVerb verb, const HttpRequest & requestData)

/**
 * Opens the http connection and send the request
 *
 * @param verb        Http verb
 * @param requestData Data to be sent  
*/
void HttpClient::Send(const std::shared_ptr<Buffer> & request, const std::string & sVerb, const std::string & sPath, std::shared_ptr<Buffer> & response )
{


  try
  {


    // Open the connection
    Open(sVerb, sPath);

    const unsigned char * pMessage(request->GetData());
    const size_t iMessageSize(request->GetLength());

    LogDbg(util::Strings::Format("Attempting to send message to (%s) ", m_url.ToString().c_str()),"HttpClient", "Send");
  
    if ( !WinHttpSendRequest( m_handle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
                              ( iMessageSize > 0 ? (LPVOID)pMessage : WINHTTP_NO_REQUEST_DATA), (DWORD)iMessageSize, (DWORD)iMessageSize, NULL) )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to send request, error (%ld)", ::GetLastError()), "HttpClient", "SendRequest"));


    // ===================
    // wait for response
    // ===================
    if( ! WinHttpReceiveResponse( m_handle, NULL ) )
      throw ex::HttpException(LogRec(util::Strings::Format("Failed to waiting for responser (%ld)", ::GetLastError()), "HttpClient", "Send"));
 

    DWORD dwHttpBytesRead(0);
    DWORD dwAvailableData(0);
    DWORD dwTotalBytesRead(0);
    unsigned char * ptr(response->GetData());


    do 
    {

      // Query to find if any data is available
      if( ! WinHttpQueryDataAvailable( m_handle, &dwAvailableData ) )
          throw ex::HttpException(LogRec(util::Strings::Format("Query for available data failed, error (%ld)", ::GetLastError()), "HttpClient", "Send"));

      if ( dwAvailableData > 0 )
      {
        
        if( ! WinHttpReadData( m_handle, (LPVOID)(ptr), dwAvailableData, &dwHttpBytesRead ) )
            throw ex::HttpException(LogRec(util::Strings::Format("Failed to read data, error (%ld)", ::GetLastError()), "HttpClient", "Send"));


        ptr += dwHttpBytesRead;
        dwTotalBytesRead += dwHttpBytesRead;


         
      }

      response->SetLength(dwTotalBytesRead);

    } while( dwAvailableData > 0 );

  }
  catch ( ex::HttpException & )
  {
    throw;
  }
  catch ( ... )
  {
    throw ex::HttpException(LogRec(util::Strings::Format("Failed to send request, unspecified error (%ld)", ::GetLastError()), "HttpClient", "SendRequest"));
  }

} // SendRequest(HttpVerb verb, const HttpRequest & requestData)


void CALLBACK HttpClient::AsyncCallback(HINTERNET handle, DWORD_PTR context, DWORD code, void * info, DWORD length)
{

  try
  {

    if ( context != 0 )
    {

      /// Cast the context to ethe request
      HttpClient * pRequest = (HttpClient *)(context);

      // ==============================
      // Switch on the callback code
      // ==============================
      switch (code)
      {

        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        {
          LogDbg("HTTP status : send request complete","HttpClient","AsyncCallback");

          if ( !::WinHttpReceiveResponse(handle, 0) )
            throw ex::HttpException(LogRec(util::Strings::Format("Failed to receive response, error (%ld)", ::GetLastError()), "HttpClient","AsyncCallback"));

          break;
        }

        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        {
          LogDbg("HTTP status : headers are available","HttpClient","AsyncCallback");

          DWORD statusCode(0);
          DWORD statusCodeSize(sizeof(DWORD));

          if ( !::WinHttpQueryHeaders( handle, 
                                       WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                       WINHTTP_HEADER_NAME_BY_INDEX,
                                       &statusCode,
                                       &statusCodeSize,
                                       WINHTTP_NO_HEADER_INDEX))
            throw ex::HttpException(LogRec(util::Strings::Format("Failed to query headers, error (%ld)", ::GetLastError()), "HttpClient","AsyncCallback"));

          // ==================
          /// Check the status
          // =====================
          if (HTTP_STATUS_OK != statusCode)
            throw ex::HttpException(LogRec(util::Strings::Format("Query headers return failed status code (%ld)", statusCode), "HttpClient","AsyncCallback"));

          //DWORD dwSize = 0;

          //if( ! WinHttpQueryDataAvailable( handle, &dwSize ) )
          //  throw HttpException(LogRec(util::Strings::Format("Query for available data failed, error (%ld)", ::GetLastError()), "HttpClient", "Send"));


          pRequest->m_response->Allocate(length);

          // Read the data
          if (!::WinHttpReadData(handle, pRequest->m_response->GetData(BUFFER_END), length, NULL)) 
            throw ex::HttpException(LogRec(util::Strings::Format("Failed to read data, error (%ld)", ::GetLastError()), "HttpClient","AsyncCallback"));

          pRequest->m_response->SetLength(length);

          break;

        }
        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
        {
          LogDbg("HTTP status : read complete","HttpClient","AsyncCallback");

          if ( length > 0 )
          {

            pRequest->Callback(pRequest->m_response);

            pRequest->m_response->Release();
            pRequest->m_response->Allocate(length);
            pRequest->m_response->Zero();


            if ( !::WinHttpReadData(handle, pRequest->m_response->GetData(), length, 0)) 
              throw ex::HttpException(LogRec(util::Strings::Format("Failed to read data, error (%ld)", ::GetLastError()), "HttpClient","AsyncCallback"));

            pRequest->m_response->SetLength(length);
           
          }
          else
            pRequest->Callback(HTTP_EVENT_READ_COMPLETED);

          break;
        }

        case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
        {

          LogDbg("HTTP status : write complete","HttpClient","AsyncCallback");

          pRequest->Callback(HTTP_EVENT_WRITE_DATA);

          if ( !::WinHttpReceiveResponse(handle, NULL) ) 
              throw ex::HttpException(LogRec(util::Strings::Format("Failed to receive response, error (%ld)", ::GetLastError()), "HttpClient","AsyncCallback"));
     
          break;
        }

        default:
          break;
      }

    } // END ... If the context is not 0

  } 
  catch ( ... )
  {
    throw ex::HttpException(LogRec(util::Strings::Format("AsyncCallback failed, unspecified error [%d]", GetLastError()), "HttpClient", "AsyncCallback"));
  }

} // AsyncCallback()


/**
 * Performs the callback
 */
void HttpClient::Callback(const HttpEvent httpEvent, const std::string & sMessage)
{

  if ( m_callbackFunction != __nullptr )
    (m_callbackFunction)(httpEvent, Buffer::Create(sMessage), m_callbackData);

} // Callback(const HttpEvent event, const std::string & sMessage)

/**
 * Performs the callback
 */
void HttpClient::Callback(std::shared_ptr<Buffer> & buffer)
{

  if ( m_callbackFunction != __nullptr )
    (m_callbackFunction)(HTTP_EVENT_READ_COMPLETED, buffer, m_callbackData);

} // Callback(const HttpEvent event, const std::string & sMessage)

/**
 * Gets the async response
 */
std::shared_ptr<http::HttpResponse> HttpClient::GetAsyncResponse()
{
  return http::HttpResponse::Create(m_response);
} // GetAsyncResponse()