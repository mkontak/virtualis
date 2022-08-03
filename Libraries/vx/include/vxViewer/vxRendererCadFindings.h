// $Id: vxRendererCadFindings.h,v 1.2 2007/04/17 20:06:03 romy Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)

#ifndef vxRendererCadFindings_h
#define vxRendererCadFindings_h


// includes
#include "vxRenderer.h"


/**
 * Renderer for displaying the contour lassos
 */
class VX_VIEWER_DLL vxRendererCadFindings : public vxRenderer
{
// functions
public:
  
  /// Constructor
  vxRendererCadFindings( vxEnvironment & environment );

  /// Destructor
  virtual ~vxRendererCadFindings() {}

  /// perform rendering operations
  virtual void Render();

protected: 

  /// Copy constructor
  vxRendererCadFindings(const vxRendererCadFindings & other);                   

  /// Assignment operator
  const vxRendererCadFindings & operator=(const vxRendererCadFindings & other); 

protected:

  //Draw a circile based on the positions
  void DrawCircle( Point3Df & fWorldPosition, PixelRGBA<uint1> & color , uint4 uOrientation )const;

  //Gets the bounding box around a centroid.
  Box<int4> GetBoundingBox( const Point3D<float4> & centroid  ) const;

}; // class vxRendererContour


#endif // vxRendererContour_h


// $Log: vxRendererCadFindings.h,v $
// Revision 1.2  2007/04/17 20:06:03  romy
// added code to display the cad findings in nearby planes also.
//
// Revision 1.1  2007/04/11 20:49:58  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererCadFindings.h,v 1.2 2007/04/17 20:06:03 romy Exp $
// $Id: vxRendererCadFindings.h,v 1.2 2007/04/17 20:06:03 romy Exp $