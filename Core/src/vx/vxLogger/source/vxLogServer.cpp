// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogServer.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "Ws2tcpip.h"
#include "vxLogServer.h"

#include "vxTaskGroup.h"
#include "vxTask.h"

/**
 * Constructor
 *
 * @param uPort       Port the server should listen on
 */
vxLogServer::vxLogServer(const unsigned int uPort) :
m_socketHandle(INVALID_SOCKET),
m_bStarted(false),
m_bAbortRequested(false),
m_uPort(uPort)
{

  /// Socket data
  WSADATA wsaData;

  m_tasks = std::make_shared<TaskGroup>();
 
  /// Create the task
  m_task = std::make_shared<Task>(Task());

  int error = WSAStartup(0x0202, &wsaData);


  if (error)
    throw vxLogException(LogRec(util::Strings::Format("Failed to start Windows Sockets (%d)",error),"vxLogServer","Run"));


  if ( wsaData.wVersion != 0x0202 )
  {
    WSACleanup(); // Clean up Winsock

    throw vxLogException(LogRec(util::Strings::Format("Invalid Windows Sockets version (%4x)",wsaData.wVersion),"vxLogServer","Run"));
  }

} // vxLogServer(const unsigned int iPort)


/**
 * Destructor
 */
vxLogServer::~vxLogServer()
{
  if ( m_socketHandle ) 
    closesocket(m_socketHandle);
 

  WSACleanup(); // Clean up Winsock
} // ~vxLogServer()


/**
 * Creates the vxLogServer shared pointer
 *
 * @param iPort     Port
 */
std::shared_ptr<vxLogServer> vxLogServer::Create(unsigned int iPort)
{
  std::shared_ptr<vxLogServer> pServer = std::make_shared<vxLogServer>(iPort);

  if ( pServer == __nullptr )
    throw vxLogOutOfMemoryException(LogRec("Failed to create logging server","vxLogServer","Create"));

  return pServer;
} // Create(unsigned int iPort)


/**
 * Aborts the file server
 */
void vxLogServer::Abort()
{

  LogFyi("Aborting the file server","FileServer","Abort");

  m_bAbortRequested = true;

  /// Create a socket
  SOCKET handle = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); 


  // Make sure the socket was created
  if (handle == INVALID_SOCKET)
    throw vxLogIOException(LogRec(util::Strings::Format("Failed to create socket (%d)",WSAGetLastError()),"FileServer","Abort"));

  DWORD size(512);
  char host[512];
  if ( ! ::GetComputerName(host, &size) )
    throw vxLogIOException(LogRec(util::Strings::Format("Failed to get host name (%d)",GetLastError()),"FileServer","Abort"));

  hostent * pHostEntry(__nullptr);
  unsigned long addr = inet_addr(host);

  if( addr == INADDR_NONE)
    pHostEntry = gethostbyname(host);
  else
    pHostEntry = gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
  
  if( pHostEntry == __nullptr)
  {
    closesocket(handle);
    throw vxLogIOException(LogRec(util::Strings::Format("Failed to get host (%d)",WSAGetLastError()),"FileServer","Abort"));

  }

  SOCKADDR_IN server;
  server.sin_addr.s_addr  = *((unsigned long*)pHostEntry->h_addr);
  server.sin_family       = AF_INET;
  server.sin_port         = htons(m_uPort);

  // Attempt to bind to the socket
  if ( connect(handle, (LPSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR)
  {
    closesocket(handle);
    throw vxLogIOException(LogRec(util::Strings::Format("Failed to connect to socket (%d)",WSAGetLastError()),"TcpClient","Connect"));
  }
  
  // close socket
  closesocket(handle);

  m_bStarted = false;

} // Abort()

/**
 * Starts the server asynchronously
 */
void vxLogServer::Start()
{

  if ( ! m_bStarted )
  {
    LogFyi(util::Strings::Format("Starting the Log Server on port %d",m_uPort),"vxLogServer","Start");

    
    /// Start the task
    m_task->Run(Concurrency::task<void>([this] () { RunSynchrounously(); } ));

  
    //m_tasks->Group.run( [this] ()  { RunSynchrounously(); }  );

    m_bStarted = true;
  }
  else
    LogWrn("Log server is already started","vxLogServer","Start");
  

} // Start

/**
 * Runs the log server
 */
void vxLogServer::RunSynchrounously()
{


  try
  {
    LogFyi("Starting log server","vxLogServer","RunSynchrounously");

    SOCKADDR_IN addr; 


    addr.sin_family = AF_INET;                  // Address family
    addr.sin_port = htons (m_uPort);            // Assign port to this socket
    addr.sin_addr.s_addr = htonl (INADDR_ANY);  // Accept a connection from any IP using INADDR_ANY
 
    /// Create a socket
    m_socketHandle = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); 

    // Make sure the socket was created
    if (m_socketHandle == INVALID_SOCKET)
      throw vxLogException(LogRec(util::Strings::Format("Failed to create socket (%d)",WSAGetLastError()),"vxLogServer","RunSynchrounously"));


    // Attempt to bind to the socket
    if ( bind(m_socketHandle, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
      throw vxLogException(LogRec(util::Strings::Format("Failed to bind to socket (%d)",WSAGetLastError()),"vxLogServer","RunSynchrounously")); 


    /// Listen for connections
    if ( listen(m_socketHandle, SOMAXCONN) == SOCKET_ERROR )
        throw vxLogException(LogRec(util::Strings::Format("Listen on port failed (%d)",WSAGetLastError()),"vxLogServer","RunSynchrounously")); 



    while ( true )
    {


      LogDbg("Waiting for connections","vxLogServer","RunSynchrounously");

      // =========================
      // Accept the connection.
      // =========================
      SOCKET acceptSocket(INVALID_SOCKET);
 
      struct sockaddr_in client;
      int size(sizeof(client));

 
    
      //if ( (acceptSocket = accept(m_socketHandle, &client, NULL )) == INVALID_SOCKET )
      if ( ( acceptSocket = WSAAccept(m_socketHandle, (SOCKADDR*) &client, &size, NULL, NULL ) ) == INVALID_SOCKET )
          throw vxLogException(LogRec(util::Strings::Format("accept failed (%d)\n", WSAGetLastError()),"vxLogServer","RunSynchrounously")); 

      char ip[32];
      RtlZeroMemory(ip,32);
      inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);

 
 
      LogDbg(util::Strings::Format("Received connection from %s",ip),"vxLogServer","RunSynchrounously"); 
 
      if ( m_bAbortRequested )
        throw vxLogAbortException(LogRec("Server is being aborted","vxLogServer","RunSynchrounously"));

      /// Starts the task
      Concurrency::task<void> task([this, acceptSocket] () { MessageHandler(acceptSocket); } );

      ///m_tasks->Group.run( [this, acceptSocket] () { MessageHandler(acceptSocket); } );

 
    } // END ... WHile forever
  
  }
  catch ( vxLogException & ex )
  {

    LogErr( util::Strings::Format("Log Server failed : %s", ex.GetTextMessage().c_str()), "LogServer","RunSynchrounously");

  }
  catch ( ... )
  {

    LogErr("Log Server failed : unknown error", "LogServer","RunSynchrounously");

  }

}



/**
 * Runs the message handler
 */
void vxLogServer::MessageHandler(SOCKET socket)
{


  LogDbg("Starting message handler...","vxLogServer","MessageHandler");

  int iBytesRead(0);


  // ==============================================================
  //
  //
  // RESPONSE rsp;
  //
  // strcpy(rsp.Tag,"RSP");
  // rsp.Failed = false;
  // memset(rsp.Message,0,sizeof(rsp.Message));
  // =============================================================
 
  try
  {

 
    REQUEST rq;

 
    // Read the RQ and Byte Count
    if ( (iBytesRead = recv(socket, (char *)&rq, REQUEST_SIZE, 0x8)) !=  REQUEST_SIZE )
      throw vxLogException(LogRec(util::Strings::Format("Did not receive RQ expected %d bytes but received only %d bytes (%d)", REQUEST_SIZE, iBytesRead, WSAGetLastError()),"vxLogServer","MessageHandler"));

    if ( strncmp(rq.Tag,"RQ", 2) != 0   )
      throw vxLogException(LogRec(util::Strings::Format("Did not receive RQ command (%d)", WSAGetLastError()),"vxLogServer","MessageHandler"));

    std::shared_ptr<char> buffer =  std::shared_ptr<char>(new char [rq.Size],  [] ( char * value ) { delete [] value; } );      

    if ( buffer == __nullptr )
      throw vxLogException(LogRec("Could not allocate buffer to receive message", "vxLogServer","MessageHandler"));

    // Read data
    if ( ( iBytesRead = recv(socket, buffer.get(), rq.Size, 0x8)) != rq.Size )
      throw vxLogException(LogRec(util::Strings::Format("Did not receive expected %d bytes but received only %d bytes (%d)", rq.Size, iBytesRead, WSAGetLastError()),"vxLogServer","MessageHandler"));
 
    closesocket(socket);
    socket = INVALID_SOCKET;

    std::string sXml(buffer.get(), rq.Size);
      
    xml::XmlDocument doc;

    doc.LoadXml(sXml);

    if ( ! doc.GetDocumentElement().IsValid() )
      throw vxLogException(LogRec("Invalid xml, no root","vxLogServer","MessageHandler"));

    if ( ! doc.GetDocumentElement().HasChildren() )
     throw vxLogException(LogRec("Invalid xml, no elements","vxLogServer","MessageHandler"));


    LogDbg("Received Message : " + sXml,"vxLogMessageHandler","MessageHandler");


    vxLoggerInfo loggerInfo(doc.GetDocumentElement());
 
    std::shared_ptr<vxLogger> pLogger( vxLogFactory::CreateLogger( loggerInfo ) );

    if ( pLogger == __nullptr )
      throw vxLogException(LogRec(util::Strings::Format("Could not create/find the specified logger %s", loggerInfo.GetHandle().c_str()), "vxLogServer","MessageHandler"));

    Sleep(10);

    pLogger->Trace(vxLogRecord(doc.GetDocumentElement().GetFirstChild()));

 
 
    LogDbg("Message has been logged...","vxLogServer","MessageHandler");



  }
  catch ( vxLogException & ex )
  {

    LogErr("Message Handler Failed : " + ex.GetTextMessage(),"vxLogMessageHandler","MessageHandler");

    if ( socket != INVALID_SOCKET )
      closesocket(socket);

    // =======================================================================
    // We are not sending response messages back at this point
    //
    // rsp.Failed = true;
    // strcpy(rsp.Message,ex.GetTextMessage().c_str());
    //
    // send(socket, (char *)&rsp, sizeof(RESPONSE), 0 );
    // =======================================================================

    //throw;

  }
  catch ( ... ) 
  {

    LogErr("Failed to process message : unspecified error","vxLogMessageHandler","MessageHandler");

    if ( socket != INVALID_SOCKET )
      closesocket(socket);

   // =======================================================================
    // We are not sending response messages back at this point
    //
    // rsp.Failed = true;
    // strcpy(rsp.Message,"Unspecified error");
    // 
    // send(socket, (char *)&rsp, sizeof(RESPONSE), 0 );
    // ======================================================================

    //throw vxLogException(LogRec("Failed to process message : unspecified error","vxLogMessageHandler","MessageHandler"));
  }


} // Run()