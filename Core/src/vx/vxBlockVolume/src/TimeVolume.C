// $Id: TimeVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete history at bottom of file.

#include "stdafx.h"
#include "TimeVolume.h"

// $Log: TimeVolume.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 11:53:31  michael
// code review
//
// Revision 1.3  2002/02/22 00:26:27  frank
// Wrote I/O routines and test for class.
//
// Revision 1.2  2002/02/15 20:19:12  frank
// Revised entire class so that it stores a vector of volumes.
//
// Revision 1.1  2002/01/11 04:55:11  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/TimeVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: TimeVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
