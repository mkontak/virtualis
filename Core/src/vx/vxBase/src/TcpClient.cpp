#include "stdafx.h"
#include "TcpClient.h"
#include "Exception.h"
#include "vxTransportLayer.h"
#include "FileServerHeader.h"


using namespace io;


/**
 * Constructor
 *
 * @param sServer         File server
 * @param uPort           Port the server is listening on
 */
TcpClient::TcpClient(const std::string & sServer, const uint4 uPort) :
m_sServer(sServer),
m_bInitialized(false),
m_bConnected(false),
m_uPort(uPort),
m_request(__nullptr),
m_uReqeustBufferSize(0)
{

  /// Socket data
  WSADATA wsaData;


  int error = WSAStartup(0x0202, &wsaData);

  if (error)
  {
   
    throw ex::IOException(LogRec(util::Strings::Format("Failed to start Windows Sockets (%d)",error), "TcpClient", "TcpClient"));
  }
  else
  {


    if ( wsaData.wVersion != 0x0202 )
    {
      WSACleanup(); // Clean up Winsock

      throw ex::IOException(LogRec(util::Strings::Format("Invalid Windows Sockets version (%4x)",wsaData.wVersion), "TcpClient", "TcpClient"));
    }
  }


  m_bInitialized = true;

}




/**
 * Destructor
 */
TcpClient::~TcpClient()
{
  Disconnect();


  // If we are initialized then cleanup winsock
  if ( m_bInitialized )
    WSACleanup(); 

} // ~TcpClient()



/**
 * Creates the TcpClient object using the server/port specified
 *
 * @param sServer   Server 
 * @param uPort     Port
 *
 * @return TcpClient
 */
std::shared_ptr<TcpClient> TcpClient::Create(const std::string & sServer, uint4 uPort)
{

  // Make sure the server was specified
  if ( sServer.empty() )
    throw ex::IllegalArgumentException(LogRec("No server specified for network connection", "TcpClient","Create"));

  // Make sure the port was specified
  if ( uPort == 0 )
    throw ex::IllegalArgumentException(LogRec("No port specified for network connection", "TcpClient","Create"));


  // Create
  std::shared_ptr<TcpClient> pClient = std::shared_ptr<TcpClient>(new TcpClient(sServer,uPort));

  // Make sure the new worked
  if ( pClient ==__nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate TcpClient object","TcpClient","Create"));


  return pClient;

} // Create(const std::string & sServer, uint4 uPort)

/**
 * Creates the TcpClient object using the server/port specified
 * *
 * @return TcpClient
 */
std::shared_ptr<TcpClient> TcpClient::Create()
{

  // Make sure the TCP transport layer is set
  if ( ! TransportLayer::GetCurrentTransportLayer().IsProtocol(Protocols::P_TCP) )
    throw ex::InvalidDataException(LogRec("Cannot create network client, invalid transport layer","TcpClient","Create"));

  return Create(TransportLayer::GetCurrentTransportLayer().GetUrl().GetServer(), TransportLayer::GetCurrentTransportLayer().GetUrl().GetPort());


} // Create(const std::string & sServer, uint4 uPort)


/**
 * Attempts to connect to the server
 */
bool TcpClient::Connect()
{

  LogFyi(util::Strings::Format("Connecting to remote file server %s on port %d", m_sServer.c_str(), m_uPort), "TcpClient","Connect");


  try
  {
    // ============================================================================================
    // If winsock initialized correctly and we are not already connected then attempt to connect
    // ============================================================================================
    if ( m_bInitialized && ! m_bConnected  )
    {
      /// Create a socket
      SOCKET socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

      /// The stream object checks the socket handle for invalid setting
      m_stream = NetworkStream::Create(socketHandle);

      hostent * pHostEntry(__nullptr);
      unsigned long addr = inet_addr(m_sServer.c_str());

      if( addr == INADDR_NONE)
        pHostEntry = gethostbyname(m_sServer.c_str());
      else
        pHostEntry = gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
  
      if( pHostEntry == __nullptr)
       throw ex::IOException(LogRec(util::Strings::Format("Failed to get servers %s host (%d)", m_sServer.c_str(),WSAGetLastError()),"TcpClient","Connect"));

      SOCKADDR_IN server;
      server.sin_addr.s_addr  = *((unsigned long*)pHostEntry->h_addr);
      server.sin_family       = AF_INET;
      server.sin_port         = htons(m_uPort);

      // Attempt to bind to the socket
      if ( connect(socketHandle, (LPSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR)
        throw ex::IOException(LogRec(util::Strings::Format("Failed to connect to server %s (%d)", m_sServer.c_str(),WSAGetLastError()),"TcpClient","Connect"));

      // Make sure then socket is blocking
      u_long iMode=0;
	    ioctlsocket(socketHandle,FIONBIO,&iMode);

      m_bConnected = true;


    } // END ... If not connected yet

  }
  catch ( ex::VException & ex )
  {
    LogErr(util::Strings::Format("Failed to connect to remote file server %s on port %d (%d): %s", 
                                  m_sServer.c_str(), m_uPort, WSAGetLastError(),  ex.GetLogRecord().GetTextMessage().c_str()), "TcpClient","Connect");

    m_stream = __nullptr;

    m_bConnected = false;

    throw;
  }
  catch ( ... )
  {

    LogErr(util::Strings::Format("Failed to connect to remote file server %s on port %d (%d): unspecified error (%d)", 
                                  m_sServer.c_str(), m_uPort, WSAGetLastError(), ::GetLastError()), "TcpClient","Connect");

    m_stream = __nullptr;

    m_bConnected = false;

    throw;
  }

  return m_bConnected;

} // Connect()


/**
 * Disconnects from the server
 */
void TcpClient::Disconnect()
{
  // =====================================
  // Only disconnect if we are connected
  // ====================================
 if ( m_bConnected )
  {
    LogDbg(util::Strings::Format("Disconnecting from  %s on port %d", m_sServer.c_str(), m_uPort), "TcpClient","Connect");
     
    m_stream = __nullptr;

    m_bConnected = false;
  }

} // Disconnect()

/**
 * Send the specified data to the file server
 *
 * @param pBuffer         Data to be sent
 * @param uSize           Number of bytes to send.
 *
 * @return Number of bytes sent
 */
uint4 TcpClient::Send(const char * pBuffer, const uint4 uSize)
{

 
  try
  {

    return m_stream->Write((char *)pBuffer, uSize, 0);

  }
  catch ( ex::NetworkException ex )
  {
    throw ex::IOException(LogRec(util::Strings::Format("Failed to send data to  %s : %s", m_sServer.c_str(), ex.GetLogRecord().GetTextMessage().c_str()),"TcpClient","Send"));   
  }

} // Send()

/**
 * Send the specified data to the file server merging the request header with the payload to issue a single send instead of doing multple sends
 *
 * @param request         Request header
 * @param pBuffer         Data to be sent
 * @param uSize           Number of bytes to send.
 *
 * @return Number of bytes sent
 */
uint4 TcpClient::Send(const FILESERVERHEADER & request, const char * pBuffer, const uint4 uSize, const bool bMerge )
{

 
  try
  {



    if ( bMerge )
    {
      ToLogger(request, "TcpClient", "Send");

      const uint4 uRequestSize(FILESERVERHEADER_SIZE + uSize);

      if ( m_uReqeustBufferSize < uRequestSize )
      {

        m_request = __nullptr;
        m_request = std::shared_ptr< char>( new  char[uRequestSize],  [] ( char * value) { delete [] value; } );

        // Make sure the buffer was allocated
        if ( m_request == __nullptr )
          throw ex::OutOfMemoryException(LogRec("Failed to allocate request buffer for merging of the send data data","FileServer","ReqeustHandler"));


      }

      memcpy(m_request.get(), (char *)&request, FILESERVERHEADER_SIZE);
      memcpy(m_request.get() + FILESERVERHEADER_SIZE, pBuffer, uSize);

      return m_stream->Write( m_request.get(), uRequestSize, 0);
    }
    else
    {
      uint4 uBytesSent(0);

      uBytesSent  =   m_stream->Write( (char *)&request, FILESERVERHEADER_SIZE, 0);
      uBytesSent +=   m_stream->Write( pBuffer, uSize, 0);

      return uBytesSent;
    }

  }
  catch ( ex::NetworkException ex )
  {
    throw ex::IOException(LogRec(util::Strings::Format("Failed to send request data to  %s : %s", m_sServer.c_str(), ex.GetLogRecord().GetTextMessage().c_str()),"TcpClient","Send"));   
  }

} // Send()


/**
 * Send the specified data to the file server merging the request header with the payload to issue a single send instead of doing multple sends
 *
 * @param request         Request header
 *
 * @return Number of bytes sent
 */
uint4 TcpClient::Send(const FILESERVERHEADER & request )
{

 
  try
  {

    ToLogger(request, "TcpClient", "Send");


    return  m_stream->Write( (char *)&request, FILESERVERHEADER_SIZE, 0);

  }
  catch ( ex::NetworkException ex )
  {
    throw ex::IOException(LogRec(util::Strings::Format("Failed to send request data to  %s : %s", m_sServer.c_str(), ex.GetLogRecord().GetTextMessage().c_str()),"TcpClient","Send"));   
  }

} // Send()

/**
 * Receives data on the TCP connection
 *
 * @param pBuffer       Buffer to load the data into (must be allocated)
 * @param iSize         Number of bytes to receive
 *
 * @return  Number of bytes received
 */
uint4 TcpClient::Receive(char * pBuffer, const uint4 uSize)
{

  try
  {

    return  m_stream->Read( pBuffer, uSize);

  }
  catch ( ex::NetworkException ex )
  {
    throw ex::IOException(LogRec(util::Strings::Format("Failed to receive response data from  %s : %s", m_sServer.c_str(), ex.GetLogRecord().GetTextMessage().c_str()),"TcpClient","Send"));   
  }

} // Receive()



/**
 * Receives data on the TCP connection
 *
 * @param response      Response header
 *
 * @return  Number of bytes received
 */
uint4 TcpClient::Receive(FILESERVERHEADER & response)
{

  try
  {

    uint4 uBytesReceived = Receive( (char *)&response, FILESERVERHEADER_SIZE);

    ToLogger(response, "TcpClient", "Receive");

    return uBytesReceived;

   }
  catch ( ex::NetworkException ex )
  {
    throw ex::IOException(LogRec(util::Strings::Format("Failed to recevie data from %s : %s", m_sServer.c_str(), ex.GetLogRecord().GetTextMessage().c_str()),"TcpClient","Send"));   
  }

} // Receive()

