#pragma once

#include "scoped_ptr.h"


using namespace System;

OPEN_LOGGING_NS



/**
 * Server
 */
public ref class Server
{
private:

  Server();


public:

  /// Destructor
  virtual ~Server();

  /// Finalizer
  !Server()
  { this->~Server(); }

   /// Creates the consumer object for the source
  static Server ^ Create(System::Xml::XmlNode ^ mpNode);

  /// Creates the consumer object for the source
  static Server ^ Create(unsigned int iPort);

  /// Creates the consumer object for the source
  static Server ^ Create(System::String ^ mpSettings);

  /// Starts the log consuming
  virtual void Start() ;

  /// Aborts the log consuming
  virtual void Stop() ;

  


#pragma region properties

public:

  // Gets the source
  property unsigned int Port
  {
    unsigned int get() { return (*m_pServer)->GetPort(); }
  } // Source



#pragma endregion


private:

  scoped_ptr<std::shared_ptr<vxLogServer>> m_pServer;
 
 
};  // class Server


CLOSE_LOGGING_NS
