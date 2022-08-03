#include "StdAfx.h"
#include "vxWebService.h"
#include <sstream>
#include <regex>


std::wstring stows( const std::string & str );
std::string wstos( const std::wstring & wstr );


WsHeap::WsHeap() : 
  m_pHeap( nullptr ) 
{
}


WsHeap::~WsHeap()
{
  if( m_pHeap ) 
    WsFreeHeap( m_pHeap );
  m_pHeap = nullptr;
}


HRESULT WsHeap::Create( SIZE_T maxSize, SIZE_T trimSize, WS_ERROR * pError, const WS_HEAP_PROPERTY* properties, ULONG propertyCount )
{
  return WsCreateHeap( maxSize, trimSize, properties, propertyCount, &m_pHeap, pError );
}


HRESULT WsHeap::Reset( WS_ERROR * pError )
{
  return WsResetHeap( m_pHeap, pError );
}


WsError::WsError() :
  m_pError( nullptr )
{
}


WsError::~WsError()
{
  if( m_pError )
    WsFreeError( m_pError );
  m_pError = nullptr;
}


HRESULT WsError::Create( const WS_ERROR_PROPERTY* properties, ULONG propertyCount )
{
  return WsCreateError( properties, propertyCount, &m_pError );
}


std::string WsError::GetTextMessage() const
{
  HRESULT hr = ERROR_SUCCESS;

  ULONG uCount = 0;
  if( FAILED( hr = GetProperty( WS_ERROR_PROPERTY_STRING_COUNT, &uCount )))
    throw ex::WSException( LogRec( "Failed to retrieve string count", "WsError", "GetMessage" ), hr );
  
  std::stringstream stream;
  for( uint8 i = 0; i < uCount; ++i )
  {
    WS_STRING msg;
    if( FAILED( hr = WsGetErrorString( m_pError, i, &msg )))
      throw ex::WSException( LogRec( "Failed to retrieve error string", "WsError", "GetMessage" ), hr );

    stream << conversion::UnicodeToAscii( msg.chars, msg.length ) << std::endl;
  }

  return stream.str();
}


HRESULT WsError::GetProperty( WS_ERROR_PROPERTY_ID id, void* buffer, ULONG bufferSize ) const
{
  return WsGetErrorProperty( m_pError, id, buffer, bufferSize );
}


template <typename T>
HRESULT WsError::GetProperty( WS_ERROR_PROPERTY_ID id, T* buffer ) const
{
  return GetProperty( id, buffer, sizeof(T) );
}


WsServiceProxy::WsServiceProxy() : 
  m_pProxy( nullptr ),
  m_bIsConnected( false )
{
}


WsServiceProxy::~WsServiceProxy()
{
  if( m_pProxy )
  {
    Close(); // error
    WsFreeServiceProxy( m_pProxy );

    m_bIsConnected = false;
  }

  m_pProxy = nullptr;
}


HRESULT WsServiceProxy::Open( const std::string & sAddress, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error )
{
  WS_ENDPOINT_ADDRESS address = {};

  std::wstring wsUrl = conversion::AsciiToUnicode( sAddress );
  WS_STRING url = { wsUrl.length(), const_cast< wchar_t * >( wsUrl.c_str() )};
  address.url = url;

  HRESULT hr = ERROR_SUCCESS;
  if( SUCCEEDED( hr = WsOpenServiceProxy( m_pProxy, &address, asyncContext, error )))
    m_bIsConnected = true;

  return hr;
}


HRESULT WsServiceProxy::Close( const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error )
{
  HRESULT hr = ERROR_SUCCESS;
  if( SUCCEEDED( hr = WsCloseServiceProxy( m_pProxy, asyncContext, error )))
    m_bIsConnected = false;

  return hr;
}

/// conversion methods

std::wstring conversion::AsciiToUnicode( const std::string & str )
{
  std::wstring dest( str.length(), ' ' );
  std::transform( str.begin(), str.end(), dest.begin(), []( char letter ) { return WCHAR( letter ); });
  return dest;
}


std::string conversion::UnicodeToAscii( const std::wstring & wstr, int4 iLength )
{
  iLength = ( iLength > 0 ) ? iLength : wstr.length();
  std::string dest( iLength, ' ' );
  std::transform( wstr.begin(), wstr.begin() + iLength, dest.begin(), []( WCHAR letter ) { return char( letter ); });
  return dest;
}