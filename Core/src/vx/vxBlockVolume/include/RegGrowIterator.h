// $Id: RegGrowIterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
//
// Copyright(c) 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter ingmar@viatronix.net


#ifndef RegGrowIterator_h
#define RegGrowIterator_h

// includes
#include "Decider.h"
#include "NeighborIterator.h"

/**
 * An iterator that linearizes region growing.
 * $Id: RegGrowIterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a> 
*/
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider , class FlagVol = vol::BlkVolBool, class FlagIter = it::IterBlkVolBool>
class RegGrowIterator
{
public:

  /// CONSTRUCTOR & ASSIGNMENT

  /// Constructor called by clients
  RegGrowIterator(const DataIter & dataIter, Decider decider, const bool bInitFlags = true);
  /// Constructor called by clients
  RegGrowIterator(const DataIter & dataIter, Decider decider, FlagVol * pOutsideFlagVolume, const bool bInitFlags = true);
  
  /// destructor to release flagVolume
  ~RegGrowIterator();

  /// Initialize members and check consistency.
  void Initialize();

  /// VOLUME TRAVERSAL

  /// Increments the iterator position in growing direction.
  inline void operator ++();
  /// is true when the traversal is finished.
  inline bool End() const;
	/// Use the position of the dataIter as a new seed for region grow. The size of the volume or the slice 
	/// for region growing should be the same. 
	/// Avoid calling constructors of various iterators, simply clear the flag volume
	inline void SetSeed(const DataIter & dataIter);
	/// Use the position as a new seed for region grow. The volume or the slice should be same before setting.
	/// Avoid calling constructors of various iterators, simply clear the flag volume
	inline void SetSeed(const Triple<uint2>& pos);

  /// GET & SET VOXELS

  /// Returns the value at the current location: Safe version.
  inline const Vox & GetVoxel() const;
  /// Returns the value at the current location: Unsafe version.
  inline const Vox & GetVoxelRaw() const;
  /// Sets the value at the current location: Safe version.
  inline void SetVoxel(const Vox & value);
  /// Sets the value at the current location: Unsafe version.
  inline void SetVoxelRaw(const Vox & value);

  /// STATUS REPORTS

  /// Returns the current x,y,z position.
  inline const Point<uint2> & GetPos() const;
  /// Returns true if the iterator is inside the volume.
  inline bool InsideVolume() const;

  /// VERIFICATION

  /// Function to demonstrate the class functionality.
  /// call as RegGrowIterator<int,int,int,int,int>::Demo(cout).
  static void Demo (std::ostream & os = std::cout);
  /// Test function to verify and demonstrate the class functionality.
  /// call as RegGrowIterator<int,int,int,int,int>::Test(cout).
  static bool Test (std::ostream & os = std::cout);
  /// Test function to verify just one particular template instance
  static bool TestByType(std::ostream & os = std::cout);

protected: // functions

  /// what to do after moving the iterators onto a potentially next region voxel
  inline  void PossiblyAddToQueue();

	/// set all voxels in the flag volume to the value specified
	void SetFlags(bool value);
  
protected: // data

  /// data iterator
  DataIter         m_dataIter;                   
  /// decides whether to include a voxel in the region
  Decider          m_decider;                 
  /// bitvolume flagging already processed voxels
  FlagVol* m_flagVol;         
  /// indicate if the blockvolume needs to be deleted in the destructor
  bool m_bLocalFlagVol;
  /// iterator for the flag volume
  FlagIter m_flagIter;          
  /// temporary data iterator used in ++() and PossiblyAddToQueue()
  DataNeighborIter m_peekDataIter;                   
  /// temporary flag iterator used in ++() and PossiblyAddToQueue()
  FlagNeighborIter m_peekFlagIter;     
  /// queue keeping track of discovered but not processed voxels
  std::queue< Point<uint2> > m_posQ; 
  /// state of region grow (ended or not)
  bool  m_bGrowEnded;     
  /// Whether to initialize the flag volume
  const bool m_bInitFlagVol;

}; // class RegGrowIterator


/// Overloaded << Operator.
template <class DataIter, class Vox, class DataNeighborIter, class FlagNeighborIter, class Decider, class FlagVol, class FlagIter >
inline std::ostream & operator<<(std::ostream & os, const RegGrowIterator<DataIter, Vox, DataNeighborIter, FlagNeighborIter, Decider, FlagVol, FlagIter> & iter);

#include "RegGrowIterator.inl"

#endif



// $Log: RegGrowIterator.h,v $
// Revision 1.3  2007/04/10 01:23:32  jmeade
// code review.
//
// Revision 1.2  2007/03/06 18:58:23  dongqing
// code review
//
// Revision 1.1  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.5  2002/10/21 22:23:20  frank
// Unused #include "Volume.h"
//
// Revision 3.4  2002/02/20 22:45:31  michael
// removed out commented line of code
//
// Revision 3.3  2001/12/26 16:15:55  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.2  2001/12/24 17:51:15  ingmar
// added class Sec_31_volIters to structure the doxygen documentation
//
// Revision 3.1  2001/11/02 15:12:15  liwei
// Added flag to indicate whether the voxels of the  flag volume need to be initialized to false.
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:48:24   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:58   ingmar
// Initial revision.
// Revision 1.1  2001/09/10 01:16:44  ingmar
// new dir structure
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.5  2001/08/24 17:41:50  liwei
// RegGrowIteratorNew merged with the previous RegGrowIterator.
//
// Revision 1.2  2001/08/16 22:48:49  liwei
// Added SetSeed() to RegGrowIterator to accelerate series of region grow on
// volumes or slices of the same size.
//
// Revision 1.1  2001/08/16 17:39:16  liwei
// The duplicated RegGrowIterator is put at 4_image with the name RegGrowIterator.
// It handles both 3D and 2D region grow. It is expected to replace the current RegGrowIterator,
// but all the existing references should use two extra templates.
// Slice and ImageIterator modified to have compatible functions with Volume
// and VolIterator required by the RegGrowIterator.
//
// Revision 1.4  2001/08/09 22:18:44  frank
// added outside flagVolume to be handed in
//
// Revision 1.3  2001/06/04 01:12:42  ingmar
// towards coding standard compliance
//
// Revision 1.2  2001/05/12 04:23:02  ingmar
// cleaner use of template classes
//
// Revision 1.1  2001/04/28 01:25:48  ingmar
// RegionGrowIterator for any neighborhood
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/RegGrowIterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
// $Id: RegGrowIterator.h,v 1.3 2007/04/10 01:23:32 jmeade Exp $
