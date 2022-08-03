#include "stdafx.h"
#include "MemoryStream.h"
#include "vxBuffer.h"



using namespace io;



/**
 * Constructor
 */
MemoryStream::MemoryStream( const size_t iSize) : 
Stream(),
m_pInputBuffer(Buffer::Create(iSize)),
m_pOutputBuffer(Buffer::Create(iSize)),
m_iIndexer(0),
m_iRead(0),
m_bCanWrite(true),
m_bCanRead(false)
{

}

/**
 * Constructor
 */
MemoryStream::MemoryStream(unsigned char * buffer, const size_t iSize) : 
Stream(),
m_pInputBuffer(Buffer::Create(buffer, iSize)),
m_pOutputBuffer(Buffer::Create(iSize)),
m_iIndexer(iSize),
m_iRead(0),
m_bCanWrite(true),
m_bCanRead(iSize > 0 ? true : false)
{


}

/**
 * Creates a memeory stream
 * 
 * @param buffer    Input buffer
 * @param size      Size of buffer
 */
std::shared_ptr<MemoryStream> MemoryStream::Create(unsigned char * buffer, const size_t size)
{

  std::shared_ptr<MemoryStream> stream(std::shared_ptr<MemoryStream>(new MemoryStream(buffer, size)));

  if ( stream == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate memory stream", "MemoryStream", "Create"));

  return stream;

} // Create(const std::shared_ptr<char> buffer, const uint4 size)

/**
 * Creates a memory stream
 */
std::shared_ptr<MemoryStream> MemoryStream::Create(const size_t iSize)
{
  std::shared_ptr<MemoryStream> stream(std::shared_ptr<MemoryStream>(new MemoryStream(iSize)));

  if ( stream == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate memory stream", "MemoryStream", "Create"));

  return stream;
} // Create(const uint4 size)


/**
 * Indicates if the stream can be written to
 */
bool MemoryStream::CanWrite()
{
  return ( m_bCanWrite && Stream::CanWrite() );
} // CanWrite()


/**
 * Indicates if the stream can be read from
 */
bool MemoryStream::CanRead()
{
  return ( m_bCanRead && Stream::CanRead() );
} // CanRead()

/**
 * Closes the memory stream
 */
void MemoryStream::Close()
{
  LogFyi("Closing memory stream","MemoryStream","Close");


  m_pOutputBuffer->Release();

  m_pInputBuffer->Release();

  Stream::Close();

} // Close()




/**
 * writes data on the memory stream (OUTPUT)
 *
 * @param buffer    Buffer to write to
 * @param size      Size of buffer
 * @param flags     Flags
 *
 * @return Bytes sent
 */
uint4 MemoryStream::Write(const char * buffer, const size_t bytesToWrite, const int4  flags )
{

  // =============================================
  // Check to make sure the etram is not closed
  // =============================================
  if ( IsClosed() ) 
    throw ex::StreamClosedException(LogRec("Memory stream is closed","MemoryStream","Read"));


  m_pOutputBuffer->Append(buffer, bytesToWrite);


  return bytesToWrite;

} // Write(const char * buffer, const uint4 size, const int4  flags )



/**
 * reads data from the memwory stream (INPUT)
 *
 * @param buffer    Buffer to read into
 * @param size      number of bytes to read
 * @param flags     Flags
 *
 * @return Bytes sent
 */
uint4 MemoryStream::Read(char * buffer, const size_t byteToRead, const int4  flags )
{

  // =============================================
  // Check to make sure the etram is not closed
  // =============================================
  if ( IsClosed() ) 
    throw ex::StreamClosedException(LogRec("Memory stream is closed","MemoryStream","Read"));

  // Remaing byte in stream
  uint4 remaining(m_pInputBuffer->GetLength() - m_iRead);

  // Byte to read read from stream (take into account the remaining byte and requested bytes )
  uint4 bytesRead(byteToRead > remaining ? remaining : byteToRead);
 
  // Error code
  errno_t error(0);

  
  if ( bytesRead > 0 )
  {
    if ( (error = memcpy_s(buffer, bytesRead, m_pInputBuffer->GetData(m_iRead), bytesRead)) != 0 )
      throw ex::StreamException(LogRec(util::Strings::Format("Failed to read memory stream (%d)",error), "MemoryStream", "Read"));
  }

  /// Increments the bytes read from the stream
  m_iRead  += bytesRead;
  
  /// Set the can read flag 
  m_bCanRead =  ( m_iRead < m_pInputBuffer->GetLength() );

  // return the bytes read
  return bytesRead;

} // Write(const char * buffer, const uint4 size, const int4  flags )

/**
 * Copies the output to a new buffer
 *
 * @param buffer      Destination buffer
 * @param size        Number of bytes to be copied
 */
uint4 MemoryStream::CopyTo(char * buffer, const size_t size)
{
  // =============================================
  // Check to make sure the etram is not closed
  // =============================================
  if ( IsClosed() ) 
    throw ex::StreamClosedException(LogRec("Memory stream is closed","MemoryStream","CopyTo"));

  if ( m_pOutputBuffer->GetLength() > 0 )
    memcpy_s(buffer, size, m_pOutputBuffer->GetData(), m_pOutputBuffer->GetLength());

  return ( m_pOutputBuffer->GetLength() < size ? m_pOutputBuffer->GetLength() : size );
}

/**
 * Pushes the supplied character into the stream (INPUT)
 *
 * @param ch  Character to be pushed.
 */
void MemoryStream::Push(char ch)
{
  
  
  // =============================================
  // Check to make sure the etram is not closed
  // =============================================
  if ( IsClosed() ) 
    throw ex::StreamClosedException(LogRec("Memory stream is closed","MemoryStream","Push"));


  m_pInputBuffer->Append(ch);

  m_bCanRead = true;
}

/**
 * Returns the number of bytes available from the output pipe
 *
 * @return 
 */
const size_t MemoryStream::GetOutputSize() const
{
  return m_pOutputBuffer->GetLength();
} // GetOutputSize() 