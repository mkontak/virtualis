// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxMessagingManager.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "Stdafx.h"
#include "vxMessagingManager.h"







using namespace messaging;

/**
 * Constructor
 */
MessagingManager::MessagingManager(const std::string & sServer, const unsigned int iPort) :
m_sServer(sServer),
m_iPort(iPort),
m_bWindowsSocketStarted(true),
m_socket(INVALID_SOCKET)
{

  DWORD rc(0);

  //Start up Winsock…
  WSADATA wsadata;
 
  // ==============================
  // Startup windows socket
  // ===============================
  if ( (rc = WSAStartup(MAKEWORD(1,1),&wsadata)) < 0 )
  {
    LogErr(util::Strings::Format("Failed to startup WinSock (%ld)", rc), "MessagingManager", "MessagingManager");
    m_bWindowsSocketStarted = false;
  }
   

}

/**
 * Destructor
 */
MessagingManager::~MessagingManager()
{
  if (  m_bWindowsSocketStarted )
    WSACleanup();
}

/**
 * Creates the MessagingManager shared ptr
 */
std::shared_ptr<MessagingManager> MessagingManager::Create(const std::string & sServer, const unsigned int iPort)
{
  std::shared_ptr<MessagingManager> pMessage = std::shared_ptr<MessagingManager>(new MessagingManager(sServer, iPort));

  if ( pMessage == __nullptr )
    throw std::exception("Failed to allocate MessagingManager");

  return pMessage;
} // Create()

/**
 * Connects to the server
 */
bool MessagingManager::Connect()
{

  bool bConnected(false);

  // ===============================================================================
  // If the Windows Socket library was started then connect otherwise return false
  // ================================================================================
  if ( m_bWindowsSocketStarted )
  {
    hostent * pHostEntry(__nullptr);
    unsigned long addr = inet_addr(m_sServer.c_str());

    if( addr == INADDR_NONE)
      pHostEntry = gethostbyname(m_sServer.c_str());
    else
      pHostEntry = gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
  
    if( pHostEntry == __nullptr)
    {
      LogErr(util::Strings::Format("Failed to get servers host (%ld)",WSAGetLastError()),"MessagingManager","Connect");
      return false;
    }


    //Fill out the information needed to initialize a socket…
    SOCKADDR_IN target; 

    target.sin_family = AF_INET;                   
    target.sin_port = htons(m_iPort);              
    target.sin_addr.s_addr  = *((unsigned long*)pHostEntry->h_addr);

    if ( (m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
      LogErr("Failed to create socket","MessagingManager","Connect");
      bConnected = false;
    }  
    else
    {

      // =====================
      // Try connecting...
      // =====================
      if ( connect(m_socket, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR )
      {
        
 
        LogErr(util::Strings::Format("Connection to message server failed (%ld)",WSAGetLastError()), "MessagingManager", "Connect");

        closesocket(m_socket);

        m_socket = INVALID_SOCKET;

        bConnected = false; 
      }
      else
        bConnected = true; 

    }

  }

  return bConnected;
}

/**
 * Disconnect
 */
void MessagingManager::Disconnect()
{
    if ( m_bWindowsSocketStarted )
    {

      //Close the socket if it exists
      if (m_socket != INVALID_SOCKET)
        closesocket(m_socket);
    }
}


void MessagingManager::Send(const std::string & sMessageXml)
{

  try
  {
 
    if ( Connect() )
    {

      // Get the XML
      std::string sMessage(sMessageXml);

      LogDbg(util::Strings::Format("Sending message %s", sMessage.c_str()), "MessagingManager","Send");

      // Send Status Message
      send(m_socket, sMessage.c_str(), (int)sMessage.size(), 0); 


      Disconnect();
    }

  }
  catch ( ... )
  {
    LogErr(util::Strings::Format("Failed to send message (%ld)", ::GetLastError()), "MessagingManager", "SendInformation");
  }

}
