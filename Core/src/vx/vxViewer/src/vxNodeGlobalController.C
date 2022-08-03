// $Id: vxNodeGlobalController.C,v 1.2 2007/10/24 20:13:58 gdavidson Exp $
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
#include "vxNodeGlobalController.h"
#include "vxVisualizationGraphNode.h"
#include "vxVisualizationGraph.h"
#include "vxControllerGlobal.h"
#include "vxViewerControl.h"


#define FILE_REVISION "$Revision: 1.2 $"


/**
 * default constructor
 */
vxNodeGlobalController::vxNodeGlobalController() : vxVisualizationGraphNode(),
  m_pClearFuncPtr( NULL )
{
}


/**
 * constructor
 *
 * @param           funcPtr             callback function  
 */
vxNodeGlobalController::vxNodeGlobalController( ClearFuncPtr pFuncPtr ) : vxVisualizationGraphNode(),
  m_pClearFuncPtr( pFuncPtr )
{
}


/**
 * adds a global controller to the node
 *
 * @param         sId                 identifier for the global controller
 * @param         pGlobalContoller    pointer to a global controller
 */
void vxNodeGlobalController::AddController( std::string sId, vxControllerGlobal * pGlobalController )
{
  m_globalControllers.push_back( pGlobalController );
  m_globalControllerMap[ sId ] = pGlobalController;
} // AddContoller( sId, pGlobalController )


/**
 * removes a global controller from the node
 *
 * @param         sId                 identifier for the global controller
 */
void vxNodeGlobalController::RemoveController( const std::string & sId )
{
  vxControllerGlobal * pController = m_globalControllerMap[ sId ];
  m_globalControllerMap.erase( sId );

  m_globalControllers.erase( std::find( m_globalControllers.begin(), m_globalControllers.end(), pController ));
} // RemoveContoller( sId )


/**
 * clears all global controllers
 */
void vxNodeGlobalController::ClearControllers()
{
  m_globalControllers.clear();
  m_globalControllerMap.clear();

  if( m_pClearFuncPtr != NULL )
    m_pClearFuncPtr();
} // Reset()


/**
 * initializes the node
 */
void vxNodeGlobalController::Initialize()
{
  std::vector< vxViewer * > viewers;
  std::vector< vxViewerControl * > & viewerControls = GetVisualizationGraph()->GetViewerControls();

  for( std::vector< vxViewerControl * >::iterator iter = viewerControls.begin(); iter != viewerControls.end(); ++iter )
    viewers.push_back( (*iter)->GetViewer( vxViewerControl::MainViewer ));

  for( std::vector< vxControllerGlobal * >::iterator iter = m_globalControllers.begin(); iter != m_globalControllers.end(); ++iter )
    (*iter)->SetViewers( viewers );
} // Initialize()


/**
 * Processes all local controllers
 */
void vxNodeGlobalController::Run()
{
  for( std::vector< vxControllerGlobal * >::iterator iter = m_globalControllers.begin(); iter != m_globalControllers.end(); ++iter )
  {
    if( (*iter)->IsProcessingNecessary() )
      (*iter)->Process();
  }
} // Run()


/**
 * resets the node
 */
void vxNodeGlobalController::Reset()
{
  ClearControllers();
} // Reset()


// $Log: vxNodeGlobalController.C,v $
// Revision 1.2  2007/10/24 20:13:58  gdavidson
// Added vxViewerControl
//
// Revision 1.1  2007/05/03 21:42:49  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxNodeGlobalController.C,v 1.2 2007/10/24 20:13:58 gdavidson Exp $
// $Id: vxNodeGlobalController.C,v 1.2 2007/10/24 20:13:58 gdavidson Exp $
