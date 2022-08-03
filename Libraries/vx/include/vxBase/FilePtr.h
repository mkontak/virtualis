#pragma once


namespace io
{


// Forward declarations
class VX_BASE_DLL FileCache;
class VX_BASE_DLL TaskGroup;



/**
 * Implements an FIlePtr abstract class
 */
class VX_BASE_DLL FilePtr 
{


#pragma region constructors/destructors

protected:

  /// Constructor
  FilePtr(const std::string & sFilePath, const uint4 uCacheSize);

 

#pragma endregion


#pragma region methods

public:

  /// 
  virtual ~FilePtr();


  /// Opens a file, modes are identical to C-style modes for fopen()
  virtual void Open( const int4 iModeFlags, const int4 iCreateFlags ) = 0;

  // Closes the file
  virtual void Close()
  { ClearCache(); }

  /// Reads data from the file
  virtual uint4 Read( void * pBuffer, const uint4 uBytesToRead ) = 0;

  /// Writes data to the file
  virtual void Write( const void * pBuffer, const uint4 uBytesToWrite ) = 0;

  /// Flush the file to disk
  virtual void Flush() = 0;

  /// Seek the file pointer ahead from the current position
  virtual bool SeekCurrent( const __int64  uNumBytes ) = 0;

  /// Returns whether we have reached the end of the file ( EOF and cache is empty), whoulc be overwritten
  virtual const bool IsEof()
  { return ( m_bEof && m_uCacheBytesRemaining == 0 ); };

  /// Checks if we have reached EOF. This function should be preferred to IsEof until and unless the programmer is sure about using IsEof.
  virtual const bool IsEofSmart() = 0;
  
  /// returns the size of the file
  virtual const uint4  Size() = 0;

  /// Determines of the file is open
  virtual const bool IsOpen() = 0;

  /// Determines of the file exists
  virtual const bool Exists() = 0;

  /// Delets the file
  virtual const bool Delete() = 0;

protected:


  /// Reads from the file directly without any buffering (Only check the if no caching then set the EOF and File EOF)
  virtual uint4 ReadFile(void * pBuffer, const uint4 uBytesToRead ) = 0;

  /// Loads the cache in a task
  void ReadAhead(bool refresh = false);

  /// Read data from the cache
  uint4 ReadCache(void * pBuffer, const uint4 uBytesToRead);
 
  /// Cleae the cache
  inline void ClearCache()
  { m_uCacheBytesRemaining = m_uCacheBytesRead = 0; }

#pragma endregion

#pragma region properties

public:


  /// Gets the file handle
  inline const std::string & GetHandle() const
  { return m_sHandle; }
  
  /// Gets the file path
  inline const std::string & GetFilePath()  const
  { return m_sFilePath; }

    /// Gets the file name
  inline const std::string & GetFilename()  const
  { return m_sFilename; }

  /// True if buffering is enabled
  inline const bool IsCachingEnabled() const
  { return (m_uCacheSize > 0); }

  /// Gets the total bytes read
  inline const __int64 GetTotalBytesRead() const
  { return m_uTotalBytesRead; }
  
  /// Gets the total bytes written
  inline const __int64 GetTotalBytesWritten() const
  { return m_uTotalBytesWritten; }

  // Gets the current buffering size
  inline const uint4 GetCacheSize() const
  { return m_uCacheSize; }

  /// Gets the file position
  inline const __int64 GetFilePosition() const
  { return m_uFilePosition; }

#pragma endregion


#pragma region fields

// Data Members
protected:

  /// Handle
  std::string m_sHandle;

  /// File path
  std::string m_sFilePath;

  /// Just the file name
  std::string m_sFilename;

  /// Asyn tasks
  std::shared_ptr<Task> m_task;

  /// Total bytes read
  __int64 m_uTotalBytesRead;

  /// File position
  __int64 m_uFilePosition;

  /// Total bytes written
  __int64 m_uTotalBytesWritten;

  /// End of file flag (Only set if we have gone beyond the file)
  bool m_bEof;

  /// Cache
  std::shared_ptr<char> m_cache;

  /// Cache size
  uint4 m_uCacheSize;

  /// Cache bytes read
  uint4 m_uCacheBytesRead;

  /// Caches bytes remaining
  uint4 m_uCacheBytesRemaining;



#pragma endregion

}; // class FilePtr

} // namespace io