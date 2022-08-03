// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogEnums.h
//
// Description: Contains Enum definitions used throughout the logger
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


  // Message Type Enumerator
 typedef enum vxLogMessageFormats
 {
    FORMAT_DEFAULT        = 0x00,                       // Default message format 
    FORMAT_TEXT           = 0x00,                       // TEXT message format 
    FORMAT_XML            = 0x01,                       // XML message format 
  } vxLogMessageFormats;


/**
 * Event types
 */
typedef enum vxLogOpCodes : unsigned char
{
  O_REGULAR,              // Regular
  O_TIMING,               // Timing
} vxLogOpCodes;

/**
 * Event types
 */
typedef enum vxLogTaskCodes : unsigned short
{
  T_NORMAL,              // Normal Task
} vxLogTaskCodes;




/**
 * Event types
 */
typedef enum vxLogEventTypes
{
  E_NONE,                 //NONE
  E_STARTED,              // Started
  E_PROGRESS,             // Progress
  E_COMPLETED,            // Completed
  E_FAILED,               // Failed
  E_ABORTED               // Aborted
} vxLogEventTypes;



typedef struct vxLogRequest
{
  char Tag[3];
  int Size;
} REQUEST;


#define REQUEST_SIZE  sizeof(REQUEST)

