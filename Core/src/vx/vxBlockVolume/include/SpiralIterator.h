// $Id: SpiralIterator.h,v 1.3 2007/04/10 01:30:16 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:  summer 00
// Author: Jay B. jay@viatronix.net


#ifndef SpiralIterator_h
#define SpiralIterator_h

// includes
#include "Triple.h"
#include "Point.h"

/**
 * SpiralIterator encapsulates a 3D region grow from a seed point.
 * The ++ operator increments the current location in the direction of this growth.
 * $Id: SpiralIterator.h,v 1.3 2007/04/10 01:30:16 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a> 
*/
template <class I, class T>
class SpiralIterator
{
// member functions
public:

  /// Constructor, can only be made from within the Volume class.
  SpiralIterator(const I & iterator);

  /// Increments the location of the iterator in the 3D order.
  inline void operator++();
  /// is true when the traversal is finished.
  inline bool End() const;

  /// Returns the value at the current location: Safe version.
  inline const T & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const T & GetVoxelRaw() const;
  /// Sets the value at the current location: Safe version.
  inline void SetVoxel(const T & value);
  /// Sets the value at the current location: Unsafe version.
  inline void SetVoxelRaw(const T & value);
  /// returns the current location.
  inline const Point<uint2> & GetPos() const;

private:
  /// Increments the location of the iterator in the 3D order.
  void Increment();

// data
private:
  /// dilator state
  enum DilatorStateEnum 
  {
    START, BASE, MID, TOP
  };
  /// seed point
  Point<uint2> m_seed;
  /// delta value
  int2 m_iDelta;
  /// state value
  DilatorStateEnum m_eState;
  /// the iterator
  I m_iter;
}; // class SpiralIterator

/// iterator typedefs
typedef  SpiralIterator<BlockVolume<bool>   ::VolIterator, bool  >  BlockVolSpiralIterB;
typedef  SpiralIterator<BlockVolume<uint1>  ::VolIterator, uint1 >  BlockVolSpiralIterUC;
typedef  SpiralIterator<BlockVolume<uint2> ::VolIterator, uint2>  BlockVolSpiralIterUS;
typedef  SpiralIterator<BlockVolume<uint4>   ::VolIterator, uint4  >  BlockVolSpiralIterUI;
typedef  SpiralIterator<BlockVolume<int1>   ::VolIterator, int1  >  BlockVolSpiralIterC;
typedef  SpiralIterator<BlockVolume<int2>  ::VolIterator, int2 >  BlockVolSpiralIterS;
typedef  SpiralIterator<BlockVolume<int4>    ::VolIterator, int4   >  BlockVolSpiralIterI;
typedef  SpiralIterator<BlockVolume<float4>  ::VolIterator, float4 >  BlockVolSpiralIterF;

typedef  SpiralIterator<LinearVolume<bool>  ::VolIterator, bool  >  LinVolSpiralIterB;
typedef  SpiralIterator<LinearVolume<uint1> ::VolIterator, uint1 >  LinVolSpiralIterUC;
typedef  SpiralIterator<LinearVolume<uint2>::VolIterator, uint2>  LinVolSpiralIterUS;
typedef  SpiralIterator<LinearVolume<uint4>  ::VolIterator, uint4  >  LinVolSpiralIterUI;
typedef  SpiralIterator<LinearVolume<int1>  ::VolIterator, int1  >  LinVolSpiralIterC;
typedef  SpiralIterator<LinearVolume<int2> ::VolIterator, int2 >  LinVolSpiralIterS;
typedef  SpiralIterator<LinearVolume<int4>   ::VolIterator, int4   >  LinVolSpiralIterI;
typedef  SpiralIterator<LinearVolume<float4> ::VolIterator, float4 >  LinVolSpiralIterF;


#include "SpiralIterator.inl"

#endif // SpiralIterator_h


// $Log: SpiralIterator.h,v $
// Revision 1.3  2007/04/10 01:30:16  jmeade
// code review.
//
// Revision 1.2  2007/03/06 18:58:23  dongqing
// code review
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 3.3  2002/04/09 18:59:03  ana
// Some deleting in order to compile :)
//
// Revision 3.2  2001/12/26 16:15:55  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.1  2001/12/24 17:50:57  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:00   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:43  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.9  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.8  2001/04/08 21:38:22  dave
// updated doc headers
//
// Revision 1.7  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.6  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.5  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.4  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.3  2000/11/07 04:46:52  dave
// added typedefs
//
// Revision 1.2  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.1  2000/10/23 20:18:08  ingmar
// removed VxDilator and added SpiralIterator
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 6     9/08/00 11:29a Ingmar
// code beautify
//
// 5     9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 4     9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 3     8/21/00 2:07a Sarang
// added header and footer
//
//*****************  Version 2  *****************
//User: Jay             Date:  8/09/00  Time: 11:54p
//Checked in $/TemplateVol/src/Volume
//Comment:
//  Template Volumes working.
//
//*****************  Version 1  *****************
//User: Jay             Date:  8/09/00  Time:  7:54p
//Created SpiralIterator.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/SpiralIterator.h,v 1.3 2007/04/10 01:30:16 jmeade Exp $
// $Id: SpiralIterator.h,v 1.3 2007/04/10 01:30:16 jmeade Exp $
