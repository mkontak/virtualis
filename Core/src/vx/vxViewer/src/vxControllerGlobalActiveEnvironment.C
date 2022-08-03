// $Id: vxControllerGlobalActiveEnvironment.C,v 1.3 2007/03/02 17:19:14 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxControllerGlobalActiveEnvironment.h"
#include "vxEnvironment.h"
#include "vxViewer.h"

// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 */
vxControllerGlobalActiveEnvironment::vxControllerGlobalActiveEnvironment() : 
 m_pLastActiveEnvironment( NULL )
{
} // vxControllerGlobalActiveEnvironment()


/**
 * Destructor
 */
vxControllerGlobalActiveEnvironment::~vxControllerGlobalActiveEnvironment()
{
} // ~vxControllerGlobalActiveEnvironment()


/**
 * Processes the controller
 */
void vxControllerGlobalActiveEnvironment::Process()
{
  // has the active environment changed?
  if( vxEnvironment::GetActiveEnvironment() != m_pLastActiveEnvironment )
  {
    // notify all environnments
    for( int4 i = 0; i < GetViewers().size(); ++i )
      GetViewers()[i]->GetEnvironment().SetModified( vxEnvironment::ACTIVE_ENVIRONMENT, true );
    
    // update last active environment
    m_pLastActiveEnvironment = const_cast< vxEnvironment * >( vxEnvironment::GetActiveEnvironment() );
  }

} // Process()


// $Log: vxControllerGlobalActiveEnvironment.C,v $
// Revision 1.3  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.2  2007/02/22 16:07:37  geconomos
// code cleanup
//
// Revision 1.1  2006/10/20 20:46:24  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxControllerGlobalActiveEnvironment.C,v 1.3 2007/03/02 17:19:14 geconomos Exp $
// $Id: vxControllerGlobalActiveEnvironment.C,v 1.3 2007/03/02 17:19:14 geconomos Exp $


