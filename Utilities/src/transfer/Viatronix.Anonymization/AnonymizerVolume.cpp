// $Id: AnonymizerVolume.cpp,v 1.11.2.1 2007/07/03 18:37:03 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
// Co-Author: [Romy Sreedharan] [ romy@viatronix.com]

// pragmas
#pragma once


// includes
#include "StdAfx.h"
#include "SearchAndReplace.h"
#include "AnonymizationInformation.h"
#include "anonymizerVolume.h"
#include "Exceptions.h"
#include "volapi.h"
#include "HeaderVolume.h"
#include "VxBlockVolume.h"

// namespaces
using namespace System;
using namespace System::Collections;
using namespace Viatronix::Utilities::IO;
using namespace Viatronix::Logging;


USING_ANMZN_NS

/*
 ^ Constructor
 */
AnonymizerVolume::AnonymizerVolume()
{
}//AnonymizerVolume()

/*
 ^ Destructor
 */
AnonymizerVolume::!AnonymizerVolume()
{
}//~AnonymizerVolume()

/*
 ^ Constructor
 *
 ^ @param anonymizationInfo       Anonymization info object
 ^ 
 */
AnonymizerVolume::AnonymizerVolume( AnonymizationInformation ^ anonymizationInfo ) : Anonymizer( anonymizationInfo )
{
} // AnonymizerVolume( AnonymizationInformation ^ anonymizationInfo )

/*
 ^ Anonymize method
 *
 ^ @param sSourceFiles      List of source files
 ^ @param sDestFiles        List of destination files 
 */
void AnonymizerVolume::Anonymize(  List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles )
{
  String ^ message = "Anonymizing Volume Files ";
  //Anonymize each file from the List
  for ( int index = 0; index < sSourceFiles->Count ; index++ )
  {
    if ( IsAbort())
      throw  gcnew AbortException( " User aborted anonymization" ); 

    System::String ^ sSourceFile = sSourceFiles[index];

    //Build the destination File.
    System::String ^ sDestFile  = ReplaceDirectoryNames(sDestFiles[index]);

    Log::Information( String::Concat( message , sSourceFile ), "AnonymizerVolume", "Anonymize");

    //Raise the progress event.
    this->RaiseProgressEvent( message , index );
    try
    {

      if ( String::Compare(PathUtilities::GetVolumeType( sSourceFile ),"LINEAR") == 0 )
        AnonymizeLinearVolume( sSourceFile,  sDestFile );
      else
        AnonimizeBlockVolume( sSourceFile,  sDestFile );
    }
    catch ( AbortException ^ ex )
    {
      throw ex;
    }
    catch ( Exception ^ ex )
    {
      Log::Error( String::Concat( "Failed to anonymize dicom file ", sSourceFile, " : ", ex->Message ) , "AnonymizerVolume", "Anonymize");
      throw gcnew AnonymizationException( "Failed to anonymize vol file ", ex );
    }
  }
 } // Anonymize


/**
 ^ Anonymize a linear volume file.
 *
 ^ @param sSourceFile            The source volume file to anonymize
 ^ @param sDestFile              The destination of the anonymized volume file.
 *
 ^ @return void
 */
void AnonymizerVolume::AnonymizeLinearVolume( String ^ sSourceFile, String ^ sDestFile )
{

  std::string sSource = GetUnManagedStdString( sSourceFile );
  std::string sDest   = GetUnManagedStdString( sDestFile );

  Log::Debug( String::Concat( "Anonymizing Linear Volume File: ", sSourceFile )  , "AnonymizerVolume", "AnonymizeLinearVolume");

  HeaderVol * pHeaderVolume( NULL );
  V3DFile * pNewFileImage( NULL );

  try
  {
    pHeaderVolume = new HeaderVol();

    // TODO :: Report Updated
    Log::Debug( String::Concat( "Reading in header from ", sSourceFile ) , "AnonymizerVolume", "AnonymizeLinearVolume");
    pHeaderVolume->Read( sSource );

    VarMap headerMap = pHeaderVolume->GetHdrMap();

    VarMap * pVarMap = & headerMap;

    // Operator Name
    SetVarMap(IntPtr((void *)pVarMap), "operatorName", "Operator Name", m_mpAnonymizationInfo->AnonymizationElementList->OperatorName );

    /// Institution Name
    SetVarMap(IntPtr((void *)pVarMap), "institutionName", "Institution Name", m_mpAnonymizationInfo->AnonymizationElementList->InstitutionName );

    // Patient Name
    SetVarMap(IntPtr((void *)pVarMap), "patientName", "Patient Name", m_mpAnonymizationInfo->AnonymizationElementList->PatientName->ToString() );

    // Patient ID
    SetVarMap(IntPtr((void *)pVarMap), "patientID", "Patient ID", m_mpAnonymizationInfo->AnonymizationElementList->PatientId );


    

    // Patient History
    SetVarMap(IntPtr((void *)pVarMap), "patientHistory", "Additional patient history", m_mpAnonymizationInfo->AnonymizationElementList->AdditionalPatientHistory );

    // Study Date
    SetVarMap(IntPtr((void *)pVarMap), "studyDate", "Study Date", m_mpAnonymizationInfo->AnonymizationElementList->StudyDate );


    // Series Date
    SetVarMap(IntPtr((void *)pVarMap), "seriesDate", "Series Date", m_mpAnonymizationInfo->AnonymizationElementList->SeriesDate );


    // Patient Birth Date
    SetVarMap(IntPtr((void *)pVarMap), "patientBirthDate", "Patient Birth Date", m_mpAnonymizationInfo->AnonymizationElementList->PatientDob );

    Viatronix::Enterprise::Entities::PersonAge ^ mpAge(gcnew Viatronix::Enterprise::Entities::PersonAge(m_mpAnonymizationInfo->AnonymizationElementList->PatientAge));

    // Patient Age
    SetVarMap(IntPtr((void *)pVarMap), "patientAge", "Patient Age", mpAge->Age.ToString());


    // Patient Sex
    SetVarMap(IntPtr((void *)pVarMap), "patientSex", "Patient Age", m_mpAnonymizationInfo->AnonymizationElementList->PatientSex );

    // Referring Physician Name
    SetVarMap(IntPtr((void *)pVarMap), "physicianName", "Physician Name", m_mpAnonymizationInfo->AnonymizationElementList->ReferringPhysician->ToString() );


    std::string sDicomHeader;
    pVarMap->GetHdrVar("dicomHeader", sDicomHeader);

    // Check if the dicom header exists
    if ( ! sDicomHeader.empty() )
    {
      String ^ mpDicomHeader = gcnew String(sDicomHeader.c_str());

      Log::Debug("Initializing element list from serialize dicomHeader variable", "AnonymizerVolume", "AnonymizeLinearVolume");

      /// TODO (Convert the seriealize string from old volume headers)
      Viatronix::Dicom::ElementCollection ^ mpElements = Viatronix::Dicom::ElementCollection::Create(mpDicomHeader, Viatronix::Dicom::ParameterType::HEADER);

      Log::Debug("Anonymizing dicomHeader variable" , "AnonymizerDicom", "AnonymizeLinearVolume");

      // TODO: REST Create modify
      // pElements->Modify(m_pAnonymizationInfo->AnonymizationElementList->ToXml());

      Log::Debug("Setting gcnew anonymized dicomHeader variable", "AnonymizerVolume", "AnonymizeLinearVolume");
      pVarMap->SetHdrVar("dicomHeader", GetUnManagedStdString(mpElements->ToString()), "Serialized Dicom Header");
    } // END ... Dicom Header exists

    pNewFileImage = new V3DFile();

    Log::Debug(String::Concat("Copying linear volume image to : ", sDestFile), "AnonymizerVolume", "AnonymizeLinearVolume");
    std::string sHeader(pVarMap->ToString());

    Log::Debug(String::Format("Copying Header from {0} to {1}", sSourceFile, sDestFile), "AnonymizerVolume", "AnonymizeLinearVolume");
    pNewFileImage->Copy(sSource, sDest, sHeader);
    // TODO:: Add Updated
  }
  catch ( System::Threading::ThreadAbortException ^ ex )
  {
    Log::Error(String::Format( "Could not anonymize volume file [{0}] : {1}", sSourceFile, ex->Message ), "AnonymizerVolume", "AnonymizeLinearVolume");
    throw gcnew AnonymizationException( "Could not anonymize volume file", ex );
  } // END ... catch ThreadAbortException
  catch ( System::Exception ^ ex )
  {
    String ^ sErrorMessage = String::Format("Could not anonymize volume file [{0}] : {1}", sSourceFile, ex->Message);
    Log::Error(sErrorMessage, "AnonymizerVolume", "AnonymizeLinearVolume");
    throw gcnew AnonymizationException(sErrorMessage);
  } // END ... catch Exception
  catch ( ... )
  {
    String ^ sErrorMessage = String::Format("Could not anonymize volume file [{0}] : unspecified exception", sSourceFile);
    Log::Error(sErrorMessage, "AnonymizerVolume", "AnonymizeLinearVolume");
    throw gcnew AnonymizationException(sErrorMessage);

  } // END ... catch everything else
  __finally 
  {
    if ( pHeaderVolume != nullptr )
      delete pHeaderVolume;

    if ( pNewFileImage != nullptr )
      delete pNewFileImage;
  } // END ... __finally

} // AnonymizeLinearVolumeFile()

/**
 ^ Sets the variable map value if it exists
 *
 ^ @param varMapPtr     VarMap pointer
 ^ @param sName         Name of variable
 ^ @param sDesc         Description to be used 
 *
 ^ @param sValue        gcnew value
 */
void AnonymizerVolume::SetVarMap(IntPtr varMapPtr, String ^ sName, String ^ sDesc, String ^ sValue )
{

    VarMap * pVarMap = reinterpret_cast<VarMap *>(varMapPtr.ToPointer());

    const std::string sVarName(ss(sName));
    std::string sVarValue;
    pVarMap->GetHdrVar(sVarName, sVarValue);

    if ( ! sVarValue.empty() )
    {
      sVarValue = ss(sValue); 
      Log::Debug(String::Format("Setting {0} = {1}", sName, sValue), "AnonymizerDicom", "SetVarMap");
      pVarMap->SetHdrVar(sVarName, sVarValue, ss(sDesc));
    }

} // SetVarMap(IntPtr varMapPtr, String ^ sName, String ^ sDesc, String ^ sValue )

/**
 ^ Anonymize a Block volume file.
 *
 ^ @param sSourceFile            The source volume file to anonymize
 ^ @param sDestFile              The destination of the anonymized volume file.
 *
 ^ @return void
 */
void AnonymizerVolume::AnonimizeBlockVolume( String ^ sSourceFile, String ^ sDestFile )
{
  std::string sSource = GetUnManagedStdString(sSourceFile);
  std::string sDest = GetUnManagedStdString(sDestFile);

  String ^ sDataType(PathUtilities::GetVolumeDataType(sSourceFile));
  Log::Debug(String::Concat("Anonymizing Block Volume File [" , sDataType , "] : ", sSourceFile), "AnonymizerVolume", "AnonimizeBlockVolume");

  vxBlockVolume<uint2> * pVolumeUint2 = NULL;
  vxBlockVolume<int2>  * pVolumeInt2  = NULL;
  vxBlockVolume<bool>  * pVolumeBool  = NULL;
  vxBlockVolume<int1>  * pVolumeInt1  = NULL;


  VarMap * pVarMap = NULL;
  VarMap * pVarHdr = NULL;

  try
  {

    std::pair<float,float> progressBar(0.0f,100.0f);
    pVarMap = new VarMap();

    Log::Debug("Read volume file", "AnonymizerVolume", "AnonimizeBlockVolume");

    if ( String::Compare(sDataType,L"UINT2") == 0 )
    {

      pVolumeUint2 = new vxBlockVolume<uint2>;
      pVolumeUint2->Read(sSource, progressBar);
      pVarMap->CreateMap(pVolumeUint2->GetHeader().GetVarMapsAsString());
    }
    else if ( String::Compare(sDataType,L"INT2") == 0 )
    {
      pVolumeInt2 = new vxBlockVolume<int2>;
      pVolumeInt2->Read(sSource, progressBar);
      pVarMap->CreateMap(pVolumeInt2->GetHeader().GetVarMapsAsString());
    }
    else if ( String::Compare(sDataType,L"BOOL") == 0 )
    {
      pVolumeBool = new vxBlockVolume<bool>;
      pVolumeBool->Read(sSource, progressBar);
      pVarMap->CreateMap(pVolumeBool->GetHeader().GetVarMapsAsString());
    }
    else if ( String::Compare(sDataType,L"INT1") == 0 || String::Compare(sDataType, L"CHAR") == 0 )
    {
      pVolumeInt1 = new vxBlockVolume<int1>;
      pVolumeInt1->Read(sSource, progressBar);
      pVarMap->CreateMap(pVolumeInt1->GetHeader().GetVarMapsAsString());
    }
    else
    {
      String ^ sErrorMessage = String::Format("Could not anonymize volume file [{0}] : unsupprted volume data type ({1}]", sSourceFile, sDataType);
      Log::Error(sErrorMessage, "AnonymizerVolume", "AnonimizeBlockVolume");
      throw gcnew System::IO::IOException(sErrorMessage);
    }

    Log::Debug("Anonymizing volume header", "AnonymizerVolume", "AnonimizeBlockVolume");

    std::string sVarHeader;
    if ( pVarMap->GetHdrVar("varHeader", sVarHeader) )
    {

      pVarHdr = new VarMap(sVarHeader);
      std::string sValue;

      // Operator Name
      try 
      { 
        if ( pVarHdr->GetHdrVar( "operatorName", sValue ) )    
        {
          const std::string sOperatorName( GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->OperatorName )); 
          pVarHdr->SetHdrVar("operatorName", sOperatorName, "Operator Name");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Instaitution Name
      try 
      { 
        if ( pVarHdr->GetHdrVar( "institutionName", sValue ) )  
        {
          const std::string sInstitutionName(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->InstitutionName)); 
          pVarHdr->SetHdrVar("institutionName", sInstitutionName, "Institution Name");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Patient Name
      try 
      { 
        if ( pVarHdr->GetHdrVar( "patientName", sValue ) )
        {
          const std::string sPatientName(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->PatientName->ToString())); 
          pVarHdr->SetHdrVar("patientName", sPatientName, "Patient Name");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Patient ID
      try 
      { 
        if ( pVarHdr->GetHdrVar( "patientID", sValue ) )
        {
          const std::string sMedicalID(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->PatientId)); 
          pVarHdr->SetHdrVar("patientID", sMedicalID, "Patient ID");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Patient History
      try 
      { 
        if ( pVarHdr->GetHdrVar( "patientHistory", sValue ) ) 
        {
          std::string sPatientHistory(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->AdditionalPatientHistory)); 
          pVarHdr->SetHdrVar("patientHistory", sPatientHistory, "Additional patient history");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Study Date
      try 
      { 
        if ( pVarHdr->GetHdrVar( "studyDate", sValue ) )
        {
          std::string sStudyDate(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->StudyDate)); 
          pVarHdr->SetHdrVar("studyDate", sStudyDate, "Study Date");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Series Date
      try 
      { 
        if ( pVarHdr->GetHdrVar( "seriesDate", sValue ) )
        {
          const std::string sSeriesDate(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->SeriesDate)); 
          pVarHdr->SetHdrVar("seriesDate", sSeriesDate, "Series Date");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Patient Birth Date
      try 
      { 
        if ( pVarHdr->GetHdrVar( "patientBirthDate", sValue ) ) 
        {
          std::string sPatientDOB(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->PatientDob)); 
          pVarHdr->SetHdrVar("patientBirthDate", sPatientDOB, "Patient Birth Date");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Patient Age
      try 
      { 
        if ( pVarHdr->GetHdrVar( "patientAge", sValue ) ) 
        {

          Viatronix::Enterprise::Entities::PersonAge ^ mpAge(gcnew Viatronix::Enterprise::Entities::PersonAge(m_mpAnonymizationInfo->AnonymizationElementList->PatientAge));

          std::string sPatientAge(GetUnManagedStdString(System::Convert::ToString(mpAge->Age))); 
          pVarHdr->SetHdrVar("patientAge", sPatientAge, "Patient Age");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Patient Sex
      try 
      { 
        if ( pVarHdr->GetHdrVar( "patientSex", sValue ) ) 
        {
          std::string sPatientSex(GetUnManagedStdString(System::Convert::ToString(m_mpAnonymizationInfo->AnonymizationElementList->PatientSex))); 
          pVarHdr->SetHdrVar("patientSex", sPatientSex, "Patient Sex");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Referring Physician Name
      try 
      { 
        if ( pVarHdr->GetHdrVar( "physicianName", sValue ) )
        {
          const std::string sPhysician(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->ReferringPhysician->ToString())); 
          pVarHdr->SetHdrVar("physicianName", sPhysician, "Physician Name");
        }
      } 
      catch ( Viatronix::Dicom::DicomException ^ ) 
      { 
        // Ignore 
      }

      // Dicom Header
      std::string sDicomHeader;
      if ( pVarHdr->GetHdrVar("dicomHeader", sDicomHeader) )
      {
        try
        {
          String ^ sDicomHdr = gcnew String(sDicomHeader.c_str());
          Viatronix::Dicom::ElementCollection ^ pElements = Viatronix::Dicom::ElementCollection::Create( sDicomHdr, Viatronix::Dicom::ParameterType::HEADER );

          // TODO: REST Need update/modify
          //pElements->Insert(m_pAnonymizationInfo->AnonymizationElementList->ToXml());
          pVarHdr->SetHdrVar( "dicomHeader", GetUnManagedStdString( pElements->SerializedHeader), "Serialized Dicom Header"  );
          
        }
        catch ( Viatronix::Dicom::DicomException ^ ex ) 
        { 
          String ^ sErrorMessage = String::Format("Could not anonymize volume file [{0}] : unspecified exception", sSourceFile);
          Log::Error(sErrorMessage, "AnonymizerVolume", "AnonimizeBlockVolume");
          throw gcnew AnonymizationException( sErrorMessage, ex );
        }
      } // END ... Dicom Header

      Log::Debug("Setting newly anonymized volume header", "AnonymizerVolume", "AnonimizeBlockVolume");

      if ( String::Compare(sDataType,L"UINT2") == 0 )
      {
        pVolumeUint2->GetHeader().SetVariableVolumeHeader(*pVarHdr);
        Log::Debug(String::Concat("Writing gcnew block volume file : ", sDestFile), "AnonymizerDicom", "AnonimizeBlockVolume");
        pVolumeUint2->Write(sDest);
      }
      else if ( String::Compare(sDataType,L"INT2") == 0 )
      {
        pVolumeInt2->GetHeader().SetVariableVolumeHeader(*pVarHdr);
        Log::Debug(String::Concat("Writing gcnew block volume file : ", sDestFile), "AnonymizerDicom", "AnonimizeBlockVolume");
        pVolumeInt2->Write(sDest);
      }
      else if ( String::Compare(sDataType,L"BOOL") == 0 )
      {
        pVolumeBool->GetHeader().SetVariableVolumeHeader(*pVarHdr);
        Log::Debug(String::Concat("Writing gcnew block volume file : ", sDestFile), "AnonymizerDicom", "AnonimizeBlockVolume");
        pVolumeBool->Write(sDest);
      }
      else if ( String::Compare(sDataType,L"INT1") == 0 || String::Compare(sDataType, L"CHAR") == 0 )
      {
        pVolumeInt1->GetHeader().SetVariableVolumeHeader(*pVarHdr);
        Log::Debug(String::Concat("Writing gcnew block volume file : ", sDestFile), "AnonymizerDicom", "AnonimizeBlockVolume");
        try
        {
          pVolumeInt1->Write(sDest);
        }
        catch( Exception ^ ex )
        {
          throw gcnew AnonymizationException ( " Writing gcnew block volume failed", ex );
        }
      }
    }

  }
  catch ( System::Threading::ThreadAbortException ^ ex )
  {
    Log::Error(String::Format("Could not anonymize volume file [{0}] : {1}", sSourceFile, ex->Message), "AnonymizerVolume", "AnonimizeBlockVolume");
    throw gcnew AnonymizationException( "Could not anonymize volume file" );
  } // END ... catch ThreadAbortException
  catch ( System::Exception ^ ex )
  {
    String ^ sErrorMessage = String::Format("Could not anonymize volume file [{0}] : {1}", sSourceFile, ex->Message);
    Log::Error(sErrorMessage, "AnonymizerVolume", "AnonimizeBlockVolume");
    throw gcnew AnonymizationException( sErrorMessage );
  } // END ... catch Exception
  catch ( ... )
  {
    String ^ sErrorMessage = String::Format("Could not anonymize volume file [{0}] : unspecified exception", sSourceFile);
    Log::Error(sErrorMessage, "AnonymizerVolume", "AnonimizeBlockVolume");
    throw gcnew AnonymizationException(sErrorMessage);

  } // END ... catch everything else
  __finally 
  {
    // Clean up resources
    if ( pVolumeUint2 != nullptr ) delete pVolumeUint2;
    if ( pVolumeInt2 != nullptr )  delete pVolumeInt2;
    if ( pVolumeBool != nullptr )  delete pVolumeBool;
    if ( pVolumeInt1 != nullptr )  delete pVolumeInt1;
    if ( pVarMap != nullptr )      delete pVarMap;
    if ( pVarHdr != nullptr )      delete pVarHdr;
  } // END ... __finally

} // AnonymizeBlockVolume()



// $Log: AnonymizerVolume.cpp,v $
// Revision 1.11.2.1  2007/07/03 18:37:03  mkontak
// Issue 5707: Makes use of the gcnew ReplaceDirectoryNames()method to anonymize the destination path.
//
// Revision 1.11  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.10  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.9  2006/03/01 20:51:50  mkontak
// Modified to fix problem with setting sld volume headers.
//
// Revision 1.8  2005/11/15 19:54:06  mkontak
// Moved the GetVolumeType() and GetVolumeDataType() out of anonymization
// and into utilties.
//
// Revision 1.7  2005/10/20 15:18:16  romy
// added abort method
//
// Revision 1.6  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerVolume.cpp,v 1.11.2.1 2007/07/03 18:37:03 mkontak Exp $
// $Id: AnonymizerVolume.cpp,v 1.11.2.1 2007/07/03 18:37:03 mkontak Exp $