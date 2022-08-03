#include "stdafx.h"
#include "AnonymizerUtlities.h"

#include "vxBlockVolume.h"
#include "VarMap.h"

USING_ANMZN_NS

using namespace System;
using namespace System::Xml;
using namespace Viatronix::Logging;



/**
 ^ Creates a Core Study object from the element list
 ^ 
 ^ @param pElementList    Element list used to extract the patient information from
 *
 ^ @return Returns a newly created patient object from the element list
 */
Viatronix::Enterprise::Entities::Study ^ AnonymizerUtilities::CreateStudy(Viatronix::Dicom::ElementCollection ^ pElementList)
{
  Viatronix::Enterprise::Entities::Study ^ mpStudy(gcnew Viatronix::Enterprise::Entities::Study());

  try
  {
    mpStudy->Dob = Viatronix::Dicom::Utility::ToDateTime(pElementList->PatientDob,"");
  }
  catch ( Exception ^ )
  {
    Log::Warning("No patient DOB found attempting to use the age to caculate the patients DOB", "AnonymizerUtilitie", "CreateStudy");
    //mpStudy->Dob = Dicom::Utilities::ToDateTime(pElementList->StudyAge);
  }

  mpStudy->Gender               = Viatronix::Enterprise::Entities::Study::ConvertToGender(pElementList->PatientSex);
  mpStudy->Weight               = pElementList->PatientWeight;
  mpStudy->MedicalId            = pElementList->PatientId;
  mpStudy->Name                 = gcnew Viatronix::Enterprise::Entities::PersonName(pElementList->PatientName);
  mpStudy->Physician            = pElementList->ReferringPhysician; //gcnew Viatronix::Enterprise::PersonName(pElementList->ReferringPhysician);
  mpStudy->AccessionNumber      = pElementList->AccessionNumber;
  mpStudy->Description          = pElementList->StudyDescription;
  mpStudy->Date                 = pElementList->StudyDateTime;
  mpStudy->Id                   = pElementList->StudyId;
  mpStudy->Uid                  = pElementList->StudyInstanceUid;

  return mpStudy;
} // CreateStudy()

/**
 ^ Creates a Core Series object from the element list
 ^ 
 ^ @param pElementList    Element list used to extract the series information from
 *
 ^ @return Returns a newly created series object from the element list
 */
Viatronix::Enterprise::Entities::Series ^ AnonymizerUtilities::CreateSeries(Viatronix::Dicom::ElementCollection ^ pElementList)
{
  Log::Debug("Attempting to create series object from element list", "AnonymizerUtilitie", "CreateSerie");

  Viatronix::Enterprise::Entities::Series ^ pSeries(gcnew Viatronix::Enterprise::Entities::Series());

  try
  {
    pSeries->Date  = pElementList->SeriesDateTime;
  }
  catch ( Exception ^ )
  {
    Log::Warning("Failed to set the series date, using the scan date", "AnonymizerUtilitie", "CreateSerie");
    //pSeries->Date = GetScanDateTime(pElementList);
  }

 
  pSeries->Description        = pElementList->SeriesDescription;
  pSeries->Number             = pElementList->SeriesNumber;
  pSeries->Modality           = pElementList->Modality;

  // TODO (do we need)
  //pSeries->StudyOrientation = pElementList->StudyOrientation;
  pSeries->Uid                = pElementList->SeriesInstanceUid;

  return pSeries;

} // CreateSeries()


/**
 ^ Creates the dataset from eth volume file
 */
Viatronix::Dicom::ElementCollection ^ AnonymizerUtilities::CreateElementList(String ^ msVolumeFile)
{

	try
	{
	
    String ^ msVersion(String::Empty);
    String ^ msHeader(String::Empty);

    vxVolumeHeader * pHeader = nullptr;

    VarMap * pVarMap = nullptr;
    VarMap * pVarHdr = nullptr;

    std::pair<float,float> progressBar(0.0f,100.0f);
    pVarMap = new VarMap();

		pHeader = new vxVolumeHeader();
		pHeader->Read( ss(msVolumeFile) );
		pVarMap->CreateMap( pHeader->GetVarMapsAsString());

    std::string sVarHeader;
    if ( pVarMap->GetHdrVar("varHeader", sVarHeader) )
    {

      pVarHdr = new VarMap(sVarHeader);
      std::string sValue;

      // Version
      if ( pVarHdr->GetHdrVar( "version", sValue ) )
      {
        msVersion = gcnew String(sValue.c_str());
      }
			
      // Dicom Header
      if ( pVarHdr->GetHdrVar("dicomHeader", sValue) )
      {
        msHeader = gcnew String(sValue.c_str());
      } // END ... Dicom Header

      // ==========================================
      // Version 5 stores the DICOM HEADER as XML
      // =========================================
      if ( msVersion == "5" )
      {
        XmlDocument ^ mpDoc = gcnew XmlDocument();
        mpDoc->LoadXml(msHeader);

        return Viatronix::Dicom::ElementCollection::Create(mpDoc->DocumentElement);
      }
      else
      {
        return Viatronix::Dicom::ElementCollection::Create(msHeader, Viatronix::Dicom::ParameterType::HEADER);
      }

      
 
		}
		else
		{
			// Change the extension on the volume file from .vol to .dcm to check for a dicom file 
			String ^ sDicomFile = System::IO::Path::ChangeExtension(msVolumeFile, ".dcm");

			// Make sure the dicom file exists
			if ( ! System::IO::File::Exists(sDicomFile) )
				throw gcnew System::IO::FileNotFoundException(L"No Dicom file found and the volume file contains no dicomHeader field, cannot initialize element list");

			// Initialize the element list for the dicom file
			return Viatronix::Dicom::ElementCollection::Create(sDicomFile, Viatronix::Dicom::ParameterType::FILE );
		}
	}
	catch ( Exception ^ )
	{
		throw;
	}


} // CreateElementList()
