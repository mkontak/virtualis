// $Id: AnonymizationInformation.h,v 1.8.2.2 2007/07/03 18:29:53 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationInformation.h,v 1.8.2.2 2007/07/03 18:29:53 mkontak Exp $
//
// Copyright© (2004), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
// Co-Author: [Romy Sreedharan] [ romy@viatronix.com]

// pragma
#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;



// include
#include "SearchAndReplace.h"


// namespace
OPEN_ANMZN_NS 

/** 
 * Class used to define the fields to be anonymized. This wraps a hashtable that 
 * contains the search and replace object for all the fields to be anonymized.
 */
public ref class AnonymizationInformation
{
public:

  /// Constructor
  AnonymizationInformation(String ^ sSourcePath );

  /// Constructor
  AnonymizationInformation( String ^ sSourcePath, String ^ sPattern );

  /// Constructor
  AnonymizationInformation( String ^ sSourcePath, String ^ sPattern, 
                                  String ^ sAnonymizationConfigutaionFile );

  /// Destructor
  ~AnonymizationInformation() { this->!AnonymizationInformation(); }

  /// Finalizer
  !AnonymizationInformation() {}

  ///Gets the original folder name
  String ^ GetOldDirectory();

  ///Gets the gcnew folder name
  String ^ GetNewDirectory();

  /// Clears the fields
  void Clear() { m_pCurrentElementValues->Clear(); }

  /// Adds a gcnew field
  void Add(String ^ sName, SearchAndReplace ^ pSearchAndReplace);

  /// Returns an array of the search and replace objects
  array<SearchAndReplace ^> ^ GetSearchAndReplaceObjects(); 

  array<SearchAndReplace ^> ^ GetSearchAndReplaceSeriesUIDObjects();

  /// Logs the search and replace objects
  void ToLogger();

  /// Adds search and replace object based on the element list 
  void AddSearchAndReplaceObjects(  Viatronix::Dicom::ElementCollection ^ pOriginalElementList );

  /// Adds search and replace object based on the series list (This is for a series uid change) 
  void AddSearchAndReplaceObjects(  List<Viatronix::Enterprise::Entities::Series ^> ^ pSeriesCollection );

private:

  /// Initialize the objcet
  void Initialize();

 
public:

  /// Returns the number of fields to be anonymized
  property int Count
  {
      int get() { return m_pCurrentElementValues->Count; }
  }

  ///Returns the elements to be anonymized
  property Dictionary<String ^, SearchAndReplace ^> ^  CurrentElements
  {
      Dictionary<String ^, SearchAndReplace ^> ^ get() { return m_pCurrentElementValues; }
  }


  
  /// Gets the Study directory
  property SearchAndReplace ^ StudyDirectoryName
  {
    SearchAndReplace ^ get() { return  m_pCurrentElementValues["StudyDirectoryName"]; }
    void set(SearchAndReplace ^ pValue) { m_pCurrentElementValues->Add("StudyDirectoryName", pValue); }
  }



  /// Gets the Study date of birth
  property SearchAndReplace ^ StudyDob
  {
    SearchAndReplace ^ get() { return m_pCurrentElementValues["StudyDob"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("StudyDob", pSearchAndReplace); }
  }



  /// Gets the study date
  property SearchAndReplace ^ StudyDate
  {
    SearchAndReplace ^ get() { return m_pCurrentElementValues["StudyDate"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) {  m_pCurrentElementValues->Add( "StudyDate",pSearchAndReplace); }
  }


 
  /// Gets the study time
  property SearchAndReplace ^ StudyTime
  {
      SearchAndReplace ^ get()  { return  m_pCurrentElementValues["StudyTime"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("StudyTime",pSearchAndReplace); }
  }



  /// Gets the series date
  property SearchAndReplace ^ SeriesDate
  {
    SearchAndReplace ^ get()  { return  m_pCurrentElementValues["SeriesDate"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("SeriesDate",pSearchAndReplace); }
  }


 
  /// Gets the series time
  property SearchAndReplace ^ SeriesTime
  {
    SearchAndReplace ^ get()  { return  m_pCurrentElementValues["SeriesTime"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("SeriesTime",pSearchAndReplace); }
  }



  /// Gets the operator name
  property SearchAndReplace ^ OperatorName
  {
    SearchAndReplace ^ get()  { return m_pCurrentElementValues["OperatorName"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("OperatorName",pSearchAndReplace); }
  }



  /// Gets the institution name
  property SearchAndReplace ^ InstitutionName
  {
    SearchAndReplace ^ get()  { return  m_pCurrentElementValues["InstitutionName"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("InstitutionName",pSearchAndReplace); }
  }


  
  /// Gets the Study last name
  property SearchAndReplace ^ StudyLastName
  {
    SearchAndReplace ^ get()  { return m_pCurrentElementValues["StudyLastName"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("StudyLastName",pSearchAndReplace); }
  }


  /// Gets the Study last name
  property SearchAndReplace ^ StudyLastNamePartial
  {
    SearchAndReplace ^ get()   { return  m_pCurrentElementValues["StudyLastNamePartial"]; }
  }

  /// Gets the Study first name
  property SearchAndReplace ^ StudyFirstName
  {
    SearchAndReplace ^ get()  { return m_pCurrentElementValues["StudyFirstName"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("StudyFirstName",pSearchAndReplace); }
  }



  /// Gets the medical id
  property SearchAndReplace ^ MedicalId
  {
    SearchAndReplace ^ get() { return  m_pCurrentElementValues["MedicalId"]; }
    void set(SearchAndReplace ^ pSearchAndReplace)  { m_pCurrentElementValues->Add("MedicalId",pSearchAndReplace); }
  }



  /// Gets the series uid
  property SearchAndReplace ^ SeriesUID
  {
    SearchAndReplace ^ get()  { return  m_pCurrentElementValues["SeriesUID"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("SeriesUID",pSearchAndReplace); }
  }



  /// Gets the study uid
  property SearchAndReplace ^ StudyUID
  {
    SearchAndReplace ^ get()  { return  m_pCurrentElementValues["StudyUID"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("StudyUID",pSearchAndReplace); }
  }


  /// Gets the file name root
  property SearchAndReplace ^ FileNameRoot
  {
    SearchAndReplace ^ get()  { return  m_pCurrentElementValues["FileNameRoot"]; }
    void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("FileNameRoot",pSearchAndReplace); }
  }



  /// Sets the Study name
  property SearchAndReplace ^ StudyName
  {
    SearchAndReplace ^ get()  { return  m_pCurrentElementValues["StudyName"]; }
   void set(SearchAndReplace ^ pSearchAndReplace) { m_pCurrentElementValues->Add("StudyName",pSearchAndReplace); }
  }

   /// Returns the Original element list
  property Viatronix::Dicom::ElementCollection ^ OriginalElementList
  {
    Viatronix::Dicom::ElementCollection ^ get() { return m_pOriginalElementValues; }
  }


  /// Returns the anonymization element list
  property Viatronix::Dicom::ElementCollection ^ AnonymizationElementList
  {
    Viatronix::Dicom::ElementCollection ^ get() { return m_pNewElementValues; }
  }

  /// Returns the anonymization configuration file
  property String ^ AnonymizationConfigurationFile
  {
    String ^ get() { return m_sAnonymizationConfigurationFile; }
  }

  /// Returns the study path
  property String ^ SourcePath
  {
    String ^ get() { return m_sSourcePath; }
  }

  /// Returns the study path
  property String ^ FilePattern
  {
    String ^ get() { return m_sFilePattern; }
  }

  /// Returns the base path
  property String ^ BasePath
  {
    String ^ get() { return m_sBasePath; }
    void set(String ^ sPath) { m_sBasePath = sPath; }
  }

  /// Returns the destination
  property String ^ DestinationPath
  {
    String ^ get();
    void set(String ^ sPath) { m_sDestinationPath = sPath; }
  }

  ///Gets the wouldbe anonymized name
  property Viatronix::Enterprise::Entities::PersonName ^ NewName
  {
    Viatronix::Enterprise::Entities::PersonName ^ get();
  }

  ///Gets the Anonimized Medical ID
  property String ^  NewMedicalID
  {
    String ^ get();
  }


private:

  /// Anonymization List
  Viatronix::Dicom::ElementCollection ^ m_pNewElementValues;

  /// Anonymization List
  Viatronix::Dicom::ElementCollection ^ m_pOriginalElementValues;

  /// Anonimization Configuration File
  String ^ m_sAnonymizationConfigurationFile;

  /// Source path
  String ^ m_sSourcePath;

  /// File pattern
  String ^ m_sFilePattern;

  /// Base path
  String ^ m_sBasePath;

  /// Destination Path
  String ^ m_sDestinationPath;

    /// Old Study structure
  Viatronix::Enterprise::Entities::Study ^ m_pOldStudy;

  /// gcnew Study structure
  Viatronix::Enterprise::Entities::Study ^ m_pNewStudy;

  /// Search and replace objects for the anonymzation fields
  Dictionary<String ^, SearchAndReplace ^> ^ m_pCurrentElementValues;


};  // class AnonymizationInformation

CLOSE_ANMZN_NS

// $Log: AnonymizationInformation.h,v $
// Revision 1.8.2.2  2007/07/03 18:29:53  mkontak
// Issue 5707: Added two gcnew methods GetSearchAndReplaceSeriesUIDObjects() amd AddSearchAndReplaceObject()
// to facilitate the anonymization of the series UID's.
//
// Revision 1.8.2.1  2007/03/27 19:18:30  mkontak
// Fix anonymization of study uid
//
// Revision 1.8  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.7  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.6  2005/10/20 15:15:12  romy
// added methods to get anonymizedf and original Study Study Folder Names
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationInformation.h,v 1.8.2.2 2007/07/03 18:29:53 mkontak Exp $
// $Id: AnonymizationInformation.h,v 1.8.2.2 2007/07/03 18:29:53 mkontak Exp $