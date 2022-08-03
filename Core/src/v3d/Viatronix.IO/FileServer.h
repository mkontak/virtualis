#pragma once

#include "vxFileServer.h"
#include "scoped_ptr.h"

namespace Viatronix { namespace IO {

/**
 * FileServer managed wrapper for unmanaged server
 */
public ref class FileServer
{
private:

  FileServer() { }



public:

  virtual ~FileServer() { m_pServer.reset(nullptr); }

  static FileServer ^ Create(unsigned int uPort);

   static FileServer ^ Create(System::Xml::XmlNode ^ mpNode);

  void Start();

  void Stop();

private:


    /// File server
  scoped_ptr<std::shared_ptr<io::FileServer>> m_pServer;




}; // class FileServer


} }