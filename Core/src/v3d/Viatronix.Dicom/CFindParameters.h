// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CFindParameters.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "Enums.h"
#include "DcmEnums.h"
#include "scoped_ptr.h"

// using
using namespace System;


OPEN_DICOM_NS

/**
 * Managed wrapper class for the DcmFindParameters.
 */
public ref class CFindParameters
{
// construction
private:
  
  /// Constructor.
  CFindParameters();

  CFindParameters( const CFindParameters ^ mpParameters) { }

public:

  /// Destructor.
  ~CFindParameters();

  /// Finalizer.
  !CFindParameters();

#pragma region methods
  
// methods
public:

  /// Creates the CFindParameters object
  static CFindParameters ^ Create();

  /// Adds a modality to the list.
  void AddModalities(String ^ mpModality);

  /// Adds a Uid to the list.
  void AddInstanceUids(String ^ mpUid);

  /// Adds a study Uid to the list.
  void AddStudyUids(String ^ mpUid);
  
  /// Adds a series Uid to the list.
  void AddSeriesUids(String ^ mpUid);

  /// Adds a patient name to the list.
  void AddPatientNames(String ^ mpPatientName);
  
  /// Clears out the patient names.
  void ClearPatientNames();

  /// Clears out the study Uids.
  void ClearStudyUids();

  /// Clears out the series Uids.
  void ClearSeriesUids();

  /// Clears out the instance Uids.
  void ClearInstanceUids();

  /// Clears out the modalities.
  void ClearModalities();

#pragma endregion

#pragma region properties

public:
  
  /// Sets the type of results the user wants.
  property String ^ Level
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetLevel().c_str()); }
    void set(String ^ type) { (*m_pDcmFindParameters)->SetLevel(ss(type->ToUpper())); }
  } // Type


  /// Gets the patient name.
  property String ^ PatientNames
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetPatientNames().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetPatientNames(ss(msValue)); }
  } // PatientName


  /// Gets or sets the patient's date of birth.
  property String ^ PatientDob
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetPatientDob().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetPatientDob(ss(msValue)); }
  } // PatientDob


  /// Gets or sets the patient's time of birth.
  property String ^ PatientTob
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetPatientTob().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetPatientTob(ss(msValue)); }
  } // PatientTob


  /// Gets the list of modalities.
  property String ^ Modalities
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetModalities().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetModalities(ss(msValue)); }
  } // Modalities


  /// Gets the study date.
  property String ^ StudyDate
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetStudyDate().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetStudyDate(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // StudyDate


  /// Gets the study time.
  property String ^ StudyTime
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetStudyTime().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetStudyTime(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // StudyTime


  /// Gets the series date.
  property String ^ SeriesDate
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetSeriesDate().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetSeriesDate(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // SeriesDate


  /// Gets the series time.
  property String ^ SeriesTime
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetSeriesTime().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetSeriesTime(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // SeriesTime


  /// Gets the list of instance Uids.
  property String ^ InstanceUids
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetInstanceUids().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetInstanceUids(ss(msValue)); }
  } // InstanceUids


  /// Gets the list of study Uids.
  property String ^ StudyUids
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetStudyUids().ToString().c_str()); }
    void set(String ^ msValue) { AddStudyUids(msValue); }
  } // StudyUids


  /// Gets the list of series Uids.
  property String ^ SeriesUids
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetSeriesUids().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmFindParameters)->SetSeriesUids(ss(msValue)); }
  } // SeriesUids


  /// Gets or sets the medical id.
  property String ^ MedicalId
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetMedicalId().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmFindParameters)->SetMedicalId(ss(mpValue)); }
  } // MedicalId


  /// Gets or sets the physician.
  property String ^ Physician
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetPhysician().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmFindParameters)->SetPhysician(ss(mpValue)); }
  } // Physician


  /// Gets or sets the study id.
  property String ^ StudyId
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetStudyId().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmFindParameters)->SetStudyId(ss(mpValue)); }
  } // StudyId

  /// Gets or sets the log file 
  property String ^ LogFile
  {
    String ^ get() { return gcnew String((*m_pDcmFindParameters)->GetLogFile().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmFindParameters)->SetLogFile(ss(mpValue)); }
  } // LogFile


  /// Gets the unmanaged pointer.
  std::shared_ptr<dcm::DcmFindParameters> GetDcmFindParameters();

  /// Sets the patient's date of birth.
  void SetPatientDob(System::DateTime ^ mpFrom, System::DateTime ^ mpTo);

  /// Sets the patient's time of birth.
  void SetPatientTob(System::DateTime ^ mpFrom, System::DateTime ^ mpTo);

  /// Sets the study date.
  void SetStudyDate(System::DateTime ^ mpFrom, System::DateTime ^ mpTo);

  /// Sets the study time.
  void SetStudyTime(System::DateTime ^ mpFrom, System::DateTime ^ mpTo);

  /// Sets the series date.
  void SetSeriesDate(System::DateTime ^ mpFrom, System::DateTime ^ mpTo);

  /// Sets the series time.
  void SetSeriesTime(System::DateTime ^ mpFrom, System::DateTime ^ mpTo);

#pragma endregion



// fields
private:
 
  /// Our internal instance to wrap.
  scoped_ptr< std::shared_ptr<dcm::DcmFindParameters> > m_pDcmFindParameters;


}; // class CFindParameters

CLOSE_DICOM_NS