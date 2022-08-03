// $Id: vxTimeVolume.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxTimeVolume.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $ 
// 
// Copyright © 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Bin Li (binli@viatronix.com)


// includes
#include "stdafx.h"
#include "vxTimeVolume.h"
#include "FileExtensions.h"
#include "Timer.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


// namespaces
using namespace Serialization;


/**
 *  Constructor
 */
vxTimeVolume::vxTimeVolume()
{
  m_pSelectedVolume = NULL;
}


/**
 * Copy constructor
 * @param other
 */
vxTimeVolume::vxTimeVolume(vxTimeVolume & other)
{
  *this = other;
} // vxTimeVolume(vxTimeVolume & other)


/**
 * Assign operator
 * @param other
 * @return
 */
vxTimeVolume & vxTimeVolume::operator=(vxTimeVolume & other)
{
  if (this != &other)
  {
    m_uNumOfDetectors = other.m_uNumOfDetectors;
    m_uNumOfPhases = other.m_uNumOfPhases;
    m_vPhaseVol = other.m_vPhaseVol;
    m_vfZPos = other.m_vfZPos;
    m_viAcquTime = other.m_viAcquTime;
    m_viSelections = other.m_viSelections;
    m_ECGSignal = other.m_ECGSignal;
    m_bECGSignalSet = other.m_bECGSignalSet;
    m_pSelectedVolume = other.m_pSelectedVolume;
  }

  return *this;
} // operator=


/**
 * Reset
 */
void vxTimeVolume::Reset()
{
  // reset basic variables.
  m_uNumOfDetectors = 0;
  m_uNumOfPhases = 0;
  m_bECGSignalSet = false;
  
  m_vfZPos.resize(0);
  m_viAcquTime.resize(0);
  m_viSelections.resize(0);
  
  // reset phase volumes.
  for (uint4 i(0); i<m_vPhaseVol.size(); i++)
  {
    m_vPhaseVol[i].Reset();
  }
  m_vPhaseVol.resize(0);

  // reset ECG signal.
  m_ECGSignal.Reset();

  m_pSelectedVolume = NULL;

} // Reset()


/**
 * General initialization
 */
void vxTimeVolume::Initialization()
{
  m_uNumOfDetectors = 0;
  m_uNumOfPhases = 0;
  m_bECGSignalSet = false;
} // Initialization()


/**
 * Create a real time volume
 * @param uNumOfDetectors number of detectors
 * @param uNumOfPhases number of phases
 * @param vfImgInfo image info
 */
void vxTimeVolume::Initialize(const uint4 uNumOfDetectors, 
                              const uint4 uNumOfPhases, 
                              std::vector<QuadFloat4> & vfImgInfo)
{
  m_uNumOfDetectors = uNumOfDetectors;
  m_uNumOfPhases = uNumOfPhases;

  uint4 uNumberOfSlicesInList( static_cast< uint4 >( vfImgInfo.size()));
      
  // 1> figure out the dims for each phase volume.
  std::vector<uint4> vuDim(uNumOfPhases);
  uint4 uCycles(uNumberOfSlicesInList / (uNumOfDetectors * uNumOfPhases));

  for (uint4 i(0); i<uNumOfPhases; i++) 
  { 
    vuDim[i] = uCycles * uNumOfDetectors; 
  }

  // in case the phase volumes have different z-dimensions.
  if ( uNumberOfSlicesInList % (uNumOfDetectors*uNumOfPhases) != 0)
  {
    // allocate slices to phase volumes (need more datasets for more complex processing).
    int4 iSliceLeft(uNumberOfSlicesInList - uCycles * uNumOfDetectors * uNumOfPhases);
    for (uint4 j(0); j<uNumOfPhases; j++)
    {
      if ( static_cast< uint4 >(iSliceLeft) > uNumOfDetectors )
      {
        vuDim[j] += uNumOfDetectors;
        iSliceLeft -= uNumOfDetectors;
      }
      else
      {
        vuDim[j] += iSliceLeft;
        break;
      }
    }
    uCycles ++;
  } // end-if.

  // 2> phase volume initilizing...
  m_vPhaseVol.resize(uNumOfPhases);

  for (uint4 i(0); i<uNumOfPhases; i++)
  {
    m_vPhaseVol[i].Initialize(vuDim[i]);
    // set up image time.
    for (uint4 j(0); j<vuDim[i]; j++)
    {
      const int4 iCycle((j/uNumOfDetectors));
      const int4 iShift(j - iCycle * uNumOfDetectors);
      m_vPhaseVol[i].SetImageTime(j ,static_cast< int4 >( vfImgInfo[iCycle * uNumOfDetectors * uNumOfPhases + i * uNumOfDetectors + iShift].m_t ));
    }
  } // end-for (i)

  // 3> Z-Position initializing...
  m_vfZPos.resize(vuDim[0]);

  for (uint4 i(0); i<vuDim[0]; i++)
  {
    const int4 iCycle(i/uNumOfDetectors);
    const int4 iShift(i - iCycle * uNumOfDetectors);
    m_vfZPos[i] = vfImgInfo[iCycle * uNumOfDetectors * uNumOfPhases + iShift].m_y;
  }

  // 4> AcquisitionTime initializing...
  m_viAcquTime.resize(uCycles);
  // set up acquisition time.
  for (uint4 i(0); i<uCycles; i++)
  {
    m_viAcquTime[i] = (int4)vfImgInfo[i * uNumOfDetectors * uNumOfPhases].m_z;
  }

  // Clear out the selections to -1 (no selection)
  m_viSelections.resize( vuDim[0], -1 );

} // Initialization()


/** 
 * create time by reading phase volumes & metafile.
 * @param sFilePrefix
 * @param progressCallback
 */
void vxTimeVolume::Read(std::string sFilePrefix, ProgressCallbackPtrType progressCallback)
{
  Timer readTimer;

  // 1> clear original time volume
  Reset();

  // 2> read metafile.
  ReadMetafile(sFilePrefix);
  if ( progressCallback != NULL ) (*progressCallback)(5.0F); // reading 5% done

  // 3> read phase volumes.
  ReadPhaseVolumes(sFilePrefix, progressCallback);
  if ( progressCallback != NULL ) (*progressCallback)(100.0F); // reading complete

  LogFyi( "Time to read TimeVolume: " + readTimer.AsString(), "vxTimeVolume", "Read" );
} // Initialization()


/**
 * Copy a slice from a phase volume into the selected volume.
 *
 * @param uPhase the phase from which to copy
 * @param uSlice the slice to copy
 */
void vxTimeVolume::CopySliceIntoSelected( const uint4 uPhase, const uint4 uSlice )
{
  if ( m_pSelectedVolume != NULL )
  {
    const Triple<int4> dim = m_pSelectedVolume->GetHeader().GetVolDim();
    vxBlockVolumeIterator<uint2> iterSource( m_vPhaseVol[ uPhase ].GetVolume() );
    vxBlockVolumeIterator<uint2> iterDest( *m_pSelectedVolume );
    iterSource.SetPos( Vector3D<int4>( 0, 0, uSlice ) );
    iterDest  .SetPos( Vector3D<int4>( 0, 0, uSlice ) );

    for (int i(0); i<dim.X()*dim.Y(); i++)
    {
      iterDest.SetVoxel( iterSource.GetVoxel() );
      iterSource.NextZYX();
      iterDest.NextZYX();
    }
  }
} // CopySliceIntoSelected


/**
 * Set selected volume pointer
 * @param pSelectedVolume
 */
void vxTimeVolume::SetSelectedVolume( vxBlockVolume<uint2> * pSelectedVolume )
{
  m_pSelectedVolume = pSelectedVolume;

  // copy over the selected slices, if any
  for (uint4 i(0); i<m_viSelections.size(); i++)
  {
    SetSelection(i, GetSelection(i));
  }
} // SetSelectedVolume()


/**
 * Set a selection
 * @param uRow row of slice planes
 * @param uPhase selection of the 'row'
 */
void vxTimeVolume::SetSelection(const uint4 uRow, const int4 iPhase)
{
  const int4 iCorrectPhase( static_cast< uint4 >( (static_cast< uint4 >( iPhase ) >= m_uNumOfPhases       ) ? (m_uNumOfPhases       -1) : iPhase) );
  const uint4 uCorrectRow ( static_cast< uint4 >( (uRow   >= m_viSelections.size()) ? (m_viSelections.size()-1) : uRow  ));
  m_viSelections[uCorrectRow] = iCorrectPhase;

  // copy data into selected volume
  if ( iCorrectPhase != -1 )
  {
    CopySliceIntoSelected( iCorrectPhase, uCorrectRow );
  }
} // SetSelection()


/**
 * Get a selection
 * @param uRow row of slice planes
 * @return the selection of dedicate row
 */
const int4 vxTimeVolume::GetSelection(const uint4 uRow)
{
  const uint4 uCorrectRow(static_cast< uint4 >( (uRow>=m_viSelections.size()) ? (m_viSelections.size()-1) : uRow));

  return m_viSelections[uCorrectRow];
} // GetSelection()


/**
 * Reset selections (default selection is the first phase)
 * @param uRow row of slice planes
 * @param uPhase selection of the 'row'
 */
void vxTimeVolume::ResetSelections(const int4 iDefaultSelection)
{
  // make sure there are enough selections allocated
  if (m_vPhaseVol.size())
  {
    const uint4 uMaxSlices(m_vPhaseVol[0].GetVolume().GetHeader().GetVolDim().Z());
    if (m_viSelections.size() < uMaxSlices)
    {
      m_viSelections.resize(uMaxSlices);
    }
  }

  // clear out the volume if -1
  if ( iDefaultSelection == -1 )
  {
    if ( m_pSelectedVolume != NULL )
    {
      m_pSelectedVolume->Clear();
      for ( uint4 i(0); i<m_viSelections.size(); i++ )
      {
        m_viSelections[i] = -1;
      }
    }
  }
  else
  {
    // otherwise copy all the data slices over from the phase volumes into the selected volume
    int4 iCorrectDefaultSelection(Bound(int4(-1), iDefaultSelection, int4(m_uNumOfPhases-1)));
    for ( uint4 i(0); i<m_viSelections.size(); i++ )
    {
      m_viSelections[i] = iCorrectDefaultSelection;
      if ( iCorrectDefaultSelection != -1 )
      {
        CopySliceIntoSelected( iCorrectDefaultSelection, i );
      }
    }
  }
} // ResetSelections()


/**
 * Get a particular phase volume (class)
 * @param index index of the phase volume to be returned.
 * @return reference to a phase volume object.
 */
vxPhaseVolume & vxTimeVolume::GetPhaseVolumeContainer(uint4 index)
{
  return m_vPhaseVol[index];
} // GetPhaseVolume()


/**
 * Get a particular phase volume (block volume)
 * @param index index of the phase volume to be returned.
 * @return reference to a vxBlockVolume.
 */
vxBlockVolume<uint2> & vxTimeVolume::GetPhaseVolume(uint4 index)
{
  return m_vPhaseVol[index].GetVolume();
} // GetPhaseVolume()


/**
 * Set a particular phase volume
 * @param phaseVol  the phase volume to be set.
 * @param index     index of the phase volume to be set.
 */
void vxTimeVolume::SetPhaseVolume(vxPhaseVolume & phaseVol, uint4 index)
{
  m_vPhaseVol[index] = phaseVol;
} // GetPhaseVolume()


/** 
 * Read meta (.rsp) file
 * @param sFilePrefix  prefix of metafile name.
 */
void vxTimeVolume::ReadMetafile(const std::string & sFilePrefix)
{
  Timer readTimer;

  try
  {
    V3DFile img;
    img.Read(sFilePrefix + vx::RetroSpectiveMetaExt, std::make_pair( 0.0F, 100.0F ) );
    MemoryBuffer memBuf(img.GetDataBuffer());
    uint4 iBufSize(memBuf.GetSize());
    int1 * pBuffer = reinterpret_cast<int1 *> (memBuf.GetDataPtr());
    ReadSerializedData(&pBuffer, pBuffer + iBufSize);
  }
  catch (ex::VException & e )
  {
    LogErr("File Read Error : " + e.GetLogRecord().GetTextMessage(),"TimeVolume", "Write");
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec("An unspecified error caught", "TimeVolume", "Read"));
  }

  LogFyi( "Time to read metafile: " + readTimer.AsString(), "vxTimeVolume", "ReadMetafile" );
  return;
} // Read()


/** 
 * Write out meta (.rsp) file
 * @param sFilePrefix  prefix of metafile name
 */
void vxTimeVolume::Write(const std::string & sFilePrefix)
{
  try
  {
    const uint4 originalBufferSize = GetSerializedDataSize();
    MemoryBuffer memBuf(originalBufferSize);
    int1 * pBuffer = reinterpret_cast<int1 *> (memBuf.GetDataPtr());
    const int1 * pBufferEnd(pBuffer + originalBufferSize);

    LogTmp("Writing the serialize data for the RSP", "TimeVolume", "Write"); 
    WriteSerializedData(&pBuffer, pBufferEnd);

    V3DFile img;
    img.SetData(memBuf);

    LogTmp("Writing the image file " + sFilePrefix + vx::RetroSpectiveMetaExt, "TimeVolume", "Write");    
    img.Write(sFilePrefix + vx::RetroSpectiveMetaExt);
  }
  catch (ex::VException & e )
  {
    LogErr("File Write Error : " + e.GetLogRecord().GetTextMessage(),"TimeVolume", "Write");
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec("An unspecified error caught","TimeVolume", "Write")); 
  }

  return;
} // Write()


/** 
 * Read metafile (.IRI, txt): call this function when creating phase volumes.
 * @param sFilePrefix  prefix of metafile name.
 */
void vxTimeVolume::ReadECGFile(const std::string & sFilePrefix)
{  
  if ( m_ECGSignal.ReadECGFile(sFilePrefix) == false )
  {
    throw ex::VException(LogRec("Error read ECG signal file.","TimeVolume", "Read ECG signal file"));
  }
} // ReadIRIFile()


/**
 * Retrieves the number of bytes of the serialized data.
 * @return  data size.
 */
uint4 vxTimeVolume::GetSerializedDataSize() const
{
  uint4 uSum(0);

  // 0. start delimiter: 'RetroSpective Meta Information'.
  uSum += static_cast< uint4 >( std::string("Viatronix::RetroSpective::Meta_Information").length() + 1 );

  // 1. basic info.
  // m_uNumOfDetectors.
  uSum += static_cast< uint4 >( sizeof(uint4) );
  // m_uNumOfPhases.
  uSum += static_cast< uint4 >( sizeof(uint4) );
  
  // 2. acquisition time.
  // start delimiter: 'RetroSpective::Acquisition_Time'
  uSum += static_cast< uint4 >( std::string("RetroSpective::Acquisition_Time").length() + 1 );
  // size of 'm_viAcquTime'
  uSum += static_cast< uint4 >( sizeof(int4) );
  // content of vector m_viAcquTime.
  uSum += static_cast< uint4 >( sizeof(int4) * m_viAcquTime.size() );
  // end delimiter.
  uSum += static_cast< uint4 >( std::string("RetroSpective::Acquisition_Time").length() + 1 );

  // 3. image time.
  // start delimiter: 'RetroSpective::Image_Time'.
  uSum += static_cast< uint4 >( std::string("RetroSpective::Image_Time").length() + 1 );
  // image time at each phase volume.
  for (uint4 i(0); i<m_uNumOfPhases; i++)
  {
    uSum += static_cast< uint4 >( m_vPhaseVol[i].GetSerializedDataSize() );
  }  
  // end delimiter: 'RetroSpective::Image_Time'.
  uSum += static_cast< uint4 >( std::string("RetroSpective::Image_Time").length() + 1 );

  // 4. z-positions
  // start delimiter: 'RetroSpective::Table_Position'
  uSum += static_cast< uint4 >( std::string("RetroSpective::Table_Position").length() + 1 );
  // size of m_vfZPos.
  uSum += static_cast< uint4 >( sizeof(int4) );
  // content of vector m_vfZPos.
  uSum += static_cast< uint4 >( sizeof(float4) * m_vfZPos.size() );
  // end delimiter: 'RetroSpective::Table_Position'
  uSum += static_cast< uint4 >( std::string("RetroSpective::Table_Position").length() + 1 );

  // 5. selections
  // start delimiter: 'RetroSpective::Selections'
  uSum += static_cast< uint4 >( std::string("RetroSpective::Selections").length() + 1 );
  // size of m_viSelections.
  uSum += static_cast< uint4 >( sizeof(int4) );
  // content of vector m_viSelections.
  uSum += static_cast< uint4 >( sizeof(uint4) * m_viSelections.size() );
  // end delimiter: 'RetroSpective::Selections'
  uSum += static_cast< uint4 >( std::string("RetroSpective::Selections").length() + 1 );

  // 6. length of ECG signals.
  uSum += static_cast< uint4 >(  m_ECGSignal.GetSerializedDataSize() );

  // 0. end delimiter: 'RetroSpective Meta Information'.
  uSum += static_cast< uint4 >( std::string("Viatronix::RetroSpective::Meta_Information").length() + 1 );

  return uSum;
} // GetSerializedDataSize()


/**
 * Reads in all the data from serialized form. (meta file)
 * @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 * @param piBufferEnd pointer to the end of strips memory buffer.
 * @return true when success, else, false.
 */
bool vxTimeVolume::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  try
  {
    int4 iSize(0);

    // 0: verify the stream by reading start delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != "Viatronix::RetroSpective::Meta_Information")
    {
      throw ex::DataCorruptionException(LogRec("data corruption data corruption: bad starting delimiter not 'RetroSpective::Meta_Information'.", "TimeVolume", "ReadSerializedData"));
    }
 
    // 1. basic info.
    // m_iNumOfDetectors.
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_uNumOfDetectors);
    // m_iNumOfPhases.
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_uNumOfPhases);
    
    // 2. acquisition time.
    // start delimiter: 'RetroSpective::Acquisition_Time'
    DeserializeStringData(hiBuffer, piBufferEnd);
    DeserializeSimpleData(hiBuffer, piBufferEnd, iSize);
    m_viAcquTime.resize(iSize);

    for (int4 i(0); i<iSize; i++)
    {
      Serialization::DeserializeSimpleData(hiBuffer, piBufferEnd, m_viAcquTime[i]);
    }
    // end delimiter: 'RetroSpective::Acquisition_Time'
    DeserializeStringData(hiBuffer, piBufferEnd);

    // 3. image time (saved in each 'PhaseVolume').
    // start delimiter: 'RetroSpective::Image_Time'
    DeserializeStringData(hiBuffer, piBufferEnd);
    m_vPhaseVol.resize(m_uNumOfPhases);
    for (uint4 i(0); i<m_uNumOfPhases; i++)
    {
      m_vPhaseVol[i].ReadSerializedData(hiBuffer, piBufferEnd);
    }
    // end delimiter: 'RetroSpective::Image_Time'
    DeserializeStringData(hiBuffer, piBufferEnd);

    // 4. z-positions
    // start delimiter: 'RetroSpective::Table_Position'
    DeserializeStringData(hiBuffer, piBufferEnd);
    Serialization::DeserializeSimpleData(hiBuffer, piBufferEnd, iSize);
    m_vfZPos.resize(iSize);
    for (int4 i(0); i<iSize; i++)
    {
      Serialization::DeserializeSimpleData(hiBuffer, piBufferEnd, m_vfZPos[i]);
    }
    // end delimiter: 'RetroSpective::Table_Position'
    DeserializeStringData(hiBuffer, piBufferEnd);

    // 5. selections
    // start delimiter: 'RetroSpective::Selections'
    DeserializeStringData(hiBuffer, piBufferEnd);
    Serialization::DeserializeSimpleData(hiBuffer, piBufferEnd, iSize);
    m_viSelections.resize(iSize);
    for (int4 i(0); i<iSize; i++)
    {
      Serialization::DeserializeSimpleData(hiBuffer, piBufferEnd, m_viSelections[i]);
    }
    // end delimiter: 'RetroSpective::Selections'
    DeserializeStringData(hiBuffer, piBufferEnd);

    // 6. ECGSignal
    m_ECGSignal.ReadSerializedData(hiBuffer, piBufferEnd);

    // 0. verify the stream again by reading end delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != "Viatronix::RetroSpective::Meta_Information") 
	  {
      throw ex::DataCorruptionException(LogRec("data corruption data corruption: bad ending delimiter: Not 'RetroSpective::Meta_Information'.","TimeVolume", "ReadSerializedData"));
     }
  }
  catch (ex::OutOfMemoryException & error)
  {
    throw ex::OutOfMemoryException(LogRec("Out of memory" + error.GetLogRecord().GetTextMessage(),"TimeVolume", "ReadSerializedData"));
   }
  catch (ex::VException & error)
  {
    throw ex::VException(LogRec(error.GetLogRecord().GetTextMessage(),"TimeVolume", "ReadSerializedData"));
  }

  return true;
} // ReadSerializedData()


/**
 * Retrieves all the data in serialized form. (meta file)
 * @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 * @param piBufferEnd pointer to the end of strips memory buffer.
 * @return true when success, else, false.
 */
bool vxTimeVolume::WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  // ensure there is enough space
  if (*hiBuffer + GetSerializedDataSize() > piBufferEnd)
  {
    throw ex::VException(LogRec("<TimeVolume::WriteSerializedData> Not enough space.","TimeVolume", "WriteSerializedData"));
  } // if not enough space

  /// 0. start delimiter: 'Viatronix::RetroSpective::Meta_Information'
  SerializeStringData(hiBuffer, piBufferEnd, "Viatronix::RetroSpective::Meta_Information");

  // 1. basic info
  // m_iNumOfDetectors
  SerializeSimpleData(hiBuffer, piBufferEnd, m_uNumOfDetectors);
  // m_iNumOfPhases
  SerializeSimpleData(hiBuffer, piBufferEnd, m_uNumOfPhases);

  // 2. acquisition time.
  // start delimiter: 'RetroSpective::Acquisition_Time'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Acquisition_Time");
  int4 iSize( static_cast< uint4 >( m_viAcquTime.size()) );
  SerializeSimpleData(hiBuffer, piBufferEnd, iSize);

  for (int4 i(0); i<iSize; i++)
  {
    SerializeSimpleData(hiBuffer, piBufferEnd, m_viAcquTime[i]);
  }
  // end delimiter: 'RetroSpective::Acquisition_Time'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Acquisition_Time");

  // 3. image time.
  // start delimiter: 'RetroSpective::Image_Time'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Image_Time");
  for (int4 i=0; static_cast< uint4 >( i ) <m_uNumOfPhases; i++)
  {
    m_vPhaseVol[i].WriteSerializedData(hiBuffer, piBufferEnd);
  }
  // end delimiter: 'RetroSpective::Image_Time'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Image_Time");

  // 4. z-positions
  // start delimiter: 'RetroSpective::Table_Position'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Table_Position");
  iSize = static_cast< int4 >( m_vfZPos.size() );
  SerializeSimpleData(hiBuffer, piBufferEnd, iSize);
  for (int4 i(0); i<iSize; i++)
  {
    SerializeSimpleData(hiBuffer, piBufferEnd, m_vfZPos[i]);
  }
  // end delimiter: 'RetroSpective::Table_Position'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Table_Position");

  // 5. selections
  // start delimiter: 'RetroSpective::Selections'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Selections");
  iSize = static_cast< int4 >( m_viSelections.size() );
  SerializeSimpleData(hiBuffer, piBufferEnd, iSize);
  for (int4 i(0); i<iSize; i++)
  {
    SerializeSimpleData(hiBuffer, piBufferEnd, m_viSelections[i]);
  }
  // end delimiter: 'RetroSpective::Selections'
  SerializeStringData(hiBuffer, piBufferEnd, "RetroSpective::Selections");

  // 6. ECGSignal
  m_ECGSignal.WriteSerializedData(hiBuffer, piBufferEnd);

  /// end delimiter: 'RetroSpective::Meta_Information'
  SerializeStringData(hiBuffer, piBufferEnd, "Viatronix::RetroSpective::Meta_Information");
  
  return true;
} // WriteSerializedData()


/**
 * Read in a phase volume
 * @param sFilePrefix
 * @param progressCallback
 */
void vxTimeVolume::ReadPhaseVolumes(const std::string & sFilePrefix, 
                                    ProgressCallbackPtrType progressCallback)
{
  Timer readTimer;

  for ( uint2 uPhase(0); uPhase<m_vPhaseVol.size(); ++uPhase )
  {
    Timer volReadTimer;
    m_vPhaseVol[uPhase].GetVolume().Read(sFilePrefix + "_" + ToAscii(uPhase) + vx::VolumeExt, std::make_pair( 0.0F, 100.0F ));
    if ( progressCallback != NULL ) ( * progressCallback ) ( float4( uPhase ) / float4( m_vPhaseVol.size() - 1 ) );
    LogDbg( "Time to read phase volume #" + ToAscii( uPhase + 1 ) + " of " + ToAscii( m_vPhaseVol.size() ) + " was " +
            volReadTimer.AsString(), "vxTimeVolume", "ReadPhaseVolumes" );
  }

  // make sure there are enough selections available
  if (m_vPhaseVol.size())
  {
    const uint4 uMaxSlices(m_vPhaseVol[0].GetVolume().GetHeader().GetVolDim().Z());
    if (m_viSelections.size() < uMaxSlices)
    {
      m_viSelections.resize(uMaxSlices, -1);
    }
  }

  LogFyi( "Time to read phase volumes: " + readTimer.AsString(), "vxTimeVolume", "ReadPhaseVolumes" );
} // ReadPhaseVolume()

  
/**
 *  Write out all the phase volumes.
 *  @param sFilePrefix
 */
void vxTimeVolume::WritePhaseVolumes(const std::string & sFilePrefix)
{
  for (uint2 uPhase(0); uPhase <m_vPhaseVol.size(); ++uPhase)
  {
    m_vPhaseVol[uPhase].GetVolume().Write(sFilePrefix + "_" + ToAscii(uPhase) + vx::VolumeExt);
  }
} // WritePhaseVolumes()


/**
 *  Write a phase volume from the selected slices.
 *  @param sFilePrefix
 */
void vxTimeVolume::WriteSelectedPhaseVolume(const std::string & sFilePrefix)
{
  // always create selected phase volume from the first one.
  vxBlockVolume<uint2> blkVol(m_vPhaseVol[0].GetVolume());
  vxBlockVolumeIterator<uint2> volIter(blkVol);

  // go through slice by slice.
  for (uint4 iCurZ(0); iCurZ<m_viSelections.size(); iCurZ++)
  {  
    // only copy those slices not from first phase volume.
    if ( m_viSelections[iCurZ] != 0 )
    {
      vxBlockVolumeIterator<uint2> phaseVolIter(m_vPhaseVol[m_viSelections[iCurZ]].GetVolume());
      for (phaseVolIter.SetPos(0,0,iCurZ), volIter.SetPos(0,0,iCurZ) ; phaseVolIter.GetPos().Z() == iCurZ ; phaseVolIter.NextZYX(), volIter.NextZYX())
      {
        volIter.SetVoxel(phaseVolIter.GetVoxel());
      }
    }
  } // end-for.
  
  // write out...
  blkVol.Write(sFilePrefix + vx::VolumeExt);

} // WriteSelectedBlockVolume()


#undef FILE_REVISION


// Revision History:
// $Log: vxTimeVolume.C,v $
// Revision 1.3  2006/05/31 16:16:41  geconomos
// porting to visual studio 2005
//
// Revision 1.2  2005/07/01 12:30:57  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2003/06/23 15:54:55  michael
// completed 2nd code review
//
// Revision 1.21  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.20  2003/05/16 14:57:04  dongqing
// code review
//
// Revision 1.19  2003/04/19 16:24:52  frank
// Added progress bar parameter
//
// Revision 1.18  2003/01/22 21:26:50  ingmar
// dimension and units are now Triples
//
// Revision 1.17  2002/10/14 15:32:17  frank
// Intial phase selection should be clear.
//
// Revision 1.16  2002/10/09 19:04:19  frank
// Issue #2745: Retain slice selection upon re-invocation.
//
// Revision 1.15  2002/09/30 22:35:27  frank
// Issue #2697: Reset the selection.
//
// Revision 1.14  2002/09/30 20:48:36  ingmar
// included Global.h to avoid debug mode warnings
//
// Revision 1.13  2002/08/28 20:32:31  frank
// Cleared whole volume upon ResetSelection(-1).
//
// Revision 1.12  2002/08/16 04:33:25  frank
// Added percent complete callbacks to the TimeVolume and some more detailed info about BlockVolume reads.
//
// Revision 1.11  2002/08/14 16:06:59  wenli
// initial version - Vascular
//
// Revision 1.10  2002/08/08 21:30:48  geconomos
// Fixed bug in SetSelctedSlice()
//
// Revision 1.9  2002/08/07 15:12:28  mkontak
// Fixed bug in the Write method
//
// Revision 1.8  2002/08/06 19:29:42  mkontak
// Fixed Error Logs
//
// Revision 1.7  2002/08/01 18:23:16  frank
// Working on selected phase volume output.
//
// Revision 1.6  2002/07/19 04:58:39  frank
// Fixed ResetSelection()
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
// Revision 1.1  2002/05/29 21:45:51  binli
// Initial version of vxTimeVolume class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxTimeVolume.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
// $Id: vxTimeVolume.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
