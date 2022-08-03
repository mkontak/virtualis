// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PdfBuffer.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "PdfBuffer.h"
#include "PdfExceptions.h"

using namespace pdf;



/**
 * Constructor
 *
 *  @param pData
 *  @param iSize
 */
PdfBuffer::PdfBuffer(unsigned char * pData, size_t iSize) :
m_pData(pData),
m_iSize(iSize),
m_bOwner(false)
{
  Validate();

} // PdfBuffer(unsigned char * pData, size_t iSize)



/**
 * Default constructor
 *
 *  @param iSize
 */
PdfBuffer::PdfBuffer(size_t iSize) :
m_pData(__nullptr),
m_iSize(iSize),
m_bOwner(true)
{ 

  // Allocate a new internal buffer with a destructor
  m_pData = new unsigned char [iSize];

  // Check to make sure the memory was allocated
  if ( m_pData == __nullptr )
    throw PdfOutOfMemoryException(LogRec("Failed to allocate internal buffer in PdfBuffer","PdfBuffer","PdfBuffer"));

  // Fill memory with null
  Fill(0);

} // PdfBuffer(size_t iSize) 




/**
 * Copy constructor (Only copies the pointer)
 *
 *  @param buffer
 */
PdfBuffer::PdfBuffer(const PdfBuffer & buffer) :
m_pData(const_cast<PdfBuffer &>(buffer).m_pData),
m_iSize(const_cast<PdfBuffer &>(buffer).m_iSize)
{ 
} // PdfBuffer(const PdfBuffer & buffer)


/**
 * Destructor
 */
PdfBuffer::~PdfBuffer()
{ 
  Release(); 
} // ~PdfBuffer()


/**
 * Creates the PdfBuffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param pData     Data pointer
 * @param iSize     Size of the data pointer in characters
 *
 * @return shared PdfBuffer pointer
 */
std::shared_ptr<PdfBuffer> PdfBuffer::Create(const std::string & sPdf)
{
  std::shared_ptr<PdfBuffer> pPdfBuffer( new PdfBuffer((unsigned int)sPdf.size()) );

  if ( pPdfBuffer == __nullptr )
    throw PdfOutOfMemoryException(LogRec("Failed to allocate PdfBuffer object", "PdfBuffer", "Create"));
  
  memcpy(pPdfBuffer->m_pData,sPdf.c_str(), pPdfBuffer->m_iSize);

  return pPdfBuffer;
} // Create(unsigned char * pData, const size_t iSize, DcmDataTypes eType)



/**
 * Creates the PdfBuffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param pData     Data pointer
 * @param iSize     Size of the data pointer in characters
 *
 * @return shared PdfBuffer pointer
 */
std::shared_ptr<PdfBuffer> PdfBuffer::Create(unsigned char * pData, const size_t iSize)
{
  std::shared_ptr<PdfBuffer> pPdfBuffer( new PdfBuffer(pData,iSize) );

  if ( pPdfBuffer == __nullptr )
    throw PdfOutOfMemoryException(LogRec("Failed to allocate PdfBuffer object", "PdfBuffer", "Create"));

  return pPdfBuffer;
} // Create(unsigned char * pData, const size_t iSize, DcmDataTypes eType)


/**
 * Creates the PdfBuffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param iSize     Size of the data pointer in characters
 *
 * @return shared PdfBuffer pointer
 */
std::shared_ptr<PdfBuffer> PdfBuffer::Create(const size_t iSize)
{
  std::shared_ptr<PdfBuffer> pPdfBuffer(new PdfBuffer(iSize));

  if ( pPdfBuffer == __nullptr )
    throw PdfOutOfMemoryException(LogRec("Failed to allocate PdfBuffer object", "PdfBuffer", "Create"));

  return pPdfBuffer;
} // Create(const size_t iSize)


/**
 * Creates the PdfBuffer using the memory supplied. It is assumed that the memory is not owned and the 
 * user is responsible for cleanup.
 *
 * @param stream     input stream
 *
 * @return shared PdfBuffer pointer
 */
std::shared_ptr<PdfBuffer> PdfBuffer::Create(std::istream & stream)
{
  stream.seekg(0,std::ios::end);
  std::istream::pos_type imageSize = stream.tellg();
  stream.seekg(0,std::ios::beg);

  std::shared_ptr<PdfBuffer> pPdfBuffer(PdfBuffer::Create(imageSize));
 
  if ( pPdfBuffer == __nullptr )
    throw PdfOutOfMemoryException(LogRec("Failed to allocate PdfBuffer object", "PdfBuffer", "Create"));

  stream.read(pPdfBuffer->GetData<char>(), imageSize);

  if ( stream.fail() )
    throw PdfIOException(LogRec("Failed to read image from stream","PdfBuffer","Create"));

  pPdfBuffer->Validate();


  return pPdfBuffer;

} // Create(std::istream & stream, DcmDataTypes eType )


/**
 * Releases the internal resources
 */
void PdfBuffer::Release()
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
void PdfBuffer::Fill(char fillChar)
{
  if ( m_pData != __nullptr )
    memset((void *)m_pData,fillChar,m_iSize); 
} // Fill(char ch)


/**
 *   check if it is pdf
 *
 *  @param  pBuffer
 *  @param  iSize
 *  @return true if it is
 */
bool PdfBuffer::IsPdf(void * pBuffer, unsigned int iSize)
{  
  bool bIsPdf(false);
  
  if ( iSize >= 4 && strncmp("%PDF", (char *)pBuffer, 4) == 0 )
  {

    try
    {
      GetVersion(pBuffer, iSize);
      bIsPdf = true;
    }
    catch ( ... )
    {
      // Do Nothing
    }
  } // if
    
  return bIsPdf; 
} //IsPdf


/**
 * Gets the data type if the buffer
 *
 *  @param pBuffer
 *  @param iSize
 *  @return the string of version
 */
std::string PdfBuffer::GetVersion(void * pBuffer, unsigned int iSize)
{    
  // PDF version
  std::string sVersion;

  bool bFound(false);

  // ========================================================
  // Make sure there is enough data to read for the version
  // ========================================================
  if ( iSize >= 10 )
  {

    // Offset past the PDF tag so we can get the version
    char * ptr((char *)pBuffer + 5);

    // ============================================================
    // Loop through the next 5 characters looking for the version
    // ============================================================
    for (unsigned short i(0); i < 5; ++i)
    {
      // =======================================================================================
      // If the version is a BackSpace or a Vertical Tab then we are at the end of the version
      // =======================================================================================
      if ( ptr[i] == 10 || ptr[i] == 13)
      {
        sVersion.push_back('\0');
        bFound = true;
        break;
      } // if

      sVersion.push_back(ptr[i]);

    } // END ... For each on the next 5 characters

  } // END ... If the image buffer is big enough to have the version

  // If the version was not found
  if (! bFound)
    throw PdfInvalidDataException(LogRec("Unable to decode PDF version","PdfBuffer","GetPdfVersion"));

  return sVersion;

} // GetVersion()


/**
 * Validates the PdfBuffer
 */
void PdfBuffer::Validate()
{
  if ( IsPdf() )
    GetVersion(m_pData, (unsigned int)m_iSize);
  else if ( ! IsBitmap() )
    throw PdfInvalidDataException(LogRec("Invalid data supplied, must be pdf or bitmap", "PdfBuffer","Validate"));

} // Validate()