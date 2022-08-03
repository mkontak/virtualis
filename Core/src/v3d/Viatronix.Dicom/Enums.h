// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Enums.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

#pragma once 

OPEN_DICOM_NS

/**
 * Blocking mode
 */
public enum class BlockingModes
{
  BLOCKING = dcm::DCM_BLOCKING,
  NONBLOCKING = dcm::DCM_NONBLOCKING
};  //BlockingMode

/**
 * Commands that can be issued.
 */
public enum class Commands
{
  None    = 0,       // No-Command
  Move    = 1,       // Move Command
  Find    = 2,       // Find/Query command
  Get     = 3,       // Get COmmand
  Store   = 4,       // Store Command
  Echo    = 5        // Echo command
}; // Commands
  
/**
 * Dicom Events
 */
public enum class Events
{
  None = 0,       // No Events
  Started,        // Started
  Completed,      // Completed
  Progress,       // Progression
  Failed,         // Failed
  Aborted         // Aborted
};  // Events

// ===========================
// Coordinate enumerator
// ===========================
public enum class Coordinate
{
  X = 0,
  Y = 1,
  Z = 2
};  // Coordinate

// =============================================================
// ImagePlanes
//  Sagittal is 0 to correspond to the X axis which is index 0
//  Cornal is 1 to correspond to the Y axis which is index 1
//  Axial is 2 to correspond to the Z axis which is index 2
// ============================================================
public enum class ImagePlanes
{

  None      = 0x0000,
  Axial     = 0x0001,   // Axial 
  Coronal   = 0x0002,   // Coronal
  Sagittal  = 0x0004,   // Sagittal
  Oblique   = 0x0008    // Oblique
};  // Axis


// =============================================================
// Axis Direction
// ============================================================
public enum class AxisDirections
{
  None = 0,

  // X Axis
  Left,
  Right,

  // Y Axis
  Anterior,
  Posterior,

  // Z axis
  HeadFirst,
  FeetFirst
};  // AxisDirection



// =====================
// Image Orientations
// =====================
public enum class ImageOrientations
{
  None = 0,     // None
  HFP,          // Head First Prone
  FFP,          // Feet First Prone
  HFS,          // Head First Supine
  FFS,          // Feet First Supine
  HFDL,         // Head First Decubitus Left
  FFDL,         // Feet First Decubitus Left
  HFDR,         // Head First Decubitus Right
  FFDR          // Feet First Decubitus Right
}; // ImageOrientations


/**
 * What type of results does the user want?  Patient, study or series.
 */
public enum class QueryRetievalLevel
{
  Patient = 0,  // Retrieve results by patient.
  Study   = 1,  // Retrieve results by study.
  Series  = 2,  // Retrieve results by series.
  Image   = 3
}; // QueryRetievalLevel

/**
 * Services types
 */
public enum class ServiceTypes : byte
{
  NONE      = dcm::ST_NONE,
  STORE     = dcm::ST_STORE,
  QUERY     = dcm::ST_QUERY,
  RETRIEVE  = dcm::ST_RETRIEVE,
  PRINT     = dcm::ST_PRINT,
  ALL       = dcm::ST_ALL,
  ANY       = dcm::ST_ANY
};  // ServiceTypes


/**
 * Parameter type
 */
public enum class ParameterType
{
  HEADER          = dcm::PT_HEADER,
  FILE            = dcm::PT_FILE
};  /// ParameterType

CLOSE_DICOM_NS