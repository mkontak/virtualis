// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Buffer.h
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#pragma once


#pragma warning(push)
#pragma warning(disable:4251)

#define BUFFER_END -1




/**
 * Buffer encoding values
 */
typedef enum BufferEncoding
{
  ENCODING_ASCII = 0,
  ENCODING_BASE64 = 1
} BufferEncoding;


/**
 * Implements a BUFFER to be used within the system
 */
class VX_BASE_DLL Buffer
{

protected:

  /// Constructor
  Buffer(size_t iSize = 0);

public:



  /// Destructor
  virtual ~Buffer();

private:

  /// Copy constructor (Only copies the pointer)
  Buffer(const Buffer & buffer) {}



#pragma endregion


#pragma region methods

#pragma region Create Methods

public:

  /// Creates the Buffer of a specific size (This memory is owned and will be cleaned up)
  static inline std::shared_ptr<Buffer> Create(const size_t iSize = 0)
  { return Create((const unsigned char *)__nullptr, iSize); }

  /// Creates the Buffer of a specific size (This memory is owned and will be cleaned up)
  static inline std::shared_ptr<Buffer> Create(const std::string & sValue)
  { return Create((unsigned char *)sValue.c_str(), sValue.size()); }

  /// Creates the buffer using the supplied memeory and length
  static std::shared_ptr<Buffer> Create(const unsigned char * pData, const size_t iDataSize);

  /// Creates the buffer using the supplied memeory and length
  static std::shared_ptr<Buffer> Create(const char * pData, const size_t iDataSize)
  { return Create((const unsigned char *)pData, iDataSize); }

#pragma endregion



public:

  
  /// Converts the contnts to this buffer to base 64
  std::shared_ptr<Buffer> ToBase64();
 
  /// Append the character to the end of the buffer
  void Append(const unsigned char ch);

   /// Append the string
  void Append(const std::string & str)
  { if ( ! str.empty() ) Append(str.c_str(), str.size()); }


  /// Appends the supplied buffer to the end of the buffer
  void Append(const unsigned char * pData, const size_t iSize);

  /// Appends the supplied buffer to the end of the buffer
  void Append(const char * pData, const size_t iSize)
  { Append((const unsigned char *)pData, iSize); }

  /// Apppend buffer
  void Append(std::shared_ptr<Buffer> buffer)
  { if ( buffer != __nullptr && buffer->GetLength() > 0 ) Append(buffer->GetData(), buffer->GetLength()); }

  /// Releases the internal resources
  void Release();

  /// Fills the buffer with the supplied character
  void Fill(char ch);

  /// Zero fills the buffer
  inline void Zero()
  { Fill('\0'); }

  /// Returns true if the buffer is empty
  bool IsEmpty()
  { return ( m_pData == __nullptr || ( m_pData != __nullptr && m_iLength == 0 ) ); }

  /// Return true if the number of bytes specified can fit in the buffer false otherwise
  bool CanFit(const size_t iBytes)
  { return ( GetRemainingFreeBytes() >= iBytes ); }

  /// Allocates memory re-allocate a new buffer of the size speicifed
  void Allocate(const size_t iSize );
  void IncreaseSize(const size_t iSize )
  { Allocate( m_iSize + iSize); }

  /// Set the used amount
  void IncrementLength(size_t iIncrement) 
  { SetLength(m_iLength + iIncrement); }


  /// REturns the data as a string
  std::string ToString();

public:

  /// Get the data pointer at the offset specified
  unsigned char * GetData(const size_t iOffset = 0)
  { return (  m_pData.get() + ( iOffset == -1 ? ( m_iSize == m_iLength ? 0 : m_iLength ) : iOffset ) ); }

  /// Get the data pointer
  std::shared_ptr<unsigned char> & GetBuffer()
  { return m_pData; }

  /// Gets the size of the buffer
  const size_t GetSize() const
  { return m_iSize; }

  /// Gets the length of the data (If the length is 0 assume that it is the size)
  const size_t GetLength() const
  { return ( m_iLength == 0 ? m_iSize : m_iLength ); }

  const size_t GetRemainingFreeBytes() const
  { return m_iSize - m_iLength; }

  /// Set the used amount
  void SetLength(size_t iLength) 
  { m_iLength = ( iLength > m_iSize ? m_iSize : iLength ); }



  /// Gets the buffers encoding
  const BufferEncoding & GetEncoding() const
  { return m_encoding; }

#pragma endregion

#pragma region fields

private:

  /// Data
  std::shared_ptr<unsigned char> m_pData;

  /// Size (Total Size of the buffer)
  size_t m_iSize;

  /// Amount used (Length of the data in the buffer)
  size_t m_iLength;

  /// Data as string
  std::string m_sData;

  /// Encoding
  BufferEncoding m_encoding;


#pragma endregion


};  // class Buffer




#pragma warning(pop)