// $Id: vxInserter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar ingmar@viatronix.com

#ifndef vxInserter_h
#define vxInserter_h


// includes
#include  "vxBlockVolumeIteratorNeighbors.h"
   
// class definition
class vxInserter 
{
// functions
public:

  /// constructor
  vxInserter() {};
  
  /// need to update heap
  virtual bool NeedToUpdateHeap(const vxComplex & src, vxComplex & dst, Vector3D<int4> iPos, uint4 neighborIndex) { return true; }
}; // vxInserter
   

#endif // vxInserter_h


// $Log: vxInserter.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/03 16:18:21  michael
// completed code review
//
// Revision 1.3  2002/09/17 17:17:54  dongqing
// Tiny bug fixing and comments after testing on the Skeletonator Module
//
// Revision 1.2  2002/08/28 16:49:19  ingmar
// replaced cxCost by vxComplex
//
// Revision 1.1  2002/08/28 15:06:53  ingmar
// initial version
//
// $Id: vxInserter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
