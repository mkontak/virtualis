// $Id: vxECGSignal.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
// 
// Copyright © 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Bin Li binli@viatronix.com


// Includes
#include "stdafx.h"
#include "vxECGSignal.h"
#include "Serialization.h"
#include "File.h"
#include "FileExtensions.h"
#include "FilePtrFactory.h"

// Defines
#define FILE_REVISION "$Revision: 1.3 $"


// Namespaces
using namespace std;
using namespace Serialization;


// Static declarations
const string vxECGSignal::m_sDelimiter("RetroSpective::ECGSignal");


/**
 *  default constructor
 */
vxECGSignal::vxECGSignal(): m_sPatientId(""), m_sScanDate(""), m_sScanTime("")
{
} // vxECGSignal()


/**
 * default destructor
 */
vxECGSignal::~vxECGSignal()
{    
} // ~vxECGSignal()


/**
 * copy constructor
 * @param other the other signal
 */
vxECGSignal::vxECGSignal(const vxECGSignal & other)
{
  m_sPatientId = other.m_sPatientId;
  m_sScanDate = other.m_sScanDate;
  m_sScanTime = other.m_sScanDate;
  m_iSampleRate = other.m_iSampleRate;
  m_iSamplePeriod = other.m_iSamplePeriod; 
  m_vReconPeriod = other.m_vReconPeriod;
  m_vRPoint = other.m_vRPoint;
  m_vSignal = other.m_vSignal;
} // vxECGSignal( other )


/**
 * assignment operator.
 * @param other
 */
vxECGSignal & vxECGSignal::operator=(const vxECGSignal & other)
{
  m_sPatientId = other.m_sPatientId;
  m_sScanDate = other.m_sScanDate;
  m_sScanTime = other.m_sScanDate;
  m_iSampleRate = other.m_iSampleRate;
  m_iSamplePeriod = other.m_iSamplePeriod; 
  m_vReconPeriod = other.m_vReconPeriod;
  m_vRPoint = other.m_vRPoint;
  m_vSignal = other.m_vSignal;

  return *this;
} // operator =()


/**
 *  Reset.
 */
void vxECGSignal::Reset()
{
  m_sPatientId = "";
  m_sScanDate = "";
  m_sScanTime = "";
  m_iSampleRate = 0;
  m_iSamplePeriod = 0; 
} // reset()


/**
 *  read .iri file.
 *  @param sFilePrefix the file prefix
 *  @return true if read
 */
bool vxECGSignal::ReadECGFile(const std::string & sFilePrefix) 
{
  // 1> file exist
  string sFileName(sFilePrefix + vx::ECGSignalExt);

  if( ! io::FilePtrFactory::Exists(sFileName) )
    throw ex::FileNotFoundException( LogRec( ".IRI file not found: ", "vxECGSignal", "ReadECGFile" ) );

  ifstream ifs(sFileName.c_str(), ios::in | ios::binary);
  const int4 uMaxLength(200);
  int1 iBuffer[uMaxLength];

  // input comments and extract: sample rate/sample period
  while ( !ifs.eof() )
  {
    ifs.getline(iBuffer, uMaxLength);
    string sStrLine(iBuffer);

    if ( !(sStrLine.find_first_of("*") == string::npos ) )
    {
      int4 iIndex(0);
      // comments section
      if ( (iIndex = static_cast< int4 >( sStrLine.find("Patient:", 0))) != string::npos )
      {
        // patient ID.
        m_sPatientId = GetEffectSubString(sStrLine, iIndex+8);
      }
      else if ( (iIndex = static_cast< int4 >( sStrLine.find("Date:", 0))) != string::npos )
      {
        // scan date.
        m_sScanDate = GetEffectSubString(sStrLine, iIndex+5);
      }
      else if ( (iIndex = static_cast< int4 >( sStrLine.find("Time:", 0))) != string::npos )
      {
        // scan time.
        m_sScanTime = GetEffectSubString(sStrLine, iIndex+5);
      }
      else if ( (iIndex = static_cast< uint4 >( sStrLine.find("Sample rate", 0))) != string::npos )
      {
        // sample rate.
        iIndex = static_cast< uint4 >( sStrLine.find("=", iIndex+11));
        m_iSampleRate = atoi(GetEffectSubString(sStrLine, iIndex+1).c_str());
      }
      else if ( sStrLine.find("Sample period", 0) != string::npos )
      {
        // sample period.
        iIndex = static_cast< uint4 >( sStrLine.find("=", iIndex+13));
        m_iSamplePeriod = static_cast< float4 >( atoi(GetEffectSubString(sStrLine, iIndex+1).c_str()));
      }
    }
    else
    {
      // no find of '/*' or '*/' or '*', we have reached the signal section.
      break;
    }
  }

  SignalTypeEnum eSignalType;
  int4 iSignalNum(0);

  do 
  {
    string sStrLine(iBuffer);

    int4 iSignal(GetDigSignal(sStrLine, eSignalType));
    switch(eSignalType)
    {
    case vxECGSignal::G_SIGNAL: 
      m_vReconPeriod.push_back(std::pair<int4, int4>(iSignalNum, iSignalNum));
      break;

    case vxECGSignal::O_SIGNAL: 
      m_vReconPeriod.back().second = iSignalNum;
      break;

    case vxECGSignal::R_SIGNAL:
      m_vRPoint.push_back(iSignalNum);
      break;

    case vxECGSignal::N_SIGNAL:
    default:
      break;
    }
    m_vSignal.push_back(iSignal);

    ifs.getline(iBuffer, uMaxLength);
    iSignalNum ++;
  }
  while ( !ifs.eof() );

  return true;

} // ReadECGFile()


/*
 * common routine: get digital signal
 * @param sStr        unknown
 * @param eSignalType unknown
 * @return            unknown
 */
int4 vxECGSignal::GetDigSignal(std::string & sStr, SignalTypeEnum & eSignalType)
{
  int4 iIndex(0);
  int4 iLength(0);
  
  if ( sStr[0] == '-' || sStr[0] == '+') iIndex = 1;

  uint4 i=iIndex;
  for (; i<sStr.length(); i++)
  {
    if ( !isdigit(sStr[i]) )  { iLength = i; break; }
  }

  if ( sStr.find_first_of('G', i) != string::npos )
  {
    eSignalType = vxECGSignal::G_SIGNAL;
  }
  else if ( sStr.find_first_of('O', i) != string::npos )
  {
    eSignalType = vxECGSignal::O_SIGNAL;
  }
  else if ( sStr.find_first_of('R', i) != string::npos )
  {
    eSignalType = vxECGSignal::R_SIGNAL;
  }
  else
  {
    eSignalType = vxECGSignal::N_SIGNAL;
  }

  return atoi(sStr.substr(0, iLength).c_str());

} // GetDigSignal()


/*
 * common routine: get effective sub-string
 * @param sStr unknown
 * @param iPos unknown
 * @return     unknown
 */
std::string vxECGSignal::GetEffectSubString(std::string & sStr, const int4 iPos)
{
  int4 iStart, iEnd = static_cast< int4 >( sStr.length() );
  
  // find first non-space char.
  uint4 i=iPos;
  for (; i<sStr.length(); i++)
  {
    if (sStr[i] != ' ') { iStart = i; break; }
  }
  if ( i>sStr.length() )  return std::string("");

  // find first space char.
  for (; i<sStr.length(); i++)
  {
    if (sStr[i] == ' ' ) { iEnd = i; break; }
  }
  if ( i>sStr.length() )  return std::string("");

  return std::string(sStr.substr(iStart, iEnd-iStart));

} // GetEffectSubString


/**
 *  Retrieves the number of bytes of the serialized data.
 *
 *  @return  data size
 */
uint4 vxECGSignal::GetSerializedDataSize() const
{
  uint4 uSum(0);

  /// vxECGSignal
  // starting delimiter
  uSum += static_cast< uint4 >( m_sDelimiter.length() + 1 );

  /// ECG info.
  // starting delimiter: 'ECGSignal::Info'.
  uSum += static_cast< uint4 >( string("ECGSignal::Info").length() + 1);
  // patient ID.
  uSum += static_cast< uint4 >( m_sPatientId.length() + 1);
  // scan date.
  uSum += static_cast< uint4 >( m_sScanDate.length() + 1);
  // scan time.
  uSum += static_cast< uint4 >( m_sScanTime.length() + 1);
  // sample rate (Hz)
  uSum += static_cast< uint4 >( sizeof(int4) );
  // total sample points.
  uSum += static_cast< uint4 >( sizeof(int4) );
  // ending delimiter: 'ECGSignal::Info'.
  uSum += static_cast< uint4 >( string("ECGSignal::Info").length() + 1 );

  /// recon period (pair<G,O>).
  // starting delimiter: 'ECGSignal::Recon'.
  uSum += static_cast< uint4 >( string("ECGSignal::Recon").length() + 1 );
  // total G-O pairs.
  uSum += static_cast< uint4 >( sizeof(int4));
  // pair<G,O>
  uSum += static_cast< uint4 >( sizeof(int4) * m_vReconPeriod.size() * 2);
  // ending delimiter: 'ECGSignal::Recon'.
  uSum += static_cast< uint4 >( string("ECGSignal::Recon").length() + 1);

  /// R-Points.
  // starting delimiter: 'ECGSignal::R-Point'.
  uSum += static_cast< uint4 >( string("ECGSignal::R-Point").length() + 1 );
  // total R-Points.
  uSum += static_cast< uint4 >( sizeof(int4) );
  // vector m_vRPoint.
  uSum += static_cast< uint4 >( sizeof(int4) * m_vRPoint.size() );
  // ending delimiter: 'ECGSignal::R-Point'.
  uSum += static_cast< uint4 >( string("ECGSignal::R-Point").length() + 1 );

  /// Signals.
  // starting delimiter: 'ECGSignal::Signal'.
  uSum += static_cast< uint4 >( string("ECGSignal::Signal").length() + 1 );
  // total signal points.
  uSum += static_cast< uint4 >( sizeof(int4) );
  // signals.
  uSum += static_cast< uint4 >( m_vSignal.size() * sizeof(int4) );
  // ending delimiter: 'ECGSignal::Signal'.
  uSum += static_cast< uint4 >( string("ECGSignal::Signal").length() + 1 );

  // ending delimiter: 'ECGSignal'.
  uSum += static_cast< uint4 >(  string("RetroSpective::ECGSignal").length() + 1 );
  
  return uSum;
} // GetSerializedDataSize()


/**
 *  Reads in all the data from serialized form.
 *
 *  @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 *  @param piBufferEnd pointer to the end of strips memory buffer.
 *  @return true when success
 */
bool vxECGSignal::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  try
  {
    // verify the stream by reading start delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sDelimiter) 
      throw ex::DataCorruptionException(LogRec( "data corruption: bad starting delimiter: Not 'RetroSpective::ECGSignal'.", "vxECGSignal", "ReadSerializedData" ) );

    /// 1. ECG Info
    // start delimiter: 'ECGSignal::Info'.
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::Info" )
      throw ex::DataCorruptionException(LogRec( "bad starting delimiter: Not 'ECGSignal::Info'", "vxECGSignal", "ReadSerializedData" ) );
    // string m_sPatientId.
    m_sPatientId = DeserializeStringData(hiBuffer, piBufferEnd);
    // string m_sScanDate.
    m_sScanDate = DeserializeStringData(hiBuffer, piBufferEnd);
    // string m_sScanTime.
    m_sScanTime = DeserializeStringData(hiBuffer, piBufferEnd);
    // int m_iSampleRate.
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_iSampleRate);
    // int4 m_iSamplePeriod.
    DeserializeSimpleData(hiBuffer, piBufferEnd, m_iSamplePeriod);
    /// end delimiter: 'ECGSignal Info'.
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::Info" )
      throw ex::DataCorruptionException(LogRec( "bad ending delimiter: Not 'ECGSignal::Info'", "vxECGSignal", "ReadSerializedData" ) );

    /// 2. recon: <G,O>.
    /// start delimiter: 'ECGSignal::Recon'.
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::Recon" )
      throw ex::DataCorruptionException(LogRec( "bad starting delimiter: Not 'ECGSignal::Recon'", "vxECGSignal", "ReadSerializedData" ) );
    // number of <G,O>pairs.
    int4 iNumOfPairs(0);
    DeserializeSimpleData(hiBuffer, piBufferEnd, iNumOfPairs);
    //  m_vSignalCycles.resize(iNumOfCycles);
    m_vReconPeriod.resize(iNumOfPairs);
    int4 iPoint;
    for (int4 i=0; i<iNumOfPairs; i++)
    {
      // retrive G,O points
      DeserializeSimpleData(hiBuffer, piBufferEnd, iPoint);
      m_vReconPeriod[i].first = iPoint;
      DeserializeSimpleData(hiBuffer, piBufferEnd, iPoint);
      m_vReconPeriod[i].second = iPoint;
    }
    /// end delimiter: 'ECGSignal::Recon'
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::Recon" )
      throw ex::DataCorruptionException(LogRec( "bad ending delimiter: Not 'ECGSignal::Recon'", "vxECGSignal", "ReadSerializedData" ) );

    /// 3. R-Points.
    //  start delimiter: 'ECGSignal::R-Point'.
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::R-Point" )
      throw ex::DataCorruptionException(LogRec( "bad starting delimiter: Not 'ECGSignal::R-Point'", "vxECGSignal", "ReadSerializedData" ) );
    int4 iNumOfRPoint(0), iRPoint(0);
    DeserializeSimpleData(hiBuffer, piBufferEnd, iNumOfRPoint);
    m_vRPoint.resize(iNumOfRPoint);
    for (int4 i=0; i<iNumOfRPoint; i++)
    {
      // retrive int signals.
      DeserializeSimpleData(hiBuffer, piBufferEnd, iRPoint);
      m_vRPoint[i] = iRPoint; 
    }
    /// end delimiter: 'ECGSignal::R-Point'.
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::R-Point" )
      throw ex::DataCorruptionException(LogRec( "bad ending delimiter: Not 'ECGSignal::R-Point'", "vxECGSignal", "ReadSerializedData" ) );

    /// 4. ECG signals.
    /// start delimiter: 'ECGSignal::Signal'.
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::Signal" )
      throw ex::DataCorruptionException(LogRec( "bad starting delimiter: Not 'ECGSignal::Signal'", "vxECGSignal", "ReadSerializedData" ) );
    int4 iNumOfSignals(0), iSignals(0);
    DeserializeSimpleData(hiBuffer, piBufferEnd, iNumOfSignals);
    m_vSignal.resize(iNumOfSignals);
    for (int4 i=0; i<iNumOfSignals; i++)
    {
      // retrive int signals.
      DeserializeSimpleData(hiBuffer, piBufferEnd, iSignals);
      m_vSignal[i] = iSignals; 
    }
    /// end delimiter: 'ECGSignal::Signal'.
    if( DeserializeStringData(hiBuffer, piBufferEnd) != "ECGSignal::Signal" )
      throw ex::DataCorruptionException( LogRec( "bad ending delimiter: Not 'ECGSignal::Signal'", "vxECGSignal", "ReadSerializedData" ) );

    // verify the stream again by reading end delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sDelimiter) 
      throw ex::DataCorruptionException( LogRec( "data corruption: bad ending delimiter: Not 'RetroSpective::ECGSignal'.", "vxECGSignal", "ReadSerializedData" ) );
  }
  catch (ex::OutOfMemoryException & error)
  {
    throw ex::OutOfMemoryException( LogRec( "Out of memory" + error.GetLogRecord().GetTextMessage(), "vxECGSignal", "ReadSerializedData" ) );
  }
  catch (ex::VException & error)
  {
    throw ex::VException( LogRec( error.GetLogRecord().GetTextMessage(), "vxECGSignal", "ReadSerializedData" ) );
  }

  return true;

} // ReadSerializedData()


/**
 *  Retrieves all the data in serialized form.
 *
 *  @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 *  @param piBufferEnd pointer to the end of strips memory buffer.
 *  @return true when success
 */
bool vxECGSignal::WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  // ensure there is enough space
  if (*hiBuffer + GetSerializedDataSize() > piBufferEnd) 
    throw ex::VException( LogRec( "Not enough space.", "vxECGSignal", "WriteSerializedData" ) );

  /// start delimiter: 'RetroSpective::ECGSignal'
  SerializeStringData(hiBuffer, piBufferEnd, m_sDelimiter);

  /// 1. ECG Info
  // write start delimiter: 'ECGSignal::Info'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::Info");
  // patient ID.
  SerializeStringData(hiBuffer, piBufferEnd, m_sPatientId);
  // scan date.
  SerializeStringData(hiBuffer, piBufferEnd, m_sScanDate);
  // scan time.
  SerializeStringData(hiBuffer, piBufferEnd, m_sScanTime);
  // sample rate (Hz): m_iSampleRate
  SerializeSimpleData(hiBuffer, piBufferEnd, m_iSampleRate);
  // sample period: m_fSamplePeriod
  SerializeSimpleData(hiBuffer, piBufferEnd, m_iSamplePeriod);
  // write end delimiter: 'ECGSignal::Info'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::Info");

  /// 2. recon: <G,O>.
  // write start delimiter: 'ECGSignal::ECGCycle'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::Recon");
  // write total G-O pairs.
  SerializeSimpleData(hiBuffer, piBufferEnd, m_vReconPeriod.size());
  // write recon period (pair<G,O>).
  for (uint4 i(0); i<m_vReconPeriod.size(); i++)
  {
    // G-point
    SerializeSimpleData(hiBuffer, piBufferEnd, m_vReconPeriod[i].first);
    // O-point
    SerializeSimpleData(hiBuffer, piBufferEnd, m_vReconPeriod[i].second);
  }
  // write end delimiter: 'ECGSignal::Recon'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::Recon");

  /// 3. R-Points.
  // write start delimiter: 'ECGSignal::ECGCycle'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::R-Point");
  // write total G-O pairs.
  SerializeSimpleData(hiBuffer, piBufferEnd, m_vRPoint.size());
  // write recon period (pair<G,O>).
  for (int4 i=0; i<m_vRPoint.size(); i++)
  {
    // R-point
    SerializeSimpleData(hiBuffer, piBufferEnd, m_vRPoint[i]);
  }
  // write end delimiter: 'ECGSignal::Recon'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::R-Point");

  /// 4. ECG Signals.
  // write start delimiter: 'ECGSignal::Signal'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::Signal");
  // write total number of sampled signals.
  SerializeSimpleData(hiBuffer, piBufferEnd, m_vSignal.size());
  for (int4 i=0; i<m_vSignal.size(); i++)
  {
    // write sampled signals.
    SerializeSimpleData(hiBuffer, piBufferEnd, m_vSignal[i]);
  }
  // write end delimiter: 'ECGSignal::Signal'
  SerializeStringData(hiBuffer, piBufferEnd, "ECGSignal::Signal");

  /// end delimiter: 'RetroSpective::ECGSignal'
  SerializeStringData(hiBuffer, piBufferEnd, m_sDelimiter);

  return true;

} // WriteSerializedData()


/**
 * get one recon start.
 * @param iPairsNum unkonwn
 * @return the start of a reconstruction period
 */
int4 vxECGSignal::GetReconPeriodStart(const int4 iPairsNum)
{
  if( static_cast< uint4 >( iPairsNum ) < m_vReconPeriod.size() )
    return m_vReconPeriod[ iPairsNum ].first;
  else
    return 0;
} // GetReconPeriodStart()


/**
 * get one recon end.
 * @param iPairsNum unknown
 * @return the end of a reconstruction period
 */
int4 vxECGSignal::GetReconPeriodEnd(const int4 iPairsNum)
{
  if( static_cast< uint4 >( iPairsNum ) < m_vReconPeriod.size() )
    return m_vReconPeriod[ iPairsNum ].second;
  else
    return 0;
} // GetReconPeriodEnd()


/**
 *  get R-point
 *  @param iRPointNum unkonwn
 *  @return an R-point
 */
int4 vxECGSignal::GetRPoint(const int4 iRPointNum)
{
  int4 iCorrectRPointNum(static_cast< int4 >( (static_cast< uint4 >( iRPointNum)<m_vRPoint.size()) ? iRPointNum : m_vRPoint.size()-1));
  return m_vRPoint[iCorrectRPointNum];
} // GetRPoint()


/** 
 * return pulse from ECG signals.
 * @param iCycleNum the cycle number of interest
 * @return a single ECG pulse
 */
const int4 vxECGSignal::GetECGPulse(const int4 iCycleNum) 
{ 
  if (m_vRPoint.size()<0) return -1;

  const int4 iCorrectCycleNum(static_cast< int4 >( static_cast< uint4 >( iCycleNum)<m_vRPoint.size() ? iCycleNum : m_vRPoint.size()-1));
  int4 iSamples(0);
  if (iCorrectCycleNum == 0)
  {
    iSamples = m_vRPoint[iCorrectCycleNum];
  }
  else
  {
    iSamples = m_vRPoint[iCorrectCycleNum] - m_vRPoint[iCorrectCycleNum-1];
  }

  return int4((float4)m_iSampleRate / (float4)iSamples * 60.0F);
} // GetECGPulse()


/**
 * Copy the signal data out into a pre-allocated buffer.
 * @param piOutBuffer the output buffer
 */
void vxECGSignal::CopySignalData( int4 * piOutBuffer )
{
  for (uint4 i(0); i<m_vSignal.size(); i++)
  {
    piOutBuffer[i] = m_vSignal[i];
  }
} // CopySignalData()


// $Log: vxECGSignal.C,v $
// Revision 1.3  2006/05/31 16:16:41  geconomos
// porting to visual studio 2005
//
// Revision 1.2  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/27 13:13:50  frank
// code review
//
// Revision 1.8  2003/05/16 14:56:54  dongqing
// code review
//
// Revision 1.7  2003/05/16 11:52:21  michael
// code review
//
// Revision 1.6  2002/10/14 15:30:54  frank
// Fixed ECG signal I/O problem.
//
// Revision 1.5  2002/08/26 18:48:09  frank
// fixed lookup into empty data struct.
//
// Revision 1.4  2002/08/02 15:45:20  frank
// Added data output functions.
//
// Revision 1.3  2002/08/01 18:23:16  frank
// Working on selected phase volume output.
//
// Revision 1.2  2002/07/16 17:08:02  frank
// Centralized the delimiter string.
//
// Revision 1.1  2002/07/10 18:34:07  frank
// Initial version.
//
// Revision 1.8  2002/07/11 16:21:26  binli
// if no ECG signal record.
//
// Revision 1.7  2002/06/18 17:41:17  binli
// added new class: ECGRender
//
// Revision 1.6  2002/06/11 13:38:51  binli
// copy constructor & assign operator
//
// Revision 1.5  2002/06/10 20:56:52  binli
// Reset
//
// Revision 1.4  2002/06/10 18:52:01  binli
// use file extension
//
// Revision 1.3  2002/05/30 21:40:50  binli
// function ECGCycle::SetPoints()
//
// Revision 1.2  2002/05/30 17:20:46  binli
// serialization ECGSignal info
//
// Revision 1.1  2002/05/29 21:52:24  binli
// Initial version of ECGSignal class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxECGSignal.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
// $Id: vxECGSignal.C,v 1.3 2006/05/31 16:16:41 geconomos Exp $
