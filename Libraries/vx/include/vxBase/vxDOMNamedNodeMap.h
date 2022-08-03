// $Id: vxDOMNamedNodeMap.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
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

// forward declarations
class vxDOMNode;

namespace XERCES_CPP_NAMESPACE {
  class DOMNamedNodeMap;
}

// class declaration
class VX_BASE_DLL vxDOMNamedNodeMap
{
// member functions
public:
    
  // default constructor
  vxDOMNamedNodeMap();
  
  // virtual destructor
  virtual ~vxDOMNamedNodeMap();

  /// adds a node 
  vxDOMNode SetNamedItem( const vxDOMNode & node );
  
  /// gets the node at the specified index
  vxDOMNode Item( int4 index ) const;  
 
  /// gets a node specified by name
  vxDOMNode GetNamedItem( const std::string & sName ) const; 
  
  /// number of entries in the map
  int4 GetLength () const; 
 
  /// removes a node specified by name
  vxDOMNode RemoveNamedItem( const std::string & sName );

  /// assignment operator
  vxDOMNamedNodeMap & operator=( const vxDOMNamedNodeMap & rhs );

// implementation
private:

  /// private constructor
  vxDOMNamedNodeMap( XERCES_CPP_NAMESPACE::DOMNamedNodeMap * pMap );

// member variables
private:

  // xerces implementation
  XERCES_CPP_NAMESPACE::DOMNamedNodeMap * m_pMap;

// friend declarations
private:

  friend class vxDOMNode;

}; // class vxDOMNamedNodeMap


// $Log: vxDOMNamedNodeMap.h,v $
// Revision 1.1  2005/04/12 15:20:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxDOMNamedNodeMap.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
// $Id: vxDOMNamedNodeMap.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
