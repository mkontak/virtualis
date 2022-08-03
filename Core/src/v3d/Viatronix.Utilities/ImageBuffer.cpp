



#include "stdafx.h"
#include "ImageBuffer.h"



// Namespaces
using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace Viatronix::Logging;

USING_IMAGING_NS

/**
 * Constructor
 */
 ImageBuffer::ImageBuffer() :
/**************************************************************************************************************************/
m_pUnmanagedBuffer(NULL),
m_pManagedBuffer(NULL), 
m_iLength(0), 
m_eType(BufferType::MANAGED),
m_iPosition(0),
m_bDisposed(false)
{
  Init();
} // ImageBuffer()

/**
 * Constructor
 */
ImageBuffer::ImageBuffer(void * pBuffer, int iBufferSize) : 
/*************************************************************************************************************************/
m_pUnmanagedBuffer((unsigned char *)pBuffer), 
m_iLength(iBufferSize), 
m_eType(BufferType::UNMANAGED),
m_iPosition(0),
m_bDisposed(false)
{
  Init();
} // ImageBuffer()

/**
 * Constructor
 */
ImageBuffer::ImageBuffer(unsigned char pBuffer __gc [], int iBufferSize) : 
/*************************************************************************************************************************/
m_pManagedBuffer(pBuffer), 
m_iLength(iBufferSize), 
m_eType(BufferType::MANAGED),
m_iPosition(0),
m_bDisposed(false)
{
  Init();
} // ImageBuffer()

/**
 *
 */
void ImageBuffer::Init()
{
  m_pGetChar = new GetCharDelegate * [2];
  m_pGetChar[0] = new GetCharDelegate(this, &ImageBuffer::GetCharManaged);
  m_pGetChar[1] = new GetCharDelegate(this, &ImageBuffer::GetCharUnmanaged);

  m_pPutChar = new PutCharDelegate * [2];
  m_pPutChar[0] = new PutCharDelegate(this, &ImageBuffer::PutCharManaged);
  m_pPutChar[1] = new PutCharDelegate(this, &ImageBuffer::PutCharUnmanaged);

} // INit()

/**
 * Dispose
 */
void ImageBuffer::Dispose()
{
  Dispose(true);
  System::GC::SuppressFinalize(this);
} // Dispose()

/**
 * Disposes of the object and cleans up any resources.
 *
 * @param bDisposing      Flag to indicate if the object is being disposed of by the GC or not.
 *
 * @return void.
 */
void ImageBuffer::Dispose(bool bDisposing)
{


  /////////////////////////////////////////////////
  // Make sure the object is not already disposed
  ////////////////////////////////////////////////
  if ( ! m_bDisposed )
  {

    Log::Debug("Disposing ImageBuffer", "ImageBuffer", "Dispose");

    ///////////////////////////////////////
    // Are we being disposed by the GC
    //////////////////////////////////////
    if ( bDisposing )
    {

    } // END ... If we are being disposed of
   
    delete [] m_pPutChar;
    delete [] m_pGetChar;

    Delete();

  } // END ... If the object is not already disposed of

  m_bDisposed = true;

} // Dispose()

/**
 * Allocate memory
 */
void ImageBuffer::New(BufferType eType, int iSize)
{

  /// Delete any previsout allcated memory
  Delete();


  if ( eType == BufferType::MANAGED )
  {
    m_pManagedBuffer = new unsigned char __gc [iSize];
  }
  else
  {
    m_pUnmanagedBuffer = new unsigned char [iSize];
  }

  m_bAllocated = true;
  m_iLength = iSize;

} // Allocate()

/**
 * Deletes any allocated memory
 */
void ImageBuffer::Delete()
{
  if ( m_bAllocated )
  {
    Log::Debug("Deleting allocated memory", "ImageBuffer", "Delete");

    if ( m_eType == BufferType::MANAGED )
    {
      if ( m_pManagedBuffer != NULL )
      {
        delete [] m_pManagedBuffer;
        m_pManagedBuffer = NULL;
      }
    }
    else
    {
      if ( m_pUnmanagedBuffer != NULL )
      {
        delete [] m_pUnmanagedBuffer;
        m_pUnmanagedBuffer = NULL;
      }
    }
    m_iLength = 0;
    m_iPosition = 0;
  }

  m_bAllocated = false;
} // Delete()

/**
 * Returns the character in the source at the current location + the offset 
 *
 * @param iOffset   Offset from the current location
 *
 * @return Returns the character at the (current location + iOffseet) in the source 
 */
unsigned char ImageBuffer::Peek(int iOffset)
{

  if ( (m_iPosition + iOffset) >= m_iLength )
  {
    throw new System::IO::EndOfStreamException("End of source image");
  }

  const int iIndex(m_iPosition + iOffset);

  if ( iIndex < 0 )
  {
    throw new System::Exception("Index is out of range");
  }
      
  return Char[iIndex];


} // Peek()

/**
 * Reads a single character from source image  
 *
 * @return Returns a single byte extracted from the source image buffer.
 */
unsigned char ImageBuffer::GetChar(void)
{

  if ( m_iPosition >= m_iLength )
  {
    throw new System::IO::EndOfStreamException("End of source image");
  }

  return Char[m_iPosition++];

} // GetChar()

/**
 * Reads a short from source image
 *
 * @param iWord     
 */
unsigned short ImageBuffer::GetWord(void)
{

  unsigned short iWord(0);

  iWord = (unsigned short)GetChar() << 8;

  iWord |= (unsigned short)GetChar();
  //const unsigned short iCh = (unsigned short)(GetChar());

	//iWord = (GetChar() << 8) | iCh;
	
  return iWord;

} // GetWord()

/**
 * Reads a block of data from source image
 *
 * @param Buffer        Buffer to load data into
 * @param iOffset       Offset into buffer
 * @param iBytes        Bytes to read
 *
 */
void ImageBuffer::Read(unsigned char Buffer __gc [], int iOffset, int iBytes)
{

  int i(0);

  for ( int j(iOffset); i < iBytes && ! EndOfStream ; i++, j++ )
  {
    Buffer[j] = GetChar();
  }

  if ( EndOfStream && i < iBytes )
  {
    throw new System::IO::EndOfStreamException("End of source image");
  }

} // Read()

/**
 * Skips the number of bytes specified
 *
 * @param iBytes      Number of bytes to skip
 */
void ImageBuffer::Skip(int iBytes)
{

  if ( m_iPosition >= m_iLength )
  {
    throw new System::IO::EndOfStreamException("End of source image");
  }

  const int iBytesRead(m_iPosition + iBytes);

  m_iPosition = ( iBytesRead > m_iLength ? m_iLength : iBytesRead );

} // Skip()


/**
 * Reads a row of data from the image
 */
int ImageBuffer::ReadRow( unsigned short * pBuffer, unsigned int iNumberOfPixels, unsigned short iBitsPerPixel )
{
  unsigned int iCount(0);

	if ( iBitsPerPixel <= 8 ) 
  {
    while ( ( iNumberOfPixels-- > 0 ) && ! EndOfStream ) 
    {
			*pBuffer++ = (unsigned short)GetChar();

      if ( m_iPosition >= m_iLength ) ++iCount;
		}
	}
	else 
  {
    while ( iNumberOfPixels-- > 0 && ! EndOfStream ) 
    {
      *pBuffer++ = GetWord();
      if ( EndOfStream ) ++iCount;
		}
	}

  return ( EndOfStream ? iCount : 0 );

} // ReadRow


