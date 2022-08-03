#include "stdafx.h"
#include "TcpNetwork.h"

using namespace io;


/**
 * Receives data on the socket
 *
 * @param socket            Socket
 * @param buffer            Buffer to write to
 * @param bytesReqested     bytes equested from socket
 * @param flags             Flags
 *
 * @return Bytes read
 */
int4 TcpNetwork::Receive(SOCKET socket, char * buffer, const uint4 bytesReqested, const int flags )
{

  int4 bytesReceived =  ThrowException(recv(socket, buffer, bytesReqested, flags ), "Receive");

#ifdef _DEBUG
  // Uncomment for more debugging
  LogDbg(util::Strings::Format("Attempting the read %d byte(s) on socket, %d received", bytesReqested, bytesReceived), "TcpNetwork","Receive");
#endif

  // =====================================================
  // If we receive nothing then the connection has closed
  // ======================================================
  if ( bytesReceived == 0 )
    throw ex::ConnectionClosedException(LogRec("Connection closed", "TcpNetwork","Receive"));
  else if ( bytesReceived != bytesReqested )
    throw ex::NetworkException(LogRec(util::Strings::Format("%d does not match the byte(s) %d requested", bytesReceived, bytesReqested), "TcpNetwork","Receive"));

  return bytesReceived;

} // Receive(SOCKET socket, char * buffer, uint4 bytesReqested, int flags )


/**
 * Receive a response header
 *
 * @param socket      Socket to read
 * @param response    Memory to load
 * @param flags       Flags
 *
 * @retun number of bytes read
 */
int4 TcpNetwork::Receive( SOCKET socket, TCPHEADER & response, const int flags)
{

 int4 bytesReceived = Receive(socket, (char *)&response, TCP_HEADER_SIZE, flags );

 ToLogger(response, "TcpNetwork", "Receive");

 return bytesReceived;
}
/**
 * Send data on the socket
 *
 * @param socket    Socket
 * @param buffer    Buffer to write to
 * @param size      Size of buffer
 * @param flags     Flags
 *
 * @return Bytes sent
 */
int4 TcpNetwork::Send(SOCKET socket, const char * buffer, const uint4 bytesToSend, const int flags )
{


  int4 bytesSent =  ThrowException(send(socket,buffer, bytesToSend, flags), "Send");

#ifdef _DEBUG
  // Uncomment
  LogDbg(util::Strings::Format("Attempting to send %d byte(s) on socket, %d sent", bytesToSend, bytesSent ), "TcpNetwork","Send");
#endif

  // =======================================
  // Make sure the entire buffer was sent
  // =======================================
  if ( bytesSent != bytesToSend )
    throw ex::NetworkException(LogRec( util::Strings::Format("Entire buffer was not sent properly : expected %d but only %d sent", bytesToSend, bytesSent), "FileServer","SendResponse") );

  return bytesSent;

} // Send(SOCKET socket, char * buffer, uint4 size, int flags )


/**
 * Throws an exception based on the return code
 *
 * @param returnCode      TCP netowrk return code
 */
int4 TcpNetwork::ThrowException( int4 returnCode, const std::string & sMethod  )
{
  // ==============================================
  // check if we received an error from the receive
  // ==============================================
  if ( returnCode == SOCKET_ERROR  )
  {
    int error = WSAGetLastError();

    switch ( error )
    {
    case WSANOTINITIALISED:
      throw ex::NotInitializedException(LogRec("WSA not initialized","TcpNetwork",sMethod));
    case WSAECONNRESET:
      throw ex::ConnectionResetException(LogRec("Connection reset by peer","TcpNetwork",sMethod));
    case WSAENETDOWN:
      throw ex::NetworkFailedException(LogRec("Network sub-system failed","TcpNetwork",sMethod));
    case WSAEFAULT:
      throw ex::BufferException(LogRec("Invalid buffer","TcpNetwork",sMethod));
    case ENOTCONN:
      throw ex::NotConnectedException(LogRec("Socket is not connected","TcpNetwork",sMethod));
    case WSAEINTR:
      throw ex::BlockingCanceledException(LogRec("Blocking was cancelled","TcpNetwork",sMethod));
    case WSAEINPROGRESS:
      throw ex::InProgressException(LogRec("Blocking call is in progress","TcpNetwork",sMethod));
    case WSAENETRESET:
      throw ex::KeepAliveException(LogRec("Keep Alive or TTL ","TcpNetwork",sMethod));
    case WSAENOTSOCK:
      throw ex::NotSocketException(LogRec("Not a socket","TcpNetwork",sMethod));
    case WSAEOPNOTSUPP:
      throw ex::UnsupportedOptionException(LogRec("Unsupported option","TcpNetwork",sMethod));
    case WSAESHUTDOWN:
      throw ex::SocketShutdownException(LogRec("Socket shut down","TcpNetwork",sMethod));
    case WSAEWOULDBLOCK:
      throw ex::SocketMarkedAsNonBlocking(LogRec("Socket is set fr blocking, transaction woould block","TcpNetwork",sMethod));
    case WSAEMSGSIZE:
      throw ex::MessageTooLargeException(LogRec("Message too large for buffer","TcpNetwork",sMethod));
    case WSAEINVAL:
      throw ex::SocketNotBoundException(LogRec("Socket is not bound","TcpNetwork",sMethod));
    case WSAECONNABORTED:
      throw ex::ConnectionAbortedException(LogRec("Connection aborted","TcpNetwork",sMethod));
    case WSAETIMEDOUT:
      throw ex::ConnectionTimeOutException(LogRec("Connection timed out","TcpNetwork",sMethod));
    default:
      throw ex::NetworkException(LogRec(util::Strings::Format("Received error on network %d", error),"TcpNetwork",sMethod));

    };

  } // END ... If we received an error

  return returnCode;
}



