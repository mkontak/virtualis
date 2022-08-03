// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: XmlConfigurationService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "IConfigurationService.h"


class XmlConfigurationService : public IConfigurationService
{
public:

  // destructor
  virtual ~XmlConfigurationService();
  
  // get Profile
  virtual std::string GetConfiguration( const std::string & sContextId, const std::string & sConfigName );

  // get default Profile
  virtual std::string GetDefaultConfiguration( const std::string & sContextId,const std::string & sConfigName );

  // set Profile
  virtual void SetConfiguration( const std::string & sContextId,const std::string & sXml );

  /// delete Profile
  virtual void DeleteConfiguration( const std::string & sContextId,const std::string & sConfigName );


}; // class XmlConfigurationService

