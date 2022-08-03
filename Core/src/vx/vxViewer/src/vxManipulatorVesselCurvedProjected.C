// $Id: vxManipulatorVesselCurvedProjected.C,v 1.3 2006/09/25 13:22:06 geconomos Exp $
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
#include "vxManipulatorVesselCurvedProjected.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "ValueRange.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "Resampler.h"
#include "vxRibbon.h"
#include "vxIDs.h"
#include "vxElementFloat4.h"
#include "Plane.h"
#include "Line.h"
#include "Intersection.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * default constructor
 * @param environment
 */
vxManipulatorVesselCurvedProjected::vxManipulatorVesselCurvedProjected(vxEnvironment & environment):
vxManipulator(environment)
{
  m_bDragging = false;
} // Constructor()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselCurvedProjected::OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState)
{
  return vxVesselUtilities::OrientCameraToVessel( GetEnvironment(), uKeyId );
} // OnKeyPressed()




/**
 * Event handler if button is pressed down
 *
 * @param position       the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselCurvedProjected::OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{
  if ( eButtonId == BUTTON_LEFT )
  {
    SetNavigationPars( position );
    m_bDragging = true;
  }

  return m_bDragging;
} // OnButtonDown()


/**
 * Event handler if mouse button is released
 *
 * @param position       the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselCurvedProjected::OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{
  OnMouseMove( position, uKeyboardState );
  m_bDragging = false;

  return m_bDragging;
} // OnButtonUp()


/**
 * Event handler if mouse is moved
 *
 * @param position       the screen position
 * @param uKeyboardState the keyboard state at the time of button press
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselCurvedProjected::OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState)
{
  if ( m_bDragging == true )
  {
    SetNavigationPars( position );
  }

  return m_bDragging;
} // OnMouseMove


/**
 * Event handler if mouse wheel is scrolled
 *
 * @param uKeyId         the key that was pressed
 * @param iDelta         the amount the wheel was scrolled in either direction
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */
bool vxManipulatorVesselCurvedProjected::OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState)
{
  return false;
} // OnMouseWheel()


/**
 * set navigation parameters
 *
 * @param position
 */
void vxManipulatorVesselCurvedProjected::SetNavigationPars( const Point2D<int2> & position )
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxManipulatorVesselCurvedProjected", "SetNavigationPars" );
    return;
  }

  // get the ribbon
  vxRibbon * pRibbon = static_cast<vxRibbon *>(GetEnvironment().GetElement( vxIDs::Ribbon ));
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxManipulatorVesselCurvedProjected", "SetNavigationPars" );
    return;
  }

  const int2 uVesselIndex( GetVesselIndex( position ) );
  if ( uVesselIndex < 0 || uVesselIndex >= pVessel->size() ) // no vessel at this position.
    return;

  vxVesselSection & section = (*pVessel)[ uVesselIndex ];
  //RelocateObliqueView( section.GetPosition(), section.GetTanDirection(), section.GetUpDirection(), uVesselIndex );

} // SetNavigationPars


/*
 *	find the vessel index in the curved MPR view
 *  using the same Zbuffer algorithm in the Renderwindow()
 *
 *  @param position : mouse clicked position on MPR window
 *  @return : the corresponding index in the vxRibbon
 */
int2 vxManipulatorVesselCurvedProjected::GetVesselIndex( const Point2D<int2> & position )
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogDbg( "Vessel pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  if (pVessel->size() < 2 ) //less than two points 
    return -1;
  
  // get the ribbon
  vxRibbon * pRibbon = static_cast<vxRibbon *>(GetEnvironment().GetElement( vxIDs::Ribbon ));
  if (pRibbon == NULL)
  {
    LogDbg( "Ribbon pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }


  // get the intensity volume
  vxBlockVolume<uint2> * pIntensityVolume = dynamic_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume());
  if (pIntensityVolume == NULL)
  {
    LogDbg( "Intensity volume pointer is NULL", "vxRendererSoftwareMPRCurved", "RenderWindow" );
    return 0;
  }

  // define the WorldToVolume matrix. 
  const Vector3D<float4> units( pIntensityVolume->GetHeader().GetVoxelUnits() );
  Matrix<float4> mWorldToVolume;
  mWorldToVolume.Scale( Vector<float4>( 1.0F /units.X(), 1.0F/units.Y(), 1.0F/units.Z() ) );

  // define the viewing matrix
  vxCamera &camera = pRibbon->GetRenderCamera();

  Triplef size(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());
  Box<float4> volBoundingBox(Box<float4>(0, 0, 0, size[0], size[1], size[2]));//camera.GetBoundingBox());
  //Box<float4> volBoundingBox(camera.GetBoundingBox());
  Point<float4> viewCenter(camera.GetViewReference());
  Point<float4> eyePoint(camera.GetEye());
  
  
  const uint4 uSliceWidth(GetEnvironment().GetWinRect().GetViewportWidth());
  const uint4 uSliceHeight(GetEnvironment().GetWinRect().GetViewportHeight());
  const Point<float4> refImagePoint( 0.5f*uSliceWidth, 0.5f*uSliceHeight, 0.0f );

  Matrix<float4> matTran;
  matTran.Translate( Vector3Df(viewCenter*(-1.0)) );

  Normal<float4> viewXdir = camera.GetViewRight();
  Normal<float4> viewYdir = camera.GetViewUp();
  Normal<float4> viewZdir = camera.GetViewDirection();
  Matrix<float4> viewRotMat( viewXdir.X(),  viewXdir.Y(),   viewXdir.Z(),   0.0f, 
                             viewYdir.X(),  viewYdir.Y(),   viewYdir.Z(),   0.0f,
                             viewZdir.X(),  viewZdir.Y(),   viewZdir.Z(),   0.0f,
                             0.0f,          0.0f,           0.0f,           1.0f );

  Matrix<float4> matZMove;
  matZMove.Translate( Vector3Df(0.0, 0.0, 2*volBoundingBox.GetOuterRadius()) );
  
  Matrix<float4> scalingMat;
  float4 scalingFactor = 0.75f*uSliceWidth / pRibbon->GetRibbonWidthHalf();
  scalingMat.Scale( Vector<float4>( scalingFactor, scalingFactor, 1.0) ); // we donot need to zoomin Z.

  Matrix<float4>  viewMat = scalingMat*mWorldToVolume*matZMove*viewRotMat*matTran;


  // define the bounding box
  const Normal<float4> rotatedRightNormal( camera.GetViewRight() );
  const Vector<float4> rotatedRightVector = Vector3Df(rotatedRightNormal);

  const Point<float4> centerOfVolume(camera.GetViewReference());
  const float4 fBoundingRadius(volBoundingBox.GetOuterRadius());
  //const float4 fBoundingRadius( camera.GetBoundingBox().GetOuterRadius() );

  Plane leftPlane (centerOfVolume - rotatedRightVector * fBoundingRadius, -rotatedRightNormal);
  Plane rightPlane(centerOfVolume + rotatedRightVector * fBoundingRadius, -rotatedRightNormal);


  // the image pixel we are interested in
  const int2 iFocusedPixelX = (float4)position.X() / (GetEnvironment().GetWinRect().GetViewportWidth()-1) * (uSliceWidth-1);
  const int2 iFocusedPixelY = (float4)(GetEnvironment().GetWinRect().GetViewportHeight()-1-position.Y()) / (GetEnvironment().GetWinRect().GetViewportHeight()-1) * (uSliceHeight-1);
  int2 iFocusedIndex = -1;
  float4 fFocusedZDepth = 1.0e5;



  int4 iNumOfVesselIndex = pVessel->size();
  int4 scanLine = 0;
  Point<float4> middlePointWorld = (*pVessel)[ scanLine ].GetPosition();
  Line scanlineRay(middlePointWorld, rotatedRightNormal);
  Point<float4> leftPointWorld ;
  Intersection::ComputeIntersection( scanlineRay, leftPlane, leftPointWorld );
  Point<float4> rightPointWorld;
  Intersection::ComputeIntersection( scanlineRay, rightPlane, rightPointWorld );

  Point<float4> leftPointVol = mWorldToVolume*leftPointWorld;
  Point<float4> rightPointVol = mWorldToVolume*rightPointWorld;
  Point<float4> middlePointVol = mWorldToVolume*middlePointWorld;

  Point<float4> leftPointImage = viewMat * leftPointWorld + Vector3Df(refImagePoint);
  Point<float4> rightPointImage = viewMat * rightPointWorld + Vector3Df(refImagePoint);
  Point<float4> middlePointImage = viewMat * middlePointWorld + Vector3Df(refImagePoint);
  for ( scanLine = 1; scanLine<iNumOfVesselIndex; scanLine++) 
  { 

    const Vector<float4> majorDirectionVector( rightPointVol - leftPointVol );
    if ( majorDirectionVector.GetLength() < 1.0f )
      continue;

    // next lines points.
    Point<float4> middlePointWorldNext = (*pVessel)[ scanLine ].GetPosition();
    Line scanlineRayNext(middlePointWorldNext, rotatedRightNormal);
    Point<float4> leftPointWorldNext ;
    Intersection::ComputeIntersection( scanlineRayNext, leftPlane, leftPointWorldNext );
    Point<float4> rightPointWorldNext;
    Intersection::ComputeIntersection( scanlineRayNext, rightPlane, rightPointWorldNext );

    Point<float4> leftPointVolNext = mWorldToVolume*leftPointWorldNext;
    Point<float4> rightPointVolNext = mWorldToVolume*rightPointWorldNext;
    Point<float4> middlePointVolNext = mWorldToVolume*middlePointWorldNext;

    Point<float4> leftPointImageNext = viewMat * leftPointWorldNext + Vector3Df(refImagePoint);
    Point<float4> rightPointImageNext = viewMat * rightPointWorldNext + Vector3Df(refImagePoint);
    Point<float4> middlePointImageNext = viewMat * middlePointWorldNext + Vector3Df(refImagePoint);

    const Vector<float4> majorDirectionVectorNext( rightPointVolNext - leftPointVolNext );
    if ( majorDirectionVectorNext.GetLength() < 1.0f )
      continue;

    bool bIsInY = (iFocusedPixelY - leftPointImageNext.m_y) * (iFocusedPixelY - leftPointImage.m_y) < 0.0f ? true : false;
    bool bIsInX = (iFocusedPixelX - leftPointImage.m_x) * (iFocusedPixelX - rightPointImage.m_x) < 0.0f ? true : false;
    
    // it is parallel line. skip the line outside the range
    if ( bIsInY && bIsInX )
    {
      const float4 dis0 = fabs(leftPointImage.m_y - iFocusedPixelY);
      const float4 dis1 = fabs(leftPointImageNext.m_y - iFocusedPixelY);
      const float4 zDepth = ( dis0 > dis1 ? leftPointImage.m_z : leftPointImageNext.m_z ); 
      const int2 zIndex = ( dis0 > dis1 ? scanLine - 1 : scanLine );
      if ( zDepth < fFocusedZDepth ) //update
      {
        fFocusedZDepth = zDepth;
        iFocusedIndex = zIndex;
      }
    }

    //next scenline
    middlePointWorld = middlePointWorldNext;
    leftPointWorld = leftPointWorldNext;
    rightPointWorld =  rightPointWorldNext;

    leftPointVol = leftPointVolNext;
    rightPointVol = rightPointVolNext;
    middlePointVol = middlePointVolNext;

    leftPointImage = leftPointImageNext;
    rightPointImage = rightPointImageNext;
    middlePointImage = middlePointImageNext;

  } //loop over all the ribbons

  return iFocusedIndex;

} // GetVesselIndex()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVesselCurvedProjected.C,v $
// Revision 1.3  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.2  2006/09/14 20:07:25  geconomos
// moved OrientCameratoVessel to vxVesselUtilities
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/06/08 15:00:14  vxconfig
// added easy select object
//
// Revision 1.2  2005/06/07 18:09:26  vxconfig
// removed the vessel array
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
// Revision 1.9  2004/03/02 01:10:32  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.8.2.1  2004/02/25 14:58:42  frank
// formatting
//
// Revision 1.8  2003/12/22 20:00:20  frank
// removed dead code
//
// Revision 1.7  2003/12/22 18:54:33  frank
// Set the current vessel section extended element modified
//
// Revision 1.6  2003/11/20 15:49:21  wenli
// Set the MPR Up direction close to +X, +Y or +Z axis
//
// Revision 1.5  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.4  2003/05/22 14:56:32  dongqing
// code review
//
// Revision 1.3  2003/05/21 20:48:11  wenli
// code review
//
// Revision 1.2  2003/05/16 20:27:52  frank
// fixed warning messages
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.9  2003/04/30 18:03:39  frank
// code review
//
// Revision 1.8  2003/04/30 16:56:47  frank
// code reviews
//
// Revision 1.7  2003/04/24 15:04:56  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.6  2003/03/13 14:29:48  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.5  2003/03/01 13:20:26  frank
// GetWinRect() was removed from vxManipulator
//
// Revision 1.4  2003/02/18 18:00:19  frank
// Cleaned up debug messages
//
// Revision 1.3  2003/02/14 19:25:55  frank
// Moved zoom function out to curved
//
// Revision 1.2  2003/02/13 17:25:16  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.1.2.1  2003/02/12 23:06:52  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.1  2003/02/12 14:24:52  frank
// renamed
//
// Revision 1.13  2003/02/03 17:28:21  frank
// Zoomed curved view to fit entire volume.
//
// Revision 1.12  2003/01/24 18:25:35  frank
// Removed Plane template parameter
//
// Revision 1.11  2003/01/22 22:16:47  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.10  2002/11/28 02:20:27  frank
// zoom 0.75
//
// Revision 1.9  2002/11/28 01:28:58  frank
// return and log, not throw
//
// Revision 1.8  2002/11/27 23:29:13  frank
// fixed oblique sync
//
// Revision 1.7  2002/11/26 21:47:49  wenli
// zooming curved-MPR using '+' and '-'
//
// Revision 1.6  2002/11/22 20:40:42  frank
// Fixed configurations
//
// Revision 1.5  2002/11/22 16:43:34  wenli
// left click on curvedMPR jump to position on oblique
//
// Revision 1.4  2002/11/18 16:55:41  michael
// made changes to account for changes to vx and v3D plane objects
//
// Revision 1.3  2002/11/11 20:07:03  frank
// Set ribbon modified upon change.
//
// Revision 1.2  2002/11/08 02:24:26  frank
// Implemented the button for x, y, z and d manipulation of the curved MPR
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.3  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVesselCurvedProjected.C,v 1.3 2006/09/25 13:22:06 geconomos Exp $
// $Id: vxManipulatorVesselCurvedProjected.C,v 1.3 2006/09/25 13:22:06 geconomos Exp $
