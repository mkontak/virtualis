// $Id: vxElementInt4.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxElementInt4_h
#define vxElementInt4_h


// includes
#include "vxElement.h"


// class definition
class VX_VIEWER_DLL vxElementInt4 : public vxElement
{
// functions
public:

  /// constructor
  vxElementInt4() : m_iValue( 0 ) {}

  /// returns value
  int4 GetValue() { return m_iValue; }

  /// sets value
  void SetValue( const int4 iValue ) { m_iValue = iValue; }

/// data
private:

  /// value
  int4 m_iValue;

}; // class vxElementInt4


#endif // vxElement_h


// $Log: vxElementInt4.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/12/16 13:33:46  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.4  2003/05/08 13:30:15  dongqing
// fix a typing error
//
// Revision 1.3  2003/05/08 12:10:40  frank
// code review
//
// Revision 1.2  2003/05/06 15:39:26  michael
// initial code reviews
//
// Revision 1.1  2002/08/02 22:39:30  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementInt4.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxElementInt4.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
