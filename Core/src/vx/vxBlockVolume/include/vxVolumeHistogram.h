// $Id: vxVolumeHistogram.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com


#ifndef vxVolumeHistogram_h
#define vxVolumeHistogram_h


// include declarations
#include "vxBlockVolume.h"


// class definition
class VX_BLOCK_VOLUME_DLL vxVolumeHistogram 
{
public:

  ///////////////////////////////////////////////////////////////////////
  // constructor & assignment

  /// int2   vol Contructor.
  vxVolumeHistogram(vxBlockVolume< int2>  & riVol);

  /// uint2  vol Contructor.
  vxVolumeHistogram(vxBlockVolume<uint2>  & ruVol);

  /// float4 vol Contructor.
  vxVolumeHistogram(vxBlockVolume<float4> & rfVol, int4 iNumBins=4096);

  /// assignment operator
  //vxVolumeHistogram & operator= (const vxVolumeHistogram & src) {}

  ///////////////////////////////////////////////////////////////////////
  // get & set info

  /// Returns the count for the bin specified by index.
  inline int4 GetCountAt(int4 iIndex) const;

  /// Returns the maximum count of all bins.
  //inline int4 GetCountMax() const {}

  /// Returns number of bins (how many bins there are).
  inline int4 GetNumberOfBins() const;



private:
  ///////////////////////////////////////////////////////////////////////
  // restricted functions

  /// Default Contructor.
  vxVolumeHistogram( );

  /// Copy Contructor.
  vxVolumeHistogram(const vxVolumeHistogram & src);

  ///////////////////////////////////////////////////////////////////////
  // data members

  /// the vector of counts (occurrances) of values in the bin associated withthe same index
  std::vector<int4> m_vCount;


}; // class vxVolumeHistogram


#include "vxVolumeHistogram.inl"

#endif // vxVolumeHistogram_h


// $Log: vxVolumeHistogram.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/16 11:51:41  michael
// code review
//
// Revision 1.2  2003/04/30 20:07:30  frank
// Won't compile as is...
//
// Revision 1.1  2002/10/01 19:53:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxVolumeHistogram.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxVolumeHistogram.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $