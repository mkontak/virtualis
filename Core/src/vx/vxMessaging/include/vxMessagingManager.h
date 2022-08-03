// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxMessagingManager.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once

#include "vxIMessage.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace messaging
{

/**
 * Implements a message manager used for sending messages
 */
class MSG_DLL MessagingManager 
{

protected:

  /// Constructor
  MessagingManager(const std::string & sServer, const unsigned int iPort); 

public:

  /// Destructor
  ~MessagingManager();


#pragma region methods

public:

  /// Creates the messaging manager
  static std::shared_ptr<MessagingManager> Create(const std::string & sServer, const unsigned int iPort); 

  /// Send Information
  inline void Send(std::shared_ptr<IMessage> pMessage)
  { Send(pMessage->ToXml()); }

  /// Send Information
  void Send(const std::string & sMessageXml);


private:

 
  /// Connects to the network
  bool Connect();

  /// Disconnects from the SCP
  void Disconnect();

#pragma endregion


#pragma region properties

public:


  /// Gets the server information
  const std::string & GetServer() const
  { return m_sServer; }

  /// Gets the port
  const unsigned int GetPort() const
  { return m_iPort; }



#pragma endregion


#pragma region fields

private:

  /// Server to connect to
  std::string m_sServer;

  /// Port the server is listening to
  unsigned int m_iPort;

  /// Socket used
  SOCKET m_socket;

  /// Socket started flag
  bool m_bWindowsSocketStarted;

#pragma endregion

};  // class MessageingManager

}  // namespace messaging

#pragma warning(pop)