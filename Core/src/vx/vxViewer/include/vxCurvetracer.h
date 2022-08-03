// $Id: vxCurvetracer.h,v 1.2 2007/03/02 17:40:47 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragma declarations
#pragma once


// include declarations
#include "vxCurvetracerRenderArgs.h"
#include "vxCurvetracerSettings.h"
#include "BoundingRect.h"

// forward declarations
class vxRenderBuffer;

/**
 * Renders the projection of a ribbon.
 */
class VX_VIEWER_DLL vxCurvetracer
{
// member functions
public:
  
  /// constructor
  vxCurvetracer();
  
  /// destructor
  virtual ~vxCurvetracer();

  /// gets the settings
  vxCurvetracerSettings & GetSettings() { return m_settings; }

  /// gets the render buffer
  vxRenderBuffer & GetRenderBuffer() { return * m_pRenderBuffer; }

  /// performs any preinitialization
  virtual void PreRender();
  
  /// renders the specified window into the buffer
  virtual void RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId );

private:

  /// initializes the raytracer arguments structure
  void InitializeRenderArguments();

  /// determines the image size
  void SetImageSize();

   /// render a MIP slab
  int4 RenderMIP( const BoundingRect< uint2 > & renderWindow, vxCurvetracerRenderArgs & args, bool bSlab = false );  

  /// render a single thin slice
  int4 RenderSingleLayer( const BoundingRect< uint2 > & renderWindow, vxCurvetracerRenderArgs & args  );

  /// render an x-ray slab
  int4 RenderXRay( const BoundingRect< uint2 > & renderWindow, vxCurvetracerRenderArgs & args );


// member variables
private:

  /// settings
  vxCurvetracerSettings m_settings;

  /// common render arguments
  vxCurvetracerRenderArgs m_arguments;

  /// render buffer
  vxRenderBuffer * m_pRenderBuffer;

	/// maximum value for voxel value clamping
	int4 m_iMaxVoxelValue;

}; // class vxCurvetracer


// $Log: vxCurvetracer.h,v $
// Revision 1.2  2007/03/02 17:40:47  geconomos
// code review preparation
//
// Revision 1.1  2006/09/14 20:06:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCurvetracer.h,v 1.2 2007/03/02 17:40:47 geconomos Exp $
// $Id: vxCurvetracer.h,v 1.2 2007/03/02 17:40:47 geconomos Exp $
