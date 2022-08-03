// $Id: AnonymizerDicom.cpp,v 1.9.2.2 2007/07/03 18:37:02 mkontak Exp $ 
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
#include "anonymizerdicom.h"
#include "Exceptions.h"

// namespaces
#using <mscorlib.dll>
using namespace System;
using namespace System::Collections;
using namespace Viatronix::Logging;

USING_ANMZN_NS

/*
 ^ Constructor
 */
AnonymizerDicom::AnonymizerDicom(void)
{
}//AnonymizerDicom(void)

/*
 ^ Destructor
 */
AnonymizerDicom::~AnonymizerDicom(void)
{
}//~AnonymizerDicom(void)

/*
 ^ Constructor
 *
 ^ @param anonymizationInfo   Anonymization information
 *
 */
AnonymizerDicom::AnonymizerDicom( AnonymizationInformation ^ anonymizationInfo ) : Anonymizer( anonymizationInfo )
{
} // AnonymizerDicom( AnonymizationInformation ^ anonymizationInfo )

/*
 ^ Anonymizes DIcom Files.
 *
 ^ @param sSourceFiles          List of source files
 ^ @param sDestFiles            List of destination files
 *
 */
void AnonymizerDicom::Anonymize(  List<String ^> ^ sSourceFiles,  List<String ^> ^ sDestFiles )
{
  System::String ^ message = "Anonymizing DICOM Files " ;

  for ( int index = 0; index < sSourceFiles->Count ; index++ )
  {
    if ( IsAbort())
      throw  gcnew AbortException( " User aborted anonymization" ); 

    System::String ^ sSourceFile = sSourceFiles[index];

    Log::Information( String::Concat( message , sSourceFile ), "AnonymizerDicom", "Anonymize");

    //Raise the progress event.
    this->RaiseProgressEvent( message , index );
    
    Log::Debug(String::Concat("Attempting to anonymize dicom file ", sSourceFile), "AnonymizerDicom", "Anonymize");

    Viatronix::Dicom::Image ^ pImage((Viatronix::Dicom::Image ^)Viatronix::Dicom::ImageFactory::Create( sSourceFile, Viatronix::Dicom::ParameterType::FILE  ));

    try
    {
      Log::Debug( "Anonymizing dicom image"  , "AnonymizerDicom", "Anonymize");
      // TODO: REST
      // pImage->Modify( m_pAnonymizationInfo->AnonymizationElementList->ToXml() );

      System::String ^ sDestFile  = ReplaceDirectoryNames(sDestFiles[index]);

      Log::Debug( String::Concat( "Writing anonymized dicom to ", sDestFile ) , "AnonymizerDicom", "Anonymize");
      pImage->Save( sDestFile );


    }
    catch ( AbortException ^ ex )
    {
      throw ex;
    }
    catch ( Exception ^ ex )
    {
      Log::Error( String::Concat( "Failed to anonymize dicom file ", sSourceFile, " : ", ex->Message ) , "AnonymizerDicom", "Anonymize");
      throw gcnew AnonymizationException( "Failed to anonymize dicom file ", ex );
    }
    __finally
    {
      pImage = nullptr;
      Log::Debug( String::Concat( "Done anonymizing ", sSourceFile ), "AnonymizerDicom", "Anonymize");
    }
  }
} // Anonymize(  ArrayList ^ sSourceFiles, ArrayList ^ sDestFiles )

// $Log: AnonymizerDicom.cpp,v $
// Revision 1.9.2.2  2007/07/03 18:37:02  mkontak
// Issue 5707: Makes use of the gcnew ReplaceDirectoryNames()method to anonymize the destination path.
//
// Revision 1.9.2.1  2007/03/22 03:33:42  mkontak
// Fix issue with anonymization
//
// Revision 1.9  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.8  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.7  2005/10/20 15:16:12  romy
// added progress events and Exceptions
//
// Revision 1.6  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerDicom.cpp,v 1.9.2.2 2007/07/03 18:37:02 mkontak Exp $
// $Id: AnonymizerDicom.cpp,v 1.9.2.2 2007/07/03 18:37:02 mkontak Exp $