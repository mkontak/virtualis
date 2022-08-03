// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmMoveParameters.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmEnums.h"
#include "DcmExceptions.h"
#include "DcmImageFactory.h"
#include "DcmMoveParameters.h"
#include "DcmImage.h"
#include "DcmList.h"


using namespace dcm;




/**
 * Constructor
 */
DcmMoveParameters::DcmMoveParameters() :
DcmQueryRetrieveParameters(),
m_sDestinationAEtitle("")
{
  m_sAffectedSopInstanceUid = UID_MOVEStudyRootQueryRetrieveInformationModel;
} // DcmMoveParameters()


/**
 * Copy Constructor
 *
 *  @param parameters
 */
DcmMoveParameters::DcmMoveParameters(const DcmMoveParameters & parameters) :
DcmQueryRetrieveParameters(static_cast<DcmQueryRetrieveParameters &>(const_cast<DcmMoveParameters &>(parameters)))
{
   m_sDestinationAEtitle       = parameters.m_sDestinationAEtitle;
} // DcmMoveParameters(const DcmMoveParameters & parameters)


/**
 * Constructor used to create the move parameters  for a patient type C-MOVE 
 *
 * @param sDestinationAETitle     Destination AE Title
 * @param sMedicalId              Medical Id
 * @param sPatientName            Patient Name
 */
DcmMoveParameters::DcmMoveParameters(const std::string & sDestinationAETitle, const std::string & sMedicalId, const std::string & sPatientName) :
DcmQueryRetrieveParameters()
{
  m_sAffectedSopInstanceUid = UID_MOVEStudyRootQueryRetrieveInformationModel;
  SetDestinationAEtitle(sDestinationAETitle);
  SetMedicalId(sMedicalId);
  SetPatientNames(sPatientName);
  m_sLevel = "PATIENT";
} // DcmMoveParameters(const std::string & sDestinationAETitle, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid)


/**
 * Constructor used to create the move parameters for a UID type C-MOVE
 *
 * @param sDestinationAETitle     Destination AE Title
 * @param sStudyUid               Study Uid
 * @param sSeriesUid              Series Uid
 * @param sInstanceUid            Image Uid
 */
DcmMoveParameters::DcmMoveParameters(const std::string & sDestinationAETitle, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid) :
DcmQueryRetrieveParameters()
{
  m_sAffectedSopInstanceUid = UID_MOVEStudyRootQueryRetrieveInformationModel;
  SetDestinationAEtitle(sDestinationAETitle);
  SetStudyUids(sStudyUid);
  SetSeriesUids(sSeriesUid);
  SetInstanceUids(sInstanceUid);
  if ( sSeriesUid.empty() ) m_sLevel = "STUDY";
} // DcmMoveParameters(const std::string & sDestinationAETitle, const std::string & sStudyUid, const std::string & sSeriesUid, const std::string & sInstanceUid)


/**
 * Destructor
 */
DcmMoveParameters::~DcmMoveParameters()
{
  
} // ~DcmMoveParameters()


/**
 * Creates the DcmFindParameters from the DcmImage supplied
 *
 * @param pDcmImage       Image to be used
 *
 * @return DcmFindParameters
 */
std::shared_ptr<DcmMoveParameters> DcmMoveParameters::Create(std::shared_ptr<DcmImage> pDcmImage)
{
  
 std::shared_ptr<DcmMoveParameters> pParameters = std::make_shared<DcmMoveParameters>();

  if ( pDcmImage == __nullptr )
    throw DcmInvalidDataException(LogRec("DcmImage is null", "DcmMoveParameters", "Create"));

  pParameters->Initialize(pDcmImage);

  return pParameters;

} // Create(std::shared_ptr<DcmImage> pDcmImage)



/**
 * Validates the parameters and if there is an error an exception is thrown.
 */
void DcmMoveParameters::Validate()
{
  // Make sure the destination ae title was specified
  if ( GetDestinationAEtitle().empty() )
    throw DcmInvalidDataException(LogRec("No destination ae title specified","DcmMoveParameters","Validate"));

  if ( m_sMedicalId.empty() && m_patients.IsEmpty() && m_studyUids.IsEmpty() &&  m_seriesUids.IsEmpty() && m_instances.IsEmpty()  )
    throw DcmInvalidDataException(LogRec("Missing search criteria","DcmMoveParameters","Validate"));

} // Validate()


/**
 * Gets the DcmImage for this grouping of parameters
 *
 *  @return the pointer of the image
 */
std::shared_ptr<DcmImage> DcmMoveParameters::GetDcmImage()
{

  std::shared_ptr<DcmImage> pDcmImage(DcmImageFactory::Create(true));

  // Makes sure the create worked
  if ( pDcmImage == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocation DcmImage", "DcmMoveParameters", "GetDcmImage"));

  std::string sEmpty("");

  pDcmImage->SetValue<std::string>( DCM_CommandGroupLength, "0" );
  //pDcmImage->SetValue<unsigned short>( DCM_MessageID, 0 );
  //pDcmImage->SetValue<unsigned short>( DCM_CommandField, 21 );
  //pDcmImage->SetValue<std::string>( DCM_MoveDestination, m_sDestinationAEtitle );
  //pDcmImage->SetValue<unsigned short>( DCM_Priority, 0 );
  //pDcmImage->SetValue<unsigned short>( DCM_AffectedSOPClassUID, m_sAffectedSopInstanceUid );

  // Patient Level
  if ( ! m_patients.IsEmpty()     )   pDcmImage->SetValue<std::string>( DCM_PatientName,              m_patients.ToString());
  if ( ! m_patientDob.IsEmpty()   )   pDcmImage->SetValue<std::string>( DCM_PatientBirthDate,         m_patientDob.ToString());
  if ( ! m_patientTob.IsEmpty()   )   pDcmImage->SetValue<std::string>( DCM_PatientBirthTime,         m_patientTob.ToString());
  if ( ! m_sMedicalId.empty()     )   pDcmImage->SetValue<std::string>( DCM_PatientID,                m_sMedicalId);

  // Study Level
  if ( ! m_sStudyId.empty()       )   pDcmImage->SetValue<std::string>( DCM_StudyID,                  m_sStudyId);
  if ( ! m_studyUids.IsEmpty()    )   pDcmImage->SetValue<std::string>( DCM_StudyInstanceUID,         m_studyUids.ToString());
  if ( ! m_studyDate.IsEmpty()    )   pDcmImage->SetValue<std::string>( DCM_StudyDate,                m_studyDate.ToString());
  if ( ! m_studyTime.IsEmpty()    )   pDcmImage->SetValue<std::string>( DCM_StudyTime,                m_studyTime.ToString());
  if ( ! m_sPhysician.empty()     )   pDcmImage->SetValue<std::string>( DCM_ReferringPhysicianName,   m_sPhysician);

  // Series Level
  if ( ! m_sSeriesNumber.empty()  )   pDcmImage->SetValue<std::string>( DCM_SeriesNumber,             m_sSeriesNumber);
  if ( ! m_seriesUids.IsEmpty()   )   pDcmImage->SetValue<std::string>( DCM_SeriesInstanceUID,        m_seriesUids.ToString());
  if ( ! m_seriesDate.IsEmpty()   )   pDcmImage->SetValue<std::string>( DCM_SeriesDate,               m_seriesDate.ToString());
  if ( ! m_seriesTime.IsEmpty()   )   pDcmImage->SetValue<std::string>( DCM_SeriesTime,               m_seriesTime.ToString());
  if ( ! m_instances.IsEmpty()    )   pDcmImage->SetValue<std::string>( DCM_SOPInstanceUID,           m_instances.ToString());

  // ========================================================================================
  // If we are doing a series level query then set the series level modalities. otherwise 
  // set the modalities in study field.
  // =======================================================================================
  if ( ! m_modalities.IsEmpty() )
  {
    if ( m_sLevel  == "SERIES" )
      pDcmImage->SetValue<std::string>( DCM_Modality,                  m_modalities.ToString());
    else 
      pDcmImage->SetValue<std::string>( DCM_ModalitiesInStudy,         m_modalities.ToString());
  } // END ... If there are modalities to search for


  pDcmImage->SetQueryRetrievalLevel(m_sLevel);
    
  return pDcmImage;

} // GetDcmImage()


