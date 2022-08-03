// $Id: AnonymizerVrx.cpp,v 1.13 2007/03/12 14:43:20 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
// Co-Author: [Romy Sreedharan] [ romy@viatronix.com]


// includes
#include "StdAfx.h"
#include "SearchAndReplace.h"
#include "AnonymizationInformation.h"
#include "Exceptions.h"
#include "AtlBase.h"
#include "AtlConv.h" 
#include "anonymizervrx.h"
#using <mscorlib.dll>

// namespaces
USING_ANMZN_NS
using namespace System;
using namespace System::Collections;
using namespace Viatronix::Logging;


/*
 ^ Constructor
 */
AnonymizerVrx::AnonymizerVrx(void)
{
}//AnonymizerVrx(void)

/*
 ^ Destructor
 */
AnonymizerVrx::~AnonymizerVrx(void)
{
}//~AnonymizerVrx(void)

/*
 ^ Constructor
 *
 ^ @param anonymizationInfo     Anonymization info object
 */
AnonymizerVrx::AnonymizerVrx( AnonymizationInformation ^ anonymizationInfo ) : Anonymizer( anonymizationInfo )
{
}//AnonymizerVrx( AnonymizationInformation ^ anonymizationInfo )

/*
 ^ Performs the anonymization
 *
 ^ @param sSourceFiles          List of source files to anonymize
 ^ @param sDestFiles            List of destination files
 */
void AnonymizerVrx::Anonymize(  List<String ^> ^ sSourceFiles, List<String ^>  ^ sDestFiles )
{
  String ^ message = "Anonymizing VRX Files ";
  //Anonymize each file from the List
  for ( int index = 0; index < sSourceFiles->Count ; index++ )
  {
    if ( IsAbort())
      throw  gcnew AbortException( " User aborted anonymization" ); 

    String ^ sSourceFile = sSourceFiles[index];
    Log::Information( String::Concat( message , sSourceFile ), "AnonymizerVrx", "Anonymize");

    //Build the destination File.
    String ^ sDestFile  = sDestFiles[index];
    Log::Information(String::Concat("Dest File" , sDestFile), "AnonymizerVrx", "Anonymize");

    System::IO::File::Copy( sSourceFile, sDestFile );
    //Raise the progress event.
    RaiseProgressEvent( message , index );

    AnonymizeVrxFile( sDestFile );
  }
} // Anonymize()


/*
 ^ Anonimizes the VRX file
 *
 ^ @param sDestFile     Destination file 
 *
 */
void AnonymizerVrx::AnonymizeVrxFile( String ^ sDestFile  )
{
USES_CONVERSION;

  String ^ message = "Anonymizing VRX Files ";
  //Anonymize each file from the List
    try
    {
      ReportLib::IReportPtr spReport;
      ReportLib::IReportRevisionPtr spRevision;

      if (FAILED(spReport.CreateInstance( __uuidof(ReportLib::Report))))
        throw gcnew AnonymizationException("failed to create Report object");
    
      spReport->Open( _bstr_t(GetUnManagedStdString(sDestFile).c_str()) );
      spRevision = spReport->Revisions->GetItem(0L);

      ReportLib::IReportInfoPtr spInfo = spRevision->Info;
      spInfo->PatientID = static_cast<LPCTSTR>(GetUnManagedStdString(m_mpAnonymizationInfo->AnonymizationElementList->PatientId).c_str());
      String ^ sLastFirst = String::Empty;

      
      Viatronix::Enterprise::Entities::PersonName ^ mpPatientName(gcnew Viatronix::Enterprise::Entities::PersonName(m_mpAnonymizationInfo->AnonymizationElementList->PatientName));

      if ( mpPatientName->LastName->Length > 0 )
        sLastFirst = mpPatientName->LastName;

      if ( mpPatientName->FirstName->Length > 0 )
      {
        if ( sLastFirst->Length > 0 )
          sLastFirst = String::Concat(sLastFirst, ", ", mpPatientName->FirstName);
        else
          sLastFirst = mpPatientName->FirstName;
      }

      spInfo->PatientName   = static_cast<LPCTSTR>(GetUnManagedStdString(sLastFirst).c_str());
      spInfo->PatientWeight = static_cast<LPCTSTR>("0");
      spInfo->PatientAge    = static_cast<LPCTSTR>("xxxx");
      spInfo->PatientSex    = ReportLib::Male;
      spInfo->ReferringDoctor = static_cast<LPCTSTR>(GetUnManagedStdString(m_mpAnonymizationInfo->CurrentElements["ReferringPhysician"]->Replace).c_str());

      spRevision->Save();
      spRevision->Close();
      spRevision = NULL;

      // If the report object is not nullptr then close report and clear
      if ( spReport != nullptr )
      {
        spReport->Close();
        spReport = NULL;
      }
    }
    catch ( AbortException ^ ex )
    {
      throw ex;
    }
    catch ( Exception ^ ex )
    {
      /// Delete the destintaion
      System::IO::File::Delete(sDestFile);
     // Log::Error(String::Concat("Failed to anonymize VRX file [", sSourceFile, "] : ", ex->Message));
      throw gcnew AnonymizationException( "Failed to anonymize VRX file ", ex );
    }
    catch ( _com_error & e )
    {
      /// Delete the destintaion
      System::IO::File::Delete(sDestFile);
      std::string sDesc(e.Description());
      String ^ sErrorMessage(String::Concat("Error anonymizing PDF file ... ", gcnew String(sDesc.c_str())));
      //Log::Error(String::Concat("Failed to anonymize VRX file [", sSourceFile, "] : ", sErrorMessage));
      throw gcnew AnonymizationException( sErrorMessage );
    }
    catch (...)
    {
      /// Delete the destintaion
      System::IO::File::Delete(sDestFile);
      //Log::Error(String::Concat("Failed to anonymize VRX file [", sSourceFile, "] : unspecified error"));
      throw gcnew AnonymizationException( "An un specified error occured in anonymizing VRX file " );
    } // catch ... 

} // AnonymizeVrxFile( String ^ sDestFile  )



// $Log: AnonymizerVrx.cpp,v $
// Revision 1.13  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.12  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.11  2006/07/21 21:30:14  romy
// modifeid project
//
// Revision 1.10  2006/07/21 16:17:23  romy
// fixed conversion issue
//
// Revision 1.7  2005/10/20 15:17:53  romy
// added abort method
//
// Revision 1.6  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerVrx.cpp,v 1.13 2007/03/12 14:43:20 mkontak Exp $
// $Id: AnonymizerVrx.cpp,v 1.13 2007/03/12 14:43:20 mkontak Exp $