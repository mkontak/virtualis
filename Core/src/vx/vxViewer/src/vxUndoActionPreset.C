// $Id: vxUndoActionPreset.C,v 1.4 2006/09/27 17:54:29 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


/**
 * Implements undo for preset manipulation actions
 */


// includes
#include "StdAfx.h"
#include "vxUndoActionPreset.h"
#include "vxEnvironment.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * constructor
 *
 * @param pColormap        the colormap object
 * @param pBackgroundColor the rendering mode object
 * @param pRenderingMode   the rendering mode object
 */
vxUndoActionPreset::vxUndoActionPreset( vxShareableObject< vxColormap > * pColormap,
                                        vxShareableObject< PixelRGBA<uint1> > * pBackgroundColor,
                                        vxShareableObject< vxRenderingMode > * pRenderingMode )
: vxUndoAction(),
m_colormapPrevious( * pColormap ),
m_pColormapCurrent( pColormap ),
m_backgroundPrevious( * pBackgroundColor ),
m_pBackgroundCurrent( pBackgroundColor ),
m_renderingModePrevious( * pRenderingMode ),
m_pRenderingModeCurrent( pRenderingMode )
{
} // vxUndoActionPreset()

/**
 * destructor
 */
vxUndoActionPreset::~vxUndoActionPreset()
{
}//~vxUndoActionPreset()

/**
 * Undo the action
 */
void vxUndoActionPreset::ExecuteAction()
{

  // restore the previous colormap
  * m_pColormapCurrent = m_colormapPrevious;

  // set colormap modified
  m_pColormapCurrent->Modified( vxModInfo( vxColormap, vxUtils::COLORMAP_MODIFIED ) );

  // restore the previous background color
  * m_pBackgroundCurrent = m_backgroundPrevious;

  // set background color modified
  m_pBackgroundCurrent->Modified( vxModInfo( PixelRGBA< uint1 >, 0 ) );

  // restore the previous rendering mode
  * m_pRenderingModeCurrent = m_renderingModePrevious;

  // set rendering mode modified
  m_pRenderingModeCurrent->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_ALGORITHM_MODIFIED ) );
  m_pRenderingModeCurrent->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_DISPLAY_MODE_MODIFIED ) );
  m_pRenderingModeCurrent->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_SLAB_THICKESSS_MODIFIED ) );

} // ExecuteAction()


/**
 * constructor
 *
 * @param pRenderingMode the rendering mode object
 */
vxUndoActionRenderingMode::vxUndoActionRenderingMode( vxShareableObject< vxRenderingMode > * pRenderingMode )
: vxUndoAction(),
m_renderingModePrevious( * pRenderingMode ),
m_pRenderingModeCurrent( pRenderingMode )
{
} // vxUndoActionRenderingMode()


/**
 * Undo the action
 */
void vxUndoActionRenderingMode::ExecuteAction()
{

  // restore the previous rendering mode
  * m_pRenderingModeCurrent = m_renderingModePrevious;

  // set rendering mode modified
  m_pRenderingModeCurrent->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_ALGORITHM_MODIFIED ) );
  m_pRenderingModeCurrent->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_DISPLAY_MODE_MODIFIED ) );
  m_pRenderingModeCurrent->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_SLAB_THICKESSS_MODIFIED ) );

} // ExecuteAction()


/**
 * constructor
 *
 * @param pColormap        the colormap object
 */
vxUndoActionColormap::vxUndoActionColormap( vxShareableObject< vxColormap > * pColormap )
: vxUndoAction(),
m_colormapPrevious( * pColormap ),
m_pColormapCurrent( pColormap )
{
} // vxUndoActionColormap()


/**
 * Undo the action
 */
void vxUndoActionColormap::ExecuteAction()
{

  // restore the previous colormap
  * m_pColormapCurrent = m_colormapPrevious;

  // set colormap modified
  m_pColormapCurrent->Modified( vxModInfo( vxColormap, vxUtils::COLORMAP_MODIFIED ) );

} // ExecuteAction()


// undefines
#undef FILE_REVISION


// $Log: vxUndoActionPreset.C,v $
// Revision 1.4  2006/09/27 17:54:29  romy
// destructor added
//
// Revision 1.3  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.2  2006/02/24 20:13:21  frank
// added undo for rendering mode and colormap
//
// Revision 1.1  2006/02/24 19:38:42  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxUndoActionPreset.C,v 1.4 2006/09/27 17:54:29 romy Exp $
// $Id: vxUndoActionPreset.C,v 1.4 2006/09/27 17:54:29 romy Exp $
