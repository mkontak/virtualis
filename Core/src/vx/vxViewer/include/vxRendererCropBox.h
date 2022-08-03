// $Id: vxRendererCropBox.h,v 1.2 2004/03/11 17:42:54 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererCropBox.h,v 1.2 2004/03/11 17:42:54 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Manjushree Nulkar (manju@viatronix.com)

/**
 * Renderer for displaying the crop box
 */

#ifndef vxRendererCropBox_h
#define vxRendererCropBox_h


// includes
#include "vxRendererAnnotations.h"


// class definition
class VX_VIEWER_DLL vxRendererCropBox : public vxRenderer
{
// functions
public:
  
  /// constructor
  vxRendererCropBox(vxEnvironment &rEnv);

  /// destructor
  virtual ~vxRendererCropBox() {}

  /// perform rendering operations
  virtual void Render();
  
// functions
protected:

  /// copy constructor
  vxRendererCropBox(const vxRendererCropBox & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererCropBox & operator=(const vxRendererCropBox & other); // should only be public if really implemented!

}; // vxRendererCropBox


#endif // vxRendererCropBox_h


// Revision History:
// $Log: vxRendererCropBox.h,v $
// Revision 1.2  2004/03/11 17:42:54  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/19 14:38:33  frank
// Removed unused return parameter
//
// Revision 1.7  2003/05/16 15:34:07  dongqing
// code review
//
// Revision 1.6  2003/05/16 14:09:28  michael
// code review
//
// Revision 1.5  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.4  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.3  2002/07/02 20:45:00  geconomos
// Exporting classes .
//
// Revision 1.2  2002/06/27 14:06:52  michael
// cosmetics
//
// Revision 1.1  2002/06/11 14:56:31  manju
// Renderer to display handle points of crop box.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererCropBox.h,v 1.2 2004/03/11 17:42:54 frank Exp $
// $Id: vxRendererCropBox.h,v 1.2 2004/03/11 17:42:54 frank Exp $
