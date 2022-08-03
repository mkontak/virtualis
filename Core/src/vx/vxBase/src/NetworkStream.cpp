
#include "stdafx.h"
#include "NetworkStream.h"
#include "Exception.h"


using namespace io;


/**
 * COnstructor
 *
 * @param socket      Socket stream
 * @param owns        Ownership flag
 */
NetworkStream::NetworkStream(SOCKET socket, bool owns) :
Stream(),
m_owns(owns),
m_socket(INVALID_SOCKET)
{

  // Make sure the socket was created
  if (socket == INVALID_SOCKET)
    throw ex::InvalidDataException(LogRec("Invalid socket","NetworkStream","NetworkStream"));

  // Set the socket
  m_socket = socket;

} // NetworkStream(SOCKET socket, bool owns)

/**
 * Creates the stream
 *
 * @param socket    Socket
 */
std::shared_ptr<NetworkStream> NetworkStream::Create(SOCKET socket, bool owns)
{

  /// Create the stream
  std::shared_ptr<NetworkStream> stream(std::shared_ptr<NetworkStream>(new NetworkStream(socket, owns)));

  /// Make sure no mempory error
  if ( stream == nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate network stream","NetworkStream", "Create"));

  return stream;

} // Create(SOCKET socket)

/**
 * Reads data on the socket
 *
 * @param buffer            Buffer to write to
 * @param bytesReqested     bytes requested from socket
 * @param flags             flags
 *
 * @return Bytes read
 */

uint4 NetworkStream::Read(char * buffer, const size_t bytesToRead, const int4 flags )
{

  // =============================================
  // Check to make sure the etram is not closed
  // =============================================
  if ( IsClosed() ) 
    throw ex::StreamClosedException(LogRec("Network stream is closed","NetworkStream","Read"));

  try
  {

    int4 bytesRead =  ThrowException(recv(m_socket, buffer, bytesToRead, flags ), "Read");

#ifdef _DEBUG
    // Uncomment for more debugging
    LogDbg(util::Strings::Format("Attempting the read %d byte(s) on socket, %d received", bytesToRead, bytesRead), "NetworkStream","Read");
#endif

    // =====================================================
    // If we receive nothing then the connection has closed
    // ======================================================
    if ( bytesRead == 0 )
      throw ex::ConnectionClosedException(LogRec("Connection closed", "NetworkStream","Read"));
    else if ( bytesRead != bytesToRead )
      throw ex::NetworkException(LogRec(util::Strings::Format("%d does not match the byte(s) %d requested", bytesRead, bytesToRead), "NetworkStream","Read"));

    return bytesRead;

  }
  catch( ex::VException  & ex )
  {
    m_errorMessage = ex.GetLogRecord().GetTextMessage();
    Close();
    throw;
  }

} // Read(char * buffer, const uint4 bytesToRead)

/**
 * writes data on the socket
 *
 * @param socket    Socket
 * @param buffer    Buffer to write to
 * @param size      Size of buffer
 * @param flags     Flags
 *
 * @return Bytes sent
 */
uint4 NetworkStream::Write(const char * buffer, const size_t byteToWrite, const int4 flags )
{

  // =============================================
  // Check to make sure the etram is not closed
  // =============================================
  if ( IsClosed() ) 
    throw ex::StreamClosedException(LogRec("Network stream is closed","NetworkStream","Write"));

  try
  {
    int4 bytesWritten =  ThrowException(send(m_socket,buffer, byteToWrite, flags), "Write");

#ifdef _DEBUG
    // Uncomment
    LogDbg(util::Strings::Format("Attempting to send %d byte(s) on socket, %d sent", byteToWrite, bytesWritten ), "NetworkStream","Write");
#endif

    // =======================================
    // Make sure the entire buffer was sent
    // =======================================
    if ( bytesWritten != byteToWrite )
      throw ex::NetworkException(LogRec( util::Strings::Format("Entire buffer was not sent properly : expected %d but only %d sent", byteToWrite, bytesWritten), "NetworkStream","Write") );

    return bytesWritten;

  }
  catch ( ex::VException & ex )
  {
    m_errorMessage = ex.GetLogRecord().GetTextMessage();
    Close();
    throw;
  }

} // Write(const char * buffer, const uint4 byteToWrite, const int4 flags )


/**
 * Close the socket
 */
void NetworkStream::Close()
{
  if ( m_owns && ! m_bClosed && m_socket != INVALID_SOCKET)
  {
    // Shutdown the connections
    if ( shutdown(m_socket, SD_BOTH) == SOCKET_ERROR )
      LogWrn(util::Strings::Format("Failed to shutdown the socket (%d)", WSAGetLastError()), "NetworkStream","Close");

    if ( closesocket(m_socket) == SOCKET_ERROR )
      LogWrn(util::Strings::Format("Failed to close the socket (%d)", WSAGetLastError()), "NetworkStream","Close");
       
  }
  
  m_socket = INVALID_SOCKET;

  Stream::Close(); 

} // Close()

/**
 * Throws an exception based on the return code
 *
 * @param returnCode      TCP netowrk return code
 */
int4 NetworkStream::ThrowException( int4 returnCode, const std::string & sMethod  )
{

  m_error = 0;

  // ==============================================
  // check if we received an error from the receive
  // ==============================================
  if ( returnCode == SOCKET_ERROR  )
  {
    m_error = WSAGetLastError();

    switch ( m_error )
    {
    case WSANOTINITIALISED:
      throw ex::NotInitializedException(LogRec("WSA not initialized","NetworkStream",sMethod));
    case WSAECONNRESET:
      throw ex::ConnectionResetException(LogRec("Connection reset by peer","NetworkStream",sMethod));
    case WSAENETDOWN:
      throw ex::NetworkFailedException(LogRec("Network sub-system failed","NetworkStream",sMethod));
    case WSAEFAULT:
      throw ex::BufferException(LogRec("Invalid buffer","NetworkStream",sMethod));
    case ENOTCONN:
      throw ex::NotConnectedException(LogRec("Socket is not connected","NetworkStream",sMethod));
    case WSAEINTR:
      throw ex::BlockingCanceledException(LogRec("Blocking was cancelled","NetworkStream",sMethod));
    case WSAEINPROGRESS:
      throw ex::InProgressException(LogRec("Blocking call is in progress","NetworkStream",sMethod));
    case WSAENETRESET:
      throw ex::KeepAliveException(LogRec("Keep Alive or TTL ","NetworkStream",sMethod));
    case WSAENOTSOCK:
      throw ex::NotSocketException(LogRec("Not a socket","NetworkStream",sMethod));
    case WSAEOPNOTSUPP:
      throw ex::UnsupportedOptionException(LogRec("Unsupported option","NetworkStream",sMethod));
    case WSAESHUTDOWN:
      throw ex::SocketShutdownException(LogRec("Socket shut down","NetworkStream",sMethod));
    case WSAEWOULDBLOCK:
      throw ex::SocketMarkedAsNonBlocking(LogRec("Socket is set for blocking, transaction woould block","NetworkStream",sMethod));
    case WSAEMSGSIZE:
      throw ex::MessageTooLargeException(LogRec("Message too large for buffer","NetworkStream",sMethod));
    case WSAEINVAL:
      throw ex::SocketNotBoundException(LogRec("Socket is not bound","NetworkStream",sMethod));
    case WSAECONNABORTED:
      throw ex::ConnectionAbortedException(LogRec("Connection aborted","NetworkStream",sMethod));
    case WSAETIMEDOUT:
      throw ex::ConnectionTimeOutException(LogRec("Connection timed out","NetworkStream",sMethod));
    default:
      throw ex::NetworkException(LogRec(util::Strings::Format("Received error on network %d", m_error),"NetworkStream",sMethod));

    };


  } // END ... If we received an error

  return returnCode;
}
