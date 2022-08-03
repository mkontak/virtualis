// $Id: Descriptions.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Descriptions.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright© (current year), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
//
// Complete history on bottom of file


// pragmas
#pragma once

// includes
#include "UtilityEnums.h"


OPEN_TYPES_NS

using namespace System::Collections;


/** 
 * Class used for converting enumerators to strings
 */
//public ref class Descriptions
//{
//
//public:
//
//
//  /// Returns the DATASET STATE's description
//  static String ^ GetDatasetStateDescription(DatasetState eDatasetState) { BuildTables(); return m_pDatasetStateDescription[((int)eDatasetState)]->ToString(); }
//  
//
//  /// Returns the MODULE's description
//  static String ^ GetModuleDescription(ModuleType eModuleType) { BuildTables(); return m_pModuleDescription[((int)eModuleType)]->ToString(); }
//  
//
//  /// Returns the MODULE STATE's descriptyion
//  static String ^ GetModuleStateDescription(ModuleState eModuleState) { BuildTables(); return m_pModuleStateDescription[((int)eModuleState)]->ToString(); }
//  
//
//  /// Returns the STUDY TYPE's description (Default is the Short Description)
//  static String ^ GetStudyTypeDescription(StudyType eStudyType) { return GetStudyTypeShortDescription(); }
//  
//
//  /// Returns the STUDY TYPES's short description
//  static String ^ GetStudyTypeShortDescription(StudyType eStudyType) { BuildTables(); return m_pStudyTypeShortDescription[((int)eStudyType)]->ToString(); }
//  
//
//  /// Returns the STUDY TYPE's long description
//  static String ^ GetStudyTypeLongDescription(StudyType eStudyType) { BuildTables(); return m_pStudyTypeLongDescription[((int)eStudyType)]->ToString(); }
//  
//
//  /// Returns the DATASET TYPE's short description
//  static String ^ GetDatasetShortDescription(DatasetType eDatasetType) { BuildTables(); return m_pDatasetTypeShortDescription->Item[((int)eDatasetType)]->ToString(); }
//  
//
//  /// Returns the DATASET TYPE's long description
//  static String ^ GetDatasetLongDescription(DatasetType eDatasetType) { BuildTables(); return m_pDatasetTypeLongDescription->Item[((int)eDatasetType)]->ToString(); }
//  
//
//private:
//
//  /// Builds the hash tables
//  static void BuildTables();
//
//private:
//
//  /// Flag used to determine if the tables are built (This was the tables are only built once).
//  static bool m_bTablesBuilt = false;
//  
//  /// Modules description table
//  static Hashtable ^ m_pModuleDescription = gcnew Hashtable();
//
//  /// Module State's description table
//  static Hashtable ^ m_pModuleStateDescription = gcnew Hashtable();
//
//  /// Dataset Type's long description
//  static Hashtable ^ m_pDatasetTypeLongDescription = gcnew Hashtable();
//
//  /// Dataset Type's short description
//  static Hashtable ^ m_pDatasetTypeShortDescription = gcnew Hashtable();
//
//  /// Study Type's long description
//  static Hashtable ^ m_pStudyTypeLongDescription = gcnew Hashtable();
//
//  /// Study Type's short description
//  static Hashtable ^ m_pStudyTypeShortDescription = gcnew Hashtable();
//
//  /// Dataset State's description
//  static Hashtable ^ m_pDatasetStateDescription = gcnew Hashtable();
//
//};  // Descriptions


CLOSE_TYPES_NS


// Revision History:
// $Log: Descriptions.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.1  2003/10/30 14:30:28  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Descriptions.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: Descriptions.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
