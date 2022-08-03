// Viatronix.Enterprise.Providers.FileServer.h

#pragma once

#include "scoped_ptr.h"
#include "vxFileServer.h"

using namespace System;
using namespace Viatronix::Enterprise;
using namespace Viatronix::Enterprise::Messaging;

OPEN_PROVIDERS_NS

/**
  * FileServer resource providers
  */
public ref class FileServerResourceProvider : Viatronix::Enterprise::Providers::ResourceProviderBase
{

#pragma region fields

private:

  scoped_ptr<std::shared_ptr<io::FileServer>> m_fileServer;
      
#pragma endregion


public:

  FileServerResourceProvider();


#pragma region methods

private:

  /// Reads from file server
  String ^ ReadFileServer(RequestMessage ^ mpRequest);

  /// Writes to the file server
  String ^ WriteFileServer(RequestMessage ^ mpRequest);

#pragma endregion

#pragma region IResourceProvider



#pragma endregion
		
};


CLOSE_PROVIDERS_NS
