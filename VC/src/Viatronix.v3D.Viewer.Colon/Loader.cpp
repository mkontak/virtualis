// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Loader.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "Loader.h"

/**
 * constructor
 *
 * @param         params            study load parameters
 * @param         callback          load callback
 */
Loader::Loader( const StudyLoadParamsEx & params, ReaderLib::StudyLoad::LoadCallback callback ) : 
  m_params( params ),
  m_callback( callback )
{
} //Loader(


/**
 * destructor
 */
Loader::~Loader()
{
} // ~Loader()
