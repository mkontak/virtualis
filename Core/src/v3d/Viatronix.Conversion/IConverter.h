#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace Viatronix::Enterprise::Processing;

OPEN_CONVERSION_NS



public ref class IConverter abstract : IPipeline
{
protected:


  /// Constructor
  IConverter();

  /// Destructor
  ~IConverter() 
  { this->!IConverter(); }

  /// Finalizer
  !IConverter() {}


public:

  /// Gets the results
  virtual IPipelineResults ^ GetResults() abstract;

  /// Starts the conversion thread
  virtual System::Threading::EventWaitHandle ^ Start(Dictionary<String ^, String ^> ^ mpArgs) abstract;

  /// Runs the converion in the current thread
  virtual void RunSynchronously(Dictionary<String ^, String ^> ^ mpArgs) abstract;

  /// Aborts the the thread
  virtual void Abort() abstract;
 
protected:

  /// Callback
  virtual void Callback(cvn::PCVN_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData) abstract;

  /// Arguements
  virtual void ProcessArguments(Dictionary<String ^, String ^> ^ mpArgs);

public:

  /// Gets the default application for the conversion
  property String ^ DefaultApplication
  { 
    String ^ get() { return m_msDefaultApplication; }
  }

  /// Gets the log source
  property String ^ LogSource
  {
    String ^ get() { return m_msLogSource; }
  } // LogSource

  /// Gets the Jobs Id
  virtual property String ^ JodId
  {
    String ^ get() { return m_msJobId; }
  }

  /// Gets the exit code
  virtual property int ExitCode
  {
    int get() { return m_exitCode; }
  }

  /// Gets the SiteId
  virtual property int SiteId
  {  
    int get() abstract; 
    void set ( int iSiteId ) abstract;
  } 
  

  /// Gets the running flag
  virtual property bool IsRunning
  { bool get() abstract; }


  ///Sets/Gets the volume file path
  virtual property String ^ VolumeFilePath
  {
    void set( String ^ sValue ) { m_msVolumeFilePath = sValue; }
    String ^ get() { return m_msVolumeFilePath; }
  }

#pragma region events

public:

  /// Event is fired when the client is started
  virtual event PipelineEventHandler ^ Started;

  /// Event is fired when the client completed
  virtual  event PipelineEventHandler ^ Completed;

  /// Event is fired when the client has aborted
  virtual event PipelineEventHandler ^ Aborted;

  /// Event is fired when the client has failed
  virtual  event PipelineEventHandler ^ Failed;

    /// Event is fired when the client has made progress
   virtual event PipelineEventHandler ^ Updated;

#pragma endregion

#pragma region fire event methods

protected:       

  /// Fires the get started event
  virtual inline void OnStarted(PipelineEventArgs ^ mpEventArgs)
  { Started(mpEventArgs); }

  /// Fires the on completed event
  virtual inline void OnCompleted(PipelineEventArgs ^ mpEventArgs)
  { Completed(mpEventArgs);  }

  /// Fires the aborted event
  virtual inline void OnAborted(PipelineEventArgs ^ mpEventArgs)
  { Aborted(mpEventArgs); }

  /// Fires the failed event with no message
  virtual inline void OnFailed(PipelineEventArgs ^ mpEventArgs)
  { Failed(mpEventArgs); }

  virtual inline void OnUpdated(PipelineEventArgs ^ mpEventArgs)
  { Updated(mpEventArgs); }



#pragma endregion

protected:

    /// Event wait handle
  System::Threading::EventWaitHandle ^ m_mpEventWaitHandle;


  /// Callback delegate used for mapping the native callback
  CallbackDelegate ^ m_mpCallbackDelegate;

  /// Exit code
  int m_exitCode;

  /// Job Id
  String ^ m_msJobId;

  /// Deafult application 
  String ^ m_msDefaultApplication;

  /// Task Id
  String ^ m_msTaskId;

  /// LogSource
  String ^ m_msLogSource;

  /// Argements
  Dictionary<String ^, String ^> ^ m_mpArguments;

  /// Voluem file path
  String ^ m_msVolumeFilePath;

};

CLOSE_CONVERSION_NS