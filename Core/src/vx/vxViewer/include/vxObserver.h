// $Id: vxObserver.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)

#ifndef vxObserver_h
#define vxObserver_h


// forward declarations
class vxModifiedInfoList;


// class definition
class VX_VIEWER_DLL vxObserver
{
// functions
public:

  /// modify the environment of the client
  virtual void OnModified(vxModifiedInfoList &) = 0;
};


#endif // vxObserver


// Revision History:
// $Log: vxObserver.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/12/16 13:25:07  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.5  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.4  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.3  2003/05/06 13:06:06  geconomos
// Code walkthru.
//
// Revision 1.2  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.5  2002/07/08 15:25:27  dmitry
// Changed to work with vxModifiedInfoList.
//
// Revision 1.4  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.3  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.2  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/17 19:23:03  dmitry
// Initial Revision
//
// Revision 1.1  2002/03/25 00:51:52  michael
// added the renamed fil4es to 74_vxUtiles, avoiding forward dependencies
//
// Revision 1.1  2002/03/07 22:46:42  dmitry
// Initial Revision
//
// Revision 1.1  2002/02/25 18:25:18  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxObserver.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxObserver.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $