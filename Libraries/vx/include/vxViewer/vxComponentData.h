// $Id: vxComponentData.h,v 1.3 2006/12/14 21:59:08 romy Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxISerialize.h"
#include <string>


/**
 * Any data that may be held by a component.  Each component may have at least one
 * object of this type.  This can hold metadata for the component and is useful
 * for keeping track of coronary plaques, lung nodules, vessels, etc.
 */
class VX_VIEWER_DLL vxComponentData : public vxISerialize
{
// member functions
public:

  /// default constructor
  vxComponentData() {}

  ///Pure virtual Clone method for derived classes
  virtual vxComponentData * Clone() const = 0; 

  /// destructor
  virtual ~vxComponentData() {}

  /// return the component data name
  virtual const std::string GetName() const = 0;

  /// perform a grow on the component data (will be called after the component grows)
  virtual void GrowComponentData() = 0;

  /// perform a shrink on the component data (will be called after the component shrinks)
  virtual void ShrinkComponentData() = 0;

//// vxISerialize functions
//public:
//
//  /// generate Xml node
//  virtual void ToXml(MSXML2::IXMLDOMElementPtr & spElement) const;
//
//  /// initialize from Xml node
//  virtual bool FromXml(MSXML2::IXMLDOMElementPtr & spElement);
//
//  /// name for Xml node
//  virtual const std::string GetElementName() const { return "vxComponentData"; }

}; // class vxComponentData



// $Log: vxComponentData.h,v $
// Revision 1.3  2006/12/14 21:59:08  romy
// added Clone interface to Component Data
//
// Revision 1.2  2005/06/07 17:59:27  vxconfig
// exported class
//
// Revision 1.1  2004/07/12 18:02:40  frank
// added component data
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxComponentData.h,v 1.3 2006/12/14 21:59:08 romy Exp $
// $Id: vxComponentData.h,v 1.3 2006/12/14 21:59:08 romy Exp $
