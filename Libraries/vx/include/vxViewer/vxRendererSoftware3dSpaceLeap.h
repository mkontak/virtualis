// $Id: vxRendererSoftware3dSpaceLeap.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (mailto:michael@viatronix.com)

#ifndef vxRendererSoftware3dSpaceLeap_h
#define vxRendererSoftware3dSpaceLeap_h


// includes
#include "vxRendererCpu3D.h"


/**
 * Software ray casting renderer.
 */
class VX_VIEWER_DLL vxRendererSoftware3dSpaceLeap : public vxRendererCpu3D
{
// functions
public:

  /// Constructor
  vxRendererSoftware3dSpaceLeap(vxEnvironment & environment);

  /// Destructor
  virtual ~vxRendererSoftware3dSpaceLeap();

  /// perform pre-rendering operations
  //virtual void Initialize();

  /// has something changed that makes redrawing necessary?
  //virtual bool IsRedrawNecessary();

  /// perform pre-rendering operations
  virtual int4 PreRender();

  /// perform post-rendering operations
  virtual int4 PostRender();

protected:

  /// cast one ray through the volume
  virtual void SampleSingleRay(Args & args);

  /// leap over empty space
  virtual void SpaceLeap(Args & args);

  /// leap over empty space
  void RaySpaceLeapDistanceVolume(Args & args);

  /// sampel intencity volume
  inline void RayGetSample(Args & args);

  /// shade the sample
  virtual void RayShadeSample(Args & args);

private:
  /// copy constructor
  vxRendererSoftware3dSpaceLeap(const vxRendererSoftware3dSpaceLeap &);                   // should only be public if really implemented!

  /// assignment operator
  const vxRendererSoftware3dSpaceLeap & operator=(const vxRendererSoftware3dSpaceLeap &); // should only be public if really implemented!

// data
private:

  // distance from boudary
  vxBlockVolume<uint2> * m_pVolumeDOB;

  // iterator
  vxBlockVolumeIterator<uint2> * m_pIterDOB;

}; // class vxRendererSoftware3dSpaceLeap


#endif // vxRendererSoftware3dSpaceLeap_h


// Revision History:
// $Log: vxRendererSoftware3dSpaceLeap.h,v $
// Revision 1.2  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.12.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1  2004/10/26 13:17:29  vxconfig
// moved from colon plugin
//
// Revision 1.4  2004/10/25 23:47:57  michael
// improved space leap raycaster
//
// Revision 1.3  2004/10/21 16:40:04  michael
// derive from ray caster in vxViewer dll, otherwise very slow!!!
//
// Revision 1.2  2004/10/20 21:49:50  michael
// Added copy of the raycaster base class so that we can play around without
// constantly recompiling vxViewer
//
// Revision 1.1  2004/10/20 13:17:34  geconomos
// renamed from viatronix.v3d.viewer.plugins.vc
//
// Revision 1.1  2004/09/09 18:22:32  michael
// added initial files
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererSoftware3dSpaceLeap.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererSoftware3dSpaceLeap.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
