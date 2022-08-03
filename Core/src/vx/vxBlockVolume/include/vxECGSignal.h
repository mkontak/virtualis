// $Id: vxECGSignal.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
// 
// Copyright © 2002, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Bin Li (binli@viatronix.com)

#ifndef vxECGSignal_h
#define vxECGSignal_h


// Includes
#include "Typedef.h"
#include <vector>


// Export the STL classes that are memebers of vxECGSignal
//template class VX_BLOCK_VOLUME_DLL std::allocator<int4>;
//template class VX_BLOCK_VOLUME_DLL std::vector<int4>;
//template class VX_BLOCK_VOLUME_DLL std::allocator< std::pair<int4, int4> >;
//template class VX_BLOCK_VOLUME_DLL std::vector< std::pair<int4, int4> >;


/**
 *  Define the class to handle the ECG signal
 */
class VX_BLOCK_VOLUME_DLL vxECGSignal  
{
// typedefs
public:

  /// a reconstruction period
  typedef std::pair<int4, int4> ReconPeriod;

// member functions
public:

  /// Constructor
	vxECGSignal();

  /// Destructor
	virtual ~vxECGSignal();

  /// copy construcor
  vxECGSignal( const vxECGSignal & other );

  /// assignment operator
  vxECGSignal & operator =( const vxECGSignal & other );
  
  /// types of signals
  enum SignalTypeEnum { N_SIGNAL,  R_SIGNAL, G_SIGNAL, O_SIGNAL };

  /// reset.
  void Reset();

  /// get sample rate.
  inline const int4 GetSampleRate() { return m_iSampleRate; };

  /// get sample period.
  inline const int4 GetSamplePeriod() { return static_cast< int4 >(m_iSamplePeriod); };

  /// get total number of recon periods.
  inline const int4 GetTotalRecons() { return static_cast< int4 >( m_vReconPeriod.size()); };

  /// get one recon period (pair<G,O>) info.
  int4 GetReconPeriodStart(const int4 iPairNum);

  /// get one recon period (pair<G,O>) info.
  int4 GetReconPeriodEnd(const int4 iPairNum);

  /// get vxECGSignal vector
  std::vector<int4> & GetSignals() { return m_vSignal; };

  /// get total sample pulses.
  inline int4 GetTotalSamplePulses() { return static_cast< int4 >( m_vRPoint.size() ); };

  /// get R-point
  int4 GetRPoint(const int4 iRPointNum);
  
  /// Get the number of signal data points.
  inline uint4 GetNumSamples() const { return static_cast< uint4 >( m_vSignal.size() ); }

  /// Get a single sample data point.
  inline uint4 GetSample( const uint4 & uIndex ) { return m_vSignal[uIndex]; }

  /// Copy the signal data out into a pre-allocated buffer.
  void CopySignalData( int4 * piOutBuffer );

  /// get pulse from ECG signals.
  const int4 GetECGPulse(const int4 iCycle);

  /// read ECG signal (.iri) file (txt).
  bool ReadECGFile(const std::string & sFileName);

  /// retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// retrieves all the data in serialized form.
  bool WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// Reads in all the data from serialized form.
  bool ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// Analysis: get average cycle time.
  float4 AverageCycleTime();

private:

  /// common routine: get digital signal
  int4 GetDigSignal(std::string & sStr, SignalTypeEnum & eSignalType);

  /// common routine: get effective string (patientId/ScanDate/ScanTime/SampleRate/SamplePeriod)
  std::string GetEffectSubString(std::string & sStr, const int4 iPos);

// data members
private:
  
  /// patientID
  std::string m_sPatientId;

  /// ScanDate
  std::string m_sScanDate;

  /// ScanTime
  std::string m_sScanTime;

  /// sample rate.
  int4 m_iSampleRate;

  /// sample period: time.
  float4 m_iSamplePeriod;

  /// recon period (pair::<G,O>).
  std::vector<ReconPeriod> m_vReconPeriod;

  /// R-point.
  std::vector<int4> m_vRPoint;

  /// ECG signals.
  std::vector<int4> m_vSignal;

  /// A string delimiter for I/O debugging.
  static const std::string m_sDelimiter;

}; // class vxECGSignal


#endif // vxECGSignal_h


// $Log: vxECGSignal.h,v $
// Revision 1.2  2004/03/04 21:00:55  geconomos
// Modifications for 7.1 compiler including: use of typename for dependant types and resolving ambiguities.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2003/05/27 13:13:50  frank
// code review
//
// Revision 1.10  2003/05/16 14:56:54  dongqing
// code review
//
// Revision 1.9  2003/05/16 13:15:58  frank
// formatting
//
// Revision 1.8  2003/05/16 11:52:21  michael
// code review
//
// Revision 1.7  2002/09/19 17:56:52  geconomos
// Exported contained STL instantiations.
//
// Revision 1.6  2002/08/02 20:40:19  frank
// Added signal data access.
//
// Revision 1.5  2002/08/02 15:45:20  frank
// Added data output functions.
//
// Revision 1.4  2002/08/01 18:23:16  frank
// Working on selected phase volume output.
//
// Revision 1.3  2002/07/16 17:08:02  frank
// Centralized the delimiter string.
//
// Revision 1.2  2002/07/15 13:22:27  geconomos
// exported classes.
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
// Revision 1.4  2002/05/30 21:40:26  binli
// function ECGCycle::SetPoints()
//
// Revision 1.3  2002/05/30 17:20:08  binli
// serialization vxECGSignal info
//
// Revision 1.2  2002/05/29 23:40:50  frank
// Formatting.
//
// Revision 1.1  2002/05/29 21:50:40  binli
// Initial version of vxECGSignal class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxECGSignal.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
// $Id: vxECGSignal.h,v 1.2 2004/03/04 21:00:55 geconomos Exp $
