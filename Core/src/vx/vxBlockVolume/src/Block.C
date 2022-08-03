// $Id: Block.C,v 1.2 2005/04/27 17:36:12 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: March. 01
// Author: Ingmar and Soeren {ingmar|soeren}@viatronix.com

// includes
#include "stdafx.h"
#include "Block.h"


#define FILE_REVISION "$Revision: 1.2 $"


#undef FILE_REVISION

// $Log: Block.C,v $
// Revision 1.2  2005/04/27 17:36:12  geconomos
// no longer exporting this class ( soon to be removed )
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/16 11:44:23  michael
// code review
//
// Revision 1.6  2002/06/04 15:47:05  soeren
// uups
//
// Revision 1.5  2002/06/04 03:26:02  soeren
// fixed memory leak
//
// Revision 1.4  2002/03/12 19:50:57  ingmar
// removed mutex locking of critical sections, now its fast again, but not thread save
//
// Revision 1.3  2002/03/11 22:24:29  ingmar
// Fixed little static member initialization raise condition ;-)
//
// Revision 1.2  2002/03/08 16:21:11  ingmar
// reorganized Block class and added mutexes
//
// Revision 1.1  2002/03/06 01:23:11  ingmar
// inital version
//
//
// $Id: Block.C,v 1.2 2005/04/27 17:36:12 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/Block.C,v 1.2 2005/04/27 17:36:12 geconomos Exp $

