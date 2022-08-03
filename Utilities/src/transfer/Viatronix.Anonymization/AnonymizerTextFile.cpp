// $Id: AnonymizerTextFile.cpp,v 1.9.2.2 2009/07/10 15:41:02 kchalupa Exp $ 
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
#include "anonymizertextfile.h"
#include "Exceptions.h"

#using <mscorlib.dll>

// namespaces
using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace Viatronix::Logging;
USING_ANMZN_NS


/*
 ^ Constructor
 */
AnonymizerTextFile::AnonymizerTextFile()
{
}//AnonymizerTextFile()

/*
 ^ Constructor
 *
 ^ @param snonymizationInfo     Anonymization information 
 */
AnonymizerTextFile::AnonymizerTextFile ( AnonymizationInformation ^ anonymizationInfo ) : Anonymizer( anonymizationInfo )
{
} // AnonymizerTextFile ( AnonymizationInformation ^ anonymizationInfo )

/*
 ^ Destructor
 */
AnonymizerTextFile::~AnonymizerTextFile()
{
} // ~AnonymizerTextFile()

/*
 ^ Anonymize method
 *
 ^ @param sSourceFiles      Source files
 ^ @param sDestFiles        Destination files
 */
void AnonymizerTextFile::Anonymize(  List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles )
{
  String ^ message = "Anonymizing Text Files ";
  //Anonymize each file from the List
  for ( int index = 0; index < sSourceFiles->Count ; index++ )
  {
     if ( IsAbort())
      throw  gcnew AbortException( " User aborted anonymization" ); 

    System::String ^ sSourceFile = sSourceFiles[index];

    Log::Information(String::Concat( message , sSourceFile), "AnonymizerTextFile", "Anonymize");

    this->RaiseProgressEvent( message , index );
    
    // Streams
    FileStream ^ pSource = nullptr;
    FileStream ^ pDest   = nullptr;
    // Setrams Reader/Writers
    StreamReader ^ pReader  = nullptr;
    StreamWriter ^ pWriter  = nullptr;

    try
    {
      // Open a source stream
      pSource = gcnew FileStream( sSourceFile, FileMode::Open, FileAccess::Read, FileShare::None);

      // If the filename contains data tha needs to be changed then it will be done here
      System::String ^ sDestFile  = ReplaceDirectoryNames(sDestFiles[index]);

      // Open a destination stream
      pDest = gcnew FileStream( sDestFile, FileMode::Create, FileAccess::Write, FileShare::None);
      // Link the reader to the source
      pReader = gcnew StreamReader(pSource);
      // Link the writer to the destintion
      pWriter = gcnew StreamWriter(pDest);

      // Read all lines in the source file and perform all search and replace operations 
      // on them and write out the result to the destnination file.
      while ( pReader->Peek() > -1 )
        pWriter->WriteLine(ReplaceAll(pReader->ReadLine()));

    }
    catch ( AbortException ^ ex )
    {
      throw ex;
    }
    catch ( System::Exception ^ ex )
    {
      Log::Error( String::Concat( "Failed to anonymize dicom file ", sSourceFile, " : ", ex->Message ) , "AnonymizerTextFile", "Anonimize");
      throw gcnew AnonymizationException( "Failed to anonymize text/xml file ", ex );
    }
    __finally
    {
      // If the reader was created then close it
      if ( pReader != nullptr )
        pReader->Close();
      else if ( pSource != nullptr )
        pSource->Close();

      // If the writer was created then close it
      if ( pWriter != nullptr )
        pWriter->Close();
      else if ( pDest != nullptr )  // Close the destination stream
        pDest->Close();
    } // END ... __finally 
  }
} // Anonymize(  ArrayList ^ sSourceFiles, ArrayList ^ sDestFiles )


// $Log: AnonymizerTextFile.cpp,v $
// Revision 1.9.2.2  2009/07/10 15:41:02  kchalupa
// Issue #6315: Need to set source file opening to read only as opposed to read write.
//
// Revision 1.9.2.1  2007/07/03 18:37:02  mkontak
// Issue 5707: Makes use of the gcnew ReplaceDirectoryNames()method to anonymize the destination path.
//
// Revision 1.9  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.8  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.7  2005/10/20 15:18:16  romy
// added abort method
//
// Revision 1.6  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerTextFile.cpp,v 1.9.2.2 2009/07/10 15:41:02 kchalupa Exp $
// $Id: AnonymizerTextFile.cpp,v 1.9.2.2 2009/07/10 15:41:02 kchalupa Exp $