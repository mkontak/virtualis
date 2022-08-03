#pragma once

#include "Stream.h"


class VX_BASE_DLL Buffer;

namespace io
{


/**
 * Implements a memory stream based on a buffer
 */
class VX_BASE_DLL MemoryStream : public Stream
{

protected:

  MemoryStream::MemoryStream( const size_t iSize );

  MemoryStream(unsigned char * buffer, const size_t iSize);

public:

  ~MemoryStream() { Close(); }

public:


  /// Creates the network stream
  static std::shared_ptr<MemoryStream> Create(unsigned char * pBuffer, const size_t iSize);

  /// Creates the network stream
  static std::shared_ptr<MemoryStream> Create(const std::shared_ptr<char> & pBuffer, const size_t iSize)
  { return Create((unsigned char *)pBuffer.get(), iSize); }

  /// Creates the network stream
  static std::shared_ptr<MemoryStream> Create(const size_t iSize);

public:

  /// Reads bytes from the memory stream (Output)
  virtual uint4 Read(char * pBuffer, const size_t iBytesToRead, const int4 flags = MSG_WAITALL);

  /// Wriets to the memory stream (Input)
  virtual uint4 Write(const char * pBuffer, const size_t iBytesToWrite, const int4 flags = MSG_WAITALL);


  /// Close the stream
  virtual void Close();

  /// True if we can read, false otherwsie
  virtual bool CanRead();

  /// True is we can write, false otherwise
  virtual bool CanWrite();

  /// Pushes the character onto the input fer
  void Push(char ch);
 
  /// Copies the output to the supplied buffer
  uint4 CopyTo(char * pBuffer, const size_t iSize);

public:

  /// Returns the number of bytes available
  const size_t GetOutputSize() const;




private:

  /// Input Buffer (Reads)
  std::shared_ptr<Buffer>  m_pInputBuffer;

  /// Output buffer (Writes)
  std::shared_ptr<Buffer> m_pOutputBuffer;

  /// Bytes read from input
  size_t m_iRead;

  /// Flag indicating we can read
  bool m_bCanRead;

  /// Flag to indicate we can write
  bool m_bCanWrite;

  /// Indexer into the input buffer used for loading
  size_t m_iIndexer;

}; // class MemoryStream

}