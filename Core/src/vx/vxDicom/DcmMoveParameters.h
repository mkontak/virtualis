// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmMoveParameters.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmQueryRetrieveParameters.h"




namespace dcm
{

/// Forward declarations
class DCM_DLL DcmImage;


/**
 * Parameters needed for a C-MOVE operation
 */
class DCM_DLL DcmMoveParameters : public DcmQueryRetrieveParameters
{
public:

  /// Default constructor
  DcmMoveParameters();

  /// Constructor
  DcmMoveParameters(const std::string & sDestinationAETitle, const std::string & sPatientId, const std::string & sPatientName);

  /// Constructor
  DcmMoveParameters(const std::string & sDestinationAETitle, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid);

  /// Copy Constructor
  DcmMoveParameters(const DcmMoveParameters & parameters);

  /// Destructor
  virtual ~DcmMoveParameters();

  /// Creates the DcmMoveParameters from the DcmImage
  static std::shared_ptr<DcmMoveParameters> Create(std::shared_ptr<DcmImage> pDcmImage);


  /// Validates the parameters
  virtual void  Validate();


  /// Determines if the two parameters lists are going to the same destination
  virtual bool SameDestination(std::shared_ptr<DcmMoveParameters> & rhs)
  { return (  GetDestinationAEtitle() == rhs->GetDestinationAEtitle() ); }


 
  /// Gets the DCM image for the  parameters
  virtual std::shared_ptr<DcmImage> GetDcmImage();
 
public:

    /// Gets the destination ae title
  const std::string & GetDestinationAEtitle()
  { return m_sDestinationAEtitle; }

  /// Sets the destination ae title
  void SetDestinationAEtitle(const std::string & sAETitle)
  { m_sDestinationAEtitle = sAETitle; }


private:

  
   /// Destination ae title 
  std::string m_sDestinationAEtitle;

};  // class DcmMoveParameters


};  // namespace dm