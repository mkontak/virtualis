// $Id: V3DFile.h,v 1.4 2006/03/29 19:39:47 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * The V3D file is a PNG image used to store V3D data of any type.
 * It replaces the external PNG file library with a C++ version capable
 * of exception handling and a better interface.
 */

#ifndef V3DFile_h
#define V3DFile_h


// Includes
#include "Variable.h"
#include "FilePtr.h"
#include "Image.h"
#include "PixelRGB.h"
#include "PixelRGBA.h"
#include "MemoryBuffer.h"


// Typedefs
typedef void (* ProgressCallbackPtrType)(float4 fPercentageComplete);


// Exceptions
namespace ex
{
  class ReachedEndOfFile : public VException
  {
  public:
    ReachedEndOfFile(const vxLogRecord& logRecord) : VException(logRecord) { }
  };
} // namespace ex

class VX_BASE_DLL V3DFile
{
// Inner Classes
public:

  /// A memory block
  struct MemBlockInfo
  {
    /// number of bytes per entry in block
    uint1 uBytesPerEntryInBlock;
    
    /// the size of a memory block
    uint4 uMemBlockByteSize;    

    /// a pointer to a memory block
    uint1 * pMemBlock;

    /// no idea what this is for
    int4 iFlag;

  }; // struct MemBlockInfo

// Enumerations
public:

  /// The type of compression used
  enum CompressionTypeEnum { NONE, ZLIB, LZO, NONE_BLOCKS, LZO_BLOCKS, LZO_BLOCKS_PLUS_BLEND_OUT_OF_HIGHER_FOUR_BIT_OF_16_BIT_DATA};

// Member Functions
// Construction & Destruction
public:

  /// Default constructor.
  V3DFile();

  /// Destructor.
  ~V3DFile();

  
// Primary Input & Output
public:

  /// Read a file from disk.
  void Read( const std::string & sFilename, std::pair< float4, float4 > & progressPair );

  /// Write a file to disk.
  void Write( const std::string & sFilename, CompressionTypeEnum eDataChunkCompression = LZO );

  /// Write a file to disk with progress metering
  void Write( const std::string & sFilename, CompressionTypeEnum eDataChunkCompression, std::pair< float4, float4 > progressPair );

  /// Copy a file changing just the comment
  void Copy( const std::string & sInFilename, const std::string & sOutFilename, const std::string & sNewComment );

  
// Access functions
public:

  /// Returns the raw data memory buffer
  inline MemoryBuffer & GetDataBuffer() { return * m_pDataBuffer; }

  /// Sets the data buffer
  inline void SetData( MemoryBuffer & memBuf ) { m_pDataBuffer = & memBuf; }
  
  /// Sets the file title
  inline void SetTitle( const std::string & sTitle ) { m_sTitle = sTitle; }
  
  /// Gets the file title.
  inline const std::string & GetTitle() const { return m_sTitle; }

  /// Sets the file comment.
  inline void SetComment( const std::string & sComment ) { m_sComment = sComment; }
  
  /// Gets the file comment.
  inline const std::string & GetComment() const { return m_sComment; }
  
  /// Sets the file description.
  inline void SetDescription( const std::string & sDescription ) { m_sDescription = sDescription; }
  
  /// Gets the file description.
  inline const std::string & GetDescription() const { return m_sDescription; }

  /// Gets a reference to the image.
  inline const Image< PixelRGB<uint1> > & GetImage() { return m_image; }
  
  /// Gets a reference to the image.
  inline const Image< PixelRGBA<uint1> > & GetAlphaImage() { return m_imageRGBA; }
  
  /// Sets the image.
  void SetImage(const Image< PixelRGB<uint1> > & image);
  
  /// Gets a particular variable.
  const Variable & GetVariable( const std::string & sName ) const;
  
  /// Sets a variable.
  void SetVariable( const Variable & variable );

  /// Skips reading the raw data chunk (usually the big part of the file).
  inline void SetSkipData( const bool bSkipData ) { m_bSkipData = bSkipData; }
  
  /// Skips reading the image data chunk.
  inline void SetSkipImage( const bool bSkipImage ) { m_bSkipImage = bSkipImage; }

  /// Sets an RGBA image.
  void SetAlphaImage( const Image< PixelRGBA<uint1> > & image );

  /// Returns whether or not the current image has alpha data or not.
  inline bool IsAlphaImage() const { return m_bAlphaImage; }
  
  /// Returns an extra chunk (one that was not recognized as text, image, or data)
  bool GetExtraChunk( const std::string & sChunkName, MemoryBuffer & destBuffer );
  
  /// Adds an extra chunk (one that is not text, image, or the data chunk)
  void SetExtraChunk( const std::string & sChunkName, const MemoryBuffer & sourceBuffer );
  
// Miscellaneous functions
public:

  ///  Retrieves the file version from disk.
  const std::pair<int4, int4> & GetFileVersion() const;

  /// Specifies the new file version.
  void SetFileVersion( const std::pair<int4, int4> & fileVersionMajorMinor );

  /// Equality operator.
  bool operator ==( const V3DFile & other ) const;
  
  /// Inequality operator.
  bool operator !=( const V3DFile & other ) const;


// Utility functions
public:

  /// Pack an RGB image into a memory buffer
  static MemoryBuffer PackImage( const Image<PixelRGB<uint1> > & srcImage );

  /// Pack an RGBA image into a memory buffer
  static MemoryBuffer PackImageRGBA( const Image<PixelRGBA<uint1> > & srcImage );

  /// Unpack an RGB image from a memory buffer
  static Image<PixelRGB<uint1> > UnpackImage( MemoryBuffer & srcMemBuffer, const Triple<uint4> & dimensions );

  /// Unpack an RGBA image from a memory buffer
  static Image<PixelRGBA<uint1> > UnpackImageRGBA( MemoryBuffer & srcMemBuffer, const Triple<uint4> & dimensions );

// protected utility functions
protected:

  /// Read the header of a chunk
  std::string ReadChunkHeader( std::shared_ptr<io::FilePtr> & inStream, uint4 & uChunkSize ) const;
  
  /// Write out the signature to disk
  void WriteSignature( std::shared_ptr<io::FilePtr> & outStream );
  
  /// Read the signature from disk
  void ReadSignature( std::shared_ptr<io::FilePtr> & inStream );
  
  /// Write the image header to disk
  void WriteImageHeader(std::shared_ptr<io::FilePtr> & outStream);
  
  /// Read the image header from disk
  void ReadImageHeader( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize );
  
  /// Write a text chunk to disk
  void WriteTextChunk( std::shared_ptr<io::FilePtr> & outStream, const Variable & variable ) const;

  /// Read a text chunk from disk
  void ReadTextChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize );

  /// Write the text chunks to disk
  void WriteTextChunks( std::shared_ptr<io::FilePtr> & outStream );
  
  /// Write the image chunks to disk
  void WriteImageChunk( std::shared_ptr<io::FilePtr> & outStream );
  
  /// Read the image chunk from disk
  void ReadImageChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize );

  /// Unpack the image from the file data
  void ExtractImage();
  
  /// Write the data chunk to disk
  virtual void WriteDataChunk( std::shared_ptr<io::FilePtr> & outStream, CompressionTypeEnum eDataChunkCompression );
 
  /// Read the data chunk from disk
  virtual void ReadDataChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize, const std::string & sChunkType );
  
  /// Write the extra chunks to disk
  void WriteExtraChunks( std::shared_ptr<io::FilePtr> & outStream );
 
  /// Read the extra chunks from disk
  void ReadExtraChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize, const std::string & sChunkType );
  
  /// Write the end chunk to disk
  void WriteEndChunk( std::shared_ptr<io::FilePtr> & outStream );
  
  /// Read the end chunk from disk
  void ReadEndChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize );

  /// Read the chunk type from disk
  std::string ReadChunkType( std::shared_ptr<io::FilePtr> & inStream ) const;

  /// Verify the checksum from disk
  void VerifyChecksum( std::shared_ptr<io::FilePtr> & inStream, const std::string & sChunkType, MemoryBuffer & memoryBuffer ) const;
  
  /// Write a chunk to disk
  void WriteChunk( std::shared_ptr<io::FilePtr> & outStream, const uint4 uBufferLength, const std::string & sChunkType, uint1 * pBuffer, const CompressionTypeEnum eCompression ) const;
  
  /// Write a chunk in blocks
  void WriteChunkInBlocks( std::shared_ptr<io::FilePtr> & outStream, MemBlockInfo * pMemBlockInfo, const uint4 uNumMemBlocks, const std::string & sChunkType, const CompressionTypeEnum eCompression ) const;
  
  /// Read a chunk from disk
  void ReadChunk( std::shared_ptr<io::FilePtr> & inStream, const std::string & sChunkType, MemoryBuffer & memBuffer );

  /// Skip over a chunk
  void SkipChunk( std::shared_ptr<io::FilePtr> & inStream, const uint4 uChunkSize ) const;
  
  /// copy a chunk from stream to stream
  void CopyChunk( std::shared_ptr<io::FilePtr> & inStream, std::shared_ptr<io::FilePtr> & outStream, const uint4 uChunkSize, const std::string & sChunkType );
  
  /// Read and verify a chunk
  void ReadAndVerifyData(std::shared_ptr<io::FilePtr> & inStream, const std::string & sChunkType, const uint4 uDiskBufferLength, MemoryBuffer & memBuffer);
  
  /// Inflate a memory buffer using LZO compression
  void InflateLZO( MemoryBuffer & destBuffer, MemoryBuffer & srcBuffer );
  
  /// Inflate a memory buffer using ZLIB compression
  void InflateZLIB( MemoryBuffer & destBuffer, MemoryBuffer & srcBuffer );

  /// Swap the byte order of a unit4
  void SwapUint4ByteOrder( uint4 & uValue ) const;
  
  /// Pack a variable into a string
  std::string PackVariable( const Variable & variable ) const;
  
  /// Unpack a variable from a string
  Variable UnpackVariable( const std::string & sString ) const;

  /// Write a unit4 to a file in raw format
  void WriteUintRaw( std::shared_ptr<io::FilePtr> & outStream, const uint4 value ) const;
  
  /// Read a uint4 from a file in raw format
  uint4 ReadUintRaw( std::shared_ptr<io::FilePtr> & inStream ) const;

  /// Initialize the icon to the new Viatronix "V".
  void InitIcon();
  
  /// performs an interlacing buffer copy
  void CopyBufferInterleaved( uint1 * & pDest, uint1 * & pSrc, bool bInterleaveDest ) const;
  
private:

  /// Copy constructor (unimplemented).
  V3DFile( const V3DFile & other );
  
  /// Assignment operator (unimplemented).
  V3DFile & operator=( const V3DFile & other );
  
// Data Members
private:

  /// The version of the file on disk.
  std::pair< int4, int4 > m_fileVersion;
  
  /// The contents of the PNG signature.
  std::vector< uint1 > m_vSignature;
  
  /// The list of variables.
  std::list< Variable > m_varlist;

  /// The file title.
  std::string m_sTitle;

  /// The file comment.
  std::string m_sComment;
  
  /// The file description.
  std::string m_sDescription;
  
  /// The RGB image.
  Image< PixelRGB<uint1> > m_image;
  
  /// The RGBA image.
  Image< PixelRGBA<uint1> > m_imageRGBA;
  
  /// The zlib compressed image data.
  MemoryBuffer m_compressedImageData;

  /// The data memory buffer to use if no external one is supplied.
  MemoryBuffer m_privateBuffer;

  /// The raw data.
  MemoryBuffer * m_pDataBuffer;

  /// Whether or not to skip reading the actual data (for reading just the header/text info).
  bool m_bSkipData;
  
  /// Whether or not to skip reading the image data (for reading just the header/text info).
  bool m_bSkipImage;

  /// If it is an RGBA image (with alpha) or not.
  bool m_bAlphaImage;

  /// The percentage of the total processing time that reading or writing comprises
  std::pair< float4, float4 > m_progressPair;

  /// a list of extra chunks
  std::list< std::pair< std::string, MemoryBuffer > > m_extraChunks;
  
}; // class V3DFile


#endif // V3DFile_h


// $Log: V3DFile.h,v $
// Revision 1.4  2006/03/29 19:39:47  geconomos
// promoted visibility of ReadDataChunk() to protected
//
// Revision 1.3  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.2  2005/07/01 12:29:26  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.17  2003/06/25 19:09:30  geconomos
// Corrected code to read and write data chunk for voxel sizes other than 16-bit.
//
// Revision 3.16  2003/05/20 15:22:55  frank
// Added progress metering to the write process
//
// Revision 3.15  2003/05/20 14:01:56  frank
// Moved V3DFile test to test environment
//
// Revision 3.14  2003/05/20 13:28:12  mkontak
// Coding standards
//
// Revision 3.13  2003/05/19 16:53:59  frank
// code review
//
// Revision 3.12  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.11  2003/05/15 19:05:37  dongqing
// code review
//
// Revision 3.10  2003/04/18 15:59:33  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 3.9  2003/04/14 16:41:31  frank
// Separated progress percent and progress message interface, added progress to volume loading.
//
// Revision 3.8  2002/08/14 16:09:28  wenli
// initial version - Vascular
//
// Revision 3.7  2002/06/17 22:37:42  soeren
// added new compression scheme
//
// Revision 3.6  2002/06/05 19:13:33  soeren
// Added Homogenous Flag
//
// Revision 3.5  2002/05/06 23:21:25  soeren
// added Backgroundflag to store
//
// Revision 3.4  2002/05/02 22:22:16  ingmar
// added progressCallback function to Read() of 2.0 FileVersion files
//
// Revision 3.3  2002/04/18 15:56:10  ingmar
// enabled optmized read and write of new vxBlockVolume
//
// Revision 3.2  2002/03/11 16:30:34  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.1.2.1  2002/02/27 00:41:44  kevin
// Tracker ID 1864
// Added File COpy routines so that anonymization does not have to read the volume completely
// and unmopress it just to write it back out. Now it just copies the chunks without
// even dealing with wheter they are ciompressed or not
//
// Revision 3.1  2001/12/24 15:36:55  ingmar
// added class Sec_20_image to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 04 2001 14:58:22   frank
// Removed const enums in header file for gcc.
// 
//    Rev 2.2   Sep 24 2001 19:46:48   frank
// Coding conventions.
// 
//    Rev 2.1   Sep 21 2001 12:22:08   pagron
// put functionality to set version number on a file
// 
//    Rev 2.0   Sep 16 2001 23:47:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:52   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:46  soeren
// renamed image directory
//
// Revision 1.26  2001/08/25 04:50:01  frank
// Revamped file versions.
//
// Revision 1.25  2001/08/22 16:44:09  frank
// Synchronized exceptions with implementation file.
//
// Revision 1.24  2001/08/20 19:03:09  frank
// Worked on incorporating RGBA images.
//
// Revision 1.23  2001/07/24 22:30:30  soeren
// no message
//
// Revision 1.22  2001/07/13 11:52:06  frank
// Moved decoding functions to ScanlineFilter class.
//
// Revision 1.21  2001/07/12 18:30:31  frank
// Made it possible to use an external buffer to avoid the intermediate uncompressed buffer.
//
// Revision 1.20  2001/07/12 17:08:52  frank
// Switched from C++ file streams to the std::shared_ptr<io::FilePtr> class.
//
// Revision 1.19  2001/07/12 16:23:32  frank
// Removed old icon.
//
// Revision 1.18  2001/07/12 15:06:26  frank
// Handled split image data over multiple IDAT chunks.
// Implemented four image filtering operations.
//
// Revision 1.17  2001/07/10 19:34:54  frank
// Implemented skipping of chunks and added new Viatronix icon.
//
// Revision 1.16  2001/07/10 17:13:24  frank
// Reorganized to read any organization of PNG file.
//
// Revision 1.15  2001/07/10 13:19:28  frank
// Added comments.
//
// Revision 1.14  2001/07/09 16:16:45  frank
// Now reads/writes PNG and lin vol files.
//
// Revision 1.13  2001/07/09 15:36:35  frank
// Worked on compression.
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
// Revision 1.7  2001/06/25 21:20:34  geconomos
// disabled warning for excess debug info
//
// Revision 1.6  2001/06/24 21:01:53  frank
// Fixed basic PNG chunk I/O, got it working for simple files.
//
// Revision 1.5  2001/06/23 20:40:37  frank
// Working on PNG format.
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
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/V3DFile.h,v 1.4 2006/03/29 19:39:47 geconomos Exp $
// $Id: V3DFile.h,v 1.4 2006/03/29 19:39:47 geconomos Exp $
