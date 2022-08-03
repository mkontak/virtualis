#pragma once

#include "vxWebService.h"
#include <sstream>
#include <string>

class EnterpriseService
{
#ifdef WEBSERVICE
public:
  
  static void Connect( const std::string & username, const std::string & pwd );

  static std::string Authenticate( const std::string & msg ) { return GetInstance().m_id = GetInstance().Process( msg ); }

  static std::string Send( const std::string & msg ) { return GetInstance().Process( msg ); }

#endif

private:

  EnterpriseService();
  ~EnterpriseService();

  static EnterpriseService & GetInstance();

  std::string Process( const std::string & msg );

  std::string GetError( HRESULT hr, const std::string & msg );

private:

  WsError m_error;
  WsHeap m_heap;
  WsServiceProxy m_proxy;

  std::string m_id;

};



