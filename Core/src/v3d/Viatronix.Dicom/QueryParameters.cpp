#include "Stdafx.h"
#include "QueryParameters.h"

using namespace System::Text;

USING_DICOM_NS

/**
 * Constructor
 */
QueryParameters::QueryParameters() :
m_msLevel(String::Empty),
m_msPhysician(String::Empty),
m_msMedicalId(String::Empty),
m_msPatients(String::Empty),
m_msStudies(String::Empty),
m_msSeries(String::Empty),
m_msInstances(String::Empty),
m_msModalities(String::Empty),
m_msStudyDates(String::Empty),
m_msStudyTimes(String::Empty),
m_msSeriesDates(String::Empty),
m_msSeriesTimes(String::Empty),
m_msPatientDobs(String::Empty),
m_msPatientTobs(String::Empty)
{

}


/**
 * Creates the query parameter from the find parameters of the callback
 *
 * @param pParameters
 *
 * @return QueryParameters object
 */
QueryParameters ^ QueryParameters::Create(dcm::DcmFindParameters * pParameters)
{
  QueryParameters ^ pQueryParameters(gcnew QueryParameters());


 

  /// Fills the patient list
  pQueryParameters->m_msPatients = ToSqlString(gcnew String(pParameters->GetPatientNames().ToString().c_str()));

 
  /// Fills the study uid list
  pQueryParameters->m_msStudies = ToSqlString(gcnew String(pParameters->GetStudyUids().ToString().c_str()));

  /// Fills the series uid list
  pQueryParameters->m_msSeries = ToSqlString(gcnew String(pParameters->GetSeriesUids().ToString().c_str()));

  /// Fills the modality list
  pQueryParameters->m_msModalities = ToSqlString(gcnew String(pParameters->GetModalities().ToString().c_str()));

  /// Fills the instance uid list
  pQueryParameters->m_msInstances = ToSqlString(gcnew String(pParameters->GetInstanceUids().ToString().c_str())); 

  pQueryParameters->m_msStudyDates = ToSqlString(gcnew String(pParameters->GetStudyDate().ToString().c_str()));
  pQueryParameters->m_msStudyTimes = ToSqlString(gcnew String(pParameters->GetStudyTime().ToString().c_str()));

  pQueryParameters->m_msSeriesDates = ToSqlString(gcnew String(pParameters->GetSeriesDate().ToString().c_str()));
  pQueryParameters->m_msSeriesTimes = ToSqlString(gcnew String(pParameters->GetSeriesTime().ToString().c_str()));

  pQueryParameters->m_msPatientDobs = ToSqlString(gcnew String(pParameters->GetPatientDob().ToString().c_str()));
  pQueryParameters->m_msPatientTobs = ToSqlString(gcnew String(pParameters->GetPatientTob().ToString().c_str()));


  pQueryParameters->m_msMedicalId   = gcnew String(pParameters->GetMedicalId().c_str());
  pQueryParameters->m_msPhysician   = gcnew String(pParameters->GetPhysician().c_str());
  pQueryParameters->m_msLevel       = gcnew String(pParameters->GetLevel().c_str());

  return pQueryParameters;

} // Create(dcm::DcmFindParameters * pParameters)


/**
 * Creates the query parameter from the find parameters of the callback
 *
 * @param pParameters
 *
 * @return QueryParameters object
 */
QueryParameters ^ QueryParameters::Create(dcm::DcmMoveParameters * pParameters)
{
  QueryParameters ^ pQueryParameters(gcnew QueryParameters());

 

  /// Fills the patient list
  pQueryParameters->m_msPatients = ToSqlString(gcnew String(pParameters->GetPatientNames().ToString().c_str()));

 
  /// Fills the study uid list
  pQueryParameters->m_msStudies = ToSqlString(gcnew String(pParameters->GetStudyUids().ToString().c_str()));

  /// Fills the series uid list
  pQueryParameters->m_msSeries = ToSqlString(gcnew String(pParameters->GetSeriesUids().ToString().c_str()));

  /// Fills the modality list
  pQueryParameters->m_msModalities = ToSqlString(gcnew String(pParameters->GetModalities().ToString().c_str()));

  /// Fills the instance uid list
  pQueryParameters->m_msInstances = ToSqlString(gcnew String(pParameters->GetInstanceUids().ToString().c_str())); 

  pQueryParameters->m_msStudyDates = ToSqlString(gcnew String(pParameters->GetStudyDate().ToString().c_str()));
  pQueryParameters->m_msStudyTimes = ToSqlString(gcnew String(pParameters->GetStudyTime().ToString().c_str()));

  pQueryParameters->m_msSeriesDates = ToSqlString(gcnew String(pParameters->GetSeriesDate().ToString().c_str()));
  pQueryParameters->m_msSeriesTimes = ToSqlString(gcnew String(pParameters->GetSeriesTime().ToString().c_str()));

  pQueryParameters->m_msPatientDobs = ToSqlString(gcnew String(pParameters->GetPatientDob().ToString().c_str()));
  pQueryParameters->m_msPatientTobs = ToSqlString(gcnew String(pParameters->GetPatientTob().ToString().c_str()));


  pQueryParameters->m_msMedicalId   = gcnew String(pParameters->GetMedicalId().c_str());
  pQueryParameters->m_msPhysician   = gcnew String(pParameters->GetPhysician().c_str());
  pQueryParameters->m_msLevel       = gcnew String(pParameters->GetLevel().c_str());

  return pQueryParameters;

} // Create(dcm::DcmMoveParameters * pParameters)



/**
 * Generates a query xml
 */
String ^ QueryParameters::ToXml()
{
  StringBuilder ^ mpXml(gcnew StringBuilder("<query "));

  AppendAttribute(mpXml, "level", m_msLevel);
  AppendAttribute(mpXml, "patientName", m_msPatients);
  AppendAttribute(mpXml, "studyUid", m_msStudies);
  AppendAttribute(mpXml, "seriesUid", m_msSeries);
  AppendAttribute(mpXml, "medicalId", m_msMedicalId);
  AppendAttribute(mpXml, "physician", m_msPhysician);
  AppendAttribute(mpXml, "modality", m_msModalities);
  AppendAttribute(mpXml, "studyDate", m_msStudyDates);
  AppendAttribute(mpXml, "studyTime", m_msStudyTimes);
  AppendAttribute(mpXml, "seriesDate", m_msSeriesDates);
  AppendAttribute(mpXml, "seriesTime", m_msSeriesTimes);
  AppendAttribute(mpXml, "dob", m_msPatientDobs);
  AppendAttribute(mpXml, "tob", m_msPatientTobs);

  mpXml->Append(" />");

  return mpXml->ToString();


} // ToXml()