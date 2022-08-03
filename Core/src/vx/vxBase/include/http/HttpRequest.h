#pragma once

#include "HttpEnums.h"


#pragma warning(push)
#pragma warning(disable:4251)


// Forward declarations
class VX_BASE_DLL Buffer;

namespace http
{


/**
 * Implements a request message
 */
class VX_BASE_DLL HttpRequest
{
protected:

  // Constructor
  HttpRequest(  HttpVerb verb,                                              // HTTP Verb (GET, POST, DELETE, PUT )
                const std::string & resource,                               // Resource name
                const std::string & operation,                              // operation (get, delete, acquire, ... )
                const std::string & contextId = "",                         // Authorization token
                const std::shared_ptr<Buffer> & message = __nullptr );      // Payload

private:

  // Copy constructor
  HttpRequest(const HttpRequest & rhs );


#pragma region Create methods

private:

  /// Creates the request data
  static std::shared_ptr<HttpRequest> Create(  HttpVerb verb, 
                                               const std::string & resource, 
                                               const std::string & operation,  
                                               const std::string & contextId = "",  
                                               const std::shared_ptr<Buffer> & message = __nullptr);

public:

  /// Creates the POST request data
  static std::shared_ptr<HttpRequest> CreatePost( const std::string & resource, const std::string & operation, const std::string & contextId = "", const std::shared_ptr<Buffer> & message = __nullptr)
  { return Create( HttpVerb::HTTP_POST, resource, operation, contextId, message ); }

  /// Creates the GET request data
  static std::shared_ptr<HttpRequest> CreateGet( const std::string & resource, const std::string & operation, const std::string & contextId = "", const std::shared_ptr<Buffer> & message = __nullptr)
  { return Create( HttpVerb::HTTP_GET, resource, operation, contextId, message ); }

  /// Creates the PUT request data
  static std::shared_ptr<HttpRequest> CreatePut( const std::string & resource, const std::string & operation, const std::string & contextId = "", const std::shared_ptr<Buffer> & message = __nullptr)
  { return Create( HttpVerb::HTTP_PUT, resource, operation, contextId, message ); }

  /// Creates the PUT request data
  static std::shared_ptr<HttpRequest> CreateDelete( const std::string & resource, const std::string & operation, const std::string & contextId = "", const std::shared_ptr<Buffer> & message = __nullptr)
  { return Create( HttpVerb::HTTP_DELETE, resource, operation, contextId, message ); }

#pragma endregion

  // Generates the send data for the specified verb
  std::shared_ptr<Buffer> GetRequest();

  /// Formats the XML <rq>PAYLOAD</rq>
  std::string ToString();


  /// Gets the PATH extenstions (Used for GET and DELETE
  std::string GetPath();

  /// Try and get the attribute
  bool TryGetAttribute(const std::string & sName, std::string & sValue);

  /// Adds new attribute
  void AddAttribute(const std::string & sName, const std::string & sValue);

  /// Adds new attribute
  void AddRangeAttribute(const std::string & sName, const std::string & sFrom, const std::string & sTo);

private:

  /// Formats the XML request
  std::shared_ptr<Buffer> FormatRequest(const std::shared_ptr<Buffer> & message);

public:

  /// Gets the message
  const std::shared_ptr<Buffer> & GetMessage() const
  { return m_message; }

  /// Sets the message
  void SetMessage(std::shared_ptr<Buffer> & message )
  { m_message = message; }

  /// Gets the resource name
  const std::string & GetResource() const
  { return m_resource; }

  /// Sets the resource name
  void SetResource(const std::string & resource)
  { m_resource = resource;  }

  /// Gets the operation
  const std::string & GetOperation() const
  { return m_operation; }

  /// Sets the operation 
  void SetOperation(const std::string & operation)
  { m_operation = operation;  }

  /// Gets the context Id
  const std::string & GetContextId() const
  { return m_contextId; }

  /// Sets the context id
  void SetContextId(const std::string & contextId)
  { m_contextId = contextId; }

   /// Gets the source
  const std::string & GetSystem() const
  { return m_system; }

  
  /// Gets the source
  const std::string & GetSource() const
  { return m_source; }

  /// Sets the source
  void SetSource(const std::string & source)
  { m_source = source; }

  /// Gets the application
  const std::string & GetApplication() const
  { return m_application; }

  /// Sets the application
  void SetApplication(const std::string & application)
  {  m_application = application;  }

  // Gets the request records version
  const std::string & GetRequestVersion() const
  { return m_version; }

  /// Gets the requests message(resource) version
  const std::string & GetResourceVersion()
  { return m_attributes["ver"]; }

  /// Sets the requests message(resource) version
  const void SetResourceVersion(const std::string & sVersion) 
  { m_attributes["ver"] = sVersion; }


   /// Gets the verb
  const HttpVerb & GetVerb() const
  { return m_verb; }

  /// Gets the verb as a string
  std::string HttpRequest::GetVerbAsString();

private:

  /// Verb used for send
  HttpVerb m_verb;

  /// Source
  std::string m_source;

  /// Application
  std::string m_application;

  /// System
  std::string m_system;

  // Context id for request
  std::string m_contextId;

  // operation
  std::string m_operation;

  // resource
  std::string m_resource;

  // version of the request message
  std::string m_version;

  // Attributes for the message
  std::map<std::string, std::string> m_attributes;

  /// Message
  std::shared_ptr<Buffer> m_message;


  /// Encoding 
  Encoding m_encoding;

};  // class HttpRequest

}


#pragma warning(pop)