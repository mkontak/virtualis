// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpResponse.h
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

#include "HttpEnums.h"

/// Forward declarations
class VX_BASE_DLL Buffer;


namespace http
{



/**
 * Interprets the HTTP response received from the request
 */
class VX_BASE_DLL HttpResponse
{
private:
  HttpResponse();


public:

  // Creates the response from the request.
  static std::shared_ptr<HttpResponse> Create(const std::shared_ptr<Buffer> & buffer);

  /// Try and get the attribute
  bool TryGetAttribute(const std::string & sName, std::string & sValue);

public:

  // Flag indicate that the response is failed
  const bool IsFailed() const
  { return ( m_errorCode != 0 ); }

  // Get the error message
  const std::string & GetError();

  // Gets the actual response xml <rsp ...>message</rsp>
  const std::string & GetResponse() const
  { return m_sResponse; }

  // Gets the actual message
  const std::string & GetMessage();


  /// Gets the error code
  const int & GetErrorCode() const
  { return m_errorCode; }

  /// Gets the message type
  const std::string & GetMessageFormat() const
  { return m_sFormat; }

  /// Gets the message encoding
  const Encoding & GetEncoding() const
  { return m_encoding; }

private:

  // Type of data in message
  std::string m_sFormat;

  /// Error Code
  int m_errorCode;

  // Error message
  std::string m_sError;

  // Response
  std::string m_sResponse;

  // Message
  std::string m_sMessage;

  // Message endocing
  Encoding m_encoding;
  


}; // class HttpResponse

} // namespace http


#pragma warning(pop)