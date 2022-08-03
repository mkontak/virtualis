#pragma once



#include "Enums.h"

using namespace System;

OPEN_DICOM_NS

public ref class BaseEventArgs : public System::EventArgs
{

protected:

  BaseEventArgs() :
  m_iCompleted(0),
  m_msMessage(String::Empty),
  m_msCallingAETitle(String::Empty),
  m_msCalledAETitle(String::Empty),
  m_iPort(0),
  m_bAbort(false),
  m_msCallbackData(String::Empty),
  m_msId(String::Empty),
  m_msServerName(String::Empty),
  m_commandType(Commands::None),
  m_eventType(Events::None),
  m_msSource(String::Empty),
  m_status(0L)
  { }


  /// Initialize state
  void Initialize(IntPtr dcmCallbackParameters, IntPtr lpCallbackDataPtr )
  {
    dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters = reinterpret_cast<dcm::PDCM_CALLBACK_PARAMETERS>(dcmCallbackParameters.ToPointer());

    char * pCallbackData = reinterpret_cast<char *>(lpCallbackDataPtr.ToPointer());


    if ( pCallbackData != __nullptr )
      m_msCallbackData    = gcnew String(pCallbackData);

    m_msSource          = gcnew String(pCallbackParameters->Id);
    m_msCallingAETitle  = gcnew String(pCallbackParameters->Connection.CallingAETitle);
    m_msCalledAETitle   = gcnew String(pCallbackParameters->Connection.CalledAETitle);
    m_iPort             = pCallbackParameters->Connection.Port;
    m_msServerName      = gcnew String(pCallbackParameters->Connection.ServerName);
    m_msMessage         = gcnew String(pCallbackParameters->Message);
    m_msId               = gcnew String(pCallbackParameters->Id);

    m_commandType       = (Commands)pCallbackParameters->CommandType;
    m_eventType         = (Events)pCallbackParameters->EventType;

    m_status            = pCallbackParameters->Status;


    switch ( m_commandType )
    {
    case Commands::None:
      break;
    case Commands::Echo:
      break;
    case Commands::Store:
      {
        m_iCompleted = pCallbackParameters->Command.Store.Completed;
      }
      break;
    case Commands::Move:
      {
        m_iCompleted  = pCallbackParameters->Command.Move.Completed;
      }
      break;

    case Commands::Get:
      {
        m_iCompleted = pCallbackParameters->Command.Get.Completed;

      }
      break;

    case Commands::Find:
      {
        m_iCompleted = pCallbackParameters->Command.Find.Completed;

      }
      break;


    } // Initialize()


  }

#pragma region properties

public:

  property String ^ CallbackData
  {
    String ^ get() { return m_msCallbackData; }
  }

  /// Returns the port
  property int Port
  {
    int get() { return m_iPort; }
  } // Port

  /// Returns the thread id of the thread raising the event
  property String ^ Source
  {
    String ^ get() { return m_msSource; }
  }

  /// Returns a basic message
  property System::String ^ Message
  {
    System::String ^ get() { return(m_msMessage); }
  } // Message

  /// Returns a server name
  property System::String ^ ServerName
  {
    System::String ^ get() { return(m_msServerName); }
  } // ServerName


  /// Gets the  Calling AE Title
  virtual property String ^ CalledAETitle
  {
    String ^ get() { return m_msCalledAETitle; }
  } // CalledAETitle

   /// Gets the   Calling AE Title
  virtual property String ^ CallingAETitle
  {
    String ^ get() { return m_msCallingAETitle; }
  } // CallingAETitle

    /// Gets the abort flag
  property bool Abort
  {
    bool get() { return m_bAbort; }
    void set(bool bAbort) { m_bAbort = bAbort; }
  } // Abort flag

  //// Gets/Sets the completed value
  property int Completed
  {
    int get() { return m_iCompleted; }
   } // Completed

  //// Gets the command type
  property Commands CommandType
  {
    Commands get() { return m_commandType; }
  } // CommandType

  /// Gets the evenet type
  property Events EventType
  {
    Events get() { return m_eventType; }
  } // EventType

  /// Gets the status code
  property long Status
  {
    long get() { return m_status; }
  } // Status

  property String ^ Id
  {
    String ^ get() { return m_msId; }
  }

#pragma endregion


#pragma region fields

protected:

  /// Callback Data
  String ^ m_msCallbackData;

  /// Identifier
  String ^ m_msId;

  /// Port
  int m_iPort;

  /// Called AE Title
  String ^ m_msCalledAETitle;
  
  /// Calling AE Title
  String ^ m_msCallingAETitle;

  /// Server Name
  String ^ m_msServerName;

  /// Completed items
  int m_iCompleted;

  /// Source
  System::String ^ m_msSource;

  /// Messages
  System::String ^ m_msMessage;

  /// Abort flag
  bool m_bAbort;
 
  /// Command type
  Commands m_commandType;

  /// Event Type
  Events m_eventType;

   /// Status
  long m_status;

#pragma endregion

};  // class BaseEventArgs


CLOSE_DICOM_NS