// $Id: 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

// pragmas
#pragma once

// namespaces
using namespace System;
using namespace System::Collections::Generic;

using namespace Viatronix::Enterprise::Entities;

OPEN_ANMZN_NS 

// forward declarations
ref class AnonymizationInformation;
ref class SearchAndReplace;
ref class AnonymizationData;

/**
 ^ Anonymization helper object provides static methods to help in the process of anonymization.
 */
public ref class AnonymizationHelper
{

public:
  
  ///Gets the DICOM Series Folder.
  static String ^ AnonymizationHelper::GetDicomFolder(Series ^ mpSeries);

  /// Modifies the series collection with gcnew file names
  static void ModifySeriesCollection(List<Series ^> ^ mpSeriesCollection,
    Dictionary<String ^, List<AnonymizationData ^> ^> ^ mpAnonymizationFilesTable, String ^ mpAnmznfolder);
  
  ///Builds destination files path by replacing strings from source path
  static String ^ BuildDestinationFilePath(String ^ mpSourcePath, String ^ mpDestFolder, List<SearchAndReplace ^> ^ mpDirectoryReplacements);

  ///Gets the Patient Folder of a series
  //static String ^ GetPatientFolder(Series ^ mpSeries);

  ///Gets the study folder for a series.
  static String ^ GetStudyFolder(Series ^ mpSeries);

   ///Extract Files with a pattern match
  static List<String ^>  ^ ExtractFilesWithMatchingPattern(String ^ mpSearchString, List<String ^> ^ mpFiles);

  ///Extract Files which starts with a pattern
  static List<String ^>  ^ ExtractFilesStartsWithPattern(String ^ mpSearchString, List<String ^> ^ mpFiles);

}; // class AnonymizationHelper

/**
 * Implements an object that holds the Anonymization Data
 */
public ref class AnonymizationData
{
// construction
public:

  /// Constructor
  AnonymizationData(List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestinationFiles,
    AnonymizationInformation ^ mpAnonymizationInfo, Series ^ mpSeries);

// properties
public:

  ///Gets the Series object
  property Viatronix::Enterprise::Entities::Series ^ Series
  {
    Viatronix::Enterprise::Entities::Series ^ get() { return m_mpSeries; }
  } // Series


  ///Gets the SourceFiles list
  property List<String ^>  ^ SourceFiles
  {
    List<String ^> ^ get() { return m_mpSourceFiles; }
  } // SourceFiles


  /// Gets the Destination Files list
  property List<String ^>  ^  DestinationFiles
  {
    List<String ^>  ^ get() { return m_mpDestinationFiles; }
  } // DestinationFiles


  /// Gets the anonymization Information.
  property Viatronix::Anonymization::AnonymizationInformation ^ AnonymizationInformation
  {
    Viatronix::Anonymization::AnonymizationInformation ^ get() { return m_mpAnmznInfo; }
  } // AnonymizationInformation

// fields
private:

  ///Series object.
  Viatronix::Enterprise::Entities::Series ^ m_mpSeries;

  ///Source Files object.
  List<String ^> ^ m_mpSourceFiles;

  ///Destination Files object.
  List<String ^> ^ m_mpDestinationFiles;

  //Holds the anonymization Information which reads from config file or from the user.
  Viatronix::Anonymization::AnonymizationInformation ^ m_mpAnmznInfo;

}; // class AnonymizationData

CLOSE_ANMZN_NS


// $Log: AnonymizationHelper.h,v $
// Revision 1.9.2.2  2007/07/03 18:26:33  mkontak
// Issue 5707: Fixed the ModifySeriesCollection() method to take a generic list instead
// of an array and make sure that the series references and dependents locations and
// paths are updated with the necessary changes.
//
// Revision 1.9.2.1  2007/03/27 19:18:30  mkontak
// Fix anonymization of study uid
//
// Revision 1.9  2007/03/12 14:56:27  mkontak
// coding standards
//
// Revision 1.8  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.7  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.6  2005/10/20 15:14:30  romy
// modified ModifySeriesCollection method
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationHelper.h,v 1.9.2.2 2007/07/03 18:26:33 mkontak Exp $
// $Id: AnonymizationHelper.h,v 1.9.2.2 2007/07/03 18:26:33 mkontak Exp $