// $Id: 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

// includes
#include "StdAfx.h"
#include "anonymizationhelper.h"
#include "AnonymizationInformation.h"
#include "Exceptions.h"

// namespaces
using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Collections::Generic;

using namespace Viatronix::Logging;
using namespace Viatronix::Utilities::IO;
using namespace Viatronix::Utilities::Registry;
using namespace Viatronix::Enterprise::Entities;

USING_ANMZN_NS

// defines
#ifdef CreateDirectory
#undef CreateDirectory
#endif

/*
 ^ Gets the Dicom File path
 *
 ^ @param  pSeries          Series to get the dicom series path for
 *
 ^ @param dicom series folder
 */
System::String ^ AnonymizationHelper::GetDicomFolder(Series ^ mpSeries)
{
  //If the Type is DICOM get the file from it
  Viatronix::Enterprise::Entities::Series ^ mpDicomSeries = nullptr;

  if(mpSeries->Type == SeriesTypes::Dicom)
  {
    mpDicomSeries = mpSeries;
  }
  else
  {
    mpDicomSeries = nullptr;
    for each ( Viatronix::Enterprise::Entities::Series ^ mpSeries in mpSeries->References )
    {
      if(mpSeries->Type == SeriesTypes::Dicom)
      {
        mpDicomSeries = mpSeries;
        break;
      }
    }
    if (mpDicomSeries == nullptr)
    {
      throw gcnew AnonymizationException("Could not find referenced DICOM series");
    }
  }

  String ^ mpDicomFolder = Path::Combine(mpDicomSeries->Url, "dicom");
  cli::array<String ^> ^ mpDicomFiles = Directory::GetFiles(mpDicomFolder, "*.dcm");
  if(mpDicomFiles->Length < 1)
  {
    throw gcnew AnonymizationException( "No valid DICOM file found in DICOM series.");
  }

  try
  {
    return mpDicomFolder;
  }
  catch(Exception ^ mpException)
  {
    throw gcnew AnonymizationException( String::Concat( "Exception occured in finding DICOM fodler from dicom file", mpException->Message ));
  }
} // GetDicomFolder( mpSeries )




/* 
 ^ Builds the destination Full path from the SourcePath and DestFolder.
 *
 ^ @param sSourcePath               Search path
 ^ @param sDestFolder               Destination flder
 ^ @param pDirctoryReplacements     Directory replacments strings
 *
 ^ @returns Destination full path
 */
String ^ AnonymizationHelper::BuildDestinationFilePath( String ^ sSourcePath, String ^ sDestFolder, List<SearchAndReplace ^> ^ pDirectoryReplacements )
{
  String ^  sCacheBasePath(SystemSettings::CurrentSystemSettings->CacheBasePath);
  String ^  sDestFile(sSourcePath->Replace( sCacheBasePath, System::String::Empty ));

  if ( sDestFile->Equals( sSourcePath ))
  {
    //There are cases whose fileNames doesn't start exactly as cachepath
    //e.g CachePath says d:\\studies but filepath goes like d:\studies
    String ^ sCacheBasePathException = sCacheBasePath->Replace( ":\\\\", ":\\" );
    sDestFile  = sDestFile->Replace( sCacheBasePathException, System::String::Empty ) ;
  }

  String ^  sDestFileRoot(sDestFile->Substring( 0, sDestFile->IndexOf(System::IO::Path::DirectorySeparatorChar))) ;
  String ^  sDirectoryName(String::Empty);
  try
  {
    for ( int i = 0; i < pDirectoryReplacements->Count; ++i )
    {
      SearchAndReplace ^ pSearchAndReplace(pDirectoryReplacements[i]);
      sDestFile =  sDestFile->Replace(pSearchAndReplace->Search, pSearchAndReplace->Replace);
    }

    sDestFile = PathUtilities::CombinePaths(sDestFolder,sDestFile);

    //sDestFile = sDestFile->Replace( sDestFileRoot, sReplaceWith );
    //sDestFile = System::IO::Path::Combine( sDestFolder, sDestFile ) ;

    //Do this beacause some of the file names are like "dicom\xyzuiii.dcm".
    sDirectoryName = System::IO::Path::GetDirectoryName ( sDestFile );
    if ( ! Directory::Exists ( sDirectoryName ) )
    {
      Directory::CreateDirectory ( sDirectoryName );
    }

  }
  catch ( Exception ^ ex )
  {
    Log::Error ( String::Concat( "Failed the creation of destination folder", sDirectoryName ) , "AnonymizerHelper", "BuildDestinationFilePath");
    throw gcnew AnonymizationException( "Failed the creation of destination folder", ex );
  }

  return sDestFile;


} // BuildDestinationFilePath( String ^ sSourcePath, String ^ sDestFolder, String ^ sReplaceWith , String ^ sSearchFor )


/*
 ^ Modify Series Collection. Modifies path of each series file which went through anonymization
 *
 ^ @param pSeriesCollection         List of seroes
 ^ @param pAnonymizationFileTable   File table
 ^ @param sAnmznFolder              Anonymization folder
 */
void AnonymizationHelper::ModifySeriesCollection ( List<Viatronix::Enterprise::Entities::Series ^>  ^ pSeriesCollection, Dictionary<String ^, List<AnonymizationData ^> ^> ^ pAnonymizationFilesTable, String ^ sAnmznFolder )
{
  //Holds the gcnew folder Name
  String ^ sStudyFolder(String::Empty);
  String ^ sBasePath(String::Empty);
  array<__wchar_t> ^ delimiter  =  { Path::DirectorySeparatorChar };

  //This is a nasty loop.
  //Modify the file path of each series by getting the same file from the Anonymization File collection.
  for ( int index=(0); index < pSeriesCollection->Count; index++ )
  {
    Viatronix::Enterprise::Entities::Series ^ pSeries(pSeriesCollection[index]);
    String ^ sSeriesPath(String::Empty); 
    String ^ sSeriesLocation(String::Empty); 
    
    //In the series update the files with gcnew file paths.
    for (int count(0); count < pSeries->Files->Count; count++ )
    {
      String ^ sSeriesFilePath(pSeries->Files[count]);
      bool bAnonymized(false);

      ///////////////////////////////////////
      // Loop through all the file maps
      //////////////////////////////////////
      for each ( KeyValuePair<String ^, List<AnonymizationData ^> ^> ^ mpItem in pAnonymizationFilesTable )
      {
        List<AnonymizationData ^> ^ pAnonymizationDataCollection(mpItem->Value);          

        for each (  AnonymizationData ^ mpAnonymizationData in pAnonymizationDataCollection ) 
        {
 
          //search the filescollection for appropriate files. If found add them in a subfileColl and then to the filePatternTable
          if ( pSeries->Equals ( mpAnonymizationData->Series ))
          {

            //int fileIndex(0);
            //for each ( String ^ sSourceFile in mpAnonymizationData->SourceFiles)
            //{

            //  if ( sSeriesFilePath->Equals( sSourceFile ) )
            //  {

            //    //Replace the series file with the destination File.
            //    String ^ sDestinationFile(mpAnonymizationData->DestinationFiles[fileIndex]);
            //    String ^ sSeriesFileName(pSeries->Files[count]);

            //    array<String ^> ^ sFields = sSeriesFileName->Split(delimiter);

            //    sSeriesPath = ( sDestinationFile->Replace( sSeriesFileName, String::Empty ))->TrimEnd( delimiter );

            //    sStudyFolder = Path::GetDirectoryName( sSeriesPath );
            //    sBasePath = Path::GetDirectoryName( sStudyFolder );

            //    //Update with the gcnew Location.
            //    String ^ sFileLocation = (sDestinationFile->Replace( sBasePath, String::Empty ))->TrimStart( delimiter );
            //    sSeriesLocation = (sFileLocation->Replace( sSeriesFileName, String::Empty ))->TrimEnd( delimiter );

            //    array<String ^> ^ sDestFields = sFileLocation->Split(delimiter);

            //    String ^ sNewFileName(String::Empty);
            //    
            //    int j(sDestFields->Length - sFields->Length);
            //    for ( int i(0); i < sFields->Length; ++i, ++j )
            //    {
            //      sNewFileName = String::Concat( ( sNewFileName->Length == 0 ? sNewFileName : String::Concat(sNewFileName,"\\") ) , sDestFields[j] ); 
            //    }
            //    
            //    pSeries->Files[count] = sNewFileName;

            //    bAnonymized = true;
            //    break;
            //  }// END ... If the series file equals the source file

            //  fileIndex++;
            //} // END ... For each series file

          } // END ... If the series equals the sanonymization series

          /// Break if anonymized
          if ( bAnonymized ) break;

        } // END ... FOr each data itm in the anonymization data collection

        /// Break is anonymized
        if ( bAnonymized ) break;

      } //  END ... While there are still file maps

      ////////////////////////////////////////
      // Check if we could not anoymize
      ////////////////////////////////////////
      if ( !bAnonymized )
      {
        //Remove files
        Viatronix::Logging::Log::Debug ( String::Concat ( "Removing file which didn't go through Anonymization " ,
                                                                pSeries->Files[count]->ToString()), "AnonymizerHelper", "ModifySeriesCollection");
                                                                pSeries->Files->RemoveAt(count--);
      }
    }// for (int count=0; count < series->Files->Count; count++ )

  
    String ^ sNewStudyLocation(Path::GetFileName(sStudyFolder));

 
    String ^ sOldStudyFolder(Path::GetDirectoryName(pSeries->Url));
    String ^ sOldPatientFolder(Path::GetDirectoryName(sOldStudyFolder));
    String ^ sOldStudyLocation(PathUtilities::CombinePaths(Path::GetFileName(sOldPatientFolder), Path::GetFileName(sOldStudyFolder)));
   
    //change the Full Path and Location on every Series.
    String ^ sPath = pSeries->Directory;
    sPath = sPath->Replace(sOldStudyLocation, sNewStudyLocation);

    // TODO: New What to do? 2 lines
    //pSeries->Path         = PathUtilities::CombinePaths(sBasePath, sPath);
    //pSeries->Directory    = PathUtilities::CombinePaths(sNewStudyLocation, Path::GetFileName(pSeries->Location));
    
  } // END .. For each seres in the coolectoin

}//ModifySeriesCollectionFiles ( SeriesCollection collectionToModify, System.Collections.Hashtable anonFilespathsMap )




///*  
// ^ Gets the Patient folder for the series 
// *
// ^ @param pSeries       Series to etract the patient folder from
// *
// ^ @return patient folder
// */
//String ^ AnonymizationHelper::GetPatientFolder ( Viatronix::Enterprise::Entities::Series ^ pSeries)
//{
//  if ( String::Compare ( pSeries->Url, String::Empty) == 0 )
//  {
//    Log::Error ( String::Concat ( " Cannot find patient folder for the Series which has an empty full path. Series: " ,  pSeries->Uid ), "AnonymizerHelper", "GetPatientFolder");
//    throw gcnew AnonymizationException ( "Cannot find patient folder for the Series which has an empty full path" );
//  }
//  return Path::GetDirectoryName( Path::GetDirectoryName( pSeries->Url ) );
//
//} // GetPatientFolder ( Series ^ pSeries)


/* 
 ^ Gets the study folder for the series 
 *
 ^ @param pSeries       Series to extract the study folder from 
 *
 ^ @return Study foler
 */
String ^ AnonymizationHelper::GetStudyFolder ( Viatronix::Enterprise::Entities::Series ^ pSeries)
{
  if ( String::Compare ( pSeries->Url, String::Empty) == 0 )
  {
    Log::Error ( String::Concat ( " Cannot find patient folder for the Series which has an empty full path. Series: " , pSeries->Uid ), "AnonymizerHelper", "GetStudyFolder");
    return String::Empty;
  }
  return System::IO::Path::GetDirectoryName( pSeries->Url ) ;

} // GetStudyFolder ( Series ^ pSeries)




/*
 ^ Extract Files which contains a matching pattern
 *
 ^ @param sSearchString           String to search for 
 ^ @param pFiles                  File list
 *
 ^ @return List of files mathich search string. 
 */
List<String ^> ^ AnonymizationHelper::ExtractFilesWithMatchingPattern( String ^ sSearchString, List<String ^>  ^ pFiles )
{
  List<String ^>  ^ pPatternFiles( gcnew  List<String ^> () );

  for( int index(0); ( index < pFiles->Count && pFiles->Count > 0); index++ )
  {
    String ^  sFile(pFiles[index]->ToLower());
    if( sFile->IndexOf( sSearchString->ToLower()) != -1 )
    {
      pPatternFiles->Add( pFiles[index] );
      pFiles->RemoveAt(index);
      index-- ;
    }
  } /// END ... For each file in list

  return pPatternFiles;

} // ExtractFilesWithMatchingPattern( String ^ sSearchString, ArrayList ^ pFiles )




/*
 ^ Extract Files that starts with a matching pattern 
 *
 ^ @param sSearchString           String to search for 
 ^ @param pFiles                  File list
 *
 ^ @return List of files mathich search string. 
*/
List<String ^>  ^ AnonymizationHelper::ExtractFilesStartsWithPattern( String ^ sSearchString, List<String ^>  ^ pFiles )
{

  List<String ^>  ^ pPatternFiles( gcnew  List<String ^>  () );

  for( int index(0); ( index < pFiles->Count && pFiles->Count > 0); index++ )
  {
    String ^  sFile(pFiles[index]->ToLower());
    if( sFile->StartsWith( sSearchString->ToLower()) )
    {
      pPatternFiles->Add( pFiles[index] );
      pFiles->RemoveAt(index);
      index-- ;
    }
  } /// END ... For each file in list

  return pPatternFiles;


}//ExtractFilesStartsWithPattern( String ^ searchString, System::Collections::ArrayList ^ files )


/**
 * Constructor.
 *
 * @param mpSourceFiles  The list of source files to be anonymized.
 * @param mpDestinationFiles  The list of destination files.
 * @param mpAnonymizationInfo  The requisite information.
 * @param mpSeries  The series that applies to this information.
 */
Viatronix::Anonymization::AnonymizationData::AnonymizationData(List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestinationFiles,
  Viatronix::Anonymization::AnonymizationInformation ^ mpAnonymizationInfo, Viatronix::Enterprise::Entities::Series ^ mpSeries) :
  m_mpAnmznInfo(mpAnonymizationInfo),
  m_mpSourceFiles(mpSourceFiles),
  m_mpDestinationFiles(mpDestinationFiles),
  // TODO: New ( series was cloned? Do we need to cloen it? )
  m_mpSeries(mpSeries)
{
}



// $Log: AnonymizationHelper.cpp,v $
// Revision 1.15.2.4  2011/05/18 01:56:57  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.15.2.3  2007/07/03 18:26:33  mkontak
// Issue 5707: Fixed the ModifySeriesCollection() method to take a generic list instead
// of an array and make sure that the series references and dependents locations and
// paths are updated with the necessary changes.
//
// Revision 1.15.2.2  2007/03/27 19:20:09  mkontak
// Fix anonymization of study
//
// Revision 1.15.2.1  2007/03/27 19:18:30  mkontak
// Fix anonymization of study uid
//
// Revision 1.15  2007/03/12 14:56:27  mkontak
// coding standards
//
// Revision 1.14  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.13  2007/03/07 17:29:34  romy
// A double check added for file names that start doesn't start with CachePath.
//
// Revision 1.12  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.11  2006/05/04 15:45:44  romy
// Proper error messaged added
//
// Revision 1.10  2006/04/10 15:35:47  romy
// added an error handling for empty dcm files
//
// Revision 1.9  2006/03/07 18:02:59  romy
// modified the way of getting referenced dcm file
//
// Revision 1.8  2006/03/07 15:53:07  romy
// modified the way of getting referenced dcm file
//
// Revision 1.7  2005/10/25 14:31:41  romy
// modify Series updated
//
// Revision 1.6  2005/10/20 15:14:30  romy
// modified ModifySeriesCollection method
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationHelper.cpp,v 1.15.2.4 2011/05/18 01:56:57 mkontak Exp $
// $Id: AnonymizationHelper.cpp,v 1.15.2.4 2011/05/18 01:56:57 mkontak Exp $