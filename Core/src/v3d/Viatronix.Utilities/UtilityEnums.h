// $Id: UtilityEnums.h,v 1.2 2005/11/16 15:32:30 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/UtilityEnums.h,v 1.2 2005/11/16 15:32:30 mkontak Exp $
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

// Pragmas
#pragma once

// Namespaces
OPEN_TYPES_NS

/// Enumerator for the STUDY TYPES (Corresponds to the DATBASE entries)
public enum class StudyType
{
  Unknown = 0,
  Colonoscopy = 1,
  AAA = 2,
  CalciumScoring = 3,
  Explorer = 4,
  Vascular = 5,
  Angiography = 6
};  // StudyType


/// Enumerator for the DATASET TYPES (Correspoonds to the DATABASE entries)
public enum class DatasetType
{
  Undefined = 0,                
  Prone = 101,                        // Used mainly for VC
  Supine = 102,                       // Used mainly for VC
  Scout = 103,                        // Usedmainly for VC
  NonContrast = 200,                  // Used only by AAA
  ImmediateContrast = 201,            // Used only by AAA
  DelayedContrast = 202,              // Used only by AAA
  Prospective = 300,                  // Used only for CS
  Retrospective = 301,                // Used only for CS
  Explorer = 400,                     // Used only by XP
  Vascular = 500,                     // Used only by VAS
  Angio = 600                         // Used only by ANGIO
};  // DatasetType


/// Enumerator for the MODULE TYPES
public enum class ModuleType
{
  Conversion = 1,
  Pipeline = 2,
  AAAPipeline = 3,                // NOT USED   
  Receiver = 4
};  // ModuleTypes


/// Enumerator for DATASET STATE ID (Corresponds to the DATABASE ENTRIES)
public enum class DatasetState
{
  Invalid = -1,
  Deleted = 0,
  StartedReceiving = 100,
  FailedReceiving = 101,
  FinishedReceiving = 102,
  StartedConversion = 200,
  FailedConversion = 201,
  FinishedConversion = 202,
  StartedPipeline = 300,
  FailedPipeline = 301,
  FinishedPipeline = 302,
  Completed = 1000
};  // DatasetState

/// Enumerator for the MODULE STATE
public enum class ModuleState
{
  Completed = 0,
  Failed = 1,
  Started = 2,
  ReceivingDicom = 3,
  CreatingVolume = 4,
  WritingVolume = 5,
  LoadingVolume = 6,
  MPR = 7,
  THR = 8,
  CLA = 9,
  ECL = 10,
  INS = 11,
  DFB = 12,
  SLV = 13,
  TRI = 14,
  SKE = 15,
  AAA_LoadingVolume = 16,
  AAA_MPR = 17,
  AAA_THR = 18,
  AAA_CLA = 19,
  AAA_ECL = 20,
  AAA_INS = 21,
  AAA_DFB = 22,
  AAA_TRI = 23,
  AAA_SKE = 24

}; // ModuleState

// namespace
CLOSE_TYPES_NS

// Revision History:
// $Log: UtilityEnums.h,v $
// Revision 1.2  2005/11/16 15:32:30  mkontak
// Added VolumeHeader utility
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.1  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/UtilityEnums.h,v 1.2 2005/11/16 15:32:30 mkontak Exp $
// $Id: UtilityEnums.h,v 1.2 2005/11/16 15:32:30 mkontak Exp $
