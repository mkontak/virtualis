// $Id: vxUndoActionPreset.h,v 1.3 2006/09/27 17:54:50 romy Exp $
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


// pragmas
#pragma once


// includes
#include "vxUndoAction.h"
#include "vxShareableObject.h"
#include "vxColormap.h"
#include "PixelRGBA.h"
#include "vxRenderingMode.h"


/**
 * Undo a recently modified preset
 */
class VX_VIEWER_DLL vxUndoActionPreset : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionPreset( vxShareableObject< vxColormap > * pColormap, vxShareableObject< PixelRGBA<uint1> > * pBackgroundColor,
                      vxShareableObject< vxRenderingMode > * pRenderingMode );

  /// perform the undo action
  virtual void ExecuteAction();
	
	//destructor
	virtual ~vxUndoActionPreset();

// member data
private:

  /// the previous colormap
  vxColormap m_colormapPrevious;

  /// the current colormap object
  vxShareableObject< vxColormap > * m_pColormapCurrent;

  /// the previous background color
  PixelRGBA<uint1> m_backgroundPrevious;

  /// the current background color object
  vxShareableObject< PixelRGBA<uint1> > * m_pBackgroundCurrent;

  /// the previous rendering mode
  vxRenderingMode m_renderingModePrevious;

  /// the current rendering mode object
  vxShareableObject< vxRenderingMode > * m_pRenderingModeCurrent;

}; // class vxUndoActionPreset


/**
 * Undo a recently modified rendering mode
 */
class VX_VIEWER_DLL vxUndoActionRenderingMode : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionRenderingMode( vxShareableObject< vxRenderingMode > * pRenderingMode );

  /// perform the undo action
  virtual void ExecuteAction();

// member data
private:

  /// the previous rendering mode
  vxRenderingMode m_renderingModePrevious;

  /// the current rendering mode object
  vxShareableObject< vxRenderingMode > * m_pRenderingModeCurrent;

}; // class vxUndoActionRenderingMode


/**
 * Undo a recently modified colormap
 */
class VX_VIEWER_DLL vxUndoActionColormap : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionColormap( vxShareableObject< vxColormap > * pColormap );

  /// perform the undo action
  virtual void ExecuteAction();

// member data
private:

  /// the previous colormap
  vxColormap m_colormapPrevious;

  /// the current colormap object
  vxShareableObject< vxColormap > * m_pColormapCurrent;

}; // class vxUndoActionColormap


// $Log: vxUndoActionPreset.h,v $
// Revision 1.3  2006/09/27 17:54:50  romy
// destructor added
//
// Revision 1.2  2006/02/24 20:13:21  frank
// added undo for rendering mode and colormap
//
// Revision 1.1  2006/02/24 19:38:42  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUndoActionPreset.h,v 1.3 2006/09/27 17:54:50 romy Exp $
// $Id: vxUndoActionPreset.h,v 1.3 2006/09/27 17:54:50 romy Exp $
