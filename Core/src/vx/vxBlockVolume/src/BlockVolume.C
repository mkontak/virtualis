// $Id: BlockVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
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
#include "Volume.h" // header

using namespace std;

bool BlockVolume<int4>::Test(ostream & os) 
{
  bool ok(true);
  os << "TEST: BlockVolume CLASS TEST (integer) ..." << std::endl;
  //BlockVolume<int1> blub(Triple<uint4>(16,16,16);

  
  
  //Volume<BlockVolume<int4>, int4> vol(dim, sizeof(T) * 8, units,
   // hdr::METER, 46, hdr::VOL,
    //"Test.vol", title1, author);
  //Volume<BlockVolume<T>, T> vol;
  
  
 // os << "TEST: READINGGGGGGGGGGGGGGGGGGGGGGGGGGG ..." << std::endl;
  //vol.Read("DILBERT_1S_0.csi");
  

  ok = ok && BlockVolume<int2>::TestByType(os);
  //os << "TEST: BlockVolume CLASS TEST (float4) ..." << std::endl;
  //ok = ok && BlockVolume<float4>::TestByType(os);
  //os << "TEST: BlockVolume CLASS TEST (uint1) ..." << std::endl;
  //ok = ok && BlockVolume<uint1>::TestByType(os);
  //ok = ok && BlockVolume<bool >::TestByType(os);

//  bool ok(true);

  bool totalOk(true);
  
  string author = "Brett Ashley";
  string title1 = "The Sun Also Rises";
  string fileName = "TheTest.vol";
  Triple<uint4> dim(7,7,7);
  Triple<float4> units(1.0,1.0,1.0);
  
  os << "TEST: SetVoxelRaw() <==> GetVoxelRaw() ..." << std::endl;
  
  BlockVolC vol(dim, sizeof(int1) * 8, units,  hdr::METER, '.', hdr::VOL, fileName, title1, author);

  int1 voxel = 'a';
  vol.SetVoxelRaw(Point<uint2>(0,0,0),(int1) voxel++);
  //  vol.SetVoxelRaw(Point<uint2>(49,49,49),(int1) 91);
  vol.SetVoxelRaw(Point<uint2>(4,4,4),(int1) voxel++);
  vol.SetVoxelRaw(Point<uint2>(4,4,5),(int1) voxel++);
  vol.SetVoxelRaw(Point<uint2>(4,5,4),(int1) voxel++);
  vol.SetVoxelRaw(Point<uint2>(4,5,5),(int1) voxel++);
  vol.SetVoxelRaw(Point<uint2>(5,4,4),(int1) voxel++);
  vol.SetVoxelRaw(Point<uint2>(5,4,5),(int1) voxel++);
  vol.SetVoxelRaw(Point<uint2>(5,5,4),(int1) voxel++);
  vol.SetVoxelRaw(Point<uint2>(5,5,5),(int1) voxel++);

  cout << "BEFORE READ:";
  vol.PrintAllRaw(std::cout);

  voxel = 'a';
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
  //  ok = ok && ((int1)91 == vol.GetVoxelRaw(Point<uint2>(49,49,49)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(4,4,4)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(4,4,5)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(4,5,4)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(4,5,5)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(5,4,4)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(5,4,5)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((int1)voxel++ == vol.GetVoxelRaw(Point<uint2>(5,5,5)));
  
  if ( ok ) os << "TEST  PASSED" << std::endl << std::endl;
  else os << "TEST  FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;
  
  hdr::FileCompressionEnum tmp;
  tmp = (hdr::FileCompressionEnum) 0;
  // vol.SetCompression(tmp);
  vol.Write("TheTest.vol");
  vol.SetVoxelRaw(Point<uint2>(0,0,0), 0);
  //  vol.SetVoxelRaw(Point<uint2>(49,49,49), 0);
  vol.SetVoxelRaw(Point<uint2>(4,4,4), 0);
  vol.SetVoxelRaw(Point<uint2>(4,4,5), 0);
  vol.SetVoxelRaw(Point<uint2>(4,5,4), 0);
  vol.SetVoxelRaw(Point<uint2>(4,5,5), 0);
  vol.SetVoxelRaw(Point<uint2>(5,4,4), 0);
  vol.SetVoxelRaw(Point<uint2>(5,4,5), 0);
  vol.SetVoxelRaw(Point<uint2>(5,5,4), 0);
  vol.SetVoxelRaw(Point<uint2>(5,5,5), 0);
  
  
  vol.Read("TheTest.vol");
  cout << "After READ:";
  vol.PrintAllRaw(std::cout);
 
  return (ok);
}


// complete revision history:
// $Log: BlockVolume.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.1  2003/05/16 11:44:24  michael
// code review
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 20 2001 20:35:48   pagron
// changes to block volume implementation will reduce memory fragmentation
// 
//    Rev 2.0   Sep 16 2001 23:47:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:00   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.11  2001/04/24 03:56:18  dave
// read should fragment disk less
//
// Revision 1.10  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.9  2001/03/30 18:50:04  dmitry
// Updated datatypes.
//
// Revision 1.8  2001/03/30 00:44:42  soeren
// BlockVolume now powered by blocks with Reference Counting
//
// Revision 1.7  2001/03/22 18:54:15  soeren
// Beautified Test function
//
// Revision 1.6  2000/12/19 16:59:45  sarang
// BUG_FIX : removed an inline which shouldn't have been there - thanks gcc
//
// Revision 1.5  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.4  2000/10/26 02:47:30  ingmar
// test functions
//
// $Id: BlockVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
