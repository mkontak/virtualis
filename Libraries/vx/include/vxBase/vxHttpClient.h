#pragma once

#include "HttpClient.h"


namespace io
{

class VX_BASE_DLL HttpClient : public http::HttpClient
{

protected:

  HttpClient(const http::Url & url, const std::shared_ptr<http::HttpConnection> & connection) : http::HttpClient(url, connection) { }

public:

  static std::shared_ptr<HttpClient> Create();

  static std::shared_ptr<HttpClient> Create(const http::Url & url);


};   // class HttpClient


} // namespace io