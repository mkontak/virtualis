#pragma once



#include "FilePtr.h"
#include "FileServerHeader.h"

#define NOCONTEXT ""


/// Forward Declartions
class VX_BASE_DLL Buffer;
class VX_BASE_DLL MemoryStream;

namespace http
{
/// Forward declaration
class VX_BASE_DLL HttpClient;
class VX_BASE_DLL HttpRequest;
class VX_BASE_DLL HttpResponse;
}

namespace io
{


// Forward declarations
class VX_BASE_DLL HttpDirPtr;

/**
 * Implements a tcp network file ptr
 */
class VX_BASE_DLL HttpFilePtr : public FilePtr
{


#pragma region constructors/destructors

public:

  /// Constructor
  HttpFilePtr( const std::string & sFilePath, const uint4 uCacheSize = 0  );


  /// Destructor, calls Close() if needed
  virtual ~HttpFilePtr();

#pragma endregion


#pragma region methods

public:

  /// Creates the TcpFilePtr object
  static std::shared_ptr<HttpFilePtr> Create(const std::string & sFilePath, const uint4 uCacheSzie = 0);

  /// Opens a file, modes are identical to C-style modes for fopen()
  virtual  void Open( const int4 iModeFlags, const int4 iCreateFlags );

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

  /// Determines of the file is open
  virtual  inline const bool IsOpen() ;

  /// Determines of the file exists
  virtual const bool Exists()
  { return Exists(m_sFilePath); }

  /// Delets the file
  virtual const bool Delete();

  /// Static Exists function
  static const bool Exists(const std::string & sFilePath);

protected:

  ///  Reads the file directly
  virtual uint4 ReadFile(void * pBuffer, const uint4 uBytesToRead);

  /// Returns the resource used
  static const std::string & GetResource() 
  { return m_sResource; }

private:

  /// Validates the response checking for exceptions
  static void ValidateResponse(const std::shared_ptr<http::HttpRequest> & request, const std::shared_ptr<http::HttpResponse> & response, const std::string & sFilePath);
  static void  ValidateResponse(const std::shared_ptr<Buffer> & request, const std::shared_ptr<Buffer> & response, const std::string & sFilePath );

#pragma endregion

private:

  /// Resource
  static std::string m_sResource;

  /// CLient connection
  std::shared_ptr<http::HttpClient> m_pHttpClient;


  /// Open flag
  bool m_bIsOpen;



  friend VX_BASE_DLL HttpDirPtr;

}; // class NetworkFile

} // namespace io
