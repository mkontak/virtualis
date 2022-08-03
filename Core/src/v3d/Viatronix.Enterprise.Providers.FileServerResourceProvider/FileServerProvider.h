// Viatronix.Enterprise.Providers.FileServer.h

#pragma once

#include "scoped_ptr.h"
#include "vxFileServer.h"



OPEN_PROVIDERS_NS

/**
  * FileServer resource providers
  */
 public ref class FileServerProvider : IFileServerProvider
{




public:

  FileServerProvider();


#pragma region methods


public:

  /// Reads from file server
  virtual array<System::Byte> ^ ProcessRequest(array<System::Byte> ^ mpRequest);





#pragma endregion

#pragma region fields

private:

  scoped_ptr<std::shared_ptr<io::FileServer>> m_fileServer;
      
#pragma endregion

};


CLOSE_PROVIDERS_NS
