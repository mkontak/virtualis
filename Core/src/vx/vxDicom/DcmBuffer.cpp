// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmBuffer.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmEnums.h"
#include "DcmBuffer.h"


using namespace dcm;



/**
 * Constructor
 *
 *  @param pData
 *  @param iSize
 *  @param eType
 */
DcmBuffer::DcmBuffer(unsigned char * pData, size_t iSize, DcmDataTypes eType) :
m_pData(pData),
m_iSize(iSize),
m_eType(eType),
m_bOwner(false)
{

  DetermineDataType();

} // DcmBuffer(unsigned char * pData, size_t iSize)



/**
 * Default constructor
 *
 *  @param iSize
 *  @param eType
 */
DcmBuffer::DcmBuffer(size_t iSize, DcmDataTypes eType ) :
m_pData(__nullptr),
m_eType(eType),
m_iSize(iSize),
m_iActualSize(iSize),
m_bOwner(true)
{ 

  /// Always round up to an even boundary
  if ( m_iSize % 2 == 1 )
    m_iSize++;

  // Allocate a new internal buffer with a destructor
  m_pData = new unsigned char [m_iSize];

  // Check to make sure the memory was allocated
  if ( m_pData == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate internal buffer in DcmBuffer","DcmBuffer","DcmBuffer"));

  // Fill memory with null
  Fill(0);

} // DcmBuffer(size_t iSize) 




/**
 * Copy constructor (Only copies the pointer)
 *
 *  @param buffer
 */
DcmBuffer::DcmBuffer(const DcmBuffer & buffer) :
m_pData(const_cast<DcmBuffer &>(buffer).m_pData),
m_iSize(const_cast<DcmBuffer &>(buffer).m_iSize),
m_eType(const_cast<DcmBuffer &>(buffer).m_eType),
m_bOwner(false)
{ 
} // DcmBuffer(const DcmBuffer & buffer)


/**
 * Destructor
 */
DcmBuffer::~DcmBuffer()
{ 
  Release(); 
} // ~DcmBuffer()


/**
 * Creates the DcmBuffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param pData     Data pointer
 * @param iSize     Size of the data pointer in characters
 *
 * @return shared DcmBuffer pointer
 */
std::shared_ptr<DcmBuffer> DcmBuffer::Create(unsigned char * pData, const size_t iSize, DcmDataTypes eType)
{
  std::shared_ptr<DcmBuffer> pDcmBuffer(std::make_shared<DcmBuffer>(pData,iSize,eType));

  if ( pDcmBuffer == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmBuffer object", "DcmBuffer", "Create"));

  return pDcmBuffer;
} // Create(unsigned char * pData, const size_t iSize, DcmDataTypes eType)


/**
 * Creates the DcmBuffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param iSize     Size of the data pointer in characters
 * @param eType     Data type
 *
 * @return shared DcmBuffer pointer
 */
std::shared_ptr<DcmBuffer> DcmBuffer::Create(const size_t iSize, DcmDataTypes eType)
{
  std::shared_ptr<DcmBuffer> pDcmBuffer(std::make_shared<DcmBuffer>(iSize, eType));

  if ( pDcmBuffer == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmBuffer object", "DcmBuffer", "Create"));

  return pDcmBuffer;
} // Create(const size_t iSize)


/**
 * Creates the DcmBuffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param stream     input stream
 *
 * @return shared DcmBuffer pointer
 */
std::shared_ptr<DcmBuffer> DcmBuffer::Create(std::istream & stream, DcmDataTypes eType )
{
  stream.seekg(0,std::ios::end);
  std::istream::pos_type imageSize = stream.tellg();
  stream.seekg(0,std::ios::beg);

  std::shared_ptr<DcmBuffer> pDcmBuffer(DcmBuffer::Create(imageSize));
  pDcmBuffer->SetDataType(eType);

  if ( pDcmBuffer == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmBuffer object", "DcmBuffer", "Create"));

  stream.read(pDcmBuffer->GetData<char>(), imageSize);

  if ( stream.fail() )
    throw DcmIOException(LogRec("Failed to read image from stream","DcmBuffer","Create"));

  pDcmBuffer->DetermineDataType();

  return pDcmBuffer;

} // Create(std::istream & stream, DcmDataTypes eType )

/**
 * Clones the buffer making a copy
 *
 * @return new buffer
 */
std::shared_ptr<DcmBuffer> DcmBuffer::Clone()
{
  std::shared_ptr<DcmBuffer> clone(DcmBuffer::Create(m_iSize));

  CopyTo(clone->m_pData, clone->m_iSize, 0);

  return clone;
} // Clone()

/**
 * Releases the internal resources
 */
void DcmBuffer::Release()
{
  
  if ( m_bOwner && m_pData != __nullptr )
    delete [] m_pData;

  m_pData = __nullptr;
  m_iSize = 0;


} // Release()


/**
 * Fills the buffer with the supplied ch
 *
 * @param ch    Fill character
 */
void DcmBuffer::Fill(char fillChar)
{
  if ( m_pData != __nullptr )
    memset((void *)m_pData,fillChar,m_iSize); 
} // Fill(char ch)


/**
 * Assigns the data to the internal shared pointer
 *
 *  @param pData
 *  @param iSize
 *  @param bTakeOwnership
 */
void DcmBuffer::Assign(unsigned char * pData, const unsigned int iSize, bool bTakeOwnership )
{
  Release();

  m_pData = pData;
  m_bOwner = bTakeOwnership;
  m_iSize = iSize;
} // Assign


/**
 * Gets the data type if the buffer
 */
void DcmBuffer::DetermineDataType()
{

  if ( m_eType == DATA_ANY )
  {
 
    const unsigned char png[4] = { 0x89, 'P', 'N', 'G' };

    unsigned char * pData((unsigned char *)m_pData);
    unsigned char * pData6(pData + 6);
    unsigned char * pData24(pData + 24);


    if ( m_iSize >= 4 && _mbsncmp(png, pData, 4) == 0 )
      m_eType = DATA_PNG;
    else if ( m_iSize >= 2 && _mbsncmp((unsigned char *)"BM", pData, 2) == 0 )
      m_eType = DATA_BITMAP;
    else if ( m_iSize >= 5 && _mbsncmp((unsigned char *)"%PDF-", pData, 5) == 0 )
      m_eType = DATA_PDF;
    else if ( m_iSize >= 28 && 
              ( _mbsncmp((unsigned char *)"Exif", pData6, 4) == 0   || 
                _mbsncmp((unsigned char *)"Exif", pData24, 4) == 0  || 
                _mbsncmp((unsigned char *)"JFIF", pData6, 4) == 0   || 
                _mbsncmp((unsigned char *)"JFIF", pData24, 4) == 0  ) )
      m_eType = DATA_JPEG;
    else
      m_eType = DATA_GENERIC;
  }
    
} // DetermineDataType()

/**
 * Copies the pixel data to the buffer supplied. Will copy the specified image size
 *
 * @param pBuffer       Destination buffer
 * @param iSize         Buffer size
 * @param iPosition     Starting position
 */
size_t DcmBuffer::CopyTo(unsigned char * pBuffer, const size_t iSize, const size_t iPosition)
{

  const size_t iRemaining(m_iSize - iPosition);
  const int iBytesCopied( iSize < iRemaining ? iSize : iRemaining );

  memcpy(pBuffer, (m_pData + iPosition), iBytesCopied);

  return iBytesCopied;

} // CopyTo()

/**
 * Copies the pixel data to the DcmBuffer supplied.
 *
 * @param buffer      Buffer to copy to
 * @param Size        Byte to be copied
 * @param iPosition   Starting position
 *
 * @return  Number of bytes copied
 */
size_t DcmBuffer::CopyTo( std::shared_ptr<DcmBuffer> & buffer, const size_t iSize,  const size_t iPosition  )
{
  if ( buffer->GetSize() >= iSize )
    return CopyTo(buffer->GetData<unsigned char>(), iSize, iPosition);
  else 
    throw DcmException(LogRec("Buffer supplied not big enough for copy", "DcmBuffer", "CopyTo"));
} // CopyTo( std::shared_ptr<DcmBuffer> & buffer, const size_t iSize,  const size_t iPosition = 0 )

/**
 * Appends the current buffer to this buffer
 *
 * @param  buffer       Buffer to be appended
 */
void DcmBuffer::Append(std::shared_ptr<DcmBuffer> buffer)
{

  
  unsigned char * pHolder = m_pData;
  size_t iHolderSize = m_iActualSize;

  m_iActualSize += buffer->m_iActualSize;

  if ( m_iActualSize % 2 == 1 )
    m_iSize = m_iActualSize + 1;

  m_pData = new unsigned char [m_iSize];

  // Clear the memory
  memset(m_pData,0,m_iSize);

  // Copy original data
  memcpy(m_pData, pHolder, iHolderSize);

  // Append the new data
  memcpy(m_pData + iHolderSize, buffer->GetData<unsigned char>(), buffer->m_iActualSize);
  
  /// Delete old memory
  delete [] pHolder;

} // Append(std::shared_ptr<DcmBuffer> buffer)