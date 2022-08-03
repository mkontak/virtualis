// $Id: vxViewerTypeMesh.h,v 1.1 2007/08/09 21:01:05 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)

#pragma once

// includes
#include "vxViewerType3D.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxViewerTypeMesh: public vxViewerType3D
{
// functions
public:

  /// constructor
  vxViewerTypeMesh(vxEnvironment & environment);

  /// copy Constructor
  virtual ~vxViewerTypeMesh();

  /// returns if the viewer supports dropping of the specified type
  virtual bool CanDragDrop(const DragDropEnum eSourceType);

 
  /// returns the name of the class for type checking (here till dynamic_cast works across DLLs)
  virtual const std::string GetClassString() const { return "vxViewerTypeMesh"; }

private:

  /// copy constructor
  vxViewerTypeMesh(const vxViewerTypeMesh & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxViewerTypeMesh & operator=(const vxViewerTypeMesh & other); // should only be public if really implemented!


}; // class vxViewerTypeMesh



// $Log: vxViewerTypeMesh.h,v $
// Revision 1.1  2007/08/09 21:01:05  romy
// initial version
//
// $Id: vxViewerTypeMesh.h,v 1.1 2007/08/09 21:01:05 romy Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewerTypeMesh.h,v 1.1 2007/08/09 21:01:05 romy Exp $
