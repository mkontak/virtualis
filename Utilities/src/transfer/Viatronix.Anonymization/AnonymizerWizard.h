// $Id: AnonymizerWizard.h,v 1.14.2.1 2007/03/27 00:50:32 romy Exp $ 
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

using namespace System;
using namespace System::Windows::Threading;
using namespace System::Collections::Generic;

using namespace Viatronix::Console;
using namespace Viatronix::Enterprise::Entities;

// namespaces
OPEN_ANMZN_NS

// forward declarations
ref class AnonymizationInformation;
ref class AnonymizationEventArgs;
ref class AnonymizationData;
ref class Anonymizer;

/**
 * This class receieves a series collection and builds a map of Anonymizers against appropriate series files.
 * From the Map it anonymizes each file
 */
public ref class  AnonymizerWizard 
{
// delegates
public:

  /// Delegate for Progress Event.
  delegate void AnonymizationEventHandler(Object ^ sender, TransferProgressEventArgs ^ ae);

  /// Delegate to display the Field Selector Interface.
  //delegate void ShowFieldEditorInterface( String ^ dcmFilePath , AnonymizationInformation ^ anonymInfo );

// events
public:

  ///AnonymizationEventHandler delegate. 
  event AnonymizationEventHandler ^ anonymizationEvent;

// construction
public:

  /// Constructor.
  AnonymizerWizard(Dispatcher ^ mpDispatcher, String ^ rulesFileName );

// methods
public:

  /// Anonymize all the files
  void Anonymize(List<Series ^> ^ mpSeriesColl, String ^ mpDestination);

  /// Abort method
  void Abort ( bool abort );
    
private:
  
  /// Builds the the destination FIle collection based on the destinaiion folder.
  void BuildDestinationFileCollection(  List<String ^> ^ filesCollection,
    AnonymizationInformation ^ anonymInfo, String ^ destination, 
    List<String ^> ^ destinationFiles );

  /// Gets the overriding values from Anonymization
  // void GetOverridingValuesForAnonymization( Windows::Forms::Control ^ String ^ dcmFolder , AnonymizationInformation ^ anonymInfo );

  /// Build Rules Map
  void BuildAnonimizers ( List<String ^> ^ filesCollection, AnonymizationInformation ^ anonymInfo, 
                                  String ^ destination, Viatronix::Enterprise::Entities::Series ^ series );
  /// Build the anonymization rules map
  void BuildFilesForAnonymization( Viatronix::Enterprise::Entities::Series ^ series, 
    AnonymizationInformation ^ anonymInfo, String ^ destination);

  /// Anonymize all files which is already Built in the table..
  void Anonymize();

  /// Loads Rules from the XML
  void BuildAnonimizationRulesMap(  String ^ subType );
 
  /// Add Global Rules from Anonimization Table
  void AddGlobalPatternsToTable(  Xml::XmlDocument ^ rulesDocu );

  /// Update the anonimization table with the override rules
  //void UpdateTableWithOverrideRules(  Xml::XmlDocument ^ rulesDocu,   String ^ subType );

  ///Exclude the patterns from the table using Global exclusion list
  void RemoveGlobalblExPatternFromTable(  Xml::XmlDocument ^ rulesDocu );

  /// Exclude the patterns from the table using override exclusion list
  void RemoveOverrideExPatternFromTable(  Xml::XmlDocument ^ rulesDocu,  String ^ subType );

  /// Find the pattern and type from a node and add/exclude that to the anonimization Table
  void AddPatternsToRulesTable(  Xml::XmlNodeList ^ rulesNodes , bool exclude );

// internal handlers.
protected:
  
   /// Handles the Progress Event.
   virtual void OnProgressEvent(Object ^ sender, AnonymizationEventArgs ^ args); 
    
   /// Method which displays the fieldselector dialog.
   //virtual void ShowFieldEditor(String ^ dcmFilePath, AnonymizationInformation ^ anonymInfo);

// fields
private:

  /// The dispatcher 
  Dispatcher ^ m_mpDispatcher;

  /// Gets the total file count.
  int m_iTotalFileCount;
  
  /// Indicates the total progres count
  int m_iTotalProgress;

  /// File Pattern Table.
  Dictionary<String ^, List<AnonymizationData ^> ^> ^ m_mpFilePatternTable;
  
  /// Rules File Name;
  String ^ m_mpRulesFile;

  /// Map which keeps the pattern->Anonymizer Type rules
  Dictionary<String ^, String ^> ^ m_mpAnonimizationRules;

  /// Abort Flag which determines the Abort Decision.
  bool m_bAbort;

  /// The anonymizer object
  Anonymizer ^ m_mpAnonymizer;

}; // class AnonymizerWizard


CLOSE_ANMZN_NS


// $Log: AnonymizerWizard.h,v $
// Revision 1.14.2.1  2007/03/27 00:50:32  romy
// Anonimize object signature modified to receive the frame object
//
// Revision 1.14  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.13  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.12  2005/10/20 15:17:28  romy
// modified anonymize method
//
// Revision 1.11  2005/09/26 15:54:53  romy
// added abort mechanism
//
// Revision 1.10  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerWizard.h,v 1.14.2.1 2007/03/27 00:50:32 romy Exp $
// $Id: AnonymizerWizard.h,v 1.14.2.1 2007/03/27 00:50:32 romy Exp $