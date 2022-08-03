// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CMoveParameters.h
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
 * Managed wrapper class for the DcmMoveParameters.
 */
public ref class CMoveParameters
{
// construction
private:
  
  /// Constructor.
  CMoveParameters();

  CMoveParameters( const CMoveParameters ^ mpParameters ) { }

public:

  /// Destructor.
  ~CMoveParameters();

  /// Finalizer.
  !CMoveParameters();

  #pragma region methods

// methods
public:


  /// Creates the CMoveParameter object
  static CMoveParameters ^ Create();

  /// Creates and initializes the CMoveParameters for patient search
  static CMoveParameters ^ Create(String ^ sDestinationAETitle, String ^ sMedicalId, String ^ sPatientName);

  /// Creates and initializes the CMoveParameters for study/series/instance searches
  static CMoveParameters ^ Create(String ^ sDestinationAETitle, String ^ sStudyUid, String ^ sSeriesUId, String ^ sInstanceUid);


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
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetLevel().c_str()); }
    void set(String ^ type) { (*m_pDcmMoveParameters)->SetLevel(ss(type->ToUpper())); }
  } // Type


  /// Gets the patient name.
  property String ^ PatientNames
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetPatientNames().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetPatientNames(ss(msValue)); }
  } // PatientName


  /// Gets or sets the patient's date of birth.
  property String ^ PatientDob
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetPatientDob().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetPatientDob(ss(msValue)); }
  } // PatientDob


  /// Gets or sets the patient's time of birth.
  property String ^ PatientTob
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetPatientTob().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetPatientTob(ss(msValue)); }
  } // PatientTob


  /// Gets the list of modalities.
  property String ^ Modalities
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetModalities().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetModalities(ss(msValue)); }
  } // Modalities


  /// Gets the study date.
  property String ^ StudyDate
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetStudyDate().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetStudyDate(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // StudyDate


  /// Gets the study time.
  property String ^ StudyTime
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetStudyTime().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetStudyTime(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // StudyTime


  /// Gets the series date.
  property String ^ SeriesDate
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetSeriesDate().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetSeriesDate(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // SeriesDate


  /// Gets the series time.
  property String ^ SeriesTime
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetSeriesTime().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetSeriesTime(ss(String::Format("{0}-{1}",msValue,msValue))); }
  } // SeriesTime


  /// Gets the list of instance Uids.
  property String ^ InstanceUids
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetInstanceUids().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetInstanceUids(ss(msValue)); }
  } // InstanceUids


  /// Gets the list of study Uids.
  property String ^ StudyUids
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetStudyUids().ToString().c_str()); }
    void set(String ^ msValue) { AddStudyUids(msValue); }
  } // StudyUids


  /// Gets the list of series Uids.
  property String ^ SeriesUids
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetSeriesUids().ToString().c_str()); }
    void set(String ^ msValue) { (*m_pDcmMoveParameters)->SetSeriesUids(ss(msValue)); }
  } // SeriesUids


  /// Gets or sets the medical id.
  property String ^ MedicalId
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetMedicalId().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmMoveParameters)->SetMedicalId(ss(mpValue)); }
  } // MedicalId


  /// Gets or sets the physician.
  property String ^ Physician
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetPhysician().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmMoveParameters)->SetPhysician(ss(mpValue)); }
  } // Physician


  /// Gets or sets the study id.
  property String ^ StudyId
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetStudyId().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmMoveParameters)->SetStudyId(ss(mpValue)); }
  } // StudyId

  /// Gets or sets the log file 
  property String ^ LogFile
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetLogFile().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmMoveParameters)->SetLogFile(ss(mpValue)); }
  } // LogFile

    /// Gets or sets the destination ae title.
  property String ^ DestinationAETitle
  {
    String ^ get() { return gcnew String((*m_pDcmMoveParameters)->GetDestinationAEtitle().c_str()); }
    void set(String ^ mpValue) { (*m_pDcmMoveParameters)->SetDestinationAEtitle(ss(mpValue)); }
  } // DestinationAETitle



  /// Gets the unmanaged pointer.
  std::shared_ptr<dcm::DcmMoveParameters> GetDcmMoveParameters();

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
  scoped_ptr< std::shared_ptr<dcm::DcmMoveParameters> > m_pDcmMoveParameters;


}; // class CMoveParameters

CLOSE_DICOM_NS