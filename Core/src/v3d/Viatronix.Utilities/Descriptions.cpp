// $Id: Descriptions.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Descriptions.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright© (current year)), Viatronix Inc::), All Rights Reserved::
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc:: The contents of this file
// may not be disclosed to third parties), copied or duplicated in any form,
// in whole or in part), without the prior written permission Viatronix Inc::
//
// Author: [Mark Kontak] [mkontak@viatronix::com]
//
// Complete history on bottom of file


// Includes
#include "Stdafx.h"
#include "Descriptions.h"


// namespaces
using namespace System;

USING_TYPES_NS



/** 
 * Builds the static hash tables to returns the descriptions
 */
//void Descriptions::BuildTables()
//{
//
//  if ( ! m_bTablesBuilt )
//  {
//
//    // build the module description hashtable
//    m_pModuleDescription->Add( ((int)ModuleType::Conversion),  gcnew String("Conversion") );
//    m_pModuleDescription->Add( ((int)ModuleType::Pipeline),    gcnew String("Pipeline") );
//    m_pModuleDescription->Add( ((int)ModuleType::AAAPipeline), gcnew String("Pipeline") );
//    m_pModuleDescription->Add( ((int)ModuleType::Receiver),    gcnew String("Dicom Receiver") );
//
//    // build the dataset long description hashtable
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Undefined),         gcnew String("Undefined") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Supine),            gcnew String("Supine") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Prone),             gcnew String("Prone") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Scout),             gcnew String("Scout") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::NonContrast),       gcnew String("Non-Contrast") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::ImmediateContrast), gcnew String("Immediate-Contrast") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::DelayedContrast),   gcnew String("Delayed-Contrast") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Prospective),       gcnew String("Prospective") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Retrospective),     gcnew String("Retrospective") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Explorer),          gcnew String("Explorer") );
//    m_pDatasetTypeLongDescription->Add( ((int)DatasetType::Vascular),          gcnew String("Vascular") );
//
//    // build the dataset short description hashtable
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Undefined),         gcnew String("Undefined") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Supine),            gcnew String("Supine") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Prone),             gcnew String("Prone") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Scout),             gcnew String("Scout") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::NonContrast),       gcnew String("NC") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::ImmediateContrast), gcnew String("IC") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::DelayedContrast),   gcnew String("DC") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Prospective),       gcnew String("Prospect") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Retrospective),     gcnew String("Retrospect") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Explorer),          gcnew String("Explorer") );
//    m_pDatasetTypeShortDescription->Add( ((int)DatasetType::Vascular),          gcnew String("Vascular") ); 
//
//    // build the state description hashtable
//    m_pDatasetStateDescription->Add( ((int)DatasetState::Completed),           gcnew String("Completed") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::Deleted),             gcnew String("Deleted Dataset") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::FailedConversion),    gcnew String("Failed Conversion") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::FailedPipeline),      gcnew String("Failed Pipeline") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::FinishedConversion),  gcnew String("Pending Pipeline") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::FinishedPipeline),    gcnew String("Finished Pipeline") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::FinishedReceiving),   gcnew String("Pending Conversion") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::StartedConversion),   gcnew String("Started Conversion") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::StartedPipeline),     gcnew String("Started Pipeline") );
//    m_pDatasetStateDescription->Add( ((int)DatasetState::StartedReceiving),    gcnew String("Receiving") );
//
//    // build the study long description hashtable
//    m_pStudyTypeLongDescription->Add( ((int)StudyType::AAA),            gcnew String("Aorta") );
//    m_pStudyTypeLongDescription->Add( ((int)StudyType::CalciumScoring), gcnew String("Calcium Scoring") );
//    m_pStudyTypeLongDescription->Add( ((int)StudyType::Colonoscopy),    gcnew String("Colon") );
//    m_pStudyTypeLongDescription->Add( ((int)StudyType::Explorer),       gcnew String("Explorer") );
//    m_pStudyTypeLongDescription->Add( ((int)StudyType::Unknown),        gcnew String("Undefined") );
//    m_pStudyTypeLongDescription->Add( ((int)StudyType::Vascular),       gcnew String("Vascular") );
//    m_pStudyTypeLongDescription->Add( ((int)StudyType::Angiography),    gcnew String("Angiography") );
//
//    // build the study short description hashtable
//    m_pStudyTypeShortDescription->Add( ((int)StudyType::AAA),             gcnew String("AAA") );
//    m_pStudyTypeShortDescription->Add( ((int)StudyType::CalciumScoring),  gcnew String("CS") );
//    m_pStudyTypeShortDescription->Add( ((int)StudyType::Colonoscopy),     gcnew String("VC") );
//    m_pStudyTypeShortDescription->Add( ((int)StudyType::Explorer),        gcnew String("XP") );
//    m_pStudyTypeShortDescription->Add( ((int)StudyType::Unknown),         gcnew String("UND") );
//    m_pStudyTypeShortDescription->Add( ((int)StudyType::Vascular),        gcnew String("VAS") );
//    m_pStudyTypeShortDescription->Add( ((int)StudyType::Angiography),     gcnew String("ANGIO") );
//
//    m_pModuleStateDescription->Add( ((int)ModuleState::Failed),             gcnew String("Failed") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::Started),            gcnew String("Started") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::ReceivingDicom),     gcnew String("Receiving") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::CreatingVolume),     gcnew String("Creating Volume") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::WritingVolume),      gcnew String("Writing Volume") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::LoadingVolume),      gcnew String("Loading Volume") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::MPR),                gcnew String("MPR") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::THR),                gcnew String("THR") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::CLA),                gcnew String("CLA") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::ECL),                gcnew String("ECL") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::INS),                gcnew String("INS") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::DFB),                gcnew String("DFB") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::SLV),                gcnew String("SLV") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::TRI),                gcnew String("TRI") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::SKE),                gcnew String("SKE") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_LoadingVolume),  gcnew String("Loading Volume") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_MPR),            gcnew String("MPR") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_THR),            gcnew String("THR") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_CLA),            gcnew String("CLA") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_ECL),            gcnew String("ECL") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_INS),            gcnew String("INS") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_DFB),            gcnew String("DFB") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_TRI),            gcnew String("TRI") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::AAA_SKE),            gcnew String("SKE") );
//    m_pModuleStateDescription->Add( ((int)ModuleState::Completed),          gcnew String("Complete") );
//
//  }
//
//  m_bTablesBuilt = true;
//
//} // BuildTables()





// Revision History:
// $Log: Descriptions.cpp,v $
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Descriptions.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: Descriptions.cpp,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
