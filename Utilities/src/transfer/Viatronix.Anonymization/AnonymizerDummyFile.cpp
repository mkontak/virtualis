// $Id: AnonymizerDummyFile.cpp,v 1.5.2.1 2007/07/03 18:37:02 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Romy Sreedharan] [ romy@viatronix.com]


// includes
#include "StdAfx.h"
#include "SearchAndReplace.h"
#include "AnonymizationInformation.h"
#include "AnonymizerDummyFile.h"
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
AnonymizerDummyFile::AnonymizerDummyFile()
{
}//AnonymizerDummyFile

/*
 ^ Constructor
 *
 ^ @param anonymizationInfo       Anonymization information object
 */
AnonymizerDummyFile::AnonymizerDummyFile ( AnonymizationInformation ^ anonymizationInfo ) : Anonymizer( anonymizationInfo )
{
} // AnonymizerDummyFile ( AnonymizationInformation ^ anonymizationInfo )

/*
 ^ Destructor
 */
AnonymizerDummyFile::~AnonymizerDummyFile()
{
}//~AnonymizerDummyFile()


/*
 ^ Anonymizes Files.
 ^ @param sSourceFiles        List of source files
 ^ @param sDestFiles          List of destination files
 */
void AnonymizerDummyFile::Anonymize(  List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles )
{
  System::String ^ message = "Additional files are transferring...";

  for( int index=0; index< sSourceFiles->Count; index++ )
  {
    this->RaiseProgressEvent( message , index );
    System::IO::File::Copy( sSourceFiles[index], ReplaceDirectoryNames(sDestFiles[index]) );
  }
} // Anonymize(  ArrayList ^ sSourceFiles, ArrayList ^ sDestFiles )

// $Log: AnonymizerDummyFile.cpp,v $
// Revision 1.5.2.1  2007/07/03 18:37:02  mkontak
// Issue 5707: Makes use of the gcnew ReplaceDirectoryNames()method to anonymize the destination path.
//
// Revision 1.5  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.4  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.3  2006/04/06 18:50:30  romy
// header change
//
// Revision 1.2  2006/02/27 20:54:35  romy
// initial version
//
// Revision 1.1  2006/02/27 17:08:20  romy
// initial version
//
// Revision 1.7  2005/10/20 15:18:16  romy
// added abort method
//
// Revision 1.6  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerDummyFile.cpp,v 1.5.2.1 2007/07/03 18:37:02 mkontak Exp $
// $Id: AnonymizerDummyFile.cpp,v 1.5.2.1 2007/07/03 18:37:02 mkontak Exp $