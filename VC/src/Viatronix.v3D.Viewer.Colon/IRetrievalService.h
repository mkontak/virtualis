// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: IRetrievalService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include <string>


/**
 * Retrieval service implementation
 */
class IRetrievalService
{
public:

  // destructor
  virtual ~IRetrievalService() = 0 {}

  // gets the launch list
  virtual std::string GetLaunchList( const std::string & sContextId, const std::string & sUid, const std::string & sApp, bool bNewSession ) = 0;

  // gets the launch list
  virtual std::string GetDicomHeader( const std::string & sContextId, const std::string & sUid ) = 0;

  // Completes the session
  virtual void CompleteSession(const std::string & sContextId, const std::string & sUid) = 0;

  // Closes the session
  virtual void CloseSession(const std::string & sContextId, const std::string & sUid) = 0;

  // Reopens the session
  virtual void ReopenSession(const std::string & sContextId, const std::string & sUid) = 0;


}; // class IRetrievalService