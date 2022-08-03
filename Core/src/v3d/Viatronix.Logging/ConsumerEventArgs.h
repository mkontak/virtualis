// Pragmas
#pragma once

#include "Enums.h"

// Namespaces
using namespace System;

OPEN_LOGGING_NS


/**
 * Implements a basis set of thread event arguments.
 */
[Serializable]
public ref class ConsumerEventArgs 
{

#pragma region constructor 

private:

  ConsumerEventArgs();

#pragma endregion

#pragma region methods

public:

  //// Creates the client args using the callback data
  static ConsumerEventArgs ^ Create(System::IntPtr vxlogCallbackParametersPtr);

#pragma endregion

#pragma region properties

public:

  /// Gets the source
  property String ^ Source
  {
    String ^ get() { return m_msSource; }
  } // Source


  /// Gets the Event type
  property Events EventType
  {
    Events get() { return m_eEventType; }
  } 


  /// Sets/Gets the abort flag
  property bool Abort
  {
    bool get() { return m_bAbort; }
    void set(bool bAbort) { m_bAbort = bAbort; }
  }

  /// Gets the event
  property String ^ Event
  {
    String ^ get() { return m_msEvent; }
  }

  /// Gets the message
  property String ^ Message
  {
    String ^ get() { return m_msMessage; }
  }

  /// Gets the provider Id
  property Guid ProviderId
  {
    Guid get() { return m_providerId; }
  }

#pragma endregion


#pragma region fields

private:

  /// Event Descriptor
  EventDescriptor m_eventDescriptor;

  /// Provider Id
  Guid m_providerId;
 
  /// Event 
  String ^ m_msEvent;

  /// Message
  String ^ m_msMessage;

  /// Study Uid
  String ^ m_msSource;

  /// Event Type
  Events m_eEventType;
  
  /// Abort flag
  bool m_bAbort;

#pragma endregion

};  // class EventArgs

/// Thread Event Handler Delegate
public delegate void ConsumerEventHandler(System::Object ^ mpSender, ConsumerEventArgs ^ e );




CLOSE_LOGGING_NS
