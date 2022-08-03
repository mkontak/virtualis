// $Id: vxTimeVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// 
// Copyright © 2002, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Bin Li (binli@viatronix.com)
//
// Complete history at bottom of file.

#ifndef vxTimeVolume_h
#define vxTimeVolume_h


// Includes
#include "vxECGSignal.h"
#include "vxPhaseVolume.h"
#include "CmdLineSwitches.h"
#include "Serialization.h"
#include "Quad.h"


// Forward declarations
typedef Quad<float4> QuadFloat4;


// class definition
class VX_BLOCK_VOLUME_DLL vxTimeVolume
{
/// functions
public:

  /// constructor
  vxTimeVolume();

  /// destructor
  virtual ~vxTimeVolume() {}

  /// copy constructor
  vxTimeVolume(vxTimeVolume & other);

  /// assignment operator
  vxTimeVolume & operator=(vxTimeVolume & other);

  /// reset time volume
  void Reset();

  /// general initialization
  void Initialization();

  /// create a real time volume by reading dcm files & .iri file
  void Initialize(const uint4 uDetectors, const uint4 uPhases, std::vector<QuadFloat4> & vfImgInfo);

  /// create time by reading phase volumes & metafile
  void Read(std::string sFilePrefix, ProgressCallbackPtrType progressCallback = NULL);
  
  /// get the number of detectors
  inline const uint4 GetNumOfDetectors() { return m_uNumOfDetectors; };

  /// get the number of phases
  inline const uint4 GetNumOfPhases() { return m_uNumOfPhases; };

  /// set number of detectors
  inline void SetNumOfDetectors(const uint4 uNumOfDetectors) { m_uNumOfDetectors = uNumOfDetectors; };

  /// set number of phases
  inline void SetNumOfPhases(const uint4 uNumOfPhases) { m_uNumOfPhases = uNumOfPhases; };

  /// get z-position
  inline float4 GetSlicePosition(const uint4 uSliceNumber) { return m_vfZPos[uSliceNumber]; };

  /// get to know the size of z-position vector
  inline const int4 GetTotalSlicePositions() { return static_cast< int4 >( m_vfZPos.size() ); };

  /// set a selection of a row (-1 indicates no selection)
  inline void SetSelection(const uint4 uRow, const int4 uPhase);

  /// get a selection of a row
  inline const int4 GetSelection(const uint4 uRow);

  /// reset selections
  void ResetSelections(const int4 iDefaultSelection = -1);

  /// set a particular phase volume
  void SetPhaseVolume(vxPhaseVolume & phaseVol, uint4 index);

  /// get a particular phase volume (class)
  vxPhaseVolume & GetPhaseVolumeContainer(uint4 index);

  /// get a particular phase volume (block volume)
  vxBlockVolume<uint2> & GetPhaseVolume(uint4 index);

  /// retrive the setting of ECGSinal flag
  inline const bool IsECGSignalFlagSet() { return m_bECGSignalSet; };

  /// set ECGSignal flag
  inline void SetECGSignalFlag(const bool bECGSignalSet) { m_bECGSignalSet = bECGSignalSet; };

  /// get ECGSignal
  inline vxECGSignal & GetECGSignal() { return m_ECGSignal; };

  /// write out meta (.rsp, V3DFile) file
  void Write(const std::string & sFilePrefix);

  /// read metafile (.rsp, V3DFile): call this function when making selection.
  void ReadMetafile(const std::string & sFilePrefix);

  /// read ECG signal (.iri, txt file): call this function when creating phase volumes.
  void ReadECGFile(const std::string & sFilePrefix);

  /// read a phase volume
  void ReadPhaseVolumes(const std::string & sFilePrefix, ProgressCallbackPtrType progressCallback = NULL);

  /// write out the phase volumes
  void WritePhaseVolumes(const std::string & sFilePrefix);

  /// write out a selected phase volume
  void WriteSelectedPhaseVolume(const std::string & sFilePrefix);

  /// get selected volume pointer
  vxBlockVolume<uint2> * GetSelectedVolume() { return m_pSelectedVolume; }

  /// set selected volume pointer
  void SetSelectedVolume( vxBlockVolume<uint2> * pSelectedVolume );

  /// copy a slice from a phase volume into the selected volume
  void CopySliceIntoSelected( const uint4 uPhase, const uint4 uSlice );

private:

  /// retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// retrieves all the data in serialized form.
  bool WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// Reads in all the data from serialized form.
  bool ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

// data
private:

  /// number of detectors
  uint4 m_uNumOfDetectors;

  /// number of phases
  uint4 m_uNumOfPhases;

  /// phase volumes
  std::vector<vxPhaseVolume> m_vPhaseVol;

  /// table position (z-pos)
  std::vector<float4> m_vfZPos;

  /// acquisition time
  std::vector<int4> m_viAcquTime;

  /// selections (-1 indicates no selection)
  std::vector<int4> m_viSelections;

  /// ECGSignal
  vxECGSignal m_ECGSignal;

  /// flag: if the ECG signal is set
  bool m_bECGSignalSet;

  /// selected phase volume
  vxBlockVolume<uint2> * m_pSelectedVolume;
}; // vxTimeVolume


#endif // vxTimeVolume_h


// Revision History:
// $Log: vxTimeVolume.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/06/23 15:54:55  michael
// completed 2nd code review
//
// Revision 1.12  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.11  2003/05/17 16:22:37  michael
// reformatting
//
// Revision 1.10  2003/05/16 13:15:58  frank
// formatting
//
// Revision 1.9  2003/05/16 11:51:41  michael
// code review
//
// Revision 1.8  2002/10/09 19:04:19  frank
// Issue #2745: Retain slice selection upon re-invocation.
//
// Revision 1.7  2002/08/16 04:33:25  frank
// Added percent complete callbacks to the TimeVolume and some more detailed info about BlockVolume reads.
//
// Revision 1.6  2002/08/01 18:23:16  frank
// Working on selected phase volume output.
//
// Revision 1.5  2002/07/17 18:12:26  frank
// Revised selection mechanism.
//
// Revision 1.4  2002/07/16 19:10:01  frank
// Changed Read naming scheme.
//
// Revision 1.3  2002/07/15 21:17:25  frank
// Removed templates
//
// Revision 1.2  2002/07/15 20:12:20  frank
// Replaced template parameter with int2.
//
// Revision 1.1  2002/07/10 18:34:07  frank
// Initial version.
//
// Revision 1.18  2002/06/18 19:27:10  binli
// added some functions
//
// Revision 1.17  2002/06/18 17:41:17  binli
// added new class: ECGRender
//
// Revision 1.16  2002/06/11 13:39:11  binli
// copy constructor & assign operator
//
// Revision 1.15  2002/06/10 20:57:58  binli
// Reset & Initialization(overload 3)
//
// Revision 1.14  2002/06/07 20:40:27  binli
// removed test code
//
// Revision 1.13  2002/06/07 20:36:23  binli
// update
//
// Revision 1.12  2002/06/06 21:32:34  uday
// Added GetPhaseVolumeVector().
//
// Revision 1.11  2002/06/06 16:45:31  binli
// function GetPhaseVolume/GetPhaseVolumeContainer
// (find a better name)
//
// Revision 1.10  2002/06/05 22:18:10  uday
// Added ReadPhaseVolumes().
//
// Revision 1.9  2002/06/05 20:45:34  uday
// Added the following functions
// 1> WritePhaseVolumes()
// 2> WriteSelectedBlockVolume()
//
// Revision 1.8  2002/06/05 17:06:10  uday
// 1> Modified Read/Write using V3DFile.
// 2> Access methods for phase volumes.
//
// Revision 1.7  2002/06/05 15:18:24  binli
// Instance function
//
// Revision 1.6  2002/06/04 20:55:31  binli
// function: mapping slice numbers
//
// Revision 1.5  2002/06/03 19:30:35  binli
// added vector member: m_vPhaseVol
//
// Revision 1.4  2002/05/31 20:33:17  binli
// functions: set number of detectors & phases
//
// Revision 1.3  2002/05/30 21:37:03  binli
// creating vxTimeVolume class: Serialization/Selection
//
// Revision 1.2  2002/05/29 23:41:02  frank
// Formatting.
//
// Revision 1.1  2002/05/29 21:27:07  binli
// Initial version of vxTimeVolume class
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxTimeVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxTimeVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
