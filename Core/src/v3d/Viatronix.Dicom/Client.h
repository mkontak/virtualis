// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Client.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "scoped_ptr.h"
#include "ClientEventArgs.h"


OPEN_DICOM_NS

// forward declarations
ref class Host;

/**
 * Base abstract class for the DICOM command objects CMove, CFind, CStore
 */
public ref class Client abstract
{
protected:

  /// Default constructor
  Client();

public:

 
  /// Starts the client operation
  virtual void Start()
  { Start(String::Empty); }

  virtual void Start(String ^ sCallbackData) abstract;

  /// Aborts the client operation
  virtual void Abort() abstract;

  /// Waits for the client operation
  virtual void Wait() abstract;
 
protected:

  /// Callback
  virtual void Callback(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData) abstract;

  /// Fires the appropriate event
  void FireEvent(dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters, ClientEventArgs ^ pArgs);


#pragma region events

public:

  /// Event is fired when the client is started
  event ClientEventHandler ^ Started;

  /// Event is fired when the client completed
  event ClientEventHandler ^ Completed;

  /// Event is fired when the client has aborted
  event ClientEventHandler ^ Aborted;

  /// Event is fired when the client has failed
  event ClientEventHandler ^ Failed;

    /// Event is fired when the client has made progress
  event ClientEventHandler ^ Progress;

#pragma endregion

#pragma region fire event methods

protected:       

  /// Fires the get started event
  virtual inline void OnStarted(ClientEventArgs ^ mpEventArgs)
  { Started(this, mpEventArgs); }

  /// Fires the on completed event
  virtual inline void OnCompleted(ClientEventArgs ^ mpEventArgs)
  { Completed(this, mpEventArgs);  }

  /// Fires the aborted event
  virtual inline void OnAborted(ClientEventArgs ^ mpEventArgs)
  { Aborted(this, mpEventArgs); }

  /// Fires the failed event with no message
  virtual inline void OnFailed(ClientEventArgs ^ mpEventArgs)
  { Failed(this, mpEventArgs); }

  inline void OnProgress(ClientEventArgs ^ mpEventArgs)
  { Progress(this, mpEventArgs); }



#pragma endregion



#pragma region properties

public:

  /// Gets/Sets the log source
  virtual property String ^ LogSource
  {
    String ^ get() = 0;
    void set(String ^ sValue) = 0;
  }

  /// Gets the server
  virtual property Host ^ Server
  {
    Host ^ get() = 0;
  } // Server


  /// Gets the network timeout
  virtual property int NetworkTimeout
  {
    int get() = 0;
    void set(int iNetworkTimeout) = 0;
  } // NetworkTimeout

  /// Gets/Sets the MAX PDU size
  virtual property int MaxPDU
  {
    int get() = 0;
    void set(int iMaxPDU) = 0;
  } // MaxPDU


  /// Gets/Sets the AE TItle of the server
  virtual property System::String ^ AETitle
  {
    System::String ^ get() = 0;
    void set(System::String ^ msAETitle) = 0;
  } // AETitle

  /// Gets/Sets the blocking mode for the server connection
  virtual property BlockingModes BlockingMode
  {
    BlockingModes get() = 0;
    void set(BlockingModes eMode) = 0;
  } // BlockingMode

  /// Gets the wait handle
  property System::Threading::EventWaitHandle ^ Handle
  {
    System::Threading::EventWaitHandle ^ get() { return m_mpEventWaitHandle; }
  } // EventWaitHandle



  

#pragma endregion

#pragma region fields

protected:

  char * m_callbackData;

  /// Event wait handle
  System::Threading::EventWaitHandle ^ m_mpEventWaitHandle;

 
  /// Callback delegate used for mapping the native callback
  DicomCallbackDelegate ^ m_mpCallbackDelegate;

#pragma endregion

};

CLOSE_DICOM_NS
