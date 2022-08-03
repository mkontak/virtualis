#include "StdAfx.h"
#include "AnonymizerPatient.h"
#include "AnonymizationInformation.h"

USING_ANMZN_NS
 
using namespace System;
using namespace System::Xml;
using namespace System::Collections::Generic;



/**
 * Constructor.
 */
AnonymizerPatient::AnonymizerPatient()
{
} // AnonymizerPatient()


/**
 * Constructor.
 *
 * @param mpInfo  The anonymization information.
 */
AnonymizerPatient::AnonymizerPatient(AnonymizationInformation ^ mpInfo) : Anonymizer(mpInfo)
{
} // AnonymizationPatient( mpInfo )


/**
 * Destructor.
 */
AnonymizerPatient::~AnonymizerPatient()
{
} // ~AnonymizerPatient()


/**
 * Anonymizes the files.
 */
void AnonymizerPatient::Anonymize(List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestinationFiles)
{
  for(int i = 0;i < mpSourceFiles->Count; ++i)
  {
    XmlDocument ^ mpDoc = gcnew XmlDocument();
    mpDoc->Load(mpSourceFiles[0]);

    mpDoc->DocumentElement->Attributes["mid"]->Value = m_mpAnonymizationInfo->CurrentElements["MedicalId"]->Replace;
    Viatronix::Enterprise::Entities::PersonName ^ mpPatientName = gcnew Viatronix::Enterprise::Entities::PersonName(m_mpAnonymizationInfo->CurrentElements["PatientName"]->Replace);
    mpDoc->DocumentElement->Attributes["fname"]->Value = mpPatientName->FirstName;
    mpDoc->DocumentElement->Attributes["lname"]->Value = mpPatientName->LastName;
    mpDoc->DocumentElement->Attributes["mname"]->Value = mpPatientName->MiddleName;
    mpDoc->DocumentElement->Attributes["sfx"]->Value = mpPatientName->Suffix;
    mpDoc->DocumentElement->Attributes["pfx"]->Value = mpPatientName->Prefix;
    mpDoc->DocumentElement->Attributes["wgt"]->Value = m_mpAnonymizationInfo->CurrentElements["PatientWeight"]->Replace;
    mpDoc->DocumentElement->Attributes["dob"]->Value = m_mpAnonymizationInfo->CurrentElements["PatientDob"]->Replace;

    mpDoc->Save(mpDestinationFiles[i]);
  }
} // Anonymize( mpSourceFiles, mpDestinationFiles )
