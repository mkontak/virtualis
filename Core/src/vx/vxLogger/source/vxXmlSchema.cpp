// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlSchema.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxXmlSchema.h"
#include "vxXmlExceptions.h"


// using
using namespace xml;


/**
 * Constructors
 */
XmlSchema::XmlSchema() :
XmlDocument(),
m_parseContext(__nullptr),
m_schema(__nullptr),
m_validationContext(__nullptr),
m_sErrorMessage(""),
m_iStatusCode(0)
{

} // XmlSchema()

/**
 * Loads the file
 *
 * @param sFilePath     File to load
 */
void XmlSchema::LoadFile(const std::string & sFilePath)
{
  XmlDocument::LoadFile(sFilePath);

  Initialize();
} // LoadFile(const std::string & sFilePath)

/**
 * Loads the xml
 *
 * @param sXml    Xml string
 *
 */
void XmlSchema::LoadXml(const std::string & sXml)
{

  XmlDocument::LoadXml(sXml);

  Initialize();
} // LoadXml(const std::string & sXml)

/**
 * Releases the resources for this object
 */
void XmlSchema::ReleaseResources()
{

  // ==============================
  // Release the validate context
  // =============================
  if ( m_validationContext != __nullptr )
  {
    xmlSchemaFreeValidCtxt(m_validationContext);
    m_validationContext = __nullptr;
  } // END ... If the validation context is not null

  // =======================
  // Release the schema
  // =====================
  if ( m_schema != __nullptr )
  {
    xmlSchemaFree(m_schema);
    m_schema = __nullptr;
  } // END ... If the schema is not null

  // ===========================
  // Release the parse context
  // ==========================
  if ( m_parseContext != __nullptr )
  {
    xmlSchemaFreeParserCtxt(m_parseContext);
    m_parseContext = __nullptr;
  } // END ... If the parse context is not null

  // Release the document
  XmlDocument::ReleaseResources();
} // ReleaseResources()


/**
 * Initializes the schema object
 */
void XmlSchema::Initialize()
{

  // Create the parse context
  if ( (m_parseContext = xmlSchemaNewDocParserCtxt(m_document)) == __nullptr  )
    throw XmlException(LogRec("Failed to create parser context", "XmlSchema", "Initialize"));

  // Create the schema using the parse context
  if ( (m_schema = xmlSchemaParse(m_parseContext) ) == __nullptr )
    throw XmlException(LogRec("Schema is invalid", "XmlSchema", "Initialize"));

  // Create the validation context using the schema
  if ( ( m_validationContext = xmlSchemaNewValidCtxt(m_schema) ) == __nullptr )
    throw XmlException(LogRec("Failed to create validation context", "XmlSchema", "Initialize"));

  xmlSetStructuredErrorFunc(NULL, NULL); 
  xmlSetGenericErrorFunc(this, XmlSchema::ErrorHandler); 
  xmlThrDefSetStructuredErrorFunc(NULL, NULL); 
  xmlThrDefSetGenericErrorFunc(this, XmlSchema::ErrorHandler);
 

} // Initialize()

/**
 * Validates the ocument without throwing and error
 *
 * @param document  XmlDocument
 * 
 * @return true if valid, false otherwise
 */
bool XmlSchema::IsValid(const XmlDocument & document)
{

  if ( m_validationContext == __nullptr )
    throw XmlException(LogRec("Invalid schema, cannot validate document","XmlSchema","IsValid"));
  

  return  ( xmlSchemaValidateDoc(m_validationContext, document.m_document ) == 0);

}

/**
 * Validates the document and throws an exception if bad
 *
 * @param document      XmlDocument
 */
void XmlSchema::Validate(const XmlDocument & document)
{
   if ( m_validationContext == __nullptr )
    throw XmlException(LogRec("Invalid schema, cannot validate document","XmlSchema","IsValid"));
  

  int status;
  
  // If no error just return
  if ( (status = xmlSchemaValidateDoc(m_validationContext, document.m_document ) ) == 0 )
    return;

  std::string message;

  switch ( status )
  {

  case XML_SCHEMAS_ERR_NOROOT:
   message = "no root";
   break;

  case XML_SCHEMAS_ERR_UNDECLAREDELEM:
    message = "undeclared element";
    break;

  case XML_SCHEMAS_ERR_NOTTOPLEVEL:
    message = "no top level";
    break;
 
  case XML_SCHEMAS_ERR_MISSING:
   message = "missing data";
   break;

  case XML_SCHEMAS_ERR_WRONGELEM:
    message = "wrong element";
    break;

  case XML_SCHEMAS_ERR_NOTYPE:
    message = "no type";
    break;

  case XML_SCHEMAS_ERR_NOROLLBACK:
    message = "no rollback";
    break;

  case XML_SCHEMAS_ERR_ISABSTRACT:
    message = "is abstract";
    break;

  case XML_SCHEMAS_ERR_NOTEMPTY:
    message = "not empty";
    break;

  case XML_SCHEMAS_ERR_ELEMCONT:
    message = "element context";
    break;

  case XML_SCHEMAS_ERR_HAVEDEFAULT:
    message = "have default";
    break;

  case XML_SCHEMAS_ERR_NOTNILLABLE:
    message = "not nillable";
    break;

  case XML_SCHEMAS_ERR_EXTRACONTENT:
    message = "extra content";
    break;

  case XML_SCHEMAS_ERR_INVALIDATTR:
    message = "invalid attribute";
    break;

  case XML_SCHEMAS_ERR_INVALIDELEM:
    message = "invalid or missing element";
    break;

  case XML_SCHEMAS_ERR_NOTDETERMINIST:
    message = "not deterministic";
    break;

  case XML_SCHEMAS_ERR_CONSTRUCT:
    message = "invalid construct";
    break;

  case XML_SCHEMAS_ERR_INTERNAL:
    message = "Internal error";
    break;

  case XML_SCHEMAS_ERR_NOTSIMPLE:
    message = "not a simple type";
    break;

  case XML_SCHEMAS_ERR_ATTRUNKNOWN:
    message = "attribute unknown";
    break;

  case XML_SCHEMAS_ERR_ATTRINVALID:
    message = "invalid attribute";
    break;

  case XML_SCHEMAS_ERR_VALUE:
    message = "missing or invalid value";

  case XML_SCHEMAS_ERR_FACET:
    message = "missing or invalid facet";

  default:
    message = m_sErrorMessage;

  };

  throw XmlException(LogRec(util::Strings::Format("Invalid xml [%ld] : %s",status, message.c_str()), "XmlSchema", "Validate"));


} // Validate(const XmlDocument & document)

/**
 * Handles the errors from the validation
 *
 * @param pContext      Context data (XmlSchema)
 * @param pMessage      Format
 * @param ...           List of parameters
 *
 */
void XmlSchema::ErrorHandler(void * pContext, const char * pMessage , ... )
{

  XmlSchema * pSchema(reinterpret_cast<XmlSchema *>(pContext));

  // Argument list
  va_list args;    

  // Initialize list
  va_start(args, pMessage);  

  pSchema->m_sErrorMessage.clear();

  char * buffer(__nullptr);
  char * realloc_ptr(__nullptr);

  try
  {

    // result of the _vsnprintf
    int   result = -1;

    // Length of the buffer
    int   length = 2048;    

   
    // ==================================================
    // While there are still % tags in the format string
    // ==================================================
    do  
    {        
      size_t size ( length * sizeof(char) );

      if ( ( realloc_ptr = (char *)realloc(buffer, size)) == __nullptr )
        throw std::exception("Failed to expand the buffer for string formatting");

      buffer = realloc_ptr;

      // Initialize to 0
      memset(buffer, 0, sizeof(buffer));        
    
      // Perform the translation
      result = _vsnprintf_s(buffer, size, length, pMessage, args);    

      // Increase the buffer length
      length *= 2;    

    }  while ( result == -1 );
  
    // Set up std string
    pSchema->m_sErrorMessage.append(buffer);    
  
  
  }
  catch ( ... )
  {
    //LogErr(Format("Failed to format message (%s), invalid arguments", sFormat), "Strings", "Format");
  }

  if ( buffer != __nullptr )
    free(buffer);

  va_end(args);


} // ErrorHandler(void * pContext, const char * pMessage , ... )