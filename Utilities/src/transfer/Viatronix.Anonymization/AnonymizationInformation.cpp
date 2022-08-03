// $Id: AnonymizationInformation.cpp,v 1.11.2.5 2011/05/18 01:56:57 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationInformation.cpp,v 1.11.2.5 2011/05/18 01:56:57 mkontak Exp $
//
// Copyright© (2004), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
// Co-Author: [Romy Sreedharan] [ romy@viatronix.com]


// include
#include "stdafx.h"
#include "AnonymizationInformation.h"
#include "SearchAndReplace.h"
#include "Exceptions.h"
#include "AnonymizerUtlities.h"

// namespace
using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace System::Collections::Generic;
using namespace Viatronix::Logging;
using namespace Viatronix::Utilities;
using namespace Viatronix::Utilities::IO;
using namespace Viatronix::Utilities::Registry;
using namespace Viatronix::Dicom;

USING_ANMZN_NS

/** 
 ^ Constructor
 *
 ^ @param sSourcePath      Path to anonymize
 */
AnonymizationInformation::AnonymizationInformation( String ^ sSourcePath ) :
/*********************************************************************************************/
m_sSourcePath(sSourcePath),
m_sDestinationPath(String::Empty),
m_sFilePattern("*"),
m_pOldStudy(gcnew Viatronix::Enterprise::Entities::Study()),
m_pNewStudy(gcnew Viatronix::Enterprise::Entities::Study()),
m_pCurrentElementValues(gcnew Dictionary<String ^, SearchAndReplace ^>()),
m_sAnonymizationConfigurationFile(PathUtilities::CombinePaths(SystemSettings::CurrentSystemSettings->ConfigDirectory, "Console", "Anonymization.xml"))
{ 

  Initialize();
} // AnonymizationInformation( String ^ sSourcePath )


/** 
 ^ Constructor
 *
 ^ @param sSourcePath            Path to anonymize
 ^ @param sFilePattern           File pattern to use
 *
 */
AnonymizationInformation::AnonymizationInformation( String ^ sSourcePath, String ^ sFilePattern ) :
/***************************************************************************************************/
m_sSourcePath(sSourcePath),
m_sDestinationPath(String::Empty),
//m_sBasePath(System::IO::Path::GetTempPath()),
m_sFilePattern(sFilePattern),
m_pOldStudy(gcnew Viatronix::Enterprise::Entities::Study()),
m_pNewStudy(gcnew Viatronix::Enterprise::Entities::Study()),
m_pCurrentElementValues(gcnew Dictionary<String ^, SearchAndReplace ^>()),
m_sAnonymizationConfigurationFile(PathUtilities::CombinePaths(SystemSettings::CurrentSystemSettings->ConfigDirectory, "Console", "Anonymization.xml"))
{ 
  Initialize();
} // AnonymizationInformation( String ^ sSourcePath, String ^ sFilePattern )



/** 
 ^ Constructor
 *
 ^ @param sSourcePath                          Path to anonymize
 ^ @param sFilePattern                         File pattern
 ^ @param  aAnonymizationConfigurationFile     Config file
 *
 */
AnonymizationInformation::AnonymizationInformation( String ^ sSourcePath, String ^ sFilePattern, String ^ sAnonymizationConfigurationFile ) :
/*****************************************************************************************************************************************/
m_sSourcePath(sSourcePath),
  m_sDestinationPath(String::Empty),
//m_sBasePath(System::IO::Path::GetTempPath()),
m_sFilePattern(sFilePattern),
m_pOldStudy(gcnew Viatronix::Enterprise::Entities::Study()),
m_pNewStudy(gcnew Viatronix::Enterprise::Entities::Study()),
m_pCurrentElementValues(gcnew Dictionary<String ^, SearchAndReplace ^>()),
m_sAnonymizationConfigurationFile(sAnonymizationConfigurationFile)
{ 
  Initialize(); 
} // AnonymizationInformation( String ^ sSourcePath, String ^ sFilePattern, String ^ sAnonymizationConfigurationFile )



/** 
* Initializes the object
*/
void AnonymizationInformation::Initialize()
{
  
  Log::Debug(String::Concat("Initializing anonymization element list from ", m_sAnonymizationConfigurationFile), "AnonymizationInformation", "Initialize");

  if ( ! System::IO::File::Exists(m_sAnonymizationConfigurationFile) )
    throw gcnew System::IO::FileNotFoundException(String::Concat("Anonymization configuration file was not found ", m_sAnonymizationConfigurationFile));


  XmlDocument ^ mpDoc = gcnew XmlDocument();
  mpDoc->Load(m_sAnonymizationConfigurationFile);

  m_pNewElementValues = ElementCollection::Create(mpDoc->DocumentElement);

  // Check for Dicom files
  array<String ^> ^ dicomFiles = gcnew array<String ^>(0);
  if ( m_sSourcePath->CompareTo( String::Empty ) != 0 )
    dicomFiles = System::IO::Directory::GetFiles(m_sSourcePath,"*.dcm");

  if ( dicomFiles->Length > 0 ) 
  {
    //Take the first DICOM File
    //String ^ sFilename(System::IO::Path::GetFileNameWithoutExtension(dicomFiles[0]));
    m_pOriginalElementValues = Viatronix::Dicom::ElementCollection::Create(dicomFiles[0], Viatronix::Dicom::ParameterType::FILE);
    //Log::Debug(String::Concat("Setting up search and replace objects from dicom file ", dicomFiles[0]), "AnonymizationInformation", "Initialize");
    // TODO: REST New AddSearchAndRelaceObjects
    AddSearchAndReplaceObjects(m_pOriginalElementValues);
  }
  else
  {
    array<String ^> ^ volumeFiles = System::IO::Directory::GetFiles( m_sSourcePath, String::Concat(m_sFilePattern, Viatronix::Utilities::IO::FileExtensions::Volume));

    // Loop through all the volume files to build the search and replace objects
    for each ( String ^ sVolumeFile in volumeFiles )
    {
      String ^ sFilename(System::IO::Path::GetFileNameWithoutExtension(sVolumeFile));

      /// TODO Create XML from Volume file
      Viatronix::Dicom::ElementCollection ^ pElementList = AnonymizerUtilities::CreateElementList(sVolumeFile);
      Log::Debug(String::Concat("Setting up search and replace objects from volume file ", sVolumeFile), "AnonymizationInformation", "Initialization");
      AddSearchAndReplaceObjects( pElementList );
      
    } // END ... for each volume file
  }
} // Initialize()

/**
 ^ Adds search and replace pair
 *
 ^ @param sName                 Name of pair
 ^ @paran pSearchAndReplace     Search and replace pair
 */
void AnonymizationInformation::Add( String ^ sName, SearchAndReplace ^ pSearchAndReplace )
{
   m_pCurrentElementValues->Add( sName, pSearchAndReplace ); 
} // Add( String ^ sName, SearchAndReplace ^ pSearchAndReplace )



/** 
* Adds the search and replace objects based on the element list
*
* @param pOriginalElementList    Original element list 
*
*/
void AnonymizationInformation::AddSearchAndReplaceObjects( ElementCollection ^ pOriginalElementList )
{
  //Clear the current search and replace values.
  m_pCurrentElementValues->Clear();

  m_pOldStudy =  AnonymizerUtilities::CreateStudy(pOriginalElementList);
  m_pNewStudy =  AnonymizerUtilities::CreateStudy(m_pNewElementValues);

  // Study name 
  if ( ! m_pNewStudy->Name->Empty )
  {
    Add( "StudyName", gcnew SearchAndReplace(  m_pOldStudy->Name->ToString(), m_pNewStudy->Name->ToString() ));
    Add( "StudyLastName" , gcnew SearchAndReplace( m_pOldStudy->Name->LastName, m_pNewStudy->Name->LastName  ));
    Add( "StudyLastNamePartial", gcnew SearchAndReplace( m_pOldStudy->Name->MiddleName, m_pNewStudy->Name->MiddleName ));
    Add( "StudyFirstName", gcnew SearchAndReplace( m_pOldStudy->Name->FirstName, m_pNewStudy->Name->FirstName ));
  }

  // Study Medical Id
  Add( "MedicalId", gcnew SearchAndReplace(m_pOldStudy->MedicalId, m_pNewStudy->MedicalId));
  Add("ReferringPhysician", gcnew SearchAndReplace(m_pOldStudy->Physician, m_pNewStudy->Physician));

  Add("StudySex", gcnew SearchAndReplace(m_pOldStudy->Gender.ToString(), m_pNewStudy->Gender.ToString()));
  Add("StudyDescription", gcnew SearchAndReplace(m_pOldStudy->Description, m_pNewStudy->Description));
  Add("StudyDate", gcnew SearchAndReplace(m_pOldStudy->Date.ToString(), m_pNewStudy->Date.ToString()));

  if(m_pNewStudy->Dob == DateTime::MinValue)
  {
    // Study Date of Birth
    Add( "StudyDob", gcnew SearchAndReplace( m_pOldStudy->Dob.ToString(), m_pNewStudy->Dob.ToString() ));
  }

  // Handle the case of Viatronix being in the DirectoryName.  
  // Anonymizer refuses any changes with Viatronix or v3D in the search and replace.
  Viatronix::Enterprise::Entities::Study ^ pNewStudy = m_pNewStudy;

  if(m_pOldStudy->Name->LastName->Equals("Viatronix") || m_pOldStudy->Name->LastName->Equals("v3D"))
  {
    m_pNewStudy->Name->LastName = m_pOldStudy->Name->LastName;
  }

  if(m_pOldStudy->Name->FirstName->Equals("Viatronix") || m_pOldStudy->Name->FirstName->Equals("v3D"))
  {
    m_pNewStudy->Name->FirstName = m_pOldStudy->Name->FirstName;
  }

  if(m_pOldStudy->MedicalId->Equals("Viatronix") || m_pOldStudy->MedicalId->Equals("v3D"))
  {
    m_pNewStudy->MedicalId = m_pOldStudy->MedicalId;
  }

  String ^ sOldStudyUID(m_pOldStudy->Uid);
  String ^ sNewStudyUID(m_pNewStudy->Uid);

  // Don't check in with this.
  m_pOldStudy->Directory = m_pOldStudy->Uid->Replace(".", "_");
  m_pNewStudy->Directory = m_pNewStudy->Uid->Replace(".", "_");

  //////////////////////////////////////////////
  // Check if we are changing the study uid
  ////////////////////////////////////////////
  if ( sNewStudyUID->Length > 0 )
  {
    Add("StudyUID", gcnew SearchAndReplace( sOldStudyUID, sNewStudyUID ));
    Add("StudyDirectoryName", gcnew SearchAndReplace(m_pOldStudy->Directory, m_pNewStudy->Directory));
  } // END ... If study uid is being anaonymized
  else
  {
    Add("StudyDirectoryName", gcnew SearchAndReplace(m_pOldStudy->Directory, m_pOldStudy->Directory));
  }

  String ^ sOldSeriesUID(pOriginalElementList->SeriesInstanceUid);
  String ^ sNewSeriesUID(m_pNewElementValues->SeriesInstanceUid);

  /////////////////////////////////////////////
  // Check if we are changing the series uid
  ////////////////////////////////////////////
  if ( sNewSeriesUID->Length > 0 )
  {
    Add( "SeriesUID", gcnew SearchAndReplace( sOldSeriesUID, sNewSeriesUID ));
  } // END ... If the series uid is being anaonymized
} //AddSearchAndReplaceObjects( ElementList ^ pOriginalElementList )

/** 
* Adds the search and replace objects based on all the series uids. This is needed to make sure all the referneces and dependent lists
* are updated in the series.xml file.
*
* @param pSeriesCollection    Series collectionb 
*
*/
void AnonymizationInformation::AddSearchAndReplaceObjects( List<Viatronix::Enterprise::Entities::Series ^> ^ pSeriesCollection )
{

  StringDictionary ^ pMap = gcnew StringDictionary();

  for ( int i(0); i < pSeriesCollection->Count; ++i )
  {
    Viatronix::Enterprise::Entities::Series ^ pSeries(pSeriesCollection[i]);

    String ^ sOldUid = pSeries->Uid;

    pSeries->Uid = Viatronix::Dicom::Utility::GenerateUid();

    pMap->Add(sOldUid, pSeries->Uid);

    for ( int j(0); j < pSeriesCollection->Count; ++j )
    {
      if ( j != i )
      {

        Viatronix::Enterprise::Entities::Series ^ pS(pSeriesCollection[j]);
 
        for ( int k(0); k < pS->References->Count; ++k )
        {
          Viatronix::Enterprise::Entities::Series ^ pRefSeries(pS->References[k]);

          String ^ sUID(pRefSeries->ToString());

          if ( pMap->ContainsKey(sUID) )
          {

            String ^ sMapUid(pMap[sUID]);

            pRefSeries->Directory = pRefSeries->Directory->Replace(pRefSeries->Uid, sMapUid);
            pRefSeries->Directory = pRefSeries->Directory->Replace(pRefSeries->Uid->Replace(",","_"), sMapUid->Replace(".","_"));
            pRefSeries->Directory = pRefSeries->Directory->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);
            pRefSeries->Directory = pRefSeries->Directory->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);


            //pRefSeries->Path = pRefSeries->Path->Replace(pRefSeries->Uid, sMapUid);
            //pRefSeries->Path = pRefSeries->Path->Replace(pRefSeries->Uid->Replace(",","_"), sMapUid->Replace(".", "_"));
            //pRefSeries->Path = pRefSeries->Path->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);
            //pRefSeries->Path = pRefSeries->Path->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);

            pRefSeries->Uid = sOldUid;
          }

        }

        //for ( int k(0); k < pS->Dependents->Count; ++k )
        //{
        //  Series ^ pDepSeries(pS->Dependents[k]);

        //  String ^ sUID(pDepSeries->ToString());

        //  if ( pMap->ContainsKey(sUID) )
        //  {
        //    UID ^ pUID(gcnew UID(pMap[sUID]));

        //    pDepSeries->Location = pDepSeries->Location->Replace(pDepSeries->Uid, pUid);
        //    pDepSeries->Location = pDepSeries->Location->Replace(pDepSeries->UID->ToString("_"), pUID->ToString("_"));
        //    pDepSeries->Location = pDepSeries->Location->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);
        //    pDepSeries->Location = pDepSeries->Location->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);

        //    pDepSeries->FullPath = pDepSeries->FullPath->Replace(pDepSeries->Uid, pUid);
        //    pDepSeries->FullPath = pDepSeries->FullPath->Replace(pDepSeries->UID->ToString("_"), pUID->ToString("_"));
        //    pDepSeries->FullPath = pDepSeries->FullPath->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);
        //    pDepSeries->FullPath = pDepSeries->FullPath->Replace(this->StudyDirectoryName->Search, this->StudyDirectoryName->Replace);

        //    pDepSeries->UID = pUID;

        //  }

        //}

      }
    }


    String ^ sOldFileName(sOldUid->Replace(".", "_"));
    String ^ sNewFileName(pSeries->Uid->Replace(".","_"));

    Add(String::Format("Series_UID_{0}",i), gcnew SearchAndReplace(sOldUid, pSeries->Uid));
    Add(String::Format("Series_FileName_{0}",i), gcnew SearchAndReplace(sOldFileName, sNewFileName));
    Add(String::Format("Series_VRX_FileName_{0}",i), gcnew SearchAndReplace(sOldFileName->Substring(0, sOldFileName->Length - 2), sNewFileName->Substring(0, sNewFileName->Length - 2)));

  } // END ... For each series in list

} // AddSearchAndReplaceObjects( SeriesCollection ^ pSeriesCollection )

/** 
 ^ Returns an array of the search and replace objects
 *
 ^ @return search and replace object lisdt
 */
array<SearchAndReplace ^> ^ AnonymizationInformation::GetSearchAndReplaceObjects()
{
    array<SearchAndReplace ^> ^ pValues = gcnew array<SearchAndReplace ^>(m_pCurrentElementValues->Count);
    m_pCurrentElementValues->Values->CopyTo(pValues,0);
    return pValues;
} // GetSearchAndReplaceObjects()

/** 
 ^ Returns an array of the search and replace objects
 *
 ^ @return search and replace object lisdt
 */
array<SearchAndReplace ^> ^  AnonymizationInformation::GetSearchAndReplaceSeriesUIDObjects() 
{

  List<SearchAndReplace ^> ^ pList = gcnew List<SearchAndReplace ^>();

  array<String ^> ^ pKeys = gcnew array<String ^>(m_pCurrentElementValues->Count);
  array<SearchAndReplace ^> ^ pValues = gcnew array<SearchAndReplace ^>(m_pCurrentElementValues->Count);
  m_pCurrentElementValues->Values->CopyTo(pValues,0);
  m_pCurrentElementValues->Keys->CopyTo(pKeys, 0);

  for ( int i(0); i < pKeys->Length; ++i )
  {
    if ( pKeys[i]->StartsWith("Series_") )
    {
      pList->Add(pValues[i]);
    }
  }

  array<SearchAndReplace ^> ^ pReturn = gcnew array<SearchAndReplace ^> (pList->Count);

  pList->CopyTo(pReturn);

  return pReturn;
} // GetSearchAndReplaceObjects()


/** 
 ^ Logs the search and replace objects
 */
void AnonymizationInformation::ToLogger()
{
  if ( m_pCurrentElementValues->Count > 0 )
  {
    array<String ^> ^ pKeys = gcnew array<String ^>(m_pCurrentElementValues->Count);
    array<SearchAndReplace ^> ^ pValues = gcnew array<SearchAndReplace ^>(m_pCurrentElementValues->Count);
    m_pCurrentElementValues->Values->CopyTo(pValues,0);
    m_pCurrentElementValues->Keys->CopyTo(pKeys, 0);

    Log::Debug("ANONYMIZATION FIELD", "AnonymizationInformation", "ToLogger");
    for ( int i(0); i < m_pCurrentElementValues->Count; ++i )
    {
      Log::Debug(String::Format("{0,-32} ... {1}", pKeys[i], pValues[i]->ToString()), "AnonymizationInformation", "ToLogger");
    }
  }

} // ToLogger()

/** 
 ^ Gets the anonymiization destintaion path
 *
 ^ @return Destnaition path
 */
String ^ AnonymizationInformation::DestinationPath::get() 
{
  String ^ sReturn(m_sDestinationPath);

  if ( sReturn->Length == 0 )
    sReturn = System::IO::Path::Combine(m_sBasePath,System::IO::Path::GetFileName(m_sSourcePath));

  return sReturn;
} // get_DestinationPath()

/*
 ^ Gets the original folder name
 *
 ^ @return Original folder name
 */
String ^ AnonymizationInformation::GetOldDirectory()
{

  String ^ sDirectory(m_pOldStudy->Directory);

  if ( m_pNewStudy->Uid->Length > 0 )
  {
    sDirectory = m_pOldStudy->Directory;
  }

  return sDirectory;
} // GetOldDirectory()

/*
 ^ Gets the gcnew folder name
 */
String ^ AnonymizationInformation::GetNewDirectory()
{

  String ^ sDirectory(m_pNewStudy->Directory);

  if ( m_pNewStudy->Uid->Length == 0 )
  {
    sDirectory = m_pOldStudy->Directory;
  }
  
  return sDirectory;
} // GetNewDirectory()


Viatronix::Enterprise::Entities::PersonName ^ AnonymizationInformation::NewName::get() 
{  
  return gcnew Viatronix::Enterprise::Entities::PersonName(m_pNewElementValues->PatientName); 
}

String ^ AnonymizationInformation::NewMedicalID::get() 
{  
  return m_pNewElementValues->PatientId; 
}
