// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpConnection.h
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#pragma once

#include "HttpHandle.h"


#pragma warning(push)
#pragma warning(disable:4251)

// Forward declarations
class VX_BASE_DLL Url;

namespace http
{

// Forward declarations
class VX_BASE_DLL HttpSession;



/**
 * Implements an HTTP connection
 */
class VX_BASE_DLL HttpConnection : public HttpHandle
{

#pragma region constructors

protected:

  // Constructor
  HttpConnection(const std::string & sServer, INTERNET_PORT iPort, const std::shared_ptr<HttpSession> & session );

private:

    /// Copy constructor
  HttpConnection(const HttpConnection & connection);

#pragma endregion

#pragma region methods

private:

  /// Connect
  void Connect();

public:

  static std::shared_ptr<HttpConnection> Create(const std::string & sServer, INTERNET_PORT iPort, const std::shared_ptr<HttpSession> & session = __nullptr);

  static std::shared_ptr<HttpConnection> Create(const Url & url, const std::shared_ptr<HttpSession> & session  = __nullptr);

#pragma endregion

#pragma region properties

public:

  const std::string & GetServer() const
  { return m_sServer; }

  const INTERNET_PORT GetPort() const
  { return m_iPort; }

#pragma endregion

#pragma region fields

private:

  std::shared_ptr<HttpSession> m_session;

  std::string m_sServer;


  INTERNET_PORT m_iPort;

#pragma endregion

};

}  // namespace http



#pragma warning(pop)