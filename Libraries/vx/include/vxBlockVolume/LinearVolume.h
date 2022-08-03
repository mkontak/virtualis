// $Id: LinearVolume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: summer 00
// Author: David McManamon dave@viatronix.com
//
// Complete history at bottom of file.

#ifndef LinearVolume_h
#define LinearVolume_h



// include declarations
#include "FileExtensions.h"
#include "VolAPI.h"
#include "Point.h"
#include "Vector.h"
#include "VxVolIterator.h"
#include "HeaderVolume.h"
#include "ProgressPrinter.h"
#include "Box.h"
#include "BoundingRect.h"
#include "MemoryBuffer.h"
#include <assert.h>



// define declarations
#define FILE_REVISION "$Revision: 1.3 $"



/**
 * Implements VolAPI using an array of size xDim * yDim * zDim stored in x,y,z order.
 * Clients should be wary of the total size of the volume array, available by a call
 * to GetMemoryUsage().
 * $Id: LinearVolume.h,v 1.3 2006/02/03 15:22:11 frank Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
 */
template <class Layout, class Vox> class Volume;
template <class Vol, class Vox> class VolIter;
template <class Vol, class Vox> class VolIterLin;
template <class Vol, class Vox> class RayIterBlk;
template <class Vol, class Vox> class RayIterLin;

template <class T>
class LinearVolume : public hdr::VolAPI<T>
{
    public:
      friend class VolIter< Volume<LinearVolume, T>, T>;
      friend class VolIterLin< Volume<LinearVolume, T>, T>;
      friend class RayIterBlk< Volume<LinearVolume, T>, T>;
      friend class RayIterLin< Volume<LinearVolume, T>, T>;

public:
  /// Constructor.
  LinearVolume();
  /// Constructor for building new volumes.
  LinearVolume(const Triple<uint4> & dim, const uint4 & uiBitsPerVoxel,
               const Triple<float4> & units = Triple<float4>(1.0,1.0,1.0),
               hdr::UnitTypeEnum unitType = hdr::MILLIMETER,
               const T & background = 0, hdr::ExtensionEnum ext=VOL,
               const std::string & sFileName = "Blank.vol",
               const std::string & sTitle    = "Blank",
               const std::string & sAuthor   = "Blank",
               const vx::Date & date = vx::Date());
  /// Assignment operator.
  LinearVolume const & operator=(LinearVolume<T> const & rhs);
  /// Copy Constructor.
  LinearVolume(LinearVolume<T> const & rhs);
private:
  /// copy all memory & ptrs.
  void DeepCopy(LinearVolume<T> const & rhs);
  /// destroy all volume data.
  void Destroy(void);
public:
  /// Constructor for converting layout types, e.g. LINEAR -> HASH.
  template <class L>
  LinearVolume( L & other, bool dummy)
  : hdr::VolAPI<T>(other.GetDim(), other.GetBitsPerVoxel(),
                   other.GetUnits(), other.GetUnitType(),
                   other.GetBackground(), other.GetExtension(),
                   other.GetFileName(), other.GetTitle(),
                   other.GetAuthor(), other.GetCreationDate())
  {
    // Copy VolAPI data.
    Copy(other);

    m_zStep = m_dimension.m_x * m_dimension.m_y;

    m_pData.resize(GetNumberOfVoxels());

    Point<uint2> pos;
    for ( pos.m_z = 0; pos.m_z < m_dimension.m_z; pos.m_z++ )
    {
      for ( pos.m_y = 0; pos.m_y < m_dimension.m_y; pos.m_y++ )
      {
        for ( pos.m_x = 0; pos.m_x < m_dimension.m_x; pos.m_x++ )
        {
          SetVoxelRaw(pos, other.GetVoxelRaw(pos));
        }
      }
    }
		m_eMemoryLayout = hdr::LINEAR;
  }

  /// Destructor.
  ~LinearVolume();
  
  /// Reads the volume specified into memory (using new V3D file format), ALWAYS check the return value.
  void Read(std::string fileName);

  inline void ReadSLC(std::string sFileName);
  
  /// Writes the volume to the specified file.
  void Write(std::string fileName = "NULL");
  /// Writes the volume to the specified file using the new V3D File format.
  void WriteNew(std::string fileName = "NULL");
  
  /// Try to remove ASAP.
  T * GetDataPtr()
  {
    return(&*m_pData.begin());
  }

  /// Set the value at the location specified (No Bounds checking!).
  inline void SetVoxelRaw(const Point<uint2> & location, T value);
  /// Gets the voxel at the specified location (No Bounds checking!).
  inline const T  GetVoxelRaw(const Point<uint2> & location) const;

  /// Computes and returns the resampled value at the location specified.
  float4 GetSample(const Point<float4> & point) const;

  /// Returns the total memory usage in bytes.
  const uint4 GetMemoryUsage() const;

  /// Reports relevant volume layout statistics
  void Stats(std::ostream & os = std::cout) const;

  ///Merge to volumes. Meaning, after merging the equal blocks are shared
  void Merge(LinearVolume<T> const & otherVolume);

    /// Resize the volume, no resampling.
  void Resize(const Triple<uint4> & dimension);

  /// Transforms the Reference frame (transverse to coronal/saggittal).
  void PermuteCoordinates(hdr::PermuteCoordinatesEnum view);

  /// Test function to verify and demonstrate the class functionality.
  static bool Test      (std::ostream & os = std::cout);
  static bool TestByType(std::ostream & os = std::cout);


#include "LinearVolIterator.h"
  
  VolIterator GetVolIterator(const Point<uint2> & startPoint = Point<uint2>(0,0,0))
  {
    if ( !InsideVolume(startPoint) )
    {
      LogWrn("<GetVolIterator> Attempted to build VolIterator outside of volume","LinearVolume","GetVolIterator");
      const Point<uint2> pos(0,0,0);
      return(VolIterator(pos, m_dimension, m_background, m_pData.begin()));
    }
    return(VolIterator(startPoint, m_dimension, m_background, m_pData.begin()));
  }
  
#include "LinearVolRayIterator.h"
  
  RayIterator GetRayIterator(const Point<float4> startPoint, 
                                            const Vector<float> & directionVector, 
                                            const float4 fSampleDistance)
  {
    return(RayIterator(startPoint, m_dimension, m_background, m_pData.begin(),
                                directionVector, fSampleDistance));
  }
	
#include "LinearSubVolIterator.h"
  SubVolIterator GetSubVolIterator(const uint2 uBoundary = 0)
  {
    return(SubVolIterator(m_dimension, m_background, m_pData.begin(), uBoundary));
  }

	SubVolIterator GetSubVolIterator(const Box<uint4>& boundingBox)
	{
    return(SubVolIterator(m_dimension, m_background, m_pData.begin(), boundingBox));
	}
	
#include "LinearVolSliceIterator.h"
  XSliceIterator GetXSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {

    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_x);
    return(XSliceIterator(uIdxSlice, m_dimension, m_background, m_pData.begin(), uBoundary));

  }

  XSliceIterator GetXSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_x); 
    return(XSliceIterator(uIdxSlice, m_dimension, m_background, m_pData.begin(), boundingBox));
  }
	
  YSliceIterator GetYSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_y );
    return(YSliceIterator(uIdxSlice, m_dimension, m_background, m_pData.begin(), uBoundary));

  }

  YSliceIterator GetYSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_y );

    return(YSliceIterator(uIdxSlice, m_dimension, m_background, m_pData.begin(), boundingBox));
  }
	
  ZSliceIterator GetZSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_z );
    return(ZSliceIterator(uIdxSlice, m_dimension, m_background, m_pData.begin(), uBoundary));
  }
	
  ZSliceIterator GetZSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    assert(uIdxSlice >= 0 && uIdxSlice < GetDim().m_z );
    return(ZSliceIterator(uIdxSlice, m_dimension, m_background, m_pData.begin(), boundingBox));
  }
	

/*
  typedef VxVolIterator<LinearVolume<T>,T> VolIterator;

  VolIterator GetVolIterator(const Point<uint2> & startPoint = Point<uint2>(0,0,0))
  {
    if ( !InsideVolume(startPoint) ) {
      Error::Warn("Attempted to build VolIterator outside of volume");
      const Point<uint2> pos(0,0,0);
      return(VolIterator(*this, pos));
    }
    return(VolIterator(*this, startPoint));
  }*/


protected: // data
  std::vector<T> m_pData;
  uint4 m_zStep;

protected: //functions
  /// returns the offset for the location.
  inline uint4 GetOffset(const Point<uint2> & p) const;
  /// Returns size of the volume array
  const uint4 GetDataSize() const;
};

#undef FILE_REVISION

#include "LinearVolume.inl"

#endif // LinearVolume_h

// $Log: LinearVolume.h,v $
// Revision 1.3  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.2  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.7.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.7  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.6  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 3.5  2002/03/15 19:38:40  ingmar
// made RayIterBlk/Lin friend classes
//
// Revision 3.4  2002/03/13 17:05:39  ingmar
// added friend declarations for new iterators
//
// Revision 3.3  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 3.2  2001/12/24 16:46:19  ingmar
// added class Sec_30_volume to structure the doxygen documentation
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:16   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:47:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:04   ingmar
// Initial revision.
// Revision 1.5  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:04:58  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:08  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.50  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.49  2001/08/30 15:32:30  soeren
// added new resize more no resample
//
// Revision 1.48  2001/08/19 22:32:57  ingmar
// renamed GetResampledValue() to GetSample()
//
// Revision 1.47  2001/08/09 21:17:26  liwei
// Constructor of BoundingRect and Box changed.
//
// Revision 1.46  2001/08/08 17:32:57  liwei
// Bounding boxes are all of the type Box<uint4>.
// Bugs in LinearVolumeIterator::NextZYX() fixed.
//
// Revision 1.45  2001/08/07 17:03:39  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.44  2001/08/06 23:24:26  liwei
// Passing boundingBox rather minCorner and maxCorner when creating SubVolume Iterator
// and slice iterators.
//
// Revision 1.43  2001/08/02 22:23:14  liwei
// Bugs in LinearSubVolumeIterator::NextZYX() fixed.
//
// Revision 1.42  2001/07/31 18:46:29  liwei
// Bugs in the revised version of SliceIterator fixed.
//
// Revision 1.41  2001/07/30 23:29:30  liwei
// Renamed VolIteratorSkipBoundary to SubVolIterator.
// Make slice iterators be subclasses of SubVolIterator.
//
// Revision 1.40  2001/07/30 17:39:07  huamin
// Added RayIterator
//
// Revision 1.39  2001/07/19 22:50:44  liwei
// Added VolIteratorSkipBoundary for linear and block volume.
// Bugs of the PeekX() functions of the block volume fixed.
//
// Revision 1.38  2001/07/18 20:53:17  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.37  2001/07/09 16:51:10  frank
// Added support for the new V3D file format.
//
// Revision 1.36  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.35  2001/06/18 23:40:12  liwei
// Slice iterator added
//
// Revision 1.34  2001/06/14 23:53:41  liwei
// Bookkeeping the information of memory layout and datatype
//
// Revision 1.33  2001/06/01 17:29:32  ana
// PrintProgress function replaced by ProgressPrinter class
//
// Revision 1.32  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.31  2001/04/12 21:30:32  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.30  2001/04/12 18:54:15  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.29  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.28  2001/04/10 16:13:26  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.27  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.26  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.25  2001/04/02 16:08:38  soeren
// added support for reading
// slc files
//
// Revision 1.24  2001/03/31 05:34:38  ingmar
// uint8 -> uint4
//
// Revision 1.23  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.22  2001/03/21 22:30:23  soeren
// added merge function
//
// Revision 1.21  2001/02/06 02:25:11  dave
// re-organized linVol Read for memory savings
//
// Revision 1.20  2000/12/19 16:59:51  sarang
// BUG_FIX : removed an inline which shouldn't have been there - thanks gcc
//
// Revision 1.19  2000/12/14 14:53:28  frank
// Removed a warning
//
// Revision 1.18  2000/12/12 18:34:13  dave
// corrected VolAPI constructor
//
// Revision 1.17  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.16  2000/11/17 20:28:30  dave
// added native VolIterator
//
// Revision 1.15  2000/11/03 18:20:15  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.14  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.13  2000/11/02 19:48:09  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.12  2000/11/01 21:11:29  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.11  2000/10/31 21:16:42  dave
// changed VOLAPI to use ZLIB
//
// Revision 1.10  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.9  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.8  2000/10/24 15:46:43  dave
// no message
//
// Revision 1.7  2000/10/24 04:10:06  dave
// improved GetVoxel
//
// Revision 1.6  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.5  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.4  2000/10/22 00:24:23  dave
// better iterators
//
// Revision 1.3  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.2  2000/09/11 22:46:09  dave
// added date field to constructors
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
//
// 23    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 22    9/04/00 1:39a Dave
// checking in for cvs change
//
// 21    9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 20    8/21/00 1:09a Sarang
// modified header
//
// 19    8/20/00 6:49p Sarang
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// 18    8/19/00 7:30p Jay
// SetVoxelRaw changes
//
// 17    8/19/00 6:19p Ingmar
// SetVoxelRaw
//
// 16    8/19/00 3:16p Ingmar
// GetVoxel/GetVoxelRaw + inline Error::DebugMsg
//
// 15    8/18/00 2:24p Lihong
// change m_data to m_pData
//
// 14    8/17/00 5:16a Jay
// CopySpecificHeader added in VolAPI.
//
// 13    8/16/00 2:39p Antonio
// added header and footer
//
// *****************  Version 12  *****************
// User: Jay             Date:  8/15/00  Time: 12:28a
// Checked in $/v2k/src/Volume
//
// *****************  Version 11  *****************
// User: Jay             Date:  8/10/00  Time: 12:16a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Linear Volume iterator is a vector::iterator
//
// *****************  Version 10  *****************
// User: Jay             Date:  8/09/00  Time: 11:54p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Template Volumes working.
//
// *****************  Version 9  *****************
// User: Dave            Date:  8/09/00  Time: 10:51p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   m_background added
//
// *****************  Version 8  *****************
// User: Jay             Date:  8/09/00  Time:  1:33a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Generic Iterator class added, Resize works fine, TransformCoords to
// be fixed
//
// *****************  Version 7  *****************
// User: Dave            Date:  8/08/00  Time: 10:07p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   improved Test() function
//
// *****************  Version 6  *****************
// User: Jay             Date:  8/08/00  Time:  9:37p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Native Iterator is now a synonymn of vector<T>::iterator
//
// *****************  Version 5  *****************
// User: Dave            Date:  8/08/00  Time:  7:44p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Lots of changes
//
// *****************  Version 4  *****************
// User: Jay             Date:  8/08/00  Time:  3:03p
// Checked in $/TemplateVol/src/Volume
//
// *****************  Version 3  *****************
// User: Dave            Date:  8/08/00  Time: 12:01p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   mmm
//
// *****************  Version 2  *****************
// User: Dave            Date:  8/08/00  Time: 11:36a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   cleaned syntax errors
//
// *****************  Version 1  *****************
// User: Jay             Date:  8/08/00  Time: 10:55a
// Created LinearVolume.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
// $Id: LinearVolume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
