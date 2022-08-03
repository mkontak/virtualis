// $Id: vxDOMAttribute.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
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
  class DOMAttr;
}

// class declaration
class VX_BASE_DLL vxDOMAttribute : public vxDOMNode
{
// member functions
public:
  
  /// default constructor
  vxDOMAttribute();

  /// gets the name of the attribute 
  std::string GetName() const;
   
  /// indicates if the node's value is set in the document or not 
  bool GetSpecified() const;

  /// gets the value of the attribute 
  std::string GetValue() const;

  /// sets the value of the attribute 
  void SetValue( const std::string & sValue );

  // assigment operator
  vxDOMAttribute & operator=( const vxDOMAttribute & rhs );

// implementation
private:

  // private constructor
  vxDOMAttribute( XERCES_CPP_NAMESPACE::DOMAttr * pAttribute );

// member variables
private:

  /// xerces implementation
  XERCES_CPP_NAMESPACE::DOMAttr * m_pAttribute;

// friend classes
private:

  friend class vxDOMDocument;
  friend class vxDOMElement;

}; // class vxDOMAttribute


// $Log: vxDOMAttribute.h,v $
// Revision 1.1  2005/04/12 15:20:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxDOMAttribute.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
// $Id: vxDOMAttribute.h,v 1.1 2005/04/12 15:20:43 geconomos Exp $
