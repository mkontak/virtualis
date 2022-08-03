// $Id: vxElementFloat4.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxElementFloat4_h
#define vxElementFloat4_h


// includes
#include "vxElement.h"


// class definition
class VX_VIEWER_DLL vxElementFloat4 : public vxElement
{
// member functions
public:

  /// constructor
  vxElementFloat4() : m_fValue( 0 ) {}

  /// returns value
  float4 GetValue() { return m_fValue; }

  /// sets value
  void SetValue( const float4 fValue ) { m_fValue = fValue; }

// data members
private:

  /// value
  float4 m_fValue;

}; // vxElementFloat4


#endif // vxElement_h


// $Log: vxElementFloat4.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/12/16 13:33:46  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.3  2003/05/08 12:10:40  frank
// code review
//
// Revision 1.2  2003/05/06 15:39:26  michael
// initial code reviews
//
// Revision 1.1  2003/03/13 14:07:21  frank
// Added new floating point element
//
// Revision 1.1  2002/08/02 22:39:30  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementFloat4.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxElementFloat4.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
