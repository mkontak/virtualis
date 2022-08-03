// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: TcpClient.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

#include "FileServerHeader.h"
#include "NetworkStream.h"


namespace io 
{



class VX_BASE_DLL TcpClient
{

protected:

  /// Constructor
  TcpClient(const std::string & sServer, uint4 uPort);

public:

  virtual ~TcpClient();


  /// Creates the client using the server/port specified
  static std::shared_ptr<TcpClient> Create(const std::string & sServer, const uint4 uPort);

 /// Creates the client using the TransportLayer settings
  static std::shared_ptr<TcpClient> Create();


  /// Attempts to connect to the server
  bool Connect();

  /// Disconnects
  void Disconnect();


   /// Send the data to the FileServer
  uint4 Send(const char * pBuffer, const uint4 uSize );
  

  /// Send the header
  uint4 Send(const FILESERVERHEADER & header);


  /// Send the header
  uint4 Send(const FILESERVERHEADER & header, const char * buffer, const uint4 uSize, const bool bMerge = false);
 


  /// Receive data from the FileServer
  uint4 Receive(char * pBuffer, const uint4 uSize);

  /// Receive a header
  uint4 Receive(FILESERVERHEADER & header);


#pragma region properties

public:


   /// Gets the file server host
 const std::string & GetServer() const
 { return m_sServer; }

 // Gets the file server port
 const uint4 GetPort() const
 { return m_uPort; }

 /// Returns the connected flag
 const bool IsConnected() const
 { return m_bConnected; }

#pragma endregion


#pragma region fields

private:

  /// Server
  std::string m_sServer;

  /// Port
  uint4 m_uPort;


  /// Stream
  std::shared_ptr<NetworkStream> m_stream;

 
  /// Connected flag
  bool m_bConnected;

  /// Winsock initialize flag
  bool m_bInitialized;

  /// Buffer used to merge data
  std::shared_ptr<char> m_request;

  /// Request Buffer size
  uint4 m_uReqeustBufferSize;

#pragma endregion


};  

} // namespace io