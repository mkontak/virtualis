// $Id: vxRendererContour.h,v 1.4 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxRendererContour_h
#define vxRendererContour_h


// includes
#include "vxRenderer.h"


// forwards
class vxContour;


/**
 * Renderer for displaying the contour lassos
 */
class VX_VIEWER_DLL vxRendererContour : public vxRenderer
{
// functions
public:
  
  /// constructor
  vxRendererContour( vxEnvironment & environment );

  /// destructor
  virtual ~vxRendererContour() {}

  /// perform rendering operations
  virtual void Render();

protected: 

  /// copy constructor
  vxRendererContour(const vxRendererContour & other);                   

  /// assignment operator
  const vxRendererContour & operator=(const vxRendererContour & other); 

private:

  /// render the collection of contours
  void RenderLassoVector();

  /// render the adjustment contours
  void RenderAdjustmentContour();

  /// render each lasso from the vector
  void RenderSingleLasso( vxContour * pRoi );

  /// check if a lasso belongs to the current slice
  bool IsInSlice( const Point3Df & fHandlePoint, const Vector3D<float4> & fUnits, const Vector3D<int4> & volDim );
  
}; // class vxRendererContour


#endif // vxRendererContour_h


// Revision History:
// $Log: vxRendererContour.h,v $
// Revision 1.4  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.3  2006/03/13 13:54:16  frank
// added adjustment contours
//
// Revision 1.2.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.2  2004/03/12 19:54:18  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/07/11 19:49:15  michael
// final code reviews
//
// Revision 1.5  2003/05/19 14:38:02  frank
// Removed unused return parameter
//
// Revision 1.4  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.3  2003/05/15 20:09:59  frank
// Moving away from old point class
//
// Revision 1.2  2003/05/13 17:43:00  frank
// code review
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.2  2002/07/11 21:09:11  manju
// Speed up.
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.1  2002/06/26 17:26:15  manju
// Renderer to draw the restricted lasso.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererContour.h,v 1.4 2007/02/15 00:08:23 jmeade Exp $
// $Id: vxRendererContour.h,v 1.4 2007/02/15 00:08:23 jmeade Exp $
