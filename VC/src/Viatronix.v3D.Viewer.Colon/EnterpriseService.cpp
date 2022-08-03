#include "StdAfx.h"
#include "EnterpriseService.h"

#include <algorithm>
#include <sstream>
#include <regex>
#include "VCSettings.h"
#include "Base64.h"
#include "crypto\AESCrypt.h"

#include "webservice\schemas.microsoft.com.2003.10.Serialization.xsd.h"
#include "webservice\tempuri.org.xsd.h"
#include "webservice\tempuri.org.wsdl.h"


EnterpriseService::EnterpriseService()
{
  HRESULT hr = ERROR_SUCCESS;
  if( FAILED( hr = m_error.Create() ))
    throw ex::WSException( LogRec( "Failed to create error", "WebService", "Connect" ), hr );

  if( FAILED( hr = m_heap.Create( 65536, 512, m_error )))
    throw ex::WSException( LogRec( GetError( hr, "Failed to create heap" ), "WebService", "Connect" ), hr );
}


EnterpriseService::~EnterpriseService()
{
}


EnterpriseService & EnterpriseService::GetInstance()
{
  static EnterpriseService instance;
  return instance;
}

#ifdef WEBSERVICE
void EnterpriseService::Connect( const std::string & sUsername, const std::string & sPwd )
{
#ifndef BYPASS_AUTHENTICATION

  HRESULT hr = ERROR_SUCCESS;

  EnterpriseService & ws = GetInstance();

  std::string url = VCSettings::GetInstance().GetValue( "webservice" );
  std::regex rx( "^https://" );
  bool bIsSecure = std::regex_search( url.begin(), url.end(), rx ); 

  // using https
  if( bIsSecure )
  {
    std::wstring wsUsername = conversion::AsciiToUnicode( sUsername );
    std::wstring wsPwd = conversion::AsciiToUnicode( sPwd );
    WS_STRING_USERNAME_CREDENTIAL explicitCredential = 
    {
      {WS_STRING_USERNAME_CREDENTIAL_TYPE},
      { wsUsername.length(), const_cast< WCHAR * >( wsUsername.c_str() )},
      { wsPwd.length(), const_cast< WCHAR * >( wsPwd.c_str() )},
     };
  
    WS_HTTP_SSL_USERNAME_BINDING_TEMPLATE templ = {};
    templ.usernameMessageSecurityBinding.clientCredential = &explicitCredential.credential;

    if( ws.m_proxy.IsConnected() )
    {
      if( FAILED( hr = ws.m_proxy.Close() ))
        throw ex::WSException( LogRec( ws.GetError( hr, "Failed to release service proxy" ), "WebService", "Connect" ), hr );
    }

    if( FAILED( hr = WSHttpBinding_IEnterpriseService_CreateServiceProxy( &templ, NULL, 0, &ws.m_proxy, ws.m_error )))
      throw ex::WSException( LogRec( ws.GetError( hr, "Failed to create secured service proxy" ), "WebService", "Connect" ), hr );
  }
  else // using http
  {
    if( FAILED( hr = WsCreateServiceProxy( WS_CHANNEL_TYPE_REQUEST, WS_HTTP_CHANNEL_BINDING, NULL,  NULL, 0,  NULL,  0,  &ws.m_proxy, ws.m_error )))
      throw ex::WSException( LogRec( ws.GetError( hr, "Failed to create service proxy" ), "WebService", "Connect" ), hr );
  }

  // Create the proxy
  if( FAILED( hr = ws.m_proxy.Open( url, NULL, ws.m_error )))
    throw ex::WSException( LogRec( ws.GetError( hr, "Failed to create proxy" ), "WebService", "Connect" ), hr );

#endif
}
#endif


std::string EnterpriseService::Process( const std::string & msg )
{
#ifndef BYPASS_AUTHENTICATION

  std::string sResult;
  try
  {
    // wrap message
    std::stringstream stream;
    stream << "<msg id=\"" << m_id << "\">" << msg << "</msg>";

    // encrypt the Ascii message and convert to Unicode
    std::wstring wsMsg( conversion::AsciiToUnicode( crypto::AESCrypt::Encrypt( stream.str() )));

    HRESULT hr = ERROR_SUCCESS;
    SCOPE_EXIT
    {
      if( FAILED( hr = m_heap.Reset( m_error )))
        throw ex::WSException( LogRec( GetError( hr, "Failed to reset heap" ), "WebService", "Process" ), hr );
    };

    WCHAR * result;	
    if( FAILED( hr = WSHttpBinding_IEnterpriseService_Process( m_proxy, const_cast< WCHAR * >( wsMsg.c_str() ), &result, m_heap, NULL, 0, NULL, m_error )))
      throw ex::WSException( LogRec( GetError( hr, "Failed to execute webservice" ), "WebService", "Process" ), hr );

    // convert Unicode message to Ascii and decrypt
    sResult = crypto::AESCrypt::Decrypt( conversion::UnicodeToAscii( result ));

    // validate response xml
    std::smatch res;
    std::regex rx( "<msg>(?:(?:<error>((?:\\S|\\s)*)</error>)|((?:\\S|\\s)*))</msg>" );
    if( !std::regex_search( sResult, res, rx ))
      throw ex::WSException( LogRec( "Invalid response", "WebService", "Process" ), ERROR_INVALID_DATA );

    // is it an error
    if( res[1].matched )
      throw ex::RuntimeException( LogRec( res[1].str(), "WebService", "Process" ));

    // return valid response
    return res[2].str();
  }
  catch( ex::ErrorCodeException & e )
  {
    throw;
  }
  catch( ex::VException & e )
  {
    throw;
  }

#else
  return "";
#endif
}

std::string EnterpriseService::GetError( HRESULT hr, const std::string & msg )
{
  std::stringstream stream;

  stream << std::hex << msg << " [0x" << hr << ": " << m_error.GetTextMessage() << "]";
  return stream.str();
}

