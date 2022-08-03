
#pragma once


#include "scoped_ptr.h"
#include "IMessage.h"

using namespace System;

OPEN_MESSAGE_NS


/**
 * Wraps the native MessagingManager from vxMessging.dll
 */
public ref class MessagingManager
{
protected:

  // Constructor
  MessagingManager(String ^ msServer, UInt32 iPort);



public:

  // Create the manager
  static MessagingManager ^ Create(String ^ msServer, UInt32 iPort);


  /// Send Message
  void Send(Viatronix::Messaging::IMessage ^ mpMessage);


#pragma region properties

public:

  /// Gets the server
  property String ^ Server
  { String ^ get(); }


  /// Gets the port
  property unsigned int Port
  { unsigned int get(); }




#pragma endregion


#pragma region fields

private:

  /// Natve Messaging Manager
  scoped_ptr<std::shared_ptr<messaging::MessagingManager>> m_pManager;

#pragma endregion

};  // class MessagingManager


CLOSE_MESSAGE_NS