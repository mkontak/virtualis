// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Buffer.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#include "stdafx.h"
#include "Exception.h"
#include "vxBuffer.h"
#include "Base64.h"


/**
 * Default constructor
 */
Buffer::Buffer(size_t iSize ) :
m_pData(__nullptr),
m_iLength(0),
m_iSize(0),
m_encoding(BufferEncoding::ENCODING_ASCII)
{ 

  Allocate(iSize);

} // Buffer(size_t iSize) 


/**
 * Destructor
 */
Buffer::~Buffer()
{ 
  Release(); 
} // ~Buffer()



/**
 * Creates the Buffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param sValue     stdring
 *
 * @return shared Buffer pointer
 */
std::shared_ptr<Buffer> Buffer::Create(const unsigned char * pData, const size_t iDataSize)
{
  std::shared_ptr<Buffer> pBuffer(std::shared_ptr<Buffer>(new Buffer(iDataSize)));


  if ( pBuffer == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate Buffer object", "Buffer", "Create"));

  if ( pData != __nullptr )
    memcpy(pBuffer->m_pData.get(), pData, iDataSize);

  return pBuffer;
} // Create(const size_t iSize)

/**
 * Releases the internal resources
 */
void Buffer::Release()
{
  
  LogDbg("Releasing buffer", "Buffer","Release");

  m_pData.reset();

  m_pData = __nullptr;
  m_iSize = 0;
  m_iLength = 0;


} // Release()

/**
 * Fills the buffer with the supplied ch
 *
 * @param ch    Fill character
 */
void Buffer::Fill(char fillChar)
{

  if ( m_pData != __nullptr )
    memset((void *)m_pData.get() , fillChar, m_iSize ); 

} // Fill(char ch)


/**
 * Encodes the buffer and returns the encoded buffer
 *
 */
std::shared_ptr<Buffer> Buffer::ToBase64()
{
  std::shared_ptr<Buffer> encoded (  Base64::Encode(m_pData.get(), m_iLength) );

  encoded->m_encoding = BufferEncoding::ENCODING_BASE64;

  return encoded;
} // ToBase64()

/**
 * Appends the data to the internal shared pointer
 *
 * @param pData     Data to be copied in
 * @param iSize     Size of the data
 */
void Buffer::Append(const unsigned char ch)
{
 
  const size_t iRemaining ( m_iSize - m_iLength );

  // ===================================================================
  // If there is not enough space for the data then we need to allocate
  // ==================================================================
  if ( 1 > iRemaining )
    IncreaseSize(256);


  m_pData.get()[m_iLength++] = ch;
  
} // Appends(std::shared_ptr<unsigned char> & pData, const size_t iSize )


/**
 * Appends the data to the internal shared pointer
 *
 * @param pData     Data to be copied in
 * @param iSize     Size of the data
 */
void Buffer::Append(const unsigned char * pData, const size_t iSize )
{
 
  const size_t iRemaining ( GetRemainingFreeBytes() );

  // ===================================================================
  // If there is not enough space for the data then we need to allocate
  // ==================================================================
  if ( iSize > iRemaining )
    Allocate(m_iSize + iSize);

  // =====================================
  // Only insert data if is was not null
  // ======================================
  if ( pData != __nullptr )
  {

   LogDbg(util::Strings::Format("Appending to buffer %d bytes at offset %d",iSize, m_iLength), "Buffer", "Allocate");

    memcpy((m_pData.get() + m_iLength), pData, iSize);

    m_iLength += iSize;

  } // END ... If not null


} // Assign(std::shared_ptr<unsigned char> & pData, const size_t iSize )


/**
 * Allocates the specified size for the buffer. Will allocate additional memory saving the current data, but if the size is smaller then the 
 * current size the buffer will not be reallocated.
 *
 * @param iSize               Size 
 *
 */
void Buffer::Allocate(const size_t iSize )
{
 
  // ====================================================
  // Only allocate new memory if the new size is greater
  // ====================================================
  if ( iSize > m_iSize )
  {

    LogDbg(util::Strings::Format("Allocating buffer of size %d",iSize), "Buffer", "Allocate");

    std::shared_ptr<unsigned char> data = std::shared_ptr<unsigned char>(new unsigned char [ iSize ], [] ( unsigned char * p ) { delete [] p; } );

    // Make sure the buffer was allocated
    if ( data == __nullptr )
      throw ex::OutOfMemoryException(LogRec(util::Strings::Format("Failed to allocated buffer of size %ld", iSize),"Buffer","Allocate"));

    ZeroMemory(data.get(), iSize);

   

    // Copy data if there is any
    if ( m_pData != __nullptr )
      memcpy(data.get(), m_pData.get(), m_iLength);

    // Set new data
    m_pData.reset();
    m_pData = data;



    // Set the new size
    m_iSize = iSize;
   

  } // END ... If the new size is greater


} // Allocate(const size_t iSize)


/**
 * Creates a std::string representation of the internal data. Http data will be scrubbed to get
 * rid of the &gt; and &lt;.
 */
std::string Buffer::ToString()
{

  std::string sString((const char *)GetData(), GetLength());


  return sString;

} // ToString()

