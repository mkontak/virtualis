// $Id: Neighbor6Iterator.h,v 1.2 2007/04/09 20:26:27 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net

#ifndef Neighbor6Iterator_h
#define Neighbor6Iterator_h



/**
 * An iterator that incrementally traverses the 6 neighbors of a voxel.
 * $Id: Neighbor6Iterator.h,v 1.2 2007/04/09 20:26:27 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
template <class Iter, class Vox>
class Neighbor6Iterator
{
public:

  /// CONSTRUCTOR & ASSIGNMENT

  /// Copy Constructor
  Neighbor6Iterator(const Neighbor6Iterator<Iter,Vox> & other);

  /// Constructor called by clients
  Neighbor6Iterator(const Iter & dataIter);


  /// Constructor called by clients
  inline Neighbor6Iterator<Iter,Vox> & operator = (const Neighbor6Iterator<Iter,Vox> & other);

  /// VOLUME TRAVERSAL

  /// Increments the iterator along all 6 face neighbors of a voxel.
  inline void operator ++();
  /// is true when the traversal is finished.
  inline bool End() const;

  /// SET & GET VOXELS

  /// Returns the value at the current location: Safe version.
  inline const Vox & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const Vox & GetVoxelRaw() const;
  /// Sets the value at the current location: Safe version.
  inline void SetVoxel(const Vox & value);
  /// Sets the value at the current location: Unsafe version.
  inline void SetVoxelRaw(const Vox & value);

  /// return a reference to the underlying VolIterator
  inline const Iter & GetDataVolIter() const;

  /// ABSOLUTE LOCATION FUNCTIONS

  /// Moves the iterator to the given absolute position.
  inline void CenterAt(const Point<uint2> & pos);

  /// STATUS REPORTS

  /// Returns the current x,y,z position.
  inline const Point<uint2> & GetPos() const;
  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const;


  /// VERIFICATION

  /// Test function to verify and demonstrate the class functionality.
  /// call as Neighbor6Iterator<int,int,int>::Test(cout).
  static bool Test (std::ostream & os = std::cout);

#ifdef _gcc
  /// Test function to verify just one particular template instance combination
  /// this needs to work for any combinations of template arguments which breaks VC++, but are correct snd work in gcc
  template < class Vol >
  static bool TestByType(std::ostream & os = std::cout, Vol dummy = 0);
#endif // _gcc

protected: // data
  /// data iterator
  Iter  m_dataIter;
  /// states
  enum StateEnum
  {
    DECREMENTED, CENTERED, INCREMENTED
  }; // StateEnum

  /// change states
  struct State
  {
    StateEnum m_eChangeX;
    StateEnum m_eChangeY;
    StateEnum m_eChangeZ;
    bool      m_bEnd;
  } m_state; // struct State
}; // class Neighbor6Iterator


/// Overloaded << Operator.
template <class Iter, class Vox>
inline std::ostream & operator<<(std::ostream & os, const Neighbor6Iterator<Iter, Vox> & iter);

/// iterator typedefs
typedef  Neighbor6Iterator<BlockVolume<bool>   ::VolIterator, bool  >  BlockVolN6IterB;
typedef  Neighbor6Iterator<BlockVolume<uint1>  ::VolIterator, uint1 >  BlockVolN6IterUC;
typedef  Neighbor6Iterator<BlockVolume<uint2>  ::VolIterator, uint2 >  BlockVolN6IterUS;
typedef  Neighbor6Iterator<BlockVolume<uint4>  ::VolIterator, uint4 >  BlockVolN6IterUI;
typedef  Neighbor6Iterator<BlockVolume<int1>   ::VolIterator, int1  >  BlockVolN6IterC;
typedef  Neighbor6Iterator<BlockVolume<int2>   ::VolIterator, int2  >  BlockVolN6IterS;
typedef  Neighbor6Iterator<BlockVolume<int4>   ::VolIterator, int4  >  BlockVolN6IterI;
typedef  Neighbor6Iterator<BlockVolume<float4> ::VolIterator, float4>  BlockVolN6IterF;

typedef  Neighbor6Iterator<LinearVolume<bool>  ::VolIterator, bool  >  LinVolN6IterB;
typedef  Neighbor6Iterator<LinearVolume<uint1> ::VolIterator, uint1 >  LinVolN6IterUC;
typedef  Neighbor6Iterator<LinearVolume<uint2> ::VolIterator, uint2 >  LinVolN6IterUS;
typedef  Neighbor6Iterator<LinearVolume<uint4> ::VolIterator, uint4 >  LinVolN6IterUI;
typedef  Neighbor6Iterator<LinearVolume<int1>  ::VolIterator, int1  >  LinVolN6IterC;
typedef  Neighbor6Iterator<LinearVolume<int2>  ::VolIterator, int2  >  LinVolN6IterS;
typedef  Neighbor6Iterator<LinearVolume<int4>  ::VolIterator, int4  >  LinVolN6IterI;
typedef  Neighbor6Iterator<LinearVolume<float4>::VolIterator, float4>  LinVolN6IterF;

#include "Neighbor6Iterator.inl"

#endif



// $Log: Neighbor6Iterator.h,v $
// Revision 1.2  2007/04/09 20:26:27  jmeade
// code review.
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.3  2002/04/04 22:40:17  ingmar
// removed obsolete recursive block volume related classes
//
// Revision 3.2  2001/12/24 17:56:21  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.1  2001/10/22 01:50:04  ingmar
// fixed re-define of FILE_REVISION
//
// Revision 3.0  2001/10/14 23:02:23  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:42   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:45  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:20  soeren
// renamed volume  directory
//
// Revision 1.14  2001/05/15 13:43:09  ingmar
// code beautify
//
// Revision 1.13  2001/04/18 03:02:43  ingmar
// updated header info
//
// Revision 1.12  2001/04/10 17:40:08  dmitry
// updated datatypes
//
// Revision 1.11  2001/04/08 22:04:17  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.10  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.9  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.8  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.7  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.6  2000/11/19 00:43:36  ingmar
// added copy constructor and assignment operator as well as CenterAt
//
// Revision 1.5  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.4  2000/11/15 03:03:37  ingmar
// Added MoveAbs()
//
// Revision 1.3  2000/11/04 22:07:36  ingmar
// disabled test for Neighbor6Iterator when VC++ is used
//
// Revision 1.2  2000/11/04 21:27:46  ingmar
// debugged templetized test for Neighbor6Iterator
//
// Revision 1.1  2000/11/04 20:54:41  ingmar
// first version of Neighbor6Iterator
//
//
// $Id: Neighbor6Iterator.h,v 1.2 2007/04/09 20:26:27 jmeade Exp $
