// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FilePtrFactory.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once


namespace io
{

// Forward declarations
class VX_BASE_DLL DirPtr;

/**
 * FilePtr factory used to abstract the FilePtr from the main application
 */
class VX_BASE_DLL DirPtrFactory
{

public:

  /// Creates the 
  static std::shared_ptr<DirPtr> CreateDirPtr(const std::string & sPath);

  /// Determines the existences of the file
  static bool Exists(const std::string & sPath);

  /// Gets the files within the path
  static std::vector<std::string> GetFiles(const std::string & sPath, const std::string & sPatterm = "*");


  /// Gets the files within the path
  static std::vector<std::string> GetDirs(const std::string & sPath, const std::string & sPatterm = "*");

};  // class DirPtrFactory

} //namespace io