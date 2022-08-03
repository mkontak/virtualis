// $Id: LocalFilePtr.C,v 1.5 2006/07/06 13:27:45 romy Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Soeren soeren@viatronix.net


// Includes
#include "stdafx.h"
#include "LocalDirPtr.h"



using namespace io;


/**
 * Constructor
 *
 * @param sFilename the filename
 * @param sMode     the file mode
 */
LocalDirPtr::LocalDirPtr( const std::string & sPath ) :
DirPtr(sPath)
{

} // LocalFilePtr()



/**
 * Destructor
 */
LocalDirPtr::~LocalDirPtr()
{
  
} // ~LocalFilePtr()


/**
 * Creates the Local file ptr object
 *
 * @param sFilePath
 *
 * @return LocalFilePtr
 */
std::shared_ptr<LocalDirPtr> LocalDirPtr::Create(const std::string & sPath)
{
  std::shared_ptr<LocalDirPtr> pDirPtr = std::shared_ptr<LocalDirPtr>( new LocalDirPtr(sPath) );

  if ( pDirPtr == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate LocalDirPtr object", "LocalDirPtr", "Create"));

  return pDirPtr;

} // Create()

/**
 * Gets files from directory
 *
 * @Param sPattern      Pattern to be matched
 */
std::vector<std::string> LocalDirPtr::GetFiles(const std::string & sPattern)
{

  return util::IO::GetFiles(m_sPath, sPattern);
} // GetFiles()

/**
 * Gets dirs from directory
 *
 * @Param sPattern      Pattern to be matched
 */
std::vector<std::string> LocalDirPtr::GetDirs(const std::string & sPattern)
{

  return util::IO::GetDirs(m_sPath, sPattern);
} // GetDirs()


/**
 * Determines if the directory exists 
 *
 * @param sPath      Path to be checked
 *
 * @return  True if the directory exists, false otherwise
 */
const bool LocalDirPtr::Exists( const std::string & sPath)
{

  bool bExists(false);


  if ( ! sPath.empty() )
  {
#ifdef _WIN32
      bExists = (_access(sPath.c_str(), 0) == 0);
#else
      bExists = (access(sPath.c_str(), F_OK) == 0);
#endif
  }

  return bExists;

}


