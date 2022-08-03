// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: MessagingManager.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "stdafx.h"
#include "MessagingManager.h"

USING_MESSAGE_NS

/**
 * Constructor
 */
MessagingManager::MessagingManager(String ^ msServer, UInt32 iPort)
{

  m_pManager.reset(new std::shared_ptr<messaging::MessagingManager>(messaging::MessagingManager::Create(ss(msServer), iPort))); 
} // MessagingManager(String ^ msServer, UInt32 iPort)

/**
 * Creates the messaging manager
 */
MessagingManager ^ MessagingManager::Create(String ^ msServer, UInt32 iPort)
{

  MessagingManager ^ mpManager = gcnew MessagingManager(msServer, iPort);

  if ( mpManager == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate MessagingManager");


  return mpManager;
} // Create(String ^ msServer, UInt32 iPort, String ^ msJob, String ^ msName)

/**
 * Sends the supplied message
 */
void MessagingManager::Send(Viatronix::Messaging::IMessage ^ pMessage)
{

  (*m_pManager)->Send( ss(pMessage->ToXml()) );

} // Send(IMessage ^ pMessage)

/**
 * Gets the server
 */
String ^ MessagingManager::Server::get()
{ 
  return gcnew String( (*m_pManager)->GetServer().c_str()); 
}

/**
 * Gets the port
 */
unsigned int MessagingManager::Port::get()
{ 
  return (*m_pManager)->GetPort(); 
}

