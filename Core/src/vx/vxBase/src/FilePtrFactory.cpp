// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: FilePtrFactory.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#include "stdafx.h"
#include "FilePtrFactory.h"
#include "FilePtr.h"
#include "vxTransportLayer.h"
#include "TcpFilePtr.h"
#include "LocalFilePtr.h"
#include "HttpFilePtr.h"
#include "vxUrl.h"

using namespace io;

/**
 * Creates the FilePtr
 */
std::shared_ptr<FilePtr> FilePtrFactory::CreateFilePtr(const std::string & sFilePath)
{

  std::shared_ptr<FilePtr> filePtr = __nullptr;

  switch ( TransportLayer::GetCurrentTransportLayer().GetUrl().GetProtocol() )
  {
  case Protocols::P_TCP:
    filePtr = TcpFilePtr::Create(sFilePath, TransportLayer::GetCurrentTransportLayer().GetCacheSize() );
    break;
  case Protocols::P_HTTP:
    filePtr = HttpFilePtr::Create(sFilePath, TransportLayer::GetCurrentTransportLayer().GetCacheSize() );
    break;
  default:
    filePtr = LocalFilePtr::Create(sFilePath, TransportLayer::GetCurrentTransportLayer().GetCacheSize() );
  }

  return filePtr;

} // Craate()


/**
 * Determines if the file exists
 *
 * @param sFilePath     File
 */
bool FilePtrFactory::Exists(const std::string & sFilePath)
{

  bool bExists(false);

  switch ( TransportLayer::GetCurrentTransportLayer().GetUrl().GetProtocol() )
  {
  case Protocols::P_TCP:
    bExists = TcpFilePtr::Exists(sFilePath);
    break;
  case Protocols::P_HTTP:
    bExists = HttpFilePtr::Exists(sFilePath);
    break;
  default:
    bExists = LocalFilePtr::Exists(sFilePath);
  }


  return bExists;


} // IsFileExist(const std::string & sFilePath)