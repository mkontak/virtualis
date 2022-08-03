// $Id: vxModifiedInfoList.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

/**
 *  Define the modified information list
 */


#ifndef vxModifiedInfoList_h
#define vxModifiedInfoList_h


// forward declaration
#include "vxModifiedInfo.h."


// class definition
class VX_VIEWER_DLL vxModifiedInfoList 
{
// functions
public:

  /// default constructor
  vxModifiedInfoList();

  /// destructor
  virtual ~vxModifiedInfoList();
  
  /// returns the element at the specified position
  const vxModifiedInfo * At(uint4 uPos);

  /// returns the first element
  const vxModifiedInfo * Begin();

  /// returns the next element from the current position
  const vxModifiedInfo * Next();

  /// returns the number of elements in the list
  int4 Size() const;

  /// adds an element to the list
  void PushBack(const vxModifiedInfo & info);

  /// releases all objects in the list
  void ReleaseObjects();
  
// data
private:

  // contains modified messages 
  std::list<vxModifiedInfo *> m_messages;

  // iterator to the current position in the list
  std::list<vxModifiedInfo *>::iterator m_iter;
};


#endif // vxModifiedInfoList_h


// Revision History:
// $Log: vxModifiedInfoList.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.3  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.2  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.3  2002/07/25 17:45:33  dmitry
// vxModifiedInfo copied and managed as an internal pointer.
//
// Revision 1.2  2002/07/10 11:44:30  geconomos
// exported classes.
//
// Revision 1.1  2002/07/08 15:24:53  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxModifiedInfoList.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxModifiedInfoList.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $