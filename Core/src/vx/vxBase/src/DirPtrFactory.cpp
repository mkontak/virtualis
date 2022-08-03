// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DirPtrFactory.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "stdafx.h"
#include "DirPtrFactory.h"
#include "DirPtr.h"
#include "vxTransportLayer.h"
#include "TcpDirPtr.h"
#include "LocalDirPtr.h"
#include "HttpDirPtr.h"

using namespace io;

/**
 * Creates the DirPtr
 */
std::shared_ptr<DirPtr> DirPtrFactory::CreateDirPtr(const std::string & sPath)
{

  std::shared_ptr<DirPtr> dirPtr = __nullptr;

  switch ( TransportLayer::GetCurrentTransportLayer().GetUrl().GetProtocol() )
  {
  case Protocols::P_TCP:
    dirPtr = TcpDirPtr::Create(sPath );
    break;
  case Protocols::P_HTTP:
    dirPtr = HttpDirPtr::Create(sPath);
    break;
  default:
    dirPtr = LocalDirPtr::Create(sPath );
  }

  return dirPtr;

} // Craate()


/**
 * Determines if the directory exists
 *
 * @param sPath     Path
 */
bool DirPtrFactory::Exists(const std::string & sPath)
{
  std::shared_ptr<DirPtr> dirPtr = CreateDirPtr(sPath);

  return dirPtr->Exists();


} // IsDirExists(const std::string sPath)



/**
 * Gets the list of files for the path specified matching the pattern specified
 *
 * @param sPath     Path
 * @param sPatterm  Pattern to be matched
 *
 * @return list of directories
 */
std::vector<std::string> DirPtrFactory::GetFiles(const std::string & sPath, const std::string & sPattern)
{
  std::shared_ptr<DirPtr> dirPtr = CreateDirPtr(sPath);

  return dirPtr->GetFiles(sPattern);


} // :GetFiles(const std::string & sPath, const std::string & sPattern)


/**
 * Gets the list of files for the path specified matching the pattern specified
 *
 * @param sPath     Path
 * @param sPatterm  Pattern to be matched
 *
 * @return list of directories
 */
std::vector<std::string> DirPtrFactory::GetDirs(const std::string & sPath, const std::string & sPattern)
{
  std::shared_ptr<DirPtr> dirPtr = CreateDirPtr(sPath);

  return dirPtr->GetDirs(sPattern);


} // :GetDirs(const std::string & sPath, const std::string & sPattern)
