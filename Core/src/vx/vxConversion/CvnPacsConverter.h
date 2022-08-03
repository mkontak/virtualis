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


// Forwartd declarations
class CVN_DLL SubVolumeInfomation;



class CVN_DLL CvnPacsConverter : public CvnConverter, public CvnThread
{
public:

  /// Constructor
  CvnPacsConverter(int iSiteId, const std::string & sAETitle);

  /// Destructor
  virtual ~CvnPacsConverter();


#pragma region methods

#pragma region Create Volume methods

public:

  
  /// Creates all volumes for the dataset and returns the list of volume files created
  virtual const std::map<uint4,std::shared_ptr<CvnSubVolumeInfo>> & Create( std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID )  > callbackFunction, LPVOID callbackData, const std::string & sVolumeFilePath = "");

#pragma endregion



  /// Sets the retrieval parameters
  void SetRetrievalParameters(std::shared_ptr<dcm::DcmHost> & pServer,  std::shared_ptr<dcm::DcmGetParameters> & pParameters);

  /// Aborts the file conversion
  virtual void Abort();

  /// Start the thread
  virtual HANDLE Start(  std::function< void ( PCVN_CALLBACK_PARAMETERS, LPVOID )  > callbackFunction = nullptr, LPVOID callbackData = nullptr, const std::string & sVolumeFilePath = "" )
  { m_sVolumeFilePath = sVolumeFilePath; return Start(callbackFunction, callbackData); }

protected:

  /// Callback relay
  virtual void CallbackRelay(const CVN_CALLBACK_PARAMETERS & parameters);

private:

  /// Runs the thread
  void RunSynchronously();

  /// Handles the get callback
  static void GetCallbackHandler(dcm::PDCM_CALLBACK_PARAMETERS callbackParameters, LPVOID callbackData);

  /// Retrieves the image collection fromn the PACS server
  void Retrieve();

#pragma endregion


#pragma region fields

 
private:

  /// Connection parameters
  std::pair<std::shared_ptr<dcm::DcmHost>, std::shared_ptr<dcm::DcmGetParameters>>  m_retrievalParameters;

  /// Get
  std::shared_ptr<dcm::DcmGet> m_get;

  /// AE Title
  std::string m_sAEtitle;


#pragma endregion

}; // class CvnFileConverter


};  // namespace cvn
