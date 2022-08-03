// $Id: vxDOMElement.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
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

// incldue declarations
#include "vxDOMNode.h"

// forward declarations
class vxDOMNodeList;
class vxDOMAttribute;

// xerces forward declarations
namespace XERCES_CPP_NAMESPACE {
  class DOMElement;
}

// class declaration
class VX_BASE_DLL vxDOMElement : public vxDOMNode
{
// member functions
public:

  /// default constructor
  vxDOMElement();

  /// gets the name of this ndoe
  std::string GetTagName() const;

  /// sets the name of this node
  void SetTagName( const std::string & sTagName );

  /// get the value of the specified attribute 
  std::string GetAttribute( const std::string & sName ) const;

  /// gets the specified attribute node as an object 
  vxDOMAttribute GetAttributeNode( const std::string sName ) const;

  /// gets the specified node, and all its child nodes, as a nodeList 
  vxDOMNodeList GetElementsByTagName( const std::string sTagName ) const;
  
  /// puts the text nodes for this element, and its child nodes, into one text node, returns nothing 
  void Normalize();
  
  /// removes the specified attribute's value
  void RemoveAttribute( const std::string sName );
  
  /// removes the specified attribute node
  vxDOMAttribute RemoveAttributeNode( const vxDOMAttribute & attribute );
  
  /// inserts a new attribute 
  void SetAttribute( const std::string & sName, const std::string &  sValue );
  
  /// inserts a new attribute
  vxDOMAttribute SetAttributeNode( const vxDOMAttribute & attribute );

  /// assigment operator
  vxDOMElement & operator=( const vxDOMElement & rhs );

// implementation
private:

  // private constructor
  vxDOMElement( XERCES_CPP_NAMESPACE::DOMElement * pElement );

// member variables
private:

  /// xerces implementation
  XERCES_CPP_NAMESPACE::DOMElement * m_pElement;

// friend declarations
private:

  friend class vxDOMDocument;
  friend class vxDOMNode;

}; // class vxDOMElement 


// $Log: vxDOMElement.h,v $
// Revision 1.1  2005/04/12 15:20:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxDOMElement.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
// $Id: vxDOMElement.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
