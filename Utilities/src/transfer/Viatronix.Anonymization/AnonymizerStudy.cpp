#include "StdAfx.h"
#include "AnonymizerStudy.h"
#include "AnonymizationInformation.h"

USING_ANMZN_NS
using namespace System;
using namespace System::Xml;
using namespace System::Collections::Generic;

/**
 * Constructor.
 */
AnonymizerStudy::AnonymizerStudy()
{
} // AnonymizerStudy()


/**
 * Constructor.
 *
 * @param mpInformation  The information necessary to perform the anonymization.
 */
AnonymizerStudy::AnonymizerStudy(AnonymizationInformation ^ mpInformation) : Anonymizer(mpInformation)
{
} // AnonymizerStudy( mpInformation )


/**
 * Destructor.
 */
AnonymizerStudy::~AnonymizerStudy()
{
} // ~AnonymizerStudy()


/**
 * Performs the anonymization.
 *
 * @param mpSourceFiles  The files to perform the anonymization on.
 * @param mpDestinationFiles  The destination paths for the files.
 */
void AnonymizerStudy::Anonymize(List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestinationFiles)
{
  for(int i = 0; i < mpSourceFiles->Count; ++i)
  {
    XmlDocument ^ mpDocument = gcnew XmlDocument();
    mpDocument->Load(mpSourceFiles[i]);

    mpDocument->DocumentElement->Attributes["name"]->Value = m_mpAnonymizationInfo->CurrentElements["StudyName"]->Replace;
    mpDocument->DocumentElement->Attributes["mid"]->Value = m_mpAnonymizationInfo->CurrentElements["MedicalId"]->Replace;
    mpDocument->DocumentElement->Attributes["uid"]->Value = m_mpAnonymizationInfo->CurrentElements["StudyUID"]->Replace;

    if (mpDocument->DocumentElement->Attributes["phys"] != nullptr)
    {
      mpDocument->DocumentElement->Attributes["phys"]->Value = m_mpAnonymizationInfo->CurrentElements["ReferringPhysician"]->Replace;
    }

    mpDocument->DocumentElement->Attributes["sex"]->Value = m_mpAnonymizationInfo->CurrentElements["StudySex"]->Replace;
    mpDocument->DocumentElement->Attributes["desc"]->Value = m_mpAnonymizationInfo->CurrentElements["StudyDescription"]->Replace;
    mpDocument->DocumentElement->Attributes["date"]->Value = m_mpAnonymizationInfo->CurrentElements["StudyDate"]->Replace;
    mpDocument->DocumentElement->Attributes["dir"]->Value = m_mpAnonymizationInfo->CurrentElements["StudyDirectoryName"]->Replace;

    mpDocument->Save(mpDestinationFiles[i]);
  }
} // Anonymize( mpSourceFiles, mpDestinationFiles )