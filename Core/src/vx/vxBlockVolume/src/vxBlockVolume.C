// $Id: vxBlockVolume.C,v 1.17.2.1 2010/07/16 20:31:42 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com

// includes
#include "stdafx.h"
#include "vxBlockVolume.h"
#include "V3DFile.h"
#include "vxBlockVolumeIterator.h"
#include "Deflate.h"
#include "Timer.h"
#include "vxComplex.h"
#include "Point2D.h"
#include "vxProgressBar.h"
#include "vxIntensityConverter.h"
#include "FilePtrFactory.h"


// defines
#define FILE_REVISION "$Revision: 1.17.2.1 $"

template class VX_BLOCK_VOLUME_DLL vxBlockVolume< int1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume< int2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume< int4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume<uint1>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume<uint2>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume<uint4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume< bool>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume<float4>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume<vxComplex>;
template class VX_BLOCK_VOLUME_DLL vxBlockVolume<vxVoxelCost>;

using namespace std;

#ifdef _WIN64

template< typename VoxelType > Triple< int4 > vxBlockVolume< VoxelType >::m_dimL0 = Triple< int4 > ( 16, 16, 16 );
template< typename VoxelType > Triple< int4 > vxBlockVolume< VoxelType >::m_dimL1 = Triple< int4 > (  4,  4,  4 );
template< typename VoxelType > Triple< int4 > vxBlockVolume< VoxelType >::m_dimL2 = Triple< int4 > ( 32, 32, 32 );

#else

template< typename VoxelType > Triple< int4 > vxBlockVolume< VoxelType >::m_dimL0 = Triple< int4 > ( 16, 16, 16 );
template< typename VoxelType > Triple< int4 > vxBlockVolume< VoxelType >::m_dimL1 = Triple< int4 > (  4,  4,  4 );
template< typename VoxelType > Triple< int4 > vxBlockVolume< VoxelType >::m_dimL2 = Triple< int4 > ( 32, 32, 32 );

#endif


/**
 * Constructor
 * @param const volume header
 */
template < >
vxBlockVolume<bool>::vxBlockVolume(const vxVolumeHeader & volumeHeader) : 
  m_header(volumeHeader),
  m_bgBlockL0Vox( m_dimL0.m_x * m_dimL0.m_y * m_dimL0.m_z, ( 0 == volumeHeader.GetBackground() ) ? false : true),
  m_bgBlockL1Blk( m_dimL1.m_x * m_dimL1.m_y * m_dimL1.m_z, m_bgBlockL0Vox ),
  m_BlockL2Vol  ( m_dimL2.m_x * m_dimL2.m_y * m_dimL2.m_z, m_bgBlockL1Blk )
//*******************************************************************
{
  GetHeader().SetVoxelTypeEnum(GetHeader().GetVoxelTypeEnumFromString(std::string(typeid(bool).name())));
  GetHeader().SetMemoryLayoutEnum(vxVolumeHeader::BLOCK);
} // constructor

/**
 * Constructor
 * @param const volume header
 */
template<class VoxelType>
vxBlockVolume<VoxelType>::vxBlockVolume(const vxVolumeHeader & volumeHeader): 
  m_header(volumeHeader),
  m_bgBlockL0Vox( m_dimL0.m_x * m_dimL0.m_y * m_dimL0.m_z, static_cast< VoxelType >( volumeHeader.GetBackground() ) ),
  m_bgBlockL1Blk( m_dimL1.m_x * m_dimL1.m_y * m_dimL1.m_z, m_bgBlockL0Vox ),
  m_BlockL2Vol  ( m_dimL2.m_x * m_dimL2.m_y * m_dimL2.m_z, m_bgBlockL1Blk )
//*******************************************************************
{
  GetHeader().SetVoxelTypeEnum(GetHeader().GetVoxelTypeEnumFromString(string(typeid(VoxelType).name())));
  GetHeader().SetMemoryLayoutEnum(vxVolumeHeader::BLOCK);
} // constructor

/**
 * From file constructor
 * @param file name
 */
template <class VoxelType> 
vxBlockVolume<VoxelType>::vxBlockVolume( const std::string & sFileName ) :
  m_bgBlockL0Vox( m_dimL0.m_x * m_dimL0.m_y * m_dimL0.m_z, 0 ),
  m_bgBlockL1Blk( m_dimL1.m_x * m_dimL1.m_y * m_dimL1.m_z, m_bgBlockL0Vox ),
  m_BlockL2Vol  ( m_dimL2.m_x * m_dimL2.m_y * m_dimL2.m_z, m_bgBlockL1Blk )
//*******************************************************************
{
  GetHeader().SetVoxelTypeEnum(GetHeader().GetVoxelTypeEnumFromString(string(typeid(VoxelType).name())));
  GetHeader().SetMemoryLayoutEnum(vxVolumeHeader::BLOCK);
  Read(sFileName, std::make_pair(0.0F, 100.0F));
} // from file constructor


/**
 * Destructor
 */
template <class VoxelType>
vxBlockVolume<VoxelType>::~vxBlockVolume()
//*******************************************************************
{
  Clear();
} // destructor


/**
 * Copy constructor
 * @param other volume
 */
template <class VoxelType>
vxBlockVolume<VoxelType>::vxBlockVolume(const vxBlockVolume<VoxelType> & other)
//*******************************************************************
{
  *this = other;
/*
  m_bgBlockL0Vox = other.m_bgBlockL0Vox;
  m_bgBlockL1Blk = other.m_bgBlockL1Blk;    
  m_BlockL2Vol   = other.m_BlockL2Vol;    
  m_header = other.m_header;
  m_BlockL2Vol.UnShare();
*/
}


/**
 * Copy other volume to current (will not allocate more memory, but share 
 * the voxels from the other volume (until written to))
 * @param other volume
 * @return const this reference
 */
template <class VoxelType> 
vxBlockVolume<VoxelType> const & vxBlockVolume<VoxelType>::operator=(const vxBlockVolume<VoxelType> & other)
//*******************************************************************
{
  if (&other != this)
  {
    m_bgBlockL0Vox = other.m_bgBlockL0Vox;
    m_bgBlockL1Blk = other.m_bgBlockL1Blk;    
    m_BlockL2Vol   = other.m_BlockL2Vol;    
    m_header = other.m_header;
    m_histogram = other.m_histogram;
    m_BlockL2Vol.UnShare();
  }

  return *this;
} // assignment operator


/**
 * Clear volume to background (release memory of old blocks)
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::Clear()
//*******************************************************************
{
  // set L1 blk blocks to background, which automatically sets L0 voxels blocks
  vxBlock<vxBlock<VoxelType> > tmpBlockL1Blk( m_dimL1.m_x * m_dimL1.m_y * m_dimL1.m_z, m_bgBlockL0Vox);
  m_bgBlockL1Blk = tmpBlockL1Blk;

  vxBlock<vxBlock<vxBlock<VoxelType> > > tmpBlockL2Vol( m_dimL2.m_x * m_dimL2.m_y * m_dimL2.m_z, m_bgBlockL1Blk);
  m_BlockL2Vol = tmpBlockL2Vol;
} // Clear()


/**
 * Clear volume to with new background color (release memory of old blocks)
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::Clear(VoxelType newBackgroundValue)
//*******************************************************************
{
  vxBlock<VoxelType> tmpBlockL0Vox( m_dimL0.m_x * m_dimL0.m_y * m_dimL0.m_z, newBackgroundValue);
  m_bgBlockL0Vox = tmpBlockL0Vox;

  vxBlock<vxBlock<VoxelType> > tmpBlockL1Blk( m_dimL1.m_x * m_dimL1.m_y * m_dimL1.m_z, m_bgBlockL0Vox);
  m_bgBlockL1Blk = tmpBlockL1Blk;

  vxBlock< vxBlock<vxBlock<VoxelType> > > tmpBlockL2Vol( m_dimL2.m_x * m_dimL2.m_y * m_dimL2.m_z, m_bgBlockL1Blk);
  m_BlockL2Vol = tmpBlockL2Vol;
} // Clear()


/**
 * Returns true if volume consist only of background
 * @return successful
 */
template <class VoxelType> 
bool vxBlockVolume<VoxelType>::IsClear()
//*******************************************************************
{
  bool bIsClear(true);
  vxBlockVolumeIterator<VoxelType> iter(*this);

  for (iter.SetPos(0,0,0); iter.IsNotAtEnd(); iter.NextBlockZYX())
  {
    if (m_bgBlockL0Vox != iter.GetCurrentBlock())
    {
      bIsClear = false;
      break;
    }
  }
  return(bIsClear);
} // IsClear()


/**
 * Read Volume from disk
 * @param file name
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::WriteAsLinVolVer1_0(const std::string & sFileName)
//*******************************************************************
{
  std::stringstream sMessage;
  try
  {
    if (sFileName != "sameAsInHeader")
    {
      GetHeader().SetFileName(sFileName);
    }
    GetHeader().SetFileVersion(1,0);
    GetHeader().SetMemoryLayoutEnum(vxVolumeHeader::LINEAR);

    VoxelType * pData = new VoxelType[GetHeader().GetNumberOfVoxels()];
    vxBlockVolumeIterator<VoxelType> blkIter(*this);

    VoxelType * pCopy(pData);
    for (blkIter.SetPos(0,0,0); blkIter.IsNotAtEnd(); blkIter.NextZYX())
    {
      *pCopy++ = blkIter.GetVoxel();
    }

    V3DFile img;
    img.SetFileVersion(GetHeader().GetFileVersion());
    img.SetDescription(GetHeader().GetAsString());
    img.SetTitle(GetHeader().GetTitle());    
    img.SetComment(GetHeader().m_varHeader.ToString());
    img.SetData(MemoryBuffer((uint1 *) pData, GetHeader().GetNumberOfVoxels() * sizeof(VoxelType)));      
    img.Write(sFileName, V3DFile::LZO); 
    delete [] pData;

    sMessage.str("");
    sMessage << "Writing of " << sFileName << " (" << GetHeader().GetVolDim().X() << ", " << GetHeader().GetVolDim().Y() << ", " << GetHeader().GetVolDim().Z() << ")"<< " succeed.";
    LogFyi(sMessage.str(),"vxBlockVolume::VoxelType","WriteAsLinVolVer1_0");    
  }
  catch (...)
  {
    sMessage.str("");
    sMessage << "Writing of " << sFileName << " (" << GetHeader().GetVolDim().X() << ", " << GetHeader().GetVolDim().Y() << ", " << GetHeader().GetVolDim().Z() << ")"<< " failed.";
    LogFyi(sMessage.str(),"vxBlockVolume::VoxelType","WriteAsLinVolVer1_0");    
  }
} // WriteAsLinVolVer1_0()


/**
 * Read Volume from disk
 * @param file name
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::WriteAsBlkVolVer1_0(const std::string & sFileName)
//*******************************************************************
{
  std::stringstream sMessage;
  try
  {
    if (sFileName != "sameAsInHeader")
    {
      GetHeader().SetFileName(sFileName);
    }
    GetHeader().SetFileVersion(1,0);
    GetHeader().SetMemoryLayoutEnum(vxVolumeHeader::BLOCK);

    uint1 * pDataBlock = new uint1[8*8*8 * sizeof(VoxelType)];
    const int4 iExpandedVolDimX(ExpandToPowerOf2(GetHeader().GetVolDim().X()));
    const int4 iExpandedVolDimY(ExpandToPowerOf2(GetHeader().GetVolDim().Y()));
    const int4 iExpandedVolDimZ(ExpandToPowerOf2(GetHeader().GetVolDim().Z()));

    const double fLog2(log(2.0F));
    const int4 iExpandedVolDimXbits(static_cast<int4>( (log(static_cast<float4>(iExpandedVolDimX))/fLog2) + 0.5F));
    const int4 iExpandedVolDimYbits(static_cast<int4>( (log(static_cast<float4>(iExpandedVolDimY))/fLog2) + 0.5F));
    const int4 iExpandedVolDimZbits(static_cast<int4>( (log(static_cast<float4>(iExpandedVolDimZ))/fLog2) + 0.5F));

    float8 dTmpResult(pow(2.0, iExpandedVolDimXbits + iExpandedVolDimYbits + iExpandedVolDimZbits  - 3 * 3));
    uint4 uNumberOfBlocks = static_cast<uint4>(dTmpResult);
    uint1 * pCompleteVolume = new uint1[static_cast<size_t>((uNumberOfBlocks * ((sizeof(VoxelType) * 8*8*8) + sizeof(int2))) * 1.2)];

    vxBlockVolumeIterator<VoxelType> blkIter(*this);

    VoxelType * pDataBlockCurrent = (VoxelType *)pDataBlock;
    uint1 * pCompleteVolumeCopy(pCompleteVolume);

    uint4 uBytesWritten(0);
    uint2 uCompressedSize;
    uint4 uNumberOfBlocksWritten(0);
    Deflate deflater;
    *((uint4*)(pCompleteVolumeCopy)) = 3; // bits of the size of a side of a block
    pCompleteVolumeCopy += 4;

    for (blkIter.SetPos(0,0,0); blkIter.GetPosZ() < iExpandedVolDimZ; blkIter.IncZbyDelta(8))
    {
      for (; blkIter.GetPosY() < iExpandedVolDimY; blkIter.IncYbyDelta(8))
      {
        for (; blkIter.GetPosX() < iExpandedVolDimX; blkIter.IncXbyDelta(8))
        {
          for (int4 iZ(0); iZ<8; ++iZ)
          {
            for (int4 iY(0); iY<8; ++iY)
            {
              for (int4 iX(0); iX<8; ++iX)
              {
                *pDataBlockCurrent++ = blkIter.GetVoxel();
                blkIter.IncX();
              }
              blkIter.DecXbyDelta(8);
              blkIter.IncY();
            } // for all y

            blkIter.DecYbyDelta(8);
            blkIter.IncZ();
          } // for all z

          blkIter.DecZbyDelta(8);
          uCompressedSize = 0;
          uint4 uTmpCompressedSize;
          deflater.DeflateIt(hdr::LZO,(uint1 *)(pDataBlock),8*8*8 * sizeof(VoxelType),(pCompleteVolumeCopy + 2), uTmpCompressedSize);        
          uCompressedSize = uTmpCompressedSize;
          *((uint2*)(pCompleteVolumeCopy)) = uCompressedSize;
          pCompleteVolumeCopy += uCompressedSize + 2;
          uBytesWritten += uCompressedSize + 2;
          uNumberOfBlocksWritten++;
          pDataBlockCurrent = (VoxelType *)pDataBlock;
        } // for all X

        blkIter.SetPosX(0);
      } // for all Y

      blkIter.SetPosY(0);
    } // for all blocks

    V3DFile img;
    img.SetFileVersion(GetHeader().GetFileVersion());
    img.SetDescription(GetHeader().GetAsString());
    img.SetTitle(GetHeader().GetTitle());    
    img.SetComment(GetHeader().m_varHeader.ToString());
    img.SetData(MemoryBuffer((uint1 *) pCompleteVolume, uBytesWritten));      
    img.Write(sFileName, V3DFile::LZO); 
    delete [] pDataBlock;
    delete [] pCompleteVolume;

    sMessage.str("");
    sMessage << "Writing of " << sFileName << " (" << GetHeader().GetVolDim().X() << ", " << GetHeader().GetVolDim().Y() << ", " << GetHeader().GetVolDim().Z() << ")"<< " succeed.";
    LogFyi(sMessage.str(),"vxBlockVolume::VoxelType","WriteAsBlkVolVer1_0");    
  }
  catch (...)
  {
    sMessage.str("");
    sMessage << "Writing of " << sFileName << " (" << GetHeader().GetVolDim().X() << ", " << GetHeader().GetVolDim().Y() << ", " << GetHeader().GetVolDim().Z() << ")"<< " failed.";
    LogFyi(sMessage.str(),"vxBlockVolume::VoxelType","WriteAsBlkVolVer1_0");    
  }
} // WriteAsBlkVolVer1_0()


/**
 * Read Volume from disk
 * @param file name
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::Write(const std::string & sFileName)
//*******************************************************************
{
	//compact the volume before the write starts
	Compact();

  std::stringstream sMessage;
  try
  {
    Timer writeTimer;
    if (sFileName != "sameAsInHeader")
    {
      GetHeader().SetFileName(sFileName);
    }

    GetHeader().SetFileVersion(2,0);
    GetHeader().SetMemoryLayoutEnum(vxVolumeHeader::BLOCK);
    //// find all unique blocks

    // first get all block pointers into one array
    vxBlockVolumeIterator<VoxelType> iterVol(*this);

    vector<int4> vVoxelBlockIndices(GetNumberOfBlocks());
    vector<const VoxelType *> vPointersToUniqueBlocks;
    vector<int4> vBackgroundFlagOfUniqueBlocks;

    // then copy the unique ones and generate an array of indices
    std::vector<const VoxelType *>::iterator dataPointerIter;
    std::vector<int4>::iterator BackgroundFlagIter;

    for (int4 k(0); iterVol.IsNotAtEnd(); iterVol.NextBlockZYX(), ++k)
    {
      int4 iIndex(0);
      bool bFound(false);
      for (dataPointerIter = vPointersToUniqueBlocks.begin(); dataPointerIter != vPointersToUniqueBlocks.end(); ++dataPointerIter, ++iIndex)
      {
        if (*dataPointerIter == iterVol.GetDataPointer())
        {
          bFound = true;
          break;
        }
      }

      if (bFound == true)
      {
        vVoxelBlockIndices[k] = iIndex;
      }
      else
      {
        vVoxelBlockIndices[k] = static_cast<int4>( vPointersToUniqueBlocks.size() );
        vPointersToUniqueBlocks.push_back(iterVol.GetDataPointer());
        vBackgroundFlagOfUniqueBlocks.push_back(((int4)iterVol.m_pL0->m_pBlockInfo->m_bIsDirty) | (((int4)(iterVol.m_pL0->m_pBlockInfo->m_bIsHomogenous))  << 8));
      }
    }

    const uint4 uByteSizeOfVoxelType(sizeof(VoxelType));
    const uint4 uByteSizeOfBlock(vxBlockVolumeIteratorConsts::eL0VoxVoxelNum * uByteSizeOfVoxelType);  
    const uint4 uByteSizeOfIndexArray(static_cast<uint4>(vVoxelBlockIndices.size() * sizeof(int4)) );

    GetHeader().m_varHeader.SetHdrVar("uTotalNumberOfBlocks", GetNumberOfBlocks(), "the number of blocks in the volume");
    GetHeader().m_varHeader.SetHdrVar("uNumberOfUniqueBlocks", static_cast<uint4>(vPointersToUniqueBlocks.size() )," the size of the index array");
    GetHeader().m_varHeader.SetHdrVar("uNumberOfVoxelsPerBlock", static_cast<uint4>( vxBlockVolumeIteratorConsts::eL0VoxVoxelNum ), "the number of voxels in one block");  
    GetHeader().m_varHeader.SetHdrVar("uByteSizeOfIndexArray", uByteSizeOfIndexArray, "byte size of the index array");

    /// set data buffer
    const uint4 uNumMemoryBlocks(static_cast<uint4>(vPointersToUniqueBlocks.size() + 1 + 1)); // one extra for the index array and one for max blocksize
    V3DFile::MemBlockInfo * pMemBlockInfo = new V3DFile::MemBlockInfo[uNumMemoryBlocks];

    // first store maximum blocksize
    pMemBlockInfo[0].uMemBlockByteSize = static_cast<uint4>(Max( (uint4)(vVoxelBlockIndices.size() * sizeof(uint4)), uByteSizeOfBlock) );

    // then store the index array
    pMemBlockInfo[1].uMemBlockByteSize = static_cast<uint4>(vVoxelBlockIndices.size() * sizeof(uint4) );
    pMemBlockInfo[1].pMemBlock = (uint1 *)&(*vVoxelBlockIndices.begin());

    // finally store all voxel data blocks
    V3DFile::MemBlockInfo * pCurrentMemBlockInfo = &pMemBlockInfo[2];
    for (dataPointerIter = vPointersToUniqueBlocks.begin(), BackgroundFlagIter = vBackgroundFlagOfUniqueBlocks.begin(); dataPointerIter != vPointersToUniqueBlocks.end(); ++dataPointerIter, ++pCurrentMemBlockInfo, ++BackgroundFlagIter)
    {
      pCurrentMemBlockInfo->uBytesPerEntryInBlock = sizeof( VoxelType );
      pCurrentMemBlockInfo->uMemBlockByteSize = uByteSizeOfBlock;
      pCurrentMemBlockInfo->pMemBlock         = (uint1 *)*dataPointerIter;
      pCurrentMemBlockInfo->iFlag             = *BackgroundFlagIter;      
    }

    V3DFile img;
    img.SetFileVersion(GetHeader().GetFileVersion());
    img.SetDescription(GetHeader().GetAsString());
    img.SetTitle(GetHeader().GetTitle());    
    // TODO this is foobar!!
    //img.SetComment(GetHeader().GetVariableVolumeHeader().ToString_1());
    img.SetComment(GetHeader().GetVarMapsAsString());

    Image<PixelRGB<uint1> > previewImage( Triple<uint4>( GetHeader().GetVolDim().X()/2, GetHeader().GetVolDim().Y()/2, 0 ), PixelRGB<uint1>( 0, 0, 0 ) );
    ComputePreviewImage(previewImage);
    img.SetImage(previewImage);
    img.SetData(MemoryBuffer((uint1 *) pMemBlockInfo, uNumMemoryBlocks * sizeof(V3DFile::MemBlockInfo)));    

    if ((GetHeader().GetSignificantBitsPerVoxel() == 12) && ( typeid(VoxelType) == typeid(int2) || typeid(VoxelType) == typeid(uint2) ))
    {
      img.Write(sFileName, V3DFile::LZO_BLOCKS_PLUS_BLEND_OUT_OF_HIGHER_FOUR_BIT_OF_16_BIT_DATA); 
    }
    else
    {
      img.Write(sFileName, V3DFile::LZO_BLOCKS); 
    }

    delete [] pMemBlockInfo;

    sMessage.str("");
    sMessage << "Writing of " << sFileName << " (" << GetHeader().GetVolDim().X() << ", " << GetHeader().GetVolDim().Y() << ", " << GetHeader().GetVolDim().Z() << ")"<< " succeed.";
    LogFyi(sMessage.str(),"vxBlockVolume::VoxelType","Write");    
    sMessage.str("");
    sMessage << "Writing " << writeTimer;
    LogDbg(sMessage.str(),"vxBlockVolume::VoxelType","Write");    
  }
  catch (...)
  {
    sMessage.str("");
    sMessage << "Writing of " << sFileName << " (" << GetHeader().GetVolDim().X() << ", " << GetHeader().GetVolDim().Y() << ", " << GetHeader().GetVolDim().Z() << ")"<< " failed.";
    LogFyi(sMessage.str(),"vxBlockVolume::VoxelType","Write");    
  }
} // Write()


/**
 * Write raw data to file
 * @param fle name
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::WriteRaw(const std::string & sFileName)
//*******************************************************************
{
  try
  {
    Timer timer;
    
    std::shared_ptr<io::FilePtr> file = io::FilePtrFactory::CreateFilePtr(sFileName);
    file->Open( GENERIC_WRITE, OPEN_ALWAYS );
    file->Write(GetHeader().GetVolDim().ToString().c_str(), static_cast<uint4>(GetHeader().GetVolDim().ToString().length() ));         
    file->Write("\n",1);
    file->Write(GetHeader().GetVoxelUnits().ToString().c_str(), static_cast<uint4>(GetHeader().GetVoxelUnits().ToString().length())); 
    file->Write("\n",1);
    
    vxBlockVolumeIterator<VoxelType> iterVol(*this);
    for (;  iterVol.IsNotAtEnd(); iterVol.NextZYX())
    {
      file->Write(&iterVol.GetVoxel(), 2);
    }
    file->Close();
    LogFyi("wrote "+sFileName+GetHeader().GetVolDim().ToString()+" in "+timer.AsString(),"vxBlockVolume","WriteRaw");    
  }
  catch (...)
  {
    LogFyi("failed writing"+sFileName+GetHeader().GetVolDim().ToString(),"vxBlockVolume","WriteRaw");    
  }
} // WriteRaw()


/**
 *  This is specific for Colon Processor SKE stage vxVoxelCost type. Dongqing
 */
template <> 
void vxBlockVolume<vxVoxelCost>::ComputePreviewImage(Image<PixelRGB<uint1> > & image)
{
}


/**
 * Compute Preview Image
 * @param image
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::ComputePreviewImage(Image<PixelRGB<uint1> > & image)
//*******************************************************************
{
  float4 fScale(1.0f);
  if (typeid(VoxelType) == typeid(bool)) fScale = 255.0f;
  if (typeid(VoxelType) == typeid(int1)) fScale =   1.0f;
  if (typeid(VoxelType) == typeid(int2)) fScale = 255.0f/2048.0f;
  if (typeid(VoxelType) == typeid(int4)) fScale = 255.0f/512.0f;
  if (typeid(VoxelType) == typeid(uint1)) fScale =   1.0f;
  if (typeid(VoxelType) == typeid(uint2)) fScale = 255.0f/65535.0f;
  if (typeid(VoxelType) == typeid(uint4)) fScale = 255.0f/4294967295.0f;
  if (typeid(VoxelType) == typeid(float4)) fScale =   1.0f;
  if (typeid(VoxelType) == typeid(float8)) fScale =   1.0f;

  vxBlockVolumeIterator<VoxelType> iterVol(*this, Vector3D<int4>(0, 0, GetHeader().GetVolDim().Z()/2));
  for (uint4 y(0); y<image.GetDim().Y(); ++y, iterVol.IncYbyDelta(2))
  {
    iterVol.SetPosX(0);
    for (uint4 x(0); x<image.GetDim().X(); ++x, iterVol.IncXbyDelta(2))
    {
      uint1 uIntensity(static_cast<uint1>(Bound(0.0f, (const float4)(iterVol.GetVoxel()) * fScale, 255.0f)));
      image.SetPixel(Point2D<uint2>(x,y), PixelRGB<uint1>(uIntensity, uIntensity, uIntensity));
    }
  }
} // ComputePreviewImage()

/**
 *  This is specific for Colon Processor SKE stage vxVoxelCost type. Dongqing
 */
template <> 
void vxBlockVolume<vxVoxelCost>::Read(const std::string & sFileName, 
                                    std::pair<float4, float4> & progressPair)
{
}

/**
 * Read volume from disk
 *
 * @param sFileName    file name
 * @param progressPair progress pair for progress bar - set either value < 0 to not update meter
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::Read(const std::string & sFileName, std::pair<float4, float4> & progressPair)
//*******************************************************************
{
  std::stringstream sMessage;
  if (sFileName.size() <= 4)
  {
    sMessage.str("");
    sMessage << "The Filename " << sFileName << " is too short - extensions required.";
    LogErr(sMessage.str(),"vxBlockVolume::VoxelType","Read");    
    return;
  }
  try
  {
    if (sFileName.substr(sFileName.size()-4, 4) == ".slc")
    {
      ReadSlc(sFileName);
      return;
    }

    // first read the header
    GetHeader().Read(sFileName);
    
    // write out a warning if the header's voxel type doesn't match this instaniation
    vxVolumeHeader::VoxelTypeEnum eVoxelType = GetHeader().GetVoxelTypeEnumFromString(string(typeid(VoxelType).name()));
    if (GetHeader().GetVoxelTypeEnum() != eVoxelType)
    {
      sMessage.str("");
      sMessage << "Unexpected datatype in Read. Expected: " 
        << GetHeader().m_vsVoxelType[eVoxelType] 
        << " Received: " 
        << GetHeader().m_vsVoxelType[GetHeader().GetVoxelTypeEnum()];
      LogWrn(sMessage.str(), "vxBlockVolume", "Read");

      if (GetHeader().GetVoxelTypeEnum() == vxVolumeHeader::INT2 && eVoxelType == vxVolumeHeader::UINT2)
      {
        sMessage.str("");
        sMessage << "This datatype (INT2) is supported." ;
        LogWrn(sMessage.str(), "vxBlockVolume", "Read");
      }
    }

    Clear();
    V3DFile img;
    Timer loadTimer;
    switch (GetHeader().GetMemoryLayoutEnum())
    {
    case vxVolumeHeader::LINEAR:  
      {
        img.SetSkipData(false);
        try
        {
          img.Read(sFileName, std::make_pair(0.0F, 100.0F));
        }
        catch (...)
        {
          throw ex::VException(LogRec("Linear volume data read failed.","vxBlockVolume","Read"));
        }

        try
        {
          // this is the standard case of equal data types and source that is not UINT1
          ///////////////////////////////////////////////////////////////////////////////
          vxBlockVolumeIterator<VoxelType> iter(*this);

          if (GetHeader().GetVoxelTypeEnum() != vxVolumeHeader::UINT1)
          {
            VoxelType * pData = reinterpret_cast<VoxelType *> (img.GetDataBuffer().GetDataPtr());
            string sDataModality;
            GetHeader().GetHeaderEntry(vxVolumeHeader::DataModality, sDataModality);

            if (typeid(VoxelType) == typeid(int2) && sDataModality == "CT")
            {
              for (iter.SetPos(0,0,0); !iter.IsAtEnd(); iter.NextZYX())
              {
                #pragma warning( disable : 4244 4800)
                iter.SetVoxel( Bound(0, static_cast<int4>(*pData++), 4095));
                #pragma warning( default : 4244 4800)
              }      
            }
            else
            {
              for (iter.SetPos(0,0,0); !iter.IsAtEnd(); iter.NextZYX())
              {
                iter.SetVoxel(*pData++);
              }        
            }
          }
          else
          {
            // this is the case where we do have an old UINT1 linear volume
            /////////////////////////////////////////////////////////////////
            uint1 * pData = reinterpret_cast<uint1 *> (img.GetDataBuffer().GetDataPtr());
            string sDataModality;
            for (iter.SetPos(0,0,0); !iter.IsAtEnd(); iter.NextZYX())
            {
              #pragma warning( disable : 4800 )
              iter.SetVoxel(*pData++);
              #pragma warning( default  : 4800 )
            }       
            GetHeader().SetSignificantBitsPerVoxel(8);
          }
        }
        catch (...)
        {
          throw ex::VException(LogRec("Linear volume to block volume conversion failed.", "vxBlockVolume", "Read"));
        }
        break;
      }
    case vxVolumeHeader::BLOCK:
      {
        if (GetHeader().GetFileVersion() == std::pair<int4,int4>(2,0))
        {
          const uint4 uSizeOfVoxelType(sizeof(VoxelType));
          uint4 uTotalNumberOfBlocks;
          uint4 uNumberOfVoxelsPerBlock;
          uint4 uNumberOfUniqueBlocks;
          uint4 uByteSizeOfIndexArray;
          GetHeader().m_varHeader.GetHdrVar("uTotalNumberOfBlocks",uTotalNumberOfBlocks);
          GetHeader().m_varHeader.GetHdrVar("uNumberOfUniqueBlocks",uNumberOfUniqueBlocks);          
          GetHeader().m_varHeader.GetHdrVar("uNumberOfVoxelsPerBlock",uNumberOfVoxelsPerBlock);
          GetHeader().m_varHeader.GetHdrVar("uByteSizeOfIndexArray", uByteSizeOfIndexArray);
          if (uNumberOfVoxelsPerBlock != vxBlockVolumeIteratorConsts::eL0VoxVoxelNum)
          {
            sMessage.str("");
            sMessage << "Reading of " << sFileName << " failed.";  std::cout << "  LLLLLLLLLLLLL  " << std::endl;
            throw ex::VException( LogRec( "Reading of " +sFileName+ " is failed", "vxBlockVolume::VoxelType", "Read" ) );
            return;
          }

          // allocate individual memory chuncks for each unique block and fill them with the voxel data
          std::vector<vxBlock<VoxelType> *> vUniqueBlocks(uNumberOfUniqueBlocks);
          std::vector<vxBlock<VoxelType> *>::iterator vUniqueBlocksIter = vUniqueBlocks.begin();

          std::vector<V3DFile::MemBlockInfo> vMemBlockInfo(uNumberOfUniqueBlocks + 1 + 1); // +1 for index array

          // allocate memory for index array 

          vMemBlockInfo[1].uMemBlockByteSize = uByteSizeOfIndexArray;
          vMemBlockInfo[1].pMemBlock = new uint1 [uByteSizeOfIndexArray];

          // allocate blocks
          for (uint4 k(2);  k < uNumberOfUniqueBlocks + 1 + 1; ++k, ++vUniqueBlocksIter)
          {
            *vUniqueBlocksIter = new vxBlock<VoxelType>(uNumberOfVoxelsPerBlock);
            vMemBlockInfo[k].pMemBlock =(uint1 *) (*vUniqueBlocksIter)->GetDataPtr();
            vMemBlockInfo[k].uMemBlockByteSize = uNumberOfVoxelsPerBlock * sizeof(VoxelType); 
            vMemBlockInfo[k].uBytesPerEntryInBlock = sizeof( VoxelType );
            
          }

          // now fill 
          img.SetSkipData(false);
          img.SetData(MemoryBuffer((uint1 *)&(*vMemBlockInfo.begin()), static_cast<uint4>(vMemBlockInfo.size() * sizeof(V3DFile::MemBlockInfo))));

          // indicate that we are done with 5 percent of the reading
          const float4 fFivePercent(Interpolate(0.0F, 5.0F, 100.0F, progressPair.first, progressPair.second));
          if ( progressPair.first >= 0 && progressPair.second >= 0 )
          {
            vxProgressBar::SetProgressPercentage(fFivePercent);
          }

          Timer ioTimer;
          img.Read(sFileName, std::pair< float4, float4 >(fFivePercent, progressPair.second));
          LogDbg("Disk I/O for read took " + ioTimer.AsString(), "vxBlockVolume", "Read");
          
          
          Timer decodeTimer;
          vUniqueBlocksIter = vUniqueBlocks.begin();
          for (int4 k = 2;  k < uNumberOfUniqueBlocks + 1 + 1; ++k, ++vUniqueBlocksIter)
          {
            (*vUniqueBlocksIter)->m_pBlockInfo->m_bIsDirty = ((vMemBlockInfo[k].iFlag & 0x000000FF) == 0) ? false : true;
            (*vUniqueBlocksIter)->m_pBlockInfo->m_bIsHomogenous = (((vMemBlockInfo[k].iFlag & 0x0000FF00) >> 8)== 0) ? false : true;
          }

          // use these blocks with proper reference counting in the current block volume
          int4 * pIndexArray((int4 *) vMemBlockInfo[1].pMemBlock);
          vxBlockVolumeIterator<VoxelType> iterVol(*this);

         for (iterVol.SetPos(0,0,0) ; iterVol.IsNotAtEnd(); iterVol.NextBlockZYX(), ++pIndexArray)
         {
           iterVol.SetBlock(*vUniqueBlocks[*pIndexArray]);
         }

          // call delete on each temporary unique block (end of scope reference count adjustment)
          for (vUniqueBlocksIter = vUniqueBlocks.begin() ; vUniqueBlocksIter != vUniqueBlocks.end(); ++vUniqueBlocksIter)
          {
            delete *vUniqueBlocksIter;
          }
          delete [] vMemBlockInfo[1].pMemBlock;
          LogDbg("Decode for read took " + decodeTimer.AsString(), "vxBlockVolume", "Read");
        } // file version == 2.0
        else
        {
          // file version == 1.0
          Timer ioTimer;
          img.SetSkipData(false);
          img.Read(sFileName, std::make_pair(0.0F, 100.0F));
          LogDbg("Disk I/O for read took " + ioTimer.AsString(), "vxBlockVolume", "Read");

          Timer logicTimer;
          if (img.GetDataBuffer().GetSize() == 0)
          {
            sMessage.str("");
            sMessage << "Reading of " << sFileName << " failed.";
            throw ex::VException( LogRec( "Reading of " +sFileName+ " is failed", "vxBlockVolume::VoxelType", "Read" ) );
            return;         
          }

          int4 iDataBlockSize(8*8*8);
          uint4 uBlockBitSize(0);
          std::istrstream dataStream((const char *) img.GetDataBuffer().GetDataPtr(), img.GetDataBuffer().GetSize());
          dataStream.read((int1 *)(&uBlockBitSize), sizeof(uBlockBitSize));

          uint2 uCompressedBlockSize(0);
          uint4 tempSize(iDataBlockSize * 2 * sizeof(VoxelType));

          int1 * pCompressedDataBlock = new int1[tempSize*2+50];
          VoxelType * pDataBlock = new VoxelType[iDataBlockSize*4+50];
          vxBlockVolumeIterator<VoxelType> iterVol(*this);
          uint4 uMaxX(ExpandToPowerOf2(GetHeader().GetVolDim().X())-8);
          uint4 uMaxY(ExpandToPowerOf2(GetHeader().GetVolDim().Y())-8);
          uint4 uMaxZ(ExpandToPowerOf2(GetHeader().GetVolDim().Z())-8);

          // read all blocks
          while (dataStream)
          {
            // read next block size
            dataStream.read((int1 *)(&uCompressedBlockSize), sizeof(uCompressedBlockSize));

            if (uCompressedBlockSize == 0)
            {
              // nop if blocks are alreaady set to background in InitMemberVars()
            }
            else
            {
              // get compressed data from stream
              dataStream.read(static_cast<int1 *>(pCompressedDataBlock), uCompressedBlockSize);

              // decompress
              Inflate::InflateIt(hdr::LZO, 
                                (uint1 *) pCompressedDataBlock, uCompressedBlockSize, 
                                (uint1 *)    pDataBlock,           iDataBlockSize * sizeof(VoxelType));

              // write into volume
              VoxelType * pVoxel(pDataBlock);
              for (int4 z=0; z<8; ++z)
              {
                for (int4 y=0; y<8; ++y)
                {
                  for (int4 x=0; x<8; ++x)
                  {
                    iterVol.SetVoxel(*pVoxel++);
                    iterVol.IncX();
                  } // for all x

                  iterVol.DecXbyDelta(8);
                  iterVol.IncY();
                } // for all y

                iterVol.DecYbyDelta(8);
                iterVol.IncZ();
              } // for all z

              iterVol.DecZbyDelta(8);
            } // end read block

            // update position
            if (static_cast<uint4>(iterVol.GetPosX()) < uMaxX)
            {
              iterVol.IncXbyDelta(8);
            }
            else
            {
              iterVol.SetPosX(0);
              if (static_cast< uint4 >(iterVol.GetPosY()) < uMaxY)
              {
                iterVol.IncYbyDelta(8);
              }
              else
              {
                iterVol.SetPosY(0);
                if (static_cast<uint4>(iterVol.GetPosZ() ) < uMaxZ)
                {
                  iterVol.IncZbyDelta(8);             
                }
              }
            } // position X at max
          } // while (dataStream)

          // delete the two temporary buffers
          delete [] pCompressedDataBlock;
          delete [] pDataBlock; 
          LogDbg("Decoding time for read took " + logicTimer.AsString(), "vxBlockVolume", "Read");
        }// file version == 1.0
        break;
      } // case vxVolumeHeader::BLOCK:
    }
    
    // clamp negtaive values to 0 when reading a INT2 volume into a vxBlockVolume<uint2>    
    if( GetHeader().GetVoxelTypeEnum() == vxVolumeHeader::INT2 && eVoxelType == vxVolumeHeader::UINT2 )
    {
      // clamp to valid range
      Timer timer;
      vxBlockVolumeIterator<VoxelType> iterVol( *this );
      for ( ; iterVol.IsNotAtEnd(); iterVol.NextBlockZYX() )
      {
        for ( ; iterVol.IsNotAtEndOfBlock(); iterVol.NextZYXinsideBlock() )
        {
          // clamp
          if ( static_cast< int2 >( iterVol.GetVoxel() ) < 0 )
            iterVol.SetVoxel( 0 );
        }
      }
      LogDbg("Clamping time for read took " + timer.AsString(), "vxBlockVolume", "Read");
    }

    if ( progressPair.first >= 0 && progressPair.second >= 0 )
    {
      vxProgressBar::SetProgressPercentage( progressPair.second );
    }
    sMessage.str("");
    sMessage << "Reading of " << sFileName << " (" << GetHeader().GetVolDim().X() << ", " << GetHeader().GetVolDim().Y() << ", " << GetHeader().GetVolDim().Z() << ")"<< " succeed.";
    LogFyi(sMessage.str(),"vxBlockVolume::VoxelType","Read");    
    sMessage.str("");
    sMessage << "Reading " << loadTimer;
    LogDbg(sMessage.str(),"vxBlockVolume::VoxelType","Read");    
  } // end of try
  catch ( ex::DataCorruptionException & ep )
  {
    Clear(); // now clear the volume, as what we may have partially read is no good
    LogErr( "Reading of " +sFileName+ " failed: " + ep.GetLogRecord().GetTextMessage(), "vxBlockVolume::VoxelType", "Read" ) ;
    throw;
  }
  catch (...)
  {
    Clear(); // now clear the volume, as what we may have partially read is no good
    throw ex::VException( LogRec( "Reading of " +sFileName+ " failed", "vxBlockVolume::VoxelType", "Read" ) );
  }
} // Read()


/**
 * Read .slc volume from disk
 * @param voxel
 * @param voxel type
 * @return voxel type
 */
template <class VoxelType> 
VoxelType LocalScope_GetScaledVoxelValue(uint1 uVoxel, 
                                         VoxelType dummy)
//*******************************************************************
{
  #pragma warning( disable : 4800 )
  return VoxelType(uVoxel);
  #pragma warning( default : 4800 )
}


/**
 * LocalScope_GetScaledVoxelValue
 * @param voxel value
 * @param dummy value
 * @return scaled value
 */
template < > 
int2 LocalScope_GetScaledVoxelValue(uint1 uVoxel, int2 dummy)
//*******************************************************************
{
  return int4(uVoxel) * 4096 / 256;
}


/**
 * Read .slc volume from disk
 * @param file name
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::ReadSlc(const std::string & sFileName)
//*******************************************************************
{
  LogFyi(string("Loading ")+sFileName,"vxBlockVolume","ReadSlc");    

#ifdef _WIN32
  ifstream srcFile(sFileName.c_str(), ios::binary);
#else
  ifstream srcFile(sFileName.c_str());
#endif 

  // load volume header
  int4 iDummyMagicDataTypeNumber(0);
  srcFile >> iDummyMagicDataTypeNumber;

  int4 iDimX(0), iDimY(0), iDimZ(0);
  srcFile >> iDimX >> iDimY >> iDimZ;
  m_header.SetVolDim(Triple<int4>(iDimX, iDimY, iDimZ));

  int4 iBitsPerVoxel;
  srcFile >> iBitsPerVoxel;
  m_header.SetSignificantBitsPerVoxel(iBitsPerVoxel);

  float4 fVoxUnitsX(0), fVoxUnitsY(0), fVoxUnitsZ(0);
  srcFile >> fVoxUnitsX >> fVoxUnitsY >> fVoxUnitsZ;
  m_header.SetVoxelUnits(Triple<float4>(fVoxUnitsX,fVoxUnitsY,fVoxUnitsZ));

  int4 iUnitType(0);
  srcFile >> iUnitType; ++iUnitType;
  m_header.SetVoxelUnitTypeEnum(vxVolumeHeader::UnitTypeEnum(iUnitType));

  int4 iDummyDataOrigin(0);
  srcFile >> iDummyDataOrigin;

  int4 iDummyDataModification(0);
  srcFile >> iDummyDataModification;

  int4 iCompressionType(0);
  srcFile >> iCompressionType;

  // load (skip) preview icon
  int4 iIconX, iIconY;

  srcFile >> iIconX >> iIconY;

  for (int1 x = '\0';  x != 'X'; srcFile.get(x));

  // load icon
  int1 * piIconMem = new char[iIconX * iIconY];
  srcFile.read(piIconMem, iIconX*iIconY);
  srcFile.read(piIconMem, iIconX*iIconY);
  srcFile.read(piIconMem, iIconX*iIconY);
  delete [] piIconMem;

  // load data slice by slice
  int4 iDataSliceSize = iDimX * iDimY;
  int4 iCompressedSize;
  uint1 * pSliceCompressed = new uint1[2*iDataSliceSize];
  uint1 * pSlicePlain      = new uint1[  iDataSliceSize];

  vxBlockVolumeIterator<VoxelType> iterVol(*this);

  for (int z=0; z<iDimZ; ++z)
  {
    switch (iCompressionType)
    {
    case 0: // NO_COMPRESSION:
      srcFile.read((char *)pSlicePlain, iDataSliceSize);
      break;

    case 1: // RUN_LENGTH_ENCODE:
      // read compressed slice size
      srcFile >> iCompressedSize;

      // advance to real data
      for (int1 x = '\0';  x != 'X'; srcFile.get((char&)x)) ;

      // read in slice
      srcFile.read((char *)pSliceCompressed, iCompressedSize);

      // decompress slice
      uint1 *pDataCompressed = pSliceCompressed;
      uint1 *pVoxel = pSlicePlain;
      uint1 uCurrentValue = *pDataCompressed;
      uint1 uRemaining;

      for (uCurrentValue = *(pDataCompressed++);  (uRemaining = (uCurrentValue & 0x7f)); uCurrentValue = *(pDataCompressed++))
      {
        if (uCurrentValue & 0x80)
        {
          while (uRemaining--) *(pVoxel++) = *(pDataCompressed++);
        }
        else
        {
          uCurrentValue = *(pDataCompressed++);
          while (uRemaining--) *(pVoxel++) = uCurrentValue;
        }
      } 
      break;
    } // switch compression type

    // now transfer the current slice to the matching blocks
    uint1 * pVoxel(pSlicePlain);
    VoxelType dummy(0);

    for (int y(0); y<iDimY; ++y)
    {
      iterVol.SetPos(0,y,z);
      for (int x(0); x<iDimX; ++x)
      {
        iterVol.SetVoxel(LocalScope_GetScaledVoxelValue(*pVoxel++, dummy));
        iterVol.IncX();
      }
    } // for all y

  } // for all z

  delete [] pSliceCompressed;
  delete [] pSlicePlain;
} // ReadSlc()


/**
 * Compact the volume
 */
template <class VoxelType> 
void vxBlockVolume<VoxelType>::Compact()
//*******************************************************************
{
  Timer timer;
  int4 iCounter(0);
  vxBlockVolumeIterator<VoxelType> iter(*this);

  for ( ;iter.IsNotAtEnd(); iter.NextBlockZYX())
  {
    if (iter.GetCurrentBlock().IsUpdateOfHomogenousBitPossible())
    {
      iter.SetClearBlock(iter.GetVoxel());
      iter.ClearCurrentBlock();
      iCounter++;
    }
  }
  LogDbg("Compact took "+ToAscii(timer.ElapsedTime())+" for "+ToAscii(iCounter)+" blocks", "vxBlockVolume::VoxelType", "Compact"); timer.Reset();
} // Compact()


/**
 * Return current memory usage
 * @return size in bytes
 */
template <class VoxelType> 
uint4 vxBlockVolume<VoxelType>::GetMemoryUsage() const
//*******************************************************************
{
  uint4 uL2Vol  ( m_dimL2.m_x * m_dimL2.m_y * m_dimL2.m_z * sizeof(vxBlock<vxBlock<VoxelType> >));
  uint4 uL1BlkBg( m_dimL1.m_x * m_dimL1.m_y * m_dimL1.m_z * sizeof(vxBlock<VoxelType>));
  uint4 uL0VoxBg( m_dimL0.m_x * m_dimL0.m_y * m_dimL0.m_z * sizeof(VoxelType));

  uint4 uL1Count(1); // at least one for bg block
  uint4 uL0Count(1); // at least one for bg block

  for (int4 uL1(0); uL1 < m_BlockL2Vol.GetSize(); ++uL1)
  {
    if (m_BlockL2Vol.At(uL1) != m_bgBlockL1Blk)
    {
      ++uL1Count;
      for (int4 uL0(0); uL0 < m_bgBlockL1Blk.GetSize(); ++uL0)
      {
        if (m_BlockL2Vol.At(uL1).At(uL0) != m_bgBlockL0Vox)
        {
          ++uL0Count;
        }
      }
    }
  }

  return uL2Vol + uL1Count * uL1BlkBg + uL0Count * uL0VoxBg;
} // GetMemoryUsage()


/**
 * Returns the number of blocks based on the volume dimensions.
 * 
 * @return The number of blocks needed for the volume.
 */
template <class VoxelType> 
uint4 vxBlockVolume<VoxelType>::GetNumberOfBlocks()
//*******************************************************************
{
  return (( (GetHeader().GetVolDim().X()-1) / vxBlockVolumeIteratorConsts::eL0VoxPosNum ) + 1) *
         (( (GetHeader().GetVolDim().Y()-1) / vxBlockVolumeIteratorConsts::eL0VoxPosNum ) + 1) *
         (( (GetHeader().GetVolDim().Z()-1) / vxBlockVolumeIteratorConsts::eL0VoxPosNum ) + 1);
} // GetNumberOfBlocks()


/**
 * compare the data against another volume
 * @param otherVol the other volume
 * @return equivalent data?
 */
template <class VoxelType>
bool vxBlockVolume<VoxelType>::IsDataEquivalent( vxBlockVolume<VoxelType> & otherVol )
{

  vxBlockVolumeIterator<VoxelType> iter1( * this );
  vxBlockVolumeIterator<VoxelType> iter2( otherVol );

  // could be faster by jumping through non-background blocks in each
  for ( ; iter1.IsNotAtEnd(); iter1.NextBlockZYX(), iter2.NextBlockZYX() )
  {
    for ( ; iter1.IsNotAtEndOfBlock(); iter1.NextZYXinsideBlock(), iter2.NextZYXinsideBlock() )
    {
      if ( iter1.GetVoxel() != iter2.GetVoxel() )
      {
        // early return for speed
        return false;
      }
    }
  }

  return true;

} // IsDataEquivalent()

template <typename VoxelType>
uint4 vxBlockVolume<VoxelType>::GetVoxelsPerBlock() const
{
  return m_dimL0.m_x * m_dimL0.m_y * m_dimL0.m_z; 
}


// undefines
#undef FILE_REVISION


// $Log: vxBlockVolume.C,v $
// Revision 1.17.2.1  2010/07/16 20:31:42  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.17  2007/07/23 16:56:18  romy
// calls the compact method before writing the vol in to the disk.
//
// Revision 1.16  2006/10/03 02:09:51  jmeade
// log message.
//
// Revision 1.15  2006/05/31 16:16:41  geconomos
// porting to visual studio 2005
//
// Revision 1.14  2006/04/03 18:31:59  geconomos
// added histogram to assignment operator
//
// Revision 1.13  2006/03/29 20:14:19  geconomos
// + statics for  L0. L1, & L2 sizes
//
// Revision 1.12  2006/03/09 18:35:39  dongqing
// add vxVoxelCost voxel type to the vxBlockVolume
//
// Revision 1.11  2006/02/09 14:02:23  geconomos
// updated external libraries
//
// Revision 1.10  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.9  2005/05/16 17:52:36  frank
// cleaned up log message when reading old volumes
//
// Revision 1.8  2005/03/23 19:29:52  michael
// Added destructor to class and changed copy constructor to call assignment
// operator instead of having the same code twice.
//
// Revision 1.7  2004/11/02 18:05:24  michael
// for consistency reasons, renamed "SetMemoryLayout" to "SetMemoryLayoutEnum"
//
// Revision 1.6  2004/10/27 17:45:20  geconomos
// volume header encoding needs to be reviewed
//
// Revision 1.5  2004/10/25 23:43:58  michael
// chance ToString_1() to const so that one can call it on const objects, same
// for VarCount()
//
// Revision 1.4  2004/08/30 23:58:04  michael
// Fixed loading of old uint1 volumes into the new UINT2 ones.
//
// Revision 1.3  2004/04/26 12:15:19  frank
// added code to compare the data in two volumes
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.87  2003/06/25 19:04:15  geconomos
// Added code to clamp negtaive values when reading a int2 volume into a uint2 instaniation.
//
// Revision 1.86  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.85  2003/06/10 12:07:13  frank
// Code review of pixel classes
//
// Revision 1.84  2003/06/09 13:56:55  frank
// did not update the progress meter if either progress percentage is negative
//
// Revision 1.83  2003/05/21 18:07:16  dongqing
// remove a debug statement
//
// Revision 1.82  2003/05/19 17:31:08  dongqing
// fix the vxBlockVolume constructor
//
// Revision 1.81  2003/05/19 14:58:54  dongqing
// repalce LogErr() in the Read() with ve::Exception throw
//
// Revision 1.80  2003/05/17 16:19:40  michael
// reformatting
//
// Revision 1.79  2003/05/05 17:32:24  frank
// Issued #3165: Fixed progress bar sometimes not clearing after volume reads.
//
// Revision 1.78  2003/04/30 20:08:43  frank
// Removed const from progress metering
//
// Revision 1.77  2003/04/30 12:00:45  michael
// initial code review
//
// Revision 1.76  2003/04/19 16:25:11  frank
// Added progress bar parameter
//
// Revision 1.75  2003/04/18 16:00:05  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 1.74  2003/04/14 17:11:48  frank
// Exposed progress metering to managed classes
//
// Revision 1.73  2003/04/14 16:44:13  frank
// Separated progress percent and progress message interface, added progress to volume loading.
//
// Revision 1.72  2003/04/07 16:59:22  michael
// renamed "background color" to "background value"
//
// Revision 1.71  2003/01/22 21:25:51  ingmar
// synchronized to new Point2D access to Image pixels
// dimension and units are now Triples
//
// Revision 1.70  2002/12/12 18:34:19  ingmar
// condensed compact debug print
//
// Revision 1.69  2002/12/12 16:59:47  ingmar
// added WriteRaw()
//
// Revision 1.68  2002/10/01 14:21:02  ingmar
// added ReadSlc()
//
// Revision 1.67  2002/09/25 21:17:20  ingmar
// removed non-namespace qualifier
//
// Revision 1.66  2002/08/28 15:05:30  ingmar
// added support for vxComplex
//
// Revision 1.65  2002/08/16 13:41:42  ingmar
// made sure that the top level block of each volume is unique (after copy and assignement)
// -> this now fixes the reference counted sharing of the two levels of blocks underneath
//
// Revision 1.64  2002/08/16 04:33:25  frank
// Added percent complete callbacks to the TimeVolume and some more detailed info about BlockVolume reads.
//
// Revision 1.63  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.62  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.61  2002/07/16 20:14:30  ingmar
// made error tracing more desciptive for linear volume read
//
// Revision 1.60  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.59  2002/07/02 17:14:46  soeren
// added compact function
//
// Revision 1.58  2002/07/01 23:23:30  soeren
// added SetHeader function
//
// $Log: vxBlockVolume.C,v $
// Revision 1.17.2.1  2010/07/16 20:31:42  dongqing
// extend the vxBlockVolume to handle 2048x2048x2048 volume dimension
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.17  2007/07/23 16:56:18  romy
// calls the compact method before writing the vol in to the disk.
//
// Revision 1.16  2006/10/03 02:09:51  jmeade
// log message.
//
// Revision 1.15  2006/05/31 16:16:41  geconomos
// porting to visual studio 2005
//
// Revision 1.14  2006/04/03 18:31:59  geconomos
// added histogram to assignment operator
//
// Revision 1.13  2006/03/29 20:14:19  geconomos
// + statics for  L0. L1, & L2 sizes
//
// Revision 1.12  2006/03/09 18:35:39  dongqing
// add vxVoxelCost voxel type to the vxBlockVolume
//
// Revision 1.11  2006/02/09 14:02:23  geconomos
// updated external libraries
//
// Revision 1.10  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.9  2005/05/16 17:52:36  frank
// cleaned up log message when reading old volumes
//
// Revision 1.8  2005/03/23 19:29:52  michael
// Added destructor to class and changed copy constructor to call assignment
// operator instead of having the same code twice.
//
// Revision 1.7  2004/11/02 18:05:24  michael
// for consistency reasons, renamed "SetMemoryLayout" to "SetMemoryLayoutEnum"
//
// Revision 1.6  2004/10/27 17:45:20  geconomos
// volume header encoding needs to be reviewed
//
// Revision 1.5  2004/10/25 23:43:58  michael
// chance ToString_1() to const so that one can call it on const objects, same
// for VarCount()
//
// Revision 1.4  2004/08/30 23:58:04  michael
// Fixed loading of old uint1 volumes into the new UINT2 ones.
//
// Revision 1.3  2004/04/26 12:15:19  frank
// added code to compare the data in two volumes
//
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.87  2003/06/25 19:04:15  geconomos
// Added code to clamp negtaive values when reading a int2 volume into a uint2 instaniation.
//
// Revision 1.86  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.85  2003/06/10 12:07:13  frank
// Code review of pixel classes
//
// Revision 1.84  2003/06/09 13:56:55  frank
// did not update the progress meter if either progress percentage is negative
//
// Revision 1.83  2003/05/21 18:07:16  dongqing
// remove a debug statement
//
// Revision 1.82  2003/05/19 17:31:08  dongqing
// fix the vxBlockVolume constructor
//
// Revision 1.81  2003/05/19 14:58:54  dongqing
// repalce LogErr() in the Read() with ve::Exception throw
//
// Revision 1.80  2003/05/17 16:19:40  michael
// reformatting
//
// Revision 1.79  2003/05/05 17:32:24  frank
// Issued #3165: Fixed progress bar sometimes not clearing after volume reads.
//
// Revision 1.78  2003/04/30 20:08:43  frank
// Removed const from progress metering
//
// Revision 1.77  2003/04/30 12:00:45  michael
// initial code review
//
// Revision 1.76  2003/04/19 16:25:11  frank
// Added progress bar parameter
//
// Revision 1.75  2003/04/18 16:00:05  dongqing
// Frank fix a problem of using std::library on dongqing's machine
//
// Revision 1.74  2003/04/14 17:11:48  frank
// Exposed progress metering to managed classes
//
// Revision 1.73  2003/04/14 16:44:13  frank
// Separated progress percent and progress message interface, added progress to volume loading.
//
// Revision 1.72  2003/04/07 16:59:22  michael
// renamed "background color" to "background value"
//
// Revision 1.71  2003/01/22 21:25:51  ingmar
// synchronized to new Point2D access to Image pixels
// dimension and units are now Triples
//
// Revision 1.70  2002/12/12 18:34:19  ingmar
// condensed compact debug print
//
// Revision 1.69  2002/12/12 16:59:47  ingmar
// added WriteRaw()
//
// Revision 1.68  2002/10/01 14:21:02  ingmar
// added ReadSlc()
//
// Revision 1.67  2002/09/25 21:17:20  ingmar
// removed non-namespace qualifier
//
// Revision 1.66  2002/08/28 15:05:30  ingmar
// added support for vxComplex
//
// Revision 1.65  2002/08/16 13:41:42  ingmar
// made sure that the top level block of each volume is unique (after copy and assignement)
// -> this now fixes the reference counted sharing of the two levels of blocks underneath
//
// Revision 1.64  2002/08/16 04:33:25  frank
// Added percent complete callbacks to the TimeVolume and some more detailed info about BlockVolume reads.
//
// Revision 1.63  2002/07/22 23:17:21  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.57.2.1  2002/07/02 20:28:40  jaddonisio
// Merge from main trunk, per Soeren changes.
//
// Revision 1.57  2002/06/18 16:10:52  soeren
// disabled performance warning
//
// Revision 1.56  2002/06/17 23:40:58  soeren
// optimized bound .... of linear volume
//
// Revision 1.55  2002/06/17 22:37:51  soeren
// added new compression scheme
//
// Revision 1.54  2002/06/17 21:25:26  ingmar
// added Bound to [0, 4095] for int2 CT datasets being loaded from a linear volume file
//
// Revision 1.53  2002/06/12 21:02:36  soeren
// fixed homogeneous flag
//
// Revision 1.52  2002/06/12 20:43:52  soeren
// fixed dirtyflag
//
// Revision 1.51  2002/06/05 19:13:34  soeren
// Added Homogenous Flag
//
// Revision 1.50  2002/05/20 20:34:47  geconomos
// Fixed compilation errors for VC7.
//
// Revision 1.49  2002/05/17 00:48:19  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 1.48  2002/05/15 18:15:42  soeren
// added isClear function in volume
//
// Revision 1.47  2002/05/14 17:15:25  ingmar
// added copy constructor
//
// Revision 1.46  2002/05/10 23:03:07  soeren
// removed some unnecessary functions and warnings
//
// Revision 1.45  2002/05/09 22:18:02  ingmar
// added ClearWithBackground() to resolve Compilre preformance warning
//
// Revision 1.44  2002/05/09 21:16:14  ingmar
// fixed some performance warnings through template specialization
//
// Revision 1.43  2002/05/08 23:04:07  ingmar
// fixed code to account for copiler performace warning
//
// Revision 1.42  2002/05/08 19:13:52  tushar
// made vxVolume complatible with float4
//
// Revision 1.41  2002/05/07 22:42:36  ingmar
// changed to coding standard compliant enums
//
// Revision 1.40  2002/05/06 23:21:25  soeren
// added Backgroundflag to store
//
// Revision 1.39  2002/05/04 16:36:47  soeren
// added timer (only visible in logdbg)
//
// Revision 1.38  2002/05/03 13:16:17  ingmar
// progress feedback nowalso consideres the work to bwe done before and after getting the blocks from the disk
//
// Revision 1.37  2002/05/02 22:21:53  ingmar
// added progressCallback function to Read() of 2.0 FileVersion files
//
// Revision 1.36  2002/05/02 21:48:26  ingmar
// improved scaling factors for auto window level during Write()
//
// Revision 1.35  2002/05/02 16:59:40  ingmar
// implemented ComputePreviewImage() and used it in Write()
//
// Revision 1.34  2002/05/02 16:52:59  soeren
// one memory leak less in read and write of block volume
//
// Revision 1.33  2002/05/02 16:30:48  soeren
// added fix for slow debug mode - blockVolume
//
// Revision 1.32  2002/05/02 15:58:18  soeren
// switch back to old constructor to avoid memory leak
//
// Revision 1.31  2002/05/01 21:10:55  soeren
// added data modality
//
// Revision 1.30  2002/05/01 17:51:31  soeren
// cleaned up error messages
//
// Revision 1.29  2002/05/01 16:49:29  soeren
// work on write of block vol ver 1.0
//
// Revision 1.28  2002/05/01 16:28:39  soeren
// work on write of block vol ver 1.0
//
// Revision 1.27  2002/05/01 04:42:23  soeren
// work on write of block vol ver 1.0
//
// Revision 1.26  2002/04/30 22:42:10  soeren
// work on write of block vol ver 1.0
//
// Revision 1.25  2002/04/30 22:06:02  soeren
// work on write of block vol ver 1.0
//
// Revision 1.24  2002/04/30 21:44:41  soeren
// fixed write of block
//
// Revision 1.23  2002/04/30 21:40:57  soeren
// added writeblkvol
//
// Revision 1.22  2002/04/30 21:30:38  soeren
// added writeblkvol
//
// Revision 1.21  2002/04/27 22:48:39  ingmar
// now using faster access to single position component
//
// Revision 1.20  2002/04/26 13:26:35  soeren
// added write of old Linear volume format
//
// Revision 1.19  2002/04/25 00:47:54  michael
// Implemented faster initialization of blocks in BlockVolume constructor (ingmar)
//
// Revision 1.18  2002/04/24 18:40:40  soeren
// fixed background bug and added clear of the whole volume by value function
//
// Revision 1.17  2002/04/19 18:43:19  jaddonisio
// added new type bool
//
// Revision 1.16  2002/04/18 20:46:16  ingmar
// removed obsolete GetVoxelRaw() from vxBlockVolumeIterator class
// and renamed GetVoxelRaw(offset) to GetVoxelAtLinOffset(offset)
//
// Revision 1.15  2002/04/18 15:56:10  ingmar
// enabled optmized read and write of new vxBlockVolume
//
// Revision 1.14  2002/04/17 13:58:34  ingmar
// renamed NextBlock() to NextBlockZYX()
//
// Revision 1.13  2002/04/17 00:57:37  ingmar
// working test for native vxBlockVolume read/write
//
// Revision 1.12  2002/04/16 22:09:21  ingmar
// finished write
//
// Revision 1.11  2002/04/16 21:28:35  ingmar
// enabled write
//
// Revision 1.10  2002/04/16 20:43:29  jenny
// commented out work in progress
//
// Revision 1.9  2002/04/16 19:46:45  ingmar
// added more functionality
//
// Revision 1.8  2002/04/12 23:06:42  ingmar
// added From file Contructor
// started OldBlockVolumeFileRead
//
// Revision 1.7  2002/04/11 19:18:05  ingmar
// worked on test for operator =
//
// Revision 1.6  2002/04/11 14:55:21  ingmar
// added GetMemoryUsage()
//
// Revision 1.5  2002/04/10 20:59:09  ingmar
// added more Get functions to VolumeHeader
//
// Revision 1.4  2002/04/10 19:14:16  ingmar
// BlockVolume supports now reading of a linearvolume
//
// Revision 1.3  2002/04/10 18:53:33  ingmar
// worked on new header volume class
//
// Revision 1.2  2002/04/10 18:07:46  ingmar
// worked on new header volume class
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxBlockVolume.C,v 1.17.2.1 2010/07/16 20:31:42 dongqing Exp $
// $Id: vxBlockVolume.C,v 1.17.2.1 2010/07/16 20:31:42 dongqing Exp $
