// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CMoveParameters.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "StdAfx.h"
#include "CMoveParameters.h"

// usings
USING_DICOM_NS

/**
 * Constructor.
 */
CMoveParameters::CMoveParameters()
{

  m_pDcmMoveParameters.reset( new std::shared_ptr<dcm::DcmMoveParameters>(std::make_shared<dcm::DcmMoveParameters>()) );

} // CMoveParameters()


/**
 * Destructor.
 */
CMoveParameters::~CMoveParameters()
{
  this->!CMoveParameters();
} // ~CMoveParameters()


/**
 * Finalizer.
 */
CMoveParameters::!CMoveParameters()
{
  m_pDcmMoveParameters.reset();
} // !CMoveParameters()


#pragma region Create Methods

/**
 * Creates the CMoveParameters object
 *
 * @return CMoveParameters object
 */
CMoveParameters ^ CMoveParameters::Create()
{
  CMoveParameters ^ mpParameters = gcnew CMoveParameters();

  if ( mpParameters == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate CMoveParameters");

  return mpParameters;
} // Create()

/**
 * Creates the CMoveParameters object
 *
 * @param sPatientId      Patient id
 * @param sPatientName    Patient Name
 *
 * @return CMoveParameters object
 */
CMoveParameters ^ CMoveParameters::Create(String ^ sDestinationAETitle, String ^ sMedicalId, String ^ sPatientName )
{

  CMoveParameters ^ mpParameters = Create();

  mpParameters->MedicalId = sMedicalId;
  mpParameters->PatientNames = sPatientName;
  mpParameters->DestinationAETitle = sDestinationAETitle;

  return mpParameters;
} // Create(String ^ sPatientId, String ^ sPatientName

/**
 * Creates the CMoveParameters object
 *
 * @param sPatientId      Patient id
 * @param sPatientName    Patient Name
 *
 * @return CMoveParameters object
 */
CMoveParameters ^ CMoveParameters::Create(String ^ sDestinationAETitle, String ^ sStudyUid, String ^ sSeriesUid, String ^ sInstanceUid)
{

  CMoveParameters ^ mpParameters = Create();

  mpParameters->StudyUids = sStudyUid;
  mpParameters->SeriesUids = sSeriesUid;
  mpParameters->InstanceUids = sInstanceUid;
  mpParameters->DestinationAETitle = sDestinationAETitle;

  return mpParameters;
} // Create(String ^ sStudyUid, String ^ sSeriesUid, String ^ sInstanceUid)

#pragma endregion

/**
 * Gets the unmanaged pointer.
 */
std::shared_ptr<dcm::DcmMoveParameters> CMoveParameters::GetDcmMoveParameters()
{
  return *m_pDcmMoveParameters.get();
} // GetDcmMoveParameter()


/**
 * Sets the date of birth.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CMoveParameters::SetPatientDob(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmMoveParameters)->SetPatientDob(ss(mpFrom->ToString("yyyyMMdd")) + "-" + ss(mpFrom->ToString("yyyyMMdd")));
} // SetPatientDob( mpFrom, mpTo )


/**
 * Sets the time of birth.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CMoveParameters::SetPatientTob(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmMoveParameters)->SetPatientTob(ss(mpFrom->ToString("HHmmss")) + "-" + ss(mpTo->ToString("HHmmss")));
} // SetPatientTob( mpFrom, mpTo )


/**
 * Sets the study date.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CMoveParameters::SetStudyDate(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  if(mpFrom != nullptr && mpTo != nullptr)
  {
    (*m_pDcmMoveParameters)->SetStudyDate(ss(mpFrom->ToString("yyyyMMdd")) + "-" + ss(mpTo->ToString("yyyyMMdd")));
  }
  else
  {
    if(mpFrom != nullptr)
    {
      (*m_pDcmMoveParameters)->SetStudyDate(ss(mpFrom->ToString("yyyyMMdd") + "-"));
    }
    else if(mpTo != nullptr)
    {
      (*m_pDcmMoveParameters)->SetStudyDate("-" + ss(mpTo->ToString("yyyyMMdd")));
    }
  }
} // SetStudyDate( mpFrom, mpTo )


/**
 * Sets the study time.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CMoveParameters::SetStudyTime(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmMoveParameters)->SetStudyTime(ss(mpFrom->ToString("HHmmss")) + "-" +  ss(mpTo->ToString("HHmmss")));
} // SetStudyTime( mpFrom, mpTo )


/**
 * Sets the series date.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CMoveParameters::SetSeriesDate(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmMoveParameters)->SetSeriesDate(ss(mpFrom->ToString("yyyyMMdd")) + "-" + ss(mpFrom->ToString("yyyyMMdd")));
} // SetSeriesDate( mpFrom, mpTo )


/**
 * Sets the series time.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CMoveParameters::SetSeriesTime(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmMoveParameters)->SetSeriesTime(ss(mpFrom->ToString("HHmmss")) + "-" + ss(mpTo->ToString("HHmmss")));
} // SetSeriesTime( mpFrom, mpTo )


/**
 * Adds the modality to the list.
 *
 * @param mpModality  The modality to add.
 */
void CMoveParameters::AddModalities(System::String ^ mpModality)
{
  (*m_pDcmMoveParameters)->AddModalities(ss(mpModality));
} // AddModaliy( mpModality )


/**
 * Adds the instance uid to the list.
 *
 * @param mpUid  The uid to add.
 */
void CMoveParameters::AddInstanceUids(System::String ^ mpUid)
{
  (*m_pDcmMoveParameters)->AddInstanceUids(ss(mpUid));
} // AddInstanceUid( mpUid )


/**
 * Adds a study uid to the list.
 *
 * @param mpUid  The uid to add.
 */
void CMoveParameters::AddStudyUids(System::String ^ mpUid)
{
  (*m_pDcmMoveParameters)->AddStudyUids(ss(mpUid));
} // AddStudyUid( mpUid )


/**
 * Adds a series uid to the list.
 *
 * @param mpUid  The uid to add.
 */
void CMoveParameters::AddSeriesUids(System::String ^ mpUid)
{
  (*m_pDcmMoveParameters)->AddSeriesUids(ss(mpUid));
} // AddSeriesUid( mpUid )


/**
 * Adds a patient name to the list.
 *
 * @param mpName  The name of the patient.
 */
void CMoveParameters::AddPatientNames(System::String ^ mpPatientName)
{
  //dcm::DcmName name(ss(mpPatientName));
  (*m_pDcmMoveParameters)->AddPatientNames(ss(mpPatientName));
} // AddPatientName( mpPatientName )


/**
 * Clears out the patient names.
 */
void CMoveParameters::ClearPatientNames()
{
  (*m_pDcmMoveParameters)->GetPatientNames().Clear();
} // ClearPatientNames()


/**
 * Clears out the study uids.
 */
void CMoveParameters::ClearStudyUids()
{
  (*m_pDcmMoveParameters)->GetStudyUids().Clear();
} // ClearStudyUids()


/**
 * Clears out the series uids.
 */
void CMoveParameters::ClearSeriesUids()
{
  (*m_pDcmMoveParameters)->GetSeriesUids().Clear();
} // ClearSeriesUids()


/**
 * Clears out the instance uids.
 */
void CMoveParameters::ClearInstanceUids()
{
  (*m_pDcmMoveParameters)->GetInstanceUids().Clear();
} // ClearInstanceUids()


/**
 * Clears out the modalities.
 */
void CMoveParameters::ClearModalities()
{
  (*m_pDcmMoveParameters)->GetModalities().Clear();
} // ClearModalities()
