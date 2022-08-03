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

#include "DirPtr.h"


namespace io 
{

/// Forward declarations
class VX_BASE_DLL TaskGroup;

/**
 * Local directory pointer   
 */
class VX_BASE_DLL LocalDirPtr : public DirPtr
{


#pragma region constructors/destructors

public:

  /// Constructor
  LocalDirPtr( const std::string & sPath );

 

  /// Destructor, calls Close() if needed
  virtual ~LocalDirPtr();

#pragma endregion


#pragma region methods

public:

  /// Creates the local file ptr
  static std::shared_ptr<LocalDirPtr> Create(const std::string & sPath);


  /// Gets files
  virtual std::vector<std::string> GetFiles(const std::string & sPattern = "*");


  /// Gets directories
  virtual std::vector<std::string> GetDirs(const std::string & sPattern = "*");
 
  /// Determines of the file exists
  virtual inline const bool Exists()
  { return Exists(m_sPath); }
 
  /// Determines the existence of the directory
  static const bool Exists( const std::string & sPath);

#pragma endregion


}; // class LocalDirPtr

} // namespace  io
