

#pragma once

#include "Stream.h"



namespace io
{


/**
 * Implements a network stream based on a SOCKET
 */
class VX_BASE_DLL NetworkStream : public Stream
{

protected:

  /// Constructor
  NetworkStream(SOCKET socket, bool owns = true);

public:

  ~NetworkStream() { Close(); }

public:

  /// Reads bytes from the network stream
  virtual uint4 Read(char * buffer, const size_t bytesToRead, const int4 flags = MSG_WAITALL);

  /// Write to stream
  virtual uint4 Write(const char * buffer, const size_t bytesToWrite, const int4 flags = 0);

  /// Creates the network stream
  static std::shared_ptr<NetworkStream> Create(SOCKET socket, bool owns = true);

  // Closes the stream 
  virtual void Close();

public:

  /// Gets the socket for this stream
  const SOCKET GetSocket() const 
  { return m_socket; }

  /// Gets the error code
  const int4 GetErrorCode() const
  { return m_error; }

  /// Get the errror message
  const std::string & GetErrorMessage() const
  { return m_errorMessage; }

private:

  
  /// Throws the exception based on the recturn code
  int ThrowException(int4 returnCode, const std::string & sMethod );

private:

  SOCKET m_socket;

  /// Flag indicating that the socket is owned by this object
  bool m_owns;

  /// Error code
  int4 m_error;

  /// Error Message
  std::string m_errorMessage;

};  // NetworkStream

} // namespace oio