// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ServiceLocator.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "ILoginService.h"
#include "IConfigurationService.h"
#include "IRetrievalService.h"
#include "IPacsService.h"


class ServiceLocator
{ 
public:

  /// returns the login service
  static std::shared_ptr<ILoginService> GetLoginService();

  /// returns the configuration service
  static std::shared_ptr<IConfigurationService> GetConfigurationService();

  /// returns the transfer service
  static std::shared_ptr<IRetrievalService> GetRetrievalService();

  /// returns the pacs service
  static std::shared_ptr<IPacsService> GetPacsService();

#pragma region fields

private:

  static std::shared_ptr<IConfigurationService> m_configurationService;

  static std::shared_ptr<IRetrievalService> m_retreivalService;

  static std::shared_ptr<ILoginService> m_loginService;

  static std::shared_ptr<IPacsService> m_pacsService;


#pragma endregion

}; // namespace ServiceLocator

