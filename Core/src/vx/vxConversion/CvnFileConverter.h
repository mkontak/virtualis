// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "CvnConverter.h"
#include "CvnThread.h"



namespace cvn
{

/**
 * DICOM converter used for creating volumes files from dicom files.
 *
 * Allows the creation of volumes with the option of writing to disk or maintaining the volume
 * object in memory. Implements the a threaded object or allows the running from within the 
 * current thread.
 *
 */
class CVN_DLL CvnFileConverter : public CvnConverter, public CvnThread
{

#pragma region constructors/destructors

public:

  /// Constructor
  CvnFileConverter( int iSiteId = 0 );

  /// Destructor
  virtual ~CvnFileConverter();

  /// Creates the foie converter object and returns the shared pointer
  static std::shared_ptr<CvnFileConverter> Create(int iSite);

#pragma endregion


#pragma region Create methods 

public:

    /// Creates all volumes for the dataset and returns the list of volume files created
    virtual const std::map<uint4,std::shared_ptr<CvnSubVolumeInfo>> & Create( std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID )  > callbackFunction, LPVOID callbackData, const std::string & sVolumeFilePath = "");

#pragma endregion


  /// Start the thread
  virtual HANDLE Start(  std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID )  > callbackFunction, LPVOID callbackData, const std::string & sVolumeFilePath  )
  { 
    m_sVolumeFilePath = sVolumeFilePath; 
    return CvnThread::Start(callbackFunction, callbackData); 
  } // Start


  /// Add files
  void AddFiles(std::vector<std::string> files);

  /// Adds file to the list
  void AddFile(const std::string & sFile);

  /// Clear files 
  inline void ClearFiles() { m_files.clear(); }

  /// Adds all files with the pattern to the list
  void AddDirectory(const std::string & sDirectory, const std::string & sPattern = "*.dcm", bool recursive = false);


  /// Aborts the file conversion
  virtual void Abort();

	/// unit test
	void UnitTest( const std::string & sVolumeFilePath );

  /// Gets the number of files to be processed
  const uint4 GetFileCount() const
  { return (uint4)m_files.size(); }

protected:

  /// Callback relay 
  virtual void CallbackRelay(const CVN_CALLBACK_PARAMETERS & parameters);

private:

  /// Runs converion in the current thread (Called by the Create)
  void RunSynchronously();


  /// Returns true if the files exists and is readable
  bool IsReadable( const std::string & sFile );


	/// Check if it is enhanced dicom file
	bool IsEnhancedDicom();

  /// Loads file into image list
  void LoadFiles();

	/// extract sub-volume information
	void ExtractSubVolInfo();
	void ExtractSubVolInfo_enh();

	/// create all sub volumes
	void CreateAllSubVolumes();
	void CreateAllSubVolumes_enh();

 
private:

  /// Dicom files
  std::vector<std::string> m_files;

	/// number of sub volumes in this series
	int4 m_iNumOfSubVol;


}; // class CvnFileConverter

}; // namespace cvn