// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: LoaderFactory.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "Loader.h"
#include <memory>
#include "ReaderGlobal.h"
#include "StudyLoadParamsEx.h"


namespace LoaderFactory
{
	/// parameters
  struct LoaderParams
  {
    StudyLoadParamsEx m_params;
    ReaderLib::StudyLoad::LoadCallback m_callback;
  };

  /// create factory method
  std::shared_ptr< Loader > Create( const LoaderParams & params ); 

};

