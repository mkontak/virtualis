// $Id: vxElementVector.h,v 1.1 2007/04/06 14:14:21 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "vxElement.h"
#include "Vector3D.h"


// class definition
template <class T>
class VX_VIEWER_DLL vxElementVector : public vxElement
{
// functions
public:

  /// constructor
  vxElementVector() {}

  /// returns value
  const Vector3D<T> & GetValue() const { return m_value; }

  /// sets value
  void SetValue( const Vector3D<T> & value ) { m_value = value; }

// data
private:

  /// value
  Vector3D<T> m_value;

}; // vxElementVector


// $Log: vxElementVector.h,v $
// Revision 1.1  2007/04/06 14:14:21  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementVector.h,v 1.1 2007/04/06 14:14:21 gdavidson Exp $
// $Id: vxElementVector.h,v 1.1 2007/04/06 14:14:21 gdavidson Exp $
