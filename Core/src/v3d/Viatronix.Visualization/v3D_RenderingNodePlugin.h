// $Id: v3D_RenderingNodePlugin.h,v 1.2 2007/05/03 21:36:02 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "vxRenderingNodePlugin.h"


// forward declarations
OPEN_VISUALIZATION_NS
ref class Triplet;
CLOSE_VISUALIZATION_NS


OPEN_GRAPH_NS


public ref class RenderingNodePlugin
{
public:

  /// constructor
  RenderingNodePlugin( System::IntPtr renderingPlugin );

  /// gets or sets the min render size
  property Viatronix::Visualization::Triplet ^ MinRenderSize
  {
    Viatronix::Visualization::Triplet ^ get();
    void set( Viatronix::Visualization::Triplet ^ mpTriplet ); 
  } // MinRenderSize


  /// gets or sets the max render size
  property Viatronix::Visualization::Triplet ^ MaxRenderSize
  {
    Viatronix::Visualization::Triplet ^ get();
    void set( Viatronix::Visualization::Triplet ^ mpTriplet ); 
  } // MaxRenderSize


  /// gets or sets the min frame rate
  property float4 MinFrameRate
  {
    float4 get() { return m_pRenderingManagerPlugin->GetMinFramerate(); }
    void set( float4 fMinFrameRate ) { m_pRenderingManagerPlugin->SetMinFramerate( fMinFrameRate ); }
  } // MinFrameRate


  /// gets or sets the max frame rate
  property float4 MaxFrameRate
  {
    float4 get() { return m_pRenderingManagerPlugin->GetMaxFramerate(); }
    void set( float4 fMaxFrameRate ) { m_pRenderingManagerPlugin->SetMaxFramerate( fMaxFrameRate ); }
  } // MaxFrameRate


  /// resets the rendering manager plugin
  void Reset();

private:

  /// unmanaged vxRenderingManagerPlugin
  vxRenderingNodePlugin * m_pRenderingManagerPlugin;

}; // class RenderingNodePlugin


CLOSE_GRAPH_NS


// $Log: v3D_RenderingNodePlugin.h,v $
// Revision 1.2  2007/05/03 21:36:02  gdavidson
// Moved to Viatronix.Visualization.Graph namespace
//
// Revision 1.1  2007/02/05 19:28:00  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.1  2007/01/16 17:14:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_RenderingNodePlugin.h,v 1.2 2007/05/03 21:36:02 gdavidson Exp $
// $Id: v3D_RenderingNodePlugin.h,v 1.2 2007/05/03 21:36:02 gdavidson Exp $

