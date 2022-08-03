// $Id: vxDOMNodeList.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
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

// include declarations
#include "vxDOMNode.h"

// xerces forward declarations
namespace XERCES_CPP_NAMESPACE {
  class DOMNodeList;
}

// class declaration
class VX_BASE_DLL vxDOMNodeList
{
// member functions
public:

  /// default constructor
  vxDOMNodeList();

  /// gets the number of nodes in the list
  int GetLength() const;

  /// gets a specific node in the list 
  vxDOMNode Item( int4 iIndex ) const;

  /// assigment operator
  vxDOMNodeList & operator=( const vxDOMNodeList & rhs );

// implementation
private:

  /// private constructor
  vxDOMNodeList( XERCES_CPP_NAMESPACE::DOMNodeList * pList );

// member variables
private:

  /// xerces implementation
  XERCES_CPP_NAMESPACE::DOMNodeList * m_pList;

// friend declarations
private:  
  
  friend class vxDOMDocument;
  friend class vxDOMNode;
  friend class vxDOMElement;
  friend class vxDOMAttribute;

}; // class vxDOMNodeList


// $Log: vxDOMNodeList.h,v $
// Revision 1.1  2005/04/12 15:20:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxDOMNodeList.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
// $Id: vxDOMNodeList.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
