// $Id: vxElementPlane.h,v 1.1 2008/01/28 19:53:34 cqian Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Cheng Qian (cqian@viatronix.com)



#ifndef vxElementPlane_h
#define vxElementPlane_h

// pragmas
#pragma once

// includes
#include "vxElement.h"
#include "Plane.h"


// class definition
class  vxElementPlane : public vxElement
{
// functions
public:

  /// constructor
  vxElementPlane() {}

  /// returns value
  Plane & GetPlane() { return m_plane; }

  /// sets value
  void SetPlane( const Plane & plane ) { m_plane = plane; }

// data
private:

  /// value
  Plane  m_plane;

}; // vxElementPlane



#endif // vxElementPlane_h


// $Log: vxElementPlane.h,v $
// Revision 1.1  2008/01/28 19:53:34  cqian
// Add a cut plane for LV segmentation
//

// $Id: vxElementPlane.h,v 1.1 2008/01/28 19:53:34 cqian Exp $
