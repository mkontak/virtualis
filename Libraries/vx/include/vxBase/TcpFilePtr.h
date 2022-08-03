#pragma once



#include "FilePtr.h"
#include "FileServerHeader.h"


namespace io
{


// Forward declarations
class VX_BASE_DLL TcpClient;



/**
 * Implements a tcp network file ptr
 */
class VX_BASE_DLL TcpFilePtr : public FilePtr
{


#pragma region constructors/destructors

public:

  /// Constructor
  TcpFilePtr( const std::string & sFilePath, const uint4 uCacheSize = 0  );


  /// Destructor, calls Close() if needed
  virtual ~TcpFilePtr();

#pragma endregion


#pragma region methods

public:

  /// Creates the TcpFilePtr object
  static std::shared_ptr<TcpFilePtr> Create(const std::string & sFilePath, const uint4 uCacheSzie = 0);

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

private:

  inline void ReceiveResponse(FILESERVERHEADER & sender, FILESERVERHEADER & receiver , bool validate = false )
  { ReceiveResponse(sender, receiver, m_pTcpClient, validate); }

  void ReceiveResponse(FILESERVERHEADER & sender, FILESERVERHEADER & receiver, std::shared_ptr<TcpClient> & client,  bool validate = true);

#pragma endregion

private:

  // Tcp client allows access to the server
  std::shared_ptr<TcpClient> m_pTcpClient;


}; // class NetworkFile

} // namespace io

