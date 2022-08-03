// $Id: vxDataRenderer.h,v 1.2 2004/11/17 22:34:16 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner {meissner@viatronix.com)

/*
  Encapsulate the data a renderer might need
 */

#ifndef vxDataRenderer_h
#define vxDataRenderer_h


// class declaration 
class VX_VIEWER_DLL vxDataRenderer
{
// functions
public:

  /// default constructor
  vxDataRenderer() {}

  /// destructor
  virtual ~vxDataRenderer() {}

private:

  /// copy constructor
  vxDataRenderer(const vxDataRenderer & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxDataRenderer & operator=(const vxDataRenderer & other); // should only be public if really implemented!

}; // class vxDataRenderer


#endif // vxDataRenderer_h


// Revision History:
// $Log: vxDataRenderer.h,v $
// Revision 1.2  2004/11/17 22:34:16  geconomos
// made destructor virtual
//
// Revision 1.1  2004/08/30 17:15:13  michael
// Data used by renderers is now polymorph as well. Adjusted manipulator, too.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDataRenderer.h,v 1.2 2004/11/17 22:34:16 geconomos Exp $
// $Id: vxDataRenderer.h,v 1.2 2004/11/17 22:34:16 geconomos Exp $