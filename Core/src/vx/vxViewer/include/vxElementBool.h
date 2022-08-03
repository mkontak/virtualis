// $Id: vxElementBool.h,v 1.2 2004/03/30 18:17:31 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Wenli Cai (mailto:wenli@viatronix.com)

#ifndef vxElementBool_h
#define vxElementBool_h


// includes
#include "vxElement.h"


// class definition
class VX_VIEWER_DLL vxElementBool : public vxElement
{
// functions
public:

  /// constructor
  vxElementBool( bool bValue = false ) : m_bValue( bValue ) {}

  /// returns value
  bool GetValue() { return m_bValue; }

  /// sets value
  void SetValue( const bool bValue ) { m_bValue = bValue; }

/// data
private:

  /// value
  bool m_bValue;

}; // class vxElementBool


#endif // vxElementBool_h


// $Log: vxElementBool.h,v $
// Revision 1.2  2004/03/30 18:17:31  frank
// fixed old cut-paste error
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/12/16 13:33:46  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.1  2003/08/11 13:14:29  wenli
// Create vxElementBool
//

// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementBool.h,v 1.2 2004/03/30 18:17:31 frank Exp $
// $Id: vxElementBool.h,v 1.2 2004/03/30 18:17:31 frank Exp $
