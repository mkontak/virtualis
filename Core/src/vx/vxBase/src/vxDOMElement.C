// $Id: vxDOMElement.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "vxDOMElement.h"
#include "vxDOMAttribute.h"
#include "vxDOMNode.h"
#include "vxDOMDocument.h"
#include "vxDOMNodeList.h"

// namespace declarations
using namespace XERCES_CPP_NAMESPACE;

/**
 * Default constructor
 */
vxDOMElement::vxDOMElement() : m_pElement( NULL )
{
} // vxDOMElement()


/**
 * Private constructor
 *
 * @param   pElement    xerces dom element implementation
 */
vxDOMElement::vxDOMElement( DOMElement * pElement ) : 
  vxDOMNode( ( XERCES_CPP_NAMESPACE::DOMNode * )( void* ) pElement ),
  m_pElement( pElement )
{
} // vxDOMElement()


/**
 * Gets the name of this node
 *
 * @return    node name
 */
std::string vxDOMElement::GetTagName() const
{
  std::string sTagName;
  try
  {
    char * sName = XMLString::transcode( m_pElement->getTagName() );
    sTagName = sName;
    XMLString::release( &sName );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetTagName" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetTagName" );
  }  
  return sTagName;
} // GetTagName()


/**
 * Sets the name of this node
 *
 * @param   sTagName   new name for element
 */
void vxDOMElement::SetTagName( const std::string & sTagName )
{
  try
  {
    XMLCh * name = XMLString::transcode( sTagName.c_str() );
    m_pElement->getOwnerDocument()->renameNode( m_pElement, NULL, name );
    XMLString::release( &name );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "SetTagName" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "SetTagName" );
  }  
} // SetTagName()


/*
 * Gets the value of the specified attribute.
 *
 * @param     sName   name of attribute to retrieve value for
 * @return    value of attribute
 */
std::string vxDOMElement::GetAttribute( const std::string & sName ) const
{
  std::string sValue;
  try
  {
    XMLCh * name = XMLString::transcode( sName.c_str() );
    const XMLCh * value = m_pElement->getAttribute( name );    
    XMLString::release( &name );

    char * tmp = XMLString::transcode( value );
    sValue = tmp;
    XMLString::release( &tmp );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetAttribute" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetAttribute" );
  }  
  return sValue;
} // GetAttribute()


/**
 * Gets the specified attribute node 
 *
 * @param     sName   nae of attribute to retrieve
 * @return    attribute node
 */
vxDOMAttribute vxDOMElement::GetAttributeNode( const std::string sName ) const
{
  XERCES_CPP_NAMESPACE::DOMAttr * pAttr( NULL );
  try
  {
    XMLCh * name = XMLString::transcode( sName.c_str() );
    pAttr = m_pElement->getAttributeNode( name );
    XMLString::release( &name );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetAttributeNode" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetAttributeNode" );
  }  
  return vxDOMAttribute( pAttr );
} // GetAttributeNode()


/**
 * Gets the specified node, and all its child nodes, as a nodeList 
 *
 * @param   sTagName    element name to retrieve
 * @return    list of nodes with the specified tag name
 */
vxDOMNodeList vxDOMElement::GetElementsByTagName( const std::string sTagName ) const
{
  XERCES_CPP_NAMESPACE::DOMNodeList * pList( NULL );
  try
  {
    XMLCh * name = XMLString::transcode( sTagName.c_str() );
    pList = m_pElement->getElementsByTagName( name );
    XMLString::release( &name );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetElementsByTagName" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "GetElementsByTagName" );
  }  
  return vxDOMNodeList( pList );
} // GetElementsByTagName


/**
 * Puts the text nodes for this element, and its child nodes, into one text node
 */
void vxDOMElement::Normalize()
{
  try
  {
    m_pElement->normalize();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "Normalize" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "Normalize" );
  }  
} // Normalize()


/**
 * Removes the specified attribute's value
 *
 * @param   sName   attribute name
 */
void vxDOMElement::RemoveAttribute( const std::string sName )
{
  try
  {
    XMLCh * name = XMLString::transcode( sName.c_str() );
    m_pElement->removeAttribute( name );
    XMLString::release( &name );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "RemoveAttribute" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "RemoveAttribute" );
  }  
} // RemoveAttribute()


/**
 * Removes the specified attribute node.
 *
 * @param   node    attribute node to remove
 * @return removed attribute node
 */
vxDOMAttribute vxDOMElement::RemoveAttributeNode( const vxDOMAttribute & node )
{
  XERCES_CPP_NAMESPACE::DOMAttr * pAttr( NULL );
  try
  {
    pAttr = m_pElement->removeAttributeNode( node.m_pAttribute );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "RemoveAttributeNode" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "RemoveAttributeNode" );
  }  
  return vxDOMAttribute( pAttr );
} // RemoveAttributeNode()


/**
 * Inserts a new attribute and sets it's value
 *
 * @param     sName     attribute name
 * @param     sValue    attribute value
 * @return    attribute node
 */
void vxDOMElement::SetAttribute( const std::string & sName, const std::string &  sValue )
{
  try
  {
    XMLCh * name = XMLString::transcode( sName.c_str() );
    XMLCh * value = XMLString::transcode( sValue.c_str() );
    m_pElement->setAttribute( name, value );
    XMLString::release( &name );
    XMLString::release( &value );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "SetAttribute" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "SetAttribute" );
  }  
} // SetAttribute()


/**
 * Inserts the specified attribute node
 *
 * @param     attribute     attribute node
 * @return    added attribute node
 */
vxDOMAttribute vxDOMElement::SetAttributeNode( const vxDOMAttribute & attribute )
{
  XERCES_CPP_NAMESPACE::DOMAttr * pAttr( NULL );
  try
  {
    pAttr = m_pElement->setAttributeNode( attribute.m_pAttribute );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "SetAttributeNode" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMElement", "SetAttributeNode" );
  }  
  return vxDOMAttribute( pAttr );
} // SetAttributeNode()


/**
 * assigment operator
 *
 * @param     rhs   right hand side of expression
 * @return    reference to this instance
 */
vxDOMElement & vxDOMElement::operator=( const vxDOMElement & rhs )
{
  if( this == &rhs )
    return *this;

  vxDOMNode::operator=( rhs );

  m_pElement = rhs.m_pElement;

  return *this;
} // operator=()


// $Log: vxDOMElement.C,v $
// Revision 1.1  2005/04/12 15:20:57  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxDOMElement.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
// $Id: vxDOMElement.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
