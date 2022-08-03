#pragma once


namespace http
{
  
typedef enum HttpEvent
{
  HTTP_EVENT_WRITE_DATA,
  HTTP_EVENT_READ_COMPLETED,
  HTTP_EVENT_FAILED
};

typedef enum HttpVerb
{
  HTTP_GET,
  HTTP_PUT,
  HTTP_DELETE,
  HTTP_POST
} HttpVerv;  


typedef enum Encoding
{
  ENCODING_ASCII = 0,
  ENCODING_BASE64 = 1
} Encoding;  


} // namespace http