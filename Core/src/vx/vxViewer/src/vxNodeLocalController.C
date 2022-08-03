// $Id: vxNodeLocalController.C,v 1.2 2007/10/24 20:13:58 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxNodeLocalController.h"
#include "vxVisualizationGraphNode.h"
#include "vxVisualizationGraph.h"
#include "vxViewer.h"
#include "vxViewerControl.h"
#include "vxController.h"


/**
 * Processes all local controllers
 */
void vxNodeLocalController::Run()
{
  std::vector< vxViewerControl * > & viewers = GetVisualizationGraph()->GetViewerControls();
  for( std::vector< vxViewerControl * >::iterator iter = viewers.begin(); iter != viewers.end(); ++iter )
  {
    vxViewer * pViewer = (*iter)->GetViewer( vxViewerControl::MainViewer );
    for ( std::list< vxController * >::iterator localIter = pViewer->GetControllerList().begin(); localIter != pViewer->GetControllerList().end(); ++localIter )
    {
      if( (*localIter)->IsProcessingNecessary() )
        (*localIter)->Process();
    }
  }
} // Run()


// $Log: vxNodeLocalController.C,v $
// Revision 1.2  2007/10/24 20:13:58  gdavidson
// Added vxViewerControl
//
// Revision 1.1  2007/05/03 21:42:49  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxNodeLocalController.C,v 1.2 2007/10/24 20:13:58 gdavidson Exp $
// $Id: vxNodeLocalController.C,v 1.2 2007/10/24 20:13:58 gdavidson Exp $
