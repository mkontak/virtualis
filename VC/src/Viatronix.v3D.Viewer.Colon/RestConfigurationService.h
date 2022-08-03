// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: RestConfigurationService.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

#include "XmlConfigurationService.h"

class RestConfigurationService : public XmlConfigurationService
{
public:

  // destructor
  virtual ~RestConfigurationService();
  
  // get Profile
  std::string GetConfiguration( const std::string & sContextId, const std::string & sConfigName );

  // set Profile
  void SetConfiguration( const std::string & sContextId, const std::string & sXml );

  /// delete Profile
  void DeleteConfiguration( const std::string & sContextId, const std::string & sConfigName );


   // get Profile
  std::string GetUserProfile( const std::string & sContextId, const std::string & sConfigName );

  // set Profile
  void SetUserProfile( const std::string & sContextId, const std::string & sXml );

  /// delete Profile
  void DeleteUserProfile( const std::string & sContextId, const std::string & sConfigName );


};

