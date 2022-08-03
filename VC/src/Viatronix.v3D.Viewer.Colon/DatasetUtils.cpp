// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatasetUtils.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "DatasetUtils.h"
#include "ReaderGlobal.h"
#include "ApplicationController.h"



USING_UTILITIES_NS


/**
 * Locks/unlocks the datasets
 *
 * @param       bLocked             true to lock datasets; false to unlock
 */
void Datasets::Lock( bool bLocked )
{
  ReaderLib::rdrGlobal.m_bSPLocked = bLocked;
  Colon::ApplicationController::Notify( Colon::DatasetsLockedArgs() );
} // Lock( bLocked )

