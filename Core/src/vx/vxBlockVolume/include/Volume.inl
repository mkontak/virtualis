// $Id: Volume.inl,v 1.5 2006/02/03 15:22:11 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this
// file may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: Summer 00
// Author: Dave & Jay  dave@viatronix.com
//
// Complete History at bottom of file.


// defines
#define FILE_REVISION "$Revision: 1.5 $"


/**
 * Convert the string str to type T and assign.
 * @param string
 * @param type
 */
template <class T> inline const T Majority(std::vector<T> vec)
{
  std::sort(vec.begin(), vec.end());

  T tCurrentValue = vec[0];
  T tMajorityValue = tCurrentValue;
  uint4 uNumOccurrence(1);
  uint4 uMaxOccurrence(0);
  std::vector<T>::iterator it(vec.begin()+1);
  std::vector<T>::iterator itEnd(vec.end());

  for (; *it==0 && it<itEnd ; ++it)
  {
  }
  if (it != itEnd)
  {
    tCurrentValue = tMajorityValue = *it;
    uNumOccurrence = 1;

    for (; it < itEnd ; ++it)
    {
      if (*it == tCurrentValue)
      {
        ++uNumOccurrence;
      }
      else
      {
        // just discovered new value, check if last value was the up-to-now majority
        if (uNumOccurrence > uMaxOccurrence)
        {
          uMaxOccurrence = uNumOccurrence;
          tMajorityValue = tCurrentValue;
        }
        uNumOccurrence = 1;
        tCurrentValue = *it;
      } // if (value was repeated)
    } // for all vector elements
  }

  return tMajorityValue;
} // Majority()


/**
 * Default constructor.
 */
template <class L, class T>
Volume<L,T>::Volume()
  : m_data()
{
	SetDataType();
	m_uRefCount = 0;
}
/**
 * Constructor for building new volumes.
 * @param dimension Triple<uint> &
 * @param units Triple<float> &
 * @param dataType hdr::DataTypeEnum
 * @param unitType hdr::UnitTypeEnum
 * @param ext hdr::ExtensionEnum
 * @param sFileName std::string
 * @param sTitle std::string
 * @param sAuthor std::string
 */
template <class L, class T>
Volume<L,T>::Volume(const Triple<uint4> & dimension, const uint4 & uiBitsPerVoxel,
                    const Triple<float4> & units, hdr::UnitTypeEnum unitType,
                    const T & background, hdr::ExtensionEnum ext,
                    const std::string & sFileName, const std::string & sTitle,
                    const std::string & sAuthor, const vx::Date & date)
  : m_data(dimension, uiBitsPerVoxel, units, unitType, background, ext,
           sFileName, sTitle, sAuthor, date)
{
	SetDataType();
	m_uRefCount = 0;
}


// same as above w/extra parameter for BlockVolume blockBitSize
template <class L, class T>
Volume<L,T>::Volume(const Triple<uint4> & dimension, const uint4 & uiBitsPerVoxel,
                    const Triple<float4> & units, hdr::UnitTypeEnum unitType,
                    const T & background, hdr::ExtensionEnum ext,
                    const std::string & sFileName, const std::string & sTitle,
                    const std::string & sAuthor, const vx::Date & date, const uint4 & uiBlockBitSize)
  : m_data(dimension, uiBitsPerVoxel, units, unitType, background, ext,
           sFileName, sTitle, sAuthor, date, uiBlockBitSize)
{
	SetDataType();
	m_uRefCount = 0;
}


/**
 * Destructor.
 */
template <class L, class T>
Volume<L,T>::~Volume()  
{
}


/**
 * Copy Constructor.
 * @param rhs const Volume<L,T> &
 */
template <class L, class T>
Volume<L,T>::Volume(const Volume<L,T> & rhs)
  : m_data(rhs.m_data)
{
	SetDataType();
	m_uRefCount = 0;
}


/**
 * Assignment operator.
 * @param rhs const Volume<L,T> &
 */
template <class L, class T>
Volume<L,T> const & Volume<L,T>::operator=(const Volume<L,T> & rhs)
{
	SetDataType();
	m_uRefCount = 0;
  m_data = rhs.m_data;
  return(*this);
}


/**
 * Merge to volumes. Meaning, after merging the equal blocks are shared
 * @param otherVolume This is the other volume
 */
template <class L, class T>
void Volume<L,T>::Merge(const Volume<L,T> & otherVolume) 
{
  m_data.Merge(otherVolume.m_data);
}


/**
 * Reads the volume specified into memory, ALWAYS check the return value.
 * @param fileName std::string.
 */
template <class L, class T>
void Volume<L,T>::Read(std::string sFileName)
{
  try 
  {
    m_data.Read(sFileName);
  }
  catch (ex::VException & error)
  {
    LogErr(error.GetLogRecord().GetTextMessage(),"Volume","Read");
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec("error read volume data error read volume data","Volume","Read"));
    
  }
}


/**
 * Writes the volume to the specified file.
 * @param sFileName std:;string
 * @param buildIcon specifies whether icon rendering is performed.
 */
template <class L, class T>
void Volume<L,T>::Write(std::string sFileName, bool buildIcon)
{
  std::stringstream tt;
  tt << "Volume<L, T>::Write(" << sFileName << " (icon:"<<buildIcon<<")"<< std::endl;
  LogDbg(tt.str(),"Volume::L::T","Write");
  tt.str("");

  if ( buildIcon )
  {
    //BuildIcon();
  }

  m_data.Write(sFileName);
} // Write


/**
 * Writes the volume to the specified file.
 * @param sFileName std:;string
 * @param buildIcon specifies whether icon rendering is performed.
 */
template <class L, class T>
void Volume<L,T>::WriteNew(std::string sFileName, bool buildIcon)
{
  std::stringstream tt;
  tt << "Volume<L, T>::Write(" << sFileName << " (icon:"<<buildIcon<<")"<< std::endl;
  LogDbg(tt.str(),"Volume::L::T","WriteNew");
  tt.str("");

  if ( buildIcon )
  {
    //BuildIcon();
  }

  m_data.WriteNew(sFileName);
} // Write


/**
 * Set the value at the location specified.  Bounds checking is performed.
 * @see SetVoxelRaw
 * @param location const Point<ushort> &
 * @param value T
 */
template <class L, class T>
void Volume<L,T>::SetVoxel(const Point<uint2> & location, T value)
{
  if ( m_data.InsideVolume(location) )
  {
    m_data.SetVoxelRaw(location, value);
  } else
  {
    LogWrn("<Volume<L,T>::SetVoxel> Attempted outside the volume at "+location,"Volume::L::T","SetVoxel");
  }
}


/**
 * Set the value at the location specified, NO bounds checking.
 * @param location const Point<ushort> &
 * @param value T
 */
template <class L, class T>
void Volume<L,T>::SetVoxelRaw(const Point<uint2> & location, T value)
{
  m_data.SetVoxelRaw(location, value);
}


/**
 * Returns the value of the voxel at the location specified by point. Bounds checking
 * is performed.
 * @see GetVoxelRaw
 * @param location the location in the volume to get the voxel.
 * @return the value at the location specified.
 */
template <class L, class T>
const T  Volume<L,T>::GetVoxel(const Point<uint2> & location) const
{
  if ( m_data.InsideVolume(location) )
  {
    return(m_data.GetVoxelRaw(location));
  } 
  else
  {
    return(m_data.GetBackground());
  }
} // GetVoxel


/**
 * Returns the value of the voxel at the location specified, NO bounds checking.
 * @param location const Point<ushort> &
 * @return the value at the location specified.
 */
template <class L, class T>
const T  Volume<L,T>::GetVoxelRaw(const Point<uint2> & location) const
{
  return(m_data.GetVoxelRaw(location));
}


/**
 * Returns the value of the voxel at the location specified, NO bounds checking.
 * @param location const Point<ushort> &
 * @return the value at the location specified.
 */
template <class L, class T>
float4 Volume<L,T>::GetSample(const Point<float4> & location) const
{
  return(m_data.GetSample(location));
}


/**
 * Gets the value returned when voxel access is attempted outside the volume.
 * @return const T &
 */
template <class L, class T>
const T & Volume<L,T>::GetBackground() const
{
  return(m_data.GetBackground());
}


/**
 * Forces the given compression type for file storage.
 * @ compressType the type of compression to be enforced
 */
template <class L, class T>
void Volume<L,T>::SetCompression(hdr::FileCompressionEnum compressType)
{
  m_data.SetCompression(compressType);
}


/**
 * Returns the current compression, normally m_bestCompression. When a file is
 * read the compression type is always set to the value of m_bestCompression as
 * specified in VolAPI.
 * @return the current compression.
 */
template <class L, class T>
hdr::FileCompressionEnum Volume<L,T>::GetCompression() const
{
  return(m_data.GetCompression());
}


/**
 * Returns the extension of the volume as read from the file header.
 * @return hdr::ExtensionEnum
 */
template <class L, class T>
hdr::ExtensionEnum Volume<L,T>::GetExtension() const
{
  return(m_data.GetExtension());
}


/**
 * Sets the author/source string in the volume header.
 * @param sAuthor Volume author to be set
 */
template <class L, class T>
void Volume<L,T>::SetAuthor(const std::string & sAuthor)
{
  m_data.SetAuthor(sAuthor);
}


/**
 * Returns the author/source string in the volume header.
 * @return std::string
 */
template <class L, class T>
const std::string & Volume<L,T>::GetAuthor() const
{
  return(m_data.GetAuthor());
}


/**
 * Sets the volume title.
 * @param sTitle Volume title to be set
 */
template <class L, class T>
void Volume<L,T>::SetTitle(const std::string & sTitle)
{
  m_data.SetTitle(sTitle);
}


/**
 * Returns the title of the volume.
 * @return const std::string &
 */
template <class L, class T>
const std::string & Volume<L,T>::GetTitle() const
{
  return(m_data.GetTitle());
}

/**
 * Sets the creation date for the volume.
 * @param date on which the file is being created.
 */
template <class L, class T>
inline void Volume<L,T>::SetCreationDate(vx::Date d)
{
  m_data.SetCreationDate(d);
}


/**
 * Returns the creation date of the volume.
 * @return const Date &
 */
template <class L, class T>
inline const vx::Date & Volume<L,T>::GetCreationDate() const
{
  return(GetCreationDate());
}


/**
 * Returns the unitType of the stored volume such as METER, MICRON, etc.
 * @return hdr::UnitTypeEnum
 */
template <class L, class T>
hdr::UnitTypeEnum Volume<L,T>::GetUnitType() const
{
  return(m_data.GetUnitType());
}


/**
 * Returns the x,y,z voxel dimensions of the volume.
 * @return const Triple<uuint> &
 */
template <class L, class T>
const Triple<uint4> & Volume<L,T>::GetDim() const
{
  return(m_data.GetDim());
}


/**
 * Returns the x,y,z units of the volume.
 * @return const Triple<float> &
 */
template <class L, class T>
const Triple<float4> & Volume<L,T>::GetUnits() const
{
  return(m_data.GetUnits());
}


/**
 * Sets the x,y,z units.
 * @param units the new units.
 */
template <class L, class T>
void Volume<L,T>::SetUnits(const Triple<float4> & units)
{
  m_data.SetUnits(units);
}


/**
 * Returns the number of significant bits per voxel.
 * @return const uint
 */
template <class L, class T>
const uint4 Volume<L,T>::GetBitsPerVoxel() const
{
  return(m_data.GetBitsPerVoxel());
}


/**
 * Returns the bytes per voxel.
 * @return const uint
 */
template <class L, class T>
const uint4 Volume<L,T>::GetBytesPerVoxel() const
{
  return(m_data.GetBytesPerVoxel());
}


/**
 * Returns the total memory usage in bytes.
 * @return the number of bytes used including data and all overhead.
 */
template <class L, class T>
const uint8 Volume<L,T>::GetMemoryUsage() const
{
  return(m_data.GetMemoryUsage());
}


/**
 * Reports relevant volume layout statistics.
 * @param os output stream
 */

template <class L, class T>
void Volume<L,T>::Stats(std::ostream & os ) const
{
  m_data.Stats(os);
}


/**
 * Returns the Filename string.
 * @return the name of the file.
 */
template <class L, class T>
const std::string & Volume<L,T>::GetFileName() const
{
  return(m_data.GetFileName());
}


/**
 * Returns the Filename string without the extension.
 * @return the file name root.
 */
template <class L, class T>
std::string Volume<L,T>::GetFileNameRoot() const
{
  return(m_data.GetFileNameRoot());
}


/**
 * Sets the file name.
 * @param sFileName the file name.
 */
template <class L, class T>
void Volume<L,T>::SetFileName(const std::string & sFileName)
{
  m_data.SetFileName(sFileName);
}


/**
 * Returns a matrix for converting from volume space into world space.
 * @return const Matrix<float>
 */
template <class L, class T>
const Matrix<float4> Volume<L,T>::GetVolumeToWorldMatrix() const
{
  return(m_data.GetVolumeToWorldMatrix());
}


/**
 * Returns a matrix for converting from world space into volume space.
 * @return const Matrix<float>
 */
template <class L, class T>
const Matrix<float4> Volume<L,T>::GetWorldToVolumeMatrix() const
{
  return(m_data.GetWorldToVolumeMatrix());
}


/**
 * Returns true if the point lies within the volume.
 * @param p location to test if it is inside
 * @return bool
 */
template <class L, class T>
bool Volume<L,T>::InsideVolume(const Point<uint2> & p) const
{
  return(m_data.InsideVolume(p));
}


/**
 * Prints all file header variables to the ostream.
 * @param os std::ostream &
 */
template <class L, class T>
std::ostream & Volume<L, T>::AsOstream (std::ostream & os) const
{
  return(m_data.AsOstream(os));
}


/**
 * Returns the number of voxels.
 * @return the voxels
 */
template <class L, class T>
const uint4 Volume<L,T>::GetNumberOfVoxels() const
{
  return(m_data.GetNumberOfVoxels());
}


/**
 * Returns the number of non-zero voxels
 */
template <class L, class T>
const uint4 Volume<L,T>::GetNumberOfNonZeroVoxels() 
{
  uint4 count = 0;
  typename L::VolIterator iterB(GetVolIterator());
  for ( ; !iterB.End(); iterB.NextNonZero() )
  {
    ++count;
  }
  return(count);
}


/**
 * Count how many times a particular value appears in the volume.
 */
template <class L, class T>
uint4 Volume<L,T>::CountInstancesOfValue(const T & value)
{
  if ( value != m_data.GetBackground() )
  {
    uint4 count = 0;
    typename L::VolIterator iterB(GetVolIterator());
    for ( ; !iterB.End(); iterB.NextNonZero() )
    {
      if ( iterB.GetVoxel() == value )
        ++count;
    }
    return(count);
  } else
  {
    return(GetNumberOfVoxels() - GetNumberOfNonZeroVoxels()); 
  }
} // CountInstancesOfValue


/**
 * Changes the reference frame. Basically stores the voxels in a different order,
 * so that the slices in X, Y, Z direction can be in a contiguous array of memory
 * for image display.
 * The initial volume is stored in zyx order and is hence the Transverse slices
 * is in contiguous array of memory.
 * For: Transverse to Coronal:
 * Applies the transformation matrix to the whole LinearVolume from (X)(Y)(Z)
 * co-ordinate system to (X)(Z)(Y)
 * The transformation matrix is:
 * |    1     0     0     0    |
 * |    0     0     1     0    |
 * |    0     1     0     0    |
 * |    0     0     0     1    |
 * For: Transverse to Saggittal:
 * Applies the transformation matrix to the whole LinearVolume from (X)(Y)(Z)
 * co-ordinate system to (Z)(Y)(X).
 * The transformation matrix is:
 * |    0     0     1     0    |
 * |    0     1     0     0    |
 * |    1     0     0     0    |
 * |    0     0     0     1    |
 * For: Coronal to Saggittal:
 * Applies the transformation matrix to the whole LinearVolume from (X)(Z)(Y)
 * co-ordinate system to (Z)(Y)(X). This implies that the Transverse volume
 * was converted to Coronal and now is being transformed for Saggittal view.
 * The transformation matrix is:
 * |    0     1     0     0    |
 * |    0     0     1     0    |
 * |    1     0     0     0    |
 * |    0     0     0     1    |
 * @param xIter 3D iterator to traverse in X direction
 * @param yIter 3D iterator to traverse in Y direction
 * @param zIter 3D iterator to traverse in Z direction
 * @param view ViewEnum (Coronal/Saggittal/Transverse)
 */
template <class L, class T>
void Volume<L,T>::PermuteCoordinates(hdr::PermuteCoordinatesEnum view)
{
  Triple<uint4> oldDim(m_data.GetDim());
  Triple<float4> oldUnits(m_data.GetUnits());

  switch ( view )
  {
  case hdr::TRANSVERSE_TO_CORONAL:
    {
      Triple<uint4>newDim(oldDim.m_x, oldDim.m_z, oldDim.m_y);
      Triple<float4>newUnits(oldUnits.m_x, oldUnits.m_z, oldUnits.m_y);
      // creates a copy of the volume with new dimension and units
      L newVol(newDim, m_data.GetBitsPerVoxel(), newUnits,
               m_data.GetUnitType(), m_data.GetBackground(),
               m_data.GetExtension(), m_data.GetFileName(),
               m_data.GetTitle(), m_data.GetAuthor());
      // Transforming from Reference frame
      for ( uint2 z = 0; z < newDim.m_z; z++ )
      {
        for ( uint2 y = 0; y < newDim.m_y; y++ )
        {
          for ( uint2 x = 0; x < newDim.m_x; x++ )
          {
            newVol.SetVoxelRaw(Point<uint2>(x,(newDim.m_y-1)-y,z), m_data.GetVoxelRaw(Point<uint2>(x,z,y)));
          }
        }
      }
      // assign the new data
      m_data = newVol;
      // icon data would change when it is written next time
      break;
    }
  case hdr::TRANSVERSE_TO_SAGGITTAL:
    {
      Triple<uint4>newDim(oldDim.m_z, oldDim.m_y, oldDim.m_x);
      Triple<float4>newUnits(oldUnits.m_z, oldUnits.m_y, oldUnits.m_x);
      L newVol(newDim, m_data.GetBitsPerVoxel(), newUnits,
               m_data.GetUnitType(), m_data.GetBackground(),
               m_data.GetExtension(), m_data.GetFileName(),
               m_data.GetTitle(), m_data.GetAuthor());
      // Transforming from Reference frame
      for ( uint2 z = 0; z < oldDim.m_z; z++ )
      {
        for ( uint2 y = 0; y < oldDim.m_y; y++ )
        {
          for ( uint2 x = 0; x < oldDim.m_x; x++ )
          {
            newVol.SetVoxelRaw(Point<uint2>(x,y,z), m_data.GetVoxelRaw(Point<uint2>(z,y,x)));
          }
        }
      }
      // assign the new data
      m_data = newVol;
      // icon data would change when it is written next time
      break;
    }
  case hdr::CORONAL_TO_SAGGITTAL:
    {
      // a coronal transform has been applied
      // the entire transform would be XYZ -> XZY -> ZYX
      Triple<uint4>newDim(oldDim.m_y, oldDim.m_z, oldDim.m_x);
      if ( newDim != oldDim )
      {
        throw ex::VException(LogRec("newDim != oldDim","Volume::L::T","CountInstancesOfValue"));
      }
      Triple<float4>newUnits(oldUnits.m_x, oldUnits.m_y, oldUnits.m_z);
      L newVol(newDim, m_data.GetBitsPerVoxel(), newUnits,
               m_data.GetUnitType(), m_data.GetBackground(),
               m_data.GetExtension(), m_data.GetFileName(),
               m_data.GetTitle(), m_data.GetAuthor());
      // Transforming from Reference frame
      for ( uint2 z = 0; z < newDim.m_z; z++ )
      {
        for ( uint2 y = 0; y < newDim.m_y; y++ )
        {
          for ( uint2 x = 0; x < newDim.m_x; x++ )
          {
            newVol.SetVoxelRaw(Point<uint2>(y,z,x), m_data.GetVoxelRaw(Point<uint2>(x,y,z)));
          }
        }
      }
      // assign the new data
      m_data = newVol;
      // icon data would change when it is written next time
      break;
    }
  default:
    LogDbg("VolAPI<T>::ApplyTransform: ViewType not supported.","Volume::L::T","PermuteCoordinates");
    break;
  }
} // PermuteCoordinates


/**
 * Fits the volume according to the Resampling type specified by the enum
 *   onto the volume given as argument; then volume data will be
 *   stretched/shrunk accordingly
 * @param dimension Triple<uint> &
 * @param resampleType hdr::ResampleTypeEnum
 */
// VC++ chokes if a defult parameter is given in a template fn.
template <class L, class T>
void Volume<L,T>::Fit(Volume<L,T> & destVol,
                      hdr::ResampleTypeEnum resampleType) const
{
  Triple<uint4>  oldDim(m_data.GetDim());
  Triple<uint4>  newDim(destVol.m_data.GetDim());
  Triple<float4> oldUnits(m_data.GetUnits());
  Triple<float4> scale(oldDim.m_x / float4(newDim.m_x),
                       oldDim.m_y / float4(newDim.m_y),
                       oldDim.m_z / float4(newDim.m_z));

  Triple<float4> newUnits(oldUnits.m_x * scale.m_x,
                          oldUnits.m_y * scale.m_y,
                          oldUnits.m_z * scale.m_z);

  destVol.SetUnits(newUnits);

  float4 onePercent( (newDim.m_z-1)/100.0 );

  switch ( resampleType )
  {
  case hdr::POINT: {
      for ( uint2 zDim=0; zDim < newDim.m_z; ++zDim )
      {
        for ( uint2 yDim=0; yDim < newDim.m_y; ++yDim )
        {
          for ( uint2 xDim=0; xDim < newDim.m_x; ++xDim )
          {
            Point<float4> targetVoxel (((float4(xDim)+0.5)*scale.m_x - 0.5),
                                       ((float4(yDim)+0.5)*scale.m_y - 0.5),
                                       ((float4(zDim)+0.5)*scale.m_z - 0.5));
            // take the voxel value at targetVoxel (with x,y,z, rounded off)
            targetVoxel = Point<float4>((targetVoxel.m_x + 0.5),
                                        (targetVoxel.m_y + 0.5),
                                        (targetVoxel.m_z + 0.5));
            Point<uint2>targetVoxelRounded;
            targetVoxelRounded = targetVoxel;
            destVol.SetVoxelRaw(Point<uint2>(xDim,yDim,zDim),
                                (m_data.GetVoxelRaw(targetVoxelRounded)));
          } // for xDim
        } // for yDim
        //ProgressPrinter::ShowProgress(true, zDim, onePercent);
      } // for zDim
      break;
    } // case hdr::POINT
  case hdr::TRILIN: {
      for ( uint2 zDim=0; zDim < newDim.m_z; ++zDim )
      {
        for ( uint2 yDim=0; yDim < newDim.m_y; ++yDim )
        {
          for ( uint2 xDim=0; xDim < newDim.m_x; ++xDim )
          {

            Point<float4> targetVoxel (((float4(xDim)+0.5)*scale.m_x - 0.5),
                                       ((float4(yDim)+0.5)*scale.m_y - 0.5),
                                       ((float4(zDim)+0.5)*scale.m_z - 0.5));
            // resample the voxel value at targetVoxel
            destVol.SetVoxelRaw(Point<uint2> (xDim,yDim,zDim),
                                GetResampledValue(targetVoxel));
          } // for xDim
        } // for yDim
        //ProgressPrinter::ShowProgress(true, zDim, onePercent);
      } // for zDim
      break;
    } // case hdr::TRILIN
  case hdr::MAJORITY: {
      scale = Triple<float4>((oldDim.m_x-1) / float4(newDim.m_x),
                             (oldDim.m_y-1) / float4(newDim.m_y),
                             (oldDim.m_z-1) / float4(newDim.m_z));

			uint4 uXB(newDim.m_x-1);
			uint4 uYB(newDim.m_y-1);
			uint4 uZB(newDim.m_z-1);
			for ( uint4 zDim=1; zDim < uZB; zDim++ )
      {
        for ( uint4 yDim=1; yDim < uYB; yDim++ )
        {
          for ( uint4 xDim=1; xDim < uXB; xDim++ )
          {
            Point<float4> targetSample(xDim*scale.m_x,
                                       yDim*scale.m_y,
                                       zDim*scale.m_z);
						// check the overflow of size is necessary: Dongqing
						if( targetSample.m_x >= oldDim.m_x-1 ) targetSample.m_x = oldDim.m_x-2; 
						if( targetSample.m_y >= oldDim.m_y-1 ) targetSample.m_y = oldDim.m_y-2; 
						if( targetSample.m_z >= oldDim.m_z-1 ) targetSample.m_z = oldDim.m_z-2; 
						if( targetSample.m_x <= 0 ) targetSample.m_x = 1; 
						if( targetSample.m_y <= 0 ) targetSample.m_y = 1; 
						if( targetSample.m_z <= 0 ) targetSample.m_z = 1;

            // take majority voxel from surrounding
            Point<uint2> targetVoxel;
            targetVoxel = targetSample;
            std::vector<T> vox(9);
						vox[0] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_x = targetVoxel.m_x+1;
						vox[1] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_y = targetVoxel.m_y+1;
						vox[2] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_x = targetVoxel.m_x-1;
						vox[3] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_x = targetVoxel.m_x-1;
						vox[4] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_y = targetVoxel.m_y-1;
						vox[5] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_y = targetVoxel.m_y-1;
						vox[6] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_x = targetVoxel.m_x+1;
						vox[7] = m_data.GetVoxelRaw(targetVoxel); targetVoxel.m_x = targetVoxel.m_x+1;
						vox[8] = m_data.GetVoxelRaw(targetVoxel);

            destVol.SetVoxelRaw(Point<uint2>(xDim,yDim,zDim), Majority(vox));
          } // for xDim
        } // for yDim
        //ProgressPrinter::ShowProgress(true, zDim, onePercent);
      } // for zDim
      break;
    } // case hdr::MAJORITY
  case hdr::BOX:
  case hdr::TENT:
  case hdr::GAUSS:
  case hdr::WAVELET:
  default:
  {
    throw ex::VException(LogRec("Resample type not yet Suppoprted","Volume","Fit"));
  }
  } // switch
} // Fit


/**
 * Resize the volume 
 * @param dimension Triple<uint> & 
 */
 // VC++ chokes if a defult parameter is given in a template fn.
 template <class L, class T>
   void Volume<L,T>::Resize(const Triple<uint4> & dimension)
 {   
    m_data.Resize(dimension);
 } 




/**
 * Resize the volume according to the Resampling type specified by the enum.
 * @param dimension Triple<uint> &
 * @param resampleType hdr::ResampleTypeEnum
 */
 // VC++ chokes if a defult parameter is given in a template fn.
 template <class L, class T>
   void Volume<L,T>::Resample(const Triple<uint4> & dimension, hdr::ResampleTypeEnum resampleType)
 {
     Volume<L,T> newVol(dimension, m_data.GetBitsPerVoxel(), m_data.GetUnits(),
       m_data.GetUnitType(), m_data.GetBackground(),
       m_data.GetExtension(), m_data.GetFileName(),
       m_data.GetTitle(), m_data.GetAuthor());
     
     // now resize the volume data
     Fit(newVol, resampleType);
     
     // assign the new data
     m_data = newVol.m_data;
 } 



/**
* Crops the volume according to the minimum and maximum 
* positions to it's smallest.
*/
template <class L, class T>
void Volume<L,T>::Crop()
{
  Triple<uint4> oldDim(m_data.GetDim());
  Point<int2> minPos; minPos.Cast(oldDim);
  Point<int2> maxPos(0,0,0);
  BlockVolIterUC iter(GetVolIterator());
  for (iter.NextNonZero(); !iter.End(); iter.NextNonZero() )
  {
    if ( minPos.m_x > iter.GetPos().m_x ) minPos.m_x = iter.GetPos().m_x;
    if ( minPos.m_y > iter.GetPos().m_y ) minPos.m_y = iter.GetPos().m_y;
    if ( minPos.m_z > iter.GetPos().m_z ) minPos.m_z = iter.GetPos().m_z;
    if ( maxPos.m_x < iter.GetPos().m_x ) maxPos.m_x = iter.GetPos().m_x;
    if ( maxPos.m_y < iter.GetPos().m_y ) maxPos.m_y = iter.GetPos().m_y;
    if ( maxPos.m_z < iter.GetPos().m_z ) maxPos.m_z = iter.GetPos().m_z;
  }
  Vector<int2> one(1,1,1);
  Triple<int2> newDim(maxPos-minPos+one);
  Point<int2> offset(minPos);

  Volume<L,T> newVol(newDim, m_data.GetBitsPerVoxel(), m_data.GetUnits(),
                     m_data.GetUnitType(), m_data.GetBackground(),
                     m_data.GetExtension(), m_data.GetFileName(),
                     m_data.GetTitle(), m_data.GetAuthor());
  // now copy the non-background volume data
  for ( iter.MoveAbs(offset) ; !iter.End(); iter.NextNonZero() )
  {
    newVol.SetVoxel(iter.GetPos()-offset, iter.GetVoxelRaw());
  }

  // assign the new data
  m_data = newVol.m_data;
}


/**
 * Computes and returns the resampled value at the location specified: boundary checking
 * @param point Point<float> &
 */
template <class L, class T>
float4 Volume<L, T>::GetResampledValue(const Point<float4> & point) const
{
  // truncate
  uint4 newx = uint4(point.m_x);
  uint4 newy = uint4(point.m_y);
  uint4 newz = uint4(point.m_z);
  Point<uint2> p(newx, newy, newz);
  
  // weights for tri-linear interpolation
  float4 weightX = point.m_x - newx;
  float4 weightY = point.m_y - newy;
  float4 weightZ = point.m_z - newz;
  
  // eight nearest neighbor voxels
  T llf(GetVoxel(p));  p.m_y++;
  T ulf(GetVoxel(p));  p.m_x++;
  T urf(GetVoxel(p));  p.m_y--;
  T lrf(GetVoxel(p));  p.m_z++;
  T lrb(GetVoxel(p));  p.m_y++;
  T urb(GetVoxel(p));  p.m_x--;
  T ulb(GetVoxel(p));  p.m_y--;
  T llb(GetVoxel(p));
  
  // tri-linear interpolation
  const float4 fOneMinusWeightZ(1 - weightZ);
  const float4 fOneMinusWeightY(1 - weightY);
  const float4 fOneMinusWeightX(1 - weightX);
  const float4 llm = llb * weightZ + llf * fOneMinusWeightZ;
  const float4 ulm = ulb * weightZ + ulf * fOneMinusWeightZ;
  const float4 lrm = lrb * weightZ + lrf * fOneMinusWeightZ;
  const float4 urm = urb * weightZ + urf * fOneMinusWeightZ;
  const float4 mlm = ulm * weightY + llm * fOneMinusWeightY;
  const float4 mrm = urm * weightY + lrm * fOneMinusWeightY;
  const float4 mmm = mrm * weightX + mlm * fOneMinusWeightX;
  
  return(mmm);
} // GetResampledValue



/**
 * Computes and returns the resampled value at the location specified
 * @param point Point<float> &
 */
template <class L, class T>
float4 Volume<L, T>::GetResampledValueRaw(const Point<float4> & point) const
{
  // truncate
  uint4 newx = uint4(point.m_x);
  uint4 newy = uint4(point.m_y);
  uint4 newz = uint4(point.m_z);
  Point<uint2> p(newx, newy, newz);

  // weights for tri-linear interpolation
  float4 weightX = point.m_x - newx;
  float4 weightY = point.m_y - newy;
  float4 weightZ = point.m_z - newz;

  // eight nearest neighbor voxels
  T llf(GetVoxelRaw(p));  p.m_y++;
  T ulf(GetVoxelRaw(p));  p.m_x++;
  T urf(GetVoxelRaw(p));  p.m_y--;
  T lrf(GetVoxelRaw(p));  p.m_z++;
  T lrb(GetVoxelRaw(p));  p.m_y++;
  T urb(GetVoxelRaw(p));  p.m_x--;
  T ulb(GetVoxelRaw(p));  p.m_y--;
  T llb(GetVoxelRaw(p));

  // tri-linear interpolation
  const float4 fOneMinusWeightZ(1 - weightZ);
  const float4 fOneMinusWeightY(1 - weightY);
  const float4 fOneMinusWeightX(1 - weightX);
  const float4 llm = llb * weightZ + llf * fOneMinusWeightZ;
  const float4 ulm = ulb * weightZ + ulf * fOneMinusWeightZ;
  const float4 lrm = lrb * weightZ + lrf * fOneMinusWeightZ;
  const float4 urm = urb * weightZ + urf * fOneMinusWeightZ;
  const float4 mlm = ulm * weightY + llm * fOneMinusWeightY;
  const float4 mrm = urm * weightY + lrm * fOneMinusWeightY;
  const float4 mmm = mrm * weightX + mlm * fOneMinusWeightX;

  return(mmm);
} // GetResampledValueRaw

/**
 * Returns the header map for setting in other volumes.
 */
template <class L, class T>
const VarMap & Volume<L, T>::GetHdrMap() const
{
  return(m_data.GetHdrMap());
}


/**
 * Sets the header map, should only be used in conjunction with GetHdrMap().
 */
template <class L, class T>
void Volume<L, T>::SetHdrMap(const VarMap & newMap)
{
  m_data.SetHdrMap(newMap);
}


/**
 * Prints all file header variables to the ostream.
 * @param os std::ostream &
 * @return true if the
 */
template <class L, class T>
bool Volume<L, T>::TestByType(std::ostream & os)
{
  bool ok(true);

  os << "BEGINNING VOLUME INTERFACE CLASS TEST"<< std::endl;

  string author = "Brett Ashley";
  string title1 = "The Sun Also Rises";
  string fileName = "TheTest.vol";
  Triple<uint4> dim(50,50,50);
  Triple<float4> units(1.0,1.0,1.0);
  uint4 bitSize = 8*sizeof(T);

  Volume<L,T> apiTest(dim, bitSize, units,
                      hdr::METER, 0, hdr::VOL, fileName, title1, author);

  apiTest.SetCompression(hdr::ZLIB);
  ok = ok && (apiTest.GetTitle() == title1);
  apiTest.SetTitle("something new");
  ok = ok && (apiTest.GetTitle() == "something new");
  ok = ok && (apiTest.GetAuthor() == author);
  ok = ok && (apiTest.GetUnitType() == hdr::METER);
  ok = ok && (apiTest.GetDim() == dim);
  ok = ok && (apiTest.GetUnits() == units);
  // ok = ok && (apiTest.GetBitsPerVoxel() == sizeof(T) * 8);
  ok = ok && (apiTest.GetBytesPerVoxel() == sizeof(T));
  ok = ok && (apiTest.GetFileName() == fileName);
  ok = ok && (apiTest.GetFileNameRoot() == "TheTest");

  ok = ok && (apiTest.InsideVolume(Point<uint2>(dim.m_x, dim.m_y, dim.m_z)) == false);
  Point<uint2> inside(20,13,10);
  ok = ok && (apiTest.InsideVolume(inside) == true);
  apiTest.AsOstream(os);
  uint8 realNumber = dim.m_x * dim.m_y * dim.m_z;
  ok = ok && (apiTest.GetNumberOfVoxels() == realNumber);


  os << "OK:"<< ok<< " Testing SetVoxel(), GetVoxel() ..."<<std::endl;

  Volume<L,T> vol(dim, bitSize, units,
                  hdr::METER, 0, hdr::VOL, "Test.vol", title1, author);

  vol.SetVoxel(Point<uint2>(0,0,0),(T) 90);
  vol.SetVoxel(Point<uint2>(49,49,49),(T) 91);
  vol.SetVoxel(Point<uint2>(4,4,4),(T) 92);
  vol.SetVoxel(Point<uint2>(4,4,5),(T) 93);
  vol.SetVoxel(Point<uint2>(4,5,4),(T) 94);
  vol.SetVoxel(Point<uint2>(4,5,5),(T) 95);
  vol.SetVoxel(Point<uint2>(5,4,4),(T) 96);
  vol.SetVoxel(Point<uint2>(5,4,5),(T) 97);
  vol.SetVoxel(Point<uint2>(5,5,4),(T) 98);
  vol.SetVoxel(Point<uint2>(5,5,5),(T) 99);

  ok = ok && ((T)90 == vol.GetVoxel(Point<uint2>(0,0,0)));
  ok = ok && ((T)91 == vol.GetVoxel(Point<uint2>(49,49,49)));
  ok = ok && ((T)92 == vol.GetVoxel(Point<uint2>(4,4,4)));
  ok = ok && ((T)93 == vol.GetVoxel(Point<uint2>(4,4,5)));
  ok = ok && ((T)94 == vol.GetVoxel(Point<uint2>(4,5,4)));
  ok = ok && ((T)95 == vol.GetVoxel(Point<uint2>(4,5,5)));
  ok = ok && ((T)96 == vol.GetVoxel(Point<uint2>(5,4,4)));
  ok = ok && ((T)97 == vol.GetVoxel(Point<uint2>(5,4,5)));
  ok = ok && ((T)98 == vol.GetVoxel(Point<uint2>(5,5,4)));
  ok = ok && ((T)99 == vol.GetVoxel(Point<uint2>(5,5,5)));

  vol.Write("TheTest.vol");

  vol.SetVoxel(Point<uint2>(0,0,0), (T)0);
  vol.SetVoxel(Point<uint2>(49,49,49), (T)0);
  vol.SetVoxel(Point<uint2>(4,4,4), (T)0);
  vol.SetVoxel(Point<uint2>(4,4,5), (T)0);
  vol.SetVoxel(Point<uint2>(4,5,4), (T)0);
  vol.SetVoxel(Point<uint2>(4,5,5), (T)0);
  vol.SetVoxel(Point<uint2>(5,4,4), (T)0);
  vol.SetVoxel(Point<uint2>(5,4,5), (T)0);
  vol.SetVoxel(Point<uint2>(5,5,4), (T)0);
  vol.SetVoxel(Point<uint2>(5,5,5), (T)0);

  vol.Read("TheTest.vol");

  ok = ok && ((T)90 == vol.GetVoxel(Point<uint2>(0,0,0)));
  ok = ok && ((T)91 == vol.GetVoxel(Point<uint2>(49,49,49)));
  ok = ok && ((T)92 == vol.GetVoxel(Point<uint2>(4,4,4)));
  ok = ok && ((T)93 == vol.GetVoxel(Point<uint2>(4,4,5)));
  ok = ok && ((T)94 == vol.GetVoxel(Point<uint2>(4,5,4)));
  ok = ok && ((T)95 == vol.GetVoxel(Point<uint2>(4,5,5)));
  ok = ok && ((T)96 == vol.GetVoxel(Point<uint2>(5,4,4)));
  ok = ok && ((T)97 == vol.GetVoxel(Point<uint2>(5,4,5)));
  ok = ok && ((T)98 == vol.GetVoxel(Point<uint2>(5,5,4)));
  ok = ok && ((T)99 == vol.GetVoxel(Point<uint2>(5,5,5)));

  os << "OK:"<< ok<< " Testing copy, assignment ..."<<std::endl;

  string oldTitle = vol.GetTitle();
  Volume<L,T> volCopy = vol;

  ok = ok && ((T)98 == volCopy.GetVoxel(Point<uint2>(5,5,4)));
  ok = ok && ((T)99 == volCopy.GetVoxel(Point<uint2>(5,5,5)));
  ok = ok && (volCopy.GetTitle() == oldTitle);

  Volume<L,T> volCopy2(volCopy);
  ok = ok && ((T)98 == volCopy.GetVoxel(Point<uint2>(5,5,4)));
  ok = ok && ((T)99 == volCopy.GetVoxel(Point<uint2>(5,5,5)));
  ok = ok && (volCopy.GetTitle() == oldTitle);

  vol.Stats(os);
  vol.GetMemoryUsage();

  os << "OK:"<<ok << " Testing TransformCoordinates(hdr::TRANSVERSE_TO_CORONAL)" << std::endl;
  volCopy = vol; // copy
  dim = vol.GetDim();
  units = vol.GetUnits();
  vol.PermuteCoordinates(hdr::TRANSVERSE_TO_CORONAL);
  ok = ok && (vol.GetDim() == Triple<uint4>(dim.m_x, dim.m_z, dim.m_y));

  os << "OK:"<<ok << " Testing TransformCoordinates(hdr::CORONAL_TO_SAGGITTAL)" << std::endl;
  vol.PermuteCoordinates(hdr::CORONAL_TO_SAGGITTAL);
  ok = ok && (vol.GetDim() == Triple<uint4>(dim.m_z, dim.m_y, dim.m_x));

  os << "OK:"<<ok << " Testing generic function Resize" << std::endl;
  vol = volCopy;
  dim = vol.GetDim();
  units = vol.GetUnits();
  dim = Triple<uint4>(dim.m_x/2, dim.m_y/2, dim.m_z/2);
  units = Triple<float4>(units.m_x*2, units.m_y*2, units.m_z*2);
  vol.Resample(dim, hdr::TRILIN);
  ok = ok && (dim == vol.GetDim());
  ok = ok && (units == vol.GetUnits());

  os << "OK:"<< ok<< " Testing 3-D volume iterator" << std::endl;

  // write some code here

/*
    os << "OK:"<< ok<< " Testing Dilator" << std::endl;
    {

      os << "Traversing the Dilator with seed in the Volume" << std::endl;
      Volume<LinearVolume<ushort>,ushort> * dv
      = new Volume<LinearVolume<ushort>,ushort>(Triple<uint>(21,21,21), Triple<float>(1,1,1),
                                                hdr::VX_USHORT, 8*sizeof(ushort), hdr::METER, hdr::VOL,
                                                "DilatorTest.vol", "Test Dilator","Made by Viatronix");
      const Point<ushort>seed(10,10,10);
      Volume<LinearVolume<ushort>,ushort>::Dilator grow = dv->GetDilator(seed);
      for ( uint count = 0; count < 344; count++ ) {
        grow.SetVoxel(255);
        grow++;
      }

      grow = dv->GetDilator(seed);
      for ( uint count1 = 0; count1 < 344; count1++ ) {
        ushort v = *grow;
        ok = ok && (v == 255);
        if ( count1%2 == 0 ) os << std::endl;
        os << "Location " << grow.GetLocation() << " Value " << v << " " ;
        grow++;
      }
      delete dv;

      os << std::endl << "Checked dilator values with those set: " << "OK:"<<ok << std::endl;
    }
    {
      os << "Traversing the Dilator with seed at one corner: tests for boundary condition" << std::endl;
      Volume<LinearVolume<ushort>,ushort> * dv
      = new Volume<LinearVolume<ushort>,ushort>(Triple<uint>(21,21,21), Triple<float>(1,1,1),
                                                hdr::VX_USHORT, 8*sizeof(ushort), hdr::METER, hdr::VOL,
                                                "DilatorTest.vol", "Test Dilator","Made by Viatronix");
      const Point<ushort>seed(1,1,1);  // to test for boundary checking
      Volume<LinearVolume<ushort>,ushort>::Dilator grow = dv->GetDilator(seed);
      for ( uint count = 0; count < 344; count++ ) {
        grow.SetVoxel(255);
        grow++;
      }

      grow = dv->GetDilator(seed);
      for ( uint count1 = 0; count1 < 344; count1++ ) {
        ushort v = *grow;
        ok = ok && (v == 255);
        if ( count1%2 == 0 ) os << std::endl;
        os << "Location " << grow.GetLocation() << " Value " << v << " " ;
        grow++;
      }
      delete dv;
      os << std::endl << "Checked dilator values with those set: " << "OK:"<<ok << std::endl;
    }
    */

  if ( ok ) os <<"COMPLETED VOLUME: Volume<L,T> CLASS TEST :)"<< std::endl;
  else os <<" FAILED VOLUME: Volume<L,T> CLASS TEST :("<< std::endl;

  return(ok);
} // TestByType


/**
 * Builds the 3D icon for the volume of the layout specified in the parameter.
 * Set the boolean parameter in the Write() function of the corresponding Volume.
 * to build an icon and write in the Image Data chunk.
 * @param layout MemoryLayoutEnum
 */
//template <class L, class T>
//void Volume<L, T>::BuildIcon()
//{
//  uint2 chartHeight = 40;
//  m_data.m_icon = PNGImage(2*GetDim().m_y, 5*chartHeight + 3*GetDim().m_z/2);
//
//  uint2 maxBuckets = m_data.m_icon.GetWidth();
//  //uint2 stepSize = Max(iterB.GetDim().m_x, iterB.GetDim().m_y, iterB.GetDim().m_z);
//  //stepSize = Max(1, stepSize/128);
//  //iterB.SetBiDirectionalStepSize(stepSize);
//  Histogram<T> volHistogram(GetVolIterator(), maxBuckets, true);
//
//  // For float/double data type the number of buckets is 512, SPECIALIZE
//  // For more number of buckets use Histogram Constructor instead of VolHistogram
//  uint2 alphaLLim(0), alphaHLim(0);
//
//  float4 maxGradientMagnitude = 0.7 * (volHistogram.GetMaxDataValue() - volHistogram.GetMinDataValue());
//  // Find the limits for the alpha ramp
//  FindAlphaLimits( volHistogram, alphaLLim, alphaHLim);
//  // Finally Ray Cast
//  RayCast(volHistogram, m_data.m_icon, alphaLLim, alphaHLim, maxGradientMagnitude);
//} // BuildIcon


/**
 * Finds the low/high limits for the alpha function for BuildIcon().
 * @param volHistogram Histogram &
 * @param alphalow unsigned long &
 * @param alphahigh unsigned long &
 */
template <class L, class T>
void Volume<L, T>::FindAlphaLimits(Histogram<T> & volHistogram,
                                   uint2 & alphaLLimBin,
                                   uint2 & alphaHLimBin)
{
  uint2 bin(0);
  uint2 backgroundBin = volHistogram.GetBin(GetBackground());
  uint4 numVoxelsBright(0), voxelCount(0);


  // find number of non-first & non-background voxels
  for ( bin=1; bin<volHistogram.GetNumOfBins(); ++bin )
  {
    if ( bin != backgroundBin )
    {
      numVoxelsBright += volHistogram.GetCountAt(bin);
    }
  } // for all histogram bins

  // set alpha ramp limits
  uint4 countLLim(static_cast<uint4>(0.20*numVoxelsBright));
  uint4 countHLim(static_cast<uint4>(0.40*numVoxelsBright));
  alphaLLimBin = alphaHLimBin = 1;

  // find the corresponding histogram bins
  for ( bin=1, voxelCount=0;
      voxelCount <= countHLim && bin < volHistogram.GetNumOfBins();
      ++bin )
  {
    if ( bin != backgroundBin )
    {
      voxelCount += volHistogram.GetCountAt(bin);
      if ( voxelCount <= countLLim )
      {
        alphaLLimBin = bin;
      }
      alphaHLimBin = bin;
    }
  } // for all histogram bins below scaleHLimBin

} // FindAlphaLimits


template <class L, class T>
inline std::ostream & operator<<(std::ostream &os, const Volume<L, T> & volume)
{
  volume.AsOstream(os);
  return(os);
} // AsOstream


/**
 *
 */
template <class L, class T>
void Volume<L, T>::PrintAllRaw(std::ostream & os, int4 width) const
{
  os << "volume dump by PrintAllRaw: with size " << m_data.GetDim() << endl;
  for ( int2 y=m_data.GetDim().m_y-1;  y >= 0; --y )
  {
    for ( uint2 z=0;  z<m_data.GetDim().m_z;  ++z )
    {
      os << " ";
      for ( uint2 x=0;  x<m_data.GetDim().m_x;  ++x )
      {
        os.width(width); os.fill('0');  os << (int4)GetVoxel(Point<uint2>(x,y,z)) << " ";
      } // for all x
      os << " ";
    } // for all z
    os << endl;
  } // for all y
} // PrintAllRaw


/**
 *
 */
template <class L, class T>
void Volume<L, T>::PrintAllRaw(std::ostream & os) const
{
  os << "volume dump by PrintAllRaw: with size " << m_data.GetDim() << endl;
  for ( int2 y=m_data.GetDim().m_y-1;  y >= 0; --y )
  {
    for ( uint2 z=0;  z<m_data.GetDim().m_z;  ++z )
    {
      os << " ";
      for ( uint2 x=0;  x<m_data.GetDim().m_x;  ++x )
      {
        os << GetVoxel(Point<uint2>(x,y,z));
      } // for all x
      os << " ";
    } // for all z
    os << endl;
  } // for all y
} // PrintAllRaw

/**
 *
 */
template <class L, class T>
void Volume<L, T>::SetDataType()
{
	T learnType = 0;
	std::string sDataType = typeid(learnType).name();
	bool bBoolTemp(true);
	uint1 uUint1Temp(0);
	uint2 uUint2Temp(0);
	uint4 uUint4Temp(0);
	int1 iInt1Temp(0);
	int2 iInt2Temp(0);
	int4 iInt4Temp(0);
	float4 fFloat4Temp(0.0F);

	if (sDataType == typeid(bBoolTemp).name()) m_eDataType = hdr::BL;
	else if (sDataType == typeid(uUint1Temp).name()) m_eDataType = hdr::UINT1;
	else if (sDataType == typeid(uUint2Temp).name()) m_eDataType = hdr::UINT2;
	else if (sDataType == typeid(uUint4Temp).name()) m_eDataType = hdr::UINT4;
	else if (sDataType == typeid(iInt1Temp).name()) m_eDataType = hdr::INT1;
	else if (sDataType == typeid(iInt2Temp).name()) m_eDataType = hdr::INT2;
	else if (sDataType == typeid(iInt4Temp).name()) m_eDataType = hdr::INT4;
	else if (sDataType == typeid(fFloat4Temp).name()) m_eDataType = hdr::FLT4;
	else m_eDataType = hdr::SPECIFIC;
}

#undef FILE_REVISION

// $Log: Volume.inl,v $
// Revision 1.5  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.4  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.2  2005/06/14 18:32:25  dongqing
// fix the bug of overflow pointer in the MAJORITY Fit
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/09/05 20:22:16  mkontak
// change for vx::Date
//
// Revision 3.8  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.7.2.2  2003/08/11 11:52:27  mkontak
// Added Date to vx namespace
//
// Revision 3.7.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.7  2003/06/05 19:34:43  mkontak
// Fixed compile issues with managed code, added namespace qualifier for std
//
// Revision 3.6  2003/05/16 11:54:10  michael
// code review
//
// Revision 3.5  2003/05/14 17:58:41  mkontak
// Fixed exception were LogRec was called instead of LogErr
//
// Revision 3.4  2003/05/13 15:15:56  michael
// code review
//
// Revision 3.3  2003/01/22 21:24:18  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.2  2002/04/26 15:57:35  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.1.2.1  2002/04/02 22:19:16  jmeade
// Fixed re-throw and LogRecord statements.
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
// Revision 1.5  2001/09/13 17:42:48  soeren
// added quotes to function name in logger macro
//
// Revision 1.4  2001/09/10 09:04:58  ingmar
// fixed use of logger defines
//
// Revision 1.3  2001/09/07 21:21:16  soeren
// added #undef FILE_VERSION in the end of each file
//
// Revision 1.2  2001/09/07 17:59:09  uday
// Logger::xyz  -> LogXyz
//
// Revision 1.1  2001/09/01 00:12:21  soeren
// renamed volume  directory
//
// Revision 1.96  2001/08/31 18:53:41  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.95  2001/08/30 23:30:09  pagron
// named Resize method on volumes more appropriately as Resample and added a Resize func that does a dummy resize
//
// Revision 1.94  2001/08/30 15:32:21  soeren
// added new resize more no resample
//
// Revision 1.93  2001/08/19 22:30:15  ingmar
// added GetSample()
//
// Revision 1.92  2001/08/15 15:39:50  liwei
// Added reference count.
//
// Revision 1.91  2001/08/13 00:24:27  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.90  2001/08/11 22:27:24  soeren
// fixed GetVoxel slowdown (pipeline now 30 min  faster :-)
// speed up in GetResampled
// specialized RayCast to no longer do anything for bool
//
// Revision 1.89  2001/08/07 17:03:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.88  2001/07/26 15:03:40  liwei
// Bug in Creating a small volume fixed. It is also the reason of why 3d renderer can't
// handle block volume correctly.
// Remember, to linearly traverse a block volume, use VolIterator.NextZYX rather
// VolIterator++.
//
// Revision 1.87  2001/07/25 19:14:23  liwei
// In voxel resampling, the multiplication is changed to voxel * float from float * voxel
// to accomodate SegmentedVoxel.
//
// Revision 1.86  2001/07/25 04:24:51  sachin
// All errors due to New Exception class fixed
//
// Revision 1.85  2001/07/25 03:45:53  sachin
// fix of exceptions code compiled properly
//
// Revision 1.84  2001/07/25 01:31:22  tushar
// fixed some compiler errors (logger)
//
// Revision 1.83  2001/07/18 20:52:40  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.82  2001/07/16 23:43:51  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.81  2001/07/13 19:51:44  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.80  2001/07/12 21:14:33  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.79  2001/07/09 16:50:58  frank
// Added support for the new V3D file format.
//
// Revision 1.78  2001/06/26 16:00:51  liwei
// Add SPECIFIC to the DATATYPE enum of voxels.
//
// Revision 1.77  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.76  2001/06/14 23:53:41  liwei
// Bookkeeping the information of memory layout and datatype
//
// Revision 1.75  2001/06/06 21:39:41  ana
// added CentralDiffGradient and CentralDiffGradientRaw for float4.
// CentralDiffGradient is using GetVoxel instead of GetVoxelRaw
//
// Revision 1.74  2001/06/01 17:29:24  ana
// PrintProgress function replaced by ProgressPrinter class
//
// Revision 1.73  2001/05/28 20:18:00  dave
// moving to UNC paths, new database
//
// Revision 1.72  2001/05/22 22:43:08  lihong
// fixed units setting in Fit()
//
// Revision 1.71  2001/05/20 23:49:03  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.70  2001/04/19 22:55:29  ana
// datatype change in Crop()
//
// Revision 1.69  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.68  2001/04/19 13:30:59  frank
// Fixed typo.  Noted bug with BlockVolume.
//
// Revision 1.67  2001/04/12 18:54:58  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.66  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.65  2001/04/10 17:36:49  dmitry
// updated datatypes
//
// Revision 1.64  2001/04/10 16:13:26  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.63  2001/04/09 22:25:44  dave
// changing from Abort to use exceptions
//
// Revision 1.62  2001/04/08 21:38:23  dave
// updated doc headers
//
// Revision 1.61  2001/04/05 18:00:47  dmitry
// updated datatypes
//
// Revision 1.60  2001/04/01 20:40:42  ingmar
// code beautify
// added Crop()
//
// Revision 1.59  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.58  2001/03/28 15:29:48  soeren
// changed print raw function
//
// Revision 1.57  2001/03/21 22:29:56  soeren
// added merge function
//
// Revision 1.56  2001/02/10 20:16:34  soeren
// Removed incorrect syntax default argument of State()
//
// Revision 1.55  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.54  2000/12/12 18:34:13  dave
// corrected VolAPI constructor
//
// Revision 1.53  2000/12/12 15:55:50  dave
// removed SetBackground()
//
// Revision 1.52  2000/12/11 19:55:09  dave
// new libpng, exception, file versions etc
//
// Revision 1.51  2000/11/22 06:43:03  dave
// Updated PNGImage class
//
// Revision 1.50  2000/11/17 01:00:22  dave
// GetHdrVar returns bool
//
// Revision 1.49  2000/11/17 00:28:41  dave
// remove syntax problems for gcc
//
// Revision 1.48  2000/11/16 17:19:49  dave
// added native BlockVolume VolIterator
//
// Revision 1.47  2000/11/14 00:46:21  ingmar
// removed stepsize debug print
//
// Revision 1.46  2000/11/14 00:41:43  ingmar
// faster approximate histogram for icon rendering
//
// Revision 1.45  2000/11/13 18:05:22  ingmar
// adapted icon code to new Histogram class
//
// Revision 1.44  2000/11/10 17:34:05  dave
// impl. API changes for VolIterator
//
// Revision 1.43  2000/11/08 19:46:36  frank
// Added CountInstancesOfValue function for quick native counting
//
// Revision 1.42  2000/11/06 03:56:39  ingmar
// better color ramp
//
// Revision 1.41  2000/11/06 01:15:11  ingmar
// somewhat improved float volume icons
//
// Revision 1.40  2000/11/05 22:37:55  ingmar
// added histograms to volume icon
//
// Revision 1.39  2000/11/04 23:45:42  ingmar
// addeed no-cast version of PrintAll
//
// Revision 1.38  2000/11/04 21:27:46  ingmar
// debugged templetized test for Neighbor6Iterator
//
// Revision 1.37  2000/11/04 03:07:54  ingmar
// icon ray cast now with space leaping and early ray termination
//
// Revision 1.36  2000/11/03 18:20:15  ingmar
// moved volume dimension back to unit and made positions inside a volume ushort
//
// Revision 1.35  2000/11/03 17:02:27  dave
// converted back
//
// Revision 1.34  2000/11/03 15:50:12  ingmar
// GetNumberOfVoxels now returns uint
//
// Revision 1.33  2000/11/03 04:51:54  ingmar
// even better icons
//
// Revision 1.32  2000/11/03 04:41:51  ingmar
// even better icons
//
// Revision 1.31  2000/11/03 04:23:18  ingmar
// even better icons
//
// Revision 1.30  2000/11/03 04:19:05  ingmar
// even better icons
//
// Revision 1.29  2000/11/03 03:14:07  ingmar
// fixed some gcc syntax errors and warnings
//
// Revision 1.28  2000/11/03 02:06:05  ingmar
// better icons
//
// Revision 1.27  2000/11/02 19:48:10  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.26  2000/11/02 16:48:51  sarang
// Removed errors (thanks to gcc!)
//
// Revision 1.25  2000/11/02 03:44:53  ingmar
// debugged midVolumeOffset, it now works fine
//
// Revision 1.24  2000/11/01 21:11:30  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.23  2000/11/01 20:36:46  ingmar
// added midVolumeOffset to preview icon generation
//
// Revision 1.22  2000/11/01 17:44:27  dave
// removed GetDataType() from Volume interface
//
// Revision 1.21  2000/10/31 22:34:47  ingmar
// improved PrintAllRaw
//
// Revision 1.20  2000/10/31 17:16:03  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.19  2000/10/26 01:37:36  ingmar
// added get/set voxel for a Point<short> position
//
// Revision 1.18  2000/10/25 14:55:56  ingmar
// removed syntax errors found by gcc
//
// Revision 1.17  2000/10/25 14:13:00  ingmar
// put SetBackground back
//
// Revision 1.16  2000/10/25 05:23:11  antonio
// added typename to front of variable declaration,
// and commented out part of test that used HASH
// and RLE volumes.
//
// Revision 1.15  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.14  2000/10/24 16:58:06  dave
// added background set
//
// Revision 1.13  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.12  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.11  2000/10/22 00:24:24  dave
// better iterators
//
// Revision 1.10  2000/10/21 06:06:03  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.9.8.3  2000/10/20 19:23:56  ingmar
// added Fit function
//
// Revision 1.9.8.2  2000/10/20 18:09:05  ingmar
// better majority resize
//
// Revision 1.9.8.1  2000/10/18 01:50:29  binli
// added majority selection for resizeing -Ingmar
//
// Revision 1.9  2000/09/24 23:21:21  ingmar
// added test to make sure permuted volume are cubic
//
// Revision 1.8  2000/09/23 17:02:09  dave
// changed permute coordinates coordinates
//
// Revision 1.7  2000/09/21 06:49:26  dave
// cosmetics
//
// Revision 1.6  2000/09/17 21:21:24  ingmar
// back to better generic Histogram of version 1.4
//
// Revision 1.5  2000/09/17 17:00:13  ingmar
// moved transfer function limits to 20% and 40%
//
// Revision 1.4  2000/09/17 04:35:11  ingmar
// use new min-ignoring Histogram
//
// Revision 1.3  2000/09/17 02:16:53  dave
// removed SpecificHeader*, as now replaced by reneric map
//
// Revision 1.2  2000/09/11 22:46:09  dave
// added date field to constructors
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 38    9/09/00 5:46p Gai
// Work in Progress in fixing Matrix Permutations
//
// 37    9/08/00 11:29a Ingmar
// code beautify
//
// 36    9/07/00 11:10a Dave
// updates to accomodate BlockVolume
//
// 35    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 34    9/04/00 1:39a Dave
// checking in for cvs change
//
// 33    9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 32    8/31/00 1:08p Dave
// added SetFileName
//
// 31    8/30/00 2:42p Dave
// added SetUnits()
//
// 30    8/23/00 12:59p Jay
// Removed dead code
//
// 29    8/21/00 10:48p Ingmar
// preview icon now a lot faster
//
// 28    8/21/00 1:58a Sarang
// updated header
//
// *****************  Version 27  *****************
// User: Jay          Date: 8/21/00    Time: 1:22a
// Updated in $/vx/src/Volume
// std::cerr -> DebugMsg::Error
//
// *****************  Version 26  *****************
// User: Jay          Date: 8/20/00    Time: 11:32p
// Updated in $/vx/src/Volume
// Synchronizing updates 25 and 21
//
// *****************  Version 25  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Volume
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// *****************  Version 21  *****************
// User: Jay          Date: 8/19/00    Time: 7:04p
// Updated in $/vx/src/Volume
//
// *****************  Version 20  *****************
// User: Ingmar       Date: 8/19/00    Time: 6:19p
// Updated in $/v2k/src/Volume
// SetVoxelRaw
//
// *****************  Version 19  *****************
// User: Jay          Date: 8/19/00    Time: 3:47p
// Updated in $/vx/src/Volume
// Function to shift volumes
//
// *****************  Version 18  *****************
// User: Ingmar       Date: 8/19/00    Time: 3:16p
// Updated in $/v2k/src/Volume
// GetVoxel/GetVoxelRaw + inline Error::DebugMsg
//
// *****************  Version 17  *****************
// User: Jay          Date: 8/19/00    Time: 12:00p
// Updated in $/vx/src/Volume
//
// *****************  Version 16  *****************
// User: Jay          Date: 8/17/00    Time: 2:27a
// Updated in $/v2k/src/Volume
// Added header for Creation Date
//
// *****************  Version 15  *****************
// User: Dave         Date: 8/16/00    Time: 1:45p
// Updated in $/v2k/src/Volume
//
// *****************  Version 14  *****************
// User: Jay          Date: 8/16/00    Time: 1:35p
// Updated in $/v2k/src/Volume
// Overloaded constructor in Volume to enable creating HashVolumes with
// various hash bits
//
// *****************  Version 13  *****************
// User: Antonio      Date: 8/16/00    Time: 11:34a
// Updated in $/vx/src/Volume
// another id test
//
// *****************  Version 12  *****************
// User: Antonio      Date: 8/16/00    Time: 11:28a
// Updated in $/vx/src/Volume
// id test 2
//
// *****************  Version 11  *****************
// User: Antonio      Date: 8/16/00    Time: 11:26a
// Updated in $/vx/src/Volume
// id test
//
// *****************  Version 10  *****************
// User: Antonio      Date: 8/16/00    Time: 11:25a
// Updated in $/vx/src/Volume
// made changes to header
//
// *****************  Version 9  *****************
// User: Antonio      Date: 8/11/00    Time: 1:41p
// Updated in $/vx/src/Volume
// added header and history to end of file.
//
// *****************  Version 8  *****************
// User: Dave            Date:  8/11/00  Time: 11:14a
// Checked in $/v2k/src/Volume
// Comment:
//   added SetVoxel comments
//
// *****************  Version 7  *****************
// User: Jay             Date:  8/11/00  Time:  1:37a
// Checked in $/v2k/src/Volume
// Comment:
//
//
// *****************  Version 6  *****************
// User: Jay             Date:  8/08/00  Time: 10:45a
// Checked in $/vx/src/Volume
// Comment:
//   got old volume library back into working state
//
// *****************  Version 5  *****************
// User: Jay             Date:  8/08/00  Time:  1:39a
// Checked in $/vx/src/Volume
// Comment:
//   Before changing the implementation of Volume to that using
// std::vector
//
// *****************  Version 4  *****************
// User: Jay             Date:  8/08/00  Time: 12:00a
// Checked in $/vx/src/Volume
// Comment:
//   Cleaned up
//
// *****************  Version 3  *****************
// User: Jay             Date:  8/07/00  Time: 10:13p
// Checked in $/vx/src/Volume
//
// *****************  Version 2  *****************
// User: Jay             Date:  8/07/00  Time:  9:34p
// Checked in $/vx/src/Volume
// Comment:
//   Made a template version of volume
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:03a
// Created Volume.inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Volume.inl,v 1.5 2006/02/03 15:22:11 frank Exp $
// $Id: Volume.inl,v 1.5 2006/02/03 15:22:11 frank Exp $