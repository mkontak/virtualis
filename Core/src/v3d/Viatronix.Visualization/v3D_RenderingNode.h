// $Id: v3D_RenderingNode.h,v 1.3 2007/03/22 18:24:01 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

// pragmas
#pragma once


// includes
#include "vxRenderingNode.h"

// namespaces
OPEN_VISUALIZATION_NS

//forward declarations
ref class Triplet;

// class declarations
public ref class RenderingNode sealed
{
// member functions
public:

  /// constructor
  RenderingNode( System::IntPtr renderingManager );

  /// aborts rendering and waits
  void AbortRenderingAndWait();

  /// gets the is rendering flag
  property bool IsRendering { bool get(); }

  /// gets the high quality thread enabled flag
  property bool EnableHighQualityThread{ bool get(); void set( bool bEnabled ); }

  /// gets the fixed size rendering flag
  property bool EnableFixedSizeRendering{ bool get(); void set( bool bEnabled ); }

  /// gets the fixed rendering size
  property Triplet ^ FixedRenderingSize { Triplet ^ get();  void set( Triplet ^ mpSize ); }

  /// gets or sets the number of threads available for rendering
  property int4 NumberOfRenderThreads { int4 get(); void set( int4 iThreadCount ); }

// member variables
private: 
  
  /// unmanaged singleton rendering manager
  vxRenderingNode * m_pRenderingNode;

}; // class RenderingNode


CLOSE_VISUALIZATION_NS

// $Log: v3D_RenderingNode.h,v $
// Revision 1.3  2007/03/22 18:24:01  gdavidson
// Added property for NumberOfRenderThreads
//
// Revision 1.2  2007/03/06 19:36:47  geconomos
// code review
//
// Revision 1.1  2007/02/05 19:28:00  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.8  2007/01/08 17:15:19  gdavidson
// Refactored VisualizationGraph code
//
// Revision 1.7  2006/12/11 19:14:09  geconomos
// issue #5152 - crashing when application is minimized or maximized
//
// Revision 1.6  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/27 16:08:14  geconomos
// added support for fixed size rendering
//
// Revision 1.4  2006/08/31 14:14:22  geconomos
// changes to rendering manager
//
// Revision 1.3  2006/08/28 21:07:55  geconomos
// added ability to disable the high quality image thread
//
// Revision 1.2  2006/08/28 14:04:07  geconomos
// added Reset() method
//
// Revision 1.1  2006/08/03 14:19:50  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_RenderingNode.h,v 1.3 2007/03/22 18:24:01 gdavidson Exp $
// $Id: v3D_RenderingNode.h,v 1.3 2007/03/22 18:24:01 gdavidson Exp $
