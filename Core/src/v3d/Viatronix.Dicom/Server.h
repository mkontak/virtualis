

#pragma once



#include "ServerEventArgs.h"
#include "scoped_ptr.h"

using namespace System;
using namespace System::Xml;
using namespace System::Runtime::InteropServices;


OPEN_DICOM_NS





//=======================
// Dicom Server Class
//======================
public ref class Server 
{



#pragma region constructor

protected:

  /// Constructor
  Server();

public:

  // Destructor
  ~Server()
  { this->!Server();  }

  
  /// Finalizer
  !Server()
  {  }

#pragma endregion

#pragma region methods

public:

  /// Starst the server
  virtual void Start();

  /// Stops the server
  virtual void Stop();

  /// Waits for the server
  virtual void Wait();

  /// Sets the transfer syntaxes
  void SetTransferSyntax(E_TransferSyntax transferSyntax);


  /// Creates a Dicom Server from the xml node
  static Server ^ Create(System::Xml::XmlNode ^ mpNode);



private:

  /// Calback used to relay events
  void Callback(dcm::PDCM_CALLBACK_PARAMETERS pParams, LPVOID lpCallbackData);

#pragma endregion


#pragma region events

/// Public Events
public:   

  /// Event is fired when the server is started
  event ServerEventHandler ^ Started;

  /// Event is fired when the server completed
  event ServerEventHandler ^ Completed;

  /// Event is fired when the server has stopped
  event ServerEventHandler ^ Stopped;

  /// Event is fired when the server has failed
  event ServerEventHandler ^ Failed;

  // Event indicates that the server received a C-STORE command
  event ServerEventHandler ^ ImageReceived;

  // Event indicates that the server saved the dicom file
  event ServerEventHandler ^ FileSaved;

  // Event indicates that the server received a C-FIND command
  event ServerEventHandler ^ QueryReceived;

  // Event indicates that the server received a C-MOVE command
  event ServerEventHandler ^ MoveReceived;

  // Event indicates that the association started
  event ServerEventHandler ^ AssociationStarted;

  // Event indicates that the association completed
  event ServerEventHandler ^ AssociationCompleted;

  // Event indicates that the association failed
  event ServerEventHandler ^ AssociationFailed;




#pragma region fire event methods

protected:       

  // Fires the get started event
  virtual inline void OnStarted(ServerEventArgs ^ mpEventArgs)
  { Started(this, mpEventArgs); }

  // Fires the on completed event
  virtual inline void OnCompleted(ServerEventArgs ^ mpEventArgs)
  { Completed(this, mpEventArgs);  }

  // Fires the aborted event
  virtual inline void OnStopped(ServerEventArgs ^ mpEventArgs)
  { Stopped(this, mpEventArgs); }

  // Fires the failed event with no message
  virtual inline void OnFailed(ServerEventArgs ^ mpEventArgs)
  { Failed(this, mpEventArgs); }

  // Fires the OnImageReceived event
  virtual inline void OnImageReceived(ServerEventArgs ^ mpEventArgs)
  { ImageReceived(this, mpEventArgs); }

  // Fires the OnFeilSaved event
  virtual inline void OnFileSaved(ServerEventArgs ^ mpEventArgs)
  { FileSaved(this, mpEventArgs); }

  // Fires the OnQueryReceived event
  virtual inline void OnQueryReceived(ServerEventArgs ^ mpEventArgs)
  { QueryReceived(this, mpEventArgs); }

  // Fires the OnMoveReceived event
  virtual inline void OnMoveReceived(ServerEventArgs ^ mpEventArgs)
  { MoveReceived(this, mpEventArgs); }


  // Fires the OnAssociationStarted event
  virtual inline void OnAssociationStarted(ServerEventArgs ^ mpEventArgs)
  { AssociationStarted(this, mpEventArgs); }

  // Fires the OnAssociationCompleted event
  virtual inline void OnAssociationCompleted(ServerEventArgs ^ mpEventArgs)
  { AssociationCompleted(this, mpEventArgs); }

  // Fires the OnAssociationFailed event
  virtual inline void OnAssociationFailed(ServerEventArgs ^ mpEventArgs)
  { AssociationFailed(this, mpEventArgs); }


#pragma endregion

#pragma endregion

#pragma region properties

public:

  
  /// Get the port number
  property unsigned int Port
  {
    unsigned int get();
  } // Port


  /// Gets the network timeout
  property unsigned int NetworkTimeout
  {
    unsigned int get();
  } // NetworkTimeout

  /// Gets/Sets the MAX PDU size
  property int MaxPDU
  {
    int get();
  } // MaxPDU

  /// Gets the servers id
  property String ^ Id
  {
    String ^ get();
  } // Id

  /// Gets the AE TItle of the server
  property String ^ AETitle
  {
    String ^ get();
  } // AETitle

  /// Gets the version of the server
  static property String ^ Version
  {
    String ^ get() { return m_sVersion; }
  } // Version

  /// Gets the IsRUnning flag
  property bool IsRunning
  {
    bool get() { return (*m_pDcmServer)->IsRunning(); }
  }

#pragma endregion

#pragma region fields

private:

  /// Version
  static String ^ m_sVersion = "5.0";

  /// Natve dicom server
  scoped_ptr<std::shared_ptr<dcm::DcmServer>> m_pDcmServer;

   /// Path to were the dicom files are to be stored.
  String ^ m_msStoragePath;

  /// Callback delegate used for mapping the native callback
  DicomCallbackDelegate ^ m_mpCallbackDelegate;


  GCHandle m_gcHandle;

#pragma endregion



};  // class Server


CLOSE_DICOM_NS
