// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: vxDOMParseErrorHandler.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.com

#include "stdafx.h"
#include "vxDOMParseErrorHandler.h"
#include "xercesc\util\XmlString.hpp"

/**
 * traps an error in the xerces parser
 * 
 * @param e     SAX exception
 */
void vxDOMParseErrorHandler::error(const XERCES_CPP_NAMESPACE::SAXParseException& e)
{
  char* systemId = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getSystemId());
  char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getMessage());

  LogErr(util::Strings::Format("Error at file %s, line %d, char %d:  %s", systemId, e.getLineNumber(), e.getColumnNumber(), message), "vxDOMParseErrorHandler", "error");

  XERCES_CPP_NAMESPACE::XMLString::release(&systemId);
  XERCES_CPP_NAMESPACE::XMLString::release(&message);
  throw e;

} // error()

/**
 * Traps a fatal error in the xerces parser
 *
 * @param e     SAX exception
 */
void vxDOMParseErrorHandler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& e)
{
  char* systemId = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getSystemId());
  char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getMessage());

  LogErr(util::Strings::Format("Fatal Error at file %s, line %d, char %d:  %s", systemId, e.getLineNumber(), e.getColumnNumber(), message), "vxDOMParseErrorHandler", "fatalError");

  XERCES_CPP_NAMESPACE::XMLString::release(&systemId);
  XERCES_CPP_NAMESPACE::XMLString::release(&message);
  throw e;
} // fatalerror()

/**
 * traps a warning in the xerces parser
 *
 * @param e     SAX exceptionb
 */
void vxDOMParseErrorHandler::warning(const XERCES_CPP_NAMESPACE::SAXParseException& e)
{
  char* systemId = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getSystemId());
  char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(e.getMessage());

  LogErr(util::Strings::Format("Warning at file %s, line %d, char %d:  %s", systemId, e.getLineNumber(), e.getColumnNumber(), message), "vxDOMParseErrorHandler", "warning");

  XERCES_CPP_NAMESPACE::XMLString::release(&systemId);
  XERCES_CPP_NAMESPACE::XMLString::release(&message);
  throw e;

} // warning()
