// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: IProfileService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include <string>
#include "AppSettings.h"


class IConfigurationService
{
public:

  // destructor
  virtual ~IConfigurationService() = 0 {}

  // get profile
  virtual std::string GetConfiguration( const std::string & sContextId, const std::string & sName ) = 0;

  // get profile
  virtual std::string GetDefaultConfiguration( const std::string & sContextId, const std::string & sName ) = 0;

  // set profile
  virtual void SetConfiguration( const std::string & sContextId, const std::string & sName ) = 0;

  /// delete profile
  virtual void DeleteConfiguration( const std::string & sContextId, const std::string & sName ) = 0;

}; //class IConfigurationService