// $Id: BlockVolume.inl,v 1.6 2006/05/31 16:16:42 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: September 2000
// Author: David dave@viatronix.com
//
// Complete history at bottom of file.

#define FILE_REVISION "$Revision: 1.6 $"


/**
* Default constructor for an empty BlockVolume.
*/
template <class T>
BlockVolume<T>::BlockVolume(const uint4 & blockBitSize) : m_uiBlockBitSize(blockBitSize)
{
  InitMemberVars();
}


/**
* Constructor for building new volumes.
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
* @param uiBlockBitSize number of bits
*/
template <class T>
BlockVolume<T>::BlockVolume(const Triple<uint4> & dim, const uint4 & uiBitsPerVoxel,
                            const Triple<float4> & units,
                            hdr::UnitTypeEnum unitType,
                            const T & background,
                            hdr::ExtensionEnum ext,
                            const std::string & sFileName,
                            const std::string & sTitle,
                            const std::string & sAuthor,
                            const vx::Date & date,
                            const uint4 & uiBlockBitSize)
: hdr::VolAPI<T>(dim, uiBitsPerVoxel, units, 
                 unitType, background, ext, sFileName, sTitle, sAuthor, date),
m_uiBlockBitSize(uiBlockBitSize)
{
  InitMemberVars();
}


/**
* Copy Constructor.
* @param rhs the BlockVolume to copy
*/
template <class T>
BlockVolume<T>::BlockVolume(BlockVolume<T> const & rhs)
{
  RefCountCopy(rhs);
  m_eMemoryLayout = hdr::BLOCK;
}


/**
* Overloaded assignment, use with caution for large volumes.
*/
template <class T>
BlockVolume<T> const & BlockVolume<T>::operator=(BlockVolume<T> const & rhs)
{
  // check for auto-assignment using memory address
  if ( this == &rhs )
  {
    return(*this);
  }

  RefCountCopy(rhs);
  m_eMemoryLayout = hdr::BLOCK;
  return(*this);
}


/**
* Copy all BlockVolume and parent data.
* @param rhs the right hand side.
* @return the result.
*/
template <class T>
void BlockVolume<T>::RefCountCopy(BlockVolume<T> const & rhs)
{
  // assign all header variables
  Copy(rhs);

  m_uiBlocksize    = rhs.m_uiBlocksize;
  m_uiBlockBitSize = rhs.m_uiBlockBitSize;
  m_uiBlockBitsX   = rhs.m_uiBlockBitsX;
  m_uiBlockBitsXY  = rhs.m_uiBlockBitsXY;
  m_uiRmHighBits   = rhs.m_uiRmHighBits;

  m_mBlocks.Clear();

  m_mBlocks.Resize(rhs.m_mBlocks.Size(), Block<T>(m_uiBlocksize, m_background)); 

  // link all other block data to the first
  for ( uint4 k(0); k<m_mBlocks.Size(); ++k )
  {
    m_mBlocks[k] = rhs.m_mBlocks[k];
  }
}


/**
* Merge to volumes. Meaning, after merging all equal blocks are shared in memory.
* @param otherVolume This is the other volume
*/
template <class T>
void BlockVolume<T>::Merge(BlockVolume<T> const & otherVolume)
{
  uint4 i;
  uint4 counter = 0;
  bool isEqual;
  // Check if merging is possible
  if ( (m_mBlocks.Size() == otherVolume.m_mBlocks.Size()) )
  {
    //Run through all the blocks an check if they are equal, if yes merge them.
    for ( i=0; i < m_mBlocks.Size(); i++ )
    {
      if ( m_mBlocks[i].Merge(otherVolume.m_mBlocks[i]) )
      {
        counter++;
      }
    }    
    LogDbg(ToAscii(counter)+" Blocks are deleted. => Saved "+ToAscii((counter*m_uiBlocksize)/(1024*1024)) + "MB of memory","BlockVolume<T>","Merge");
  }
  else
  {
    LogWrn("Operation not supported on block volumes with different size","BlockVolume<T>","Merge");
  }
} // Merge


/**
* Creates a difference Volume. All blocks of rhs which are different to this, will be
* stored in separately volume. The return value is a pointer to the separately volume. 
* @param rhs This is the other volume
*/
template <class T>
BlockVolume<T> * BlockVolume<T>::CreateDiffVol(BlockVolume<T> const & rhs) 
{
  uint4 i;
  BlockVolume<T> *diffVol; //pointer to the DifferenceVolume.
  if ( (m_mBlocks.Size() == rhs.m_mBlocks.Size()) )
  {
    diffVol = new BlockVolume<T>(m_dimension); //create the DifferenceVolume with the dimensions of this.
    Merge(rhs); //First merge the two Volumens to reduce memory wastage.
    for ( i = 0; i < m_mBlocks.Size() ; ++i )
    {
      if ( m_mBlocks[i] != rhs.m_mBlocks[i] )
      {//search for different blocks
        if ( m_mBlocks[i].isDirty() )
        { // if not background store the block
          diffVol->m_mBlocks[i] = rhs.m_mBlocks[i];
        }
      }
    }
    return(diffVol);
  }
  else
  {
    LogWrn("Unable to create DifferenceVolume! Reason: Number of blocks differ","BlockVolume<T>","CreateDiffVol");    
    return(this);
  }
}

/**
* Destructor.
*/
template <class T>
BlockVolume<T>::~BlockVolume()
{
}



template <class T> 
void BlockVolume<T>::Serialize(SerializedBlockVolume& out)
{
  std::map<const T*, int4 > indexOf;    // maps pointer values to indexes

  // construct map and copy blocks blocks while at it
  uint4 uBlockCount(0);

  for ( int i=0; i<m_mBlocks.Size(); ++i )
  {
    const T * pRawBlock = m_mBlocks[i].Raw();

    if ( m_mBlocks[i].IsDirty() )
    {
      if ( indexOf.find(pRawBlock) != indexOf.end() )
      {   // if map contains index for the block
        out.Index(i) = indexOf[pRawBlock];          
      }
      else
      {                      // encountered block for the first time
        indexOf[pRawBlock] = (out.Index(i) = uBlockCount);
        out.CopyBlockIn(uBlockCount, reinterpret_cast<void*const>(const_cast<T*>(pRawBlock)));          
        ++uBlockCount;
      }        
    }
    else
    {
      indexOf[m_mBlocks[i].Raw()] = out.Index(i) = -1; // Magic numbers are a bad coding practice 
      // but we really have to get this product out
      // quickly, so if you are reading this you         
      // should fix it.                                                      
    }
  }    

  //pParamBuffer = pBuffer;
  //pParamBufferSize = uiBufferByteSize;
}

/**
  * Write using the new V3D File format.
  * If no file name is specified the same
  * name is used from the call to Read().  The filename specified should end
  * with a valid ExtensionEnum from that enumeration.
  * @param sFileName the file name to write, default value is the same as the name
  * of the input file.
  */
template <class T> void BlockVolume<T>::WriteV2(std::string sFileName )
{
  if ( sFileName == "NULL" )
  {
    //use fileName stored from Read()
    sFileName = m_sFileName;
  }

  if ( sFileName.size() <= 4 )
  {
    std::string newName = sFileName + vx::VolumeExt;      
    LogWrn(sFileName + " appears invalid file name, storing as " + newName,"BlockVolume<T>","WriteV2");      
    sFileName = newName;
  }

  std::string suffix = sFileName.substr(sFileName.size() - 4, 4);
  hdr::ExtensionEnum extension = GetExtension(suffix);   //convert to enum


  const uint4 uBlockArraySize = AllocatedBlocks();  
  const uint4 uBlockByteSize = sizeof(T) * m_uiBlocksize; // (size of a voxel) * (num voxels per block)

  SerializedBlockVolume srlBlockVol(uBlockByteSize, m_mBlocks.Size(), AllocatedBlocks());

  Serialize(srlBlockVol);

  m_headerMap.SetHdrVar("uAllocatedBlocks", AllocatedBlocks(), "number of blocks allocated in memory");
  m_headerMap.SetHdrVar("uBllockBitSize", m_uiBlockBitSize, "number of bits required to store block size");  

  V3DFile img;

  img.SetFileVersion(1, 1);  
  img.SetDescription(BuildAsciiHeader(hdr::BLOCK, extension));
  img.SetTitle(m_sTitle);    
  img.SetComment(m_headerMap.ToString());
  img.SetData(MemoryBuffer(srlBlockVol.GetBuffer(), srlBlockVol.GetBufferSize()));    
  img.Write(sFileName); 

}


template <class T> void BlockVolume<T>::Write(std::string sFileName) 
{
  BlockVolume<T>::WriteV1(sFileName);
}

/**
  * Reads the file specified using the new V3D File format.
  * @param sFileName the name of the volume to read.
  */
template <class T> void BlockVolume<T>::Read(std::string sFileName) 
{
  if ( sFileName.size() <= 4 )
  {
    throw ex::VException(LogRec("IO error","BlockVolume<T>","Read"));
  }

  std::string suffix = sFileName.substr(sFileName.size() - 4, 4);
  hdr::ExtensionEnum extension = GetExtension(suffix);   //convert to enum

  V3DFile img;
  try
  {
    img.SetSkipData(true);
    img.Read(sFileName, std::make_pair( 0.0F, 100.0F ) ); // could throw many exceptions
  }
  catch ( ex::VException & error )
  {
    LogRec(error.GetLogRecord().GetTextMessage(),"BlockVolume<T>","Read");
    throw;
  }

  m_sFileName = sFileName;

  try
  {
    std::string header = img.GetDescription();
    int4 memLayout = ReadCommonHeader(header);

    std::string extraVars = img.GetComment();    
    m_headerMap = VarMap(extraVars);        // convert string back to map

    //check suffix matches m_extension
    if ( extension != m_extension )
    {
      LogWrn("File name " + m_sFileName + " does not match extension label within file of " + ExtensionStr[m_extension].c_str(),"BlockVolume<T>","Read");      
    }

    hdr::MemoryLayoutEnum ml = (hdr::MemoryLayoutEnum) memLayout;  
    switch ( ml )
    {
    case hdr::LINEAR:
      {
        LogDbg("CONVERTING FROM LINEAR TO BLOCK","BlockVolume<T>","Read");       
        LinearVolume<T> volume;
        try
        {
          volume.Read(m_sFileName);
        }
        catch ( ex::IOException )
        {
          *this = BlockVolume<T>();
          throw;
        }

        LogDbg("Assigning *this","BlockVolume<T>","Read");
        *this = BlockVolume<T>(volume, true);
        LogDbg("Converted from linear volume format","BlockVolume<T>","Read");
        break;
      }
    case hdr::BLOCK:
      {        
        // parse vdAt chunk
        if ( img.GetFileVersion() == std::pair<int4, int4>(1, 0) )
        {
          img.SetSkipData(false);
          img.Read(sFileName, std::make_pair( 0.0F, 100.0F ) );

          MemoryBuffer & diskDataBuffer = img.GetDataBuffer();
          if ( diskDataBuffer.GetSize() == 0 )
          {
            *this = BlockVolume<T>();
            throw ex::DataCorruptionException(LogRec("could not find volume data chunk","BlockVolume<T>","Read"));            
          }
          uint1 * data = diskDataBuffer.GetDataPtr();
          uint4 dataLength = diskDataBuffer.GetSize();
          ReadDataChunk(data, dataLength);       

          // if old file version ... rewrite
          // Write(sFileName);  
        }
        else if ( img.GetFileVersion() == std::pair<int4, int4>(1, 1) )
        {
          uint4 uAllocatedBlocs(0);

          m_headerMap.GetHdrVar("uBllockBitSize", m_uiBlockBitSize);          
          m_headerMap.GetHdrVar("uAllocatedBlocks", uAllocatedBlocs);

          InitMemberVars();                     // allocates Block<T> array

          uint4 uIndexArraySize(m_mBlocks.Size());          
          const uint4 uBlockByteSize = sizeof(T) * m_uiBlocksize; // (size of voxel) * (voxels per block)

          const uint4 uRefBlockByteSize = uBlockByteSize + 32;  // actual size of an allocated block           
          const uint4 uRefBlockArrayByteSize = uAllocatedBlocs * uRefBlockByteSize;

          uint1 * const pRefBlockArray = new uint1[uRefBlockArrayByteSize];

          SerializedBlockVolume srlBlockVol(uBlockByteSize, uIndexArraySize, uAllocatedBlocs);

          img.SetSkipData(false);
          img.SetData(MemoryBuffer(srlBlockVol.GetBuffer(), srlBlockVol.GetBufferSize()));
          img.Read(sFileName, std::make_pair( 0.0F, 100.0F ) );


          // copy blocks out of serialized locations to custom buffer
          for ( int j=0; j<uAllocatedBlocs; j++ )
          {
            srlBlockVol.CopyBlockOut(pRefBlockArray + j * uRefBlockByteSize + 32, j);
          }

          // convert indexes into Block<T> objects          
          for ( int4 j=0; j<uIndexArraySize; j++ )
          {
            if ( srlBlockVol.Index(j) >= 0 )
            {
              T* pData = reinterpret_cast<T*>(pRefBlockArray + srlBlockVol.Index(j) * uRefBlockByteSize + 32);
              m_mBlocks[j] = Block<T>(pData, 2, m_uiBlocksize, true);
            }
          }          

        }
        else
        {
          // TODO: throw somethig
        }

        break;
      }
    case hdr::REC_BLOCK:
      {
        LogDbg("<BlockVolume<T>::Read> RECURSIVE_BLOCKING TO Block CONVERSION NOT SUPPORTED","BlockVolume<T>","Read");
        break;
      }
    } 
  }
  catch ( ex::VException & error )
  {
    LogRec(error.GetLogRecord().GetTextMessage(),"BlockVolume<T>","Read");
    throw;
  }
  catch ( ... )
  {
    throw ex::VException(LogRec("error loading data files - read block volume","BlockVolume<T>","Read"));
  }
  return;
} 


/**
 * If no file name is specified the same
 * name is used from the call to Read().  The filename specified should end
 * with a valid ExtensionEnum from that enumeration.
 * @param sFileName the file name to write, default value is the same as the name
 * of the input file.
 */
template <class T> void BlockVolume<T>::WriteV1(std::string sFileName)
{
  std::stringstream tt; 
  if ( sFileName == "NULL" )
  {  //use fileName stored from Read()
    sFileName = m_sFileName;
  }
  if ( sFileName.size() <= 4 )
  {
    std::string newName = sFileName + vx::VolumeExt;
    tt <<"<BlockVolume::Write> sFileName "+ sFileName+ " appears invalid, storing as " <<  newName <<std::endl;
    LogWrn(tt.str(),"BlockVolume<T>","Write");
    tt.str("");

    sFileName = newName;
  }
  std::string suffix = sFileName.substr(sFileName.size() - 4, 4);
  hdr::ExtensionEnum extension = GetExtension(suffix);   //convert to enum

  // build volume chunk
  // allocate max space possibly needed (we buy nice computers)
  uint8 bufferSize = (AllocatedBlocks() * (m_uiBlocksize * sizeof(T) + 2)) + ((m_mBlocks.Size() - AllocatedBlocks()) * 2) + 1000;
  int1 * buffer = new int1 [bufferSize];
  std::ostrstream dataStream(buffer, bufferSize);
  uint8 compressedSize = 0;
  if ( (compressedSize = WriteVolDataToStream(dataStream)) < 0 )
  {
    delete [] buffer;
    throw ex::IOException(LogRec("Compressed size less than zero","BlockVolume","Write"));
    return;
  }

  // Set the text chunks for the image
  V3DFile img;
  img.SetDescription( BuildAsciiHeader( hdr::BLOCK, extension ) );
  img.SetTitle( m_sTitle );
  img.SetComment( m_headerMap.ToString() );

  uint1 name[3] = {'v', 'o', 'L'};
  uint1 * const pBuffer = reinterpret_cast< uint1 * const >( dataStream.str() );
  MemoryBuffer memoryBuffer( pBuffer, compressedSize );
  img.SetData( memoryBuffer );
  img.Write(sFileName);

  delete [] buffer;

  return;
} 





/**
 * Sets the value a the location specified without bound checking.
 * 
 * @param pos    position at which to set the voxel
 * @param value  value to be assigned to the voxel.
 */
template <class T>
void BlockVolume<T>::SetVoxelRaw(const Point<uint2> & pos, T value)
{
  uint4 block = GetBlock(pos);
  /*
  if ( m_mBlocks[block] == m_vBaseBlock )
  {
  
    if ( value == m_background )
    {
    // make sure we don't waste space
    return;
    }
    
    // first non background voxel in this block -> allocate new block
    m_mBlocks[block] = *(new Block<T>(m_uiBlocksize));
    SetToBackground(m_mBlocks[block]);
    }
  */ 
  uint2 index = GetBlockIndex(pos);
  m_mBlocks[block].Set(value,index);
}


/**
* Gets the voxel at the specified location.
* @param location the location in the volume to get the voxel.
*/
template <class T>
const T & BlockVolume<T>::GetVoxelRaw(const Point<uint2> & location) const
{
  return(m_mBlocks[GetBlock(location)].At(GetBlockIndex(location)));
}


/**
* Computes and returns the resampled value at the location specified.
* @param point the location to compute the resampled value.
* @return the resampled value.
*/
template <class T>
float4 BlockVolume<T>::GetSample(const Point<float4> & point) const
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
  uint4 x = uint4(point.m_x);
  uint4 y = uint4(point.m_y);
  uint4 z = uint4(point.m_z);

  // weights for tri-linear interpolation
  float4 weightX = point.m_x - x;
  float4 weightY = point.m_y - y;
  float4 weightZ = point.m_z - z;

  // voxels for lower left front (llf), lower left back (llb), etc
  int2 illf(GetVoxelRaw(Point<uint2>(x  , y  , z  )));
  int2 illb(GetVoxelRaw(Point<uint2>(x  , y  , z+1)));
  int2 iulf(GetVoxelRaw(Point<uint2>(x  , y+1, z  )));
  int2 iulb(GetVoxelRaw(Point<uint2>(x  , y+1, z+1)));
  int2 ilrf(GetVoxelRaw(Point<uint2>(x+1, y  , z  )));
  int2 ilrb(GetVoxelRaw(Point<uint2>(x+1, y  , z+1)));
  int2 iurf(GetVoxelRaw(Point<uint2>(x+1, y+1, z  )));
  int2 iurb(GetVoxelRaw(Point<uint2>(x+1, y+1, z+1)));

  // tri-linear interpolation
  float4 fllm = weightZ * illb + (1 - weightZ) * illf;
  float4 fulm = weightZ * iulb + (1 - weightZ) * iulf;
  float4 flrm = weightZ * ilrb + (1 - weightZ) * ilrf;
  float4 furm = weightZ * iurb + (1 - weightZ) * iurf;
  float4 fmlm = weightY * fulm + (1 - weightY) * fllm;
  float4 fmrm = weightY * furm + (1 - weightY) * flrm;
  float4 fmmm = weightX * fmrm + (1 - weightX) * fmlm;

  return(fmmm);
} // GetResampledValue


/**
* Returns the XYZ position given by the two indices.
*/
template <class T>
Point<uint2> BlockVolume<T>::GetPosFromIndices(const uint4 & block, const uint2 & index) const
{
  // example dimension 2^7
  // X = X6 X5 X4 X3 X2 X1 X0
  // Y = Y6 Y5 Y4 Y3 Y2 Y1 Y0
  // Z = Z6 Z5 Z4 Z3 Z2 Z1 Z0
  // block B = B11 B10 B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
  //         =  Z6  Z5 Z4 Z3 Y6 Y5 Y4 Y3 X6 X5 X4 X3
  // index I = I8 I7 I6 I5 I4 I3 I2 I1 I0
  //         = Z2 Z1 Z0 Y2 Y1 Y0 X2 X1 X0
  // so X =  B3  B2 B1 B0 I2 I1 I0
  // so Y =  B7  B6 B5 B4 I5 I4 I3
  // so Z = B11 B10 B9 B8 I8 I7 I6
  // Our task is to reconstruct (X,Y,Z) given (B,I)
  uint2 mask = pow(2, m_uiBlockBitsX) - 1;
  uint2 x = ((block & mask) << 3) | (index & 7);
  uint2 y = (((block >> m_uiBlockBitsX) & mask) << 3) | (index & (7 << 3));
  uint2 z = (((block >> m_uiBlockBitsXY) & mask) << 3) | (index & (7 << 6));
  return(Point<uint2>(x,y,z));
}


/**
* Computes the total memory usage of the block volume including overhead.
* @return the number of bytes used.
*/
template <class T>
const uint4 BlockVolume<T>::GetMemoryUsage() const
{
  uint4 nBlocks = AllocatedBlocks() + 1;
  return(nBlocks * m_uiBlocksize * sizeof(T) + (m_mBlocks.Size() * 4));
}


/**
* Print Block Volume statistics.
* @param os the stream to print to.
*/
template <class T>
void BlockVolume<T>::Stats(std::ostream & os) const
{
  os << "** BLOCKVOLUME STATISTICS **" << std::endl;
  os << GetMemoryUsage() / static_cast<uint8>(1024.0 * 1024)<< " MB used"<<std::endl;
  os << "TOTAL BLOCKS: "<< m_mBlocks.Size() << " UNIQUE BLOCKS ALLOCATED: ";
  os << AllocatedBlocks() + 1<< std::endl;
  os << "BLOCK BIT SIZE:" << m_uiBlockBitSize << " VOXELS PER BLOCK: ";
  os << m_uiBlocksize << std::endl;
}


/**
* Test the BlockVolume class.
* @param os the stream to send results to.
* @return true for success else false.
*/
template <class T>
bool BlockVolume<T>::TestByType(std::ostream & os)
{
  bool ok(true);
  bool totalOk(true);

  os << "TEST: VOLAPI ..." << std::endl;
  string author = "Brett Ashley";
  string title1 = "The Sun Also Rises";
  string sFileName = "TheTest.vol";
  Triple<uint4> dim(128,128,128);
  Triple<float4> units(1.0,1.0,1.0);

  Volume<BlockVolume<T>, T> apiTest(dim, sizeof(T) * 8, units, hdr::METER, 46, hdr::VOL,
                                    sFileName, title1, author);

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
  ok = ok && (apiTest.InsideVolume(Point<uint2>(dim.m_x, dim.m_y, dim.m_z)) == false);
  //Point<uint2> inside(20,13,10);
  //ok = ok && (apiTest.InsideVolume(inside) == true);
  apiTest.AsOstream(os); os << std::endl;
  uint8 realNumber = dim.m_x * dim.m_y * dim.m_z;
  ok = ok && (apiTest.GetNumberOfVoxels() == realNumber);

  if ( ok ) os << "TEST PASSED" << std::endl << std::endl;
  else os << "TEST FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;




  os << "TEST: SetVoxelRaw() <==> GetVoxelRaw() ..." << std::endl;

  Volume<BlockVolume<T>, T> vol(dim, sizeof(T) * 8, units,
                                hdr::METER, 46, hdr::VOL,
                                "Test.vol", title1, author);


  // os << "TEST: READINGGGGGGGGGGGGGGGGGGGGGGGGGGG ..." << std::endl;
  //vol.Read("DILBERT_1S_0.csi");



  vol.SetVoxelRaw(Point<uint2>(0,0,0),(T) 65);
  vol.SetVoxelRaw(Point<uint2>(49,49,49),(T) 91);
  vol.SetVoxelRaw(Point<uint2>(4,4,4),(T) 66);
  vol.SetVoxelRaw(Point<uint2>(4,4,5),(T) 67);
  vol.SetVoxelRaw(Point<uint2>(4,5,4),(T) 68);
  vol.SetVoxelRaw(Point<uint2>(4,5,5),(T) 69);
  vol.SetVoxelRaw(Point<uint2>(5,4,4),(T) 70);
  vol.SetVoxelRaw(Point<uint2>(5,4,5),(T) 71);
  vol.SetVoxelRaw(Point<uint2>(5,5,4),(T) 72);
  vol.SetVoxelRaw(Point<uint2>(0,0,5),(T) 73);


  // vol.PrintAllRaw(std::cout);

  ok = ok && ((T)65 == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
  ok = ok && ((T)91 == vol.GetVoxelRaw(Point<uint2>(49,49,49)));
  ok = ok && ((T)66 == vol.GetVoxelRaw(Point<uint2>(4,4,4)));
  ok = ok && ((T)67 == vol.GetVoxelRaw(Point<uint2>(4,4,5)));
  ok = ok && ((T)68 == vol.GetVoxelRaw(Point<uint2>(4,5,4)));
  ok = ok && ((T)69 == vol.GetVoxelRaw(Point<uint2>(4,5,5)));
  ok = ok && ((T)70 == vol.GetVoxelRaw(Point<uint2>(5,4,4)));
  ok = ok && ((T)71 == vol.GetVoxelRaw(Point<uint2>(5,4,5)));
  ok = ok && ((T)72 == vol.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T)73 == vol.GetVoxelRaw(Point<uint2>(0,0,5)));

  if ( ok ) os << "TEST  PASSED" << std::endl << std::endl;
  else os << "TEST  FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;

  cout << "WRITING ..."; 
  vol.Write("TheTest.vol");
  cout << " (DONE)" << endl; 

  vol.SetVoxelRaw(Point<uint2>(0,0,0), 0);
  vol.SetVoxelRaw(Point<uint2>(49,49,49), 0);
  vol.SetVoxelRaw(Point<uint2>(4,4,4), 0);
  vol.SetVoxelRaw(Point<uint2>(4,4,5), 0);
  vol.SetVoxelRaw(Point<uint2>(4,5,4), 0);
  vol.SetVoxelRaw(Point<uint2>(4,5,5), 0);
  vol.SetVoxelRaw(Point<uint2>(5,4,4), 0);
  vol.SetVoxelRaw(Point<uint2>(5,4,5), 0);
  vol.SetVoxelRaw(Point<uint2>(5,5,4), 0);
  vol.SetVoxelRaw(Point<uint2>(0,0,5), 0);

  cout << "READING ..."; 
  vol.Read("TheTest.vol");
  cout << " (DONE)" << endl; 
  // vol.PrintAllRaw(std::cout);

  ok = ok && ((T) 65 == vol.GetVoxelRaw(Point<uint2>(0,0,0)));
  ok = ok && ((T) 91 == vol.GetVoxelRaw(Point<uint2>(49,49,49)));
  ok = ok && ((T) 66 == vol.GetVoxelRaw(Point<uint2>(4,4,4)));
  ok = ok && ((T) 67 == vol.GetVoxelRaw(Point<uint2>(4,4,5)));
  ok = ok && ((T) 68 == vol.GetVoxelRaw(Point<uint2>(4,5,4)));
  ok = ok && ((T) 69 == vol.GetVoxelRaw(Point<uint2>(4,5,5)));
  ok = ok && ((T) 70 == vol.GetVoxelRaw(Point<uint2>(5,4,4)));
  ok = ok && ((T) 71 == vol.GetVoxelRaw(Point<uint2>(5,4,5)));
  ok = ok && ((T) 72 == vol.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T) 73 == vol.GetVoxelRaw(Point<uint2>(0,0,5)));

  if ( ok ) os << "TEST  PASSED" << std::endl << std::endl;
  else os << "TEST  FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;


  os << "TEST: copy, assignment ..." << std::endl;

  string oldTitle = vol.GetTitle();
  Volume<BlockVolume<T>,T> volCopy = vol;
  ok = ok && ((T) 72 == volCopy.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T) 73 == volCopy.GetVoxelRaw(Point<uint2>(0,0,5)));
  ok = ok && (volCopy.GetTitle() == oldTitle);

  Volume<BlockVolume<T>,T> volCopy2(volCopy);
  ok = ok && ((T) 72 == volCopy.GetVoxelRaw(Point<uint2>(5,5,4)));
  ok = ok && ((T) 73 == volCopy.GetVoxelRaw(Point<uint2>(0,0,5)));
  ok = ok && (volCopy.GetTitle() == oldTitle);

  if ( ok ) os << "TEST PASSED" << std::endl << std::endl;
  else os << "TEST FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;

  os << "TEST: volume iterators ..." << std::endl;
  //  vol = BlockVolume<T>(Triple<uint4>(3,3,3));
  //  vol.SetVoxelRaw(Triple<uint2>(0,0,0),2);
  //  vol.SetVoxelRaw(Triple<uint2>(2,1,0),3);

  BlockVolume<T>::VolIterator iter(vol.GetVolIterator(Point<uint2>(0,0,0)));

  int4 noVoxels0 = 0;
  int4 noVoxels1 = 0;

  int4 countNonBackgroundVoxels0 = 0;
  int4 countNonBackgroundVoxels1 = 0;
  int4 countNonBackgroundVoxels2 = 0;
  for ( ; !iter.End(); ++iter )
  {
    //os << iter.GetPos() << " "<< iter.GetVoxel() << " == " << *iter << endl;
    ++noVoxels0;
    if ( *iter != vol.GetBackground() ) ++countNonBackgroundVoxels0;
  } 
  iter.MoveAbs(Point<uint2>(0,0,0));
  for ( ; !iter.End(); iter.NextZYX() )
  {
    ++noVoxels1;
    if ( *iter != vol.GetBackground() ) ++countNonBackgroundVoxels1;
  }
  iter.MoveAbs(Point<uint2>(0,0,0));
  for ( ; !iter.End(); iter.NextNonZero() )
  {
    ++countNonBackgroundVoxels2;
  }

  ok &= (noVoxels0 == noVoxels1);
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;
  ok &= (noVoxels0 == vol.GetNumberOfVoxels());
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;
  ok &= (noVoxels1 == vol.GetNumberOfVoxels());
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;
  ok &= (countNonBackgroundVoxels0 == countNonBackgroundVoxels1);
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;
  ok &= (countNonBackgroundVoxels0 == countNonBackgroundVoxels2);
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;
  ok &= (countNonBackgroundVoxels1 == countNonBackgroundVoxels2);
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;

  os << "Background color: :" << vol.GetBackground() << endl;
  os << "Number of Voxels: " << noVoxels0 << " == " << noVoxels1 << " == " << vol.GetNumberOfVoxels() << endl;
  os << "Number of non background voxels: " << countNonBackgroundVoxels0 << " == " << countNonBackgroundVoxels1 << " == " << countNonBackgroundVoxels2 << endl;

  if ( ok ) os << "volume iterators TEST 8 PASSED" << std::endl << std::endl;
  else os << "volume iterators TEST 8 FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;

  os << "TEST: Still testing iterator ..." << std::endl;

  vol.SetVoxelRaw(Point<uint2>(0,0,1),(T) 8);
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;

  iter.MoveAbs(Point<uint2>(0,0,0));
  os << iter.PeekNextZ() << " == "<< vol.GetVoxelRaw(Point<uint2>(0,0,1)) << endl;

  iter.MoveAbs(Point<uint2>(0,0,0));
  //T a,b;
  //a = iter.PeekNextZ();
  //b = vol.GetVoxelRaw(Point<uint2>(0,0,1));
  //ok &= (a==b);

  ok = true;
  //ok &= (  (a= iter.PeekNextZ()) == (b = vol.GetVoxelRaw(Point<uint2>(0,0,1))));
  ok &= ( iter.PeekNextZ() == iter.PeekNextZ());
  if ( ok ) os << "OK :-)" << std::endl;
  else os << "NOT OK:-(" << std::endl;  
  ok &= ( vol.GetVoxelRaw(Point<uint2>(0,0,1)) == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
  if ( ok ) os << "OK :-)" << std::endl;
  else os << "NOT OK:-(" << std::endl;  
  ok &= ( iter.PeekNextZ() == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
  if ( ok ) os << "OK :-)" << std::endl;
  else os << "NOT OK:-(" << std::endl;  
  ok &= ( 8 == vol.GetVoxelRaw(Point<uint2>(0,0,1)));
  if ( ok ) os << "OK :-)" << std::endl;
  else os << "NOT OK:-(" << std::endl;  
  ok &= ( iter.PeekNextZ() == 8);
  if ( ok ) os << "OK :-)" << std::endl;
  else os << "NOT OK:-(" << std::endl;  

  ok = true;
  ok &= (iter.PeekNextY() == vol.GetVoxelRaw(Point<uint2>(0,1,0)));
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;
  ok &= (iter.PeekNextX() == vol.GetVoxelRaw(Point<uint2>(1,0,0)));
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;

  iter.IncX(); iter.IncY(); iter.IncZ();
  iter.IncX(); iter.IncY(); iter.IncZ();

  ok &= (iter.GetVoxel() == vol.GetVoxelRaw(Point<uint2>(2,2,2)));
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;
  iter.DecX(); iter.DecY(); iter.DecZ();

  ok &= (iter.GetVoxel() == vol.GetVoxelRaw(Point<uint2>(1,1,1)));
  if ( ok ) os << ":-)" << std::endl;
  else os << ":-(" << std::endl;

  if ( ok ) os << "TEST 9 PASSED" << std::endl << std::endl;
  else os << "TEST 9 FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;

  os << "TEST: BLOCKS";
  Block<T> b0(0xFFFF,4);
  Block<T> b1(0xFFFF,4);
  Block<T> b2(0xFFFF,5);

  ok &= b0.DeepCmp(b1);
  if ( ok ) std::cout << ":-)" << std::endl;
  else std::cout << ":-(" << std::endl;
  ok &= b1.DeepCmp(b0);
  if ( ok ) std::cout << ":-)" << std::endl;
  else std::cout << ":-(" << std::endl;
  ok &= b0.DeepCmp(b0);
  if ( ok ) std::cout << ":-)" << std::endl;
  else std::cout << ":-(" << std::endl;
  ok &= (!b0.DeepCmp(b2));
  if ( ok ) std::cout << ":-)" << std::endl;
  else std::cout << ":-(" << std::endl;
  ok &= b0.Merge(b1);
  if ( ok ) std::cout << ":-)" << std::endl;
  else std::cout << ":-(" << std::endl;
  ok &= b0.DeepCmp(b1);
  if ( ok ) std::cout << ":-)" << std::endl;
  else std::cout << ":-(" << std::endl;

  if ( ok ) os << "TEST BLOCKS PASSED" << std::endl << std::endl;
  else os << "TEST BLOCKS FAILED" << std::endl << std::endl;
  totalOk &= ok; ok = true;

  if ( totalOk ) os <<"COMPLETED BlockVolume CLASS TEST :)"<< std::endl << std::endl << std::endl;
  else os <<"FAILED VOLUME CLASS TEST :(" << std::endl << std::endl << std::endl;
  return(totalOk);
}


/**
* Initialize all data members to default except m_uiBlockBitSize.
*/
template <class T>
void BlockVolume<T>::InitMemberVars()
{
  if ( m_uiBlockBitSize != 3 )
  {
    throw ex::VException(LogRec("BLOCK BIT SIZE SPECIFICATION WAS REMOVED TO MAKE COMMON CASE FAST, DISCUSS WITH INGMAR TO GET IT BACK",
      "BlockVolume","InitMemberVars"));
  }

  if ( m_uiBlockBitSize > 5 )
  {
    LogWrn("Block Bit size max. is 5, use LinearVolume if you want a larger block","BlockVolume","InitMemberVars");
    m_uiBlockBitSize = 5;
  }
  if ( m_uiBlockBitSize == 0 )
  {
    LogWrn("Block bit size must be greater than zero, assigning to 1","BlockVolume","InitMemberVars");
    m_uiBlockBitSize = 1;
  }

  if ( m_uiBlockBitSize >= SmPwr2(m_dimension.m_x) )
  {
    m_uiBlockBitsX = 0;
  }
  else
  {
    m_uiBlockBitsX = SmPwr2(m_dimension.m_x) - m_uiBlockBitSize;
  }

  if ( m_uiBlockBitSize >= SmPwr2(m_dimension.m_y) )
  {
    m_uiBlockBitsXY = m_uiBlockBitsX;
  }
  else
  {
    m_uiBlockBitsXY = m_uiBlockBitsX + SmPwr2(m_dimension.m_y) - m_uiBlockBitSize;
  }

  m_uiRmHighBits = static_cast<uint4>(pow(2.0F, static_cast<float4>(m_uiBlockBitSize))) - 1;

  m_uiBlocksize = static_cast<uint4> (pow(pow(2.0F, static_cast<float4>(m_uiBlockBitSize)), 3.0F));

  // allocate space for lots of instances of Block (not the actual voxels)
  m_mBlocks.Resize(CalculateNBlocks(), Block<T>(m_uiBlocksize, m_background)); 

  m_eMemoryLayout = hdr::BLOCK;
} // InitMemberVars


/**
* Calculate the number of blocks based on dimensions.
* @return number of blocks needed for the volume.
*/
template <class T>
uint4 BlockVolume<T>::CalculateNBlocks()
{
  // calculate bits needed to store all pointers
  int4 totalBits = SmPwr2(m_dimension.m_x) + SmPwr2(m_dimension.m_y) +
                   SmPwr2(m_dimension.m_z) - (3 * m_uiBlockBitSize);
  if ( totalBits < 0 ) totalBits = 0; // blocks are larger than the volume

  return(static_cast<uint4> (pow(2.0F,totalBits) +1));// one extra block as dedicated background block
}


/**
* Writes all volume data to stream.
* @return -n for error, n for success.
*/
template <class T>
uint8 BlockVolume<T>::WriteVolDataToStream(std::ostrstream & os)
{
  // write block bit size
  uint8 bytesWritten = 0;

  os.write( (int1 *) &m_uiBlockBitSize, sizeof(m_uiBlockBitSize));
  bytesWritten += sizeof(m_uiBlockBitSize);

  // compressed buffer
  uint8 destLenLong = m_uiBlocksize * sizeof(T) * 2; // allocate plenty extra
  uint4 destLenUI = m_uiBlocksize * sizeof(T) * 2; 
  uint2 destLen = 0;
  int1 * temp = new int1 [destLenLong];  // convert to vector<char> ??
  uint1 * tempBeginPtr = (uint1 *)(temp);

  if ( lzo_init() != LZO_E_OK )
  {

  }

  lzo_byte * m_lzoWrkmem = new uint1 [LZO1X_1_MEM_COMPRESS];

  int4 compResult;
  for ( uint4 i = 0; i < m_mBlocks.Size() -1; i++ )
  {
    // check if block needs to be written
    if ( !m_mBlocks[i].IsDirty() )
    {
      destLen = 0;
      os.write((int1 *)(&destLen), sizeof(destLen));
      bytesWritten += sizeof(destLen);
    }
    else
    {
      switch ( m_fileCompression )
      {
      case hdr::LZO:
        {
          destLenUI = m_uiBlocksize * sizeof(T) * 2;
          lzo_uint tmp( destLenUI ) ;
          lzo1x_1_compress((uint1 *)(m_mBlocks[i].Raw()), m_uiBlocksize * sizeof(T), tempBeginPtr, &tmp, m_lzoWrkmem);
          destLenUI = tmp;
          destLen = destLenUI;
          os.write((int1 *)(&destLen), sizeof(destLen));
          // write block data
          os.write(temp, destLen);
          bytesWritten += sizeof(destLen) + destLen;
          break;
        }
      case hdr::NONE:
        destLen = m_uiBlocksize * sizeof(T);
        os.write((int1 *)(&destLen), sizeof(destLen));
        os.write((int1 *)(m_mBlocks[i].Raw()), destLen);
        bytesWritten += sizeof(destLen) + destLen;
        break;
      case hdr::ZLIB:
        destLenLong = m_uiBlocksize * sizeof(T) * 2;
        compResult = compress2(tempBeginPtr, reinterpret_cast<unsigned long *>(&destLenLong), (Bytef *) m_mBlocks[i].Raw(), m_uiBlocksize * sizeof(T), 3);
        if ( compResult != Z_OK )
        {
          LogWrn("<BlockVolume::WriteVolDataToStream> ZLIB compression failure","BlockVolume<T>","WriteVolDataToStream");
          return(-1);
        }
        destLen = destLenLong;
        // write length of block
        os.write((int1 *)(&destLen), sizeof(destLen));
        // write block data
        os.write(temp, destLen);
        bytesWritten += sizeof(destLen) + destLen;
        break;
      } // switch
    }
  } // for each block
  delete [] m_lzoWrkmem;
  delete [] temp;

  return(bytesWritten);
}


/**
* Sets all block values to m_background.
*/
template <class T>
void BlockVolume<T>::SetToBackground(Block<T>  &blockPtr)
{
  for ( uint4 i = 0; i < m_uiBlocksize; i++ )
  {
    blockPtr[i] = m_background;
  }
}


template <class T>
void BlockVolume<T>::Resize(const Triple<uint4> & dimension)
{
  if(m_dimension != dimension)
  {
    m_dimension = dimension;
    m_mBlocks.Resize(CalculateNBlocks(), Block<T>(m_uiBlocksize, m_background));   
  }
}


/**
* Write values of private data members to cerr.
*/
template <class T>
void BlockVolume<T>::WritePrivateData()
{
  std::cerr<< "BLOCK BIT SIZE:"<< m_uiBlockBitSize << std::endl;
  std::cerr<< "BLOCK SIZE:"<< m_uiBlocksize << std::endl;
  std::cerr<< "BLOCK BITS X:"<< m_uiBlockBitsX << std::endl;
  std::cerr<< "BLOCK BITS XY:"<< m_uiBlockBitsXY << std::endl;
  std::cerr<< "RM HIGH BITS:" << m_uiRmHighBits << std::endl;
}


/**
* Returns the number of blocks with non-zero data.  Only these blocks are allocated.
* @return number of blocks with data.
*/
template <class T>
const uint4 BlockVolume<T>::AllocatedBlocks() const
{
  uint4 count = 0;
  for ( uint4 i = 0; i < m_mBlocks.Size(); i++ )
  {
    if ( m_mBlocks[i].IsDirty() )
      count++;
  }

  return(count);
}


/**
*  Returns the number of bits needed to store dimension of n. Actually n-1.
* @param n the dimension
* @return the number of bits need to store n-1.
*/
template <class T>
const uint4 BlockVolume<T>::SmPwr2(const uint4 & n) const
{
  // could use log
  uint4 i = 0;
  for ( ; pow(2.0F, static_cast<float4>(i)) < n; i++ )
  {
  }

  return(i);
}


/**
* Returns the block for the xyz point specified.
*/
template <class T>
uint4 BlockVolume<T>::GetBlock(const Point<uint2> & p) const
{
  //return((p.m_x >> m_uiBlockBitSize) | ((p.m_y >> m_uiBlockBitSize) << (m_uiBlockBitsX)) |
  //      ((p.m_z >> m_uiBlockBitSize) << (m_uiBlockBitsXY)));

  return((p.m_x >> 3) | 
         ((p.m_y >> 3)  << (m_uiBlockBitsX))   |
         (uint4((p.m_z >> 3)) << (m_uiBlockBitsXY)));
}


/**
* Returns the index within a block for the point specified.
* @param p the point to find the index.
* @return the index for the point.
*/
template <class T>
uint2 BlockVolume<T>::GetBlockIndex(const Point<uint2> & p) const
{
  //return ((p.m_x & m_uiRmHighBits) | ((p.m_y & m_uiRmHighBits) << m_uiBlockBitSize) |
  //      (((p.m_z & m_uiRmHighBits) << m_uiBlockBitSize ) << m_uiBlockBitSize));
  return( (p.m_x & 7) | 
          ((p.m_y & 7) << 3) |
          ((p.m_z & 7) << 6 ));
}


/**
*
*/
template <class T>
void BlockVolume<T>::ReadDataChunk(uint1 * dataIn, uint4 dataLength)
{
  uint4 blockBitSize = 0;
  std::istrstream data((const char *) dataIn, dataLength);
  data.read((int1 *)(&blockBitSize), sizeof(blockBitSize));

  if ( blockBitSize != m_uiBlockBitSize )
  {
    LogWrn("Block Size changes from read/write not supported, Damn!","BlockVolume<T>","ReadDataChunk");
  }

  InitMemberVars();
  uint2 compBlockSize = 0;
  int4 rc;
  uint4 tempSize = m_uiBlocksize * 2 * sizeof(T);

  int1 * temp = new int1[tempSize]; // convert to vector<int1> ??
  uint1 * otherTemp = new uint1[m_uiBlocksize * sizeof(T)];
  // read all blocks
 
  for ( uint4 i = 0; i < m_mBlocks.Size(); ++i )
  {
    // read next block size
    data.read((int1 *)(&compBlockSize), sizeof(compBlockSize));
    // check if block is empty
    if ( compBlockSize != 0 )
    { // nop if blocks are alreaady set to background in InitMemberVars()
      // remove switch after profiling fully ?? or place outside for()
      switch ( m_fileCompression )
      {
      case hdr::LZO: 
      case hdr::ZLIB:
        {
          data.read(static_cast<int1 *>(temp), compBlockSize);
          rc = Inflate::InflateIt(m_fileCompression, (uint1 *) temp,
                             compBlockSize, static_cast<uint1 *>(otherTemp), m_uiBlocksize * sizeof(T));
          m_mBlocks[i].Assign(reinterpret_cast<T*>(otherTemp), m_uiBlocksize );
          break;
        }
      case hdr::NONE:
        {
          uint1 * buf = new uint1[compBlockSize];
          data.read((int1 *)(buf), compBlockSize);
          m_mBlocks[i].Assign(reinterpret_cast<T*>(buf),m_uiBlocksize);
          delete [] buf;
          break;
        }
      }  // switch
    }
  }  // for nBlocks

  delete [] otherTemp;
  delete [] temp;
}

#undef FILE_REVISION

// complete revision history:
// $Log: BlockVolume.inl,v $
// Revision 1.6  2006/05/31 16:16:42  geconomos
// porting to visual studio 2005
//
// Revision 1.5  2006/03/29 19:51:25  geconomos
// changed Inflate methods to static
//
// Revision 1.4  2006/02/09 14:01:50  geconomos
// updated external libraries
//
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
// Revision 3.7  2003/05/16 11:44:24  michael
// code review
//
// Revision 3.6  2003/04/18 15:59:54  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 3.5  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.4.2.1  2002/04/02 22:19:16  jmeade
// Fixed re-throw and LogRecord statements.
//
// Revision 3.4  2001/12/24 16:46:48  ingmar
// moved SerializedBlockVolume into its own file
//
// Revision 3.3  2001/11/09 01:23:14  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/10/31 01:11:20  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding the volumes
//
// Revision 3.2  2001/10/31 00:45:57  soeren
// removed memeory leaks and improved resetting of Studybrowser regarding
// the volumes
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.4   Oct 02 2001 17:44:58   ingmar
// fixed misplaced "else" and LogDbg in Merge()
// 
//    Rev 2.3   Sep 23 2001 00:29:44   pagron
// fixed a couple of bugs... primarily Blocks were not getting deleted where they shouldn't have been 
// 
//    Rev 2.2   Sep 21 2001 12:24:16   pagron
// a temporary fixup: disabled rewriting of files with old versions
// 
//    Rev 2.1   Sep 20 2001 20:35:48   pagron
// changes to block volume implementation will reduce memory fragmentation
// 
//    Rev 2.0   Sep 16 2001 23:47:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:00   ingmar
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
// Revision 1.1  2001/09/01 00:12:18  soeren
// renamed volume  directory
//
// Revision 1.85  2001/08/31 18:53:40  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.84  2001/08/28 23:58:03  ingmar
// std::vector<Block<T >, BlockAllocator<Block<T> > > -> BlockVector<T>
//
// Revision 1.83  2001/08/19 22:28:11  ingmar
// added GetSample()
//
// Revision 1.82  2001/08/14 21:01:50  liwei
// Comitted for huamin to compile.
//
// Revision 1.81  2001/08/14 19:57:12  ingmar
// improved logging
//
// Revision 1.80  2001/08/13 00:24:26  ingmar
// GetMessageString() -> GetLogRecord().GetTestMessage()
//
// Revision 1.79  2001/08/07 17:03:39  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.78  2001/07/26 20:29:23  binli
// ID 89: exception handling.
//
// Revision 1.77  2001/07/25 01:31:22  tushar
// fixed some compiler errors (logger)
//
// Revision 1.76  2001/07/18 20:53:17  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.75  2001/07/16 22:56:28  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.74  2001/07/13 19:51:43  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.73  2001/07/12 18:30:59  frank
// Avoided using an intermediate buffer for uncompressed volume data during read.
//
// Revision 1.72  2001/07/12 17:09:45  frank
// Properly read block volumes with the new V3D file format.
//
// Revision 1.71  2001/07/09 16:52:48  frank
// Added support for the new V3D file format.
//
// Revision 1.70  2001/06/20 00:29:11  liwei
// Bug fixing for slice iterators
//
// Revision 1.69  2001/06/14 23:53:41  liwei
// Bookkeeping the information of memory layout and datatype
//
// Revision 1.68  2001/05/20 23:49:02  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.67  2001/04/24 03:56:18  dave
// read should fragment disk less
//
// Revision 1.66  2001/04/19 20:59:04  dave
// updated variable names
//
// Revision 1.65  2001/04/19 15:55:31  jmeade
// removed unreferenced vars
//
// Revision 1.64  2001/04/13 00:33:17  jmeade
// file extensions
//
// Revision 1.63  2001/04/12 18:53:18  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.62  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.61  2001/04/10 18:18:16  dmitry
// updated datatypes
//
// Revision 1.60  2001/04/10 16:13:25  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.59  2001/04/09 22:25:43  dave
// changing from Abort to use exceptions
//
// Revision 1.58  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.57  2001/04/06 13:03:58  frank
// Reduced the number of warning messages in V1k compilation.
//
// Revision 1.56  2001/04/03 05:47:00  soeren
// added documentation
//
// Revision 1.55  2001/04/01 20:41:12  ingmar
// removed obsolete Debug message
//
// Revision 1.54  2001/03/31 18:38:29  ingmar
// added sample \todo and \bug comment
//
// Revision 1.53  2001/03/31 05:34:38  ingmar
// uint8 -> uint4
//
// Revision 1.52  2001/03/30 18:50:04  dmitry
// Updated datatypes.
//
// Revision 1.51  2001/03/30 00:44:52  soeren
// BlockVolume now powered by blocks with Reference Counting
//
// Revision 1.50  2001/03/23 14:20:17  soeren
// added test (disabled) function for merge,
//
// Revision 1.49  2001/03/22 18:54:14  soeren
// Beautified Test function
//
// Revision 1.48  2001/03/22 14:43:12  soeren
// optimized merge function
//
// Revision 1.47  2001/03/21 23:49:11  soeren
// changed cout message
//
// Revision 1.46  2001/03/21 23:24:31  soeren
// changed messages
//
// Revision 1.45  2001/03/21 22:30:35  soeren
// added merge function
//
// Revision 1.44  2001/02/12 03:54:44  ingmar
// code cosmetics
//
// Revision 1.43  2001/01/26 04:22:00  dave
// tryin' understand skel stage
//
// Revision 1.42  2000/12/12 18:34:12  dave
// corrected VolAPI constructor
//
// Revision 1.41  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.40  2000/11/22 06:43:03  dave
// Updated PNGImage class
//
// Revision 1.39  2000/11/17 00:28:40  dave
// remove syntax problems for gcc
//
// Revision 1.38  2000/11/16 17:34:35  frank
// Back to non-specialized
//
// Revision 1.37  2000/11/16 17:32:17  frank
// Specialized block iterator and GetPosFromIndices function
//
// Revision 1.36  2000/11/16 17:19:48  dave
// added native BlockVolume VolIterator
//
// Revision 1.35  2000/11/10 17:34:05  dave
// impl. API changes for VolIterator
//
// Revision 1.34  2000/11/08 19:46:36  frank
// Added CountInstancesOfValue function for quick native counting
//
// Revision 1.33  2000/11/05 00:06:15  dave
// minor update to InitMemberVars()
//
// Revision 1.32  2000/11/03 23:58:42  dave
// debug conversion to uint
//
// Revision 1.31  2000/11/03 22:19:07  dave
// debuggin'
//
// Revision 1.30  2000/11/03 17:02:25  dave
// converted back
//
// Revision 1.29  2000/11/03 03:14:07  ingmar
// fixed some gcc syntax errors and warnings
//
// Revision 1.28  2000/11/02 19:48:08  dave
// Dimensions changed to Triple<short> from uint
//
// Revision 1.27  2000/11/02 04:17:47  dave
// misc updates
//
// Revision 1.26  2000/11/01 21:11:29  dave
// LZO default, GetDataType() fully removed, re-order Vol. declaration
//
// Revision 1.25  2000/10/31 21:16:42  dave
// changed VOLAPI to use ZLIB
//
// Revision 1.24  2000/10/31 20:12:46  dave
// *** empty log message ***
//
// Revision 1.23  2000/10/31 17:16:02  dave
// template<bool> corrected, LZO kinda added
//
// Revision 1.22  2000/10/26 13:58:33  mkontak
// improved code comments
//
// Revision 1.21  2000/10/26 03:20:24  ingmar
// fixed missing std
//
// Revision 1.20  2000/10/26 02:48:46  ingmar
// added Set/GetVoxel for Point<short>
//
// Revision 1.19  2000/10/25 23:58:36  ingmar
// fixed block read/write inconsistency
//
// Revision 1.18  2000/10/25 14:55:56  ingmar
// removed syntax errors found by gcc
//
// Revision 1.17  2000/10/25 14:05:43  ingmar
// put GetDataPtr back
//
// Revision 1.16  2000/10/25 03:00:24  ingmar
// removed collisions
//
// Revision 1.15  2000/10/25 01:49:28  dave
// no message
//
// Revision 1.14  2000/10/25 01:35:24  ingmar
// removed VxIterator and improved VxVolIterator and RegGrow6Iterator
//
// Revision 1.13  2000/10/24 02:56:46  ingmar
// removed VxIterator and added the functionality to the VolIterator
//
// Revision 1.12  2000/10/24 02:10:15  dave
// improved SetVoxel logic
//
// Revision 1.11  2000/10/24 01:27:35  dave
// changed from variables to constants in GetVoxel to make general case faster
//
// Revision 1.10  2000/10/22 23:48:21  dave
// added RecBlockVolume
//
// Revision 1.9  2000/10/22 03:02:44  dave
// added LinVol GetDataPtr(), and background in all constructors
//
// Revision 1.8  2000/10/22 00:52:09  dave
// better Iterators
//
// Revision 1.7  2000/10/22 00:24:22  dave
// better iterators
//
// Revision 1.6  2000/09/21 03:35:50  dave
// searching for segfaults
//
// Revision 1.5  2000/09/13 22:39:57  dave
// BlockVolume Write improved
//
// Revision 1.4  2000/09/11 22:46:08  dave
// added date field to constructors
//
// Revision 1.3  2000/09/11 12:56:02  frank
// cvs test
//
// Revision 1.2  2000/09/11 12:51:15  frank
// cvs test
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
//
// 5     9/09/00 4:24p Dave
//
// 4     9/09/00 4:04p Dave
// corrected WriteVolDataToStream temp allocation
//
// 3     9/08/00 6:48p Dave
// more debugging performed
//
// 2     9/05/00 9:24p Dave
// many walkthrough changes in documentation, functionality
//
// 1     9/04/00 9:44p Ingmar
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/BlockVolume.inl,v 1.6 2006/05/31 16:16:42 geconomos Exp $
// $Id: BlockVolume.inl,v 1.6 2006/05/31 16:16:42 geconomos Exp $
