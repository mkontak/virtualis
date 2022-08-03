// $Id: VolumeHeader.cpp,v 1.5.2.3 2011/05/18 01:56:59 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mkontak@viatronix.com)


/// includes
#include "stdafx.h"
#include "VolumeHeader.h"
#include "PathUtilities.h"
#include "Volume.h"
#include "vxBlockVolume.h"
#include "vxVolumeHeader.h"


// namespaces
using namespace System;
using namespace System::Text;
using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace Viatronix::Logging;
using namespace Viatronix::v3D::Utilities::IO;

USING_UTILITIES_NS

/**
 * Constructor
 *
 * @param sVolumeFile     Voluem file
 */
VolumeHeader::VolumeHeader(String * sVolumeFile) :
m_sVolumeFile(sVolumeFile),
m_sType(String::Empty),
m_sStudyUID(String::Empty),
m_sSeriesUID(String::Empty),
m_sPatientAgeUnits(String::Empty),
m_sOperatorName(String::Empty),
m_sInstitutionName(String::Empty),
m_sPatientID(String::Empty),
m_sPatientName(String::Empty),
m_sPatientHistory(String::Empty),
m_sStudyDate(String::Empty),
m_sSeriesDate(String::Empty),
m_sPatientDOB(String::Empty),
m_sPatientAge(String::Empty),
m_sPatientSex(String::Empty),
m_sReferringPhysician(String::Empty),
m_sDicomHeader(String::Empty),
m_sMA(String::Empty),
m_sSliceThickness(String::Empty),
m_sPitch(String::Empty),
m_sFOV(String::Empty),
m_sVersion(String::Empty),
m_sSiteID(String::Empty),
m_sManufacturer(String::Empty),
m_sTilt(String::Empty),
m_sKV(String::Empty),
m_sPatientPosition(String::Empty),
m_sStudyType(String::Empty),
m_bZOrderReversed(false),
m_sEncryptedPatientName(String::Empty),
m_sEncryptedPatientID(String::Empty),
m_iTotalNumberOfBlocks(0),
m_iNumberOfUniqueBlocks(0),
m_iNumberOfVoxelsPerBlock(0),
m_iByteSizeOfIndexArray(0),
m_sBodyPart(String::Empty),
m_sAnonymousPatientDOB(String::Empty)
{

  m_sType = PathUtilities::GetVolumeType(sVolumeFile);
  m_sDataType = PathUtilities::GetVolumeDataType(sVolumeFile);

  if ( String::Compare(m_sType, "LINEAR", true) == 0 )
  {
    InitializeFromLinearVolume(sVolumeFile);
  }
  else
  {
    InitializeFromBlockVolume(sVolumeFile);

  }
} // VolumeHeader(String * sVolumeFile)


/**
 * Initializes the header object using the linear volume
 *
 * @param sVolumeFile     Volume file to read
 */
void VolumeHeader::InitializeFromLinearVolume(String * sVolumeFile)
{

  std::string sSource = GetUnManagedStdString( sVolumeFile );

  Log::Debug( String::Concat( "Initializing from linear file [FILE=", sVolumeFile, "]" ), "VolumeHeader", "InitializeFromLinearVolume");

  HeaderVol * pHeaderVolume( NULL );
  V3DFile * pNewFileImage( NULL );
  std::string sValue;

  try
  {
    pHeaderVolume = new HeaderVol();

    // TODO :: Report Updated
    Log::Debug( String::Concat( "Reading in header from [FILE=", sVolumeFile,"]" ), "VolumeHeader", "InitializeFromLinearVolume");
    pHeaderVolume->Read( sSource );

    m_dim.X = pHeaderVolume->GetDim().m_x;
    m_dim.Y = pHeaderVolume->GetDim().m_y;
    m_dim.Z = pHeaderVolume->GetDim().m_z;


    m_units.X = pHeaderVolume->GetUnits().m_x;
    m_units.Y = pHeaderVolume->GetUnits().m_y;
    m_units.Z =  pHeaderVolume->GetUnits().m_z;

    // Operator Name
    if ( pHeaderVolume->GetHdrVar( "operatorName", sValue ) )    
    {
      m_sOperatorName = new String(sValue.c_str());
    }

    // Instaitution Name
    if ( pHeaderVolume->GetHdrVar( "institutionName", sValue ) )  
    {
      m_sInstitutionName = new String(sValue.c_str());
    }

    // Patient Name
    if ( pHeaderVolume->GetHdrVar( "patientName", sValue ) )
    {
      m_sPatientName = new String(sValue.c_str());
    }

    // Patient ID
    if ( pHeaderVolume->GetHdrVar( "patientID", sValue ) )
    {
      m_sPatientID = new String(sValue.c_str());
    }

    // Patient History
    if ( pHeaderVolume->GetHdrVar( "patientHistory", sValue ) ) 
    {
      m_sPatientHistory = new String(sValue.c_str());
    }

    // Patient Position
    if ( pHeaderVolume->GetHdrVar( "patientPosition", sValue ) ) 
    {
      m_sPatientPosition = new String(sValue.c_str());
    }

    // Study UID
    if ( pHeaderVolume->GetHdrVar( "studyUID", sValue ) )
    {
      m_sStudyUID = new String(sValue.c_str());
    }

    // Series UID
    if ( pHeaderVolume->GetHdrVar( "seriesUID", sValue ) )
    {
      m_sSeriesUID = new String(sValue.c_str());
    }

    // Study Date
    if ( pHeaderVolume->GetHdrVar( "studyDate", sValue ) )
    {
      m_sStudyDate = new String(sValue.c_str());
    }

    // Series Date
    if ( pHeaderVolume->GetHdrVar( "seriesDate", sValue ) )
    {
      m_sSeriesDate = new String(sValue.c_str());
    }

    // Patient Birth Date
    if ( pHeaderVolume->GetHdrVar( "patientBirthDate", sValue ) ) 
    {
      m_sPatientDOB = new String(sValue.c_str());
    }

    // Patient Age
    if ( pHeaderVolume->GetHdrVar( "patientAge", sValue ) ) 
    {
      m_sPatientAge = new String(sValue.c_str());
    }

    // Patient Age Units
    if ( pHeaderVolume->GetHdrVar( "patientAgeUnits", sValue ) ) 
    {
      m_sPatientAgeUnits = new String(sValue.c_str());
    }

    // Patient Sex
    if ( pHeaderVolume->GetHdrVar( "patientSex", sValue ) ) 
    {
      m_sPatientSex = new String(sValue.c_str());
    }

    // Referring Physician Name
    if ( pHeaderVolume->GetHdrVar( "physicianName", sValue ) )
    {
      m_sReferringPhysician = new String(sValue.c_str());
    }

    // Dicom Header
    if ( pHeaderVolume->GetHdrVar("dicomHeader", sValue) )
    {
      m_sDicomHeader = new String(sValue.c_str());
    } // END ... Dicom Header


    // mA
    if ( pHeaderVolume->GetHdrVar( "mA", sValue ) )
    {
      m_sMA = new String(sValue.c_str());
    }

    // sliceThickness
    if ( pHeaderVolume->GetHdrVar( "sliceThickness", sValue ) )
    {
      m_sSliceThickness = new String(sValue.c_str());
    }

    // pitch
    if ( pHeaderVolume->GetHdrVar( "pitch", sValue ) )
    {
      m_sPitch = new String(sValue.c_str());
    }

    // FOV
    if ( pHeaderVolume->GetHdrVar( "FOV", sValue ) )
    {
      m_sPitch = new String(sValue.c_str());
    }

    // Version
    if ( pHeaderVolume->GetHdrVar( "version", sValue ) )
    {
      m_sVersion = new String(sValue.c_str());
    }

    // SiteID
    if ( pHeaderVolume->GetHdrVar( "siteID", sValue ) )
    {
      m_sSiteID = new String(sValue.c_str());
    }


    // menufacturer
    if ( pHeaderVolume->GetHdrVar( "manufacturer", sValue ) )
    {
      m_sManufacturer = new String(sValue.c_str());
    }

    // tilt
    if ( pHeaderVolume->GetHdrVar( "tilt", sValue ) )
    {
      m_sTilt = new String(sValue.c_str());
    }

     // KV
    if ( pHeaderVolume->GetHdrVar( "kv", sValue ) )
    {
      m_sKV = new String(sValue.c_str());
    }

    // studyType
    if ( pHeaderVolume->GetHdrVar( "studyType", sValue ) )
    {
      m_sStudyType = new String(sValue.c_str());
    }

    // zOrderReversed
    if ( pHeaderVolume->GetHdrVar( "zOrderReversed", sValue ) )
    {
      m_bZOrderReversed = ( String::Compare((new String(sValue.c_str())),"1") == 0 ? true : false );
    }


    // patientBirthday
    if ( pHeaderVolume->GetHdrVar( "patientBirthday", sValue ) )
    {
      m_sAnonymousPatientDOB = new String(sValue.c_str());
    }

    // encryptedPatientName
    if ( pHeaderVolume->GetHdrVar( "encryptedPatientName", sValue ) )
    {
      m_sEncryptedPatientName = new String(sValue.c_str());
    }

    // encryptedPatientID
    if ( pHeaderVolume->GetHdrVar( "encryptedPatientID", sValue ) )
    {
      m_sEncryptedPatientID = new String(sValue.c_str());
    }

    // BodyPart
    if ( pHeaderVolume->GetHdrVar( "BodyPart", sValue ) )
    {
      m_sBodyPart = new String(sValue.c_str());
    }

    // uTotalNumberOfBlocks
    if ( pHeaderVolume->GetHdrVar( "uTotalNumberOfBlocks", sValue ) )
    {
      String * sStr(new String(sValue.c_str()));
      m_iTotalNumberOfBlocks = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
    }

    // uNumberOfUniqueBlocks
    if ( pHeaderVolume->GetHdrVar( "uNumberOfUniqueBlocks", sValue ) )
    {
      String * sStr(new String(sValue.c_str()));
      m_iNumberOfUniqueBlocks = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
    }

    // uNumberOfVoxelsPerBlock
    if ( pHeaderVolume->GetHdrVar( "uNumberOfVoxelsPerBlock", sValue ) )
    {
      String * sStr(new String(sValue.c_str()));
      m_iNumberOfVoxelsPerBlock = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
    }


    // uByteSizeOfIndexArray
    if ( pHeaderVolume->GetHdrVar( "uByteSizeOfIndexArray", sValue ) )
    {
      String * sStr(new String(sValue.c_str()));
      m_iByteSizeOfIndexArray = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
    }


    // TODO:: Add Updated
  }
  catch ( System::Exception * ex )
  {
    Log::Error(String::Format("Could not anonymize volume file [FILE={0}] : {1}", sVolumeFile, ex->Message), "VolumeHeader", "InitializeFromLinear" );
    throw;
  } // END ... catch Exception
  catch ( ... )
  {
    String * sErrorMessage = String::Format("Could not anonymize volume file [FILE={0}] : unspecified exception", sVolumeFile);
    Log::Error(sErrorMessage, "VolumeHeader", "InitializeFromLinearVolume");
    throw new Exception(sErrorMessage);

  } // END ... catch everything else
  __finally 
  {
    if ( pHeaderVolume != NULL )
    {
      delete pHeaderVolume;
    }

  } // END ... __finally

} // InitializeFromLinearVolume(String * sVolumeFile)


/**
 * Initializes the volume header using the block volume
 *
 * @param sVolumeFile     Volume file to read
 */
void VolumeHeader::InitializeFromBlockVolume(String * sVolumeFile)
{
  std::string sSource = GetUnManagedStdString(sVolumeFile);

  String * sDataType(PathUtilities::GetVolumeDataType(sVolumeFile));
  Log::Debug(String::Concat("Initializingfrom block volume file [" , sDataType , "] : ", sVolumeFile), "VolumeHeader", "InitializeFromBlockVolume");

  vxBlockVolume<uint2> * pVolumeUint2 = NULL;
  vxBlockVolume<int2>  * pVolumeInt2  = NULL;
  vxBlockVolume<bool>  * pVolumeBool  = NULL;
  vxBlockVolume<int1>  * pVolumeInt1  = NULL;
  vxVolumeHeader * pHeader = NULL;


  VarMap * pVarMap = NULL;
  VarMap * pVarHdr = NULL;

  try
  {

    std::pair<float,float> progressBar(0.0f,100.0f);
    pVarMap = new VarMap();

    Log::Debug("Read volume file", "VolumeHeader", "InitializeFromBlockVolume");

    if ( String::Compare(sDataType,L"UINT2") == 0 )
    {
			pHeader = new vxVolumeHeader();
			pHeader->Read( sSource );
			pVarMap->CreateMap( pHeader->GetVarMapsAsString());

   }
    else if ( String::Compare(sDataType,L"INT2") == 0 )
    {
      pVolumeInt2 = new vxBlockVolume<int2>;
      pVolumeInt2->Read(sSource, progressBar);
      pVarMap->CreateMap(pVolumeInt2->GetHeader().GetVarMapsAsString());

      pHeader = &pVolumeInt2->GetHeader();
    }
    else if ( String::Compare(sDataType,L"BOOL") == 0 )
    {
      pVolumeBool = new vxBlockVolume<bool>;
      pVolumeBool->Read(sSource, progressBar);
      pVarMap->CreateMap(pVolumeBool->GetHeader().GetVarMapsAsString());
      pHeader = &pVolumeBool->GetHeader();

    }
    else if ( String::Compare(sDataType,L"INT1") == 0 || String::Compare(sDataType, L"CHAR") == 0 )
    {
      pVolumeInt1 = new vxBlockVolume<int1>;
      pVolumeInt1->Read(sSource, progressBar);
      pVarMap->CreateMap(pVolumeInt1->GetHeader().GetVarMapsAsString());
      pHeader = &pVolumeInt1->GetHeader();

    }
    else
    {
      String * sErrorMessage = String::Format("Could not read volume file [FILE={0}] : unsupprted volume data type ({1}]", sVolumeFile, sDataType);
      Log::Error(sErrorMessage, "VolumeHeader", "InitializeFromBlockVolume");
      throw new System::IO::IOException(sErrorMessage);
    }

    Log::Debug("Extracting volume header", "VolumeHeader", "InitializeFromBlockVolume");


    m_dim.X = pHeader->GetVolDim().m_x;
    m_dim.Y = pHeader->GetVolDim().m_y;
    m_dim.Z = pHeader->GetVolDim().m_z;


    m_units.X = pHeader->GetVoxelUnits().m_x;
    m_units.Y = pHeader->GetVoxelUnits().m_y;
    m_units.Z =  pHeader->GetVoxelUnits().m_z;


    std::string sVarHeader;
    if ( pVarMap->GetHdrVar("varHeader", sVarHeader) )
    {

      pVarHdr = new VarMap(sVarHeader);
      std::string sValue;

      // Operator Name
      if ( pVarHdr->GetHdrVar( "operatorName", sValue ) )    
      {
        m_sOperatorName = new String(sValue.c_str());
      }

      // Instaitution Name
      if ( pVarHdr->GetHdrVar( "institutionName", sValue ) )  
      {
        m_sInstitutionName = new String(sValue.c_str());
      }

      // Patient Name
      if ( pVarHdr->GetHdrVar( "patientName", sValue ) )
      {
        m_sPatientName = new String(sValue.c_str());
      }

      // Patient ID
      if ( pVarHdr->GetHdrVar( "patientID", sValue ) )
      {
        m_sPatientID = new String(sValue.c_str());
      }

      // Patient History
      if ( pVarHdr->GetHdrVar( "patientHistory", sValue ) ) 
      {
        m_sPatientHistory = new String(sValue.c_str());
      }

      // Patient Position
      if ( pVarHdr->GetHdrVar( "patientPosition", sValue ) ) 
      {
        m_sPatientPosition = new String(sValue.c_str());
      }


      // Study UID
      if ( pVarHdr->GetHdrVar( "studyUID", sValue ) )
      {
        m_sStudyUID = new String(sValue.c_str());
      }

      // Series UID
      if ( pVarHdr->GetHdrVar( "seriesUID", sValue ) )
      {
        m_sSeriesUID = new String(sValue.c_str());
      }

      // Study Date
      if ( pVarHdr->GetHdrVar( "studyDate", sValue ) )
      {
        m_sStudyDate = new String(sValue.c_str());
      }

      // Series Date
      if ( pVarHdr->GetHdrVar( "seriesDate", sValue ) )
      {
        m_sSeriesDate = new String(sValue.c_str());
      }

      // Patient Birth Date
      if ( pVarHdr->GetHdrVar( "patientBirthDate", sValue ) ) 
      {
        m_sPatientDOB = new String(sValue.c_str());
      }

      // Patient Age
      if ( pVarHdr->GetHdrVar( "patientAge", sValue ) ) 
      {
        m_sPatientAge = new String(sValue.c_str());
      }

      // Patient Age Units
      if ( pVarHdr->GetHdrVar( "patientAgeUnits", sValue ) ) 
      {
        m_sPatientAgeUnits = new String(sValue.c_str());
      }


      // Patient Sex
      if ( pVarHdr->GetHdrVar( "patientSex", sValue ) ) 
      {
        m_sPatientSex = new String(sValue.c_str());
      }

      // Referring Physician Name
      if ( pVarHdr->GetHdrVar( "physicianName", sValue ) )
      {
        m_sReferringPhysician = new String(sValue.c_str());
      }

      // Dicom Header
      if ( pVarHdr->GetHdrVar("dicomHeader", sValue) )
      {
        m_sDicomHeader = new String(sValue.c_str());
      } // END ... Dicom Header

       // mA
      if ( pVarHdr->GetHdrVar( "mA", sValue ) )
      {
        m_sMA = new String(sValue.c_str());
      }

      // sliceThickness
      if ( pVarHdr->GetHdrVar( "sliceThickness", sValue ) )
      {
        m_sSliceThickness = new String(sValue.c_str());
      }

      // pitch
      if ( pVarHdr->GetHdrVar( "pitch", sValue ) )
      {
        m_sPitch = new String(sValue.c_str());
      }

      // FOV
      if ( pVarHdr->GetHdrVar( "FOV", sValue ) )
      {
        m_sPitch = new String(sValue.c_str());
      }

      // Version
      if ( pVarHdr->GetHdrVar( "version", sValue ) )
      {
        m_sVersion = new String(sValue.c_str());
      }

      // SiteID
      if ( pVarHdr->GetHdrVar( "siteID", sValue ) )
      {
        m_sSiteID = new String(sValue.c_str());
      }


      // menufacturer
      if ( pVarHdr->GetHdrVar( "manufacturer", sValue ) )
      {
        m_sManufacturer = new String(sValue.c_str());
      }

      // tilt
      if ( pVarHdr->GetHdrVar( "tilt", sValue ) )
      {
        m_sTilt = new String(sValue.c_str());
      }

      // KV
      if ( pVarHdr->GetHdrVar( "kv", sValue ) )
      {
        m_sKV = new String(sValue.c_str());
      }

      // KV
      if ( pVarHdr->GetHdrVar( "studyType", sValue ) )
      {
        m_sStudyType = new String(sValue.c_str());
      }

      // zOrderReversed
      if ( pVarHdr->GetHdrVar( "zOrderReversed", sValue ) )
      {
        m_bZOrderReversed = (String::Compare((new String(sValue.c_str())),"1") == 0 ? true : false );
      }


      // patientBirthday
      if ( pVarHdr->GetHdrVar( "patientBirthday", sValue ) )
      {
        m_sAnonymousPatientDOB = new String(sValue.c_str());
      }

      // encryptedPatientName
      if ( pVarHdr->GetHdrVar( "encryptedPatientName", sValue ) )
      {
        m_sEncryptedPatientName = new String(sValue.c_str());
      }

      // encryptedPatientID
      if ( pVarHdr->GetHdrVar( "encryptedPatientID", sValue ) )
      {
        m_sEncryptedPatientID = new String(sValue.c_str());
      }

      // BodyPart
      if ( pVarHdr->GetHdrVar( "BodyPart", sValue ) )
      {
        m_sBodyPart = new String(sValue.c_str());
      }

      // uTotalNumberOfBlocks
      if ( pVarHdr->GetHdrVar( "uTotalNumberOfBlocks", sValue ) )
      {
        String * sStr(new String(sValue.c_str()));
        m_iTotalNumberOfBlocks = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
      }

      // uNumberOfUniqueBlocks
      if ( pVarHdr->GetHdrVar( "uNumberOfUniqueBlocks", sValue ) )
      {
        String * sStr(new String(sValue.c_str()));
        m_iNumberOfUniqueBlocks = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
      }

      // uNumberOfVoxelsPerBlock
      if ( pVarHdr->GetHdrVar( "uNumberOfVoxelsPerBlock", sValue ) )
      {
        String * sStr(new String(sValue.c_str()));
        m_iNumberOfVoxelsPerBlock = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
      }


      // uByteSizeOfIndexArray
      if ( pVarHdr->GetHdrVar( "uByteSizeOfIndexArray", sValue ) )
      {
        String * sStr(new String(sValue.c_str()));
        m_iByteSizeOfIndexArray = ( sStr->Length == 0 ? 0 : System::Convert::ToUInt32(sStr));
      }

    } // END ... If the varMap was found
    else
    {
      Log::Warning(String::Concat("No var map found in volume file [FILE=", sVolumeFile,"]"), "VolumeHeader", "InializeFromBlockVolume");
    }

  }
  catch ( System::Exception * ex )
  {
    Log::Error(String::Format("Could not read volume file [{0}] : {1}", sVolumeFile, ex->Message), "VolumeHeader", "InializeFromBlockVolume");
    throw;
  } // END ... catch Exception
  catch ( ... )
  {
    String * sErrorMessage = String::Format("Could not read volume file [FILE={0}] : unspecified exception", sVolumeFile);
    Log::Error(sErrorMessage, "VolumeHeader", "InitializeFromBlockVolume");
    throw new Exception(sErrorMessage);

  } // END ... catch everything else
  __finally 
  {
    // Clean up resources
    if ( pVolumeUint2 != NULL ) delete pVolumeUint2;
    if ( pVolumeInt2 != NULL )  delete pVolumeInt2;
    if ( pVolumeBool != NULL )  delete pVolumeBool;
    if ( pVolumeInt1 != NULL )  delete pVolumeInt1;
    if ( pVarMap != NULL )      delete pVarMap;
    if ( pVarHdr != NULL )      delete pVarHdr;
  } // END ... __finally

} // InitializeFromBlockVolume(String * sVolumeFile)


/**
 * Write the volume header to the logger
 */
void VolumeHeader::ToLogger()
{

  Log::Debug("VOLUME HEADER", "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Volume File ....................... {0}", m_sVolumeFile), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Type .............................. {0}", m_sType), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Dimension ......................... {0}", m_dim.ToString()), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Units ............................. {0}", m_units.ToString()), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Data Type ......................... {0}", m_sDataType), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Version ........................... {0}", m_sVersion), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Site ID ........................... {0}", m_sSiteID), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Study UID ......................... {0}", m_sStudyUID), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Series UID ........................ {0}", m_sSeriesUID), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Operator Name ..................... {0}", m_sOperatorName), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Institution Name .................. {0}", m_sInstitutionName), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient ID ........................ {0}", m_sPatientID), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient Name ...................... {0}", m_sPatientName), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient History ................... {0}", m_sPatientHistory), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Study Date ........................ {0}", m_sStudyDate), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Series Date ....................... {0}", m_sSeriesDate), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient DOB ....................... {0}", m_sPatientDOB), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient Age ....................... {0}", m_sPatientAge), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient Age Units ................. {0}", m_sPatientAgeUnits), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient Sex ....................... {0}", m_sPatientSex), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Referring Physician ............... {0}", m_sReferringPhysician), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("MA ................................ {0}", m_sMA), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("SliceThickness .................... {0}", m_sSliceThickness), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Pitch ............................. {0}", m_sPitch), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("FOV ............................... {0}", m_sFOV), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Manufacturer ...................... {0}", m_sManufacturer), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Tilt .............................. {0}", m_sTilt), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("KV ................................ {0}", m_sKV), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Patient Position .................. {0}", m_sPatientPosition), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Study Type ........................ {0}", m_sStudyType), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Anonymous Patient DOB ............. {0}", m_sAnonymousPatientDOB), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Z Order Reversed .................. {0}", __box(m_bZOrderReversed)), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Encrypted Patient Name ............ {0}", m_sEncryptedPatientName), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Encrypted Patient ID .............. {0}", m_sEncryptedPatientID), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Total Number Of Blocks ............ {0}", __box(m_iTotalNumberOfBlocks)), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Number Of Unique Blocks ........... {0}", __box(m_iNumberOfUniqueBlocks)), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Number Of Voxels Per Block ........ {0}", __box(m_iNumberOfVoxelsPerBlock)), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Byte Size Of Index Array .......... {0}", __box(m_iByteSizeOfIndexArray)), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Body Part ......................... {0}", m_sBodyPart), "VolumeHeader", "ToLogger");
  Log::Debug(String::Format("Dicom Header ...................... {0}", m_sDicomHeader), "VolumeHeader", "ToLogger");

} // ToLogger()

/**
 * Write the volume header to the logger
 */
String * VolumeHeader::ToArray() []
{

  StringCollection * pCol = new StringCollection();

  pCol->Add("VOLUME HEADER");
  pCol->Add("");
  pCol->Add(String::Format("Volume File ....................... {0}", m_sVolumeFile));
  pCol->Add(String::Format("Type .............................. {0}", m_sType));
  pCol->Add(String::Format("Dimension ......................... {0}", m_dim.ToString()));
  pCol->Add(String::Format("Units ............................. {0}", m_units.ToString()));
  pCol->Add(String::Format("Data Type ......................... {0}", m_sDataType));
  pCol->Add(String::Format("Version ........................... {0}", m_sVersion));
  pCol->Add(String::Format("Site ID ........................... {0}", m_sSiteID));
  pCol->Add(String::Format("Study UID ......................... {0}", m_sStudyUID));
  pCol->Add(String::Format("Series UID ........................ {0}", m_sSeriesUID));
  pCol->Add(String::Format("Operator Name ..................... {0}", m_sOperatorName));
  pCol->Add(String::Format("Institution Name .................. {0}", m_sInstitutionName));
  pCol->Add(String::Format("Patient ID ........................ {0}", m_sPatientID));
  pCol->Add(String::Format("Patient Name ...................... {0}", m_sPatientName));
  pCol->Add(String::Format("Patient History ................... {0}", m_sPatientHistory));
  pCol->Add(String::Format("Study Date ........................ {0}", m_sStudyDate));
  pCol->Add(String::Format("Series Date ....................... {0}", m_sSeriesDate));
  pCol->Add(String::Format("Patient DOB ....................... {0}", m_sPatientDOB));
  pCol->Add(String::Format("Patient Age ....................... {0}", m_sPatientAge));
  pCol->Add(String::Format("Patient Age Units ................. {0}", m_sPatientAgeUnits));
  pCol->Add(String::Format("Patient Sex ....................... {0}", m_sPatientSex));
  pCol->Add(String::Format("Referring Physician ............... {0}", m_sReferringPhysician));
  pCol->Add(String::Format("MA ................................ {0}", m_sMA));
  pCol->Add(String::Format("SliceThickness .................... {0}", m_sSliceThickness));
  pCol->Add(String::Format("Pitch ............................. {0}", m_sPitch));
  pCol->Add(String::Format("FOV ............................... {0}", m_sFOV));
  pCol->Add(String::Format("Manufacturer ...................... {0}", m_sManufacturer));
  pCol->Add(String::Format("Tilt .............................. {0}", m_sTilt));
  pCol->Add(String::Format("KV ................................ {0}", m_sKV));
  pCol->Add(String::Format("Patient Position .................. {0}", m_sPatientPosition));
  pCol->Add(String::Format("Study Type ........................ {0}", m_sStudyType));
  pCol->Add(String::Format("Anonymous Patient DOB ............. {0}", m_sAnonymousPatientDOB));
  pCol->Add(String::Format("Z Order Reversed .................. {0}", __box(m_bZOrderReversed)));
  pCol->Add(String::Format("Encrypted Patient Name ............ {0}", m_sEncryptedPatientName));
  pCol->Add(String::Format("Encrypted Patient ID .............. {0}", m_sEncryptedPatientID));
  pCol->Add(String::Format("Total Number Of Blocks ............ {0}", __box(m_iTotalNumberOfBlocks)));
  pCol->Add(String::Format("Number Of Unique Blocks ........... {0}", __box(m_iNumberOfUniqueBlocks)));
  pCol->Add(String::Format("Number Of Voxels Per Block ........ {0}", __box(m_iNumberOfVoxelsPerBlock)));
  pCol->Add(String::Format("Byte Size Of Index Array .......... {0}", __box(m_iByteSizeOfIndexArray)));
  pCol->Add(String::Format("Body Part ......................... {0}", m_sBodyPart));
  pCol->Add(String::Format("Dicom Header ...................... {0}", m_sDicomHeader));


  String * sLines [] = new String * [pCol->Count];

  pCol->CopyTo(sLines,pCol->Count);

  return sLines;
} // ToArray()


/**
 * Write the volume header to the logger
 */
DictionaryEntry VolumeHeader::get_Properties() []
{

  DictionaryEntry entries [] = new DictionaryEntry[40];

  entries[0]  = DictionaryEntry(S"volumeFile",m_sVolumeFile);
  entries[1]  = DictionaryEntry(S"type", m_sType);

  entries[2]  = DictionaryEntry(S"dataType", m_sDataType);
  entries[3]  = DictionaryEntry(S"version", m_sVersion);
  entries[4]  = DictionaryEntry(S"siteID", m_sSiteID);
  entries[5]  = DictionaryEntry(S"studyUID", m_sStudyUID);
  entries[6]  = DictionaryEntry(S"seriesUID", m_sSeriesUID);
  entries[7]  = DictionaryEntry(S"operatorName", m_sOperatorName);
  entries[8]  = DictionaryEntry(S"institutionName", m_sInstitutionName);
  entries[9]  = DictionaryEntry(S"patientID", m_sPatientID);
  entries[10] = DictionaryEntry(S"patientName", m_sPatientName);
  entries[11] = DictionaryEntry(S"patientHistory", m_sPatientHistory);
  entries[12] = DictionaryEntry(S"studyDate", m_sStudyDate);
  entries[13] = DictionaryEntry(S"seriesDate", m_sSeriesDate);
  entries[14] = DictionaryEntry(S"patientBirthDate", m_sPatientDOB);
  entries[15] = DictionaryEntry(S"patientAge", m_sPatientAge);
  entries[16] = DictionaryEntry(S"patientAgeUnits", m_sPatientAgeUnits);
  entries[17] = DictionaryEntry(S"patientSex", m_sPatientSex);
  entries[18] = DictionaryEntry(S"referringPhysician", m_sReferringPhysician);
  entries[19] = DictionaryEntry(S"mA", m_sMA);
  entries[20] = DictionaryEntry(S"sliceThickness", m_sSliceThickness);
  entries[21] = DictionaryEntry(S"pitch", m_sPitch);
  entries[22] = DictionaryEntry(S"FOV", m_sFOV);
  entries[23] = DictionaryEntry(S"manufacturer", m_sManufacturer);
  entries[24] = DictionaryEntry(S"tilt", m_sTilt);
  entries[25] = DictionaryEntry(S"KV", m_sKV);
  entries[26] = DictionaryEntry(S"patientPosition", m_sPatientPosition);
  entries[27] = DictionaryEntry(S"studyType", m_sStudyType);
  entries[28] = DictionaryEntry(S"anonymousPatientBirthDate", m_sAnonymousPatientDOB);
  entries[29] = DictionaryEntry(S"zOrderReversed", System::Convert::ToString(__box(m_bZOrderReversed)));
  entries[30] = DictionaryEntry(S"encryptedPatientName", m_sEncryptedPatientName);
  entries[31] = DictionaryEntry(S"encryptedPatientID", m_sEncryptedPatientID);
  entries[32] = DictionaryEntry(S"uTotalNumberOfBlocks", System::Convert::ToString(__box(m_iTotalNumberOfBlocks)));
  entries[33] = DictionaryEntry(S"uNumberOfUniqueBlocks", System::Convert::ToString(__box(m_iNumberOfUniqueBlocks)));
  entries[34] = DictionaryEntry(S"uNumberOfVoxelsPerBlock", System::Convert::ToString(__box(m_iNumberOfVoxelsPerBlock)));
  entries[35] = DictionaryEntry(S"uByteSizeOfIndexArray", System::Convert::ToString(__box(m_iByteSizeOfIndexArray)));
  entries[36] = DictionaryEntry(S"bodyPart", m_sBodyPart);
  entries[37] = DictionaryEntry(S"dicomHeader", m_sDicomHeader);
  entries[38] = DictionaryEntry(S"dimensions", m_dim.ToString());
  entries[39] = DictionaryEntry(S"units", m_units.ToString());


  return entries;
} // get_Properties()


// $Log: VolumeHeader.cpp,v $
// Revision 1.5.2.3  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.5.2.2  2008/08/07 19:19:31  mkontak
// Added methods for AddStudy and AddSeriesReferences without a connection string
// parameter.
//
// Revision 1.5.2.1  2008/08/07 15:28:02  mkontak
// Added units and dimension
//
// Revision 1.5  2007/03/09 21:07:03  mkontak
// Coding standards
//
// Revision 1.4  2006/10/10 16:37:58  romy
// modified the reading the header
//
// Revision 1.3  2006/02/09 14:10:37  geconomos
// removed using namepace from header
//
// Revision 1.2  2005/11/16 19:35:49  mkontak
// Missing the patient position
//
// Revision 1.1  2005/11/16 15:32:30  mkontak
// Added VolumeHeader utility
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Utilities/VolumeHeader.cpp,v 1.5.2.3 2011/05/18 01:56:59 mkontak Exp $
// $Id: VolumeHeader.cpp,v 1.5.2.3 2011/05/18 01:56:59 mkontak Exp $

