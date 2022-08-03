// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmQueryRetrieveParameters.h
//
// Description: 
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmEnums.h"
#include "DcmName.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmRange.h"
#include "DcmList.h"

#pragma warning(push)
#pragma warning(disable:4251)



namespace dcm
{

/// Forward declarations
class DCM_DLL DcmImage;

/**
 * Implements a class that wraps the query/retrieval parameters used in the C-FIND, C-MOVE and C-GET
 */
class DCM_DLL DcmQueryRetrieveParameters
{
public:

  /// Constructor (Default)
  DcmQueryRetrieveParameters();

  /// Copy Constructor
  DcmQueryRetrieveParameters(const DcmQueryRetrieveParameters & parameters);

  /// Destructor
  virtual ~DcmQueryRetrieveParameters();

  /// Creates the DcmQueryRetrieveParameters from the DcmImage
  static std::shared_ptr<DcmQueryRetrieveParameters> Create(std::shared_ptr<DcmImage> pDcmImage);

  /// Adds modalities
  virtual void AddModalities(const std::string & sModality)
  { m_modalities.Add(sModality); }

  /// Adds instance uid to the list
  virtual void AddInstanceUids(const std::string & sUid)
  { m_instances.Add(sUid); }

  /// Adds study uid to the list
  virtual void AddStudyUids(const std::string & sUid)
  { m_studyUids.Add(sUid); }

  /// Adds series uid to the list
  virtual void AddSeriesUids(const std::string & sUid)
  { m_seriesUids.Add(sUid); }

  /// Adds patient name to the list
  virtual void AddPatientName(const DcmName & name)
  { m_patients.Add(name); }

  /// Adds the patients names in the delimited list
  virtual void AddPatientNames(const std::string & sName) 
  { m_patients.Add(sName); }

  /// Validates the object 
  virtual void Validate() { };

  virtual void ClearPatientNames()
  { m_patients.Clear(); }
  
protected:

  /// Initializes the objects state from the DcmImage 
  virtual void Initialize(std::shared_ptr<DcmImage> pDcmImage);

public:

  /// Gets the DCM image for the  set parameters
  virtual std::shared_ptr<DcmImage> GetDcmImage();


  /// Gets the affected sop class uid for this move
  const std::string &  GetAffectedSopInstanceUid() const
  { return m_sAffectedSopInstanceUid; }

  /// Sets the Affected Sop Uid
  void SetAffectedSopInstanceUid(const std::string & sUid)
  { m_sAffectedSopInstanceUid = sUid; }

  /// Gets the Query Retriveal level
  const std::string & GetLevel()
  { return m_sLevel; }

  /// Sets the Query Retriveal level
  virtual void SetLevel(const std::string & sLevel);


  /// Gets the patient name
  DcmList<DcmName> & GetPatientNames()
  { return m_patients; }

  /// Sets the patient name
  void SetPatientNames(const std::string & sNames)
  { m_patients.Assign(sNames); }

  /// Gets the patient dob
  DcmRange<dcm::DcmDate> & GetPatientDob()
  { return m_patientDob; }

  /// Set the patient dob
  void SetPatientDob(const std::string & sDob)
  { m_patientDob.Set(sDob); }

  /// Gets the patient tob
  DcmRange<dcm::DcmTime> & GetPatientTob()
  { return m_patientTob; }

  /// Set the patient tob
  void SetPatientTob(const std::string & sTob)
  { m_patientTob.Set(sTob); }

   /// Gets the modalities
  DcmList<std::string> & GetModalities()
  { return m_modalities; }

  /// Set the modalities
  void SetModalities(const std::string & sModalities)
  { m_modalities.Assign(sModalities); }

  /// Gets the study date
  DcmRange<dcm::DcmDate> & GetStudyDate()
  { return m_studyDate; }

  /// Sets the study dates (FROM-TO)
  void SetStudyDate(const std::string & sDate)
  { m_studyDate.Set(sDate); }

  /// Gets the study time
  DcmRange<dcm::DcmTime> & GetStudyTime()
  { return m_studyTime; }

  /// Sets the study time (FROM-TO)
  void SetStudyTime(const std::string & sTime)
  { m_studyTime.Set(sTime); }

  /// Gets the series date
  DcmRange<dcm::DcmDate> & GetSeriesDate()
  { return m_seriesDate; }

  /// Sets the series dates (FROM-TO)
  void SetSeriesDate(const std::string & sDate)
  { m_seriesDate.Set(sDate); }

  /// Gets the series time
  DcmRange<dcm::DcmTime> & GetSeriesTime()
  { return m_seriesTime; }

  /// Sets the series times (FROM-TO)
  void SetSeriesTime(const std::string & sTime)
  { m_seriesTime.Set(sTime); }

   /// Gets the instances uids 
  DcmList<std::string> & GetInstanceUids()
  { return m_instances; }

  /// Sets the instance uids
  void SetInstanceUids(const std::string & sUids)
  { if ( ! sUids.empty() ) m_instances.Assign(sUids); }

  /// Gets the study uids 
  DcmList<std::string> & GetStudyUids()
  { return m_studyUids; }

  /// Sets the study uids
  void SetStudyUids(const std::string & sUids)
  { if ( ! sUids.empty() ) m_studyUids.Assign(sUids); }

  /// GEts the series uid
  DcmList<std::string> & GetSeriesUids()
  { return m_seriesUids; }

  /// Sets the series uids
  void SetSeriesUids(const std::string & sUids)
  { if ( ! sUids.empty() ) m_seriesUids.Assign(sUids); }

  /// Gets the patients medical id
  const std::string & GetMedicalId() const
  { return m_sMedicalId; }

  /// Sets the medical id
  void SetMedicalId(const std::string & sMedicalId)
  { m_sMedicalId = sMedicalId; }

  
  /// Gets the physician
  const std::string & GetPhysician() const
  { return m_sPhysician; }

  /// Sets the physician
  void SetPhysician(const std::string & sPhysician)
  { m_sPhysician = sPhysician; }


  /// Gets the study id 
  const std::string & GetStudyId() const
  { return m_sStudyId; }

  /// Sets the study id
  void SetStudyId(const std::string & sStudyId)
  { m_sStudyId = sStudyId; }

  /// Gets the series number
  const std::string & GetSeriesNumber() const
  { return m_sSeriesNumber; }

  /// Sets the series number
  void SetSeriesNumber(const std::string & sSeriesNumber)
  { m_sSeriesNumber = sSeriesNumber; }

  /// Gets the log file
  const std::string & GetLogFile() const
  { return m_sLogFile; }

  /// Sets the log file
  void SetLogFile(const std::string & sLogFile)
  { m_sLogFile = sLogFile; }

protected:

   /// Affected Sop Uid
  std::string m_sAffectedSopInstanceUid;


  /// Patient name list
  DcmList<DcmName> m_patients;

  /// Patient DOB range
  DcmRange<dcm::DcmDate> m_patientDob;

  /// Patient TOB range
  DcmRange<dcm::DcmTime> m_patientTob;

  /// Patients medical id
  std::string m_sMedicalId;

  /// Physician
  std::string m_sPhysician;

  /// Study id
  std::string m_sStudyId;

  /// Series number
  std::string m_sSeriesNumber;

  /// Instance Uids
  DcmList<std::string> m_instances;

  /// Modalities
  DcmList<std::string>  m_modalities;

  /// Study uid list
  DcmList<std::string>  m_studyUids;

  /// Study Date range
  DcmRange<dcm::DcmDate> m_studyDate;

  /// Study time range
  DcmRange<dcm::DcmTime> m_studyTime;

  /// Series uid list
  DcmList<std::string>  m_seriesUids;

  /// Series Date range
  DcmRange<dcm::DcmDate> m_seriesDate;

  /// Series time range
  DcmRange<dcm::DcmTime> m_seriesTime;
  
  /// Level of query/retrieval
  std::string m_sLevel;

  /// Log file user to log request/dataset/response for the current operation
  std::string m_sLogFile;


};  // class DcmQueryRetrieveParameters

};  // namespace dcm

#pragma warning(pop)