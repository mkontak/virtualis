// $Id: vxRendererSampleRange.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/**
 *  Define the sample range for the render
 */


#ifndef vxRendererSampleRange_h
#define vxRendererSampleRange_h


// includes
#include "vxRenderer.h"


// class definition
class VX_VIEWER_DLL vxRendererSampleRange : public vxRenderer
{
// functions
public:

  /// constructor
  vxRendererSampleRange(vxEnvironment & environment);

  /// destructor
  virtual ~vxRendererSampleRange() {};
  
public:
  
  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// perform rendering operations
  virtual void Render();
  
private:

  /// copy constructor
  vxRendererSampleRange(const vxRendererSampleRange & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererSampleRange & operator=(const vxRendererSampleRange & other); // should only be public if really implemented!

  /// draw a simple circle
  void DrawCircle();

  /// draw a circle in 3D within the given oblique quadrilateral
  void DrawObliqueCircle( const Quadrilateral & quadrilateral );

  /// Render a wireframe sphere
  void RenderSphereWireframe( const Point3Df & position, const float4 fRadius ) const;

// inner classes
public:

  /**
   * Local class declaration for the path of a circle.  It precomputes the path
   * in the constructor, then you can render the points using OpenGL in any major plane.
   */
  class vxCirclePath
  {
  // member functions
  public:

    /// Constructor
    vxCirclePath( const uint4 uNumSteps, const float4 fRadius );

    /// Render the circle in the XY plane
    void RenderXY() const;

    // Render the circle in the YZ plane
    void RenderYZ() const;

    // Render the circle in the XZ plane
    void RenderXZ() const;

  // data members
  private:

    /// The points on the circle
    std::vector< Point2D< float4 > > m_circlePoints;

  }; // class vxCirclePath

}; // vxRendererSampleRange


#endif // vxRendererSampleRange_h


// $Log: vxRendererSampleRange.h,v $
// Revision 1.2  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.1.1.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/01/23 16:23:56  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.9  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.8  2003/09/23 22:08:27  michael
// added "mailto:"
//
// Revision 1.7  2003/05/19 14:40:40  frank
// Removed unused return parameter
//
// Revision 1.6  2003/05/16 20:46:41  dongqing
// code review
//
// Revision 1.5  2003/05/16 18:38:39  michael
// code review
//
// Revision 1.4  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.3  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.2  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.1  2002/06/19 18:38:00  michael
// added renderer for the smaple range circle needed to display the region used
// by the interactive segmentation.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSampleRange.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererSampleRange.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
