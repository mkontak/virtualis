// $Id: vxElementPixelRGBA.h,v 1.1 2006/03/27 20:01:52 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Greg Davidson (gdavidson@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxElement.h"
#include "PixelRGBA.h"


// class definition
template <class T>
class VX_VIEWER_DLL vxElementPixelRGBA : public vxElement
{
// functions
public:

  /// constructor
  vxElementPixelRGBA() { };

  /// returns value
  const PixelRGBA<T> & GetValue() const { return m_value; }

  /// sets value
  void SetValue( const PixelRGBA<T> & value ) { m_value = value; }

// data
private:

  /// value
  PixelRGBA<T> m_value;

}; // vxElementPixelRGBA



// $Log: vxElementPixelRGBA.h,v $
// Revision 1.1  2006/03/27 20:01:52  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementPixelRGBA.h,v 1.1 2006/03/27 20:01:52 gdavidson Exp $
// $Id: vxElementPixelRGBA.h,v 1.1 2006/03/27 20:01:52 gdavidson Exp $
