// $Id: vxVolumeHistogram.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar ingmar@viatronix.com


// include declarationss
#include "stdafx.h"
#include "vxVolumeHistogram.h"
#include "vxBlockVolumeIterator.h"



// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"

using namespace std;


///////////////////////////////////////////////////////////////////////
// constructors & assignment


/**
 * int2 vol Contructor.
 * Assumed to be CT/MRI data volume with fixed data range of 0-4095.
 */
vxVolumeHistogram::vxVolumeHistogram(vxBlockVolume< int2>  & riVol)
{
  m_vCount.resize(4096);
  vxBlockVolumeIterator<int2> volIter(riVol);
  for ( volIter.SetPos(0,0,0); volIter.IsNotAtEnd(); volIter.NextBlockZYX() )
  {
    for ( ; volIter.IsNotAtEndOfBlock(); volIter.NextZYXinsideBlock() )
    {
      int4 iVoxel = volIter.GetVoxel();
      if ( 0 <= iVoxel  &&  iVoxel < 4096 )
      {
        ++m_vCount[iVoxel];
      }
      else
      {
        throw ex::VException(LogRec("voxel not bount to [0,4095]", "vxVolumeHistogram", "int2 vol constructor"));
      }
    }
  }
} // int2 constructor


/**
 * uint2 vol Contructor.
 * Assumed to be label volume few labels,
 * The constructor will adjust to take all label values from 0 to max.
 */
vxVolumeHistogram::vxVolumeHistogram(vxBlockVolume<uint2>  & ruVol)
{
  vxBlockVolumeIterator<uint2> volIter(ruVol);
  int4 iMaxValue(0);
  for ( volIter.SetPos(0,0,0); volIter.IsNotAtEnd(); volIter.NextBlockZYX() )
  {
    for ( ; volIter.IsNotAtEndOfBlock(); volIter.NextZYXinsideBlock() )
    {
      int4 iVoxel = volIter.GetVoxel();
      if ( iMaxValue < iVoxel ) iMaxValue = iVoxel;
    }
  }
  m_vCount.resize(iMaxValue+1);
  for ( volIter.SetPos(0,0,0); volIter.IsNotAtEnd(); volIter.NextBlockZYX() )
  {
    for ( ; volIter.IsNotAtEndOfBlock(); volIter.NextZYXinsideBlock() )
    {
      {
        ++m_vCount[volIter.GetVoxel()];
      }
    }
  }
} // uint2 constructor


/**
 * float4 vol Contructor.
 * Assumed to be any float range. possibly even negative and with single extreme low or high values.
 * The constructor will adjust to make min amd max a seperate bin and a linear distribution of the remainder.
 */
vxVolumeHistogram::vxVolumeHistogram(vxBlockVolume<float4> & rfVol, int4 iNumBins)
{
  vxBlockVolumeIterator<float4> volIter(rfVol);
  /// find min & max
  // consider that the data could be all zeros or just 0 and 1
  float4 fValMin(0), fValMinNext(0), fValMax(0), fValMaxPrev(0);
  fValMin = fValMinNext = fValMax = fValMaxPrev = volIter.GetVoxel();
  for ( volIter.SetPos(0,0,0); volIter.IsNotAtEnd(); volIter.NextBlockZYX() )
  {
    for ( ; volIter.IsNotAtEndOfBlock(); volIter.NextZYXinsideBlock() )
    {
      const float4 fVoxel = volIter.GetVoxel();
      if ( fValMin > fVoxel )
      {
        fValMinNext = fValMin;
        fValMin = fVoxel;
      } // find min
      if ( fValMax < fVoxel )
      {
        fValMaxPrev = fValMax;
        fValMax = fVoxel;
      } // find max
      if ( (fValMinNext > fVoxel && fVoxel > fValMin) || (fValMinNext == fValMin) ) fValMinNext = fVoxel;
      if ( (fValMaxPrev < fVoxel && fVoxel < fValMax) || (fValMaxPrev == fValMax) ) fValMaxPrev = fVoxel;
    } // for all voxels
  }
  m_vCount.resize(iNumBins);
  const int4 iMinIndex(0);
  const int4 iMaxIndex( static_cast< int4 >( m_vCount.size()-1));
  const float4 fScale( (iNumBins - 2) / (fValMaxPrev - fValMinNext) );
  for ( volIter.SetPos(0,0,0); volIter.IsNotAtEnd(); volIter.NextBlockZYX() )
  {
    for ( ; volIter.IsNotAtEndOfBlock(); volIter.NextZYXinsideBlock() )
    {
      const float4 fVoxel = volIter.GetVoxel();
      if      (fVoxel == fValMin) ++m_vCount[iMinIndex];
      else if (fVoxel == fValMax) ++m_vCount[iMaxIndex];
      else
      {
        ++m_vCount[(int4)(fScale*(fVoxel-fValMinNext))];
      }
    }
  }
} // float4 constructor



#undef FILE_REVISION


// $Log: vxVolumeHistogram.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/06/30 17:48:44  vxconfig
// changed to vxBlockVolumeIterator<int2>
//
// Revision 1.3  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.2  2003/05/16 11:51:41  michael
// code review
//
// Revision 1.1  2002/10/01 19:53:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxVolumeHistogram.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxVolumeHistogram.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $