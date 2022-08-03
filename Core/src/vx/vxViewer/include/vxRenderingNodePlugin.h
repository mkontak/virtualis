// $Id: vxRenderingNodePlugin.h,v 1.4 2007/06/29 20:22:37 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)

// pragma declarations
#pragma once

// include declarations
#include "Triple.h"

// forward declarations
class vxRenderBuffer;
class vxEnvironment;
class vxRenderingNodeThreadManager;
class vxDataset;
template <typename T> class BoundingRect;

/**
 * Provides the interface for a rendering node plugin.
 */
class VX_VIEWER_DLL vxRenderingNodePlugin
{
// member functions
public:

  /// constructor
  vxRenderingNodePlugin( vxEnvironment & environment );

  /// destructor
  virtual ~vxRenderingNodePlugin();

  /// gets the render buffer used by the plugin
  virtual vxRenderBuffer & GetRenderBuffer() = 0;
  
  /// renders the specified window into the buffer
  virtual void RenderWindow( const BoundingRect< uint2 > & window, const uint1 & uThreadId ) = 0;

  /// gets if setting the render size is allowed
  virtual const bool SupportsAdaptiveRendering() { return false; }
  
  /// sets the render size to use (if allowed)
  virtual void SetRenderSize( const Triple< uint4 > & renderSize ) {}

  /// call prior to renderwindow calls
  virtual void PreRender() {}

  /// aborts rendering
  virtual void Abort() {}

  /// resets
  virtual void Reset() {}

  /// initializes the plugin from the specified vxEnvironment
  virtual void InitializeFromEnvironment() {}

  /// updates the plugin from the specified vxEnvironment
  virtual void UpdateFromEnvironment() {}

  /// updates the plugin from the specified vxDataset
  virtual void UpdateFromDataset( vxDataset & dataset ) {}

  /// is rendering necessary?
  virtual bool IsRenderingNecessary() const { return false; }

  /// gets the minimum render size
  inline const Triple< uint4 > & GetMinRenderSize() const { return m_minRenderSize; }

  /// sets the minimum render size
  inline void SetMinRenderSize( const Triple< uint4 > & minRenderSize ){ m_minRenderSize = minRenderSize; }

  /// gets the maximum render size
  inline const Triple< uint4 > & GetMaxRenderSize() const { return m_maxRenderSize; }

  /// sets the maximum render size
  inline void SetMaxRenderSize( const Triple< uint4 > & maxRenderSize ){ m_maxRenderSize = maxRenderSize; }

  /// gets the initial render size
  inline const Triple< uint4 > & GetInitialRenderSize() const { return m_initialRenderSize; }

  /// sets the initial render size
  inline void SetInitialRenderSize( const Triple< uint4 > & initialRenderSize ){ m_initialRenderSize = initialRenderSize; }

  /// gets the render tile size
  inline const Triple< uint4 > & GetTileSize() const { return m_tileSize; }

  /// sets the render tile size
  inline void SetTileSize( const Triple< uint4 > & tileSize ){ m_tileSize = tileSize; }

  /// gets the minimum frame rate
  inline const float4 GetMinFramerate() const { return m_fMinFramerate; }

  /// gets the minimum frame rate
  inline void SetMinFramerate( const float4 fMinFramerate ) { m_fMinFramerate = fMinFramerate; }

  /// gets the maxnimum frame rate
  inline const float4 GetMaxFramerate() const { return m_fMaxFramerate; }

  /// gets the maximum frame rate
  inline void SetMaxFramerate( const float4 fMaxFramerate ) { m_fMaxFramerate = fMaxFramerate; }

protected:

  /// get the environment
  vxEnvironment & GetEnvironment() const { return m_environment; }

/// member variables
private:

  /// environment
  vxEnvironment & m_environment;

  /// mimimum render size
  Triple< uint4 > m_minRenderSize;

  /// maximum render size
  Triple< uint4 > m_maxRenderSize;

  /// initial render size
  Triple< uint4 > m_initialRenderSize;

  /// tile size
  Triple< uint4 > m_tileSize;

  /// initial render size
  float4 m_fMinFramerate;

  /// maximum frame rate
  float4 m_fMaxFramerate;

}; // class vxRenderingNodePlugin


// $Log: vxRenderingNodePlugin.h,v $
// Revision 1.4  2007/06/29 20:22:37  geconomos
// added reset
//
// Revision 1.3  2007/04/02 21:29:18  gdavidson
// Added UpdateFromDataset
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
// Revision 1.6  2007/01/24 20:26:38  gdavidson
// Added initial render size
//
// Revision 1.5  2007/01/16 16:49:54  gdavidson
// Changed the constructor to take a vxEnvironment
//
// Revision 1.4  2006/12/04 20:34:16  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.3  2006/08/28 14:02:26  geconomos
// moved all frame rate related stuff to vxRenderingNodePlugin
//
// Revision 1.2  2006/08/03 15:45:58  geconomos
// exported classes
//
// Revision 1.1  2006/08/03 13:58:20  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRenderingNodePlugin.h,v 1.4 2007/06/29 20:22:37 geconomos Exp $
// $Id: vxRenderingNodePlugin.h,v 1.4 2007/06/29 20:22:37 geconomos Exp $
