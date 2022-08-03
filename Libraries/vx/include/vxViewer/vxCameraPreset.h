// $Id: vxCameraPreset.h,v 1.2 2005/02/10 16:32:13 michael Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)

#ifndef vxCameraPreset_h
#define vxCameraPreset_h


// includes
#include "vxCamera.h"
#include "Box.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxCameraPreset
{
// functions
public:

  /// default constructor
  vxCameraPreset();
  
  /// constructor
  vxCameraPreset(const std::string & sName, const vxCamera & camera, const Box<float4> & boundingBox);

  /// copy constructor
  vxCameraPreset(const vxCameraPreset & other);

  /// assignment operator
  const vxCameraPreset & operator=(const vxCameraPreset & other);

  /// get name
  std::string GetName() const { return m_sName; };

  /// set name
  void SetName(const std::string & sName) { m_sName = sName; };

  /// get camera
  vxCamera GetCamera() const { return m_camera; };

  /// set camera
  void SetCamera(const vxCamera & camera) { m_camera = camera; };

  /// get bounding box
  Box<float4> GetBoundingBox() const { return m_boundingBox; };

  /// set bounding box
  void SetBoundingBox(const Box<float4> & boundingBox) { m_boundingBox = boundingBox; };

// data
private:

  /// name
  std::string m_sName;

  /// camera
  vxCamera m_camera;

  /// bounding box
  Box<float4> m_boundingBox;
};


#endif


// Revision History:
// $Log: vxCameraPreset.h,v $
// Revision 1.2  2005/02/10 16:32:13  michael
// Added initial version (after recovering some older version from Attic)
//
// Revision 1.1.2.1  2005/02/10 15:46:01  michael
// Added initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCameraPreset.h,v 1.2 2005/02/10 16:32:13 michael Exp $
// $Id: vxCameraPreset.h,v 1.2 2005/02/10 16:32:13 michael Exp $
