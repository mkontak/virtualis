// includes
#include "StdAfx.h"
#include "AnonymizerSeries.h"
#include "AnonymizationInformation.h"

// namespaces
USING_ANMZN_NS
using namespace System;
using namespace System::Xml;
using namespace System::Collections::Generic;

/**
 * Constructor.
 */
AnonymizerSeries::AnonymizerSeries()
{
} // AnonymizerSeries()


/**
 * Constructor.
 *
 * @param mpInformation  The information necessary to perform the anonymization.
 */
AnonymizerSeries::AnonymizerSeries(AnonymizationInformation ^ mpInformation) : Anonymizer(mpInformation)
{
} // AnonymizerSeries( mpInformation )


/**
 * Destructor.
 */
AnonymizerSeries::~AnonymizerSeries()
{
} // ~AnonymizerSeries()


/**
 * Performs the anonymization.
 *
 * @param mpSourceFiles  The source files to perform the anonymization on.
 * @param mpDestinationFiles  The destination paths for the anonymized files.
 */
void AnonymizerSeries::Anonymize(List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestinationFiles)
{
  for(int i = 0; i < mpSourceFiles->Count; ++i)
  {
    XmlDocument ^ mpDocument = gcnew XmlDocument();
    mpDocument->Load(mpSourceFiles[i]);

    mpDocument->DocumentElement->Attributes["suid"]->Value = m_mpAnonymizationInfo->CurrentElements["StudyUID"]->Replace;
    
    // Iterate through the dictionary to find the new value for the series uid.
    for each(KeyValuePair<String ^, SearchAndReplace ^> ^ mpKeyPair in m_mpAnonymizationInfo->CurrentElements)
    {
      if(mpKeyPair->Value->Search == mpDocument->DocumentElement->Attributes["uid"]->Value)
      {
        mpDocument->DocumentElement->Attributes["uid"]->Value = mpKeyPair->Value->Replace;
      }
    }
    
    String ^ mpValue = String::Empty;

    // ----------------------------------------------------
    // Modify xml fields.  Don't touch the Directory Field.
    // ====================================================

    mpDocument->DocumentElement->Attributes["desc"]->Value = m_mpAnonymizationInfo->AnonymizationElementList->GetElementValue(0x0008, 0x103e, mpValue) ? mpValue : "Anonymized";
    mpDocument->DocumentElement->Attributes["date"]->Value = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");
    mpDocument->DocumentElement->Attributes["on"]->Value = DateTime::Now.ToString("yyyy-MM-ddTHH:mm:ss");

    if (mpDocument->DocumentElement->SelectSingleNode("references") != nullptr)
    {
      for each(XmlNode ^ mpReference in mpDocument->DocumentElement->SelectSingleNode("references")->SelectNodes("series"))
      {
        // Iterate through the dictionary to find the new value for the referenced series uid.
        for each(KeyValuePair<String ^, SearchAndReplace ^> ^ mpKeyPair in m_mpAnonymizationInfo->CurrentElements)
        {
          if(mpKeyPair->Value->Search == mpReference->Attributes["uid"]->Value)
          {
            mpReference->Attributes["uid"]->Value = mpKeyPair->Value->Replace;
          }
        }
      }
    }

    mpDocument->Save(mpDestinationFiles[i]);
  }
} // Anonymize( mpSourceFiles, mpDestinationFiles )
