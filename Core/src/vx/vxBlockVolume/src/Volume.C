// $Id: Volume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/Volume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Description: see .h
// Created: 25 October 2000
// Author: the coding mcmanamon
//
// Revision history:
// $Log: Volume.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.1  2003/05/16 11:54:10  michael
// code review
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:10   ingmar
// Initial revision.
// Revision 1.2  2001/09/10 02:18:23  soeren
// some changes for new directory structure
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.2  2000/10/31 17:16:03  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.1  2000/10/26 02:47:26  ingmar
// test functions
//
//
// Complete history at bottom of file.

#include "stdafx.h"
#include "Volume.h" // header

using namespace std;

// <int, int> will cause compile errors due to type checking
bool Volume<BlockVolume<int>,int>::Test(ostream & os) 
{
  bool ok(true);
  //ok = ok && Volume<LinearVolume<bool>, bool>::TestByType(os);
  //ok = ok && Volume<LinearVolume<uchar>, uchar>::TestByType(os);
  //ok = ok && Volume<LinearVolume<int>, int>::TestByType(os);
  //ok = ok && Volume<LinearVolume<float>, float>::TestByType(os);

  ok = ok && Volume<BlockVolume<bool>, bool>::TestByType(os);
  ok = ok && Volume<BlockVolume<uint1>, uint1>::TestByType(os);
  ok = ok && Volume<BlockVolume<int>, int>::TestByType(os);
  ok = ok && Volume<BlockVolume<float>, float>::TestByType(os);
  
  return (ok);
}


// complete revision history:
// $Log: Volume.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.1  2003/05/16 11:54:10  michael
// code review
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// Revision 1.2  2001/09/10 02:18:23  soeren
// some changes for new directory structure
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.2  2000/10/31 17:16:03  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.1  2000/10/26 02:47:26  ingmar
// test functions
//
// $Id: Volume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
