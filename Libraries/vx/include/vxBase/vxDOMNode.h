// $Id: vxDOMNode.h,v 1.4 2007/08/22 19:07:04 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#pragma once

// forward declarations
class vxDOMDocument;
class vxDOMElement;
class vxDOMNamedNodeMap;
class vxDOMNodeList;

// xerces forward declarations
namespace XERCES_CPP_NAMESPACE { 
  class DOMNode;
}

class VX_BASE_DLL vxDOMNode
{
// member functions
public:

  /// default constructor
  vxDOMNode();

  /// gets a vxDOMNamedNodeMap containing all attributes for this node 
  vxDOMNamedNodeMap GetAttributes() const;
  
  /// gets a vxDOMNodeList containing all children for this node
  vxDOMNodeList GetChildNodes() const;
  
  /// gets the first child node for this node 
  vxDOMNode GetFirstChild() const;
  
  /// gets the last child node for this node 
  vxDOMNode GetLastChild() const;
  
  /// gets the next sibling node
  vxDOMNode GetNextSibling() const; 
  
  /// gets the node type as a number 
  int4 GetNodeType() const; 
  
  ///  gets the value of this node
  std::string GetNodeValue() const;

  ///  sets the value of this node
  void SetNodeValue( const std::string & sValue );
  
  ///  gets the root the document 
  vxDOMDocument GetOwnerDocument() const;
  
  /// returns the parent node for this node 
  vxDOMNode GetParentNode() const;
  
  ///  gets the previous sibling node
  vxDOMNode GetPreviousSibling() const;

  /// gets the name of this node
  std::string GetNodeName() const;

  /// indicates if the node is null (BOOL instead of bool because of virtual bool bug MS #823071)
  virtual BOOL IsNull() { return m_pNode == NULL; }

  ///  appends the node at the end of the child nodes for this node 
  vxDOMNode AppendChild( const vxDOMNode & node );
  
  /// gets an exact clone of this node
  vxDOMNode CloneNode( bool bCloneChildren );
  
  /// returns true if this node has any child nodes 
  bool HasChildNodes();

  /// inserts a new node, newNode, before the existing node, refNode 
  vxDOMNode InsertBefore( const vxDOMNode & newNode, const vxDOMNode &  refNode );
  
  /// removes the specified node
  vxDOMNode RemoveChild( const vxDOMNode & node );
  
  /// replaces a node with another
  vxDOMNode ReplaceChild( const vxDOMNode & newNode, const vxDOMNode & oldNode ); 

  /// creates a vxDOMElement from this node
  vxDOMElement ToElement();

  /// assignment operator
  vxDOMNode & operator=( const vxDOMNode & rhs );

  /// node as string
  std::string ToString();

// implementation
private:

  /// private constructor
  vxDOMNode( XERCES_CPP_NAMESPACE::DOMNode * pNode ) { m_pNode = pNode; }

// member variables
private:

  /// xerces implementation
  XERCES_CPP_NAMESPACE::DOMNode * m_pNode;

// friend classes
private:
  
  friend class vxDOMAttribute;
  friend class vxDOMDocument;
  friend class vxDOMElement;
  friend class vxDOMNamedNodeMap;
  friend class vxDOMNodeList;

}; // class vxDOMNode


// $Log: vxDOMNode.h,v $
// Revision 1.4  2007/08/22 19:07:04  geconomos
// added ToString method
//
// Revision 1.3  2006/01/27 20:36:15  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.2  2005/11/18 21:12:52  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.8.1  2006/01/10 14:14:02  frank
// Added node name property
//
// Revision 1.1  2005/04/12 15:20:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxDOMNode.h,v 1.4 2007/08/22 19:07:04 geconomos Exp $
// $Id: vxDOMNode.h,v 1.4 2007/08/22 19:07:04 geconomos Exp $
