// $Id: vxRibbon.C,v 1.4.2.3 2009/06/25 20:14:12 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxRibbon.h"
#include "Matrix.h"
#include "Plane.h"
#include "Box.h"
#include "Line.h"
#include "Intersection.h"
#include "ValueRange.h"
#include "PrimitiveUtilities.h"
#include "vxUtils.h"
#include "File.h"
#include "vxVessel.h"
#include "vxEnvironment.h"
#include "vxVesselPreferences.h"


// namespaces
using namespace std;


// Defines
#define FILE_REVISION "$Revision: 1.4.2.3 $"


/**
 * Compute all the scanlines for a projected curved MPR projection given all the necessary information.
 *
 * @param vessel      the vessel of interest
 * @param environment the environment
 * @param imageDim    the dimensions of the desired image
 */
void vxRibbon::ComputeScanlinesCurvedProjected(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim)
{
  // set the projection type for display
  m_sProjectionType = "Curved Projected MPR";

  // make sure there is some data in the vessel
  if ( vessel.size() < 2 )
    return;

  // get the curved plane
  vxPlaneCurved * pPlaneCurved = dynamic_cast< vxPlaneCurved * >( environment.GetPlaneCurved() );
  if ( pPlaneCurved == NULL )
  {
    LogDbg( "Curved Plane pointer is NULL", "vxRibbon", "ComputeScanlinesCurvedProjected" );
    return;
  }

  // set up coordinate conversion matrices
  const float4 fAngleRadians( float4( pPlaneCurved->GetAngle() ) / 360.0F * 2.0F * M_PI );
  const float4 fRibbonWidthHalf = pPlaneCurved->GetWidth()*2;
  m_widthHalf = fRibbonWidthHalf;
  const Vector3D<float4> units( environment.GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  const Vector3D<int4>   dim  ( environment.GetIntensityVolume()->GetHeader().GetVolDim    () );
  
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale(Vector<float4>(1.0F / units.X(), 1.0F / units.Y(), 1.0F / units.Z()));
  
  // pre-processing compute the projection of vessel on Y and Z axis - curvProjection
  vector< float4 > curvProjection; 
  curvProjection.push_back(0.0F);
  double fAccLen = 0.0F;
  Point<float4> prevPoint = vessel[0].GetPosition();
  float4 minX, maxX, minY, maxY, minZ, maxZ;
  minX = minY = minZ = 1.0e5;
  maxX = maxY = maxZ = -1.0e5;
  for ( int4 v=1; v < vessel.size(); v++ )
  {
    Point<float4> currPoint = vessel[v].GetPosition();
    Vector<float4> currSegment = currPoint - prevPoint;

    const float4 fMaxRadius = vessel[v].GetDiameterRange().GetMaximum() * 0.5F;
    
    // bounding box
    Vector<float4> vDeviation( fMaxRadius, fMaxRadius, fMaxRadius );
    Point<float4> minCurrPoint = currPoint - vDeviation;
    Point<float4> maxCurrPoint = currPoint + vDeviation;

    minX = minX > minCurrPoint.X() ? minCurrPoint.X() : minX;
    minY = minY > minCurrPoint.Y() ? minCurrPoint.Y() : minY;
    minZ = minZ > minCurrPoint.Z() ? minCurrPoint.Z() : minZ;

    maxX = maxX < maxCurrPoint.X() ? maxCurrPoint.X() : maxX;
    maxY = maxY < maxCurrPoint.Y() ? maxCurrPoint.Y() : maxY;
    maxZ = maxZ < maxCurrPoint.Z() ? maxCurrPoint.Z() : maxZ;

    fAccLen += currPoint.GetEuclideanDistance( prevPoint );
    curvProjection.push_back( (float4)(fAccLen) );
    prevPoint = currPoint;
  }
  Point3D<float4> boxCenter( 0.5f*(minX+maxX), 0.5f*(minY+maxY), 0.5F*(minZ+maxZ) );
  // the size of cropping box
  Vector3D<float4> boxRadius(fRibbonWidthHalf, fRibbonWidthHalf, fRibbonWidthHalf);
  Box<float4> BoundingBox( boxCenter-boxRadius, boxCenter+boxRadius );
  const Vector<float4> centerOfVolume( BoundingBox.GetMidPoint() );
  vxCamera & refCamera = GetRefCamera();
  vxCamera & renderCamera = GetRenderCamera();
  const Normal<float4> viewUpDir = refCamera.GetViewUp(); 
  renderCamera.SetViewUp( viewUpDir );

  // define the transformation of the camera. 
  Matrix<float4> transMat;
  transMat.Translate( centerOfVolume * ( -1.0F ) );
  transMat.Rotate( Normal<float4>( viewUpDir ), fAngleRadians );
  transMat.Translate( centerOfVolume );

  // set camera for rendering
  //renderCamera.SetBoundingBox( BoundingBox );  
  renderCamera.SetViewReference( Point3Df(centerOfVolume) ); 
  //refCamera.SetBoundingBox( BoundingBox );
  refCamera.SetViewReference( Point3Df(centerOfVolume) );

  Vector<float4> viewVector( refCamera.GetViewDirection() );
  Point<float4> eye( centerOfVolume - viewVector*BoundingBox.GetOuterRadius() );  
  renderCamera.SetEye( transMat*eye );

  // define the viewing box
  const float4 fBoundingRadius( BoundingBox.GetOuterRadius() );
  const Normal<float4> rotatedRightNormal( renderCamera.GetViewRight() );
  const Vector<float4> rotatedRightVector = Vector3Df(rotatedRightNormal);
  
  Plane leftPlane ( Point3Df( centerOfVolume - rotatedRightVector * fBoundingRadius ), -rotatedRightNormal);
  Plane rightPlane( Point3Df( centerOfVolume + rotatedRightVector * fBoundingRadius ), -rotatedRightNormal);

  //it should be the number of the scanlines
  int4 yStart( 0 );
  int4 yEnd( fAccLen );
  if ( yEnd > GetScanlineData().size()-1 )
    yEnd = GetScanlineData().size()-1;

  // loop over all scanlines
  const Point<float4> zeroPoint( 0.0F, 0.0F, 0.0F );
  m_scanlineData.resize(imageDim.m_y);
  int4 y=yEnd;
  for (; y<GetScanlineData().size(); y++)
  {
    m_scanlineData[y].m_leftPoint = zeroPoint;
    m_scanlineData[y].m_rightPoint = zeroPoint;
    m_scanlineData[y].m_centerlinePoint = zeroPoint;
  }
  // scanlines should be in world space. 
  // we keep these scanlines here and render them later in render. 

  // loop over all the scanlines
  for (y=0; y<yEnd; y++)
  {
    // create a line in world space
    const float4 fInterpolatedLength( Interpolate( (float4)yEnd, (float4)y, (float4)yStart, (float4)fAccLen, 0.0F ) );
    
    int4 iInterpolatedIndex( 0 );
    for ( bool bStillLookingForIndex(true); iInterpolatedIndex < vessel.size() && bStillLookingForIndex; iInterpolatedIndex++ )
    {
      if ( curvProjection[iInterpolatedIndex] >= fInterpolatedLength ) 
        bStillLookingForIndex = false;
    }
    iInterpolatedIndex--; 

    // must find a value - then interpolate it 
    Point<float4> centralPointWorld;
    if ( iInterpolatedIndex ) 
    {
      float fL0 = iInterpolatedIndex == 0 ? 0.0F : curvProjection[iInterpolatedIndex-1];
      float fL1 = curvProjection[iInterpolatedIndex];
      Point<float4> pnt0 = vessel[ iInterpolatedIndex-1 ].GetPosition();
      Point<float4> pnt1 = vessel[ iInterpolatedIndex ].GetPosition();
      centralPointWorld.m_x = Interpolate( fL1, fInterpolatedLength, fL0, pnt1.m_x, pnt0.m_x ); 
      centralPointWorld.m_y = Interpolate( fL1, fInterpolatedLength, fL0, pnt1.m_y, pnt0.m_y ); 
      centralPointWorld.m_z = Interpolate( fL1, fInterpolatedLength, fL0, pnt1.m_z, pnt0.m_z ); 
    }
    else
      centralPointWorld = vessel[ iInterpolatedIndex ].GetPosition();

    Line scanlineRay(centralPointWorld, rotatedRightNormal);

    // intersect the line with the left plane
    Point<float4> leftIntersectionPoint;
    Point<float4> rightIntersectionPoint;
    Intersection::ComputeIntersection( scanlineRay,  leftPlane,  leftIntersectionPoint );
    Intersection::ComputeIntersection( scanlineRay, rightPlane, rightIntersectionPoint );
    
    Normal<float4> test2right( rightIntersectionPoint - centralPointWorld ) ;
    if ( test2right.Dot( rotatedRightVector )  < 0.0 ) //it is wrong interesction direction
    {
      Point<float4> tmpPoint = leftIntersectionPoint;
      leftIntersectionPoint = rightIntersectionPoint;
      rightIntersectionPoint = tmpPoint;
    }

    m_scanlineData[y].m_leftPoint = leftIntersectionPoint;
    m_scanlineData[y].m_rightPoint = rightIntersectionPoint;
    m_scanlineData[y].m_centerlinePoint = centralPointWorld;

  } // loop over all the scanlines
} // ComputeScanlinesCurvedProjected()




/**
 * Compute all the scanlines for a curved MPR projection given all the necessary information.
 *
 * @param vessel      the vessel of interest
 * @param environment the environment
 * @param imageDim    the dimensions of the desired image
 */
void vxRibbon::ComputeScanlinesCurved(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim)
{
  ComputeScanlinesCurved_XYZ( vessel, environment, imageDim ); 
}


/**
 * Compute all the scanlines for a curved MPR projection given all the necessary information. ( only used when the viewing mode is "vessel" )
 *
 * @param vessel      the vessel of interest
 * @param environment the environment
 * @param imageDim    the dimensions of the desired image
 */
void vxRibbon::ComputeScanlinesCurved_VESSEL(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim)
{
  // set the projection type for display
  m_sProjectionType = "Curved MPR";

  // early return if there is no data
  if( vessel.size() < 2 )
    return;

  // get the curved plane
  vxPlaneCurved * pPlaneCurved = dynamic_cast< vxPlaneCurved * >( environment.GetPlaneCurved() );
  if ( pPlaneCurved == NULL )
  {
    LogDbg( "Curved Plane pointer is NULL", "vxRibbon", "ComputeScanlinesCurved" );
    return;
  }

  ClampZoom( pPlaneCurved, environment.GetIntensityVolume() );
  //get the camera
  vxCamera & renderCamera = GetRenderCamera();
  renderCamera = GetRefCamera();

  // set up coordinate conversion matrices   
  const float4 fAngleRadians( GetProjectionAngleDegrees( pPlaneCurved ) / 360.0F * 2.0F * M_PI );
  const Vector3D<float4> units( environment.GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  const Vector3D<int4>   dim  ( environment.GetIntensityVolume()->GetHeader().GetVolDim    () );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale(Vector<float4>(1.0F / units.X(), 1.0F / units.Y(), 1.0F / units.Z()));

  //compute the coordinate system fit for the vessel and the plane where the centerline is projected onto 
  Point<float4> p0 = vessel.at(0).GetPosition();
  Point<float4> p1 = vessel.at( vessel.size()-1 ).GetPosition();
  Vector<float4> upVect( p1 - p0 );
      
  // find the right orientation up or down
  const Normal<float4> vXAxis( 1.0f, 0.0f, 0.0f );
  const Normal<float4> vYAxis( 0.0f, 1.0f, 0.0f );
  const Normal<float4> vZAxis( 0.0f, 0.0f, 1.0f );      
      
  const float4 fUpOnXProjection( upVect.Dot(vXAxis) );
  const float4 fUpOnYProjection( upVect.Dot(vYAxis) );
  const float4 fUpOnZProjection( upVect.Dot(vZAxis) );

  if ( fabs(fUpOnZProjection) > fabs(fUpOnYProjection) )
  {
     if ( fabs(fUpOnZProjection) > fabs(fUpOnXProjection) )  // most on Z axis
     {
        if ( fUpOnZProjection < 0.0F )
          upVect *= -1.0F;
     }
     else // most on X axis
     {
        if ( fUpOnXProjection < 0.0F )
          upVect *= -1.0F;
     }
  }
  else
  {
     if ( fabs(fUpOnYProjection) > fabs(fUpOnXProjection) )  // most on Y axis
     {
        if ( fUpOnYProjection < 0.0F )
          upVect *= -1.0F;
     }
     else //most on X axis
     {
        if ( fUpOnXProjection < 0.0F )
          upVect *= -1.0F;
     }
  }      

  Vector<float4> vfHornAxis, vfViewDir;
  if ( upVect.GetLength() > 1.0f )
  { 
     if ( fabs(upVect.X()) > 1.0f || fabs(upVect.Y()) > 1.0f )
     {
        vfViewDir.m_x = upVect.Y();
        vfViewDir.m_y = -upVect.X();
        vfViewDir.m_z = 0.0f;
     }
     else
     {
        vfViewDir.m_x = upVect.Z();
        vfViewDir.m_y = 0.0;
        vfViewDir.m_z = -upVect.X();
     }
     vfHornAxis = vfViewDir.Cross(upVect);
     vfViewDir.Normalize();
     vfHornAxis.Normalize();
  }
  else
  {
    vfViewDir  = Vector3D<float4>( renderCamera.GetViewDirection() );
    vfHornAxis = Vector3D<float4>( renderCamera.GetViewRight() );
  }
  Point<float4>  vfCenter;
  vfCenter.m_x = ( p0.m_x + p1.m_x )/2.0;
  vfCenter.m_y = ( p0.m_y + p1.m_y )/2.0;
  vfCenter.m_z = ( p0.m_z + p1.m_z )/2.0;

  //rotate the camera and the projective plane
  Matrix<float4> matrixRotate;
  matrixRotate.Identity();
  Normal3D<float4> axis ( upVect ); 
  matrixRotate.Rotate( axis, fAngleRadians + M_PI );
  vfViewDir = matrixRotate * vfViewDir;
  vfHornAxis = matrixRotate * vfHornAxis;
    
  // translate the render camera to the middle
  renderCamera.SetEye( vfCenter );
  renderCamera.SetViewUp( axis );
  renderCamera.SetViewDirection( Normal3D<float4>(vfViewDir) );

  // save camera to environment so the 3D orientation lettering is correct
  * environment.GetCamera() = renderCamera;
  //environment.GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

  // get the total vessel length
  double fAccLen = vessel.GetLength( vessel.GetEntireDomain() );

  // decide the overall height of the image in mm
  const float4 fAspectRatio( float4( imageDim.m_y ) / float4( imageDim.m_x ) );
  const float4 fWidthMM( pPlaneCurved->GetWidth() * 2 );
  m_fHeightImageMM = fWidthMM * fAspectRatio;
  const float4 fHeightVesselMM( vessel.GetLength( vessel.GetEntireDomain() ) );
  const float4 fMMperPixel( fWidthMM / imageDim.m_x );
  const float4 fHeightVesselScanlines( imageDim.m_y * fHeightVesselMM / m_fHeightImageMM );
  const float4 fInitialScanline( imageDim.m_y / 2.0F - fHeightVesselScanlines / 2.0F );

  // setup the scanline that each vessel section will be located at
  std::vector< float4 > scanlineForSections;
  int i=0;
  for( ; i<vessel.size(); ++i )
  {
    float4 fLengthMM( vessel.GetLength( vxVessel::MeasurePair( 0, i ) ) );
    scanlineForSections.push_back( fInitialScanline + fLengthMM / fMMperPixel );
  }

  // setup the vessel section that will be on each scanline
  // -1 indicates below start -2 indicates above end
  m_scanlineData.resize(imageDim.m_y);
  for( i=0; i<imageDim.m_y; ++i )
  {
    float fLocation( -1 );
    for( int j=0; j<scanlineForSections.size()-1; ++j )
    {
      ValueRange<float4> scanlineRangeForTwoSections;
      scanlineRangeForTwoSections.AddValue( scanlineForSections[j] );
      scanlineRangeForTwoSections.AddValue( scanlineForSections[j+1] );
      if ( scanlineRangeForTwoSections.GetMinimum() <= i && i < scanlineRangeForTwoSections.GetMaximum() )
      {
        fLocation = Interpolate( scanlineForSections[j], float4(i), scanlineForSections[j+1], float4(j), float4(j+1) );
      }
    }

    // see if we have found a valid section for this scanline
    if ( fLocation >= 0.0F )
      m_scanlineData[i].m_fSectionIndex = fLocation;
    else if ( i < scanlineForSections.front() )
      m_scanlineData[i].m_fSectionIndex = -1;
    else
      m_scanlineData[i].m_fSectionIndex = -2;
  }

  // loop over all scanlines
  // scanlines are in world space
  for ( uint4 y=0; y<imageDim.m_y; ++y )
  {
    const float4 fSection( m_scanlineData[y].m_fSectionIndex );

    // check for a valid section index
    if( fSection >= 0.0F )
    {
      // we have a valid section
      // get the interpolated section location
      const int4 iSectionIndex( fSection );
      Point<float4> pos0( vessel[ iSectionIndex   ].GetPosition() );
      Point<float4> pos1( vessel[ iSectionIndex+1 ].GetPosition() );
      float4 fRadius0( sqrt( M_1_PI*vessel[ iSectionIndex ].GetWallArea() ) );
      float4 fRadius1( sqrt( M_1_PI*vessel[ iSectionIndex+1 ].GetWallArea() ) );

      const float4 fWeight1( fSection - iSectionIndex );
      Point<float4> midPos( Vector<float4>( pos0 ) * ( 1.0F - fWeight1 ) + Vector<float4>( pos1 ) * fWeight1 );
      float4 fRadius( fRadius0 * ( 1.0F - fWeight1 ) + fRadius1 * fWeight1 );

      //interpolate the direction perpendicular to the targent 
      float4  fPosOfSectionOnCurveMPR = (midPos.m_x - vfCenter.m_x)*vfHornAxis.m_x + 
                                        (midPos.m_y - vfCenter.m_y)*vfHornAxis.m_y +   
                                        (midPos.m_z - vfCenter.m_z)*vfHornAxis.m_z;  

      Vector3D<float4>  fDir0 = GetSmoothScanLineDir( vessel, iSectionIndex, fAngleRadians  );
      Vector3D<float4>  fDir1 = GetSmoothScanLineDir( vessel, iSectionIndex+1,   fAngleRadians  );
      Vector3D<float4>  scanDir;
      scanDir.m_x = fDir0.m_x * ( 1.0F - fWeight1 ) + fDir1.m_x * fWeight1;
      scanDir.m_y = fDir0.m_y * ( 1.0F - fWeight1 ) + fDir1.m_y * fWeight1;
      scanDir.m_z = fDir0.m_z * ( 1.0F - fWeight1 ) + fDir1.m_z * fWeight1;
      scanDir.Normalize();

      // get the left and right edges
      Point<float4> leftEdge  = midPos - ( fWidthMM/2.0 + fPosOfSectionOnCurveMPR )*scanDir;
      Point<float4> rightEdge = midPos + ( fWidthMM/2.0 - fPosOfSectionOnCurveMPR )*scanDir;
      
      m_scanlineData[y].m_leftPoint = leftEdge;
      m_scanlineData[y].m_rightPoint = rightEdge;
      m_scanlineData[y].m_centerlinePoint = midPos;
      m_scanlineData[y].m_fRadius = fRadius;
    }
    else
    {
      // retain the section index
      const float4 fPriorSectionIndex( m_scanlineData[y].m_fSectionIndex );
      m_scanlineData[y] = ScanlineData();
      m_scanlineData[y].m_fSectionIndex = fPriorSectionIndex;
      m_scanlineData[y].m_fRadius = 0.0F;
    }
  } // loop over all scanlines


  // potentially reverse the scanlines to flip the orientation
  if ( GetFlipNecessary( vessel, renderCamera ) )
    std::reverse( m_scanlineData.begin(), m_scanlineData.end() );

} // ComputeScanlinesCurved_Vessel()



/**
 * Compute all the scanlines for a curved MPR projection given all the necessary information (only used when the view direction mode is "X" "Y" "Z" )
 *
 * @param vessel      the vessel of interest
 * @param environment the environment
 * @param imageDim    the dimensions of the desired image
 */
void vxRibbon::ComputeScanlinesCurved_XYZ(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim)
{
  // set the projection type for display
  m_sProjectionType = "Curved MPR";

  // early return if there is no data
  if( vessel.size() < 2 )
    return;

  // get the curved plane
  vxPlaneCurved * pPlaneCurved = dynamic_cast< vxPlaneCurved * >( environment.GetPlaneCurved() );
  if ( pPlaneCurved == NULL )
  {
    LogDbg( "Curved Plane pointer is NULL", "vxRibbon", "ComputeScanlinesCurved" );
    return;
  }

  ClampZoom( pPlaneCurved, environment.GetIntensityVolume() );

  // set up coordinate conversion matrices   
  const float4 fAngleRadians( GetProjectionAngleDegrees( pPlaneCurved ) / 360.0F * 2.0F * M_PI );
  const Vector3D<float4> units( environment.GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  const Vector3D<int4>   dim  ( environment.GetIntensityVolume()->GetHeader().GetVolDim    () );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale(Vector<float4>(1.0F / units.X(), 1.0F / units.Y(), 1.0F / units.Z()));

  // rotate the camera
  int4  iVesselSize = vessel.size();
  Normal3Df  vfDirVertVessel =  Normal3Df( vessel[iVesselSize-1].GetPosition() - vessel[0].GetPosition() ); 
    
  vxCamera & renderCamera = GetRenderCamera();
  renderCamera = GetRefCamera();
  renderCamera.SetViewUp( vfDirVertVessel );  
  renderCamera.Rotate( renderCamera.GetViewUp(), fAngleRadians + M_PI, renderCamera.GetEye() );

  // compute the projection of all the points onto the camera right axis
  ValueRange<float4> planeDistances;
  Plane rightHalfSpace( renderCamera.GetEye(), renderCamera.GetViewRight() );
  Box<float4> bbox = Box<float4>( vessel[0].GetPosition(), vessel[0].GetPosition() );
  for( int i(0); i<vessel.size(); ++i )
  {
    planeDistances.AddValue( rightHalfSpace.GetDistance( vessel[i].GetPosition() ) );
    bbox.Union( vessel[i].GetPosition() );
  }

  // translate the render camera to the middle
  const float4 fMiddlePlaneDistance( ( planeDistances.GetMinimum() + planeDistances.GetMaximum() ) / 2.0F );
  const Point<float4> middle( renderCamera.GetEye() + Vector<float4>( renderCamera.GetViewRight() ) * fMiddlePlaneDistance );
  Normal<float4> viewDirection( renderCamera.GetViewDirection() );
  renderCamera.SetEye( middle );
  renderCamera.SetViewDirection( viewDirection );

  // set camera for rendering
  //renderCamera.SetBoundingBox( bbox );

  // save camera to environment so the 3D orientation lettering is correct
  * environment.GetCamera() = renderCamera;
  //environment.GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

  // Qian added: After changing the curved MPR image creation, the actual curve length
  // will no longer use for the view setting since we no longer display 'blocked' vessel
  // section.
  float fEntireVesselDistance = 0; 
  for( int i = 1 ; i<vessel.size(); ++i )
  {
    float4  fDisHorz = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).Dot( renderCamera.GetViewRight() );
    float4  fDisBetSections = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).GetLength();
    fEntireVesselDistance = fEntireVesselDistance + sqrt( fDisBetSections*fDisBetSections - fDisHorz*fDisHorz ); 
  }
  // End -- added by Qian

  // decide the overall height of the image in mm
  const float4 fAspectRatio( float4( imageDim.m_y ) / float4( imageDim.m_x ) );
  const float4 fWidthMM( pPlaneCurved->GetWidth() * 2 );
    
  m_fHeightImageMM = fWidthMM * fAspectRatio;
  const float4 fHeightVesselMM( fEntireVesselDistance );
  const float4 fMMperPixel( fWidthMM / imageDim.m_x );
  const float4 fHeightVesselScanlines( imageDim.m_y * fHeightVesselMM / m_fHeightImageMM );
  const float4 fInitialScanline( imageDim.m_y / 2.0F - fHeightVesselScanlines / 2.0F );

  // setup the scanline that each vessel section will be located at
  std::vector< float4 > scanlineForSections;

  /*  int i=0;
  for( ; i<vessel.size(); ++i )
  {
    float4 fLengthMM( vessel.GetLength( vxVessel::MeasurePair( 0, i ) ) );
    scanlineForSections.push_back( fInitialScanline + fLengthMM / fMMperPixel );
  }*/

  //Qian added
  int i=0;
  float fDistance = 0; 
  scanlineForSections.push_back( fInitialScanline + fDistance/fMMperPixel );
  for( i = 1 ; i<vessel.size(); ++i )
  {
    float4  fDisHorz = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).Dot( renderCamera.GetViewRight() );
    float4  fDisBetSections = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).GetLength();
    fDistance = fDistance + sqrt( fDisBetSections*fDisBetSections - fDisHorz*fDisHorz ); 
    scanlineForSections.push_back( fInitialScanline + fDistance/fMMperPixel );
  }
  //added by Qian

             
  // setup the vessel section that will be on each scanline
  // -1 indicates below start -2 indicates above end
  m_scanlineData.resize(imageDim.m_y);
  for( i=0; i<imageDim.m_y; ++i )
  {
    float fLocation( -1 );
    for( int j=0; j<scanlineForSections.size()-1; ++j )
    {
      ValueRange<float4> scanlineRangeForTwoSections;
      scanlineRangeForTwoSections.AddValue( scanlineForSections[j] );
      scanlineRangeForTwoSections.AddValue( scanlineForSections[j+1] );
      if ( scanlineRangeForTwoSections.GetMinimum() <= i && i < scanlineRangeForTwoSections.GetMaximum() )
      {
        fLocation = Interpolate( scanlineForSections[j], float4(i), scanlineForSections[j+1], float4(j), float4(j+1) );
      }
    }

    // see if we have found a valid section for this scanline 
    if ( fLocation >= 0.0F )
      m_scanlineData[i].m_fSectionIndex = fLocation;
    else if ( i < scanlineForSections.front() )
      m_scanlineData[i].m_fSectionIndex = -1;
    else
      m_scanlineData[i].m_fSectionIndex = -2;
  }

  const Normal<float4> rotatedRightNormal( renderCamera.GetViewRight() );
  const Vector<float4> rotatedRightVector = Vector3Df( rotatedRightNormal );

  vxPlaneOblique<float4> leftPlane ( -rotatedRightNormal, Point3Df( middle - rotatedRightVector * fWidthMM / 2.0F ) );
  vxPlaneOblique<float4> rightPlane( -rotatedRightNormal, Point3Df( middle + rotatedRightVector * fWidthMM / 2.0F ) );

  //////////////////////////
 // vxPlaneOblique<float4> leftPlane ( -rotatedRightNormal, Point3Df( middle - rotatedRightVector * fMiddlePlaneDistance ) );
 // vxPlaneOblique<float4> rightPlane( -rotatedRightNormal, Point3Df( middle + rotatedRightVector * fMiddlePlaneDistance ) );
  //////////////////////////

  // loop over all scanlines
  // scanlines are in world space
  for ( uint4 y=0; y<imageDim.m_y; ++y )
  {
    const float4 fSection( m_scanlineData[y].m_fSectionIndex );

    // check for a valid section index
    if( fSection >= 0.0F )
    {
      // we have a valid section
      // get the interpolated section location
      const int4 iSectionIndex( fSection );
      Point<float4> pos0( vessel[ iSectionIndex   ].GetPosition() );
      Point<float4> pos1( vessel[ iSectionIndex+1 ].GetPosition() );
      float4 fRadius0( sqrt( M_1_PI*vessel[ iSectionIndex ].GetWallArea() ) );
      float4 fRadius1( sqrt( M_1_PI*vessel[ iSectionIndex+1 ].GetWallArea() ) );

      const float4 fWeight1( fSection - iSectionIndex );
      Point<float4> midPos( Vector<float4>( pos0 ) * ( 1.0F - fWeight1 ) + Vector<float4>( pos1 ) * fWeight1 );
      float4 fRadius( fRadius0 * ( 1.0F - fWeight1 ) + fRadius1 * fWeight1 );

      // get the left and right edges
      Line scanline( midPos, renderCamera.GetViewRight() );
      Point<float4> leftEdge, rightEdge;
      if ( leftPlane.GetIntersection( scanline.GetPoint(), scanline.GetDirection(), leftEdge  ) == true &&
            rightPlane.GetIntersection( scanline.GetPoint(), scanline.GetDirection(), rightEdge ) == true )
      {
        float4 fProLen1 = ( midPos - leftEdge ).Dot( scanline.GetDirection() );
        float4 fProLen2 = ( rightEdge - leftEdge ).Dot( scanline.GetDirection() );
        if( fProLen1 < 0 ) 
          midPos = leftEdge; 
        if( fProLen1 > fProLen2 ) 
          midPos = rightEdge; 
       
        m_scanlineData[y].m_leftPoint = leftEdge;
        m_scanlineData[y].m_rightPoint = rightEdge;
        m_scanlineData[y].m_centerlinePoint = midPos;
        m_scanlineData[y].m_fRadius = fRadius;
      }
      else
      {
        // retain the section index
        const float4 fPriorSectionIndex( m_scanlineData[y].m_fSectionIndex );
        m_scanlineData[y] = ScanlineData();
        m_scanlineData[y].m_fSectionIndex = fPriorSectionIndex;
        m_scanlineData[y].m_centerlinePoint = midPos;
        m_scanlineData[y].m_fRadius = fRadius;
      }

    }
    else
    {
      // retain the section index
      const float4 fPriorSectionIndex( m_scanlineData[y].m_fSectionIndex );
      m_scanlineData[y] = ScanlineData();
      m_scanlineData[y].m_fSectionIndex = fPriorSectionIndex;
      m_scanlineData[y].m_fRadius = 0.0F;
    }
  } // loop over all scanlines


  // potentially reverse the scanlines to flip the orientation
  if ( GetFlipNecessary( vessel, renderCamera ) )
    std::reverse( m_scanlineData.begin(), m_scanlineData.end() );

} // ComputeScanlinesCurved_XYZ()



/**
 * Compute all the scanlines for a luminal MPR projection given all the necessary information. 
 *
 * @param vessel          the vessel of interest
 * @param imageDim        the dimensions of the desired image
 */
void vxRibbon::ComputeScanlinesLuminal(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim )
{
   ComputeScanlinesLuminal_VESSEL( vessel, environment, imageDim );
}//ComputeScanlinesLuminal



/**
 * Compute all the scanlines for a luminal MPR projection given all the necessary information. ( only used when the viewing mode is "vessel" )
 *
 * @param vessel          the vessel of interest
 * @param imageDim        the dimensions of the desired image
 */
void vxRibbon::ComputeScanlinesLuminal_VESSEL(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim )
{

  try
  {

    // set the projection type for display
    m_sProjectionType = "Luminal MPR";

    // check if there's any data to process
    if ( vessel.size() < 2 )
      return;

    // get the curved plane
    vxPlaneCurved * pPlaneCurved = dynamic_cast< vxPlaneCurved * >( environment.GetPlaneCurved() );
    if ( pPlaneCurved == NULL )
    {
      LogDbg( "Curved Plane pointer is NULL", "vxRibbon", "ComputeScanlinesCurved" );
      return;
    }
    
    ClampZoom( pPlaneCurved, environment.GetIntensityVolume() );

    // Get the major projection direction
    const float4 fRibbonWidthHalf = pPlaneCurved->GetWidth();
    const uint4 uSliceWidth(imageDim.m_x);
    const float4 fZoomingFactor = 0.5F * uSliceWidth / fRibbonWidthHalf; //same as Projection

    // compute the projection of vessel - curvProjection
    const float4 fAccLen = vessel.GetLength( vxVessel::MeasurePair(0, vessel.size()-1) ); //(vessel.size()-1) * fInterval;
    
    int4 yStart = Max(0                   , (int4)( imageDim.m_y*0.5f - fZoomingFactor*fAccLen*0.5f ) );
    int4 yEnd   = Min(int4(imageDim.m_y-1), (int4)( imageDim.m_y*0.5f + fZoomingFactor*fAccLen*0.5f ) );
    
    // do some bounds checking
    if ( yStart > yEnd || yStart >= imageDim.m_y - 1 )
    {
      throw ex::VException( LogRec( "yStart out of bounds: " + ToAscii( yStart ), "vxRibbon", "ComputeScanlinesLuminal" ) );
    }
    if ( yEnd < 0 )
    {
      throw ex::VException( LogRec( "yEnd out of bounds: " + ToAscii( yEnd ), "vxRibbon", "ComputeScanlinesLuminal" ) );
    }

    // loop over all scanlines outside visible centerline
    m_scanlineData.resize(imageDim.m_y);
    for (int4 y=0; y<yStart; y++)
    {
      m_scanlineData.at( y ) = ScanlineData();
      m_scanlineData.at( y ).m_fSectionIndex = -1;
      m_scanlineData.at( y ).m_fRadius = 0.0F;
    }
    if ( yStart == 0 )
      m_scanlineData.at( 0 ).m_fSectionIndex = -1;

    for (int4 y=yEnd+1; y<imageDim.m_y; y++)
    {
      m_scanlineData.at( y ) = ScanlineData();
      m_scanlineData.at( y ).m_fSectionIndex = -2;
      m_scanlineData.at( y ).m_fRadius = 0.0F;
    }
    if ( yEnd == imageDim.m_y )
      m_scanlineData.at( imageDim.m_y-1 ).m_fSectionIndex = -2;


    // setup the scanline that each vessel section will be located at
    const float4 fMMperPixel( fAccLen / (yEnd-yStart) );
    const float4 fInitialScanline( yStart );
    std::vector< float4 > scanlineForSections;
    scanlineForSections.push_back( fInitialScanline );
    for( int4 i=1; i<vessel.size(); ++i )
    {
      float4 fLengthMM( vessel.GetLength( vxVessel::MeasurePair( 0, i ) ) );
      scanlineForSections.push_back( fInitialScanline + fLengthMM / fMMperPixel );
    }

    // rotate the camera 
    const float4 fAngleRadians( GetProjectionAngleDegrees( pPlaneCurved ) / 360.0F * 2.0F * M_PI );
    vxCamera & renderCamera = GetRenderCamera();
    renderCamera = GetRefCamera();
    //renderCamera.Rotate( renderCamera.GetViewUp(), fAngleRadians + M_PI, renderCamera.GetEye() );

   // ValueRange<float4> planeDistances;
  //  Plane rightHalfSpace( renderCamera.GetEye(), renderCamera.GetViewRight() );
   // Box<float4> bbox = Box<float4>( vessel.front().GetPosition(), vessel.front().GetPosition() );
   // for( int i(0); i<vessel.size(); ++i )
   // {
   //   planeDistances.AddValue( rightHalfSpace.GetDistance( vessel.at( i ).GetPosition() ) );
   //   bbox.Union( vessel.at( i ).GetPosition() );
   // }

    // set camera for rendering
    //renderCamera.SetBoundingBox( bbox );
    // save camera to environment so the 3D orientation lettering is correct
    * environment.GetCamera() = renderCamera;

    // loop over all scanlines with visible data
    const float4 fWidth = imageDim.m_x / fZoomingFactor;
    //const Vector<float4> fHalfVector( renderCamera.GetViewRight() * fWidth * 0.5F );

    ValueRange<float4> scanlineRangeForTwoSections;
    scanlineRangeForTwoSections.AddValue( scanlineForSections.at( 0 ) );
    scanlineRangeForTwoSections.AddValue( scanlineForSections.at( 1 ) );
    int4 iIndexScanlineForSections(1);
    
    // the average viewing direction for the camera use
    Vector3D<float4> vfAvDirCamera( 0, 0, 0 );
    bool             bHaveDirCamera = false; 

    for (int4 y=yStart; y<=yEnd; y++)
    {
      while ( scanlineRangeForTwoSections.GetMaximum() < y && iIndexScanlineForSections < scanlineForSections.size() - 1 )
      {
        scanlineRangeForTwoSections.AddValue( scanlineForSections.at( ++iIndexScanlineForSections ) );
      }

      // create a line in world space
      const float4 fInterpolatedLength( Interpolate( (float4)yEnd, (float4)y, (float4)yStart, (float4)fAccLen, 0.0F ) );
      
      // choose first index with length larger than current interpolated length
      int4 iInterpolatedIndex( iIndexScanlineForSections );

      // must find a value - then interpolate it
      Point<float4> centralPointWorld;
      float4 fRadius(0.0F);
      Vector<float4> scanDir;

      if ( iInterpolatedIndex ) 
      {
        m_scanlineData.at( y ).m_fSectionIndex = Interpolate( scanlineForSections.at( iIndexScanlineForSections ), float4(y), scanlineForSections.at( iIndexScanlineForSections-1 ), float4(iIndexScanlineForSections), float4(iIndexScanlineForSections-1) );

        // we have a valid scanline
        float4 fL0 = (scanlineForSections.at( iInterpolatedIndex-1 )-yStart)*fMMperPixel;
        float4 fL1 = (scanlineForSections.at( iInterpolatedIndex   )-yStart)*fMMperPixel;
              
        // interpolate the centerpoint
        Point<float4> pnt0( vessel.at( iInterpolatedIndex-1 ).GetPosition() );
        Point<float4> pnt1( vessel.at( iInterpolatedIndex   ).GetPosition() );
        centralPointWorld = Point<float4>( Interpolate( fL1, fInterpolatedLength, fL0, pnt1.m_x, pnt0.m_x ),
                                           Interpolate( fL1, fInterpolatedLength, fL0, pnt1.m_y, pnt0.m_y ),
                                           Interpolate( fL1, fInterpolatedLength, fL0, pnt1.m_z, pnt0.m_z ) );    

        float4 fRadius0( sqrt( M_1_PI*vessel.at( iInterpolatedIndex-1 ).GetWallArea() ) );
        float4 fRadius1( sqrt( M_1_PI*vessel.at( iInterpolatedIndex   ).GetWallArea() ) );
        fRadius = Interpolate( fL1, fInterpolatedLength, fL0, fRadius1, fRadius0 );

        //interpolate the direction perpendicular to the targent 
        Vector3D<float4>  fDir0 = GetSmoothScanLineDir( vessel, iInterpolatedIndex-1, fAngleRadians  );
        Vector3D<float4>  fDir1 = GetSmoothScanLineDir( vessel, iInterpolatedIndex,   fAngleRadians  );
        scanDir = Vector3D<float4>( Interpolate( fL1, fInterpolatedLength, fL0, fDir1.m_x, fDir0.m_x ),
                                    Interpolate( fL1, fInterpolatedLength, fL0, fDir1.m_y, fDir0.m_y ),
                                    Interpolate( fL1, fInterpolatedLength, fL0, fDir1.m_z, fDir0.m_z ) );
        scanDir.Normalize();

        vfAvDirCamera = vfAvDirCamera + fDir0.Cross( vessel.at( iInterpolatedIndex-1 ).GetTanDirection() );
        bHaveDirCamera = true;
      }
      else
      {
        m_scanlineData.at( y ).m_fSectionIndex = 0.0F;
        fRadius = sqrt( M_1_PI * vessel.front().GetWallArea() );
        centralPointWorld = vessel.front().GetPosition();
        scanDir = Vector3D<float4>(vessel.front().GetUpDirection()); 
        scanDir.Normalize();
      }

      const Point<float4> leftPositionWorld ( centralPointWorld - fWidth * 0.5F*scanDir );
      const Point<float4> rightPositionWorld( centralPointWorld + fWidth * 0.5F*scanDir );
      //const Point<float4> leftPositionWorld ( centralPointWorld - fHalfVector );
      //const Point<float4> rightPositionWorld( centralPointWorld + fHalfVector );

      m_scanlineData.at( y ).m_leftPoint = leftPositionWorld;
      m_scanlineData.at( y ).m_rightPoint = rightPositionWorld;
      m_scanlineData.at( y ).m_centerlinePoint = centralPointWorld;
      m_scanlineData.at( y ).m_fRadius = fRadius;
    }

    // reset the camera's perspective to the vessel
    if( bHaveDirCamera == true ) 
       renderCamera.SetViewDirection( Normal3D<float4>( vfAvDirCamera ) );
    
    // potentially reverse the scanlines to flip the orientation
    if ( GetFlipNecessary( vessel, GetRefCamera() ) )
      std::reverse( m_scanlineData.begin(), m_scanlineData.end() );

  } // try

  catch (ex::VException & e )
  {
    LogErr( "exception: " + e.GetLogRecord().GetTextMessage(), "vxVessel", "Read" );
    throw ex::VException( LogRec( "exception", "vxRibbon", "ComputeScanlinesLuminal" ) );
  }

  catch (...)
  {
    throw ex::VException( LogRec( "unhandled exception", "vxRibbon", "ComputeScanlinesLuminal" ) );
  }
} // ComputeScanlinesLuminal_vessel()



/**
 * Compute all the scanlines for a luminal MPR projection given all the necessary information ( only used when the viewing mode is 'x' or 'y' or 'z' ) 
 *
 * @param vessel          the vessel of interest
 * @param imageDim        the dimensions of the desired image
 */
void vxRibbon::ComputeScanlinesLuminal_XYZ(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim )
{
  // set the projection type for display
  m_sProjectionType = "Curved MPR";

  // early return if there is no data
  if( vessel.size() < 2 )
    return;

  // get the curved plane
  vxPlaneCurved * pPlaneCurved = dynamic_cast< vxPlaneCurved * >( environment.GetPlaneCurved() );
  if ( pPlaneCurved == NULL )
  {
    LogDbg( "Curved Plane pointer is NULL", "vxRibbon", "ComputeScanlinesCurved" );
    return;
  }

  ClampZoom( pPlaneCurved, environment.GetIntensityVolume() );

  // set up coordinate conversion matrices   
  const float4 fAngleRadians( GetProjectionAngleDegrees( pPlaneCurved ) / 360.0F * 2.0F * M_PI );
  const Vector3D<float4> units( environment.GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  const Vector3D<int4>   dim  ( environment.GetIntensityVolume()->GetHeader().GetVolDim    () );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale(Vector<float4>(1.0F / units.X(), 1.0F / units.Y(), 1.0F / units.Z()));

  // rotate the camera
  vxCamera & renderCamera = GetRenderCamera();
  renderCamera = GetRefCamera();
  renderCamera.Rotate( renderCamera.GetViewUp(), fAngleRadians + M_PI, renderCamera.GetEye() );

  // compute the projection of all the points onto the camera right axis
  ValueRange<float4> planeDistances;
  Plane rightHalfSpace( renderCamera.GetEye(), renderCamera.GetViewRight() );
  Box<float4> bbox = Box<float4>( vessel[0].GetPosition(), vessel[0].GetPosition() );
  for( int i(0); i<vessel.size(); ++i )
  {
    planeDistances.AddValue( rightHalfSpace.GetDistance( vessel[i].GetPosition() ) );
    bbox.Union( vessel[i].GetPosition() );
  }

  // translate the render camera to the middle
  const float4 fMiddlePlaneDistance( ( planeDistances.GetMinimum() + planeDistances.GetMaximum() ) / 2.0F );
  const Point<float4> middle( renderCamera.GetEye() + Vector<float4>( renderCamera.GetViewRight() ) * fMiddlePlaneDistance );
  Normal<float4> viewDirection( renderCamera.GetViewDirection() );
  renderCamera.SetEye( middle );
  renderCamera.SetViewDirection( viewDirection );

  // set camera for rendering
  //renderCamera.SetBoundingBox( bbox );

  // save camera to environment so the 3D orientation lettering is correct
  * environment.GetCamera() = renderCamera;
  //environment.GetCamera()->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );

  // get the total vessel length
  //double fAccLen = vessel.GetLength( vessel.GetEntireDomain() );
  // This length is no longer used after we no longer display vessel segments that parallel to 
  // the scan line. Dongqing 24 June 2009

  // Qian added
  float fEntireVesselDistance = 0; 
  for( int i = 1 ; i<vessel.size(); ++i )
  {
    float4  fDisHorz = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).Dot( renderCamera.GetViewRight() );
    float4  fDisBetSections = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).GetLength();
    fEntireVesselDistance = fEntireVesselDistance + sqrt( fDisBetSections*fDisBetSections - fDisHorz*fDisHorz ); 
  }
  // End -- added by Qian

  // decide the overall height of the image in mm
  const float4 fAspectRatio( float4( imageDim.m_y ) / float4( imageDim.m_x ) );
  const float4 fWidthMM( pPlaneCurved->GetWidth() * 2 );
    
  m_fHeightImageMM = fWidthMM * fAspectRatio;
  const float4 fHeightVesselMM( fEntireVesselDistance ) ;  //vessel.GetLength( vessel.GetEntireDomain() ) );
  const float4 fMMperPixel( fWidthMM / imageDim.m_x );
  const float4 fHeightVesselScanlines( imageDim.m_y * fHeightVesselMM / m_fHeightImageMM );
  const float4 fInitialScanline( imageDim.m_y / 2.0F - fHeightVesselScanlines / 2.0F );

  // setup the scanline that each vessel section will be located at
  std::vector< float4 > scanlineForSections;

  /*  int i=0;
  for( ; i<vessel.size(); ++i )
  {
    float4 fLengthMM( vessel.GetLength( vxVessel::MeasurePair( 0, i ) ) );
    scanlineForSections.push_back( fInitialScanline + fLengthMM / fMMperPixel );
  }*/

  //Qian added
  int i=0;
  float fDistance = 0; 
  scanlineForSections.push_back( fInitialScanline + fDistance/fMMperPixel );
  for( i = 1 ; i<vessel.size(); ++i )
  {
    float4  fDisHorz = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).Dot( renderCamera.GetViewRight() );
    float4  fDisBetSections = ( vessel.at(i).GetPosition() - vessel.at(i-1).GetPosition() ).GetLength();
    fDistance = fDistance + sqrt( fDisBetSections*fDisBetSections - fDisHorz*fDisHorz ); 
    scanlineForSections.push_back( fInitialScanline + fDistance/fMMperPixel );
  }
  //added by Qian

             
  // setup the vessel section that will be on each scanline
  // -1 indicates below start -2 indicates above end
  m_scanlineData.resize(imageDim.m_y);
  for( i=0; i<imageDim.m_y; ++i )
  {
    float fLocation( -1 );
    for( int j=0; j<scanlineForSections.size()-1; ++j )
    {
      ValueRange<float4> scanlineRangeForTwoSections;
      scanlineRangeForTwoSections.AddValue( scanlineForSections[j] );
      scanlineRangeForTwoSections.AddValue( scanlineForSections[j+1] );
      if ( scanlineRangeForTwoSections.GetMinimum() <= i && i < scanlineRangeForTwoSections.GetMaximum() )
      {
        fLocation = Interpolate( scanlineForSections[j], float4(i), scanlineForSections[j+1], float4(j), float4(j+1) );
      }
    }

    // see if we have found a valid section for this scanline 
    if ( fLocation >= 0.0F )
      m_scanlineData[i].m_fSectionIndex = fLocation;
    else if ( i < scanlineForSections.front() )
      m_scanlineData[i].m_fSectionIndex = -1;
    else
      m_scanlineData[i].m_fSectionIndex = -2;
  }


  const Normal<float4> rotatedRightNormal( renderCamera.GetViewRight() );
  const Vector<float4> rotatedRightVector = Vector3Df( rotatedRightNormal );

  vxPlaneOblique<float4> leftPlane ( -rotatedRightNormal, Point3Df( middle - rotatedRightVector * fWidthMM / 2.0F ) );
  vxPlaneOblique<float4> rightPlane( -rotatedRightNormal, Point3Df( middle + rotatedRightVector * fWidthMM / 2.0F ) );

  //////////////////////////
 // vxPlaneOblique<float4> leftPlane ( -rotatedRightNormal, Point3Df( middle - rotatedRightVector * fMiddlePlaneDistance ) );
 // vxPlaneOblique<float4> rightPlane( -rotatedRightNormal, Point3Df( middle + rotatedRightVector * fMiddlePlaneDistance ) );
  //////////////////////////

  // loop over all scanlines
  // scanlines are in world space
  for ( uint4 y=0; y<imageDim.m_y; ++y )
  {
    const float4 fSection( m_scanlineData[y].m_fSectionIndex );

    // check for a valid section index
    if( fSection >= 0.0F )
    {
      // we have a valid section
      // get the interpolated section location
      const int4 iSectionIndex( fSection );
      Point<float4> pos0( vessel[ iSectionIndex   ].GetPosition() );
      Point<float4> pos1( vessel[ iSectionIndex+1 ].GetPosition() );
      float4 fRadius0( sqrt( M_1_PI*vessel[ iSectionIndex ].GetWallArea() ) );
      float4 fRadius1( sqrt( M_1_PI*vessel[ iSectionIndex+1 ].GetWallArea() ) );

      const float4 fWeight1( fSection - iSectionIndex );
      Point<float4> midPos( Vector<float4>( pos0 ) * ( 1.0F - fWeight1 ) + Vector<float4>( pos1 ) * fWeight1 );
      float4 fRadius( fRadius0 * ( 1.0F - fWeight1 ) + fRadius1 * fWeight1 );

      // get the left and right edges
      Line scanline( midPos, renderCamera.GetViewRight() );
      Point<float4> leftEdge, rightEdge;
      m_scanlineData[y].m_leftPoint = midPos - scanline.GetDirection() * fWidthMM / 2.0F; 
      m_scanlineData[y].m_rightPoint = midPos + scanline.GetDirection() * fWidthMM / 2.0F;
      m_scanlineData[y].m_centerlinePoint = midPos;
      m_scanlineData[y].m_fRadius = fRadius;
    }
    else
    {
      // retain the section index
      const float4 fPriorSectionIndex( m_scanlineData[y].m_fSectionIndex );
      m_scanlineData[y] = ScanlineData();
      m_scanlineData[y].m_fSectionIndex = fPriorSectionIndex;
      m_scanlineData[y].m_fRadius = 0.0F;
    }
  } // loop over all scanlines


  // potentially reverse the scanlines to flip the orientation
  if ( GetFlipNecessary( vessel, renderCamera ) )
    std::reverse( m_scanlineData.begin(), m_scanlineData.end() );

} // ComputeScanlinesLuminal_XYZ()




/**
 * Decide whether the vessel should be flipped
 *
 * @param vessel  the vessel data
 * @param camera  the camera
 *
 * @return if the vessel needs to be flipped vertically
 */
bool vxRibbon::GetFlipNecessary( const vxVessel & vessel, const vxCamera & camera )
{

  // compute the projection of the first and last centerline points onto the camera up axis
  Plane upHalfSpace( camera.GetEye(), camera.GetViewUp() );

  const float4 fDistanceFirst( upHalfSpace.GetDistance( vessel[        0          ].GetPosition() ) );
  const float4 fDistanceLast ( upHalfSpace.GetDistance( vessel[ vessel.size() - 1 ].GetPosition() ) );

  return fDistanceFirst > fDistanceLast;

} // GetFlipNecessary()


/**
 * Get the projection angle in degrees - taking into account the curved MPR rotation mode
 *
 * @param pCurvedPlane the curved plane object
 *
 * @return the projection angle in degrees
 */
float4 vxRibbon::GetProjectionAngleDegrees( vxPlaneCurved * pCurvedPlane )
{

  float4 fAngleDegrees( pCurvedPlane->GetAngle() );

  if ( vxVesselPreferences::GetRotatingCurvedMPR() == false )
    fAngleDegrees = int4( fAngleDegrees * 90 ) % 360;

  return fAngleDegrees;

} // GetProjectionAngleDegrees()


/**
 * Clamp the curved plane zoom to a reasonable size
 *
 * @param pPlaneCurved the curved plane
 * @param pVolume      the intensity volume to clamp to
 */
void vxRibbon::ClampZoom( vxPlaneCurved * pPlaneCurved, vxBlockVolume<uint2> * pVolume )
{

  const float4 fMaxWidth = Vector3Df( pVolume->GetHeader().GetVolDim() ).GetLength() / 2;
  if ( pPlaneCurved->GetWidth() > fMaxWidth )
    pPlaneCurved->SetWidth( fMaxWidth );

} // ClampZoom()


/**
 * get the smoothed ( after rotation around the targent ) scanline direction for each vessel section
 *
 * @param: vessel  - reference to the vessel 
 * @param: iInterpolatedIndex - the index of the section of the vessel
 * @param: fAngleRadians - rotation angle
 & 
 * @return: the smoothed direction
 */
Vector3D<float4> vxRibbon::GetSmoothScanLineDir( vxVessel & vessel, int4 iInterpolatedIndex, float4  fAngleRadians )
{
  Vector<float4> vN0, vN1, vN;

  vN = Vector3Df( vessel.at( iInterpolatedIndex ).GetUpDirection() );
  Matrix<float4> matrixRotate;
  matrixRotate.Identity();
  matrixRotate.Rotate( vessel.at( iInterpolatedIndex ).GetTanDirection(), fAngleRadians);
  vN = matrixRotate * vN;

  if( iInterpolatedIndex > 0 )
  {
    vN0 = Vector3Df( vessel.at( iInterpolatedIndex-1 ).GetUpDirection() );
    Matrix<float4> matrixRotate;
    matrixRotate.Identity();
    matrixRotate.Rotate( vessel.at( iInterpolatedIndex-1 ).GetTanDirection(), fAngleRadians );
    vN0 = matrixRotate * vN0;
  }

  if( iInterpolatedIndex < vessel.size()-1 )
  {
    vN1 = Vector3Df( vessel.at( iInterpolatedIndex+1 ).GetUpDirection() );
    Matrix<float4> matrixRotate;
    matrixRotate.Identity();
    matrixRotate.Rotate( vessel.at( iInterpolatedIndex+1 ).GetTanDirection(), fAngleRadians );
    vN1 = matrixRotate * vN1;
  }

  Vector<float4> vAvgN;
  if( iInterpolatedIndex>0 && iInterpolatedIndex<(vessel.size()-1) ) 
     vAvgN = (vN0+vN1+vN*2.0F)*0.25f;
  else if( iInterpolatedIndex == 0 )
     vAvgN = (vN1+vN)*0.5f;
  else if( iInterpolatedIndex == vessel.size()-1 )
     vAvgN = (vN0+vN)*0.5f;
  else
     vAvgN = Vector3D<float4>( 0, 0, 0 );

  return vAvgN; 
} // GetSmoothScanLineDir()



// undefines
#undef FILE_REVISION


// $Log: vxRibbon.C,v $
// Revision 1.4.2.3  2009/06/25 20:14:12  dongqing
// comments out non-use codes
//
// Revision 1.4.2.2  2008/08/12 17:35:49  cqian
// Remove curved-vessel
//
// Revision 1.4.2.1  2008/08/01 18:33:20  dongqing
// Fix the curved planar reformatted center position and scan direction
// strip artifacts. Dongqing and Cheng
//
// Revision 1.4  2008/04/03 22:40:06  gdavidson
// Changed the OrientationModeOfMPR enumeration
//
// Revision 1.3  2007/11/05 19:32:30  cqian
// the vessel moves with mouse
//
// Revision 1.2  2007/10/18 19:12:08  cqian
// MPR is changed
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2006/06/05 15:07:56  frank
// updated for visual studio 2005
//
// Revision 1.3  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.2.3  2005/08/23 19:07:35  frank
// Issue #4377: Prevented over-zooming out
//
// Revision 1.2.2.2  2005/08/19 17:19:18  frank
// Issue #4377: Clamped minimum zoom
//
// Revision 1.2.2.1  2005/08/01 20:05:42  frank
// changed function name according to coding standards
//
// Revision 1.2  2005/06/08 15:02:26  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.3  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.9.2.3  2004/02/25 15:00:32  frank
// formatting
//
// Revision 1.9.2.2  2004/02/24 20:18:42  frank
// fixed another problem with going over the ends of vessels when the view is zoomed in
//
// Revision 1.9.2.1  2004/02/24 17:53:18  frank
// fixed major problem with the luminal layout when the displayed vessel is much larger than the window.  Bounds checking in the vector has been
// enabled by using the at() function instead of the operator[] and catching any exceptions.
//
// Revision 1.9  2003/11/24 16:33:01  frank
// Fixed the field of view display
//
// Revision 1.8  2003/11/18 15:53:40  wenli
// 3D Biconvex Slab MIP
//
// Revision 1.7  2003/10/29 20:56:35  frank
// Added rotating curved MPR preference and disablement
//
// Revision 1.6  2003/10/22 22:05:11  wenli
// #3540: fix scanline for luminalMPR
//
// Revision 1.5  2003/08/08 20:20:44  wenli
// Issue #3396 ray segemtn is (0,0,0) to (0,0,0)
//
// Revision 1.4  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.3  2003/06/04 19:00:00  michael
// added namespace declaration
//
// Revision 1.2  2003/06/03 13:15:52  frank
// Removed modified event on camera
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.37  2003/05/05 16:03:07  frank
// Added the projection type string for display on the upper right of the overlay
//
// Revision 1.36  2003/05/05 13:58:19  frank
// code review
//
// Revision 1.35  2003/05/02 18:28:00  frank
// Issue #3169: Rotated curved MPR by 180 degrees
//
// Revision 1.34  2003/05/02 15:25:33  frank
// Removed Ray.h
//
// Revision 1.33  2003/04/09 14:23:00  frank
// Moved vascular preprocessing and intermediate results to a subdirectory
//
// Revision 1.32  2003/04/02 20:42:33  wenli
// fInterval is added
//
// Revision 1.30  2003/04/02 19:49:43  wenli
// fInterval is added & the scaling of Luminal MPR is fixed
//
// Revision 1.29  2003/03/28 18:50:18  frank
// Prevented scanline interpolation when straddling the invalid border
//
// Revision 1.28  2003/03/24 15:45:35  frank
// Fixed projection for endpoints
//
// Revision 1.27  2003/03/24 14:06:37  frank
// Fixed the orientation of the luminal view
//
// Revision 1.26  2003/03/21 19:26:49  frank
// Fixed handle selection and flipped vessel so that up is up
//
// Revision 1.25  2003/03/21 17:09:55  frank
// Flipped the scanlines in the curved view so that up is always up
//
// Revision 1.24  2003/02/28 19:25:28  frank
// Fixed the section index outside of the curved plane
//
// Revision 1.23  2003/02/20 16:38:20  wenli
// remove the first unstable scan line in ribbon
//
// Revision 1.22  2003/02/18 18:47:36  frank
// Added centerline point in luminal projection
//
// Revision 1.21  2003/02/18 17:58:31  frank
// Added graph on curved and fixed it on luminal
//
// Revision 1.20  2003/02/14 19:29:14  frank
// Fixed up initial curved zoom and made it possible to grab the endpoints of selection without using the ALT key
//
// Revision 1.19  2003/02/13 17:01:58  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.18  2003/02/12 15:08:21  wenli
// fix the zoom-in/out of the LuminalMPR
//
// Revision 1.17  2003/02/11 21:47:17  frank
// saved section index to luminal ribbon
//
// Revision 1.16  2003/02/11 18:45:55  wenli
// fix the centerlines and iterator problems in CurvedMPRProject
//
// Revision 1.15  2003/02/06 20:11:16  frank
// Fixed aspect ratio
//
// Revision 1.14  2003/02/05 23:21:37  wenli
// Fix the bugs in Luminal View
//
// Revision 1.13.2.1  2003/02/12 23:11:08  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.13  2003/01/28 23:17:26  frank
// Cleaned up, added a utility function.
//
// Revision 1.12  2003/01/28 15:09:51  frank
// Fixed section index.
//
// Revision 1.11  2003/01/24 20:16:29  frank
// Overhauled the ribbon to contain more helpful data per scanline.
//
// Revision 1.10  2003/01/24 18:21:22  frank
// Added stretched curved
//
// Revision 1.9  2003/01/22 22:20:15  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.8  2002/11/26 18:32:00  wenli
// zooming curved-MPR using '+' and '-'
//
// Revision 1.6  2002/11/22 16:39:11  wenli
// fixed the MPR width
//
// Revision 1.5  2002/11/18 16:55:41  michael
// made changes to account for changes to vx and v3D plane objects
//
// Revision 1.4  2002/11/15 02:09:11  frank
// Reflected name change in vxCamera.
//
// Revision 1.3  2002/11/15 00:05:26  frank
// Changed ribbon from volume/world coordinates to always world coordinates.
//
// Revision 1.2  2002/11/08 19:05:21  frank
// Made the curved MPR wider
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.13  2002/10/30 00:07:49  wenli
// change the width of Luminal
//
// Revision 1.12  2002/10/29 15:41:23  geconomos
// Added inclusion of header file.
//
// Revision 1.11  2002/10/07 21:38:27  wenli
// view label in MPR
//
// Revision 1.10  2002/09/27 21:47:10  wenli
// add curved MPR camera
//
// Revision 1.9  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.8  2002/09/16 20:22:14  dmitry
// New vxID (s) used.
//
// Revision 1.7  2002/08/22 17:18:06  frank
// Cleaned up warnings.
//
// Revision 1.6  2002/08/22 02:58:00  frank
// Stretched to full height, removed dead code.
//
// Revision 1.5  2002/08/21 23:27:54  wenli
// initial version - Vascular
//
// Revision 1.4  2002/08/19 23:15:38  wenli
// initial version - Vascular
//
// Revision 1.3  2002/08/19 14:37:32  wenli
// initial version - Vascular
//
// Revision 1.2  2002/08/15 18:20:36  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/15 14:27:10  wenli
// initial version - Vascular
//
// Revision 1.5  2002/08/02 15:29:11  michael
// Completed CameraNew --> vxCamera
//
// Revision 1.4  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.3.2.1  2002/07/04 05:00:14  frank
// Formatting.
//
// Revision 1.1  2002/06/21 19:03:20  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRibbon.C,v 1.4.2.3 2009/06/25 20:14:12 dongqing Exp $
// $Id: vxRibbon.C,v 1.4.2.3 2009/06/25 20:14:12 dongqing Exp $
