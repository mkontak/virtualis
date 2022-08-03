// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmGetParameters.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmGetParameters.h"
#include "DcmExceptions.h"
#include "DcmImage.h"


using namespace dcm;

/**
 * Constructor
 */
DcmGetParameters::DcmGetParameters() : 
DcmMoveParameters(),
m_iDestinationPort(0)
{ 
} // DcmGetParameters()

/**
 * Copy conctructor
 *
 *  @param parameters
 */
DcmGetParameters::DcmGetParameters(const DcmGetParameters & parameters) :
DcmMoveParameters(static_cast<DcmMoveParameters>(const_cast<DcmGetParameters &>(parameters))),
m_iDestinationPort(parameters.m_iDestinationPort)
{
} // DcmGetParameters(const DcmGetParameters & parameters)


/**
 * Constructor 
 *
 * @param sDestinationAETitle     Destination AE Title ( Required )
 * @param iDestinationPort        Port the destination server is lisetning on ( Required )
 * @param sStudyUid               Study Uid
 * @param sSeriesUid              Series Uid
 * @param sInstanceUid            Image Uid
 */
DcmGetParameters::DcmGetParameters(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid) :
DcmMoveParameters(sDestinationAETitle, sStudyUid, sSeriesUid, sInstanceUid ),
m_iDestinationPort(iDestinationPort)
{
 } // DcmGetParameters(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid)


/**
 * Constructor 
 *
 * @param sDestinationAETitle     Destination AE Title ( Required )
 * @param iDestinationPort        Port the destination server is lisetning on ( Required )
 * @param sMedicalId              Medical Id
 * @param sPatientName            Patient Name
 *
 */
DcmGetParameters::DcmGetParameters(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sMedicalId, const std::string & sPatientName) :
DcmMoveParameters(sDestinationAETitle, sMedicalId, sPatientName ),
m_iDestinationPort(iDestinationPort)
{
 } // DcmGetParameters(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sMedicalId, const std::string & sPatientName)


/**
 * Creates the GET parameters using the uid parameters
 *
 * @param sDestinationAETitle       Destination of the MOVE
 * @param iDestinationPort          Port the destination server is listening on
 * @param sStudyUid                 Study Uid
 * @param sSeriesUId                Series Uid
 * @param sInstanceUid              Instance UID
 * 
 * @return DcmGetParameters
 */
 std::shared_ptr<DcmGetParameters> DcmGetParameters::Create(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid)
 {

   std::shared_ptr<DcmGetParameters> pParameters(std::make_shared<DcmGetParameters>(sDestinationAETitle, iDestinationPort, sStudyUid, sStudyUid, sInstanceUid ));

   if ( pParameters == __nullptr )
     throw DcmOutOfMemoryException( LogRec("Failed to allocate DcmGetParameters", "DcmGetParameters", "Create"));

   return pParameters;

 }  //  Create(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid)


/**
 * Creates the GET parameters using the patient info
 *
 * @param sDestinationAETitle       Destination of the MOVE
 * @param iDestinationPort          Port the destination server is listening on
 * @param sMeidcalId                Medical Id
 * @param sPatientName              Patient Name
 * 
 * @return DcmGetParameters
 */
std::shared_ptr<DcmGetParameters> DcmGetParameters::Create(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sMedicalId, const std::string & sPatientName)
{

   std::shared_ptr<DcmGetParameters> pParameters(std::make_shared<DcmGetParameters>(sDestinationAETitle, iDestinationPort, sMedicalId, sPatientName ) );

   if ( pParameters == __nullptr )
     throw DcmOutOfMemoryException( LogRec("Failed to allocate DcmGetParameters", "DcmGetParameters", "Create"));

   return pParameters;

} // Create(const std::string & sDestinationAETitle, const int iDestinationPort, const std::string & sMedicalId, const std::string & sPatientName)


/**
 * Valdates the object
 */
void DcmGetParameters::Validate()
{

  /// Validates the base
  DcmMoveParameters::Validate();

  if ( m_iDestinationPort == 0 ) 
   throw DcmInvalidDataException(LogRec("Invalid destination port","DcmGetParameters","Create"));

} // Validate()


/**
 * Get the DCmImag repsesentation
 */
std::shared_ptr<DcmImage> DcmGetParameters::GetDcmImage()
{

  std::shared_ptr<DcmImage> pDcmImage(DcmMoveParameters::GetDcmImage());

  return pDcmImage;

} // GetDcmImage()