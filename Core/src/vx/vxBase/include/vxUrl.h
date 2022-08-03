// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Url.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once



  
#pragma warning(push)
#pragma warning(disable:4251)

/**
  * Defines the IO Protocol 
  *
  * Local    - files are local or a UNC
  * Tcp      - files accessed through the file server set up as a regular tcp server
  * Http     - Uses the rest protocol for transport
  * Https    - Uses SSQL over http
  * Ssl      - Uses SSL over tcp
  */
typedef enum Protocols
{
  P_LOCAL,
  P_TCP,
  P_HTTP,
  P_HTTPS,
  P_SSL
} Protocols;

/**
  * Base class for working with URL's
  */
class VX_BASE_DLL Url
{

#pragma region constructors

public:

  /// Default URL ( local:// )
  Url();

  /// Construct the URL from the ursl string
  Url(const std::string & sUrl);

  /// Constructs the URL from the pieces
  Url(const Protocols & protocol, const std::string & sServer, const std::string & sPath = "", const unsigned int uPort = 80 );

  /// Copy constrctor
  Url( const Url & url );

#pragma endregion

public:

  /// Is protocol
  inline bool IsProtocol(Protocols protocol) const
  { return ( protocol == m_protocol ); } 

  const std::string & ToString() const
  { return m_sUrl; }


#pragma region proeprties

public:

  /// Gets the server
  const std::string & GetServer() const
  { return m_sServer; }
    
  /// Gets the path
  const std::string & GetPath() const
  { return m_sPath; }

  /// Gets the protocol
  const Protocols & GetProtocol() const
  { return m_protocol; }

  /// Gets the port
  const unsigned int GetPort() const
  { return m_uPort; }

#pragma endregion


#pragma region fields

private:


  /// Url
  std::string m_sUrl;

  /// Server to connect to
  std::string m_sServer;

  /// Path in server
  std::string m_sPath;

  /// Port to connect to
  unsigned int m_uPort;

  /// Protocol
  Protocols m_protocol;

#pragma endregion

};  // class Url

