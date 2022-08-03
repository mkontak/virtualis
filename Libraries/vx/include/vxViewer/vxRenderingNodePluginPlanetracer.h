// $Id: vxRenderingNodePluginPlanetracer.h,v 1.3 2007/06/29 20:22:10 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// pragama declarations
#pragma once


// incldue declarations
#include "vxPlanetracer.h"
#include "vxRenderingNodePlugin.h"


// forward declarations
class vxEnvironment;

/**
 * Rendering node plugin for the planetracer 
 */
class VX_VIEWER_DLL vxRenderingNodePluginPlanetracer : public vxRenderingNodePlugin
{
// member functions
public:
  
  /// constructor
  vxRenderingNodePluginPlanetracer( vxEnvironment & environment );

  /// destructor
  virtual ~vxRenderingNodePluginPlanetracer();

  /// gets the render buffer used by the plugin  
  virtual vxRenderBuffer & GetRenderBuffer() { return m_planetracer.GetRenderBuffer(); }

  /// pre render
  virtual void PreRender() { m_planetracer.PreRender(); }

  /// renders the specified window into the buffer
  virtual void RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId ) { m_planetracer.RenderWindow( window, uThreadId ); }

  /// initializes the plugin from the specified vxEnvironment
  virtual void InitializeFromEnvironment();

  /// updates the pluging from the specified vxEnvironment
  virtual void UpdateFromEnvironment();

  /// is rendering necessary?
  virtual bool IsRenderingNecessary() const;

  /// resets the plugin
  virtual void Reset();


// member variables
private:

  /// planetracer
  vxPlanetracer m_planetracer;

}; // class vxRenderingNodePluginPlanetracer


// $Log: vxRenderingNodePluginPlanetracer.h,v $
// Revision 1.3  2007/06/29 20:22:10  geconomos
// added reset
//
// Revision 1.2  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.1  2007/02/05 19:27:53  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.3  2007/01/16 16:49:54  gdavidson
// Changed the constructor to take a vxEnvironment
//
// Revision 1.2  2006/08/03 15:45:58  geconomos
// exported classes
//
// Revision 1.1  2006/08/03 13:58:20  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingNodePluginPlanetracer.h,v 1.3 2007/06/29 20:22:10 geconomos Exp $
// $Id: vxRenderingNodePluginPlanetracer.h,v 1.3 2007/06/29 20:22:10 geconomos Exp $
