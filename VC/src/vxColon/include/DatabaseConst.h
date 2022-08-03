// $Id: DatabaseConst.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: george geconomos@viatronix.com
//
// Complete History at bottom of file.

#ifndef DatabaseConst_h_
#define DatabaseConst_h_

/*
* This header file includes constants and enumerated values for the database class
*
*/


typedef enum UserTypeEnum
{
  UserType_Unknown       = 000,
  UserType_Administrator = 100,
  UserType_Service       = 200,
  UserType_Physician     = 300,
  UserType_User          = 400,
  UserType_System        = 500,
} UserTypeEnum;

typedef enum StudyTypeEnum
{
  StudyType_Unknown =  0,
  StudyType_Colonoscopy = 1,
  StudyType_AAA = 2,
  StudyType_CalciumScoring = 3,
  StudyType_Explorer = 4,
  StudyType_Vascular = 5,
  StudyType_Angio = 6,
  NUM_OF_STUDYTYPE_ENUM
} StudyTypeEnum;

#define STUDY_TYPE_STRINGS \
{"UNKNOWN","VC","AAA","CS", "XP", "VAS", "ANGIO"};



typedef enum DatasetTypeEnum
{
  DatasetType_Undefined = 0,
  DatasetType_VC_Supine = 100,
  DatasetType_VC_Prone = 101,
  DatasetType_VC_Scout = 102,
  DatasetType_AAA_Non_Contrast = 200,
  DatasetType_AAA_Immediate_Contrast = 201,
  DatasetType_AAA_Delayed_Contrast = 202,
  DatasetType_CS_Prospective = 300,
  DatasetType_CS_Retrospective = 301,
  DatasetType_XP = 400,
  DatasetType_Vascular = 500,
  DatasetType_Angio = 600
} DatasetTypeEnum;

typedef enum DatasetStateEnum
{
  DatasetState_Invalid             =  -1,
  DatasetState_Deleted             = 000,
  DatasetState_StartedReceiving    = 100,
  DatasetState_FinishedReceiving   = 102,
  DatasetState_StartedConverting   = 200,
  DatasetState_FailedConverting    = 201,
  DatasetState_FinishedConverting  = 202,    
  DatasetState_StartedPipelining   = 300,
  DatasetState_FailedPipelining    = 301,
  DatasetState_FinishedPipelining  = 302,

  DatasetState_Complete            = 1000    
} DatasetStateEnum;

typedef enum ServerTypeEnum
{
  DICOM_SERVER = 0,
  WORK_QUEUE_SERVER = 1,
}ServerTypeEnum;

typedef enum ModuleStateEnum
{
    COMPLETE = 0,
    FAILED = 1,
    START = 2,
    RECEIVING_DCM_FILES = 3,
    CREATING_VOL_FILE = 4, 
    WRITING_VOL_FILE = 5, 
    VC_LOADING_VOLUME = 6, 
    VC_MPR = 7, 
    VC_THR = 8,
    VC_CLA = 9, 
    VC_ECL = 10, 
    VC_INS = 11, 
    VC_DFB = 12, 
    VC_SLV = 13,
    VC_TRI = 14, 
    VC_SKE = 15,
    AAA_LOADING_VOLUME = 16, 
    AAA_MPR = 17, 
    AAA_THR = 18,
    AAA_CLA = 19, 
    AAA_ECL = 20, 
    AAA_INS = 21, 
    AAA_DFB = 22, 
    AAA_TRI = 23, 
    AAA_SKE = 24,
}ModuleStateEnum;

typedef enum ModuleTypeEnum
{
  Module_DCMToVOL = 1,
  Module_VCPipeline = 2,
  Module_AAAPipeline = 3,
  Module_DCMReceiver = 4
}ModuleTypeEnum;

typedef enum MessageTypeEnum 
{
  MessageType_Error = 1,
  MessageType_Warning = 2,
  MessageType_Information = 3    
} MessageTypeEnum;

typedef enum RemittanceStatusEnum
{
  DISABLE = 0,
  OK = 1,
  CURRENT = 2,
  SUBMIT_REPORT = 3,
  REPORT_PRINTED = 4,
  PASSED_DUE = 5,
  DELINQUENT = 6,
  CORRUPTED = 7,
} RemittanceStatusEnum;

typedef enum DBLocationEnum
{
  DBLOC_LOCAL, DBLOC_V2K, DBLOC_CD, DBLOC_PACS, DBLOC_OTHER
} DBLocationEnum;  

#endif

// $Log: DatabaseConst.h,v $
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.11.10.1  2003/10/22 14:01:45  mkontak
// Issue 3513
//
// Revision 3.14  2003/09/29 20:00:49  mkontak
// Angio processing
//
// Revision 3.13  2003/05/16 11:48:29  michael
// code review
//
// Revision 3.12  2003/03/21 14:51:07  mkontak
// Fix for CTA
//
// Revision 3.11  2002/05/14 15:57:59  mkontak
// Added DatasetType_XP = 400
//
// Revision 3.10  2002/05/06 16:09:35  jmeade
// UserType_System user type enum.
//
// Revision 3.9  2002/01/17 18:58:52  frank
// Added Retrospective Calcium Scoring dataset type.
//
// Revision 3.8  2002/01/11 20:40:38  mkontak
// Added moduleTypes
//
// Revision 3.7  2002/01/10 17:53:31  mkontak
// Fixed database rebuilder
//
// Revision 3.6  2002/01/09 17:21:28  jmeade
// Added "Complete" dataset state.
//
// Revision 3.5  2002/01/08 22:30:42  jmeade
// Comment.
//
// Revision 3.4  2002/01/03 22:00:13  mkontak
// Changes for 1.2
//
// Revision 3.3  2001/12/13 19:17:18  dmitry
// UserTypeEnum added
//
// Revision 3.2  2001/12/07 00:02:59  jmeade
// Added type for Calcium Scoring
//
// Revision 3.1  2001/10/20 01:40:52  jmeade
// Invalid dataset status
//
// Revision 3.0  2001/10/14 23:02:24  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 04 2001 11:09:08   jmeade
// Added Deleted Dataset state enum value
// 
//    Rev 2.0   Sep 16 2001 23:48:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:08   ingmar
// Initial revision.
// Revision 1.16  2001/08/28 22:45:05  soeren
// renamed some enums
//
// Revision 1.15  2001/08/28 15:39:08  soeren
// added numbers to stage enums
//
// Revision 1.14  2001/08/27 02:04:35  soeren
// added Module DCMRECEIVER
//
// Revision 1.13  2001/08/24 18:58:31  soeren
// adde new messages to WQHandler
//
// Revision 1.12  2001/08/16 15:56:36  jenny
// added end, started and failed
//
// Revision 1.11  2001/08/02 23:09:35  soeren
// first revision
//
// Revision 1.10  2001/08/01 17:12:08  soeren
// changed back to ewn version of DatabaseConst
//
// Revision 1.7  2001/07/27 18:39:37  soeren
// made some changed for the messenger service
//
// Revision 1.6  2001/07/20 19:27:10  soeren
// added some enums
//
// Revision 1.5  2001/07/19 00:39:11  pagron
// Second round of updates to Interbase port of the interface
//
// Revision 1.4  2001/07/16 14:16:46  soeren
// fixed typo
//
// Revision 1.3  2001/07/13 22:27:10  pagron
// first version of interbase based database
//
// Revision 1.1  2001/07/13 02:28:58  pagron
// A port of the database to support InterBase
//
// Revision 1.9  2001/06/14 02:13:20  soeren
// working queue
//
// Revision 1.8  2001/06/07 23:20:37  dave
// adding working queue table stuff
//
// Revision 1.7  2001/06/05 15:26:06  dmitry
// Remittance Status enumeration added
//
// Revision 1.6  2001/06/04 19:51:00  geconomos
// Changed StudyType enum values for Dmitry(cry baby)
//
// Revision 1.5  2001/06/01 17:17:37  jmeade
// ex::Database exception
//
// Revision 1.4  2001/05/29 18:24:32  jmeade
// Ummm, coding standards??
//
// Revision 1.3  2001/05/23 20:32:19  geconomos
// Added Dataset status
//
// Revision 1.2  2001/05/22 18:12:34  geconomos
// Added comments, headers and footers.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/DatabaseConst.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
// $Id: DatabaseConst.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
