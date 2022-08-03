// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmEnums.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once 

namespace dcm
{

// Default network timeout
#define DCM_DefaultNetworkTimeout         30

// Attribute Defines
#define DCM_SupportsEncapsulatedPdf       "supports_encapsulated_pdf"
#define DCM_SupportsGetCommand            "supports_get_command"
#define DCM_ExplicitTransfer              "explicit_transfer"
#define DCM_ValidateServerPriorToStore    "validate_server_prior_to_store"
#define DCM_NetworkTimeout                "network_timeout"
#define DCM_AttributeFile                 "attribute_file"
#define DCM_SupportsSeriesLevelQuery      "supports_series_level_query"


/**
 * Blocking modes
 */
typedef enum DcmBlockingMode
{
  DCM_BLOCKING = DIMSE_BLOCKING,
  DCM_NONBLOCKING = DIMSE_NONBLOCKING
} DcmBlockingMode;


/**
 * Pixel Formats
 */
typedef enum DcmPixelFormats
{
  PF_None,
  PF_16bppGrayScale  = PixelFormat16bppGrayScale,
  PF_16bppRgb555     = PixelFormat16bppRGB555,
  PF_16bppRgb565     = PixelFormat16bppRGB565,
  PF_16bppArgb1555   = PixelFormat16bppARGB1555,
  PF_24bppRgb        = PixelFormat24bppRGB,
  PF_32bppArgb       = PixelFormat32bppARGB,
  PF_32bppRgb        = PixelFormat32bppRGB,
  PF_32bppPArgb      = PixelFormat32bppPARGB
} DcmPixelFormats;


/**
  * Coordinate
  */
typedef enum DcmCoordinate
{
  X = 0,
  Y,
  Z
} DcmCoordinate;


/**
  * Dicom Commands available
  */
typedef enum DcmCommands
{
  C_NONE = 0,         // No-Command
  C_MOVE,             // Move Command
  C_FIND,             // Find/Query command
  C_GET,              // Get Command
  C_STORE,            // Store Command
  C_ECHO              // Echo command
} DcmCommands;        // DcmCommands


/**
 * Dicom Events, used in the callback to identify what triggered
 * the callback.
 */
typedef enum DcmEvents
{
  E_NONE = 0,       // No Events
  E_STARTED,        // Started
  E_COMPLETED,      // Completed
  E_PROGRESS,       // Progression
  E_FAILED,         // Failed
  E_ABORTED         // Aborted
} DcmEvents;  // DcmEvents



/**
 * Image Plane
 *
 * Leaving ths comment though I changed the values to correspond to wht is being used
 * at the above layer. Did a search in the project and did not find anywhere I was using 
 * the plane as an index. This was probably from an older design that has since been changed
 *
 *  Sagittal is 0 to correspond to the X axis which is index 0
 *  Cornal   is 1 to correspond to the Y axis which is index 1
 *  Axial    is 2 to correspond to the Z axis which is index 2
 */
typedef enum DcmPlanes
{
  P_NONE      = 0x000,          // NONE
  P_AXIAL     = 0x0001,         // Axial 
  P_CORONAL   = 0x0002,         // Coronal
  P_SAGITTAL  = 0x0004,         // Sagittal
  P_OBLIQUE   = 0x0008          // Oblique
} DcmPlanes;



/**
 * States enumertion
 */
typedef enum DcmStates
{
  S_IDLE,             // Idle
  S_RUNNING,          // Running
  S_COMPLETED,        // Completed
  S_FAILED,           // Failed
  S_STOPPED,          // Stopped
  S_STOPPING          // Stopping
} DcmStates; 


/**
 * Image Orientations
 */
typedef enum DcmOrientations
{
  O_NONE = 0,     // None
  O_HFP,          // Head First Prone
  O_FFP,          // Feet First Prone
  O_HFS,          // Head First Supine
  O_FFS,          // Feet First Supine
  O_HFDL,         // Head First Decubitus Left
  O_FFDL,         // Feet First Decubitus Left
  O_HFDR,         // Head First Decubitus Right
  O_FFDR          // Feet First Decubitus Right
} DcmOrientations; 


/** 
 * Query/Retrieval level : Identifies the level of information to be returned
 */
typedef enum DcmQueryRetrievalLevels
{
  L_PATIENT,          // Patient Level
  L_STUDY,            // Study Level
  L_SERIES,           // Series Level
  L_IMAGE             // Image Level
} DcmQueryRetrievalLevels;

/**
 * Services (Used in the host table)
 */
typedef enum DcmServiceTypes 
{
  ST_NONE     = 0x00,    // None
  ST_STORE    = 0x01,    // Store
  ST_QUERY    = 0x02,    // Query 
  ST_RETRIEVE = 0x04,    // Retrieve
  ST_PRINT    = 0x08,    // Print
  ST_ALL      = ST_STORE | ST_RETRIEVE | ST_QUERY,     // ALL
  ST_ANY      = 0xFF
} DcmServiceTypes;


/**
 * Parameter type
 */
typedef enum DcmParameterType
{
  PT_HEADER,            // Parameter value is xml header
  PT_FILE               // Parameter value is a  file path
} DcmParameterType;

}; // namespace dcm