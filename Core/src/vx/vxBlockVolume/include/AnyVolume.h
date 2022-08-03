// $Id: AnyVolume.h,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li liwei@viatronix.com
//	
//
// Complete History at bottom of file.

#ifndef AnyVolume_h
#define AnyVolume_h


// include declarations
#include "Volume.h"



// forward declarations
class Skeleton;



// define declarations
#define FILE_REVISION "$Revision: 1.2 $"



namespace ex
{
  class InvalidVolume : public RuntimeException
  {
  public:
    InvalidVolume(const vxLogRecord& logRecord) : RuntimeException(logRecord)
    {
    }

  };

  class UnsupportedVoxelFormat : public RuntimeException
  {
  public:
    UnsupportedVoxelFormat(const vxLogRecord& logRecord) : RuntimeException(logRecord)
    {
    }
  };
};

class AnyVolumeAction 
{
public:
  template<class VOLUME, class VOLAPI, class VOXEL>
  void Action(VOLUME&, VOLAPI&, VOXEL&);
};

/**
 * This class is the encapsulation of various templatized volume classes.
 */
class AnyVolume
{
// Member Functions
public:
	/**
	 * The voxel type in the volume pointed by the volume pointer.
	 */
  hdr::DATATYPE m_eDataType;
	/**
	 * Default constructor
	 */
  AnyVolume();
	/**
	 * Constructor with a pointer to an existing volume object.
	 * 
	 * @param volume
	 * @exception ex::InvalidVolume
	 */
  AnyVolume(void * volume);

  /// Assignment operator, sharing the memory of the volume object
  void operator = (const AnyVolume& other);

  /// Clone from the other anyvolume
  void DeepCopy(const AnyVolume& other);

	/// Create an empty AnyVolume with the specified layout, data type, dimension and units
  void Create(const hdr::MemoryLayoutEnum eLayout, const hdr::DATATYPE eDataType, const Triple<uint4>& dim , const Triple<float4>& units = Triple<float4>(1.0f, 1.0f, 1.0f));

  /// Set the volume pointer
  void SetVolumePtr(void* pVolume)
  {
    m_pVolume = pVolume;
  }

  /// Destructor
  virtual ~AnyVolume();

  const VarMap& GetHdrMap() const;
  const Triple<uint4> GetDim() const;
  const Triple<float4> GetSize() const;
  const Triple<float4> GetUnits() const;

  /// Returns the number of significant bits per voxel.
  const uint4 GetBitsPerVoxel() const;

  const float4 GetVoxel(const Point<uint2> & position) const;
  void Read(const std::string sFilePath);

  void Write(const std::string sFilePath);

  void Clear();
  const float4 GetMaxValue() const;
  const std::pair<float4, float4> GetDataMinMax();

  const HeaderVolume<int4> GetHeader() const 
  {
    return m_header;
  }

  const bool GetDataRangeObtained() const
  {
    return (m_bDataRangeObtained);
  }

  const bool IsValid() const
  {
    return (m_bValid);
  }

  inline void* GetVolume() const
  {
    return (m_pVolume);
  }

  /// return the data type
  const hdr::DATATYPE GetDataType() const
  {
    return (m_eDataType);
  }

  /// return the memory layout
  hdr::MemoryLayoutEnum GetLayout() const
  {
    return (m_eLayout);
  }

  /// Returns the file name string
  const std::string & GetFileName() const;

  /// Set min max value otained through other source, such as histogram
  void SetMinMaxValue(float4 minValue, float4 maxValue);

  void Fit(AnyVolume & destVol, hdr::ResampleTypeEnum resampleType) const;

private:
  template<class VOLUME, class VOLAPI, class VOXEL>
  void Read(VOLUME& vol, VOLAPI&, VOXEL&, std::vector<void*>& args);

  template<class VOLUME, class VOLAPI, class VOXEL>
  void Clear(VOLUME& vol, VOLAPI&, VOXEL&, std::vector<void*>& args);

  /// Increase the reference count to the void*
  const uint4 IncRefCount();

  /// Decrease the reference count to the void*
  const uint4 DecRefCount();

  // Data Members
public:
  /// The layout of the volume data;
  hdr::MemoryLayoutEnum m_eLayout;

  /// The information from the volume header.
  HeaderVolume<int4> m_header;

  /// The data range which corresponds to the range of Hounsfield units.
  std::pair<float4, float4> m_fDataRange;

private:
  /// Whether or not the volume has been sucessfully loaded.
  bool m_bValid;

  /// A pointer to the mystery layout volume data.
  void * m_pVolume;

  /// Whether the min max value of the volume have been computed
  mutable bool m_bDataRangeObtained;

// body of templatized functions
public:
  /// Apply action to the AnyVolume object
  template<class AVAction>
  void ApplyAction(AVAction& avAction) const
  {
    if ( IsValid() )
    {
      if ( m_eLayout == hdr::LINEAR )
      {
        switch ( m_eDataType )
        {
        case hdr::BL:    { vol::LinVolBool & vol = *static_cast<vol::LinVolBool  *>(GetVolume()); bool  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::UINT1: { vol::LinVolUInt1& vol = *static_cast<vol::LinVolUInt1 *>(GetVolume()); uint1 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::UINT2: { vol::LinVolUInt2& vol = *static_cast<vol::LinVolUInt2 *>(GetVolume()); uint2 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::UINT4: { vol::LinVolUInt4& vol = *static_cast<vol::LinVolUInt4 *>(GetVolume()); uint4 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break; 
        case hdr::INT1:  { vol::LinVolInt1 & vol = *static_cast<vol::LinVolInt1  *>(GetVolume()); int1  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break; 
        case hdr::INT2:  { vol::LinVolInt2 & vol = *static_cast<vol::LinVolInt2  *>(GetVolume()); int2  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::INT4:  { vol::LinVolInt4 & vol = *static_cast<vol::LinVolInt4  *>(GetVolume()); int4  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
     // case hdr::FLT4:  { vol::LinVolFloat4&vol = *static_cast<vol::LinVolFloat4*>(GetVolume());float4 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        default:
          LogErr("Unsupported voxel format for linear volume","AnyVolume","Action");
        }
      }
      else if ( m_eLayout == hdr::BLOCK )
      {
        switch ( m_eDataType )
        {
        case hdr::BL:    { vol::BlkVolBool & vol = *static_cast<vol::BlkVolBool  *>(GetVolume()); bool  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::UINT1: { vol::BlkVolUInt1& vol = *static_cast<vol::BlkVolUInt1 *>(GetVolume()); uint1 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::UINT2: { vol::BlkVolUInt2& vol = *static_cast<vol::BlkVolUInt2 *>(GetVolume()); uint2 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::UINT4: { vol::BlkVolUInt4& vol = *static_cast<vol::BlkVolUInt4 *>(GetVolume()); uint4 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break; 
        case hdr::INT1:  { vol::BlkVolInt1 & vol = *static_cast<vol::BlkVolInt1  *>(GetVolume()); int1  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break; 
        case hdr::INT2:  { vol::BlkVolInt2 & vol = *static_cast<vol::BlkVolInt2  *>(GetVolume()); int2  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::INT4:  { vol::BlkVolInt4 & vol = *static_cast<vol::BlkVolInt4  *>(GetVolume()); int4  voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        case hdr::FLT4:  { vol::BlkVolFloat4&vol = *static_cast<vol::BlkVolFloat4*>(GetVolume());float4 voxel(0); avAction.Action(vol, vol.GetVolAPI(), voxel); } break;
        default:
          LogErr("Unsupported voxel format for bloxk volume","AnyVolume","Action");
        }
      }
    }
  } // ApplyAction()
}; // class AnyVolume

#undef FILE_REVISION

#endif // AnyVolume_h

// $Log: AnyVolume.h,v $
// Revision 1.2  2005/07/01 12:30:57  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.14  2003/05/16 11:44:23  michael
// code review
//
// Revision 3.13  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 3.12  2002/03/15 19:40:08  ingmar
// removed  =0 on Action to make Compiler happy
//
// Revision 3.11  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 3.10  2002/01/21 17:33:54  jaddonisio
// BlkVolSegmented is not in use any more.
//
// Revision 3.9  2001/12/24 16:46:19  ingmar
// added class Sec_30_volume to structure the doxygen documentation
//
// Revision 3.8  2001/12/18 19:22:24  dmitry
// AVGetHdrMap class and GetHdrMap() added.
//
// Revision 3.7  2001/12/13 22:19:22  soeren
// Added Patient Information in 3D View
//
// Revision 3.6  2001/12/07 19:07:25  liwei
// Code beautify.
//
// Revision 3.5  2001/11/19 22:56:18  liwei
// Minor changes.
//
// Revision 3.4  2001/11/09 00:19:35  ingmar
// made volume histogram appear at full 4096 density range in transfer function editor
//
// Revision 3.3  2001/10/30 15:22:33  liwei
// Minor change.
//
// Revision 3.2  2001/10/26 18:25:59  liwei
// Removed the support to Linear floating point volume which causes "Internal Compiler Error"
// for release build.
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
//    Rev 2.4   Oct 10 2001 14:35:08   soeren
// some changes for sachin
// 
//    Rev 2.3   Sep 26 2001 19:37:10   liwei
// Bugs in GetMinMaxData() fixed.
// 
//    Rev 2.2   Sep 25 2001 12:55:22   liwei
// Replaced throw exception with loggings. 
// 
//    Rev 2.1   Sep 24 2001 14:42:14   frank
// Spelling
// 
//    Rev 2.0   Sep 16 2001 23:47:28   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:58   ingmar
// Initial revision.
// Revision 1.3  2001/09/13 21:33:45  ingmar
// code beautify
//
// Revision 1.2  2001/09/13 15:59:15  liwei
// Minor change.
//
// Revision 1.1  2001/09/01 00:12:17  soeren
// renamed volume  directory
//
// Revision 1.20  2001/08/21 00:09:39  liwei
// All action on AnyVolume noe uses AnyVolumeAction class; AnyVolumeAction.h removed,
//
// Revision 1.19  2001/08/19 19:14:05  ingmar
// removed warnings for uninitialized vars
//
// Revision 1.18  2001/08/18 00:23:42  liwei
// Added AnyVolumeAction class. It will replace the old way using define and include.
//
// Revision 1.17  2001/08/15 15:40:28  liwei
// Replace ownership flag with the reference count in Volume object.
//
// Revision 1.16  2001/08/15 13:46:06  frank
// Added skeleton voxelization function.
//
// Revision 1.15  2001/08/14 15:19:15  liwei
// Added Create method.
//
// Revision 1.14  2001/08/08 17:33:36  liwei
// Using vol.GetVolAPI rather than creating an actual object.
//
// Revision 1.13  2001/08/02 15:54:17  liwei
// GetVoxel rewritten with the new prototype of AnyVolumeAction
//
// Revision 1.12  2001/07/27 23:59:59  liwei
// Max, min values are not searched unless GetMaxMin() is called and SetMinMax
// has not been called. Prefer to obtain the values from the histogram if available.
//
// Revision 1.11  2001/07/26 21:07:59  liwei
// Minor changes.
//
// Revision 1.10  2001/07/25 22:15:35  liwei
// GetDataMinMax rewritten with AnyVolumeAction to support BlkSegVol as well as using
// iterator for efficiency.
//
// Revision 1.9  2001/07/25 19:08:11  liwei
// AnyVolume now supports BlkSegVol.
// Exception handling updated to use the LogRecord.
//
// Revision 1.8  2001/07/25 01:31:22  tushar
// fixed some compiler errors (logger)
//
// Revision 1.7  2001/07/17 20:19:27  liwei
// Added more exception handling.
//
// Revision 1.6  2001/07/17 19:47:30  liwei
// Added exception handling.
//
// Revision 1.5  2001/06/28 20:39:18  liwei
// Code cleaned.
//
// Revision 1.4  2001/06/27 15:27:25  geconomos
// Worked on organiziation
//
// Revision 1.3  2001/06/26 22:38:06  liwei
// Adding convertion functions to convert any voxel type to unsigned char.
//
// Revision 1.2  2001/06/26 19:25:21  liwei
// Code rearrangement.
//
// Revision 1.1  2001/06/25 17:54:10  liwei
// VolumeVVS renamed back to AnyVolume
//
// Revision 1.3  2001/06/25 11:58:20  frank
// Added proper header/footer.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/AnyVolume.h,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
// $Id: AnyVolume.h,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
