// $Id: LinearVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: 25 October 2000
// Author: the coding mcmanamon
//
// Complete history at bottom of file.

#include "stdafx.h"
#include "LinearVolume.h" // header
#include "BlockVolume.h"

using namespace std;

bool LinearVolume<int4>::Test(ostream & os) 
{
  bool ok(true);
  ok = ok && LinearVolume<uint1>::TestByType(os);
  ok = ok && LinearVolume<int4 >::TestByType(os);
  ok = ok && LinearVolume<float4>::TestByType(os);
  ok = ok && LinearVolume<bool >::TestByType(os);
  return (ok);
}




// complete revision history:
// $Log: LinearVolume.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.1  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:04   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.11  2001/06/25 21:23:56  geconomos
// moved from 51_applicationModel
//
// Revision 1.10  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.9  2001/04/12 18:54:15  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.8  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.7  2001/04/09 22:25:44  dave
// changing from Abort to use exceptions
//
// Revision 1.6  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.5  2001/03/30 18:50:04  dmitry
// Updated datatypes.
//
// Revision 1.4  2000/12/19 16:59:48  sarang
// BUG_FIX : removed an inline which shouldn't have been there - thanks gcc
//
// Revision 1.3  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.2  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.1  2000/10/26 02:47:26  ingmar
// test functions
//
// $Id: LinearVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
