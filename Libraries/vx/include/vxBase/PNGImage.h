// $Id: PNGImage.h,v 1.2 2006/02/02 20:02:38 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: David M. 
//
// Complete History at bottom of file.

/**
 * Class for storing RGB images and data in the PNG format.  
 * Allows for reading, storing, writing of an image with text chunks or user 
 * defined chunks.  
 * For each instance of PNGImage, only one Read() or Write()
 * should be performed and they should not be used simultaneously.  Declare a
 * new instance if you want to Read multiple files.
 * Currently uses RGB format with bit depth of 8, could be changed if needed 
 * to RGBA or larger bit depth or ?? -- see png.h or png web page.
 * Maintainers should read the PNG documentation before attempting much modification.
 * \bug GetPixel(), SetPixel() methods will only work with certain types of images and not others
 */

#ifndef PNGImage_h
#define PNGImage_h



// include declarations
#include <png.h>
#include "Triple.h"

// class definition
class VX_BASE_DLL PNGImage
{
public:
  /// Constructor.
  PNGImage(bool bKeepUnknown = true, void * pDataPtr = NULL);
  /// Constructor.
  PNGImage(const uint4 & uWidth, const uint4 & uHeight, int4 iBitDepth = 8, 
    int4 iColorType = PNG_COLOR_TYPE_RGB, const uint1 * pBuffer = NULL, uint4 uBufSize = 0);
  /// Copy Constructor.
  PNGImage(const PNGImage & rhs);
  /// Assignment operator.
  PNGImage const & operator=(const PNGImage & rhs);
  /// Destructor.
  ~PNGImage();

  /// Returns the image width.
  uint4 GetWidth() const;
  /// Returns the image height.
  uint4 GetHeight() const;
  /// Returns the bit depth
  const int4 & GetBitDepth() const;
  /// Returns the color type.
  const int4 & GetColorType() const;

  /// Get, Set routines need re-thinking if this will be used for more types
  /// maybe templatized version, currently bad things could happen if wrong method invoked

  /// Gets the pixel at (x,y)
  const Triple<uint1> GetPixel(uint4 uX, uint4 uY) const;
  /// Sets the pixel at (x,y)
  void SetPixel(uint4 uX, uint4 uY, const Triple<uint1> & color);
  /// only used with PNG_COLOR_TYPE GRAY, bitDepth 8
  void SetPixel(uint4 uX, uint4 uY, const uint2 & uDensity);

  // Functions for I/O

  /// Reads a PNG image stored in RGB format
  void Read(const std::string & sFilename);
  /// Writes the PNG image to the specified file.
  void Write(const std::string & sFilename);

  /// Returns the text chunk with the given name or empty string
  const std::string & GetText(const std::string & sTextName) const;
  /// Sets a png text chunk, multiple chunks are okay, don't bother compressing
  /// unless more than 5000 bytes.
  void SetText(const std::string & sTextName, const std::string & sText,  bool bCompress = false);

  /// Sets a chunk for png to write. 
  void SetChunk(uint1 vName[3], uint1 * pData, uint4 uDataLength, uint4 uCompType);
  /// Attempt to retrieve information about a PNG chunk.
  bool GetChunk(std::string sName, uint1 ** hData, uint4 & uDataLength);
  /// Can be used to force early deletion of png_chunks after GetChunk() on a read,
  /// else cleaned up in destructor
  void DeleteChunks();

  /// Returns the pair<major,minor> version number of the most current version.
  const std::pair<int4,int4> & GetLatestVersion() const;
  /// Overrides the default version number when writing a file.
  void SetLatestVersion(const std::pair<int4,int4> & fileVersion);
  /// Returns the version for most recent file read from disk, zero,zero if no files read.
  const std::pair<int4,int4> & GetFileVersion() const;

protected:
  /// Replaces fwrite() in libpng writing
  static void WriteData(png_structp png_ptr, png_bytep data, png_size_t length);
  /// Flushes the stream in libpng writing
  static void WriteFlushData(png_structp png_ptr);
  /// Replaces fread() in libpng so the poor Windows network can work
  static void ReadData(png_structp png_ptr, png_bytep data, png_size_t length);
  /// Copies all image data.
  void Copy(PNGImage const & rhs);
  /// destroys all image data.
  void Destroy();
  // returns pointers to the image array
  std::vector<uint1 *> GetRowPointers() const;
  /// function called by libpng for reading unknown chunks. (not currently used)
  static int4 ReadChunkCallback(png_structp pngPtr, png_unknown_chunkp chunk);
  /// Stores the version number read from file.
  void StoreVersionNumber(const std::string & sText); 
  /// Function called by LIBPNG if compiled with PNG_SETJMP_NOT_SUPPORTED
  static void LibPNGError(png_structp pngPtr, png_const_charp errorMsg);
  /// Initialize the viatron icon
  void InitIcon();

protected: // data
  std::pair<int4,int4> m_fv;      // current file version, no minor needed 1.1.1 ??
  std::pair<int4,int4> m_fvReadIn;

  // png_uint_32 is unsigned long in Windows, need to convert from our types at some point
  png_uint_32 m_uWidth, m_uHeight, m_uRowBytes; 
  int4 m_iBitDepth, m_iColorType; // int type in png library
  std::vector<uint1> m_vData;     // the image data, by default a cute little viatronix V
  std::string m_sEmptyStr;        // the empty string stored for when text chunk lookup fails
  png_structp m_pngPtr;           // for de-allocation of unknown chunks

  
  class textChunk
  {
  public:
    std::string sName;
    std::string sData;
    int4 iCompression;
  };
  std::list<textChunk> m_text;
  
  bool m_bKeepUnknown;  // will ignore all unknown chunks (3D data chunk for Vx volumes)
  void * m_pData;    // 
  std::list<png_unknown_chunk> m_chunks;
};

#endif

// $Log: PNGImage.h,v $
// Revision 1.2  2006/02/02 20:02:38  geconomos
// added considerations for 64-bit builds
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.7  2003/01/06 12:52:53  jmeade
// Removed reference return in GetPixel(), which returns local memory.
//
// Revision 3.6  2001/01/02 14:13:12  ingmar
// added missing include
// code beautify
//
// Revision 3.5  2001/01/02 14:01:42  ingmar
// removed explicit throw on function interface
// added missing includes
//
// Revision 3.4  2002/12/23 22:50:38  ingmar
// removed redundant use of Triple copy constructor
//
// Revision 3.3  2002/07/18 16:33:44  mkontak
// Added VXDLL to PNGImage
//
// Revision 3.2  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.1  2001/12/24 15:36:55  ingmar
// added class Sec_20_image to structure the doxygen documentation
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
//    Rev 1.0   Sep 16 2001 17:34:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:11:45  soeren
// renamed image directory
//
// Revision 1.24  2001/05/30 22:42:49  dave
// C++ streams for I/O
//
// Revision 1.23  2001/04/25 19:28:13  dave
// closer standards compliance, more comments at George's request
//
// Revision 1.22  2001/04/19 20:59:40  dave
// updated variable names
//
// Revision 1.21  2001/04/12 21:30:03  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.20  2001/04/12 18:52:27  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.19  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.18  2001/04/12 14:37:52  dmitry
// Sec_5_image base class added
//
// Revision 1.17  2001/04/10 16:13:49  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.16  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.15  2001/04/05 18:00:14  dmitry
// updated datatypes
//
// Revision 1.14  2001/03/28 20:06:54  dmitry
// Update typedef, implemented proper casting
//
// Revision 1.13  2001/03/12 18:25:56  dave
// updated header
//
// Revision 1.12  2001/03/05 15:28:43  dave
// overloaded constructor for image buffers
//
// Revision 1.11  2001/02/06 02:24:43  dave
// added hack for reading LinearVolume
//
// Revision 1.10  2001/01/15 17:38:41  dave
// corrected Gray SetPixel, added icon but it needs fixin'
//
// Revision 1.9  2001/01/08 19:23:51  dave
// added constructor argument
//
// Revision 1.8  2000/12/14 05:25:52  dave
// changes from am meeting
//
// Revision 1.7  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.6  2000/11/22 17:14:58  binli
// fixed display list for strips rendering.
//
// Revision 1.5  2000/11/22 06:42:39  dave
// Rewrote to make sexier
//
// Revision 1.4  2000/11/13 18:07:01  ingmar
// Added GetPixel
//
// Revision 1.3  2000/11/03 02:06:28  ingmar
// Resize and operator <<
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
//
// 6     9/04/00 11:10p Ingmar
// now works with new naming sceme
//
// 5     8/22/00 5:39p Jay
// SetPixel function added
//
// 4     8/21/00 6:48p Antonio
// updated header
//
// 3     8/21/00 1:11a Sarang
// modified header and footer
//
// *****************  Version 2  *****************
// User: Antonio      Date: 8/11/00    Time: 1:15p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:03a
// Created PNGImage.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/PNGImage.h,v 1.2 2006/02/02 20:02:38 geconomos Exp $
// $Id: PNGImage.h,v 1.2 2006/02/02 20:02:38 geconomos Exp $
