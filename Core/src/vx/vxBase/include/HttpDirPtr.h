#pragma once



#include "DirPtr.h"
#include "FileServerHeader.h"



namespace io
{


#define NOCONTEXT ""


// Forward declarations
class VX_BASE_DLL HttpClient;



/**
 * Implements a tcp network dir ptr
 */
class VX_BASE_DLL HttpDirPtr : public DirPtr
{


#pragma region constructors/destructors

public:

  /// Constructor
  HttpDirPtr( const std::string & sPath  );


  /// Destructor, calls Close() if needed
  virtual ~HttpDirPtr();

#pragma endregion


#pragma region methods

public:

  /// Creates the HttpFilePtr object
  static std::shared_ptr<HttpDirPtr> Create(const std::string & sPath);


  /// Gets files
  virtual std::vector<std::string> GetFiles(const std::string & sPattern = "*");


  /// Gets directories
  virtual std::vector<std::string> GetDirs(const std::string & sPattern = "*");
 
  /// Determines of the file exists
  virtual const bool Exists();



#pragma endregion

private:


  /// Resource name
  static std::string m_sResource;

}; // class NetworkFile

} // namespace io

