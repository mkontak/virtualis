// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FilePtr.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

#include "FilePtr.h"


namespace io 
{

/// Forward declarations
class VX_BASE_DLL TaskGroup;

/**
 * Class  
 * 1) to correct bug in network reads for Windows 2000
 * 2) to encapsulate C style reading for C++ as recommended by Stroustrup
 *
 * Choices for file I/O w/VC++ & Windows 2000 are fread, C++ streams, or ReadFile()
 * 
 * fread() and ReadFile() have nearly identical performance over the network
 * AND fail seemingly arbitrarily when attempting to read files over 60 MB
 * C++ streams do not crash, however the performance is horrible (more than 3x slower).
 *
 * This class uses C-style reading and writing and breaks reads over 60 MB
 * into smaller sizes.
 * Alternatively, C++ streams could be used with similar performance
 * if the class basic_filebuf<> was re-implemented b/c the VC++ implementation is terrible.
 * If greater functionality is needed than this class offers it may indicate that
 * it is time to implement basic_filebuf<> instead of adding functions here.
 * Reference "Standard C++ IOStreams & Locales" to implement.
 * Also, increasing the buffer size for C++ filestreams had no impact on the performance
 * of network reads.
 *
 * With regards to point 2 above, the destructor is automatically called since this is a class
 * Therefore is much more convenient and less error-prone to use in a function with multiple throw() statements
 */
class VX_BASE_DLL LocalFilePtr : public FilePtr
{


#pragma region constructors/destructors

public:

  /// Constructor
  LocalFilePtr( const std::string & sFilePath, const uint4 uCacheSize = 0 );

 

  /// Destructor, calls Close() if needed
  virtual ~LocalFilePtr();

#pragma endregion


#pragma region methods

public:

  /// Creates the local file ptr
  static std::shared_ptr<LocalFilePtr> Create(const std::string & sFilePath, uint4 uCacheSzie = 0);

  /// Opens a file, modes are identical to C-style modes for fopen()
  virtual void Open( const int4 iModeFlag, const int4 iCreateFlags );


  // Closes the file
  virtual void Close();

  /// Reads data from the file
  virtual uint4 Read( void * pBuffer, const uint4 uBytesToRead );

  /// Writes data to the file
  virtual void Write( const void * pBuffer, const uint4 uBytesToWrite );

  /// Flush the file to disk
  virtual void Flush();

  /// Seek the file pointer ahead from the current position
  virtual bool SeekCurrent( const __int64  uNumBytes );

  /// Returns whether we have reached the end of the file, calls feof. See implementation for documentation of potential problem with this function
  virtual const bool IsEof();

  /// Checks if we have reached EOF. This function should be preferred to IsEof until and unless the programmer is sure about using IsEof.
  virtual const bool IsEofSmart();
  
  /// returns the size of the file
  virtual const uint4  Size();

  /// Returns true of the file is open
  virtual inline const bool IsOpen() 
  { return ( m_hFile != INVALID_HANDLE_VALUE ); }

  /// Determines of the file exists
  virtual inline const bool Exists()
  { return Exists(m_sFilePath); }
 
  /// Determines the existence of the specified file
  static const bool Exists(const std::string & sFilePath);

  /// Delets the file specified
  virtual inline const bool Delete()
  { return ( IsOpen() ? false : Delete(m_sFilePath) ); }

  /// Delets the file path specified
  static const bool Delete(const std::string & sFilePath);

  /// Returns true if the file contains a UNC path
  const bool ContainsUNCPath() const;

  /// Returns the UNC systemname if the file contains a UNC path otherwise an empty string is returned
  const std::string GetUNCSystemName() const;

 
protected:

  ///  Reads the file directly
  virtual uint4 ReadFile(void * pBuffer, const uint4 uBytesToRead);


#pragma endregion



#pragma region fields

// Data Members
private:

  /// The underlying file pointer
  HANDLE m_hFile;


 
#pragma endregion

}; // class FilePtr

} // namespace  io
