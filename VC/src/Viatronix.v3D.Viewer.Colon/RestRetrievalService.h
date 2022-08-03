// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: RestRetreivalSystem.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

#include "IRetrievalService.h"


class RestRetrievalService : public IRetrievalService
{
public:

  // destructor
  virtual ~RestRetrievalService();
  
  // gets the launch list
  std::string GetLaunchList( const std::string & sContextId, const std::string & sUid, const std::string & sApp, bool bNewSession );

  // get dicom header
  std::string GetDicomHeader( const std::string & sContextId, const std::string & sUid );

  // Completes the session
  void CompleteSession(const std::string & sContextId, const std::string & sUID);

  // Close the session
  void CloseSession(const std::string & sContextId, const std::string & sUID);

  // Reopens the session
  void ReopenSession(const std::string & sContextId, const std::string & sUID);


}; // class RestRetrievalService

