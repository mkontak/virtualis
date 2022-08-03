// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CFindParameters.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "StdAfx.h"
#include "CFindParameters.h"

// usings
USING_DICOM_NS

/**
 * Constructor.
 */
CFindParameters::CFindParameters()
{

  m_pDcmFindParameters.reset( new std::shared_ptr<dcm::DcmFindParameters>(std::make_shared<dcm::DcmFindParameters>()) );

} // CFindParameters()


/**
 * Destructor.
 */
CFindParameters::~CFindParameters()
{
  this->!CFindParameters();
} // ~CFindParameters()


/**
 * Finalizer.
 */
CFindParameters::!CFindParameters()
{
  m_pDcmFindParameters.reset();
} // !CFindParameters()

/**
 * Creates the CMoveParameters object
 *
 * @return CMoveParameters object
 */
CFindParameters ^ CFindParameters::Create()
{
  CFindParameters ^ mpParameters = gcnew CFindParameters();

  if ( mpParameters == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate CFindParameters");

  return mpParameters;
} // Create()


/**
 * Gets the unmanaged pointer.
 */
std::shared_ptr<dcm::DcmFindParameters> CFindParameters::GetDcmFindParameters()
{
  return *m_pDcmFindParameters.get();
} // GetDcmFindParameters()


/**
 * Sets the date of birth.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CFindParameters::SetPatientDob(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmFindParameters)->SetPatientDob(ss(mpFrom->ToString("yyyyMMdd")) + "-" + ss(mpFrom->ToString("yyyyMMdd")));
} // SetPatientDob( mpFrom, mpTo )


/**
 * Sets the time of birth.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CFindParameters::SetPatientTob(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmFindParameters)->SetPatientTob(ss(mpFrom->ToString("HHmmss")) + "-" + ss(mpTo->ToString("HHmmss")));
} // SetPatientTob( mpFrom, mpTo )


/**
 * Sets the study date.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CFindParameters::SetStudyDate(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  if(mpFrom != nullptr && mpTo != nullptr)
  {
    (*m_pDcmFindParameters)->SetStudyDate(ss(mpFrom->ToString("yyyyMMdd")) + "-" + ss(mpTo->ToString("yyyyMMdd")));
  }
  else
  {
    if(mpFrom != nullptr)
    {
      (*m_pDcmFindParameters)->SetStudyDate(ss(mpFrom->ToString("yyyyMMdd") + "-"));
    }
    else if(mpTo != nullptr)
    {
      (*m_pDcmFindParameters)->SetStudyDate("-" + ss(mpTo->ToString("yyyyMMdd")));
    }
  }
} // SetStudyDate( mpFrom, mpTo )


/**
 * Sets the study time.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CFindParameters::SetStudyTime(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmFindParameters)->SetStudyTime(ss(mpFrom->ToString("HHmmss")) + "-" +  ss(mpTo->ToString("HHmmss")));
} // SetStudyTime( mpFrom, mpTo )


/**
 * Sets the series date.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CFindParameters::SetSeriesDate(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmFindParameters)->SetSeriesDate(ss(mpFrom->ToString("yyyyMMdd")) + "-" + ss(mpFrom->ToString("yyyyMMdd")));
} // SetSeriesDate( mpFrom, mpTo )


/**
 * Sets the series time.
 *
 * @param mpFrom  The from date.
 * @param mpTo    The to date.
 */
void CFindParameters::SetSeriesTime(System::DateTime ^ mpFrom, System::DateTime ^ mpTo)
{
  (*m_pDcmFindParameters)->SetSeriesTime(ss(mpFrom->ToString("HHmmss")) + "-" + ss(mpTo->ToString("HHmmss")));
} // SetSeriesTime( mpFrom, mpTo )


/**
 * Adds the modality to the list.
 *
 * @param mpModality  The modality to add.
 */
void CFindParameters::AddModalities(System::String ^ mpModality)
{
  (*m_pDcmFindParameters)->AddModalities(ss(mpModality));
} // AddModaliy( mpModality )


/**
 * Adds the instance uid to the list.
 *
 * @param mpUid  The uid to add.
 */
void CFindParameters::AddInstanceUids(System::String ^ mpUid)
{
  (*m_pDcmFindParameters)->AddInstanceUids(ss(mpUid));
} // AddInstanceUid( mpUid )


/**
 * Adds a study uid to the list.
 *
 * @param mpUid  The uid to add.
 */
void CFindParameters::AddStudyUids(System::String ^ mpUid)
{
  (*m_pDcmFindParameters)->AddStudyUids(ss(mpUid));
} // AddStudyUid( mpUid )


/**
 * Adds a series uid to the list.
 *
 * @param mpUid  The uid to add.
 */
void CFindParameters::AddSeriesUids(System::String ^ mpUid)
{
  (*m_pDcmFindParameters)->AddSeriesUids(ss(mpUid));
} // AddSeriesUid( mpUid )


/**
 * Adds a patient name to the list.
 *
 * @param mpName  The name of the patient.
 */
void CFindParameters::AddPatientNames(System::String ^ mpPatientName)
{
  //dcm::DcmName name(ss(mpPatientName));
  (*m_pDcmFindParameters)->AddPatientNames(ss(mpPatientName));
} // AddPatientName( mpPatientName )


/**
 * Clears out the patient names.
 */
void CFindParameters::ClearPatientNames()
{
  (*m_pDcmFindParameters)->GetPatientNames().Clear();
} // ClearPatientNames()


/**
 * Clears out the study uids.
 */
void CFindParameters::ClearStudyUids()
{
  (*m_pDcmFindParameters)->GetStudyUids().Clear();
} // ClearStudyUids()


/**
 * Clears out the series uids.
 */
void CFindParameters::ClearSeriesUids()
{
  (*m_pDcmFindParameters)->GetSeriesUids().Clear();
} // ClearSeriesUids()


/**
 * Clears out the instance uids.
 */
void CFindParameters::ClearInstanceUids()
{
  (*m_pDcmFindParameters)->GetInstanceUids().Clear();
} // ClearInstanceUids()


/**
 * Clears out the modalities.
 */
void CFindParameters::ClearModalities()
{
  (*m_pDcmFindParameters)->GetModalities().Clear();
} // ClearModalities()
