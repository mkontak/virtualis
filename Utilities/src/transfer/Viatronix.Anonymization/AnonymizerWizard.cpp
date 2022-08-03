// $Id: AnonymizerWizard.cpp,v 1.15.2.3 2007/07/03 18:32:20 mkontak Exp $ 
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
#using <mscorlib.dll>
#include "anonymizerwizard.h"
#include "anonymizer.h"
#include "AnonymizationInformation.h"
#include "anonymizationeventargs.h"
#include "anonymizationHelper.h"
#include "Exceptions.h"

// namespaces
using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Collections;
using namespace System::Windows::Threading;
using namespace System::Collections::Specialized;

using namespace Viatronix::UI;
using namespace Viatronix::Logging;

#ifdef CreateDirectory
#undef CreateDirectory
#endif

USING_ANMZN_NS

/*
 ^ Constructor
 *
 ^ @param mpRulesFile  rules file name
 */
 AnonymizerWizard::AnonymizerWizard(Dispatcher ^ mpDispatcher, String ^ mpRulesFile) : m_mpDispatcher(mpDispatcher), m_mpRulesFile(mpRulesFile)
{
  m_mpAnonimizationRules = gcnew Dictionary<String ^, String ^>();
  m_mpFilePatternTable   = gcnew Dictionary<String ^, List<AnonymizationData ^> ^>();
  m_iTotalFileCount = 0;
  m_iTotalProgress  = 0;
  m_bAbort = false;
  m_mpAnonymizer = nullptr;
} // AnonymizerWizard(  String ^ rulesFile )

/*
 ^ Abort method
 *
 ^ @param abort     Abort falg
 */
void AnonymizerWizard::Abort ( bool bAbort )
{
  m_bAbort = bAbort;
  if ( m_mpAnonymizer != nullptr )
    m_mpAnonymizer->Abort( true );
}//Abort ( bool abort )


/**
 * Anonymize SeriesCollection
 *
 * @param mpSeriesColl   The series collection.
 * @param mpDestination  Destination   
 */
void AnonymizerWizard::Anonymize(List<Series ^> ^ mpSeriesColl, String ^ mpDestination)
{
  try
  {
    String ^ mpSeriesRoot = String::Empty;
    String ^ mpStudyFolder = String::Empty;
    AnonymizationInformation ^ mpAnonymInfo = nullptr;
   
    OnProgressEvent ( this, gcnew AnonymizationEventArgs( "Gathering Anonymization data", 0 ) );

    //Prepare Anonymization Data for each Series.
    for	(	int	index=0; index < mpSeriesColl->Count;	index++	)
		{
      if ( m_bAbort ==	true )
        throw	 gcnew AbortException( " User	aborted	anonymization" );	

      Series ^ mpSeries = mpSeriesColl[index];
      
      // Make	sure all these series	are	from the same	patient.
      String ^ mpNewStudyFolder	=	AnonymizationHelper::GetStudyFolder( mpSeries);
      if (!mpStudyFolder->Equals(mpNewStudyFolder))
      {
        mpStudyFolder = mpNewStudyFolder;
        String ^ mpDcmFolder = AnonymizationHelper::GetDicomFolder(mpSeries);
        mpAnonymInfo = gcnew AnonymizationInformation(mpDcmFolder);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // If the series uid is to be changed then add the list of series with their ne UID's into the serahc and replace list
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(mpAnonymInfo->AnonymizationElementList->Contains(0x0020, 0x000e))
        {
          mpAnonymInfo->AddSearchAndReplaceObjects(mpSeriesColl);
        }

        // Overriding	reqd..for	AnonymInfo?
        //if(	fieldSelector	)
        //  GetOverridingValuesForAnonymization( frame,	dcmFolder,	anonymInfo );
      }

      if ( mpSeries->Files->Count	<= 0 ) 
        continue;
            
      //Builds a file	collection for anonymization based on	the	Map.
      BuildFilesForAnonymization(mpSeries,	mpAnonymInfo,	mpDestination);
		}

    //Anonymize based on the Anonymizer Table.
    Anonymize();

    //After anonymization Modify Series collection with the gcnew file names.
    AnonymizationHelper::ModifySeriesCollection(mpSeriesColl, m_mpFilePatternTable, mpDestination);
    
    //raise the status that Anonymization is over.
    OnProgressEvent ( this, gcnew AnonymizationEventArgs( "Finished Anonymization", m_iTotalFileCount ) );
  }
  catch ( AnonymizationException ^ ex )
  {
    Log::Error( String::Concat ( "Failed Anonymization", ex->Message ), "AnonymizerWizard", "Anonymize");
    throw ex;
  }
  catch ( AbortException ^ ex )
  {
    throw ex;
  }
  catch ( Exception ^ ex )
  {
    Log::Error( String::Concat ( "Unknown error during anonymization", ex->Message ), "AnonymizerWizard", "Anonymize");
    throw gcnew AnonymizationException ( String::Concat ( "Unknown error during anonymization", ex->Message )); ;
  }
} // Anonymize( mpSeriesCollection, mpDestination )


/*
* Creates a Map whiich contains paths for source and destination paths.
*
* @param sourceFiles          List of files to be anonymized
* @param pInfo                Anonymization information
* @param destination          Destination
* @param destinationFiles     Destination files
*/
void AnonymizerWizard::BuildDestinationFileCollection(  List<String ^> ^ sourceFiles, 
                                                        AnonymizationInformation ^ pInfo, 
                                                        String ^ destination,
                                                        List<String ^> ^ destinationFiles )
{
  try
  {
    for ( int index=0; index < sourceFiles->Count; index++ )
    {

      array<SearchAndReplace ^> ^ pInfoList = pInfo->GetSearchAndReplaceSeriesUIDObjects();

      System::Collections::Generic::List<SearchAndReplace ^> ^ pList = gcnew System::Collections::Generic::List<SearchAndReplace ^>();

     
      pList->Add(pInfo->StudyDirectoryName);
      
      for ( int i(0); i < pInfoList->Length; ++i )
      {
        pList->Add(pInfoList[i]);
      }
          
      //Build the destination File Path and Its folder.
      String ^ destFile    = AnonymizationHelper::BuildDestinationFilePath( sourceFiles[index], destination, pList );

      destinationFiles->Add( destFile );
    }
  }
  catch ( Exception ^ ex )
  {
    Log::Error ( String::Concat ( "Failed to Build Destination File Collection " , ex->Message->ToString() ), "AnonymizerWizard", "BuildDestinationFileCollection");
    throw gcnew AnonymizationException ( "Anonymization Failed", ex );
  }
}//BuildDestinationFileCollection( ....


///*
// ^ Gets the Overriding anonymization Information Values for anonymization.
// *
// ^ @ param frame         Frame control
// ^ @ param dcmFolder     Dicom  folder 
// ^ @ param anonymInfo    Anonymization information
// */
//void AnonymizerWizard::GetOverridingValuesForAnonymization( System::Windows::Forms::Control ^ frame, String ^ dcmFolder,
//                                                            AnonymizationInformation ^ anonymInfo )
//{
//  try
//  {
//		array<String ^> ^ files	=	System::IO::Directory::GetFiles	(	dcmFolder	);
//    if ( files->Length < 1 )
//      throw gcnew AnonymizationException ( " Series doesn't have any DICOM files " );
//
//    String ^ dcmFile; 
//
//    for each ( String ^ sFile in files )
//    {
//      //Assume the DCM File has .dcm extension always     
//      if ( sFile->ToLower()->EndsWith( ".dcm" ))
//        dcmFile = sFile;
//    }
//
//    if ( dcmFile == nullptr || dcmFile->Length == 0  )
//      throw gcnew AnonymizationException ( " Series doesn't have any DICOM files " );
//
//    ShowFieldEditorInterface ^ showInterface = gcnew ShowFieldEditorInterface( this, 
//                                                &Viatronix::v3D::Anonymization::AnonymizerWizard::ShowFieldEditor );
//
//    array<Object ^> ^ args =  gcnew array<Object ^>(2);
//    args[0] = dcmFile;
//    args[1] = anonymInfo;
//    frame->Invoke( showInterface, args );
//
//    Viatronix::Logging::Log::Debug ( " Anonymization Information from Anonymization Field Selector " , "AnonymizerWizard", "GetOverridingValuesForAnonymization");
//    anonymInfo->ToLogger();
//  }
//  catch ( Exception ^ ex )
//  {
//    Log::Error ( "error in Get Overriding Values " , "AnonymizerWizard", "GetOverridingValuesForAnonymization");
//    throw gcnew AnonymizationException ( "Failed to get overriding value", ex );
//  }
//} // GetOverridingValuesForAnonymization( System::Windows::Forms::Control ^ frame, String ^ dcmFilePath )
//
//
///*
//* Shows the field editor GUI
//*
//* @param dcmFilePath          Dicom file path
//* @param anonymInfo           Anonymous information
//*/
//void AnonymizerWizard::ShowFieldEditor( String ^ dcmFilePath, AnonymizationInformation ^ anonymInfo )
//{
//  if ( String::Compare( dcmFilePath, String::Empty ) !=0 )
//  {
//    FieldEditor ^ fldSelector = gcnew FieldEditor( anonymInfo, dcmFilePath );
//    fldSelector->ShowDialog();
//  }
//  else
//  {
//    Log::Error ( "Could not find a DCM Series for this collection ", "AnonymizerWizard", "ShowFieldEditor");
//    FileNotFoundException ^ ex = gcnew FileNotFoundException(" Could not find a DCM file in the series Collection" );
//    throw ex;
//  }
//}//ShowFieldEditor( String ^ dcmFilePath )


/**
 * Performs the anonymization
 */
void AnonymizerWizard::Anonymize()
{
  if ( m_mpFilePatternTable->Count <=0 ) 
    return;


  //raise the initial status of Anonymization 
  int progress = 1;
  for each (KeyValuePair<String ^, List<AnonymizationData ^> ^> ^ mpItem in m_mpFilePatternTable)
  {
    if(m_bAbort)
      throw  gcnew AbortException( " User aborted anonymization" ); 

    List<AnonymizationData ^> ^ anonymizationDataCollection(mpItem->Value);

    for ( int index =0; index < anonymizationDataCollection->Count; index++ )
    {
      AnonymizationData ^ anonymizationData = anonymizationDataCollection[index];
      array<Object ^> ^ arguments = gcnew array<Object ^>(1);
      arguments[0] = anonymizationData->AnonymizationInformation;
      try
      {
        m_mpAnonymizer = nullptr;
        String ^ mpAnonimizerType = mpItem->Key;
        Log::Information (String::Concat("Initializing Anonymizor of type ", mpAnonimizerType), "AnonymizerWizard", "Anonymize");
        
        try
        {
          m_mpAnonymizer = dynamic_cast<Anonymizer ^>(System::Activator::CreateInstance(System::Type::GetType(mpAnonimizerType), arguments));
        }
        catch ( Exception ^ ex )
        {
          Log::Error ( String::Concat ( " Invalid AnonymizerType: ", mpAnonimizerType ), "AnonymizerWizard", "Anonymize");
          throw  gcnew InvalidAnonymizorTypeException( " Invalid AnonymizerType", ex ); 
        }
        
        m_mpAnonymizer->SetDispatcher(m_mpDispatcher);

        m_mpAnonymizer->anonymizationProgressEvent += gcnew  Anonymizer::AnonymizationProgressEventHandler( this,
          &AnonymizerWizard::OnProgressEvent );

        m_mpAnonymizer->Anonymize( anonymizationData->SourceFiles, anonymizationData->DestinationFiles );
      }
      catch ( AbortException ^ ex )
      {
        throw ex;
      }
      catch ( InvalidAnonymizorTypeException ^ ex )
      {
        throw ex;
      }
      catch ( Exception ^ ex)
      {
        Log::Error ( String::Concat ( " Failed to anonymize  ", ex->Message ), "AnonymizerWizard", "Anonymize");
        throw  gcnew AnonymizationException( " Failed to anonymize ", ex ); 
      }
    }
  }
} // Anonymize()



/*
 ^ Handles the Progress Event.
 *
 ^ @param sender        Object sending evenet
 ^ @param args          Event args
 */
void AnonymizerWizard::OnProgressEvent( Object ^ sender, AnonymizationEventArgs ^ args ) 
{
  TransferProgressEventArgs ^ progressArgs = gcnew TransferProgressEventArgs( args->Message, m_iTotalProgress++, m_iTotalFileCount );
  anonymizationEvent( this, progressArgs ); 
}//OnProgressEvent( Object ^ sender, AnonymizationEventArgs ^ args ) 


/*
 ^ Builds files for  Anonymization
 ^ 
 ^ @param series        Series to extract files from
 ^ @param anonmINfo       Anoymization info
 ^ @param destination     Destination
 *
 ^ @return void
 */
void AnonymizerWizard::BuildFilesForAnonymization(Series ^ mpSeries, AnonymizationInformation ^ mpAnonymInfo, String ^ mpDestination)
{
  try
  {
    if ( mpAnonymInfo	== nullptr	)
				throw	gcnew	AnonymizationException(	"	Anonymization	error: Failed	to build anonymization Information.	"	);

    List<String ^> ^ mpSeriesFiles = gcnew List<String ^>();

    //Add series Files in to  the anonymization file List.
    // Series files are full path.
    for ( int index = 0; index < mpSeries->Files->Count; index++ )
      mpSeriesFiles->Add(mpSeries->Files[index]);

    //Add Patient files(patient.XMl) and Series Files (study.XML) in to the List.
    //String ^ patientFolder = AnonymizationHelper::GetPatientFolder ( series );

    //Get all Files under Patient Folder. 
    //array<String ^> ^ patientFiles = System::IO::Directory::GetFiles( patientFolder );
    //for each ( String ^ sFile in patientFiles )
    //{
    //  if ( !seriesFiles->Contains( sFile ))
    //    seriesFiles->Add ( sFile );
    //}

    //Get all Files under Study Folder. This is just to get the Study.XML
    array<String ^> ^ mpStudyFiles = System::IO::Directory::GetFiles(System::IO::Path::Combine(mpDestination, mpAnonymInfo->StudyUID->Search->Replace('.', '_')));
    for each(String ^ mpFile in mpStudyFiles)
    {
      if (!mpSeriesFiles->Contains(mpFile))
        mpSeriesFiles->Add(mpFile);
    }

    //Build the anonymizers for these files based on their subType.
    BuildAnonimizers(mpSeriesFiles, mpAnonymInfo, mpDestination, mpSeries);
  }
  catch( Exception ^ ex )
  {
    Log::Error ( ex->Message, "AnonymizerWizard", "BuildAnonimizersAndFilesMap");
    throw gcnew AnonymizationException( "Anonymization Failed" , ex );
  }
} // BuildAnonimizersAndFilesMap(  Series ^ series )




/*
 ^ Builds the Anonimization Rules Map
 *
 ^ @param filesCollection   collection of files to anonimize
 ^ @param anonymInfo        Anonymization info
 ^ @param destination       Destination
 ^ @param series            Series to build anonymizers from
 *
 ^ @return void
 */
void  AnonymizerWizard::BuildAnonimizers ( List<String ^> ^ files, AnonymizationInformation ^ anonymInfo, String ^ destination, Viatronix::Enterprise::Entities::Series ^ series )
{
  //Pick the file from the collection and get the Anonymizer type
  try
  {
    //Build the rules map.
    BuildAnonimizationRulesMap( series->Application );


    System::Collections::IDictionaryEnumerator ^ rules = m_mpAnonimizationRules->GetEnumerator();
    while( rules->MoveNext() )
    {
      String ^ pattern    = dynamic_cast<String^>(rules->Key);     
      String ^ anonymizer = dynamic_cast<String^>(rules->Value);          

      //search the filescollection for appropriate files. If found add them in a subfileColl and then to the filePatternTable
      List<String ^> ^ subFiles = gcnew List<String ^>();

      //Find the files based on associtaed pattern search
      if (pattern->StartsWith("*"))
        subFiles = AnonymizationHelper::ExtractFilesWithMatchingPattern( pattern->Substring( 1, pattern->Length - 1 ), files);
      else if (pattern->EndsWith("*"))
        subFiles = AnonymizationHelper::ExtractFilesStartsWithPattern( pattern->Substring(pattern->Length - 1, pattern->Length ), files);
      else
        subFiles = AnonymizationHelper::ExtractFilesWithMatchingPattern( pattern, files);

      if( subFiles->Count <= 0 )
        continue;
     
      m_iTotalFileCount += subFiles->Count;

      List<String ^> ^ destinationFiles = gcnew List<String ^>();
      BuildDestinationFileCollection( subFiles, anonymInfo,  destination, destinationFiles );

      List<AnonymizationData ^> ^ anonymizationDataCollection = gcnew List<AnonymizationData ^>();
      anonymizationDataCollection->Add( gcnew AnonymizationData ( subFiles, destinationFiles, anonymInfo, series));

      //Before adding the subFiles items check whether the same key already in the table.
      if ( !m_mpFilePatternTable->ContainsKey( anonymizer ))
        m_mpFilePatternTable[ anonymizer ] =  anonymizationDataCollection ;
      else
        m_mpFilePatternTable[ anonymizer ]->AddRange( anonymizationDataCollection );
    }
  }
  catch ( Exception ^ ex )
  {
    Log::Error ( ex->Message , "AnonymizerWizard", "BuildAnonimizer");
    throw gcnew AnonymizationException ( "Failed to build Anonymizer", ex );
  }

} // BuildAnonimizers ( ArrayList ^ filesCollection, AnonymizationInformation ^ anonymInfo, String ^ destination, Series ^ series )




/*
 ^ Builds the anonimization table from the Global Rules
 *
 ^ @param subType  study subtype string.
 */
void AnonymizerWizard::BuildAnonimizationRulesMap( String ^ mpSubType )
{
  Viatronix::Logging::Log::Debug( String::Concat( "Reads rules from anonimization file: " , m_mpRulesFile ), "AnonymizerWizard", "BuildAnonimizationRulesMap");

  try
  {
    XmlDocument ^ mpRulesDoc = gcnew XmlDocument();
    mpRulesDoc->Load( m_mpRulesFile );

    //Load the Global Rules.
    AddGlobalPatternsToTable( mpRulesDoc );

    //Load the override rules for the sub type.
    //UpdateTableWithOverrideRules( rulesDoc, subType );

    //Filter the rules using override Rules.
    RemoveOverrideExPatternFromTable( mpRulesDoc, mpSubType );

    //Filter the rules using Global rules.
    RemoveGlobalblExPatternFromTable( mpRulesDoc );

    //Log the anonimzation table for debug purposes.
    if( Viatronix::Logging::Log::GetLogLevel() >=  Viatronix::Logging::LogLevel::Debug )
    {
      System::Collections::IDictionaryEnumerator ^ myEnumerator = m_mpAnonimizationRules->GetEnumerator();
      if ( m_mpAnonimizationRules->Count > 0 ) 
        Viatronix::Logging::Log::Debug("Anonimization Table: \t-KEY-\t\t-VALUE-"  , "AnonymizerWizard", "BuildAnonimizationRulesMap");
      else
        Viatronix::Logging::Log::Debug(" "  , "AnonymizerWizard", "BuildAnonimizationRulesMap");

      while ( myEnumerator->MoveNext() )
        Viatronix::Logging::Log::Debug( String::Concat("pattern: " , myEnumerator->Key->ToString() , "  Anonymizer: " ,  myEnumerator->Value->ToString() ) , "AnonymizerWizard", "BuildAnonimizationRulesMap");
    }
  }
  catch( System::Xml::XmlException ^ ex )
  {
    Log::Error( ex->Message->ToString() , "AnonymizerWizard", "BuildAnonimizationRulesMap");
    throw gcnew AnonymizationException( String::Concat ( "Failed reading Anonymization rules." , ex->Message->ToString()));
  }

} // BuildAnonimizationRulesMap( String ^ subType )



/*
 ^ Builds the anonimization table from the Global Rules
 ^ 
 ^ @param rulesDoc   XmlDocument from the anonimization rules File.
 */
void AnonymizerWizard::AddGlobalPatternsToTable( XmlDocument ^ rulesDoc )
{
  Viatronix::Logging::Log::Debug( "Reads Global rule" , "AnonymizerWizard", "AddGlobalPatternsToTable");

  //reads the Global Pattern 
  XmlNodeList ^ rulesNode = rulesDoc->SelectNodes( "Anonymization/Rules/global/files/file" );
  AddPatternsToRulesTable ( rulesNode , false);

} // AddGlobalPatternsToTable( XmlDocument ^ rulesDoc )



/*
 ^ Exclude the patterns from the table using Global exclusion list
 *
 ^ @param rulesDoc   XmlDocument from the anonimization rules File.
 */
void AnonymizerWizard::RemoveGlobalblExPatternFromTable( XmlDocument ^ rulesDoc )
{
  Viatronix::Logging::Log::Debug( "Reads Global exclusion rule" , "AnonymizerWizard", "RemoveGlobalblExPatternFromTable");
  //reads the Global exclusion rules
  XmlNodeList ^ exclusionNode = rulesDoc->SelectNodes( "Anonymization/Rules/global/exclusions/exclusion" );

  //exclude patterns from the table
  AddPatternsToRulesTable ( exclusionNode , true );

} // RemoveGlobalblExPatternFromTable( XmlDocument ^ rulesDoc )



/*
 ^ Update the anonimization table with the override rules
 ^ 
 ^ @param rulesDoc    XmlDocument from the anonimization rules File.
 ^ @param subType     Study type string
 */
//void AnonymizerWizard::UpdateTableWithOverrideRules(  XmlDocument ^ rulesDoc, String ^ subType )
//{
//  Viatronix::Logging::Log::Debug( "Reads override rule" , "AnonymizerWizard", "UpdateTableWithOverrideRule");
//
//  for each ( XmlNode ^ mpNode in   rulesDoc->SelectNodes( "Anonymization/Rules/overrides/serie" ) )
//  {    
//  
//    //Match the sub type
//    if( String::Compare( mpNode->Attributes["subtype"]->Value, subType )== 0 )
//    {
//      XmlNodeList ^ seriesNodes =  mpNode->SelectNodes( "files/file" );
//      //Add patter
//      AddPatternsToRulesTable ( seriesNodes, false );
//      return;
//    }
//  }
//  Viatronix::Logging::Log::Debug( "No override rules found" , "AnonymizerWizard", "UpdateTableWithOverrideRule");
//
//} // UpdateTableWithOverrideRules( System::Xml::XmlDocument ^ rulesDoc, String ^ subType )


/*
 ^ Exclude the patterns from the table using override exclusion list
  *
 ^ @param rulesDoc   XmlDocument from the anonimization rules File.
 ^ @param subType    Study type string
 */
void AnonymizerWizard::RemoveOverrideExPatternFromTable( XmlDocument ^ rulesDoc,  String ^ subType )
{
  Viatronix::Logging::Log::Debug( "Reads override rule" , "AnonymizerWizard", "RemoveOverrideExPatternFromTable");


  
  for each ( XmlNode ^ mpNode in rulesDoc->SelectNodes( "Anonymization/Rules/overrides/serie" ) )
  {    
  
    //Match the subType
    if( String::Compare( mpNode["subtype"]->Value, subType )== 0 )
    {
      Viatronix::Logging::Log::Debug( "Reads override exclusion rule" , "AnonymizerWizard", "RemoveOverrideExPatternFromTable");

      //get the override exclusion list & remove it from the main list
      XmlNodeList ^ exclusionNode =  mpNode->SelectNodes( "exclusions/exclusion" );

      //Exclude Patterns From Rules Table
      AddPatternsToRulesTable ( exclusionNode , true );
      return;
    }
  }
  Viatronix::Logging::Log::Debug( "No override exclusion rules found" , "AnonymizerWizard", "RemoveOverrideExPatternFromTable");

} // RemoveOverrideExPatternFromTable( System::Xml::XmlDocument ^ rulesDoc, String ^ subType )



/*
 ^ Finds the pattern and type and add/remove them to the anonimization Table.
 *
 ^ @param rulesNode   XmlDocument from the anonimization rules File.
 ^ @param exclude     Flag which determines merge/remove options
 */
void AnonymizerWizard::AddPatternsToRulesTable(   XmlNodeList ^ rules , bool exclude )
{
  
  for each ( XmlNode ^ rule in rules ) 
  {    
    // Get a pointer to the node  
    
    String ^ pattern    = rule->Attributes[ "pattern" ]->Value->ToString();

    //split the pattern if there are many
    array<Char> ^ delimiter       = {','}; //comma delimiter
    array<String ^> ^ subPatterns = pattern->Split( delimiter );

    //Add to global rules
    for each ( String ^ subPattern in subPatterns )
    {
      if(!exclude)
        m_mpAnonimizationRules[ subPattern ] =  rule->Attributes["anonymizer"]->Value->ToString() ;
      else
        m_mpAnonimizationRules->Remove( subPattern );
    }
  }

} // AddPatternsToRulesTable( XmlNodeList ^ rulesNode )


// $Log: AnonymizerWizard.cpp,v $
// Revision 1.15.2.3  2007/07/03 18:32:20  mkontak
// Issue 5707: Makes use of the gcnew method  to add the series uid objects to the search and replace list as well some parameter changes
//
// Revision 1.15.2.2  2007/03/27 19:20:02  mkontak
// Fix anonymization of study
//
// Revision 1.15.2.1  2007/03/27 00:50:32  romy
// Anonimize object signature modified to receive the frame object
//
// Revision 1.15  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.14  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.13  2006/06/05 18:28:44  gdavidson
// ported to vs2005
//
// Revision 1.12  2006/03/07 15:52:40  romy
// modified the way of getting referenced dcm file
//
// Revision 1.11  2005/10/20 15:17:28  romy
// modified anonymize method
//
// Revision 1.10  2005/09/30 16:01:21  romy
// More exceptions. Added abort method
//
// Revision 1.9  2005/09/26 15:54:53  romy
// added abort mechanism
//
// Revision 1.8  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerWizard.cpp,v 1.15.2.3 2007/07/03 18:32:20 mkontak Exp $
// $Id: AnonymizerWizard.cpp,v 1.15.2.3 2007/07/03 18:32:20 mkontak Exp $