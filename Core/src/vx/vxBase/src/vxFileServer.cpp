// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxFileServer.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "stdafx.h"
#include "FilePtr.h"
#include "FilePtrFactory.h"
#include "DirPtr.h"
#include "DirPtrFactory.h"
#include "vxFileServer.h"
#include "Exception.h"
#include "Ws2tcpip.h"
#include "vxTask.h"
#include "scope_exit.h"
#include "Stream.h"
#include "NetworkStream.h"

using namespace Concurrency;
using namespace io;


/**
 * Constructor
 *
 * @param uPort     Port the server is to listen on
 */
FileServer::FileServer(const uint4 uPort) :
m_socketHandle(INVALID_SOCKET),
m_bInitialized(false),
m_bAbortRequested(false),
m_bStarted(false),
m_task(std::make_shared<::Task>()),
m_uPort(uPort)
{



  /// Socket data
  WSADATA wsaData;


  int error = WSAStartup(0x0202, &wsaData);

  if (error)
  {
   
    throw ex::IOException(LogRec(util::Strings::Format("Failed to start Windows Sockets [%ld]",error), "TcpClient", "TcpClient"));
  }
  else
  {


    if ( wsaData.wVersion != 0x0202 )
    {
      WSACleanup(); // Clean up Winsock

      throw ex::IOException(LogRec(util::Strings::Format("Invalid Windows Sockets version [%4x]",wsaData.wVersion), "TcpClient", "TcpClient"));
    }
  }

  m_bInitialized = true;

} // FileServer


/**
 * Destructor
 */
FileServer::~FileServer()
{


  // Close the socket
  if ( m_socketHandle!= INVALID_SOCKET ) 
    closesocket(m_socketHandle);

  if ( m_bInitialized )
    WSACleanup(); 

}


/**
 * Creates the file server
 *
 * @param iPort       Port
 *
 * @return shared ptr to FileServer
 */
std::shared_ptr<FileServer> FileServer::Create( const uint4 uPort)
{


  std::shared_ptr<FileServer> pFileServer = std::shared_ptr<FileServer>(new FileServer(uPort));

  if ( pFileServer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate FileServer", "FileServer", "Create"));

  return pFileServer;

} // Create()


/**
 * Aborts the file server
 *
 * Must make a connection to the server in order for it to shut down
 */
void FileServer::Abort()
{

  LogFyi("Aborting the file server","FileServer","Abort");

  // Set the abort flag
  m_bAbortRequested = true;

  /// Create a socket
  SOCKET handle = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); 


  // Make sure the socket was created
  if (handle == INVALID_SOCKET)
    throw ex::IOException(LogRec(util::Strings::Format("Failed to create socket (%d)",WSAGetLastError()),"FileServer","Abort"));

  SCOPE_EXIT
  {
    closesocket(handle);
  };


  DWORD size(512);
  char host[512];
  if ( ! ::GetComputerName(host, &size) )
    throw ex::IOException(LogRec(util::Strings::Format("Failed to get host name (%d)",GetLastError()),"FileServer","Abort"));

  hostent * pHostEntry(__nullptr);
  unsigned long addr = inet_addr(host);

  if( addr == INADDR_NONE)
    pHostEntry = gethostbyname(host);
  else
    pHostEntry = gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
  
  if( pHostEntry == __nullptr)
    throw ex::IOException(LogRec(util::Strings::Format("Failed to get host (%d)",WSAGetLastError()),"FileServer","Abort"));
 
  SOCKADDR_IN server;
  server.sin_addr.s_addr  = *((unsigned long*)pHostEntry->h_addr);
  server.sin_family       = AF_INET;
  server.sin_port         = htons(m_uPort);

  // Attempt to bind to the socket
  if ( connect(handle, (LPSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR)
    throw ex::IOException(LogRec(util::Strings::Format("Failed to connect to socket (%d)",WSAGetLastError()),"TcpClient","Connect"));

  // Resets the started flag
  m_bStarted = false;

} // Abort()

/**
 * Starts the server asynchronously
 */
void FileServer::Start()
{

  // Throw exception if we try to start the server with a port = 0
  if ( m_uPort == 0 )
    ex::UnsupportedOperationException(LogRec("Cannot start server, port = 0","FileServer","Start"));

  if ( ! m_bStarted )
  {
    LogFyi(util::Strings::Format("Starting the File Server on port %d",m_uPort),"FileServer","Start");

    /// Run the server
    m_task->Run( Concurrency::task<void>( [this] ()  { RunSynchrounously(); }  ) );

    // Set the started flag to true
    m_bStarted = true;
  }
  else
    LogWrn("File server is already started","FileServer","Start");
  

} // Start

/**
 * Runs the file server synchronously. This method executed in a thread makes it asynchronous.
 */
void FileServer::RunSynchrounously()
{

  LogFyi("File Server has been started","FileServer","RunSynchrounously");

  try
  {

    SOCKADDR_IN addr; 


    addr.sin_family = AF_INET;                  // Address family
    addr.sin_port = htons (m_uPort);            // Assign port to this socket
    addr.sin_addr.s_addr = htonl (INADDR_ANY);  // Accept a connection from any IP using INADDR_ANY
 
    /// Create a socket
    m_socketHandle = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); 


    // Make sure the socket was created
    if (m_socketHandle == INVALID_SOCKET)
      throw ex::IOException(LogRec(util::Strings::Format("Failed to create socket (%d)",WSAGetLastError()),"FileServer","RunSynchrounously"));


    // Attempt to bind to the socket
    if ( bind(m_socketHandle, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
      throw ex::IOException(LogRec(util::Strings::Format("Failed to bind to socket (%d)",WSAGetLastError()),"FileServer","RunSynchrounously")); 


     /// Listen for connections
    if ( listen(m_socketHandle, SOMAXCONN) == SOCKET_ERROR )
        throw ex::IOException(LogRec(util::Strings::Format("Listen on port failed (%d)",WSAGetLastError()),"FileServer","RunSynchrounously")); 


    // ======================================
    // Loop forever waiting for connections
    // =======================================
    while ( true )
    {


        LogFyi("Waiting for connections","FileServer","RunSynchrounously");

        // =========================
        // Accept the connection.
        // =========================
        SOCKET acceptSocket(INVALID_SOCKET);
        sockaddr_in client;
        int length(sizeof(client));


        if ( (acceptSocket = accept(m_socketHandle, (sockaddr *)&client, &length )) == INVALID_SOCKET )
            throw ex::IOException(LogRec(util::Strings::Format("accept failed (%d)", WSAGetLastError()),"FileServer","RunSynchrounously")); 


        char ip[32];
        RtlZeroMemory(ip,32);
        inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);

    
        LogFyi(util::Strings::Format("Received connection from %s", inet_ntoa(client.sin_addr)), "FileServer","RunSynchrounously");

        if ( m_bAbortRequested )
          throw ex::AbortOperationException(LogRec("Server is being aborted","FileServer","RunSynchrounously"));


        Concurrency::task<void> task( [this, acceptSocket] () 
        {
          std::shared_ptr<Stream> stream(std::dynamic_pointer_cast<Stream>(NetworkStream::Create(acceptSocket)));

          SCOPE_EXIT
          {
            stream->Close();
          };

          RequestHandler(stream); 

        } 
        );

       
        

        Sleep(100);
 
    
    } // END ... While Forever

  }
  catch ( ex::VException ex )
  {
    LogErr(util::Strings::Format("File server detected exception : %s", ex.GetLogRecord().GetTextMessage().c_str()), "FileServer","RunSynchrounously");
    //throw;
  }
  catch ( ... )
  {
    LogErr("File server detected exception : unknown", "FileServer","RunSynchrounously");
    //throw;

  }

} // RunSynchronously()


/**
 * Handles the request received from the connection
 *
 * @param socket      The socket the client connection came in on
 */
void FileServer::RequestHandler(const std::shared_ptr<Stream> & stream)
{


  LogFyi( "Starting request handler...", "FileServer", "RequestHandler" );

  // Make sure then socket is blocking
  //u_long iMode=0;
	//ioctlsocket(socket,FIONBIO,&iMode);

  FILESERVERHEADER request;


  std::string sCommand("");
  std::shared_ptr<FilePtr> filePtr(__nullptr);

 
  try
  {

    // =====================================
    // Loop until the connection is closed
    // =====================================
    while ( ! m_bAbortRequested  )
    {

      RtlZeroMemory((void *)&request, FILESERVERHEADER_SIZE);

      LogDbg("Waiting for request request from client", "FileServer","RequestHandler");

 
      // Read the request request
      if ( stream->Read((char *)&request, FILESERVERHEADER_SIZE, 0x8) == 0 )
        break;

      // Get the command id as a string for logging
      sCommand = ToString(request.Command);

      LogFyi(util::Strings::Format("RQ:%d:%s:%s:%d:%d:%d", (int)request.Type, sCommand.c_str(), request.Handle, request.Size, request.CreateFlags, request.ModeFlags ), "FileServer","RequestHandler");
 

      // =================================================
      // Check for request otherwise we have a problem
      // =================================================
      if ( request.Type != FileServerTranscationTypes::REQUEST  )
        throw ex::IOException( LogRec("Invalid data received, did not receive REQUEST", "FileServer", "RequestHandler" ) );
 
      // ===========================================================
      // If we are opening a file then we do not need to look it up
      // ===========================================================
      switch ( request.Command )
      {
      case io::CMD_OPEN:
        Open(request, stream);
        break;
      case  io::CMD_QUERY:
        Query(request, stream);
        break;
      case  io::CMD_DELETE:
        Delete(request, stream);
        break;
      case  io::CMD_EXISTS:
        Exists(request, stream);
        break;
      case  io::CMD_GETFILES:
        GetFiles(request, stream);
        break;
      case  io::CMD_GETDIRS:
        GetDirs(request, stream);
        break;
      case  io::CMD_PING:
        Ping(request, stream);
        break;
      case  io::CMD_WRITE:
        Write(request, stream);
        break;
      case  io::CMD_READ:
        Read(request, stream);
        break;
      case  io::CMD_CLOSE:
        Close(request, stream);
        break;
      case  io::CMD_SIZE:
        Size(request, stream);
        break;
      case  io::CMD_FEOF:
        Eof(request, stream);
        break;
      case  io::CMD_SMART_EOF:
        SmartEof(request, stream);
        break;
      case  io::CMD_SEEK:
        Seek(request, stream);
        break;
      case  io::CMD_FLUSH:
        Flush(request, stream);
        break;
      default:
        throw ex::InvalidDataException(LogRec("Invalid file operation","FileServer","RequestHandler"));

      } // END ... switch 


      

  
    } // While not aborted


  }
  catch ( ex::VException & ex )
  {
    LogErr("Request failed : " + ex.GetLogRecord().GetTextMessage(), "FileServer", "RequestHandler");

    if ( stream->CanWrite() )
    {
      SendResponse(stream, request, STATUS_FAIL, ex.GetLogRecord().GetTextMessage().size() );
      stream->Write( ex.GetLogRecord().GetTextMessage().c_str(), ex.GetLogRecord().GetTextMessage().size(), 0);
    }

    //throw;
  }
  catch ( ... ) 
  {

    LogErr("Request failed : unknown exception", "FileServer", "RequestHandler");

    std::string sFilePath(filePtr != __nullptr ? filePtr->GetFilePath() : "" );
    std::string sMessage(util::Strings::Format("unspecified error (%d)",errno));

    if ( stream->CanWrite() )
    {
      SendResponse(stream, request, STATUS_FAIL, sMessage.size() );
      stream->Write( sMessage.c_str(), sMessage.size(), 0);
    }

    //throw ex::IOException(LogRec(util::Strings::Format("Failed to process (%s) message on %s : %s", sCommand.c_str(), sFilePath.c_str(), sMessage.c_str()),"FileServer","RequestHandler"));
  }




} // RequestHandler()

/**
 * Sends the response header
 *
 * @param socket      Socket to send on
 * @param header      Header to send back
 * @param iStatusCode Status code or operation 0 - Success, otherwise failure
 * @param bEof        Enf Of File flag
 * @param uSize       Size of the payload in bytes
 */
void FileServer::SendResponse(const std::shared_ptr<Stream> & stream, const FILESERVERHEADER & request, const int4 iStatusCode, const uint4 uSize , const bool bEof )
{


  // Response header 
  FILESERVERHEADER response;

  // Clear memory
  RtlZeroMemory((char *)&response, FILESERVERHEADER_SIZE);

  response.Type                 = FileServerTranscationTypes::RESPONSE;   // Response type
  response.StatusCode           = iStatusCode;                            // Status code
  response.Size                 = uSize;                                  // Size (Default to 0)
  response.IsEof                = bEof;                                   // EOF flag
  response.Command              = request.Command;

  memcpy(response.Handle, request.Handle, FILE_HANDLE_SIZE);              // File handle


  std::string sCommand(ToString(request.Command));

  LogFyi( util::Strings::Format("RSP:%d:%s:%s:%d:%s:%d", (int)response.Type, sCommand.c_str(), response.Handle, response.Size, ( response.IsEof ? "true" : "false" ) , response.StatusCode ), "FileServer","SendResponse");

  // Send the header back with the status
  int rc = stream->Write((char *)&response, FILESERVERHEADER_SIZE, 0);   

 
} // SendResponse(SOCKET socket, FILESERVERHEADER & header, int4 iStatusCode, uint4 uSize )


#pragma region command handlers



/**
 * Open Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Open(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{

  LogFyi("Handling OPEN command", "FileServer", "Open");

 
  std::shared_ptr<char> buffer = std::shared_ptr< char>( new  char[request.Size],  [] ( char * value) { delete [] value; } );

  // Make sure the buffer was allocated
  if ( buffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer to received file path","FileServer","Open"));

  // Get the filepath to be open
  stream->Read(buffer.get(), request.Size, 0);

  std::string sFilePath(buffer.get(), request.Size);

  // Create a new file pointer
  std::shared_ptr<FilePtr> filePtr = io::FilePtrFactory::CreateFilePtr(sFilePath);

  // Add file to table
  m_fileTable.Add(filePtr);


  // Open the file
  filePtr->Open(request.ModeFlags, request.CreateFlags);

  /// Set the handle
  strncpy(request.Handle, filePtr->GetHandle().c_str(), filePtr->GetHandle().size());

  // Send the response
  SendResponse(stream, request, STATUS_OK );   
    
} // HandleOpenCommand(FILESERVERHEADER request, std::shared_ptr<NetworkStream> & stream)


/**
 * Query Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Query(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{

  LogFyi("Handling QUERY command", "FileServer", "Query");

  // Get the file table
  std::string sTable(m_fileTable.ToXml());

  // Send the response request 
  SendResponse( stream, request, STATUS_OK, sTable.size() );

  // Send the table
  stream->Write(sTable.c_str(), sTable.size(), 0 );

} // Query(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)

/**
 * Delete Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Delete(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{

  LogFyi("Handling DELETE command", "FileServer", "Delete");

  std::shared_ptr<char> buffer = std::shared_ptr< char>( new  char[request.Size],  [] ( char * value) { delete [] value; } );

  // Make sure the buffer was allocated
  if ( buffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer to received file path","FileServer","Delete"));

  stream->Read(buffer.get(), request.Size);

  std::string sFilePath(buffer.get(), request.Size);
 
  std::shared_ptr<FilePtr> filePtr = io::FilePtrFactory::CreateFilePtr(sFilePath);

  SendResponse( stream, request, ( filePtr->Delete() ? STATUS_OK : STATUS_FAIL ));

} // Delete(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)


/**
 * Exists Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Exists(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  LogFyi("Handling EXISTS command", "FileServer", "Exists");

  std::shared_ptr<char> buffer = std::shared_ptr< char>( new  char[request.Size],  [] ( char * value) { delete [] value; } );

  // Make sure the buffer was allocated
  if ( buffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer to received file path","FileServer","Exists"));

  stream->Read(buffer.get(), request.Size);

  std::string sFilePath(buffer.get(), request.Size);

  SendResponse( stream, request, ( io::FilePtrFactory::Exists(sFilePath) ? STATUS_OK : STATUS_FAIL ));

} // Exists(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)


/**
 * GetFiles Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::GetFiles(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  LogFyi("Handling GETFILES command", "FileServer", "GetFiles");
 
  // Create buffer to hold the directory information
  std::shared_ptr<char> buffer = std::shared_ptr< char>( new  char[request.Size],  [] ( char * value) { delete [] value; } );

  // Make sure the buffer was allocated
  if ( buffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer to received file path","FileServer","GetFiles"));

  // Receive the data associated with the GETFILES command which shoul be "dir|pattern"
  stream->Read( buffer.get(), request.Size, 0);

  std::string sFilePath(buffer.get(), request.Size);

  // Split the fields (0-directory, 1-pattern)
  std::vector<std::string> fields =  util::Strings::Split(sFilePath, '|');

  // Create a DirPtr
  std::shared_ptr<io::DirPtr> dirPtr = io::DirPtrFactory::CreateDirPtr(fields[0]);

  LogDbg(util::Strings::Format("Getting files for %s with pattern %s", fields[0].c_str(), fields[1].c_str()), "FileServer", "GetFiles");

  // Get the files
  std::vector<std::string> files = dirPtr->GetFiles(fields[1]);

  // Join the files together with a '|' as a delimter
  std::string sList = util::Strings::Join(files,"|");

  LogDbg(util::Strings::Format("File list (%d) : %s", files.size(), sList.c_str()), "FileServer", "GetFiles");

  // Send the request indoicating OK and and the size of the directories file list
  SendResponse( stream, request, STATUS_OK, sList.size());

  // Do not send the list if no files were found
  if ( sList.size() > 0 )
    stream->Write( sList.c_str(), sList.size(), 0);

} // GetFiles(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)



/**
 * GetDirs Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::GetDirs(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{

  LogFyi("Handling GETDIRS command", "FileServer", "GetDirs");
  
  // Create buffer to hold the directory information
  std::shared_ptr<char> buffer = std::shared_ptr< char>( new  char[request.Size],  [] ( char * value) { delete [] value; } );

  // Make sure the buffer was allocated
  if ( buffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer to received file path","FileServer","GetDirs"));

  stream->Read( buffer.get(), request.Size);
        
  std::string sFilePath(buffer.get(), request.Size);

  std::vector<std::string> fields =  util::Strings::Split(sFilePath, '|');

  std::shared_ptr<io::DirPtr> dirPtr = io::DirPtrFactory::CreateDirPtr(fields[0]);

  LogDbg(util::Strings::Format("Getting sub-dirs for %s with pattern %s", fields[0].c_str(), fields[1].c_str()), "FileServer", "GetDirs");

  std::vector<std::string> dirs = dirPtr->GetDirs(fields[1]);

  std::string sList= util::Strings::Join(dirs,"|");

  LogDbg(util::Strings::Format("Directory list (%d): %s", dirs.size(), sList.c_str()), "FileServer", "GetDirs");

  
  SendResponse( stream, request, STATUS_OK, sList.size() );

  if ( sList.size() > 0 )
    stream->Write( sList.c_str(), sList.size(), 0);

} // GetDirs(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)

/**
 * Ping Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Ping(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  LogFyi("Handling PING command", "FileServer", "Ping");

  SendResponse( stream, request, STATUS_OK );
} // Ping(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)



/**
 * Write Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Write(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling WRITE command on %s",filePtr->GetFilePath().c_str()), "FileServer", "Write");

  // Buffer to hold the data received from the client
  std::shared_ptr<char> buffer = std::shared_ptr< char>( new  char[request.Size],  [] ( char * value) { delete [] value; } );

  // Make sure the buffer was allocated
  if ( buffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer to received data to be written","FileServer","Write"));
         

  // Get the client from the tcp socket
  stream->Read( buffer.get(), request.Size);

  // Write to the file
  filePtr->Write(buffer.get(),request.Size);

  // Send the response
  SendResponse(stream, request, STATUS_OK );   


} // Write(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)

/**
 * Read Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Read(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling READ command on %s",filePtr->GetFilePath().c_str()), "FileServer", "Read");

  // allocate buffer to read into ( Use the size specified in the request )
  std::shared_ptr<char> buffer = std::shared_ptr< char>( new  char[request.Size],  [] ( char * value) { delete [] value; } );

  // Make sure the buffer was allocated
  if ( buffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate buffer to read data into","FileServer","ReqeustHandler"));

  // Read the number of bytes specified
  const uint4 uBytesRead = filePtr->Read(buffer.get(),request.Size);

  // =================================================================
  // Log warning if the requested bytes do not match the byte read
  // =================================================================
  if ( uBytesRead != request.Size )
    LogWrn(util::Strings::Format("Bytes read (%d) does not match the bytes expecting to be read (%d)", uBytesRead, request.Size), "FileServer", "RequestHandler");

  SendResponse(stream, request, STATUS_OK,  uBytesRead, filePtr->IsEof() );   

  stream->Write( (char *)buffer.get(), uBytesRead, 0);   // Payload

} // Read(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)


/**
 * Close Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Close(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling READ command on %s",filePtr->GetFilePath().c_str()), "FileServer", "Close");

  // Close the file
  filePtr->Close();
           
  // Remove from the table
  m_fileTable.Remove(filePtr->GetHandle());

} // Close(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)


/**
 * Size Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Size(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling READ command on %s",filePtr->GetFilePath().c_str()), "FileServer", "Size");
  
  SendResponse(stream, request, STATUS_OK, filePtr->Size() );   

} // Size(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)

/**
 * EOF Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Eof(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling EOF command on %s",filePtr->GetFilePath().c_str()), "FileServer", "Eof");
  
  SendResponse(stream, request, STATUS_OK, 0, filePtr->IsEof() )  ;

} // Eof(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)


/**
 * SMART-EOF Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::SmartEof(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling SMART-EOF command on %s",filePtr->GetFilePath().c_str()), "FileServer", "SmartEof");
  
  SendResponse(stream, request, STATUS_OK, 0, filePtr->IsEofSmart() );

} // SmartEof(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)




/**
 * Seek Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Seek(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling SEEK command on %s",filePtr->GetFilePath().c_str()), "FileServer", "Seek");
  
  SendResponse(stream, request, STATUS_OK, 0, filePtr->SeekCurrent(request.Size) );     

} // Seek(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)



/**
 * Seek Command Handler
 *
 * @param request       Request header
 * @param stream        Stream
 */
void FileServer::Flush(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)
{
  // ===========================================================================================================
  // Get the file from the table if the command is not a one time ( If not found then an exception is thrown )
  // ===========================================================================================================
  std::shared_ptr<FilePtr> filePtr(m_fileTable.GetFile(request.Handle));
      
  LogFyi(util::Strings::Format("Handling FLUSH command on %s",filePtr->GetFilePath().c_str()), "FileServer", "Flush");
  
  filePtr->Flush();

  SendResponse(stream, request, STATUS_OK );             

} // Flush(FILESERVERHEADER request, const std::shared_ptr<Stream> & stream)

 

#pragma endregion