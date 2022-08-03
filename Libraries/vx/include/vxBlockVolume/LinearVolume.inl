// $Id: LinearVolume.inl,v 1.8 2006/06/21 17:32:39 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: David McManamon dave@viatronix.com
//
// Complete history at bottom of file.

#include "V3DFile.h"
#include <math.h>

// declare all explicit specializations

#define FILE_REVISION "$Revision: 1.8 $"

/*
template<>
void LinearVolume<bool>::Write(std::string sFileName) throw (ex::IOException);

template<>
void LinearVolume<bool>::Read(std::string sFileName) throw (ex::IOException);
*/

/**
* Default constructor for an empty volume.
*/
template <class T>
LinearVolume<T>::LinearVolume()
{
  m_zStep = m_dimension.m_x * m_dimension.m_y;

  uint4 size = (GetNumberOfVoxels());
  m_pData.resize(size);
  for ( uint4 i = 0; i < size; i++ )
    m_pData[i] = 0;
  m_eMemoryLayout = hdr::LINEAR;
}


template <class T>
void LinearVolume<T>::Merge(LinearVolume<T> const & otherVolume)
{
  LogDbg("Merge Operation not supported on linear volumes","LinearVolume::T","Merge");
  return;
}


/**
* Constructor for building new volumes.  All voxels are initialized to
* zero.
* @see VolAPI
* @param dim the volume dimensions.
* @param units the volume units.
* @param dataType the volume dataType.
* @param bitsPerVoxel the bits per voxel.
* @param unitType
* @param ext the volume extension.
* @param sFileName the file name.
* @param title the volume title.
* @param author the volume author/source.
*/
template <class T>
LinearVolume<T>::LinearVolume(const Triple<uint4> & dim, const uint4 & bitsPerVoxel,
                              const Triple<float4> & units,
                              hdr::UnitTypeEnum unitType,
                              const T & background,
                              hdr::ExtensionEnum ext,
                              const std::string & sFileName,
                              const std::string & title,
                              const std::string & author,
                              const vx::Date & date) :
hdr::VolAPI<T>(dim, bitsPerVoxel, units, 
               unitType, background, ext, sFileName, title, author, date)

{
  m_zStep = m_dimension.m_x * m_dimension.m_y;

  m_pData.resize((GetNumberOfVoxels()));
  // set all voxels to zero
  for ( uint4 k=0; k< m_pData.size(); k++ )
  {
    m_pData[k] = 0;
  }
  m_eMemoryLayout = hdr::LINEAR;
}


/**
* Overloaded assignment operator.
* @param rhs the right hand side.
* @return the result.
*/
template <class T>
LinearVolume<T> const &
LinearVolume<T>::operator=(LinearVolume<T> const & rhs)
{
  //check for auto-assignment using memory address
  if ( this == &rhs )
  {
    return(*this);
  }

  Destroy();

  DeepCopy(rhs);
  return(*this);
}


/**
* Copy Constructor.
*/
template <class T>
LinearVolume<T>::LinearVolume(LinearVolume<T> const & rhs)
{
  DeepCopy(rhs);
}


/**
* Copies all volume data.
* @param rhs the right hand side.
*/
template <class T>
void LinearVolume<T>::DeepCopy(LinearVolume<T> const & rhs)
{
  LogDbg("Performing slow deep copy of volume data.","LinearVolume::T","DeepCopy");
  // assign all header variables
  Copy(rhs);
  m_pData = rhs.m_pData;

  m_zStep = rhs.m_zStep;

  m_eMemoryLayout = hdr::LINEAR;
}


/**
* Destroys all volume data.
*/
template <class T>
void LinearVolume<T>::Destroy(void)
{
#ifdef _WIN32
  m_pData.~vector();
  new (&m_pData) std::vector<T>;
#else  // for gcc on linux  
  std::vector<T> deleteMem;
  m_pData = deleteMem;
#endif
}


/**
* Destructor.
*/
template <class T>
LinearVolume<T>::~LinearVolume()
{
  Destroy();
}


/**
* Reads the volume specified into memory using the new V3D File format.
* @param sFileName the name of the volume to read.
*/
template <class T>
void LinearVolume<T>::Read(std::string sFileName)
{
  std::stringstream tt;
  if ( sFileName.size() <= 4 )
  {
    throw ex::IOException(LogRec("file read fails"+ sFileName, "LinearVolume::T","Read"));
  }

  std::string suffix = sFileName.substr(sFileName.size() - 4, 4);
  hdr::ExtensionEnum extension = GetExtension(suffix);   //convert to enum

  hdr::MemoryLayoutEnum memLayout;
  Triple<uint4> dim;
  try
  {
    HeaderVolume<uint1> checkHead;
    checkHead.Read(sFileName);
    if(m_pData.size() != checkHead.GetNumberOfVoxels()) 
    {
      std::strstream tmp; tmp << "LinVol Resize " << m_pData.size() << " to " << checkHead.GetNumberOfVoxels() << std::ends;
      LogFyi(tmp.str(),"LinearVolume","Read");
      m_pData.resize(checkHead.GetNumberOfVoxels());
    }
    dim = checkHead.GetDim();
    memLayout = (hdr::MemoryLayoutEnum) checkHead.GetDiskLayout();
  }
  catch ( ex::FileNotFoundException )
  {
    LogErr(util::Strings::Format("File read error:  %s not found", sFileName.c_str()) ,"LinearVolume::T","Read");
    throw;// ex::VException(LogRec("file read error: can not found","LinearVolume::T","Read"));
  }
  catch ( ex::IOException )
  {
    if ( sizeof(uint1) != sizeof(T) )
    {
      // do not attempt to read as .slc volume
      LogErr("IO exception error no to attempt to read as .slc volume <LinearVolume<T>::Read>","LinearVolume::T","Read");
      throw;// ex::VException(LogRec("IO exception error no to attempt to read as .slc volume <LinearVolume<T>::Read>","LinearVolume::T","Read"));
    }
    ReadSLC(sFileName);
  }

  try
  {
    m_zStep = dim.m_x * dim.m_y;
    m_sFileName = sFileName; 
    switch ( memLayout )
    {
    case hdr::LINEAR:
      {
        V3DFile img;
        MemoryBuffer memBuff((uint1 *) &*m_pData.begin(), m_pData.size() * sizeof(T));
        img.SetData(memBuff);
        img.Read(sFileName, std::make_pair( 0.0F, 100.0F ) );
        // could check version number now
        // std::pair<int,int> version = img.GetFileVersion();
        std::string header = img.GetDescription();
        ReadCommonHeader(header);

        std::string extraVars = img.GetComment();
        m_headerMap = VarMap(extraVars); // convert string back to map

        //check suffix matches m_extension
        if ( extension != m_extension )
        {
          tt <<"File name " << m_sFileName << " does not match extension label within file of " << ExtensionStr[m_extension].c_str() << std::endl;
          LogWrn(tt.str(),"LinearVolume::T","Read");
          tt.str("");
        }
        break;
      }
    case hdr::BLOCK:
      {
        LogDbg("<LinearVolume<T>::Read> CONVERTING FROM BLOCK TO LINEAR ","LinearVolume::T","Read");
        BlockVolume<T> volume;
        try
        {
          volume.Read(m_sFileName);
        }
        catch ( ex::IOException )
        {
          *this = LinearVolume<T>();
          throw;
        }
        LogDbg("Assigning *this","LinearVolume::T","Read");
        *this = LinearVolume<T>(volume, true);
        LogDbg("Converted from block volume format to linear volume.","LinearVolume::T","Read");
        break;
      }
    case hdr::REC_BLOCK:
      {
        LogWrn("RECURSIVE_BLOCKING TO LINEAR CONVERSION NOT SUPPORTED","LinearVolume::T","Read");
        break;
      }
    } // end-switch()
  } // end-try
  catch ( ex::VException & error )
  {
    LogErr(error.GetLogRecord().GetTextMessage(),"LinearVolume::T","Read");
    throw;// ex::VException(LogRec(error.GetLogRecord().GetTextMessage(),"LinearVolume::T","Read"));
  }
  catch ( ... )
  {
    throw ex::VException(LogRec("error loading data files","LinearVolume::T","Read"));
  }

  return;
} // Read()


template <class T>
void LinearVolume<T>::ReadSLC(std::string sFileName)
{
}

template <>
void LinearVolume<uint1>::ReadSLC(std::string sFileName)
{
  int x,y,z, xDim, yDim, zDim;
  float xU,yU,zU;
  char X; 
  int compressedSize;
  int _magicDataTypeNumber;
  int _bitsPerVoxel;
  int iconSize;
  int dataSliceSize;
  int datasetSize;
  int power;
  int _unitType,_dataOrigin,_dataModification,_compressionType;

  // currently a bit unsafe probably, segfault under some conditions of corrupt input
  std::ifstream srcFile(sFileName.c_str(),std::ios::binary);
  if ( !srcFile )
  {
    throw ex::IOException(LogRec("Could not open input file "+sFileName,"LinearVolume","ReadSLC"));
  }

  srcFile >> _magicDataTypeNumber;
  if ( _magicDataTypeNumber != 11111 )
  {
    throw ex::IOException(LogRec("Invalid magic number in "+sFileName,"LinearVolume","ReadSLC"));
  }

  srcFile >> xDim >> yDim >> zDim; 
  m_dimension = Triple<uint4>(xDim,yDim,zDim);

  srcFile >> _bitsPerVoxel;
  m_uiBitsPerVoxel = _bitsPerVoxel; 

  srcFile >> xU >> yU >> zU; 
  m_units = Triple<float4>(xU,yU,zU);

  // make sure no units are zero
  if ( m_units.m_x == 0.0F ) m_units.m_x = 1.0F;
  if ( m_units.m_y == 0.0F ) m_units.m_y = 1.0F;
  if ( m_units.m_z == 0.0F ) m_units.m_z = 1.0F;

  // set up coordinate conversion matrices
  m_mVolumeToWorld.Identity();
  m_mWorldToVolume.Identity();
  m_mVolumeToWorld.Scale(Vector<float4>(m_units.m_x, m_units.m_y, m_units.m_z));
  m_mWorldToVolume.Scale(Vector<float4>(1.0F / m_units.m_x, 1.0F / m_units.m_y, 1.0F / m_units.m_z));

  srcFile >> _unitType >> _dataOrigin >> _dataModification >> _compressionType;
  m_unitType = (hdr::UnitTypeEnum)_unitType;

  power = int( ceil( log( static_cast<float4>(Max(xDim, yDim, zDim)))/log(2.0F)));
  datasetSize   = static_cast<int>(pow(2.0F,3*power));
  dataSliceSize = m_dimension.m_x * m_dimension.m_y;


  // load icon
  srcFile >> x >> y; iconSize = x*y;

  for ( X = '\0';  X != 'X'; srcFile.get(X) ); 

  srcFile.seekg(iconSize*3,std::ios::cur);
  m_pData.resize(datasetSize);

  uint1 * compressedSlice = new uint1 [2*dataSliceSize];
  uint1 * slice = &m_pData[0];

  for ( z=0; z<m_dimension.m_z; ++z, slice += dataSliceSize )
  {
    // read compressed slice size
    srcFile >> compressedSize;  
    // advance to real data
    for ( X = '\0';  X != 'X'; srcFile.get(X) );    
    // read in slice
    srcFile.read((char *)compressedSlice, compressedSize);
    // decompress slice
    register uint1 *compressData = compressedSlice;
    register uint1 *voxel = slice;
    register uint1 currentValue = *compressData;
    register uint1 remaining;

    for ( currentValue = *(compressData++);
        (remaining = (currentValue & 0x7f));
        currentValue = *(compressData++) )
    {
      if ( currentValue & 0x80 )
      {
        while ( remaining-- )
          *(voxel++) = *(compressData++);
      }
      else
      {
        currentValue = *(compressData++);
        while ( remaining-- )
          *(voxel++) = currentValue;
      }       
    }
    //ProgressPrinter::ShowProgress(true, z, (m_dimension.m_z-1)/100.);
  } // for z
  if ( compressedSlice ) delete compressedSlice;

  m_zStep = m_dimension.m_x * m_dimension.m_y;
  m_sFileName = sFileName;
} // ReadSLC


/**
* If no file name is specified the same
* name is used from the call to Read().  The filename specified should end
* with a valid hdr::ExtensionEnum from that enumeration.
* @param sFileName the file name to write, default value is the same as the name
* of the input file.
* @param buildIcon by default a new icon is rendered unless this value is set
* to false.
*/
template <class T>
void LinearVolume<T>::Write(std::string sFileName)
{
  std::stringstream tt;
  if ( sFileName == "NULL" )
  {  //use sFileName stored from Read()
    sFileName = m_sFileName;
  }
  if ( sFileName.size() <= 4 )
  {
    std::string newName = sFileName + vx::VolumeExt;
    tt << "<BlockVolume::Write> sFileName "+ sFileName+ " appears invalid, storing as " << newName <<std::endl;
    LogWrn(tt.str(),"LinearVolume::T","Write");
    tt.str("");
    sFileName = newName;
  }
  std::string suffix = sFileName.substr(sFileName.size() - 4, 4);
  hdr::ExtensionEnum extension = GetExtension(suffix);   //convert to enum

  V3DFile img;

  // Set the text chunks for the image
  img.SetDescription( BuildAsciiHeader( hdr::LINEAR, extension ) );
  img.SetTitle( m_sTitle );
  img.SetComment( m_headerMap.ToString() );

  //uchar name[3] = {118, 100, 65}; //"vdA"
  uint1 name[3] = {'v', 'o', 'L'};

  MemoryBuffer memoryBuffer( reinterpret_cast< uint1 * const >( & m_pData[0] ), GetDataSize() );
  img.SetData( memoryBuffer );

  img.Write(sFileName);

  return;
}


/**
* Writes a file using the new V3D File format.
* If no file name is specified the same
* name is used from the call to Read().  The filename specified should end
* with a valid hdr::ExtensionEnum from that enumeration.
* @param sFileName the file name to write, default value is the same as the name
* of the input file.
* @param buildIcon by default a new icon is rendered unless this value is set
* to false.
*/
template <class T>
void LinearVolume<T>::WriteNew(std::string sFileName)
{
  std::stringstream tt;
  if ( sFileName == "NULL" )
  {  //use sFileName stored from Read()
    sFileName = m_sFileName;
  }
  if ( sFileName.size() <= 4 )
  {
    std::string newName = sFileName + vx::VolumeExt;
    tt << "<BlockVolume::Write> sFileName "+ sFileName+ " appears invalid, storing as " << newName <<std::endl;
    LogWrn(tt.str(),"LinearVolume::T","WriteNew");
    tt.str("");
    sFileName = newName;
  }
  std::string suffix = sFileName.substr(sFileName.size() - 4, 4);
  hdr::ExtensionEnum extension = GetExtension(suffix);   //convert to enum

  V3DFile img;
  img.SetDescription(BuildAsciiHeader(hdr::LINEAR, extension));
  img.SetTitle(m_sTitle);
  img.SetComment(m_headerMap.ToString());
  img.SetData(MemoryBuffer((uint1 *) &(m_pData.begin()), GetDataSize()));
  img.Write(sFileName);

  return;
} // WriteNew()


/**
* Set the value at the location specified.
* @param location const Point<ushort> &
* @param value the new value
*/
template <class T>
void LinearVolume<T>::SetVoxelRaw(const Point<uint2> & location, T value)
{
  //Can either implement by indexing into x,y,z offset arrays (if they
  //existed) or by simply making the computation for each access and saving
  //the space of the array, make your own arrays if you want them
  m_pData[GetOffset(location)] = value;
}


/**
* Gets the voxel at the specified location.  No bounds checking!
* @param location the location in the volume to get the voxel.
* @return the value at the location specified.
*/
template <class T>
const T  LinearVolume<T>::GetVoxelRaw(const Point<uint2> & location) const
{
  return(m_pData[GetOffset(location)]);
}


/**
* Computes and returns the resampled value at the location specified.
* @param point the location to compute the resampled value.
* @return the resampled value.
*/
template <class T>
float4 LinearVolume<T>::GetSample(const Point<float4> & point) const
{
  // does return float cause any problems when volume is of double

  // bounds check
  if ( point.m_x < 0 || point.m_x >= m_dimension.m_x ||
       point.m_y < 0 || point.m_y >= m_dimension.m_y ||
       point.m_z < 0 || point.m_z >= m_dimension.m_z )
  {
    return(m_background);
  }

  // truncate
  uint4 newx = uint4(point.m_x);
  uint4 newy = uint4(point.m_y);
  uint4 newz = uint4(point.m_z);

  // weights for tri-linear interpolation
  float4 weightX = point.m_x - newx;
  float4 weightY = point.m_y - newy;
  float4 weightZ = point.m_z - newz;

  // values for tri-linear interpolation
  uint4 xstep = 1;
  uint4 ystep = m_dimension.m_x;
  uint4 zstep = m_dimension.m_y * m_dimension.m_x;
  uint4 index = newx + (newy * ystep) + (newz * zstep);
  uint4 maxIndex = zstep * m_dimension.m_z;

  // indices for lower left front (llf), lower left back (llb), etc
  uint4 illf = index;
  uint4 illb = index + zstep;
  uint4 iulf = index + ystep;
  uint4 iulb = index + ystep + zstep;
  uint4 ilrf = index + xstep;
  uint4 ilrb = index + xstep + zstep;
  uint4 iurf = index + xstep + ystep;
  uint4 iurb = index + xstep + ystep + zstep;

  const T * data = &*(m_pData.begin());
  T llf, llb, ulf, ulb, lrf, lrb, urf, urb;
  // bounds check indices and retrieve values
  if ( illf >= 0 && illf <= maxIndex ) llf = *(data + illf);
  else llf = 0;
  if ( illb >= 0 && illb <= maxIndex ) llb = *(data + illb);
  else llb = 0;
  if ( iulf >= 0 && iulf <= maxIndex ) ulf = *(data + iulf);
  else ulf = 0;
  if ( iulb >= 0 && iulb <= maxIndex ) ulb = *(data + iulb);
  else ulb = 0;
  if ( ilrf >= 0 && ilrf <= maxIndex ) lrf = *(data + ilrf);
  else lrf = 0;
  if ( ilrb >= 0 && ilrb <= maxIndex ) lrb = *(data + ilrb);
  else lrb = 0;
  if ( iurf >= 0 && iurf <= maxIndex ) urf = *(data + iurf);
  else urf = 0;
  if ( iurb >= 0 && iurb <= maxIndex ) urb = *(data + iurb);
  else urb = 0;

  // tri-linear interpolation
  float4 llm = weightZ * llb + (1 - weightZ) * llf;
  float4 ulm = weightZ * ulb + (1 - weightZ) * ulf;
  float4 lrm = weightZ * lrb + (1 - weightZ) * lrf;
  float4 urm = weightZ * urb + (1 - weightZ) * urf;
  float4 mlm = weightY * ulm + (1 - weightY) * llm;
  float4 mrm = weightY * urm + (1 - weightY) * lrm;
  float4 mmm = weightX * mrm + (1 - weightX) * mlm;

  return(mmm);
} // GetResampledValue


/**
* Note actual volume requires GetDataSize() + GetBytesPerVoxel() for
* storage because the End() iterator must point to one past the last voxel.
* @return the total bytes of data needed to store the volume.
*/
template <class T>
const uint4 LinearVolume<T>::GetMemoryUsage() const
{
  return(GetDataSize() + sizeof(LinearVolume<T>));
}

/**
* Reports relevant volume layout statistics.
*/
template <class T>
void LinearVolume<T>::Stats(std::ostream & os) const
{
  os << "** LINEARVOLUME STATISTICS **" <<std::endl;
  os << GetMemoryUsage() / (1024.0 * 1024)<< " MB used"<<std::endl;
}

/**
 * Resize volume to given dimension and make sure internal memory usage 
 * is an exact match of the dimensions
 */
template <class T>
void LinearVolume<T>::Resize(const Triple<uint4> & dimension)
{
  if(m_dimension != dimension)
  {    
    m_dimension = dimension;
		m_zStep = m_dimension.m_x * m_dimension.m_y;
    m_pData.~vector();
    new (&m_pData) std::vector<T>;
    m_pData.resize(GetNumberOfVoxels());
  }
}

/**
* Resize the volume according to the Resampling type specified by the enum.
* @param dimension Triple<uint> &
* @param resampleType hdr::ResampleTypeEnum
*/
// VC++ chokes if a defult parameter is given in a template fn.
/*
template <class T>
void LinearVolume<T>::Resize(const Triple<uint> & dimension, const hdr::ResampleTypeEnum resampleType)
{
Triple<float>scale(m_dimension.m_x/float(dimension.m_x),
m_dimension.m_y/float(dimension.m_y),
m_dimension.m_z/float(dimension.m_z));
ulong newSize(dimension.m_x * dimension.m_y * dimension.m_z);
std::vector<T>data(newSize);
Triple<ulong>step(1, dimension.m_x, dimension.m_x*dimension.m_y);
for (uint zDim=0; zDim<dimension.m_z; zDim++) {
for (uint yDim=0; yDim<dimension.m_y; yDim++) {
for (uint xDim=0; xDim<dimension.m_x; xDim++) {
Point<float>targetVoxel(((float(xDim)+0.5)*scale.m_x - 0.5),
((float(yDim)+0.5)*scale.m_y - 0.5),
((float(zDim)+0.5)*scale.m_z - 0.5));
switch (resampleType) {
case hdr::POINT:
{
// take the voxel value at targetVoxel (with x,y,z, rounded off)
targetVoxel = Point<float>((targetVoxel.m_x + 0.5),
(targetVoxel.m_y + 0.5),
(targetVoxel.m_z + 0.5));
Point<ushort>targetVoxelRounded;
targetVoxelRounded.Cast(targetVoxel);
data[(xDim*step.m_x + yDim*step.m_y + zDim*step.m_z)] =
GetVoxelRaw(targetVoxelRounded);
break;
}
case hdr::TRILIN:
{
// resample the voxel value at targetVoxel
data[(xDim*step.m_x + yDim*step.m_y + zDim*step.m_z)] =
GetResampledValue(targetVoxel);
break;
}
case hdr::BOX:
case hdr::TENT:
case hdr::GAUSS:
default:
Error::DebugMsg ("Resample type not yet Suppoprted");
break;
};
} // for xDim
} // for yDim
} // for zDim
// update m_dimension
m_dimension = dimension;
// update m_units
m_units.m_x *= scale.m_x;
m_units.m_y *= scale.m_y;
m_units.m_z *= scale.m_z;
// assign the new data
m_pData = data;
// icon data would change when it is written next time
} // end of Resize()
*/

/**
* Changes the reference fram. Basically stores the voxels in a different order,
* so that the slices in X, Y, Z direction can be in a contiguous array of memory
* for image display.
* The initial volume is stored in zyx order and is hence the Transverse slices
* is in contiguous array of memory.
* For: Transverse to Coronal:
* Applies the transformation matrix to the whole LinearVolume from (X)(Y)(Z)
* co-ordinate system to (X)(-Z)(Y)
* The transformation matrix is:
* |    1     0     0     0    |
* |    0     0     1     0    |
* |    0     1     0     0    |
* |    0     0     0     1    |
* For: Transverse to Saggittal:
* Applies the transformation matrix to the whole LinearVolume from (X)(Y)(Z)
* co-ordinate system to (-Z)(Y)(X).
* The transformation matrix is:
* |    0     0     1     0    |
* |    0     1     0     0    |
* |    1     0     0     0    |
* |    0     0     0     1    |
* For: Coronal to Saggittal:
* Applies the transformation matrix to the whole LinearVolume from (X)(-Z)(Y)
* co-ordinate system to (-Z)(Y)(X). This implies that the Transverse volume
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
template <class T>
void LinearVolume<T>::PermuteCoordinates(hdr::PermuteCoordinatesEnum view)
{
  uint4 xOffset, yOffset, zOffset;
  switch ( view )
  {
  case hdr::TRANSVERSE_TO_CORONAL:
    {
      Triple<uint>newDim(m_dimension.m_x, m_dimension.m_z, m_dimension.m_y);
      Triple<float4>newUnits(m_units.m_x, m_units.m_z, m_dimension.m_y);
      uint4 size = (m_dimension.m_x * m_dimension.m_y * m_dimension.m_z);
      std::vector<T> data(size);
      uint4 zStep(m_dimension.m_x*m_dimension.m_z),
      yStep(m_dimension.m_x),
      xStep(1);
      // Transforming from Reference frame (X)(Y)(Z) to (X)(Z)(Y)
      for ( uint4 z=0; z<m_dimension.m_z; z++ )
      {
        yOffset = z*yStep;
        for ( uint4 y=0; y<m_dimension.m_y; y++ )
        {
          zOffset = y*zStep;
          for ( uint4 x=0; x<m_dimension.m_x; x++ )
          {
            xOffset = x*xStep;
            data[xOffset+yOffset+zOffset] =
            GetVoxelRaw(Point<uint2>(x, y, z));
          }
        }
      }
      // update the new dimensions and units
      m_dimension = newDim;
      m_units = newUnits;
      // make sure no units are zero
      if ( m_units.m_x == 0.0F ) m_units.m_x = 1.0F;
      if ( m_units.m_y == 0.0F ) m_units.m_y = 1.0F;
      if ( m_units.m_z == 0.0F ) m_units.m_z = 1.0F;
      // set up coordinate conversion matrices
      m_mVolumeToWorld.Identity();
      m_mWorldToVolume.Identity();
      m_mVolumeToWorld.Scale(Vector<float4>(m_units.m_x, m_units.m_y, m_units.m_z));
      m_mWorldToVolume.Scale(Vector<float4>(1.0F / m_units.m_x, 1.0F / m_units.m_y, 1.0F / m_units.m_z));
      // assign the new data
      m_pData = data;
      // icon data would change when it is written next time
      break;
    }
  case hdr::TRANSVERSE_TO_SAGGITTAL:
    {
      Triple<uint4>newDim(m_dimension.m_z, m_dimension.m_y, m_dimension.m_x);
      Triple<float4>newUnits(m_units.m_z, m_units.m_y, m_dimension.m_x);
      uint4 size = (m_dimension.m_x * m_dimension.m_y * m_dimension.m_z);
      std::vector<T>data(size);
      uint4 zStep(m_dimension.m_y*m_dimension.m_z),
      yStep(m_dimension.m_z),
      xStep(1);
      // Transforming from Reference frame (X)(Y)(Z) to (Z)(Y)(X)
      for ( uint4 z=0; z<m_dimension.m_z; z++ )
      {
        xOffset = z*xStep;
        for ( uint4 y=0; y<m_dimension.m_y; y++ )
        {
          yOffset = y*yStep;
          for ( uint4 x=0; x<m_dimension.m_x; x++ )
          {
            zOffset = x*zStep;
            data[xOffset+yOffset+zOffset]
            = GetVoxelRaw(Point<uint2>(x, y, z));
          }
        }
      }
      // update the new dimensions and units
      m_dimension = newDim;
      m_units = newUnits;
      // make sure no units are zero
      if ( m_units.m_x == 0.0F ) m_units.m_x = 1.0F;
      if ( m_units.m_y == 0.0F ) m_units.m_y = 1.0F;
      if ( m_units.m_z == 0.0F ) m_units.m_z = 1.0F;
      // set up coordinate conversion matrices
      m_mVolumeToWorld.Identity();
      m_mWorldToVolume.Identity();
      m_mVolumeToWorld.Scale(Vector<float4>(m_units.m_x, m_units.m_y, m_units.m_z));
      m_mWorldToVolume.Scale(Vector<float4>(1.0F / m_units.m_x, 1.0F / m_units.m_y, 1.0F / m_units.m_z));
      // assign the new data
      m_pData = data;
      // icon data would change when it is written next time
      break;
    }
  case hdr::CORONAL_TO_SAGGITTAL:
    {
      // a coronal transform has been applied
      // the entire transform would be XYZ -> XZY -> ZYX
      Triple<uint4>newDim(m_dimension.m_y, m_dimension.m_z, m_dimension.m_x);
      Triple<float4>newUnits(m_units.m_y, m_units.m_z, m_dimension.m_x);
      uint4 size = (m_dimension.m_x * m_dimension.m_y * m_dimension.m_z);
      std::vector<T> data(size);
      uint4 zStep(m_dimension.m_y*m_dimension.m_z),
      yStep(m_dimension.m_y),
      xStep(1);
      // Transforming from Reference frame  (X)(Z)(Y) to (Z)(Y)(X)
      for ( uint4 z=0; z<m_dimension.m_z; z++ )
      {
        xOffset = z*xStep; // changed for the old view
        //yOffset = z*yStep;
        for ( uint4 y=0; y<m_dimension.m_y; y++ )
        {
          yOffset = y*yStep;
          //xOffset = y*xStep;
          for ( uint4 x=0; x<m_dimension.m_x; x++ )
          {
            zOffset = x*zStep;
            uint4 testoffset = xOffset+yOffset+zOffset;
            data[xOffset+yOffset+zOffset]
            = GetVoxelRaw(Point<uint2>(x, y, z));
          }
        }
      }
      // update the new dimensions and units
      m_dimension = newDim;
      m_units = newUnits;
      // make sure no units are zero
      if ( m_units.m_x == 0.0F ) m_units.m_x = 1.0F;
      if ( m_units.m_y == 0.0F ) m_units.m_y = 1.0F;
      if ( m_units.m_z == 0.0F ) m_units.m_z = 1.0F;
      // set up coordinate conversion matrices
      m_mVolumeToWorld.Identity();
      m_mWorldToVolume.Identity();
      m_mVolumeToWorld.Scale(Vector<float4>(m_units.m_x, m_units.m_y, m_units.m_z));
      m_mWorldToVolume.Scale(Vector<float4>(1.0F / m_units.m_x, 1.0F / m_units.m_y, 1.0F / m_units.m_z));
      // assign the new data
      m_pData = data;
      // icon data would change when it is written next time
      break;
    }
  default:
    LogWrn("VolAPI<T>::ApplyTransform: ViewType not supported.","LinearVolume::T","PermuteCoordinates");
    break;
  }
}


/**
* Performs Volume class unit test.  Successful execution requires all
* files from "/usr/people/dave/volumeTest" to be copied to the local
* directory "volumeTest".
* @param os the stream to send output.
* @return true if success, else false.
*/
template <class T>
bool LinearVolume<T>::TestByType(std::ostream & os)
{
  bool ok(true);

  os << "BEGINNING VOLUME CLASS TEST"<< std::endl;
  os << "OK:" <<ok<< " BEGINNING VOLAPI TEST"<<std::endl;

  string author = "Brett Ashley";
  string title1 = "The Sun Also Rises";
  string sFileName = "TheTest.vol";
  Triple<uint4> dim(50,50,50);
  Triple<float4> units(1.0,1.0,1.0);

  LinearVolume<T> apiTest(dim, sizeof(T) * 8, units,
                          hdr::METER, 0, hdr::VOL,
                          sFileName, title1, author);

  apiTest.SetCompression(hdr::ZLIB);
  ok = ok && (apiTest.GetTitle() == title1);
  apiTest.SetTitle("something new");
  ok = ok && (apiTest.GetTitle() == "something new");
  ok = ok && (apiTest.GetAuthor() == author);
  ok = ok && (apiTest.GetUnitType() == hdr::METER);
  ok = ok && (apiTest.GetDim() == dim);
  ok = ok && (apiTest.GetUnits() == units);
  ok = ok && (apiTest.GetBitsPerVoxel() == sizeof(T) * 8);
  ok = ok && (apiTest.GetBytesPerVoxel() == sizeof(T));
  ok = ok && (apiTest.GetFileName() == sFileName);
  ok = ok && (apiTest.GetFileNameRoot() == "TheTest");
  std::string sauce = "The special sauce";
  apiTest.SetHdrVar("GFUNK", sauce, "some description");
  std::string sauce2;
  ok &= (apiTest.GetHdrVar("GFUNK", sauce2));
  ok &= (sauce == sauce2);

  ok = ok && (apiTest.InsideVolume(Point<uint2>(dim.m_x, dim.m_y, dim.m_z)) == false);
  Point<uint2> inside(20,13,10);
  ok = ok && (apiTest.InsideVolume(inside) == true);
  apiTest.AsOstream(os);
  uint4 realNumber = dim.m_x * dim.m_y * dim.m_z;
  ok = ok && (apiTest.GetNumberOfVoxels() == realNumber);


  os << "OK:"<< ok<< " Testing SetVoxelRaw(), GetVoxelRaw() ..."<< std::endl;

  LinearVolume<T> vol(dim, sizeof(T) * 8, units,
                      hdr::METER, 0, hdr::VOL,
                      "Test.vol", title1, author);
  vol.SetVoxelRaw(Point<uint2>(0,0,0),(T) 90);
  vol.SetVoxelRaw(Point<uint2>(49,49,49),(T)  91);
  vol.SetVoxelRaw(Point<uint2>(4,4,4),(T) 92);
  vol.SetVoxelRaw(Point<uint2>(4,4,5),(T)  93);
  vol.SetVoxelRaw(Point<uint2>(4,5,4),(T)  94);
  vol.SetVoxelRaw(Point<uint2>(4,5,5),(T)  95);
  vol.SetVoxelRaw(Point<uint2>(5,4,4),(T)  96);
  vol.SetVoxelRaw(Point<uint2>(5,4,5),(T) 97);
  vol.SetVoxelRaw(Point<uint2>(5,5,4),(T) 98);
  vol.SetVoxelRaw(Point<uint2>(5,5,5),(T)  99);

  ok = ok && ((T) 90 == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
  ok = ok && ((T) 91 == vol.GetVoxelRaw(Point<uint2>(49,49,49)));
  ok = ok && ((T) 92 == vol.GetVoxelRaw(Point<uint2>(4,4,4)));
  ok = ok && ((T) 93 == vol.GetVoxelRaw(Point<uint2>(4,4,5)));
  ok = ok && ((T) 94 == vol.GetVoxelRaw(Point<uint2>(4,5,4)));
  ok = ok && ((T) 95 == vol.GetVoxelRaw(Point<uint2>(4,5,5)));
  ok = ok && ((T) 96 == vol.GetVoxelRaw(Point<uint2>(5,4,4)));
  ok = ok && ((T) 97 == vol.GetVoxelRaw(Point<uint2>(5,4,5)));
  ok = ok && ((T) 98 == vol.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T) 99 == vol.GetVoxelRaw(Point<uint2>(5,5,5)));

  vol.Write("TheTest.vol");

  vol.SetVoxelRaw(Point<uint2>(0,0,0), 0);
  vol.SetVoxelRaw(Point<uint2>(49,49,49), 0);
  vol.SetVoxelRaw(Point<uint2>(4,4,4), 0);
  vol.SetVoxelRaw(Point<uint2>(4,4,5), 0);
  vol.SetVoxelRaw(Point<uint2>(4,5,4), 0);
  vol.SetVoxelRaw(Point<uint2>(4,5,5), 0);
  vol.SetVoxelRaw(Point<uint2>(5,4,4), 0);
  vol.SetVoxelRaw(Point<uint2>(5,4,5), 0);
  vol.SetVoxelRaw(Point<uint2>(5,5,4), 0);
  vol.SetVoxelRaw(Point<uint2>(5,5,5), 0);

  vol.Read("TheTest.vol");

  ok = ok && ((T) 90 == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
  ok = ok && ((T) 91 == vol.GetVoxelRaw(Point<uint2>(49,49,49)));
  ok = ok && ((T) 92 == vol.GetVoxelRaw(Point<uint2>(4,4,4)));
  ok = ok && ((T) 93 == vol.GetVoxelRaw(Point<uint2>(4,4,5)));
  ok = ok && ((T) 94 == vol.GetVoxelRaw(Point<uint2>(4,5,4)));
  ok = ok && ((T) 95 == vol.GetVoxelRaw(Point<uint2>(4,5,5)));
  ok = ok && ((T) 96 == vol.GetVoxelRaw(Point<uint2>(5,4,4)));
  ok = ok && ((T) 97 == vol.GetVoxelRaw(Point<uint2>(5,4,5)));
  ok = ok && ((T) 98 == vol.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T) 99 == vol.GetVoxelRaw(Point<uint2>(5,5,5)));

  os << "OK:"<< ok<< " Testing copy, assignment ..."<<std::endl;

  string oldTitle = vol.GetTitle();
  LinearVolume<T> volCopy = vol;
  ok = ok && ((T) 98 == volCopy.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T) 99 == volCopy.GetVoxelRaw(Point<uint2>(5,5,5)));
  ok = ok && (volCopy.GetTitle() == oldTitle);

  LinearVolume<T> volCopy2(volCopy);
  ok = ok && ((T) 98 == volCopy.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T) 99 == volCopy.GetVoxelRaw(Point<uint2>(5,5,5)));
  ok = ok && (volCopy.GetTitle() == oldTitle);

  os <<"OK:"<<ok <<" Testing miscellaneos functions ..."<<std::endl;
  vol.Stats(os);
  vol.GetDataSize();

  os <<" OK:" <<ok <<" Testing Iterators ..."<<endl;

  LinearVolume<T>::VolIterator iter(vol.GetVolIterator(Point<uint2>(0,0,0)));

  int4 count = 0;
  for ( ; !iter.End(); iter.NextNonZero() )
  {
    ++count;
  }

  int4 countNonZero = 0;
  uint4 totalCount = 0;
  iter.MoveAbs(Point<uint2>(0,0,0));
  for ( ; !iter.End(); iter.NextZYX() )
  {
    if ( *iter != vol.GetBackground() )
      ++countNonZero;
    ++totalCount;
  }

  int4 countNonZero2 = 0;
  uint4 totalCount2 = 0;
  iter.MoveAbs(Point<uint2>(0,0,0));
  for ( ; !iter.End(); ++iter )
  {
    if ( *iter != vol.GetBackground() )
      ++countNonZero2;
    ++totalCount2;
  } 


  std::cout<<"VOX:"<<vol.GetNumberOfVoxels()<<" "<<totalCount<<" " <<totalCount2<<" "<<count<<" "<<countNonZero<<std::endl;

  ok &= (totalCount == vol.GetNumberOfVoxels());
  ok &= (totalCount == totalCount2);
  ok &= (count == countNonZero);
  ok &= (countNonZero == countNonZero2);

  iter.MoveAbs(Point<uint2>(0,0,0));

  os << "OK:"<<ok <<" Still testing iterator ..."<< std::endl;

  vol.SetVoxelRaw(Point<uint2>(2,2,2), (T) 88);

  ok &= (iter.PeekNextZ() == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
  ok &= (iter.PeekNextY() == vol.GetVoxelRaw(Point<uint2>(0,1,0)));
  ok &= (iter.PeekNextX() == vol.GetVoxelRaw(Point<uint2>(1,0,0)));

  iter.IncX(); iter.IncY(); iter.IncZ();
  iter.IncX(); iter.IncY(); iter.IncZ();

  ok &= (iter.GetVoxel() == vol.GetVoxelRaw(Point<uint2>(2,2,2)));

  iter.DecX(); iter.DecY(); iter.DecZ();

  ok &= (iter.GetVoxel() == vol.GetVoxelRaw(Point<uint2>(1,1,1)));

  os << "OK:"<<ok << endl;
  if ( ok ) os <<"COMPLETED VOLUME CLASS TEST :)"<< std::endl;
  else os <<" FAILED VOLUME CLASS TEST :("<< std::endl;
  return(true);
} // TestByType()


/**
* Returns the offset for the location.
* @param p the point to calculate the offset.
* @param the offset at that location.
*/
template <class T>
uint4 LinearVolume<T>::GetOffset(const Point<uint2> & p) const
{
  return(p.m_x + (p.m_y * m_dimension.m_x) + (p.m_z * m_zStep));
} // GetOffset()


/**
* Returns the size of the volume array.
* @return the size of the volume array.
*/
template <class T>
const uint4 LinearVolume<T>::GetDataSize() const
{
  return(m_dimension.m_x * m_dimension.m_y * m_dimension.m_z *
         sizeof(T));
} // GetDataSize()


#undef FILE_REVISION

// $Log: LinearVolume.inl,v $
// Revision 1.8  2006/06/21 17:32:39  dongqing
// add m_stepZ update to Resize
//
// Revision 1.7  2006/02/03 15:22:11  frank
// removed png library
//
// Revision 1.6  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.5  2005/08/05 13:01:31  geconomos
// moved from vx repository
//
// Revision 1.4  2005/06/14 18:31:51  dongqing
// fix the bug of overflow pointer in the MAJORITY Fit
//
// Revision 1.3  2004/03/21 23:10:16  geconomos
// Upgrade to Visual Studio .NET 2003
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.13.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.13  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.12  2003/04/18 15:59:54  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 3.11  2003/01/22 21:24:30  ingmar
// added missing include
//
// Revision 3.10  2002/11/26 00:36:44  kevin
// Don't change the exception that is re-thrown in a "catch"
//
// Revision 3.9  2002/03/11 16:30:15  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.8  2002/03/06 16:22:46  ingmar
// I fixed a bug in the resize funtion. It was the same than in the destroy function.
//
// Revision 3.7  2002/03/06 03:36:13  ingmar
// I fixed the destroy bug :-)
//
// Revision 3.6  2002/03/06 03:35:34  ingmar
// I fixed the destroy bug :-)
//
// Revision 3.5  2002/03/05 23:02:42  ingmar
// fixed bug in destroy function. this makes sure that assigning an empty dummy volume does not crash the system, but also does not necessarrily free the memory.
//
// Revision 3.4  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 3.3.2.2  2002/03/01 21:08:44  jmeade
// Unterminated strstream; Fixing log messages.
//
// Revision 3.3.2.1  2002/02/25 15:58:55  kevin
// Only resize on read if the volume size changes (Note this allows V.C. to
// resize the linear volumes long before loading them before address space
// gets fragmented)
//
// Revision 3.3  2001/11/09 04:49:43  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.2  2001/10/31 00:45:57  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding
// the volumes
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0.2.1  2001/10/31 01:14:07  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding the volumes
//
// Revision 3.2  2001/10/31 00:45:57  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding
// the volumes
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
//    Rev 2.4   Oct 02 2001 19:10:32   ingmar
// added commentsResolution for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.3   Oct 02 2001 19:07:48   ingmar
// added commentResolution for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.2   Oct 02 2001 19:05:34   ingmar
// added comment
// 
//    Rev 2.1   Oct 02 2001 18:37:18   ingmar
// moved MemoryBuffer constructor out of call to V3DFile.SetData() -> gcc should now be happy
// 
//    Rev 2.0   Sep 16 2001 23:47:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:04   ingmar
// Initial revision.
// Revision 1.8  2001/09/16 00:35:41  soeren
// changed LogFyi to LogDbg
//
// Revision 1.7  2001/09/14 23:30:19  soeren
// changed LogFyi to LogDbg
//
// Revision 1.6  2001/09/13 21:11:43  soeren
// changed messages of deep copy
//
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
// Revision 1.61  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.60  2001/08/30 15:32:30  soeren
// added new resize more no resample
//
// Revision 1.59  2001/08/19 22:33:09  ingmar
// renamed GetResampledValue() to GetSample()
//
// Revision 1.58  2001/08/13 00:24:27  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.57  2001/08/09 22:19:45  frank
// cerr -> logger::warning
//
// Revision 1.56  2001/08/07 17:03:39  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.55  2001/07/26 20:29:23  binli
// ID 89: exception handling.
//
// Revision 1.54  2001/07/25 01:31:22  tushar
// fixed some compiler errors (logger)
//
// Revision 1.53  2001/07/24 23:00:14  tushar
// fixed a byg
//
// Revision 1.52  2001/07/18 20:53:17  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.51  2001/07/16 23:23:36  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.50  2001/07/13 19:51:43  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.49  2001/07/12 21:14:47  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.48  2001/07/12 18:30:59  frank
// Avoided using an intermediate buffer for uncompressed volume data during read.
//
// Revision 1.47  2001/07/09 16:51:41  frank
// Added support for the new V3D file format.
//
// Revision 1.46  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.45  2001/06/14 23:53:41  liwei
// Bookkeeping the information of memory layout and datatype
//
// Revision 1.44  2001/06/01 17:29:32  ana
// PrintProgress function replaced by ProgressPrinter class
//
// Revision 1.43  2001/05/20 23:49:02  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.42  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.41  2001/04/16 12:53:04  frank
// Added permanent volume coordinate conversion matrix.
//
// Revision 1.40  2001/04/14 04:24:32  ingmar
// added throw in Write()
//
// Revision 1.39  2001/04/13 00:34:59  jmeade
// file extensions
//
// Revision 1.38  2001/04/12 18:54:24  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.37  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.36  2001/04/10 16:13:26  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.35  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.34  2001/04/02 16:08:37  soeren
// added support for reading
// slc files
//
// Revision 1.33  2001/03/31 05:34:38  ingmar
// uint8 -> uint4
//
// Revision 1.32  2001/03/30 18:50:03  dmitry
// Updated datatypes.
//
// Revision 1.31  2001/03/21 23:24:26  soeren
// changed messages
//
// Revision 1.30  2001/03/21 22:30:17  soeren
// added merge function
//
// Revision 1.29  2001/02/06 02:25:11  dave
// re-organized linVol Read for memory savings
//
// Revision 1.28  2000/12/19 16:59:54  sarang
// BUG_FIX : removed an inline which shouldn't have been there - thanks gcc
//
// Revision 1.27  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.26  2000/11/22 06:43:03  dave
// Updated PNGImage class
//
// Revision 1.25  2000/11/18 23:33:25  dave
// corrected GetHdrVar() for type std::string
//
// Revision 1.24  2000/11/17 20:28:30  dave
// added native VolIterator
//
// Revision 1.23  2000/11/13 21:48:43  sarang
// reason # 66543 why VC++ really sucks added ;)
// added #ifdef _WIN32 around the crap code to keep gcc/linux away from such code :)
//
// Revision 1.22  2000/11/08 19:44:51  frank
// Strange semantic problem involving namespace and typedefs fixed
//
// Revision 1.21  2000/11/08 17:20:11  frank
// Fixed memory de-allocation problem.
// Now you can run the pipeline on a small memory machine!
//
// Revision 1.20  2000/11/08 17:01:07  dave
// assignment op. de-allocates mem. correctly, thanks Frank
//
// Revision 1.19  2000/11/04 21:27:46  ingmar
// debugged templetized test for Neighbor6Iterator
//
// Revision 1.18  2000/11/03 17:02:26  dave
// converted back
//
// Revision 1.17  2000/11/02 19:48:09  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.16  2000/11/02 02:11:02  dave
// correct Write() mem leak
//
// Revision 1.15  2000/11/01 21:11:29  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.14  2000/11/01 17:44:25  dave
// removed GetDataType() from Volume interface
//
// Revision 1.13  2000/10/31 21:16:42  dave
// changed VOLAPI to use ZLIB
//
// Revision 1.12  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.11  2000/10/25 14:18:51  dave
// added zStep in GetVoxel
//
// Revision 1.10  2000/10/25 01:35:25  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.9  2000/10/24 15:46:43  dave
// no message
//
// Revision 1.8  2000/10/24 04:10:06  dave
// improved GetVoxel
//
// Revision 1.7  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.6  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.5  2000/10/22 00:52:09  dave
// better Iterators
//
// Revision 1.4  2000/10/22 00:24:23  dave
// better iterators
//
// Revision 1.3  2000/09/21 03:35:50  dave
// searching for segfaults
//
// Revision 1.2  2000/09/11 22:46:09  dave
// added date field to constructors
//
// Revision 1.1.1.1  2000/08/11 22:15:52  ingmar
// unit tests complete
//
//
// 35    9/07/00 11:10a Dave
// updates to accomodate BlockVolume
//
// 34    9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 33    9/05/00 6:07p Dave
// memory leak removed from Write
//
// 32    9/04/00 1:39a Dave
// checking in for cvs change
//
// 31    9/01/00 5:56p Dave
// initial code walkthrough changes
//
// 30    8/22/00 5:40p Jay
// *this = *new(XXX) changed to *this = XXX
//
// 29    8/21/00 1:22a Jay
// std::cerr -> DebugMsg::Error
//
// 28    8/20/00 11:53p Ingmar
// added missing hdr and std
//
// 27    8/20/00 11:32p Jay
// Synchronizing updates 25 and 21
//
// 26    8/20/00 6:49p Sarang
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// 24    8/19/00 6:19p Ingmar
// SetVoxelRaw
//
// 23    8/19/00 3:16p Ingmar
// GetVoxel/GetVoxelRaw + inline Error::DebugMsg
//
// 22    8/18/00 3:30p Jay
//
// 21    8/18/00 10:40a Jay
//
// 20    8/18/00 10:34a Jay
// Changing the Coronal to Sagittal view to the lod volume views
//
// 19    8/16/00 6:13p Jay
//
// 18    8/16/00 3:55p Jay
//
// 17    8/16/00 2:39p Antonio
// added header and footer
//
// *****************  Version 16  *****************
// User: Ingmar          Date:  8/10/00  Time: 10:26a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   faster read volume
//
// *****************  Version 15  *****************
// User: Ingmar          Date:  8/10/00  Time:  9:20a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   added missing std::
//
// *****************  Version 14  *****************
// User: Jay             Date:  8/10/00  Time: 12:16a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Linear Volume iterator is a vector::iterator
//
// *****************  Version 13  *****************
// User: Jay             Date:  8/09/00  Time: 11:54p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Template Volumes working.
//
// *****************  Version 12  *****************
// User: Dave            Date:  8/09/00  Time: 10:51p
// Checked in $/TemplateVol/src/Volume
// Comment:
//   m_background added
//
// *****************  Version 11  *****************
// User: Jay             Date:  8/09/00  Time:  2:35a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Can dereference the pointer and assign to it.
//
// *****************  Version 10  *****************
// User: Jay             Date:  8/09/00  Time:  2:19a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   ApplyTransform works
//
// *****************  Version 9  *****************
// User: Jay             Date:  8/09/00  Time:  1:33a
// Checked in $/TemplateVol/src/Volume
// Comment:
//   Generic Iterator class added, Resize works fine, TransformCoords to
// be fixed
//
// *****************  Version 8  *****************
// User: Dave            Date:  8/09/00  Time:  1:28a
// Checked in $/TemplateVol/src/Volume
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
// Created LinearVolume.inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/LinearVolume.inl,v 1.8 2006/06/21 17:32:39 dongqing Exp $
// $Id: LinearVolume.inl,v 1.8 2006/06/21 17:32:39 dongqing Exp $
