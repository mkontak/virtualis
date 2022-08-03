#pragma once

#include <vector>


namespace io
{


// Forward declarations
class VX_BASE_DLL FileCache;
class VX_BASE_DLL TaskGroup;

/**
 * Implements an FIlePtr abstract class
 */
class VX_BASE_DLL DirPtr 
{


#pragma region constructors/destructors

protected:

  /// Constructor
  DirPtr(const std::string & sPath) : 
  m_sPath( sPath )
  { }

 

#pragma endregion


#pragma region methods

public:

  /// 
  virtual ~DirPtr() { }


  /// Get the list of files within the directory
  virtual std::vector<std::string> GetFiles(const std::string & sPattern = "*") = 0;
 
  /// Gets the list of directories within the specified directory that match the pattern
  virtual std::vector<std::string> GetDirs(const std::string & sPattern = "*") = 0;

  /// Determines if the directory exists
  virtual const bool Exists() = 0;

#pragma endregion


#pragma region properties

public:


  /// Gets the path
  const std::string GetPath() const
  { return m_sPath; }

#pragma endregion

protected:


  /// Directory path
  std::string m_sPath;

}; // class DirPtr

} // namespace io