// $Id: Callback.h,v 1.4 2009/06/22 18:36:43 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File: DcmCallback.h
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

// Pragmas
#pragma once

// includes
#include "DcmEnums.h"


namespace dcm
{

  // Forward declarations
  class DCM_DLL DcmAssociation;
  class DCM_DLL DcmImage;
  class DCM_DLL DcmFindParameters;
  class DCM_DLL DcmMoveParameters;

 
  /**
   * Dicom Store callback parameters
   *
   * The image is __nullptr for a SCU callback and contains the actual image received for a SCP callback.
   */
  typedef struct DcmStoreCallbackParameters
  {
    int Remaining;                      // Remaining count      SCP/SCU
    int Completed;                      // Completed count      SCP/SCU
    DcmImage * Image;                   // Image received       SCP
  } DcmStoreCallbackParameters;   



  /**
   * Dicom Echo callback parameters
   *
   * Since the echo is just a ping there are not parameters to pass back.
   */
  typedef struct DcmEchoCallbackParameters
  {
    char noop;
  } DcmEchoCallbackParameters;


  /**
   * Dicom Move callback parameters
   *
   * These field correspond to the responses received by the PACS 
   */
  typedef struct DcmMoveCallbackParameters
  {
    DcmMoveParameters * Parameters;             // Move Parameters                  SCU
    char * Directories;                         // List of delimited directories    SCP
    int Remaining;                              // Remaining count                  SCU
    int Warning;                                // Warning count                    SCU
    int Failed;                                 // Failed count                     SCU
    int Completed;                              // Completed count                  SCU
   } DcmMoveCallbackParameters;


  /**
   * Dicom Find callback parameters
   *
   * These field correspond to the responses received by the PACS 
   */
  typedef struct DcmFindCallbackParameters
  {
    int Completed;                            // Completed count                  SCU/SCP
    DcmImage * Image;                         // Image                            SCU
    DcmFindParameters * Parameters;           // Find Parameters                  SCP
    char * Results;                           // Find/Query results in xml format SCP
  } DcmFindCallbackParameters;


  /**
   * Dicom Get callback parameters
   */
  typedef struct DcmGetCallbackParameters
  {
    int Remaining;                            // Total images remaining   SCU
    int Completed;                            // Total images received    SCU
    int Warning;                              // Warning count                    SCU
    int Failed;                               // Failed count                     SCU
    DcmImage * Image;                         // Image                    SCU
  } DcmGetCallbackParameters;


  /**
   * Connection information
   */
  typedef struct DcmConnectionInformation
  {                                           //                        SCU         SCP
    const char * CallingAETitle;              // Calling AE TItle       Client      Server
    const char * CalledAETitle;               // Called AE TItle        Server      Server
    const char * ServerName;                  // Server Name/Ip         Server      Server
    unsigned int Port;                        // Server Port            Server      Server
  } DcmConnectionInformation;


  /**
   * Dicom Callback Parameters
   */
  typedef struct DcmCallbackParameters
  {

    const char * Id;                        // [in]   Association ID
    const char * Message;                   // [in]   Message
    enum DcmEvents EventType;               // [in]   Callback Event Type
    enum DcmCommands CommandType;           // [in]   Command Type        
    unsigned long Status;                   // [in]   Status

    DcmConnectionInformation Connection;     // [in]   Connection information 
    

    /**
     * The next grouping of parameters are defined by the command type
     */
    union Command
    {
      DcmStoreCallbackParameters Store;       // C-STORE command parameters
      DcmMoveCallbackParameters Move;         // C-MOVE command parameters
      DcmGetCallbackParameters Get;           // C-GET command parameters
      DcmEchoCallbackParameters Echo;         // C-ECHO command parameters
      DcmFindCallbackParameters Find;         // C-FIND command parameters
    } Command;

  } DCM_CALLBACK_PARAMETERS, *PDCM_CALLBACK_PARAMETERS, *LPDCM_CALLBACK_PARAMETERS;

  /// Callback function 
  typedef void (*DCM_CALLBACK_FUNCTION)(PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);

  typedef DCM_CALLBACK_FUNCTION (*PDCM_CALLBACK_FUNCTION);


#pragma region DcmCallback


/**
 * Implements a class that handles the callback events for the DICOM library
 */
class DCM_DLL DcmCallback
{
protected:

  /// constructor
  DcmCallback();

  /// Destructor
  virtual ~DcmCallback() {}

private:

  /// Copy constructor
  DcmCallback(const DcmCallback & pCallback);

protected:

  /// Performs a callback using the internal parameters
  inline void Callback()
  { Callback(&m_callbackParameters); }

  /// Callback based on event
  void Callback(DcmEvents eEventType);

  /// Performs the callback using the supplied parameters
  void Callback(PDCM_CALLBACK_PARAMETERS pParameters);
  
protected:

  /// Callback data
  LPVOID m_callbackData;

  /// Callback function
  std::function< void ( PDCM_CALLBACK_PARAMETERS, LPVOID ) > m_callbackFunction;

  /// Callback parameters
  DCM_CALLBACK_PARAMETERS m_callbackParameters;


}; // class DcmCallback

#pragma endregion


}; // namespace dcm