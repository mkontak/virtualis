// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HttpResponse.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

#include "stdafx.h"
#include "http\HttpResponse.h"
#include "Exception.h"
#include "vxBuffer.h"
#include "http\HttpRequest.h"





using namespace http;

/**
 * Constructor
 */
HttpResponse::HttpResponse() :
m_sError(""),
m_sResponse(""),
m_sMessage(""),
m_errorCode(0),
m_sFormat("xml"),
m_encoding(Encoding::ENCODING_ASCII)
{
} // HttpResponse()


/**
 * Creates the response form the request
 *
 * @param request     Http Request
 *
 * @return Http Response
 */
std::shared_ptr<HttpResponse> HttpResponse::Create(const std::shared_ptr<Buffer> & buffer)
{


  std::shared_ptr<HttpResponse> response = std::shared_ptr<HttpResponse>(new HttpResponse());

  if ( response == __nullptr )
    throw ex::OutOfMemoryException(LogRec("Failed to allocate HttpResponse","HttpResponse","Create"));



  std::string sXml((char *)buffer->GetData());

  if ( sXml.size() > 0 )
  {


    xml::XmlDocument doc;

    doc.LoadXml(sXml);

    xml::XmlNode message(doc.GetDocumentElement().GetFirstChild());
   
    // Make sure we received a valid response
    if ( ! message.HasContent() )
      throw ex::HttpException(LogRec("No valid response was received", "HttpResponse", "Create"));
 
    // Save the actual message's xml
    response->m_sResponse = message.GetContent();

    LogDbg("Received response: " + response->m_sResponse, "HttpResponse","Create");

    // Parse the message to validate the rsp
    doc.LoadXml(response->m_sResponse);

    xml::XmlNode docNode(doc.GetDocumentElement());

    // Make sure the tag  is "rsp" for a response
    if ( docNode.GetName() != "rsp" )
      throw ex::HttpException(LogRec("Invalid response message was received", "HttpResponse", "Create"));
    
    // Schema file
    std::string schemaFile(util::IO::GetDefaultViatronixDirectory() + "schemas\\" + docNode.GetName() + ".xsd");

    // =================================================
    // If the schema exists then validate the document 
    // =================================================
    if ( util::IO::Exists(schemaFile) )
    {
      xml::XmlSchema schema;


      schema.LoadFile(schemaFile);

      schema.Validate(doc);
    } // END ... If the schema file exists 


    // Make sure that the response contains a status code
    if ( ! docNode.TryGetAttributeValue<int>("code", response->m_errorCode) )
      throw ex::HttpException(LogRec("Invalid response message was received", "HttpResponse", "Create"));

    // Make sure that the response contains a type
    if ( ! docNode.TryGetAttributeValue<std::string>( "fmt", response->m_sFormat) )
      throw ex::HttpException(LogRec("Invalid response message was received", "HttpResponse", "Create"));

    int encoding(0);
    docNode.TryGetAttributeValue<int>( "enc", encoding);
    response->m_encoding = (Encoding)encoding;

  }

  return response;


} // Create(std::shared_ptr<HttpRequest> request)

/**
 * Gets the error message if failed
 */
const std::string & HttpResponse::GetError()
{
 
  if ( m_errorCode != 0 && m_sError.size() == 0)
  {

    xml::XmlDocument doc;

    // Parse the message
    doc.LoadXml(m_sResponse);

    // ========================================
    // The message type should always be text
    // =======================================
    if ( lstrcmpi(m_sFormat.c_str(),"text") == 0 )
    {
      // Get the text node 
      xml::XmlNode text = doc.GetDocumentElement().SelectSingleNode( "text");

      // =========================================================================================
      // If there is content then set the error otherwise indicate that no message was received
      // =========================================================================================
      if ( text.HasContent() )
        m_sError = text.GetContent();
      else
        m_sError = "No error message received";
    } // END ... If the message type is text

  } // GetError()

  return m_sError;

} // GetError()

/**
 * Attempts to get the attribute specified from the xml
 *
 * @param sName   Attribute name
 * @param sValue    Value
 * 
 * @return TRue if found false otherwise
 */
bool HttpResponse::TryGetAttribute(const std::string & sName, std::string & sValue)
{

  bool found(false);


  if ( m_sResponse.size() > 0 )
  {
    xml::XmlDocument doc;

    doc.LoadXml(m_sResponse);

    if (  lstrcmpi(m_sFormat.c_str(),"xml") == 0  )
    {
      xml::XmlNode docNode(doc.GetDocumentElement());

      if ( docNode.HasChildren() )
        found = docNode.GetFirstChild().TryGetAttributeValue<std::string>(sName, sValue);
    }
    
  }

  return found;

} // TryGetAttribute(const std::string & sName, std::string & sValue)

/**
 * Gets the message body from the response
 *
 * @return  Message body
 */
const std::string & HttpResponse::GetMessage()
{

  
  // ==================================================================================
  // If there is a response and the message has not already been parsed out then parse
  // ==================================================================================
  if ( m_sResponse.size() > 0 && m_sMessage.size() == 0 )
  {

    xml::XmlDocument doc;

    doc.LoadXml(m_sResponse);

    xml::XmlNode docNode(doc.GetDocumentElement());

    // ===================================================================================
    // Check for an XML format and if so extract the xml as is from the child otherwise 
    // we need to extract the text.
    // ===================================================================================
    if (lstrcmpi(m_sFormat.c_str(),"xml") == 0 )
    {

      if ( docNode.HasChildren() )
      {
        xml::XmlNode child = docNode.GetFirstChild();

        m_sMessage = child.ToXml();

        int j(0);

      }

    }
    else
    {

      // Get the text node 
      xml::XmlNode text = docNode.SelectSingleNode("text");

      // =========================================================================================
      // If there is content then set the error otherwise indicate that no message was received
      // =========================================================================================
      m_sMessage =  text.GetContent();

    }

  }


  return m_sMessage;
} // GetMessage()