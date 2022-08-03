#pragma once

#include "WebServices.h"
#include <string>
#include "Exception.h"

namespace ex
{

// class definition
class WSException : public HResultException
{
// functions
public:

  /// constructor
  WSException( const vxLogRecord & logRec, HRESULT hr ) : HResultException( logRec, hr ) { }

};

}


class WsError
{
public:
  WsError();
  ~WsError();

  HRESULT Create( const WS_ERROR_PROPERTY* properties = NULL, ULONG propertyCount = 0 );

  std::string GetTextMessage() const;

  operator WS_ERROR*() const { return m_pError; }

private:

  HRESULT GetProperty( WS_ERROR_PROPERTY_ID id, void * buffer, ULONG bufferSize ) const;
  
  template <typename T>
  HRESULT GetProperty( WS_ERROR_PROPERTY_ID id, T * buffer ) const;

private:

  WS_ERROR* m_pError;

};


class WsHeap
{
public:

  WsHeap();

  ~WsHeap();

  HRESULT Create( SIZE_T maxSize, SIZE_T trimSize, WS_ERROR * pError = NULL, const WS_HEAP_PROPERTY* properties = NULL, ULONG propertyCount = 0 );

  HRESULT Reset( WS_ERROR * pError = NULL );

  operator WS_HEAP*() const { return m_pHeap; }

private:

  WS_HEAP * m_pHeap;

};


class WsServiceProxy
{
public:
    WsServiceProxy();

    ~WsServiceProxy();

    HRESULT Open( const std::string & sAddress, const WS_ASYNC_CONTEXT* asyncContext, WS_ERROR* error = NULL );

    HRESULT Close( const WS_ASYNC_CONTEXT* asyncContext = NULL, WS_ERROR* error = NULL );

    WS_SERVICE_PROXY** operator&() { return &m_pProxy; }

    operator WS_SERVICE_PROXY*() const { return m_pProxy; }

    bool IsConnected() const { return m_bIsConnected; }

private:

  WS_SERVICE_PROXY * m_pProxy;

  bool m_bIsConnected;

};


namespace conversion
{

  std::wstring AsciiToUnicode( const std::string & str );

  std::string UnicodeToAscii( const std::wstring & wstr, int4 uLength = -1 );

}