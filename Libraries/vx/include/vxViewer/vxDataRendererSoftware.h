// $Id: vxDataRendererSoftware.h,v 1.5 2006/08/07 19:45:13 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  Encapsulate the data a renderer might need
*/

#ifndef vxDataRendererSoftware_h
#define vxDataRendererSoftware_h


// includes
#include "vxDataRenderer.h"
#include "Image.h"
#include "PixelRGBA.h"
#include "vxElement.h"


// class declaration 
class VX_VIEWER_DLL vxDataRendererSoftware : public vxElement, public vxDataRenderer
{
// functions
public:

  /// default constructor
  vxDataRendererSoftware();

  /// destructor
  virtual ~vxDataRendererSoftware() {}

  /// get image
  Image<PixelRGBA<uint1> > & GetImage() { return m_image; }

  /// set image
  void SetImage(Image<PixelRGBA<uint1> > & image) { m_image = image; }

  /// get depth image
  Image<float4> & GetDepthImage() { return m_depthImage; }

  /// set depth image
  void SetDepthImage( Image<float4> & depthImage ) { m_depthImage = depthImage; }

  /// frame time
  void SetFrameTime(float4 fFrameTime){ m_fFrameTime = fFrameTime; }

  /// frame time
  float4 GetFrameTime() { return m_fFrameTime; }

  /// copy constructor
  vxDataRendererSoftware(const vxDataRendererSoftware & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxDataRendererSoftware & operator=(const vxDataRendererSoftware & other); // should only be public if really implemented!

// data
private:

  /// image that stores what is generated in software
  Image<PixelRGBA<uint1> > m_image;

  /// image that stores the depth per ray generated in software raycasting
  Image<float4> m_depthImage;

  /// frame time
  float4 m_fFrameTime;

}; // class vxDataRendererSoftware


#endif // vxDataRendererSoftware_h


// Revision History:
// $Log: vxDataRendererSoftware.h,v $
// Revision 1.5  2006/08/07 19:45:13  gdavidson
// Extends vxElement
//
// Revision 1.4  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.3  2004/11/17 22:34:16  geconomos
// made destructor virtual
//
// Revision 1.2  2004/10/21 16:40:40  michael
// Added frame time to software renderers to check how long the rendering took
//
// Revision 1.1  2004/08/30 17:15:13  michael
// Data used by renderers is now polymorph as well. Adjusted manipulator, too.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDataRendererSoftware.h,v 1.5 2006/08/07 19:45:13 gdavidson Exp $
// $Id: vxDataRendererSoftware.h,v 1.5 2006/08/07 19:45:13 gdavidson Exp $