// $Id: vxRendererPush.h,v 1.3 2004/03/30 18:19:45 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)

#ifndef vxRendererPush_h
#define vxRendererPush_h


// includes
#include "vxRenderer.h"


// forward declarations
class vxElementStack;
class vxEnvironment;


/**
 * Defines a renderer that pushes a secondary volume into the environment
 */
class VX_VIEWER_DLL vxRendererPush : public vxRenderer
{
// member functions
public:

  /// constructor
  vxRendererPush( vxEnvironment & environment );
 
  /// destructor
  ~vxRendererPush() {}

  /// perform pre-rendering operations
  virtual int4 PreRender();
  
  /// main render function
  virtual void Render();

  /// perform post-rendering operations
  virtual int4 PostRender();

private:
  
  /// push all the elements
  void PushElements();

  /// safely get the element stack from the environment
  vxElementStack * GetElementStack();

  /// copy constructor
  vxRendererPush(const vxRendererPush & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererPush & operator=(const vxRendererPush & other); // should only be public if really implemented!

}; // class vxRendererPush


#endif // vxRendererPush_h


// $Log: vxRendererPush.h,v $
// Revision 1.3  2004/03/30 18:19:45  frank
// saved the rendering mode on the stack also
//
// Revision 1.2  2004/03/26 15:51:17  frank
// now volume, colormap, and cpu data are all replaced
//
// Revision 1.1  2004/03/26 13:49:46  frank
// working on PET fusion
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererPush.h,v 1.3 2004/03/30 18:19:45 frank Exp $
// $Id: vxRendererPush.h,v 1.3 2004/03/30 18:19:45 frank Exp $
