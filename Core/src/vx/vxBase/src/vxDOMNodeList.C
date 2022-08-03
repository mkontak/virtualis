// $Id: vxDOMNodeList.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "vxDOMNodeList.h"
#include "vxDOMDocument.h"

/**
 * Default constructor
 */
vxDOMNodeList::vxDOMNodeList() : m_pList( NULL )
{
} // vxDOMNodeList()


/**
 * Private constructor
 */
vxDOMNodeList::vxDOMNodeList( XERCES_CPP_NAMESPACE::DOMNodeList * pList ) : m_pList( pList )
{
} // vxDOMNodeList


/**
 * Gets the number of nodes in the list
 *
 * @return node count
 */
int vxDOMNodeList::GetLength() const
{
  int iLength( 0 );
  try
  {
    iLength = m_pList->getLength();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNodeList", "GetLength" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNodeList", "GetLength" );
  }  
  return iLength;
} // GetLength()


/** 
 * Gets a specific node in the list 
 *
 * @param   iIndex  Index of node to get
 * @return  requested node
 */
vxDOMNode vxDOMNodeList::Item( int4 iIndex ) const
{
  XERCES_CPP_NAMESPACE::DOMNode * pNode( NULL );
  try
  {
    pNode= m_pList->item( iIndex );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNodeList", "Item" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMNodeList", "Item" );
  }
  return vxDOMNode( pNode );
} // Item()


// $Log: vxDOMNodeList.C,v $
// Revision 1.1  2005/04/12 15:20:57  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxDOMNodeList.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
// $Id: vxDOMNodeList.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
