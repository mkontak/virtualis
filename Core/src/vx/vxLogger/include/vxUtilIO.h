// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxUtilIO.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


// namespace
namespace util
{


 /**
  * Implements IO utilities
  */
  class  VX_LOGGER_DLL  IO
  {

  public:

  /**
   * Split Path Parts
   */
    typedef enum PathParts
    {
      DRIVE = 0,
      DIRECTORY = 1,
      FILE = 2,
      EXTENSION = 3
    } PathParts;

  public:

    /// Get the app data folder
    static std::string GetAppDataFolder();

    /// Gets the file name from the specified file path
    static std::string GetFilename(const std::string & sFilePath);

    /// Gets the file name without the extension
    static std::string GetFilenameWithoutExt(const std::string & sFilePath);

    /// SPlits the path and returns (drive, directory, filename and extension)
    static std::vector<std::string> SplitPath(const std::string & sFilePath);

    /// Creates the directory of the path specified
    static void CreateDirectory(const std::string & sFilePath);

    /// Gets the default log directory
    static std::string GetDefaultLogDirectory();

    /// Gets the default viatronix directory
    static std::string GetDefaultViatronixDirectory();

    /// Determines existence
    static bool Exists(const std::string & sFilePath);

    /// Gets the file list for the directory specified
    static std::vector<std::string> GetFiles(const std::string & sDirectory, const std::string & sPattern = "*");

    /// Gets the dir list for the directory specified
    static std::vector<std::string> GetDirs(const std::string & sDirectory, const std::string & sPattern = "*");

    /// Combines 2 paths
    static std::string Combine(const std::string & sPath1, const std::string & sPath2);


  }; // class IO

};  // namespace util

#pragma warning(pop)