// $Id: Anonymizer.cpp,v 1.11.2.2 2007/07/03 18:35:03 mkontak Exp $ 
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
#include "anonymizationeventargs.h"
#include "Exceptions.h"
#include "anonymizer.h"
#using <mscorlib.dll>

// defines
#ifdef CreateDirectory
#undef CreateDirectory
#endif


// namesaces
USING_ANMZN_NS
using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace Viatronix::Dicom;
using namespace Viatronix::Logging;
using namespace Viatronix::Utilities;

/* 
 * Constructor
 */
Anonymizer::Anonymizer(void) : m_bAbort(false)
        {
  m_bAbort = false;
  m_mpDispatcher = nullptr;
} // Anonymizer(void)

/**
 * Constructor.
 *
 * @param aonymizationInfo    Anonymization information
 *
 */
Anonymizer::Anonymizer(AnonymizationInformation ^ anonymizationInfo) : m_bAbort(false)
{
  m_mpAnonymizationInfo =  anonymizationInfo; 
} // Anonymizer( anonymizationInfo )

/** 
 * Desturctor.
 */
Anonymizer::~Anonymizer()
{
} // ~Anonymizer()

/**
 * Abort method
 * 
 * @param bAbort       Abort flag
 */
void Anonymizer::Abort( bool bAbort )
{
  m_bAbort = bAbort;
}//Abort( bool abort )


/**
 * Returns the abort status
 *
 * @return true if aborting 
 */ 
bool Anonymizer::IsAbort()
{
  return m_bAbort;
} // IsAbort()


/**
 * Performs the search and replace operation for all the search and replace pairs in the list on the  supplied string. 
 *
 * @param mpOriginal  Original text to perform the search and replace operation(s) on.
 * @return  Returns a string that has been modified by performing all the search and replace operations in the list.
 */
String ^ Anonymizer::ReplaceAll(String ^ mpOriginal)
{
  String ^ mpReturn = mpOriginal;
  array<SearchAndReplace ^> ^ mpValues = m_mpAnonymizationInfo->GetSearchAndReplaceObjects();

  //////////////////////////////////////////////////////////////////////////////////////
  // Added code to not replace Viatronix just in case we anonymize one of our datasets
  ///////////////////////////////////////////////////////////////////////////////////////
  for each ( SearchAndReplace ^ mpValue in mpValues )
    mpReturn = ((mpValue->Search->Length > 0 && String::Compare(mpValue->Search, "Viatronix", true) != 0 && String::Compare(mpValue->Search, "v3D", true) != 0) ? mpReturn->Replace(mpValue->Search, mpValue->Replace) : mpReturn);
 
  return mpReturn;
} // ReplaceAll()


/**
 * Performs the search and replace operation for all the search and replace pairs in the list on the  supplied string.
 *
 * @param mpPath  Original text to perform the search and replace operation(s) on.
 * @return Returns a string that has been modified by performing all the search and replace operations in the list.
 */
String ^ Anonymizer::ReplaceDirectoryNames(String ^ sPath)
{

  String ^ mpReturn = String::Empty;

  array<Char> ^ mpSep = gcnew array<Char> { Path::AltDirectorySeparatorChar, Path::DirectorySeparatorChar };


  List<SearchAndReplace ^> ^ mpList = gcnew List<SearchAndReplace ^>();

  array<SearchAndReplace ^> ^ mpUIDS = m_mpAnonymizationInfo->GetSearchAndReplaceSeriesUIDObjects();

  // Populate the uids list inside the loop.
  for ( int i(0); i < mpUIDS->Length; mpList->Add(mpUIDS[i++]) );

  mpList->Add(m_mpAnonymizationInfo->StudyDirectoryName);

  array<String ^> ^ mpNames = sPath->Split(mpSep);

  for ( int i(0); i < mpNames->Length; ++i )
  {
    String ^ mpNew(mpNames[i]);

    for ( int j(0); j < mpList->Count; ++j )
    {
      // --------------------------------------------------------------------------------
      // The following line is correct.  It will fail only if an incomplete study is run.
      // ================================================================================
      SearchAndReplace ^ mpSRObject(mpList[i]);

      if ( String::Compare(mpSRObject->Search, mpNew, true) == 0 )
      {
        mpNew = mpSRObject->Replace;
        break;
      }
    }

    mpReturn = Viatronix::Utilities::IO::PathUtilities::CombinePaths(mpReturn, mpNew);
  }
 
  return mpReturn;
} // ReplaceDirectoryNames()


/*
 ^ Raise Progress Event
 *
 ^ @param sMessage        Progress message
 ^ @param iFileCount      File count
 */
void Anonymizer::RaiseProgressEvent(String ^ mpMessage, int iFileCount)
{
  AnonymizationEventArgs ^ mpArgs = gcnew AnonymizationEventArgs(mpMessage, iFileCount);
 
  // Now, create a public event  whose type is our AnonymizationEventHandler delegate. 
  anonymizationProgressEvent(this, mpArgs);
} // RaiseProgressEvent( mpMessage, iFileCount )


// $Log: Anonymizer.cpp,v $
// Revision 1.11.2.2  2007/07/03 18:35:03  mkontak
// Issue 5707: Added gcnew method ReplaceDirectoryNames() that will anonymize the individual directory names
// within the supplied path. Uses only search and replace object that are associated
// with our naming conventions.
//
// Revision 1.11.2.1  2007/03/27 00:49:12  romy
// added Frame object as a datamember
//
// Revision 1.11  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.10  2007/03/02 19:49:42  mkontak
// Do not anonymize Viatronix or V3D
//
// Revision 1.9  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.8  2005/11/15 19:54:06  mkontak
// Moved the GetVolumeType() and GetVolumeDataType() out of anonymization
// and into utilties.
//
// Revision 1.7  2005/10/20 15:15:47  romy
// added abort methid
//
// Revision 1.6  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/Anonymizer.cpp,v 1.11.2.2 2007/07/03 18:35:03 mkontak Exp $
// $Id: Anonymizer.cpp,v 1.11.2.2 2007/07/03 18:35:03 mkontak Exp $