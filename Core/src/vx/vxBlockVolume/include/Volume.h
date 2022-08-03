// $Id: Volume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: Summer 00
// Author: David & Jayanta dave@viatronix.com
//
// Complete History at bottom of file.


#ifndef Volume_h
#define Volume_h

// includes
#include "FileExtensions.h"
#include "Histogram.h"
#include "Triple.h"
#include "Point.h"
#include "Vector.h"
#include "VolAPI.h"
#include "LinearVolume.h"
#include "BlockVolume.h"
#include "HeaderVolume.h"
#include "Date.h"
#include "ProgressPrinter.h"

/**
 *
PLEASE READ THE DESCRIPTION OF THE VOLUME CLASS

   DESIGN

   The design of the volume class had to meet several specifications.
   Specifically the class needed to be able to:<UL>
<LI>1. Work with different data types and bit depths (int, float, byte, etc.).
<LI>2. Be able to handle different in memory layouts (linear array, hash, RLE,
 etc.).
<LI>3. Support header information unique to extension type.  For example, a
user may need to know the number of connected components in files with
extension ".ins" so this field must be stored; however,this has no meaning
and should not appear in ".vol" files.
<LI>4. Support a variety of compression modes, NONE, ZLIB, etc.
<LI>5. Perform all operations in an optimum or near optimum manner, i.e. no
late binding of function calls.
</UL>

  BRIEF USER MANUAL

Due to point 5 above the volume classes make extensive use of templates.
However, due to point 2 above writing a volume class templatized on only one
parameter, i.e. Volume<T> makes writing generic functions nearly impossible.
More specifically if there exists a HashVolume<T> and LinearVolume<T> and
one wishes to write a generic function "GetResampledValue()", that function
must be written inside each class so that the call to "GetVoxel()" finds the
correct function.  In order to avoid this unnecessary code replication and
provide a common interface for all volume layouts the Volume class requires
two template parameters, Volume<L, T>, and one could declare a linear volume
of shorts by writing:<BR>

"Volume<LinearVolume<short>, short> myShorts;"<BR>

Note the inheritance diagrams drawn by the auto-documentation.  Each specific
volume memory layout "is-a" valid "VolAPI" instance. The Volume<L, T>
"has-a" valid VolAPI instance L, therefore the generic function mentioned
early, "GetResampledValue()" resides in Volume<L, T>. Code replication is
eliminated, type and layout polymorphism is achieved and the class clients
have one identical interface for all layouts and types.<BR>

View the Test() function in each volume implementation to see examples.
Users can create a new volume by calling the default constructor followed by
a call to Read() or users can create a volume themselves by calling the
second constructor that takes up to nine parameters.  By the latter method
the volume is initialized to all zero values and the user is expected to set
the voxels as they wish.<BR>

If the volume is initialized by a call to Read() the return value should
ALWAYS be checked to see if the read was successful. During the read, any
extra header variables unique to this extension are also read.  These
variables can be retrieved and set with the functions "GetHdrVar()" and
"SetHdrVar" respectively.  Clients that want to retrieve a header variable
must know the variable name that it is stored by. The name is available
by looking at the code where the variable is set or checking the name within
the text of the volume file.  Clients that just look at the volume file to
retrieve a variable by name must be sure they also know the type of the variable.
For example a value written as an int should not be retrieved as a short.  Any
variable types, including user defined types can by set in the header but the
input and output operators must be overloaded for the type conversions to
work properly and the string must not contain the "#" symbol since it is
used for parsing the input file.<BR>

Normally when one declares a "Volume<LinearVolume<short>, short>" then the
file read from disk will also be in the "LinearVolume<T>" layout however
conversion between layouts is automatically performed if necessary.  Note
that declaring a LinearVolume and reading a file stored in HashVolume layout
wastes both computation time and space since it requires both volumes to
exist in memory for a short time and for all voxels to be traversed on more
time than needed.  Although a LinearVolume<T> can read a volume stored in
any layout it can only write to disk in it's own linear format.<BR>

Class users should find the bi-directional iterators, available by a call to
Begin(), Location(), or End() very convenient.  The bi-directional iterators
are simply an abstraction of pointers with one important exception.  An
iterator can perform all the same operations a normal pointer could BUT a
normal pointer CANNOT perform all the operations an iterator can perform.
All bi-directional iterators support increment, decrement, dereferencing and
assignment.  Since layout LinearVolume<T> contains a linear block of memory
for storing voxels, the iterators for this class can also support random
access as in the following code example:<BR>

Volume<LinearVolume<short>, short> vol;  <BR>
if(vol.Read("Volume.vol" < )) <BR>
  exit(1);<BR>
LinearVolume<short>::Iterator iterB = vol.Begin();<BR>
*iterB = 20;          // assignment<BR>
iterB[5] = 10;        // assigning location 5<BR>
iterB = iterB + 7;  // move iterator location 7<BR>

Question: Yeah okay that looks nice, but I don't care about following OO
principles, where's my "GetDataPointer()" function?<BR>

Actually, the statement "LinearVolume<short>::Iterator iterB = vol.Begin()"
and "short * iterB = vol.Begin()" happen to be identical for the
LinearVolume<T> layout.  Of course the HashVolume<T> layout does not support
random access and a statement like "iterB = iterB + 10" would have no meaning.
<BR>
Iterators offer several useful features and advantages over a normal pointer,
as the following code fragments show.<BR>

* // code fragment 1 <BR>
HashVolume<uchar>::Iterator iterB = vol.Begin(); <BR>
HashVolume<uchar>::Iterator iterE = vol.End(); <BR>
for(; iterB != iterE; ++iterB) { <BR>
  *iterB;  // access value and do something <BR>
} <BR>

The Volume class iterators are also compatible with all Standard Template
Library generic algorithms.

* // code fragment 2 <BR>
#include \<algorithm\> <BR>
uchar max = max_element(iterB, iterE); <BR>

Users should be aware of several disadvantages.  <UL>
<LI> 1. The order voxels are traversed when using iterators is not defined.
Code fragment 1 above is guaranteed to traverse the entire volume although
the order voxels are access may differ if a LinearVolume iterator was
instanciated.<BR>

<LI> 2. Code written using the random access iterator of LinearVolume is not
portable to other layouts.  Perhaps a new layout type will be implemented
that requires less storage space and voxel access 5x faster than LinearVolume.
The client would be required to re-write the code so that it works with the
new layout.
 </UL>

With regard to point two above, two solutions exist.  Do not use the random
access properties of the LinearVolume iterator, instead access voxels using
GetVoxel(), SetVoxel().  This approach carries a slight overhead in some
cases for the offset calculation when LinearVolume layout is used.  The
better solution is to use the 3D iterator VolIterator, which provides
functions for moving through a 3D space.<BR>

SetVoxel() and GetVoxel() have been updated from the original volume class
implementation to always perform boundary checking.  Attempted access outside
the volume results in a Error::Warn message and m_background is returned.  By
default the m_background variable is zero, users can change the value by a
call to SetBackground.  Unchecked voxel access is possible with the functions
SetVoxelRaw and GetVoxelRaw see Stroustrup page 446 for a description of when
 to use checked or unchecked access.<BR>

 * All recommendations and comments about the design and/or implementation of
 * the classes are welcome.<BR>
 * $Id: Volume.h,v 1.3 2006/02/03 15:22:11 frank Exp $ <a href="../../vxBase/8_volume/Spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
namespace hdr {
	enum DATATYPE { BL=0, UINT1, UINT2, UINT4, INT1, INT2, INT4, FLT4, SEG, SPECIFIC = -1};
}

template <class V, class T> class VolIter;
template <class V, class T> class VolIterLin;
template <class V, class T> class VolIterBlk;
template <class V, class T> class RayIterBlk;
template <class V, class T> class RayIterLin;


#define FILE_REVISION "$Revision: 1.3 $"


template <class L, class T>
class Volume 
{
public:
  friend class VolIter< Volume<L, T>, T>;
  friend class VolIterLin< Volume<L, T>, T>;
  friend class VolIterBlk< Volume<L, T>, T>;
  friend class RayIterBlk< Volume<L, T>, T>;
  friend class RayIterLin< Volume<L, T>, T>;
public:
  /// Default constructor. Optionally specify a file name to be read from disk.
  Volume();
  /// Constructor for building new volumes.
  Volume(const Triple<uint4> & dim, const uint4 & uiBitsPerVoxel,
         const Triple<float4> & units = Triple<float4>(1.0,1.0,1.0),
         hdr::UnitTypeEnum unitType = hdr::MILLIMETER,
         const T & background = 0,
         hdr::ExtensionEnum ext = hdr::VOL,
         const std::string & sFileName = "Blank.vol",
         const std::string & sTitle    = "Blank",
         const std::string & sAuthor   = "Blank",
         const vx::Date & date = vx::Date());
  // Constructor for building new Block volumes.
  Volume(const Triple<uint4> & dim, const uint4 & bitsPerVoxel,
         const Triple<float4> & units, hdr::UnitTypeEnum unitType,
         const T & background, hdr::ExtensionEnum ext,
         const std::string & sFileName, const std::string & sTitle,
         const std::string & sAuthor, const vx::Date & date,
         const uint4 & uiBlockBitSize);

#ifdef _WIN32
  /// Needed to access m_data of other Volumes in the Conversion Constructor.
  friend class Volume<LinearVolume<T>,T>;
  friend class Volume<BlockVolume <T>,T>;
#endif
  /// Constructor for converting layout types, e.g. LINEAR -> HASH, requires
  /// dummy parameter due to VC++ shortcomings.
  template <class Layout>
  Volume( Layout & other, bool dummy)
  : m_data(other.m_data, dummy)
  {
		SetDataType();
		m_uRefCount = 0;
  }
  /// Destructor.
  ~Volume();
  /// Copy Constructor.
  Volume(const Volume<L,T> & rhs);
  /// Assignment operator.
  Volume<L,T> const & operator=(const Volume<L,T> & rhs);
  /// Comparison operator
  template <class Vol> 
    bool operator==(const Vol & other) const
  {
    T dummyType = 0;
    Point<uint2> zero(0,0,0);
    std::string sNameA(typeid(dummyType).name());
    std::string sNameB(typeid(other.GetVoxel(zero)).name());
    if ( sNameA != sNameB ) return (false);
    
    if ( m_data.GetDim() != other.GetDim() ) return (false);
    
    // don't know how to name VolIterator to get it here, just using for() :(
    Point<uint2> pos;
    Triple<uint4> uiDim = m_data.GetDim();
    // could sample a few points randomly to try and improve != efficiency
    for ( pos.m_z = 0; pos.m_z < uiDim.m_z; ++pos.m_z )
    {
      for ( pos.m_y = 0; pos.m_y < uiDim.m_y; ++pos.m_y )
      {
        for ( pos.m_x = 0; pos.m_x < uiDim.m_x; ++pos.m_x )
        {
          if ( m_data.GetVoxelRaw(pos) != other.GetVoxelRaw(pos) )
            return (false);
        } // x
      } // y
    } // z

    return (true);
  }

  /// Not equal to operator
  template <class Vol> bool operator !=(const Vol & other) const
  {
    return (!operator==(other));
  }
  

  ///Merge to volumes. Meaning, after merging the equal blocks are shared
  inline void Merge(const Volume<L,T> & otherVolume); 
  /// Reads the volume specified into memory (using V3D file format).
  inline void Read(std::string fileName);
  /// Writes the volume to the specified file.
  inline void Write(std::string fileName = "NULL", bool buildIcon = true) ;
  /// Writes the volume to the specified file using the new V3D File format.
  inline void WriteNew(std::string fileName = "NULL", bool buildIcon = true);
  
  /// :( Buyer beware.
  inline T * GetDataPtr()
  {
    return (m_data.GetDataPtr());
  }

  /// Sets the value at the position specified.
  inline void SetVoxel(const Point<uint2> & pos, T value);
  /// Returns the value of the voxel at the position specified by point.
  inline void SetVoxelRaw(const Point<uint2> & pos, T value);
  /// Returns the value of the voxel at the position specified by point.
  inline const T  GetVoxel(const Point<uint2> & pos) const;
  /// Returns the value of the voxel at the position specified by point, NO bounds checking.
  inline const T  GetVoxelRaw(const Point<uint2> & pos) const;
  /// Returns the value of the sample at the floating point position specified by point.
  inline float4 GetSample(const Point<float4> & pos) const;  
  /// Sets the value returned when voxel access is attempted outside the volume.
  // Difficult to implement efficiently, contact Dave or Ingmar if needed.
  inline void SetBackground(const T & value);
  /// Gets the value returned when voxel access is attempted outside the volume.
  inline const T & GetBackground() const;

  /// Forces the given compression type for file storage.
  inline void SetCompression(hdr::FileCompressionEnum compressType);
  /// Returns the current compression, normally m_bestCompression.
  inline hdr::FileCompressionEnum GetCompression() const;

  /// Returns the extension of the volume as read from the file header.
  inline hdr::ExtensionEnum GetExtension() const;

  /// Sets the author/source string in the volume header.
  inline void SetAuthor(const std::string & sAuthor);
  /// Returns the author/source string in the volume header.
  inline const std::string & GetAuthor() const;

  /// Sets the volume title.
  inline void SetTitle(const std::string & sTitle);
  /// Returns the title of the volume.
  inline const std::string & GetTitle() const;

  /// Sets the date on which the file is created.
  inline void SetCreationDate(vx::Date d);
  /// Returns the date on which the file was created.
  inline const vx::Date & GetCreationDate() const;

  /// Returns the unitType of the stored volume such as METER, MICRON, etc.
  inline hdr::UnitTypeEnum GetUnitType() const;

  /// Returns the x,y,z voxel dimensions of the volume.
  inline const Triple<uint4> & GetDim() const;
  /// Returns the x,y,z units of the volume.
  inline const Triple<float4> & GetUnits() const;
  /// Returns the x,y,z values in world coordinates.
  inline const Triple<float4> GetSize() const
	{	
		Triple<float4> szVol;
		szVol.m_x = GetUnits().m_x * (GetDim().m_x -1);
		szVol.m_y = GetUnits().m_y * (GetDim().m_y -1);
		szVol.m_z = GetUnits().m_z * (GetDim().m_z -1);
		return szVol;
	}

  /// Set the x,y,z units of the volume.
  inline void SetUnits(const Triple<float4> & units);

  /// Returns the number of significant bits per voxel.
  inline const uint4 GetBitsPerVoxel() const;
  /// Returns the bytes per voxel.
  inline const uint4 GetBytesPerVoxel() const;
  /// Returns the total memory usage in bytes.
  inline const uint8 GetMemoryUsage() const;
  /// Reports relevant volume layout statistics
  void Stats(std::ostream & os = std::cout) const;

  /// Returns the file name string
  inline const std::string & GetFileName() const;
  /// Returns the file name string without the extension
  inline std::string GetFileNameRoot() const;
  /// Sets the file name
  inline void SetFileName(const std::string & sFileName);

  /// Returns a matrix for converting from volume space into world space.
  inline const Matrix<float4> GetVolumeToWorldMatrix() const;
  /// Returns a matrix for converting from world space into volume space.
  inline const Matrix<float4> GetWorldToVolumeMatrix() const;

  /// Returns true if the point lies within the volume.
  inline bool InsideVolume(const Point<uint2> & p) const;

  /// Prints all file header variables to the ostream.
  inline std::ostream & AsOstream (std::ostream & os = std::cout) const;

  /// Reports the number of voxels
  inline const uint4 GetNumberOfVoxels() const;
  /// Returns the number of non-zero voxels
  inline const uint4 GetNumberOfNonZeroVoxels(); // should be const, need const-Iterator
  /// Count how many times a particular value appears in the volume.
  inline uint4 CountInstancesOfValue(const T & value);

  /// Transforms the Reference frame (transverse to coronal/saggittal).
  void PermuteCoordinates(hdr::PermuteCoordinatesEnum view);
  /// Fit the volume according to the Resampling type specified by the enum to the destVolume
  void Fit(Volume<L,T> & destVol, hdr::ResampleTypeEnum resampleType) const;
  /// strech|shrink the volume according to the Resampling type specified by the enum
  void Resample(const Triple<uint4> & dimension, hdr::ResampleTypeEnum resampleType = hdr::TRILIN);
  /// resize the volume, but do not resample... results in loss of all data
  void Resize(const Triple<uint4> & dimension);
  
  void Crop(); // crops the volume to the tightest bounding box that does not loose any non-background data
  /// Computes and returns the resampled value at the location specified: boundary checking
  float4 GetResampledValue(const Point<float4> & point4) const;
    /// Computes and returns the resampled value at the location specified
  float4 GetResampledValueRaw(const Point<float4> & point4) const;

  /// Returns the header map for setting in other volumes.
  const VarMap & GetHdrMap() const;
  /// Sets the header map, should only be used in conjunction with GetHdrMap().
  void SetHdrMap(const VarMap & newMap);

  /**
   * Set a new header variable name, value and description.
   * @param sVarName the variable name.
   * @param value the variable value, the << operator must be available
   * for conversion to string, the string must not contain the # symbol
   * since it is used for parsing the header file text.
   * @param sDesc the variable description.
   */
  template <class Type> void SetHdrVar(const std::string & sVarName,
                                       const Type & value, const std::string & sDesc)
  {
    m_data.SetHdrVar(sVarName, value, sDesc);
  }

  /**
   * Looks up varName and assigns value.  Behavior is undefined for value 
   * of type char*, std::string type is preferred.
   * @param varName the name of the variable to retrieve values for.
   * @param value the value associated with varName is assigned to, the >>
   * operator must be available for coversion to Type.
   * @param desc the description associated with the varName.
   * @return false if varName does not exist in the header, true otherwise.
   */
  template <class Type> bool GetHdrVar(const std::string & sVarName, Type & value)
  {
    return (m_data.GetHdrVar(sVarName, value) );
  }


  /// Test function for the Volume class contained.
  static bool Test      (std::ostream & os = std::cout);
  static bool TestByType(std::ostream & os = std::cout);

  inline typename L::VolIterator GetVolIterator(const Point<uint2> & startPoint = Point<uint2>(0,0,0))
  {
    return (m_data.GetVolIterator(startPoint));
  }

  inline typename L::RayIterator GetRayIterator(const Point<float4> startPoint, 
                                            const Vector<float> & directionVector, 
                                            const float4 fSampleDistance)
  {
    return (m_data.GetRayIterator(startPoint,directionVector,fSampleDistance));
  }
	
	/// Sub volume interator
  inline typename L::SubVolIterator GetSubVolIterator(const uint2 uBoundary = 0)
  {
    return (m_data.GetSubVolIterator(uBoundary));
  }

  inline typename L::SubVolIterator GetSubVolIterator(const Box<uint4>& boundingBox)
  {
    return (m_data.GetSubVolIterator(boundingBox));
  }
	
	/// X slice iterator
  inline typename L::XSliceIterator GetXSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {
    return (m_data.GetXSliceIterator(uIdxSlice, uBoundary));
  }

  inline typename L::XSliceIterator GetXSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    return (m_data.GetXSliceIterator(uIdxSlice, boundingBox));
  }
	
	/// Y slice iterator
  inline typename L::YSliceIterator GetYSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {
    return (m_data.GetYSliceIterator(uIdxSlice, uBoundary));
  }
  inline typename L::YSliceIterator GetYSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    return (m_data.GetYSliceIterator(uIdxSlice, boundingBox));
  }

	/// Z slice iterator
  inline typename L::ZSliceIterator GetZSliceIterator(const uint2 uIdxSlice, const uint2 uBoundary = 0)
  {
    return (m_data.GetZSliceIterator(uIdxSlice, uBoundary));
  }

  inline typename L::ZSliceIterator GetZSliceIterator(const uint2 uIdxSlice, const BoundingRect<uint4>& boundingBox)
  {
    return (m_data.GetZSliceIterator(uIdxSlice, boundingBox));
  }
	

	/**
   * Compute Gradients at the Point specified.
   * The type of the Gradient Vector to be returned is type T
   * @param point Point<unsigned int> &
   * @param gradType T &
   */
  template <class C>
  Vector<C> CentralDiffGradientRaw(const Point<uint2> & point, C & gradType)
  {
    uint4 x=point.m_x, y=point.m_y, z=point.m_z;
    return (Vector<C>(
                    static_cast<C>(m_data.GetVoxelRaw(Point<uint2>(x+1,y  ,z  ))) -
                    static_cast<C>(m_data.GetVoxelRaw(Point<uint2>(x-1,y  ,z  ))),
                    static_cast<C>(m_data.GetVoxelRaw(Point<uint2>(x  ,y+1,z  ))) -
                    static_cast<C>(m_data.GetVoxelRaw(Point<uint2>(x  ,y-1,z  ))),
                    static_cast<C>(m_data.GetVoxelRaw(Point<uint2>(x  ,y  ,z+1))) -
                    static_cast<C>(m_data.GetVoxelRaw(Point<uint2>(x  ,y  ,z-1)))  ));
  } // end of CentralDiffGradientRaw()

  /**
   * Compute Gradients at the Point specified: boundary checking.
   * The type of the Gradient Vector to be returned is type T
   * @param point Point<unsigned int> &
   * @param gradType T &
   */
  template <class C>
  Vector<C> CentralDiffGradient(const Point<uint2> & point, C & gradType)
  {
    Triple<uint4>dimensions(m_data.GetDim());
    if ( 
       point.m_x>=1 && point.m_x<=dimensions.m_x-2 &&
       point.m_y>=1 && point.m_y<=dimensions.m_y-2 &&
       point.m_z>=1 && point.m_z<=dimensions.m_z-2 )
    {
      return (CentralDiffGradientRaw(point, gradType));
    } 
    else
    {
      uint4 x=point.m_x, y=point.m_y, z=point.m_z;      
      return (Vector<C>(
        static_cast<C>(GetVoxel(Point<uint2>(x+1,y  ,z  ))) -
        static_cast<C>(GetVoxel(Point<uint2>(x-1,y  ,z  ))),
        static_cast<C>(GetVoxel(Point<uint2>(x  ,y+1,z  ))) -
        static_cast<C>(GetVoxel(Point<uint2>(x  ,y-1,z  ))),
        static_cast<C>(GetVoxel(Point<uint2>(x  ,y  ,z+1))) -
        static_cast<C>(GetVoxel(Point<uint2>(x  ,y  ,z-1)))  ));
    }
  } // end of CentralDiffGradient()


  /**
   * Compute Gradients at the Point specified.
   * The type of the Gradient Vector to be returned is type T
   * @param point Point<float4> &
   * @param gradType T &
   */
  template <class C>
  Vector<C> CentralDiffGradientRaw(const Point<float4> & point, C & gradType)
  {
    uint4 x=point.m_x, y=point.m_y, z=point.m_z;
    return Vector<C>(
                    GetResampledValueRaw(Point<float4>(x+1,y  ,z  )) -
                    GetResampledValueRaw(Point<float4>(x-1,y  ,z  )),
                    GetResampledValueRaw(Point<float4>(x  ,y+1,z  )) -
                    GetResampledValueRaw(Point<float4>(x  ,y-1,z  )),
                    GetResampledValueRaw(Point<float4>(x  ,y  ,z+1)) -
                    GetResampledValueRaw(Point<float4>(x  ,y  ,z-1)) );
  } // end of CentralDiffGradientRaw()


  /**
   * Compute Gradients at the Point specified: boundary checking.
   * The type of the Gradient Vector to be returned is type T
   * @param point Point<float4> &
   * @param gradType T &
   */
  template <class C>
  Vector<C> CentralDiffGradient(const Point<float4> & point, C & gradType)
  {
    Triple<uint4>dimensions(m_data.GetDim());
    if ( 
       point.m_x>=1 && point.m_x<=dimensions.m_x-3 &&
       point.m_y>=1 && point.m_y<=dimensions.m_y-3 &&
       point.m_z>=1 && point.m_z<=dimensions.m_z-3 )
    {
      return (CentralDiffGradientRaw(point, gradType));
    } 
    else
    {
      uint4 x=point.m_x, y=point.m_y, z=point.m_z;
      return Vector<C>(GetResampledValue(Point<float4>(x+1,y  ,z  )) -
                       GetResampledValue(Point<float4>(x-1,y  ,z  )),
                       GetResampledValue(Point<float4>(x  ,y+1,z  )) -
                       GetResampledValue(Point<float4>(x  ,y-1,z  )),
                       GetResampledValue(Point<float4>(x  ,y  ,z+1)) -
                       GetResampledValue(Point<float4>(x  ,y  ,z-1)) );
    }
  } // end of CentralDiffGradient()


  void PrintAllRaw(std::ostream & os = std::cout) const;
  void PrintAllRaw(std::ostream & os, int4 noCastDummy) const;
private:
  /// Builds a preview image.
  //void BuildIcon();
  /// Finds the limits for the alpha function for BuildIcon().
  void FindAlphaLimits(Histogram<T> & volHistogram, uint2 & alphaLLim,
                       uint2 & alphaHLim);
  
/**
 * Ray casts the Volume data for building a preview image (icon) for the PNG file.
 */
  //void RayCast( const Histogram<T> & volHistogram,
  //  PNGImage & icon,
  //  uint2 & alphaLLimBin, uint2 & alphaHLimBin,
  //  const float4 maxGradientMagnitude)
  //{
  //  uint2 histsize(volHistogram.GetNumOfBins());
  //  // color transfer function cutoffs
  //  uint2 pallettewidth(static_cast<uint2>(pow(2.0F,icon.GetBitDepth())));
  //  Vector< float4 > colorBlk(0,0,0);
  //  Vector< float4 > colorBlu(0,0,1);
  //  Vector< float4 > colorGrn(0,1,0);
  //  Vector< float4 > colorRed(1,0,0);
  //  Vector< float4 > colorCya(1,0,1);
  //  Vector< float4 > colorMag(0,1,1);
  //  Vector< float4 > colorYel(1,1,0);
  //  Vector< float4 > colorWhi(1,1,1);
  //  std::vector< Vector< float4 > > colors(20);
  //  float4 f05(0.5F), f13(1.0F/3.0F), f23(2.0F/3.0F);
  //  colors[ 0] = colorBlk;
  //  colors[ 1] = colorBlk*f05 + colorBlu*f05;
  //  colors[ 2] = colorBlu;
  //  colors[ 3] = colorBlu*f23 + colorGrn*f13;
  //  colors[ 4] = colorBlu*f13 + colorGrn*f23;
  //  colors[ 5] = colorGrn;
  //  colors[ 6] = colorGrn*f23 + colorRed*f13;
  //  colors[ 7] = colorGrn*f13 + colorRed*f23;
  //  colors[ 8] = colorRed;
  //  colors[ 9] = colorRed*f05 + colorCya*f05;
  //  colors[10] = colorCya;
  //  colors[11] = colorCya*f23 + colorMag*f13;
  //  colors[12] = colorCya*f13 + colorMag*f23;
  //  colors[13] = colorMag;
  //  colors[14] = colorMag*f23 + colorYel*f13;
  //  colors[15] = colorMag*f13 + colorYel*f23;
  //  colors[16] = colorYel;
  //  colors[17] = colorYel*f05 + colorWhi*f05;
  //  colors[18] = colorWhi;
  //  colors[19] = colorWhi;
  //  
  //  Vector<float4> lightDir( -1, 0, 0.5);
  //  lightDir.Normalize();
  //  // allocating for transfer func and RGB lookup tables
  //  std::vector<float4> alphafn(histsize);
  //  std::vector<float4> Rval(histsize);
  //  std::vector<float4> Bval(histsize);
  //  std::vector<float4> Gval(histsize);
  //  // Computing lookup tables for alpha, R, G, B values
  //  for ( int8 i = 0; i < histsize; i++ )
  //  {
  //    if ( i < alphaLLimBin )
  //    {
  //      alphafn[i] = 0;
  //    } else if ( i >= alphaLLimBin && i < alphaHLimBin )
  //    {
  //      alphafn[i] = (i - alphaLLimBin)/float4(alphaHLimBin - alphaLLimBin);
  //    } else
  //    {
  //      alphafn[i] = 1;
  //    } // transfer function (density to color mapping)
  //    float4 floatIndex(i/(histsize-1.0)*(colors.size()-1));
  //    uint4 indexColor0(floatIndex);
  //    uint4 indexColor1(indexColor0+1);
  //    float4 weight(floatIndex-indexColor0), oneMinusWeight(1.0F-weight);
  //    
  //    Rval[i] = colors[indexColor0].m_x*oneMinusWeight + colors[indexColor1].m_x*weight;
  //    Gval[i] = colors[indexColor0].m_y*oneMinusWeight + colors[indexColor1].m_y*weight;
  //    Bval[i] = colors[indexColor0].m_z*oneMinusWeight + colors[indexColor1].m_z*weight;
  //  } // for (i<histsize)
  //  // make sure m_backbgound was set to alpha == 0
  //  alphafn[volHistogram.GetBin(GetBackground())] = 0;
  //  
  //  /// Ray Casting
  //  uint2 endX = 0, startX = 0;
  //  uint2 endY = 0, startY = 0;
  //  
  //  /// gradient magnitude modulated full resolution image
  //  Vector<float4> gradVec;
  //  float4 gradientTypeFloat(0);
  //  T voxelvalue;
  //  typename L::VolIterator iter(GetVolIterator());
  //  uint2 v,w;
  //  for ( w=1; w<GetDim().m_z-2; w++ )
  //  {
  //    for ( v=1; v<GetDim().m_y-2; v++ )
  //    {
  //      iter.MoveAbs(Point<uint2>(1,v,w));
  //      float4 alpha, illum;
  //      // accumulated colors for true colors
  //      float4 accumT = 1.0F, accbrR = 0, accbrB = 0, accbrG = 0;
  //      //  appoximate space leap through zero density and zero alpha voxels
  //      for ( ; !iter.End() && ((voxelvalue=iter.GetVoxelRaw()) <= 0 ||
  //        alphafn[volHistogram.GetBin(voxelvalue)] == 0); iter.IncX(8) )
  //      {
  //      }
  //      if ( iter.GetPos().m_x > 0 && !iter.End() )
  //      {
  //        // space leaping found some non-zero voxel, go back to begining of non-zero area
  //        // which must be somewere at index 0 1 2 3 4 5 6 7 8
  //        // with 8 == last test and 0 == previous test
  //        for ( iter.DecX(7); iter.GetVoxelRaw() == 0; iter.IncX() )
  //        {
  //          // noop, just looking for first non-zero voxel
  //        }
  //      }
  //      // for one ray till early ray termination or till end of volume
  //      for ( ; accumT > 0.1 && !iter.End(); iter.IncX() )
  //      {
  //        // GetVoxel for the right data type
  //        voxelvalue = iter.GetVoxelRaw();
  //        if ( voxelvalue > 0 )
  //        {
  //          // Gets the bin of the Histogram for this data value
  //          uint2 bin(volHistogram.GetBin(voxelvalue));
  //          // alpha value from the transfer func lookup table
  //          alpha = alphafn[bin];
  //          if ( alpha > 0 )
  //          {
  //            gradVec = CentralDiffGradientRaw(iter.GetPos(), gradientTypeFloat);
  //            const float4 gradMagnitude = gradVec.GetLength();
  //            float4 gradMagnitudeModulation = Bound(0.0F, gradMagnitude / maxGradientMagnitude, 1.0F);
  //            gradMagnitudeModulation = 0.75 * (gradMagnitudeModulation * gradMagnitudeModulation);
  //            // gradient computed but unnormalized
  //            gradVec.Normalize();
  //            illum = Abs(lightDir.Dot(gradVec));
  //            const float4 ka(float4(0.20)), kd(float4(1.0) - ka);
  //            // illumination computed from the gradients(ambient and diffuse)
  //            illum = ka + (kd * illum);
  //            alpha *= gradMagnitudeModulation;
  //            // F2B accumulated colors in the true-color-scale
  //            accumT *= (1-alpha);
  //            const float4 accumA = 1 - accumT;
  //            accbrR = (( accbrR * (accumA) ) + ( illum * Rval[bin] * accumT ));
  //            accbrG = (( accbrG * (accumA) ) + ( illum * Gval[bin] * accumT ));
  //            accbrB = (( accbrB * (accumA) ) + ( illum * Bval[bin] * accumT ));
  //          }
  //        }
  //      }// for one ray
  //      icon.SetPixel(v,w,Triple<uint1>(uint1((pallettewidth-1) * accbrR),
  //        uint1((pallettewidth-1) * accbrG),
  //        uint1((pallettewidth-1) * accbrB) ));
  //    } // for (v)
  //    ProgressPrinter::ShowProgress(true , w, (GetDim().m_z-1)/100.0);
  //  }  // for (w)
  //  endX += GetDim().m_y;
  //  endY += GetDim().m_z;
  //  
  //  /// mid volume slice
  //  
  //  startX = endX;
  //  // do not change startY; this image is just to the right of the first
  //  for ( w=1; w<GetDim().m_z-1; ++w )
  //  {
  //    for ( v=1; v<GetDim().m_y-1; ++v )
  //    {
  //      iter.MoveAbs(Point<uint2>(GetDim().m_x/2,v,w));
  //      // accumulated colors for true colors
  //      float4 r = 0, g = 0, b = 0;
  //      // GetVoxel for the right data type
  //      voxelvalue = iter.GetVoxelRaw();
  //      if ( voxelvalue > 0 )
  //      {
  //        // Gets the bin of the Histogram for this data value
  //        uint8 bin(volHistogram.GetBin(voxelvalue));
  //        // alpha value from the transfer func lookup table
  //        r = Rval[bin];
  //        g = Gval[bin];
  //        b = Bval[bin];
  //      }
  //      
  //      icon.SetPixel(startX + v, startY + w,
  //        Triple<uint1>(uint1((pallettewidth-1) * r),
  //        uint1((pallettewidth-1) * g),
  //        uint1((pallettewidth-1) * b) ));
  //    } // for (v)
  //    ProgressPrinter::ShowProgress(true, w, (GetDim().m_z-1)/100.0);
  //  }  // for (w)
  //  
  //  endX += GetDim().m_y;
  //  // endY is still the same;
  //  
  //  /// small high opacity volume renderings
  //  
  //  startX = 0;
  //  startY = endY;
  //  const float4 slabThickness = GetDim().m_x/4.0F;
  //  for ( w=1; w<GetDim().m_z-1; w +=2 )
  //  {
  //    for ( v=1; v<GetDim().m_y-1; v += 2 )
  //    {
  //      iter.MoveAbs(Point<uint2>(1,v,w));
  //      float4 alpha, illum;
  //      // accumulated colors for true colors
  //      float4 accbrR = 0, accbrB = 0, accbrG = 0;
  //      for ( uint4 slab=0; slab<4; ++slab )
  //      {
  //        for ( ; iter.GetPos().m_x < (1+slab)*slabThickness; iter.IncX() )
  //        {
  //          // GetVoxel for the right data type
  //          voxelvalue = iter.GetVoxelRaw();
  //          if ( voxelvalue > 0 )
  //          {
  //            // Gets the bin of the Histogram for this data value
  //            uint8 bin(volHistogram.GetBin(voxelvalue));
  //            // alpha value from the transfer func lookup table
  //            alpha = alphafn[bin];
  //            if ( alpha > 0 )
  //            {
  //              gradVec = CentralDiffGradientRaw(iter.GetPos(), gradientTypeFloat);
  //              gradVec.Normalize();
  //              illum = Abs(lightDir.Dot(gradVec));
  //              const float4 ka(float4(0.20)), kd(float4(1.0) - ka);
  //              // illumination computed from the gradients(ambient and diffuse)
  //              illum = ka + (kd * illum);
  //              // B2F accumulated colors in the true-color-scale
  //              accbrR = (( accbrR * (1 - alpha) ) + ( illum * Rval[bin] * alpha ));
  //              accbrG = (( accbrG * (1 - alpha) ) + ( illum * Gval[bin] * alpha ));
  //              accbrB = (( accbrB * (1 - alpha) ) + ( illum * Bval[bin] * alpha ));
  //            }
  //          }
  //        }// for one slab ray
  //        icon.SetPixel(startX + slab*(GetDim().m_y/2) + (v/2),
  //          startY + (w/2),
  //          Triple<uint1>(uint1((pallettewidth-1) * accbrR),
  //          uint1((pallettewidth-1) * accbrG),
  //          uint1((pallettewidth-1) * accbrB) ));
  //      } // for one complete ray
  //    } // for (v)
  //    ProgressPrinter::ShowProgress(true, w, (GetDim().m_z-1)/100.0);
  //  }  // for (w)
  //  endY += GetDim().m_z/2;
  //  
  //  /// draw color transfer function
  //  
  //  startX = 0;
  //  startY = endY;
  //  uint2 yy=0;
  //  uint2 chartHeight = 40;
  //  uint2 x=0;
  //  
  //  for ( yy=0; yy<chartHeight; ++yy ) {
  //    for ( x=0; x<icon.GetWidth(); x += 1 ) {
  //      uint8 bin = histsize*x/(icon.GetWidth()-1);
  //      
  //      icon.SetPixel(x, startY + yy,
  //        Triple<uint1>(uint1((pallettewidth-1) * Rval[bin]),
  //        uint1((pallettewidth-1) * Gval[bin]),
  //        uint1((pallettewidth-1) * Bval[bin]) ));
  //    } // for all x
  //  } // for all y
  //  endY += chartHeight;
  //  
  //  /// draw alpha transfer function (on top of colorTransfer function)
  //  
  //  for ( x=0; x<icon.GetWidth(); ++x )
  //  {
  //    uint4 bin = (histsize-1)*x/(icon.GetWidth()-1);
  //    icon.SetPixel(uint4(x+0.5), startY +1+ (chartHeight-2) - (chartHeight-2)*alphafn[bin],
  //      Triple<uint1>(255,255,255));
  //  } // for all x
  //  for ( x=0; x<icon.GetWidth(); ++x )
  //  {
  //    uint4 bin = (histsize-1)*x/(icon.GetWidth()-1);
  //    icon.SetPixel(uint4(x+0.5), startY + (chartHeight-2) - (chartHeight-2)*alphafn[bin],
  //      Triple<uint1>(0,0,0));
  //  } // for all x
  //  
  //  
  //  /// draw histogram
  //  startY = endY;
  //  PNGImage histImage(icon.GetWidth(), 4*chartHeight);
  //  volHistogram.MakeHistogramImage(histImage);
  //  for ( x=0; x<icon.GetWidth(); ++x )
  //  {
  //    for ( yy=0; yy<4*chartHeight; ++yy )
  //    {
  //      icon.SetPixel(x,endY+yy,histImage.GetPixel(x,yy));
  //    } // for all y
  //  } // for all x
  //  endY += 4*chartHeight;
  //  
  //} // RayCast <T>
    


public:
	const hdr::DATATYPE GetDataType() const { return m_eDataType;}
	virtual void SetDataType();

	/// Increase the reference count
	const uint4 IncRefCount() { return ++m_uRefCount;}

	/// Decrease the reference count
	const uint4 DecRefCount() { return --m_uRefCount;}
	
	inline L& GetVolAPI() {return m_data;}
	hdr::MemoryLayoutEnum GetMemoryLayout() {return m_data.GetMemoryLayout();}
protected:
	hdr::DATATYPE m_eDataType;
	uint4 m_uRefCount;
#ifdef __linux // under GCC you can't be friend of yourself.
public: // data
  // instance of a volume
  L m_data;
#endif // __linux

#ifdef _WIN32 // VC++ lets you be friend of yourself.
private:
  // instance of a volume
  L m_data;
#endif // _WIN32

}; // Volume

//template<>
//void Volume<LinearVolume<bool>, bool>::RayCast( const Histogram<bool> & volHistogram, PNGImage & icon,
//                                              uint2 & alphaLLim, uint2 & alphaHLim,
//                                              const float4 maxGradientMagnitude)
//{
//  throw ex::VException(LogRec( "please implement RayCast for bool","Volume<LinearVolume<bool>, bool>","RayCast"));
//
//} // RayCast <bool>

//template<>
//void Volume<BlockVolume<bool>, bool>::RayCast( const Histogram<bool> & volHistogram, PNGImage & icon,
//                                              uint2 & alphaLLim, uint2 & alphaHLim,
//                                              const float4 maxGradientMagnitude)
//{
//  throw ex::VException(LogRec( "please implement RayCast for bool","Volume<LinearVolume<bool>, bool>", "RayCast"));  
//} // RayCast <bool>

template <class L, class T>
inline std::ostream & operator<<(std::ostream &os, const Volume<L, T> & volume);  // operator <<

typedef  Volume< LinearVolume<bool  >,bool  >  LinVolB;
typedef  Volume< LinearVolume<uint1 >,uint1 >  LinVolUC;
typedef  Volume< LinearVolume<uint2 >,uint2 >  LinVolUS;
typedef  Volume< LinearVolume<uint4 >,uint4 >  LinVolUI;
typedef  Volume< LinearVolume<int1  >,int1  >  LinVolC;
typedef  Volume< LinearVolume<int2  >,int2  >  LinVolS;
typedef  Volume< LinearVolume<int4  >,int4  >  LinVolI;
typedef  Volume< LinearVolume<float4>,float4>  LinVolF;

typedef  Volume< BlockVolume<bool  >,bool  >  BlockVolB;
typedef  Volume< BlockVolume<uint1 >,uint1 >  BlockVolUC;
typedef  Volume< BlockVolume<uint2 >,uint2 >  BlockVolUS;
typedef  Volume< BlockVolume<uint4 >,uint4 >  BlockVolUI;
typedef  Volume< BlockVolume<int1  >,int1  >  BlockVolC;
typedef  Volume< BlockVolume<int2  >,int2  >  BlockVolS;
typedef  Volume< BlockVolume<int4  >,int4  >  BlockVolI;
typedef  Volume< BlockVolume<float4>,float4>  BlockVolF;

typedef  LinearVolume<bool  >::VolIterator LinVolIterB;
typedef  LinearVolume<uint1 >::VolIterator LinVolIterUC;
typedef  LinearVolume<uint2 >::VolIterator LinVolIterUS;
typedef  LinearVolume<uint4 >::VolIterator LinVolIterUI;
typedef  LinearVolume<int1  >::VolIterator LinVolIterC;
typedef  LinearVolume<int2  >::VolIterator LinVolIterS;
typedef  LinearVolume<int4  >::VolIterator LinVolIterI;
typedef  LinearVolume<float4>::VolIterator LinVolIterF;

typedef  BlockVolume<bool  >::VolIterator BlockVolIterB;
typedef  BlockVolume<uint1 >::VolIterator BlockVolIterUC;
typedef  BlockVolume<uint2 >::VolIterator BlockVolIterUS;
typedef  BlockVolume<uint4 >::VolIterator BlockVolIterUI;
typedef  BlockVolume<int1  >::VolIterator BlockVolIterC;
typedef  BlockVolume<int2  >::VolIterator BlockVolIterS;
typedef  BlockVolume<int4  >::VolIterator BlockVolIterI;
typedef  BlockVolume<float4>::VolIterator BlockVolIterF;

namespace vol {

  typedef  Volume< LinearVolume<bool  >,bool  >  LinVolBool;
  typedef  Volume< LinearVolume<uint1 >,uint1 >  LinVolUInt1;
  typedef  Volume< LinearVolume<uint2 >,uint2 >  LinVolUInt2;
  typedef  Volume< LinearVolume<uint4 >,uint4 >  LinVolUInt4;
  typedef  Volume< LinearVolume<int1  >,int1  >  LinVolInt1;
  typedef  Volume< LinearVolume<int2  >,int2  >  LinVolInt2;
  typedef  Volume< LinearVolume<int4  >,int4  >  LinVolInt4;
  typedef  Volume< LinearVolume<float4>,float4>  LinVolFloat4;

  typedef  Volume<  BlockVolume<bool  >,bool  >  BlkVolBool;
  typedef  Volume<  BlockVolume<uint1 >,uint1 >  BlkVolUInt1;
  typedef  Volume<  BlockVolume<uint2 >,uint2 >  BlkVolUInt2;
  typedef  Volume<  BlockVolume<uint4 >,uint4 >  BlkVolUInt4;
  typedef  Volume<  BlockVolume<int1  >,int1  >  BlkVolInt1;
  typedef  Volume<  BlockVolume<int2  >,int2  >  BlkVolInt2;
  typedef  Volume<  BlockVolume<int4  >,int4  >  BlkVolInt4;
  typedef  Volume<  BlockVolume<float4>,float4>  BlkVolFloat4;

}

namespace it {

  typedef  LinearVolume<bool  >::VolIterator IterLinVolBool;
  typedef  LinearVolume<uint1 >::VolIterator IterLinVolUInt1;
  typedef  LinearVolume<uint2 >::VolIterator IterLinVolUInt2;
  typedef  LinearVolume<uint4 >::VolIterator IterLinVolUInt4;
  typedef  LinearVolume<int1  >::VolIterator IterLinVolInt1;
  typedef  LinearVolume<int2  >::VolIterator IterLinVolInt2;
  typedef  LinearVolume<int4  >::VolIterator IterLinVolInt4;
  typedef  LinearVolume<float4>::VolIterator IterLinVolFloat4;

  typedef  BlockVolume< bool  >::VolIterator IterBlkVolBool;
  typedef  BlockVolume< uint1 >::VolIterator IterBlkVolUInt1;
  typedef  BlockVolume< uint2 >::VolIterator IterBlkVolUInt2;
  typedef  BlockVolume< uint4 >::VolIterator IterBlkVolUInt4;
  typedef  BlockVolume< int1  >::VolIterator IterBlkVolInt1;
  typedef  BlockVolume< int2  >::VolIterator IterBlkVolInt2;
  typedef  BlockVolume< int4  >::VolIterator IterBlkVolInt4;
  typedef  BlockVolume< float4>::VolIterator IterBlkVolFloat4;

}

#undef FILE_REVISION


#include "Volume.inl"


#endif

// $Log: Volume.h,v $
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
// Revision 3.10  2003/09/05 20:22:16  mkontak
// change for vx::Date
//
// Revision 3.9  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.8.2.1  2003/08/11 11:52:27  mkontak
// Added Date to vx namespace
//
// Revision 3.8  2003/05/16 11:54:10  michael
// code review
//
// Revision 3.7  2003/05/14 17:49:01  mkontak
// Added FILE_REVISION
//
// Revision 3.6  2002/04/04 22:40:17  ingmar
// removed obsolete recursive block volume related classes
//
// Revision 3.5  2002/04/04 22:19:49  ingmar
// removed obsolete recursive block volume related classes
//
// Revision 3.4  2002/03/15 19:41:05  ingmar
// made RayIterBlk/Lin friend classes
//
// Revision 3.3  2002/03/13 17:05:27  ingmar
// added friend declarations for new iterators
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
//    Rev 2.0   Sep 16 2001 23:47:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:10   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.92  2001/08/30 23:30:09  pagron
// named Resize method on volumes more appropriately as Resample and added a Resize func that does a dummy resize
//
// Revision 1.91  2001/08/30 15:32:21  soeren
// added new resize more no resample
//
// Revision 1.90  2001/08/28 23:53:13  ingmar
// Sec_volume -> Sec_8_volume
//
// Revision 1.89  2001/08/27 00:02:42  ingmar
// better colormap in icon
//
// Revision 1.88  2001/08/19 22:30:15  ingmar
// added GetSample()
//
// Revision 1.87  2001/08/16 22:46:17  jaddonisio
// Improved comments.
//
// Revision 1.86  2001/08/15 15:39:50  liwei
// Added reference count.
//
// Revision 1.85  2001/08/12 18:54:53  soeren
// fixed GetResampledGradient()
//
// Revision 1.84  2001/08/11 22:30:19  soeren
// exceptions for unimplemented functions
//
// Revision 1.83  2001/08/11 22:27:24  soeren
// fixed GetVoxel slowdown (pipeline now 30 min  faster :-)
// speed up in GetResampled
// specialized RayCast to no longer do anything for bool
//
// Revision 1.82  2001/08/09 21:17:26  liwei
// Constructor of BoundingRect and Box changed.
//
// Revision 1.81  2001/08/08 17:32:57  liwei
// Bounding boxes are all of the type Box<uint4>.
// Bugs in LinearVolumeIterator::NextZYX() fixed.
//
// Revision 1.80  2001/08/06 23:24:26  liwei
// Passing boundingBox rather minCorner and maxCorner when creating SubVolume Iterator
// and slice iterators.
//
// Revision 1.79  2001/07/31 18:46:29  liwei
// Bugs in the revised version of SliceIterator fixed.
//
// Revision 1.78  2001/07/30 23:29:30  liwei
// Renamed VolIteratorSkipBoundary to SubVolIterator.
// Make slice iterators be subclasses of SubVolIterator.
//
// Revision 1.77  2001/07/30 17:39:00  huamin
// Added RayIterator
//
// Revision 1.76  2001/07/25 19:08:11  liwei
// AnyVolume now supports BlkSegVol.
// Exception handling updated to use the LogRecord.
//
// Revision 1.75  2001/07/19 22:50:44  liwei
// Added VolIteratorSkipBoundary for linear and block volume.
// Bugs of the PeekX() functions of the block volume fixed.
//
// Revision 1.74  2001/07/18 20:52:40  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.73  2001/07/09 16:50:58  frank
// Added support for the new V3D file format.
//
// Revision 1.72  2001/06/26 19:05:27  liwei
// Added GetVolAPI().
//
// Revision 1.71  2001/06/26 16:00:50  liwei
// Add SPECIFIC to the DATATYPE enum of voxels.
//
// Revision 1.70  2001/06/26 14:52:08  ana
// CentralDiffGradient uses GetResampledValue
//
// Revision 1.69  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.68  2001/06/18 23:40:12  liwei
// Slice iterator added
//
// Revision 1.67  2001/06/14 23:53:41  liwei
// Bookkeeping the information of memory layout and datatype
//
// Revision 1.66  2001/06/06 21:39:41  ana
// added CentralDiffGradient and CentralDiffGradientRaw for float4.
// CentralDiffGradient is using GetVoxel instead of GetVoxelRaw
//
// Revision 1.65  2001/06/01 17:29:24  ana
// PrintProgress function replaced by ProgressPrinter class
//
// Revision 1.64  2001/05/22 22:21:49  liwei
// GetSize() added to return the size of the volume in world coordinates
//
// Revision 1.63  2001/05/02 19:33:38  ingmar
// Volume.h
//
// Revision 1.62.2.1  2001/05/01 22:06:16  jmeade
// misspelled enum (April 26 - jmeade)
//
// Revision 1.63  2001/04/26 15:42:31  jmeade
// misspelled enum
//
// Revision 1.62  2001/04/26 02:01:59  ingmar
// added namespace it and vol
//
// Revision 1.61  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.60  2001/04/12 21:30:16  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.59  2001/04/12 18:54:58  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.58  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.57  2001/04/10 16:13:26  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.56  2001/04/08 22:04:16  ingmar
// prepended Sec_ to documentation section dummy classes
//
// Revision 1.55  2001/04/08 21:38:23  dave
// updated doc headers
//
// Revision 1.54  2001/04/05 18:00:47  dmitry
// updated datatypes
//
// Revision 1.53  2001/04/05 03:41:35  ingmar
// added documentation classes
//
// Revision 1.52  2001/04/03 05:46:59  soeren
// added documentation
//
// Revision 1.51  2001/04/01 20:40:42  ingmar
// code beautify
// added Crop()
//
// Revision 1.50  2001/04/01 16:46:48  ingmar
// code beautify
//
// Revision 1.49  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.48  2001/03/21 22:30:03  soeren
// added merge function
//
// Revision 1.47  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.46  2000/12/12 18:34:13  dave
// corrected VolAPI constructor
//
// Revision 1.45  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.44  2000/11/18 23:33:25  dave
// corrected GetHdrVar() for type std::string
//
// Revision 1.43  2000/11/17 01:00:22  dave
// GetHdrVar returns bool
//
// Revision 1.42  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.41  2000/11/13 18:04:45  ingmar
// adapted code to new Histogram class
//
// Revision 1.40  2000/11/08 19:46:36  frank
// Added CountInstancesOfValue function for quick native counting
//
// Revision 1.39  2000/11/07 04:07:37  dave
// added SetHdrMap()
//
// Revision 1.38  2000/11/04 23:45:42  ingmar
// addeed no-cast version of PrintAll
//
// Revision 1.37  2000/11/03 17:02:27  dave
// converted back
//
// Revision 1.36  2000/11/03 15:50:12  ingmar
// GetNumberOfVoxels now returns uint
//
// Revision 1.35  2000/11/03 04:19:05  ingmar
// even better icons
//
// Revision 1.34  2000/11/03 03:14:07  ingmar
// fixed some gcc syntax errors and warnings
//
// Revision 1.33  2000/11/03 02:06:05  ingmar
// better icons
//
// Revision 1.32  2000/11/02 19:48:10  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.31  2000/11/02 04:30:37  dave
// Removed desc from GetHdrVar()
//
// Revision 1.30  2000/11/02 04:17:50  dave
// misc updates
//
// Revision 1.29  2000/11/01 21:11:30  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.28  2000/11/01 20:36:46  ingmar
// added midVolumeOffset to preview icon generation
//
// Revision 1.27  2000/11/01 17:44:26  dave
// removed GetDataType() from Volume interface
//
// Revision 1.26  2000/10/31 22:34:47  ingmar
// improved PrintAllRaw
//
// Revision 1.25  2000/10/31 19:40:44  dave
// compiles in gcc
//
// Revision 1.24  2000/10/31 17:16:03  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.23  2000/10/28 01:40:30  ingmar
// Fixed GetVoxelRaw for Point<ushort>
//
// Revision 1.22  2000/10/26 15:42:24  ingmar
// fixed default arg
//
// Revision 1.21  2000/10/26 01:37:36  ingmar
// added get/set voxel for a Point<short> position
//
// Revision 1.20  2000/10/25 14:13:00  ingmar
// put SetBackground back
//
// Revision 1.19  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.18  2000/10/24 16:58:06  dave
// added background set
//
// Revision 1.17  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.16  2000/10/23 20:18:08  ingmar
// removed VxDilator and added SpiralIterator
//
// Revision 1.15  2000/10/22 23:48:21  dave
// added RecBlockVolume
//
// Revision 1.14  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.13  2000/10/22 00:52:09  dave
// better Iterators
//
// Revision 1.12  2000/10/22 00:24:23  dave
// better iterators
//
// Revision 1.11  2000/10/21 06:06:03  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.10  2000/10/19 19:59:18  ingmar
// moved Global.h up to get rid of VC++ compile warnings
//
// Revision 1.9.6.1  2000/10/20 19:23:56  ingmar
// added Fit function
//
// Revision 1.9  2000/10/03 21:38:08  dave
// added HeaderVolume
//
// Revision 1.8  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.7  2000/09/24 23:22:55  ingmar
// moved Global.h up and added Error.h
//
// Revision 1.6.2.1  2000/09/25 18:56:37  kevin
// Made Static Libraries
//
// Revision 1.6  2000/09/19 20:05:17  ingmar
// generic makefile with full optimizations
//
// Revision 1.5  2000/09/17 18:34:24  ingmar
// overloaded GetHdrVar to possibly ignore the description string
//
// Revision 1.4  2000/09/17 02:16:53  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.3  2000/09/13 22:39:57  dave
// BlockVolume Write improved
//
// Revision 1.2  2000/09/11 22:46:09  dave
// added date field to constructors
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 47    9/08/00 11:29a Ingmar
// code beautify
//
// 46    9/07/00 11:10a Dave
// updates to accomodate BlockVolume
//
// 45    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 44    9/04/00 1:39a Dave
// checking in for cvs change
//
// 43    9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 42    8/31/00 1:08p Dave
// added SetFileName
//
// 41    8/30/00 2:42p Dave
// added SetUnits()
//
// 40    8/21/00 1:56a Sarang
// updatedheader
//
// 39    8/20/00 6:49p Sarang
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// 38    8/19/00 3:16p Ingmar
// GetVoxel/GetVoxelRaw + inline Error::DebugMsg
//
// 37    8/17/00 2:27a Jay
// Added header for Creation Date
//
// 36    8/16/00 1:35p Jay
// Overloaded constructor in Volume to enable creating HashVolumes with
// various hash bits
//
// 35    8/16/00 11:45a Antonio
// made header modifications
//
// *****************  Version 34  *****************
// User: Antonio      Date: 8/11/00    Time: 1:40p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 33  *****************
// User: Jay             Date:  8/11/00  Time:  1:37a
// Checked in $/v2k/src/Volume
// Comment:
//
//
// *****************  Version 32  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 31  *****************
// User: Jay             Date:  8/08/00  Time: 10:29a
// Checked in $/vx/src/Volume
//
// *****************  Version 30  *****************
// User: Dave            Date:  8/08/00  Time: 10:34a
// Checked in $/vx/src/Volume
//
// *****************  Version 29  *****************
// User: Jay             Date:  8/08/00  Time: 10:11a
// Checked in $/vx/src/Volume
// Comment:
//   new template version
//
// *****************  Version 28  *****************
// User: Jay             Date:  8/08/00  Time:  1:39a
// Checked in $/vx/src/Volume
// Comment:
//   Before changing the implementation of Volume to that using
// std::vector
//
// *****************  Version 27  *****************
// User: Jay             Date:  8/08/00  Time: 12:00a
// Checked in $/vx/src/Volume
// Comment:
//   Cleaned up
//
// *****************  Version 26  *****************
// User: Jay             Date:  8/07/00  Time: 10:10p
// Checked in $/vx/src/Volume
//
// *****************  Version 25  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 24  *****************
// User: Kevin           Date:  8/01/00  Time:  2:00p
// Checked in $/v2k/src/Volume
// Comment:
//   Coronal and Saggittal views ok
//
// *****************  Version 23  *****************
// User: Jay             Date:  8/01/00  Time: 11:52a
// Checked in $/v2k/src/Volume
// Comment:
//   Checking transforms .. saggitta/coronal
//
// *****************  Version 22  *****************
// User: Jay             Date:  7/31/00  Time:  1:54p
// Checked in $/v2k/src/Volume
// Comment:
//   Pipeline modified to write transverse and coronal view of Volumes
//
// *****************  Version 21  *****************
// User: Jay             Date:  7/31/00  Time:  1:27p
// Checked in $/vx/src/Volume
// Comment:
//   Added the transformation from Coronal to Saggittal view.
// If time permits one could add the other transformations too. Also instead of
// performing Matrix multiplications (9 multiplications) a 3 additions are done as
// we know that the transformation matrix has most elements zero in it.
//
// *****************  Version 20  *****************
// User: Jay             Date:  7/31/00  Time: 11:57a
// Checked in $/vx/src/Volume
// Comment:
//   Was not updating the dimension when doing a Coronal transform ..
// removed the bug, Transform (Transverse to Coronal/Saggittal) works fine
//
// *****************  Version 19  *****************
// User: Jay             Date:  7/29/00  Time:  3:16p
// Checked in $/vx/src/Volume
// Comment:
//   Added a template function RayCast to build the icon
// Moved  it in the VolAPI class
//
// *****************  Version 18  *****************
// User: Jay             Date:  7/29/00  Time:  2:22p
// Checked in $/vx/src/Volume
// Comment:
//   Transforming view to Coronal and Saggittal working
//
// *****************  Version 17  *****************
// User: Jay             Date:  7/29/00  Time:  3:26a
// Checked in $/vx/src/Volume
// Comment:
//   Added TransformCoordinates() and a template fn ApplyTransform() to
// do the same. Tried to add it in VolAPI but the different implementations of
// their data representation suggest non template implementation. Cant create
// instatnces of types passed as template parameters other than the default
// constructors and the copy constructors. Both donot work here as the dimension of
// the volumes would change here due to the transformation.
//
// *****************  Version 16  *****************
// User: Jay             Date:  7/29/00  Time:  1:49a
// Checked in $/vx/src/Volume
// Comment:
//   Removing the Transform function added temporarily
//
// *****************  Version 15  *****************
// User: Jay             Date:  7/29/00  Time: 12:21a
// Checked in $/vx/src/Volume
// Comment:
//   Added enums for the different views: CORONAL, SAGGITTAL, TRANSVERSE
//
// *****************  Version 14  *****************
// User: Jay             Date:  7/27/00  Time:  2:16p
// Checked in $/vx/src/Volume
// Comment:
//   Added Resize function to resize Volumes
//
// *****************  Version 13  *****************
// User: Dave            Date:  7/27/00  Time:  2:04a
// Checked in $/v2k/src/Volume
//
// *****************  Version 12  *****************
// User: Lihong          Date:  7/26/00  Time:  2:01p
// Checked in $/v2k/src/Volume
// Comment:
//   added SetVoxel specialization for uint
//
// *****************  Version 11  *****************
// User: Jay             Date:  7/21/00  Time:  7:33p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   RleVolume assignment operator added
//
// *****************  Version 10  *****************
// User: Antonio         Date:  7/21/00  Time: 12:36p
// Checked in $/v2k/src/Volume
// Comment:
//   added specialization for getvoxel ushort to make VC++ linking work
//
// *****************  Version 9  *****************
// User: Frank           Date:  7/20/00  Time:  1:08p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Added PDS field using new Volume class
//
// *****************  Version 8  *****************
// User: Jay             Date:  7/19/00  Time:  6:01p
// Checked in $/vx/src/Volume
// Comment:
//   Added declarations fro GetDataPtr and GetVoxelCount
//
// *****************  Version 7  *****************
// User: Jay             Date:  7/19/00  Time:  3:31p
// Checked in $/vx/src/Volume
// Comment:
//   GetStep declaration added
//
// *****************  Version 6  *****************
// User: Ingmar          Date:  7/19/00  Time:  2:41p
// Checked in $/v2k/src/Volume
// Comment:
//   fixed broken compile & link
//
// *****************  Version 5  *****************
// User: Jay             Date:  7/19/00  Time:  2:15p
// Checked in $/vx/src/Volume
// Comment:
//   Synchronized with CVS
//
// *****************  Version 4  *****************
// User: Michael         Date:  7/18/00  Time:  6:44p
// Checked in $/v2k/src/Volume
// Comment:
//   specialized Volume::GetVoxel to make linker happy
//
// *****************  Version 3  *****************
// User: Jay             Date:  7/18/00  Time:  5:51p
// Checked in $/vx/src/Volume
//
// *****************  Version 2  *****************
// User: Frank           Date:  7/18/00  Time:  3:57p
// Checked in $/Viatron1000/src/Volume
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:03a
// Created Volume.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Volume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
// $Id: Volume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Volume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
// $Id: Volume.h,v 1.3 2006/02/03 15:22:11 frank Exp $
