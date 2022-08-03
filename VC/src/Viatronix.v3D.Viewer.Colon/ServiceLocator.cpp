// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ServiceLocator.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net


#include "StdAfx.h"
#include "ServiceLocator.h"
#include "VCSettings.h"

// xml implementations
#include "XmlConfigurationService.h"

// database implementations
#include "DatabaseLoginService.h"
#include "DatabaseConfigurationService.h"
#include "DatabaseRetrievalService.h"

// dicom implementation
#include "DicomPacsService.h"

// rest implementation
#include "RestLoginService.h"
#include "RestConfigurationService.h"
#include "RestRetrievalService.h"

std::shared_ptr<IConfigurationService> ServiceLocator::m_configurationService(__nullptr);

std::shared_ptr<IRetrievalService> ServiceLocator::m_retreivalService(__nullptr);

std::shared_ptr<ILoginService> ServiceLocator::m_loginService(__nullptr);

std::shared_ptr<IPacsService> ServiceLocator::m_pacsService(__nullptr);


/**
 * returns the login service
 *
 * @return        ILoginService instance
 */
std::shared_ptr<ILoginService> ServiceLocator::GetLoginService()
{
  if ( m_loginService == __nullptr )
  {
    if ( ! VCSettings::GetInstance().GetValue("webservice").empty() )
      m_loginService = std::make_shared<RestLoginService>();
    else
     m_loginService = std::make_shared<DatabaseLoginService>();

  }
  return m_loginService;
} // GetLoginService()


/**
 * returns the configuration service
 *
 * @return        IConfigurationService instance
 */
std::shared_ptr<IConfigurationService> ServiceLocator::GetConfigurationService()
{

  if ( m_configurationService == __nullptr )
  {
    if ( ! VCSettings::GetInstance().GetValue("webservice").empty() )
      m_configurationService = std::make_shared<RestConfigurationService>();
    else
     m_configurationService = std::make_shared<DatabaseConfigurationService>();

  }

  return m_configurationService;
} // GetConfigurationService()


/**
 * returns the retrieval service
 *
 * @return        IRetrievalService instance
 */
std::shared_ptr<IRetrievalService> ServiceLocator::GetRetrievalService()
{
  if ( m_retreivalService == __nullptr )
  {
    if ( ! VCSettings::GetInstance().GetValue("webservice").empty() )
      m_retreivalService = std::make_shared<RestRetrievalService>();
    else
     m_retreivalService = std::make_shared<DatabaseRetrievalService>();

  }
 return m_retreivalService;
} // GetRetrievalService()


/**
 * returns the pacs service
 *
 * @return        IPacsService instance
 */
std::shared_ptr<IPacsService> ServiceLocator::GetPacsService()
{

  if ( m_pacsService == __nullptr )
    m_pacsService = std::make_shared<DicomPacsService>();

  return m_pacsService;
} // GetPacsService()