// $Id: v3D_ViewerTypes.cpp,v 1.2 2004/04/23 16:52:36 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"

// disable the BOOL forced to bool warning - remove after the 'virtual bool' big goes away
#pragma warning (disable : 4800)

#include "v3D_ViewerTypes.h"


// $Log: v3D_ViewerTypes.cpp,v $
// Revision 1.2  2004/04/23 16:52:36  frank
// workaround the virtual bool bug
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.11  2003/05/16 13:13:57  frank
// formatting
//
// Revision 1.10  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.9  2003/05/13 13:52:11  geconomos
// More coding standards.
//
// Revision 1.8  2003/05/13 13:50:04  geconomos
// Coding standards.
//
// Revision 1.7  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.6  2003/03/20 13:46:02  geconomos
// Moved defintions to header file.
//
// Revision 1.5  2003/03/19 16:16:30  michael
// fixed compile issues and added it to DEFINES in managed code
//
// Revision 1.4  2003/02/28 17:54:33  michael
// partially moved SetupView() from vxViewer over to vxViewerType ...
//
// Revision 1.3  2002/12/18 18:30:48  michael
// cosmetics
//
// Revision 1.2  2002/12/18 18:23:19  michael
// added oblique viewer type
//
// Revision 1.1  2002/12/18 15:41:27  geconomos
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewerTypes.cpp,v 1.2 2004/04/23 16:52:36 frank Exp $
// $Id: v3D_ViewerTypes.cpp,v 1.2 2004/04/23 16:52:36 frank Exp $
