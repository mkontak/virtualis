// $Id: vxRayGenerator.C,v 1.10 2007/03/02 19:17:42 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:geconmos@viatronix.net)


// includes
#include "StdAfx.h"
#include "vxRayGenerator.h"
#include "vxEnvironment.h"
#include "vxRaytracerSettings.h"


// defines
#define FILE_REVISION "$Revision: 1.10 $"


/**
 * Constructor
 */
vxRayGenerator::vxRayGenerator() :
  m_fScaleFactorX( 1.0F ),
  m_fScaleFactorY( 1.0F ),
  m_fPlaneThickness( 1.0F ),
  m_eType( vxRayGenerator::VOLUME ),
  m_pCamera( NULL ),
  m_pRenderingMode( NULL ),
  m_pVolume( NULL )
{
} // vxRayGenerator()


/** 
 * Constructor from environment and type
 *
 * @param   environment   vxEnvironment to initialize from
 * @param   eType         generator type
 */
vxRayGenerator::vxRayGenerator( vxEnvironment & environment, const GeneratorTypeEnum & eType )
{
  SetType( eType );
  Initialize( environment );
} // vxRayGenerator( vxEnvironment & environment, const GeneratorTypeEnum & eType )


/**
 * Initializes the generator from vxRaytracerSettings
 *
 * @param   pSettings   vxRaytracerSettings to initialize from
 */
void vxRayGenerator::Initialize( vxRaytracerSettings * pSettings )
{
  m_pCamera = const_cast< vxCamera * >( &pSettings->GetCamera() );
  m_pRenderingMode = const_cast< vxRenderingMode * >( &pSettings->GetRenderingMode() );
  m_pVolume = &pSettings->GetIntensityVolume();
  m_winrect = pSettings->GetWinRect();
  m_rendersize = pSettings->GetRenderSize();

  if( m_eType == vxRayGenerator::OBLIQUE )
    m_plane = pSettings->GetPlaneOblique();
 
  CommonInit();
} // Initialize( vxRaytracerSettings * pSettings )


/**
 * Initializes the generator from vxEnvironment
 *
 * @param   environment   vxEnvironment to initialize from
 */
void vxRayGenerator::Initialize( vxEnvironment & environment )
{
  m_pCamera = environment.GetCamera();
  m_pRenderingMode = environment.GetRenderingMode();
  m_pVolume = environment.GetIntensityVolume();
  m_winrect = environment.GetWinRect();
  m_rendersize = environment.GetRenderBuffer().GetRenderSize();

  if( m_eType == vxRayGenerator::OBLIQUE )
    m_plane = *environment.GetPlaneOblique();
 
  CommonInit();
} // Initialize( vxEnvironment & environment )


/**
 * Common initialization routine
 */
void vxRayGenerator::CommonInit()
{
  const float4 fFOV = m_pCamera->GetFovDegrees() * M_PI / 180.0F;
  const float4 fTanHalfAngle = tan( fFOV / 2.0F );
  const Vector3Df fRightVector = fTanHalfAngle * static_cast< Vector3Df >( m_pCamera->GetViewRight() );
  const Vector3Df fUpVector    = fTanHalfAngle * static_cast< Vector3Df >( m_pCamera->GetViewUp   () );
  
  // compute up and right vector for near plane
  const float4 fNearDistance = GetNearPlane();
  m_rightNear = fNearDistance * fRightVector;
  m_upNear    = fNearDistance * fUpVector;

  // compute up and right vector for far plane
  const float4 fFarDistance = GetFarPlane();
  m_rightFar = fFarDistance * fRightVector;
  m_upFar    = fFarDistance * fUpVector;

  // compute up and right vector for middle  plane
  const float4 fMiddleDistance = ( fNearDistance + fFarDistance ) / 2.0F;
  m_rightMiddle = fMiddleDistance * fRightVector;
  m_upMiddle    = fMiddleDistance * fUpVector;

  // compute scale factors based on viewport size
  const std::pair< uint2, uint2 > size = m_winrect.GetViewportSize();
  m_fScaleFactorX = size.first < size.second ? 1.0F :  static_cast< float4 >( size.first ) / size.second;
  m_fScaleFactorY = size.first < size.second ? static_cast< float4 >( size.second ) / size.first : 1.0F;
  
  // compute center point on near, middle and far planes
  m_centerOfNearImage   = m_pCamera->GetEye() + static_cast< Vector3Df >( m_pCamera->GetViewDirection() ) * fNearDistance  ;
  m_centerOfFarImage    = m_pCamera->GetEye() + static_cast< Vector3Df >( m_pCamera->GetViewDirection() ) * fFarDistance   ;
  m_centerOfMiddleImage = static_cast< Point3Df >( ( Vector3Df( m_centerOfNearImage ) + Vector3Df( m_centerOfFarImage ) ) / 2.0F );

  // set the plane thickness for oblique views
  if( m_eType == vxRayGenerator::OBLIQUE )
  {
    // default volume rendering?
    if( m_pRenderingMode->GetRenderingAlgorithm() == vxRenderingMode::DEFAULT )
    {
      const Triplef & units = m_pVolume->GetHeader().GetVoxelUnits();
      
      /// non-axis aligned case?
      if( m_plane.GetNormal().GetMaxAbsComponent() != 1 )
      {
        Normal< float4 > normal = m_plane.GetNormal();
        m_fPlaneThickness = max( max( fabs( units.m_x * normal.X() ), fabs( units.m_y * normal.Y() )), fabs( units.m_z * normal.Z() ));
      }
      else // axis-aligned case
      {
        m_fPlaneThickness = units[ m_plane.GetNormal().GetMaxComponentCoord() ];
      }      
    }
    else // assumes thin slab
    {
      /// thin slab uses the slab thickness from the rendering mode
       m_fPlaneThickness = m_pRenderingMode->GetSlabThickness();
    }
  }  // if( m_eType == vxRayGenerator::OBLIQUE )
} // CommonInit()


/**
 * Generates a ray in world coordinates
 *
 * @param   u   pixel column
 * @param   v   pixel row
 * @return    ray segment
 */
RaySegment vxRayGenerator::GenerateRay( const int4 u, const int4 v )
{
  const float4 x = Interpolate( 0.0F, static_cast< float4 >( u ), static_cast< float4 >( m_rendersize.m_x - 1 ), -m_fScaleFactorX, m_fScaleFactorX );
  const float4 y = Interpolate( 0.0F, static_cast< float4 >( v ), static_cast< float4 >( m_rendersize.m_y - 1 ), -m_fScaleFactorY, m_fScaleFactorY );

  Point3Df pointOnNearPlane   = m_centerOfNearImage   + m_rightNear   * x + m_upNear   * y;
  Point3Df pointOnFarPlane    = m_centerOfFarImage    + m_rightFar    * x + m_upFar    * y;
  Point3Df pointOnMiddlePlane = m_centerOfMiddleImage + m_rightMiddle * x + m_upMiddle * y;

  // adjust for parallel projections
  if( m_pCamera->GetProjectionType() == vxCamera::ORTHOGRAPHIC )
  {
    Vector3Df fromNearToFar = m_centerOfFarImage - m_centerOfNearImage;
    pointOnNearPlane = pointOnMiddlePlane - fromNearToFar / 2.0F;
    pointOnFarPlane = pointOnMiddlePlane + fromNearToFar / 2.0F;
  }

  // create ray enpoints
  Vector4d< float4 > fRayBgnWC( pointOnNearPlane, 0 );
  Vector4d< float4 > fRayEndWC( pointOnFarPlane,  1 );

  // oblique ray ends are based around the plane location and thickness
  if( m_eType == vxRayGenerator::OBLIQUE )
  {
    const Normal3Df direction( Point3Df( fRayEndWC ) - Point3Df( fRayBgnWC ) );
    
    Point3Df intersection;
    if( m_plane.GetIntersection( Point3Df( fRayBgnWC ), direction, intersection ) )
    {
      fRayBgnWC = Vector4d< float4 >( intersection - Vector3Df( direction ) * m_fPlaneThickness / 2.0F, 0 );
      fRayEndWC = Vector4d< float4 >( intersection + Vector3Df( direction ) * m_fPlaneThickness / 2.0F, 1 );
    }
  }

  // adjust T parameter
  fRayBgnWC.T() = 0.0F;
  fRayEndWC.T() = ( fRayEndWC - fRayBgnWC ).GetLengthXYZ();

  // return ray segment
  return RaySegment( fRayBgnWC, fRayEndWC );
}// GenerateRay( const int4 u, const int4 v )


/**
 * Gets  the near plane
 * @param tight fit or outer circle
 * @return near plane
 */
float4 vxRayGenerator::GetNearPlane() 
{
  Point3Df center(0.5 * m_pVolume->GetHeader().GetWorldDim());
  Point3Df size(m_pVolume->GetHeader().GetWorldDim());
  float4 fOuterRadius((size - center).GetLength());
  float4 fNearDist(m_pCamera->GetEye().GetEuclideanDistance(center));
  fNearDist -= fOuterRadius;

  return Bound(0.001f, fNearDist, (std::numeric_limits<float>::max)());
} // GetNearPlane()


/**
 * Gets far plane
 * @param tight fit or outer circle
 * @return far plane
 */
float4 vxRayGenerator::GetFarPlane() 
{
  Point3Df center( 0.5 * m_pVolume->GetHeader().GetWorldDim() );
  Point3Df size( m_pVolume->GetHeader().GetWorldDim() );
  float4 fOuterRadius((size - center).GetLength());
  float4 fFarDist(m_pCamera->GetEye().GetEuclideanDistance(center));
  fFarDist += fOuterRadius;

  return fFarDist;
} // GetFarPlane()


// $Log: vxRayGenerator.C,v $
// Revision 1.10  2007/03/02 19:17:42  geconomos
// more code cleanup
//
// Revision 1.9  2007/02/28 20:11:27  geconomos
// code cleanup
//
// Revision 1.8  2007/02/12 14:23:14  geconomos
// Added initialization from vxEnvironment
//
// Revision 1.7  2007/01/26 22:11:22  gdavidson
// Fixed slice thickness
//
// Revision 1.6  2007/01/23 19:04:10  geconomos
// corrected ray setup for parallel projections
//
// Revision 1.5  2007/01/12 14:45:30  romy
// Camera adjusted for Oblique View Issue #5282
//
// Revision 1.4  2007/01/10 14:46:56  geconomos
// correction for ray setup for non axis aligned planes
//
// Revision 1.3  2007/01/10 14:18:18  geconomos
// Issue# 5254: Oblique image not appearing for datasets with large voxel spacings
//
// Revision 1.2  2006/10/04 14:57:04  geconomos
// corrected width of plane for oblique renderings
//
// Revision 1.1  2006/08/03 13:58:31  geconomos
// initial revision
//
// Revision 1.3  2006/07/07 13:35:46  geconomos
// + added initialization from raytracer settings
// + added support for full vr, oblique and mpr ray generation
//
// Revision 1.2  2006/06/27 18:20:28  geconomos
// set T component for beginning ray segment
//
// Revision 1.1  2006/06/22 14:19:42  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRayGenerator.C,v 1.10 2007/03/02 19:17:42 geconomos Exp $
// $Id: vxRayGenerator.C,v 1.10 2007/03/02 19:17:42 geconomos Exp $
