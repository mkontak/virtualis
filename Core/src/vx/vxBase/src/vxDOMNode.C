// $Id: vxDOMNode.C,v 1.4 2007/08/22 19:07:04 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "vxDOMNode.h"
#include "vxDOMNodeList.h"
#include "vxDOMDocument.h"
#include "vxDOMElement.h"
#include "vxDOMNamedNodeMap.h"
#include "vxDOMParseErrorHandler.h"

// namespace declarations
using namespace XERCES_CPP_NAMESPACE;


/**
 * Default constructor
 */
vxDOMNode::vxDOMNode() : m_pNode( NULL )
{
} // vxDOMNode()


/**
 * Gets a name mapped containing all attributes for this node 
 *
 * @return attributes map
 */
vxDOMNamedNodeMap vxDOMNode::GetAttributes() const
{
  XERCES_CPP_NAMESPACE::DOMNamedNodeMap * pMap( NULL );
  try
  {
    pMap = m_pNode->getAttributes();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetAttributes" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetAttributes" );
  }  
  return vxDOMNamedNodeMap( pMap );
} // GetAttributes()


/**
 * Gets a node list containing all children for this node
 *
 * @return child node list
 */
vxDOMNodeList vxDOMNode::GetChildNodes() const
{
  XERCES_CPP_NAMESPACE::DOMNodeList * pNodeList( NULL );
  try
  {
    pNodeList = m_pNode->getChildNodes();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetChildNodes" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetChildNodes" );
  }  
  return vxDOMNodeList( pNodeList );
} // GetChildNodes()


/**
 * Gets the first child node for this node 
 *
 * @return first child node
 */
vxDOMNode vxDOMNode::GetFirstChild() const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->getFirstChild();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetFirstChild" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetFirstChild" );
  }  
  return vxDOMNode( pNode );
} // GetFirstChild()


/**
 * Gets the last child node for this node 
 *
 * @return  last child node
 */
vxDOMNode vxDOMNode::GetLastChild() const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->getLastChild();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetLastChild" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetLastChild" );
  }  
  return vxDOMNode( pNode );
} // GetLastChild()


/**
 * Gets the next sibling node
 *
 * @return next sibling node
 */
vxDOMNode vxDOMNode::GetNextSibling() const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->getNextSibling();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNextSibling" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNextSibling" );
  }  
  return vxDOMNode( pNode );
} // GetNextSibling()


/**
 * Gets the node type
 *
 * @return node type
 */
int4 vxDOMNode::GetNodeType() const
{
  int4 iNodeType( -1 );
  try
  {
    iNodeType = m_pNode->getNodeType();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNodeType" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNodeType" );
  }  
  return iNodeType;
} // GetNodeType()


/**
 * Gets the value of this node
 *
 * @return node value
 */
std::string vxDOMNode::GetNodeValue() const
{
  std::string sValue;
  try
  {
    char * sz = XMLString::transcode( m_pNode->getTextContent() );
    sValue = sz;
    XMLString::release( &sz );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNodeValue" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNodeValue" );
  }  
  return sValue;
} // GetNodeValue()


/**
 * Sets the value of this node
 *
 * @param   sValue    Node value
 */
void vxDOMNode::SetNodeValue( const std::string & sValue )
{
  try
  {
    XMLCh * value = XMLString::transcode( sValue.c_str() );
    m_pNode->setTextContent( value );
    XMLString::release( &value );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "SetNodeValue" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "SetNodeValue" );
  }  
} // SetNodeValue()


/**
 * Gets the document containing this node
 *
 * @return    Owner document
 */
vxDOMDocument vxDOMNode::GetOwnerDocument() const
{
  XERCES_CPP_NAMESPACE::DOMDocument * pDocument;
  try
  {
    pDocument = m_pNode->getOwnerDocument();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetOwnerDocument" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetOwnerDocument" );
  }  
  return vxDOMDocument( pDocument );
} // GetOwnerDocument()


/**
 * Gets the parent node for this node 
 *
 * @return  Parent node
 */
vxDOMNode vxDOMNode::GetParentNode() const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->getParentNode();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetParentNode" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetParentNode" );
  }  
  return vxDOMNode( pNode );
} // GetParentNode()


/**
 * Gets the previous sibling node
 *
 * @return  Previous sibling node
 */
vxDOMNode vxDOMNode::GetPreviousSibling() const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->getPreviousSibling();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetPreviousSibling" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetPreviousSibling" );
  }  
  return vxDOMNode( pNode );
} // GetPreviousSibling()


/**
 * Gets the name of this node
 *
 * @return node name
 */
std::string vxDOMNode::GetNodeName() const
{
  std::string sName;
  try
  {
    char * sz = XMLString::transcode( m_pNode->getNodeName() );
    sName = sz;
    XMLString::release( &sz );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNodeName" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "GetNodeName" );
  }  
  return sName;
} // GetNodeName()


/**
 *  Appends the node at the end of the child nodes for this node 
 *
 * @param   newChild    Child to append
 * @return  newly appended child
 */
vxDOMNode vxDOMNode::AppendChild( const vxDOMNode & newChild )
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->appendChild( newChild.m_pNode );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "AppendChild" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "AppendChild" );
  }  
  return vxDOMNode( pNode );
} // AppendChild()


/**
 * Gets an exact clone of this node. 
 *
 * @param   bCloneChildren  if true, the cloned node contains all the child nodes as well 
 * @return  cloned node
 */
vxDOMNode vxDOMNode::CloneNode( bool bCloneChildren )
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->cloneNode( bCloneChildren );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "CloneNode" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "CloneNode" );
  }  
  return vxDOMNode( pNode );
} // CloneNode()


/**
 * Returns true if this node has any child nodes 
 *
 * @return true ifnode has children
 */
bool vxDOMNode::HasChildNodes()
{
  bool bHasChildNodes( false );
  try
  {
    bHasChildNodes = m_pNode->hasChildNodes();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "HasChildNodes" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "HasChildNodes" );
  }  
  return bHasChildNodes;
} // HasChildNodes()


/**
 * Inserts a new node, newNode, before the existing node, refNode 
 *
 * @param     newNode     node to insert
 * @param     refNode     inserts the node before this node
 * @return    inserted node
 */
vxDOMNode vxDOMNode::InsertBefore( const vxDOMNode & newNode, const vxDOMNode &  refNode )
{
 XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->insertBefore( newNode.m_pNode, refNode.m_pNode );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "InsertBefore" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "InsertBefore" );
  }  
  return vxDOMNode( pNode );
} // InsertBefore()


/**
 * Removes the specified node
 *
 * @param     node     node to remove
 * @return    removed node
 */
vxDOMNode vxDOMNode::RemoveChild( const vxDOMNode & node )
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->removeChild( node.m_pNode );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "RemoveChild" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "RemoveChild" );
  }  
  return vxDOMNode( pNode );
 } // RemoveChild()


/**
 * Replaces a node with another
 *
 * @param   newNode     new node
 * @param   oldNode     node to replace
 */
vxDOMNode vxDOMNode::ReplaceChild( const vxDOMNode & newNode, const vxDOMNode & oldNode )
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pNode->replaceChild( newNode.m_pNode, oldNode.m_pNode );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "ReplaceChild" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNode", "ReplaceChild" );
  }  
  return vxDOMNode( pNode );
} // ReplaceChild()


/**
 * Creates a vxDOMElement from this node
 *
 * @return node of type element
 */
//vxDOMElement vxDOMNode::ToElement()
//{
//  return vxDOMElement( (XERCES_CPP_NAMESPACE::DOMElement *)(void*)m_pNode );
//} // ToElement()

vxDOMElement vxDOMNode::ToElement()
{
	return  vxDOMElement(dynamic_cast<XERCES_CPP_NAMESPACE::DOMElement *> (m_pNode)); //(XERCES_CPP_NAMESPACE::DOMElement *) m_pNode );
} // ToElement()




/**
 * Assignment operator
 *
 * @param     rhs   right hand side of asignment
 * @return    reference to this instance
 */
vxDOMNode & vxDOMNode::operator=( const vxDOMNode & rhs )
{
  if( this == &rhs )
    return *this;

  m_pNode = rhs.m_pNode;

  return *this;
} // operator=()


/**
 * Returns the contents of the document as a string
 *
 * @return    node as string
 */
std::string vxDOMNode::ToString()
{
  std::string sXml;

  static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
  DOMImplementation *impl = 
     DOMImplementationRegistry::getDOMImplementation(gLS);

  DOMLSSerializer* pWriter = ((DOMImplementationLS*)impl)->createLSSerializer();
 
  // serialize the DOMNode to a UTF-16 string
  XMLCh* theXMLString_Unicode = pWriter->writeToString(m_pNode);

  char * sText = XMLString::transcode( theXMLString_Unicode );
  sXml = sText;
  XMLString::release( &sText );

  // release the memory
  XMLString::release(&theXMLString_Unicode); 
  
  pWriter->release();

  return sXml;
} // ToString()

// $Log: vxDOMNode.C,v $
// Revision 1.4  2007/08/22 19:07:04  geconomos
// added ToString method
//
// Revision 1.3  2006/08/23 19:28:11  romy
// modified Read to eliminate null DOM character elements
//
// Revision 1.2  2006/01/27 20:36:14  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.8.1  2006/01/10 14:14:02  frank
// Added node name property
//
// Revision 1.1  2005/04/12 15:20:57  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxDOMNode.C,v 1.4 2007/08/22 19:07:04 geconomos Exp $
// $Id: vxDOMNode.C,v 1.4 2007/08/22 19:07:04 geconomos Exp $
