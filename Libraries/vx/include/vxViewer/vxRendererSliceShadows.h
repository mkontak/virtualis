// $Id: vxRendererSliceShadows.h,v 1.4 2006/10/05 16:59:44 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:george@viatronix.net)

// pragama declarations
#pragma once


// include declarations
#include "vxRenderer.h"
#include "vxElementPixelRGBA.h"



// forward declarations
class vxEnvironment;
class Quadrilateral;
class vxCamera;
template <class T> class PlaneOblique;

// class declaration
class VX_VIEWER_DLL vxRendererSliceShadows : public vxRenderer
{
// member functions
public:

  /// constructor
  vxRendererSliceShadows( vxEnvironment & environment );
  
  /// destructor
  virtual ~vxRendererSliceShadows() {}
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform rendering operations
  virtual void Render() = 0;

protected:

  /// gets the color for the slice shadow to be represented by the specified environment
  const PixelRGBA<uint1> GetSliceShadowColor( const vxEnvironment & environment );

  /// gets the ortho/oblique plane for the specified environment
  const vxPlaneOblique< float4 > * GetPlane( const vxEnvironment & environment );

 }; // class vxRendererSliceShadows


// Revision History:
// $Log: vxRendererSliceShadows.h,v $
// Revision 1.4  2006/10/05 16:59:44  geconomos
// redesgined and implemented
//
// Revision 1.3  2006/10/03 20:02:17  geconomos
// continued reworking of slice shadows
//
// Revision 1.2  2006/03/27 19:13:35  gdavidson
// Marked Render method abstract and added RenderEnvironment method
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.19  2003/11/04 15:30:18  frank
// Issue #2798: Added oblique slice shadow on oblique views
//
// Revision 1.18  2003/09/25 04:24:56  frank
// Added ortho slice shadows on oblique views
//
// Revision 1.17  2003/09/12 14:23:38  frank
// Implemented the oblique slice shadow
//
// Revision 1.16  2003/09/09 13:35:20  frank
// working on oblique slice shadows
//
// Revision 1.15  2003/05/21 18:39:32  frank
// code review
//
// Revision 1.14  2003/05/19 14:40:41  frank
// Removed unused return parameter
//
// Revision 1.13  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.12  2003/05/08 20:16:06  michael
// code reviews
//
// Revision 1.11  2003/05/08 20:02:54  frank
// code review
//
// Revision 1.10  2003/05/08 17:33:33  michael
// code review
//
// Revision 1.9  2003/05/08 15:26:54  frank
// code review
//
// Revision 1.8  2003/01/21 22:16:43  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.7  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.6  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.5  2002/05/07 18:34:40  frank
// Removed unneccessary includes.
//
// Revision 1.4  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.3  2002/04/25 18:07:06  uday
// Added PreRender()
//
// Revision 1.2  2002/04/24 16:07:18  uday
// Removed list of planes and updated Render()
//
// Revision 1.1  2002/04/23 17:06:23  uday
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSliceShadows.h,v 1.4 2006/10/05 16:59:44 geconomos Exp $
// $Id: vxRendererSliceShadows.h,v 1.4 2006/10/05 16:59:44 geconomos Exp $
