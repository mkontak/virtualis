// $Id: vxRenderingNodePlugin.C,v 1.3 2007/06/29 20:22:37 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

#include "StdAfx.h"
#include "vxRenderingNodePlugin.h"
#include "vxEnvironment.h"


/**
 * Constructor from environment
 *
 * @param   environment environment
 */
vxRenderingNodePlugin::vxRenderingNodePlugin( vxEnvironment & environment ) : 
  m_environment( environment ),
  m_minRenderSize( 128, 128, 0 ),
  m_maxRenderSize( 1024, 1024, 0  ),
  m_tileSize( 32, 32, 0 ),
  m_initialRenderSize( 128, 128, 0  ),
  m_fMinFramerate( 3.0F ),
  m_fMaxFramerate( 5.0F )
{
} // vxRenderingNodePlugin()


/**
 * Denstructor
 */
vxRenderingNodePlugin::~vxRenderingNodePlugin()
{
} // ~vxRenderingNodePlugin()


// $Log: vxRenderingNodePlugin.C,v $
// Revision 1.3  2007/06/29 20:22:37  geconomos
// added reset
//
// Revision 1.2  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.1  2007/02/05 19:27:52  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.5  2007/01/24 20:26:38  gdavidson
// Added initial render size
//
// Revision 1.4  2007/01/16 16:49:41  gdavidson
// Changed the constructor to take a vxEnvironment
//
// Revision 1.3  2006/12/04 20:42:11  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.2  2006/08/28 14:02:26  geconomos
// moved all frame rate related stuff to vxRenderingNodePlugin
//
// Revision 1.1  2006/08/03 13:58:31  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderingNodePlugin.C,v 1.3 2007/06/29 20:22:37 geconomos Exp $
// $Id: vxRenderingNodePlugin.C,v 1.3 2007/06/29 20:22:37 geconomos Exp $
