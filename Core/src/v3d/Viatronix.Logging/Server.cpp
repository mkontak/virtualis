#include "Stdafx.h"
#include "Server.h"
#include "Log.h"


#pragma region usings

using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;

USING_LOGGING_NS

#pragma endregion



/**
 * Constructor
 */
Server::Server() :
m_pServer(scoped_ptr<std::shared_ptr<vxLogServer>>(__nullptr))
{
 
} // Server()


/**
 * Destructor
 */
Server::~Server()
{

  m_pServer.reset();

} // ~Server()


/**
 * Creates the server
 *
 * @param iPort   Port
 *
 * @return Returns the newly created consumer
 */
Server ^ Server::Create( System::String ^ mpSettings)
{

  try
  {

    XmlDocument ^ mpDoc = gcnew XmlDocument();
    mpDoc->LoadXml(mpSettings);

    return Create(mpDoc->DocumentElement);
   }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException("Failed to create server object : " + gcnew String(ex.GetTextMessage().c_str()));
  }


} // Create( )



/**
 * Creats the server
 *
 * @param iPort   Port
 *
 * @return Returns the newly created consumer
 */
Server ^ Server::Create( unsigned int iPort)
{
  try
  {
    Server ^ mpServer(gcnew Server());

    if ( mpServer == nullptr )
      throw gcnew OutOfMemoryException("Failed to allocate Server");

    mpServer->m_pServer.reset(new std::shared_ptr<vxLogServer>(vxLogServer::Create(iPort)));

    return mpServer;
  }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException("Failed to create server object : " + gcnew String(ex.GetTextMessage().c_str()));
  }
  catch ( ... )
  {
    throw gcnew LogException("Failed to create server object : unspecified error");
  }


} // Create( unsigned int iPort)

/**
 * Creats the server
 *
 * @param iPort   Port
 *
 * @return Returns the newly created consumer
 */
Server ^ Server::Create( System::Xml::XmlNode ^ mpNode)
{

  if ( mpNode->Attributes["port"] == nullptr )
    throw gcnew  LogException("Failed to create server object : missing port attribute");


  try
  {
    return Create(UInt32::Parse(mpNode->Attributes["port"]->Value));
  }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException("Failed to create server object : " + gcnew String(ex.GetTextMessage().c_str()));
  }



} // Create( unsigned int iPort)

/** 
 * Starts the store
 */
void Server::Start()
{
  (*m_pServer)->Start();
} // Start()


/**
 * Aborts the thread
 *
 * @return void.
 */
void Server::Stop()
{

  try
  {
    (*m_pServer)->Abort();
  }
  catch ( vxLogException & ex )
  {
    throw gcnew LogException( gcnew String(ex.GetTextMessage().c_str()) );
  }

 
} // Stop()

