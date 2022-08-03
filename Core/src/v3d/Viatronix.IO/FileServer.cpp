#include "Stdafx.h"
#include "FileServer.h"
#include "TransportLayer.h"

using namespace System;


Viatronix::IO::FileServer ^ Viatronix::IO::FileServer::Create(unsigned int uPort)
{

  Viatronix::IO::FileServer ^ mpFileServer = gcnew Viatronix::IO::FileServer();


  if ( mpFileServer == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate FileServer");

  // Sets the port
  mpFileServer->m_pServer.reset(new std::shared_ptr<io::FileServer>(io::FileServer::Create(uPort)));

  return mpFileServer;

} // Create()

Viatronix::IO::FileServer ^ Viatronix::IO::FileServer::Create(System::Xml::XmlNode ^ mpNode)
{

  Viatronix::IO::FileServer ^ mpFileServer = gcnew Viatronix::IO::FileServer();


  if ( mpFileServer == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate FileServer");

  TransportLayer::Initialize( mpNode->SelectSingleNode("transportLayer") );

  mpFileServer->m_pServer.reset(new std::shared_ptr<io::FileServer>(io::FileServer::Create(UInt32::Parse(mpNode->Attributes["port"]->Value))));



  return mpFileServer;

} // Create()


void Viatronix::IO::FileServer::Start()
{
     
    // Starts the server
    (*m_pServer)->Start();

}

void Viatronix::IO::FileServer::Stop()
{
     
  
    // Starts the server
    (*m_pServer)->Abort();

}