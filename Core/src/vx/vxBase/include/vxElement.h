// $Id: vxElement.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: George Economos (mailto:george@viatronix.com)

#pragma once

// class definition
class VX_BASE_DLL vxElement
{
// functions
public:

  /// destructor
  virtual ~vxElement(); 
}; // vxElement


// Revision History:
// $Log: vxElement.h,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/07/05 15:07:14  romy
// moved from vxViewer to vxBase
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/08/20 14:09:58  michael
// added "mailto:"
//
// Revision 1.3  2003/05/06 15:39:26  michael
// initial code reviews
//
// Revision 1.2  2002/10/17 15:30:01  frank
// george == geigr ?
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.3  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.2  2002/06/11 15:19:20  manju
// Added virtual destructor.
//
// Revision 1.1  2002/06/05 21:38:11  geconomos
// Initial extended element implementation.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxElement.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: vxElement.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $