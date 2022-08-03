// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlSchema.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "vxXmlDocument.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <stdafx.h>


// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

// namespace
namespace xml
{


/**
 * Implements a schema definition for xml files. 
 */
class VX_LOGGER_DLL XmlSchema : public XmlDocument
{
public:

  // Constructor
  XmlSchema();

  /// Loads the XML
  virtual void LoadXml(const std::string & sXml);

  /// Loads Xml File
  virtual void LoadFile(const std::string & sFilePath);

  /// Validates the document against the schema
  void Validate(const XmlDocument & document);

  /// Validates the document against the schema
  bool IsValid(const XmlDocument & document);

private:

  /// Release the resources
  virtual void ReleaseResources();

  /// Initilizes the object
  void Initialize();

  /// Callback error handler
  static void ErrorHandler(void * pContext, const char * pMessage, ... );


public:

  /// Gets the error message
  const std::string & GetErrorMessage() const
  { return m_sErrorMessage; }

  /// Gets the status code
  const int & GetStatusCode() const
  { return m_iStatusCode; }

private:

  /// Statue code
  int m_iStatusCode;

  /// Error Message
  std::string m_sErrorMessage;

 
  // Parse context
  xmlSchemaParserCtxtPtr m_parseContext;

  // Schema 
  xmlSchemaPtr m_schema;

  // Validation context
  xmlSchemaValidCtxtPtr m_validationContext;

};  // class XmlSchema

} // namespace xml

#pragma warning(pop)