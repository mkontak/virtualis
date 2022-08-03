// $Id: AnyVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $$
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/AnyVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: ColorTable.cpp
// Description: Implementation of the ColorTable class. 
// Created: June 13, 2001
// Author: [Name] [Email]
//
// Complete History at bottom of file.

/*
  Class for retriving the dimension information
*/


// include declarations
#include "stdafx.h"
#include "AnyVolume.h"



// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"



// namespace declarations
using namespace hdr;
using namespace std;



class AVGetHdrMap : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVGetHdrMap() {}

  /// action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline void Action(VOLUME& vol, VOLAPI&, VOXEL&)
  {
    m_pMap = &vol.GetHdrMap();
  }

// data members
public:
  const VarMap* m_pMap;
};


/**
 *  Class for retriving the dimension information
 */
class AVGetDim : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVGetDim() : m_dim(0, 0, 0) {}

  /// action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline void Action(VOLUME& vol, VOLAPI&, VOXEL&)
  {
    m_dim = vol.GetDim();
  }

// data members
public:
  Triple<uint4> m_dim;
};


/**
 *  Class for retriving the dimension information
 */
class AVGetUnits : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVGetUnits() : m_units(1.0f, 1.0f, 1.0f) {}

  /// action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline void Action(VOLUME& vol, VOLAPI&, VOXEL&)  {
    m_units = vol.GetUnits();
  }
// data members
public:
  Triple<float4> m_units;
};


/**
 *  Class for retriving the size information
 */
class AVGetSize : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVGetSize() : m_size(0.0f, 0.0f, 0.0f) {}

  /// action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline void Action(VOLUME& vol, VOLAPI&, VOXEL&)  {
    m_size = vol.GetSize();
  }
// data members
public:
  Triple<float4> m_size;
};


/**
 *  Class for retriving the bits per voxel information
 */
class AVGetBitsPerVoxel : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVGetBitsPerVoxel() : m_uBitsPerVoxel(0) {}

  /// action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline void Action(VOLUME& vol, VOLAPI&, VOXEL&)  {
    m_uBitsPerVoxel = vol.GetBitsPerVoxel();
  }
// data members
public:
  uint1 m_uBitsPerVoxel;
};



/**
 *  Class for retriving the min max voxel information
 */
class AVGetDataMinMax : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVGetDataMinMax() : m_fMinMax(0.0f, 0.0f) {}

  /// action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline void Action(VOLUME& vol, VOLAPI&, VOXEL&)  
  {
    VOLAPI::VolIterator vIter(vol.GetVolIterator()); 
    for ( ; !vIter.End(); ++vIter) {
      const float4 voxelData = vIter.GetVoxelRaw();
      m_fMinMax.first = Min(m_fMinMax.first, voxelData);
      m_fMinMax.second = Max(m_fMinMax.second, voxelData);
    }
  }
// data members
public:
  std::pair<float4, float4> m_fMinMax;
};


class AVCreateByDim : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVCreateByDim(AnyVolume& anyVolume, const Triple<uint4>& dim, const Triple<float4>& units, const uint4 bitsPerVoxel)
  :m_anyVolume(anyVolume), m_dim(dim), m_units(units), m_bitsPerVoxel(bitsPerVoxel) {}

  /// Action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline  void Action(VOLUME& vol, VOLAPI&, VOXEL&) {
    m_anyVolume.SetVolumePtr(static_cast<VOLUME*> ( new VOLUME(m_dim, m_bitsPerVoxel, m_units)));
  }

// data members
public:
  const Triple<uint4>& m_dim;
  const Triple<float4>& m_units;
  const uint4& m_bitsPerVoxel;
  AnyVolume& m_anyVolume;
};


class AVCopyVolume : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVCopyVolume(AnyVolume& anyVolume, const AnyVolume& other)
  :m_anyVolume(anyVolume), m_other(other) {}

  /// Action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline  void Action(VOLUME& vol, VOLAPI&, VOXEL&) 
  {
    *static_cast<VOLUME*>(m_anyVolume.GetVolume()) = *static_cast<VOLUME*> ( m_other.GetVolume());
    (*static_cast<VOLUME*>(m_anyVolume.GetVolume())).IncRefCount();
  }

// data members
public:
  AnyVolume& m_anyVolume;
  const AnyVolume& m_other;
};


class AVGetVoxel : public AnyVolumeAction {
// member functions
public:
  AVGetVoxel(const Point<uint2>& pos)
  : m_pos(pos){}

  /// Action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline  void Action(VOLUME& vol, VOLAPI&, VOXEL&) 
  {
    m_voxel = vol.GetVoxel(m_pos);
  }

// data members
public:
  Point<uint2> m_pos;
  float4 m_voxel;
};

class AVRead : public AnyVolumeAction {
// member functions
public:
  AVRead(const std::string& sFilePath)
  : m_sFilePath(sFilePath), m_pVolume(NULL){}

  /// Action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline  void Action(VOLUME&, VOLAPI&, VOXEL&) 
  {
    VOLUME * pVolume = new VOLUME;
    pVolume->Read(m_sFilePath);
    m_pVolume = static_cast<void *>(pVolume);
  /*
    // Temp code for creating a smaller volume
    const Triple<uint4> newSize(256, 256, 104);
    pVolume->Resize(newSize);
    pVolume->Write("d:/small.vol");
    // end of temp code
  */
  }

// data members
public:
  const std::string& m_sFilePath;
  void * m_pVolume;
};


class AVWrite : public AnyVolumeAction {
// member functions
public:
  AVWrite(const std::string& sFilePath)
  : m_sFilePath(sFilePath){}

  /// Action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline  void Action(VOLUME& vol, VOLAPI&, VOXEL&) 
  {
    vol.Write(m_sFilePath);
  }

// data members
public:
  const std::string& m_sFilePath;
};


/**
 *  Class for retriving the dimension information
 */
class AVFit : public AnyVolumeAction {
// member functions
public:
  /// constructor
  AVFit(AnyVolume& destVol, const hdr::ResampleTypeEnum resampleType) 
    : m_destVol(destVol), m_resampleType(resampleType) {}

  /// action
  template<class VOLUME, class VOLAPI, class VOXEL>
  inline void Action(VOLUME& vol, VOLAPI&, VOXEL&)
  {
    VOLUME& destVol = *(static_cast<VOLUME*>(m_destVol.GetVolume()));
    vol.Fit(destVol, m_resampleType);
  }

// data members
public:
  AnyVolume& m_destVol;
  const hdr::ResampleTypeEnum m_resampleType;
};


/**
 *  Default constructor
 */
AnyVolume::AnyVolume()
{
  m_bValid = false;
  m_bDataRangeObtained = false;
  m_eDataType = UINT1;
  m_eLayout = hdr::LINEAR;
  m_pVolume = NULL;
} // AnyVolume()


/**
 *  Constructor with a pointer to volume
 */
AnyVolume::AnyVolume(void * volume)
{
  m_bValid = false;
  m_bDataRangeObtained = false;
  m_eDataType = UINT1;
  m_eLayout = hdr::LINEAR;
  m_pVolume = NULL;

  Clear();
  m_pVolume = volume;
  if (!volume) {
    m_bValid = false;
    LogErr("<message for log>","AnyVolume","AnyVolume");

  } else {
    try {
      BlockVolB* vol = static_cast<BlockVolB  *>(m_pVolume);
      m_eLayout = vol->GetMemoryLayout();
      m_eDataType = vol->GetDataType();
      m_bValid = true;
      vol->IncRefCount();
    } catch (...) {
      LogErr("<message for log>","AnyVolume","AnyVolume");
    }
  }

}


/**
 * Destructor
 */
AnyVolume::~AnyVolume()
{
  Clear();
} // ~AnyVolume()


/**
 * Create an empty AnyVolume with the specified layout.
 * 
 * @param eLayout
 * @param eDataType
 * @param dim
 * @param units
 */
void AnyVolume::Create(const hdr::MemoryLayoutEnum eLayout, const hdr::DATATYPE eDataType, const Triple<uint4>& dim, const Triple<float4>& units)
{
  Clear();
  uint4 bitsPerVoxel;
  switch (eDataType) {
  case  BL:
    bitsPerVoxel = 1;
    break;
  case UINT1:
    bitsPerVoxel = 8;
    break;
  case UINT2:
    bitsPerVoxel = 16;
    break;
  case UINT4:
    bitsPerVoxel = 32;
    break;
  case INT1:
    bitsPerVoxel = 8;
    break;
  case INT2:
    bitsPerVoxel = 16;
    break;
  case INT4:
    bitsPerVoxel = 32;
    break;
  case FLT4:
    bitsPerVoxel = 32;
    break;
  case SEG:
    bitsPerVoxel = 32;
    break;
  default:
    throw ex::VException(LogRec("Unsupported voxel format","AnyVolume","Create"));
  };

  m_eDataType = eDataType;
  m_eLayout = eLayout;
  m_bValid = true;
  AVCreateByDim avCreateByDim(*this, dim, units, bitsPerVoxel);
  ApplyAction(avCreateByDim);
  IncRefCount();
}


void AnyVolume::operator = (const AnyVolume& other)
{
  m_bValid = other.IsValid();
  if (m_bValid) {
    m_bDataRangeObtained = other.GetDataRangeObtained();
    m_fDataRange = const_cast<AnyVolume&>(other).GetDataMinMax();
    m_eDataType = other.GetDataType();
    m_eLayout = other.GetLayout();
    m_pVolume = other.GetVolume();
    IncRefCount();
  }
}


void AnyVolume::DeepCopy(const AnyVolume& other)
{
  m_bValid = other.IsValid();
  if (m_bValid) {
    m_bDataRangeObtained = other.GetDataRangeObtained();
    m_fDataRange = const_cast<AnyVolume&>(other).GetDataMinMax();
    m_eDataType = other.GetDataType();
    m_eLayout = other.GetLayout();

    AVCopyVolume avCopyVolume(*this, other);
    ApplyAction(avCopyVolume);
  }
}


void AnyVolume::Clear()
{
 if (m_bValid && DecRefCount() == 0) {
    if (m_eLayout == hdr::LINEAR) {
      if (m_eDataType == BL   ) delete static_cast<const LinVolB  *>(m_pVolume);
      else if (m_eDataType == UINT1) delete (const LinVolUC *)(m_pVolume);
      else if (m_eDataType == UINT2) delete (const LinVolUS *)(m_pVolume);
      else if (m_eDataType == UINT4) delete (const LinVolUI *)(m_pVolume);
      else if (m_eDataType == INT1 ) delete (const LinVolC  *)(m_pVolume);
      else if (m_eDataType == INT2 ) delete (const LinVolS  *)(m_pVolume);
      else if (m_eDataType == INT4 ) delete (const LinVolI  *)(m_pVolume);
      else if (m_eDataType == FLT4 ) delete (const LinVolF  *)(m_pVolume);

      /*
      else if (m_eDataType == UINT1) delete static_cast<const LinVolUC *>(m_pVolume);
      else if (m_eDataType == UINT2) delete static_cast<const LinVolUS *>(m_pVolume);
      else if (m_eDataType == UINT4) delete static_cast<const LinVolUI *>(m_pVolume);
      else if (m_eDataType == INT1 ) delete static_cast<const LinVolC  *>(m_pVolume);
      else if (m_eDataType == INT2 ) delete static_cast<const LinVolS  *>(m_pVolume);
      else if (m_eDataType == INT4 ) delete static_cast<const LinVolI  *>(m_pVolume);
      else if (m_eDataType == FLT4 ) delete static_cast<const LinVolF  *>(m_pVolume);
*/
      else {
      }
    } else if (m_eLayout == hdr::BLOCK) {
      if (m_eDataType == BL   ) delete static_cast<const BlockVolB  *>(m_pVolume);
      else if (m_eDataType == UINT1) delete (const BlockVolUC *)(m_pVolume);
      else if (m_eDataType == UINT2) delete (const BlockVolUS *)(m_pVolume);
      else if (m_eDataType == UINT4) delete (const BlockVolUI *)(m_pVolume);
      else if (m_eDataType == INT1 ) delete (const BlockVolC  *)(m_pVolume);
      else if (m_eDataType == INT2 ) delete (const BlockVolS  *)(m_pVolume);
      else if (m_eDataType == INT4 ) delete (const BlockVolI  *)(m_pVolume);
      else if (m_eDataType == FLT4 ) delete (const BlockVolF  *)(m_pVolume);

/*
      else if (m_eDataType == UINT1) delete static_cast<const BlockVolUC *>(m_pVolume);
      else if (m_eDataType == UINT2) delete static_cast<const BlockVolUS *>(m_pVolume);
      else if (m_eDataType == UINT4) delete static_cast<const BlockVolUI *>(m_pVolume);
      else if (m_eDataType == INT1 ) delete static_cast<const BlockVolC  *>(m_pVolume);
      else if (m_eDataType == INT2 ) delete static_cast<const BlockVolS  *>(m_pVolume);
      else if (m_eDataType == INT4 ) delete static_cast<const BlockVolI  *>(m_pVolume);
      else if (m_eDataType == FLT4 ) delete static_cast<const BlockVolF  *>(m_pVolume);
*/
      else {
      }
    } else {
    }
 }
 m_bValid = false;
 m_bDataRangeObtained = false;
 m_eDataType = UINT1;
 m_eLayout = hdr::LINEAR;
 m_pVolume = NULL;
}

const std::string & AnyVolume::GetFileName() const
{
  return static_cast<LinVolUC*>(m_pVolume)->GetFileName();
}

const VarMap& AnyVolume::GetHdrMap() const
{
  AVGetHdrMap avAction;
  ApplyAction(avAction);
  return *avAction.m_pMap;
}

const Triple<uint4> AnyVolume::GetDim() const
{
  AVGetDim avAction;
  ApplyAction(avAction);
  return avAction.m_dim;
} // GetDim()


const Triple<float4> AnyVolume::GetSize() const
{
  AVGetSize avGetSize;
  ApplyAction(avGetSize);
  return avGetSize.m_size;
}

const Triple<float4> AnyVolume::GetUnits() const
{
  AVGetUnits avGetUnits;
  ApplyAction(avGetUnits);
  return avGetUnits.m_units;
}

const float4 AnyVolume::GetVoxel(const Point<uint2> & position) const
{
  AVGetVoxel avGetVoxel(position);
  ApplyAction(avGetVoxel);
  return avGetVoxel.m_voxel;
}// GetVoxel()



/**
 * Read a volume from disk
 * 
 * @param sFilePath
 * @exception ex::FileNotFoundException
 * @exception ex::DataCorruptionException
 * @exception ex::IOException
 * @exception ex::OutOfMemoryException
 * @exception ex::InvalidVolume
 */
void AnyVolume::Read(const std::string sFilePath)
{
  Clear();
  m_header.Clear();
  m_header.Read(sFilePath);
  m_bValid = true;

  bool bBoolTemp(true);
  uint1 uUint1Temp(0);
  uint2 uUint2Temp(0);
  uint4 uUint4Temp(0);
  int1 iInt1Temp(0);
  int2 iInt2Temp(0);
  int4 iInt4Temp(0);
  float4 fFloat4Temp(0.0F);

  const std::string & sDataType = m_header.GetDataType();
  if (sDataType == typeid(bBoolTemp).name()) m_eDataType = BL;
  else if (sDataType == typeid(uUint1Temp).name()) m_eDataType = UINT1;
  else if (sDataType == typeid(uUint2Temp).name()) m_eDataType = UINT2;
  else if (sDataType == typeid(uUint4Temp).name()) m_eDataType = UINT4;
  else if (sDataType == typeid(iInt1Temp).name()) m_eDataType = INT1;
  else if (sDataType == typeid(iInt2Temp).name()) m_eDataType = INT2;
  else if (sDataType == typeid(iInt4Temp).name()) m_eDataType = INT4;
  else if (sDataType == typeid(fFloat4Temp).name()) m_eDataType = FLT4;
  else
    m_bValid = false;

  m_eLayout = static_cast<hdr::MemoryLayoutEnum>(m_header.GetDiskLayout());

  // save the pointer in a void pointer
  if (m_bValid) {
    AVRead avRead(sFilePath);
    ApplyAction(avRead);
    m_pVolume = avRead.m_pVolume;
    assert(m_pVolume);
    IncRefCount();
  }

  // Force the range to be [0, 4095] for ushort and short, RSNA ONLY HACK!!!
  if (GetDataType() == hdr::UINT2 || GetDataType() == hdr::INT2) 
    SetMinMaxValue(0, 4095);
} // Read()


void AnyVolume::Write(const std::string sFilePath)
{
  AVWrite avWrite(sFilePath);
  ApplyAction(avWrite);
}


const float4 AnyVolume::GetMaxValue() const
{
  // note: should use "numeric_limits<bool>::max()"
  // but Global.h already defines max()!
  switch (m_eDataType) 
  {
  case BL:  
    return static_cast<float4>(true);
  case UINT1: 
    return static_cast<float4>(UCHAR_MAX);
  case UINT2: // ushorts contain 12 bits of significant data usually
    return static_cast<float4>(4096); 
    //return static_cast<float4>(USHRT_MAX);
  case UINT4: 
    return static_cast<float4>(UINT_MAX);
  case INT1:  
    return static_cast<float4>(CHAR_MAX);
  case INT2:  
    return static_cast<float4>(4096);
    //return static_cast<float4>(SHRT_MAX);
  case INT4:  
    return static_cast<float4>(INT_MAX);
  case FLT4:  
    return static_cast<float4>(FLT_MAX);
  default:
    return 0;
  }
} // GetMaxValue()


const std::pair<float4, float4> AnyVolume::GetDataMinMax()
{
  if (m_bDataRangeObtained)
    return m_fDataRange;
  AVGetDataMinMax avGetDataMinMax;
  ApplyAction(avGetDataMinMax);
  m_bDataRangeObtained = true;
  m_fDataRange = avGetDataMinMax.m_fMinMax; 
  return avGetDataMinMax.m_fMinMax;
} // GetDataMinMax()


const uint4 AnyVolume::GetBitsPerVoxel() const
{
  AVGetBitsPerVoxel avGetBitsPerVoxel;
  ApplyAction(avGetBitsPerVoxel);
  return avGetBitsPerVoxel.m_uBitsPerVoxel;
}

void AnyVolume::SetMinMaxValue(float4 minValue, float4 maxValue)
{
  m_fDataRange = pair<float4, float4>(minValue, maxValue);
  m_bDataRangeObtained = true;
}


/// Increase the reference count to the void*
const uint4 AnyVolume::IncRefCount()
{
  try {
    BlockVolB* vol = static_cast<BlockVolB  *>(m_pVolume);
    return vol->IncRefCount();
  } catch (...) {
    LogErr("Invalid volume","AnyVolume","IncRefCount");
    return 0;
  }
}

/// Decrease the reference count to the void*
const uint4 AnyVolume::DecRefCount()
{
  try {
    BlockVolB* vol = static_cast<BlockVolB  *>(m_pVolume);
    return vol->DecRefCount();
  } catch (...) {
    LogErr("Invalid volume","AnyVolume","DecRefCount");
    return 0;
  }
}


void AnyVolume::Fit(AnyVolume & destVol, hdr::ResampleTypeEnum resampleType) const
{
  //assert(GetDataType() == destVol.GetDataType() && GetLayout() == destVol.GetLayout());
  AVFit avFit(destVol, resampleType);
  ApplyAction(avFit);
}



#undef FILE_REVISION



// $Log: AnyVolume.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.15  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 3.14  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 3.13  2002/03/05 19:58:05  ingmar
// changed cast to C-style, as Intel C++ compiler complained about the static_cast
//
// Revision 3.12  2002/02/20 01:47:42  michael
// removed out commented code and warning
//
// Revision 3.11  2002/01/21 17:34:40  jaddonisio
// BlkVolSegmented is not in use any more.
//
// Revision 3.10  2002/01/15 21:13:27  uday
// modfied createsolidcylinder() for demo
//
// Revision 3.9  2001/12/18 19:22:24  dmitry
// AVGetHdrMap class and GetHdrMap() added.
//
// Revision 3.8  2001/12/07 19:07:25  liwei
// Code beautify.
//
// Revision 3.7  2001/11/19 22:56:18  liwei
// Minor changes.
//
// Revision 3.6  2001/11/15 23:15:57  liwei
// Assume the data range of any uint2 and int2 volume to be [0, 4095].
//
// Revision 3.5  2001/11/06 16:54:31  soeren
// fixed destructor
//
// Revision 3.4  2001/11/06 15:05:45  soeren
// beautified
//
// Revision 3.3  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.2  2001/10/23 21:47:03  liwei
// Code cleaned.
//
// Revision 3.1  2001/10/16 19:52:49  liwei
// A bug that cause the CPU based renderer to crash has been fixed.
//
// Revision 3.0  2001/10/14 23:02:20  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Sep 26 2001 19:37:10   liwei
// Bugs in GetMinMaxData() fixed.
// 
//    Rev 2.2   Sep 25 2001 12:55:22   liwei
// Replaced throw exception with loggings. 
// 
//    Rev 2.1   Sep 18 2001 18:01:00   liwei
// Code cleaned.
// 
//    Rev 2.0   Sep 16 2001 23:47:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:56   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:17  soeren
// renamed volume  directory
//
// Revision 1.26  2001/08/22 00:17:07  liwei
// Bilinear interpolation with Block texture volume added.
//
// Revision 1.25  2001/08/21 00:09:39  liwei
// All action on AnyVolume noe uses AnyVolumeAction class; AnyVolumeAction.h removed,
//
// Revision 1.24  2001/08/20 20:10:36  ingmar
// included Global.h to reduce compiler warnings
//
// Revision 1.23  2001/08/18 00:23:42  liwei
// Added AnyVolumeAction class. It will replace the old way using define and include.
//
// Revision 1.22  2001/08/15 15:40:28  liwei
// Replace ownership flag with the reference count in Volume object.
//
// Revision 1.21  2001/08/15 15:25:17  frank
// Voxelized shell of cylinder instead of solid.
//
// Revision 1.20  2001/08/15 14:34:46  frank
// Extended error message.
//
// Revision 1.19  2001/08/15 14:02:40  frank
// Typo.
//
// Revision 1.18  2001/08/15 13:46:06  frank
// Added skeleton voxelization function.
//
// Revision 1.17  2001/08/14 15:19:15  liwei
// Added Create method.
//
// Revision 1.16  2001/08/08 17:33:36  liwei
// Using vol.GetVolAPI rather than creating an actual object.
//
// Revision 1.15  2001/08/02 15:54:17  liwei
// GetVoxel rewritten with the new prototype of AnyVolumeAction
//
// Revision 1.14  2001/07/27 23:59:59  liwei
// Max, min values are not searched unless GetMaxMin() is called and SetMinMax
// has not been called. Prefer to obtain the values from the histogram if available.
//
// Revision 1.13  2001/07/26 21:07:59  liwei
// Minor changes.
//
// Revision 1.12  2001/07/26 15:03:40  liwei
// Bug in Creating a small volume fixed. It is also the reason of why 3d renderer can't
// handle block volume correctly.
// Remember, to linearly traverse a block volume, use VolIterator.NextZYX rather
// VolIterator++.
//
// Revision 1.11  2001/07/25 23:00:30  liwei
// It seems the CVS merging lost some of the changes I made before. Have to do again.
//
// Revision 1.10  2001/07/25 22:15:35  liwei
// GetDataMinMax rewritten with AnyVolumeAction to support BlkSegVol as well as using
// iterator for efficiency.
//
// Revision 1.9  2001/07/25 19:08:11  liwei
// AnyVolume now supports BlkSegVol.
// Exception handling updated to use the LogRecord.
//
// Revision 1.8  2001/07/25 01:31:12  tushar
// fixed some compiler errors (logger)
//
// Revision 1.7  2001/07/17 19:47:19  liwei
// Added exception handling.
//
// Revision 1.6  2001/07/17 18:14:44  geconomos
// Removed try/catch handler around in ReadMethod(); let propagate
//
// Revision 1.5  2001/07/10 16:37:53  liwei
// Conversion function revised
//
// Revision 1.4  2001/06/27 15:27:25  geconomos
// Worked on organiziation
//
// Revision 1.3  2001/06/26 22:38:06  liwei
// Adding convertion functions to convert any voxel type to unsigned char.
//
// Revision 1.2  2001/06/25 21:23:56  geconomos
// moved from 51_applicationModel
//
// Revision 1.1  2001/06/25 17:54:10  liwei
// VolumeVVS renamed back to AnyVolume
//
// Revision 1.2  2001/06/20 21:39:20  liwei
// AnyVolume::GetDim() rewritten with AnyVolumeAction file
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/AnyVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: AnyVolume.C,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
