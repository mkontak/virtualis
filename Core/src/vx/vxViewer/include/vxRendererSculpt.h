// $Id: vxRendererSculpt.h,v 1.2 2004/03/12 19:54:28 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxRendererSculpt_h
#define vxRendererSculpt_h


// Includes
#include "vxRenderer.h"
#include "vxContour.h"


/**
 * Render the sculpt lasso
 */
class VX_VIEWER_DLL vxRendererSculpt : public vxRenderer
{
// Member Functions
public:
  
  /// Constructor
  vxRendererSculpt( vxEnvironment & environment );

  /// Destructor
  virtual ~vxRendererSculpt() {}

  /// perform rendering operations
  virtual void Render();

private:

  /// Copy constructor
  vxRendererSculpt( const vxRendererSculpt & other );                    // should only be public if really implemented!

  /// Assignment operator.
  const vxRendererSculpt & operator =( const vxRendererSculpt & other ); // should only be public if really implemented!

  /// Render Lasso annotation
  void RenderContour( vxContour * pContour, bool bPrimary, bool bFlag );

}; // class vxRendererSculpt


#endif // vxRendererSculpt_h


// $Log: vxRendererSculpt.h,v $
// Revision 1.2  2004/03/12 19:54:28  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/22 13:04:30  frank
// code review
//
// Revision 1.7  2003/05/19 14:40:40  frank
// Removed unused return parameter
//
// Revision 1.6  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.5  2003/05/16 13:22:14  frank
// formatting
//
// Revision 1.4  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.3  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.2  2002/05/17 14:58:08  dmitry
// Got rid of unused stuff, Push/pop opengl matrices
//
// Revision 1.1  2002/04/18 18:42:03  frank
// Initial version.
//
// $Id: vxRendererSculpt.h,v 1.2 2004/03/12 19:54:28 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSculpt.h,v 1.2 2004/03/12 19:54:28 frank Exp $
