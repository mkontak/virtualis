// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FileTable.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "stdafx.h"
#include "FileTable.h"
#include "FilePtr.h"
#include "Exception.h"

using namespace io;



/**
 * Destructor
 */
FileTable::~FileTable()
{

} // ~FileTable

/**
 * Adds the file point object to list
 */
void FileTable::Add(std::shared_ptr<FilePtr> & pFilePtr)
{
  if ( m_files.find(pFilePtr->GetHandle()) == m_files.end() ) 
  {

    LogFyi(util::Strings::Format("Adding %s - %s to file table",pFilePtr->GetHandle().c_str(), pFilePtr->GetFilePath().c_str()), "FileTable", "Add");

    m_files.insert(FILEENTRY(pFilePtr->GetHandle(), pFilePtr));

  }
  else
    LogWrn(util::Strings::Format("File %s (%s) already exists in the table",pFilePtr->GetHandle().c_str(), pFilePtr->GetFilePath().c_str()), "FileTable", "Add");
    
} // Add(FilePtr & filePtr)

/**
 * Gets the file ptr
 *
 * @parm sHandle      File handle
 */
std::shared_ptr<FilePtr> FileTable::GetFile(const std::string & sHandle)
{
  if ( m_files.find(sHandle) != m_files.end() ) 
    return m_files[sHandle];
  else
    throw ex::FileNotFoundException(LogRec(util::Strings::Format("File (%s) does not exists in the table",sHandle.c_str()),"FileTable", "GetFile"));
 
    
} // GetFile(const std::string & sHandle)


/**
 * Adds the file point object to list
 */
void FileTable::Remove(const std::string & sHandle)
{

  try
  {
    std::shared_ptr<FilePtr> filePtr = GetFile(sHandle);
   
    LogFyi(util::Strings::Format("Removing %s - %s from file table",filePtr->GetHandle().c_str(), filePtr->GetFilePath().c_str()), "FileTable", "Remove");

    m_files.erase(sHandle);
  }
  catch ( ex::FileNotFoundException )
  {
    LogWrn(util::Strings::Format("File %s does not exists in the table, cannot remove",sHandle.c_str()),"FileTable", "GetFile");
  }

    
} // Remove(const std::string & sHandle)


/**
 * Generates the file table as xml
 */
std::string FileTable::ToXml()
{
  std::stringstream os;


  os << "<table>";

  std::for_each(m_files.begin(), m_files.end(), [&os] ( std::pair<std::string,std::shared_ptr<FilePtr>> item ) 
  {
    os << "<file handle=\"" << item.second->GetHandle();
    os << " filepath=\"" << item.second->GetFilePath();
    os << " read=\"" << item.second->GetTotalBytesRead();
    os << " written=\"" << item.second->GetTotalBytesWritten();
    os << " eof=\"" << ( item.second->IsEof() ? "true" : "false" );
    os << " cache=\"" << item.second->GetCacheSize();
    os << "/>";
  });

  os << "</table>";

  return os.str();
}   // ToXml()