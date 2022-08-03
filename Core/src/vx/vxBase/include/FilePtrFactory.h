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
class VX_BASE_DLL FilePtr;

/**
 * FilePtr factory used to abstract the FilePtr from the main application
 */
class VX_BASE_DLL FilePtrFactory
{

public:

  /// Creates the 
  static std::shared_ptr<FilePtr> CreateFilePtr(const std::string & sFilePath);

  /// Determines the existences of the file
  static bool Exists(const std::string & sFilePath);


};  // class FilePtrFactpry

} //namespace io