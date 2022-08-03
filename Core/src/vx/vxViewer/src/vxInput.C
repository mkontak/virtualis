// $Id: vxInput.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxInput.h"


// static initialization
bool vxInput::m_bModal = false;


// $Log: vxInput.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.4  2003/05/01 13:20:53  michael
// cosmetics
//
// Revision 1.3  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.2  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
//
// Revision 1.1  2002/09/25 16:53:52  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxInput.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxInput.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $