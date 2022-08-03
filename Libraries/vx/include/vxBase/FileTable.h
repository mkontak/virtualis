// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FileTable.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once


namespace io
{

// Forward declarations
class VX_BASE_DLL FilePtr;

typedef std::map<std::string, std::shared_ptr<FilePtr>> FILEMAP;
typedef std::pair<std::string, std::shared_ptr<FilePtr>> FILEENTRY;

/**
 * Implements a FilePtr table.
 *
 * This is maintained by the server to indicate which files are opened and being read.
 */
class VX_BASE_DLL FileTable
{
public:

  FileTable() { }

  virtual ~FileTable();

  /// Adds the file ptr
  void Add(std::shared_ptr<FilePtr> & pFilePtr);

  /// Gets the file ptr
  std::shared_ptr<FilePtr> GetFile(const std::string & sHandle);

  /// Removes the specified handle from the table
  void Remove(const std::string & sHansle);

  /// Clears the table
  void Clear()
  { m_files.clear(); }

  /// Generates the XML for the table
  std::string ToXml();

private:

  /// File list
  FILEMAP m_files;

};  // class FileTable

} // namespace io
