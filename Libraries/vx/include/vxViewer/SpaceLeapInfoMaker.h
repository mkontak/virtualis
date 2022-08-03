// $Id: SpaceLeapInfoMaker.h,v 1.3 2006/03/29 20:03:19 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

#ifndef SpaceLeapInfoMaker_h
#define SpaceLeapInfoMaker_h


// include declarations
#include "vxBlockVolume.h"
#include "Box.h"


// class definition
template <class VoxelType>
class VX_VIEWER_DLL SpaceLeapInfoMaker
{
// member functions
public:

  /// default contructor
  SpaceLeapInfoMaker() {};

  /// compute single bit for being within radius
  void ComputeSingleBitForWithinRadius(vxBlockVolume<VoxelType> & ctvVol, 
                                       const VoxelType rangeMin, 
                                       const VoxelType rangeMax,
                                       const Vector3D<float4> & fvBBoxMin,  
                                       const Vector3D<float4> & fvBBoxMax,
                                       const int4 iRadius = 8, 
                                       const int4 iMask = 0x1000);

  /// compute single bit for being within radius
  void ComputeSingleBitForWithinRadius(vxBlockVolume<VoxelType> & ctvInputVol, 
                                       vxBlockVolume<VoxelType> & ctvOutPutVol, 
                                       const VoxelType rangeMin, 
                                       const VoxelType rangeMax,
                                       const Vector3D<float4> & fvBBoxMin,  
                                       const Vector3D<float4> & fvBBoxMax,
                                       const int4 iRadius = 8, 
                                       const int4 iMask = 0x1000);

  /// compute four bit for being within radius
  void ComputeFourBitsSpaceLeapRadius(vxBlockVolume<VoxelType> & ctvInputVol, 
                                      vxBlockVolume<VoxelType> & ctvOutPutVol, 
                                      const VoxelType rangeMin, 
                                      const VoxelType rangeMax, 
                                      const Vector3D<float4> & fvBBoxMin, 
                                      const Vector3D<float4> & fvBBoxMax, 
                                      const int4 iRadiusMax = 15);

  /// compute space leap radius
  void ComputeInt1SpaceLeapRadius(vxBlockVolume<uint2> & volIntensity, 
                                  vxBlockVolume<uint2> & volLabel, 
                                  vxBlockVolume<int1> & sldVol, 
                                  const int4 iMaskIntensity, 
                                  const int4 iMaskLabel, 
                                  const int4 iRadiusMax = 15);

  /// compute a summed count on a block base
  void ComputeSummedCountVolumeBlockWise( vxBlockVolume<uint2> & inputVol,
                                          vxBlockVolume<int4> & histogramIntensityVol,
                                          vxBlockVolume<int4> & histogramLabelVol,
                                          const Vector3D<float4> & fvBBoxMin, 
                                          const Vector3D<float4> & fvBBoxMax, 
                                          const int4 iMaskIntensity, 
                                          const int4 iMaskLabel );

private:

  /// copy constructor
  SpaceLeapInfoMaker(const SpaceLeapInfoMaker & other);             // should only be public if really implemented!

  /// assignment operator
  const SpaceLeapInfoMaker & operator=(SpaceLeapInfoMaker & other); // should only be public if really implemented!

  /// compute summed count volume
  void ComputeSummedCountVolume(vxBlockVolume<VoxelType> & ctvInputVol, 
                                const VoxelType rangeMin, 
                                const VoxelType rangeMax, 
                                const Vector3D<float4> & fvBBoxMin, 
                                const Vector3D<float4> & fvBBoxMax,
                                int4 iRadiusMax);

  /// compute small summed count volume
  void ComputeSmallSummedCountVolume(vxBlockVolume<uint2> & volIntensity, 
                                     vxBlockVolume<uint2> & volLabel,
                                     const int4 iMaskIntensity, 
                                     const int4 iMaskLabel, 
                                     const int4 iRadiusMax);

// data members
public:

  /// summed count block volume
  vxBlockVolume<int4> m_summedCountBlkVol;

}; // class SpaceLeapInfoMaker


#endif // SpaceLeapInfoMaker_h


// $Log: SpaceLeapInfoMaker.h,v $
// Revision 1.3  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.2  2004/10/20 21:50:23  michael
// added VX_VIEWER_DLL to export classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/06/18 15:19:51  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.15  2003/04/29 14:33:43  frank
// code review
//
// Revision 1.14  2003/04/15 21:39:38  michael
// performed more code review
//
// Revision 1.13  2003/04/14 21:06:02  michael
// performed initial code review
//
// Revision 1.12  2003/02/25 19:02:41  frank
// Removed empty inl file
//
// Revision 1.11  2002/12/11 15:53:12  ingmar
// removed unused box from function interface
//
// Revision 1.10  2002/08/21 22:05:33  ingmar
// now differentiating between visibleVoxelCropBox and enclosing multipleOf16BlockCropBox
//
// Revision 1.9  2002/07/08 20:35:37  ingmar
// seperated the histogram information into two volumes, one for intensity and one for label, each now with 32 bits
//
// Revision 1.8  2002/07/01 23:23:55  soeren
// added more timings
//
// Revision 1.7  2002/06/28 22:58:56  soeren
// added new summed count computation .....
//
// Revision 1.6  2002/05/30 17:02:05  soeren
// added space leaping though turned off components
//
// Revision 1.5  2002/05/28 21:53:58  soeren
// added new spaceleap method
//
// Revision 1.4  2002/04/08 22:34:54  ingmar
// moved duplicated code into seperate function ComputeSummedCountVolume()
//
// Revision 1.3  2002/04/08 21:48:06  ingmar
// new spaceleap computation
//
// Revision 1.2  2002/04/05 15:15:12  ingmar
// activated new space leap function (still broken)
//
// Revision 1.1  2002/04/04 19:09:45  ingmar
// inital version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/SpaceLeapInfoMaker.h,v 1.3 2006/03/29 20:03:19 geconomos Exp $
// $Id: SpaceLeapInfoMaker.h,v 1.3 2006/03/29 20:03:19 geconomos Exp $