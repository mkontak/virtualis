// $Id: PNGImage.C,v 1.2 2006/02/02 20:02:38 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: David dave@viatronix.com
//
// Complete History at bottom of file.


/**
 * define PNG utilities package
 */

// include declarations
#include "stdafx.h"
#include "PNGImage.h"
#include "Inflate.h"
#include "File.h"
#include "FilePtr.h"
#include "V2kVersion.h"
#include "lzo1x.h"
#include "utilities.h" 



using namespace std;



#define FILE_REVISION "$Revision: 1.2 $"



/**
 * Default Constructor.
 * @param bKeepUnkown whether to read chunks with unknown names, if false and reading a large volume file
 * the specially named chunk with the 3D volume data will not be read and no space allocated for it.
 * @param dataPtr kludge, ugly hack, not to be used.  Not extensible b/c will only work with PNG images
 * with 1 unknown compressed chunk, in that case compressed data is inflated directly into the dataPtr memory space.
 */
PNGImage::PNGImage(bool bKeepUnknown, void * pData) :m_fv(pair<int4,int4>(1,0)), m_fvReadIn(pair<int4,int4>(0,0)),
  m_uWidth(74), m_uHeight(117)
{
  m_bKeepUnknown = bKeepUnknown;
  m_pData = pData;

  m_iBitDepth = 8;
  m_iColorType = PNG_COLOR_TYPE_RGB;
  m_uRowBytes = m_uWidth * 3; // * (bitDepth/8) if depth != 8
  m_vData.resize(m_uHeight * m_uRowBytes);
  InitIcon();

  //PNGImage(m_uWidth, m_uHeight);
  m_pngPtr = NULL;
} // Constructor


/**
 * Constructor with width, height, bitDepth, colorType and optional buffer.
 * @param buffer if non-NULL the given buffer is copied into the PNG buffer,
 * buffer size must be correct for the colortype, width, height or it is ignored.
 * /todo convert ignore to exception ??
 */
PNGImage::PNGImage(const uint4 & uWidth, const uint4 & uHeight, int4 iBitDepth, 
                   int4 iColorType, const uint1 * pBuffer, uint4 uBufSize) : 
m_fv(pair<int4,int4>(1,0)), m_fvReadIn(pair<int4,int4>(0,0)),
  m_uWidth(uWidth), m_uHeight(uHeight)
{
  m_bKeepUnknown = true;
  m_iBitDepth    = iBitDepth;
  m_iColorType   = iColorType;

  int4 iNColors = 0;
  if ( m_iColorType == PNG_COLOR_TYPE_RGB )
  {
    iNColors = 3;
  }
  else if ( m_iColorType == PNG_COLOR_TYPE_GRAY )
  {
    iNColors = 1;
  }
  else
  {
    throw ex::UnsupportedOperationException(LogRec("PNGImage::PNGImage() - m_iColorType not yet supported or invalid.",
      "PNGImager", "PNGImage"));
  }

  if ( m_iBitDepth == 1 )
  {
    m_uRowBytes = static_cast<png_uint_32> (ceil((m_uWidth * iNColors) / 8.0));
  }
  else
  {
    m_uRowBytes = m_uWidth * iNColors * (m_iBitDepth/8);
  }

  uint4 uSpace = m_uHeight * m_uRowBytes;
  m_vData.resize(uSpace);
  if ( uBufSize != 0 && pBuffer != NULL )
  {
    if ( uBufSize == uSpace )
    {
      const uint1 * pBuf2 = pBuffer;
      uint1 * pngPtr = &*m_vData.begin();
      for ( ; pngPtr != &*m_vData.end(); ++pBuf2, ++pngPtr )
      {
        *pngPtr = *pBuf2; // plead the 5th about this
      }
    } 
    else
    {
      // convert to exception ??
      LogWrn("<PNGImage> Input buffer size invalid, ignoring buffer.", "PNGImager", "PNGImage");
    }
  }

  m_pngPtr = NULL;
} // Constructor


/**
 * Copy Constructor.
 */
PNGImage::PNGImage(const PNGImage & rhs)
{
  Copy(rhs);
}


/**
 * Assignment operator.
 * @param rhs the right hand side.
 * @reture the new instance.
 */
PNGImage const & PNGImage::operator=(const PNGImage & rhs)
{
  //check for auto-assignment using memory address
  if ( this == &rhs )
  {
    return(*this);
  }

  Destroy();
  Copy(rhs);

  return(*this);
} // operator=


/**
 * Destructor.
 */
PNGImage::~PNGImage()  
{
  Destroy();
}


/**
 * @param sFilename
 */
void PNGImage::Read(const std::string & sFilename) 
{
  cout<<"<PNGImage::Read> Reading PNG file: " << sFilename<< endl;

  FilePtr pngFile;
  try
  {
    pngFile.Open(sFilename, "rb"); // could throw
  }
  catch (ex::FileNotFoundException & )
  {
    throw ex::FileNotFoundException(LogRec("<PNGImage::Read>", "PNGImager", "Read"));
  }

  const int4 iBytesRead = 4;
  uint1 uHeader[iBytesRead + 1];
  pngFile.Read(uHeader, iBytesRead); // could throw
  uHeader[iBytesRead] = '\0';

  // checks first 5 bytes to see if this is a PNG image
  if ( png_sig_cmp(uHeader, 0, iBytesRead) )
  {
    throw ex::IOException(LogRec("IO exception error-read error: signature does not match correct PNG file signature <PNGImage::Read>",
      "PNGImager", "Read"));
	}

  try
  {
    // could pass in memory pointers here to take over mem. allocation within library, reference libpng
    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PNGImage::LibPNGError, NULL);
    m_pngPtr = pngPtr;
    if ( pngPtr == NULL )
    {
      throw ex::OutOfMemoryException(LogRec("Out of memory read error at stage 'reading PNG header' <PNGImage::Read>",
              "PNGImager", "Read"));
    }
 
    png_infop infoPtr = png_create_info_struct(pngPtr);
    if ( infoPtr == NULL )
    {
      throw ex::OutOfMemoryException(LogRec("Out of memory read error at stage 'reading PNG header' <PNGImage::Read>",
        "PNGImager", "Read"));
    }

    // call setjmp() for error handling, called by libPng when the shit hits the fan
    if ( setjmp(png_jmpbuf(pngPtr)) )
    {
      png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
      throw ex::DataCorruptionException(LogRec("data corruption read error at stage 'reading PNG header': data corruption: "
        "internal libpng error likely due to checksum mismatch <PNGImage::Read>","PNGImager", "Read"));
    }

    // this method of getting unknown chunks is no longer used because libpng
    // quietly destroys the memory since the chunk is not longer considered unknown
    // as of version 1.0.8
    //png_voidp userChunkPtr = this; // store ptr
    // tell libpng which function to call for unknown chunks.
    //png_set_read_user_chunk_fn(pngPtr, userChunkPtr, PNGImage::ReadChunkCallback);
    if ( m_bKeepUnknown )
    {
      int4 iKeep = 2; // 0-do not keep, 1-keep safe to copy, 2-keep unsafe to copy
      int4 iChunks = 0; //number of chunks affected, 0 means all are affected
      png_set_keep_unknown_chunks(pngPtr, iKeep, '\0', iChunks);
    } 
    else
    {
      png_set_keep_unknown_chunks(pngPtr, 0, '\0', 0);  
    }
  
    // tell libpng to use streams instead of fread()
    png_set_read_fn(pngPtr, &pngFile, PNGImage::ReadData);
    //tell libpng how many bytes already read
    png_set_sig_bytes(pngPtr, iBytesRead);
    png_read_info(pngPtr, infoPtr);
    //get width, height from header
    png_get_IHDR(pngPtr, infoPtr, &m_uWidth, &m_uHeight, &m_iBitDepth, &m_iColorType, NULL, NULL, NULL);

    //don't check for background color, gamma, palette
    //int numPlt; png_colorp palette;
    //png_get_PLTE(pngPtr, infoPtr,  &palette, &numPlt);
    //don't update infoptr for image transformations
    m_uRowBytes = png_get_rowbytes(pngPtr, infoPtr);
    m_vData.resize(m_uRowBytes * m_uHeight);  // could throw OutofMemory
    // reads image into the allocated space
    png_read_image(pngPtr, &*GetRowPointers().begin());
    // parse header info stored in Comment chunk
    png_textp pngTextp;
    
    int4 numText = png_get_text(pngPtr, infoPtr, &pngTextp, NULL);
    for (int4 i = 0; i < numText; ++i, ++pngTextp )
    {
      if ( string(pngTextp->key) == "Software" )
      { // this is the version 0number
        StoreVersionNumber(pngTextp->text);
      } 
      else
      {
        textChunk tc;
        tc.sName = pngTextp->key;
        tc.sData = pngTextp->text;
        tc.iCompression = PNG_TEXT_COMPRESSION_NONE;
        m_text.push_back(tc);
      }
    } // for numText

    if ( m_bKeepUnknown )
    { // this will cause the 3D data to be read in a VX volume
      png_read_end(pngPtr, infoPtr);
    }
    png_unknown_chunkp chunks;

    // parse all unknown chunks
    // if compressed, decompress to new space & delete compressed or decompress to input area
    int4 iNumChunks = png_get_unknown_chunks(pngPtr, infoPtr, &chunks);
    for ( i = 0; i < iNumChunks; ++i, ++chunks )
    {
      if ( chunks->name[3] != 'n' )
      { // some compression used
        uint4 uDestLen;
        memcpy(&uDestLen, chunks->data, 4); // set destLen from first bytes
        hdr::FileCompressionEnum eCompression;
        if ( chunks->name[3] == 'z' )
        {
          eCompression = hdr::ZLIB;
        }
        else
        {
          eCompression = hdr::LZO;
        }

        uint1 * pDest = NULL;
        if ( m_pData == NULL )
        {
          // png_malloc defaults to new normally although this can't be counted on
          // must use png_malloc currently b/c if the chunk isn't compressed it stays as allocated
          // by PNG lib otherwise the compressed chunk is replaced by the uncompressed
          pDest = static_cast<uint1 *>(png_malloc(pngPtr, uDestLen)); 
          if(pDest == NULL)
          {
            throw ex::OutOfMemoryException(
              LogRec("Out of memory read error at stage 'reading PNG header': Out of memory <PNGImage::Read>",
              "PNGImager", "Read"));
          }
        }
        else // hack from constructor
        {
          pDest = static_cast<uint1 *>(m_pData);
        }

        Inflate i;
        i.InflateIt(eCompression, chunks->data+4, chunks->size - 4, pDest, uDestLen);

        // delete compressed data
        if ( m_pData == NULL )
        {
          png_free(pngPtr, chunks->data);
          chunks->data = pDest;
          chunks->size = uDestLen;
        }
      } // when compression used

      if ( m_pData == NULL ) // when not NULL data was inflated directly to the right place without intermediate buffer
      {
        m_chunks.push_back(*chunks);
      }
    } // for each unknown chunk

    if ( m_pData == NULL )
    {
      png_data_freer(pngPtr, infoPtr, PNG_USER_WILL_FREE_DATA, PNG_FREE_UNKN);
    }
    png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
  }
  catch (ex::OutOfMemoryException)
  {
    throw;
  }
  catch (ex::DataCorruptionException)
  {
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec("error read data files read error at stage 'reading PNG header'<PNGImage::Read>","PNGImager", "Read"));
  }

  return;
} // Read


/**
 * Writes the PNG image to the specified file.
 * @param sFilename
 */
void PNGImage::Write(const std::string & sFilename) 
{
  LogDbg("<PNGImage::Write> Storing file "+ToAscii(sFilename)+" ... ", "PNGImage", "Write");
  FilePtr pngFile;
  pngFile.Open(sFilename, "wb");
  
  png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if ( pngPtr == NULL )
  {
    throw ex::OutOfMemoryException(LogRec("Error creating write struct","PNGImager", "Write"));
  }

  png_infop infoPtr = png_create_info_struct(pngPtr);
  if ( infoPtr == NULL )
  {
    png_destroy_write_struct(&pngPtr, (png_infopp) NULL);
    throw ex::OutOfMemoryException(LogRec("Error creating info struct","PNGImager", "Write"));
  }

  if (setjmp(png_jmpbuf(pngPtr)))
  {
    png_destroy_write_struct(&pngPtr, &infoPtr);
    throw ex::IOException(LogRec("<PNGImage::Read> internal libpng error.","PNGImager", "Write"));
  }

  // tell libpng about the new write function
  png_set_write_fn(pngPtr, (void *) &pngFile, PNGImage::WriteData, PNGImage::WriteFlushData);

  png_set_IHDR(pngPtr, infoPtr, m_uWidth, m_uHeight, m_iBitDepth, m_iColorType,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  //could add gamma chunk here

  // add text chunk for version number
  string sMajor = ToAscii(m_fv.first);
  string sMinor = ToAscii(m_fv.second);
  string sVersion = sMajor;
   sVersion+= " ";
   sVersion+= sMinor+ " BUILD:";
   sVersion+= V2K_VERSION;
  SetText("Software", sVersion);

  //prepare to write text chunks
  int4 iNumChunks   = static_cast< int4 >( m_text.size() );
  png_textp pngText = new png_text[iNumChunks];
  list<textChunk>::iterator text = m_text.begin();

  for ( int4 i=0; i < iNumChunks; ++text, ++i )
  {
    pngText[i].compression = text->iCompression;;
    pngText[i].key  = const_cast<char *> (text->sName.c_str());
    pngText[i].text = const_cast<char *> (text->sData.c_str());
    pngText[i].text_length = text->sData.size();
  }

  png_set_text(pngPtr, infoPtr, pngText, iNumChunks);

  png_write_info(pngPtr, infoPtr);

  //write volume icon
  png_write_image(pngPtr, &*GetRowPointers().begin());

  // tell lippng about extra chunks if they exist
  if ( m_chunks.size() > 0 )
  {
    png_unknown_chunkp pngChunk = new png_unknown_chunk[m_chunks.size()];

    list<png_unknown_chunk>::iterator chunk = m_chunks.begin();
    for ( uint4 i2=0; i2 < m_chunks.size(); ++chunk, ++i2 )
    {
      pngChunk[i2] = *chunk;
    }

    png_set_unknown_chunks(pngPtr, infoPtr, pngChunk, static_cast< int4 >( m_chunks.size() ));
    infoPtr->unknown_chunks[0].location = 0x08; //set write location

    png_write_end(pngPtr, infoPtr);
    png_destroy_write_struct(&pngPtr, &infoPtr);

    delete [] pngChunk;
  } 
  else
  {
    png_write_end(pngPtr, infoPtr);
    png_destroy_write_struct(&pngPtr, &infoPtr); // clean up memory
  }

  delete [] pngText;

  // delete compressed chunks if exist
  list<png_unknown_chunk>::iterator chunk = m_chunks.begin();
  for ( uint4 i3=0; i3 < m_chunks.size(); ++chunk, ++i3 )
  {
    if ( chunk->name[3] != 'n' )
    {
      delete [] chunk->data;
    }
  }

  m_chunks.clear();
  LogDbg(" Write was successful", "PNGImage", "Write");
  return;
} // Write


/**
 * Returns the image width. Doesn't return & of because of type conversion to uint, change ??
 */
uint4 PNGImage::GetWidth() const
{
  return (m_uWidth);
} // GetWidth()


/**
 * @returns the image height.
 */
uint4 PNGImage::GetHeight() const
{
  return (m_uHeight);
} // GetHeight()


/**
 * @returns the bit depth.
 */
const int4 & PNGImage::GetBitDepth() const
{
  return (m_iBitDepth);
} // GetBitDepth()


/**
 * @returns the color type.
 */
const int4 & PNGImage::GetColorType() const
{
  return (m_iColorType);
} // GetColorType()


/**
 * Gets the pixel at (x,y).
 * @param uX
 * @param uY
 * @return
 */
const Triple<uint1> PNGImage::GetPixel(uint4 uX, uint4 uY) const
{
  if ( uX >= m_uWidth || uY >= m_uHeight )
  {
    LogWrn(" <PNGImage::GetPixel> At coordinates: "+ToAscii(uX)+" "+ToAscii(uY), "PNGImage", "GetPixel");
    uX=0; uY=0;
  }
  uint8 uOffset = uY*m_uRowBytes + 3*uX;

  return( Triple<uint1>(m_vData[uOffset], m_vData[uOffset+1], m_vData[uOffset+2]) );
} // GetPixel


/**
 * Sets the pixel at (x,y).
 * @param uX
 * @param uY
 * @param color
 */
void PNGImage::SetPixel(uint4 uX, uint4 uY, const Triple<uint1> & color)
{
  if ( uX >= m_uWidth || uY >= m_uHeight )
  {
    LogWrn(" At coordinates: "+ToAscii(uX)+" "+ToAscii(uY), "PNGImage", "SetPixel");
    return;
  }

  uint8 uOffset = uY*m_uRowBytes + 3*uX;
  m_vData[uOffset  ] = color.m_x;
  m_vData[uOffset+1] = color.m_y;
  m_vData[uOffset+2] = color.m_z;
} // SetPixel


/**
 * @param uX
 * @param uY
 * @param uDensity
 */
void PNGImage::SetPixel(uint4 uX, uint4 uY, const uint2 & uDensity)
{
  if ( uX >= m_uWidth || uY >= m_uHeight )
  {
    LogWrn(" <PNGImage::SetPixel> At coordinates: "+ToAscii(uX)+" "+ToAscii(uY), "PNGImage", "SetPixel");
    return;
  }

  uint8 uOffset = uY * m_uRowBytes + (m_iBitDepth/8 * uX);
  m_vData[uOffset] = uDensity;
} // SetPixel



/**
 * Returns the text chunk with the given name or empty string
 * @param sTextName
 * @return
 */
const std::string & PNGImage::GetText(const std::string & sTextName) const
{
  list< textChunk >::const_iterator text = m_text.begin();

  for ( ; text != m_text.end(); ++text )
  {
    if ( text->sName == sTextName )
    {
      return(text->sData);
    }
  }

  return (m_sEmptyStr);
} // GetText


/**
 * @param sTextName
 * @param sText
 * @param bCompress
 */
void PNGImage::SetText(const std::string & sTextName, const std::string & sText, 
                       bool bCompress)
{
  int4 iCompType = 0;
  if ( bCompress ) 
  {
    iCompType = PNG_TEXT_COMPRESSION_zTXt;
  }
  else
  {
    iCompType = PNG_TEXT_COMPRESSION_NONE;
  }

  // check if already there
  list< textChunk >::iterator stored = m_text.begin();
  for ( ; stored != m_text.end(); ++stored )
  {
    if ( stored->sName == sTextName )
    {
      stored->sData = sText;
      stored->iCompression = iCompType;
      return;
    }
  }

  // push new entry to the back
  textChunk tc;
  tc.sName = sTextName;
  tc.sData = sText;
  tc.iCompression = iCompType;
  m_text.push_back(tc);
} // SetText


/**
 * Sets a chunk for png to write. The chunk name must be 4 characters and they
 * thould be lowercase, lowercase, uppercase, lowercase as in 'vdA' to
 * conform to PNG naming conventions.
 * Note that dataLength is currently limited to 32 bits !!
 * @param vName must be 3 characters, fourth character stores compression type
 * @param pData
 * @uDataLength
 * @param iCompType
 */
void PNGImage::SetChunk(uint1 vName[3], uint1 * pData, uint4 uDataLength, uint4 iCompType)
{
  png_unknown_chunk chunk;

  uint1 vName2[5] = { 0,0,0,0, '\0'};
  memcpy(vName2, vName, 3); // copy first 3 bytes to name2

  // set chunk data and data length, compress if needed
  switch ( iCompType )
  {
  case hdr::NONE:
    vName2[3] = 'n';
    chunk.data = pData;
    chunk.size = uDataLength;
    break;
  case hdr::LZO:
    {
      vName2[3] = 'l';
      // those code sure looks shitty, clean up??
      bool bInitOkay = true;
      if ( lzo_init() != LZO_E_OK )
      {
        bInitOkay = false;
      }
      lzo_byte * pLzoWrkmem = new uint1 [LZO1X_1_MEM_COMPRESS];

      uint4 uSpace2Compress = (uDataLength + (uDataLength / 64) + 16 + 3) + 4;
      chunk.data = new uint1[uSpace2Compress];
      // store uncomp. length
      memcpy(chunk.data, &uDataLength, 4);
      uint4 uDestLen = uSpace2Compress - 4;

      // always returns success
      lzo1x_1_compress(pData, uDataLength, 
                       chunk.data+4, &uDestLen, pLzoWrkmem);
      chunk.size = uDestLen + 4;
      delete [] pLzoWrkmem;

      if ( !bInitOkay || uDestLen > uDataLength )
      {
        LogWrn("<PNGImage::Write> LZO could not compress the chunk or initialization failed. Storing w/out compression.", "PNGImage", "SetChunk");
        delete [] chunk.data;
        chunk.data = pData;
        chunk.size = uDataLength;
        vName2[3] = 'n';
      }
      break;
    }
  case hdr::ZLIB:
    {
      vName2[3] = 'z';
      // reference doc. for this 
      uint4 uSpace2Compress = ((uDataLength * 1.01) + 12) + 4;
      chunk.data = new uint1[uSpace2Compress];
      memcpy(chunk.data, &uDataLength, 4);
      uint4 uDestLen = uSpace2Compress - 4; // zlib supports unsigned long
      int4 iCompResult =
        compress(chunk.data+4, reinterpret_cast<unsigned long*>(&uDestLen), (Bytef *) pData, uDataLength);
      chunk.size = uDestLen + 4;

      if ( iCompResult != Z_OK )
      {
        LogDbg("<PNGImage::> ZLIB compression failed, storing w/o compression", "PNGImage", "SetChunk");
        delete [] chunk.data;
        chunk.data = pData;
        chunk.size = uDataLength;
        vName2[3] = 'n';
      }
      break;
    } // case ZLIB
  } // switch

  // copy completed name to chunk.name
  memcpy(chunk.name, vName2, 5);

  m_chunks.push_back(chunk);
} // SetChunk


/**
 * Attempt to retrieve information about a PNG chunk.
 * @param sNane 
 * @param hData
 * @uDataLength
 * @return true if the chunk is found.
 */
bool PNGImage::GetChunk(std::string sName, uint1 ** hData, uint4 & uDataLength)
{
  list<png_unknown_chunk>::iterator chunk = m_chunks.begin();
  bool bResult = false;
  for ( ; chunk != m_chunks.end(); ++chunk )
  {
    chunk->name[3] = '\0'; // remove compression type from name
    if ( sName == string((char *) chunk->name) )
    {
      *hData      = chunk->data;
      uDataLength = static_cast< uint4 >( chunk->size );
      bResult = true;
      break;
    } // if match
  } // for 

  return (bResult);
} // GetChunk


/**
 *
 */
void PNGImage::DeleteChunks()
{
  list<png_unknown_chunk>::iterator chunk = m_chunks.begin();
  for ( ; chunk != m_chunks.end(); ++chunk )
  {
    png_free(m_pngPtr, chunk->data);
  }
  m_chunks.clear();
} // DeleteChunks


/**
 * @returns the pair<major,minor> version number of the most current version.
 */
const pair<int4,int4> & PNGImage::GetLatestVersion() const
{
  return(m_fv);
} // GetLatestVersion()


/**
 * Overrides the default version number when writing a file.
 * @param fileVersion
 */
void PNGImage::SetLatestVersion(const pair<int4,int4> & fileVersion)
{
  m_fv = fileVersion;
} // SetLatestVersion


/**
 * @return the version for most recent file read from disk, zero,zero if no files read.
 */
const pair<int4,int4> & PNGImage::GetFileVersion() const
{
  return(m_fvReadIn);
} // GetFileVersion


/**
 * Replaces fwrite() in libpng writing
 * function signature determined by libpng, unsigned int length type needed due to libpng error ??
 * @param png_ptr
 * @param data
 * @param length
 */
void PNGImage::WriteData(png_structp png_ptr, png_bytep data, png_size_t length)
{
  voidp pWrite = png_get_io_ptr(png_ptr);
  FilePtr * pFile = static_cast<FilePtr *> (pWrite);

  pFile->Write(data, length);
} // WriteData


/**
 * Flushes the stream in libpng writing
 * @param png_ptr
 */
void PNGImage::WriteFlushData(png_structp png_ptr)
{
  voidp pWrite = png_get_io_ptr(png_ptr);
  FilePtr * pFile = static_cast<FilePtr *> (pWrite);

  pFile->Flush();
} // WriteFlushData


/**
 * Replaces fread() in libpng so the poor Windows network can work
 * @param png_ptr
 * @param data
 * @param length
 */
void PNGImage::ReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
  voidp pRead = png_get_io_ptr(png_ptr);
  FilePtr * file = static_cast<FilePtr* > (pRead);

  file->Read(data, length);
} // ReadData


/**
 * Copies all image data.
 * @param rhs the right hand side.
 */
void PNGImage::Copy(PNGImage const & rhs)
{
  m_fv         = rhs.m_fv;
  m_fvReadIn   = rhs.m_fvReadIn;

  m_uWidth     = rhs.m_uWidth; 
  m_uHeight    = rhs.m_uHeight; 
  m_uRowBytes  = rhs.m_uRowBytes;
  m_iBitDepth  = rhs.m_iBitDepth; 
  m_iColorType = rhs.m_iColorType;
  m_text       = rhs.m_text;
  m_vData      = rhs.m_vData;
} // Copy


/**
 * Destroys all image data.
 */
void PNGImage::Destroy()
{
  m_vData = std::vector<uint1>();
  if ( m_pngPtr != NULL )
  { // see DeleteChunks(), info may already have been deleted
    DeleteChunks();
  }
} // Destroy


/**
 * @return pointers to the image array
 */
std::vector<uint1 *> PNGImage::GetRowPointers() const
{
  std::vector<uint1 *> rowPointers(m_uHeight);
  uint1 * pData = const_cast<uint1 *> (&*m_vData.begin());

  for ( uint4 u = 0;  u < m_uHeight; u++ )
  {
    rowPointers[u] = pData + u * m_uRowBytes;
  }

  return (rowPointers);
} // GetRowPointers
 

/**
 * function called by libpng for reading unknown chunks.
 * Not currently used.
 * @param pngPtr
 * @param chunk
 * @return
 */
int4 PNGImage::ReadChunkCallback(png_structp pngPtr, png_unknown_chunkp chunk)
{
  LogDbg("<PNGImage::ReadChunkCallback> Reading user Chunk", "PNGImage", "ReadChunkCallback");
  void * pPngImage = png_get_user_chunk_ptr(pngPtr);
  PNGImage * pImg = static_cast<PNGImage *> (pPngImage);
  pImg->m_chunks.push_back(*chunk);
  //uint1 * data = chunk->data;
  return (2);
} // ReadChunkCallback


/**
 * string has version "int int" ex. "1 2"
 * @param sText
 */
void PNGImage::StoreVersionNumber(const std::string & sText)
{
  int4 iSpace1 = static_cast< int4 >( sText.find(" ") );
  int4 iSpace2 = static_cast< int4 >( sText.substr(iSpace1+1, sText.size()).find(" ") + iSpace1 + 1 );
  m_fvReadIn.first  = atoi(sText.substr(0, iSpace1).c_str());
  m_fvReadIn.second = atoi(sText.substr(iSpace1+1, iSpace2).c_str());
} // StoreVersionNumber


/**
 * @param pngPtr
 * @param errorMsg
 */
void PNGImage::LibPNGError(png_structp pngPtr, png_const_charp errorMsg)
{
  // *** clean up memory
  throw ex::DataCorruptionException(LogRec(errorMsg,"PNGImager", "LibPNGError"));
} // LibPNGError


/**
 *
 */
void PNGImage::InitIcon()
{
  const int4 size = 25974;
  static const int4 vData[size] = { 255,255,255,206,206,206,214,214,214,197,197,206,206,206,206,214,214,214,199,199,197,206,214,214,206,206,206,206,206,206,206,206,206,206,206,206,
    206,206,206,206,206,206,206,206,206,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,206,206,206,206,
    206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,206,206,206,206,206,206,206,
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,
    214,206,214,214,206,214,206,206,206,207,214,205,207,214,205,207,214,205,206,206,206,206,206,206,206,206,214,206,206,214,214,206,214,214,206,214,
    206,206,214,206,206,214,197,206,206,197,206,206,206,214,214,206,214,214,219,224,225,206,206,206,206,206,206,206,206,206,199,199,197,214,214,214,
    214,214,214,255,255,255,206,206,214,25,25,33,25,25,25,25,25,25,25,25,33,28,33,37,25,25,25,25,25,25,25,25,25,25,25,25,
    25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,33,16,25,25,16,25,25,16,25,25,16,25,25,16,25,25,16,25,25,
    25,16,25,16,16,25,25,25,33,25,25,33,25,25,33,25,25,33,25,25,33,25,25,33,25,25,33,25,25,33,25,25,33,25,25,25,
    25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
    25,25,25,25,25,25,25,16,25,25,16,25,25,25,25,16,25,16,16,25,16,16,25,16,16,25,25,16,25,25,25,25,33,25,25,33,
    25,25,33,25,25,33,25,25,33,25,25,33,16,25,25,16,25,25,16,16,16,8,16,16,16,16,16,8,8,16,25,25,33,25,25,33,
    25,25,25,25,25,25,58,58,58,214,214,214,214,214,214,28,33,37,156,169,182,214,214,222,206,206,206,199,199,197,214,214,214,214,214,214,
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,222,214,214,222,214,214,214,214,214,214,
    214,214,214,206,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,
    214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,214,206,214,214,214,214,207,214,205,207,214,205,207,214,205,207,214,205,206,214,214,
    214,214,214,214,214,222,214,214,222,214,214,222,214,214,222,214,214,214,206,214,214,206,214,214,219,224,225,214,214,222,214,214,214,206,206,206,
    219,224,225,206,206,214,206,206,206,181,181,181,8,8,8,199,199,197,199,199,197,16,16,16,197,206,206,255,255,255,255,255,255,255,255,255,
    255,255,255,247,247,247,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,
    247,255,255,247,255,255,247,255,255,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,247,255,255,247,255,255,247,255,255,255,255,
    255,247,255,255,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,
    247,255,255,247,255,255,247,255,255,247,255,255,247,255,255,255,255,255,255,247,255,255,247,255,247,247,247,243,255,247,243,255,247,243,255,247,
    243,255,247,247,255,255,255,255,255,255,247,255,255,247,255,255,247,255,255,247,255,247,255,255,247,255,255,247,255,255,235,247,247,247,255,255,
    255,255,255,247,247,255,255,255,255,247,247,247,247,247,247,206,206,206,28,33,37,214,214,214,214,214,222,28,33,37,214,214,222,247,255,255,
    255,255,255,255,255,255,255,255,255,247,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    247,255,255,247,255,255,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,247,255,255,247,255,255,
    255,255,255,255,255,255,255,255,255,235,237,236,255,255,255,255,255,255,255,255,255,206,206,206,25,25,25,206,206,206,199,199,197,25,25,25,
    219,224,225,247,247,247,156,169,182,66,66,74,58,58,58,76,85,92,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,
    66,66,74,66,66,74,66,66,66,63,74,66,63,74,66,63,74,66,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,
    66,66,74,66,66,74,74,66,66,74,66,66,74,66,66,74,66,66,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,
    66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,66,66,66,66,
    66,66,66,63,74,66,63,74,66,63,74,66,63,74,66,63,74,66,74,66,66,74,66,66,74,66,66,74,66,66,74,66,66,63,74,66,
    63,74,66,63,74,66,66,66,74,66,66,66,66,66,74,66,66,74,140,140,132,247,247,247,255,255,255,199,199,197,16,16,16,214,214,214,
    214,214,222,25,25,25,206,206,214,247,255,255,140,140,140,8,8,8,0,0,0,0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,
    0,0,8,0,0,8,0,0,8,0,0,8,0,8,8,0,8,0,0,8,0,0,8,8,0,8,8,0,8,8,0,8,8,0,8,8,
    0,8,8,0,0,8,0,0,8,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,8,0,0,8,
    0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,0,0,8,0,0,0,
    0,0,0,0,0,0,0,8,0,0,8,8,0,8,8,0,8,8,0,8,0,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,
    0,0,0,0,0,0,0,8,0,0,8,8,0,8,8,0,0,8,0,0,0,0,0,0,66,66,74,247,255,255,255,255,255,206,206,206,
    28,33,37,214,214,214,206,214,214,16,16,16,214,214,214,255,255,255,118,126,130,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,8,0,8,8,0,8,8,0,8,8,0,8,8,
    0,8,8,0,8,8,0,8,8,0,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,8,16,16,8,16,25,0,16,16,0,16,16,0,8,8,8,8,8,8,8,0,
    8,8,0,8,8,0,8,8,0,8,8,8,0,8,8,0,8,8,8,16,16,0,0,8,16,16,16,0,0,8,58,58,58,255,255,255,
    255,255,255,206,206,206,25,25,25,199,199,197,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,7,16,7,7,16,7,0,8,8,
    0,8,8,0,8,16,16,25,25,55,64,66,8,16,16,0,8,16,8,8,16,8,8,16,8,8,8,8,8,0,8,8,0,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,0,8,8,0,0,8,80,99,118,27,37,49,0,0,8,8,16,16,
    8,8,8,8,8,0,8,8,0,8,8,0,8,8,0,8,8,8,8,8,16,0,8,16,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,16,7,8,8,8,
    0,8,8,0,8,8,0,8,8,0,8,8,55,64,66,76,85,92,8,16,16,0,8,16,0,8,16,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,0,7,16,7,8,16,16,8,16,25,133,140,149,41,59,72,
    0,8,16,0,8,16,8,8,8,8,8,0,8,8,0,8,8,0,8,8,0,8,8,8,8,8,16,0,8,16,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    7,16,7,0,8,0,0,8,0,0,8,8,0,16,16,0,0,8,118,126,130,76,85,92,8,16,25,0,8,16,0,8,16,8,8,16,
    8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,0,0,8,8,0,8,16,27,37,49,
    165,189,197,49,65,90,0,8,25,0,8,16,8,8,8,8,8,0,8,8,0,8,8,0,8,8,0,8,8,8,8,8,16,0,8,16,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,0,8,8,8,16,16,0,8,16,8,16,16,8,25,25,165,189,197,60,76,89,0,16,25,0,8,16,
    0,8,16,8,8,16,8,8,16,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,16,7,0,8,8,
    0,0,8,33,52,70,181,197,222,49,65,90,8,16,33,0,8,16,8,8,8,8,8,0,8,8,0,8,8,0,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,8,16,16,16,16,16,0,8,8,0,8,16,49,66,74,198,214,230,49,66,74,
    8,16,33,0,8,25,0,8,16,0,8,16,8,8,16,8,8,16,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    7,16,7,0,8,8,0,0,16,60,76,89,197,206,230,49,65,90,8,23,41,0,8,25,8,8,16,8,8,0,8,8,0,8,8,0,
    8,8,8,8,8,16,8,8,16,8,8,25,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,0,16,16,16,8,8,16,0,0,8,0,8,16,118,126,130,
    216,231,239,49,66,74,8,23,41,0,16,25,0,8,16,0,8,16,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,0,8,8,0,8,8,0,8,25,92,116,137,201,213,240,49,65,90,16,33,58,0,8,25,8,8,16,8,8,8,
    8,8,0,8,8,0,8,8,8,8,8,16,8,8,25,8,8,25,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,0,0,8,0,8,16,
    16,25,33,156,169,182,199,209,222,49,66,74,8,23,41,0,16,33,0,8,16,0,8,16,8,8,16,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,8,16,16,8,23,41,132,152,171,189,206,239,49,65,90,33,41,66,0,8,25,
    8,8,16,8,8,8,8,8,0,8,8,0,8,8,8,8,8,16,8,8,25,8,8,25,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    0,0,8,16,25,33,33,41,66,184,206,222,165,191,206,41,59,72,8,23,41,0,16,33,0,8,16,0,8,16,8,8,16,8,8,16,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,8,16,25,8,23,41,148,173,197,173,189,222,41,58,92,
    28,41,74,8,16,33,0,8,16,8,8,8,8,8,0,8,8,0,8,8,8,8,8,16,8,8,25,8,8,25,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,0,0,0,0,8,16,0,8,33,79,101,132,173,206,241,137,173,206,25,50,71,25,41,66,0,16,25,8,16,16,16,16,16,
    8,8,0,8,8,8,16,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,16,41,59,72,148,173,197,
    173,196,230,16,41,74,41,58,92,8,23,41,0,8,16,0,8,8,7,16,7,16,8,8,8,8,8,8,8,16,8,8,25,0,8,25,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,7,16,7,0,8,8,0,8,25,16,33,58,86,121,156,181,205,242,135,164,206,16,33,70,25,50,71,7,25,33,
    7,16,7,7,16,7,8,8,0,8,8,0,16,8,16,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,16,
    49,65,90,132,163,197,173,196,230,16,41,74,24,46,84,16,41,58,0,8,16,0,8,8,7,16,7,8,8,8,8,8,8,8,8,16,
    8,8,25,8,8,25,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,16,25,49,65,90,108,130,171,181,205,242,123,154,197,0,23,58,
    25,50,71,16,25,33,7,16,7,8,8,8,8,8,0,8,8,0,8,8,16,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,
    0,16,16,0,16,25,80,99,118,132,158,189,181,205,242,24,46,84,24,46,84,33,52,70,0,16,25,0,8,8,8,8,0,8,8,0,
    8,8,0,8,8,8,8,8,25,8,8,25,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,8,0,16,25,92,116,137,115,149,180,189,214,253,
    99,123,169,0,23,58,25,50,71,15,33,39,8,16,16,0,8,0,8,8,0,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,16,0,8,16,11,33,49,92,116,137,132,158,189,178,214,250,31,58,94,16,33,70,49,65,90,8,16,33,0,8,8,
    0,8,0,8,8,0,8,8,0,8,8,8,8,8,25,8,8,16,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,16,11,33,49,132,158,189,
    123,151,184,187,222,255,86,119,148,5,33,66,25,41,66,15,33,39,8,16,16,0,8,0,8,8,0,16,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,16,0,8,16,33,52,70,112,140,165,132,158,189,189,214,253,39,74,111,5,33,66,41,58,92,
    18,25,41,0,16,16,0,8,0,8,8,0,8,8,0,8,8,8,8,8,16,8,8,16,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,8,8,0,8,16,
    49,65,90,158,180,210,115,149,180,187,222,255,82,107,142,16,41,74,16,41,58,27,37,49,8,16,16,0,0,0,8,8,8,16,8,8,
    8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,0,8,16,57,82,108,123,140,173,132,158,189,189,214,253,62,92,123,
    4,25,66,25,50,71,27,37,49,8,16,16,0,8,8,8,8,8,8,8,0,8,8,8,8,8,16,8,8,16,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,
    0,8,16,0,16,25,112,134,153,148,173,197,115,149,180,187,222,255,86,119,148,16,41,74,16,33,58,25,41,58,16,25,25,0,8,8,
    8,8,8,8,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,0,16,33,86,119,148,123,140,173,123,154,197,
    189,214,253,82,107,142,5,33,66,25,41,66,33,41,58,8,16,25,0,8,8,8,8,8,8,8,0,8,8,8,8,8,16,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,16,0,16,25,11,33,49,158,180,210,132,163,197,123,151,184,187,222,255,99,135,165,15,49,73,16,33,58,33,41,58,
    16,25,33,0,8,8,8,8,16,8,8,8,0,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,8,23,41,123,140,173,
    115,149,180,132,163,197,187,222,255,89,123,165,16,41,74,16,41,74,33,52,70,7,25,33,0,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,16,16,16,0,8,8,8,8,8,
    16,16,16,8,8,8,8,8,8,0,8,16,0,0,16,60,76,89,173,197,222,111,140,173,137,173,206,178,214,250,137,173,206,27,58,82,
    8,33,58,25,41,58,8,16,33,8,8,16,8,8,16,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,8,0,0,8,16,16,16,8,8,16,8,8,8,8,16,16,0,0,16,
    16,33,58,148,176,212,89,123,165,138,173,214,178,214,250,151,181,224,31,58,94,0,25,49,33,52,70,18,25,41,0,0,8,16,16,16,
    8,8,8,0,0,0,16,25,16,0,8,8,8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,16,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,0,16,33,92,116,137,165,197,222,99,135,165,148,176,212,189,230,255,
    154,189,222,38,69,99,16,33,58,25,41,58,18,25,41,8,8,16,8,8,16,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,
    8,8,16,0,0,16,41,58,92,148,176,212,89,123,165,151,181,224,187,222,255,165,197,230,58,90,112,8,33,58,33,52,70,27,37,49,
    0,8,16,8,16,16,8,8,8,0,0,0,16,16,16,0,8,8,8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,16,16,16,8,8,8,0,0,8,8,8,8,8,8,16,0,0,8,16,41,58,148,173,197,148,176,212,99,135,165,
    173,206,230,197,230,255,173,206,230,58,90,112,16,33,58,25,41,58,27,37,49,8,16,25,0,8,8,8,8,8,8,8,8,0,8,0,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,0,8,16,79,101,132,132,163,197,86,121,156,166,196,240,189,230,255,178,214,239,86,119,148,16,41,74,
    25,41,66,33,41,66,8,16,33,8,8,16,8,8,16,0,8,0,8,8,8,0,8,8,8,8,8,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,0,0,0,16,16,16,8,8,16,0,0,8,8,16,16,8,8,16,0,8,16,57,82,108,173,206,230,
    115,149,180,99,135,165,187,222,255,197,230,255,173,206,230,82,107,142,16,33,58,25,33,58,33,41,58,8,16,33,0,0,8,8,8,8,
    0,8,8,0,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,0,0,8,8,8,8,8,16,16,8,8,8,8,16,25,123,140,173,115,149,180,86,121,156,178,214,250,189,230,255,184,223,244,
    112,140,165,27,58,82,16,33,58,33,52,70,18,25,41,0,0,16,8,8,16,0,8,8,0,8,0,8,8,8,8,8,8,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,16,0,0,0,8,16,16,8,8,8,8,8,8,8,16,16,0,0,8,0,16,25,
    99,126,156,173,206,241,86,121,156,123,165,197,197,239,255,184,223,244,173,206,230,112,140,165,25,41,66,10,24,53,33,41,58,8,16,33,
    0,0,8,8,16,16,0,8,8,0,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,0,8,8,8,16,16,0,8,8,8,8,8,8,8,16,0,0,0,18,25,41,148,173,197,99,134,173,99,134,173,189,230,255,
    187,222,255,184,223,244,132,163,197,38,69,99,8,33,58,33,41,66,25,33,58,0,0,16,8,8,16,8,8,16,0,8,0,8,8,8,
    8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,16,16,16,0,0,0,8,8,16,0,0,8,8,8,8,8,8,8,
    0,0,0,27,37,49,148,173,197,151,181,224,86,121,156,163,206,239,197,239,255,178,214,239,165,197,230,148,173,197,27,58,82,10,24,53,
    33,41,58,18,25,41,0,0,8,8,16,16,0,8,0,7,16,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,0,8,8,16,16,16,8,8,8,8,8,16,8,8,8,0,0,0,41,49,58,148,176,212,99,123,169,
    115,152,189,197,239,255,178,214,250,178,214,239,154,189,214,79,101,132,16,33,58,25,41,66,25,33,58,8,8,25,0,0,16,8,8,16,
    0,8,8,8,8,8,8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,16,16,8,8,8,16,16,16,0,0,0,
    8,8,16,8,8,8,0,0,8,76,85,92,173,197,222,115,152,189,99,134,173,197,239,255,189,230,255,178,214,239,173,206,230,165,197,222,
    52,71,110,10,24,53,33,41,66,18,25,41,0,8,16,8,16,16,0,8,0,7,16,7,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,16,16,16,0,0,0,16,16,16,8,8,8,8,8,16,0,8,8,0,0,0,76,85,92,
    135,164,206,89,123,165,137,173,206,214,247,255,178,214,239,178,214,239,173,206,230,99,126,156,25,50,71,25,33,58,33,41,66,18,25,41,
    0,0,16,8,16,16,0,8,8,8,8,8,8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    16,16,16,0,0,0,16,16,16,8,16,16,8,16,25,112,134,153,173,196,230,99,134,173,115,152,189,220,255,255,178,214,239,173,214,230,
    173,206,230,187,215,230,58,90,112,10,24,53,25,41,66,25,33,58,0,8,16,8,16,16,0,0,0,7,16,7,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,16,0,0,0,8,8,8,8,8,8,8,16,16,0,8,8,
    0,16,16,80,99,118,132,158,189,89,123,165,151,181,224,220,255,255,173,206,230,173,214,230,178,214,239,115,149,180,27,58,82,16,33,58,
    28,41,74,18,25,41,0,8,25,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,214,206,206,25,25,25,206,206,214,255,255,255,133,140,149,0,8,8,0,8,16,8,8,8,
    0,0,0,16,8,8,8,8,0,8,8,8,0,8,16,0,8,25,16,41,58,165,197,222,137,173,206,86,121,156,173,206,230,220,255,255,
    158,180,210,203,230,247,184,206,222,187,215,230,92,116,137,8,33,58,8,33,58,16,41,58,0,16,25,8,8,16,8,8,8,8,8,0,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,8,8,8,16,8,8,8,0,0,16,8,8,8,8,8,
    0,8,8,7,25,33,0,8,25,123,140,173,111,140,173,86,121,156,187,222,255,214,247,255,165,192,214,203,230,247,173,197,222,154,189,214,
    57,82,108,5,33,66,16,41,74,16,41,66,0,8,25,8,8,16,8,8,0,16,8,8,8,8,8,8,8,16,8,8,8,0,0,0,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,214,206,206,25,25,25,206,206,214,247,255,255,133,140,149,0,8,8,
    0,8,16,8,8,8,8,8,0,8,8,0,16,8,0,8,8,8,8,16,25,0,8,25,57,82,108,165,197,230,107,149,189,99,135,165,
    184,223,244,203,230,247,173,193,204,206,222,239,199,209,222,189,206,230,112,140,165,16,41,66,8,33,58,16,41,66,0,16,33,8,8,16,
    16,8,8,16,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,8,16,16,16,16,8,8,
    16,8,8,0,0,8,0,8,16,0,16,25,8,41,58,132,163,197,99,135,165,99,134,173,197,230,255,206,230,255,173,189,214,206,230,255,
    181,206,230,165,189,222,64,101,139,7,41,70,16,41,74,15,49,73,0,16,33,8,8,16,16,0,0,16,8,0,8,8,8,8,8,8,
    8,8,8,0,0,0,66,66,66,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,214,206,206,25,25,25,206,206,214,247,255,255,
    133,140,149,0,8,8,0,8,16,8,8,8,16,8,8,0,0,0,16,8,8,8,8,8,8,16,25,0,8,25,115,149,180,154,189,222,
    89,132,168,123,165,197,214,247,255,173,206,222,184,206,222,206,222,239,198,214,230,184,206,222,148,173,197,25,50,71,0,25,49,25,50,71,
    8,23,41,0,8,16,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,8,
    16,8,8,16,8,8,16,8,8,0,8,8,0,16,25,0,8,16,49,65,90,148,173,197,86,121,156,123,165,197,206,239,255,189,214,239,
    181,197,222,217,229,255,189,214,239,165,197,222,111,140,173,24,46,84,7,41,70,25,50,71,8,23,41,0,8,16,16,8,0,16,8,0,
    8,8,8,8,8,8,8,8,8,0,0,0,66,66,66,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,214,206,206,25,25,25,
    206,206,214,255,255,255,133,140,149,0,8,8,8,8,16,8,8,8,7,16,7,0,0,0,16,8,8,8,8,16,0,8,16,16,41,58,
    165,197,222,135,164,206,89,123,165,173,206,230,230,255,255,165,191,206,198,214,230,222,239,247,206,222,239,173,197,214,165,192,214,37,67,87,
    0,25,49,25,50,71,11,33,49,0,8,16,16,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,16,8,8,8,8,0,0,16,8,8,16,16,16,8,8,16,7,25,33,0,8,16,79,101,132,132,163,197,82,107,142,154,189,222,
    214,247,255,173,197,222,198,214,230,214,239,255,197,222,239,165,192,214,137,173,206,38,69,99,5,33,66,25,50,71,11,33,49,0,8,16,
    8,8,8,16,8,0,8,8,8,8,8,8,8,8,8,0,0,0,66,66,66,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,
    206,206,206,25,25,25,206,206,214,255,255,255,133,140,149,0,8,8,8,8,8,8,8,8,8,8,0,8,8,8,8,8,8,8,8,16,
    0,0,16,58,90,112,181,205,242,111,139,181,106,148,180,197,230,255,214,239,255,165,191,206,206,222,239,239,255,255,206,222,239,173,193,204,
    173,197,222,57,82,108,0,25,49,16,41,66,27,37,49,0,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,16,8,8,8,8,0,0,8,8,8,8,8,16,0,8,16,8,16,33,0,8,25,112,134,153,123,151,184,
    82,107,142,173,206,241,214,239,255,165,192,214,206,222,239,222,239,255,202,222,247,165,192,214,154,189,214,62,92,123,5,33,66,25,50,71,
    25,41,58,8,8,25,8,8,8,16,8,0,8,8,8,8,8,8,8,8,8,0,0,0,66,66,66,255,255,255,255,255,255,206,206,214,
    25,25,25,214,214,214,206,206,206,25,25,25,206,206,206,255,255,255,140,140,140,0,8,8,8,8,8,8,8,8,0,0,0,8,16,16,
    0,8,8,0,8,25,0,8,33,123,140,173,165,189,222,99,135,165,132,163,197,197,239,255,187,215,230,173,197,214,216,231,239,247,255,255,
    206,222,239,173,193,204,173,197,222,79,101,132,0,33,58,16,41,66,25,41,58,0,8,25,8,0,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,0,0,8,0,8,25,0,25,49,
    132,158,189,111,140,173,86,121,156,189,230,255,206,230,255,165,191,206,216,230,247,230,247,255,216,230,247,173,197,222,165,197,222,86,121,156,
    16,41,74,16,41,66,25,41,66,8,16,33,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,0,0,0,66,66,66,255,255,255,
    255,255,255,206,206,214,25,25,25,214,214,214,206,206,206,25,33,25,207,214,205,255,255,255,140,140,140,0,0,0,8,8,8,8,8,8,
    8,8,8,0,8,8,0,8,16,0,8,25,33,52,70,158,180,210,132,163,197,89,123,165,173,206,241,206,230,255,173,197,222,199,209,222,
    222,239,247,247,255,255,222,239,247,173,197,214,181,206,230,112,140,165,16,41,74,8,33,58,25,41,66,8,16,25,8,0,8,16,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,0,0,8,
    0,8,25,27,58,82,148,173,197,99,135,165,115,149,180,206,239,255,189,214,239,173,197,214,216,230,247,247,255,255,222,239,255,184,206,222,
    173,197,222,123,151,184,31,58,94,5,33,66,33,52,70,18,25,41,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,
    66,66,66,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,206,206,206,25,33,25,207,214,205,255,255,255,140,140,140,0,0,0,
    8,8,8,8,8,8,8,16,16,0,0,8,8,16,25,0,8,25,57,82,108,181,206,230,111,139,181,99,135,165,206,239,255,206,239,255,
    173,197,214,198,214,230,239,247,255,239,247,255,230,239,255,181,197,214,181,206,230,132,158,189,27,58,82,0,25,49,25,41,66,8,16,33,
    0,0,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,
    8,16,16,0,16,25,0,16,33,57,82,108,148,176,212,86,119,148,132,163,197,222,247,255,181,206,230,181,197,214,216,230,247,247,255,255,
    230,239,255,187,215,230,173,197,222,137,173,206,38,69,99,5,33,66,33,52,70,18,25,41,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,0,0,0,66,66,66,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,206,206,206,25,25,33,206,206,206,255,255,255,
    140,140,132,0,0,0,8,8,8,8,8,8,0,8,16,0,8,16,0,16,33,0,16,33,132,158,189,165,189,222,99,123,169,148,176,212,
    230,255,255,173,206,222,173,197,214,230,247,255,247,255,255,247,255,255,239,255,255,184,206,222,173,197,222,165,189,222,27,58,82,8,33,58,
    25,41,58,8,16,33,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,
    16,8,8,8,0,8,8,16,16,0,0,16,0,8,25,123,140,173,115,152,189,86,121,156,173,206,230,206,239,255,173,197,214,199,209,222,
    233,238,247,247,255,255,230,247,255,206,222,239,165,192,214,165,197,230,62,92,123,7,41,70,25,41,66,33,41,58,0,0,16,8,8,16,
    8,8,8,8,8,8,8,8,8,0,0,8,66,66,74,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,206,206,214,25,25,33,
    206,206,206,255,255,255,140,140,132,8,0,0,16,8,8,8,8,8,0,8,16,0,16,25,0,0,25,16,41,66,148,176,212,148,176,212,
    99,123,169,165,197,230,214,239,255,173,206,222,184,206,222,239,247,255,247,255,255,247,255,255,247,255,255,198,214,230,173,197,222,173,197,222,
    38,69,99,8,33,58,25,41,58,18,25,41,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,0,8,16,0,19,41,132,158,189,111,139,181,89,123,165,184,223,244,206,239,255,
    173,197,214,198,214,230,239,247,255,247,255,255,230,247,255,216,230,247,173,197,214,173,206,230,86,119,148,15,49,73,16,41,66,33,41,66,
    8,8,25,8,8,16,8,8,8,8,8,8,8,8,8,0,0,8,66,66,74,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,
    206,206,214,25,25,33,206,206,214,255,255,255,148,140,132,8,0,0,16,8,8,8,8,8,0,8,16,8,16,33,0,8,25,62,92,123,
    165,189,222,123,151,184,111,140,173,206,230,255,202,222,247,184,206,222,198,214,230,247,255,255,247,255,255,247,255,255,247,255,255,206,222,239,
    173,197,222,178,214,239,62,92,123,8,33,58,16,41,66,27,37,49,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,8,16,8,8,16,0,8,25,16,41,66,148,176,212,89,123,165,99,134,173,
    206,239,255,203,230,247,181,197,214,216,231,239,247,255,255,247,255,255,239,247,255,222,239,255,181,197,222,173,206,230,132,158,189,27,58,82,
    16,41,58,33,52,70,16,25,33,8,8,16,8,8,8,8,8,8,8,8,8,0,0,8,66,66,74,255,255,255,255,255,255,206,206,214,
    25,25,25,214,214,214,199,209,222,18,25,41,206,206,214,255,255,255,148,140,132,8,0,0,16,8,8,8,8,8,0,8,16,0,8,25,
    0,16,33,132,158,189,154,189,222,99,135,165,137,173,206,222,247,255,181,206,230,184,206,222,216,231,239,247,255,255,255,255,255,247,255,255,
    247,255,255,216,230,247,173,197,214,189,214,239,86,119,148,5,33,66,16,41,58,25,41,58,8,8,25,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,8,8,16,0,8,16,0,8,25,49,65,90,151,181,224,
    80,114,167,115,154,197,214,247,255,187,215,230,181,197,214,222,239,247,255,255,255,247,255,255,239,247,255,239,255,255,187,215,230,173,197,222,
    154,189,222,38,69,99,8,33,58,33,52,70,27,37,49,8,8,16,8,8,16,8,8,8,8,8,8,0,0,8,66,66,74,255,255,255,
    255,255,255,206,206,214,25,25,25,214,214,214,199,209,222,18,25,41,206,206,214,255,255,255,148,140,132,8,0,0,16,8,8,8,8,8,
    0,8,16,0,8,16,27,58,82,165,197,222,132,163,197,99,126,156,181,205,242,222,247,255,165,192,214,189,206,230,222,239,247,247,255,255,
    255,255,255,247,255,255,247,255,255,222,239,247,173,197,222,189,214,239,115,149,180,15,49,73,16,33,58,25,41,58,8,16,25,8,8,16,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,0,16,0,8,25,
    80,99,118,148,176,212,80,114,167,138,178,224,214,247,255,173,197,214,184,209,212,235,247,247,255,255,255,247,255,255,247,255,255,247,255,255,
    206,222,239,173,197,222,173,206,230,62,92,123,8,33,58,25,50,71,33,41,58,0,8,16,8,8,16,8,8,8,8,8,8,0,0,8,
    66,66,74,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,197,206,230,18,25,41,206,206,214,255,255,255,148,140,132,8,0,0,
    16,8,8,8,8,8,0,8,16,0,8,25,79,101,132,165,189,222,99,135,165,115,149,180,206,230,255,197,230,255,173,189,214,206,222,239,
    230,247,255,247,255,255,255,255,255,247,255,255,247,255,255,230,239,255,184,206,222,181,206,230,137,173,206,31,58,94,10,24,53,25,41,58,
    16,25,33,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,
    0,0,16,8,23,41,99,126,156,123,154,197,80,114,167,166,196,240,214,247,255,165,191,206,202,223,225,239,247,255,255,255,255,243,255,247,
    255,255,255,247,255,255,222,239,255,165,192,214,178,214,239,112,140,165,16,41,66,16,33,58,33,41,66,8,16,25,8,8,16,8,8,8,
    8,8,8,0,0,8,66,66,74,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,197,206,230,18,25,41,206,206,214,255,255,255,
    148,140,132,16,0,0,16,8,8,8,8,8,0,8,16,11,33,49,123,151,184,132,163,197,99,126,156,154,189,214,214,239,255,181,206,230,
    181,197,222,222,239,247,239,247,255,255,255,255,255,255,255,255,255,255,247,255,255,230,239,255,187,215,230,173,206,230,165,197,222,39,74,111,
    0,25,49,25,41,58,18,25,41,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,0,8,16,25,41,58,132,158,189,111,139,181,96,130,181,187,222,255,206,239,255,165,189,197,216,231,239,247,255,255,
    247,255,255,255,247,247,255,255,255,239,255,255,222,239,255,165,192,214,178,214,239,154,189,222,37,67,87,10,24,53,33,52,70,16,25,33,
    8,8,16,8,8,8,8,8,8,0,0,8,66,66,74,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,199,209,222,18,25,41,
    206,206,214,255,255,255,148,140,132,8,0,0,16,8,8,8,8,16,0,8,16,37,67,87,154,189,214,99,134,173,89,123,165,187,222,255,
    214,247,255,173,197,222,189,206,230,230,247,255,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,230,247,255,197,222,239,173,197,222,
    173,206,230,57,82,108,0,25,49,25,41,58,27,37,49,0,8,16,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,
    8,8,8,8,8,8,8,8,8,0,8,8,0,8,25,33,52,70,148,176,212,99,123,169,108,140,191,197,230,255,206,230,255,165,191,206,
    230,247,255,247,255,255,247,247,247,255,255,255,255,255,255,239,247,255,222,247,255,165,192,214,189,214,239,197,230,255,38,69,99,0,19,41,
    33,41,66,18,25,41,8,8,16,8,8,8,8,8,8,0,0,8,66,66,74,255,255,255,255,255,255,206,206,214,25,25,25,214,214,214,
    199,209,222,18,25,41,206,206,214,255,255,255,148,140,132,8,0,0,16,8,16,8,8,16,0,8,25,79,101,132,89,123,165,89,123,165,
    107,149,189,214,247,255,184,223,244,181,206,230,206,222,239,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,247,255,255,
    216,230,247,165,191,206,189,214,239,79,101,132,8,33,58,16,41,66,16,33,58,8,16,33,0,8,8,0,8,0,8,8,8,8,8,16,
    8,8,16,8,8,8,16,8,8,8,8,0,7,16,7,0,16,16,0,0,25,62,92,123,148,176,212,86,119,148,135,164,206,214,247,255,
    173,206,230,184,206,222,216,230,247,247,255,255,255,255,255,255,255,255,255,255,255,235,247,247,216,231,239,165,189,197,202,222,247,230,255,255,
    57,82,108,0,19,41,27,37,49,27,37,49,8,8,16,8,8,8,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,199,209,222,15,33,39,206,214,214,255,255,255,148,140,132,8,0,0,16,8,16,8,8,25,0,0,25,41,58,92,
    64,101,139,64,101,139,107,149,189,197,239,255,187,222,255,165,192,214,214,222,247,247,255,255,255,255,255,247,247,255,247,247,247,255,255,255,
    255,255,255,247,255,255,216,230,247,173,197,214,187,215,230,99,126,156,16,41,66,16,41,58,16,41,58,7,25,33,0,8,8,0,8,0,
    8,8,8,8,8,16,8,0,16,16,8,8,16,8,0,8,8,0,7,16,7,0,16,16,0,8,33,86,119,148,135,164,206,86,121,156,
    154,189,222,222,247,255,173,197,222,189,206,230,230,239,255,247,255,255,255,255,255,255,255,255,255,255,255,235,247,247,198,214,230,165,189,197,
    222,247,255,187,215,230,33,52,70,0,8,25,18,25,41,18,25,41,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,199,209,222,15,33,39,206,206,214,255,255,255,148,140,132,8,0,0,16,8,8,8,8,16,
    0,8,25,8,33,58,62,92,123,64,101,139,107,149,189,173,206,241,187,222,255,154,189,214,216,230,247,239,247,255,247,255,255,247,247,247,
    247,247,247,255,255,255,255,255,255,247,255,255,222,239,247,184,206,222,173,206,222,132,158,189,25,50,71,8,33,58,25,41,66,8,23,41,
    0,8,8,0,8,0,8,8,8,8,8,16,8,8,16,16,8,8,16,8,0,8,8,0,0,8,8,0,8,16,0,25,49,115,149,180,
    123,151,184,89,123,165,178,214,239,214,247,255,165,192,214,197,222,239,239,255,255,247,255,255,255,255,255,255,255,255,247,255,255,216,231,239,
    173,193,204,173,197,214,230,255,255,112,134,153,8,23,41,0,8,16,18,25,41,16,25,25,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,199,209,222,25,25,33,206,206,214,255,255,255,148,140,132,8,0,0,
    16,8,8,8,8,16,0,8,25,0,16,33,39,74,111,89,132,168,107,149,189,133,163,214,166,196,240,165,189,222,214,222,247,239,247,255,
    247,255,255,247,247,255,255,255,255,255,255,255,255,255,255,247,255,255,222,239,247,198,214,230,165,192,214,165,192,214,37,67,87,0,25,49,
    25,50,71,11,33,49,0,8,8,0,8,0,8,8,8,8,8,16,8,8,16,16,8,8,16,8,0,8,8,0,0,8,8,0,8,16,
    25,50,71,137,173,206,108,130,171,99,134,173,197,230,255,206,230,255,165,192,214,204,232,239,247,255,255,247,255,255,255,255,255,255,255,255,
    233,238,247,202,223,225,165,189,197,216,230,247,202,222,247,37,67,87,0,16,25,0,16,25,16,25,33,16,16,25,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,199,209,222,25,25,33,206,206,214,255,255,255,
    148,140,132,8,0,0,16,8,8,8,8,8,0,8,16,0,8,25,25,50,71,99,134,173,107,149,189,107,149,189,133,163,214,165,197,230,
    187,215,230,230,239,255,247,255,255,247,255,255,255,255,255,255,255,255,247,255,255,247,255,255,230,247,255,206,222,239,158,180,210,189,206,230,
    57,82,108,0,25,49,25,41,66,16,33,58,0,16,16,7,16,7,8,8,16,8,8,16,8,8,16,8,8,8,8,8,0,8,8,0,
    0,8,8,0,8,25,52,71,110,148,176,212,89,123,165,115,152,189,214,247,255,189,214,239,173,197,214,216,230,247,247,255,255,247,255,255,
    247,255,255,247,255,255,216,231,239,184,206,222,181,197,214,239,255,255,132,152,171,0,16,33,0,16,25,16,25,33,16,25,25,8,16,16,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,199,209,222,28,33,37,
    214,206,214,255,255,255,148,140,132,8,0,0,16,8,0,8,8,8,0,16,16,0,8,16,0,19,41,82,107,142,123,154,197,115,154,197,
    115,154,197,151,181,224,173,197,222,216,230,247,239,255,255,247,255,255,255,255,255,255,255,255,247,255,255,247,255,255,239,247,255,216,230,247,
    165,191,206,189,214,239,79,101,132,8,33,58,16,41,66,25,41,58,0,16,16,0,8,8,8,8,16,8,8,16,8,8,16,8,8,8,
    8,8,0,8,8,0,0,8,0,0,16,25,79,101,132,135,164,206,89,123,165,138,173,214,214,247,255,184,206,222,184,209,212,222,239,247,
    247,255,255,239,255,255,247,255,255,239,247,255,206,222,239,181,197,214,202,222,247,222,239,255,57,82,108,0,0,25,7,25,33,16,25,33,
    8,16,16,0,8,0,8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,214,25,25,33,214,206,214,255,255,255,148,140,132,8,0,0,8,8,0,8,8,0,0,16,16,0,16,25,0,8,25,27,58,82,
    123,154,197,115,154,197,115,154,197,133,163,214,165,189,222,197,222,239,222,239,255,239,255,255,255,255,255,255,255,255,247,255,255,247,255,255,
    239,255,255,222,239,255,173,189,214,189,214,239,99,126,156,16,41,74,16,33,70,25,41,58,0,16,16,0,8,8,8,8,16,8,8,16,
    8,8,8,8,8,8,8,8,0,0,8,0,0,8,8,8,23,41,108,130,171,123,154,197,99,123,169,165,197,230,214,247,255,173,193,204,
    202,223,225,235,247,247,247,255,255,239,255,255,239,247,255,222,239,255,197,206,230,181,197,222,222,239,255,148,173,197,16,33,58,0,8,25,
    7,25,33,16,25,25,7,16,7,0,8,0,8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,214,25,25,33,214,206,214,255,255,255,140,140,132,8,0,0,8,8,0,8,8,0,0,8,0,8,25,25,
    0,8,16,0,19,41,115,149,180,99,134,173,108,140,191,115,154,197,154,189,222,187,215,230,216,230,247,230,247,255,247,255,255,255,255,255,
    247,255,255,255,255,255,247,255,255,222,239,255,181,197,214,189,214,239,123,140,173,24,46,84,8,33,58,16,41,66,0,16,16,8,16,16,
    8,8,16,8,8,16,8,8,8,8,8,8,8,8,0,0,8,0,0,16,16,11,33,49,132,158,189,108,140,191,99,134,173,173,206,241,
    214,239,255,165,189,197,202,223,225,243,255,247,247,255,255,239,255,255,230,247,255,214,222,247,183,197,230,183,197,230,230,239,255,80,99,118,
    0,8,25,0,16,25,8,25,25,8,16,16,7,16,7,8,8,0,8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,214,206,214,25,25,33,214,206,214,255,255,255,140,140,132,0,8,0,8,8,0,8,8,0,
    0,8,0,8,16,16,0,8,16,0,8,25,38,69,99,135,164,206,108,140,191,96,140,181,138,173,214,173,206,230,197,222,239,230,247,255,
    247,255,255,255,255,255,247,247,247,255,255,255,247,255,255,230,239,255,189,206,230,181,206,230,148,173,197,24,46,84,8,33,58,25,41,66,
    16,25,33,0,8,8,0,0,8,8,8,16,8,8,8,8,8,8,7,16,7,0,8,0,0,8,16,33,52,70,151,181,224,111,139,181,
    111,139,181,214,247,255,187,215,230,173,193,204,219,224,225,247,255,255,247,255,255,239,255,255,230,247,255,189,206,230,181,197,222,230,239,255,
    158,180,210,33,41,66,0,8,25,16,25,33,16,25,25,7,16,7,0,8,0,16,16,16,8,8,8,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,214,206,214,25,25,33,214,206,214,255,255,255,140,140,140,0,8,0,
    8,8,0,8,8,0,0,8,0,0,8,8,0,8,8,0,8,16,16,41,66,108,130,171,135,164,206,96,140,181,123,154,197,154,189,222,
    189,214,239,216,230,247,247,255,255,255,255,255,247,255,255,255,255,255,247,255,255,230,247,255,198,214,230,181,197,222,158,180,210,41,58,92,
    8,33,58,16,41,66,15,33,39,0,8,16,0,0,16,8,8,16,8,8,8,8,8,8,7,16,7,0,8,0,0,16,16,57,82,108,
    138,173,214,99,123,169,123,154,197,214,247,255,185,214,222,173,193,204,235,237,236,243,255,247,247,255,255,247,255,255,206,222,239,189,206,230,
    183,197,230,230,239,255,92,116,137,8,23,41,0,8,25,25,25,33,16,25,25,7,16,7,0,0,0,7,16,7,8,8,8,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,214,206,214,25,25,33,206,206,214,255,255,255,
    140,140,140,0,8,0,8,8,8,8,8,0,7,16,7,0,0,0,0,8,16,0,0,16,0,8,33,58,90,112,135,164,206,96,140,181,
    96,140,181,137,173,206,173,196,230,197,222,239,239,247,255,247,255,255,255,255,255,255,255,255,247,255,255,239,247,255,206,222,239,173,197,214,
    181,197,222,57,82,108,5,33,66,16,41,58,27,37,49,8,8,16,0,8,16,8,8,16,8,8,8,8,8,0,8,8,0,0,8,0,
    0,16,25,99,126,156,135,164,206,80,114,167,151,181,224,214,247,255,173,206,214,184,209,212,243,255,247,243,255,247,247,255,255,230,255,255,
    184,206,222,173,197,214,206,230,255,189,206,230,33,41,66,0,0,16,8,16,25,25,25,33,16,16,16,8,8,8,0,8,0,8,8,8,
    8,8,8,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,214,206,206,25,25,25,
    206,206,214,255,255,255,133,140,149,0,8,8,8,8,8,8,8,8,8,8,8,0,0,0,8,16,16,0,8,16,0,8,25,16,41,66,
    111,140,173,107,149,189,96,140,181,120,166,211,154,189,222,189,214,239,222,239,247,247,255,255,255,255,255,247,255,255,247,255,255,239,255,255,
    216,230,247,165,191,206,189,214,239,82,107,142,16,41,66,8,33,58,27,37,49,8,16,25,0,8,16,8,8,16,8,8,8,0,8,0,
    0,8,0,0,8,0,7,25,33,132,158,189,115,152,189,80,114,167,178,214,250,214,239,255,173,206,214,184,209,212,243,255,247,255,255,255,
    235,247,247,206,222,239,184,206,222,165,192,214,230,255,255,99,126,156,0,16,33,0,8,25,16,25,33,25,25,25,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,33,25,206,214,214,247,255,255,133,140,149,0,8,8,0,8,16,8,8,8,8,0,0,8,8,8,8,8,8,8,16,16,
    8,16,25,0,8,25,79,101,132,123,165,197,97,148,189,111,153,207,151,181,224,181,206,230,206,222,239,239,247,255,255,255,255,247,255,255,
    255,255,255,247,255,255,222,239,255,165,191,206,197,222,239,112,140,165,15,49,73,8,33,58,33,41,58,16,16,25,8,8,16,8,8,16,
    8,8,8,0,8,0,0,8,0,0,8,0,11,33,49,148,176,212,96,130,181,96,130,181,206,230,255,197,230,255,173,206,214,202,223,225,
    247,255,255,255,255,255,233,238,247,184,206,222,173,197,222,206,239,255,189,214,239,33,52,70,0,16,25,16,25,33,25,25,25,16,16,16,
    8,0,0,8,8,8,16,16,16,0,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,33,25,207,214,205,247,255,255,133,140,149,0,8,16,0,8,16,8,8,16,8,0,0,16,8,8,
    8,0,0,8,8,8,8,16,25,0,0,16,27,58,82,132,163,197,103,150,197,103,150,197,133,163,214,165,197,230,198,214,230,233,238,247,
    255,255,255,255,255,255,255,255,255,247,255,255,230,247,255,173,197,214,187,215,230,148,173,197,27,58,82,0,25,49,33,41,58,21,16,33,
    8,8,16,8,8,16,8,8,8,0,8,0,0,8,0,0,8,8,49,66,74,148,173,197,88,123,173,111,153,207,214,239,255,184,223,244,
    173,193,204,216,231,239,255,255,255,247,247,247,222,239,247,181,197,214,165,192,214,230,255,255,112,134,153,8,23,41,0,8,16,25,25,33,
    16,16,16,16,8,8,8,8,8,8,8,8,8,16,16,0,0,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,33,25,207,214,205,247,255,255,133,140,149,0,8,16,0,8,25,8,8,16,
    8,8,8,16,8,8,8,0,0,8,8,8,8,8,16,0,8,16,11,33,49,86,121,156,111,153,207,94,138,193,115,154,197,154,189,222,
    189,206,230,216,231,239,247,255,255,255,255,255,255,255,255,247,255,255,239,255,255,184,206,222,184,206,222,165,192,214,37,67,87,11,33,49,
    33,41,66,21,16,33,8,8,25,8,8,16,0,8,0,0,8,0,7,16,7,7,16,7,80,99,118,132,163,197,88,123,173,138,178,224,
    214,247,255,173,206,230,173,206,214,235,247,247,255,255,255,235,237,236,206,222,239,181,197,214,197,222,239,203,230,247,37,67,87,0,16,33,
    0,16,16,25,25,25,8,8,8,16,8,8,16,8,8,8,8,8,8,16,16,0,0,16,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,33,25,207,214,205,247,255,255,133,140,149,0,8,16,
    0,8,16,8,8,16,16,8,16,8,0,0,16,8,8,16,16,16,0,0,8,0,16,16,0,8,16,38,69,99,120,166,211,94,138,193,
    103,150,197,151,181,224,173,196,230,206,222,239,235,247,247,247,255,255,255,255,255,247,255,255,239,255,255,198,214,230,181,197,222,181,206,230,
    38,69,99,8,33,58,33,41,66,16,25,33,8,8,16,8,8,16,8,8,0,0,8,0,7,16,7,0,16,16,112,134,153,123,154,197,
    96,130,181,166,196,240,214,247,255,173,197,222,185,214,222,239,255,255,247,255,255,233,238,247,184,206,222,173,193,204,239,255,255,112,140,165,
    11,33,49,0,16,16,16,25,25,25,33,25,8,8,0,8,8,0,16,8,8,0,0,0,8,8,16,8,8,16,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,33,25,206,214,214,255,255,255,
    133,140,149,0,0,8,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,8,16,0,16,25,
    99,135,165,107,149,189,108,140,191,133,163,214,151,181,224,189,214,253,206,222,239,239,255,255,247,255,255,247,255,255,247,255,255,201,213,240,
    183,197,230,189,214,253,62,92,123,8,41,58,16,41,66,33,41,66,0,16,16,7,16,7,0,0,0,8,8,8,8,16,25,16,33,58,
    135,164,206,96,130,181,88,123,173,187,222,255,206,239,255,165,191,206,216,231,239,247,255,255,230,247,255,206,222,239,165,189,197,198,214,230,
    230,255,255,41,59,72,7,25,33,0,8,16,16,25,25,16,16,16,8,8,8,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,0,8,16,58,90,112,115,152,189,108,140,191,103,150,197,138,178,224,173,206,241,197,222,239,235,247,247,243,255,247,247,255,255,
    255,255,255,214,222,247,183,197,230,189,214,253,86,119,148,15,49,73,16,41,66,33,41,66,8,16,16,8,8,0,8,8,0,8,8,8,
    0,8,25,41,58,92,133,163,214,96,130,181,96,140,181,189,230,255,203,230,247,165,189,197,222,239,247,247,255,255,230,247,255,189,206,230,
    173,197,214,222,239,255,165,189,197,27,37,49,0,8,16,16,16,25,16,16,25,16,16,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,0,8,16,11,33,49,111,140,173,115,154,197,96,130,181,133,163,214,151,191,230,187,215,230,222,239,247,
    235,247,247,255,255,255,255,255,255,230,239,255,183,197,230,181,205,242,115,149,180,27,58,82,8,33,58,33,41,66,16,25,33,0,8,0,
    8,8,0,8,8,16,0,0,25,62,92,123,133,163,214,88,123,173,123,165,197,204,247,255,187,215,230,165,189,197,239,247,255,239,247,255,
    216,230,247,173,197,214,197,222,239,222,239,255,76,85,92,8,16,25,0,8,16,28,33,37,8,16,16,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,0,8,16,80,99,118,135,164,206,94,138,193,111,153,207,138,173,214,
    173,206,230,204,232,239,235,237,236,255,255,255,255,255,255,239,247,255,189,206,230,181,205,242,140,181,211,31,58,94,8,33,58,25,41,66,
    28,33,37,0,0,0,7,16,7,8,8,16,0,0,25,99,126,156,123,154,197,80,114,167,138,178,224,214,247,255,173,206,222,173,197,214,
    239,247,255,230,239,255,189,206,230,173,197,214,222,239,255,173,193,204,15,33,39,0,8,16,8,16,25,28,33,37,8,8,16,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,8,16,25,41,66,123,154,197,111,153,207,
    94,138,193,120,166,211,154,189,222,197,222,239,235,237,236,255,255,255,255,247,255,255,247,255,201,213,240,181,206,230,163,206,239,39,74,111,
    5,33,66,25,41,66,27,37,49,0,8,0,7,16,7,0,8,8,0,8,33,123,140,173,111,139,181,80,114,167,163,206,239,214,247,255,
    165,192,214,185,214,222,239,247,255,214,222,247,173,193,204,198,214,230,222,239,255,80,99,118,0,16,25,0,16,25,16,25,33,16,25,25,
    0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,16,16,0,8,16,
    86,121,156,120,166,211,94,138,193,111,153,207,140,181,211,173,214,230,216,231,239,247,247,247,255,247,247,255,255,255,214,222,247,173,196,230,
    178,214,250,64,101,139,8,33,74,16,41,66,33,41,58,0,8,8,0,8,8,0,0,8,25,41,66,123,151,184,99,123,169,96,140,181,
    189,230,255,197,239,255,165,191,206,204,232,239,230,239,255,199,209,222,173,193,204,222,239,255,181,197,214,27,37,49,8,16,33,8,16,33,
    16,25,33,0,8,16,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    16,16,16,0,0,8,37,67,87,115,154,197,103,150,197,94,138,193,138,173,214,165,197,222,202,223,225,235,237,236,255,247,247,255,255,255,
    216,230,247,173,197,222,178,214,250,89,132,168,16,41,74,16,33,58,33,41,58,8,25,25,0,8,8,0,8,16,57,82,108,123,151,184,
    99,123,169,135,164,206,204,247,255,184,223,244,165,191,206,222,239,255,216,230,247,181,197,214,184,206,222,230,247,255,92,116,137,0,16,25,
    8,16,25,16,25,33,8,16,25,0,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,0,0,8,8,16,0,19,41,96,140,181,111,153,207,94,138,193,120,166,211,154,189,214,198,214,230,235,237,236,
    255,255,255,255,255,255,216,230,247,173,197,222,178,214,250,107,149,189,16,41,74,8,33,58,33,41,58,15,33,39,0,8,16,0,8,25,
    79,101,132,123,151,184,99,134,173,151,181,224,220,255,255,173,206,222,165,191,206,230,247,255,206,222,239,173,189,214,198,214,230,222,247,255,
    33,41,58,0,16,25,0,16,25,28,33,37,8,8,16,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,16,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,16,8,0,16,16,16,0,0,25,62,92,123,97,148,189,96,140,181,120,166,211,123,165,197,
    181,197,222,219,224,225,235,237,236,255,255,255,230,247,255,181,206,230,165,197,230,120,166,211,31,58,94,8,33,58,25,41,66,15,33,39,
    0,16,25,0,16,33,111,140,173,108,130,171,115,152,189,187,222,255,206,239,255,173,206,214,185,214,222,222,239,255,201,213,240,173,189,214,
    239,255,255,92,116,137,7,25,33,0,16,25,7,25,33,16,25,25,8,8,16,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,0,8,0,8,0,8,25,16,41,66,96,140,181,97,148,189,
    103,150,197,123,154,197,158,180,210,198,214,230,235,237,236,247,255,255,239,247,255,187,215,230,165,197,230,135,164,206,38,69,99,10,24,53,
    33,52,70,11,33,49,0,8,25,8,41,58,115,149,180,111,139,181,138,173,214,206,239,255,206,239,255,165,191,206,204,232,239,197,222,239,
    173,189,214,201,213,240,198,214,230,60,76,89,0,16,25,8,16,25,16,25,33,8,16,25,0,8,8,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,0,8,8,16,25,0,8,25,
    64,101,139,123,165,197,97,148,189,115,154,197,135,164,206,189,206,230,206,222,239,239,247,255,239,255,255,206,222,239,165,197,222,148,176,212,
    52,71,110,0,23,58,33,52,70,16,41,58,0,8,25,37,67,87,123,151,184,123,154,197,165,197,230,206,239,255,184,223,244,165,191,206,
    222,247,255,184,206,222,173,189,214,230,247,255,132,152,171,15,33,39,0,0,16,16,25,33,16,25,25,8,8,16,0,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,16,16,8,16,
    0,8,16,0,16,16,16,41,58,132,163,197,108,140,191,108,140,191,123,154,197,173,196,230,189,214,239,222,239,255,239,255,255,211,240,247,
    173,197,222,165,189,222,79,101,132,0,23,58,25,50,71,16,41,66,0,8,25,62,92,123,123,154,197,138,173,214,178,214,250,197,230,255,
    173,197,214,185,214,222,211,240,247,165,191,206,206,230,255,222,239,255,60,76,89,0,8,16,0,8,16,28,33,37,8,16,16,0,8,8,
    8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,
    8,8,16,8,8,16,0,0,8,0,16,16,0,8,16,92,116,137,123,154,197,96,130,181,115,154,197,151,181,224,178,214,239,203,230,247,
    230,255,255,222,247,255,184,206,222,189,206,230,99,126,156,16,33,70,16,41,66,15,49,73,0,25,49,86,121,156,137,173,206,151,191,230,
    187,222,255,173,206,241,165,191,206,204,232,239,185,214,222,165,191,206,239,255,255,148,173,197,18,25,41,0,0,8,16,25,33,28,33,37,
    0,8,8,8,8,16,16,16,16,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,16,8,8,16,0,0,8,8,8,8,0,8,8,0,8,8,41,59,72,123,154,197,108,140,191,103,150,197,133,163,214,
    165,197,230,184,223,244,220,255,255,230,247,255,198,214,230,201,213,240,123,151,184,24,46,84,0,23,58,15,49,73,27,58,82,115,149,180,
    138,178,224,151,191,230,173,206,241,165,189,222,165,191,206,211,240,247,173,193,204,187,215,230,222,247,255,80,99,118,0,8,25,0,8,16,
    27,37,49,16,25,25,0,0,8,8,16,16,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,16,8,8,25,0,0,16,7,16,7,0,8,0,7,16,7,7,25,33,86,119,148,123,154,197,
    103,150,197,111,153,207,138,178,224,163,206,239,203,230,247,230,255,255,216,231,239,217,229,255,173,189,222,52,71,110,0,23,58,27,58,82,
    64,101,139,137,173,206,138,178,224,138,178,224,148,176,212,165,189,222,185,214,222,202,223,225,173,193,204,222,247,255,158,180,210,28,41,74,
    0,8,25,7,25,33,28,33,37,0,8,16,0,8,8,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,8,25,8,16,25,0,8,0,0,8,0,0,8,0,0,8,16,
    41,58,92,133,163,214,94,138,193,94,138,193,138,178,224,163,206,239,184,223,244,230,255,255,233,238,247,233,238,247,202,218,255,79,101,132,
    16,41,66,38,69,99,99,135,165,140,181,211,151,181,224,137,173,206,137,173,206,181,206,230,203,230,247,185,214,222,184,209,212,239,255,255,
    79,101,132,0,8,33,8,16,33,16,25,33,16,25,25,0,0,8,8,8,16,16,16,16,0,0,0,16,16,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,8,16,0,8,8,7,16,7,0,0,0,
    16,16,16,8,8,16,8,23,41,111,140,173,120,166,211,94,138,193,120,166,211,151,191,230,178,214,239,204,232,239,247,255,255,255,255,255,
    230,247,255,132,158,189,57,82,108,62,92,123,123,151,184,165,197,222,165,197,230,165,197,222,165,197,222,214,247,255,203,230,247,181,197,214,
    222,239,247,183,194,204,33,41,58,0,8,25,25,25,33,16,16,25,8,8,16,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,16,0,8,0,
    8,8,8,8,0,0,16,8,16,8,8,16,0,0,25,62,92,123,123,154,197,97,148,189,103,150,197,138,178,224,173,206,241,197,222,239,
    239,255,255,247,255,255,230,247,255,181,206,230,132,158,189,123,151,184,148,173,197,165,192,214,173,206,230,178,214,239,197,222,239,206,239,255,
    197,222,239,181,197,214,239,255,255,118,126,130,25,25,33,8,8,16,25,25,33,16,16,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,
    8,16,16,0,0,8,8,8,8,8,0,0,16,8,8,8,8,8,0,0,16,25,41,66,99,134,173,115,154,197,96,140,181,120,166,211,
    165,197,230,187,215,230,222,239,247,247,255,255,230,247,255,230,247,255,217,229,255,189,214,239,173,206,222,181,206,230,202,222,247,206,230,255,
    214,247,255,197,222,239,181,197,214,206,222,239,216,231,239,27,37,49,8,8,25,8,16,25,25,25,25,16,16,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,16,8,16,16,0,0,8,8,8,8,16,8,8,16,8,0,16,8,16,0,0,16,0,16,33,62,92,123,120,166,211,
    96,140,181,107,149,189,140,181,211,181,206,230,216,231,239,247,255,255,239,255,255,239,255,255,239,255,255,222,247,255,214,239,255,214,239,255,
    222,247,255,222,247,255,214,239,255,187,215,230,173,197,214,230,247,255,132,152,171,0,8,25,0,0,16,25,25,33,16,16,25,16,16,16,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,0,0,8,16,8,16,16,8,8,8,0,0,16,8,8,8,8,16,0,8,25,
    25,50,71,115,152,189,107,149,189,107,149,189,123,165,197,165,197,230,197,222,239,230,247,255,247,255,255,239,255,255,230,247,255,239,255,255,
    239,255,255,239,255,255,230,255,255,230,255,255,197,222,239,181,197,214,197,222,239,216,230,247,49,66,74,0,8,16,8,16,25,25,25,33,
    8,16,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,16,8,16,16,8,8,8,0,0,8,8,0,
    8,8,8,0,8,16,0,19,41,86,119,148,123,165,197,106,148,180,107,149,189,154,189,222,181,205,242,216,230,247,247,255,255,239,255,255,
    239,255,255,247,255,255,247,255,255,239,255,255,230,247,255,222,247,255,184,206,222,181,197,214,230,247,255,132,152,171,7,25,33,0,8,16,
    15,33,39,8,16,25,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,8,16,8,8,
    16,8,0,8,8,0,8,8,0,0,8,8,0,8,16,37,67,87,115,149,180,106,148,180,106,148,180,137,173,206,165,197,230,189,214,239,
    230,247,255,247,255,255,247,255,255,247,255,255,247,255,255,239,255,255,222,239,247,206,222,239,181,197,214,198,214,230,216,231,239,60,76,89,
    0,16,25,0,16,25,15,33,39,8,16,25,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,25,
    8,0,8,8,8,0,16,8,8,8,8,0,8,8,0,0,8,8,0,8,16,0,25,49,112,140,165,106,148,180,96,140,181,123,154,197,
    151,181,224,181,205,242,216,231,239,247,247,247,247,255,255,239,247,255,247,255,255,247,255,255,230,247,255,199,209,222,173,193,204,222,239,247,
    173,193,204,7,25,33,0,16,25,7,25,33,7,25,33,8,16,25,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,
    8,8,16,8,8,16,8,8,8,8,8,8,16,8,0,8,8,0,8,8,0,0,8,8,0,16,16,0,16,25,38,69,99,132,158,189,
    115,152,189,88,123,173,138,178,224,166,196,240,206,222,239,233,238,247,247,255,255,247,255,255,239,247,255,222,239,247,216,231,239,165,189,197,
    198,214,230,222,239,247,76,85,92,0,16,25,0,0,16,15,33,39,16,25,33,0,8,8,8,8,16,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,16,8,8,16,8,8,16,8,8,8,8,8,8,8,8,0,8,8,0,8,8,0,8,8,8,0,8,8,0,16,25,
    11,33,49,115,149,180,115,149,180,96,140,181,120,166,211,151,191,230,198,214,230,235,237,236,247,255,255,247,255,255,247,255,255,222,239,247,
    202,223,225,173,193,204,216,231,239,165,189,197,27,37,49,0,16,25,0,16,25,16,25,33,8,16,25,0,8,8,8,8,16,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,8,8,8,8,8,8,0,8,8,0,8,8,0,8,8,8,
    0,8,8,0,8,16,0,8,25,79,101,132,123,154,197,108,140,191,111,153,207,138,173,214,184,206,222,216,231,239,239,255,255,247,255,255,
    239,255,255,216,231,239,181,197,214,199,209,222,230,239,255,80,99,118,0,0,16,8,16,25,28,33,37,8,16,25,0,8,8,0,8,16,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,16,16,16,0,0,8,0,16,33,16,41,66,123,151,184,107,149,189,103,150,197,115,154,197,165,192,214,198,214,230,
    230,247,255,239,255,255,230,247,255,199,209,222,173,193,204,216,230,247,199,209,222,27,37,49,0,0,16,16,25,33,28,33,37,0,8,16,
    0,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,8,0,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,16,16,16,0,0,8,8,23,41,0,0,25,86,119,148,123,154,197,96,140,181,106,148,180,
    154,189,214,173,206,222,214,239,255,230,255,255,214,239,255,181,197,214,181,197,222,239,255,255,112,134,153,0,8,25,8,16,25,16,25,33,
    16,25,25,0,8,8,8,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,8,
    0,8,8,0,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,0,0,16,8,16,33,0,0,25,27,58,82,123,165,197,
    96,140,181,107,149,189,137,173,206,154,189,214,203,230,247,230,255,255,197,222,239,173,197,222,202,222,247,206,222,239,33,41,66,0,0,16,
    28,33,37,16,16,25,8,8,16,8,8,8,16,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    0,8,8,0,8,8,0,8,8,0,8,8,8,8,16,8,8,16,8,8,16,16,8,16,8,0,8,16,16,25,8,8,25,0,8,33,
    0,25,49,89,123,165,107,149,189,107,149,189,115,152,189,148,176,212,178,214,239,214,239,255,173,206,230,189,206,239,222,239,255,112,134,153,
    0,8,33,0,8,25,28,33,37,16,16,25,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,0,8,8,0,8,8,0,8,8,8,8,8,8,8,16,8,8,16,8,8,16,16,8,16,8,0,8,16,16,25,
    0,8,16,0,8,25,0,8,33,39,74,111,123,165,197,96,140,181,106,148,180,140,181,211,173,206,241,197,230,255,165,189,222,189,214,239,
    222,239,255,41,59,72,0,8,25,16,25,33,25,25,33,8,16,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,
    16,16,16,8,8,8,0,0,16,8,16,33,0,0,25,8,33,58,99,126,156,115,152,189,106,148,180,106,148,180,173,206,241,187,222,255,
    165,192,214,214,239,255,156,169,182,0,16,25,0,8,16,16,25,25,16,25,25,0,8,8,0,8,0,0,8,8,8,16,16,0,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,16,8,8,
    16,8,8,16,8,8,16,8,8,16,8,16,0,8,16,0,8,25,0,8,25,0,19,41,58,90,112,132,158,189,107,149,189,107,149,189,
    151,191,230,189,230,255,173,206,230,203,230,247,80,99,118,0,8,16,8,16,16,16,25,25,16,25,16,0,8,8,0,8,0,0,8,8,
    0,16,16,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,16,16,16,8,8,16,0,8,16,0,16,33,0,8,16,16,41,58,123,140,173,
    107,149,189,96,140,181,135,164,206,178,214,250,197,230,255,165,189,197,0,16,33,0,8,16,16,25,25,16,25,16,7,16,7,8,8,8,
    0,8,8,0,8,8,0,8,8,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,16,8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,8,0,8,8,8,8,16,16,0,0,8,0,16,25,0,8,16,
    0,8,25,79,101,132,115,154,197,89,123,165,115,154,197,165,197,230,206,230,255,79,101,132,0,8,16,8,16,25,28,33,37,16,16,16,
    8,8,8,8,8,8,8,8,8,0,8,8,0,8,8,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,0,0,16,16,16,0,8,8,
    0,8,16,0,16,25,0,8,16,25,50,71,115,152,189,88,123,173,99,134,173,154,189,222,154,189,214,25,41,66,0,8,16,16,25,33,
    25,25,33,8,16,16,0,8,8,8,8,8,8,8,8,0,8,8,0,8,8,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,0,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,16,8,8,0,0,0,
    8,8,8,8,8,8,0,8,8,8,16,25,8,16,25,0,8,25,86,121,156,96,140,181,86,121,156,148,176,212,82,107,142,0,19,41,
    0,16,33,15,33,39,16,25,25,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,0,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,
    8,8,8,16,8,8,8,8,0,8,8,8,8,8,8,8,8,8,8,16,16,0,0,16,38,69,99,99,134,173,86,121,156,115,149,180,
    27,58,82,0,16,33,0,16,33,15,33,39,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,0,8,16,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,0,8,8,0,25,23,16,0,0,0,8,8,0,16,16,16,0,0,0,8,8,8,0,8,16,0,33,58,86,119,148,
    89,123,165,79,101,132,16,41,66,0,16,33,8,23,41,16,25,33,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,8,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,0,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,0,8,16,
    0,8,33,31,58,94,99,135,165,82,107,142,0,25,49,0,16,33,27,37,49,0,8,16,8,8,16,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,0,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,0,8,16,0,8,33,5,33,66,92,116,137,99,126,156,0,16,33,15,33,39,16,25,33,0,8,16,8,8,16,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,
    140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,16,0,16,33,0,8,33,57,82,108,112,134,153,0,16,25,27,37,49,0,8,16,0,8,16,
    8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,
    206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,16,25,0,0,16,33,52,70,118,126,130,27,37,49,28,33,37,
    0,0,8,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,
    206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,0,8,16,0,8,25,8,16,33,76,85,92,
    55,64,66,8,16,16,0,0,8,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,
    25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,8,8,8,16,8,8,8,8,8,8,8,8,16,0,0,8,16,16,25,
    0,8,8,49,49,49,66,66,74,0,8,0,0,8,8,8,16,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,66,66,66,255,255,255,
    255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,
    8,8,8,8,8,16,0,8,8,16,25,25,28,33,37,0,0,0,7,16,7,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,16,8,8,16,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,8,0,0,8,
    66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,206,206,206,25,25,33,206,206,214,255,255,255,140,140,140,0,0,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,
    8,8,16,8,8,16,8,8,16,0,0,0,8,8,8,16,16,16,8,8,8,8,8,8,8,16,16,0,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,16,8,8,16,8,8,16,8,8,16,8,8,16,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,0,0,8,66,66,66,255,255,255,255,255,255,206,206,206,25,25,25,214,214,214,214,214,214,16,16,16,219,224,225,247,247,255,
    219,224,225,181,181,181,199,199,197,181,181,181,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,
    181,181,181,181,181,181,181,181,181,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,
    181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,
    181,181,181,181,181,181,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,
    181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,189,181,181,181,
    181,181,181,181,181,181,181,181,181,181,181,181,206,206,206,255,255,255,247,247,247,214,214,214,16,16,16,206,206,206,206,206,214,16,16,25,
    214,214,214,255,255,255,255,255,255,247,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,247,247,247,255,255,255,255,255,255,247,247,247,206,206,206,28,33,37,214,214,214,
    197,206,206,28,33,37,156,169,182,199,199,197,214,214,214,214,214,214,197,197,206,214,214,214,206,206,206,206,206,206,206,206,206,206,206,206,
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,
    206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,214,206,206,206,206,206,206,206,206,206,206,206,206,
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,214,206,206,214,206,206,214,206,206,214,
    206,206,214,206,206,214,206,206,214,206,206,214,199,199,197,199,199,197,206,206,206,206,206,206,206,206,206,214,214,214,206,206,206,181,181,181,
    16,16,16,199,199,197,235,237,236,66,66,66,49,58,58,49,49,49,58,58,58,66,66,74,58,58,58,49,49,49,58,58,58,58,58,58,
    58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
    58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
    58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
    58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,
    58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,58,66,66,66,58,58,58,58,58,58,
    66,66,66,58,58,58,76,85,92,214,214,214,247,247,247,235,237,236,247,247,247,255,255,255,235,237,236,219,224,225,247,247,247,247,247,247,
    247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,
    247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,
    247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,
    247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,
    233,238,247,233,238,247,233,238,247,233,238,247,233,238,247,233,238,247,233,238,247,233,238,247,255,255,255,247,247,247,247,247,247,247,247,247,
    235,237,236,235,237,236,235,237,236,247,247,247,235,237,236,255,255,255};

  for ( int4 i=0; i < size; ++i )
  {
    m_vData[i] = vData[i];
  }
} // InitIcon

#undef FILE_REVISION

// $Log: PNGImage.C,v $
// Revision 1.2  2006/02/02 20:02:38  geconomos
// added considerations for 64-bit builds
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.9  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.8  2003/01/22 21:20:07  ingmar
// added missing include
//
// Revision 3.7  2001/01/06 13:06:24  jmeade
// Removed reference return in GetPixel(), which returns local memory.
//
// Revision 3.6  2001/01/02 14:01:42  ingmar
// removed explicit throw on function interface
// added missing includes
//
// Revision 3.5  2002/12/23 22:50:38  ingmar
// removed redundant use of Triple copy constructor
//
// Revision 3.4  2002/09/30 20:46:57  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.3  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.2  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:44   ingmar
// Initial revision.
// Revision 1.5  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:05:00  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:08  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.46  2001/08/31 18:53:39  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.45  2001/08/13 20:05:28  tushar
// removed errors due to the new logger
//
// Revision 1.44  2001/08/07 17:02:45  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.43  2001/07/25 00:29:10  tushar
// fixed some compiler errors (logger)
//
// Revision 1.42  2001/07/24 23:56:26  tushar
// fixed some compiler errors (logger)
//
// Revision 1.41  2001/07/16 23:31:19  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.40  2001/07/13 19:52:04  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.39  2001/06/23 19:52:44  dave
// moved no C++ streams, now uses FilePtr class
//
// Revision 1.38  2001/06/23 14:28:57  frank
// Added back the fopen() in read.  Its presence mysteriously makes other code not crash.
//
// Revision 1.37  2001/06/04 14:50:47  dave
// removed extra fopen() in Read
//
// Revision 1.36  2001/05/30 22:42:49  dave
// C++ streams for I/O
//
// Revision 1.35  2001/05/29 04:12:04  dave
// corrected a bug if Soeren didn't already
//
// Revision 1.34  2001/05/28 23:01:16  soeren
// changed order of closing the file in png::write
//
// Revision 1.33  2001/05/28 22:57:56  soeren
// changed order of closing the file in png::write
//
// Revision 1.32  2001/04/25 19:28:13  dave
// closer standards compliance, more comments at George's request
//
// Revision 1.31  2001/04/19 20:59:40  dave
// updated variable names
//
// Revision 1.30  2001/04/12 21:30:03  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.29  2001/04/12 18:52:27  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.28  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.27  2001/04/10 16:13:49  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.26  2001/04/09 22:25:43  dave
// changing from Abort to use exceptions
//
// Revision 1.25  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.24  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.23  2001/04/05 18:00:14  dmitry
// updated datatypes
//
// Revision 1.22  2001/04/01 20:52:30  ingmar
// code beautify
// dbg info specific to read header vs. read volume
//
// Revision 1.21  2001/03/28 20:06:54  dmitry
// Update typedef, implemented proper casting
//
// Revision 1.20  2001/03/25 18:36:51  dave
// minor debug stmts added
//
// Revision 1.19  2001/03/12 12:58:09  dave
// Changed m_rowBytes for 1 bit images
//
// Revision 1.18  2001/03/05 15:28:43  dave
// overloaded constructor for image buffers
//
// Revision 1.17  2001/02/12 17:13:10  jeff
// throw viatronix exceptions for V1K errors instead of aborting
//
// Revision 1.16  2001/02/06 02:24:42  dave
// added hack for reading LinearVolume
//
// Revision 1.15  2001/02/01 20:11:48  dave
// added extra check of m_keepUnknown
//
// Revision 1.14  2001/01/25 18:28:36  dave
// PNGIcon for default is now correct
//
// Revision 1.13  2001/01/16 17:06:37  dave
// added V2k Build version to header
//
// Revision 1.12  2001/01/15 21:11:43  dave
// removed extra print stmt
//
// Revision 1.11  2001/01/15 17:38:41  dave
// corrected Gray SetPixel, added icon but it needs fixin'
//
// Revision 1.10  2001/01/15 17:05:44  dave
// updated lipng108 for UNIX flavors, correct some syntax problems found by gcc
//
// Revision 1.9  2001/01/08 19:23:51  dave
// added constructor argument
//
// Revision 1.8  2000/12/14 21:36:11  dave
// log dir location cmd option, need C++ directory manip class
//
// Revision 1.7  2000/12/14 05:25:52  dave
// changes from am meeting
//
// Revision 1.6  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.5  2000/11/22 17:53:00  dave
// c
//
// Revision 1.4  2000/11/22 06:42:39  dave
// Rewrote to make sexier
//
// Revision 1.3  2000/11/13 18:07:01  ingmar
// Added GetPixel
//
// Revision 1.2  2000/11/03 02:06:28  ingmar
// Resize and operator <<
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
// $Id: PNGImage.C,v 1.2 2006/02/02 20:02:38 geconomos Exp $
