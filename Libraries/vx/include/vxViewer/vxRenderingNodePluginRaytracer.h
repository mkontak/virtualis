// $Id: vxRenderingNodePluginRaytracer.h,v 1.6 2007/06/29 20:22:01 geconomos Exp $
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

// include declarations
#include "vxRaytracer.h"
#include "vxRenderingNodePlugin.h"
//#include "vxRayGenerator.h"

// forward declarations
class vxEnvironment;
class vxRayGenerator;
class vxDataset;

/**
 * Rendering node plugin for the raytracer ( handles 3d and oblique renderings ). 
 */
class VX_VIEWER_DLL vxRenderingNodePluginRaytracer : public vxRenderingNodePlugin
{
// member functions
public:
  
  /// constructor
  vxRenderingNodePluginRaytracer( vxEnvironment & environment );

  /// destructor
  virtual ~vxRenderingNodePluginRaytracer();

  /// gets the render buffer to be used  
  virtual vxRenderBuffer & GetRenderBuffer() { return m_raytracer.GetRenderBuffer(); }

   /// gets if setting the render size is permitted
  virtual const bool SupportsAdaptiveRendering() { return true; }
  
  /// sets the render size to use
  virtual void SetRenderSize( const Triple< uint4 > & renderSize ) { m_raytracer.GetSettings().SetRenderSize( renderSize ); }

  /// pre render
  virtual void PreRender() { m_raytracer.PreRender(); }

  /// renders the specified window into the buffer
  virtual void RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId ) { m_raytracer.RenderWindow( window, uThreadId ); }

  /// aborts rendering
  virtual void Abort() { m_raytracer.Abort(); }

  /// resets the plugin
  virtual void Reset();

  /// initializes the plugin from the specified vxEnvironment
  virtual void InitializeFromEnvironment();
  
  /// updates the plugin from the specified vxEnvironment
  virtual void UpdateFromEnvironment();

  /// updates the plugin from the specified vxDataset
  virtual void UpdateFromDataset( vxDataset & dataset );

  /// is rendering necessary?
  virtual bool IsRenderingNecessary() const;

  /// gets the raytracer
  vxRaytracer & GetRaytracer() { return m_raytracer; }

private:

  /// is 3D rendering necessary?
  bool IsRenderingNecessary3D() const;

  /// is oblique rendering necessary?
  bool IsRenderingNecessaryOblique() const;

// member variables
private:

  /// raytracer
  vxRaytracer m_raytracer;

  /// last active enviroment
  const vxEnvironment * m_pLastActiveEnvironment;

}; // class vxRenderingNodePluginRaytracer


// $Log: vxRenderingNodePluginRaytracer.h,v $
// Revision 1.6  2007/06/29 20:22:01  geconomos
// comments
//
// Revision 1.5  2007/04/02 21:29:18  gdavidson
// Added UpdateFromDataset
//
// Revision 1.4  2007/03/07 21:13:52  romy
// removed unnecessary include files.
//
// Revision 1.3  2007/03/05 15:23:27  geconomos
// code review preparation
//
// Revision 1.2  2007/02/07 20:50:39  gdavidson
// Added a pointer to track the last active environment
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingNodePluginRaytracer.h,v 1.6 2007/06/29 20:22:01 geconomos Exp $
// $Id: vxRenderingNodePluginRaytracer.h,v 1.6 2007/06/29 20:22:01 geconomos Exp $
