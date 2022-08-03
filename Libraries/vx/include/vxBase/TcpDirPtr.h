#pragma once



#include "DirPtr.h"
#include "FileServerHeader.h"



namespace io
{


// Forward declarations
class VX_BASE_DLL TcpClient;



/**
 * Implements a tcp network dir ptr
 */
class VX_BASE_DLL TcpDirPtr : public DirPtr
{


#pragma region constructors/destructors

public:

  /// Constructor
  TcpDirPtr( const std::string & sPath  );


  /// Destructor, calls Close() if needed
  virtual ~TcpDirPtr();

#pragma endregion


#pragma region methods

public:

  /// Creates the TcpFilePtr object
  static std::shared_ptr<TcpDirPtr> Create(const std::string & sPath);


  /// Gets files
  virtual std::vector<std::string> GetFiles(const std::string & sPattern = "*");


  /// Gets directories
  virtual std::vector<std::string> GetDirs(const std::string & sPattern = "*");
 
  /// Determines of the file exists
  virtual const bool Exists();


private:

 void ReceiveResponse(FILESERVERHEADER & sender, FILESERVERHEADER & receiver, std::shared_ptr<TcpClient> & client);

#pragma endregion


}; // class NetworkFile

} // namespace io

