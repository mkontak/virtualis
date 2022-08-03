// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: LoaderFactory.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "LoaderFactory.h"
#include "StudyLoadParamsEx.h"
#include "Exception.h"

#include "FileSystemLoader.h"
#include "DatabaseLoader.h"


/**
 * Creates a loader
 *
 * @param       params              loader parameters
 * @return      loader instance
 */
std::shared_ptr< Loader > LoaderFactory::Create( const LoaderFactory::LoaderParams & params )
{
  switch( params.m_params.GetLoaderStyle() )
  {
  case StudyLoadParamsEx::LOADER_STYLE_LEGACY:
    return std::make_shared< FileSystemLoader >( params.m_params, params.m_callback ); 

  case StudyLoadParamsEx::LOADER_STYLE_CONSOLE:
  case StudyLoadParamsEx::LOADER_STYLE_DATABASE:
    return std::make_shared< DatabaseLoader >( params.m_params, params.m_callback ); 

  default:
    throw ex::LoadFailedException( LogRec( "Unable to determine loader.", "LoaderFactory", "Create" ));
  }

} // Create( params )