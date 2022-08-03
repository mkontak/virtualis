// $Id: vxDOMDocument.h,v 1.4 2006/12/05 23:01:40 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragma declarations
#pragma once

#include "vxDOMParseErrorHandler.h"

// forward declarations
class vxDOMElement;
class vxDOMAttribute;

// xerces forward declarations
namespace XERCES_CPP_NAMESPACE {
  class XercesDOMParser;
  class DOMDocument;
  class XMLException;
  class DOMException;
}

// class declaration
class VX_BASE_DLL vxDOMDocument
{
// public methods
public:

  /// default constructor
  vxDOMDocument( const std::string & sTagName );

  /// destructor
  virtual ~vxDOMDocument();

  /// ensure operations in this class will not crash the whole software....sigh
  inline bool IsValid() const { return (m_pDocument != NULL); }

  /// gets the root element for the document
  vxDOMElement GetDocumentElement() const;

  /// creates an vxDOMElement node
  vxDOMElement CreateElement( const std::string & sName );

  /// creates an vxDOMAttribute node
  vxDOMAttribute  CreateAttribute( const std::string & sName );

  // saves the current document to disk
  void Save( const std::string & sFilename );

   // assigment operator
  vxDOMDocument & operator=( const vxDOMDocument & rhs );

  // returns the contents of the document as a string
  std::string ToString();

  /// creates a vxDOMDocument from the specified file
  static vxDOMDocument FromFile( const std::string & sFile );

  /// creates a vxDOMDocument from the specified xml
  static vxDOMDocument FromXml( const std::string  & sXml );

  /// initializes the xml implementation ( needed for xerces )
  static void Initialize();

  /// uninitializes the xml implementation ( needed for xerces )
  static void Terminate();

private:

  /// private constructor
  vxDOMDocument( XERCES_CPP_NAMESPACE::DOMDocument * pDocument );
  
  /// rethrows the xerces' XMLException as an ex::XmlException
  static void Exception( XERCES_CPP_NAMESPACE::XMLException & e, const std::string & sClass,  const std::string & sMethod );

  /// rethrows the xerces' DOMException as an ex::XmlException
  static void Exception( XERCES_CPP_NAMESPACE::DOMException & e, const std::string & sClass,  const std::string & sMethod );

// member variables
private:

  /// xerces implementation
  XERCES_CPP_NAMESPACE::DOMDocument * m_pDocument;

  /// xerces parser
  static XERCES_CPP_NAMESPACE::XercesDOMParser * m_pParser;

  static vxDOMParseErrorHandler m_errorHandler;

// friend declarations
private:
  
  friend class vxDOMAttribute;
  friend class vxDOMElement;
  friend class vxDOMNamedNodeMap;
  friend class vxDOMNode;
  friend class vxDOMNodeList;
  
}; // class vxDOMDocument


// $Log: vxDOMDocument.h,v $
// Revision 1.4  2006/12/05 23:01:40  jmeade
// cvs footer
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxDOMDocument.h,v 1.4 2006/12/05 23:01:40 jmeade Exp $
// $Id: vxDOMDocument.h,v 1.4 2006/12/05 23:01:40 jmeade Exp $
