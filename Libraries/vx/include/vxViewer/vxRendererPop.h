// $Id: vxRendererPop.h,v 1.3 2004/03/30 18:19:45 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)

#ifndef vxRendererPop_h
#define vxRendererPop_h


// includes
#include "vxRenderer.h"


// forward declarations
class vxElementStack;
class vxEnvironment;


/**
 * Defines a renderer that pushes a secondary volume into the environment
 */
class VX_VIEWER_DLL vxRendererPop : public vxRenderer
{
// member functions
public:

  /// constructor
  vxRendererPop( vxEnvironment & environment );
 
  /// destructor
  ~vxRendererPop() {}

  /// perform pre-rendering operations
  virtual int4 PreRender();
  
  /// main render function
  virtual void Render();

  /// perform post-rendering operations
  virtual int4 PostRender();

private:

  /// pop all the elements
  void PopElements();
  
  /// safely get the element stack from the environment
  vxElementStack * GetElementStack();

  /// copy constructor
  vxRendererPop(const vxRendererPop & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererPop & operator=(const vxRendererPop & other); // should only be public if really implemented!

}; // class vxRendererPop


#endif // vxRendererPop_h


// $Log: vxRendererPop.h,v $
// Revision 1.3  2004/03/30 18:19:45  frank
// saved the rendering mode on the stack also
//
// Revision 1.2  2004/03/26 15:51:17  frank
// now volume, colormap, and cpu data are all replaced
//
// Revision 1.1  2004/03/26 13:49:46  frank
// working on PET fusion
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererPop.h,v 1.3 2004/03/30 18:19:45 frank Exp $
// $Id: vxRendererPop.h,v 1.3 2004/03/30 18:19:45 frank Exp $
