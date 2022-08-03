// $Id: vxRayGenerator.h,v 1.6 2007/03/02 19:17:42 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:geconmos@viatronix.net)


// pragama declarations
#pragma once


// include declarations
#include "vxPlaneOblique.h"
#include "vxPlaneOrtho.h"
#include "RaySegment.h"
#include "vxWinRect.h"

// forward delcarations
class vxRaytracerSettings;
class vxEnvironment;
class vxCamera;
class vxRenderingMode;
template < typename VoxelType > class vxBlockVolume;


/**
 * Computes the ray of intersection for a given image pixel.
 */
class VX_VIEWER_DLL vxRayGenerator 
{
// member functions
public:

  enum GeneratorTypeEnum{ VOLUME, OBLIQUE };

  /// constructor
  vxRayGenerator();

  /// constructor from environment and type
  vxRayGenerator( vxEnvironment & environment, const GeneratorTypeEnum & eType );

  /// initialize from raytracer settings
  void Initialize( vxRaytracerSettings * pSettings );

  /// initialize from vx environment
  void Initialize( vxEnvironment & environment );

  /// generate a ray in world coordinates
  RaySegment GenerateRay( const int4 u, const int4 v );

  /// gets generator type
  const GeneratorTypeEnum & GetType() const { return m_eType; }

  /// sets generator type
  void SetType( const GeneratorTypeEnum & eType ){ m_eType = eType; }

private:

  /// common initilaization routine
  void CommonInit();

  /// gets near plane
  float4 GetNearPlane();

  /// gets far plane
  float4 GetFarPlane();


// member variables
private:
  
  /// generator type
  GeneratorTypeEnum m_eType;

  /// scaling factor in x
  float4 m_fScaleFactorX;
  
  /// scaling factor in y
  float4 m_fScaleFactorY;

  /// near image plane center point 
  Point3Df m_centerOfNearImage;   

  /// near image plane up vector
  Vector3Df m_upNear;
  
  /// near image plane right vector
  Vector3Df m_rightNear;

  /// far image plane center point 
  Point3Df m_centerOfFarImage;   
  
  /// far image plane up vector
  Vector3Df m_upFar;

  /// far image plane right vector
  Vector3Df m_rightFar;

  /// middle image plane center point 
  Point3Df m_centerOfMiddleImage; 

  /// middle image plane right vector
  Vector3Df m_rightMiddle;
 
  /// middle image plane up vector
  Vector3Df m_upMiddle;

  /// current oblique plane
  vxPlaneOblique<float4> m_plane;

  /// current plane thickness
  float4 m_fPlaneThickness;

  /// current camera
  vxCamera * m_pCamera;

  /// current rendering mode
  vxRenderingMode * m_pRenderingMode;

  /// current intensity volume
  vxBlockVolume< uint2 > * m_pVolume;

  /// current winrect
  vxWinRect m_winrect;

  /// current render size
  Triple< uint4 > m_rendersize;

}; // class vxRayGenerator


// $Log: vxRayGenerator.h,v $
// Revision 1.6  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.5  2007/02/28 20:11:27  geconomos
// code cleanup
//
// Revision 1.4  2007/02/12 14:23:20  geconomos
// Added initialization from vxEnvironment
//
// Revision 1.3  2006/08/03 14:09:59  geconomos
// renamed vxRayGeneratorOpenGL -> vxRayGenerator
//
// Revision 1.2  2006/07/07 13:35:46  geconomos
// + added initialization from raytracer settings
// + added support for full vr, oblique and mpr ray generation
//
// Revision 1.1  2006/06/21 18:46:32  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRayGenerator.h,v 1.6 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxRayGenerator.h,v 1.6 2007/03/02 19:17:42 geconomos Exp $
