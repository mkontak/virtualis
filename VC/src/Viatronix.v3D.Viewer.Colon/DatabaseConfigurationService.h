// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatabaseConfigurationService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "XmlConfigurationService.h"

class DatabaseConfigurationService : public XmlConfigurationService
{
public:

  // destructor
  virtual ~DatabaseConfigurationService();
  
  // get Profile
  std::string GetConfiguration( const std::string & sContextId, const std::string & sConfigName );

  // set Profile
  void SetConfiguration( const std::string & sContextId, const std::string & sXml );

  /// delete Profile
  void DeleteConfiguration( const std::string & sContextId, const std::string & sConfigName );


};

