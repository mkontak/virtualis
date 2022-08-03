// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once


#include "scoped_ptr.h"
#include "IConverter.h"

using namespace System::Collections::Generic;

OPEN_CONVERSION_NS


/**
 * Object encapsulates the native CvnFileConverter object
 */
public ref class FileConverter : IConverter
{
public:

  FileConverter();

  FileConverter(int iSiteId);

    /// Destructor
  ~FileConverter() 
  { Abort(); m_pFileConverter.reset(); this->!FileConverter(); }

  /// Finalizer
  !FileConverter() {  m_gcHandle.Free(); }

  /// Add File
  void AddFile(String ^ sFile);

  /// Add files from list
  void AddFiles(array<String ^> ^ fileList);

  /// Add directory
  void AddDirectory(String ^ sDirectory, String ^ sPattern, bool bRecursive);

  /// Clears the files
  void ClearFiles();

  /// Write the volume to the file specified 
  void Write(String ^ sVolumeFile);

	/// unit test
	void UnitTest( const std::string & sVolumeFilePath );

  /// Starts the client operation
  virtual System::Threading::EventWaitHandle ^ Start(Dictionary<String ^, String ^> ^ mpArgs) override;

  /// Aborts the client operation
  virtual void Abort() override;

  /// Runs the process in the current thread
  virtual void RunSynchronously(Dictionary<String ^, String ^> ^ mpArgs) override;

  /// Gets the results of the process
  virtual IPipelineResults ^ GetResults() override;

protected:

  /// Callback
  virtual void Callback(cvn::PCVN_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData) override;

  /// Override the bas converter 
  virtual void ProcessArguments(Dictionary<String ^, String ^> ^ mpArgs) override;


private:

  /// Initializes object
  void Initialize();

public:


  /// Returns true if the coversion is running
  virtual property bool IsRunning
  {
    bool get() override { return (*m_pFileConverter)->IsRunning(); }
  }

  /// Gets/Sete the size id
  virtual property int SiteId
  {
    int get() override { return  (*m_pFileConverter)->GetSiteId(); } ;
    void set(int iSiteId) override { (*m_pFileConverter)->SetSiteId(iSiteId); } 
  }

private:

  /// File converter
  scoped_ptr<std::shared_ptr<cvn::CvnFileConverter>> m_pFileConverter;

  /// Callback pointer
  IntPtr m_callbackPtr;

  /// Handle
  System::Runtime::InteropServices::GCHandle m_gcHandle;

};

CLOSE_CONVERSION_NS