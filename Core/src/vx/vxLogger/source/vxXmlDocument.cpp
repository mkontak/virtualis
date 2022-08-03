// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlDocument.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxXmlDocument.h"
#include "vxXmlExceptions.h"
#include "vxXmlNode.h"
#include "vxUtilStrings.h"

// usings
using namespace xml;


/**
 * Constructor
 */
XmlDocument::XmlDocument() :  
m_document(xmlNewDoc((xmlChar *)"1.0"))
{

  if ( m_document == __nullptr )
    throw XmlException(LogRec("Failed to allocate document","XmlDocument","XmlDocument"));
}


/**
 * Destructor
 */
XmlDocument::~XmlDocument()
{ 
  ReleaseResources();
} // ~XmlDocument()


/**
 * Frees the document object
 */
void XmlDocument::ReleaseResources()
{
   if ( m_document != __nullptr )
  {
    xmlFreeDoc(m_document);
    m_document = __nullptr;
  }
} // FreeDocument()

/**
 * Loads the Xml into the document
 *
 * @param sXml      Xml string
 */
void XmlDocument::LoadXml(const std::string & sXml )
{
  // Frees any resources first
  ReleaseResources();

  try
  {
    m_document = xmlReadMemory(sXml.c_str(), (int)sXml.size(), __nullptr, __nullptr, XML_PARSE_RECOVER | XML_PARSE_NOERROR | XML_PARSE_NOWARNING ); 

    // Cleans up the parser
    //xmlCleanupParser();

    if ( m_document == __nullptr )
    {
      //if ( ! ignoreWarnings )
      //  LogWrn("Failed to parse xml, attempting to parse with XML_PARSE_RECOVER flag","XmlDocument","LoadXml");

      //m_document = xmlReadMemory(sXml.c_str(), (int)sXml.size(), __nullptr, __nullptr, XML_PARSE_RECOVER | XML_PARSE_NOERROR | XML_PARSE_NOWARNING ); 

      //// Cleans up the parser
      //xmlCleanupParser();

      //if ( m_document == __nullptr )
        throw XmlException(LogRec("Failed to parse xml, check the xml", "XmlDocument","LoadXml"));
    }

  }
  catch ( XmlException & )
  {
    throw;
  }
  catch ( ... )
  {
     throw XmlException(LogRec("Failed to parse xml : unspecified exception", "XmlDocument","LoadXml"));
  }

} // LoadXml(const std::string & sXml)

/**
 * Loads the file
 *
 * @param sFilePath      File path
 */
void XmlDocument::LoadFile(const std::string & sFilePath)
{
  // Frees any resources first
  ReleaseResources();

  try
  {
    m_document = xmlReadFile(sFilePath.c_str(), __nullptr,0); 

    // Cleans up the parser
    xmlCleanupParser();

    if ( m_document == __nullptr )
      throw XmlException(LogRec(util::Strings::Format("Failed to parse xml file %s, check the file",sFilePath.c_str()), "XmlDocument","LoadFile"));

  }
  catch ( XmlException & )
  {
    throw;
  }
  catch ( ... )
  {
     throw XmlException(LogRec(util::Strings::Format("Failed to parse xml file %s, check the file",sFilePath.c_str()), "XmlDocument","LoadFile"));
  }

} // LoadXml(const std::string & sXml)

/**
 * Saves the sml document to the specified file
 *
 * @param sFilePath     File to save to
 */
void XmlDocument::Save(const std::string & sFilePath)
{
  // ======================================
  // Only save if the document is not null
  // ======================================
  if ( m_document != __nullptr )
  {
    if ( xmlSaveFormatFile(sFilePath.c_str(), m_document, 1) == -1 )
      throw XmlException (LogRec(util::Strings::Format("Failed to save file %s",sFilePath.c_str()), "XmlDocument","Save"));
  }
} // Save(const std::string & sFilePath)

/**
 * Sets the Document element node pointer
 *
 */
XmlNode XmlDocument::SetDocumentElement(XmlNode root)
{

  if ( m_document != __nullptr && root.IsValid() )
    return XmlNode(m_document, xmlDocSetRootElement(m_document, root.GetNodePtr() ));
  else
    return XmlNode(__nullptr, __nullptr);

} // SetDocumentElement()


/**
 * Gets the Document element node pointer
 *
 */
XmlNode XmlDocument::GetDocumentElement()
{

  if ( m_document != __nullptr )
    return XmlNode(m_document, xmlDocGetRootElement(m_document));
  else
    return XmlNode(__nullptr, __nullptr);

} // GetDocumentElement()


/**
 * Convert the document to an xml string
 */
std::string XmlDocument::ToXml()
{
  std::string xml("");

  if ( m_document != __nullptr )
  {
    xmlChar * buffer;
    int size;
    xmlDocDumpMemory(m_document, &buffer, &size);
    xml = std::string((char *)buffer,size);
  }

  return xml;
} // ToString() 


