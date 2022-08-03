// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmQueryRetrieveParameters.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "Stdafx.h"
#include "DcmUtility.h"
#include "DcmExceptions.h"
#include "DcmQueryRetrieveParameters.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmName.h"
#include "DcmImageFactory.h"
#include "DcmLog.h"


// usings
using namespace dcm;


/**
 * Constructor - Default
 */
DcmQueryRetrieveParameters::DcmQueryRetrieveParameters() :
m_sMedicalId(""),
m_sPhysician(""),
m_sStudyId(""),
m_sLevel("SERIES"),
m_sAffectedSopInstanceUid("")
{



} // DcmQueryRetrieveParameters() 


/**
 * Copy constructor
 *
 * @param parameters    DcmQueryRetrieveParameters to be copied
 */
DcmQueryRetrieveParameters::DcmQueryRetrieveParameters(const DcmQueryRetrieveParameters & parameters) 
{

  m_sMedicalId                = parameters.m_sMedicalId;
  m_sPhysician                = parameters.m_sPhysician;
  m_sStudyId                  = parameters.m_sStudyId;

  m_patientDob                = parameters.m_patientDob;    
  m_studyDate                 = parameters.m_studyDate;
  m_seriesDate                = parameters.m_seriesDate;
  m_patientTob                = parameters.m_patientTob;
  m_studyTime                 = parameters.m_studyTime;
  m_seriesTime                = parameters.m_seriesTime;
  m_patients                  = parameters.m_patients;
  m_instances                 = parameters.m_instances;
  m_modalities                = parameters.m_modalities;
  m_studyUids                 = parameters.m_studyUids;
  m_seriesUids                = parameters.m_seriesUids;
  m_sLevel                    = parameters.m_sLevel;
  m_sAffectedSopInstanceUid   = parameters.m_sAffectedSopInstanceUid;

} // DcmQueryRetrieveParameters() 


/**
 * Destructor
 */
DcmQueryRetrieveParameters::~DcmQueryRetrieveParameters() 
{  
} // ~DcmQueryRetrieveParameters() 


/**
 * Creates the DcmQueryRetrieveParameters from the DcmImage supplied
 *
 * @param pDcmImage       Image to be used
 *
 * @return DcmQueryRetrieveParameters
 */
std::shared_ptr<DcmQueryRetrieveParameters> DcmQueryRetrieveParameters::Create(std::shared_ptr<DcmImage> pDcmImage)
{
     
  std::shared_ptr<DcmQueryRetrieveParameters> pParameters = std::make_shared<DcmQueryRetrieveParameters>();

  if ( pDcmImage == __nullptr )
    throw DcmInvalidDataException(LogRec("DcmImage is null", "DcmQueryRetrieveParameters", "Create"));

  pParameters->Initialize(pDcmImage);

  return pParameters;

} // Create(std::shared_ptr<DcmImage> pDcmImage)


/**
 * Initializes the state of the Query/Parameters object using the DcmImage object
 *
 * @param pDcmImage       Image to be used
 */
void  DcmQueryRetrieveParameters::Initialize(std::shared_ptr<DcmImage> pDcmImage)
{
     

  m_patients.Assign(pDcmImage->GetPatientName());
  m_studyUids.Assign(pDcmImage->GetStudyInstanceUid());
  m_seriesUids.Assign(pDcmImage->GetSeriesInstanceUid());


  m_sLevel                   = pDcmImage->GetQueryRetrievalLevel();
  m_sAffectedSopInstanceUid  = pDcmImage->GetAffectedSopClassUid();
  m_sMedicalId               = pDcmImage->GetPatientId();

  std::string sDOB("");
  std::string sTOB("");
  std::string sStudyDate("");
  std::string sStudyTime("");
  std::string sSeriesDate("");
  std::string sSeriesTime("");
  std::string sModalities("");
  std::string sInstanceUids("");
  std::string sStudyUid("");
  std::string sSeriesUId("");

   
  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0010, 0x0030), sDOB);
  SetPatientDob(sDOB);
  
  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0010, 0x0032), sTOB);
  SetPatientTob(sTOB);


  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0010, 0x0020), m_sMedicalId);
  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0020, 0x0010), m_sStudyId);


  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0008, 0x0020), sStudyDate);
  SetStudyDate(sStudyDate);

  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0008, 0x0030), sStudyTime);
  SetStudyTime(sStudyTime);

  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0008, 0x0090), m_sPhysician);


  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0008, 0x0021), sSeriesDate);
  SetSeriesDate(sSeriesDate);

  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0008, 0x0031), sSeriesTime);
  SetSeriesTime(sSeriesTime);

  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0008, 0x0018), sInstanceUids);
  pDcmImage->TryGetValue<std::string>( DcmUtility::CreateDcmTag(0x0008, 0x0060), sModalities);
 
} // Initialize(std::shared_ptr<DcmImage> pDcmImage)



 
/**
 * Builds the DcmImage contains the query elements and requested return elements.
 *
 * @return DcmImage containing elements for a query.
 */
std::shared_ptr<DcmImage> DcmQueryRetrieveParameters::GetDcmImage()
{

  std::shared_ptr<DcmImage> pDcmImage(DcmImageFactory::Create(true));

  // Makes sure the create worked
  if ( pDcmImage == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocation DcmImage", "DcmQueryRetrieveParameters", "GetDcmImage"));

  // Empty
  std::string sEmpty("");

  pDcmImage->SetValue<std::string>( DCM_CommandGroupLength,       "0" );

  // Patient Level
  pDcmImage->SetValue<std::string>( DCM_PatientName,              m_patients.ToString());
  pDcmImage->SetValue<std::string>( DCM_PatientBirthDate,         m_patientDob.ToString());
  pDcmImage->SetValue<std::string>( DCM_PatientBirthTime,         m_patientTob.ToString());
  pDcmImage->SetValue<std::string>( DCM_PatientID,                m_sMedicalId);

  // Study Level
  pDcmImage->SetValue<std::string>( DCM_StudyID,                  m_sStudyId);
  pDcmImage->SetValue<std::string>( DCM_StudyInstanceUID,         m_studyUids.ToString());
  pDcmImage->SetValue<std::string>( DCM_StudyDate,                m_studyDate.ToString());
  pDcmImage->SetValue<std::string>( DCM_StudyTime,                m_studyTime.ToString());
  pDcmImage->SetValue<std::string>( DCM_ReferringPhysicianName,   m_sPhysician);
  pDcmImage->SetValue<std::string>( DCM_StudyDescription,         sEmpty);        

  // Series Level
  pDcmImage->SetValue<std::string>( DCM_SeriesNumber,             m_sSeriesNumber);
  pDcmImage->SetValue<std::string>( DCM_SeriesInstanceUID,        m_seriesUids.ToString());
  pDcmImage->SetValue<std::string>( DCM_SeriesDate,               m_seriesDate.ToString());
  pDcmImage->SetValue<std::string>( DCM_SeriesTime,               m_seriesTime.ToString());
   pDcmImage->SetValue<std::string>( DCM_SeriesDescription,       sEmpty);        


  // Instance Level
  pDcmImage->SetValue<std::string>( DCM_SOPInstanceUID,           m_instances.ToString());

  // ========================================================================================
  // If we are doing a series level query then set the series level modalities. otherwise 
  // set the modalities in study field.
  // =======================================================================================
  if ( m_sLevel  == "SERIES" )
  {
    pDcmImage->SetValue<std::string>( DCM_Modality,                  m_modalities.ToString());
    pDcmImage->SetValue<std::string>( DCM_ModalitiesInStudy,         sEmpty);
  } 
  else 
  {
    pDcmImage->SetValue<std::string>( DCM_Modality,                  sEmpty);
    pDcmImage->SetValue<std::string>( DCM_ModalitiesInStudy,         m_modalities.ToString());
  }  

  pDcmImage->SetValue<std::string>( DCM_StorageMediaFileSetID,  sEmpty);    
  pDcmImage->SetValue<std::string>( DCM_StorageMediaFileSetUID, sEmpty); 

  pDcmImage->SetValue<std::string>( DCM_RetrieveAETitle,        sEmpty);    
  //pDcmImage->SetValue<std::string>( DCM_InstanceAvailability,   sEmpty);         // Instance Availability ( ONLINE, NEARLINE, OFFLINE, UNAVAILABLE )
 
  // These fields are empty because we are requesting them
  pDcmImage->SetValue<std::string>( DCM_NumberOfPatientRelatedStudies,    sEmpty);    // Number of patient related studies
  pDcmImage->SetValue<std::string>( DCM_NumberOfPatientRelatedSeries,     sEmpty);    // Number of patient related series
  pDcmImage->SetValue<std::string>( DCM_NumberOfPatientRelatedInstances,  sEmpty);    // Number of patient related instances
  pDcmImage->SetValue<std::string>( DCM_NumberOfStudyRelatedSeries,       sEmpty);    // Number of study related series
  pDcmImage->SetValue<std::string>( DCM_NumberOfStudyRelatedInstances,    sEmpty);    // Number of study related instances
  pDcmImage->SetValue<std::string>( DCM_NumberOfSeriesRelatedInstances,   sEmpty);    // Number of series related instances

  pDcmImage->SetQueryRetrievalLevel(m_sLevel);
    
  return pDcmImage;

} // GetDcmImage()


/**
 * Sets the Query/Retrieve level
 *
 * @param sLevel      New level
 */
void DcmQueryRetrieveParameters::SetLevel(const std::string & sLevel)
{
  // ====================
  // Validate the level 
	// =====================
  if ( sLevel != "PATIENT" && sLevel != "STUDY" &&  sLevel != "SERIES" && sLevel != "IMAGE" )
    throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid query retrieval level specified (%s)",sLevel), "DcmQueryRetrieveParameters", "SetLevel"));

  m_sLevel = sLevel;
} // SetLevel(const std::string & sLevel)
