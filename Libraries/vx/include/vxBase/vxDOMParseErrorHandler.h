// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: vxDOMParseErrorHandler.h
//
// Owner: Mark Kontak  mkontak@viatronix.com

// pragma declarations
#pragma once

#include "xercesc\sax\ErrorHandler.hpp"
#include "xercesc\sax\SAXParseException.hpp"

/**
 * Error handler to be used by the xerces parser
 */
class vxDOMParseErrorHandler : public XERCES_CPP_NAMESPACE::ErrorHandler
{

public:
  
  // Warning
  void warning(const XERCES_CPP_NAMESPACE::SAXParseException & e);

  // Error
  void error(const XERCES_CPP_NAMESPACE::SAXParseException& e);

  // Fatal Error
  void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& e);

  // Reset errors
  void resetErrors() {};

};  // class vxDOMParseErrorHandler