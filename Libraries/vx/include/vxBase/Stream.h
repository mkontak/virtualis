#pragma once



namespace io
{


/**
 * Streams interface
 */
class VX_BASE_DLL Stream
{

public:

  /// Constructor
  Stream() :  m_bClosed(false) { }

  /// Destrcutor
  ~Stream() 
  { Close(); }

public:

  // Read from the stream
  virtual uint4 Read( char * buffer, const size_t bytesToRead, const int4 flags = MSG_WAITALL) = 0;

  // Write to the stream
  virtual uint4 Write(const char * buffer, const size_t bytesToWrite, const int4 flags = MSG_WAITALL) = 0;

public:

  /// Returns true if the stream is closed
  virtual bool IsClosed() { return m_bClosed; }

  // Close the stream
  virtual void Close() { m_bClosed = true; }

  /// Can read
  virtual bool CanRead() 
  { return ! m_bClosed; }

  /// Can write
  virtual bool CanWrite()
  { return ! m_bClosed; }

#pragma region fields

protected:

  /// Open flag
  bool m_bClosed;

};  // Stream

} // namespace io