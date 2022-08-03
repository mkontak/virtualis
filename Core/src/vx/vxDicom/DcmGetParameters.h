// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmGetParameters.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmMoveParameters.h"

namespace dcm
{


class DCM_DLL DcmGetParameters : public DcmMoveParameters
{
public:

  /// Constructor
  DcmGetParameters();
  
  /// Copy constructor
  DcmGetParameters(const DcmGetParameters & parameters);

  /// Constructor
  DcmGetParameters(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid);

  /// Constructor
  DcmGetParameters(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sMedicalId, const std::string & sPatientName);

  /// Destrcutor
  virtual ~DcmGetParameters() { }

  /// Validates the parameters
  virtual void  Validate();

  /// Creates the GET using the uids
  static std::shared_ptr<DcmGetParameters> Create(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid);

  /// Craetes the GET using the pateint
  static std::shared_ptr<DcmGetParameters> Create(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sMedicalId, const std::string & sPatientName);


  /// Determines if the two parameters lists are going to the same destination
  virtual bool SameDestination(std::shared_ptr<DcmGetParameters> & rhs)
  { return (  m_iDestinationPort == rhs->m_iDestinationPort && DcmMoveParameters::SameDestination(std::shared_ptr<DcmMoveParameters>(rhs) )); }


  /// Gets the DcmImage represetation
  virtual std::shared_ptr<DcmImage> GetDcmImage();

public:
  /// Gets the destination port
  const int GetDestinationPort()
  { return m_iDestinationPort; }

 private:

  /// Destination Port
  int m_iDestinationPort;


}; // class DcmGetParameters

}; // namespace dcm