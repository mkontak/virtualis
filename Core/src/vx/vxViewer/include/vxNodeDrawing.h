// $Id: vxNodeDrawing.h,v 1.3 2007/10/29 20:34:27 gdavidson Exp $
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
#include "vxVisualizationGraphNode.h"
#include "vxDib.h"

// forward declaration
class vxViewerControl;
class vxViewer;


class VX_VIEWER_DLL vxNodeDrawing : public vxVisualizationGraphNode
{
// functions
public:

  /// constructor
  vxNodeDrawing();

  /// destructor
  virtual ~vxNodeDrawing();

  /// executes the node
  void Run();

  /// redraws the image
  void Redraw( vxViewerControl & viewer );

private:

  /// clears the OpenGL buffers
  void ClearGlBuffer( vxViewer * pViewer );

  /// Draws a border around the window.
  void DrawBorder( vxViewerControl & viewer );

private:

  /// indicates if timing statistics should be output
  bool m_bDrawTimingStatistics;

  vxDib m_dib;

}; // class vxNodeDrawing


// $Log: vxNodeDrawing.h,v $
// Revision 1.3  2007/10/29 20:34:27  gdavidson
// Added a Redraw method
//
// Revision 1.2  2007/10/24 20:13:42  gdavidson
// Moved rendering (drawing) into node
//
// Revision 1.1  2007/05/03 21:42:28  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeDrawing.h,v 1.3 2007/10/29 20:34:27 gdavidson Exp $
// $Id: vxNodeDrawing.h,v 1.3 2007/10/29 20:34:27 gdavidson Exp $
