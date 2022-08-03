// $Id: V3DFile.C,v 1.6 2007/03/22 01:10:10 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "Deflate.h"
#include "V3DFile.h"
#include "VarMap.h"
#include "V2kVersion.h"
#include "ScanlineFilter.h"
#include "CRC.h"
#include "Point2D.h" 
#include "vxProgressBar.h"
#include "Timer.h"
#include "FilePtrFactory.h"


// Namespaces
using namespace std;


// Defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Constructor
 */
V3DFile::V3DFile()
: m_image(PixelRGB<uint1>(0,0,0)),
  m_imageRGBA(PixelRGBAu(0,0,0,0))
{
  m_bSkipData = false;
  m_bSkipImage = false;
  m_pDataBuffer = & m_privateBuffer;
  m_bAlphaImage = false;
  m_fileVersion = make_pair( 1, 0 );
  m_progressPair = std::pair< float4, float4 >( 0.0F, 100.0F );

  // add the PNG file signature
  m_vSignature.push_back(137);
  m_vSignature.push_back(80);
  m_vSignature.push_back(78);
  m_vSignature.push_back(71);
  m_vSignature.push_back(13);
  m_vSignature.push_back(10);
  m_vSignature.push_back(26);
  m_vSignature.push_back(10);

  InitIcon();
} // V3DFile()


/**
 * Destructor
 */
V3DFile::~V3DFile()
{
} // ~V3DFile()


/**
 * Read a file from disk
 *
 * @param sFilename    the file name
 * @param progressPair the start and end progress indication - set either value < 0 to not update meter
 */
void V3DFile::Read( const std::string & sFilename, std::pair< float4, float4 > & progressPair )
{
  std::shared_ptr<io::FilePtr> inStream = io::FilePtrFactory::CreateFilePtr( sFilename );
  inStream->Open ( GENERIC_READ, OPEN_EXISTING );

  m_compressedImageData.Resize(0);
  m_compressedImageData.Clear(0);
  ReadSignature( inStream );
  m_fileVersion = pair<int,int>( -1, -1 );
  m_progressPair = progressPair;

  // read chunks
  while ( inStream->IsEofSmart() == false )
  {
    try
    {
      uint4 uChunkSize(0);
      string sChunkType = ReadChunkHeader( inStream, uChunkSize );
      if ( sChunkType == "IHDR" )
      {
        LogDbg(util::Strings::Format("Reading %s chunk of size %d",sChunkType.c_str(), uChunkSize), "V3FFile","Read");
        ReadImageHeader( inStream, uChunkSize );
      }
      else if ( sChunkType == "tEXt" )
      {
        LogDbg(util::Strings::Format("Reading %s chunk of size %d",sChunkType.c_str(), uChunkSize), "V3FFile","Read");
        ReadTextChunk( inStream, uChunkSize );
      }
      else if ( sChunkType == "IDAT" )
      {
        LogDbg(util::Strings::Format("Reading %s chunk of size %d",sChunkType.c_str(), uChunkSize), "V3FFile","Read");
        ReadImageChunk( inStream, uChunkSize );
      }
      else if ( sChunkType.substr(0, 3) == "voL" )
      {
        LogDbg(util::Strings::Format("Reading %s chunk of size %d",sChunkType.c_str(), uChunkSize), "V3FFile","Read");
        ReadDataChunk( inStream, uChunkSize, sChunkType );
      }
      else if ( sChunkType == "IEND" )
      {
        LogDbg(util::Strings::Format("Reading %s chunk of size %d",sChunkType.c_str(), uChunkSize), "V3FFile","Read");

        ReadEndChunk( inStream, uChunkSize );
        ExtractImage();
      }
      else
      {
        LogDbg(util::Strings::Format("Reading EXTRA chunk of size %d",sChunkType.c_str(), uChunkSize), "V3FFile","Read");

        ReadExtraChunk( inStream, uChunkSize, sChunkType );
      }
    }
    catch ( ex::ReachedEndOfFile )
    {
      ; // no-op
    }
  }

} // Read()


/**
 * Read a file from disk
 *
 * @param sFilename    the input file name
 * @param sOutFilename the output file name
 * @param sNewComment  the new comment
 */
void V3DFile::Copy( const std::string & sInFilename, const std::string & sOutFilename, const std::string & sNewComment )
{

  std::shared_ptr<io::FilePtr> inStream  = io::FilePtrFactory::CreateFilePtr( sInFilename ); 
  inStream->Open( GENERIC_READ, OPEN_EXISTING );

  std::shared_ptr<io::FilePtr> outStream = io::FilePtrFactory::CreateFilePtr( sOutFilename ); 
  outStream->Open( GENERIC_WRITE, OPEN_ALWAYS );

  ReadSignature( inStream );
  WriteSignature( outStream );

  // read chunks
  while ( inStream->IsEofSmart() == false )
  {
    try
    {
      uint4 uChunkSize(0);
      string sChunkType = ReadChunkHeader( inStream, uChunkSize );
      if ( sChunkType == "tEXt" )
      {
        ReadTextChunk( inStream, uChunkSize );
        SetComment( sNewComment );
        WriteTextChunks( outStream );
      }
      else
      {
        CopyChunk( inStream, outStream, uChunkSize, sChunkType );
      }
    }
    catch ( ex::ReachedEndOfFile )
    {
      ; // no-op
    }
  } // endwhile

} // Copy()


/**
 * Specifies the new file version
 *
 * @param fileVersionMajorMinor the new file version
 */
void V3DFile::SetFileVersion( const std::pair<int4, int4> & fileVersionMajorMinor )
{
  m_fileVersion = fileVersionMajorMinor;
} // SetFileVersion()


/**
* Write a file to disk
*
* @param sFilename             the file name
* @param eDataChunkCompression the type of compression
*/
void V3DFile::Write( const std::string & sFilename, CompressionTypeEnum eDataChunkCompression )
{
  Write( sFilename, eDataChunkCompression, make_pair( 0.0F, 100.0F ) );
} // Write()


/**
 * Write a file to disk with progress metering
 *
 * @param sFilename             the file name
 * @param progressPair          the start and end progress indication - set either value < 0 to not update meter
 * @param eDataChunkCompression the type of compression
 */
void V3DFile::Write( const std::string & sFilename, CompressionTypeEnum eDataChunkCompression, std::pair< float4, float4 > progressPair )
{
  m_progressPair = progressPair;

  std::shared_ptr<io::FilePtr> outStream = io::FilePtrFactory::CreateFilePtr( sFilename );
  outStream->Open( GENERIC_WRITE, OPEN_ALWAYS );

  WriteSignature( outStream );
  WriteImageHeader( outStream );
  WriteTextChunks( outStream );
  WriteImageChunk( outStream );
  WriteDataChunk( outStream, eDataChunkCompression );
  WriteExtraChunks( outStream );
  WriteEndChunk( outStream );
} // Write()


/**
 * Retrieves the file version from disk
 *
 * @return the file version
 */
const std::pair<int4, int4> & V3DFile::GetFileVersion() const
{
  return m_fileVersion;
} // GetFileVersion()


/**
 * equality operator
 *
 * @param other the other V3D file
 * @return true if ==
 */
bool V3DFile::operator ==( const V3DFile & other ) const
{
  // test variables
  if ( m_varlist.size() != other.m_varlist.size() )
    return false;

  std::list<Variable>::const_iterator it = m_varlist.begin();
  std::list<Variable>::const_iterator otherIt = other.m_varlist.begin();
  for ( ; it != m_varlist.end() && otherIt != other.m_varlist.end(); ++it, ++otherIt )
  {
    if ( *it != *otherIt ) return false;
  }

  // test other objects
  if ( m_fileVersion != other.m_fileVersion ) return false;

  if ( m_sTitle != other.m_sTitle ) return false;

  if ( m_sComment != other.m_sComment ) return false;

  if ( m_sDescription != other.m_sDescription ) return false;

  if ( m_image != other.m_image ) return false;

  if ( *m_pDataBuffer != *other.m_pDataBuffer ) return false;

  return true;
} // operator ==()


/**
 * equality operator
 *
 * @param other the other V3D file
 * @return true if ==
 */
bool V3DFile::operator !=( const V3DFile & other ) const
{
  return ! ( * this == other );
} // operator !=()


/**
 * Write out the signature to disk
 *
 * @param outStream the output file
 */
void V3DFile::WriteSignature( std::shared_ptr<io::FilePtr> & outStream )
{
  const uint4 uSignatureSize( static_cast< uint4 >( m_vSignature.size() ));
  uint1 * pSignature = new uint1[ uSignatureSize ];

  for ( uint4 i(0); i< uSignatureSize; ++i )
    pSignature[i] = m_vSignature[i];

  outStream->Write( pSignature, uSignatureSize );

  delete [] pSignature;

} // WriteSignature()


/**
 * Read the signature from disk
 *
 * @param inStream the input file
 */
void V3DFile::ReadSignature( std::shared_ptr<io::FilePtr> & inStream )
{
  const uint4 uSignatureSize( static_cast< uint4 >( m_vSignature.size() ));
  uint1 * pDiskSignature = new uint1[ uSignatureSize ];

  inStream->Read( (void *) pDiskSignature, uSignatureSize );

  for ( uint4 i(0); i<uSignatureSize; ++i )
  {
    if ( pDiskSignature[i] != m_vSignature.at(i) )
    {
      delete [] pDiskSignature;
      throw ex::DataCorruptionException( LogRec( "Unrecognized file signature.", "V3DFile", "ReadSignature" ) );
    }
  }

  delete [] pDiskSignature;

} // ReadSignature()



/**
 * Write the image header to disk
 *
 * @param outStream the output file
 */
void V3DFile::WriteImageHeader( std::shared_ptr<io::FilePtr> & outStream )
{
  const uint4 uHeaderLength( 13 );
  uint1 vBuffer[ uHeaderLength ];
  uint4 * puWidth             = (uint4 *) (vBuffer + 0);
  uint4 * puHeight            = (uint4 *) (vBuffer + 4);
  uint1 * puBitDepth          = (uint1 *) (vBuffer + 8);
  uint1 * puColorType         = (uint1 *) (vBuffer + 9);
  uint1 * puCompressionMethod = (uint1 *) (vBuffer + 10);
  uint1 * puFilterMethod      = (uint1 *) (vBuffer + 11);
  uint1 * puInterlaceMethod   = (uint1 *) (vBuffer + 12);

  *puWidth = m_bAlphaImage ? m_imageRGBA.GetDim().m_x : m_image.GetDim().m_x; SwapUint4ByteOrder(*puWidth);
  *puHeight = m_bAlphaImage ? m_imageRGBA.GetDim().m_y : m_image.GetDim().m_y; SwapUint4ByteOrder(*puHeight);
  *puBitDepth = 8;
  *puColorType = m_bAlphaImage ? 6 : 2;
  *puCompressionMethod = 0;
  *puFilterMethod = 0;
  *puInterlaceMethod = 0;

  WriteChunk( outStream, uHeaderLength, "IHDR", vBuffer, NONE );

} // WriteImageHeader()


/**
 * Read the image header from disk
 *
 * @param inStream   the input file
 * @param uChunkSize the size of the chunk
 */
void V3DFile::ReadImageHeader( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize )
{
  // check header length
  const uint4 uExpectedHeaderLength( 13 );
  if ( uChunkSize != uExpectedHeaderLength )
    throw ex::DataCorruptionException( LogRec( "Bad IHDR header length", "V3DFile", "ReadImageHeader" ) );

  // read the chunk from disk
  MemoryBuffer memBuf( uChunkSize );
  ReadChunk( inStream, "IHDR", memBuf );
  uint1 * pBuffer = memBuf.GetDataPtr();

  if ( m_bSkipImage == false )
  {

    // index into the buffer
    uint4 * puWidth             = (uint4 *) (pBuffer + 0); SwapUint4ByteOrder(*puWidth);
    uint4 * puHeight            = (uint4 *) (pBuffer + 4); SwapUint4ByteOrder(*puHeight);
    uint1 * puBitDepth          = (uint1 *) (pBuffer + 8);
    uint1 * puColorType         = (uint1 *) (pBuffer + 9);
    uint1 * puCompressionMethod = (uint1 *) (pBuffer + 10);
    uint1 * puFilterMethod      = (uint1 *) (pBuffer + 11);
    uint1 * puInterlaceMethod   = (uint1 *) (pBuffer + 12);

    // check some things
    if ( *puBitDepth != 8 )
      throw ex::DataCorruptionException( LogRec( "Unhandled bit depth.", "V3DFile", "ReadImageHeader" ) );

    if ( *puColorType != 2 && *puColorType != 6 )
      throw ex::DataCorruptionException( LogRec( "Unhandled color type.", "V3DFile", "ReadImageHeader" ) );

    if ( *puCompressionMethod != 0 )
      throw ex::DataCorruptionException( LogRec( "Unhandled compression method.", "V3DFile", "ReadImageHeader" ) );

    if ( *puFilterMethod != 0 )
      throw ex::DataCorruptionException( LogRec( "Unhandled filter method.", "V3DFile", "ReadImageHeader" ) );

    if ( *puInterlaceMethod != 0 )
      throw ex::DataCorruptionException( LogRec( "Unhandled interlace method.", "V3DFile", "ReadImageHeader" ) );

    // resize the image
    m_bAlphaImage = ((*puColorType) == 6);
    if ( m_bAlphaImage )
      m_imageRGBA.Resize( Triple<uint4>( * puWidth, * puHeight, 0 ) );
    else
      m_image.Resize( Triple<uint4>( * puWidth, * puHeight, 0 ) );

  } // if not skipping image

} // ReadImageHeader()


/**
 * Write a text chunk to disk
 *
 * @param outStream the output file
 * @param variable  the variable to write
 */
void V3DFile::WriteTextChunk( std::shared_ptr<io::FilePtr> & outStream, const Variable & variable ) const
{
  string sString = PackVariable( variable );
  WriteChunk( outStream, static_cast< uint4 >( sString.size() ), "tEXt", (uint1 *) sString.c_str(), NONE );
}  // WriteTextChunk()


/**
 * Read a text chunk from disk
 *
 * @param inStream   the input file
 * @param uChunkSize the size of the chunk
 */
void V3DFile::ReadTextChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize )
{
  // read the text chunk
  MemoryBuffer memBuf( uChunkSize );
  ReadChunk( inStream, "tEXt", memBuf );

  // convert to an std::string
  string sString( uChunkSize, 0 );
  for ( uint4 i(0); i<uChunkSize; i++ )
    sString[i] = memBuf[i];

  // add to list
  Variable var;
  try
  {
    var = UnpackVariable( sString );
  }
  catch ( ... )
  {
    // this was not in the right format, must be an non-V3D format image
    var.SetName(sString);
    var.SetValue(sString);
  }
  if ( var.GetName() == "Description" )
  {
    m_sDescription = var.GetValue();
  }
  else if ( var.GetName() == "Title" )
  {
    m_sTitle = var.GetValue();
  }
  else if ( var.GetName() == "Comment" )
  {
    m_sComment = var.GetValue();
  }
  else if ( var.GetName() == "Software" )
  {
    stringstream ss;
    ss << var.GetValue();
    ss >> m_fileVersion.first >> m_fileVersion.second;
  }
  else
  {
    m_varlist.push_back(var);
  }

} // ReadTextChunk()


/**
 * Write the text chunks to disk
 *
 * @param outStream the output file
 */
void V3DFile::WriteTextChunks( std::shared_ptr<io::FilePtr> & outStream )
{
  // write a description text chunk
  WriteTextChunk( outStream, Variable( "Description", m_sDescription, "The description" ) );

  // write the file title
  WriteTextChunk( outStream, Variable( "Title", m_sTitle, "The title of the file" ) );

  // write a comment text chunk
  WriteTextChunk( outStream, Variable( "Comment", m_sComment, "The comment" ) );

  // write a software text chunk
  string sMajor = ToAscii( m_fileVersion.first );
  string sMinor = ToAscii( m_fileVersion.second );
  string sVersion = sMajor + string(" ") + sMinor + string(" BUILD:") + string(V2K_VERSION);
  WriteTextChunk( outStream, Variable( "Software", sVersion, "The version of the software used to create this." ) );

  // write out the variables
  for ( list<Variable>::const_iterator it( m_varlist.begin() ); it!=m_varlist.end(); ++it )
    WriteTextChunk( outStream, * it );

} // WriteTextChunks()


/**
 * Pack an RGB image into a memory buffer
 *
 * @param image the image to pack
 *
 * @return the memory buffer containing the image
 */
MemoryBuffer V3DFile::PackImage( const Image<PixelRGB<uint1> > & image )
{
  // write a zero byte at the front of each scanline to indicate "no filtering"
  const uint4 uScanlineBytes( 3 * image.GetDim().m_x + 1 );
  MemoryBuffer previousScanline( uScanlineBytes );
  const uint4 uSizeBytes( image.GetDim().m_y * uScanlineBytes );
  MemoryBuffer memBuffer( uSizeBytes );
  Point2D<uint2> pos;
  const uint4 uImageDimY( image.GetDim().m_y );
  const uint4 uImageDimX( image.GetDim().m_x );
  for ( pos.m_y=0; pos.m_y < uImageDimY; ++pos.m_y )
  {
    MemoryBuffer scanlineBuffer( memBuffer.GetDataPtr() + uScanlineBytes * pos.m_y, uScanlineBytes );
    uint1 * pBufferPtr = scanlineBuffer.GetDataPtr();
    *pBufferPtr++ = 0;
    for ( pos.m_x=0; pos.m_x < uImageDimX; ++pos.m_x )
    {
      const PixelRGB<uint1> & pixel = image.GetPixel(pos);
      *pBufferPtr++ = pixel.R();
      *pBufferPtr++ = pixel.G();
      *pBufferPtr++ = pixel.B();
    }

    // encode
    ScanlineFilter lineFilter( scanlineBuffer, previousScanline, 3 );
    lineFilter.EncodeScanline();
  }
  return memBuffer;
} // PackImage()


/**
 * Pack an RGBA image into a memory buffer
 *
 * @param image the image to pack
 *
 * @return the memory buffer containing the image
 */
MemoryBuffer V3DFile::PackImageRGBA( const Image<PixelRGBA<uint1> > & image )
{
  // write a zero byte at the front of each scanline to indicate "no filtering"
  const uint4 uScanlineBytes( 4 * image.GetDim().m_x + 1 );
  MemoryBuffer previousScanline( uScanlineBytes );
  const uint4 uSizeBytes( image.GetDim().m_y * uScanlineBytes );
  MemoryBuffer memBuffer( uSizeBytes );
  Point2D<uint2> pos;
  const uint4 uImageDimY( image.GetDim().m_y );
  const uint4 uImageDimX( image.GetDim().m_x );
  for ( pos.m_y=0; pos.m_y < uImageDimY; ++pos.m_y )
  {
    MemoryBuffer scanlineBuffer( memBuffer.GetDataPtr() + uScanlineBytes * pos.m_y, uScanlineBytes );
    uint1 * pBufferPtr = scanlineBuffer.GetDataPtr();
    *pBufferPtr++ = 0;
    for ( pos.m_x=0; pos.m_x<uImageDimX; ++pos.m_x )
    {
      const PixelRGBA<uint1> & pixel = image.GetPixel(pos);
      *pBufferPtr++ = pixel.R();
      *pBufferPtr++ = pixel.G();
      *pBufferPtr++ = pixel.B();
      *pBufferPtr++ = pixel.A();
    }

    // encode
    ScanlineFilter lineFilter( scanlineBuffer, previousScanline, 4 );
    lineFilter.EncodeScanline();
  }
  return memBuffer;
} // PackImageRGBA()


/**
 * Unpack an RGB image from a memory buffer
 *
 * @param srcMemBuffer the source memory buffer
 * @param dimensions   the dimensions of the image
 *
 * @return the unpacked RGB image
 */
Image< PixelRGB<uint1> > V3DFile::UnpackImage( MemoryBuffer & srcMemBuffer, const Triple<uint4> & dimensions )
{
  // unpack the rows (each row starts with a zero byte which indicates "no filtering"
  const uint4 uScanlineBytes( 3 * dimensions.m_x+ 1 );
  MemoryBuffer previousScanline( uScanlineBytes );
  previousScanline.Clear(0);
  Image<PixelRGB<uint1> > image( dimensions, PixelRGB<uint1>( 0, 0, 0 ) );
  Point2D<uint2> pos;
  const uint4 uImageDimY( image.GetDim().m_y );
  const uint4 uImageDimX( image.GetDim().m_x );
  PixelRGB<uint1> pixel;
  uint4 uTmp;
  for ( pos.m_y=0; pos.m_y < uImageDimY; ++pos.m_y )
  {
    // decode
    MemoryBuffer scanlineBuffer( srcMemBuffer.GetDataPtr() + uScanlineBytes * pos.m_y, uScanlineBytes );
    ScanlineFilter lineFilter( scanlineBuffer, previousScanline, 3 );
    lineFilter.DecodeScanline();

    // extract pixels now
    for ( pos.m_x=0; pos.m_x < uImageDimX; ++pos.m_x )
    {
      uTmp = 3 * pos.m_x;
      pixel.R() = scanlineBuffer[ ++uTmp ]; // offset by 1 because of filter
      pixel.G() = scanlineBuffer[ ++uTmp ]; // byte at scanline start
      pixel.B() = scanlineBuffer[ ++uTmp ];
      image.SetPixel( pos, pixel );
    }

    // save this scanline as previous
    previousScanline = scanlineBuffer;
  }
  return image;

} // UnpackImage()


/**
 * Unpack an RGBA image from a memory buffer
 *
 * @param srcMemBuffer the source memory buffer
 * @param dimensions   the dimensions of the image
 *
 * @return the unpacked RGBA image
 */
Image< PixelRGBA<uint1> > V3DFile::UnpackImageRGBA( MemoryBuffer & srcMemBuffer, const Triple<uint4> & dimensions )
{
  // unpack the rows (each row starts with a zero byte which indicates "no filtering"
  const uint4 uScanlineBytes( 4 * dimensions.m_x+ 1 );
  MemoryBuffer previousScanline( uScanlineBytes );
  previousScanline.Clear(0);
  Image< PixelRGBA<uint1> > image( dimensions, PixelRGBA<uint1>( 0, 0, 0, 0 ) );
  Point2D<uint2> pos;
  const uint4 uImageDimY( image.GetDim().m_y );
  const uint4 uImageDimX( image.GetDim().m_x );
  PixelRGBA<uint1> pixel;
  uint4 uTmp;
  for ( pos.m_y=0; pos.m_y < uImageDimY; ++pos.m_y )
  {
    // decode
    MemoryBuffer scanlineBuffer( srcMemBuffer.GetDataPtr() + uScanlineBytes * pos.m_y, uScanlineBytes );
    ScanlineFilter lineFilter( scanlineBuffer, previousScanline, 4 );
    lineFilter.DecodeScanline();    
    // extract pixels now
    for ( pos.m_x=0; pos.m_x < uImageDimX; ++pos.m_x )
    {
      uTmp = 4 * pos.m_x;
      pixel.R() = scanlineBuffer[ ++uTmp ]; // offset by 1 because of filter
      pixel.G() = scanlineBuffer[ ++uTmp ]; // byte at scanline start
      pixel.B() = scanlineBuffer[ ++uTmp ];
      pixel.A() = scanlineBuffer[ ++uTmp ];
      image.SetPixel( pos, pixel );
    }

    // save this scanline as previous
    previousScanline = scanlineBuffer;
  }
  return image;
} // UnpackImageRGBA()


/**
 * Write the image chunks to disk
 *
 * @param outStream the output file
 */
void V3DFile::WriteImageChunk( std::shared_ptr<io::FilePtr> & outStream )
{
  // write a zero byte at the front of each scanline to indicate "no filtering"
  if ( m_bAlphaImage )
  {
    MemoryBuffer memBuffer = PackImageRGBA( m_imageRGBA );
    WriteChunk( outStream, memBuffer.GetSize(), "IDAT", (uint1 *) ( memBuffer.GetDataPtr() ), ZLIB );
  }
  else
  {
    MemoryBuffer memBuffer = PackImage( m_image );
    WriteChunk( outStream, memBuffer.GetSize(), "IDAT", (uint1 *) ( memBuffer.GetDataPtr() ), ZLIB );
  }
} // WriteImageChunk()


/** 
 * Read the image chunk from disk
 *
 * @param inStream   the input file
 * @param uChunkSize the size of the chunk
 */
void V3DFile::ReadImageChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize )
{
  const string sChunkType( "IDAT" );
  if ( m_bSkipImage )
  {
    SkipChunk( inStream, uChunkSize );
  }
  else
  {
    uint4 uOriginalDataSize( m_compressedImageData.GetSize() );
    m_compressedImageData.Resize( uOriginalDataSize+uChunkSize );
    MemoryBuffer compressedBuf( m_compressedImageData.GetDataPtr() + uOriginalDataSize, uChunkSize );
    ReadChunk( inStream, sChunkType, compressedBuf );
  }
} // ReadImageChunk()


/**
 * Unpack the image from the file data
 */
void V3DFile::ExtractImage()
{
  if ( m_bSkipImage == false )
  {
    if ( m_bAlphaImage )
    {
      const uint4 uSizeBytes( m_image.GetDim().m_x * m_image.GetDim().m_y * 4 + m_image.GetDim().m_y );
      MemoryBuffer uncompressedBuf( uSizeBytes );
      InflateZLIB( uncompressedBuf, m_compressedImageData );
      m_imageRGBA = UnpackImageRGBA( uncompressedBuf, m_imageRGBA.GetDim() );
    }
    else
    {
      const uint4 uSizeBytes( m_image.GetDim().m_x * m_image.GetDim().m_y * 3 + m_image.GetDim().m_y );
      MemoryBuffer uncompressedBuf( uSizeBytes );
      InflateZLIB( uncompressedBuf, m_compressedImageData );
      m_image = UnpackImage( uncompressedBuf, m_image.GetDim() );
    }
  }
} // ExtractImage()


/**
 * Read the header of a chunk
 *
 * @param inStream   the input file
 * @param uChunkSize the size of the chunk
 * @return           the string just read
 */
string V3DFile::ReadChunkHeader( std::shared_ptr<io::FilePtr> & inStream, uint4 & uChunkSize ) const
{
  if ( inStream->IsEof() )
    throw ex::ReachedEndOfFile( LogRec( "End of file.", "V3DFile", "ReadChunkHeader" ) );

  // read the data length
  try
  {
    uChunkSize = ReadUintRaw( inStream );
  }
  catch ( ex::IOException e )
  {
    if ( inStream->IsEof() )
    {
      // just as we suspected, reached the end of the file
      throw ex::ReachedEndOfFile( LogRec( "End of file.", "V3DFile", "ReadChunkHeader" ) );
    }
    else
    {
      throw e;
    }
  }

  // read the chunk type text
  string sChunkType = ReadChunkType( inStream );

  if ( inStream->IsEof() )
    throw ex::ReachedEndOfFile( LogRec( "End of file.", "V3DFile", "ReadChunkHeader" ) );

  return( sChunkType );

} // ReadChunkHeader()


/**
 * Read the data chunk from disk
 *
 * @param inStream   the input file
 * @param uChunkSize the size of the chunk
 * @param sChunkType the type of the chunk
 */
void V3DFile::ReadDataChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize, const string & sChunkType )
{
  // check some things
  const std::string sExpectedChunkType( "voL" );
  if ( sChunkType.substr( 0, 3 ) != sExpectedChunkType )
    throw ex::DataCorruptionException( LogRec( "Expected " + sExpectedChunkType + " chunk.", "V3DFile", "ReadDataChunk" ) );
  
  if ( m_bSkipData )
  {
    SkipChunk(inStream, uChunkSize);
  } // don't skip the data, read it
  else
  {
    if ( sChunkType[3] == 'a' )  // array of memory blocks
    {
      //// assumes that m_dataBuffer holds an array of pointers to memory blocks 
      //// that are of sufficient size to hold the memory blocks on the disk
      
      CRC crc;
      unsigned long lCheckSum( crc.UpdateCrc( crc.CrcInit(), (unsigned char *) sChunkType.c_str(), 4 ) );
      vxProgressBar::SetProgress( 10, "Starting read" );
      
      // get max final destination block size
      uint4 uBlockByteSize;
      inStream->Read( (char *)( & uBlockByteSize ), sizeof( uBlockByteSize ) );
      MemoryBuffer scratchMemory( static_cast< uint4 >( uBlockByteSize * 1.1 ));
      
      MemBlockInfo * pMemBlockInfo = (MemBlockInfo *) m_pDataBuffer->GetDataPtr();
      uint4 uNumMemBlocks( m_pDataBuffer->GetSize() / sizeof( MemBlockInfo ) );
      
      // read all blocks
      for ( uint4 k = 1; k < uNumMemBlocks; ++k )
      {
        // get compressed block size
        uint4 uCompressedBlockSize;
        inStream->Read( ( char * )( & uCompressedBlockSize ), sizeof( uCompressedBlockSize ) );
        
        // read compressed block
        inStream->Read( ( char * ) ( scratchMemory.GetDataPtr() ), uCompressedBlockSize );
        // read 
        inStream->Read( ( char * ) & ( pMemBlockInfo[k].iFlag ), sizeof( int4 ) );
        
        
        lCheckSum = crc.UpdateCrc( lCheckSum, scratchMemory.GetDataPtr(), uCompressedBlockSize );
        
        // uncompress the block into final destination memory
        Inflate::InflateIt( hdr::LZO, scratchMemory.GetDataPtr(), uCompressedBlockSize, pMemBlockInfo[k].pMemBlock, pMemBlockInfo[k].uMemBlockByteSize );

      } // finished reading all blocks
      
      lCheckSum = crc.CrcComplement( lCheckSum );
      unsigned long lSavedCheckSum;
      inStream->Read( ( char * ) ( & lSavedCheckSum ), sizeof( lSavedCheckSum ) );
      
      if( lSavedCheckSum != lCheckSum )
        throw ex::DataCorruptionException( LogRec( "Invalid checksum.", "V3DFile", "ReadDataChunk" ) );

    } // finished chunk of memory blocks
    else if ( sChunkType[3] == 'b' ) // no compression    
    {
      //// assumes that m_dataBuffer holds an array of pointers to memory blocks 
      //// that are of sufficient size to hold the memory blocks on the disk
      
      CRC crc;
      unsigned long lCheckSum( crc.UpdateCrc( crc.CrcInit(), ( unsigned char * ) sChunkType.c_str(), 4 ) );
      
      // get max final destination block size
      uint4 uBlockByteSize;
      inStream->Read( ( char * ) ( & uBlockByteSize ), sizeof( uBlockByteSize ) );
      MemoryBuffer scratchMemory( static_cast< uint4 >( ( uBlockByteSize + 4 ) * 32 * 1.1 ));
      MemBlockInfo * pMemBlockInfo = ( MemBlockInfo * ) m_pDataBuffer->GetDataPtr();
      uint4 uNumMemBlocks( m_pDataBuffer->GetSize() / sizeof( MemBlockInfo ) );

      // read all blocks
      
      // get compressed block size
      uint4 uCompressedBlockSize;
      inStream->Read( ( char * ) ( & uCompressedBlockSize ), sizeof( uCompressedBlockSize ) );
      
      // read compressed block
      inStream->Read( ( char * ) ( scratchMemory.GetDataPtr() ), uCompressedBlockSize );

      // read 

      inStream->Read( ( char * ) & ( pMemBlockInfo[1].iFlag ), sizeof( int4 ) );
      lCheckSum = crc.UpdateCrc( lCheckSum, scratchMemory.GetDataPtr(), uCompressedBlockSize );
      
      // uncompress the block into final destination memory
      Inflate::InflateIt( hdr::LZO, scratchMemory.GetDataPtr(), uCompressedBlockSize, pMemBlockInfo[1].pMemBlock, pMemBlockInfo[1].uMemBlockByteSize );
      
      int4 iBlockByteSize( 3072 * 2 + 4 );
      const int4 iNumberOfBlocksInOneChunk( 32 );
      unsigned char * pTmpBuf = new unsigned char[ iBlockByteSize * iNumberOfBlocksInOneChunk ];
      
      // This is the start of the actual data
      int4 k( 2 );
      while( static_cast< uint4 >(( k + iNumberOfBlocksInOneChunk )) < uNumMemBlocks )
      {
        // get compressed block size
        uint4 uCompressedBlockSize;
        inStream->Read( ( char * ) ( & uCompressedBlockSize ), sizeof( uCompressedBlockSize ) );
        
        // read compressed block
        inStream->Read( ( char * ) scratchMemory.GetDataPtr(), uCompressedBlockSize );

        lCheckSum = crc.UpdateCrc( lCheckSum,scratchMemory.GetDataPtr(), uCompressedBlockSize );
        
        // uncompress the block into final destination memory
        Inflate::InflateIt( hdr::LZO, scratchMemory.GetDataPtr(), uCompressedBlockSize, pTmpBuf, iBlockByteSize * iNumberOfBlocksInOneChunk );
       
        unsigned char * pSrcCurrent = pTmpBuf;
        for( int4 g(0); g < iNumberOfBlocksInOneChunk; ++g,++k )
        {          
          unsigned char * pDestCurrent = (unsigned char *) pMemBlockInfo[k].pMemBlock;
          int4  i;
          for( i=0; i<4096; ++i )
          {
            *pDestCurrent = *pSrcCurrent++;
            pDestCurrent += 2;
          }
          
          pDestCurrent = (unsigned char *) pMemBlockInfo[k].pMemBlock;
          
          for( i=0; i<4096/2; ++i )
          {
            *( pDestCurrent + 1 ) = ( * pSrcCurrent & 0xF0 ) >> 4;
            *( pDestCurrent + 3 ) = ( * pSrcCurrent & 0x0F );
            pSrcCurrent++;
            pDestCurrent += 4;
          }
          int4 * pDum = ( int4 * ) pSrcCurrent;
          pMemBlockInfo[k].iFlag = * pDum;
          pSrcCurrent += 4;
        }
        
      } // finished reading all blocks
      
      for ( ; static_cast< uint4 >(k)<uNumMemBlocks; ++k )
      {
        // get compressed block size
        uint4 uCompressedBlockSize;
        inStream->Read( ( char * ) ( & uCompressedBlockSize ), sizeof( uCompressedBlockSize ) );
        
        // read compressed block
        inStream->Read( ( char * ) scratchMemory.GetDataPtr(), uCompressedBlockSize );

        // read 
        inStream->Read( ( char * ) & ( pMemBlockInfo[k].iFlag ), sizeof( int4 ) );
        
        lCheckSum = crc.UpdateCrc( lCheckSum,scratchMemory.GetDataPtr(), uCompressedBlockSize );
        
        // uncompress the block into final destination memory
        Inflate::InflateIt( hdr::LZO, scratchMemory.GetDataPtr(), uCompressedBlockSize, pTmpBuf, iBlockByteSize );
        
        unsigned char * pDestCurrent = ( unsigned char * ) pMemBlockInfo[k].pMemBlock;
        unsigned char * pSrcCurrent = pTmpBuf;
        int4  i;
        for( i=0; i<4096; ++i )
        {
          *pDestCurrent = *pSrcCurrent++;
          pDestCurrent += 2;
        }
        
        pDestCurrent = ( unsigned char * ) pMemBlockInfo[k].pMemBlock;
        
        for( i=0; i<4096/2; ++i )
        {
          *( pDestCurrent + 1 ) = ( * pSrcCurrent & 0xF0 ) >> 4;
          *( pDestCurrent + 3 ) = ( * pSrcCurrent & 0x0F );
          pSrcCurrent++;
          pDestCurrent += 4;
        }
                
      } // finished reading all blocks

      lCheckSum = crc.CrcComplement( lCheckSum );
      unsigned long lSavedCheckSum;
      inStream->Read( ( char * ) ( & lSavedCheckSum ), sizeof( lSavedCheckSum ) );

      if( lSavedCheckSum != lCheckSum )
        throw ex::DataCorruptionException( LogRec( "Invalid checksum.", "V3DFile", "ReadDataChunk" ) );

    }
    else if ( sChunkType[3] == 'c' ) // no compression    
    {
      //// assumes that m_dataBuffer holds an array of pointers to memory blocks 
      //// that are of sufficient size to hold the memory blocks on the disk
      
      CRC crc;
      unsigned long lCheckSum( crc.UpdateCrc( crc.CrcInit(), ( unsigned char * ) sChunkType.c_str(), 4 ) );
      
      // get max final destination block size
      uint4 uBlockByteSize;
      inStream->Read( ( char * ) ( & uBlockByteSize ), sizeof( uBlockByteSize ) );
      MemoryBuffer scratchMemory( static_cast< uint4 >(( uBlockByteSize + 4 ) * 32 * 1.1 )); 
      MemBlockInfo * pMemBlockInfo = ( MemBlockInfo * ) m_pDataBuffer->GetDataPtr();
      uint4 uNumMemBlocks( m_pDataBuffer->GetSize() / sizeof( MemBlockInfo ) );

      // read all blocks      
      
      // get compressed block size
      uint4 uCompressedBlockSize;
      inStream->Read( ( char * ) ( & uCompressedBlockSize ), sizeof( uCompressedBlockSize ) );
      
      // read compressed block
      inStream->Read( ( char * ) scratchMemory.GetDataPtr(), uCompressedBlockSize );

      // read 
      inStream->Read( ( char * ) & ( pMemBlockInfo[1].iFlag ), sizeof( int4 ) );
      
      lCheckSum = crc.UpdateCrc( lCheckSum,scratchMemory.GetDataPtr(), uCompressedBlockSize );
      
      // uncompress the block into final destination memory
      Inflate::InflateIt( hdr::LZO, scratchMemory.GetDataPtr(), uCompressedBlockSize, pMemBlockInfo[1].pMemBlock, pMemBlockInfo[1].uMemBlockByteSize );
      
      int4 iBlockByteSize( 4096 * pMemBlockInfo[2].uBytesPerEntryInBlock + 4 );
      const int4 iNumberOfBlocksInOneChunk( 32 );
      unsigned char * pTmpBuf = new unsigned char[ iBlockByteSize * iNumberOfBlocksInOneChunk ];
      
      // This is the start of the actual data
      int4 k( 2 );
      float4 fLastProgressPercentage( -1000.0F );
      while( static_cast< uint4 >( ( k + iNumberOfBlocksInOneChunk )) < uNumMemBlocks )
      {
        // get compressed block size
        uint4 uCompressedBlockSize;
        inStream->Read( ( char * ) ( & uCompressedBlockSize ), sizeof( uCompressedBlockSize ) );
        
        // read compressed block
        inStream->Read( ( char * ) scratchMemory.GetDataPtr(), uCompressedBlockSize );
        
        lCheckSum = crc.UpdateCrc( lCheckSum,scratchMemory.GetDataPtr(), uCompressedBlockSize );
        
        // uncompress the block into final destination memory
        Inflate::InflateIt( hdr::LZO, scratchMemory.GetDataPtr(), uCompressedBlockSize, pTmpBuf, iBlockByteSize * iNumberOfBlocksInOneChunk );
        
        unsigned char * pSrcCurrent = pTmpBuf;
        for( int4 g(0); g<iNumberOfBlocksInOneChunk; ++g,++k )
        {        
          uint1 * pDestCurrent = ( uint1* ) pMemBlockInfo[k].pMemBlock;
          if( pMemBlockInfo[k].uBytesPerEntryInBlock == 2 )
            CopyBufferInterleaved( pDestCurrent, pSrcCurrent, true );
          else
          {
            const uint2 uSize = 4096 *  pMemBlockInfo[k].uBytesPerEntryInBlock;
            memcpy( pDestCurrent, pSrcCurrent,  uSize);
            pSrcCurrent += uSize;
          }
  
          int4 * pDum = ( int4 * ) pSrcCurrent;
          pMemBlockInfo[k].iFlag = * pDum;
          pSrcCurrent +=4;
        }

        // update the progress bar
        if ( m_progressPair.first >= 0 && m_progressPair.second >= 0 )
        {
          const float4 fPercentage( Interpolate( float4( iNumberOfBlocksInOneChunk ), float4( k+iNumberOfBlocksInOneChunk ),
                                                 float4( uNumMemBlocks ), m_progressPair.first, m_progressPair.second ) );
          const float4 fPercentageThreshold( 5.0F );
          if ( abs( static_cast< int4 >( fPercentage - fLastProgressPercentage ) ) > fPercentageThreshold )
          {
            fLastProgressPercentage = fPercentage;
            vxProgressBar::SetProgressPercentage( fPercentage );
          }
        } // if updating the progress meter

      } // finished reading all blocks
      

      for ( ; static_cast<uint4>(k)<uNumMemBlocks; ++k )
      {

        // get compressed block size
        uint4 uCompressedBlockSize;
        inStream->Read( ( char * ) ( & uCompressedBlockSize ), sizeof( uCompressedBlockSize ) );
        
        // read compressed block
        inStream->Read( ( char * ) scratchMemory.GetDataPtr(), uCompressedBlockSize );

        // read 
        inStream->Read( ( char * ) & ( pMemBlockInfo[k].iFlag ), sizeof( int4 ) );
        
        lCheckSum = crc.UpdateCrc( lCheckSum,scratchMemory.GetDataPtr(), uCompressedBlockSize );
        
        // uncompress the block into final destination memory
        Inflate::InflateIt( hdr::LZO, scratchMemory.GetDataPtr(), uCompressedBlockSize, pTmpBuf, iBlockByteSize );
        
        uint1* pDestCurrent = (uint1*) pMemBlockInfo[k].pMemBlock;
        if( pMemBlockInfo[k].uBytesPerEntryInBlock == 2 )
          CopyBufferInterleaved( pDestCurrent , pTmpBuf, true );
        else
          memcpy( pDestCurrent, pTmpBuf, 4096 *  pMemBlockInfo[k].uBytesPerEntryInBlock );
                
      } // finished reading all blocks

      lCheckSum = crc.CrcComplement( lCheckSum );
      unsigned long lSavedCheckSum;
      inStream->Read( ( char * ) ( & lSavedCheckSum ), sizeof( lSavedCheckSum ) );
      if( lSavedCheckSum != lCheckSum )
        throw ex::DataCorruptionException( LogRec( "Invalid checksum.", "V3DFile", "ReadDataChunk" ) );

    }
    else if ( sChunkType[3] == 'n' ) // no compression
    {
      MemoryBuffer scratchMemory( uChunkSize );
      m_pDataBuffer->Resize( scratchMemory.GetSize() );
      ReadChunk( inStream, sChunkType, scratchMemory );
      m_pDataBuffer->CopyIn( scratchMemory.GetDataPtr(), m_pDataBuffer->GetSize() );
    }
    else if ( sChunkType[3] == 'l' ) // LZO compression 
    {
      MemoryBuffer scratchMemory( uChunkSize );
      ReadChunk( inStream, sChunkType, scratchMemory );
      InflateLZO( * m_pDataBuffer, scratchMemory );
    }
    else if ( sChunkType[3] == 'z' ) // ZLIB compression
    {
      MemoryBuffer scratchMemory( uChunkSize );
      ReadChunk( inStream, sChunkType, scratchMemory );
      InflateZLIB( * m_pDataBuffer, scratchMemory );
    }
    else
    {
      throw ex::DataCorruptionException( LogRec( "Unhandled compression method.", "V3DFile", "ReadDataChunk" ) );
    }
  }
} // ReadDataChunk()


/**
 * Write the extra chunks to disk
 *
 * @param outStream the output file stream
 */
void V3DFile::WriteExtraChunks( std::shared_ptr<io::FilePtr> & outStream )
{
  for ( std::list< std::pair< std::string, MemoryBuffer > >::iterator iter = m_extraChunks.begin();
        iter != m_extraChunks.end();
        ++iter )
  {
    WriteChunk( outStream, iter->second.GetSize(), iter->first, iter->second.GetDataPtr(), LZO );
  }
} // WriteExtraChunks()


/**
 * Read an extra chunk from disk
 *
 * @param inStream   the input file
 * @param uChunkSize the size of the chunk
 * @param sChunkType the type of the chunk
 */
void V3DFile::ReadExtraChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize, const std::string & sChunkType )
{

  // possibly just skip the data
  if ( m_bSkipData )
  {
    SkipChunk( inStream, uChunkSize );
    return;
  }

  // don't skip the data, read it
  MemoryBuffer scratchMemory( uChunkSize );
  ReadChunk( inStream, sChunkType, scratchMemory );
  MemoryBuffer uncompressedMemory;
  InflateLZO( uncompressedMemory, scratchMemory );

  // save it
  m_extraChunks.push_back( make_pair( sChunkType, uncompressedMemory ) );

} // ReadExtraChunk()


/**
 * Performs an interlacing buffer copy
 *
 * @param   pDestCurtent    Destination buffer
 * @param   pSrcCurrent     Source buffer
 * @param   bInterleaveDest   true if the values should be written to the destination interleaved
 */
 void V3DFile::CopyBufferInterleaved( uint1 * & pDestCurrent, uint1 * &  pSrcCurrent, bool bInterleaveDest ) const
{
  if( bInterleaveDest )
  {
    uint1 * pOffset = pDestCurrent;
    
    for( int4 i( 0 ); i < 4096; ++i )
    {
      *pDestCurrent = *pSrcCurrent++;
      pDestCurrent += 2;
    }

    pDestCurrent = pOffset + 1;
    for( int4 j( 0 ); j < 4096; ++j )
    {
      *pDestCurrent = *pSrcCurrent++;
      pDestCurrent += 2;
    }
   }
   else
   {
     uint1 * pOffset = pSrcCurrent;
     for( int i(0); i<4096; ++i )
     {
       *pDestCurrent++ = * pSrcCurrent;
       pSrcCurrent += 2;
     }
     
     uint1 * pSrcCurrent2 = ( uint1 * ) ( pOffset + 1 );
     for( int j(0); j<4096; ++j )
     {
       * pDestCurrent++ = * pSrcCurrent2;
       pSrcCurrent2 += 2;
     }
   }
 } // CopyBufferInterleaved()


/**
 * Write the end chunk to disk
 *
 * @param outStream the output file
 */
void V3DFile::WriteEndChunk( std::shared_ptr<io::FilePtr> & outStream )
{
  WriteChunk( outStream, 0, "IEND", NULL, NONE );
} // WriteEndChunk()


/** 
 * Read the end chunk from disk
 *
 * @param inStream   the input file
 * @param uChunkSize the chunk size
 */
void V3DFile::ReadEndChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize )
{
  if ( uChunkSize != 0 )
    throw ex::DataCorruptionException( LogRec( "Bad IEND data length", "V3DFile", "ReadEndChunk" ) );

  MemoryBuffer memBuf;
  ReadChunk( inStream, "IEND", memBuf );

} // ReadEndChunk()


/**
 * Write a chunk to disk
 *
 * @param outStream     the output file
 * @param uBufferLength the buffer length
 * @param sChunkType    the type of chunk
 * @param pBuffer       the output buffer
 * @param eCompression  the compression type
 */
void V3DFile::WriteChunk( std::shared_ptr<io::FilePtr> & outStream, const uint4 uBufferLength, const std::string & sChunkType, uint1 * pBuffer,
                          const CompressionTypeEnum eCompression ) const
{
  // check the type length
  const uint4 uTypeLength( 4 );
  if ( sChunkType.size() != uTypeLength )
    throw ex::IllegalArgumentException( LogRec( "Bad chunk type string.", "V3DFile", "WriteChunk" ) );

  string sMyChunkType( sChunkType );

  // compress the data first
  uint4 uNewBufferLength( uBufferLength );
  uint1 * pNewBuffer = pBuffer;
  if ( eCompression != NONE )
  {
    uNewBufferLength += 20;
    pNewBuffer = new uint1[ uNewBufferLength ];

    // copy the uncompressed size into the buffer
    if ( eCompression == LZO )
      * ( ( uint4 * ) pNewBuffer ) = uBufferLength;

    Deflate deflater;
    int4 result;
    switch ( eCompression )
    {
    case LZO:
      result = deflater.DeflateIt( hdr::LZO,  pBuffer, uBufferLength, pNewBuffer+4, uNewBufferLength );
      break;
    case ZLIB:
      result = deflater.DeflateIt( hdr::ZLIB, pBuffer, uBufferLength, pNewBuffer, uNewBufferLength );
      break;
    default:
      throw ex::RuntimeException( LogRec( "Unhandled compression method.", "V3DFile", "WriteChunk" ) );
      break;
    }
    if ( result < 0 )
      throw ex::RuntimeException( LogRec( "Unable to deflate chunk data.", "V3DFile", "WriteChunk" ) );

    if ( eCompression == LZO )
    {
      if ( uNewBufferLength > uBufferLength )
      {
        // compression was unsuccessful in saving space, revert to uncompressed chunk
        sMyChunkType[3] = 'n';
        memcpy( pNewBuffer, pBuffer, uBufferLength );
        uNewBufferLength = uBufferLength;
      }
      else
      {
        // compression was successful
        uNewBufferLength += 4;
      }
    }
  }

  // write the data length
  WriteUintRaw( outStream, uNewBufferLength );

  // write the chunk type text
  for ( uint1 i(0); i<uTypeLength; ++i )
  {
    int1 c( sMyChunkType[i] );
    outStream->Write( & c, 1 );
  }

  // write the data
  if ( uNewBufferLength > 0 )
    outStream->Write( ( char * ) pNewBuffer, uNewBufferLength );

  // write the checksum
  CRC crc;
  uint4 uChecksum( crc.ComputeChecksum( sMyChunkType, pNewBuffer, uNewBufferLength ) );
  WriteUintRaw( outStream, uChecksum );

} // WriteChunk()


/**
 * Write the data chunk to disk
 *
 * @param outStream             the output file
 * @param eDataChunkCompression the compression type
 */
void V3DFile::WriteDataChunk( std::shared_ptr<io::FilePtr> & outStream, CompressionTypeEnum eDataChunkCompression )
{
  if ( m_pDataBuffer->GetSize() == 0 )
    m_pDataBuffer->Resize(1);

  switch ( eDataChunkCompression )
  {
  case LZO_BLOCKS:
    WriteChunkInBlocks( outStream, ( V3DFile::MemBlockInfo * ) m_pDataBuffer->GetDataPtr(), m_pDataBuffer->GetSize() / sizeof( V3DFile::MemBlockInfo ), "voLc", eDataChunkCompression );
    break;
  case LZO_BLOCKS_PLUS_BLEND_OUT_OF_HIGHER_FOUR_BIT_OF_16_BIT_DATA:
    WriteChunkInBlocks( outStream, ( V3DFile::MemBlockInfo * ) m_pDataBuffer->GetDataPtr(), m_pDataBuffer->GetSize() / sizeof( V3DFile::MemBlockInfo ), "voLb", eDataChunkCompression );
    break;
  case NONE_BLOCKS:    
    break;
  case LZO:
  default:
    WriteChunk( outStream, m_pDataBuffer->GetSize(), "voLl", m_pDataBuffer->GetDataPtr(), LZO );
  }
} // WriteDataChunk()


/**
 * Write a chunk in blocks
 *
 * @param outStream             the output file
 * @param pMemBlockInfo         the memory block information structure
 * @param uNumMemBlocks         the number of memory blocks
 * @param sChunkType            the type of chunk to write
 * @param eDataChunkCompression the compression type
 */
void V3DFile::WriteChunkInBlocks( std::shared_ptr<io::FilePtr> & outStream, MemBlockInfo * pMemBlockInfo, const uint4 uNumMemBlocks, 
                                  const std::string & sChunkType, const CompressionTypeEnum eCompression ) const
{

  // write the maximum block size in bytes
  switch ( eCompression )
  {
  case NONE_BLOCKS:
    break;
  case LZO_BLOCKS:
    {
      const uint4 uTypeLength( 4 );

      // write the data length -- makes no sense here, but needed.
      WriteUintRaw( outStream, uTypeLength );

      // write the chunk type text
      for ( uint1 i(0); i<uTypeLength; ++i )
      {
        int1 c( sChunkType[i] );
        outStream->Write( & c, 1 );
      }
      const uint4 uMaxBlockByteSize( pMemBlockInfo[0].uMemBlockByteSize );
      outStream->Write( & uMaxBlockByteSize, sizeof( uint4 ) );
      
      MemoryBuffer compressedMemBlock( static_cast< uint4 >(( uMaxBlockByteSize + 4 ) * 32 * 1.1 ));
      
      CRC crc;
      unsigned long lCheckSum( crc.UpdateCrc( crc.CrcInit(), ( unsigned char * ) sChunkType.c_str(), 4) );
      
      Deflate deflater;
      
      uint4 uCompressedBlockSize;
      deflater.DeflateIt( hdr::LZO, pMemBlockInfo[1].pMemBlock, pMemBlockInfo[1].uMemBlockByteSize, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      outStream->Write( & uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
      outStream->Write( compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      outStream->Write( & ( pMemBlockInfo[1].iFlag ), sizeof( int4 ) );
      lCheckSum = crc.UpdateCrc( lCheckSum, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      
      // 4 for flags
      int4 iBlockByteSize( 4096 * pMemBlockInfo[2].uBytesPerEntryInBlock + 4 );
      const int4 iNumberOfBlocksInOneChunk( 32 );
      uint1 * pTmpBuf = new uint1[ iBlockByteSize * iNumberOfBlocksInOneChunk ];
      
      // This is the start of the actual data
      int4 k( 2 );
      while( static_cast< uint4 >(( k + iNumberOfBlocksInOneChunk )) < uNumMemBlocks )
      {
        uint1 * pDestCurrent = pTmpBuf;
        for( int4 g(0); g<iNumberOfBlocksInOneChunk; ++g,++k )
        {          
          uint1 * pSrcCurrent = ( unsigned char * ) pMemBlockInfo[k].pMemBlock;
          if( pMemBlockInfo[k].uBytesPerEntryInBlock == 2 )
            CopyBufferInterleaved( pDestCurrent, pSrcCurrent, false );
          else
          {
            const uint4 uSizeInBytes = 4096 * pMemBlockInfo[k].uBytesPerEntryInBlock;
            memcpy( pDestCurrent, pSrcCurrent, uSizeInBytes ); 
            pDestCurrent += uSizeInBytes;
          }
          
          int4 * pDum = ( int4 * ) pDestCurrent;
          *pDum = pMemBlockInfo[ k ].iFlag;
          pDestCurrent += 4;
        }        
        uint4 uCompressedBlockSize;
        deflater.DeflateIt( hdr::LZO, pTmpBuf, iBlockByteSize * iNumberOfBlocksInOneChunk, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        outStream->Write( & uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
        outStream->Write( compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        lCheckSum = crc.UpdateCrc( lCheckSum, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      }
      for ( ; static_cast<uint4>(k)<uNumMemBlocks; ++k )
      {
        uint4 uCompressedBlockSize;
        
        unsigned char * pSrcCurrent = ( unsigned char * ) pMemBlockInfo[k].pMemBlock;
        unsigned char * pDestCurrent = pTmpBuf;

        if( pMemBlockInfo[k].uBytesPerEntryInBlock == 2 )
          CopyBufferInterleaved( pDestCurrent, pSrcCurrent, false );
         else
           memcpy( pDestCurrent, pSrcCurrent, 4096 * pMemBlockInfo[k].uBytesPerEntryInBlock ); 
        
        deflater.DeflateIt( hdr::LZO, pTmpBuf, iBlockByteSize, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        outStream->Write( & uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
        outStream->Write( compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        outStream->Write( & ( pMemBlockInfo[k].iFlag ), sizeof( int4 ) );
        lCheckSum = crc.UpdateCrc( lCheckSum, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      } // finished writing all blocks
      
      lCheckSum = crc.CrcComplement( lCheckSum );
      outStream->Write( & lCheckSum, sizeof( lCheckSum ) );
    }
    break;
  case LZO_BLOCKS_PLUS_BLEND_OUT_OF_HIGHER_FOUR_BIT_OF_16_BIT_DATA:
    {
      const uint4 uTypeLength( 4 );

      // write the data length -- makes no sense here, but needed.
      WriteUintRaw( outStream, uTypeLength );

      // write the chunk type text
      for ( uint1 i(0); i<uTypeLength; ++i )
      {
        int1 c( sChunkType[i] );
        outStream->Write( & c, 1 );
      }
      const uint4 uMaxBlockByteSize( pMemBlockInfo[0].uMemBlockByteSize );
      outStream->Write( & uMaxBlockByteSize, sizeof( uint4 ) );
      
      MemoryBuffer compressedMemBlock( static_cast< uint4 >(( uMaxBlockByteSize + 4 ) * 32 * 1.1 ));
      
      CRC crc;
      unsigned long lCheckSum( crc.UpdateCrc( crc.CrcInit(), ( unsigned char * ) sChunkType.c_str(), 4 ) );
      
      Deflate deflater;
      
      uint4 uCompressedBlockSize;
      deflater.DeflateIt( hdr::LZO,  pMemBlockInfo[1].pMemBlock, pMemBlockInfo[1].uMemBlockByteSize, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      outStream->Write( & uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
      outStream->Write( compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      outStream->Write( & ( pMemBlockInfo[1].iFlag ), sizeof( int4 ) );
      lCheckSum = crc.UpdateCrc( lCheckSum, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      
      // 4 for flags
      int4 iBlockByteSize( 3072 * 2 + 4);
      const int4 iNumberOfBlocksInOneChunk( 32 );
      unsigned char * pTmpBuf = new unsigned char[ iBlockByteSize * iNumberOfBlocksInOneChunk ];
      
      // This is the start of the actual data
      int4 k( 2 );
      ProgressBarUpdater progressUpdater;
      // need to update progress meter within this loop
      while( static_cast<uint4>(( k + iNumberOfBlocksInOneChunk )) < uNumMemBlocks )
      {
        unsigned char * pDestCurrent = pTmpBuf;
        for( int4 g(0); g<iNumberOfBlocksInOneChunk; ++g,++k )
        {
          
          unsigned char * pSrcCurrent = ( unsigned char * ) pMemBlockInfo[k].pMemBlock;
          int4  i;
          for( i=0; i<4096; ++i )
          {
            * pDestCurrent++ = * pSrcCurrent;
            pSrcCurrent += 2;
          }
          int2 * pSrcCurrent2 = ( int2 * ) pMemBlockInfo[k].pMemBlock;
          for( i=0; i<2048; ++i )
          {
            * pDestCurrent  = ( ( * pSrcCurrent2++ ) & 0x0F00 ) >> 4;
            * pDestCurrent |= ( ( * pSrcCurrent2++ ) & 0x0F00 ) >> 8;
            pDestCurrent++;
          }
          int4 * pDum = ( int4 * ) pDestCurrent;
          * pDum = pMemBlockInfo[k].iFlag;
          pDestCurrent += 4;
        }        
        uint4 uCompressedBlockSize;
        deflater.DeflateIt( hdr::LZO, pTmpBuf, iBlockByteSize * iNumberOfBlocksInOneChunk, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        outStream->Write( & uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
        outStream->Write( compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        lCheckSum = crc.UpdateCrc( lCheckSum, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        progressUpdater.Update( float4( k + iNumberOfBlocksInOneChunk + 1 ) / float( uNumMemBlocks ) * 100.0F );
      }
      for ( ; static_cast<uint4>(k)<uNumMemBlocks; ++k )
      {
        uint4 uCompressedBlockSize;
        
        unsigned char * pDestCurrent = pTmpBuf;
        unsigned char * pSrcCurrent = ( unsigned char * ) pMemBlockInfo[k].pMemBlock;
        int4  i;
        for( i=0; i<4096; ++i )
        {
          * pDestCurrent++ = * pSrcCurrent;
          pSrcCurrent += 2;
        }
        int2 * pSrcCurrent2 = ( int2 * ) pMemBlockInfo[k].pMemBlock;
        for( i=0; i<2048; ++i )
        {
          * pDestCurrent  = ( ( * pSrcCurrent2++ ) & 0x0F00 ) >> 4;
          * pDestCurrent |= ( ( * pSrcCurrent2++ ) & 0x0F00 ) >> 8;
          pDestCurrent++;
        }
        
        deflater.DeflateIt( hdr::LZO, pTmpBuf, iBlockByteSize, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        outStream->Write( & uCompressedBlockSize, sizeof( uCompressedBlockSize ) );
        outStream->Write( compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
        outStream->Write( & ( pMemBlockInfo[k].iFlag ), sizeof( int4 ) );
        lCheckSum = crc.UpdateCrc( lCheckSum, compressedMemBlock.GetDataPtr(), uCompressedBlockSize );
      } // finished writing all blocks
      
      lCheckSum = crc.CrcComplement( lCheckSum );
      outStream->Write( & lCheckSum, sizeof( lCheckSum ) );
    }
    break;
  }
} // WriteChunkInBlocks()


/**
 * Read the chunk type from disk
 *
 * @param inStream the input file
 * @return the type of chunk
 */
std::string V3DFile::ReadChunkType( std::shared_ptr<io::FilePtr> & inStream ) const
{
  char chunkType[5];

  RtlZeroMemory(chunkType,5);

  inStream->Read( chunkType, 4 );

 /* const uint4 uTypeLength( 4 );
  string sChunkType( uTypeLength, 0 );
  for ( uint1 i(0); i<uTypeLength; ++i )
  {
    char c;
    inStream->Read( & c, 1 );
    sChunkType[i] = c;
  }*/
 
  return std::string(chunkType);
} // ReadChunkType()


/**
 * Verify the checksum from disk
 *
 * @param inStream     the input file
 * @param sChunkType   the type of chunk
 * @param memoryBuffer the input memory buffer
 */
void V3DFile::VerifyChecksum( std::shared_ptr<io::FilePtr> & inStream, const std::string & sChunkType, MemoryBuffer & memoryBuffer ) const
{
  CRC crc;

  // calculate the checksum from memory
  uint4 uMemoryChecksum( crc.ComputeChecksum( sChunkType, memoryBuffer.GetDataPtr(), memoryBuffer.GetSize() ) );

  // read the checksum from disk
  uint4 uDiskChecksum( ReadUintRaw( inStream ) );

  // test the checksum
  if ( uMemoryChecksum != uDiskChecksum )
    throw ex::DataCorruptionException( LogRec( "Invalid checksum in chunk.", "V3DFile", "VerifyChecksum" ) );

} // VerifyChecksum()


/**
 * Read a chunk from disk
 *
 * @param inStream   the input file
 * @param sChunkType the type of chunk
 * @param memBuf     the memory buffer
 */
void V3DFile::ReadChunk( std::shared_ptr<io::FilePtr> & inStream, const string & sChunkType, MemoryBuffer & memBuf )
{

  // read the actual data
  if ( memBuf.GetSize() > 0 )
    inStream->Read( ( char * ) memBuf.GetDataPtr(), memBuf.GetSize() );

  // read the checksum
  CRC crc;
  uint4 uChecksum( crc.ComputeChecksum( sChunkType, memBuf.GetDataPtr(), memBuf.GetSize() ) );
  uint4 uDiskChecksum( ReadUintRaw( inStream ) );

  // test the checksum
  if ( uChecksum != uDiskChecksum )
    throw ex::DataCorruptionException( LogRec( "Invalid checksum in chunk.", "V3DFile", "ReadChunk" ) );

} // ReadChunk()


/**
 * Skip over a chunk
 *
 * @param inStream   the input file
 * @param uChunkSize the chunk size
 */
void V3DFile::SkipChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize ) const
{

  // add four bytes for the checksum
  inStream->SeekCurrent( uChunkSize + 4 );

} // SkipChunk()


/**
 * copy a chunk from stream to stream
 *
 * @param inStream   the input file
 * @param outStream  the output file
 * @param uChunksize the chunk size
 * @param sChunkType the type of chunk
 */
void V3DFile::CopyChunk( std::shared_ptr<io::FilePtr> & inStream, std::shared_ptr<io::FilePtr> & outStream, const uint4 uChunkSize, const string & sChunkType )
{

  MemoryBuffer memBuf( uChunkSize );

  // read the actual data
  if ( memBuf.GetSize() > 0 )
    inStream->Read( ( char * ) memBuf.GetDataPtr(), memBuf.GetSize() );

  // read the checksum
  CRC crc;
  uint4 uChecksum( crc.ComputeChecksum( sChunkType, memBuf.GetDataPtr(), memBuf.GetSize() ) );
  uint4 uDiskChecksum( ReadUintRaw( inStream ) );

  // test the checksum
  if ( uChecksum != uDiskChecksum )
    throw ex::DataCorruptionException( LogRec( "Invalid checksum in chunk.", "V3DFile", "CopyChunk" ) );

  // write the data length
  WriteUintRaw( outStream, uChunkSize );

  // write the chunk type text
  for ( uint1 i(0); i<sChunkType.size(); ++i )
  {
    int1 c( sChunkType[i] );
    outStream->Write( & c, 1 );
  }

  // write the data
  if ( uChunkSize > 0 )
    outStream->Write( ( char * ) memBuf.GetDataPtr(), uChunkSize );

  // write the checksum
  WriteUintRaw( outStream, uChecksum );

} // ReadChunk()


/** 
 * Read and verify a chunk
 *
 * @param inStream          the input file
 * @param sChunkType        the chunk type
 * @param uDiskBufferLength the length of the disk buffer
 * @param memBuffer         the memory buffer
 */
void V3DFile::ReadAndVerifyData( std::shared_ptr<io::FilePtr> & inStream, const std::string & sChunkType, const uint4 uDiskBufferLength, MemoryBuffer & memBuffer )
{
  memBuffer.Resize( uDiskBufferLength );
  inStream->Read( ( char * ) memBuffer.GetDataPtr(), memBuffer.GetSize() );
  VerifyChecksum( inStream, sChunkType, memBuffer );
} // ReadAndVerifyData()


/**
 * Inflate a memory buffer using LZO compression
 *
 * @param destBuffer the destination buffer
 * @param srcBuffer  the source buffer
 */
void V3DFile::InflateLZO( MemoryBuffer & destBuffer, MemoryBuffer & srcBuffer )
{

  // read the first four bytes as the uncompressed size
  destBuffer.Resize( * ( ( uint4 * ) srcBuffer.GetDataPtr() ) );
  Inflate::InflateIt( hdr::LZO, srcBuffer.GetDataPtr() + 4, srcBuffer.GetSize() - 4, destBuffer.GetDataPtr(), destBuffer.GetSize() );

} // InflateLZO()


/**
 * Inflate a memory buffer using ZLIB compression.  The dest buffer should already be the right size.
 *
 * @param destBuffer the destination buffer
 * @param srcBuffer  the source buffer
 */
void V3DFile::InflateZLIB( MemoryBuffer & destBuffer, MemoryBuffer & srcBuffer )
{

  // the dest buffer should already be the right size
  Inflate::InflateIt( hdr::ZLIB, srcBuffer.GetDataPtr(), srcBuffer.GetSize(), destBuffer.GetDataPtr(), destBuffer.GetSize() );

} // InflateZLIB()


/**
 * Pack a variable into a string
 *
 * @param variable the variable to pack
 * @return the string representation of it
 */
std::string V3DFile::PackVariable( const Variable & variable ) const
{

  std::string sString( variable.GetName() );
  sString += " ";
  sString[ sString.size() - 1 ] = 0;
  sString += variable.GetValue();
  return sString;

} // PackVariable()


/**
 * Unpack a variable from a string
 *
 * @param sString the packed string representation
 * @return the variable
 */
Variable V3DFile::UnpackVariable( const std::string & sString ) const
{

  Variable variable;
  std::string sNullString(1, 0);
  std::string::size_type position = sString.find( sNullString );
  std::string sName( sString.begin(), sString.begin() + position );
  std::string sValue( sString.begin() + position + 1, sString.end() );
  variable.SetName( sName );
  variable.SetValueAsString( sValue );
  return variable;

} // UnpackVariable()


/**
 * Set the RGB image
 *
 * @param image the input RGB image
 */
void V3DFile::SetImage( const Image< PixelRGB<uint1> > & image )
{
  m_image = image;
  m_bAlphaImage = false;
} // SetImage()


/**
 * Set the RGBA image
 *
 * @param image the input RGBa image
 */
void V3DFile::SetAlphaImage( const Image< PixelRGBA<uint1> > & image )
{
  m_imageRGBA = image;
  m_bAlphaImage = true;
} // SetAlphaImage()


/**
 * Returns an extra chunk (one that was not recognized as text, image, or data)
 *
 * @param sChunkName the name of the chunk to retrieve
 * @param destBuffer the destination buffer (must be resizable to hold uncompressed chunk data)
 *
 * @return if found
 */
bool V3DFile::GetExtraChunk( const std::string & sChunkName, MemoryBuffer & destBuffer )
{

  for ( std::list< std::pair< std::string, MemoryBuffer > >::iterator iter = m_extraChunks.begin();
        iter != m_extraChunks.end();
        ++iter )
  {
    const std::string sName = iter->first;
    if ( sName == sChunkName )
    {
      destBuffer = iter->second;
      return true;
    }
  }

  return false;

} // GetExtraChunk()


/**
 * Adds an extra chunk (one that is not text, image, or the data chunk)
 *
 * @param sChunkName   the name of the chunk
 * @param sourceBuffer the chunk data
 */
void V3DFile::SetExtraChunk( const std::string & sChunkName, const MemoryBuffer & sourceBuffer )
{

  // see if it already exists
  for ( std::list< std::pair< std::string, MemoryBuffer > >::iterator iter = m_extraChunks.begin();
        iter != m_extraChunks.end();
        ++iter )
  {
    const std::string sName = iter->first;
    if ( sName == sChunkName )
    {
      iter->second = sourceBuffer;
      return;
    }
  }

  // add it since it does not already exist
  m_extraChunks.push_back( make_pair( sChunkName, sourceBuffer ) );

} // SetExtraChunk()


/**
 * Get a variable from the variable list
 *
 * @param sName the desired variable name
 */
const Variable & V3DFile::GetVariable( const std::string & sName ) const
{
  for ( std::list<Variable>::const_iterator it( m_varlist.begin() ); it != m_varlist.end(); ++it )
  {
    if ( ( * it ).GetName() == sName )
      return * it;
  }

  throw ex::IllegalArgumentException( LogRec( "no such variable", "V3DFile", "GetVariable" ) );

} // GetVariable()


/**
 * Sets a variable
 *
 * @param variable the variable to set
 */
void V3DFile::SetVariable( const Variable & variable )
{

  m_varlist.push_back( variable );

} // SetVariable()


/**
 * Write a unit4 to a file in raw format
 *
 * @param outStream the output stream
 * @param value     the value to write
 */
void V3DFile::WriteUintRaw( std::shared_ptr<io::FilePtr> & outStream, const uint4 value ) const
{

  uint4 uSwappedValue( value );
  SwapUint4ByteOrder( uSwappedValue );

  outStream->Write( (void *) & uSwappedValue, sizeof( uint4 ) );

} // WriteUintRaw()



/**
 * Read a uint4 from a file in raw format
 *
 * @param inStream the input stream
 * @return         the read value
 */
uint4 V3DFile::ReadUintRaw( std::shared_ptr<io::FilePtr> & inStream ) const
{

  // read the raw value
  uint4 value;
  inStream->Read( (void *) & value, sizeof( uint4 ) );

  // swap the byte order
  SwapUint4ByteOrder( value );

  return value;

} // ReadUintRaw()


/**
 * Swap the byte order of a unit4
 *
 * @param & value
 */
void V3DFile::SwapUint4ByteOrder( uint4 & uValue ) const
{
  uint1 * pBuffer = (uint1 *) (& uValue);
  std::swap(pBuffer[0], pBuffer[3]);
  std::swap(pBuffer[1], pBuffer[2]);
} // SwapUint4ByteOrder()


/**
 * Initialize the icon to the new Viatronix "V".
 */
void V3DFile::InitIcon()
{
  PixelRGB<uint1> clearPixel( 0, 0, 0 );
  const uint4 uWidth( 88 );
  const uint4 uHeight( 120 );
  m_image.Resize( Triple<uint4>( uWidth, uHeight, 0 ) );
  const uint4 uDataSize( 3 * uWidth * uHeight );
  static const int4 vData[ uDataSize ] =
  {
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,
    254,254,225,224,224,221,220,220,238,237,237,250,250,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,252,252, 90, 86, 87, 34, 30, 31,
    48, 44, 45, 76, 72, 73,117,114,114,171,169,170,238,237,237,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,255,255, 84, 81, 81,  9,  5,  6, 22, 18, 19, 23, 19, 20, 20, 16,
    17, 25, 21, 22, 58, 54, 55,155,153,154,249,249,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,164,162,162,110,108,108, 94, 91, 92, 59, 55, 56, 34, 30, 31, 23, 18, 19, 25, 21, 22, 22,
    17, 18, 97, 94, 95,240,240,240,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,242,242,242,198,197,197,125,122,123, 41, 37, 37, 26, 23, 24, 20, 17, 17, 99, 96, 97,
    249,249,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,204,203,203, 61, 57, 57, 26, 22, 23, 21, 17, 18,156,154,155,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,200,199,199, 37, 32, 33, 24, 21, 21, 66, 63, 63,247,246,246,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,137,134,135,142,140,140,220,219,219,254,254,254,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,129,127,128, 22, 18, 19, 33, 29, 30,200,198,198,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,233,232,232, 42, 37, 39, 11,  7,  8, 87, 84, 85,251,251,251,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,200,199,199, 34,
    29, 30, 21, 17, 18,158,156,156,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,186,
    184,185, 30, 26, 27, 19, 15, 15,134,132,133,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,226,225,225, 44, 39, 40, 17, 13, 14,136,134,134,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,147,145,145, 20, 15, 16, 29, 25, 26,
    182,180,181,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,227,226,226, 44, 40, 41, 17, 13, 14,134,132,132,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,115,112,113, 19, 15, 16, 41, 37, 38,220,219,219,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,208,207,207, 38, 34, 35, 19, 15, 16,151,149,150,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,254,254,254, 93, 90, 90, 21, 17, 18, 51, 48, 48,242,242,242,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,169,167,
    167, 24, 20, 21, 29, 25, 26,182,180,181,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,251,
    251, 82, 78, 79, 21, 17, 18, 63, 59, 60,246,246,246,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,102, 99,100, 21, 17, 18, 43, 38, 39,225,
    224,224,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,251,251, 81, 77, 78, 21, 17, 18, 63,
    59, 60,247,246,246,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,210,210,210, 36, 32, 33, 23, 18, 19, 91, 88, 88,254,254,254,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,254, 94, 91, 91, 21, 17, 18, 51, 47, 48,241,240,240,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,113,111,111, 22, 18, 19, 25, 21, 22,171,169,170,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,118,115,116, 19, 15, 16, 38, 34, 35,211,210,210,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,212,211,211, 37, 33, 34,
    25, 21, 22, 60, 56, 57,242,241,241,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,161,159,159, 23, 19, 20, 24, 19, 20,151,149,149,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,250,250,250, 91, 88, 88, 24, 20, 21, 23, 19, 20,158,156,156,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,224,224,224, 44, 40,
    41, 25, 21, 22, 61, 58, 58,238,237,238,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,254,254,254,137,135,135, 22, 18, 19, 23, 19, 20, 81, 78, 79,248,248,248,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,111,108,109, 20, 16, 17, 24, 20, 21,113,
    109,110,243,243,243,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,
    254,147,144,144, 20, 16, 17, 41, 37, 38,210,209,210,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,222,221,221, 56, 52, 53, 20, 16, 17, 33, 29, 30,210,208,208,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,157,155,156,172,
    170,170,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,208,206,207, 51, 47, 48,141,138,139,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,229,228,228,249,248,248,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,223,222,222,243,243,242,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,254,254,254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,250,251,250, 83, 80, 80,
    78, 75, 75,205,204,204,254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,253,254,251,
    251,253,253,252,254,254,254,255,254,254,255,254,254,255,252,252,254,255,255,255,192,190,191, 30, 26, 27, 14, 10, 11,100, 97, 97,253,253,
    253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,248,247,252,248,247,251,
    249,248,252,248,247,251,245,246,251,254,254,255,133,130,131, 19, 15, 16, 30, 26, 27,183,181,181,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,232,231,231,167,165,165,222,221,222,245,245,248,239,239,248,237,238,247,234,234,246,231,233,
    246,236,237,250,112,109,111, 20, 16, 16, 42, 39, 40,230,229,229,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    229,229,229, 89, 86, 87, 19, 15, 16, 36, 32, 31,169,168,171,219,221,245,204,205,232,200,203,230,204,211,235,215,222,247,119,118,125, 22,
    18, 17, 31, 27, 28,135,132,133,243,242,242,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,240,239,246,220,220,237,222,222,
    238,222,222,238,222,222,238,222,222,238,222,222,238,220,220,237,218,218,236,218,219,236,218,219,236,218,219,236,217,217,235,228,228,241,
    254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,242,241,241,161,159,159, 60, 56, 57, 24, 20, 21, 25, 20,
    20, 64, 60, 61,172,178,207,176,185,225,165,175,217,151,159,208,150,160,208,158,169,218,142,144,177, 43, 39, 37, 29, 25, 25, 27, 22, 23,
    69, 65, 66,157,155,155,226,226,226,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,241,241,246,199,200,226,202,202,
    228,202,202,227,202,202,227,202,202,227,202,202,227,203,203,228,206,206,229,206,206,229,206,206,230,206,206,229,205,205,229,208,208,231,
    246,246,251,255,255,255,255,255,255,255,255,255,255,255,255,244,244,247,157,158,203,123,125,188,127,129,191,129,131,193,129,131,193,129,
    130,193,128,130,193,127,129,192,126,129,192,126,129,191,126,129,192,126,128,191,122,124,189,149,151,202,252,252,253,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,244,244,244,173,171,172, 69, 66, 66, 24, 19, 21, 26, 22, 23, 26, 20, 20, 60, 54, 57,151,159,186,158,178,224,140,
    157,207,128,143,199,110,123,188,117,135,195,126,150,204,128,144,204, 92, 94,126, 37, 32, 32, 29, 23, 21, 26, 21, 22, 24, 20, 21, 46, 42,
    43,110,107,108,171,169,169,218,217,217,248,248,248,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,253,210,210,219,111,113,175,118,121,189,119,121,188,119,121,189,119,
    121,189,119,121,188,119,122,189,120,122,189,120,122,189,120,122,189,119,122,189,117,120,186,117,119,185,184,185,220,254,254,255,255,255,
    255,255,255,255,249,249,245,173,174,197,108,111,178,114,117,184,108,111,176, 93, 96,161, 92, 95,160, 94, 97,161, 94, 97,162, 95, 98,163,
    96, 99,164, 97,100,165, 98,101,167, 98,100,166, 92, 95,163,130,132,184,253,253,253,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,254,254,254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,223,222,222,143,140,141, 68, 64,
    65, 26, 22, 23, 24, 20, 21, 26, 20, 20, 41, 36, 35,103,107,116,157,175,208,134,158,213,118,145,201,115,142,199,114,141,200, 81,104,177,
    67, 90,168, 92,122,188, 69, 92,171, 94,114,187,114,130,171, 71, 75, 94, 39, 35, 37, 29, 24, 21, 28, 22, 21, 22, 17, 18, 27, 23, 24, 40,
    36, 37, 69, 65, 66,122,120,120,178,176,177,230,229,229,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,250,250,249,239,238,236,198,198,204, 87, 89,144, 91, 95,159, 92, 95,158, 93, 96,158, 93, 95,157, 93, 95,157, 94, 96,157,
    93, 95,156, 92, 95,155, 91, 94,153, 97, 99,159,111,115,179,113,117,184,121,123,184,232,233,242,255,255,255,245,245,245,226,226,223,170,
    170,180, 94, 96,153,116,119,188,117,120,172,136,136,156,134,134,153,130,131,152,129,130,151,127,127,150,125,125,149,125,126,149,123,123,
    148,128,128,152,129,129,155,174,175,194,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,251,250,253,
    251,251,253,254,254,255,255,255,255,255,255,255,211,210,210,121,118,118, 46, 42, 43, 22, 18, 19, 26, 21, 22, 26, 20, 19, 45, 41, 41, 90,
    94,103,126,138,174,150,176,223,149,184,227,120,155,208,133,170,217,134,171,218,119,156,209, 74,107,179, 41, 68,156, 43, 70,158, 39, 67,
    155, 77,110,182,118,152,210,142,174,223,128,153,190, 85, 97,122, 52, 55, 69, 37, 33, 37, 29, 23, 22, 25, 19, 18, 26, 20, 21, 21, 17, 17,
    28, 24, 25, 48, 44, 45,106,103,104,183,181,181,247,247,246,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,240,239,238,225,
    225,224,208,208,209,143,143,159,138,138,156,139,139,157,142,142,159,144,144,161,145,144,160,149,149,163,149,148,163,151,151,164,155,154,
    166,140,140,161,110,113,174,110,114,182,125,127,174,245,245,248,253,253,253,234,233,233,223,223,222,210,210,208,108,110,147,107,110,178,
    133,136,194,221,221,227,239,238,233,233,233,230,232,232,229,232,231,229,230,229,227,230,229,227,231,231,227,250,249,244,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,255,255,255,255,255,255,255,233,233,
    233,116,113,114, 21, 16, 17,  8,  3,  4, 22, 18, 16, 31, 24, 22, 45, 42, 42, 90, 94,105,173,192,208,206,235,255,153,184,228,141,176,220,
    130,169,216, 97,134,195,111,150,205,118,157,210,112,152,206, 85,124,189, 49, 82,165, 35, 59,151, 50, 82,165, 86,124,190,105,143,201,116,
    155,209,109,146,206,103,137,204,117,153,207,109,135,182, 95,113,138, 79, 88, 99, 51, 51, 56, 36, 31, 31, 30, 23, 21, 24, 19, 17, 11,  6,
    6, 12,  7,  8, 57, 53, 54,164,162,163,239,239,239,255,255,255,255,255,255,255,255,255,243,242,239,233,232,229,232,231,228,241,241,235,
    240,240,234,240,239,235,240,239,235,237,237,234,237,237,234,236,236,233,236,236,233,238,236,234,236,235,229,172,173,201,113,117,185,100,
    104,169,178,179,202,255,255,255,251,251,251,238,237,237,228,227,226,225,225,222,156,156,171, 95, 98,159,117,120,187,183,183,215,248,247,
    243,242,241,241,241,241,240,241,240,240,241,240,240,246,245,242,232,231,236,217,218,235,229,229,242,235,235,245,217,218,236,219,220,237,
    218,218,236,232,232,243,229,230,242,218,219,236,216,216,235,216,217,235,234,234,245,228,229,241,203,202,204,133,131,132,147,144,142,146,
    143,143, 88, 90,106, 88,100,123,127,148,188,154,184,228,195,225,250,189,219,244,132,169,216,157,198,233,179,220,246,125,165,214,140,181,
    223,153,193,230,119,158,210,119,160,211, 90,127,192, 39, 66,155, 90,128,192,111,151,206,110,150,205,143,183,225,116,155,209,113,153,208,
    164,207,241,139,181,227,127,169,223,158,200,238,144,182,220, 94,120,161, 74, 87,112, 82, 85,102,129,127,135,150,147,145,141,139,137,139,
    136,137,207,206,206,248,248,251,212,213,234,222,223,238,226,226,235,206,206,223,210,210,226,209,209,225,219,219,230,224,224,234,231,230,
    237,250,249,247,247,247,245,244,244,243,245,245,245,240,239,237,212,211,213,131,134,188,111,114,183,111,114,168,236,236,240,255,255,255,
    254,254,255,245,244,244,232,231,231,225,224,224,210,209,210,104,105,149,109,112,180,141,143,198,245,245,250,255,255,254,254,254,254,254,
    254,254,255,255,255,215,216,235,130,133,191,110,113,182,154,157,204,183,185,219,108,111,180,116,119,184,109,112,181,171,173,212,160,162,
    207,110,114,182,115,118,184,113,116,183,188,189,221,221,221,238,255,255,255,255,255,255,236,240,255,165,186,229,141,175,224,159,199,239,
    97,132,198,125,161,212,150,186,225,147,184,225,110,148,204,132,171,217,147,186,226,103,142,201,134,174,219,157,196,232,131,170,217,154,
    194,231,119,160,211, 68,105,179,120,160,211,148,190,228,127,168,216,151,193,231,117,158,210, 97,136,197,130,171,218,113,153,207, 93,130,
    193,122,162,214,123,163,215, 97,135,201, 85,121,193,133,173,223,138,165,220,176,188,228,247,249,255,255,255,255,255,255,255,252,253,254,
    188,189,221,146,149,200,180,182,217,110,112,181,117,120,185,112,115,183,161,162,208,171,173,212,119,122,187,173,175,214,243,243,249,255,
    255,255,252,252,252,233,233,229,176,176,200,115,118,186,102,105,172,160,162,193,255,255,255,255,255,255,255,255,255,250,250,249,237,236,
    236,227,226,226,229,229,226,145,146,167, 97,100,164,116,119,187,205,205,230,255,255,255,255,255,255,255,255,255,251,250,253,135,138,195,
    108,111,181,110,113,181,153,156,204,182,184,218,106,110,179,115,118,183,108,111,180,170,172,212,159,161,207,109,113,181,114,117,183,109,
    112,180,193,195,225,255,255,255,255,255,255,220,221,238, 78,105,178, 88,130,194,120,160,211,144,183,224,144,181,223,169,205,236,189,222,
    245,191,223,246,149,186,225,171,206,236,182,214,241,157,192,229,140,177,221,154,190,228,115,153,207,117,156,208, 90,129,193,115,155,208,
    86,126,191,120,160,211,106,146,203,139,181,223,116,157,209,138,178,222,150,191,230,139,180,223,115,156,209,149,190,230,145,187,228,124,
    164,214,112,152,207,126,168,217, 98,137,198, 59, 91,171, 62, 83,165,185,190,221,255,255,255,255,255,255,255,255,255,212,213,234,176,178,
    215,106,110,179,115,118,183,109,113,181,159,161,207,170,172,212,106,109,180,103,106,178,166,168,210,255,255,255,245,244,243,217,217,215,
    140,142,190,112,115,184,102,104,161,223,223,232,255,255,255,255,255,255,255,255,255,254,254,253,242,241,241,231,230,230,227,226,225,196,
    195,200, 95, 96,147,111,114,183,157,159,206,254,254,255,255,255,255,255,255,255,230,231,242,116,119,184,112,116,183,109,112,181,153,156,
    203,182,184,218,106,109,179,114,117,183,107,110,180,170,172,211,159,161,206,108,112,180,113,116,182,113,116,183,217,217,236,255,255,255,
    255,254,255,132,148,200, 44, 75,162,154,191,229,126,163,212,134,170,216,149,184,224,160,193,228,185,217,242,187,217,242,145,181,222,175,
    207,236,198,228,247,188,219,243,158,192,229,203,233,251,157,192,228,143,180,222,105,142,200,122,161,211,102,138,198,136,174,218,145,185,
    225,183,224,248,141,181,223,166,208,239,172,214,243,153,194,231,120,161,212,156,197,234,151,194,232,120,162,213,128,170,218, 98,137,198,
    90,128,192,111,150,206, 33, 56,146, 44, 70,157,155,168,211,255,255,255,255,255,255,255,255,255,183,184,218,104,107,178,114,117,183,109,
    112,181,159,161,206,170,172,212,107,110,180,110,113,181,135,138,194,250,250,252,237,236,232,191,191,203,119,122,186,104,107,174,149,151,
    185,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,249,248,248,237,236,235,226,225,225,226,226,223,127,127,154, 97,100,165,
    123,126,190,222,223,239,255,255,255,255,255,255,221,222,238,124,127,189,124,127,188,120,123,187,160,163,207,188,189,220,117,120,185,125,
    128,189,118,121,186,176,178,215,166,168,210,119,123,186,120,123,186,148,150,200,252,253,253,255,255,255,221,223,239, 72,103,177, 82,115,
    184,129,167,216,179,210,238,203,229,248,134,168,215,125,160,210,164,194,229,164,195,229,113,147,203,147,180,221,160,193,228,157,190,228,
    128,164,212,168,200,232,159,193,229,151,186,225,129,166,214,141,177,220,127,164,212,150,186,225,153,191,228,157,195,231,120,159,211,140,
    180,223,135,176,221,121,162,212, 94,132,194,129,170,217,125,167,216, 91,130,194, 98,138,198,143,185,227,128,170,217,101,141,201, 61, 86,
    162, 44, 70,156, 55, 86,168,167,178,215,255,255,255,255,255,255,197,198,225,117,120,185,124,127,188,120,123,186,166,168,210,176,178,215,
    118,121,186,119,121,186,161,163,208,248,248,247,222,222,218,149,151,192,112,115,185,105,107,166,221,221,230,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,253,253,252,242,241,240,230,230,229,227,226,225,192,192,196, 93, 95,147,111,114,184,173,174,214,255,255,
    255,255,255,255,248,249,252,206,207,231,201,202,227,201,202,227,217,218,236,228,229,241,199,201,227,203,204,228,200,201,227,223,224,239,
    219,220,237,200,202,227,199,200,227,229,229,241,255,255,255,255,255,255,178,190,222, 52, 86,167, 59, 85,164,156,188,226,166,197,231,182,
    208,236,161,191,228,174,201,232,226,242,252,218,236,249,129,162,211,187,212,237,207,227,245,204,225,244,152,183,222,164,195,229,159,189,
    226,118,155,207, 99,138,198,118,154,207,100,138,198,116,154,207,145,180,222,142,178,221,131,168,215,166,205,237,164,205,237,140,180,223,
    100,140,199,160,201,235,158,199,234,120,161,212,111,152,207,140,183,225,121,162,213, 95,132,195, 49, 73,156, 46, 72,158, 67, 99,176, 93,
    122,188,238,243,249,255,255,255,244,244,249,203,205,229,202,203,228,201,202,228,219,220,237,223,224,239,200,201,227,199,200,227,236,236,
    245,242,241,237,192,192,205,119,122,186,105,109,177,140,142,181,255,255,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,247,247,246,235,234,233,225,224,223,226,225,222,126,126,156,102,105,172,131,134,194,232,232,244,255,255,255,255,255,255,186,
    188,220,115,118,183,118,121,185,159,162,206,186,188,220,115,118,184,123,126,187,116,119,185,175,177,214,165,166,209,115,119,184,123,125,
    188,226,226,240,255,255,255,250,249,252,125,149,203, 80,119,187, 41, 67,155,171,194,227,203,224,244,126,161,211,169,195,229,169,195,228,
    226,240,251,214,230,246,144,176,219,194,216,239,232,245,254,232,245,254,186,210,236,200,222,243,215,234,248,163,194,229,114,150,204, 83,
    124,191,115,151,205,156,189,227,194,224,246,175,208,237,165,200,234,191,226,248,185,224,248,149,189,228,115,155,209,165,207,239,168,209,
    240,127,167,216,121,162,213, 92,132,195,136,179,223,104,138,197, 31, 53,144, 63, 93,172, 77,110,182, 68, 98,175,185,194,224,255,255,255,
    242,242,249,129,132,191,121,124,186,117,121,185,165,166,209,175,177,214,111,115,182,149,151,202,248,248,249,228,227,223,155,156,192,112,
    115,185,101,104,167,198,198,215,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,253,252,240,239,
    239,229,228,227,228,227,225,180,180,188, 94, 96,152,109,113,184,190,191,223,255,255,255,255,255,255,243,243,249,130,133,191,105,109,179,
    153,156,203,182,184,218,106,109,179,114,117,184,107,110,180,170,172,211,159,161,206,104,108,179,135,137,194,250,250,253,255,255,255,219,
    221,238,126,155,207,109,148,204, 73,109,182,105,139,198,181,206,234,232,242,251,172,197,228,126,158,208,184,207,234,166,192,225,123,156,
    207,147,176,218,182,206,233,177,202,232,152,182,222,147,177,218,186,210,237,163,193,228,133,167,214, 97,138,199,139,171,216,159,190,227,
    176,207,236,136,171,216,139,176,220,152,189,227,151,189,227,120,159,210, 99,138,199,127,168,216,134,175,220, 93,132,194,123,163,213,142,
    184,226,124,165,215, 70,103,177, 70,102,177, 77,110,182, 91,127,192, 80,113,183,181,191,222,255,255,255,242,242,249,121,125,187,112,115,
    182,108,112,181,159,161,206,170,172,212,102,105,177,203,204,229,249,248,244,214,213,212,130,132,176,105,109,179,124,127,173,248,248,249,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,247,246,246,233,232,233,225,224,223,220,
    219,218,115,116,152, 98,101,173,184,186,220,255,255,255,255,255,255,255,255,255,189,190,222,102,105,177,152,155,203,182,184,217,105,108,
    179,113,117,183,106,109,180,170,172,211,158,160,206, 99,103,176,171,172,212,255,255,255,255,255,255,171,180,218,118,153,206, 85,123,189,
    89,127,192,102,137,197,150,179,220,196,215,237,194,213,236,189,209,234,247,251,254,208,222,240,182,203,230,179,201,230,230,239,249,223,
    234,246,200,218,240,166,192,226,201,220,241,143,173,217,108,145,202, 84,123,190,114,150,204,136,170,215,177,205,234,144,178,220,171,203,
    234,179,212,239,178,212,240,142,180,222,122,161,211,149,189,228,160,202,235,120,161,212,131,171,218,144,185,227, 96,134,196, 52, 80,164,
    62, 95,172, 83,119,187, 91,126,191, 87,121,189,201,212,234,255,255,255,234,235,245,118,122,186,111,115,182,108,112,181,158,160,206,166,
    168,210,127,130,191,245,244,247,235,234,232,212,211,210,127,129,174, 97,100,167,183,183,206,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,251,251,251,238,238,237,227,226,226,231,230,227,164,164,178, 87, 90,155,
    184,185,221,255,255,255,255,255,255,255,255,255,240,240,248,131,134,192,164,166,209,191,193,222,125,128,189,132,135,192,126,129,190,181,
    183,217,171,173,212,121,125,187,205,205,230,255,255,255,250,248,251,151,167,212, 95,130,193, 75,109,180,117,151,205,113,145,200,165,191,
    226,129,162,211,197,214,236,183,203,230,247,249,252,211,223,240,215,228,244,203,219,239,255,255,255,255,255,255,232,240,249,187,208,234,
    252,255,255,186,209,235,114,148,202, 55, 95,173,118,152,205,177,203,233,228,244,254,165,195,229,193,220,242,202,228,247,199,229,248,158,
    193,229,145,182,223,159,198,233,173,214,242,133,175,220,131,172,218, 87,126,192,100,141,200, 55, 85,167, 47, 75,161, 77,112,183, 91,127,
    192,104,140,200,206,214,234,255,255,255,230,231,243,134,138,194,131,134,192,127,131,191,171,173,213,172,174,213,179,180,216,254,253,249,
    231,230,230,211,211,210,123,125,173,107,109,165,237,237,241,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,244,244,243,232,231,230,229,228,227,219,218,218, 89, 90,139,164,166,204,255,255,255,255,255,
    255,255,255,255,255,255,255,221,221,237,206,207,230,223,223,238,188,190,221,193,194,223,189,191,222,217,218,236,211,212,233,191,193,223,
    247,247,251,255,255,255,249,249,252,139,161,209, 65, 97,174,107,138,196,142,172,216, 77,107,179,136,165,212,202,219,239,186,205,232,130,
    160,209,203,219,239,156,180,219,170,193,225,157,182,220,206,219,238,219,228,243,218,227,242,182,200,229,254,255,255,199,217,238,140,170,
    215, 78,119,188,147,177,219,192,213,237,230,243,253,162,191,226,185,211,237,183,212,239,173,204,235,129,166,213,124,162,212,122,160,211,
    145,185,225,101,140,199,134,174,220,119,158,210, 81,119,187, 79,114,184, 47, 73,159, 56, 86,166, 81,117,186, 87,120,187,214,218,236,255,
    255,255,237,237,246,192,193,223,192,193,223,190,191,222,211,213,233,219,220,237,245,245,250,248,247,245,231,230,229,213,213,213, 98,100,
    145,153,155,186,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,250,250,249,237,236,236,231,230,229,241,240,238,175,174,188,191,191,207,255,255,255,255,255,255,255,255,255,255,255,255,226,
    226,240,157,159,205,179,181,217,105,108,179,113,116,183,106,109,180,169,171,211,153,156,203,133,136,193,251,251,253,255,255,255,247,248,
    252,164,181,219,124,147,202,191,206,232,188,209,234, 64, 92,170,117,145,200,215,229,244,128,159,210,143,170,214,182,203,231,136,167,212,
    193,209,233,176,196,227,231,239,248,221,230,244,196,208,232,157,172,214,200,209,233,170,185,221,128,156,207, 78,117,186,135,166,213,163,
    189,224,178,203,232,135,168,214,165,193,227,176,203,233,180,209,237,143,178,220,152,187,225,114,153,207,137,176,220, 99,138,198,102,141,
    200,138,179,223, 63, 94,171, 99,136,197, 71,102,177, 71,101,176, 60, 92,171, 65, 93,171,228,231,242,255,255,255,206,207,231,110,113,181,
    112,115,183,107,111,180,152,155,203,212,213,234,251,251,253,247,246,245,229,228,227,227,227,227,181,181,194,233,233,237,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,253,242,241,
    241,235,234,233,244,244,243,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,255,204,205,229,176,178,215,
    107,110,180,115,118,184,108,111,181,171,173,212,151,154,203,171,173,213,255,255,255,254,254,255,246,246,251,207,214,235,166,178,216,207,
    216,236,179,200,230, 75,100,174, 88,114,184,138,171,216,166,186,222,175,177,216,178,195,226,124,159,209,151,178,218,145,171,214,204,216,
    237,213,219,237,219,222,238,194,200,227,244,246,250,189,199,228,109,132,193, 63, 96,173,110,143,200,176,199,229,231,242,251,169,195,228,
    195,216,239,186,210,237,166,197,230,121,157,209,110,149,204, 90,129,192,112,152,207,100,141,200, 85,127,192, 95,136,198, 58, 86,164,119,
    160,210,115,152,207,109,147,204, 89,125,191, 59, 80,156,238,238,245,255,255,255,198,199,226,109,112,181,114,118,184,109,112,181,152,155,
    203,251,250,253,255,255,255,244,244,243,232,231,231,248,247,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,248,248,247,245,244,244,254,254,254,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,252,254,178,180,217,101,104,177,111,114,182,103,106,
    178,167,169,210,153,155,204,202,203,228,255,255,255,249,249,252,186,189,221,164,172,213,167,176,215,178,189,222,163,183,221, 70, 90,165,
    46, 70,156,130,156,207,251,251,253,254,253,254,213,219,237,129,155,206,220,227,241,190,203,229,226,231,243,227,231,243,194,200,227,169,
    175,214,215,218,236,172,180,217,139,154,205, 90,118,186,143,168,213,163,188,223,201,218,238,146,175,217,167,193,227,194,216,239,189,212,
    237,150,182,222,176,206,235,105,143,200,127,161,211,126,144,199,171,189,223, 83,107,177, 46, 71,152,101,140,200, 98,136,197, 90,126,192,
    86,123,190, 89,111,179,244,244,248,255,255,255,187,189,220,101,105,177,110,114,181,104,107,178,150,152,202,255,255,255,255,255,255,247,
    247,246,248,248,248,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,202,203,228,152,155,203,160,162,207,155,158,205,197,198,226,192,193,223,236,
    236,246,255,255,255,244,245,250,183,186,220,213,217,236,219,222,238,222,227,241,215,223,240, 83, 99,174, 48, 67,156,183,194,225,255,255,
    255,255,255,255,208,216,236,161,179,218,176,190,222,158,169,212,201,206,231,215,218,236,195,200,227,181,185,219,194,197,226,186,191,223,
    129,140,196,108,125,189,125,148,202,174,195,226,188,206,232,151,179,219,176,200,230,186,208,234,164,191,225,122,154,206,139,171,215, 87,
    125,190,181,200,229,253,251,253,255,255,255,142,152,201, 44, 69,154,118,157,210,129,169,217,107,147,204,105,144,203,109,135,194,249,249,
    252,255,255,255,208,209,231,154,155,204,160,163,207,153,155,203,195,196,225,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,244,245,250,173,175,213,170,172,212,167,168,210,201,202,227,218,218,236,252,252,254,255,255,255,220,221,237,
    171,175,214,176,181,217,187,191,222,178,183,218,163,169,211, 76, 86,166, 63, 73,159,211,215,234,255,255,255,254,254,255,171,178,215,187,
    194,225,237,239,248,203,207,231,226,228,243,235,236,246,198,201,228,166,171,212,182,187,220,166,171,213,175,182,218,123,131,191,172,185,
    221,162,179,218,168,189,223,136,166,212,190,210,235,214,230,245,184,206,233,172,200,230,205,228,247,123,158,209,194,210,233,255,255,255,
    255,255,255,128,139,194, 51, 81,165,100,139,198,113,154,207, 92,129,193, 81,116,186, 99,125,189,250,250,253,255,255,255,213,214,234,165,
    167,209,169,171,211,180,182,217,251,252,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,
    254,254,152,154,203,102,105,177,102,105,177,158,161,206,213,214,234,254,254,254,255,254,255,194,196,225,208,210,232,217,219,237,235,236,
    245,217,220,237,194,198,226,140,145,198,113,118,183,242,242,248,255,255,255,230,231,243,146,152,202,177,183,218,190,195,225,168,172,213,
    175,180,217,208,210,233,192,196,225,177,182,218,215,218,237,146,152,202,157,162,207,119,127,189,160,168,211,141,156,205,208,217,238,156,
    178,218,172,195,227,181,205,233,138,168,213,143,176,218,147,179,220,109,145,202,189,201,227,255,255,255,252,251,253, 87,104,176, 74,110,
    182,120,160,211,120,160,212,108,146,203, 95,133,195,103,128,191,247,247,251,255,255,255,183,184,219, 99,102,176,101,105,177,163,165,209,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,220,220,237,114,118,184,
    107,111,180,166,168,210,247,248,252,255,255,255,251,251,253,159,163,207,183,187,220,183,187,221,199,202,228,187,191,222,147,152,202,140,
    144,197,134,136,190,255,255,255,255,255,255,203,204,229,157,162,208,198,202,228,213,216,236,177,181,217,111,117,183,178,181,217,166,169,
    211,146,152,202,174,179,217,152,158,205,134,140,196, 86, 93,171,135,142,197,154,163,209,161,174,215,127,146,200,157,176,216,153,177,218,
    87,120,187,148,177,219,159,189,226,144,177,220,212,220,237,255,255,255,249,249,252, 95,117,183, 66,101,175,117,156,209,122,160,211,126,
    160,211,100,133,195, 90,109,177,242,242,247,255,255,255,194,195,224,108,111,180,115,118,184,222,223,238,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,255,150,152,202, 94, 98,173,166,168,210,255,255,
    255,255,255,255,233,233,244,153,156,205,186,189,222,204,206,231,190,194,224,211,213,234,136,142,197,156,161,207,199,200,226,255,255,255,
    249,249,252,177,180,217,182,187,220,210,213,234,207,211,233,209,212,233,151,157,205,117,124,187,113,119,185,156,162,207,219,221,238,182,
    186,219,146,152,202,110,117,184,152,157,205,195,199,226,203,208,232,136,147,200, 95,111,181, 99,121,187,119,146,200,192,212,236,171,197,
    230,155,184,223,237,241,248,255,255,255,249,250,252, 91,115,184, 71,103,178,124,160,212,101,130,193,130,154,206, 96,118,186, 86,101,170,
    231,231,241,255,255,255,208,209,231,101,104,176,143,145,198,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,217,218,236,158,161,206,205,206,230,255,255,255,255,255,255,225,226,240,183,
    185,220,210,210,234,219,220,238,207,209,233,197,199,227,156,160,206,217,220,238,251,251,254,255,255,255,249,249,253,174,179,216,171,177,
    216,239,240,248,238,239,247,165,170,211,142,148,200,160,166,209,186,190,222,180,184,218,223,225,240,198,201,228,192,196,225,144,151,202,
    199,203,229,202,205,230,214,217,236,161,167,210,156,166,210,118,133,193,129,148,201,153,174,216,206,223,241,193,213,237,248,250,253,255,
    255,255,249,250,252, 83,102,175, 86,113,183,154,181,221,151,171,215,186,201,231,155,167,211,142,150,200,234,234,244,255,255,255,236,236,
    246,163,166,209,210,210,232,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,187,188,221,204,206,229,255,255,255,255,255,255,174,175,214,145,147,200,198,199,227,174,176,215,
    164,167,210,133,137,194,135,139,195,210,212,233,255,255,255,255,255,255,209,211,232,192,195,224,214,217,236,181,186,220,201,204,229,215,
    218,236,185,191,223,213,216,235,250,251,253,214,216,235,249,249,252,206,209,232,176,181,217,119,126,189,193,197,225,211,213,234,248,248,
    251,200,204,229,241,242,248,173,181,218,165,174,215,193,203,230,173,187,222,152,174,216,249,251,253,255,255,255,243,242,248,104,121,186,
    107,128,191,124,138,196,145,155,204,190,196,226,147,152,202,136,139,194,208,209,231,255,255,255,255,255,255,197,198,226,248,248,252,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,205,206,230,163,165,209,196,197,225,187,188,221,116,120,184,113,116,184,194,196,224,147,148,200,135,137,195,118,121,186,162,164,
    209,234,234,245,255,255,255,244,244,250,188,191,222,190,194,224,255,255,255,216,219,236,142,148,200,209,213,233,142,149,200,178,182,218,
    206,210,232,195,199,227,222,225,240,193,197,226,179,183,218,130,137,195,201,204,230,181,186,220,216,219,237,176,180,217,199,203,229,145,
    152,202,146,152,202,192,197,226,116,130,191,213,222,240,255,255,255,255,255,255,241,242,248,112,130,191,122,139,198,107,115,183,124,130,
    191,186,190,222,115,119,185,101,106,177,160,162,207,255,255,255,247,247,251,208,208,227,251,251,253,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,249,248,252,203,204,229,
    150,152,202,104,107,179,113,116,183,113,116,183,192,193,223,136,138,195,117,119,185,117,119,185,194,194,224,255,255,255,255,255,255,186,
    188,221,112,117,184,239,240,248,219,221,238,212,215,234,205,207,230,146,152,202,133,139,195,187,191,222,190,194,224,169,174,213,192,196,
    224,182,187,220,163,168,210,112,119,184,183,187,220,167,171,213,201,204,229,163,168,210,192,196,225,148,154,203,116,122,186,143,149,200,
    211,215,235,199,205,230,255,255,255,255,255,255,244,245,250,120,133,193,116,124,188,111,116,183,125,128,190,188,191,223,118,122,186,111,
    114,181,125,128,189,211,212,233,192,193,222,217,217,232,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,248,248,252,150,152,202,105,109,179,112,116,
    182,111,115,182,191,193,223,131,133,192,104,107,178,157,159,206,249,250,252,255,255,255,240,241,248,107,112,180, 86, 92,170,208,210,232,
    226,228,241,194,198,226,220,222,238,200,203,229,132,137,194,145,151,202,177,182,218,162,167,210,183,188,221,166,172,212,160,166,209,128,
    134,193,173,178,215,157,162,208,185,190,222,149,155,203,166,171,213,112,118,184,142,147,199,183,188,221,200,204,230,179,183,218,254,254,
    254,255,255,255,246,246,250,129,135,194,110,114,182,110,113,182,122,125,188,189,191,222,116,120,184,110,113,181,107,110,179,135,138,194,
    224,224,239,251,251,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,210,211,232,193,194,224,196,197,225,196,198,226,229,230,242,201,
    202,228,198,199,226,247,247,251,255,255,255,255,255,255,231,232,243,140,142,196,109,115,182,136,142,197,201,205,229,233,235,245,186,190,
    222,181,186,219,137,144,198,160,165,209,161,166,209,202,205,230,208,211,233,183,188,221,177,181,217,142,148,201,195,199,226,180,184,219,
    214,217,236,176,180,217,160,166,210,132,139,195,114,121,185,192,196,225,172,177,215,243,244,250,253,253,254,255,255,255,251,251,253,199,
    201,227,195,197,226,197,198,225,201,202,227,228,229,241,197,199,226,193,195,224,196,197,225,206,207,230,252,252,254,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,215,216,235,146,148,199,151,154,203,150,152,202,207,208,231,155,158,205,198,199,226,255,255,255,
    255,255,255,234,235,245,150,152,202,130,133,192,120,126,188,157,162,207,134,139,195,239,240,247,188,192,223,143,150,201,161,167,210,237,
    239,246,188,192,223,211,213,234,205,209,231,192,195,225,177,182,217,136,144,198,203,206,230,186,190,222,216,219,237,196,199,227,194,197,
    225,203,207,231,125,132,192,133,140,196,198,203,229,240,241,248,253,253,254,255,255,255,249,249,252,156,159,206,150,153,202,151,153,202,
    159,162,206,208,210,231,154,157,204,149,151,201,148,150,200,171,173,213,252,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,242,242,249,125,128,189,106,110,180,109,112,182,189,191,222,124,127,189,126,129,190,200,201,227,193,194,224,194,195,224,119,121,
    185,108,112,180,111,115,183,185,189,222,154,160,206,156,160,206,192,196,225,156,162,207,153,159,205,208,211,233,188,192,223,237,238,247,
    212,214,234,208,210,232,187,192,222,145,152,202,215,217,236,193,197,225,227,229,242,224,226,241,187,191,222,188,192,222,123,130,191,155,
    161,207,158,163,208,152,156,204,252,252,253,255,255,255,247,247,251,120,124,187,109,112,181,109,112,180,121,125,187,190,191,222,115,118,
    184,108,112,180,102,105,177,178,179,216,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,190,191,222,
    109,112,181,112,116,183,190,192,222,131,134,192,111,114,182,109,113,181,114,117,184,187,189,221,126,129,189,107,110,180,114,118,183,166,
    170,213,199,202,228,184,189,222,158,163,208,152,157,205,163,169,211,202,205,230,188,192,223,218,220,238,193,196,226,201,204,229,177,182,
    218,137,144,197,203,206,230,182,186,220,210,213,233,204,206,231,185,189,222,180,183,219,133,141,197,138,144,198,134,141,196,179,183,219,
    252,252,254,255,255,255,247,247,251,124,127,189,113,116,183,113,116,183,125,128,189,192,194,224,119,122,186,111,115,182,119,122,186,234,
    234,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,255,255,248,248,251,136,139,195,106,110,179,190,191,
    222,130,133,192,111,114,182,112,115,182,117,120,185,189,191,222,123,127,188,108,111,180,111,114,181,175,178,216,186,189,221,205,208,232,
    166,170,212,149,154,204,122,128,189,166,171,212,147,152,202,206,209,232,186,190,222,185,189,221,165,170,211,128,135,193,185,188,222,174,
    178,216,204,208,231,192,195,225,141,146,199,132,138,195,116,123,186,149,155,203,159,164,209,189,192,224,237,238,246,255,255,255,247,247,
    251,122,125,188,111,114,182,112,115,183,123,127,188,191,193,223,117,120,185,105,109,179,157,160,205,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,254,255,252,252,254,255,255,255,218,219,237,185,187,220,225,226,240,199,200,227,191,193,223,192,
    194,223,193,195,224,225,226,240,197,199,226,188,189,221,130,132,191,180,183,219,205,208,232,188,192,223,208,211,233,169,173,213,106,113,
    181,169,174,213,162,167,210,185,190,222,160,166,209,156,161,207,144,150,201,112,119,184,161,166,210,146,152,202,174,179,216,169,174,214,
    158,163,208,132,138,194,105,111,180,188,191,223,176,180,217,160,165,210,235,235,245,255,255,255,251,251,253,195,196,225,190,192,222,192,
    193,223,197,198,226,227,227,241,194,196,224,185,187,220,225,225,240,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,253,253,254,251,251,253,255,255,255,248,248,252,146,148,200,196,198,226,149,151,201,133,135,193,134,136,193,137,140,196,200,202,228,
    144,146,199,127,130,190,115,118,183,145,150,201,225,227,241,173,177,215,171,174,214,182,186,220,168,173,213,210,213,234,191,194,224,209,
    211,233,172,176,215,164,169,211,153,158,205,130,136,194,165,168,211,148,154,203,200,203,229,202,205,230,185,188,221,179,183,219,150,155,
    204,176,180,217,140,146,199,185,188,222,231,232,244,255,255,255,252,252,253,155,157,205,131,133,192,132,135,192,143,145,198,201,202,228,
    132,135,193,155,157,205,251,250,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,252,254,251,251,253,254,
    254,255,255,255,255,180,181,217,185,186,219,133,135,193,115,117,184,116,118,184,120,123,187,193,194,223,127,130,190,106,109,179,123,126,
    189,166,168,211,217,219,237,172,176,215,176,179,217,169,173,213,142,147,199,185,188,221,207,208,232,229,229,243,189,191,223,215,218,236,
    187,190,222,157,163,208,208,209,233,189,192,224,218,220,237,227,228,242,188,191,223,155,159,206,128,134,193,165,169,211,155,159,206,167,
    170,213,203,206,231,255,255,255,255,255,255,158,160,205,111,113,182,115,118,184,126,129,190,193,194,224,113,116,183,194,196,224,255,255,
    255,253,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,251,251,254,251,251,253,253,253,254,255,255,255,234,234,244,
    203,204,229,132,134,193,116,119,184,117,120,185,122,124,187,194,195,224,128,131,191,109,111,180,134,136,194,161,163,209,210,212,234,170,
    173,214,171,174,215,164,168,211,149,154,204,205,208,232,188,190,223,209,211,233,174,178,216,198,200,227,172,177,215,147,152,202,188,191,
    223,172,177,216,203,205,230,202,204,229,177,181,218,164,169,211,137,142,197,163,168,211,156,160,206,159,164,209,209,211,234,254,254,255,
    255,255,255,178,179,216,110,113,182,116,119,185,128,131,191,190,192,223,132,135,193,240,240,248,255,255,255,253,253,254,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,251,251,253,252,252,253,254,253,254,255,255,255,255,255,255,239,240,248,138,140,196,129,132,
    191,130,132,191,134,136,194,199,200,226,138,140,196,125,127,189,208,209,232,225,225,240,212,213,233,164,167,210,178,181,218,160,163,209,
    144,149,201,182,185,220,187,189,222,232,231,244,195,197,226,213,213,235,197,200,227,176,179,217,204,205,231,190,193,224,230,231,243,221,
    223,239,167,171,212,141,146,199,150,154,203,162,166,210,153,156,205,173,176,216,199,200,228,249,249,252,255,255,255,202,202,228,124,127,
    189,128,130,191,140,142,197,192,193,223,180,181,217,255,255,255,254,254,255,253,253,254,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    254,254,255,252,252,253,253,253,254,255,254,255,255,255,255,255,255,255,251,251,253,200,201,227,193,195,224,195,196,224,197,198,225,228,
    229,241,194,196,224,203,203,229,255,255,255,255,255,255,233,233,244,188,190,222,197,199,227,178,181,217,160,164,208,148,151,201,167,171,
    212,207,209,232,178,180,218,208,210,233,183,186,220,168,171,213,198,200,227,182,185,220,202,204,229,182,186,220,133,137,194,147,152,202,
    164,167,210,181,183,219,174,177,215,154,157,205,185,189,222,246,247,250,255,255,255,240,240,247,191,192,222,191,192,223,199,199,227,228,
    228,241,243,243,249,255,255,255,254,254,255,253,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,253,254,253,253,254,254,254,
    255,254,254,255,255,255,255,255,255,255,255,255,255,183,184,218,120,123,187,126,129,189,130,133,192,196,198,225,133,135,193,216,216,235,
    255,255,255,255,255,255,219,220,238,168,170,213,176,177,216,167,169,212,109,114,181,139,143,197,139,143,198,159,163,208,129,134,193,170,
    174,214,164,167,211,155,158,205,160,163,208,136,141,196,159,164,208,146,151,202,116,121,186,102,107,178,134,138,195,179,181,218,146,150,
    201,140,144,199,151,155,204,224,224,238,255,255,255,245,245,250,140,142,197,122,124,188,132,134,192,222,222,238,255,255,255,255,255,255,
    255,255,255,253,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,253,254,253,253,254,254,254,255,254,254,255,255,255,255,255,
    255,255,255,255,255,236,237,245,130,132,192,121,123,187,127,130,191,192,193,223,156,157,205,250,250,252,255,255,255,233,233,242,201,202,
    229,175,176,216,196,197,227,198,199,228,172,175,215,175,178,216,161,164,209,163,167,211,119,125,187,142,146,199,127,130,190,113,118,183,
    131,135,193,119,126,188,164,168,211,157,162,208,140,145,199,147,151,202,167,169,212,204,205,232,164,165,211,129,133,192,119,122,187,188,
    189,219,255,255,255,255,255,255,184,185,219,115,118,184,126,128,190,230,230,243,255,255,255,255,255,255,255,255,255,253,253,254,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,252,252,254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    179,181,217,117,119,185,128,130,191,189,190,222,197,198,226,255,255,255,254,254,254,188,189,220,186,187,222,151,153,203,169,171,213,166,
    168,212,160,164,209,190,192,224,175,178,216,178,181,218,146,151,202,144,146,199, 86, 86,164, 62, 65,152,115,119,184,135,140,197,184,187,
    221,184,186,220,158,161,208,141,144,198,143,145,199,185,185,221,142,144,198,121,124,188,123,126,189,139,141,195,245,246,249,255,255,255,
    236,236,245,127,129,190,134,135,194,242,242,248,255,255,255,255,255,255,255,255,255,254,254,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,252,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,232,232,243,142,143,198,145,147,
    200,203,204,229,230,230,243,255,255,255,250,250,252,173,173,212,193,194,225,161,163,208,184,185,221,185,186,222,181,183,220,186,189,222,
    165,167,210,186,189,222,158,160,207,185,187,221,147,150,201,106,111,179,175,178,217,150,153,203,191,193,224,174,177,215,146,149,201,151,
    155,204,158,160,207,200,200,228,156,157,205,138,141,196,142,144,198,143,145,198,233,234,244,255,255,255,255,255,255,164,166,209,173,174,
    214,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,253,254,252,252,253,254,254,255,
    254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,221,221,238,196,197,225,236,236,245,255,255,255,255,
    255,255,238,238,246,204,205,229,217,217,236,192,193,225,197,199,227,198,200,228,177,178,216,193,195,225,209,211,233,233,233,245,212,214,
    235,233,234,245,210,212,234,183,186,221,223,223,240,207,208,232,244,244,250,220,221,238,181,183,219,172,174,214,185,186,220,216,217,237,
    196,196,226,190,191,222,195,196,225,196,197,225,240,239,247,255,255,255,255,255,255,244,244,252,245,245,251,254,254,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,253,254,252,251,253,253,253,254,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,225,225,240,132,134,193,219,219,236,255,255,255,255,255,255,199,199,225,142,143,197,
    203,204,230,149,150,202,152,153,204,166,168,212,166,167,211,190,191,224,172,174,216,181,183,219,170,171,214,183,185,220,177,178,216,139,
    142,197,170,172,213,156,159,207,208,209,232,181,183,219,148,150,202,136,139,196,143,144,199,192,193,224,146,148,200,130,132,192,131,133,
    192,136,138,195,224,223,238,236,236,243,255,255,255,207,206,212,243,242,243,254,254,255,254,254,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,253,253,254,254,254,255,254,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,179,180,217,216,216,235,255,255,255,255,255,255,182,182,217,140,141,197,210,211,233,153,154,204,164,165,
    210,162,163,209,164,166,210,172,173,214,154,156,204,173,174,215,178,179,218,186,187,221,183,185,220,145,147,200,168,169,212,168,170,213,
    208,208,232,171,172,213,133,135,194,140,141,197,144,145,199,198,199,227,151,151,202,136,137,195,135,137,195,143,144,198,219,219,236,208,
    208,230,255,255,254, 93, 90, 94,208,208,207,255,255,255,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    234,235,245,230,230,243,255,255,255,255,255,255,169,169,210,140,141,197,212,213,234,148,149,202,158,160,208,171,172,214,183,184,220,193,
    195,225,164,165,210,159,160,208,159,160,207,174,175,215,173,175,214,138,140,196,149,151,203,157,158,207,187,188,222,177,178,216,142,144,
    198,156,157,206,149,150,202,199,200,227,150,150,202,136,137,195,136,137,195,143,144,198,221,221,238,192,193,225,209,208,211, 28, 24, 26,
    165,163,163,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,245,245,246,255,255,
    255,254,254,254,194,195,224,183,183,219,229,229,242,183,184,219,187,188,222,188,189,222,204,205,231,212,212,234,195,195,225,199,199,228,
    202,203,229,213,213,235,211,212,234,188,189,222,195,196,226,203,203,230,217,218,238,213,214,235,173,174,214,184,185,220,175,176,215,214,
    214,234,185,186,220,179,179,217,179,180,217,183,183,219,234,234,245,209,210,235,127,125,130,  9,  4,  5,132,129,130,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,164,162,163,236,236,236,255,255,255,200,201,227,196,
    197,226,232,232,244,194,195,225,195,196,225,198,198,227,205,205,231,219,219,238,195,195,226,201,201,229,201,201,228,215,215,236,216,217,
    236,194,196,226,196,197,227,202,202,229,225,225,240,215,215,236,179,180,217,187,188,221,187,188,221,219,220,237,196,196,226,190,191,223,
    191,192,223,195,195,225,241,241,250,190,190,216, 58, 54, 57, 18, 14, 14,115,112,113,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,112,109,110,165,164,167,255,255,255,161,161,206,159,160,208,219,219,238,157,158,207,
    157,158,207,160,161,209,173,174,215,213,213,234,161,161,208,162,162,209,160,161,208,186,187,222,197,198,227,159,160,208,160,161,209,161,
    162,209,207,208,232,190,191,223,154,155,206,154,155,205,147,149,201,203,203,229,161,162,208,148,148,202,148,149,202,155,156,205,230,231,
    246,162,161,194, 41, 37, 38, 22, 18, 19,107,104,104,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,112,109,110, 67, 64, 67,246,246,248,173,173,213,169,169,213,224,224,240,165,165,211,165,166,211,165,165,211,179,179,
    218,216,216,236,166,166,211,167,168,212,164,165,210,194,195,225,202,202,229,164,165,210,169,170,213,166,166,212,213,213,235,196,196,226,
    161,162,209,162,162,209,156,157,206,210,210,233,171,171,213,159,159,207,159,160,207,164,164,209,237,237,249,205,204,215, 47, 43, 42, 20,
    16, 17,112,109,109,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,120,117,118,
    15, 11, 12,171,169,175,182,182,223,170,170,213,224,224,240,166,166,211,166,166,212,165,166,211,177,177,217,216,216,235,163,164,210,165,
    166,211,162,162,209,195,195,225,202,202,229,164,163,210,168,168,213,164,164,210,212,212,234,197,197,226,162,163,210,161,162,209,159,159,
    208,212,213,234,172,172,214,159,159,207,160,160,208,164,164,210,245,245,254,219,217,217, 41, 37, 38, 19, 15, 16,124,121,122,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,123,120,121, 13,  9,  9, 82, 78, 82,209,210,
    232,210,210,233,238,238,247,208,208,232,208,208,232,208,208,232,213,214,235,234,234,245,206,207,231,207,208,232,205,206,231,223,223,239,
    226,227,241,206,206,231,208,208,232,206,206,231,231,232,244,224,224,240,205,206,231,206,207,232,206,206,231,233,233,244,212,212,234,205,
    205,230,205,205,230,206,206,231,250,250,255,203,201,201, 36, 32, 33, 18, 13, 14,137,134,134,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,130,127,128, 19, 14, 15, 40, 36, 36,185,184,200,217,217,239,240,240,248,212,
    212,234,211,212,234,211,211,233,217,217,236,235,235,246,210,210,233,211,212,234,210,210,233,226,225,241,229,229,242,210,210,233,212,212,
    234,209,210,233,234,234,245,227,227,241,210,210,233,211,211,233,210,210,233,235,235,245,216,216,235,209,210,232,209,209,232,214,214,235,
    255,255,255,185,183,182, 30, 26, 27, 21, 17, 18,157,155,155,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,141,138,139, 18, 14, 15, 39, 35, 33,172,170,184,191,191,229,227,227,241,178,178,217,178,178,217,177,177,217,
    187,187,222,221,220,238,176,176,216,177,178,217,175,175,216,203,203,229,209,209,232,175,175,216,179,179,218,174,174,216,217,217,236,205,
    205,231,175,175,216,177,177,217,175,175,216,220,220,238,185,186,221,174,174,215,171,171,214,203,203,230,255,255,255,167,165,165, 24, 20,
    21, 29, 25, 26,181,179,179,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,151,
    149,149, 19, 15, 16, 33, 29, 29,188,186,187,217,218,242,228,228,242,187,186,222,186,186,221,185,185,221,194,194,226,224,224,240,185,185,
    221,186,186,221,184,184,221,208,209,233,213,214,235,184,184,221,187,187,223,183,183,220,222,222,239,211,211,234,183,184,221,186,186,222,
    185,184,221,224,224,240,193,193,225,182,182,220,182,183,219,235,235,245,255,255,255,151,149,149, 19, 15, 15, 36, 32, 33,202,200,200,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,163,161,161, 23, 19, 20, 28, 23, 25,
    178,176,176,248,248,255,230,230,243,188,188,223,187,187,222,187,186,222,195,195,226,225,225,240,187,187,222,188,188,222,186,186,222,210,
    209,233,215,215,236,185,185,222,189,188,223,184,184,221,222,222,239,212,211,234,185,185,222,188,188,222,186,186,221,224,224,240,194,194,
    226,182,182,220,199,199,228,253,253,254,255,255,255,139,137,137, 17, 13, 14, 41, 37, 38,219,218,217,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,176,174,174, 28, 24, 25, 24, 20, 21,163,162,162,255,255,255,244,244,
    250,223,223,239,223,222,240,222,222,239,227,227,241,240,240,248,222,222,239,223,223,240,222,222,239,233,233,245,236,236,246,221,222,239,
    223,223,240,221,221,239,239,239,248,235,234,245,222,222,239,223,223,240,222,222,239,240,240,248,227,227,241,219,219,238,236,236,246,255,
    255,255,255,255,255,128,125,126, 18, 14, 15, 46, 42, 43,234,233,233,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,188,187,187, 33, 29, 30, 19, 15, 16,151,149,149,255,255,255,243,243,250,211,211,234,212,212,235,211,
    212,235,218,218,237,236,236,246,211,212,234,212,213,235,211,211,234,226,226,241,230,230,243,210,211,234,213,213,235,210,211,234,234,234,
    245,228,228,242,211,211,234,212,212,235,212,212,234,236,236,246,217,217,236,215,215,236,253,253,254,255,255,255,255,255,255,124,121,122,
    18, 14, 15, 49, 46, 46,242,242,242,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,199,197,197, 36, 32, 33, 17, 13, 14,141,139,140,255,255,255,251,251,253,209,209,234,198,198,228,199,199,229,207,206,232,231,231,244,
    199,199,229,200,200,229,198,198,228,218,218,237,222,222,239,198,197,228,200,200,229,197,197,227,228,228,243,219,219,238,198,198,228,200,
    200,228,199,199,229,231,231,244,204,204,231,221,221,239,255,255,255,255,255,255,255,255,255,117,114,115, 10,  6,  7, 45, 42, 42,243,243,
    243,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,208,207,207, 38, 35, 35, 19,
    15, 16,131,128,128,255,255,255,255,255,255,239,239,248,205,205,231,206,205,231,212,212,234,233,233,244,206,205,232,207,206,232,205,205,
    231,222,222,239,226,226,241,205,204,230,207,207,232,204,204,231,231,231,244,224,224,240,205,205,231,206,206,232,206,206,232,232,232,244,
    213,213,235,245,245,250,255,255,255,255,255,255,255,255,255,146,144,144, 67, 63, 63, 96, 93, 93,246,246,246,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,219,218,218, 41, 37, 38, 19, 15, 16,124,121,121,255,255,255,
    255,255,255,254,254,255,220,220,238,208,208,233,215,215,236,235,235,245,209,209,234,210,210,234,208,208,233,225,225,241,228,228,242,208,
    208,233,210,210,234,207,207,233,233,233,245,226,226,242,208,208,233,210,210,234,209,209,234,233,233,245,228,228,242,255,255,255,255,255,
    255,255,255,255,255,255,255,252,252,252,251,251,251,252,252,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,222,221,221, 43, 38, 39, 19, 15, 16,120,117,118,255,255,255,255,255,255,255,255,255,246,246,
    251,235,234,245,239,238,247,247,247,251,236,236,246,236,236,246,235,235,246,242,242,249,244,244,250,235,235,246,236,236,246,235,235,246,
    246,246,251,243,243,249,235,235,246,236,236,246,236,236,246,246,246,251,248,248,252,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,227,226,226, 44, 40, 41, 20, 16, 17,112,109,110,255,255,255,255,255,255,255,255,255,255,255,255,229,228,243,225,225,241,241,
    241,248,222,222,240,223,223,240,221,221,239,234,233,245,236,235,246,221,221,239,223,223,240,220,221,239,239,239,248,234,234,245,221,221,
    239,223,223,240,222,222,239,242,242,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,227,226,226, 44, 40,
    41, 20, 16, 17,112,109,110,255,255,255,255,255,255,255,255,255,255,255,255,241,241,249,224,223,240,240,240,248,220,220,239,221,221,239,
    220,220,238,232,232,244,235,235,245,219,219,238,221,221,239,219,219,238,238,238,247,233,233,245,220,220,238,221,221,239,220,220,238,241,
    241,248,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,222,221,221, 37, 33, 34, 15, 11, 12,116,113,114,255,
    255,255,255,255,255,255,255,255,255,255,255,253,253,254,232,232,244,241,240,248,223,223,240,224,224,241,223,223,240,234,234,245,236,236,
    246,223,223,240,224,224,240,222,222,239,240,240,248,235,235,246,223,223,239,224,224,240,225,224,240,248,248,252,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,224,223,223, 66, 62, 63, 36, 32, 33,132,129,129,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,247,247,252,243,243,249,229,229,243,230,230,243,229,229,243,238,238,247,240,240,248,229,229,242,230,230,243,228,
    228,242,242,242,249,239,239,247,229,229,242,229,228,243,235,235,246,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,254,253,253,245,245,245,242,241,241,247,247,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,250,250,253,244,244,250,245,245,250,244,244,250,248,248,252,249,248,252,244,244,250,245,245,250,244,244,250,250,250,253,248,248,252,
    244,244,250,244,244,250,252,252,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,248,248,251,234,233,245,234,
    234,245,233,233,245,241,240,248,243,242,249,233,233,244,235,234,245,234,233,245,245,245,250,242,241,249,233,232,244,238,237,247,254,254,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,255,239,239,248,235,234,246,235,234,246,242,241,249,
    244,243,250,235,234,245,236,235,246,234,234,246,246,245,251,242,242,249,234,233,245,246,245,250,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,249,249,252,237,237,246,237,236,247,243,243,250,245,244,250,237,236,247,238,237,
    247,237,236,247,247,246,251,244,243,250,238,238,247,253,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,244,244,250,241,241,248,247,246,251,247,247,251,241,240,249,242,242,249,241,240,249,249,248,252,246,
    246,251,245,245,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,254,254,254,249,249,252,251,251,253,252,251,254,249,249,252,249,249,252,249,248,252,252,252,254,251,251,253,254,253,254,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,255,245,245,251,247,
    246,251,248,248,252,242,242,249,243,243,249,242,242,249,249,249,252,249,249,252,254,254,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,252,254,248,248,252,249,248,252,244,245,250,
    245,245,251,244,244,250,250,250,252,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,251,
    252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,252,254,250,250,252,247,247,251,247,247,251,246,246,251,251,251,
    253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,222,222,231,148,149,189,169,169,212,
    169,170,211,221,221,238,255,255,255,255,255,255,251,250,253,248,247,251,248,248,252,247,247,252,251,252,254,255,255,255,250,250,249,184,
    185,217,168,169,212,159,160,204,190,191,217,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,254,253,253,246,246,245,238,236,234,132,132,155, 87, 89,152,112,115,184,144,146,199,240,240,
    248,255,255,255,253,253,254,250,250,253,249,250,253,250,251,253,255,255,255,253,253,250,195,195,216,115,118,185,107,111,180,101,104,157,
    224,224,231,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    254,254,254,245,245,245,235,234,234,229,228,227,215,214,213,103,104,139,105,108,174,115,118,185,175,175,214,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,253,252,251,220,219,221,136,137,190,114,118,185, 96, 98,162,173,174,195,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,254,247,246,246,236,235,
    235,226,226,225,226,226,224,183,182,188, 89, 91,142,113,117,184,122,125,188,213,213,234,255,255,255,255,255,255,255,255,255,255,254,255,
    233,232,228,164,165,197,116,118,186,103,106,173,123,124,164,247,247,248,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,254,244,244,243,234,232,232,224,224,223,228,
    227,224,143,143,161, 93, 96,156,114,118,186,143,145,199,243,243,249,255,255,255,255,255,255,245,244,241,189,189,204,123,125,188,111,115,
    182,104,106,157,222,222,228,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,251,251,251,239,239,238,230,229,229,225,225,224,213,213,212,101,102,139,
    105,109,175,114,118,185,184,184,219,255,255,255,255,254,253,218,218,219,139,141,191,115,118,187, 96, 99,163,174,174,196,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,247,247,246,237,235,235,228,227,226,228,227,225,178,178,185, 85, 87,136,111,115,183,126,129,
    191,216,217,236,244,243,236,170,170,199,116,119,187,106,109,176,124,126,167,247,247,248,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,254,254,254,244,243,243,234,234,233,225,224,224,228,228,224,150,150,165, 91, 93,152,115,118,187,145,146,198,194,194,210,129,
    131,189,112,116,185,100,102,158,215,215,224,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,251,
    251,241,240,240,231,230,230,225,224,223,222,221,219,113,113,144,102,105,170,120,123,188,130,132,191,118,122,188, 95, 98,163,160,160,186,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,249,248,247,237,236,236,227,
    226,226,227,226,225,188,187,192, 89, 91,140,114,117,185,117,121,188,107,111,178,115,117,160,239,239,242,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,254,245,244,244,234,233,233,226,225,224,226,226,223,
    143,143,159, 78, 80,135, 92, 94,156, 91, 93,146,214,215,224,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,252,252,251,241,240,239,231,231,231,224,223,223,212,211,211,153,152,163,158,157,
    172,196,196,206,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,248,247,247,237,236,236,229,228,228,227,227,226,242,242,240,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,254,254,253,246,246,245,244,243,243,245,245,245,254,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};

  // copy the data into the image
  Point2D<uint2> position( 0, 0 );
  uint4 uCount( 0 );
  const uint4 uImageDimY( m_image.GetDim().m_y );
  const uint4 uImageDimX( m_image.GetDim().m_x );

  for ( position.m_y=0; position.m_y < uImageDimY; ++position.m_y )
  {
    for ( position.m_x=0; position.m_x < uImageDimX ; ++position.m_x )
    {
      uint1 uRed  ( vData[ uCount++ ] );
      uint1 uGreen( vData[ uCount++ ] );
      uint1 uBlue ( vData[ uCount++ ] );
      PixelRGB<uint1> pixel( uRed, uGreen, uBlue );
      m_image.SetPixel( position, pixel );
    } // loop over columns
  } // loop over rows

} // InitIcon()


#undef FILE_REVISION


// $Log: V3DFile.C,v $
// Revision 1.6  2007/03/22 01:10:10  geconomos
// removed incorrect "inline" fom SetVariable
//
// Revision 1.5  2006/04/03 16:36:51  geconomos
// formatting
//
// Revision 1.4  2006/03/29 19:36:22  geconomos
// changed Inflate methods to static
//
// Revision 1.3  2006/03/13 17:43:02  dongqing
// add WriteExtraChunk() to fix the bug in SegmentList
//
// Revision 1.2  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.34  2003/07/15 17:14:01  vxconfig
// Changed the redeclaration of variable 'i ' from the function CopyBufferInterleaved
// by Romy
//
// Revision 3.33  2003/06/25 19:09:30  geconomos
// Corrected code to read and write data chunk for voxel sizes other than 16-bit.
//
// Revision 3.32  2003/06/10 12:07:24  frank
// Code review of pixel classes
//
// Revision 3.31  2003/06/09 13:56:55  frank
// did not update the progress meter if either progress percentage is negative
//
// Revision 3.30  2003/05/20 15:22:54  frank
// Added progress metering to the write process
//
// Revision 3.29  2003/05/20 14:36:17  frank
// Bad merge
//
// Revision 3.28  2003/05/20 14:01:56  frank
// Moved V3DFile test to test environment
//
// Revision 3.27  2003/05/20 13:28:12  mkontak
// Coding standards
//
// Revision 3.26  2003/05/20 12:46:02  frank
// code review
//
// Revision 3.25  2003/05/19 16:54:12  frank
// code review in progress
//
// Revision 3.24  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.23  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.22  2003/04/18 15:59:33  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 3.21  2003/04/14 16:41:31  frank
// Separated progress percent and progress message interface, added progress to volume loading.
//
// Revision 3.20  2003/01/22 21:21:16  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 3.19  2002/12/20 19:01:15  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 3.18  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 3.17  2002/10/21 16:16:29  kevin
// Cleaned up some debug
//
// Revision 3.16  2002/09/30 20:46:33  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.15  2002/09/25 18:46:02  ingmar
// added missing datatype declaration
//
// Revision 3.14  2002/06/19 17:49:26  soeren
// removed dead code and unnecessary comments
//
// Revision 3.13  2002/06/18 15:11:08  soeren
// added compression scheme c
//
// Revision 3.12  2002/06/17 22:37:41  soeren
// added new compression scheme
//
// Revision 3.11  2002/06/05 19:13:33  soeren
// Added Homogenous Flag
//
// Revision 3.10  2002/05/06 23:21:25  soeren
// added dirtyflag to store
//
// Revision 3.9  2002/05/03 13:16:17  ingmar
// progress feedback nowalso consideres the work to bwe done before and after getting the blocks from the disk
//
// Revision 3.8  2002/05/02 22:22:16  ingmar
// added progressCallback function to Read() of 2.0 FileVersion files
//
// Revision 3.7  2002/04/26 12:38:55  soeren
// added checksum test for reading and writing vxBlockVolumes
//
// Revision 3.6  2002/04/18 20:46:16  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
// and renamed GetVoxelRaw(offset) to GetVoxelAtLinOffset(offset)
//
// Revision 3.5  2002/04/18 15:56:10  ingmar
// enabled optmized read and write of new vxBlockVolume
//
// Revision 3.4  2002/03/11 16:30:26  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.3  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.2.2.1  2002/02/27 00:41:44  kevin
// Tracker ID 1864
// Added File COpy routines so that anonymization does not have to read the volume completely
// and unmopress it just to write it back out. Now it just copies the chunks without
// even dealing with wheter they are ciompressed or not
//
// Revision 3.2  2001/10/30 17:00:55  soeren
// optimized and beautified some functions
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 04 2001 14:58:20   frank
// Removed const enums in header file for gcc.
// 
//    Rev 2.2   Sep 24 2001 19:46:52   frank
// Coding conventions.
// 
//    Rev 2.1   Sep 21 2001 12:22:08   pagron
// put functionality to set version number on a file
// 
//    Rev 2.0   Sep 16 2001 23:47:24   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:50   ingmar
// Initial revision.
// Revision 1.6  2001/09/14 23:30:19  soeren
// changed LogFyi to LogDbg
//
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
// Revision 1.1  2001/09/01 00:11:46  soeren
// renamed image directory
//
// Revision 1.36  2001/08/25 04:50:01  frank
// Revamped file versions.
//
// Revision 1.35  2001/08/23 13:21:27  frank
// Writes both RGB and RGBA files now.
//
// Revision 1.34  2001/08/13 20:05:28  tushar
// removed errors due to the new logger
//
// Revision 1.33  2001/08/13 00:24:27  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.32  2001/08/07 17:02:45  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.31  2001/07/31 20:14:44  tushar
// changed IsEof to IsEofSmart in the read function. IsEofSmart checks the end of file more correctly
//
// Revision 1.30  2001/07/25 00:29:20  tushar
// fixed some compiler errors (logger)
//
// Revision 1.29  2001/07/23 12:25:46  frank
// Reflected change in Image()
//
// Revision 1.28  2001/07/20 11:54:03  frank
// Encoded scanlines.
//
// Revision 1.27  2001/07/16 23:42:44  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.26  2001/07/13 11:52:06  frank
// Moved decoding functions to ScanlineFilter class.
//
// Revision 1.25  2001/07/12 19:49:35  frank
// Beefed up unit test.
//
// Revision 1.24  2001/07/12 18:30:31  frank
// Made it possible to use an external buffer to avoid the intermediate uncompressed buffer.
//
// Revision 1.23  2001/07/12 17:09:07  frank
// Switched from C++ file streams to the std::shared_ptr<io::FilePtr> class.
//
// Revision 1.22  2001/07/12 16:23:32  frank
// Removed old icon.
//
// Revision 1.21  2001/07/12 15:06:26  frank
// Handled split image data over multiple IDAT chunks.
// Implemented four image filtering operations.
//
// Revision 1.20  2001/07/10 19:50:01  frank
// Removed debug info.
//
// Revision 1.19  2001/07/10 19:34:54  frank
// Implemented skipping of chunks and added new Viatronix icon.
//
// Revision 1.18  2001/07/10 17:13:24  frank
// Reorganized to read any organization of PNG file.
//
// Revision 1.17  2001/07/10 13:19:28  frank
// Added comments.
//
// Revision 1.16  2001/07/09 16:19:04  frank
// Didn't skip image data.
//
// Revision 1.15  2001/07/09 16:16:45  frank
// Now reads/writes PNG and lin vol files.
//
// Revision 1.14  2001/07/09 15:36:35  frank
// Worked on compression.
//
// Revision 1.13  2001/07/09 12:09:12  frank
// Skipped reading data during preview read.
//
// Revision 1.12  2001/07/06 19:44:46  frank
// Worked out a lot of bugs.  First working version.
//
// Revision 1.11  2001/07/02 20:06:03  frank
// Added inequality test
//
// Revision 1.10  2001/06/29 19:47:40  frank
// Updated compression methods.
//
// Revision 1.9  2001/06/27 04:31:18  frank
// Working on compression.
//
// Revision 1.8  2001/06/26 15:06:35  frank
// Improved numerous things.
//
// Revision 1.7  2001/06/24 21:01:53  frank
// Fixed basic PNG chunk I/O, got it working for simple files.
//
// Revision 1.6  2001/06/23 20:40:37  frank
// Working on PNG format.
//
// Revision 1.5  2001/06/20 15:15:55  frank
// Reflected change in Image class.
//
// Revision 1.4  2001/06/19 19:49:21  frank
// Improved I/O
//
// Revision 1.3  2001/06/19 19:08:49  frank
// Began implementing I/O.
//
// Revision 1.2  2001/06/19 16:29:52  frank
// Integrated V3DFile class into system.
//
// Revision 1.1  2001/06/19 15:53:06  frank
// Initial version.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/V3DFile.C,v 1.6 2007/03/22 01:10:10 geconomos Exp $
// $Id: V3DFile.C,v 1.6 2007/03/22 01:10:10 geconomos Exp $
