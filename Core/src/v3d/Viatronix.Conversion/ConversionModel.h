// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#include "Enums.h"
#include "FileConverter.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace Viatronix::Enterprise::Processing;

OPEN_CONVERSION_NS



/**
 * Implements a conversion ConversionModel that can be used by a view ConversionModel or any other application
 */
public ref class ConversionModel : IComparable<ConversionModel ^>
{


 #pragma region constructors/destructors

public:

  /// Constructor
  ConversionModel();
  
  /// Destructor
  ~ConversionModel();
  
  /// Finalizer
  !ConversionModel();

 #pragma endregion

 #pragma region events

public:

  /// <summary>
  /// Conversion state was updated 
  /// </summary>
  event PipelineEventHandler ^ Updated;

  /// <summary>
  /// Conversion started
  /// </summary>
  event PipelineEventHandler ^ Started;

  /// <summary>
  /// Conversion fialed
  /// </summary>
  event PipelineEventHandler ^ Failed;

  /// <summary>
  /// Conversion completed
  /// </summary>
  event PipelineEventHandler ^ Completed;

#pragma endregion

  
#pragma region event handlers

private:

  inline void OnFileConverterStarted(PipelineEventArgs ^ mpArgs)
  { OnStarted(mpArgs);  } 
  
  inline void OnFileConverterCompleted(PipelineEventArgs ^ mpArgs)
  { /* OnCompleted(mpArgs); */ } 

  inline void OnFileConverterFailed(PipelineEventArgs ^ mpArgs)
  { OnFailed(mpArgs);  } 

  inline void OnFileConverterUpdated(PipelineEventArgs ^ mpArgs)
  { Updated(mpArgs); } 

   
  /// Create updated handler
  inline void OnUpdated(PipelineEventArgs ^ mpArgs)
  { Updated(mpArgs);  }

  /// Conversion completed handler
  inline void OnStarted(PipelineEventArgs ^ mpArgs)
  { Started(mpArgs);  }
  
  /// Conversion completed handler
  inline void OnCompleted(PipelineEventArgs ^ mpArgs)
  { Completed(mpArgs);  }
 
  /// Conversion failed handler
  inline void OnFailed(PipelineEventArgs ^ mpArgs)
  { Failed(mpArgs);  }


#pragma endregion


#pragma region methods

public:

  /// Creates the conversion model used create the volume using the path/pattern 
  static ConversionModel ^ Create( Dictionary<String ^, String ^> ^ mpArgs );

  /// Generates the volume file based on the path
  static String ^ GenerateVolumeFile(String ^ msPath)
  {
    return System::IO::Path::Combine(msPath, String::Concat(System::IO::Path::GetFileName(msPath), ".vol"));
  } //  GenerateVolumeFile(String ^ msPath)

  /// Performs the volume creation
  void CreateVolume();

	/// Performs unit test
	void UnitTest();

  /// Aborts the current conversion
  void Abort();

  /// Override for the base Equals
  virtual bool Equals(Object ^ mpObject) override
  { return Equals(dynamic_cast<ConversionModel ^>(mpObject)); }

  inline bool Equals(ConversionModel ^ mpModel)
  { return m_id.Equals(mpModel->Id); }

  /// CompareTo
  virtual int CompareTo(ConversionModel ^ mpModel) 
  { return m_id.CompareTo(mpModel->Id); }

private:

  /// Runs the thread
  void Run();


#pragma endregion


#pragma region properties

public:

  property IPipelineResults ^ Results
  {
    IPipelineResults ^ get() { return m_mpConverter->GetResults(); }
  }

  /// Gets the SiteId of the model
  property int SiteId
  {
    int get() { return m_mpConverter->SiteId; }
  } // SiteId

  /// Gets/Sets the source level
  property System::Diagnostics::SourceLevels SourceLevel
  {
    System::Diagnostics::SourceLevels get() { return m_eSourceLevel; }
    void set( System::Diagnostics::SourceLevels eValue ) { m_eSourceLevel = eValue; }
  } // SourceLevel

  /// Flag indicating the ConversionModel is processing
  property bool Processing
  {
    bool get() { return (m_mpTask == nullptr || m_mpTask->Status != System::Threading::Tasks::TaskStatus::Running ? false : true); }
  } // Processing

  /// <summary>
  /// Gets/Sets the pattern
  /// </summary>
  property String ^ Pattern
  {
    String ^ get() 
    {
      if ( m_mpArgs->ContainsKey("pattern") )
        return m_mpArgs["pattern"];
      else
        return gcnew String("");
    } // get

    void set(String ^ msValue) 
    {
      if ( m_mpArgs->ContainsKey("pattern") )
        m_mpArgs["pattern"] = msValue; 
      else
        m_mpArgs->Add("pattern",msValue);
    } // set

  } // Pattern


  /// <summary>
  /// Gets/Sets the Path
  /// </summary>
  property String ^ Path
  {
    String ^ get() 
    {
      if ( m_mpArgs->ContainsKey("sourceFilePath") )
        return m_mpArgs["sourceFilePath"];
      else
        return gcnew String("");
    } // get

    void set(String ^ msValue) 
    {
      if ( m_mpArgs->ContainsKey("sourceFilePath") )
        m_mpArgs["sourceFilePath"] = msValue; 
      else
      {
        m_mpArgs->Add("sourceFilePath",msValue);
        this->VolumeFile = GenerateVolumeFile(msValue);
      }
    } // set

  } // Path
 
  /// Gets/Sets the volume file
  property String ^ VolumeFile
  {
    String ^ get() 
    {
      if ( m_mpArgs->ContainsKey("destFilePath") )
        return m_mpArgs["destFilePath"];
      else
        return gcnew String("");
    } // get

    void set(String ^ msValue) 
    {
      if ( m_mpArgs->ContainsKey("destFilePath") )
        m_mpArgs["destFilePath"] = msValue; 
      else
        m_mpArgs->Add("destFilePath",msValue);
    } // set
  } // VolumeFile

  /// Gets the Id of the model
  property Guid Id
  {
    Guid get() { return m_id; }
  } // Id

  /// Sets/Gets the job Id
  property String ^ JobId
  {
    String ^ get() { return m_msJobId; }
    void set(String ^ msValue) { m_msJobId = msValue; }
  }

  /// Sets/Gets the default application Id
  property String ^ DefaultApplication
  {
    String ^ get() { return m_msApplication; }
    void set(String ^ msValue) { m_msApplication = msValue; }
  } // DefaultApplication

  /// Completed event 
  property EventWaitHandle ^ CompletedEventHandle
  {
    EventWaitHandle ^ get() { return m_mpCompletedEventHandle; }
  } // EventHandle


#pragma endregion


#pragma region fields

private:

  FileConverter ^ m_mpConverter;

  /// Source level
  System::Diagnostics::SourceLevels m_eSourceLevel;

  /// Event Handle
  EventWaitHandle ^ m_mpCompletedEventHandle;

  /// Default application
  String ^ m_msApplication;

  /// Id of this conversion model
  Guid m_id;

  /// Conversion task
  System::Threading::Tasks::Task ^ m_mpTask;

  Dictionary<String ^, String ^> ^ m_mpArgs;

  /// job Id
  String ^ m_msJobId;

#pragma endregion


};  // class ConversionModel

CLOSE_CONVERSION_NS
