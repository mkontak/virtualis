// $Id: vxRendererMesh.h,v 1.2 2007/08/09 16:59:54 frank Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#pragma once


// includes
#include "vxRenderer.h"
#include "vxMesh.h"
#include "GlDisplayList.h"


/**
 * Renders the vessel centerline and outline in 3D.
 */
class VX_VIEWER_DLL vxRendererMesh : public vxRenderer
{
// methods
public:

  /// constructor
  vxRendererMesh( vxEnvironment & environment );

  /// destructor
  virtual ~vxRendererMesh();

  /// has something changed that makes redrawing necessary?
  virtual bool IsRedrawNecessary();

  /// render
  virtual void Render();

private:

  /// copy constructor
  vxRendererMesh(const vxRendererMesh & other);                   // not implemented

  /// assignment operator
  const vxRendererMesh & operator=(const vxRendererMesh & other); // not implemented

  /// create a sample mesh
  vxMesh * GetSampleMesh() const;

// data
private:

  /// the OpenGl display list used to improve performance
  OpenGLUtils::GlDisplayList m_displayList;

}; // class vxRendererMesh 


// $Log: vxRendererMesh.h,v $
// Revision 1.2  2007/08/09 16:59:54  frank
// moved display list class to vxViewer
//
// Revision 1.1  2007/07/14 01:23:12  frank
// Added mesh renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererMesh.h,v 1.2 2007/08/09 16:59:54 frank Exp $
// $Id: vxRendererMesh.h,v 1.2 2007/08/09 16:59:54 frank Exp $
