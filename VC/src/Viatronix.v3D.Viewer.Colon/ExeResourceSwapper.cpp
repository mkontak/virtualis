// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ExeResourceSwapper.cpp
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


#include "stdafx.h"
#include "ExeResourceSwapper.h"
#include "LanguageSupport.h"


/**
 * Switches to the exe resources
 *
 * @param bUseSatellite
 */
ExeResourceSwapper::ExeResourceSwapper( bool bUseSatellite ) :
  m_currentResource( AfxGetResourceHandle() )
{
  AfxSetResourceHandle( ( bUseSatellite && CLanguageSupport::GetInstance().GetResourceHandle() != NULL ) ? CLanguageSupport::GetInstance().GetResourceHandle() : AfxGetApp()->m_hInstance );
} // ExeResourceSwapper(


/**
 * Switches back to the previous resources
 */
ExeResourceSwapper::~ExeResourceSwapper()
{
  AfxSetResourceHandle( m_currentResource );
} // ~ExeResourceSwapper()