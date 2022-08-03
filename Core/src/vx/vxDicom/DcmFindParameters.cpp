// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmFindParameters.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "DcmUtility.h"
#include "DcmExceptions.h"
#include "DcmFindParameters.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmName.h"
#include "DcmImage.h"


using namespace dcm;


/**
 * Constructor - Default
 */
DcmFindParameters::DcmFindParameters() :
DcmQueryRetrieveParameters()
{
  m_sAffectedSopInstanceUid = UID_FINDStudyRootQueryRetrieveInformationModel;
} // DcmFindParameters() 


/**
 * Copy constructor
 *
 * @param parameters    DcmFindParameters to be copied
 */
DcmFindParameters::DcmFindParameters(const DcmFindParameters & parameters) :
DcmQueryRetrieveParameters(static_cast<DcmQueryRetrieveParameters &>(const_cast<DcmFindParameters &>(parameters)))
{
  m_sAffectedSopInstanceUid = UID_FINDStudyRootQueryRetrieveInformationModel;
} // DcmFindParameters() 


/**
 * Destructor
 */
DcmFindParameters::~DcmFindParameters() 
{  
} // ~DcmFindParameters() 


/**
 * Creates the DcmFindParameters from the DcmImage supplied
 *
 * @param pDcmImage       Image to be used
 *
 * @return DcmFindParameters
 */
std::shared_ptr<DcmFindParameters> DcmFindParameters::Create(std::shared_ptr<DcmImage> pDcmImage)
{
  
  if ( pDcmImage == __nullptr )
    throw DcmInvalidDataException(LogRec("DcmImage is null", "DcmFindParameters", "Create"));

  std::shared_ptr<DcmFindParameters> pParameters = std::make_shared<DcmFindParameters>();

  if ( pParameters == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocated DcmFinParameters", "DcmFindParameters", "Create"));

  pParameters->Initialize(pDcmImage);

  return pParameters;

} // Create(std::shared_ptr<DcmImage> pDcmImage)


/**
 * Creates the DcmFindParameters from the DcmImage supplied
 *
 * @return DcmFindParameters
 */
std::shared_ptr<DcmFindParameters> DcmFindParameters::Create()
{
  
 std::shared_ptr<DcmFindParameters> pParameters = std::make_shared<DcmFindParameters>();

 if ( pParameters == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocated DcmFinParameters", "DcmFindParameters", "Create"));

  return pParameters;

} // Create(std::shared_ptr<DcmImage> pDcmImage)


/**
 * Sets the level of the query
 *
 * @param sLevel      New level
 */
void DcmFindParameters::SetLevel(const std::string & sLevel)
{
  DcmQueryRetrieveParameters::SetLevel(sLevel);
  
  if ( GetLevel() == "PATIENT" )
    m_sAffectedSopInstanceUid = UID_FINDPatientRootQueryRetrieveInformationModel;
  else 
    m_sAffectedSopInstanceUid = UID_FINDStudyRootQueryRetrieveInformationModel;
} // SetLevel(const std::string & sLevel)