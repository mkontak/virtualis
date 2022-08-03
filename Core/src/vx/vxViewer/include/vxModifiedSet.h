// $Id: vxModifiedSet.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

/**
 *  Define the modified set
 */

#ifndef vxModifiedSet_h
#define vxModifiedSet_h


// includes
#include "vxModifiedInfoList.h"


// forward declaration
class vxModifiedInfo;
class vxObserver;


// class definition
class VX_VIEWER_DLL vxModifiedSet
{
// functions
public:

  /// constructor
  vxModifiedSet(vxObserver * pClient, const vxModifiedInfo & info);

  // destructor
  virtual ~vxModifiedSet() {};

  /// return a pointer to the client
  vxObserver * GetClient() { return m_pClient; }

  /// returns a reference to the messages
  vxModifiedInfoList & GetMessages() { return m_messages; }

  /// appends the collection of messages
  void AddMessage(const vxModifiedInfo & info);

// data
private:

  // client pointer
  vxObserver * m_pClient;

  // contains modified messages 
  vxModifiedInfoList m_messages;
};


#endif // vxModifiedSet_h


// Revision History:
// $Log: vxModifiedSet.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.4  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.3  2003/05/07 15:01:04  dongqing
// code walkthrough
//
// Revision 1.2  2003/05/06 15:11:40  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.9  2002/07/25 17:43:08  dmitry
// Changed vxModifiedInfo to be passed as a reference not a pointer.
//
// Revision 1.8  2002/07/15 15:21:30  geconomos
// Removed virtually inherited classes.
//
// Revision 1.7  2002/07/08 15:25:27  dmitry
// Changed to work with vxModifiedInfoList.
//
// Revision 1.6  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.5  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.4  2002/05/10 21:11:45  michael
// fixed wrong comment
//
// Revision 1.3  2002/05/07 18:22:43  frank
// Moved includes from .inl file to .h file.
//
// Revision 1.2  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/17 19:23:03  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxModifiedSet.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxModifiedSet.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $