// $Id: vxElementPoint3D.h,v 1.2 2006/05/18 17:49:18 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxElementPoint3D_h
#define vxElementPoint3D_h


// includes
#include "vxElement.h"
#include "Point3D.h"


// class definition
template <class T>
class VX_VIEWER_DLL vxElementPoint3D : public vxElement
{
// functions
public:

  /// constructor
  vxElementPoint3D() {}

  /// returns value
  Point3D<T> & GetValue() { return m_value; }

  /// sets value
  void SetValue( const Point3D<T> & value ) { m_value = value; }

// data
private:

  /// value
  Point3D<T> m_value;

}; // vxElementPoint3D



#endif // vxElement_h


// $Log: vxElementPoint3D.h,v $
// Revision 1.2  2006/05/18 17:49:18  gdavidson
// Changed the get return value and et parameter to references
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/08 12:10:40  frank
// code review
//
// Revision 1.2  2003/05/06 15:39:26  michael
// initial code reviews
//
// Revision 1.1  2003/03/17 18:13:59  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementPoint3D.h,v 1.2 2006/05/18 17:49:18 gdavidson Exp $
// $Id: vxElementPoint3D.h,v 1.2 2006/05/18 17:49:18 gdavidson Exp $
