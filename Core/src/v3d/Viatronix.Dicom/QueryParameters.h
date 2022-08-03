#pragma once


using namespace System;
using namespace System::Text;

OPEN_DICOM_NS

/**
 * 
 */
public ref class QueryParameters 
{
public:


  /// constructor
  QueryParameters();


  /// Creates the query parameters using the find parameters from the callback
  static QueryParameters ^ Create(dcm::DcmFindParameters * pParameters);

  /// Creates the query parameters using the move parameters from the callback
  static QueryParameters ^ Create(dcm::DcmMoveParameters * pParameters);

  /// Generates a query xml
  String ^ ToXml();

private:

  /// Takes a source string (dicom) and generates a sql compliant string
  inline static String ^ ToSqlString(String ^ sSource)
  { return sSource->Replace('/','|')->Replace('-','|')->Replace('*','%'); }

  inline void  AppendAttribute(StringBuilder ^ mpXml, String ^ msName, String ^ msValue)
  { if ( ! String::IsNullOrEmpty(msValue) ) mpXml->Append(String::Format(" {0}=\"{1}\"", msName, msValue)); }
  
public:

  /// Gets the query level
  property String ^ Level
  {
    String ^ get() { return m_msLevel; }
  } // Level

  /// Gets the list of patient names
  property String ^ Patients
  {
    String ^ get() { return m_msPatients; }
  } // Patients

  /// Gets the list of study uids
  property String ^ Studies
  {
    String ^ get() { return m_msStudies; }
  } // Studies

  /// Gets the list of series uids
  property String ^ Series
  {
    String ^ get() { return m_msSeries; }
  } // Series

  /// Gets the list of instance uids
  property String ^ Instances
  {
    String ^ get() { return m_msInstances; }
  } // Instances

  /// Gets the medical id
  property String ^ MedicalId
  {
    String ^ get() { return m_msMedicalId; }
  } // MedicalId

  /// Gets the physician
  property String ^ Physician
  {
    String ^ get() { return m_msPhysician; }
  } // Physician

  /// Gets the list modalities
  property String ^ Modalities
  {
    String ^ get() { return m_msModalities; }
  } // Modalities

  /// Gets the from to study dates
  property String ^ StudyDates
  {
    String ^ get() { return m_msStudyDates; }
  } // StudyDates

  /// Gets the from to study times
  property String ^ StudyTimes
  {
    String ^ get() { return m_msStudyTimes; }
  } // StudyTimes
  

  /// Gets the from to series dates
  property String ^ SeriesDates
  {
    String ^ get() { return m_msSeriesDates; }
  } // SeriesDates

  /// Gets the from to series times
  property String ^ SeriesTimes
  {
    String ^ get() { return m_msSeriesTimes; }
  } // SeriesTimes


  /// Gets the from to patient dob
  property String ^ PatientDobs
  {
    String ^ get() { return m_msPatientDobs; }
  } // PatientDobs

  /// Gets the from to patient tob
  property String ^ PatientTobs
  {
    String ^ get() { return m_msPatientTobs; }
  } // PatientTobs

private:
  
  /// Query Level
  String ^ m_msLevel;

  /// Patient list to query for
  String ^ m_msPatients;

  /// Study uid list to query for
  String ^ m_msStudies;

  /// Series uid list to query for
  String ^ m_msSeries;

  /// Instance uid list
  String ^ m_msInstances;

  /// Modality list
  String ^ m_msModalities;

  /// Medical id
  String ^ m_msMedicalId;

  /// Physician 
  String ^ m_msPhysician;

  /// From/To study date
  String ^  m_msStudyDates;

  /// From/To study time
  String ^ m_msStudyTimes;

  /// From/To series date
  String ^ m_msSeriesDates;

  /// From/To series time
  String ^ m_msSeriesTimes;

  /// From/To patient dobs
  String ^ m_msPatientDobs;

  /// From/To patient tobs
  String ^ m_msPatientTobs;

};  // class QueryParameters


CLOSE_DICOM_NS
