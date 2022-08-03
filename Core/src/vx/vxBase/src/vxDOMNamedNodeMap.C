// $Id: vxDOMNamedNodeMap.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "vxDOMNamedNodeMap.h"
#include "vxDOMDocument.h"
#include "vxDOMNode.h"

// namespace declaration
using namespace XERCES_CPP_NAMESPACE;

/**
 * Default constructor
 */
vxDOMNamedNodeMap::vxDOMNamedNodeMap()
{
} // vxDOMNamedNodeMap()


/**
 * Private constructor
 */
vxDOMNamedNodeMap::vxDOMNamedNodeMap( XERCES_CPP_NAMESPACE::DOMNamedNodeMap * pMap ) : 
  m_pMap( pMap )
{
} // vxDOMNamedNodeMap


/**
 * Virtual destructor
 */
vxDOMNamedNodeMap::~vxDOMNamedNodeMap()
{
} // ~vxDOMNamedNodeMap()


/**
 * Adds a named node to the map
 *
 * @param     node    node to add
 * @return    added named node
 */
vxDOMNode vxDOMNamedNodeMap::SetNamedItem( const vxDOMNode & node )
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pMap->setNamedItem( node.m_pNode );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "SetNamedItem" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "SetNamedItem" );
  }  
  return vxDOMNode( pNode );
} // SetNamedItem()


/**
 * Gets the node at the specified index
 *
 * @param   iIndex    Index of node
 * @return  node at the specified index
 */
vxDOMNode vxDOMNamedNodeMap::Item( int4 iIndex ) const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode = m_pMap->item( iIndex );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "Item" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "Item" );
  }  
  return vxDOMNode( pNode );
} // Item()


/**
 * Gets a node specified by name
 *
 * @param   sName   Name of node
 * @rturn   node with specified name
 */
vxDOMNode vxDOMNamedNodeMap::GetNamedItem( const std::string & sName ) const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    XMLCh * name = XMLString::transcode( sName.c_str() );
    pNode = m_pMap->getNamedItem( name );
    XMLString::release( &name );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "GetNamedItem" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "GetNamedItem" );
  }  
  return vxDOMNode( pNode );

} // GetNamedItem()


/**
 * Gets the number of entries in the map
 *
 * @return  number of entries in the map
 */
int4 vxDOMNamedNodeMap::GetLength() const
{
  int4 iLength( 0 );
  try
  {
    iLength = m_pMap->getLength();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "GetLength" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "GetLength" );
  }  
  return iLength;
} // GetLength()


/**
 * Removes a node specified by name
 *
 * @param   sName   name of node to remove
 * @return  removed node
 */
vxDOMNode vxDOMNamedNodeMap::RemoveNamedItem( const std::string & sName )
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    XMLCh * name = XMLString::transcode( sName.c_str() );
    pNode = m_pMap->removeNamedItem( name );
    XMLString::release( &name );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "RemoveNamedItem" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNamedNodeMap", "RemoveNamedItem" );
  }  
  return vxDOMNode( pNode );
} // RemoveNamedItem();


/**
 * assigment operator
 *
 * @param     rhs   right hand side of expression
 * @return    reference to this instance
 */
vxDOMNamedNodeMap & vxDOMNamedNodeMap::operator=( const vxDOMNamedNodeMap & rhs )
{
  if( this == &rhs )
    return *this;


  m_pMap = rhs.m_pMap;

  return *this;
} // operator=


// $Log: vxDOMNamedNodeMap.C,v $
// Revision 1.1  2005/04/12 15:20:57  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxDOMNamedNodeMap.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
// $Id: vxDOMNamedNodeMap.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
